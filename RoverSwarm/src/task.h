#ifndef TASK_H
#define TASK_H

#include <Arduino.h>
#include <array>



/*<-----    Task handles    ----->*/
extern TaskHandle_t ledTaskHandle;
extern TaskHandle_t orientationTaskHandle;
extern TaskHandle_t I2CTaskHandle;
extern TaskHandle_t sendDataTaskHandle;
extern TaskHandle_t reflectTaskHandle;
extern TaskHandle_t driveTaskHandle;

/*<-----    Task functions  ----->*/

void measureOrientation(void *pvParameter);
void measureSensor(void *pvParameter);
void sendData(void *pvParameter);
void measurePhoton(void *pvParameter);
void driveRover(void *pvParameter);
// void ledIndicate(void *pvParameter);
 

/*<-----    Shared variables    ----->*/
struct sysState {
    volatile int16_t distanceL;
    volatile int16_t distanceR;
    volatile float pitch;
    volatile float roll;
    volatile float yaw;
    volatile int16_t travelled;
    volatile std::array<float, 4> photon;
    volatile char command;
    volatile uint8_t status;
    SemaphoreHandle_t mutex;
  };

  extern sysState RoverState;

#endif