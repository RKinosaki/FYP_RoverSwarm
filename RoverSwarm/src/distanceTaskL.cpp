#include "task.h"
#include "config.h"
#include <Arduino.h>
#include <VL53L1X.h>
#include <Wire.h>

#define ToF_SDA 4
#define ToF_SCL 5

TwoWire TL = TwoWire(0);
VL53L1X ToF;

// === GLOBAL VARIABLES === //

// Task handles
TaskHandle_t distanceLTaskHandle = nullptr;

void setupDist() {
  TL.begin(ToF_SDA, ToF_SCL);
  TL.setClock(400000);
  ToF.setBus(&TL);
  ToF.setTimeout(500);
  if (!ToF.init()){
    Serial.println("Failed to detect and initalise sensor!");
    while(1);
  }
  ToF.setDistanceMode(VL53L1X::Short);
  ToF.setMeasurementTimingBudget(20000);
  ToF.startContinuous(20);
  
}

float measureDist() {
    float distance = ToF.read();
    if(ToF.timeoutOccurred()){
        Serial.print(" TIMEOUT");
        Serial.println();
    }
    return distance;
}

void measureDistanceL(void *pvParameters) {
    (void)pvParameters;

    /* Make the task execute at a specified frequency */
    const TickType_t xFrequency = configTICK_RATE_HZ / ORIENTATION_FREQ;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    setupDist(); //sets up the parameter for ToF Sensor
    Serial.println("Set up measureDistanceTask");

    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        //put the distance value 
        distanceL = measureDist();
    }
}