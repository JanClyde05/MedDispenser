# Hatch Design

## Status: FINALIZED (concept), NOT FINALIZED (geometry)

## Purpose
- Restrict access to medicine compartment
- Only open when user is present (proximity confirmed)
- Reduce contamination/exposure

## Servo Control
- Closed: `HATCH_CLOSED_ANGLE` (0° placeholder)
- Open: `HATCH_OPEN_ANGLE` (90° placeholder)
- Command: `OPEN,N` / `CLOSE,N` via [[UART Protocol]]

## Hatch Position Sensor
Status: OPTIONAL / NOT FINALIZED for first prototype

Possible implementations:
- Magnetic reed switch + magnet on hatch
- Hall-effect sensor
- Limit/micro switch

Purpose: confirm physical open/closed state (not pill detection).

## Timing
- Stays open for `HATCH_OPEN_WAIT_MS` (15 sec default)
- Closes automatically after timeout

## Related
- [[Dispenser Mechanism]]
- [[State Machine]]
