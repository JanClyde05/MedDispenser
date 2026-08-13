# Device Sync

## Status: Scaffold implemented

## Sync Flow
```
ESP32-S3 (periodic, every 5 min)
     ↓
GET /api/sync
     ↓
Backend returns JSON schedule array
     ↓
ESP32 parses → clears old → adds new → saves to NVS
```

## Sync Payload Format (Backend → ESP32)
```json
{
  "serverTime": 1723593600,
  "schedules": [
    {
      "moduleId": 1,
      "medicineName": "Metformin",
      "pillsPerDose": 1,
      "hour": 7,
      "minute": 0,
      "daysOfWeek": 62,
      "startDate": 0,
      "endDate": 0,
      "enabled": true
    }
  ]
}
```

## Offline Behavior
- If sync fails, device continues using last saved schedule
- Schedules are persisted in NVS (flash memory)
- Device does **not** depend on Internet at dispensing time

## Sync Interval
- Default: `API_SYNC_INTERVAL_MS = 300000` (5 minutes)
- Can be adjusted in `config.h`

## Related
- [[Schedule Storage]]
- [[API Endpoints]]
- [[S3 Firmware Overview]]
