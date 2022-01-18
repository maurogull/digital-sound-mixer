
#include "memory.h"
#include "timing.h"
#include "gpio.h"



/* Core 1: Manejo de entradas con ADC */

float sine1khz_data[] = {.0f, 0.0913683f, 0.1811733f, 0.2678784f, 0.3500000f, 0.4261330f, 0.4949747f, 0.5553473f, 0.6062178f, 0.6467157f, 0.6761481f, 0.6940114f, 0.7000000f, 0.6940114f, 0.6761481f, 0.6467157f, 0.6062178f, 0.5553473f, 0.4949747f, 0.4261330f, 0.3500000f, 0.2678784f, 0.1811733f, 0.0913683f, 0.0000000f, -0.0913683f, -0.1811733f, -0.2678784f, -0.3500000f, -0.4261330f, -0.4949747f, -0.5553473f, -0.6062178f, -0.6467157f, -0.6761481f, -0.6940114f, -0.7000000f, -0.6940114f, -0.6761481f, -0.6467157f, -0.6062178f, -0.5553473f, -0.4949747f, -0.4261330f, -0.3500000f, -0.2678784f, -0.1811733f, -0.0913683f};
int sine1khz_size = 48;
int sine1khz_pos = 0;

float sine480hz_data[] = {.0f, 0.0439534f, 0.0877333f, 0.1311669f, 0.1740829f, 0.2163119f, 0.2576872f, 0.2980455f, 0.3372276f, 0.3750788f, 0.4114497f, 0.4461968f, 0.4791830f, 0.5102780f, 0.5393593f, 0.5663119f, 0.5910295f, 0.6134147f, 0.6333789f, 0.6508436f, 0.6657395f, 0.6780082f, 0.6876011f, 0.6944803f, 0.6986187f, 0.7000000f, 0.6986187f, 0.6944803f, 0.6876011f, 0.6780082f, 0.6657395f, 0.6508436f, 0.6333789f, 0.6134147f, 0.5910295f, 0.5663119f, 0.5393593f, 0.5102780f, 0.4791830f, 0.4461968f, 0.4114497f, 0.3750788f, 0.3372276f, 0.2980455f, 0.2576872f, 0.2163119f, 0.1740829f, 0.1311669f, 0.0877333f, 0.0439534f, 0.0000000f, -0.0439534f, -0.0877333f, -0.1311669f, -0.1740829f, -0.2163119f, -0.2576872f, -0.2980455f, -0.3372276f, -0.3750788f, -0.4114497f, -0.4461968f, -0.4791830f, -0.5102780f, -0.5393593f, -0.5663119f, -0.5910295f, -0.6134147f, -0.6333789f, -0.6508436f, -0.6657395f, -0.6780082f, -0.6876011f, -0.6944803f, -0.6986187f, -0.7000000f, -0.6986187f, -0.6944803f, -0.6876011f, -0.6780082f, -0.6657395f, -0.6508436f, -0.6333789f, -0.6134147f, -0.5910295f, -0.5663119f, -0.5393593f, -0.5102780f, -0.4791830f, -0.4461968f, -0.4114497f, -0.3750788f, -0.3372276f, -0.2980455f, -0.2576872f, -0.2163119f, -0.1740829f, -0.1311669f, -0.0877333f, -0.0439534f};
int sine480hz_size = 100;
int sine480hz_pos = 0;

float sine4_8khz_data[] = {.0f, 0.4114497f, 0.6657395f, 0.6657395f, 0.4114497f, 0.0000000f, -0.4114497f, -0.6657395f, -0.6657395f, -0.4114497f};
int sine4_8khz_size = 10;
int sine4_8khz_pos = 0;


