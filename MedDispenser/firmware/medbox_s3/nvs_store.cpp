/*
 * MedBox — NVS Persistent Storage
 */

#include "nvs_store.h"
#include "config.h"
#include <Preferences.h>

static Preferences _prefs;

void nvsStoreInit() {
  _prefs.begin(SCHEDULE_NVS_NAMESPACE, false);  // "medbox", read/write
  Serial.println(F("NVS store initialized"));
}

bool nvsHasWifiCreds() {
  return _prefs.getString("wifi_ssid", "").length() > 0;
}

String nvsGetWifiSsid() {
  return _prefs.getString("wifi_ssid", "");
}

String nvsGetWifiPass() {
  return _prefs.getString("wifi_pass", "");
}

void nvsSaveWifi(const String& ssid, const String& pass) {
  _prefs.putString("wifi_ssid", ssid);
  _prefs.putString("wifi_pass", pass);
  Serial.print(F("NVS: saved WiFi credentials for "));
  Serial.println(ssid);
}

void nvsClearWifi() {
  _prefs.remove("wifi_ssid");
  _prefs.remove("wifi_pass");
  Serial.println(F("NVS: WiFi credentials cleared"));
}
