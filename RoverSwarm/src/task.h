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
void ledIndicate(void *pvParameter);
 

/*<-----    Shared variables    ----->*/
struct sysState {
    const uint8_t id = 1;
    volatile float distanceL = 0;
    volatile float distanceR = 0;
    volatile float yaw = 0;
    volatile float encoderL = 0;
    volatile float encoderR = 0;
    volatile float photon0 = 0;
    volatile float photon1 = 0;
    volatile float photon2 = 0;
    volatile float photon3 = 0;
    volatile char command = 'f';
    volatile uint8_t status = 0;
    SemaphoreHandle_t mutex;
  };

  extern sysState RoverState;

#endif

void IRAM_ATTR controlMotorISR();
void IRAM_ATTR sampleEncoderISRR();
void IRAM_ATTR sampleEncoderISRL();

extern portMUX_TYPE timerMux;