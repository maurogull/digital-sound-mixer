

#include "memory.h"
#include "eq.h"




extern biquad_filter CHANNEL_BIQUAD_FILTERS[IN_CHANNELS_COUNT][4];






void apply_eq(int ch, int pending_block) {

	int filter, to, from;


	if(pending_block==1) {from = 0; to = SAMPLES/2; }
	else {from = SAMPLES/2; to = SAMPLES; }


	float PrevSample0; 
	float PrevSample1;
	float PrevSample2;
	float initialPrevSample0 = (pending_block==1)? PROCESS_BUFFER[ch][SAMPLES-1] : PROCESS_BUFFER[ch][SAMPLES/2-1];
	float initialPrevSample1 = (pending_block==1)? PROCESS_BUFFER[ch][SAMPLES-2] : PROCESS_BUFFER[ch][SAMPLES/2-2];


	for(filter=0; filter<4; filter++) {

		if(CHANNEL_BIQUAD_FILTERS[ch][filter].active==0) continue;  //banda apagada?

		PrevSample0 = initialPrevSample0;
		PrevSample1 = initialPrevSample1;

	    for(int i=from; i<to; i++)  {
	        PrevSample2 = PrevSample1;   
	        PrevSample1 = PrevSample0;
	        PrevSample0 = PROCESS_BUFFER[ch][i];

			int iMinus1 = i-1;
			int iMinus2 = i-2;
			if(iMinus1 < 0) iMinus1 += SAMPLES;
			if(iMinus2 < 0) iMinus2 += SAMPLES;


	        PROCESS_BUFFER[ch][i] = (CHANNEL_BIQUAD_FILTERS[ch][filter].term1 * PrevSample0) +
	                        (CHANNEL_BIQUAD_FILTERS[ch][filter].term2 * PrevSample1) +
	                        (CHANNEL_BIQUAD_FILTERS[ch][filter].term3 * PrevSample2) -
	                        (CHANNEL_BIQUAD_FILTERS[ch][filter].term4 * PROCESS_BUFFER[ch][iMinus1]) -
	                        (CHANNEL_BIQUAD_FILTERS[ch][filter].term5 * PROCESS_BUFFER[ch][iMinus2]);

	    }

	}


	


}


void eq_init() {


	for(int i=0; i<IN_CHANNELS_COUNT; i++) {
		for(int j=0; j<4; j++) {
			CHANNEL_BIQUAD_FILTERS[i][j].active = 0;  //desactivamos los 4 eq de todos los canales
			CHANNEL_BIQUAD_FILTERS[i][j].term1 = 1.041710;   //términos de ejemplo, para un peak eq en 6kHz, Q 0.522 y gain 6 dB
			CHANNEL_BIQUAD_FILTERS[i][j].term2 = -1.354947;  
			CHANNEL_BIQUAD_FILTERS[i][j].term3 = 0.874474;  
			CHANNEL_BIQUAD_FILTERS[i][j].term4 = -1.354947;  
			CHANNEL_BIQUAD_FILTERS[i][j].term5 = 0.916184; 

		}
	}



}