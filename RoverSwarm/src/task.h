#ifndef TASK_H
#define TASK_H

#include <Arduino.h>



/*<-----    Task handles    ----->*/
extern TaskHandle_t ledIndicate;
extern TaskHandle_t measureObst;
extern TaskHandle_t measureOrientation;
extern TaskHandle_t measureDistance;
extern TaskHandle_t measurePhoton;

/*<-----    Task functions  ----->*/

void measureObst(void *pvParameter);
void measureOrientation(void *pvParameter);
void measureDistance(void *pvParameter);
void measurePhoton(void *pvParameter);
void ledIndicate(void *pvParameter);


/*<-----    Shared variables    ----->*/
uint16_t obstacleLeft;
uint16_t obstacleRight;
uint16_t pitch;
uint16_t roll;
uint16_t yaw;
uint16_t distance;
uint16_t photon0;
uint16_t photon1;
uint16_t photon2;
uint16_t photon3;

/*<-----    Shared structs  ----->*/
typedef enum MessageType {
    ORIENTATION,
    SPEED,
    WALLDIST_L,
    WALLDIST_R,
    DISTANCE,
    PHOTO0,
    PHOTO1,
    PHOTO2,
    PHOTO3
};
 

struct Message {
    MessageType type;
    union {
        bool pingValue;     // PING_IN, PING_OUT
        int errorCode;      // ERROR
        int motorValue;     // MOTOR
    };
};

#endif