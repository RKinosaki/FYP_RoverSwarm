#include <Arduino.h>

#include "task.h"
#include "FreeRTOS.h"
#include "config.h"
#include "Wire.h"

#include "freertos/task.h"





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

  //Create Tasks//
  #if LED_ENABLE
    xTaskCreate(
      ledIndicate,        //Function Name
      "LED",              //Text Name
      2500,               //Stack size (bytes)
      NULL,               //Parameters
      LED_PRIORITY,       // Priority
      &ledTaskHandle        // Pointer
    );
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

  #if DISTANCE_ENABLE
  xTaskCreate(
    measureDistanceL,              //Function Name
    "Distance",                   //Text Name
    2500,                       //Stack size (bytes)
    NULL,                         //Parameters
    DISTANCE_PRIORITY,            // Priority
    &distanceRTaskHandle             // Pointer
  );

  xTaskCreate(
    measureDistanceR,              //Function Name
    "Distance",                   //Text Name
    2500,                       //Stack size (bytes)
    NULL,                         //Parameters
    DISTANCE_PRIORITY,            // Priority
    &distanceLTaskHandle             // Pointer
  );
#endif

#if PHOTON_ENABLE
xTaskCreate(
  measurePhoton,              //Function Name
  "PHOTON",                   //Text Name
  2500,                       //Stack size (bytes)
  NULL,                         //Parameters
  PHOTON_PRIORITY,            // Priority
  &reflectTaskHandle             // Pointer
);
#endif


}

void loop() {

  vTaskDelay(pdMS_TO_TICKS(1000));
}
