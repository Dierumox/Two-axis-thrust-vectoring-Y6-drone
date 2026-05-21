#include <math.h>

struct cp_variables {
    float I_alpha;
    float I_beta;
    float I_a = 1; // igualar a lectura ADC  fase A
    float I_b = 0.5; // igualar a lectura ADC  fase B
}


inline float clarkepark[] (cp_variables) {
    I_alpha = I_a
    I_beta = (2 * I_b + I_a) / sqrt(3);
    return 

    // park 
}