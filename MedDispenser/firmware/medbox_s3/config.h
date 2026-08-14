/*
 * MedBox ESP32-S3 — Configuration
 * =================================
 * All pin assignments, timing constants, and configurable parameters.
 *
 * STATUS: Pin assignments are NOT FINALIZED — placeholders based on
 * ESP32-S3 SuperMini available GPIOs. Validate against actual board.
 */

#ifndef MEDBOX_S3_CONFIG_H
#define MEDBOX_S3_CONFIG_H

// ── Board: ESP32-S3 SuperMini ──────────────────────────────────────────────
// Available GPIOs vary by board revision. Avoid strapping pins (GPIO0, GPIO45, GPIO46).

// ── UART to ESP32-C3 ──────────────────────────────────────────────────────
// Using UART1 (hardware serial). GPIO pins are NOT FINALIZED.
#define S3_UART_TX_PIN        17   // S3 TX → C3 RX
#define S3_UART_RX_PIN        18   // S3 RX ← C3 TX
#define S3_UART_BAUD          115200

// ── Proximity / IR Sensor ─────────────────────────────────────────────────
// Sensor model: NOT FINALIZED. Pin is placeholder.
#define PROXIMITY_PIN         5    // Digital input from proximity sensor
#define PROXIMITY_ACTIVE      LOW  // Active level depends on sensor model

// ── Buzzer ────────────────────────────────────────────────────────────────
#define BUZZER_PIN            6    // PWM-capable GPIO for buzzer
#define BUZZER_FREQ_HZ        2700 // Typical piezo resonant frequency
#define BUZZER_CHANNEL        0    // LEDC channel for tone generation

// ── Wi-Fi ─────────────────────────────────────────────────────────────────
// Credentials are stored in NVS (provisioned via captive portal on first boot).
// No hardcoded SSID/password — the AP captive portal handles everything.
#define WIFI_AP_SSID           "MedBox_WiFi"        // SoftAP name for provisioning
#define WIFI_CONNECT_TIMEOUT_MS  15000             // Per-attempt connection timeout
#define WIFI_RETRY_INTERVAL_MS   30000             // Reconnect retry interval

// ── NTP / Time ────────────────────────────────────────────────────────────
#define NTP_SERVER            "pool.ntp.org"
#define NTP_GMT_OFFSET_SEC    28800   // UTC+8 (Philippines)
#define NTP_DAYLIGHT_OFFSET   0

// ── Backend API ───────────────────────────────────────────────────────────
#define API_BASE_URL          "https://modular-med-dispenser.netlify.app"
#define API_SYNC_ENDPOINT     "/api/sync"
#define API_LOG_ENDPOINT      "/api/dispense-log"
#define API_NOTIFY_ENDPOINT   "/api/notify"
#define API_DEVICE_ENDPOINT   "/api/devices"
#define API_SYNC_INTERVAL_MS  300000  // 5 minutes
#define API_HTTP_TIMEOUT_MS   10000   // HTTP request timeout
#define API_DEVICE_ID         "medbox-001"   // Unique device identifier

// ── Medication Reminder ───────────────────────────────────────────────────
#define REMINDER_INTERVAL_MS  300000  // Re-remind every 5 minutes
#define REMINDER_MAX_RETRIES  12      // Max reminders before giving up (1 hour)
#define BUZZER_ON_DURATION_MS 3000    // Buzz duration per reminder cycle
#define BUZZER_PATTERN_COUNT  3       // Number of beeps per reminder

// ── Proximity Detection ──────────────────────────────────────────────────
#define PROXIMITY_DEBOUNCE_MS   500   // Debounce for presence detection
#define PROXIMITY_HOLD_TIME_MS  2000  // User must be present this long

// ── Hatch Timing ─────────────────────────────────────────────────────────
#define HATCH_OPEN_WAIT_MS    15000   // Time to keep hatch open for user
#define HATCH_CLOSE_DELAY_MS  1000    // Delay before closing after timeout

// ── Schedule Storage ─────────────────────────────────────────────────────
#define MAX_SCHEDULES         16      // Max locally stored schedules
#define SCHEDULE_NVS_NAMESPACE "medbox"

// ── System ───────────────────────────────────────────────────────────────
#define SERIAL_BAUD           115200  // USB serial debug
#define LED_PIN               -1      // Onboard LED, -1 if not used
#define WATCHDOG_TIMEOUT_S    30      // Task watchdog timeout

#endif // MEDBOX_S3_CONFIG_H
