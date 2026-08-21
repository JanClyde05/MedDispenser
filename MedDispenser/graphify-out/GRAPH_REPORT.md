# Graph Report - MedDispenser  (2026-08-22)

## Corpus Check
- 57 files · ~94,616 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 337 nodes · 463 edges · 36 communities (28 shown, 8 thin omitted)
- Extraction: 89% EXTRACTED · 11% INFERRED · 0% AMBIGUOUS · INFERRED: 51 edges (avg confidence: 0.85)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `d22ebba7`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- api_client.cpp
- time_manager.cpp
- wifi_manager.cpp
- servo_controller.cpp
- MedBox — Complete Device Architecture & Operation Guide
- MedBox — Complete Pin Mapping & Interconnect Specification
- data/app.js
- MedBox — UART Protocol Specification
- MedBox — System Architecture
- js/app.js
- buzzer.cpp
- Arduino IDE Setup
- js/medications.js
- settings.js
- MedBox — Local Testing Guide
- MedBox — Firmware State Machine
- package.json
- MedBox — Modular Smart Medication Dispenser
- MedBox — Power Architecture
- functions/medications.js
- MedBox — Netlify Functions
- MedBox — Web Application
- MedBox C3 Firmware — ESP32-C3 SuperMini
- MedBox — Frontend
- MedBox S3 Firmware — ESP32-S3 SuperMini
- history.js
- devices.js
- dispense-command.js
- dispense-log.js
- notify.js
- schedules.js
- sync.js
- api.js

## God Nodes (most connected - your core abstractions)
1. `stateMachineUpdate()` - 11 edges
2. `MedBox — Complete Device Architecture & Operation Guide` - 11 edges
3. `apiClientSyncSchedules()` - 9 edges
4. `_processCommand()` - 8 edges
5. `_beginHttp()` - 8 edges
6. `apiClientUpdate()` - 8 edges
7. `_getTime()` - 8 edges
8. `wifiIsConnected()` - 8 edges
9. `MedBox — System Architecture` - 8 edges
10. `wifiManagerInit()` - 7 edges

## Surprising Connections (you probably didn't know these)
- `apiClientUpdate()` --calls--> `buzzerPatternStart()`  [INFERRED]
  firmware/medbox_s3/api_client.cpp → firmware/medbox_s3/buzzer.cpp
- `apiClientSyncSchedules()` --calls--> `scheduleAdd()`  [INFERRED]
  firmware/medbox_s3/api_client.cpp → firmware/medbox_s3/schedule_manager.cpp
- `apiClientSyncSchedules()` --calls--> `scheduleClearAll()`  [INFERRED]
  firmware/medbox_s3/api_client.cpp → firmware/medbox_s3/schedule_manager.cpp
- `apiClientSyncSchedules()` --calls--> `scheduleSaveToNVS()`  [INFERRED]
  firmware/medbox_s3/api_client.cpp → firmware/medbox_s3/schedule_manager.cpp
- `apiClientSyncSchedules()` --calls--> `timeSetEpoch()`  [INFERRED]
  firmware/medbox_s3/api_client.cpp → firmware/medbox_s3/time_manager.cpp

## Import Cycles
- None detected.

## Communities (36 total, 8 thin omitted)

### Community 0 - "api_client.cpp"
Cohesion: 0.08
Nodes (28): apiClientHeartbeat(), apiClientLogDispense(), apiClientNotify(), _apiClientPollCommands(), apiClientSyncSchedules(), apiClientUpdate(), _beginHttp(), String (+20 more)

### Community 1 - "time_manager.cpp"
Cohesion: 0.16
Nodes (19): MedSchedule, scheduleAdd(), scheduleGetPending(), scheduleHasPending(), scheduleLoadFromNVS(), scheduleManagerInit(), scheduleManagerUpdate(), String (+11 more)

### Community 2 - "wifi_manager.cpp"
Cohesion: 0.16
Nodes (20): String, nvsClearWifi(), nvsGetWifiPass(), nvsGetWifiSsid(), nvsHasWifiCreds(), nvsSaveWifi(), String, _scanNetworksJson() (+12 more)

