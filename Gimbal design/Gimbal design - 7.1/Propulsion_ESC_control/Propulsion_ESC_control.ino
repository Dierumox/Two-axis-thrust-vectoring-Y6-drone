#include <Arduino.h>

const int PIN_S = 6;
const int PIN_POT = 1;
volatile int microsegundos_compartidos = 1000;

void tareaMotorPWM(void *pvParameters);

void setup() {
  analogReadResolution(12);
  xTaskCreatePinnedToCore(
    tareaMotorPWM,
    "Tarea_Motor",
    4096,
    NULL,
    3,
    NULL,
    1
  );
}

void tareaMotorPWM(void *pvParameters) {
  pinMode(PIN_S, OUTPUT);
  pinMode(PIN_POT, INPUT);
  unsigned long inicio = millis();
  while (millis() - inicio < 5000) {
    unsigned long tiempo = millis() - inicio;
    int tiempo_alto;
    if (tiempo < 1000) {
      tiempo_alto = 1000;
    }
    else if (tiempo < 2000) {
      tiempo_alto = 2000;
    }
    else {
      tiempo_alto = 1000;
    }

    digitalWrite(PIN_S, HIGH);
    delayMicroseconds(tiempo_alto);
    digitalWrite(PIN_S, LOW);
    delayMicroseconds(20000 - tiempo_alto);
  }

  while (true) {
    int valorADC = analogRead(PIN_POT);
    int tiempo_alto = map(
      valorADC,
      0,
      4095,
      1000,
      2000
    );

    microsegundos_compartidos = tiempo_alto;
    digitalWrite(PIN_S, HIGH);
    delayMicroseconds(tiempo_alto);
    digitalWrite(PIN_S, LOW);
    delayMicroseconds(20000 - tiempo_alto);
  }
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}