/*
 * Copyright 2026 Diego Rubio Mozo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://apache.org
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <Arduino.h>

const int PIN_S = 6;
const int PIN_POT = 1;

void PWM_motor_task(void *pvParameters){
  unsigned long start = millis();
  while(millis() - start < 5000){
    unsigned long time = millis() - start;
    int time_high;
    if(time < 1000){
      time_high = 1000;
    } 
    else if(time < 2000){
      time_high = 2000;
    } 
    else {
      time_high = 1000;
    }

    digitalWrite(PIN_S, HIGH);
    delayMicroseconds(time_high);
    digitalWrite(PIN_S, LOW);
    delayMicroseconds(20000 - time_high);
  }

  while(true){
    int ADC_value = analogRead(PIN_POT);
    int time_high = map(
      ADC_value,
      0,
      4095,
      1000,
      2000
    );

    digitalWrite(PIN_S, HIGH);
    delayMicroseconds(time_high);
    digitalWrite(PIN_S, LOW);
    delayMicroseconds(20000 - time_high);
  }
}

void setup(){
  pinMode(PIN_S, OUTPUT);
  pinMode(PIN_POT, INPUT);
  analogReadResolution(12);
  xTaskCreatePinnedToCore(
    PWM_motor_task,
    "Motor_Task",
    4096,
    NULL,
    3,
    NULL,
    1
  );
}

void loop(){
  vTaskDelay(pdMS_TO_TICKS(1000));
}