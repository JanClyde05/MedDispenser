/*
 * MedBox — Buzzer Control
 * Standard GPIO control compatible with Active and Passive buzzers.
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

static void _soundOn() {
  digitalWrite(BUZZER_PIN, HIGH);
}

static void _soundOff() {
  digitalWrite(BUZZER_PIN, LOW);
}

void buzzerInit() {
  pinMode(BUZZER_PIN, OUTPUT);
  _soundOff();
  Serial.println(F("Buzzer initialized"));
}

void buzzerUpdate() {
  if (!_active) return;

  unsigned long now = millis();

  // Single beep mode (patternTotal == 0)
  if (_patternTotal == 0) {
    if (now >= _actionMs) {
      _soundOff();
      _active = false;
    }
    return;
  }

  // Pattern mode
  unsigned long elapsed = now - _actionMs;

  if (_patternPhaseOn) {
    if (elapsed >= (unsigned long)_patternOnMs) {
      _soundOff();
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
      _soundOn();
      _patternPhaseOn = true;
      _actionMs = now;
    }
  }
}

void buzzerBeep(int durationMs) {
  _patternTotal = 0;
  _soundOn();
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
  _soundOn();
  _active = true;
}

void buzzerStop() {
  _soundOff();
  _active = false;
}

bool buzzerIsActive() {
  return _active;
}

