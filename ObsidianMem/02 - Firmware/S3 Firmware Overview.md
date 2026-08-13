# S3 Firmware Overview

## Board: ESP32-S3 SuperMini
## Arduino IDE, Pure Arduino (no PlatformIO)

## Modules

| Module | File | Purpose |
|--------|------|---------|
| Main | `medbox_s3.ino` | Setup/loop, debug CLI |
| Config | `config.h` | All pin defs and constants |
| Wi-Fi | `wifi_manager` | Connection & reconnect |
| Time | `time_manager` | NTP sync |
| Schedule | `schedule_manager` | Local schedule storage (NVS) |
| State Machine | `state_machine` | Medication event flow |
| Proximity | `proximity` | User presence detection |
| Buzzer | `buzzer` | Non-blocking tone patterns |
| UART | `uart_command` | S3→C3 command/response |
| API | `api_client` | Backend sync (stub) |
| Protocol | `protocol.h` | Shared protocol defs |

## Architecture
- Non-blocking state machine in `loop()`
- Each module has an `init()` and `update()` function
- No `delay()` calls in main loop (except startup)
- NVS used for persistent schedule storage

## Dependencies
- `WiFi.h` (built-in)
- `Preferences.h` (built-in)
- `ArduinoJson` (for API JSON parsing)

## Related
- [[C3 Firmware Overview]]
- [[UART Protocol]]
- [[State Machine]]
- [[Schedule Storage]]
