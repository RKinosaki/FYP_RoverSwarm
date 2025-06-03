#include "task.h"
#include "config.h"
#include <Arduino.h>
#include "WiFi.h"
#include <ArduinoJson.h>
#include <array>


// === GLOBAL VARIABLES === //
// Task handles
TaskHandle_t sendDataTaskHandle = nullptr;
extern WiFiClient client;


JsonDocument package;

void updatePackage(){
  package["distance"][0] = RoverState.distanceL;
  package["distance"][1] = RoverState.distanceR;
  package["IMU"]["a"][0] = RoverState.ax; 
  package["IMU"]["a"][1] = RoverState.ay; 
  package["IMU"]["a"][2] = RoverState.az; 
  package["IMU"]["g"][0] = RoverState.gx; 
  package["IMU"]["g"][1] = RoverState.gy; 
  package["IMU"]["g"][2] = RoverState.gz; 
  package["Photon"][0] = RoverState.photon0;
  package["Photon"][1] = RoverState.photon1;
  package["Photon"][2] = RoverState.photon2;
  package["Photon"][3] = RoverState.photon3;
  package["travelled"] = RoverState.travelled;
  package["status"] = RoverState.status;
}


void sendData(void *pvParameters) {
    (void)pvParameters;

    /* Make the task execute at a specified frequency */
    const TickType_t xFrequency = configTICK_RATE_HZ / DATA_FREQ;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    for (;;)
    {
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
      xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
      updatePackage();
      // serializeJson(package, Serial);
      serializeJson(package, client);
      client.write('\n');
      xSemaphoreGive(RoverState.mutex);
    }
}