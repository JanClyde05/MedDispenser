/*
 * MedBox — UART Handler (C3 side)
 * Parses incoming commands from S3 and dispatches to servo controller.
 */

#ifndef MEDBOX_UART_HANDLER_H
#define MEDBOX_UART_HANDLER_H

#include <Arduino.h>

void uartHandlerInit();
void uartHandlerUpdate();

// Send a response back to S3
void uartSendResponse(const char* prefix, const char* cmd, uint8_t moduleId);
void uartSendError(const char* cmd, uint8_t moduleId, int errorCode);

#endif
