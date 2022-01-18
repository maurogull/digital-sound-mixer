
#include "gpio.h"



void gpio_init() {


	/*

	0
	1

	2  FSYNC in -OK
	
	3

	4  ADC in, ch 0 -OK
	5  ADC in, ch 1 -OK
	6  ADC in, ch 2 -OK

	7  DAC out, master LR -OK

	8  DAC out, aux 1 y 2
	9  DAC out, aux 3 y 4
	10 DAC out, aux 5 y 6
	11 ADC in, ch 3
	12 ADC in, ch 4
	13 ADC in, ch 5
	14 txd serie -OK
	15 rxd serie -OK
	16 ADC in, ch 6
	17 ADC in, ch 7
	18 ADC buffer selection bit 0
	19 ADC buffer selection bit 1

	20 MCLK out -OK

	21 dig poten SCK
	22 dig poten SDATA
	23 dig poten deco selection bit 0
	24 dig poten deco selection bit 1
	25 dig poten deco selection bit 2
	26 dig poten deco selection bit 3
	27 dig poten deco selection bit 4


	*/

}







void gpio_setup (unsigned int gpio, GPIOMODE mode) 
{
	/* bcm2837 pag 89 */

    unsigned int* GPFSEL  = (unsigned int*) (MMIO_BASE_PHYSICAL + 0x200000 + 0x0);
	if (gpio > 54) return;		// Check GPIO pin number valid, return false if invalid
	if (mode < 0 || mode > GPIO_ALTFUNC3) return;	// Check requested mode is valid, return false if invalid
	unsigned int bit = ((gpio % 10) * 3);	// Create bit mask
	unsigned int mem = GPFSEL[gpio / 10];	// Read register
	mem &= ~(7 << bit);		// Clear GPIO mode bits for that port
	mem |= (mode << bit);		// Logical OR GPIO mode bits
	GPFSEL[gpio / 10] = mem;	 // Write value to register
}

int gpio_input (unsigned int gpio)   /* not tested ! */
{
	/* bcm2837 pag 89 */
	
    unsigned int* GPLEV  = (unsigned int*) (MMIO_BASE_PHYSICAL + 0x200000 + 0x34);
	if (gpio < 54)	// Check GPIO pin number valid, return false if invalid
	{
		unsigned int bit = 1 << (gpio % 32);	// Create mask bit
		unsigned int  mem = GPLEV[gpio / 32];	// Read port level
		if (mem & bit) return 1;	// Return true if bit set
	}
	return 0;	// Return false
}

void gpio_output (unsigned int gpio, int value) 
{
	
	/* bcm2837 pag 89 */

	if(gpio > 54) return;
	if(value!=1 && value!=0) return;

    unsigned int* GPSET  = 		(unsigned int*) (MMIO_BASE_PHYSICAL + 0x200000 + 0x1c);
    unsigned int* GPCLEAR  = 	(unsigned int*) (MMIO_BASE_PHYSICAL + 0x200000 + 0x28);
    unsigned int* USE;

	unsigned int bit = 1 << (gpio % 32);	// Create mask bit
	unsigned int mem;

	if(value==1) USE = GPSET;
	else USE = GPCLEAR;

	mem = USE[gpio / 32];	// Read port level... esto no es necesario.....
	mem |= bit;
	USE[gpio / 32] = mem;

}