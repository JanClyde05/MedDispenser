/*
 * MedBox ESP32-S3 — Main Firmware
 * =================================
 * System brain: Wi-Fi, schedules, time, proximity, buzzer, UART to C3.
 *
 * Architecture:
 *   - Non-blocking state machine in loop()
 *   - Modular subsystems (each .h/.cpp pair)
 *   - UART command interface to ESP32-C3 servo controller
 *
 * Arduino IDE Board: ESP32-S3 Dev Module (or SuperMini variant)
 * Partition Scheme: Default 4MB with spiffs (NVS + LittleFS for portal assets)
 */

#include "config.h"
#include "nvs_store.h"
#include "wifi_manager.h"
#include "time_manager.h"
#include "schedule_manager.h"
#include "state_machine.h"
#include "proximity.h"
#include "buzzer.h"
#include "uart_command.h"
#include "api_client.h"

// Copy from firmware/shared/ — keep in sync with canonical version
#include "protocol.h"

// ── Forward declarations ───────────────────────────────────────────────────
void handleSerialDebug();

void setup() {
  // Debug serial
  Serial.begin(SERIAL_BAUD);
  delay(1000);  // Allow USB serial to initialize
  Serial.println(F("=== MedBox S3 Starting ==="));

  // Initialize subsystems
  buzzerInit();
  proximityInit();
  uartCommandInit();
  nvsStoreInit();
  wifiManagerInit();
  timeManagerInit();
  scheduleManagerInit();
  stateMachineInit();
  apiClientInit();

  Serial.println(F("=== MedBox S3 Ready ==="));

  // Short startup beep
  buzzerBeep(200);
}

void loop() {
  // ── Non-blocking subsystem updates ─────────────────────────────────────
  wifiManagerUpdate();        // WiFi provisioning + connection management
  timeManagerUpdate();        // NTP sync checks
  scheduleManagerUpdate();    // Evaluate active schedules
  stateMachineUpdate();       // Run medication event state machine
  proximityUpdate();          // Read proximity sensor
  buzzerUpdate();             // Handle non-blocking buzzer patterns
  uartCommandUpdate();        // Process any pending UART responses
  apiClientUpdate();          // Periodic backend sync

  // ── Debug interface ────────────────────────────────────────────────────
  handleSerialDebug();
}

// ── Serial debug commands ────────────────────────────────────────────────
// Type commands into the Arduino Serial Monitor for testing.
void handleSerialDebug() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();

  if (cmd == "status") {
    Serial.println(F("--- MedBox Status ---"));
    Serial.print(F("Wi-Fi: "));
    Serial.print(wifiIsConnected() ? "Connected" : "Disconnected");
    Serial.print(F(" (")); Serial.print(wifiGetIP()); Serial.println(F(")"));
    Serial.print(F("Time: "));
    Serial.println(timeManagerGetTimeString());
    Serial.print(F("State: "));
    Serial.println(stateMachineGetStateName());
    Serial.print(F("Proximity: "));
    Serial.println(proximityIsDetected() ? "DETECTED" : "clear");
    Serial.print(F("Schedules: "));
    Serial.println(scheduleManagerGetCount());

  } else if (cmd == "dispense1") {
    Serial.println(F("DEBUG: Sending DISPENSE,1"));
    uartSendCommand(CMD_DISPENSE, 1);

  } else if (cmd == "open1") {
    Serial.println(F("DEBUG: Sending OPEN,1"));
    uartSendCommand(CMD_OPEN, 1);

  } else if (cmd == "close1") {
    Serial.println(F("DEBUG: Sending CLOSE,1"));
    uartSendCommand(CMD_CLOSE, 1);

  } else if (cmd == "buzz") {
    Serial.println(F("DEBUG: Buzzer test"));
    buzzerBeep(1000);

  } else if (cmd == "ping") {
    Serial.println(F("DEBUG: Pinging C3"));
    uartSendCommand(CMD_PING, 0);

  } else if (cmd == "sync") {
    Serial.println(F("DEBUG: Force backend sync"));
    apiClientForceSync();

  } else if (cmd == "wifi") {
    Serial.print(F("WiFi State: "));
    switch (wifiGetState()) {
      case WIFI_DISCONNECTED: Serial.println(F("DISCONNECTED")); break;
      case WIFI_CONNECTING:   Serial.println(F("CONNECTING")); break;
      case WIFI_CONNECTED:    Serial.print(F("CONNECTED — ")); Serial.println(wifiGetIP()); break;
      case WIFI_AP_MODE:      Serial.print(F("AP MODE — ")); Serial.println(wifiGetIP()); break;
    }

  } else if (cmd == "forget") {
    Serial.println(F("Forgetting WiFi credentials and restarting AP..."));
    wifiForgetNetwork();

  } else if (cmd == "help") {
    Serial.println(F("Commands: status, wifi, forget, dispense1, open1, close1, buzz, ping, sync, help"));

  } else {
    Serial.print(F("Unknown command: "));
    Serial.println(cmd);
  }
}
