/*
 * MedBox — API Client
 * HTTPS communication with Netlify backend for schedule sync,
 * dispense logging, and notifications.
 */

#ifndef MEDBOX_API_CLIENT_H
#define MEDBOX_API_CLIENT_H

#include <Arduino.h>

void apiClientInit();
void apiClientUpdate();      // Periodic sync check
void apiClientForceSync();   // Manual trigger

// Sync schedules from backend → local NVS
bool apiClientSyncSchedules();

// Log a dispense event to backend
void apiClientLogDispense(uint8_t moduleId, const char* medicineName,
                          uint8_t qty, const char* status);

// Send a notification (reminder or missed dose) via backend → ntfy
void apiClientNotify(uint8_t moduleId, const char* medicineName,
                     uint8_t dose, const char* time, const char* type);

// Register device / send heartbeat
void apiClientHeartbeat();

#endif
