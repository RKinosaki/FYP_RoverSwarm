#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// === TASKS === //
#define HARDWARE_TIMER_PRESCALER 80

// LED Task
#define LED_ENABLE false
#define LED_FREQ 1
#define LED_PRIORITY 5

// ORIENTATION task
#define ORIENTATION_ENABLE false
#define ORIENTATION_FREQ 1
#define ORIENTATION_PRIORITY 4

#define ORIENTATION_ACCEL_RANGE MPU6050_RANGE_4_G
#define ORIENTATION_GYRO_RANGE MPU6050_RANGE_250_DEG
#define ORIENTATION_BANDWIDTH MPU6050_BAND_21_HZ

//SendData Task
#define SEND_DATA false
#define DATA_FREQ 4
#define DATA_PRIORITY 2

// DISTANCE task left
#define DISTANCE_ENABLE_L false
#define DISTANCE_FREQ_L 10
#define DISTANCE_PRIORITY_L 1

// PHOTON task
#define PHOTON_ENABLE false
#define PHOTON_FREQ 20
#define PHOTON_PRIORITY 3

//DRIVE task
#define DRIVE_ENABLE true
#define DRIVE_FREQ 20
#define DRIVE_PRIORITY 5

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


#endif