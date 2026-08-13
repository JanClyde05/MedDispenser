# Expansion Concept

## Status: FINALIZED

## Principle
> One medication type = one medication module.

## Module Configurations
- **1 medicine**: Main module only (no expansion needed)
- **2 medicines**: Main + 1 expansion
- **3 medicines**: Main + 2 expansions
- **N medicines**: Main + (N-1) expansions

## Expansion Module Contents
- Cylindrical medicine reservoir
- Spiral/rotary dispenser mechanism
- 1 dispenser servo
- 1 hatch servo
- Mechanical/electrical attachment interface

## What Expansion Modules Do NOT Have
- No MCU
- No proximity sensor
- No buzzer
- No power input

## Electrical Connection
Per expansion module: 4 signals
1. 5V (servo power)
2. GND
3. Dispenser servo signal (from C3)
4. Hatch servo signal (from C3)

## Mechanical Connection
Physical locking mechanism (NOT FINALIZED):
- Slide rails / dovetail / tongue-and-groove / keyed latch

## Maximum Modules
NOT FINALIZED — depends on C3 GPIO count, servo power budget, and mechanical size.
Current firmware supports 3 modules (1 main + 2 expansion).

## Related
- [[System Architecture]]
- [[Connectors]]
- [[GPIO Map]]
