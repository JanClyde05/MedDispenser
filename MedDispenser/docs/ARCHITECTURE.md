# MedBox — System Architecture

> **Last Updated:** 2026-08-22 — Reflects dual-path time sync (NTP + server), 10s command polling, and HTTP/HTTPS dual compatibility.

## Overview

```
┌──────────────────────────────────────────────────────────────────────┐
│                           USER                                       │
│                            │                                         │
│              ┌─────────────┴─────────────┐                           │
│              ▼                           ▼                           │
│          WEB APP                    ntfy PUSH                        │
│          (any page)                    ▲                             │
│              │                         │                             │
│              ▼                         │                             │
│       NETLIFY BACKEND ─────────────────┘                             │
│              │                                                       │
│              │ HTTPS sync (5 min) + command poll (10s)               │
│              ▼                                                       │
│   ┌────────────────────────┐                                         │
│   │      ESP32-S3          │  Wi-Fi, Schedule, Time, Proximity,      │
│   │      MAIN BRAIN        │  Buzzer, System logic                   │
│   └───────────┬────────────┘                                         │
│               │ UART (115200 baud)                                   │
│               ▼                                                      │
│   ┌────────────────────────┐                                         │
│   │      ESP32-C3          │  Servo control for all modules          │
│   │   SERVO CONTROLLER     │                                         │
│   └───┬──────┬──────┬──────┘                                         │
│       │      │      │                                                │
│       ▼      ▼      ▼                                                │
│     MOD1   MOD2   MOD3    (each: 2 servos — dispenser + hatch)      │
│                                                                      │
│            USB-C 5V POWER                                            │
│               │                                                      │
│        ┌──────┴──────┐                                               │
│        ▼             ▼                                               │
│   LOGIC POWER   SERVO POWER                                         │
└──────────────────────────────────────────────────────────────────────┘
```

## Controller Responsibilities

| Controller | Role | Responsibilities |
|-----------|------|-----------------|
| ESP32-S3 SuperMini | Main brain / IoT | Wi-Fi, API sync, schedule, NTP/RTC, server time sync, proximity, buzzer, state machine, UART to C3 |
| ESP32-C3 SuperMini | Servo controller | All dispenser servos (360° continuous rotation), all hatch servos (0°–90°), UART command execution |

## Communication
- S3 ↔ C3: UART (115200 baud), text-based command/response protocol
- S3 TX: GPIO 1 → C3 RX: GPIO 20
- S3 RX: GPIO 2 ← C3 TX: GPIO 21
- S3 ↔ Backend: HTTP/HTTPS polling (every 5 min for schedule sync, every 10s for hardware test commands)
- Backend → Phone: ntfy push notifications

## Key Design Principles
1. **Separation**: High-level logic (S3) vs actuator control (C3) vs mechanical dose control
2. **Local operation**: Device retains schedule locally in NVS Flash, operates during Internet loss
3. **Mechanical safety**: Dispenser geometry prevents double-feeding, not just software
4. **IR-gated dispensing**: Hatch opens **ONLY** when IR proximity sensor detects user hand — never simultaneously with buzzer/dispenser
5. **Dual time sync**: NTP (pool.ntp.org) as primary, server epoch from `/api/sync` response as fallback

## Servo Motor Details

### Dispenser Servo (SG90R — Modified 360° Continuous Rotation)
- **Modification**: Internal potentiometer disconnected, physical stopper removed
- **Control method**: `attach()` → `write(180)` (full speed forward) → `delay(DISPENSER_REV_TIME_MS)` → `detach()` (hard stop)
- **Why `detach()`**: Without an internal potentiometer, `write(90)` does NOT stop the motor. Only cutting the PWM signal via `detach()` forces the internal motor driver to kill power instantly.
- **Rev time**: 2400ms per 360° physical revolution (calibrated empirically)
- **Multi-pill**: For `pillsPerDose = N`, the servo runs N consecutive 2.4s revolutions with 300ms pause between each

### Hatch Servo (Standard SG90 — 0° to 90°)
- **Closed**: `write(0)` → 0° position (sealed)
- **Open**: `write(90)` → 90° position (user retrieval window)
- Uses standard `attach()` → `write(angle)` → `delay(300ms)` → `detach()`

## HTTP / HTTPS Dual Compatibility

The API client uses `_beginHttp()` helper to detect URL scheme:
- `http://` URLs: Uses plain HTTP (for local testing with `http://192.168.x.x:8888`)
- `https://` URLs: Uses HTTPS with `WiFiClientSecure` + `setInsecure()` (for Netlify deployment)

This prevents `HTTP -1` SSL errors when testing locally.

## Schedule Activation Architecture

Schedules are triggered by **two independent systems** running in parallel:

| System | Location | Mechanism |
|--------|----------|-----------|
| **Web Schedule Checker** | `app.js` (browser) | Polls `/api/medications` every 30s, sends `/api/dispense-command` when time matches |
| **ESP32 Schedule Manager** | `schedule_manager.cpp` (firmware) | Evaluates NVS-stored schedules every `loop()` iteration against NTP/server time |

Both paths converge: Web sends a dispense command → ESP32 polls it via `/api/dispense-command` every 10s. ESP32's own schedule manager can also trigger independently if time matches.
