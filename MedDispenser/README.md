# MedBox — Modular Smart Medication Dispenser

## Project Structure

```
MedDispenser/
│
├── firmware/
│   ├── medbox_s3/          ESP32-S3 — Main brain (Wi-Fi, schedule, sensors)
│   ├── medbox_c3/          ESP32-C3 — Servo controller
│   ├── shared/             Shared UART protocol definitions
│   └── README.md
│
├── web/
│   ├── frontend/           Static HTML/CSS/JS web application
│   ├── netlify/            Serverless backend functions
│   ├── netlify.toml        Deployment config
│   └── README.md
│
├── docs/
│   ├── ARCHITECTURE.md     System architecture overview
│   ├── UART_PROTOCOL.md    S3↔C3 command protocol
│   ├── PIN_MAP.md          GPIO assignments
│   ├── POWER.md            Power architecture & sizing
│   └── STATE_MACHINE.md    Firmware state machine
│
└── README.md               ← you are here
```

## Quick Start

### Firmware (Arduino IDE)
1. Install ESP32 board support in Arduino IDE
2. Install required libraries: `ESP32Servo`, `ArduinoJson`
3. Copy `firmware/shared/protocol.h` into both sketch folders
4. Open `firmware/medbox_s3/medbox_s3.ino` — select ESP32-S3 board — upload
5. Open `firmware/medbox_c3/medbox_c3.ino` — select ESP32-C3 board — upload

### Web App
1. Install [Netlify CLI](https://docs.netlify.com/cli/get-started/): `npm i -g netlify-cli`
2. `cd web && netlify dev` — runs frontend + functions locally
3. Deploy: `netlify deploy --prod`

## Documentation
- Full specification: `../MD Files/MedBox_Final_Project_Specification.md`
- Obsidian knowledge base: `../ObsidianMem/`
- Architecture docs: `docs/`

## Status
🟡 Scaffold — code structure and stubs created, not yet compiled/tested.
