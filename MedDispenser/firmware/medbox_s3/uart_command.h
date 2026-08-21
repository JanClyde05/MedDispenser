/*
 * MedBox — UART Command Interface (S3 side)
 * Sends commands to ESP32-C3 and processes responses.
 */

#ifndef MEDBOX_UART_COMMAND_H
#define MEDBOX_UART_COMMAND_H

#include <Arduino.h>

void uartCommandInit();
void uartCommandUpdate();

// Send a command to the C3. Returns true if sent successfully.
bool uartSendCommand(const char* cmd, uint8_t moduleId);
bool uartSendCommandEx(const char* cmd, uint8_t moduleId, uint8_t extraVal);

// Check if the last command was acknowledged
bool uartLastAcknowledged();

// Get the last response string (for debugging)
String uartGetLastResponse();

#endif
