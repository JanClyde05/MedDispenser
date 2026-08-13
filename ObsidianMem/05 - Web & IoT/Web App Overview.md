# Web App Overview

## Status: Scaffold created

## Architecture
```
Browser → Netlify CDN → Static HTML/CSS/JS
                ↓
         /api/* → Netlify Functions → Database (TBD)
                                ↓
                           ntfy (notifications)
```

## Frontend Pages
| Page | Purpose |
|------|---------|
| Dashboard (`index.html`) | Today's schedule, system status |
| Medications (`medications.html`) | CRUD for medications + schedules |
| History (`history.html`) | Dispense event log |
| Settings (`settings.html`) | Device, notifications, modules |

## Backend Functions
| Function | Endpoint | Purpose |
|----------|----------|---------|
| `medications.js` | `/api/medications` | Medication CRUD |
| `schedules.js` | `/api/schedules` | Schedule views |
| `dispense-log.js` | `/api/dispense-log` | Event logging |
| `sync.js` | `/api/sync` | ESP32 schedule sync |
| `notify.js` | `/api/notify` | Server-side ntfy publishing |

## Database
NOT FINALIZED — currently using in-memory stubs.

## Related
- [[API Endpoints]]
- [[Database Schema]]
- [[ntfy Integration]]
- [[Device Sync]]
