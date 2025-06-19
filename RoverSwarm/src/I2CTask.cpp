#include "task.h"
#include "config.h"
#include <Arduino.h>
#include <VL53L1X.h>
#include <Wire.h>
#include "TCA9548A.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <MPU6050_light.h>

extern VL53L1X ToF_L, ToF_R;
extern TCA9548A I2CMux;
extern Adafruit_MPU6050 imu;  //Initialise Object

// === GLOBAL VARIABLES === //
// Task handles
TaskHandle_t I2CTaskHandle = nullptr;
const uint8_t avgFilterLength = 4;
float prevYaw[avgFilterLength];


void measureDistR() {
  I2CMux.openChannel(Channel_ToF_R);
  int16_t distanceR = ToF_R.read();
  if(ToF_R.timeoutOccurred()){
      Serial.print(" TIMEOUT");
      distanceR = -1;
  }
  xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
  RoverState.distanceR = distanceR;
  xSemaphoreGive(RoverState.mutex);
  I2CMux.closeChannel(Channel_ToF_R);
}

void measureDistL() {
  I2CMux.openChannel(Channel_ToF_L);
  int16_t distanceL =ToF_L.read();
  if(ToF_L.timeoutOccurred()){
    Serial.print(" TIMEOUT");
    Serial.println();
  }
  xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
  RoverState.distanceL = distanceL;
  xSemaphoreGive(RoverState.mutex);
  I2CMux.closeChannel(Channel_ToF_L);
}

void measureIMU(){
  I2CMux.openChannel(Channel_IMU);
  sensors_event_t a, g, temp;
  float dt = 0.01;
  float sum = 0;
  // Attempt to get sensor readings.
  bool readSuccess = imu.getEvent(&a, &g, &temp);
  if (!readSuccess) {
    Serial.println("Failed to get sensor event");
  } else {
    for(int i=0; i<avgFilterLength-1; i++){
      prevYaw[i+1] = prevYaw[i];
    }
    
    xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
    float localRoverYaw = RoverState.yaw;
    xSemaphoreGive(RoverState.mutex);
    prevYaw[0] = localRoverYaw + g.gyro.z*dt;  
    for(int i=0; i<avgFilterLength; i++){
      sum+=prevYaw[i];
    }
    localRoverYaw = sum/avgFilterLength;
    xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
    RoverState.yaw = localRoverYaw;
    xSemaphoreGive(RoverState.mutex);
    // Serial.println(localRoverYaw);
    // Serial.println(g.gyro.z);
    // Serial.println(dt);
  }
  I2CMux.closeChannel(Channel_IMU);
}


void measureSensor(void *pvParameters) {
    (void)pvParameters;

    /* Make the task execute at a specified frequency */
    const TickType_t xFrequency = configTICK_RATE_HZ / I2C_FREQ;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
      if(EN_TOF_L){
        measureDistL();
        // Serial.println();
        // Serial.print("Left: ");
        // Serial.print(RoverState.distanceL);
        // Serial.println();
      }
      if(EN_TOF_R){
        measureDistR();
        // Serial.println();
        // Serial.print("Right: ");
        // Serial.print(RoverState.distanceR);
        // Serial.println();
        }
        if(EN_IMU){
        measureIMU();
        }
      }
    TickType_t endTask = xTaskGetTickCount();
    Serial.println("I2cTask Timing");
    Serial.println(xLastWakeTime-endTask);
      
}