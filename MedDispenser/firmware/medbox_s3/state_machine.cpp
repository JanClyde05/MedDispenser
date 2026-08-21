/*
 * MedBox — Medication Event State Machine
 *
 * Flow:
 *   IDLE → REMINDER → WAITING_FOR_USER → HATCH_OPENING
 *   → DISPENSING → USER_TAKING_MEDICINE → HATCH_CLOSING → COMPLETED → IDLE
 */

#include "state_machine.h"
#include "config.h"
#include "proximity.h"
#include "buzzer.h"
#include "uart_command.h"
#include "protocol.h"
#include "schedule_manager.h"
#include "time_manager.h"
#include "api_client.h"

static SystemState _state = STATE_IDLE;
static MedSchedule _activeSchedule;
static unsigned long _stateEnteredMs = 0;
static unsigned long _lastReminderMs = 0;
static int _reminderCount = 0;

static void _enterState(SystemState newState);

void stateMachineInit() {
  _state = STATE_IDLE;
  Serial.println(F("State machine: IDLE"));
}

void stateMachineUpdate() {
  unsigned long now = millis();
  unsigned long elapsed = now - _stateEnteredMs;

  switch (_state) {

    // ── IDLE: Wait for a pending schedule ─────────────────────────────
    case STATE_IDLE:
      if (scheduleHasPending()) {
        _activeSchedule = scheduleGetPending();
        _enterState(STATE_REMINDER);
      }
      break;

    // ── REMINDER: Buzz + notify, then wait for user ──────────────────
    case STATE_REMINDER:
      // Initial buzz
      buzzerPatternStart(BUZZER_PATTERN_COUNT, BUZZER_ON_DURATION_MS / BUZZER_PATTERN_COUNT, 200);
      _reminderCount = 1;
      _lastReminderMs = now;

      // Send ntfy notification via backend
      {
        char timeBuf[6];
        snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d",
                 _activeSchedule.hour, _activeSchedule.minute);
        apiClientNotify(_activeSchedule.moduleId,
                        _activeSchedule.medicineName,
                        _activeSchedule.pillsPerDose,
                        timeBuf, "reminder");
      }

      Serial.print(F("REMINDER: "));
      Serial.print(_activeSchedule.medicineName);
      Serial.print(F(" Module "));
      Serial.println(_activeSchedule.moduleId);

      _enterState(STATE_WAITING_FOR_USER);
      break;

    // ── WAITING_FOR_USER: Poll proximity, re-remind periodically ─────
    case STATE_WAITING_FOR_USER:
      if (proximityIsDetected()) {
        Serial.println(F("User detected — opening hatch"));
        _enterState(STATE_HATCH_OPENING);
        break;
      }

      // Re-remind every REMINDER_INTERVAL_MS
      if ((now - _lastReminderMs) >= REMINDER_INTERVAL_MS) {
        _reminderCount++;
        if (_reminderCount > REMINDER_MAX_RETRIES) {
          Serial.println(F("Max reminders reached — logging missed dose"));

          // Log missed dose to backend
          apiClientLogDispense(_activeSchedule.moduleId,
                               _activeSchedule.medicineName,
                               _activeSchedule.pillsPerDose, "missed");

          // Send missed notification via ntfy
          {
            char timeBuf[6];
            snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d",
                     _activeSchedule.hour, _activeSchedule.minute);
            apiClientNotify(_activeSchedule.moduleId,
                            _activeSchedule.medicineName,
                            _activeSchedule.pillsPerDose,
                            timeBuf, "missed");
          }

          scheduleMarkHandled(_activeSchedule.moduleId,
                              _activeSchedule.hour,
                              _activeSchedule.minute);
          _enterState(STATE_IDLE);
          break;
        }
        buzzerPatternStart(BUZZER_PATTERN_COUNT, BUZZER_ON_DURATION_MS / BUZZER_PATTERN_COUNT, 200);
        _lastReminderMs = now;
        Serial.print(F("Re-reminder #"));
        Serial.println(_reminderCount);
      }
      break;

    // ── HATCH_OPENING: Command C3 to open hatch ─────────────────────
    case STATE_HATCH_OPENING:
      uartSendCommand(CMD_OPEN, _activeSchedule.moduleId);
      // TODO: wait for acknowledgement / hatch sensor confirmation
      // For now, proceed after a fixed delay
      if (elapsed >= 1000) {
        _enterState(STATE_DISPENSING);
      }
      break;

    // ── DISPENSING: Command C3 to dispense ───────────────────────────
    case STATE_DISPENSING:
      // Send one DISPENSE command per pill
      // For multiple pills, the C3 can handle multi-step indexing,
      // or S3 sends N commands. Keeping it simple: one command = one dose event.
      uartSendCommand(CMD_DISPENSE, _activeSchedule.moduleId);
      // TODO: for pillsPerDose > 1, implement multi-dispense loop
      if (elapsed >= 1500) {
        _enterState(STATE_USER_TAKING_MEDICINE);
      }
      break;

    // ── USER_TAKING_MEDICINE: Wait for user to retrieve ──────────────
    case STATE_USER_TAKING_MEDICINE:
      // Wait a fixed duration for the user to take the medicine
      if (elapsed >= HATCH_OPEN_WAIT_MS) {
        _enterState(STATE_HATCH_CLOSING);
      }
      break;

    // ── HATCH_CLOSING: Command C3 to close hatch ────────────────────
    case STATE_HATCH_CLOSING:
      uartSendCommand(CMD_CLOSE, _activeSchedule.moduleId);
      if (elapsed >= 1000) {
        _enterState(STATE_COMPLETED);
      }
      break;

    // ── COMPLETED: Log event, return to IDLE ─────────────────────────
    case STATE_COMPLETED:
      Serial.print(F("Dispense completed: "));
      Serial.print(_activeSchedule.medicineName);
      Serial.print(F(" Module "));
      Serial.println(_activeSchedule.moduleId);

      // Log successful dispense to backend
      apiClientLogDispense(_activeSchedule.moduleId,
                           _activeSchedule.medicineName,
                           _activeSchedule.pillsPerDose, "dispensed");

      scheduleMarkHandled(_activeSchedule.moduleId,
                          _activeSchedule.hour,
                          _activeSchedule.minute);
      _enterState(STATE_IDLE);
      break;

    // ── ERROR: Recoverable error state ──────────────────────────────
    case STATE_ERROR:
      // Log error, attempt recovery after delay
      if (elapsed >= 5000) {
        Serial.println(F("Recovering from error → IDLE"));
        _enterState(STATE_IDLE);
      }
      break;
  }
}

