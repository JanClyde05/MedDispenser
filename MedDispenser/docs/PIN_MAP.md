# MedBox — GPIO Pin Map

> ⚠️ **STATUS: NOT FINALIZED** — These are placeholder assignments.
> Validate against the actual SuperMini board pinouts before wiring.

## ESP32-S3 SuperMini

| GPIO | Function | Notes |
|------|----------|-------|
| 17 | UART1 TX → C3 RX | S3↔C3 communication |
| 18 | UART1 RX ← C3 TX | S3↔C3 communication |
| 5 | Proximity sensor (digital input) | Active level TBD (sensor model not finalized) |
| 6 | Buzzer (PWM output) | LEDC channel 0 |
| — | USB Serial | Debug / programming |

### Strapping Pins to Avoid
| GPIO | Reason |
|------|--------|
| 0 | Boot strapping |
| 45 | Boot strapping |
| 46 | Boot strapping |

---

## ESP32-C3 SuperMini

| GPIO | Function | Notes |
|------|----------|-------|
| 20 | UART RX ← S3 TX | S3↔C3 communication |
| 21 | UART TX → S3 RX | S3↔C3 communication |
| 0 | Module 1 Dispenser servo | Signal only — power from servo rail |
| 1 | Module 1 Hatch servo | Signal only |
| 3 | Module 2 Dispenser servo | Signal only |
| 4 | Module 2 Hatch servo | Signal only |
| 5 | Module 3 Dispenser servo | Signal only |
| 6 | Module 3 Hatch servo | Signal only |

### Strapping Pins to Avoid
| GPIO | Reason |
|------|--------|
| 2 | Boot strapping |
| 8 | Boot strapping |
| 9 | Boot button (usable but risky) |

---

## Expansion Module Connector (per module)

| Pin | Signal |
|-----|--------|
| 1 | 5V (servo power rail) |
| 2 | GND |
| 3 | Dispenser servo signal |
| 4 | Hatch servo signal |
