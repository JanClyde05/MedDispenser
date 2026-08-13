/*
 * MedBox — Schedule Manager
 * Stores and evaluates medication schedules locally using NVS/Preferences.
 */

#ifndef MEDBOX_SCHEDULE_MANAGER_H
#define MEDBOX_SCHEDULE_MANAGER_H

#include <Arduino.h>
#include "config.h"

// Schedule entry stored locally on device
struct MedSchedule {
  bool     active;
  uint8_t  moduleId;          // 1-3
  char     medicineName[32];
  uint8_t  pillsPerDose;
  uint8_t  hour;              // 0-23
  uint8_t  minute;            // 0-59
  uint8_t  daysOfWeek;        // Bitmask: bit0=Sun, bit1=Mon, ..., bit6=Sat
  uint32_t startDate;         // Epoch (date only, midnight)
  uint32_t endDate;           // Epoch (date only, midnight), 0 = no end
  bool     enabled;
};

void  scheduleManagerInit();
void  scheduleManagerUpdate();
int   scheduleManagerGetCount();

// Returns true if any schedule is due RIGHT NOW and not yet handled
bool  scheduleHasPending();

// Get the next pending schedule (call only if scheduleHasPending() == true)
MedSchedule scheduleGetPending();

// Mark a schedule event as handled for this cycle
void  scheduleMarkHandled(uint8_t moduleId, uint8_t hour, uint8_t minute);

// Schedule CRUD (called from API sync)
bool  scheduleAdd(const MedSchedule& sched);
bool  scheduleClearAll();
void  scheduleSaveToNVS();
void  scheduleLoadFromNVS();

#endif
