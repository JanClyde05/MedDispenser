# MedBox — Firmware

## Structure

```
firmware/
├── medbox_s3/      ESP32-S3 SuperMini — Main brain / IoT
├── medbox_c3/      ESP32-C3 SuperMini — Dedicated servo controller
└── shared/         Shared protocol definitions
```

## Arduino IDE Setup

### Board Support
1. Add ESP32 board URL in Arduino IDE Preferences:
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
2. Install **esp32** board package via Board Manager.

### Board Selection
- **S3**: Select `ESP32S3 Dev Module` (or your SuperMini variant)
- **C3**: Select `ESP32C3 Dev Module` (or your SuperMini variant)

### Required Libraries
Install via Arduino IDE Library Manager:
- `ESP32Servo` by Kevin Harrington (for C3 servo control)
- `ArduinoJson` by Benoit Blanchon (for API JSON parsing, S3 only)
- `WiFi` (built-in with ESP32 board package)
- `Preferences` (built-in with ESP32 board package)

### Shared Protocol Header
The `shared/protocol.h` file must be present in **both** sketch folders.
Copy it manually:
```
copy shared\protocol.h medbox_s3\protocol.h
copy shared\protocol.h medbox_c3\protocol.h
```
Always edit the canonical version in `shared/` first, then copy.

## Communication
S3 ↔ C3 communicate via UART using a simple text protocol.
See `shared/protocol.h` and `docs/UART_PROTOCOL.md` for details.
