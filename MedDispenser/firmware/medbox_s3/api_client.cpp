/*
 * MedBox — API Client
 * Stub implementation — flesh out when Netlify backend is ready.
 */

#include "api_client.h"
#include "config.h"
#include "wifi_manager.h"
#include "schedule_manager.h"

static unsigned long _lastSyncMs = 0;

void apiClientInit() {
  Serial.println(F("API client initialized (stub)"));
}

void apiClientUpdate() {
  if (!wifiIsConnected()) return;

  unsigned long now = millis();
  if ((now - _lastSyncMs) >= API_SYNC_INTERVAL_MS) {
    _lastSyncMs = now;
    // TODO: Implement schedule sync
    // 1. GET API_BASE_URL + API_SYNC_ENDPOINT
    // 2. Parse JSON schedule array
    // 3. scheduleClearAll() + scheduleAdd() for each
    // 4. scheduleSaveToNVS()
    Serial.println(F("API sync: stub — not yet implemented"));
  }
}

void apiClientForceSync() {
  _lastSyncMs = 0;  // Force next update cycle to sync
  Serial.println(F("API sync forced"));
}
