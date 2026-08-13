/*
 * MedBox — Time Manager
 * NTP synchronization and local timekeeping.
 * RTC support can be added when hardware is finalized.
 */

#ifndef MEDBOX_TIME_MANAGER_H
#define MEDBOX_TIME_MANAGER_H

#include <Arduino.h>

void  timeManagerInit();
void  timeManagerUpdate();
bool  timeIsSynced();
String timeManagerGetTimeString();    // "HH:MM:SS"
String timeManagerGetDateString();    // "YYYY-MM-DD"
int   timeGetHour();
int   timeGetMinute();
int   timeGetSecond();
int   timeGetDayOfWeek();             // 0=Sunday, 6=Saturday
unsigned long timeGetEpoch();

#endif
