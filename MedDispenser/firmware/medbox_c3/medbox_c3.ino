/*
 * MedBox ESP32-C3 — Servo Controller Firmware
 * =============================================
 * Dedicated actuator controller: receives UART commands from ESP32-S3,
 * drives dispenser and hatch servos for all medication modules.
 *
 * Arduino IDE Board: ESP32-C3 Dev Module (or SuperMini variant)
 */

#include "config.h"
#include "uart_handler.h"
#include "servo_controller.h"

// Copy from firmware/shared/ — keep in sync with canonical version
#include "protocol.h"

void setup() {
  // Debug serial (USB)
  Serial.begin(SERIAL_DEBUG_BAUD);
  delay(500);
  Serial.println(F("=== MedBox C3 Servo Controller Starting ==="));

  // Initialize subsystems
  servoControllerInit();
  uartHandlerInit();

  Serial.println(F("=== MedBox C3 Ready ==="));
}

void loop() {
  // Check for incoming commands from S3
  uartHandlerUpdate();

  // Process any pending servo movements
  servoControllerUpdate();
}