### Community 3 - "servo_controller.cpp"
Cohesion: 0.17
Nodes (12): _getDispenser(), _getHatch(), _rotatePillRevolution(), servoCloseHatch(), servoDispense(), servoHome(), servoOpenHatch(), _processCommand() (+4 more)

### Community 4 - "MedBox — Complete Device Architecture & Operation Guide"
Cohesion: 0.11
Nodes (18): 10. Revision History, 1. System Overview & Core Principles, 2. High-Level System Architecture, 3.1 ESP32-S3 SuperMini (System Brain), 3.2 ESP32-C3 SuperMini (Dedicated Servo Controller), 3. Microcontroller Responsibilities & Hardware Interconnects, 4. Electrical & Power Topology, 5. Firmware State Machine & Medication Flow (+10 more)

### Community 5 - "MedBox — Complete Pin Mapping & Interconnect Specification"
Cohesion: 0.14
Nodes (13): 1. System Interconnect Summary, 2. ESP32-S3 SuperMini (Main IoT Controller) Pin Map, 3. ESP32-C3 SuperMini (Servo Controller) Pin Map, 4.1 Module 1 (Main Box Dispenser & Hatch), 4.2 Module 2 (Expansion Unit #1), 4.3 Module 3 (Expansion Unit #2), 4.4 Proximity Sensor & Buzzer, 4. Peripherals & Actuators Connection Breakout (+5 more)

### Community 6 - "data/app.js"
Cohesion: 0.25
Nodes (11): closeModal(), connectFromModal(), connectManual(), connectToNetwork(), escapeHtml(), getSignalIcon(), pollConnectionStatus(), scanNetworks() (+3 more)

### Community 7 - "MedBox — UART Protocol Specification"
Cohesion: 0.17
Nodes (11): Command Reference, Commands (S3 → C3), Error Codes, ESP32-C3 Silicon Constraint, Extended Format: `DISPENSE,N,C`, MedBox — UART Protocol Specification, Message Format, Physical Layer (+3 more)

### Community 8 - "MedBox — System Architecture"
Cohesion: 0.18
Nodes (10): Communication, Controller Responsibilities, Dispenser Servo (SG90R — Modified 360° Continuous Rotation), Hatch Servo (Standard SG90 — 0° to 90°), HTTP / HTTPS Dual Compatibility, Key Design Principles, MedBox — System Architecture, Overview (+2 more)

### Community 9 - "js/app.js"
Cohesion: 0.33
Nodes (9): checkSchedulesNow(), loadDashboard(), loadDevices(), loadTodaySchedules(), renderDeviceHub(), renderScheduleCard(), showToast(), startScheduleChecker() (+1 more)

### Community 10 - "buzzer.cpp"
Cohesion: 0.39
Nodes (7): buzzerBeep(), buzzerInit(), buzzerPatternStart(), buzzerStop(), buzzerUpdate(), _soundOff(), _soundOn()

### Community 11 - "Arduino IDE Setup"
Cohesion: 0.22
Nodes (8): Arduino IDE Setup, Board Selection, Board Support, Communication, MedBox — Firmware, Required Libraries, Shared Protocol Header, Structure

### Community 12 - "js/medications.js"
Cohesion: 0.39
Nodes (7): deleteMedication(), getDaysArray(), getDaysBitmask(), handleSaveMedication(), loadMedications(), renderMedicationCard(), toggleMedication()

### Community 13 - "settings.js"
Cohesion: 0.28
Nodes (4): cleanTopicName(), handleSaveNotificationSettings(), handleTestNotification(), HW_COMMAND_LABELS

### Community 14 - "MedBox — Local Testing Guide"
Cohesion: 0.25
Nodes (7): 1. Running the Local Development Server, 2. Testing Frontend & Backend Features Locally, 3. Connecting Physical ESP32 Hardware to Localhost (Optional), A. Manual Test Alert & Hardware Trigger, B. Automatic Scheduled Notifications, MedBox — Local Testing Guide, Steps:

### Community 15 - "MedBox — Firmware State Machine"
Cohesion: 0.25
Nodes (7): Dispensing Sequence Detail, Error State, MedBox — Firmware State Machine, State Transition Diagram, States, Timing Constants, UART Command Spam Prevention

### Community 16 - "package.json"
Cohesion: 0.25
Nodes (7): @netlify/blobs, dependencies, @netlify/blobs, description, name, private, version

### Community 17 - "MedBox — Modular Smart Medication Dispenser"
Cohesion: 0.25
Nodes (7): Documentation, Firmware (Arduino IDE), MedBox — Modular Smart Medication Dispenser, Project Structure, Quick Start, Status, Web App

### Community 18 - "MedBox — Power Architecture"
Cohesion: 0.29
Nodes (6): Current Budget (Estimated), Key Rule, MedBox — Power Architecture, Power Supply Recommendation, Protection Considerations, Topology

### Community 19 - "functions/medications.js"
Cohesion: 0.48
Nodes (6): addToIndex(), getAllMedications(), getNextId(), { getStore, connectLambda }, handler(), removeFromIndex()

### Community 20 - "MedBox — Netlify Functions"
Cohesion: 0.33
Nodes (5): Database, Environment Variables (Netlify Dashboard), Functions, Local Development, MedBox — Netlify Functions

### Community 21 - "MedBox — Web Application"
Cohesion: 0.33
Nodes (5): Architecture, Deployment, Local Development, MedBox — Web Application, Structure

### Community 22 - "MedBox C3 Firmware — ESP32-C3 SuperMini"
Cohesion: 0.40
Nodes (4): Important Notes, MedBox C3 Firmware — ESP32-C3 SuperMini, Modules, Required Library

### Community 24 - "MedBox — Frontend"
Cohesion: 0.40
Nodes (4): Deployment, JS Modules, MedBox — Frontend, Pages

### Community 25 - "MedBox S3 Firmware — ESP32-S3 SuperMini"
Cohesion: 0.50
Nodes (3): MedBox S3 Firmware — ESP32-S3 SuperMini, Modules, Serial Debug Commands

## Knowledge Gaps
- **92 isolated node(s):** `Api`, `_triggeredSchedules`, `HW_COMMAND_LABELS`, `{ getStore, connectLambda }`, `{ getStore, connectLambda }` (+87 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **8 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `stateMachineUpdate()` connect `api_client.cpp` to `time_manager.cpp`, `buzzer.cpp`?**
  _High betweenness centrality (0.009) - this node is a cross-community bridge._
- **Why does `apiClientSyncSchedules()` connect `api_client.cpp` to `time_manager.cpp`?**
  _High betweenness centrality (0.007) - this node is a cross-community bridge._
- **Why does `wifiIsConnected()` connect `api_client.cpp` to `time_manager.cpp`, `wifi_manager.cpp`?**
  _High betweenness centrality (0.006) - this node is a cross-community bridge._
- **Are the 9 inferred relationships involving `stateMachineUpdate()` (e.g. with `apiClientLogDispense()` and `apiClientNotify()`) actually correct?**
  _`stateMachineUpdate()` has 9 INFERRED edges - model-reasoned connections that need verification._
- **Are the 6 inferred relationships involving `apiClientSyncSchedules()` (e.g. with `scheduleAdd()` and `scheduleClearAll()`) actually correct?**
  _`apiClientSyncSchedules()` has 6 INFERRED edges - model-reasoned connections that need verification._
- **Are the 4 inferred relationships involving `_processCommand()` (e.g. with `servoCloseHatch()` and `servoDispense()`) actually correct?**
  _`_processCommand()` has 4 INFERRED edges - model-reasoned connections that need verification._
- **What connects `Api`, `_triggeredSchedules`, `HW_COMMAND_LABELS` to the rest of the system?**
  _92 weakly-connected nodes found - possible documentation gaps or missing edges._