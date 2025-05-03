#include "task.h"
#include "config.h"
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 imu;  //Initialise Object

#define IMU_SDA 8
#define IMU_SCL 9

// === GLOBAL VARIABLES === //

// Task handles
TaskHandle_t orientationTaskHandle = nullptr;


void setupIMU(){
    
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

void measureParam(){
    sensors_event_t a, g, temp;
  
  // Attempt to get sensor readings.
  bool readSuccess = imu.getEvent(&a, &g, &temp);
  if (!readSuccess) {
    Serial.println("Failed to get sensor event");
  } else {
    Serial.print("Acceleration X: ");
    Serial.print(a.acceleration.x);
    Serial.print(", Y: ");
    Serial.print(a.acceleration.y);
    Serial.print(", Z: ");
    Serial.print(a.acceleration.z);
    Serial.println(" m/s^2");

    Serial.print("Rotation X: ");
    Serial.print(g.gyro.x);
    Serial.print(", Y: ");
    Serial.print(g.gyro.y);
    Serial.print(", Z: ");
    Serial.print(g.gyro.z);
    Serial.println(" rad/s");

    Serial.print("Temperature: ");
    Serial.print(temp.temperature);
    Serial.println(" degC");

    Serial.println("");
  }
}

//--------TASK--------
void serialTask(void *pvParameters) {
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
