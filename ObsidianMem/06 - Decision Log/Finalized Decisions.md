# Finalized Decisions

> Last updated: 2026-08-14

## Architecture
- [x] ESP32-S3 SuperMini = main brain / IoT controller
- [x] ESP32-C3 SuperMini = dedicated servo controller
- [x] Main module = Medication Module #1
- [x] UART between S3 and C3 (115200 baud, text protocol)

## Hardware
- [x] 2 servos per module (dispenser + hatch)
- [x] Expansion modules have no MCU
- [x] USB-C 5V main power input (single input)
- [x] Dedicated servo power rail (separate from ESP32 logic)
- [x] Common ground

## Sensors
- [x] Proximity/IR sensor = detects medicine taker presence
- [x] Proximity sensor is NOT a pill-drop sensor
- [x] No pill-drop IR confirmation in base design

## Mechanical
- [x] Cylindrical gravity-fed medicine reservoir
- [x] Spiral/rotary/flower-hopper metering mechanism
- [x] Mechanical anti-double-feed through geometry
- [x] Every module uses same mechanical concept

## Software/IoT
- [x] Netlify = web frontend + backend
- [x] ntfy = push notifications (server-side publishing)
- [x] HTTPS/API + database (no WebSockets required)
- [x] Local schedule retained on device (NVS)
- [x] NTP for time sync, RTC recommended
- [x] Arduino IDE (no PlatformIO)

## Product
- [x] Main module is the minimum viable product
- [x] Expansion is optional
- [x] One medication type = one module
