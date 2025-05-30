#include "task.h"
#include "config.h"
#include <Arduino.h>
#include <VL53L1X.h>
#include <Wire.h>
#include "TCA9548A.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <MPU6050_light.h>



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
  float dt = 1/I2C_FREQ;
  // Attempt to get sensor readings.
  bool readSuccess = imu.getEvent(&a, &g, &temp);
  if (!readSuccess) {
    Serial.println("Failed to get sensor event");
  } else {
    xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
    float localRoverYaw = RoverState.az;
    xSemaphoreGive(RoverState.mutex);
    localRoverYaw+=g.gyro.z/131*dt;
    xSemaphoreTake(RoverState.mutex, portMAX_DELAY);
    RoverState.az = localRoverYaw;
    xSemaphoreGive(RoverState.mutex);
    Serial.println(g.gyro.z);
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
      setupDistL(); //sets up the parameter for ToF Sensor'
      if(RoverState.mutex==NULL){
        Serial.println("No Mutex");
      }
      measureDistL();
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
      // Serial.println();
      // Serial.print("Right: ");
      // Serial.print(RoverState.distanceR);
      // Serial.println();
      }
      setupIMU(); //sets up the parameter for IMU
      if(RoverState.mutex==NULL){
        Serial.println("No Mutex");
      }
      else{
      measureIMU();
      // Serial.println(RoverState.ax);
      // Serial.println(RoverState.ay);
      // Serial.println(RoverState.az);
      // Serial.println(RoverState.gx);
      // Serial.println(RoverState.gy);
      // Serial.println(RoverState.gz);
      }
    }
}