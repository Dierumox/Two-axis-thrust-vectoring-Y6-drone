#include <math.h>

float theta_e;
float I_a, I_b;
float I_alpha, I_beta;
float cos_theta_e, sin_theta_e;
float I_d, I_q;
float I_q_target;
float V_d, V_q;
float error_I_q;

struct PI_properties {
  float K_p;
  float K_i;
  float integral_val;
  float max_voltage;
};

PI_properties I_d_properties;
PI_properties I_q_properties;

void setup() {
  Serial.begin(9600);

  I_d_properties.K_p = 1.2f;
  I_d_properties.K_i = 0.05f;
  I_d_properties.integral_val = 0.0f;
  I_d_properties.max_voltage = 12.0f; // Limitado al voltaje de tu batería

  I_q_properties.K_p = 1.2f;
  I_q_properties.K_i = 0.05f;
  I_q_properties.integral_val = 0.0f;
  I_q_properties.max_voltage = 12.0f;
}

void loop() {
  cp_transformations();
  // error_I_d = I_d
  error_I_q = I_q - I_q_target;
  V_d = FOC_algorithm(&I_d_properties, I_d);
  V_q = FOC_algorithm(&I_q_properties, error_I_q);

}

inline void cp_transformations() {
  //***// Clarke transformation //***//
  // I_alpha = I_a
  I_beta = ((2.0f * I_b) + I_a)/sqrtf(3.0f);

  //***// Park transformation //***//
  cos_theta_e = cosf(theta_e);
  sin_theta_e = sinf(theta_e);

  I_d = (I_a * cos_theta_e) + (I_beta * sin_theta_e);
  I_q = (-I_a * sin_theta_e) + (I_beta * cos_theta_e);
}

inline float FOC_algorithm(PI_properties* properties, float error) {
  properties->integral_val = properties->integral_val + (-error * properties->K_i);
  return (-error * properties->K_p) + (properties->integral_val);
}