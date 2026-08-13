/*
 * MedBox — Wi-Fi Manager
 */

#include "wifi_manager.h"
#include "config.h"
#include <WiFi.h>

static unsigned long _lastRetryMs = 0;
static bool _connected = false;

void wifiManagerInit() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print(F("Wi-Fi connecting"));
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < WIFI_CONNECT_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    _connected = true;
    Serial.print(F("Wi-Fi connected: "));
    Serial.println(WiFi.localIP());
  } else {
    _connected = false;
    Serial.println(F("Wi-Fi connection failed — will retry"));
  }
}

void wifiManagerUpdate() {
  bool nowConnected = (WiFi.status() == WL_CONNECTED);

  if (nowConnected && !_connected) {
    Serial.print(F("Wi-Fi reconnected: "));
    Serial.println(WiFi.localIP());
  } else if (!nowConnected && _connected) {
    Serial.println(F("Wi-Fi lost"));
  }

  _connected = nowConnected;

  // Non-blocking reconnect attempt
  if (!_connected && (millis() - _lastRetryMs) >= WIFI_RETRY_INTERVAL_MS) {
    _lastRetryMs = millis();
    Serial.println(F("Wi-Fi reconnecting..."));
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
}

bool wifiIsConnected() {
  return _connected;
}

String wifiGetIP() {
  if (_connected) {
    return WiFi.localIP().toString();
  }
  return "0.0.0.0";
}
