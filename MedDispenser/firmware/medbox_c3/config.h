/*
 * MedBox ESP32-C3 — Configuration
 * =================================
 * Pin assignments, servo angle constants, and module-to-GPIO mapping.
 *
 * STATUS: GPIO assignments are NOT FINALIZED.
 * ESP32-C3 SuperMini has limited GPIOs. Strapping pins: GPIO2, GPIO8, GPIO9.
 * Avoid GPIO2 and GPIO8 for servo signals if possible.
 */

#ifndef MEDBOX_C3_CONFIG_H
#define MEDBOX_C3_CONFIG_H

// ── Board: ESP32-C3 SuperMini ──────────────────────────────────────────────
// Typical usable GPIOs: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 21
// GPIO2 = boot strapping (avoid for critical signals)
// GPIO8 = boot strapping (avoid for critical signals)
// GPIO9 = boot button (usable but be aware)
// GPIO20/21 = default UART0 (used for debug / S3 comms)

// ── UART from ESP32-S3 ────────────────────────────────────────────────────
// Using default UART0 pins for S3 communication.
// If USB debug is also needed, use UART1 or software serial.
#define C3_UART_RX_PIN        20   // C3 RX ← S3 TX
#define C3_UART_TX_PIN        21   // C3 TX → S3 RX
#define C3_UART_BAUD          115200

// ── Servo GPIO Map ────────────────────────────────────────────────────────
// Format: { dispenser_pin, hatch_pin } per module
// Module 1 = Main module, Module 2-3 = Expansion modules

// Module 1 (Main)
#define SERVO_M1_DISPENSER_PIN  0
#define SERVO_M1_HATCH_PIN      1

// Module 2 (Expansion)
#define SERVO_M2_DISPENSER_PIN  3
#define SERVO_M2_HATCH_PIN      4

// Module 3 (Expansion)
#define SERVO_M3_DISPENSER_PIN  5
#define SERVO_M3_HATCH_PIN      6

// ── Servo Angle Constants ─────────────────────────────────────────────────
// These are placeholder values — MUST be calibrated experimentally.

// Hatch positions
#define HATCH_CLOSED_ANGLE    0     // Hatch closed position (degrees)
#define HATCH_OPEN_ANGLE      90    // Hatch open position (degrees)

// Dispenser positions
#define DISPENSER_HOME_ANGLE  0     // Dispenser home/rest position
// The dispense step angle depends on the spiral/rotor geometry.
// Example: 24-pocket rotor → 15° per pocket. NOT FINALIZED.
#define DISPENSER_STEP_ANGLE  15    // Degrees per single dispense index

// ── Servo Timing ──────────────────────────────────────────────────────────
#define SERVO_MIN_PULSE_US    500   // Typical SG90-class min pulse
#define SERVO_MAX_PULSE_US    2400  // Typical SG90-class max pulse
#define SERVO_MOVE_DELAY_MS   300   // Time to allow servo to reach position
#define SERVO_DETACH_DELAY_MS 500   // Delay before detaching servo (save power)

// ── Module Count ──────────────────────────────────────────────────────────
#define NUM_MODULES           3     // Max supported modules (1 main + 2 expansion)

// ── System ───────────────────────────────────────────────────────────────
#define SERIAL_DEBUG_BAUD     115200

#endif // MEDBOX_C3_CONFIG_H
