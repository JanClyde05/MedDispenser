/*
 * MedBox — Proximity Sensor
 * Generic digital-input driver with debounce.
 * Adapt for the specific sensor model when finalized.
 */

#include "proximity.h"
#include "config.h"

static bool _detected = false;
static bool _lastRaw = false;
static unsigned long _lastChangeMs = 0;

void proximityInit() {
  pinMode(PROXIMITY_PIN, INPUT_PULLUP);
  Serial.println(F("Proximity sensor initialized"));
}

void proximityUpdate() {
  bool raw = (digitalRead(PROXIMITY_PIN) == PROXIMITY_ACTIVE);

  if (raw != _lastRaw) {
    _lastChangeMs = millis();
    _lastRaw = raw;
  }

  // Debounce: only update state after stable for PROXIMITY_DEBOUNCE_MS
  if ((millis() - _lastChangeMs) >= PROXIMITY_DEBOUNCE_MS) {
    _detected = _lastRaw;
  }
}

bool proximityIsDetected() {
  return _detected;
}
