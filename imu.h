#pragma once
#include "MPU6050.h"

// Convert accelerometer data to pitch angle (degrees)
float accelPitchDeg(int16_t ax, int16_t az);

// Gyroscope Y-axis calibration
float calibrateGyroY(MPU6050 &mpu);

// Baseline calibration (sit straight)
float calibrateBaseline(MPU6050 &mpu, float &angleState);
