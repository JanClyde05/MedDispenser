# Controller Responsibilities

## Status: FINALIZED

## ESP32-S3 SuperMini — Main Brain

- Wi-Fi connectivity
- Web/backend communication (HTTPS sync)
- Medication schedule synchronization
- Local schedule storage (NVS/Preferences)
- NTP time synchronization
- Optional RTC support
- Medication event [[State Machine|state machine]]
- Proximity/IR sensor logic (user presence)
- Buzzer control
- UART communication to [[C3 Firmware Overview|ESP32-C3]]
- Error handling and logging

## ESP32-C3 SuperMini — Servo Controller

- Receive [[UART Protocol|UART commands]] from S3
- Control dispenser servos (all modules)
- Control hatch servos (all modules)
- Send acknowledgement responses
- No Wi-Fi, no schedule knowledge, no sensor reading

## Why Two MCUs?
- Clean separation of IoT workload from real-time servo control
- C3 can drive up to 6 servos (3 modules × 2 servos) without Wi-Fi overhead
- S3 can focus on connectivity and logic without servo timing concerns

## Related
- [[System Architecture]]
- [[GPIO Map]]
