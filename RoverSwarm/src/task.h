#ifndef TASK_H
#define TASK_H

#include <Arduino.h>



/*<-----    Task handles    ----->*/
extern TaskHandle_t ledTaskHandle;
extern TaskHandle_t orientationTaskHandle;
extern TaskHandle_t distanceRTaskHandle;
extern TaskHandle_t distanceLTaskHandle;
// extern TaskHandle_t reflectTaskHandle;

/*<-----    Task functions  ----->*/

void measureOrientation(void *pvParameter);
void measureDistanceR(void *pvParameter);
void measureDistanceL(void *pvParameter);
// void measurePhoton(void *pvParameter);
// void ledIndicate(void *pvParameter);


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