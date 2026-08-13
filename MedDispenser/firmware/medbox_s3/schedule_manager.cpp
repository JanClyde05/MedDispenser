/*
 * MedBox — Schedule Manager
 */

#include "schedule_manager.h"
#include "time_manager.h"
#include <Preferences.h>

static MedSchedule _schedules[MAX_SCHEDULES];
static int _count = 0;
static Preferences _prefs;

// Track which schedules have been handled this minute to avoid re-triggering
static bool _handledThisMinute[MAX_SCHEDULES];
static int  _lastCheckedMinute = -1;

void scheduleManagerInit() {
  memset(_schedules, 0, sizeof(_schedules));
  memset(_handledThisMinute, 0, sizeof(_handledThisMinute));
  scheduleLoadFromNVS();
  Serial.print(F("Schedules loaded: "));
  Serial.println(_count);
}

void scheduleManagerUpdate() {
  if (!timeIsSynced()) return;

  int currentMinute = timeGetMinute();
  int currentHour   = timeGetHour();

  // Reset handled flags when the minute changes
  if (currentMinute != _lastCheckedMinute) {
    _lastCheckedMinute = currentMinute;
    memset(_handledThisMinute, false, sizeof(_handledThisMinute));
  }
}

int scheduleManagerGetCount() {
  return _count;
}

bool scheduleHasPending() {
  if (!timeIsSynced()) return false;

  int h   = timeGetHour();
  int m   = timeGetMinute();
  int dow = timeGetDayOfWeek();  // 0=Sun

  for (int i = 0; i < _count; i++) {
    if (!_schedules[i].enabled) continue;
    if (_handledThisMinute[i]) continue;
    if (_schedules[i].hour != h) continue;
    if (_schedules[i].minute != m) continue;
    if (!((_schedules[i].daysOfWeek >> dow) & 1)) continue;

    // TODO: check start/end date range against current date
    return true;
  }
  return false;
}

MedSchedule scheduleGetPending() {
  int h   = timeGetHour();
  int m   = timeGetMinute();
  int dow = timeGetDayOfWeek();

  for (int i = 0; i < _count; i++) {
    if (!_schedules[i].enabled) continue;
    if (_handledThisMinute[i]) continue;
    if (_schedules[i].hour != h) continue;
    if (_schedules[i].minute != m) continue;
    if (!((_schedules[i].daysOfWeek >> dow) & 1)) continue;
    return _schedules[i];
  }

  // Should not reach here if scheduleHasPending() was checked first
  MedSchedule empty;
  memset(&empty, 0, sizeof(empty));
  return empty;
}

void scheduleMarkHandled(uint8_t moduleId, uint8_t hour, uint8_t minute) {
  for (int i = 0; i < _count; i++) {
    if (_schedules[i].moduleId == moduleId &&
        _schedules[i].hour == hour &&
        _schedules[i].minute == minute) {
      _handledThisMinute[i] = true;
    }
  }
}

bool scheduleAdd(const MedSchedule& sched) {
  if (_count >= MAX_SCHEDULES) {
    Serial.println(F("Schedule storage full"));
    return false;
  }
  _schedules[_count] = sched;
  _schedules[_count].active = true;
  _count++;
  return true;
}

bool scheduleClearAll() {
  memset(_schedules, 0, sizeof(_schedules));
  _count = 0;
  return true;
}

void scheduleSaveToNVS() {
  _prefs.begin(SCHEDULE_NVS_NAMESPACE, false);
  _prefs.putInt("count", _count);
  _prefs.putBytes("data", _schedules, sizeof(MedSchedule) * _count);
  _prefs.end();
  Serial.print(F("Schedules saved to NVS: "));
  Serial.println(_count);
}

void scheduleLoadFromNVS() {
  _prefs.begin(SCHEDULE_NVS_NAMESPACE, true);
  _count = _prefs.getInt("count", 0);
  if (_count > 0 && _count <= MAX_SCHEDULES) {
    _prefs.getBytes("data", _schedules, sizeof(MedSchedule) * _count);
  } else {
    _count = 0;
  }
  _prefs.end();
}
