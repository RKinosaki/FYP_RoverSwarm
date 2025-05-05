#include "task.h"
#include "config.h"
#include <Arduino.h>
#include <VL53L1X.h>
#include <Wire.h>
#include "TCA9548A.h"


VL53L1X ToF_L, ToF_R;
extern TCA9548A I2CMux;

// === GLOBAL VARIABLES === //
// Task handles
TaskHandle_t distanceLTaskHandle = nullptr;

void setupDistL() {
  // Serial.println("Setting up distL!");
  I2CMux.openChannel(5);
  if (!ToF_L.init()){
    Serial.println("Failed to detect and initalise sensor!");
    while(1);
  }
  ToF_L.setDistanceMode(VL53L1X::Short);
  ToF_L.setMeasurementTimingBudget(20000);
  ToF_L.startContinuous(20);
  // Serial.println("Set up measureDistanceTask");
}

void setupDistR() {
  I2CMux.openChannel(4);
  if (!ToF_R.init()){
    Serial.println("Failed to detect and initalise sensor!");
    while(1);
  }
  ToF_R.setDistanceMode(VL53L1X::Short);
  ToF_R.setMeasurementTimingBudget(20000);
  ToF_R.startContinuous(20);
  
}

int16_t measureDistR() {
    int16_t distanceR = ToF_R.read();
    if(ToF_R.timeoutOccurred()){
        Serial.print(" TIMEOUT");
        distanceR = -1;
    }
    I2CMux.closeChannel(4);
  return distanceR;
}

int16_t measureDistL() {
  int16_t distanceL =ToF_L.read();
    if(ToF_L.timeoutOccurred()){
        Serial.print(" TIMEOUT");
        Serial.println();
    }
    I2CMux.closeChannel(5);
  return distanceL;
}

void measureDistanceL(void *pvParameters) {
    (void)pvParameters;

    /* Make the task execute at a specified frequency */
    const TickType_t xFrequency = configTICK_RATE_HZ / DISTANCE_FREQ_L;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
      setupDistL(); //sets up the parameter for ToF Sensor'
      if(RoverState.mutex==NULL){
        Serial.println("No Mutex");
      }
      xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
      RoverState.distanceL = measureDistL();
      xSemaphoreGive(RoverState.mutex);
      Serial.println();
      Serial.print("Left: ");
      Serial.print(RoverState.distanceL);
      Serial.println();

      setupDistR(); //sets up the parameter for ToF Sensor
      if(RoverState.mutex==NULL){
        Serial.println("No Mutex");
      }
      else{
      xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
      RoverState.distanceR = measureDistR();
      xSemaphoreGive(RoverState.mutex);
      Serial.println();
      Serial.print("Right: ");
      Serial.print(RoverState.distanceR);
      Serial.println();
      }
    }
}