#include "task.h"
#include "config.h"
#include <Arduino.h>

// === GLOBAL VARIABLES === //
// Task handles
TaskHandle_t reflectTaskHandle = nullptr;

float readPhoton(int pin){
    return analogRead(pin);
}
 
void measurePhoton(void *pvParameters) {
    (void)pvParameters;

    /* Make the task execute at a specified frequency */
    const TickType_t xFrequency = configTICK_RATE_HZ / PHOTON_FREQ;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
      Serial.println("Sensor 0: ");
      Serial.print(readPhoton(R0));
      Serial.println("Sensor 1: ");
      Serial.print(readPhoton(R1));
      Serial.println("Sensor 2: ");
      Serial.print(readPhoton(R2));
      Serial.println("Sensor 3: ");
      Serial.print(readPhoton(R3));
    }
}