#include "memory.h"
#include "timing.h"
#include "gpio.h"



/* Core 2: Manejo de salidas a DAC */

void core2(void) {

	wait_cycles(3e6);

	
	//unsigned int ch;
	int out_buffer_LR_next = 0;

	gpio_setup(7, GPIO_OUTPUT);   //gpio 7: data DAC LR 
	//otros GPIOs inicializados por CORE 1

	wait_cycles(1e3);

	volatile unsigned int* bit[24];

	while(1) {


		// ----------- OUTPUT TO DAC 

		
			// clamp!! ojo clipping, los float pueden estár más allá de -1 y 1 cuando se suman en core0
			// ojo NaN y infinitos de ieee754


			#define GPIO_0_SET  (volatile unsigned int*)(MMIO_BASE_PHYSICAL + 0x200000 + 0x1c)
			#define GPIO_0_CLEAR  (volatile unsigned int*)(MMIO_BASE_PHYSICAL + 0x200000 + 0x28)



			//-----output LR master  LEFT

			signed int output = OUT_BUFFER_L[out_buffer_LR_next] * (float)(1<<23);
			//output &= 0x00000;
			//output |= 0xFF0000;

			bit[23] = (output & (1<<23)) ? GPIO_0_SET : GPIO_0_CLEAR; 
			bit[22] = (output & (1<<22)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[21] = (output & (1<<21)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[20] = (output & (1<<20)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[19] = (output & (1<<19)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[18] = (output & (1<<18)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[17] = (output & (1<<17)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[16] = (output & (1<<16)) ? GPIO_0_SET : GPIO_0_CLEAR; 
			bit[15] = (output & (1<<15)) ? GPIO_0_SET : GPIO_0_CLEAR; 
			bit[14] = (output & (1<<14)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[13] = (output & (1<<13)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[12] = (output & (1<<12)) ? GPIO_0_SET : GPIO_0_CLEAR; 
			bit[11] = (output & (1<<11)) ? GPIO_0_SET : GPIO_0_CLEAR; 
			bit[10] = (output & (1<<10)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[9] =  (output & (1<< 9)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[8] =  (output & (1<< 8)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[7] =  (output & (1<< 7)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[6] =  (output & (1<< 6)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[5] =  (output & (1<< 5)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[4] =  (output & (1<< 4)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[3] =  (output & (1<< 3)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[2] =  (output & (1<< 2)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[1] =  (output & (1<< 1)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[0] =  (output & (1<< 0)) ? GPIO_0_SET : GPIO_0_CLEAR;


			while( ( (*GPLEV0)&(1<<2)) ==0 ) { asm volatile("nop"); }  //espero high en fsync (gpio 2), señal LRCK para el DAC, alto es L


			*((volatile unsigned int*) bit[23]) = (1 << 7);

			for(int i=22;i>=0;i--) {
				wait_cycles(229);
				*((volatile unsigned int*) bit[i]) = (1 << 7);
			}

			wait_cycles(300);
			*(GPIO_0_CLEAR) = (1 << 7);  //output low


			//-----output LR master  RIGHT


			output = OUT_BUFFER_R[out_buffer_LR_next] * (float)(1<<23);
			//output &= 0x00000;
			//output |= 0xFF0000;

			bit[23] = (output & (1<<23)) ? GPIO_0_SET : GPIO_0_CLEAR; 
			bit[22] = (output & (1<<22)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[21] = (output & (1<<21)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[20] = (output & (1<<20)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[19] = (output & (1<<19)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[18] = (output & (1<<18)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[17] = (output & (1<<17)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[16] = (output & (1<<16)) ? GPIO_0_SET : GPIO_0_CLEAR; 
			bit[15] = (output & (1<<15)) ? GPIO_0_SET : GPIO_0_CLEAR; 
			bit[14] = (output & (1<<14)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[13] = (output & (1<<13)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[12] = (output & (1<<12)) ? GPIO_0_SET : GPIO_0_CLEAR; 
			bit[11] = (output & (1<<11)) ? GPIO_0_SET : GPIO_0_CLEAR; 
			bit[10] = (output & (1<<10)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[9] =  (output & (1<< 9)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[8] =  (output & (1<< 8)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[7] =  (output & (1<< 7)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[6] =  (output & (1<< 6)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[5] =  (output & (1<< 5)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[4] =  (output & (1<< 4)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[3] =  (output & (1<< 3)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[2] =  (output & (1<< 2)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[1] =  (output & (1<< 1)) ? GPIO_0_SET : GPIO_0_CLEAR;
			bit[0] =  (output & (1<< 0)) ? GPIO_0_SET : GPIO_0_CLEAR;



			while( ( (*GPLEV0)&(1<<2)) !=0 ) { asm volatile("nop"); }  //espero low en fsync (gpio 2), señal LRCK para el DAC, bajo es R

			*(bit[23]) = (1 << 7);

			for(int i=22;i>=0;i--) {
				wait_cycles(229);
				*(bit[i]) = (1 << 7);
			}



		wait_cycles(300);
		*(GPIO_0_CLEAR) = (1 << 7);  //output low

		// ----------- END OUTPUT TO DAC 


		
		
		out_buffer_LR_next++;
		
		if(out_buffer_LR_next==SAMPLES/2) {
			memory_barrier();
			OUT_BUFFER_LR_BLOCK1_EMPTY = 1;  //block 1 vacio
		}

		if(out_buffer_LR_next==SAMPLES) {   //block 2 vacio
			memory_barrier();
			OUT_BUFFER_LR_BLOCK2_EMPTY = 1;
			out_buffer_LR_next=0;  //ring buffer
		}



	}

}

