#include "imu.h"
#include <math.h>

static inline float rad2deg(float r) {
  return r * 180.0f / M_PI;
}

float accelPitchDeg(int16_t ax, int16_t az) {
  return rad2deg(atan2f((float)ax, fabs((float)az)));
}

float calibrateGyroY(MPU6050 &mpu) {
  long sum = 0;
  int16_t ax, ay, az, gx, gy, gz;

  for (int i = 0; i < 800; i++) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    sum += gy;
    delay(2);
  }
  return (float)sum / 800.0f;
}

float calibrateBaseline(MPU6050 &mpu, float &angleState) {
  float sum = 0;
  int16_t ax, ay, az, gx, gy, gz;

  for (int i = 0; i < 200; i++) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    sum += accelPitchDeg(ax, az);
    delay(5);
  }

  angleState = 0.0f;
  return sum / 200.0f;
}
