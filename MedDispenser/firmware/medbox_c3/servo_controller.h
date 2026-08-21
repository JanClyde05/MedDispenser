/*
 * MedBox — Servo Controller
 * Abstraction layer for dispenser and hatch servos across all modules.
 */

#ifndef MEDBOX_SERVO_CONTROLLER_H
#define MEDBOX_SERVO_CONTROLLER_H

#include <Arduino.h>

void servoControllerInit();
void servoControllerUpdate();

// High-level servo commands (module 1-based index)
void servoDispense(uint8_t moduleId, uint8_t count = 1);
void servoOpenHatch(uint8_t moduleId);
void servoCloseHatch(uint8_t moduleId);
void servoHome(uint8_t moduleId);

#endif
