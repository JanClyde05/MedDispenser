# MedBox — UART Protocol Specification

## Physical Layer
- **Interface**: UART (Hardware Serial)
- **Baud rate**: 115200
- **Data format**: 8N1
- **Wiring**: S3 TX → C3 RX, S3 RX ← C3 TX, common GND

## Message Format

### Commands (S3 → C3)
```
CMD,<module_id>\n
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
| `DISPENSE,N` | Dispense one dose from module N | `DISPENSE,1\n` |
| `OPEN,N` | Open hatch on module N | `OPEN,2\n` |
| `CLOSE,N` | Close hatch on module N | `CLOSE,2\n` |
| `HOME,N` | Return module N to home position | `HOME,1\n` |
| `STATUS,N` | Query servo status of module N | `STATUS,3\n` |
| `PING,0` | Connection check | `PING,0\n` |

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
