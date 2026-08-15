# MedBox — Firmware State Machine

## States

```cpp
enum SystemState {
    STATE_IDLE,
    STATE_REMINDER,
    STATE_WAITING_FOR_USER,
    STATE_HATCH_OPENING,
    STATE_DISPENSING,
    STATE_USER_TAKING_MEDICINE,
    STATE_HATCH_CLOSING,
    STATE_COMPLETED,
    STATE_ERROR
};
```

## State Transition Diagram

```
                    ┌──────────────────────────────────┐
                    │                                  │
                    ▼                                  │
              ┌──────────┐                             │
              │   IDLE   │◄────────────────────────────┤
              └────┬─────┘                             │
                   │ schedule pending                  │
                   ▼                                   │
              ┌──────────┐                             │
              │ REMINDER │ buzzer + ntfy               │
              └────┬─────┘                             │
                   │                                   │
                   ▼                                   │
         ┌─────────────────────┐                       │
         │ WAITING_FOR_USER    │                       │
         │                     │──── max retries ──────┘
         │ (re-remind every    │     → log missed
         │  5 min)             │
         └────────┬────────────┘
                  │ proximity detected
                  ▼
         ┌─────────────────────┐
         │   HATCH_OPENING     │ UART: OPEN,N
         └────────┬────────────┘
                  │ delay
                  ▼
         ┌─────────────────────┐
         │    DISPENSING        │ UART: DISPENSE,N
         └────────┬────────────┘
                  │ delay
                  ▼
         ┌─────────────────────┐
         │ USER_TAKING_MEDICINE│ wait HATCH_OPEN_WAIT_MS
         └────────┬────────────┘
                  │ timeout
                  ▼
         ┌─────────────────────┐
         │   HATCH_CLOSING     │ UART: CLOSE,N
         └────────┬────────────┘
                  │ delay
                  ▼
         ┌─────────────────────┐
         │    COMPLETED        │ log event → IDLE
         └────────────────────┘
```

## Error State
- Entered on servo communication failure, timeout, or invalid state
- Recovers to IDLE after 5 seconds
- Should log the error for backend reporting

## Timing Constants

| Parameter | Default | Config Key |
|-----------|---------|-----------|
| Reminder interval | 5 min | `REMINDER_INTERVAL_MS` |
| Max reminders | 12 | `REMINDER_MAX_RETRIES` |
| Buzzer duration | 3 sec | `BUZZER_ON_DURATION_MS` |
| Hatch open wait | 15 sec | `HATCH_OPEN_WAIT_MS` |
