/*
 * MedBox — Wi-Fi Manager
 * Handles WiFi provisioning via captive portal, connection, and reconnection.
 *
 * Boot sequence:
 *   1. Try NVS-saved credentials (from previous captive portal provisioning)
 *   2. Fall back to SoftAP + captive portal for user to enter credentials
 *
 * Captive portal serves HTML/CSS/JS from LittleFS with:
 *   - Network scanner (tappable list)
 *   - Manual SSID/password entry
 */

#ifndef MEDBOX_WIFI_MANAGER_H
#define MEDBOX_WIFI_MANAGER_H

#include <Arduino.h>

// WiFi connection state
enum WifiState : uint8_t {
  WIFI_DISCONNECTED = 0,
  WIFI_CONNECTING,
  WIFI_CONNECTED,
  WIFI_AP_MODE
};

void      wifiManagerInit();
void      wifiManagerUpdate();
bool      wifiIsConnected();
String    wifiGetIP();
WifiState wifiGetState();

// Force re-enter AP mode (e.g., from serial command)
void      wifiStartAP();

// Disconnect and clear saved credentials, re-enter AP mode
void      wifiForgetNetwork();

#endif // MEDBOX_WIFI_MANAGER_H
