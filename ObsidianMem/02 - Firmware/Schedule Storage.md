# Schedule Storage

## Status: Implemented (firmware scaffold)

## Storage Method
ESP32 NVS (Non-Volatile Storage) via `Preferences` library.

## Data Structure
```cpp
struct MedSchedule {
  bool     active;
  uint8_t  moduleId;          // 1-3
  char     medicineName[32];
  uint8_t  pillsPerDose;
  uint8_t  hour;              // 0-23
  uint8_t  minute;            // 0-59
  uint8_t  daysOfWeek;        // Bitmask: bit0=Sun, bit6=Sat
  uint32_t startDate;         // Epoch or 0
  uint32_t endDate;           // Epoch or 0
  bool     enabled;
};
```

## Capacity
- `MAX_SCHEDULES = 16`
- NVS namespace: `"medbox"`

## Sync Flow
1. ESP32 calls `GET /api/sync` periodically
2. Receives JSON array of schedules
3. `scheduleClearAll()` + `scheduleAdd()` for each
4. `scheduleSaveToNVS()` persists to flash

## Day-of-Week Bitmask
| Bit | Day |
|-----|-----|
| 0 | Sunday |
| 1 | Monday |
| 2 | Tuesday |
| 3 | Wednesday |
| 4 | Thursday |
| 5 | Friday |
| 6 | Saturday |

Example: Mon-Fri = `0b0111110` = 62

## Related
- [[S3 Firmware Overview]]
- [[Device Sync]]
