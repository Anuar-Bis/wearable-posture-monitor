# Wearable Posture Monitor – Firmware
Firmware for a real-time posture monitoring device based on MPU6050 IMU data.

## Description:

### This project implements:
* IMU initialization and calibration
* Orientation (pitch) calculation
* Posture detection using angle thresholds
* State machine for posture classification

#### The system evaluates spine deviation from a calibrated neutral position and determines posture state in real time.

### Project Structure:

- imu.h / imu.cpp:
   * IMU setup, calibration, angle calculation
- posture.h / posture.cpp
   * Posture evaluation logic
   * State machine implementation
- wearable-posture-monitor.ino
   * Main loop and device control

### Posture Logic:
1. Read IMU data
2. Compute pitch angle
3. Compare with baseline
4. Classify state:
   * GOOD
   * WARNING
   * BAD
6. Trigger feedback if BAD posture persists beyond delay threshold

### Build:
  - Compatible with Arduino IDE or PlatformIO.
  - Requires an MPU6050 library.
