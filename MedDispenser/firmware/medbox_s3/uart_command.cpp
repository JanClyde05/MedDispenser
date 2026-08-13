/*
 * MedBox — UART Command Interface (S3 side)
 */

#include "uart_command.h"
#include "config.h"
#include "protocol.h"

// Use HardwareSerial UART1 for C3 communication
static HardwareSerial SerialC3(1);

static String _lastResponse = "";
static bool   _acknowledged = false;
static char   _rxBuffer[PROTOCOL_MAX_MSG_LEN];
static int    _rxIndex = 0;

void uartCommandInit() {
  SerialC3.begin(S3_UART_BAUD, SERIAL_8N1, S3_UART_RX_PIN, S3_UART_TX_PIN);
  Serial.println(F("UART to C3 initialized"));
}

void uartCommandUpdate() {
  // Read incoming bytes from C3
  while (SerialC3.available()) {
    char c = SerialC3.read();

    if (c == PROTOCOL_TERMINATOR) {
      _rxBuffer[_rxIndex] = '\0';
      _lastResponse = String(_rxBuffer);
      _rxIndex = 0;

      // Check for OK/ERR prefix
      if (_lastResponse.startsWith(RSP_OK) || _lastResponse.startsWith(RSP_PONG)) {
        _acknowledged = true;
      } else if (_lastResponse.startsWith(RSP_ERR)) {
        _acknowledged = false;
        Serial.print(F("C3 error: "));
        Serial.println(_lastResponse);
      }

      Serial.print(F("C3 → S3: "));
      Serial.println(_lastResponse);

    } else if (_rxIndex < PROTOCOL_MAX_MSG_LEN - 1) {
      _rxBuffer[_rxIndex++] = c;
    }
  }
}

bool uartSendCommand(const char* cmd, uint8_t moduleId) {
  _acknowledged = false;

  String msg = String(cmd) + PROTOCOL_DELIMITER + String(moduleId);
  SerialC3.print(msg);
  SerialC3.print(PROTOCOL_TERMINATOR);

  Serial.print(F("S3 → C3: "));
  Serial.println(msg);

  return true;
}

bool uartLastAcknowledged() {
  return _acknowledged;
}

String uartGetLastResponse() {
  return _lastResponse;
}
