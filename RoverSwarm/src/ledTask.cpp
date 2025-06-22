#include "task.h"
#include "config.h"
#include <Arduino.h>

// === GLOBAL VARIABLES === //
// Task handles
TaskHandle_t ledTaskHandle = nullptr;

bool flashFlag = false;
int count = 0;

void checkstatus(){
    xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
    int status = RoverState.status;
    xSemaphoreGive(RoverState.mutex);
    if(status==0){
        flashFlag = false;
    }
    else if(status == 1){ //Connecting to TCP Socket
        if(count < 20){
            flashFlag = true;
        }
        else{
            flashFlag = false;
        }
    }
    else if (status == 2){ //One of the sensors is not connected
        if(count < 10){
            flashFlag = true;
        }
        else{
            flashFlag = false;
        }
    }
}



 
void ledIndicate(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    /* Make the task execute at a specified frequency */
    const TickType_t xDelay = pdMS_TO_TICKS(100);
    for (;;)
    {
        vTaskDelay(xDelay);
        checkstatus();
        if(flashFlag){
            count+=1;
            pinMode(LED_PIN, HIGH);
        }
        else{
            count = 0;
            pinMode(LED_PIN, HIGH);
        }
        TickType_t endTask = xTaskGetTickCount();
      Serial.println("LEDTask Timing");
      Serial.println(xLastWakeTime-endTask);
    }
}