#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// === TASKS === //
#define HARDWARE_TIMER_PRESCALER 80

// LED Task
#define LED_ENABLE true
#define LED_FREQ 1
#define LED_PRIORITY 5

//IMU settings
#define ORIENTATION_ACCEL_RANGE MPU6050_RANGE_4_G
#define ORIENTATION_GYRO_RANGE MPU6050_RANGE_250_DEG
#define ORIENTATION_BANDWIDTH MPU6050_BAND_21_HZ

//SendData Task
#define SEND_DATA true
#define DATA_FREQ 4
#define DATA_PRIORITY 3

// I2C task
#define I2C_ENABLE true
#define I2C_FREQ 400
#define I2C_PRIORITY 1
#define EN_TOF_R true
#define EN_TOF_L true
#define EN_IMU true

// PHOTON task
#define PHOTON_ENABLE false
#define PHOTON_FREQ 20
#define PHOTON_PRIORITY 3

//DRIVE task
#define DRIVE_ENABLE true
#define DRIVE_FREQ 400
#define DRIVE_PRIORITY 2

// I2C Pins
#define SDA 21
#define SCL 47

// Photon Pins
#define R0 11
#define R1 13
#define R2 12
#define R3 14

// Drive Pins
#define ML_P 6
#define ML_N 7
#define ML_S 5

#define MR_P 43
#define MR_N 2
#define MR_S 44

// Encoder Pins
#define EL_A 16
#define EL_B 17
#define ER_A 38
#define ER_B 39

// Channel Pins
#define Channel_ToF_L 7
#define Channel_ToF_R 4
#define Channel_IMU 3

//LED Pin

#define LED_PIN 8


//Tyre Parameters
#define TYRE_DIAMETER 34
#define ENCODER_RES 700
#define PI 3.1415

//WIFI creds
#define PORT 50
#define IP1 192
#define IP2 168
#define IP3 136
#define IP4 232

#endif