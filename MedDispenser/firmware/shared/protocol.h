/*
 * MedBox — Shared UART Protocol Definitions
 * ===========================================
 * This file defines the command/response protocol between ESP32-S3 and ESP32-C3.
 *
 * COPY this file into both medbox_s3/ and medbox_c3/ sketch folders for Arduino IDE.
 * This copy in shared/ is the CANONICAL version — always edit here first.
 *
 * Protocol format (text-based, newline-terminated):
 *   Command:  CMD,<module_id>[,<extra>]\n
 *   Response: OK,CMD,<module_id>\n  or  ERR,CMD,<module_id>,<error_code>\n
 *
 * Module IDs: 1 = main module, 2+ = expansion modules
 */

#ifndef MEDBOX_PROTOCOL_H
#define MEDBOX_PROTOCOL_H

// ── UART Settings ──────────────────────────────────────────────────────────
#define PROTOCOL_BAUD_RATE    115200
#define PROTOCOL_TIMEOUT_MS   2000     // Max wait for response
#define PROTOCOL_MAX_MSG_LEN  64       // Max message length in bytes
#define PROTOCOL_DELIMITER    ','
#define PROTOCOL_TERMINATOR   '\n'

// ── Command Strings ────────────────────────────────────────────────────────
// S3 → C3 commands
#define CMD_DISPENSE    "DISPENSE"   // Dispense one dose from module
#define CMD_OPEN        "OPEN"       // Open hatch on module
#define CMD_CLOSE       "CLOSE"      // Close hatch on module
#define CMD_HOME        "HOME"       // Return dispenser to home position
#define CMD_STATUS      "STATUS"     // Query module servo status
#define CMD_PING        "PING"       // Heartbeat / connection check

// C3 → S3 response prefixes
#define RSP_OK          "OK"
#define RSP_ERR         "ERR"
#define RSP_PONG        "PONG"

// ── Error Codes ────────────────────────────────────────────────────────────
#define ERR_UNKNOWN_CMD     1   // Unrecognized command
#define ERR_INVALID_MODULE  2   // Module ID out of range
#define ERR_SERVO_FAULT     3   // Servo did not reach target (future)
#define ERR_BUSY            4   // Another operation in progress
#define ERR_PARSE           5   // Malformed message

// ── Module Limits ──────────────────────────────────────────────────────────
#define MODULE_MIN_ID       1
#define MODULE_MAX_ID       3   // Current design supports up to 3 modules
#define SERVOS_PER_MODULE   2   // 1 dispenser + 1 hatch

#endif // MEDBOX_PROTOCOL_H
