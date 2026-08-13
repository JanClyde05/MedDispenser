# API Endpoints

## Base URL
Production: `https://your-medbox-app.netlify.app/api/`
Local dev: `http://localhost:8888/api/`

## Endpoints

### Medications
| Method | Path | Description |
|--------|------|-------------|
| GET | `/api/medications` | List all medications |
| POST | `/api/medications` | Create medication |
| PUT | `/api/medications/:id` | Update medication |
| DELETE | `/api/medications/:id` | Delete medication |

### Schedules
| Method | Path | Description |
|--------|------|-------------|
| GET | `/api/schedules` | Get all schedules |
| GET | `/api/schedules?today=true` | Get today's schedules |

### Dispense Log
| Method | Path | Description |
|--------|------|-------------|
| GET | `/api/dispense-log?limit=50` | Get recent logs |
| POST | `/api/dispense-log` | Record dispense event |

### Device Sync
| Method | Path | Description |
|--------|------|-------------|
| GET | `/api/sync` | Get ESP32 schedule payload |

### Notifications
| Method | Path | Description |
|--------|------|-------------|
| POST | `/api/notify` | Publish ntfy notification |

## Related
- [[Web App Overview]]
- [[Database Schema]]
- [[Device Sync]]
