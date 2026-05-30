#include <math.h>
#include <atomic>
#include "esp_attr.h"
#include "esp_err.h"

int16_t LUT_cos_theta_e[1024];
int16_t LUT_sin_theta_e[1024];
int theta_e = 0;
uint32_t angle_index;
int I_a, I_b;
int I_beta;
int I_d, I_q;
int I_q_target;
int V_d, V_q;
int error_I_q;
int vdvq_module;
int V_alpha, V_beta;
int U, V, W;
int sector;
int t1, t0, t2;
float prov_angle = 0.0f;
uint32_t ini_encoder_val;
uint32_t current_encoder_val;

struct PI_properties {
  int K_p;
  int K_i_dt;
  int integral_val;
};

PI_properties I_d_properties;
PI_properties I_q_properties;

std::atomic<float> target_angle(0.0f);
std::atomic<long> cuenta(0);

hw_timer_t * timerFOC = NULL;

void ESPNOW_procedure(void * pvParameters){
  while(true){
    // Procedimiento ESPNOW, interpolaciones...
    prov_angle = prov_angle + 0.00001f;
    while(prov_angle >= TWO_PI) prov_angle = prov_angle - TWO_PI;
    while(prov_angle < 0.0f) prov_angle = prov_angle + TWO_PI;
    target_angle.store(prov_angle, std::memory_order_relaxed);
    cuenta.store(cuenta.load(std::memory_order_relaxed) + 1, std::memory_order_relaxed);
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void IRAM_ATTR FOC_algorithm(){
  I_q_target = 4915; // This is the value for testing, 0.15f * 32768
  angle_index = (uint32_t)theta_e & 1023;
  if(I_q_target > 32768) I_q_target = 32768;
  if(I_q_target < -32768) I_q_target = -32768;
  cp_transformations();
  // error_I_d = I_d
  error_I_q = I_q - I_q_target;
  V_d = PI_current_controller(&I_d_properties, I_d);
  V_q = PI_current_controller(&I_q_properties, error_I_q);
  vdvq_module = ((V_d * V_d) >> 15) + ((V_q * V_q) >> 15);
  int32_t abs_V_d = (V_d < 0) ? -V_d : V_d;
  int32_t abs_V_q = (V_q < 0) ? -V_q : V_q;
  int32_t max_V = (abs_V_d > abs_V_q) ? abs_V_d : abs_V_q;
  int32_t min_V = (abs_V_d > abs_V_q) ? abs_V_q : abs_V_d;
  int32_t vdvq_module_estimated = max_V + (min_V >> 2);
  if(vdvq_module_estimated > 393216){
    V_d = ((int64_t)V_d * 393216) / vdvq_module_estimated;
    V_q = ((int64_t)V_q * 393216) / vdvq_module_estimated;
  }
  inverse_park_transformation();
  svpwm_modulation();
}

void setup(){
  // Escribir corriente fija para theta_e = 0, delay(100) y leer encoder. ini_encoder_val es ese valor
  for(int i = 0; i < 1024; i++){
    float angle_rad = ((float)i / 1024.0f) * TWO_PI;
    LUT_cos_theta_e[i] = (int16_t)(cos(angle_rad) * 32767.0f);
    LUT_sin_theta_e[i] = (int16_t)(sin(angle_rad) * 32767.0f);
  }
  Serial.begin(9600);
  uint32_t start_time = millis();
  while (!Serial);
  I_d_properties.K_p = 39322; // This is a temporary estimated value, 1.2f * 32768
  I_d_properties.K_i_dt = 42; // This is a temporary estimated value, 0.05 * 0.000005 * 2^24
  I_d_properties.integral_val = 0;

  I_q_properties.K_p = 39322;
  I_q_properties.K_i_dt = 42;
  I_q_properties.integral_val = 0;

  timerFOC = timerBegin(1000000); 
  timerAttachInterrupt(timerFOC, &FOC_algorithm);
  timerWrite(timerFOC, 0);
  timerAlarm(timerFOC, 50, true, 0);

  xTaskCreatePinnedToCore(
    ESPNOW_procedure,
    "Task_ESPNOW",
    4096,
    NULL,
    1,
    NULL,
    0
  );
}

void loop(){
  Serial.println("Y6");
  //Serial.println(cuenta.load(std::memory_order_relaxed));
  delay(100);
}

inline void cp_transformations() {
  //***// Clarke transformation //***//
  // I_alpha = I_a
  I_beta = (((2 * I_b) + I_a) * 18919) >> 15; // 18919 is the inverse of sqrt(3) (0.57735) by 2^15

  //***// Park transformation //***//
  I_d = ((I_a * LUT_cos_theta_e[angle_index]) >> 15) + ((I_beta * LUT_sin_theta_e[angle_index]) >> 15);
  I_q = ((-I_a * LUT_sin_theta_e[angle_index]) >> 15) + ((I_beta * LUT_cos_theta_e[angle_index]) >> 15);
}

inline int PI_current_controller(PI_properties* properties, int error) {
  //***// PI algorithm //***//
  properties->integral_val += (-error * properties->K_i_dt) >> 15;
  if(properties->integral_val > 201326592) properties->integral_val = 201326592;
  if(properties->integral_val < -201326592) properties->integral_val = -201326592;
  return (((-error * properties->K_p) >> 15) + (properties->integral_val >> 9));
}

inline void inverse_park_transformation(){
  //***// Inverse Park transformation //***//
  V_alpha = ((V_d * LUT_cos_theta_e[angle_index]) >> 15) - ((V_q * LUT_sin_theta_e[angle_index]) >> 15);
  V_beta = ((V_d * LUT_sin_theta_e[angle_index]) >> 15) + ((V_q * LUT_cos_theta_e[angle_index]) >> 15);
}

inline void svpwm_modulation(){
  //***// Inverse Clarke transformation //***//
  U = V_beta;
  V = ((V_alpha * 28378) >> 15) - ((V_beta * 16384) >> 15);
  W = ((-V_alpha * 28378) >> 15) - ((V_beta * 16384) >> 15);
  sector = 0;
  if (U > 0) sector += 1;
  if (V > 0) sector += 2;
  if (W > 0) sector += 4;
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
  t0 = 32768 - t1 - t2;
}