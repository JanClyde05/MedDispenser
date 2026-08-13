# System Architecture

## Status: FINALIZED

The MedBox uses a dual-MCU architecture with a cloud backend.

## Block Diagram

```
USER → WEB APP → NETLIFY BACKEND → ESP32-S3 (UART) → ESP32-C3 → Servos
                        ↓
                   ntfy → Phone
```

## Key Components

| Component | Role |
|-----------|------|
| ESP32-S3 SuperMini | Main brain — Wi-Fi, schedule, sensors, buzzer |
| ESP32-C3 SuperMini | Servo controller — all dispenser + hatch servos |
| Netlify | Web frontend + serverless backend |
| ntfy | Push notifications |
| USB-C 5V | Single power input |

## Design Principles
1. **Separation of concerns**: S3 handles logic, C3 handles actuators
2. **Local-first**: Device operates from local schedule during Internet loss
3. **Mechanical safety**: Dispenser geometry prevents double-feed
4. **Presence-gated**: Hatch opens only when user is detected nearby

## Related
- [[Controller Responsibilities]]
- [[Power Architecture]]
- [[Expansion Concept]]
