
/*  


NOTE: qemu does not redirect UART1 to terminal by default, only UART0, so you have to use -serial null -serial stdio.



miniuart, external pins: 

etiqueta de interfaz rs232:  RXD -> pin 8 (GPIO14)    TXD -> pin 10 (GPIO15)

*/



void comm_init();
void comm_send(unsigned int );
char comm_getc();
void comm_puts(char *);
void comm_hex64(unsigned int);
void comm_hex32(unsigned int);
void comm_float(float);
void comm_hex16(unsigned short int);
void comm_hex8(unsigned char );
void comm_binary8(unsigned char);

void comm_send_buffer();
void comm_real_send(unsigned int c);


#define COMM_SOMETHING_RECEIVED ((*((volatile unsigned int*)(0xfe000000+0x00215054)) & 0x01) != 0)

#define COMM_GET_RECEIVED  (char)(*((volatile unsigned int*)(0xfe000000+0x00215040)))



inline void comm_bell() { comm_send(0x07); }


inline void comm_endline() { comm_send('\n'); }