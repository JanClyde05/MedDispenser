# MedBox C3 Firmware — ESP32-C3 SuperMini

Dedicated servo controller. Receives UART commands from S3, drives all servos.

## Modules

| File | Responsibility |
|------|---------------|
| `medbox_c3.ino` | Main entry, setup/loop |
| `config.h` | Servo pin map, angle constants |
| `uart_handler` | UART command parser & response |
| `servo_controller` | Servo abstraction per module |
| `protocol.h` | Shared protocol definitions (copy from `shared/`) |

## Required Library
- `ESP32Servo` by Kevin Harrington — install via Arduino IDE Library Manager

## Important Notes
- Servo angle constants in `config.h` are **placeholders** — calibrate experimentally
- `DISPENSER_STEP_ANGLE` depends on the physical spiral/rotor geometry
- Servos are attached only during movement and detached after to save power
