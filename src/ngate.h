


typedef struct {

    // parameters
    unsigned int active;
    unsigned int force_mute;
    float threshold;  //linear
    float attenuation;  //percent ej: 0.05f   atenuación cuando está closed
    unsigned int attack; //# samples. 1ms = 48 samples @ 48kHz
    unsigned int hold; //# samples
    unsigned int release; //# samples


    //state
    enum { ng_closed, ng_attacking, ng_open, ng_holding, ng_releasing} state;
    float gain_reduction;
    float attack_ratio_persample;   //  (1.0f-ratio)  /  attack
    float release_ratio_persample;  //  (1.0f-ratio)  /  release
    unsigned int hold_counter;


    //meters
    float in;
    float out;

} noise_gate;



void apply_ngate(int ch, int pending_block);

void ngate_init();
    
void ngate_change_parameters();