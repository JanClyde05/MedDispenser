/*
 * MedBox — API Client
 * HTTPS communication with Netlify backend for schedule sync and logging.
 */

#ifndef MEDBOX_API_CLIENT_H
#define MEDBOX_API_CLIENT_H

#include <Arduino.h>

void apiClientInit();
void apiClientUpdate();      // Periodic sync check
void apiClientForceSync();   // Manual trigger

// TODO: Implement when backend is ready
// void apiClientLogDispense(uint8_t moduleId, const char* medicine, uint8_t qty, const char* status);
// void apiClientNotify(uint8_t moduleId, const char* medicine);

#endif
