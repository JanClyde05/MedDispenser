# UART Protocol

## Status: FINALIZED (format), NOT FINALIZED (error handling details)

## Physical
- UART, 115200 baud, 8N1
- S3 TX → C3 RX, S3 RX ← C3 TX, common GND

## Format
```
Command:  CMD,moduleId\n
Response: OK,CMD,moduleId\n
Error:    ERR,CMD,moduleId,errorCode\n
```

## Commands
| Command | Description |
|---------|-------------|
| `DISPENSE,N` | Dispense one dose from module N |
| `OPEN,N` | Open hatch N |
| `CLOSE,N` | Close hatch N |
| `HOME,N` | Return module N to home |
| `STATUS,N` | Query module N status |
| `PING,0` | Heartbeat check |

## Error Codes
| Code | Name |
|------|------|
| 1 | Unknown command |
| 2 | Invalid module |
| 3 | Servo fault (future) |
| 4 | Busy |
| 5 | Parse error |

## Timing
- Timeout: 2000 ms
- Max message: 64 bytes

## Related
- [[S3 Firmware Overview]]
- [[C3 Firmware Overview]]
