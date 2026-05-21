#include <math.h>
#include <iostream>

struct cp_variables {

    float I_alpha;
    float I_beta;

    float I_a = 1.0f;
    float I_b = 0.5f;
};

inline float* clarkepark(const cp_variables& v) {

    static float out[2];

    out[0] = v.I_a;

    out[1] = (2.0f * v.I_b + v.I_a) / sqrtf(3.0f);

    return out;
}


int main() {

    cp_variables vars;
    clarkepark(vars);
    std::cout << "I_alpha: " << vars.I_alpha << ", I_beta: " << vars.I_beta << std::endl;
    return 0;
}
