# MedBox — Local Testing Guide

This guide explains how to run and test the MedBox web dashboard, backend API functions, and ESP32 hardware flow on `localhost` before deploying to Netlify.

---

## 1. Running the Local Development Server

Netlify CLI serves both the static web frontend (`web/frontend/`) and the serverless backend functions (`web/netlify/functions/`) locally, along with an in-memory Netlify Blobs store.

### Steps:
1. Open a terminal in the `web` directory:
   ```powershell
   cd web
   ```
2. Launch the Netlify development server:
   ```powershell
   npx netlify dev
   ```
3. Open your browser and navigate to:
   ```
   http://localhost:8888
   ```

---

## 2. Testing Frontend & Backend Features Locally

### A. Manual Test Alert & Hardware Trigger
1. Navigate to **Medications** (`http://localhost:8888/medications.html`).
2. Click the **"🔔 Test Alert"** button on any medication card.
3. **Expected Behavior:**
   - Sends a push notification via `ntfy` to your mobile device.
   - Queues a pending dispense command in the local `/api/dispense-command` store.
   - Toast popup confirms success in the browser interface.

### B. Automatic Scheduled Notifications
1. Navigate to **Dashboard** (`http://localhost:8888/index.html`).
2. Add a test medication with a scheduled time **1–2 minutes in the future**.
3. Keep the Dashboard page open.
4. **Expected Behavior:**
   - Every 30 seconds, the client-side schedule checker compares local time against active schedules.
   - When the scheduled minute arrives:
     - Prominent warning toast appears on screen.
     - `ntfy` notification is sent to your phone.
     - Dispense command is queued for the ESP32.

---

## 3. Connecting Physical ESP32 Hardware to Localhost (Optional)

To test the physical ESP32-S3 board with your local development server:

1. **Find your Local IP Address:**
   ```powershell
   ipconfig
   ```
   *Look for your IPv4 address (e.g., `192.168.1.5`).*

2. **Update Hardware Configuration:**
   In `firmware/medbox_s3/config.h`, update `API_BASE_URL` to point to your computer's local IP address:
   ```cpp
   // Point to local development server
   #define API_BASE_URL "http://192.168.1.5:8888"
   ```

3. **Ensure Network Connectivity:**
   - Both your computer and the ESP32 must be connected to the **same Wi-Fi network**.
   - Compile and flash the updated firmware to the ESP32-S3.

4. **Verify Hardware Execution:**
   - Open Arduino IDE Serial Monitor (115200 baud).
   - During sync (`/api/sync`), the ESP32 will pick up the queued dispense commands.
   - The state machine will execute:
     1. **Buzzer Alarm** sound pattern
     2. **IR Proximity Sensor** detection loop
     3. **Servo Hatch Opening** command (`OPEN`)
     4. **Servo Dispenser** movement (`DISPENSE`)
     5. **Servo Hatch Closing** command (`CLOSE`)
