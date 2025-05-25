#include "task.h"
#include "config.h"
#include <Arduino.h>

// === GLOBAL VARIABLES === //
// Task handles
TaskHandle_t driveTaskHandle = nullptr;



void driveL(){
    Serial.println("Driving left");
    ledcWrite(0, 0);
    ledcWrite(1, 255);
}

void driveR(){
    Serial.println("Drivin right");
    ledcWrite(2, 0);
    ledcWrite(3, 255);
}

 
void driveRover(void *pvParameters) {
    (void)pvParameters;

    /* Make the task execute at a specified frequency */
    const TickType_t xFrequency = configTICK_RATE_HZ / DRIVE_FREQ;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
      driveL();
      driveR();
    }
}