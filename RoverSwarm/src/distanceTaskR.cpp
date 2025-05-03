#include "task.h"
#include "config.h"
#include <Arduino.h>
#include <VL53L1X.h>
#include <Wire.h>

#define ToF_R_SDA 6
#define ToF_R_SCL 7

TwoWire TL1 = TwoWire(1);
VL53L1X ToF_R;

// === GLOBAL VARIABLES === //
extern float distanceR;

// Task handles
TaskHandle_t distanceRTaskHandle = nullptr;

void setupDistR() {
  TL1.begin(ToF_R_SDA, ToF_R_SCL);
  TL1.setClock(400000);
  ToF_R.setBus(&TL1);
  if (!ToF_R.init()){
    Serial.println("Failed to detect and initalise sensor!");
    while(1);
  }
  ToF_R.setDistanceMode(VL53L1X::Short);
  ToF_R.setMeasurementTimingBudget(20000);
  ToF_R.startContinuous(20);
  
}

void measureDistR() {
    distanceR = ToF_R.read();
    if(ToF_R.timeoutOccurred()){
        Serial.print(" TIMEOUT");
        Serial.println();
    }
}

void measureDistanceR(void *pvParameters) {
    (void)pvParameters;

    /* Make the task execute at a specified frequency */
    const TickType_t xFrequency = configTICK_RATE_HZ / DISTANCE_FREQ_R;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    setupDistR(); //sets up the parameter for ToF Sensor
    Serial.println("Set up measureDistanceTask");
    
    for (;;)
    {
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
      measureDistR();
      Serial.println("Right:");
      Serial.print(distanceR);
    }
}