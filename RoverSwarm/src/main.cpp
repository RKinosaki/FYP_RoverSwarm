#include <Arduino.h>
#include "task.h"
#include "FreeRTOS.h"
#include "config.h"
#include "Wire.h"
#include "WiFi.h"
#include "TCA9548A.h"

#include "freertos/task.h"

#define ARDUINO_USB_CDC_ON_BOOT 1

//Credentials for mobile hotspot
const char* ssid = "bobcat-dev";
const char* password = "bobcat2025";

//Constructor declarations
WiFiClient client;
TCA9548A I2CMux;

sysState RoverState;

// Hardware timer

hw_timer_t *encoderTimer = NULL;

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

  while(!client.connect(IPAddress(192,168,136,232), 50)){
    Serial.println("Connection to host failed");
    delay(1000);
  }
  Serial.println("Connected to server!");
}

void setupI2C(){
  Serial.println("Setting up I2C..");
  Wire.begin(SDA, SCL);
  I2CMux.begin(Wire);
  I2CMux.closeAll();
}

void setupPWM(){
  ledcSetup(0, 1000, 8);
  ledcSetup(1, 1000, 8);
  ledcSetup(2, 1000, 8);
  ledcSetup(3, 1000, 8);

  ledcAttachPin(ML_P, 0);
  ledcAttachPin(ML_N, 1);
  ledcAttachPin(MR_P, 2);
  ledcAttachPin(MR_N, 3);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.print("Hello");


  // Reflectance Sensors
  pinMode(R0, INPUT);
  pinMode(R1, INPUT);
  pinMode(R2, INPUT);
  pinMode(R3, INPUT);

  //LED Pins;
  pinMode(LED_PIN, OUTPUT);

  // Motor Pins
  pinMode(ML_P, OUTPUT);
  pinMode(ML_N, OUTPUT);
  pinMode(ML_S, OUTPUT);
  pinMode(MR_P, OUTPUT);
  pinMode(MR_N, OUTPUT);
  pinMode(MR_S, OUTPUT);
  digitalWrite(ML_S, HIGH);
  digitalWrite(MR_S, HIGH);
  
  pinMode(EL_A, INPUT);
  pinMode(EL_B, INPUT);
  pinMode(ER_A, INPUT);
  pinMode(ER_B, INPUT);

  


  RoverState.mutex = xSemaphoreCreateMutex();

  if(RoverState.mutex == NULL){
    Serial.println("Failed to create mutex!");
    }

  setupCommunication();
  setupI2C();
  setupPWM();
  // setupCommunication();
  
  

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

  #if DRIVE_ENABLE
    xTaskCreate(
      driveRover,         //Function Name
      "Drive",              //Text Name
      8192,                       //Stack size (bytes)
      NULL,                       //Parameters
      DRIVE_PRIORITY,       // Priority
      &driveTaskHandle        // Pointer
    );


    attachInterrupt(digitalPinToInterrupt(ER_A), sampleEncoderISRR, RISING);
    attachInterrupt(digitalPinToInterrupt(EL_A), sampleEncoderISRL, RISING);
    // encoderTimer = timerBegin(0, HARDWARE_TIMER_PRESCALER, true);
    // timerAttachInterrupt(encoderTimer, sampleEncoderISR, true);
    // timerAlarmWrite(encoderTimer, (APB_CLK_FREQ/HARDWARE_TIMER_PRESCALER)/DRIVE_FREQ, true);
    // timerAlarmEnable(encoderTimer);
  #endif

  #if I2C_ENABLE

  xTaskCreate(
    measureSensor,              //Function Name
    "DistanceLeft",                   //Text Name
    5000,                       //Stack size (bytes)
    NULL,                         //Parameters
    I2C_PRIORITY,            // Priority
    &I2CTaskHandle             // Pointer
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
  // scanI2C(); //Scan I2c for debugging
  Serial.println("Starting Loop...");
  vTaskDelay(pdMS_TO_TICKS(1000));
}