void core1(void) {

	wait_cycles(1e6);


	unsigned int ch;
	//register unsigned int omit_samples = 5;

	/*
	register signed long ticks_remaining = 80000;   //timer tick is 32nseg (freq 31,25 MHz)
	register unsigned int ticks_ellapsed;
	register unsigned int last_tick = timer_tick();
	register unsigned int now;
	*/

	gpio_setup(2, GPIO_INPUT);   //gpio 2: fsync (48khz)
	//gpio_setup(3, GPIO_INPUT);   //gpio 3: data clock (128 x 48khz = 6.144 MHz))
	//gpio_setup(2, GPIO_OUTPUT);   //gpio 2: fsync (48khz)
	//gpio_setup(3, GPIO_OUTPUT);   //gpio 3: data clock (128 x 48khz = 6.144 MHz)
	gpio_setup(4, GPIO_INPUT);   //gpio 4: data in 
	gpio_setup(5, GPIO_INPUT);   //gpio 5: data in 
	gpio_setup(6, GPIO_INPUT);   //gpio 6: data in 

	gpio_setup(16, GPIO_OUTPUT);  //debug

	*GPAREN0 = (1 << 2); //rising edge detect gpio 2, fsync

	*GPIO_PUP_PDN_CNTRL_REG0 = 0;  //saca todos los pull up/down en los GPIO 0 a 15 inclusive.
	*GPIO_PUP_PDN_CNTRL_REG1 = 0;  //saca todos los pull up/down en los GPIO 16 a 31 inclusive.


	wait_cycles(1e3);

	register unsigned int in_buffer_next = 0;  

	while(1) {

		/*
		while(now = timer_tick(), 
				ticks_ellapsed = (now >= last_tick)? now - last_tick : 0xffffffff - last_tick + now + 1, 
				last_tick=now, 
				ticks_remaining -= ticks_ellapsed, 
				ticks_remaining > 0 ) { 

									asm volatile("nop"); 
		}  

		//OJO aca: ticks_remaining es cero ó negativo!! por eso es que cortó el while! o sea que la siguiente suma es una resta!
		ticks_remaining = 640 + ticks_remaining;  //62500000 es 0.5 Hz (para debug)  ---  para 48,003 kHz es 651 (teórico, hay que medir) ... to-do

		*/

		/*
		for(ch=0; ch<IN_CHANNELS_COUNT; ch++) {
			//lectura de ADCs



			if(ch==0) IN_BUFFER[0][in_buffer_next] 		= sine1khz_data[sine1khz_pos];
			else if(ch==1) IN_BUFFER[1][in_buffer_next] = sine4_8khz_data[sine4_8khz_pos];
			else if(ch==2) IN_BUFFER[2][in_buffer_next] = sine480hz_data[sine480hz_pos];
			else IN_BUFFER[ch][in_buffer_next] = 0.0f;
			
		}
		*/


		unsigned int bits[24];
		register int bitcount;

		//espero high en master clock (gpio 20)
		//espero high en bck clock (gpio 21)
		//while( ( (*GPLEV0)&(1<<21)) ==0 ) { asm volatile("nop"); }
		//while( ( (*GPLEV0)&(1<<21)) !=0 ) { asm volatile("nop"); }

		//espero flanco ascendente en fsync (gpio 2)
		//while( ( (*GPLEV0)&(1<<2)) !=0 ) { asm volatile("nop"); }
		//while( ( (*GPLEV0)&(1<<2)) ==0 ) { asm volatile("nop"); }

		*GPEDS0 = (1 << 2); //clear event detect gpio 2, fsync
		
		//espero flanco ascendente en fsync (gpio 2)
		while( ( (*GPEDS0)&(1<<2)) ==0 ) { asm volatile("nop"); }


		//lectura de ADC

		//*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x200000 + 0x1c) = (1 << 2);  //fsync high
		//register int t = 0;
		//t=0; while(t++ < 5) { asm volatile("nop"); }
		

		*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x200000 + 0x1c) = (1 << 16);  //gpio 16 debug SET
		bits[0] = *GPLEV0; //----- gpio read, primer bit (MSB)
		*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x200000 + 0x28) = (1 << 16);  //gpio 16 debug CLEAR

		for(bitcount=1; bitcount<24; bitcount++) {
			
			wait_cycles(209);
			*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x200000 + 0x1c) = (1 << 16);  //gpio 16 debug SET
			bits[bitcount] = *GPLEV0; //----- gpio read
			*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x200000 + 0x28) = (1 << 16);  //gpio 16 debug CLEAR

		}

		//*(unsigned int*) (MMIO_BASE_PHYSICAL + 0x200000 + 0x28) = (1 << 2);  //fsync low
			


		//armo samples y pongo en buffer. ADC mandó MSB first, y es signado complemento a 2
		signed int result[3];
		result[0] = result[1] = result[2] = 0;

		for(int i = 0; i<24; i++) {
			if(bits[i] & (1<<4)) result[0] |= (1<<(23-i));  //channel 0
			if(bits[i] & (1<<5)) result[1] |= (1<<(23-i));  //channel 1
			if(bits[i] & (1<<6)) result[2] |= (1<<(23-i));  //channel 2
		}

		if((result[0] & (1<<23)) != 0) result[0] |= 0xFF000000; //relleno de 1s si ADC mandó un negativo
		if((result[1] & (1<<23)) != 0) result[1] |= 0xFF000000; 
		if((result[2] & (1<<23)) != 0) result[2] |= 0xFF000000; 

		//IN_BUFFER[0][in_buffer_next] = (result[0] - (1<<23)) / (float)(1<<23);
		IN_BUFFER[0][in_buffer_next] = (result[0] ) / (float)(1<<23);
		IN_BUFFER[1][in_buffer_next] = (result[1] ) / (float)(1<<23);
		IN_BUFFER[2][in_buffer_next] = (result[2] ) / (float)(1<<23);


		for(ch=3; ch<IN_CHANNELS_COUNT; ch++) {  //otros canales en 0f
			IN_BUFFER[ch][in_buffer_next] = 0.0f;
		}


		in_buffer_next++;

		

		// bloques llenos? aviso para que core0 procese

		if(in_buffer_next==SAMPLES/2) { memory_barrier(); IN_BUFFER_BLOCK1_FULL = 1;  }  //block 1 lleno  

		if(in_buffer_next==SAMPLES) {   //block 2 lleno
			memory_barrier();
			IN_BUFFER_BLOCK2_FULL = 1;
			in_buffer_next=0;   // es un ring buffer
		}

		

		/*
		sine1khz_pos++;
		sine480hz_pos++;
		sine4_8khz_pos++;
		if(sine1khz_pos >= sine1khz_size) sine1khz_pos=0;
		if(sine480hz_pos >= sine480hz_size) sine480hz_pos=0;
		if(sine4_8khz_pos >= sine4_8khz_size) sine4_8khz_pos=0;
		*/


	}

}
