#ifndef TASK_H
#define TASK_H

#include <Arduino.h>



/*<-----    Task handles    ----->*/
extern TaskHandle_t ledTaskHandle;
extern TaskHandle_t orientationTaskHandle;
extern TaskHandle_t distanceLTaskHandle;
extern TaskHandle_t sendDataTaskHandle;
// extern TaskHandle_t reflectTaskHandle;

/*<-----    Task functions  ----->*/

void measureOrientation(void *pvParameter);
void measureDistanceL(void *pvParameter);
void sendData(void *pvParameter);
// void measurePhoton(void *pvParameter);
// void ledIndicate(void *pvParameter);
 

/*<-----    Shared variables    ----->*/
struct sysState {
    volatile int16_t distanceL;
    volatile int16_t distanceR;
    volatile int16_t pitch;
    volatile int16_t roll;
    volatile int16_t yaw;
    SemaphoreHandle_t mutex;
  };
  // uint16_t photon0;
  // uint16_t photon1;
  // uint16_t photon2;
  // uint16_t photon3;

  extern sysState RoverState;

#endif