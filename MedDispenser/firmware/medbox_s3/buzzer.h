/*
 * MedBox — Buzzer Control
 * Non-blocking buzzer with pattern support.
 */

#ifndef MEDBOX_BUZZER_H
#define MEDBOX_BUZZER_H

#include <Arduino.h>

void buzzerInit();
void buzzerUpdate();           // Call every loop iteration
void buzzerBeep(int durationMs);
void buzzerPatternStart(int beepCount, int onMs, int offMs);
void buzzerStop();
bool buzzerIsActive();

#endif
