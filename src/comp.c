
#include "memory.h"
#include "math.h"
#include "comp.h"
#include "eq.h"


extern compressor CHANNEL_COMPRESSORS[IN_CHANNELS_COUNT];



void apply_comp(int ch, int pending_block) {

    int to, from;

	if(pending_block==1) {from = 0; to = SAMPLES/2; }
	else {from = SAMPLES/2; to = SAMPLES; }


    if (!CHANNEL_COMPRESSORS[ch].active) return;

    for(int s = from; s < to; s++) {


        float rms_squared = 0.0f;
        if(CHANNEL_COMPRESSORS[ch].mode == rms) {
            for(int i = (32-1); i > 0; i--) CHANNEL_COMPRESSORS[ch].rms_buffer[i] = CHANNEL_COMPRESSORS[ch].rms_buffer[i-1];  //rotate buffer
            CHANNEL_COMPRESSORS[ch].rms_buffer[0] = PROCESS_BUFFER[ch][s];
            for(int i = 0; i < 32; i++) rms_squared += CHANNEL_COMPRESSORS[ch].rms_buffer[i] * CHANNEL_COMPRESSORS[ch].rms_buffer[i];
            rms_squared /= 32.0f;
        }


        switch(CHANNEL_COMPRESSORS[ch].state) {

            case comp_open:
                break;

            case comp_compressing:
                break;

            case comp_attacking:
                CHANNEL_COMPRESSORS[ch].gain_reduction -= CHANNEL_COMPRESSORS[ch].attack_ratio_persample ;

                if(CHANNEL_COMPRESSORS[ch].gain_reduction <= CHANNEL_COMPRESSORS[ch].ratio) {
                    CHANNEL_COMPRESSORS[ch].state = comp_compressing;
                }

                break;

            case comp_holding:
                CHANNEL_COMPRESSORS[ch].hold_counter++;
                if(CHANNEL_COMPRESSORS[ch].hold_counter >= CHANNEL_COMPRESSORS[ch].hold) {
                    CHANNEL_COMPRESSORS[ch].state = comp_releasing;
                }
                break;

            case comp_releasing:
                CHANNEL_COMPRESSORS[ch].gain_reduction += CHANNEL_COMPRESSORS[ch].release_ratio_persample ;
                if(CHANNEL_COMPRESSORS[ch].gain_reduction >= 1.0f) {
                    CHANNEL_COMPRESSORS[ch].state = comp_open;
                }

                break;

        }



        if( (CHANNEL_COMPRESSORS[ch].mode == peak && absf( PROCESS_BUFFER[ch][s] ) >= CHANNEL_COMPRESSORS[ch].threshold)
           || (CHANNEL_COMPRESSORS[ch].mode == rms && rms_squared >= CHANNEL_COMPRESSORS[ch].threshold*CHANNEL_COMPRESSORS[ch].threshold)  ) {   //señal superó el umbral?
            if(CHANNEL_COMPRESSORS[ch].state == comp_open) {
                CHANNEL_COMPRESSORS[ch].state = comp_attacking;
            }

            if(CHANNEL_COMPRESSORS[ch].state == comp_releasing){
                CHANNEL_COMPRESSORS[ch].state = comp_attacking;
            }

            if(CHANNEL_COMPRESSORS[ch].state == comp_holding){
                CHANNEL_COMPRESSORS[ch].hold_counter = 0;
            }
        }
        else {                                  //señal bajó del umbral
            if(CHANNEL_COMPRESSORS[ch].state == comp_compressing) {
                CHANNEL_COMPRESSORS[ch].hold_counter = 0;
                CHANNEL_COMPRESSORS[ch].state = comp_holding;
            }
        }



        //comp.in = samplebuff[s];
        PROCESS_BUFFER[ch][s] *= CHANNEL_COMPRESSORS[ch].gain_reduction * CHANNEL_COMPRESSORS[ch].makeup;
        //comp.out = samplebuff[s];

    }


}




void comp_init(){

    for(int i=0; i<IN_CHANNELS_COUNT; i++) {

        CHANNEL_COMPRESSORS[i].active = 0;
        CHANNEL_COMPRESSORS[i].mode = rms;
        CHANNEL_COMPRESSORS[i].state = comp_open;
        CHANNEL_COMPRESSORS[i].gain_reduction = 1.0f;
        CHANNEL_COMPRESSORS[i].threshold = 0.6f;
        CHANNEL_COMPRESSORS[i].ratio = 0.5f;
        CHANNEL_COMPRESSORS[i].attack = 480; //10ms
        CHANNEL_COMPRESSORS[i].hold = 350;
        CHANNEL_COMPRESSORS[i].release = 480; //20ms
        CHANNEL_COMPRESSORS[i].makeup = 1.0f;

        for(int j = 0; j<32; j++) CHANNEL_COMPRESSORS[i].rms_buffer[j] = 0.0f;


        //precalculated:
        CHANNEL_COMPRESSORS[i].attack_ratio_persample = (1.0f-CHANNEL_COMPRESSORS[i].ratio) / CHANNEL_COMPRESSORS[i].attack;
        CHANNEL_COMPRESSORS[i].release_ratio_persample = (1.0f-CHANNEL_COMPRESSORS[i].ratio) / CHANNEL_COMPRESSORS[i].release;

    }
}

void comp_change_parameters() {

    //to-do

}