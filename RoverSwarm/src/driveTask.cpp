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
const float K_P = 1.6;
const float K_D = 0.8;
float prevError = 0;
int basePWM = 128;

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
    if(distanceL==0 and distanceR==0){
      ledcWrite(1, 0);
      ledcWrite(2, 0);
    }
    else{
      int error = distanceL-distanceR;
      float comp = 0.8*(K_P*error + K_D*(error/HARDWARE_TIMER_PRESCALER));
      if(comp<0){
        ledcWrite(0 , 0);
        ledcWrite(1, basePWM - comp); //left motor change
        ledcWrite(2, 0);
        ledcWrite(3, basePWM + comp);
        Serial.print("0: 0, 1:");
        Serial.print(basePWM-comp);
        Serial.print("2:0, 3:");
        Serial.println(basePWM + comp);
      }
      else{
        ledcWrite(0, basePWM-comp);
        ledcWrite(1, 0);
        ledcWrite(2, basePWM + comp); //right motor change
        ledcWrite(3, 0);
        Serial.print("0: ");
        Serial.print(basePWM-comp);
        Serial.print("1:0, 2:");
        Serial.print(basePWM + comp);
        Serial.println("3:0");
      }
    }
    
    
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
      float distanceR = (PI*TYRE_DIAMETER*encoderCountR)/(ENCODER_RES);
      float distanceL = (PI*TYRE_DIAMETER*encoderCountL/(ENCODER_RES));
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