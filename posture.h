#pragma once

enum PostureState {
  GOOD,
  SLOUCH,
  ALERTED
};

extern PostureState posture;
extern unsigned long slouchStartMs;

// Update posture using spine angle
void updatePostureWithTimer(float spineDeg);
