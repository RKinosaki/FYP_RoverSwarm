#include "task.h"
#include "config.h"
#include <Arduino.h>
#include <VL53L1X.h>
#include <Wire.h>

#define ToF_SDA 4
#define ToF_SCL 5

TwoWire TL = TwoWire(0);
VL53L1X ToF;

// === GLOBAL VARIABLES === //

// Task handles
TaskHandle_t distanceRTaskHandle = nullptr;

void setup() {
  Serial.begin(115200);
  TL.begin(ToF_SDA, ToF_SCL);
  TL.setClock(400000);
  ToF.setBus(&TL);
  ToF.setTimeout(500);
  if (!ToF.init()){
    Serial.println("Failed to detect and initalise sensor!");
    while(1);
  }
  ToF.setDistanceMode(VL53L1X::Short);
  ToF.setMeasurementTimingBudget(20000);
  ToF.startContinuous(20);
  
}

void loop() {
  Serial.println("Start Loop");
  Serial.println(ToF.read());
  if(ToF.timeoutOccurred()){
    Serial.print(" TIMEOUT");
    Serial.println();
  }
}