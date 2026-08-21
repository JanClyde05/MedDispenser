# MedBox — Firmware State Machine

> **Last Updated:** 2026-08-22 — Reflects IR-gated hatch opening & dispense-first flow.

## States

```cpp
enum SystemState {
    STATE_IDLE,
    STATE_REMINDER,
    STATE_DISPENSING,
    STATE_WAITING_FOR_USER,
    STATE_HATCH_OPENING,
    STATE_USER_TAKING_MEDICINE,
    STATE_HATCH_CLOSING,
    STATE_COMPLETED,
    STATE_ERROR
};
```

## State Transition Diagram

```
                    ┌──────────────────────────────────────┐
                    │                                      │
                    ▼                                      │
              ┌──────────┐                                 │
              │   IDLE   │◄────────────────────────────────┤
              └────┬─────┘                                 │
                   │ schedule pending                      │
                   ▼                                       │
              ┌──────────┐                                 │
              │ REMINDER │ buzzer + ntfy push              │
              └────┬─────┘                                 │
                   │ immediately                           │
                   ▼                                       │
         ┌─────────────────────┐                           │
         │    DISPENSING        │ UART: DISPENSE,N,count    │
         │                     │ (hatch stays CLOSED)      │
         │ Spins 360° × count  │                           │
         └────────┬────────────┘                           │
                  │ pill(s) dropped into compartment       │
                  ▼                                        │
         ┌─────────────────────┐                           │
         │ WAITING_FOR_USER    │                           │
         │                     │──── max retries ──────────┘
         │ Hatch CLOSED (0°)   │     → log missed dose
         │ Re-remind buzzer    │
         │  every 5 min        │
         └────────┬────────────┘
                  │ 🔴 IR proximity sensor DETECTED
                  ▼
         ┌─────────────────────┐
         │   HATCH_OPENING     │ UART: OPEN,N → 90°
         └────────┬────────────┘
                  │ 1s delay
                  ▼
         ┌─────────────────────┐
         │ USER_TAKING_MEDICINE│ Hatch OPEN for 5 seconds
         └────────┬────────────┘
                  │ timeout
                  ▼
         ┌─────────────────────┐
         │   HATCH_CLOSING     │ UART: CLOSE,N → 0°
         └────────┬────────────┘
                  │ 1s delay
                  ▼
         ┌─────────────────────┐
         │    COMPLETED        │ log event → IDLE
         └────────────────────┘
```

## Dispensing Sequence Detail

1. **REMINDER** — Buzzer alarm (3 beeps × 500ms) + ntfy push notification sent to phone
2. **DISPENSING** — Dispenser servo spins `pillsPerDose` × 360° revolutions behind **closed** hatch; pill(s) drop into internal staging compartment
3. **WAITING_FOR_USER** — Hatch remains sealed at 0°. System waits for user to place hand near IR sensor. Buzzer re-reminds every 5 minutes. After 12 retries (1 hour), logs as "missed dose"
4. **HATCH_OPENING** — Only triggered when `proximityIsDetected() == true`. Sends `OPEN` command → hatch rotates to 90°
5. **USER_TAKING_MEDICINE** — Hatch stays open for 5 seconds for user to retrieve pill
6. **HATCH_CLOSING** — Sends `CLOSE` command → hatch rotates back to 0°
7. **COMPLETED** — Logs "dispensed" to backend, marks schedule handled, returns to IDLE

## UART Command Spam Prevention

The state machine runs inside `loop()` at ~1000 iterations/second. A `_cmdSentInState` flag ensures each UART command (`DISPENSE`, `OPEN`, `CLOSE`) is sent **exactly once** per state entry. The flag resets automatically in `_enterState()`.

## Error State
- Entered on servo communication failure, timeout, or invalid state
- Recovers to IDLE after 5 seconds
- Should log the error for backend reporting

## Timing Constants

| Parameter | Default | Config Key |
|-----------|---------|------------|
| Reminder interval | 5 min | `REMINDER_INTERVAL_MS` |
| Max reminders | 12 | `REMINDER_MAX_RETRIES` |
| Buzzer pattern | 3 beeps × 500ms | `BUZZER_PATTERN_COUNT` |
| Dispenser rev time | 2.4s per pill | `DISPENSER_REV_TIME_MS` |
| S3 wait per pill | 3s per pill + 0.5s | Calculated in state machine |
| Hatch open duration | 5s | Hardcoded in `STATE_USER_TAKING_MEDICINE` |
| Hatch servo delay | 1s | Hardcoded in `STATE_HATCH_OPENING/CLOSING` |
