# Open Questions

> Items pending validation, prototyping, or explicit user decision.

## Hardware
- [ ] Exact servo model (MG90S recommended, not confirmed)
- [ ] Exact proximity sensor model and range
- [ ] Exact ESP32-S3 GPIO map (placeholders in `config.h`)
- [ ] Exact ESP32-C3 GPIO map (placeholders in `config.h`)
- [ ] Exact module-to-module connector type
- [ ] Whether hatch position sensor is included in first prototype

## Mechanical
- [ ] Exact dispenser step angle (depends on pocket count)
- [ ] Exact spiral/rotor geometry and dimensions
- [ ] Exact cylinder diameter and height
- [ ] Hatch geometry and actuation method
- [ ] Module enclosure dimensions
- [ ] Mechanical locking mechanism between modules

## Power
- [ ] Exact USB-C charger current rating
- [ ] Exact protection circuit (diode vs MOSFET)
- [ ] Servo rail fuse rating
- [ ] Bulk capacitance value

## Software
- [ ] Database provider (Supabase? MongoDB? other?)
- [ ] Maximum number of supported expansion modules
- [ ] Reminder persistence and escalation rules
- [ ] Missed dose handling policy
- [ ] User authentication for web app

## Validation
- [ ] Test dispensing with client's actual medications
- [ ] Verify anti-double-feed mechanism
- [ ] Measure actual servo current draw
- [ ] Verify proximity sensor reliability
