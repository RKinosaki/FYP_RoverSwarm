#include "task.h"
#include "config.h"
#include <Arduino.h>
#include <VL53L1X.h>
#include <Wire.h>
#include "TCA9548A.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>


VL53L1X ToF_L, ToF_R;
extern TCA9548A I2CMux;

Adafruit_MPU6050 imu;  //Initialise Object

// === GLOBAL VARIABLES === //
// Task handles
TaskHandle_t I2CTaskHandle = nullptr;

void setupIMU(){
  I2CMux.openChannel(Channel_IMU);
  if (!imu.begin()){     //Initialise IMU
      Serial.println("Failed to initialize MPU6050");
      while (1) {
      }
  }
  Serial.println("MPU6050 Found");
  imu.setAccelerometerRange(ORIENTATION_ACCEL_RANGE);
  imu.setGyroRange(ORIENTATION_GYRO_RANGE);
  imu.setFilterBandwidth(ORIENTATION_BANDWIDTH);
  
}

void setupDistL() {
  // Serial.println("Setting up distL!");
  I2CMux.openChannel(Channel_ToF_L);
  if (!ToF_L.init()){
    Serial.println("Failed to detect and initalise sensor!");
  }
  ToF_L.setDistanceMode(VL53L1X::Short);
  ToF_L.setMeasurementTimingBudget(20000);
  ToF_L.startContinuous(20);
  // Serial.println("Set up measureDistanceTask");
}

void setupDistR() {
  I2CMux.openChannel(Channel_ToF_R);
  if (!ToF_R.init()){
    Serial.println("Right sensor failed to detect and initalise sensor!");
  }
  ToF_R.setDistanceMode(VL53L1X::Short);
  ToF_R.setMeasurementTimingBudget(20000);
  ToF_R.startContinuous(20);
  
}

void measureDistR() {
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
    sensors_event_t a, g, temp;
  
  // Attempt to get sensor readings.
  bool readSuccess = imu.getEvent(&a, &g, &temp);
  if (!readSuccess) {
    Serial.println("Failed to get sensor event");
  } else {
  xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
  RoverState.ax = a.acceleration.x;
  RoverState.ay = a.acceleration.y;
  RoverState.az = a.acceleration.z;
  RoverState.gx = g.gyro.x;
  RoverState.gy = g.gyro.y;
  RoverState.gz = g.gyro.z;
  xSemaphoreGive(RoverState.mutex);
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
      // setupDistL(); //sets up the parameter for ToF Sensor'
      // if(RoverState.mutex==NULL){
      //   Serial.println("No Mutex");
      // }
      // Serial.println();
      // Serial.print("Left: ");
      // Serial.print(RoverState.distanceL);
      // Serial.println();

      setupDistR(); //sets up the parameter for ToF Sensor
      if(RoverState.mutex==NULL){
        Serial.println("No Mutex");
      }
      else{
      measureDistR();
      Serial.println();
      Serial.print("Right: ");
      Serial.print(RoverState.distanceR);
      Serial.println();
      }
      setupIMU(); //sets up the parameter for IMU
      if(RoverState.mutex==NULL){
        Serial.println("No Mutex");
      }
      else{
      measureIMU();
      Serial.print(RoverState.ax);
      Serial.print(RoverState.ay);
      Serial.print(RoverState.az);
      Serial.print(RoverState.gx);
      Serial.print(RoverState.gy);
      Serial.print(RoverState.gz);
      }
    }
}