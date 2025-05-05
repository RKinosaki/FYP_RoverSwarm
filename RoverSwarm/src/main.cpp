#include <Arduino.h>
#include "task.h"
#include "FreeRTOS.h"
#include "config.h"
#include "Wire.h"
#include "WiFi.h"
#include "TCA9548A.h"

#include "freertos/task.h"

//Credientials for mobile hotspot
const char* ssid = "Rees";
const char* password = "password42";

//Constructor declarations
WiFiClient client;
TCA9548A I2CMux;

sysState RoverState;

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

void setupCommunication(){
  WiFi.begin(ssid, password);
  Serial.println("Connecting to network...");

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to network!:");
  Serial.println(WiFi.localIP());
  WiFi.setTxPower(WIFI_POWER_7dBm);

  while(!client.connect(IPAddress(192,168,75,151), 10000)){
    Serial.println("Connection to host failed");
    delay(1000);
  }
  Serial.println("Connected to server!");
}

void setupI2C(){
  Serial.println("Setting up I2C..");
  Wire.begin(13, 12);
  I2CMux.begin(Wire);
  I2CMux.closeAll();
}

void setup() {
  Serial.begin(115200);

  RoverState.mutex = xSemaphoreCreateMutex();

  if(RoverState.mutex == NULL){
    Serial.println("Failed to create mutex!");
    }
  setupI2C();
  setupCommunication();
  
  

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

  #if SEND_DATA

  xTaskCreate(
    sendData,
  "SendData",
  2500,
  NULL,
  DATA_PRIORITY,
  &sendDataTaskHandle
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
  // scanI2C(); //Scan I2c for debugging
  Serial.println("Starting Loop...");
  vTaskDelay(pdMS_TO_TICKS(1000));
}
