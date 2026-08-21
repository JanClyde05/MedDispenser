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
#include "state_machine.h"
#include "time_manager.h"
#include "proximity.h"
#include "buzzer.h"
#include "uart_command.h"
#include "protocol.h"

#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

static unsigned long _lastSyncMs = 0;
static unsigned long _lastHeartbeatMs = 0;
static unsigned long _lastCmdPollMs = 0;
static WiFiClientSecure _secureClient;

// Helper to support both HTTP (localhost testing) and HTTPS (Netlify cloud)
static void _beginHttp(HTTPClient& http, const String& url) {
  if (url.startsWith("https://")) {
    http.begin(_secureClient, url);
  } else {
    http.begin(url);  // Uses standard unencrypted WiFiClient for http://
  }
}

// ── IR Test Mode State ───────────────────────────────────────────────
static bool    _irTestActive  = false;
static uint8_t _irTestModule  = 1;
static char    _irTestAction[8] = "";  // "buzz", "servo", "both"

// Forward declaration
static void _processTestCommand(JsonObject& cmd);
static void _apiClientPollCommands();

void apiClientInit() {
  _secureClient.setInsecure();  // Skip cert validation (Let's Encrypt rotates)
  Serial.println(F("API client initialized"));
}

void apiClientUpdate() {
  if (!wifiIsConnected()) return;

  unsigned long now = millis();

  // Periodic schedule sync (every 5 min)
  if ((now - _lastSyncMs) >= API_SYNC_INTERVAL_MS) {
    _lastSyncMs = now;
    apiClientSyncSchedules();
  }

  // Fast command poll for hardware test commands (every 10 sec)
  if ((now - _lastCmdPollMs) >= API_COMMAND_POLL_MS) {
    _lastCmdPollMs = now;
    _apiClientPollCommands();
  }

  // Periodic heartbeat (every 2 minutes)
  if ((now - _lastHeartbeatMs) >= 120000) {
    _lastHeartbeatMs = now;
    apiClientHeartbeat();
  }

  // ── IR Test Mode — check proximity sensor ──────────────────────────
  if (_irTestActive && proximityIsDetected()) {
    Serial.print(F("[IR Test] Proximity detected! Action: "));
    Serial.println(_irTestAction);

    if (strcmp(_irTestAction, "buzz") == 0) {
      buzzerPatternStart(3, 300, 200);
    } else if (strcmp(_irTestAction, "servo") == 0) {
      uartSendCommand(CMD_OPEN, _irTestModule);
    } else if (strcmp(_irTestAction, "both") == 0) {
      buzzerPatternStart(3, 300, 200);
      uartSendCommand(CMD_OPEN, _irTestModule);
    }

    _irTestActive = false;  // One-shot: disarm after first detection
    Serial.println(F("[IR Test] Disarmed"));
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

  _beginHttp(http, url);
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

  // Sync server time to ESP32 system clock
  if (doc["serverTime"].is<unsigned long>()) {
    unsigned long serverEpoch = doc["serverTime"].as<unsigned long>();
    timeSetEpoch(serverEpoch);
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

  // ── Process pending dispense commands from web frontend ────────────
  JsonArray pendingCmds = doc["pendingCommands"].as<JsonArray>();
  if (!pendingCmds.isNull() && pendingCmds.size() > 0) {
    Serial.print(F("API sync: "));
    Serial.print(pendingCmds.size());
    Serial.println(F(" pending dispense command(s)"));

    for (JsonObject cmd : pendingCmds) {
      MedSchedule triggerSched;
      memset(&triggerSched, 0, sizeof(triggerSched));

      triggerSched.active      = true;
      triggerSched.moduleId    = cmd["moduleId"] | 1;
      triggerSched.pillsPerDose = cmd["dose"] | 1;
      triggerSched.enabled     = true;

      const char* medName = cmd["medicineName"] | "Test";
      strncpy(triggerSched.medicineName, medName,
              sizeof(triggerSched.medicineName) - 1);
      triggerSched.medicineName[sizeof(triggerSched.medicineName) - 1] = '\0';

      // Parse time if provided ("HH:MM" format)
      const char* timeStr = cmd["time"] | "00:00";
      if (strlen(timeStr) >= 5 && timeStr[2] == ':') {
        triggerSched.hour   = (timeStr[0] - '0') * 10 + (timeStr[1] - '0');
        triggerSched.minute = (timeStr[3] - '0') * 10 + (timeStr[4] - '0');
      }

      // Trigger the state machine — this starts buzzer → IR → servo flow
      if (stateMachineTriggerDispense(triggerSched)) {
        Serial.print(F("Web command accepted: "));
        Serial.println(triggerSched.medicineName);
        break;  // Only process one command at a time (state machine is single-threaded)
      } else {
        Serial.println(F("Web command rejected — state machine busy"));
      }
    }
  }

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

  _beginHttp(http, url);
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

  _beginHttp(http, url);
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

  _beginHttp(http, url);
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

// ══════════════════════════════════════════════════════════════════════
// Hardware Test Command Poll — 10-second fast poll
// ══════════════════════════════════════════════════════════════════════

static void _apiClientPollCommands() {
  if (!wifiIsConnected()) return;

  HTTPClient http;
  String url = String(API_BASE_URL) + API_COMMAND_ENDPOINT;

  _beginHttp(http, url);
  http.setTimeout(API_HTTP_TIMEOUT_MS);

  int httpCode = http.GET();
  if (httpCode != 200) {
    http.end();
    return;  // Silent fail — this runs every 10s, no need to spam
  }

  String payload = http.getString();
  http.end();

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err) return;

  JsonArray cmds = doc.as<JsonArray>();
  if (cmds.isNull() || cmds.size() == 0) return;

  Serial.print(F("[CmdPoll] "));
  Serial.print(cmds.size());
  Serial.println(F(" command(s) found"));

  // Process each command
  bool hasTestCommands = false;
  for (JsonObject cmd : cmds) {
    const char* type = cmd["type"] | "";
    if (strcmp(type, "test_hardware") == 0) {
      _processTestCommand(cmd);
      hasTestCommands = true;
    } else if (strcmp(type, "test") == 0 || strcmp(type, "scheduled") == 0 || strcmp(type, "reminder") == 0) {
      MedSchedule triggerSched;
      memset(&triggerSched, 0, sizeof(triggerSched));
      triggerSched.active       = true;
      triggerSched.moduleId     = cmd["moduleId"] | 1;
      triggerSched.pillsPerDose = cmd["dose"] | 1;
      triggerSched.enabled      = true;

      const char* medName = cmd["medicineName"] | "Test";
      strncpy(triggerSched.medicineName, medName, sizeof(triggerSched.medicineName) - 1);
      triggerSched.medicineName[sizeof(triggerSched.medicineName) - 1] = '\0';

      if (stateMachineTriggerDispense(triggerSched)) {
        Serial.print(F("[CmdPoll] Triggered dispense & buzzer for: "));
        Serial.println(triggerSched.medicineName);
      }
      hasTestCommands = true;
    }
  }

  // Clear the command queue after processing
  if (hasTestCommands) {
    HTTPClient httpDel;
    _beginHttp(httpDel, url);
    httpDel.setTimeout(API_HTTP_TIMEOUT_MS);
    httpDel.addHeader("Content-Type", "application/json");
    httpDel.sendRequest("DELETE", "{\"clearAll\":true}");
    httpDel.end();
    Serial.println(F("[CmdPoll] Queue cleared"));
  }
}

static void _processTestCommand(JsonObject& cmd) {
  const char* command  = cmd["command"] | "";
  int         moduleId = cmd["moduleId"] | 1;

  Serial.print(F("[HW Test] Command: "));
  Serial.print(command);
  Serial.print(F(" Module: "));
  Serial.println(moduleId);

  // ── Buzzer ─────────────────────────────────────────────────────────
  if (strcmp(command, "BUZZ") == 0) {
    Serial.println(F("[HW Test] Buzzer ON"));
    buzzerPatternStart(3, 500, 300);  // 3 beeps, 500ms on, 300ms off

  // ── UART Ping ──────────────────────────────────────────────────────
  } else if (strcmp(command, "PING") == 0) {
    Serial.println(F("[HW Test] Pinging C3..."));
    uartSendCommand(CMD_PING, 0);

  // ── Servo: Open Hatch ──────────────────────────────────────────────
  } else if (strcmp(command, "OPEN") == 0) {
    Serial.print(F("[HW Test] Opening hatch M"));
    Serial.println(moduleId);
    uartSendCommand(CMD_OPEN, moduleId);

  // ── Servo: Close Hatch ─────────────────────────────────────────────
  } else if (strcmp(command, "CLOSE") == 0) {
    Serial.print(F("[HW Test] Closing hatch M"));
    Serial.println(moduleId);
    uartSendCommand(CMD_CLOSE, moduleId);

  // ── Servo: Dispense ────────────────────────────────────────────────
  } else if (strcmp(command, "DISPENSE") == 0) {
    Serial.print(F("[HW Test] Dispensing M"));
    Serial.println(moduleId);
    uartSendCommand(CMD_DISPENSE, moduleId);

  // ── Servo: Home ────────────────────────────────────────────────────
  } else if (strcmp(command, "HOME") == 0) {
    Serial.print(F("[HW Test] Homing M"));
    Serial.println(moduleId);
    uartSendCommand(CMD_HOME, moduleId);

  // ── IR Sensor Test ─────────────────────────────────────────────────
  } else if (strcmp(command, "IR_TEST") == 0) {
    const char* irAction = cmd["irAction"] | "buzz";
    _irTestModule = moduleId;
    strncpy(_irTestAction, irAction, sizeof(_irTestAction) - 1);
    _irTestAction[sizeof(_irTestAction) - 1] = '\0';
    _irTestActive = true;

    Serial.print(F("[HW Test] IR armed → "));
    Serial.print(_irTestAction);
    Serial.print(F(" M"));
    Serial.println(_irTestModule);

  } else {
    Serial.print(F("[HW Test] Unknown command: "));
    Serial.println(command);
  }
}
