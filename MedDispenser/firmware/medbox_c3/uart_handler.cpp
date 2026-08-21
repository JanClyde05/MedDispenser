/*
 * MedBox — UART Handler (C3 side)
 *
 * IMPORTANT: Uses Serial1 on dedicated pins for S3 communication.
 * Serial (USB CDC) is used ONLY for debug output.
 * This avoids the UART0 conflict on ESP32-C3 SuperMini.
 *
 * Requires: "USB CDC On Boot" = Enabled in Arduino IDE board settings.
 */

#include "uart_handler.h"
#include "config.h"
#include "protocol.h"
#include "servo_controller.h"

// Dedicated UART0 for S3 communication on pins 20/21 (separate from USB CDC Serial debug)
static HardwareSerial SerialS3(0);  // UART0

static char _rxBuffer[PROTOCOL_MAX_MSG_LEN];
static int  _rxIndex = 0;

// Forward declaration
static void _processCommand(const char* message);

void uartHandlerInit() {
  SerialS3.begin(C3_UART_BAUD, SERIAL_8N1, C3_UART_RX_PIN, C3_UART_TX_PIN);
  Serial.println(F("UART handler initialized (Serial1 on RX="));
  Serial.print(C3_UART_RX_PIN);
  Serial.print(F(", TX="));
  Serial.print(C3_UART_TX_PIN);
  Serial.println(F(")"));
}

void uartHandlerUpdate() {
  while (SerialS3.available()) {
    char c = SerialS3.read();

    if (c == PROTOCOL_TERMINATOR) {
      _rxBuffer[_rxIndex] = '\0';
      _processCommand(_rxBuffer);
      _rxIndex = 0;
    } else if (_rxIndex < PROTOCOL_MAX_MSG_LEN - 1) {
      _rxBuffer[_rxIndex++] = c;
    }
  }
}

static void _processCommand(const char* message) {
  // Parse: CMD,moduleId
  String msg = String(message);
  int delimIdx = msg.indexOf(PROTOCOL_DELIMITER);

  Serial.print(F("S3 → C3: "));
  Serial.println(msg);

  if (delimIdx < 0) {
    uartSendError("UNKNOWN", 0, ERR_PARSE);
    return;
  }

  String cmd = msg.substring(0, delimIdx);
  int moduleId = msg.substring(delimIdx + 1).toInt();

  // Validate module ID (except for PING which uses 0)
  if (cmd != CMD_PING && (moduleId < MODULE_MIN_ID || moduleId > MODULE_MAX_ID)) {
    uartSendError(cmd.c_str(), moduleId, ERR_INVALID_MODULE);
    return;
  }

  // ── Dispatch commands ────────────────────────────────────────────
  if (cmd == CMD_DISPENSE) {
    int count = 1;
    int secondDelim = msg.indexOf(PROTOCOL_DELIMITER, delimIdx + 1);
    if (secondDelim > 0) {
      count = msg.substring(secondDelim + 1).toInt();
      if (count <= 0) count = 1;
    }
    servoDispense(moduleId, count);
    uartSendResponse(RSP_OK, CMD_DISPENSE, moduleId);

  } else if (cmd == CMD_OPEN) {
    servoOpenHatch(moduleId);
    uartSendResponse(RSP_OK, CMD_OPEN, moduleId);

  } else if (cmd == CMD_CLOSE) {
    servoCloseHatch(moduleId);
    uartSendResponse(RSP_OK, CMD_CLOSE, moduleId);

  } else if (cmd == CMD_HOME) {
    servoHome(moduleId);
    uartSendResponse(RSP_OK, CMD_HOME, moduleId);

  } else if (cmd == CMD_STATUS) {
    // TODO: return actual servo positions
    uartSendResponse(RSP_OK, CMD_STATUS, moduleId);

  } else if (cmd == CMD_PING) {
    SerialS3.print(RSP_PONG);
    SerialS3.print(PROTOCOL_TERMINATOR);
    Serial.println(F("PONG sent"));

  } else {
    uartSendError(cmd.c_str(), moduleId, ERR_UNKNOWN_CMD);
  }
}

void uartSendResponse(const char* prefix, const char* cmd, uint8_t moduleId) {
  // Send response to S3 over dedicated UART
  SerialS3.print(prefix);
  SerialS3.print(PROTOCOL_DELIMITER);
  SerialS3.print(cmd);
  SerialS3.print(PROTOCOL_DELIMITER);
  SerialS3.print(moduleId);
  SerialS3.print(PROTOCOL_TERMINATOR);

  // Also echo to USB debug
  Serial.print(F("C3 → S3: "));
  Serial.print(prefix);
  Serial.print(PROTOCOL_DELIMITER);
  Serial.print(cmd);
  Serial.print(PROTOCOL_DELIMITER);
  Serial.println(moduleId);
}

void uartSendError(const char* cmd, uint8_t moduleId, int errorCode) {
  // Send error to S3 over dedicated UART
  SerialS3.print(RSP_ERR);
  SerialS3.print(PROTOCOL_DELIMITER);
  SerialS3.print(cmd);
  SerialS3.print(PROTOCOL_DELIMITER);
  SerialS3.print(moduleId);
  SerialS3.print(PROTOCOL_DELIMITER);
  SerialS3.print(errorCode);
  SerialS3.print(PROTOCOL_TERMINATOR);

  // Also echo to USB debug
  Serial.print(F("C3 ERR: "));
  Serial.print(cmd);
  Serial.print(F(" M"));
  Serial.print(moduleId);
  Serial.print(F(" code="));
  Serial.println(errorCode);
}
