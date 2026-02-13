#include "posture.h"
#include <Arduino.h>

static const float GOOD_LIMIT   = 5.0f;
static const float SLOUCH_LIMIT = 8.0f;
static const unsigned long SLOUCH_TIME_MS = 3000;

PostureState posture = GOOD;
unsigned long slouchStartMs = 0;

void updatePostureWithTimer(float spineDeg) {
  float mag = fabs(spineDeg);
  unsigned long now = millis();

  if (posture == GOOD && mag > SLOUCH_LIMIT) {
    posture = SLOUCH;
    slouchStartMs = now;
  }
  else if ((posture == SLOUCH || posture == ALERTED) && mag < GOOD_LIMIT) {
    posture = GOOD;
    slouchStartMs = 0;
  }
  else if (posture == SLOUCH && now - slouchStartMs >= SLOUCH_TIME_MS) {
    posture = ALERTED;
  }
}
