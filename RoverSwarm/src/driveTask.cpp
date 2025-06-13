#include "task.h"
#include "config.h"
#include <Arduino.h>

// === GLOBAL VARIABLES === //
// Task handles
TaskHandle_t driveTaskHandle = nullptr;
volatile int encoderCountL = 0;
volatile int encoderCountR = 0;
volatile bool controlFlag = false;
int prevCount[2] = {0, 0}; //L, R
const float GPD[3]={0.2, 0.4, 0.005}; //Gain, K_P, K_D constants for PD controller
float prevError = 0;
const int basePWM = 128;
const int maxPWM = 196;
const int threshold[2] = {80, 40}; //error margin, distance threshold
const int avgFilterLength = 600;
int prevDistL[avgFilterLength];
int prevDistR[avgFilterLength];
int avgL, avgR;

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

void stop(){
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
}


void drive(){
    xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
    int distanceL = RoverState.distanceL;
    int distanceR = RoverState.distanceR;
    xSemaphoreGive(RoverState.mutex);
    float sumL = 0;
    float sumR = 0;
    //Moving average filter
    for (int i = 0; i<avgFilterLength-1; i++){
      prevDistL[i+1] = prevDistL[i];
      prevDistR[i+1] = prevDistR[i];
    }
    prevDistL[0] = distanceL;
    prevDistR[0] = distanceR;
    for (int i=0; i<avgFilterLength; i++){
      sumL+=prevDistL[i];
      sumR+=prevDistR[i];
    }
    avgL = sumL/avgFilterLength;
    avgR = sumR/avgFilterLength;
    int error = avgL-avgR;
    if(error<threshold[0] and error > -threshold[0]){
      ledcWrite(0, 0);
      ledcWrite(1, basePWM);
      ledcWrite(2, basePWM);
      ledcWrite(3, 0);
    }
    else if(distanceR< threshold[1] and distanceL < threshold[1]){
      ledcWrite(0, basePWM);
      ledcWrite(1, 0);
      ledcWrite(2, 0);
      ledcWrite(3, basePWM);
    }
    else{
      float comp = GPD[0]*(GPD[1]*error + GPD[2]*((error-prevError)/HARDWARE_TIMER_PRESCALER));
      prevError = error;
      if(comp<0){
        ledcWrite(0 , 0);
        ledcWrite(1, constrain(basePWM - comp, 48, maxPWM)); //left motor change
        ledcWrite(2, 0);
        ledcWrite(3, constrain(basePWM + comp, 48, maxPWM));
          // Serial.print("0: 0, 1:");
          // Serial.print(basePWM-comp);
          // Serial.print("2:0, 3:");
          // Serial.println(basePWM + comp);
      }
      else{
        ledcWrite(0, constrain(basePWM-comp, 40, maxPWM));
        ledcWrite(1, 0);
        ledcWrite(2, constrain(basePWM + comp, 40, maxPWM)); //right motor change
        ledcWrite(3, 0);
          // Serial.print("0: ");
          // Serial.print(basePWM-comp);
          // Serial.print("1:0, 2:");
          // Serial.print(basePWM + comp);
          // Serial.println("3:0");
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
      xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
      Serial.println(RoverState.command);
      xSemaphoreGive(RoverState.mutex);
      if(RoverState.command=='s'){
        stop();
      }
      else{
        if(controlFlag_){
          drive();
        }
      }
      float distanceR = (PI*TYRE_DIAMETER*encoderCountR)/(ENCODER_RES);
      float distanceL = -(PI*TYRE_DIAMETER*encoderCountL/(ENCODER_RES));
      if(distanceR != prevCount[1]){
        // Serial.println("Right Encoder count is: ");
        // Serial.println(distanceR);
        prevCount[1] = distanceR;
        xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
        RoverState.encoderR = distanceR;
        xSemaphoreGive(RoverState.mutex);
      }
      if (distanceL != prevCount[0]){
        // Serial.println("Left Encoder count is: ");
        // Serial.println(distanceL);
        xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
        RoverState.encoderL = distanceL;
        xSemaphoreGive(RoverState.mutex);
        prevCount[0] = distanceL;
      }
      TickType_t endTask = xTaskGetTickCount();
      Serial.println("DriveTask Timing");
      Serial.println(xLastWakeTime-endTask);
    }
    
}
