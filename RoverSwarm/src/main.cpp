#include <Arduino.h>

#include "task.h"
#include "FreeRTOS.h"
#include "config.h"
#include "Wire.h"

#include "freertos/task.h"


//global variables

/*<-----    Shared variables    ----->*/
uint16_t pitch;
uint16_t roll;
uint16_t yaw;
float distanceL;
float distanceR;
// uint16_t photon0;
// uint16_t photon1;
// uint16_t photon2;
// uint16_t photon3;


void scanI2C() {
  Serial.println("Scanning I2C bus...");
  uint8_t count = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("I2C device found at 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
      count++;
    }
  }
  if (count == 0) {
    Serial.println("No I2C devices found.");
  }
  Serial.println("");
  delay(2000);
}

void setup() {
  Serial.begin(115200);
  scanI2C();
  //Create Tasks//
  #if LED_ENABLE
    // xTaskCreate(
    //   ledIndicate,        //Function Name
    //   "LED",              //Text Name
    //   2500,               //Stack size (bytes)
    //   NULL,               //Parameters
    //   LED_PRIORITY,       // Priority
    //   &ledTaskHandle        // Pointer
    // );
  #endif


  #if ORIENTATION_ENABLE
    xTaskCreate(
      measureOrientation,         //Function Name
      "Orientation",              //Text Name
      2500,                       //Stack size (bytes)
      NULL,                       //Parameters
      ORIENTATION_PRIORITY,       // Priority
      &orientationTaskHandle         // Pointer
    );
  #endif

  #if DISTANCE_ENABLE_R
  xTaskCreate(
    measureDistanceR,              //Function Name
    "DistanceRight",                   //Text Name
    5000,                       //Stack size (bytes)
    NULL,                         //Parameters
    DISTANCE_PRIORITY_R,            // Priority
    &distanceRTaskHandle             // Pointer
  );

  #endif

  #if DISTANCE_ENABLE_L

  xTaskCreate(
    measureDistanceL,              //Function Name
    "DistanceLeft",                   //Text Name
    5000,                       //Stack size (bytes)
    NULL,                         //Parameters
    DISTANCE_PRIORITY_L,            // Priority
    &distanceLTaskHandle             // Pointer
  );
#endif

#if PHOTON_ENABLE
// xTaskCreate(
//   measurePhoton,              //Function Name
//   "PHOTON",                   //Text Name
//   2500,                       //Stack size (bytes)
//   NULL,                         //Parameters
//   PHOTON_PRIORITY,            // Priority
//   &reflectTaskHandle             // Pointer
// );
#endif


}

void loop() {
  Serial.println("Starting Loop...");
  vTaskDelay(pdMS_TO_TICKS(1000));
}
