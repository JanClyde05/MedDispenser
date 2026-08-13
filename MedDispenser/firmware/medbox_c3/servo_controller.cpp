/*
 * MedBox — Servo Controller
 *
 * Uses ESP32Servo library (Arduino IDE: install "ESP32Servo" by Kevin Harrington).
 * Maps module IDs to GPIO pins defined in config.h.
 *
 * IMPORTANT: Dispenser step angle is NOT FINALIZED.
 * The DISPENSER_STEP_ANGLE in config.h is a placeholder.
 * Calibrate against the actual spiral/rotor geometry.
 */

#include "servo_controller.h"
#include "config.h"
#include <ESP32Servo.h>

// ── Servo objects ─────────────────────────────────────────────────────────
// Index 0 = Module 1 dispenser, Index 1 = Module 1 hatch, etc.
static Servo _dispenserServo[NUM_MODULES];
static Servo _hatchServo[NUM_MODULES];

// Current dispenser positions (accumulated angle for indexing)
static int _dispenserAngle[NUM_MODULES];

// GPIO pin lookup tables
static const int _dispenserPins[NUM_MODULES] = {
  SERVO_M1_DISPENSER_PIN,
  SERVO_M2_DISPENSER_PIN,
  SERVO_M3_DISPENSER_PIN
};

static const int _hatchPins[NUM_MODULES] = {
  SERVO_M1_HATCH_PIN,
  SERVO_M2_HATCH_PIN,
  SERVO_M3_HATCH_PIN
};

void servoControllerInit() {
  // Allow allocation of all timers for servo library
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  for (int i = 0; i < NUM_MODULES; i++) {
    _dispenserServo[i].setPeriodHertz(50);
    _hatchServo[i].setPeriodHertz(50);
    _dispenserAngle[i] = DISPENSER_HOME_ANGLE;
  }

  Serial.print(F("Servo controller initialized: "));
  Serial.print(NUM_MODULES);
  Serial.println(F(" modules"));
}

void servoControllerUpdate() {
  // Currently no background servo tasks.
  // Future: smooth movement, position feedback, detach idle servos.
}

// ── Helper: attach, move, delay, detach ─────────────────────────────────
static void _moveServo(Servo& servo, int pin, int angle) {
  servo.attach(pin, SERVO_MIN_PULSE_US, SERVO_MAX_PULSE_US);
  servo.write(angle);
  delay(SERVO_MOVE_DELAY_MS);  // Blocking — acceptable for C3 single-task role
  // Detach after movement to reduce power/jitter
  delay(SERVO_DETACH_DELAY_MS);
  servo.detach();
}

static int _moduleIndex(uint8_t moduleId) {
  return moduleId - 1;  // moduleId is 1-based, array is 0-based
}

// ── Public API ──────────────────────────────────────────────────────────

void servoDispense(uint8_t moduleId) {
  int idx = _moduleIndex(moduleId);
  if (idx < 0 || idx >= NUM_MODULES) return;

  // Advance dispenser by one step
  _dispenserAngle[idx] += DISPENSER_STEP_ANGLE;
  if (_dispenserAngle[idx] >= 360) {
    _dispenserAngle[idx] -= 360;
  }

  Serial.print(F("Dispense M"));
  Serial.print(moduleId);
  Serial.print(F(" → angle "));
  Serial.println(_dispenserAngle[idx]);

  _moveServo(_dispenserServo[idx], _dispenserPins[idx], _dispenserAngle[idx]);
}

void servoOpenHatch(uint8_t moduleId) {
  int idx = _moduleIndex(moduleId);
  if (idx < 0 || idx >= NUM_MODULES) return;

  Serial.print(F("Open hatch M"));
  Serial.println(moduleId);

  _moveServo(_hatchServo[idx], _hatchPins[idx], HATCH_OPEN_ANGLE);
}

void servoCloseHatch(uint8_t moduleId) {
  int idx = _moduleIndex(moduleId);
  if (idx < 0 || idx >= NUM_MODULES) return;

  Serial.print(F("Close hatch M"));
  Serial.println(moduleId);

  _moveServo(_hatchServo[idx], _hatchPins[idx], HATCH_CLOSED_ANGLE);
}

void servoHome(uint8_t moduleId) {
  int idx = _moduleIndex(moduleId);
  if (idx < 0 || idx >= NUM_MODULES) return;

  _dispenserAngle[idx] = DISPENSER_HOME_ANGLE;

  Serial.print(F("Home M"));
  Serial.println(moduleId);

  _moveServo(_dispenserServo[idx], _dispenserPins[idx], DISPENSER_HOME_ANGLE);
  _moveServo(_hatchServo[idx], _hatchPins[idx], HATCH_CLOSED_ANGLE);
}
