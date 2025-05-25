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


void sendData(void *pvParameters) {
    (void)pvParameters;

    /* Make the task execute at a specified frequency */
    const TickType_t xFrequency = configTICK_RATE_HZ / DATA_FREQ;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    for (;;)
    {
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
      xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
      client.write(RoverState.distanceR);
      client.write(RoverState.distanceL);
      int16_t localDistanceR = RoverState.distanceR;
      int16_t localDistanceL = RoverState.distanceL;
      xSemaphoreGive(RoverState.mutex);
    }
}