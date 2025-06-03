#include "task.h"
#include "config.h"
#include <Arduino.h>

// === GLOBAL VARIABLES === //
// Task handles
TaskHandle_t driveTaskHandle = nullptr;
volatile int encoderCountL = 0;
volatile int encoderCountR = 0;
int prevCountR = 0;
int prevCountL = 0;
const float K_P = 1.2;
const float K_D = 0.8;
float prevError = 0;
int basePWM = 64;

volatile bool controlFlag = false;

// === INTERRUPT === //

void IRAM_ATTR controlMotorISR(){
    portENTER_CRITICAL_ISR(&timerMux);
    controlFlag = true;
    portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR sampleEncoderISRL(){
    bool B = digitalRead(EL_B);
        if(B){
            encoderCountL--;
        }
        else{
            encoderCountL++;
        }
    }

    void IRAM_ATTR sampleEncoderISRR(){
    bool B = digitalRead(ER_B);
        if(B){
            encoderCountR--;
        }
        else{
            encoderCountR++;
        }
    }


void drive(){
    xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
    int distanceL = RoverState.distanceL;
    int distanceR = RoverState.distanceR;
    xSemaphoreGive(RoverState.mutex);
    int error = distanceL-distanceR;
    float comp = K_P*error + K_D*(error/HARDWARE_TIMER_PRESCALER);
    ledcWrite(1, basePWM - comp); //left motor change
    ledcWrite(3, basePWM + comp); //right motor change
    // Serial.println("Compensation is: ");
    // Serial.println(comp);
    controlFlag = false;
}



 
void driveRover(void *pvParameters) {
    (void)pvParameters;

    /* Make the task execute at a specified frequency */
    const TickType_t xFrequency = configTICK_RATE_HZ / DRIVE_FREQ;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
      bool controlFlag_ = controlFlag;
    //   Serial.println(controlFlag_);
      if(controlFlag_){
        drive();
      }
      int distanceR = ENCODER_RES*encoderCountR/(2*3.1415);
      int distanceL = ENCODER_RES*encoderCountL/(2*3.1415);
      if(distanceR != prevCountR){
        Serial.println("Right Encoder count is: ");
        Serial.println(distanceR);
        prevCountR = distanceR;
        xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
        RoverState.travelled = distanceR;
        xSemaphoreGive(RoverState.mutex);
      }
      if (distanceL != prevCountL){
        Serial.println("Left Encoder count is: ");
        Serial.println(distanceL);
        prevCountL = distanceL;
      }
    }
    
}