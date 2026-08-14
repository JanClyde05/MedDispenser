/*
 * MedBox — Time Manager
 */

#include "time_manager.h"
#include "config.h"
#include "wifi_manager.h"
#include <time.h>

static bool _synced = false;
static unsigned long _lastSyncAttempt = 0;
static const unsigned long NTP_RETRY_INTERVAL = 60000; // 1 min

void timeManagerInit() {
  configTime(NTP_GMT_OFFSET_SEC, NTP_DAYLIGHT_OFFSET, NTP_SERVER);
  Serial.println(F("NTP time sync initiated"));
}

void timeManagerUpdate() {
  if (_synced) return;

  if (!wifiIsConnected()) return;

  if ((millis() - _lastSyncAttempt) < NTP_RETRY_INTERVAL && _lastSyncAttempt > 0) return;
  _lastSyncAttempt = millis();

  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 5000)) {
    _synced = true;
    Serial.print(F("NTP synced: "));
    Serial.println(timeManagerGetTimeString());
  } else {
    Serial.println(F("NTP sync pending..."));
  }
}

bool timeIsSynced() {
  return _synced;
}

static struct tm _getTime() {
  struct tm t;
  getLocalTime(&t);
  return t;
}

String timeManagerGetTimeString() {
  struct tm t = _getTime();
  char buf[12];
  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
  return String(buf);
}

String timeManagerGetDateString() {
  struct tm t = _getTime();
  char buf[12];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
  return String(buf);
}

int timeGetHour()      { return _getTime().tm_hour; }
int timeGetMinute()    { return _getTime().tm_min; }
int timeGetSecond()    { return _getTime().tm_sec; }
int timeGetDayOfWeek() { return _getTime().tm_wday; }

unsigned long timeGetEpoch() {
  time_t now;
  time(&now);
  return (unsigned long)now;
}

String timeGetISO8601() {
  struct tm t = _getTime();
  char buf[30];
  int offsetH = NTP_GMT_OFFSET_SEC / 3600;
  int offsetM = (NTP_GMT_OFFSET_SEC % 3600) / 60;
  snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d+%02d:%02d",
           t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
           t.tm_hour, t.tm_min, t.tm_sec,
           offsetH, offsetM);
  return String(buf);
}
