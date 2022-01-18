#include "eq.h"


typedef struct {

    // parameters
    unsigned int active;
    enum { peak, rms } mode;
    float threshold;  //linear
    float ratio;  //percent ej: 0.8f
    unsigned int attack; //# samples. 1ms = 48 samples @ 48kHz
    unsigned int hold; //# samples
    unsigned int release; //# samples
    float makeup; //linear

    unsigned int sidechain;  //-1 for self, or channel source #    to-do
    biquad_filter sidechain_filter;

    //state
    enum { comp_open, comp_attacking, comp_compressing, comp_holding, comp_releasing} state;
    float gain_reduction;
    float attack_ratio_persample;   //  (1.0f-ratio)  /  attack
    float release_ratio_persample;  //  (1.0f-ratio)  /  release
    unsigned int hold_counter;
    float rms_buffer[32];


    //meters
    float in;   //to-do
    float out;

} compressor;




void apply_comp(int ch, int pending_block);

void comp_init();
    
void comp_change_parameters();