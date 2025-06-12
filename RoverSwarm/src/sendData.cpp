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
char prevCommand = 's';

JsonDocument package;

void updatePackage(){
  xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
  package["distance"][0] = RoverState.distanceL;
  package["distance"][1] = RoverState.distanceR;
  package["yaw"] = RoverState.yaw;
  package["Photon"][0] = RoverState.photon0;
  package["Photon"][1] = RoverState.photon1;
  package["Photon"][2] = RoverState.photon2;
  package["Photon"][3] = RoverState.photon3;
  package["encoder"][0] = RoverState.encoderL;
  package["encoder"][1] = RoverState.encoderR;
  package["status"] = RoverState.status;
  xSemaphoreGive(RoverState.mutex);
}

// void receiveCommand(){
//   char command = client.read();
//   if(command != prevCommand){
//     xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
//     RoverState.command = client.read();
//     Serial.println(RoverState.command);
//     xSemaphoreGive(RoverState.mutex);
//     prevCommand = command;
//   }
// }


void sendData(void *pvParameters) {
    (void)pvParameters;

    /* Make the task execute at a specified frequency */
    const TickType_t xFrequency = configTICK_RATE_HZ / DATA_FREQ;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    for (;;)
    {
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
      updatePackage();
      // serializeJson(package, Serial);
      serializeJson(package, client);
      client.write('\n');
      TickType_t endTask = xTaskGetTickCount();
      Serial.println("sendData Timing");
      Serial.println(xLastWakeTime-endTask);
    }
}