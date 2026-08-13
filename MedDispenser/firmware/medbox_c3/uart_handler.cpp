/*
 * MedBox — UART Handler (C3 side)
 */

#include "uart_handler.h"
#include "config.h"
#include "protocol.h"
#include "servo_controller.h"

// Use Serial0 (default UART) for S3 communication
// Note: This shares with USB serial on C3 SuperMini.
// For production, consider using Serial1 with custom pins.
#define SerialS3 Serial0

static char _rxBuffer[PROTOCOL_MAX_MSG_LEN];
static int  _rxIndex = 0;

// Forward declaration
static void _processCommand(const char* message);

void uartHandlerInit() {
  // If using separate UART pins:
  // Serial1.begin(C3_UART_BAUD, SERIAL_8N1, C3_UART_RX_PIN, C3_UART_TX_PIN);
  // For now, using default Serial which may conflict with USB debug.
  // Adjust based on actual board wiring.
  Serial.println(F("UART handler initialized"));
}

void uartHandlerUpdate() {
  while (Serial.available()) {
    char c = Serial.read();

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
    servoDispense(moduleId);
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
    Serial.print(RSP_PONG);
    Serial.print(PROTOCOL_TERMINATOR);

  } else {
    uartSendError(cmd.c_str(), moduleId, ERR_UNKNOWN_CMD);
  }
}

void uartSendResponse(const char* prefix, const char* cmd, uint8_t moduleId) {
  Serial.print(prefix);
  Serial.print(PROTOCOL_DELIMITER);
  Serial.print(cmd);
  Serial.print(PROTOCOL_DELIMITER);
  Serial.print(moduleId);
  Serial.print(PROTOCOL_TERMINATOR);
}

void uartSendError(const char* cmd, uint8_t moduleId, int errorCode) {
  Serial.print(RSP_ERR);
  Serial.print(PROTOCOL_DELIMITER);
  Serial.print(cmd);
  Serial.print(PROTOCOL_DELIMITER);
  Serial.print(moduleId);
  Serial.print(PROTOCOL_DELIMITER);
  Serial.print(errorCode);
  Serial.print(PROTOCOL_TERMINATOR);
}
