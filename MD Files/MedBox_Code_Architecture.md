# MedBox — Code Architecture Reference

> This document describes the code structure and organization of the MedBox project source code located in `MedDispenser/`.

---

## Directory Map

```
MedDispenser/
│
├── firmware/
│   ├── medbox_s3/                    ESP32-S3 SuperMini — Main Brain
│   │   ├── medbox_s3.ino             Main entry, setup/loop, debug CLI
│   │   ├── config.h                  Pin definitions, timing, constants
│   │   ├── wifi_manager.h / .cpp     Wi-Fi connection & reconnect
│   │   ├── time_manager.h / .cpp     NTP sync, time utilities
│   │   ├── schedule_manager.h / .cpp Local schedule storage (NVS)
│   │   ├── state_machine.h / .cpp    Medication event state machine
│   │   ├── proximity.h / .cpp        Proximity sensor driver
│   │   ├── buzzer.h / .cpp           Non-blocking buzzer patterns
│   │   ├── uart_command.h / .cpp     UART command interface to C3
│   │   ├── api_client.h / .cpp       Backend sync (stub)
│   │   ├── protocol.h                ← copy from shared/
│   │   └── README.md
│   │
│   ├── medbox_c3/                    ESP32-C3 SuperMini — Servo Controller
│   │   ├── medbox_c3.ino             Main entry, setup/loop
│   │   ├── config.h                  Servo pin map, angle constants
│   │   ├── uart_handler.h / .cpp     UART command parser & dispatch
│   │   ├── servo_controller.h / .cpp Per-module servo abstraction
│   │   ├── protocol.h                ← copy from shared/
│   │   └── README.md
│   │
│   ├── shared/
│   │   └── protocol.h                Canonical UART protocol definitions
│   │
│   └── README.md                     Firmware overview
│
├── web/
│   ├── frontend/                     Static HTML/CSS/JS
│   │   ├── index.html                Dashboard
│   │   ├── medications.html          Medication CRUD
│   │   ├── history.html              Dispense history
│   │   ├── settings.html             System settings
│   │   ├── css/style.css             Design system
│   │   ├── js/api.js                 Backend API client
│   │   ├── js/app.js                 Core app logic, toasts
│   │   ├── js/medications.js         Medication form handling
│   │   ├── js/schedule.js            Schedule display utilities
│   │   ├── js/history.js             History table rendering
│   │   └── README.md
│   │
│   ├── netlify/functions/            Serverless backend
│   │   ├── medications.js            Medication CRUD API
│   │   ├── schedules.js              Schedule views API
│   │   ├── dispense-log.js           Dispense event log API
│   │   ├── sync.js                   ESP32 schedule sync API
│   │   ├── notify.js                 ntfy publisher (server-side)
│   │   └── README.md
│   │
│   ├── netlify.toml                  Netlify config + API redirects
│   └── README.md                     Web stack overview
│
├── docs/
│   ├── ARCHITECTURE.md               System architecture
│   ├── UART_PROTOCOL.md              S3↔C3 command protocol
│   ├── PIN_MAP.md                    GPIO assignments
│   ├── POWER.md                      Power architecture
│   └── STATE_MACHINE.md              State machine diagram
│
└── README.md                         Project root
```

---

## Design Decisions

### Firmware Modularity
Each firmware subsystem is a `.h/.cpp` pair with `init()` and `update()` functions. The main `.ino` calls all `init()` in `setup()` and all `update()` in `loop()`. This keeps the loop non-blocking and each subsystem testable independently.

### Shared Protocol
`shared/protocol.h` is the single source of truth for the UART command format. It must be **copied** into both sketch folders for Arduino IDE compatibility. Always edit the canonical version first.

### Web Frontend
Plain HTML/CSS/JS with no build step. This matches the project's embedded-systems focus and avoids unnecessary toolchain complexity. API calls use `fetch()` wrappers in `api.js`.

### Backend Functions
Netlify Functions provide a serverless API. Each function handles one resource. Currently using in-memory stubs — the database integration point is marked with `// TODO` in each function.

### In-Repo Docs
Quick-reference docs in `docs/` are derived from the main specification but optimized for day-to-day development use (tables, diagrams, pin maps).

---

## Companion Resources

| Resource | Location |
|----------|----------|
| Full specification | `../MD Files/MedBox_Final_Project_Specification.md` |
| Obsidian knowledge base | `../ObsidianMem/` |
| This document | `../MD Files/MedBox_Code_Architecture.md` |
