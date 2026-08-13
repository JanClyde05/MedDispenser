/*
 * MedBox — Buzzer Control
 * Uses LEDC for tone generation on ESP32.
 */

#include "buzzer.h"
#include "config.h"

static bool _active = false;
static unsigned long _actionMs = 0;

// Pattern state
static int  _patternTotal = 0;
static int  _patternIndex = 0;
static int  _patternOnMs  = 0;
static int  _patternOffMs = 0;
static bool _patternPhaseOn = false;

void buzzerInit() {
  ledcAttach(BUZZER_PIN, BUZZER_FREQ_HZ, 8);
  ledcWrite(BUZZER_PIN, 0);
  Serial.println(F("Buzzer initialized"));
}

void buzzerUpdate() {
  if (!_active) return;

  unsigned long now = millis();

  // Single beep mode (patternTotal == 0)
  if (_patternTotal == 0) {
    if (now >= _actionMs) {
      ledcWrite(BUZZER_PIN, 0);
      _active = false;
    }
    return;
  }

  // Pattern mode
  unsigned long elapsed = now - _actionMs;

  if (_patternPhaseOn) {
    if (elapsed >= (unsigned long)_patternOnMs) {
      ledcWrite(BUZZER_PIN, 0);
      _patternPhaseOn = false;
      _actionMs = now;
    }
  } else {
    if (elapsed >= (unsigned long)_patternOffMs) {
      _patternIndex++;
      if (_patternIndex >= _patternTotal) {
        _active = false;
        return;
      }
      ledcWrite(BUZZER_PIN, 128);
      _patternPhaseOn = true;
      _actionMs = now;
    }
  }
}

void buzzerBeep(int durationMs) {
  _patternTotal = 0;
  ledcWrite(BUZZER_PIN, 128);
  _actionMs = millis() + durationMs;
  _active = true;
}

void buzzerPatternStart(int beepCount, int onMs, int offMs) {
  _patternTotal = beepCount;
  _patternIndex = 0;
  _patternOnMs  = onMs;
  _patternOffMs = offMs;
  _patternPhaseOn = true;
  _actionMs = millis();
  ledcWrite(BUZZER_PIN, 128);
  _active = true;
}

void buzzerStop() {
  ledcWrite(BUZZER_PIN, 0);
  _active = false;
}

bool buzzerIsActive() {
  return _active;
}
