# MedBox — UART Protocol Specification

> **Last Updated:** 2026-08-22 — Added extended `DISPENSE,N,count` format for multi-pill dosing.

## Physical Layer
- **Interface**: UART (Hardware Serial)
- **Baud rate**: 115200
- **Data format**: 8N1
- **Wiring**: S3 TX (GPIO 1) → C3 RX (GPIO 20), S3 RX (GPIO 2) ← C3 TX (GPIO 21), common GND

### ESP32-C3 Silicon Constraint

The ESP32-C3 has **only 1 UART peripheral** (`UART0`). Using `HardwareSerial(1)` fails silently. The firmware uses `HardwareSerial SerialS3(0)` on pins 20 & 21 with **USB CDC On Boot = Enabled** in Arduino IDE board settings, keeping USB debug output functional alongside UART communication.

## Message Format

### Commands (S3 → C3)
```
CMD,<module_id>\n
CMD,<module_id>,<extra_value>\n
```

### Responses (C3 → S3)
```
OK,CMD,<module_id>\n
ERR,CMD,<module_id>,<error_code>\n
PONG\n
```

## Command Reference

| Command | Description | Example |
|---------|-------------|---------|
| `DISPENSE,N` | Dispense 1 pill from module N (default count=1) | `DISPENSE,1\n` |
| `DISPENSE,N,C` | Dispense C pills from module N (C revolutions) | `DISPENSE,1,3\n` |
| `OPEN,N` | Open hatch on module N (90°) | `OPEN,1\n` |
| `CLOSE,N` | Close hatch on module N (0°) | `CLOSE,1\n` |
| `HOME,N` | Return module N dispenser to home position | `HOME,1\n` |
| `STATUS,N` | Query servo status of module N | `STATUS,3\n` |
| `PING,0` | Connection check (heartbeat) | `PING,0\n` |

### Extended Format: `DISPENSE,N,C`

The `uartSendCommandEx()` function on the S3 sends multi-parameter messages. The C3's `uart_handler.cpp` parses the optional third field:

```
DISPENSE,1,2\n   →  Module 1, dispense 2 pills (2 full 360° revolutions)
DISPENSE,1\n     →  Module 1, dispense 1 pill (default)
```

## Response Examples

| Response | Meaning |
|----------|---------|
| `OK,DISPENSE,1` | Module 1 dispense completed |
| `OK,OPEN,2` | Module 2 hatch opened |
| `ERR,DISPENSE,4,2` | Error: module 4 is invalid (error code 2) |
| `PONG` | Heartbeat response |

## Error Codes

| Code | Name | Meaning |
|------|------|---------|
| 1 | `ERR_UNKNOWN_CMD` | Unrecognized command |
| 2 | `ERR_INVALID_MODULE` | Module ID out of range (1-3) |
| 3 | `ERR_SERVO_FAULT` | Servo did not reach target (future) |
| 4 | `ERR_BUSY` | Another operation in progress |
| 5 | `ERR_PARSE` | Malformed message |

## Timing
- **Timeout**: 2000 ms max wait for response
- **Max message length**: 64 bytes
- **Delimiter**: `,` (comma)
- **Terminator**: `\n` (newline)
