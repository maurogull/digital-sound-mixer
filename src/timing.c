
#include "gpio.h"



#define ARM_TIMER_CTL   (MMIO_BASE_PHYSICAL+0x0000B408)
#define ARM_TIMER_CNT   (MMIO_BASE_PHYSICAL+0x0000B420)

#define SYSTMR_LO   ((volatile unsigned int*)(MMIO_BASE_PHYSICAL+0x00003004))
#define SYSTMR_HI   ((volatile unsigned int*)(MMIO_BASE_PHYSICAL+0x00003008))



void  timer_init ( void )
{
    //prescaler 15 (F) dependiendo de freq core. en rpi3 era 7 porque el counter va a 250mhz
    //freq counter = 500MHz / (15+1) = 31,25 MHz (lo que dividido 651 ~ 48,003kHz)
    *((volatile unsigned int*)ARM_TIMER_CTL) = 0x000F0200; // bit 9: free running counter enabled + prescaler


    /*
    // AUDIO MASTER CLOCK slave mode - 256 x 48khz 

	gpio_setup(20, GPIO_ALTFUNC5);  // gpio 20 outputs clock0 at 12.288 MHz  (256 x 48khz)
	*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x101000 + 0x70) = 0x5A000000 ; //stop clock 0
	for(int i=0; i<1e3; i++) asm volatile ("nop");  //wait
	*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x101000 + 0x74)  = 0x5A000000 | (61 << 12) |  (158 << 0);  //divi y divf
	for(int i=0; i<1e3; i++) asm volatile ("nop");  //wait
	*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x101000 + 0x70) = 0x5A000000 |  6;  // selecciona osc 6 (pll 750 mhz)
	for(int i=0; i<1e3; i++) asm volatile ("nop");  //wait
	*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x101000 + 0x70) = 0x5A000000 | 0x10 | 6  | (2<<9);  // enable clock + mash type
    */


    // AUDIO MASTER CLOCK master mode - 512 x fs

	gpio_setup(20, GPIO_ALTFUNC5);  // gpio 20 outputs clock0 at 24.576 MHz  (512 x 48khz)
	*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x101000 + 0x70) = 0x5A000000 ; //stop clock 0
	for(int i=0; i<1e3; i++) asm volatile ("nop");  //wait
	*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x101000 + 0x74)  = 0x5A000000 | (30 << 12) |  (2127 << 0);  //divi y divf para 48khz
	//*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x101000 + 0x74)  = 0x5A000000 | (33 << 12) |  (4985 << 0);  //divi y divf para ~ 44.1khz
	for(int i=0; i<1e3; i++) asm volatile ("nop");  //wait
	*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x101000 + 0x70) = 0x5A000000 |  6;  // selecciona osc 6 (pll 750 mhz)
	for(int i=0; i<1e3; i++) asm volatile ("nop");  //wait
	*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x101000 + 0x70) = 0x5A000000 | 0x10 | 6  | (1<<9);  // enable clock + mash type





    /*
    // BCK CLOCK - 128 x 48 khz

	gpio_setup(21, GPIO_ALTFUNC5);  // gpio 21 outputs clock1 at 6.144 MHz  (128 x 48khz)
	*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x101000 + 0x78) = 0x5A000000 ; //stop clock 1
	for(int i=0; i<1e3; i++) asm volatile ("nop");  //wait
	*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x101000 + 0x7c)  = 0x5A000000 | (122 << 12) |  (315 << 0);  //divi y divf
	for(int i=0; i<1e3; i++) asm volatile ("nop");  //wait
	*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x101000 + 0x78) = 0x5A000000 |  6;  // selecciona osc 6 (pll 750 mhz)
	for(int i=0; i<1e3; i++) asm volatile ("nop");  //wait
	*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x101000 + 0x78) = 0x5A000000 | 0x10 | 6  | (3<<9);  // enable clock + mash type

    */

}


/* returns actual timer tick. every tick is 32nseg */
unsigned int timer_tick ( void )
{
    return *((volatile unsigned int*)(ARM_TIMER_CNT));
}



/**
 * Wait N microsec (ARM CPU only)
 */
/*
void wait_usec(unsigned int n)
{
    register unsigned long f, t, r;
    // get the current counter frequency
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
    // read the current counter
    asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
    // calculate expire value for counter
    t+=((f/1000)*n)/1000;
    do{asm volatile ("mrs %0, cntpct_el0" : "=r"(r));}while(r<t);
}
*/

/**
 * Get System Timer's counter
 */
/*
unsigned long get_system_timer()
{
    unsigned int h=-1, l;
    // we must read MMIO area as two separate 32 bit reads
    h=*SYSTMR_HI;
    l=*SYSTMR_LO;
    // we have to repeat it if high word changed during read
    if(h!=*SYSTMR_HI) {
        h=*SYSTMR_HI;
        l=*SYSTMR_LO;
    }
    // compose long int value
    return ((unsigned long) h << 32) | l;
}

*/