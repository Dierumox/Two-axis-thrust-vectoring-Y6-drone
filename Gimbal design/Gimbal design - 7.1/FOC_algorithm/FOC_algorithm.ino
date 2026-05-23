#include <math.h>

float theta_e;
float I_a, I_b;
float I_beta;
float cos_theta_e, sin_theta_e;
float I_d, I_q;
float I_q_target;
float V_d, V_q;
float error_I_q;
const float dt = 0.00005f;
float vdvq_module, vdvq_scale;
float V_alpha, V_beta;
float U, V, W;
float sector;
float t1, t0, t2;

struct PI_properties {
  float K_p;
  float K_i_dt;
  float integral_val;
};

PI_properties I_d_properties;
PI_properties I_q_properties;

void setup() {
  Serial.begin(9600);

  I_d_properties.K_p = 1.2f; // This is a temporary estimated value
  I_d_properties.K_i_dt = 0.05f * 0.00005f; // This is a temporary estimated value
  I_d_properties.integral_val = 0.0f;

  I_q_properties.K_p = 1.2f;
  I_q_properties.K_i_dt = 0.05f * 0.00005f;
  I_q_properties.integral_val = 0.0f;
}

void loop() {
  if(theta_e >= TWO_PI) theta_e -= TWO_PI;
  if(theta_e < 0.0f) theta_e += TWO_PI;
  if(I_q_target > 1.0f) I_q_target = 1.0f;
  if(I_q_target < -1.0f) I_q_target = -1.0f;
  cp_transformations();
  // error_I_d = I_d
  error_I_q = I_q - I_q_target;
  V_d = PI_current_controller(&I_d_properties, I_d);
  V_q = PI_current_controller(&I_q_properties, error_I_q);
  vdvq_module = V_d * V_d + V_q * V_q;
  if(144.0f < vdvq_module){
    vdvq_scale = (12.f/sqrtf(vdvq_module));
    V_d *= vdvq_scale;
    V_q *= vdvq_scale;
  }
  inverse_park_transformation();
}

inline void cp_transformations() {
  //***// Clarke transformation //***//
  // I_alpha = I_a
  I_beta = ((2.0f * I_b) + I_a) * 0.57735f; // 0.57735 is the inverse of sqrt(3)

  //***// Park transformation //***//
  cos_theta_e = cosf(theta_e);
  sin_theta_e = sinf(theta_e);

  I_d = (I_a * cos_theta_e) + (I_beta * sin_theta_e);
  I_q = (-I_a * sin_theta_e) + (I_beta * cos_theta_e);
}

inline float PI_current_controller(PI_properties* properties, float error) {
  //***// PI algorithm //***//
  properties->integral_val += (-error * properties->K_i_dt);
  if(properties->integral_val > 12.0f) properties->integral_val = 12.0f;
  if(properties->integral_val < -12.0f) properties->integral_val = -12.0f;
  return ((-error * properties->K_p) + (properties->integral_val));
}

inline void inverse_park_transformation(){
  //***// Inverse Park transformation //***//
  V_alpha = (V_d * cos_theta_e) - (V_q * sin_theta_e);
  V_beta = (V_d * sin_theta_e) + (V_q * cos_theta_e);
}

inline void svpwm_modulation(){
  //***// Inverse Clarke transformation //***//
  U = V_beta;
  V = V_alpha * 0.866025f - V_beta * 0.5f;
  W = -V_alpha * 0.866025f - V_beta * 0.5f;
  if (U > 0.0f) sector += 1;
  if (V > 0.0f) sector += 2;
  if (W > 0.0f) sector += 4;
  switch(sector){
    case 3:
      t1 = W;
      t2 = -V;
      break;
    case 1:
      t1 = -W;
      t2 = U;
      break;
    case 5:
      t1 = V;
      t2 = -U;
      break;
    case 4:
      t1 = -V;
      t2 = W;
      break;
    case 6:
      t1 = U;
      t2 = -W;
      break;
    case 2:
      t1 = -U;
      t2 = V;
      break;
  }
  t0 = 1.0f - t1 - t2;
}