static void _enterState(SystemState newState) {
  _state = newState;
  _stateEnteredMs = millis();
}

SystemState stateMachineGetState() {
  return _state;
}

const char* stateMachineGetStateName() {
  switch (_state) {
    case STATE_IDLE:                  return "IDLE";
    case STATE_REMINDER:              return "REMINDER";
    case STATE_WAITING_FOR_USER:      return "WAITING_FOR_USER";
    case STATE_HATCH_OPENING:         return "HATCH_OPENING";
    case STATE_DISPENSING:            return "DISPENSING";
    case STATE_USER_TAKING_MEDICINE:  return "USER_TAKING_MEDICINE";
    case STATE_HATCH_CLOSING:         return "HATCH_CLOSING";
    case STATE_COMPLETED:             return "COMPLETED";
    case STATE_ERROR:                 return "ERROR";
    default:                          return "UNKNOWN";
  }
}

MedSchedule stateMachineGetActiveSchedule() {
  return _activeSchedule;
}

bool stateMachineTriggerDispense(const MedSchedule& schedule) {
  if (_state != STATE_IDLE) {
    Serial.println(F("TriggerDispense: rejected — state machine busy"));
    return false;
  }

  _activeSchedule = schedule;
  Serial.print(F("TriggerDispense: forcing dispense for "));
  Serial.print(_activeSchedule.medicineName);
  Serial.print(F(" Module "));
  Serial.println(_activeSchedule.moduleId);

  _enterState(STATE_REMINDER);
  return true;
}
