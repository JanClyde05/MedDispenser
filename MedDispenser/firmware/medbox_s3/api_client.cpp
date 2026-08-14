/*
 * MedBox — API Client
 * HTTPS communication with Netlify backend.
 * Uses HTTPClient + WiFiClientSecure for TLS.
 * JSON parsing via ArduinoJson.
 */

#include "api_client.h"
#include "config.h"
#include "wifi_manager.h"
#include "schedule_manager.h"
#include "time_manager.h"
#include "protocol.h"

#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

static unsigned long _lastSyncMs = 0;
static unsigned long _lastHeartbeatMs = 0;
static WiFiClientSecure _secureClient;

void apiClientInit() {
  _secureClient.setInsecure();  // Skip cert validation (Let's Encrypt rotates)
  Serial.println(F("API client initialized"));
}

void apiClientUpdate() {
  if (!wifiIsConnected()) return;

  unsigned long now = millis();

  // Periodic schedule sync
  if ((now - _lastSyncMs) >= API_SYNC_INTERVAL_MS) {
    _lastSyncMs = now;
    apiClientSyncSchedules();
  }

  // Periodic heartbeat (every 2 minutes)
  if ((now - _lastHeartbeatMs) >= 120000) {
    _lastHeartbeatMs = now;
    apiClientHeartbeat();
  }
}

void apiClientForceSync() {
  _lastSyncMs = 0;  // Force next update cycle to sync
  Serial.println(F("API sync forced"));
}

// ── Schedule Sync ──────────────────────────────────────────────────────
bool apiClientSyncSchedules() {
  if (!wifiIsConnected()) return false;

  HTTPClient http;
  String url = String(API_BASE_URL) + API_SYNC_ENDPOINT
             + "?deviceId=" + API_DEVICE_ID;

  Serial.print(F("API sync: GET "));
  Serial.println(url);

  http.begin(_secureClient, url);
  http.setTimeout(API_HTTP_TIMEOUT_MS);

  int httpCode = http.GET();
  if (httpCode != 200) {
    Serial.print(F("API sync failed, HTTP "));
    Serial.println(httpCode);
    http.end();
    return false;
  }

  String payload = http.getString();
  http.end();

  // Parse JSON
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    Serial.print(F("API sync JSON error: "));
    Serial.println(err.c_str());
    return false;
  }

  // Optional: sync server time
  if (doc["serverTime"].is<unsigned long>()) {
    // Could update RTC here if needed
  }

  JsonArray schedules = doc["schedules"].as<JsonArray>();
  if (schedules.isNull()) {
    Serial.println(F("API sync: no schedules array"));
    return false;
  }

  // Replace all local schedules
  scheduleClearAll();

  int count = 0;
  for (JsonObject s : schedules) {
    MedSchedule sched;
    memset(&sched, 0, sizeof(sched));

    sched.active     = true;
    sched.moduleId   = s["moduleId"] | 1;
    sched.pillsPerDose = s["pillsPerDose"] | 1;
    sched.hour       = s["hour"] | 0;
    sched.minute     = s["minute"] | 0;
    sched.daysOfWeek = s["daysOfWeek"] | 0;
    sched.startDate  = s["startDate"] | 0;
    sched.endDate    = s["endDate"] | 0;
    sched.enabled    = s["enabled"] | true;

    const char* name = s["medicineName"] | "Unknown";
    strncpy(sched.medicineName, name, sizeof(sched.medicineName) - 1);
    sched.medicineName[sizeof(sched.medicineName) - 1] = '\0';

    if (scheduleAdd(sched)) {
      count++;
    }
  }

  scheduleSaveToNVS();
  Serial.print(F("API sync: fetched "));
  Serial.print(count);
  Serial.println(F(" schedules"));

  return true;
}

// ── Dispense Logging ───────────────────────────────────────────────────
void apiClientLogDispense(uint8_t moduleId, const char* medicineName,
                          uint8_t qty, const char* status) {
  if (!wifiIsConnected()) {
    Serial.println(F("API log: no WiFi, skipping"));
    return;
  }

  HTTPClient http;
  String url = String(API_BASE_URL) + API_LOG_ENDPOINT;

  JsonDocument doc;
  doc["moduleId"]          = moduleId;
  doc["medicineName"]      = medicineName;
  doc["requestedQuantity"] = qty;
  doc["status"]            = status;
  doc["actualTime"]        = timeGetISO8601();
  doc["deviceId"]          = API_DEVICE_ID;

  String body;
  serializeJson(doc, body);

  http.begin(_secureClient, url);
  http.setTimeout(API_HTTP_TIMEOUT_MS);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(body);
  Serial.print(F("Dispense logged: HTTP "));
  Serial.println(httpCode);
  http.end();
}

// ── Notifications ──────────────────────────────────────────────────────
void apiClientNotify(uint8_t moduleId, const char* medicineName,
                     uint8_t dose, const char* time, const char* type) {
  if (!wifiIsConnected()) {
    Serial.println(F("API notify: no WiFi, skipping"));
    return;
  }

  HTTPClient http;
  String url = String(API_BASE_URL) + API_NOTIFY_ENDPOINT;

  JsonDocument doc;
  doc["moduleId"]     = moduleId;
  doc["medicineName"] = medicineName;
  doc["dose"]         = dose;
  doc["time"]         = time;
  doc["type"]         = type;

  String body;
  serializeJson(doc, body);

  http.begin(_secureClient, url);
  http.setTimeout(API_HTTP_TIMEOUT_MS);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(body);
  Serial.print(F("Notify sent: HTTP "));
  Serial.println(httpCode);
  http.end();
}

// ── Device Heartbeat ───────────────────────────────────────────────────
void apiClientHeartbeat() {
  if (!wifiIsConnected()) return;

  HTTPClient http;
  String url = String(API_BASE_URL) + API_DEVICE_ENDPOINT;

  JsonDocument doc;
  doc["deviceId"]    = API_DEVICE_ID;
  doc["name"]        = "MedBox Main";
  doc["isMain"]      = true;
  doc["moduleCount"] = MODULE_MAX_ID;
  doc["ip"]          = wifiGetIP();
  doc["firmware"]    = "1.0.0";

  JsonArray modules = doc["activeModules"].to<JsonArray>();
  for (int i = MODULE_MIN_ID; i <= MODULE_MAX_ID; i++) {
    modules.add(i);
  }

  String body;
  serializeJson(doc, body);

  http.begin(_secureClient, url);
  http.setTimeout(API_HTTP_TIMEOUT_MS);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(body);
  if (httpCode == 200) {
    Serial.println(F("Heartbeat sent"));
  } else {
    Serial.print(F("Heartbeat failed: HTTP "));
    Serial.println(httpCode);
  }
  http.end();
}
