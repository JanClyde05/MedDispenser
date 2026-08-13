/*
 * MedBox — Proximity Sensor
 * Detects whether the medicine taker is nearby.
 * NOT a pill-drop sensor.
 */

#ifndef MEDBOX_PROXIMITY_H
#define MEDBOX_PROXIMITY_H

#include <Arduino.h>

void proximityInit();
void proximityUpdate();
bool proximityIsDetected();   // Debounced presence result

#endif
