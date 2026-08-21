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

void handleSerialDebug() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();

  if (cmd == "open1") {
    Serial.println(F("DEBUG: Opening hatch M1"));
    servoOpenHatch(1);
  } else if (cmd == "close1") {
    Serial.println(F("DEBUG: Closing hatch M1"));
    servoCloseHatch(1);
  } else if (cmd == "dispense1") {
    Serial.println(F("DEBUG: Dispensing M1"));
    servoDispense(1);
  } else if (cmd == "home1") {
    Serial.println(F("DEBUG: Homing M1"));
    servoHome(1);
  } else if (cmd == "help") {
    Serial.println(F("C3 Commands: open1, close1, dispense1, home1"));
  } else if (cmd.length() > 0) {
    Serial.print(F("Unknown C3 command: "));
    Serial.println(cmd);
  }
}

void loop() {
  // Check for incoming commands from S3
  uartHandlerUpdate();

  // Process any pending servo movements
  servoControllerUpdate();

  // Serial debug commands from C3 USB Serial Monitor
  handleSerialDebug();
}
