
#include "memory.h"
#include "gpio.h"
#include "timing.h"
#include "comm.h"
#include "screen.h"
#include "math.h"

#include "eq.h"
#include "comp.h"
#include "ngate.h"





/* son traidos aqui por core0 desde los IN_BUFFER cuando el block se llena */
float PROCESS_BUFFER[IN_CHANNELS_COUNT][SAMPLES];

/*  1 si el bloque de buffers de proceso están pendientes de procesar */
unsigned int PROCESS_BUFFER_BLOCK1_PENDING;
unsigned int PROCESS_BUFFER_BLOCK2_PENDING;



/*  eq */
biquad_filter CHANNEL_BIQUAD_FILTERS[IN_CHANNELS_COUNT][4];


/*  compressor */
compressor CHANNEL_COMPRESSORS[IN_CHANNELS_COUNT];

/*   noise gate  */
noise_gate CHANNEL_NOISEGATES[IN_CHANNELS_COUNT];


float  MASTER_VOLUME[IN_CHANNELS_COUNT]; //linear
float  MASTER_BALANCE[IN_CHANNELS_COUNT];  //-1 L, 0 center, 1 R

float  MASTER_VOLUME_L; //linear
float  MASTER_VOLUME_R;




void system_init();

void transfer_input_to_process_buffer();
void transfer_processed_to_output();
void process_buffer();

void process_comm();



int getExceptionLevel (void); /* asm */


/* Core 0 (principal): Inicialización y proceso de señales */

void core0(void) {


	//inicialización y limpieza de bits mientras los otros cores esperan un poco para arrancar
	
	system_init();
	comm_puts("dcore0 EL"); comm_hex8(getExceptionLevel()); comm_endline();

	
	//screen_print(0, 0, "JAP21 is up");

	PROCESS_BUFFER_BLOCK1_PENDING = 0;
	PROCESS_BUFFER_BLOCK2_PENDING = 0;


	/* channel properties */

	for(int i=0; i<IN_CHANNELS_COUNT; i++) {
		MASTER_VOLUME[i] = 1.0f;   //ganancia 1
		MASTER_BALANCE[i] = 0; 
	}


	MASTER_VOLUME_L = 1.0f;   
	MASTER_VOLUME_R = 1.0f;

	MASTER_VOLUME[0] = 0.8f;     //pruebassss. to-do sacar
	MASTER_VOLUME[1] = 0.0f;
	MASTER_VOLUME[2] = 0.0f;
	
	//MASTER_BALANCE[1] = -0.3f; 

	int t = 0;

	wait_cycles(2e6);

	while(1) {
		unsigned long end_tick, init_tick = timer_tick();



		transfer_input_to_process_buffer();


		process_buffer();


		transfer_processed_to_output();


		process_comm();
		
		if(t % 2000000==0) { 
			comm_send('d');
			comm_hex32(CORE0_TICK_MAX); 
			comm_endline();
			CORE0_TICK_MAX = 0; 
		}

		
		if(t % 100000 == 0) { 
			comm_send('m');
			comm_send( (unsigned char)((absf(PROCESS_BUFFER[0][5])+absf(PROCESS_BUFFER[0][10])+absf(PROCESS_BUFFER[0][15]))/3.0 * 255)  );
			comm_send( (unsigned char)(absf(PROCESS_BUFFER[1][5]+PROCESS_BUFFER[1][10]+PROCESS_BUFFER[1][15])/3.0 * 255) );
			comm_send( (unsigned char)(absf(PROCESS_BUFFER[2][5]+PROCESS_BUFFER[2][10]+PROCESS_BUFFER[2][15])/3.0 * 255) );
			//comm_endline();
			//comm_send('d');
			//comm_float( PROCESS_BUFFER[0][5] );
			//comm_endline();
		}

		t++;

		end_tick = timer_tick();

		int tick_diff = (end_tick >= init_tick)? end_tick - init_tick : 0xffffffff - init_tick + end_tick + 1;
		if (tick_diff > CORE0_TICK_MAX) CORE0_TICK_MAX = tick_diff;

		
	}
}


void system_init() {
	comm_init();
	
	timer_init();
	memory_init();
	gpio_init();
	//screen_init();
	

	eq_init();
	comp_init();
	ngate_init();

	memory_barrier();
}



void transfer_input_to_process_buffer() {

	//vemos los IN_BUFFER_BLOCK1_FULL
	//transferimos bloques completos a los process queues si el bloque estuviera full.
	//suponemos que el process queue ya está listo (transferido a los buffers de salida), porque lo vamos a pisar!


	int ch, i;
	int block = 0;
	int from, to;

	
	if(IN_BUFFER_BLOCK1_FULL==1) 		{  from = 0; 			to = SAMPLES/2;		block=1; 	}
	else if(IN_BUFFER_BLOCK2_FULL==1)	{  from = SAMPLES/2; 	to = SAMPLES; 		block=2; 	}
		
	if(!block) return;


	for(ch=0; ch<IN_CHANNELS_COUNT; ch++)   //copiamos sample a sample, canal x canal
		for(i=from; i<to; i++) 
				PROCESS_BUFFER[ch][i] = IN_BUFFER[ch][i];


	//bajamos las banderas
	memory_barrier();
	if(block==1) {  		IN_BUFFER_BLOCK1_FULL = 0;	PROCESS_BUFFER_BLOCK1_PENDING = 1;	}
	else if(block==2) {  	IN_BUFFER_BLOCK2_FULL = 0;	PROCESS_BUFFER_BLOCK2_PENDING = 1; 	}
	
}


