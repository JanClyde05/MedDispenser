# MedBox — System Architecture

## Overview

```
┌──────────────────────────────────────────────────────────────────────┐
│                           USER                                       │
│                            │                                         │
│              ┌─────────────┴─────────────┐                           │
│              ▼                           ▼                           │
│          WEB APP                    ntfy PUSH                        │
│              │                         ▲                             │
│              ▼                         │                             │
│       NETLIFY BACKEND ─────────────────┘                             │
│              │                                                       │
│              │ HTTPS sync                                            │
│              ▼                                                       │
│   ┌────────────────────────┐                                         │
│   │      ESP32-S3          │  Wi-Fi, Schedule, Time, Proximity,      │
│   │      MAIN BRAIN        │  Buzzer, System logic                   │
│   └───────────┬────────────┘                                         │
│               │ UART                                                 │
│               ▼                                                      │
│   ┌────────────────────────┐                                         │
│   │      ESP32-C3          │  Servo control for all modules          │
│   │   SERVO CONTROLLER     │                                         │
│   └───┬──────┬──────┬──────┘                                         │
│       │      │      │                                                │
│       ▼      ▼      ▼                                                │
│     MOD1   MOD2   MOD3    (each: 2 servos + dispenser + hatch)      │
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
| ESP32-S3 SuperMini | Main brain / IoT | Wi-Fi, API sync, schedule, NTP/RTC, proximity, buzzer, state machine, UART to C3 |
| ESP32-C3 SuperMini | Servo controller | All dispenser servos, all hatch servos, UART command execution |

## Communication
- S3 ↔ C3: UART (115200 baud), text-based command/response protocol
- S3 ↔ Backend: HTTPS polling (every 5 min)
- Backend → Phone: ntfy push notifications

## Key Design Principles
1. **Separation**: High-level logic (S3) vs actuator control (C3) vs mechanical dose control
2. **Local operation**: Device retains schedule locally, operates during Internet loss
3. **Mechanical safety**: Dispenser geometry prevents double-feeding, not just software
4. **Presence-gated**: Hatch only opens when proximity sensor detects user
