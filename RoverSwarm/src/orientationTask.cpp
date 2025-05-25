#include "task.h"
#include "config.h"
#include <Wire.h>
// === GLOBAL VARIABLES === //

// Task handles
TaskHandle_t orientationTaskHandle = nullptr;





//--------TASK--------
void measureOrientation(void *pvParameters) {
    (void)pvParameters;

    /* Make the task execute at a specified frequency */
    const TickType_t xFrequency = configTICK_RATE_HZ / ORIENTATION_FREQ;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    Serial.println("Set up serialTask");

    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
