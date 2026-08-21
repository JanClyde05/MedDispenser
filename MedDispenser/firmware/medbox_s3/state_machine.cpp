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

static SystemState   _state = STATE_IDLE;
static unsigned long _stateEnteredMs = 0;
static unsigned long _lastReminderMs = 0;
static int           _reminderCount = 0;
static MedSchedule   _activeSchedule;
static bool          _cmdSentInState = false;

static void _enterState(SystemState newState);

void stateMachineInit() {
  _enterState(STATE_IDLE);
  Serial.println(F("State machine initialized"));
}

void stateMachineUpdate() {
  unsigned long now = millis();
  unsigned long elapsed = now - _stateEnteredMs;

  switch (_state) {

    // ── IDLE: Check for scheduled dispense triggers ─────────────────
    case STATE_IDLE:
      if (scheduleHasPending()) {
        _activeSchedule = scheduleGetPending();
        _enterState(STATE_REMINDER);
      }
      break;

    // ── REMINDER: Alarm buzzer + notify ──────────────────────────────
    case STATE_REMINDER:
      _reminderCount = 1;
      _lastReminderMs = now;

      // Sound buzzer alarm pattern immediately when reminder starts
      buzzerPatternStart(3, 500, 300);

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
      Serial.print(_activeSchedule.moduleId);
      Serial.print(F(" Dose: "));
      Serial.println(_activeSchedule.pillsPerDose);

      // Dispense pill into internal staging while hatch stays CLOSED (0°)
      _enterState(STATE_DISPENSING);
      break;

    // ── DISPENSING: Command C3 to spin dispenser (hatch stays CLOSED) 
    case STATE_DISPENSING:
      if (!_cmdSentInState) {
        uint8_t count = _activeSchedule.pillsPerDose;
        if (count == 0) count = 1;

        Serial.print(F("Dispensing pill(s) behind closed hatch: "));
        Serial.println(count);

        uartSendCommandEx(CMD_DISPENSE, _activeSchedule.moduleId, count);
        _cmdSentInState = true;
      }

      // Wait for C3 to finish 360° revolutions
      {
        uint8_t count = _activeSchedule.pillsPerDose;
        if (count == 0) count = 1;
        unsigned long waitMs = (unsigned long)count * 3000 + 500;
        if (elapsed >= waitMs) {
          Serial.println(F("Pill(s) dropped into compartment. Waiting for hand at IR sensor..."));
          _enterState(STATE_WAITING_FOR_USER);
        }
      }
      break;

    // ── WAITING_FOR_USER: Hatch CLOSED (0°) — waiting for IR hand sensor
    case STATE_WAITING_FOR_USER:
      // Hatch opens ONLY when IR proximity sensor detects hand presence!
      if (proximityIsDetected()) {
        Serial.println(F("🔴 IR Sensor Activated! Opening hatch (90°)..."));
        _enterState(STATE_HATCH_OPENING);
        break;
      }

      // Re-remind every REMINDER_INTERVAL_MS with buzzer if user hasn't approached
      if ((now - _lastReminderMs) >= REMINDER_INTERVAL_MS) {
        _reminderCount++;
        if (_reminderCount > REMINDER_MAX_RETRIES) {
          Serial.println(F("Max reminders reached — logging missed dose"));

          apiClientLogDispense(_activeSchedule.moduleId,
                               _activeSchedule.medicineName,
                               _activeSchedule.pillsPerDose, "missed");

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
        buzzerPatternStart(3, 500, 300);
        _lastReminderMs = now;
        Serial.print(F("Re-reminder #"));
        Serial.println(_reminderCount);
      }
      break;

    // ── HATCH_OPENING: Command C3 to open hatch (90°) ────────────────
    case STATE_HATCH_OPENING:
      if (!_cmdSentInState) {
        uartSendCommand(CMD_OPEN, _activeSchedule.moduleId);
        _cmdSentInState = true;
      }
      if (elapsed >= 1000) {
        _enterState(STATE_USER_TAKING_MEDICINE);
      }
      break;

    // ── USER_TAKING_MEDICINE: Hatch OPEN (90°) for user to retrieve ────
    case STATE_USER_TAKING_MEDICINE:
      // Keep hatch open for 5 seconds for user to retrieve pill
      if (elapsed >= 5000) {
        _enterState(STATE_HATCH_CLOSING);
      }
      break;

    // ── HATCH_CLOSING: Command C3 to close hatch (0°) ────────────────
    case STATE_HATCH_CLOSING:
      if (!_cmdSentInState) {
        uartSendCommand(CMD_CLOSE, _activeSchedule.moduleId);
        _cmdSentInState = true;
      }
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
  _cmdSentInState = false;  // Reset command-sent flag on state transition!
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
