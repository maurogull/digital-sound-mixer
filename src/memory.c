

/* 
aarch64:
short int = 2 bytes
int = 4 bytes
long = 8 bytes
*/

#include "memory.h"




unsigned long __attribute__((section (".noncacheable"))) CORE0_TICK_MAX;




/* los datos leidos de los ADC por core 1 */
float __attribute__((section (".noncacheable"))) IN_BUFFER[IN_CHANNELS_COUNT][SAMPLES];

/*  1 si el bloque de los buffers de entrada están llenos, para pasar a process */
unsigned int __attribute__((section (".noncacheable"))) IN_BUFFER_BLOCK1_FULL;
unsigned int __attribute__((section (".noncacheable"))) IN_BUFFER_BLOCK2_FULL;








/* los datos escritos a los DAC por core 2 */
float __attribute__((section (".noncacheable"))) OUT_BUFFER_L[SAMPLES];
float __attribute__((section (".noncacheable"))) OUT_BUFFER_R[SAMPLES];

/*  1 si el bloque de los buffers de salida ya fueron sacados y pueden ser rellenados para la prox vuelta */
unsigned int __attribute__((section (".noncacheable"))) OUT_BUFFER_LR_BLOCK1_EMPTY;
unsigned int __attribute__((section (".noncacheable"))) OUT_BUFFER_LR_BLOCK2_EMPTY;





unsigned int __attribute__((section (".noncacheable"))) COMM_BYTE_1;
unsigned int __attribute__((section (".noncacheable"))) COMM_BYTE_2;
unsigned int __attribute__((section (".noncacheable"))) COMM_BYTE_3;
unsigned int __attribute__((section (".noncacheable"))) COMM_BYTES_RECEIVED;


unsigned int __attribute__((section (".noncacheable"))) COMM_BUFFER[COMM_BUFFER_SIZE];
unsigned int __attribute__((section (".noncacheable"))) COMM_BUFFER_NEXT;





unsigned int __attribute__((section (".noncacheable"))) SEMAFOROTEST;




/* limpia buffers y bits */
void memory_init() {
	int i;


	//CORE0_LAST_TICK_COUNT = 0;
	CORE0_TICK_MAX = 0L;



	IN_BUFFER_BLOCK1_FULL=0;
	IN_BUFFER_BLOCK2_FULL=0;

	


	/* salidas */

	for(i=0; i < SAMPLES; i++) {
		OUT_BUFFER_L[i] = OUT_BUFFER_R[i] = 0;  
	}

	OUT_BUFFER_LR_BLOCK1_EMPTY = 0; //inicialmente marcamos las salidas como "llenas": saldrá silencio durante SAMPLES samples
	OUT_BUFFER_LR_BLOCK2_EMPTY = 0;





	


	/* comm data */

	COMM_BYTE_1 = COMM_BYTE_2 = COMM_BYTE_3 = COMM_BYTES_RECEIVED = 0;

	COMM_BUFFER_NEXT = 0;


	memory_barrier();



}


/*
	do an armv8 DMB instruction
*/
void memory_barrier() {
	asm volatile("dmb st");
}


/*
inline float db2lin(float db){ // dB to linear
	return powf(10.0f, 0.05f * db);
}

*/