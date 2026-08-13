# MedBox S3 Firmware — ESP32-S3 SuperMini

Main system brain handling Wi-Fi, scheduling, time, sensors, and UART commands to C3.

## Modules

| File | Responsibility |
|------|---------------|
| `medbox_s3.ino` | Main entry, setup/loop, debug CLI |
| `config.h` | Pin definitions, timing constants |
| `wifi_manager` | Wi-Fi connection & reconnect |
| `time_manager` | NTP sync & time utilities |
| `schedule_manager` | Local schedule storage (NVS) & evaluation |
| `state_machine` | Medication event state machine |
| `proximity` | Proximity sensor driver |
| `buzzer` | Non-blocking buzzer with patterns |
| `uart_command` | UART protocol to C3 |
| `api_client` | HTTPS backend sync (stub) |
| `protocol.h` | Shared protocol definitions (copy from `shared/`) |

## Serial Debug Commands
Open Arduino Serial Monitor at 115200 baud:
- `status` — System status overview
- `dispense1` — Test dispense on module 1
- `open1` / `close1` — Test hatch on module 1
- `buzz` — Test buzzer
- `ping` — Ping C3
- `sync` — Force backend sync
- `help` — List commands
