# Database Schema

## Status: NOT FINALIZED (database provider TBD)

## Proposed Tables

### PATIENT
| Field | Type | Notes |
|-------|------|-------|
| id | string/UUID | Primary key |
| name | string | Patient name |
| notification_topic | string | ntfy topic |
| created_at | timestamp | |

### MEDICATION
| Field | Type | Notes |
|-------|------|-------|
| id | string/UUID | Primary key |
| patient_id | string | FK → Patient |
| name | string | Medicine name |
| module_id | int | 1-3 |
| pills_per_dose | int | Tablets per intake |
| time | string | "HH:MM" |
| days_of_week | int | Bitmask (bit0=Sun) |
| start_date | date | Optional |
| end_date | date | Optional |
| enabled | bool | Active/disabled |
| active | bool | Soft delete |

### DISPENSE_LOG
| Field | Type | Notes |
|-------|------|-------|
| id | string/UUID | Primary key |
| medication_id | string | FK → Medication |
| module_id | int | Module that dispensed |
| scheduled_time | timestamp | When it was scheduled |
| actual_time | timestamp | When it actually dispensed |
| requested_quantity | int | Pills requested |
| status | string | dispensed/missed/error |
| error | string | Error message if any |

## Database Candidates
- Supabase (PostgreSQL)
- MongoDB Atlas
- PlanetScale / Turso (serverless SQL)
- Netlify Blobs (simple, limited)

## Related
- [[API Endpoints]]
- [[Web App Overview]]
