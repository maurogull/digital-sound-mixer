#include "memory.h"
#include "timing.h"

#include "comm.h"

#include "gpio.h"



void _debug_comm_monitor();
void _debug_comm_master_left();


int getExceptionLevel (void); /* asm */





/* Core 3:   Timing & Comm   */

void core3(void) {


	wait_cycles(1e3);


	//comm_puts("\n % Ciao JAP21 core3 EL"); comm_hex8(getExceptionLevel());



	unsigned char comm_recv_byte[3] = {(char)0,(char)0,(char)0} ;
	register unsigned char comm_recv_count=0;
	register int comm_try;


	//comm_send('\033');
	//comm_send('\143');


	while(1) {

			
		comm_send_buffer();



		/* ------------ COMM receive commands */

		for(comm_try=0; comm_try<3; comm_try++) {   //intentamos 3 veces por si ya llegó todo en esta vuelta del core
			if( COMM_SOMETHING_RECEIVED ) {
				comm_recv_byte[comm_recv_count] = COMM_GET_RECEIVED;
				comm_recv_count++;
				if(comm_recv_count==3) {
					COMM_BYTE_1 = comm_recv_byte[0];
					COMM_BYTE_2 = comm_recv_byte[1];
					COMM_BYTE_3 = comm_recv_byte[2];
					comm_recv_count = 0;

					memory_barrier();
					COMM_BYTES_RECEIVED = 3;
					
					break;
				}
			}
		}





		/* ------------ COMM send debug */
		//_debug_comm_monitor();
		//_debug_comm_master_left();


		//led 
		//gpio_output (23, 0) ;

	}


}



/*
	
void _debug_comm_monitor() {

	comm_send('\033');
	comm_send('\143');
	comm_hex8(getExceptionLevel());

	comm_puts("  C0 max "); 
	comm_hex32(CORE0_TICK_MAX);
	comm_puts("  C0 last "); 
	comm_hex32(CORE0_LAST_PROCESS_TIME);
	comm_puts("  C1 max "); 
	comm_hex32(CORE1_TICK_MAX);
	//comm_puts("  C2 max "); 
	//comm_hex32(CORE2_TICK_MAX);


	comm_puts("\n");




	comm_puts("\t I0\t\tI1\t\tI3\t\tOL\t\tOR\n");

	int i;

	for(i=SAMPLES-1; i>=0; i--) {


		comm_hex8(i);
		if(i==SAMPLES/2) comm_puts(" __");
		comm_puts("\t");
		comm_send( (IN_BUFFER_NEXT==i)? '>':' ' );
		comm_float(IN_BUFFER[0][i]);
		comm_send('\t');
		comm_send( (IN_BUFFER_NEXT==i)? '>':' ' );
		comm_float(IN_BUFFER[1][i]);
		comm_send('\t');
		comm_send( (IN_BUFFER_NEXT==i)? '>':' ' );
		comm_float(IN_BUFFER[2][i]);
		comm_send('\t');
		comm_send( (OUT_BUFFER_LR_NEXT==i)? '>':' ' );
		comm_float(OUT_BUFFER_L[i]);  
		comm_send('\t');
		comm_send( (OUT_BUFFER_LR_NEXT==i)? '>':' ' );
		comm_float(OUT_BUFFER_R[i]);

		if(i==SAMPLES/2) comm_puts(" ___");

		comm_puts("\n");  
	}

	//wait_usec(1); 
	


  	comm_puts("Timer "); 
	comm_hex32(timer_tick());  

	comm_puts("\n");  
	comm_puts("VOL\t");  
	comm_hex8(MASTER_VOLUME[0]);
	comm_puts("\t");  
	comm_hex8(MASTER_VOLUME[1]);

	comm_puts("\t\t\t");
	comm_hex8(MASTER_VOLUME_L);
	comm_puts("\t");  
	comm_hex8(MASTER_VOLUME_R);

	comm_puts("\n");  
	comm_puts("PAN\t");  
	comm_hex8(MASTER_BALANCE[0]);
	comm_puts("\t");  
	comm_hex8(MASTER_BALANCE[1]);
	


	comm_puts("\n");  

	if(COMM_BYTES_RECEIVED >= 1) {
		if(COMM_BYTE_1 == 0x01) comm_puts("CH VOL");  
		if(COMM_BYTE_1 == 0x02) comm_puts("CH BAL"); 
	}

	if(COMM_BYTES_RECEIVED >= 2) {
		comm_puts("ch");
		comm_hex8(COMM_BYTE_2);
	}
}


void _debug_comm_master_left() {

	
	comm_puts("0x");
	comm_float(OUT_BUFFER_L[OUT_BUFFER_LR_NEXT]);  
	comm_send(',');


}


*/