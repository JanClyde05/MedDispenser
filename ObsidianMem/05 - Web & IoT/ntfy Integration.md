# ntfy Integration

## Status: FINALIZED (concept), scaffold implemented

## Architecture
```
Medication event
     ↓
Backend (Netlify Function)
     ↓
ntfy API (server-side publish)
     ↓
User's phone (push notification)
```

## Security
- ntfy credentials stored as Netlify environment variables
- **Never** exposed in frontend JavaScript
- Publishing done server-side via `notify.js` function

## Environment Variables
| Variable | Description |
|----------|-------------|
| `NTFY_TOPIC` | Topic name (e.g., "medbox-user123") |
| `NTFY_SERVER` | Server URL (default: `https://ntfy.sh`) |
| `NTFY_TOKEN` | Optional auth token for private topics |

## Notification Format
```
Title: 💊 Medication Reminder
Body:
  Medicine: Metformin
  Dose: 1 tablet(s)
  Time: 07:00
  Module: 1

  Please take your scheduled medication.
```

## Trigger Points
1. Schedule time reached → reminder notification
2. Max retries exceeded → missed dose notification
3. System error → alert notification (future)

## Related
- [[Web App Overview]]
- [[State Machine]]
