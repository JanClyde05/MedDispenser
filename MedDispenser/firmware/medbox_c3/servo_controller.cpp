/*
 * MedBox — Servo Controller
 * Dispenser: 1 revolution cycle (0° -> 180° -> 0°) per pill requested.
 * Hatch: 90° open, 0° closed.
 */

#include "servo_controller.h"
#include "config.h"
#include <ESP32Servo.h>

// Dedicated Servo objects per module
static Servo _m1Dispenser;  // GPIO 0
static Servo _m1Hatch;      // GPIO 1

static Servo _m2Dispenser;  // GPIO 3
static Servo _m2Hatch;      // GPIO 4

static Servo _m3Dispenser;  // GPIO 5
static Servo _m3Hatch;      // GPIO 6

// Helper to get dispenser servo by module ID
static Servo* _getDispenser(uint8_t moduleId) {
  if (moduleId == 1) return &_m1Dispenser;
  if (moduleId == 2) return &_m2Dispenser;
  if (moduleId == 3) return &_m3Dispenser;
  return nullptr;
}

// Helper to get hatch servo by module ID
static Servo* _getHatch(uint8_t moduleId) {
  if (moduleId == 1) return &_m1Hatch;
  if (moduleId == 2) return &_m2Hatch;
  if (moduleId == 3) return &_m3Hatch;
  return nullptr;
}

// Rotate DIY modified 360° dispenser servo 1 full revolution, then detach to hard-stop motor
static void _rotatePillRevolution(Servo& dispenser, int pin) {
  dispenser.attach(pin);
  dispenser.write(DISPENSER_SPEED_FORWARD); // 180° = Full speed spin
  delay(DISPENSER_REV_TIME_MS);             // 2400ms = 1 full 360° revolution
  dispenser.detach();                       // Cut signal ➔ Motor stops instantly!
  delay(300);
}

void servoControllerInit() {
  // Attach hatch servos to 0° home position
  _m1Hatch.attach(SERVO_M1_HATCH_PIN);
  _m2Hatch.attach(SERVO_M2_HATCH_PIN);
  _m3Hatch.attach(SERVO_M3_HATCH_PIN);

  _m1Hatch.write(HATCH_CLOSED_ANGLE); // 0°
  _m2Hatch.write(HATCH_CLOSED_ANGLE); // 0°
  _m3Hatch.write(HATCH_CLOSED_ANGLE); // 0°

  // Dispenser servos remain detached while idle to prevent continuous spin
  _m1Dispenser.detach();
  _m2Dispenser.detach();
  _m3Dispenser.detach();

  Serial.println(F("Servo controller initialized (DIY 360° auto-detach stop)"));
}

void servoControllerUpdate() {
  // Non-blocking update loop
}

void servoDispense(uint8_t moduleId, uint8_t count) {
  if (count == 0) count = 1;

  Serial.print(F("Dispensing M"));
  Serial.print(moduleId);
  Serial.print(F(" → "));
  Serial.print(count);
  Serial.println(F(" pill(s)"));

  Servo* d = _getDispenser(moduleId);
  if (!d) return;

  int pin = (moduleId == 1) ? SERVO_M1_DISPENSER_PIN :
            (moduleId == 2) ? SERVO_M2_DISPENSER_PIN : SERVO_M3_DISPENSER_PIN;

  // Perform 1 full 360° revolution (attach ➔ spin ➔ detach) for each pill
  for (uint8_t i = 0; i < count; i++) {
    Serial.print(F("Pill #"));
    Serial.println(i + 1);
    _rotatePillRevolution(*d, pin);
  }
}

void servoOpenHatch(uint8_t moduleId) {
  Serial.print(F("Opening hatch M"));
  Serial.println(moduleId);

  Servo* h = _getHatch(moduleId);
  if (h) {
    h->write(HATCH_OPEN_ANGLE);  // 90°
  }
}

void servoCloseHatch(uint8_t moduleId) {
  Serial.print(F("Closing hatch M"));
  Serial.println(moduleId);

  Servo* h = _getHatch(moduleId);
  if (h) {
    h->write(HATCH_CLOSED_ANGLE); // 0°
  }
}

void servoHome(uint8_t moduleId) {
  Serial.print(F("Homing M"));
  Serial.println(moduleId);

  Servo* d = _getDispenser(moduleId);
  Servo* h = _getHatch(moduleId);

  if (d) d->detach();                  // Hard stop continuous 360° dispenser motor
  if (h) h->write(HATCH_CLOSED_ANGLE);   // Close hatch (0°)
}



