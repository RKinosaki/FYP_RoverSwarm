#include "task.h"
#include "config.h"
#include <Arduino.h>

// === GLOBAL VARIABLES === //
// Task handles
TaskHandle_t ledTaskHandle = nullptr;

void sendLight(bool on){
    if (on){
        digitalWrite(LED_PIN, HIGH);
    }
    else{
        digitalWrite(LED_PIN, LOW);
    }
    
}
 
void ledIndicate(void *pvParameters) {
    (void)pvParameters;

    /* Make the task execute at a specified frequency */
    const TickType_t xFrequency = configTICK_RATE_HZ / LED_FREQ;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
      sendLight(true);
    }
}