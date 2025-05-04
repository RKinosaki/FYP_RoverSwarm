#include "task.h"
#include "config.h"
#include <Arduino.h>
#include "WiFi.h"

// === GLOBAL VARIABLES === //
extern SemaphoreHandle_t mutex;
extern float distanceR;
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
      xSemaphoreTake(mutex, portMAX_DELAY);
      float localDistanceR = distanceR;
      xSemaphoreGive(mutex);
      client.write(distanceR);
    }
}