# C3 Firmware Overview

## Board: ESP32-C3 SuperMini
## Arduino IDE, Pure Arduino (no PlatformIO)

## Modules

| Module | File | Purpose |
|--------|------|---------|
| Main | `medbox_c3.ino` | Setup/loop |
| Config | `config.h` | Servo pin map, angle constants |
| UART | `uart_handler` | Command parser & response |
| Servos | `servo_controller` | Per-module servo abstraction |
| Protocol | `protocol.h` | Shared protocol defs |

## Architecture
- Simple command dispatcher: receive UART → parse → act → respond
- Servos attached only during movement, detached after (power saving)
- Dispenser uses incremental angle stepping
- Module ID maps to GPIO pin pair via lookup table in `config.h`

## Dependencies
- `ESP32Servo` by Kevin Harrington

## Important Notes
- `DISPENSER_STEP_ANGLE` is a placeholder — calibrate with real hardware
- `delay()` is used during servo movements — acceptable since C3's only job is servo control
- Servo min/max pulse widths may need tuning per servo model

## Related
- [[S3 Firmware Overview]]
- [[UART Protocol]]
- [[GPIO Map]]
- [[Servo Selection]]