void transfer_processed_to_output() {

	//vemos los Block Pending de los processing buffers.
	//si están en 1, vemos los Block empty de las salidas.
	//si ese bloque listo está también listo a la salida, lo transferimos (copiamos a través de la fc de mezcla [suma, balance]).
	//si no seguiremos esperando hasta que la salida esté empty.

	int ch, i;
	int block_a_trabajar = 0;
	int to, from;

	if(OUT_BUFFER_LR_BLOCK1_EMPTY == 1 && PROCESS_BUFFER_BLOCK1_PENDING==0) block_a_trabajar=1;  // bloque 1 libre en salida LR y mismo bloque listo en process buffer
	if(OUT_BUFFER_LR_BLOCK2_EMPTY == 1 && PROCESS_BUFFER_BLOCK2_PENDING==0) block_a_trabajar=2;  // idem bloque 2
	if(!block_a_trabajar) return; //nada que hacer, esperamos a la prox


	if(block_a_trabajar==1) {from = 0; to = SAMPLES/2; }
	else {from = SAMPLES/2; to = SAMPLES; }


	//AQUI VAMOS! ésta es la razón de ser de una consola de mezcla: MEZCLAR !
	//transferimos a la salida a través de la fc de mezcla


	float out_buffer_l[SAMPLES];
	float out_buffer_r[SAMPLES];

	//suma hacia buffers de salida
	for(ch=0; ch<IN_CHANNELS_COUNT; ch++) {
		for(i=from; i<to; i++) {


			// ===============   PRE FADER SENDS   (effect, auxs.... mono? stereo? balance? to-do ) ===================






			// ===============   POST FADER SENDS   (master LR) ===================



			//aplico volumenes
			float temp_l = PROCESS_BUFFER[ch][i] * MASTER_VOLUME[ch];   //linear
			float temp_r = temp_l;

			//aplico balance. 0 center , -1 full L , +1 full R ---- atenuo el canal linealmente si está paneado para el otro lado
			if (MASTER_BALANCE[ch] > 0) temp_l = temp_l * (1-MASTER_BALANCE[ch]);
			if (MASTER_BALANCE[ch] < 0) temp_r = temp_r * (1+MASTER_BALANCE[ch]);
 

			if(ch==0) { out_buffer_l[i] = out_buffer_r[i] = 0.0f; }  //si es el primer canal que estamos sumando, reseteamos el buffer para no sumar sobre lo anterior
			out_buffer_l[i] += temp_l;
			out_buffer_r[i] += temp_r;
		}		
	}

	for(i=from; i<to; i++) {  //paso a shared memory
		OUT_BUFFER_L[i] = clampf(out_buffer_l[i]);
		OUT_BUFFER_R[i] = clampf(out_buffer_r[i]);
	}


	//listo, marcamos el bloque como no-empty 
	memory_barrier();
	if(block_a_trabajar==1) OUT_BUFFER_LR_BLOCK1_EMPTY = 0;
	if(block_a_trabajar==2) OUT_BUFFER_LR_BLOCK2_EMPTY = 0;
	

}


void process_buffer() {

	unsigned int ch;
	int pending_block=0;
	unsigned int channels_processed_count = 0;



	if(PROCESS_BUFFER_BLOCK1_PENDING == 1 ) pending_block=1;  // bloque 1 pendiente de proceso
	if(PROCESS_BUFFER_BLOCK2_PENDING == 1 ) pending_block=2;  // idem bloque 2

	if(!pending_block) return;  //todo listo, nada que hacer

	//if(pending_block==1) {from = 0; to = SAMPLES/2; }
	//else {from = SAMPLES/2; to = SAMPLES; }

	//vamos a procesar todos los canales entonces !!
	for(ch=0; ch<IN_CHANNELS_COUNT; ch++) {


		// ---------------------- PROCESS SIGNAL FOR THIS CHANNEL


			apply_ngate(ch, pending_block);
			apply_eq(ch, pending_block);
			apply_comp(ch, pending_block);


		// ---------------------- END PROCESS SIGNAL FOR THIS CHANNEL



		channels_processed_count++;
	}


	//marcamos bloque como "no pendiente"
	memory_barrier();
	if(pending_block==1) { PROCESS_BUFFER_BLOCK1_PENDING = 0; }
	if(pending_block==2) { PROCESS_BUFFER_BLOCK2_PENDING = 0; }


}


void process_comm() {

	if(COMM_BYTES_RECEIVED!=3) return;

	comm_send('d');
	comm_hex8(COMM_BYTE_1);
	comm_hex8(COMM_BYTE_2);
	comm_hex8(COMM_BYTE_3);
	comm_endline();

	//channel master volume
	if(COMM_BYTE_1 == 0x01) {   
		if(COMM_BYTE_2 <= IN_CHANNELS_COUNT) MASTER_VOLUME[COMM_BYTE_2] = COMM_BYTE_3;  //es un volumen de canal de entrada

		if(COMM_BYTE_2==0xFE) { MASTER_VOLUME_L = COMM_BYTE_3; }  //volumen de master L
		if(COMM_BYTE_2==0xFF) { MASTER_VOLUME_R = COMM_BYTE_3; }  //volumen de master R
	}

	//channel master balance
	if(COMM_BYTE_1 == 0x02) {  
		if(COMM_BYTE_2 <= IN_CHANNELS_COUNT) MASTER_BALANCE[COMM_BYTE_2] = COMM_BYTE_3;  //es balance de canal de entrada

	}

	//analog preamplifier gain
	if(COMM_BYTE_1 == 0x03) {   
		if(COMM_BYTE_2 <=  IN_CHANNELS_COUNT) {

			

		}  


	}

	memory_barrier();

	COMM_BYTES_RECEIVED = 0;

}

