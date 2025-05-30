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

// === INTERRUPT === //

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


void driveL(){
    // Serial.println("Driving left");
    ledcWrite(0, 0);
    ledcWrite(1, 255);
}

void driveR(){
    // Serial.println("Drivin right");
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
      int distanceR = ENCODER_RES*encoderCountR/(2*3.1415);
      int distanceL = ENCODER_RES*encoderCountL/(2*3.1415);
      if(distanceR != prevCountR){
        Serial.println("Right Encoder count is: ");
        Serial.println(distanceR);
        prevCountR = distanceR;
        RoverState.travelled = distanceR;
      }
      if (distanceL != prevCountL){
        Serial.println("Left Encoder count is: ");
        Serial.println(distanceL);
        prevCountL = distanceL;
      }
    }
    
}