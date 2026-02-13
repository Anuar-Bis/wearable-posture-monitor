#include <Wire.h>
#include "MPU6050.h"

#include "imu.h"
#include "posture.h"

// ================= CONFIG =================
static const uint8_t MPU1_ADDR = 0x68;   // lower sensor
static const uint8_t MPU2_ADDR = 0x69;   // upper sensor

static const int SDA_PIN = 21;
static const int SCL_PIN = 22;

// Change if your board LED is different
static const int LED_PIN = 2;

// Complementary filter
static const float ALPHA = 0.98f;
static const float DT = 0.01f;           // 10 ms -> 100 Hz loop

// LED behavior
static const unsigned long BLIP_MS = 60;
static const unsigned long BLIP_PERIOD_SLOUCH_MS = 450;
static const unsigned long BLIP_PERIOD_ALERT_MS  = 150;

// ================= OBJECTS =================
MPU6050 mpu1(MPU1_ADDR);
MPU6050 mpu2(MPU2_ADDR);

// ================= STATE =================
float gyroBiasY1 = 0.0f;
float gyroBiasY2 = 0.0f;

float angle1 = 0.0f;   // lower sensor angle
float angle2 = 0.0f;   // upper sensor angle

float baseline1 = 0.0f;
float baseline2 = 0.0f;

unsigned long lastBlip = 0;
bool ledOn = false;

// ================= HELPERS =================
void recalibrate() {
  Serial.println("\n[CAL] Recalibration requested.");

  gyroBiasY1 = calibrateGyroY(mpu1);
  gyroBiasY2 = calibrateGyroY(mpu2);

  baseline1 = calibrateBaseline(mpu1, angle1);
  baseline2 = calibrateBaseline(mpu2, angle2);

  posture = GOOD;
  slouchStartMs = 0;

  Serial.println("[CAL] Done. Sit-right should be near 0.\n");
}

void updateLED(unsigned long now) {
  if (posture == GOOD) {
    digitalWrite(LED_PIN, LOW);
    ledOn = false;
    return;
  }

  unsigned long period =
    (posture == ALERTED) ? BLIP_PERIOD_ALERT_MS : BLIP_PERIOD_SLOUCH_MS;

  if (!ledOn && (now - lastBlip >= period)) {
    ledOn = true;
    lastBlip = now;
    digitalWrite(LED_PIN, HIGH);
  }

  if (ledOn && (now - lastBlip >= BLIP_MS)) {
    ledOn = false;
    digitalWrite(LED_PIN, LOW);
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("\n=== Dual MPU6050 Spine Posture Monitor ===");
  Serial.println("Commands:");
  Serial.println("  c  -> recalibrate (sit straight)");
  Serial.println();

  mpu1.initialize();
  mpu2.initialize();

  Serial.println(mpu1.testConnection() ? "MPU1 OK" : "MPU1 FAIL");
  Serial.println(mpu2.testConnection() ? "MPU2 OK" : "MPU2 FAIL");

  // Initial calibration
  gyroBiasY1 = calibrateGyroY(mpu1);
  gyroBiasY2 = calibrateGyroY(mpu2);

  baseline1 = calibrateBaseline(mpu1, angle1);
  baseline2 = calibrateBaseline(mpu2, angle2);

  Serial.println("[CAL] Done. Sit-right should be near 0.");
}

// ================= LOOP =================
void loop() {
  // --- Serial command ---
  if (Serial.available()) {
    char ch = (char)Serial.read();
    if (ch == 'c' || ch == 'C') {
      recalibrate();
    }
  }

  // --- Maintain ~100 Hz loop ---
  static unsigned long last = millis();
  if (millis() - last < 10) return;
  last = millis();

  int16_t ax, ay, az, gx, gy, gz;

  // ----- Lower sensor -----
  mpu1.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float acc1 = accelPitchDeg(ax, az) - baseline1;
  float gyroRate1 = ((float)gy - gyroBiasY1) / 131.0f;  // deg/s
  angle1 = ALPHA * (angle1 + gyroRate1 * DT)
         + (1.0f - ALPHA) * acc1;

  // ----- Upper sensor -----
  mpu2.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float acc2 = accelPitchDeg(ax, az) - baseline2;
  float gyroRate2 = ((float)gy - gyroBiasY2) / 131.0f;  // deg/s
  angle2 = ALPHA * (angle2 + gyroRate2 * DT)
         + (1.0f - ALPHA) * acc2;

  // Spine bend angle
  float spine = angle2 - angle1;

  // Update posture state machine
  updatePostureWithTimer(spine);

  // LED feedback
  updateLED(millis());

  // --- Debug output ---
  Serial.print("Lower:");
  Serial.print(angle1, 2);
  Serial.print("  Upper:");
  Serial.print(angle2, 2);
  Serial.print("  Spine:");
  Serial.print(spine, 2);
  Serial.print("  |Spine|:");
  Serial.print(fabs(spine), 2);
  Serial.print("  State:");

  if (posture == GOOD) {
    Serial.println("GOOD");
  } else if (posture == SLOUCH) {
    Serial.print("SLOUCH ");
    Serial.print((millis() - slouchStartMs) / 1000.0f, 1);
    Serial.println("s");
  } else {
    Serial.println("ALERTED");
  }
}
