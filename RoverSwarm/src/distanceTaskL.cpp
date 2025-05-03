#include "task.h"
#include "config.h"
#include <Arduino.h>
#include <VL53L1X.h>
#include <Wire.h>

#define ToF_L_SDA 4
#define ToF_L_SCL 5

TwoWire TL2 = TwoWire(0);
VL53L1X ToF_L;

// === GLOBAL VARIABLES === //
extern float distanceL;
// Task handles
TaskHandle_t distanceLTaskHandle = nullptr;

void setupDistL() {
  TL2.begin(ToF_L_SDA, ToF_L_SCL);
  TL2.setClock(400000);
  ToF_L.setBus(&TL2);
  if (!ToF_L.init()){
    Serial.println("Failed to detect and initalise sensor!");
    while(1);
  }
  ToF_L.setDistanceMode(VL53L1X::Short);
  ToF_L.setMeasurementTimingBudget(20000);
  ToF_L.startContinuous(20);
  
}

void measureDistL() {
    distanceL = ToF_L.read();
    if(ToF_L.timeoutOccurred()){
        Serial.print(" TIMEOUT");
        Serial.println();
    }
}

void measureDistanceL(void *pvParameters) {
    (void)pvParameters;

    /* Make the task execute at a specified frequency */
    const TickType_t xFrequency = configTICK_RATE_HZ / DISTANCE_FREQ_L;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    setupDistL(); //sets up the parameter for ToF Sensor
    Serial.println("Set up measureDistanceTask");

    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        measureDistL();
        Serial.println("Left:");
        Serial.print(distanceL);
    }
}