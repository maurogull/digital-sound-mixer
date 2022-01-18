#include "gpio.h"
#include "memory.h"


/* Auxilary mini UART registers */
#define AUX_ENABLE      ((volatile unsigned int*)(MMIO_BASE_PHYSICAL+0x00215004))
#define AUX_MU_IO       ((volatile unsigned int*)(MMIO_BASE_PHYSICAL+0x00215040))
#define AUX_MU_IER      ((volatile unsigned int*)(MMIO_BASE_PHYSICAL+0x00215044))
#define AUX_MU_IIR      ((volatile unsigned int*)(MMIO_BASE_PHYSICAL+0x00215048))
#define AUX_MU_LCR      ((volatile unsigned int*)(MMIO_BASE_PHYSICAL+0x0021504C))
#define AUX_MU_MCR      ((volatile unsigned int*)(MMIO_BASE_PHYSICAL+0x00215050))
#define AUX_MU_LSR      ((volatile unsigned int*)(MMIO_BASE_PHYSICAL+0x00215054))
#define AUX_MU_MSR      ((volatile unsigned int*)(MMIO_BASE_PHYSICAL+0x00215058))
#define AUX_MU_SCRATCH  ((volatile unsigned int*)(MMIO_BASE_PHYSICAL+0x0021505C))
#define AUX_MU_CNTL     ((volatile unsigned int*)(MMIO_BASE_PHYSICAL+0x00215060))
#define AUX_MU_STAT     ((volatile unsigned int*)(MMIO_BASE_PHYSICAL+0x00215064))
#define AUX_MU_BAUD     ((volatile unsigned int*)(MMIO_BASE_PHYSICAL+0x00215068))


/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void comm_init()
{
    register int r;

    /* initialize UART */
    *AUX_ENABLE |=1;       // enable UART1, AUX mini uart
    *AUX_MU_CNTL = 0;  //disable for config
    *AUX_MU_LCR |= 0x03;       // 8 bits. decía 3
    *AUX_MU_MCR = 0;  //decia 0
    *AUX_MU_IER = 0;     // disable interrupts.
    *AUX_MU_IIR = 0xc6;    // clear fifos. decía 0xc6
    *AUX_MU_BAUD = 541;    // 115200 baud: divisor 541 para clock 500mhz
    /* map UART1 to GPIO pins */
    r=*GPFSEL1;
    r&=~((7<<12)|(7<<15)); // gpio14, gpio15
    r|=(2<<12)|(2<<15);    // alt5
    *GPFSEL1 = r;
    
    //*GPPUD = 0;            // enable pins 14 and 15

    //*GPIO_PUP_PDN_CNTRL_REG0 = 0;  //saca todos los pull up/down en los primeros GPIO. rpi 4
    
    r=200; while(r--) { asm volatile("nop"); }
    
    
    //*GPPUDCLK0 = (1<<14)|(1<<15);
    //r=150; while(r--) { asm volatile("nop"); }
    //*GPPUDCLK0 = 0;        // flush GPIO setup
    

    *AUX_MU_CNTL = 0x03;      // enable Tx, Rx
}



/**
 * Puts a character into the buffer, doesnt really send!
 */
void comm_send(unsigned int c) {
    COMM_BUFFER[COMM_BUFFER_NEXT] = c;
    memory_barrier();
    COMM_BUFFER_NEXT++;
    if(COMM_BUFFER_NEXT==COMM_BUFFER_SIZE) COMM_BUFFER_NEXT = 0; //ring buffer
    memory_barrier();
}




/**
 * Sends a character. BLOCKING FUNCTION
 */
void comm_real_send(unsigned int c) {
    do{asm volatile("nop");}while(!(*AUX_MU_LSR&0x20));  //wait fifo
    *AUX_MU_IO=c;  //send it
}

/**
 * Process 4 items in the send buffer. 
 */
void comm_send_buffer() {

    static unsigned int sent_pos = 0;
    unsigned int sent_count = 4;

    while(sent_count--) {

        if (sent_pos==COMM_BUFFER_NEXT) return;

        comm_real_send(COMM_BUFFER[sent_pos]);

        sent_pos++;
        if(sent_pos==COMM_BUFFER_SIZE) sent_pos = 0; //ring buffer
        sent_count++;
    }


}



/**
 * Receive a character, waiting for it. BLOCKING FUNCTION
 */
char comm_getc() {
    char r;
    /* wait until something is in the buffer */
    do{asm volatile("nop");}while(!(*AUX_MU_LSR&0x01));
    /* read it and return */
    r=(char)(*AUX_MU_IO);
    /* convert carrige return to newline */
    return r=='\r'?'\n':r;
}


/**
 * Display a string
 */
void comm_puts(char *s) {
    while(*s) {
        /* convert newline to carrige return + newline */
        //if(*s=='\n') comm_send('\r');
        comm_send(*s++);
    }
}



/**
 * Send a 64 bit binary value in hexadecimal
 */
void comm_hex64(unsigned long d) {
    unsigned int n;
    int c;
    for(c=60;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        comm_send(n);
    }
}


/**
 * Send a 32 bit binary value in hexadecimal
 */
void comm_hex32(unsigned int d) {
    unsigned int n;
    int c;
    for(c=28;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        comm_send(n);
    }
}


/**
 * Send a float (32bit) in hexadecimal
 */
void comm_float(float d) {
    union { float f; unsigned int i;} in;
    in.f = d;

    unsigned int n;
    int c;
    for(c=28;c>=0;c-=4) {
        // get highest tetrad
        n=(in.i >> c) & 0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n += (n > 9) ? 0x37 : 0x30;
        comm_send(n);
    }
}


/**
 * Send a 16 bit binary value in hexadecimal
 */
void comm_hex16(unsigned short int d) {
    unsigned int n;
    int c;
    for(c=12;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        comm_send(n);
    }
}


/**
 * Send a 8 bit binary value in hexadecimal
 */
void comm_hex8(unsigned char d) {
    unsigned int n;
    int c;
    for(c=4;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        comm_send(n);
    }
}

/**
 * Send a byte value in binary
 */
void comm_binary8(unsigned char d) {
    unsigned char n;
    int c;
    for(c=7; c>=0; c--) {
        n = (d>>c) & 0x01;
        if(n==1) comm_send('1');
        else comm_send('0');
    }
}



