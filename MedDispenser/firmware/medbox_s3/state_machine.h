/*
 * MedBox — Medication Event State Machine
 * Implements the dispensing sequence from IDLE through COMPLETED.
 */

#ifndef MEDBOX_STATE_MACHINE_H
#define MEDBOX_STATE_MACHINE_H

#include <Arduino.h>
#include "schedule_manager.h"

// Medication event states — matches spec section 37
enum SystemState {
  STATE_IDLE,
  STATE_REMINDER,
  STATE_WAITING_FOR_USER,
  STATE_HATCH_OPENING,
  STATE_DISPENSING,
  STATE_USER_TAKING_MEDICINE,
  STATE_HATCH_CLOSING,
  STATE_COMPLETED,
  STATE_ERROR
};

void         stateMachineInit();
void         stateMachineUpdate();
SystemState  stateMachineGetState();
const char*  stateMachineGetStateName();

// Get the schedule currently being processed (valid only when not IDLE)
MedSchedule  stateMachineGetActiveSchedule();

// Externally trigger a dispense cycle (e.g. from web command or test)
// Only works when the state machine is IDLE. Returns true if accepted.
bool         stateMachineTriggerDispense(const MedSchedule& schedule);

#endif
