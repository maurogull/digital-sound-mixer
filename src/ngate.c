#include "memory.h"
#include "math.h"
#include "ngate.h"



extern noise_gate CHANNEL_NOISEGATES[IN_CHANNELS_COUNT];




void apply_ngate(int ch, int pending_block) {


    int to, from;

	if(pending_block==1) {from = 0; to = SAMPLES/2; }
	else {from = SAMPLES/2; to = SAMPLES; }



    if(CHANNEL_NOISEGATES[ch].force_mute) {
        for(int s = from; s < to; s++) { PROCESS_BUFFER[ch][s] = 0.0f; }
        return;
    }

    if (!CHANNEL_NOISEGATES[ch].active) return;

    for(int s = from; s < to; s++) {


        switch(CHANNEL_NOISEGATES[ch].state) {

            case ng_open:
                break;

            case ng_closed:
                break;

            case ng_attacking:
                CHANNEL_NOISEGATES[ch].gain_reduction += CHANNEL_NOISEGATES[ch].attack_ratio_persample ;

                if(CHANNEL_NOISEGATES[ch].gain_reduction >= 1.0f) {
                    CHANNEL_NOISEGATES[ch].state = ng_open;
                }

                break;

            case ng_holding:
                CHANNEL_NOISEGATES[ch].hold_counter++;

                if(CHANNEL_NOISEGATES[ch].hold_counter >= CHANNEL_NOISEGATES[ch].hold) {
                    CHANNEL_NOISEGATES[ch].state = ng_releasing;
                }
                break;

            case ng_releasing:
                CHANNEL_NOISEGATES[ch].gain_reduction -= CHANNEL_NOISEGATES[ch].release_ratio_persample ;

                if(CHANNEL_NOISEGATES[ch].gain_reduction <= CHANNEL_NOISEGATES[ch].attenuation) {
                    CHANNEL_NOISEGATES[ch].state = ng_closed;
                }

                break;

        }



        if( absf( PROCESS_BUFFER[ch][s] ) >= CHANNEL_NOISEGATES[ch].threshold ) {   //señal superó el umbral?
            if(CHANNEL_NOISEGATES[ch].state == ng_closed) {
                CHANNEL_NOISEGATES[ch].state = ng_attacking;
            }

            if(CHANNEL_NOISEGATES[ch].state == ng_holding) {
                CHANNEL_NOISEGATES[ch].hold_counter = 0;
            }

            if(CHANNEL_NOISEGATES[ch].state == ng_releasing){
                CHANNEL_NOISEGATES[ch].state = ng_attacking;
            }
        }
        else {                                  //señal bajó del umbral

            if(CHANNEL_NOISEGATES[ch].state == ng_open) {
                CHANNEL_NOISEGATES[ch].hold_counter = 0;
                CHANNEL_NOISEGATES[ch].state = ng_holding;
            }


        }







        //comp.in = samplebuff[s];
        PROCESS_BUFFER[ch][s] *= CHANNEL_NOISEGATES[ch].gain_reduction;
        //comp.out = samplebuff[s];

    }


}


void ngate_init(){

    for(int i=0; i<IN_CHANNELS_COUNT; i++) {

        CHANNEL_NOISEGATES[i].force_mute = 0;
        CHANNEL_NOISEGATES[i].active = 0;
        CHANNEL_NOISEGATES[i].state = ng_closed;
        CHANNEL_NOISEGATES[i].gain_reduction = 0.05f;
        CHANNEL_NOISEGATES[i].attenuation = 0.05f;
        CHANNEL_NOISEGATES[i].threshold = 0.7f;
        CHANNEL_NOISEGATES[i].attack = 480; //10ms
        CHANNEL_NOISEGATES[i].hold = 350;
        CHANNEL_NOISEGATES[i].release = 480; //20ms


        //precalculated:
        CHANNEL_NOISEGATES[i].attack_ratio_persample = (1.0f-CHANNEL_NOISEGATES[i].attenuation) / CHANNEL_NOISEGATES[i].attack;
        CHANNEL_NOISEGATES[i].release_ratio_persample = (1.0f-CHANNEL_NOISEGATES[i].attenuation) / CHANNEL_NOISEGATES[i].release;

    }
}

void ngate_change_parameters() {
//to-do

}
