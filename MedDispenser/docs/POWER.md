# MedBox — Power Architecture

## Topology

```
USB-C INPUT (5V)
      │
  Protection (reverse polarity, overcurrent, ESD)
      │
  5V MAIN BUS
      │
  ┌───┴────────────────┐
  │                    │
  ▼                    ▼
LOGIC POWER        SERVO POWER RAIL
  │                    │
  ├── ESP32-S3         ├── Module 1 Dispenser Servo
  ├── ESP32-C3         ├── Module 1 Hatch Servo
  ├── Proximity Sensor ├── Module 2 Dispenser Servo
  └── Buzzer           ├── Module 2 Hatch Servo
                       ├── Module 3 Dispenser Servo
                       └── Module 3 Hatch Servo
```

## Current Budget (Estimated)

> ⚠️ Exact values depend on servo model (NOT FINALIZED)

| Component | Typical Current | Peak Current |
|-----------|----------------|-------------|
| ESP32-S3 (Wi-Fi active) | ~150 mA | ~350 mA |
| ESP32-C3 | ~80 mA | ~150 mA |
| Proximity sensor | ~20 mA | ~30 mA |
| Buzzer | ~30 mA | ~50 mA |
| **Logic subtotal** | **~280 mA** | **~580 mA** |
| | | |
| SG90-class servo (idle) | ~10 mA | — |
| SG90-class servo (moving) | ~200 mA | ~500 mA |
| **Per module (2 servos)** | **~20 mA idle** | **~1000 mA peak** |
| | | |
| **Worst case (3 modules, 2 moving)** | — | **~1580 mA** |

## Power Supply Recommendation

- Minimum: **5V 2A** USB-C charger (for 1-2 module prototype)
- Recommended: **5V 3A** for 3-module system with margin
- Servo operations should be sequenced (not all at once) to reduce peak draw

## Protection Considerations

| Protection | Purpose | Implementation |
|-----------|---------|---------------|
| Reverse polarity | Prevent damage from miswired supply | Schottky diode or P-MOSFET circuit |
| Overcurrent | Protect wiring/PCB traces | Resettable fuse (PTC) on servo rail |
| Transient/ESD | Protect USB-C port | TVS diode on VBUS |
| Bulk capacitance | Smooth servo current spikes | 100–470 µF electrolytic near servo rail |
| Decoupling | Clean logic power | 100 nF ceramic near each ESP32 |

## Key Rule
> Servos are powered from the dedicated 5V servo rail, NOT through the ESP32's 5V pin.
