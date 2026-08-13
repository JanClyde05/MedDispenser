# GPIO Map

## Status: NOT FINALIZED — placeholder assignments

## ESP32-S3 SuperMini
| GPIO | Function |
|------|----------|
| 17 | UART1 TX → C3 |
| 18 | UART1 RX ← C3 |
| 5 | Proximity sensor |
| 6 | Buzzer (PWM) |

Avoid: GPIO 0, 45, 46 (strapping)

## ESP32-C3 SuperMini
| GPIO | Function |
|------|----------|
| 20 | UART RX ← S3 |
| 21 | UART TX → S3 |
| 0 | M1 Dispenser servo |
| 1 | M1 Hatch servo |
| 3 | M2 Dispenser servo |
| 4 | M2 Hatch servo |
| 5 | M3 Dispenser servo |
| 6 | M3 Hatch servo |

Avoid: GPIO 2, 8 (strapping), GPIO 9 (boot button)

## Related
- [[Controller Responsibilities]]
- [[Servo Selection]]
- [[Connectors]]
