/*
 * MedBox — Wi-Fi Manager
 * Handles connection, reconnection, and status reporting.
 */

#ifndef MEDBOX_WIFI_MANAGER_H
#define MEDBOX_WIFI_MANAGER_H

#include <Arduino.h>

void wifiManagerInit();
void wifiManagerUpdate();
bool wifiIsConnected();
String wifiGetIP();

#endif
