# Power Architecture

## Status: FINALIZED (topology), NOT FINALIZED (exact ratings)

## Topology
- Single USB-C 5V input on main module
- Power protection (reverse polarity, overcurrent, ESD)
- 5V main bus splits to logic power and servo power rail
- Common ground throughout

## Key Rule
> Servos are powered from the dedicated servo power rail, NOT through ESP32's 5V pin.

## Current Estimates
- Logic side: ~280 mA typical, ~580 mA peak
- Per module (2 servos): ~20 mA idle, ~1000 mA peak
- 3-module worst case: ~1580 mA peak (assumes 2 servos moving simultaneously)

## Recommended Supply
- Prototype (1 module): 5V 2A minimum
- Full system (3 modules): 5V 3A recommended
- Sequence servo operations to avoid simultaneous peak draw

## Protection
- Reverse polarity: Schottky diode or P-MOSFET
- Overcurrent: PTC resettable fuse on servo rail
- Transient/ESD: TVS diode on VBUS
- Bulk capacitance: 100-470 µF near servo rail
- Decoupling: 100 nF ceramic near each ESP32

## Related
- [[System Architecture]]
- [[Servo Selection]]
