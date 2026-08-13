# Proximity Sensor

## Status: NOT FINALIZED

## Purpose
Detect whether the medicine taker is in the vicinity of the MedBox.
**NOT** a pill-drop sensor.

## Candidates

| Sensor | Type | Range | Output | Notes |
|--------|------|-------|--------|-------|
| HC-SR501 | PIR | ~7m | Digital | Motion-based, large, warm-up delay |
| RCWL-0516 | Microwave | ~5-7m | Digital | Through-enclosure, compact |
| VL53L0X | ToF laser | ~2m | I2C (digital) | Precise distance, narrow FOV |
| GP2Y0A21 | IR analog | 10-80cm | Analog | Classic, good short range |
| APDS-9960 | Multi-sensor | ~30cm | I2C | Gesture+proximity, very short range |

## Considerations
- Range should be "nearby" (0.5-2m typical use case)
- Must not false-trigger from pets or distant movement
- Should work reliably indoors
- Power consumption matters (always-on polling)

## Recommendation
**VL53L0X** (ToF) or **RCWL-0516** (microwave) for prototype.
- VL53L0X: precise distance, can set threshold, I2C interface
- RCWL-0516: simple digital output, detects through plastic enclosure

## Related
- [[GPIO Map]]
- [[State Machine]]
