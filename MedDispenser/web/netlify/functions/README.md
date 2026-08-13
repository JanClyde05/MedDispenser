# MedBox — Netlify Functions

Serverless backend functions deployed alongside the frontend.

## Functions

| Function | Endpoint | Methods | Purpose |
|----------|----------|---------|---------|
| `medications.js` | `/api/medications` | GET, POST, PUT, DELETE | Medication CRUD |
| `schedules.js` | `/api/schedules` | GET | Schedule views |
| `dispense-log.js` | `/api/dispense-log` | GET, POST | Dispense event log |
| `sync.js` | `/api/sync` | GET | ESP32 schedule sync payload |
| `notify.js` | `/api/notify` | POST | Server-side ntfy publishing |

## Environment Variables (Netlify Dashboard)

| Variable | Description |
|----------|-------------|
| `NTFY_TOPIC` | ntfy topic name |
| `NTFY_SERVER` | ntfy server URL (default: `https://ntfy.sh`) |
| `NTFY_TOKEN` | Optional auth token for private ntfy topics |

## Database
Currently using in-memory stub stores. Replace with a real database client
when the DB choice is finalized. Each function has a TODO marker where the
DB integration should go.

## Local Development
```bash
npm install -g netlify-cli
netlify dev
```
This serves the frontend and functions locally on `http://localhost:8888`.
