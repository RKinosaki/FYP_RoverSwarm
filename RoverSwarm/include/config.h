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
#define SEND_DATA true
#define DATA_FREQ 4
#define DATA_PRIORITY 2

// DISTANCE task left
#define DISTANCE_ENABLE_L true
#define DISTANCE_FREQ_L 10
#define DISTANCE_PRIORITY_L 1

// PHOTON task
#define PHOTON_ENABLE false
#define PHOTON_FREQ 20
#define PHOTON_PRIORITY 3

// I2C Pins
#define SDA 21
#define SCL 47


#endif