# State Machine

## Status: FINALIZED (states), firmware implemented

## States
1. **IDLE** — waiting for a schedule to trigger
2. **REMINDER** — buzzer + ntfy notification sent
3. **WAITING_FOR_USER** — polling proximity, re-reminding every 5 min
4. **HATCH_OPENING** — UART OPEN command to C3
5. **DISPENSING** — UART DISPENSE command to C3
6. **USER_TAKING_MEDICINE** — hatch open, waiting for user
7. **HATCH_CLOSING** — UART CLOSE command to C3
8. **COMPLETED** — event logged, return to IDLE
9. **ERROR** — recovers to IDLE after timeout

## Key Behaviors
- Re-reminder every `REMINDER_INTERVAL_MS` (5 min default)
- Max `REMINDER_MAX_RETRIES` (12 default = 1 hour) before logging missed dose
- Proximity must be confirmed before hatch opens
- Hatch stays open for `HATCH_OPEN_WAIT_MS` (15 sec default)

## Implementation
File: `medbox_s3/state_machine.cpp`

## Related
- [[S3 Firmware Overview]]
- [[UART Protocol]]
