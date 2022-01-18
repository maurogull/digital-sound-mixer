

/* 
AARCH64

short int = 2 bytes
int = 4 bytes
long = 8 bytes
*/


extern unsigned long CORE0_TICK_MAX;




#define IN_CHANNELS_COUNT 30
#define SAMPLES 32
#define SAMPLING_FREQ 48000


/* los datos leidos de los ADC por core 1 */
extern float IN_BUFFER[IN_CHANNELS_COUNT][SAMPLES];

/*  1 si el bloque de los buffers de entrada están llenos, para pasar a process */
extern unsigned int IN_BUFFER_BLOCK1_FULL;
extern unsigned int IN_BUFFER_BLOCK2_FULL;



extern unsigned int SEMAFOROTEST;



/* los datos que se procesan en core0. son traidos aqui por core0 desde los IN_BUFFER cuando el block se llena */
extern float PROCESS_BUFFER[IN_CHANNELS_COUNT][SAMPLES];

/*  1 si el bloque de buffers de proceso están pendientes de procesar */
extern unsigned int PROCESS_BUFFER_BLOCK1_PENDING;
extern unsigned int PROCESS_BUFFER_BLOCK2_PENDING;





/* los datos escritos a los DAC por core 2 */
extern float OUT_BUFFER_L[SAMPLES];
extern float OUT_BUFFER_R[SAMPLES];

/*  1 si el bloque de los buffers de salida ya fueron sacados y pueden ser rellenados para la prox vuelta */
extern unsigned int OUT_BUFFER_LR_BLOCK1_EMPTY;
extern unsigned int OUT_BUFFER_LR_BLOCK2_EMPTY;











// =========================================== comm data

extern unsigned int COMM_BYTE_1; /* command */
extern unsigned int COMM_BYTE_2; /* channel */
extern unsigned int COMM_BYTE_3; /* data */
extern unsigned int COMM_BYTES_RECEIVED;


#define COMM_BUFFER_SIZE 256

extern unsigned int COMM_BUFFER[COMM_BUFFER_SIZE];
extern unsigned int COMM_BUFFER_NEXT;




// =========================================== precalculated values







void memory_init();

void memory_barrier();

float db2lin(float db);




void semaphore_take (unsigned int * sem);
void semaphore_give (unsigned int * sem);
