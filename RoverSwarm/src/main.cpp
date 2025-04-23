#include <Arduino.h>
#include <VL53L1X.h>
#include <Wire.h>

#define ToF_SDA_L 12
#define ToF_SCL_L 9

TwoWire TL = TwoWire(0);
VL53L1X ToF_L;

void setup() {
  Serial.begin(115200);
  TL.begin(ToF_SDA_L, ToF_SCL_L);
  TL.setClock(400000);
  ToF_L.setBus(&TL);
  ToF_L.setTimeout(500);
  if (!ToF_L.init()){
    Serial.println("Failed to detect and initalise sensor!");
    while(1);
  }
  ToF_L.setDistanceMode(VL53L1X::Short);
  ToF_L.setMeasurementTimingBudget(20000);
  ToF_L.startContinuous(20);
  
}

void loop() {
  Serial.println("Start Loop");
  Serial.println(ToF_L.read());
  if(ToF_L.timeoutOccurred()){
    Serial.print(" TIMEOUT");
    Serial.println();
  }
}
