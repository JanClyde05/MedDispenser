# MedBox — Graphify Knowledge Graph Setup & Usage

> **Last Updated:** 2026-08-22 — Initial setup of Graphify for AI codebase token management.

---

## What is Graphify?

[Graphify](https://github.com/closedloop-technologies/graphifyy) is an AI codebase analysis tool that uses **Tree-sitter** (local AST parsing) to create a structured knowledge graph of your project. It helps AI coding assistants (Claude, Cursor, Copilot, etc.) navigate your codebase efficiently by providing a dependency map instead of re-reading raw files — **drastically reducing token consumption**.

- **No cloud upload** — all parsing is done locally via deterministic AST analysis
- **No API cost** — `graphify update .` runs entirely offline
- **Supports C/C++, JavaScript, HTML, JSON, and 20+ languages**

---

## Installation

Graphify was installed via pip:

```bash
pip install graphifyy
```

**Executable location:**
```
C:\Users\Jan Clyde Talosig\AppData\Roaming\Python\Python314\Scripts\graphify.exe
```

**Agent integration installed via:**
```bash
graphify install
```

This created:
- Skill file: `C:\Users\Jan Clyde Talosig\.claude\skills\graphify\SKILL.md`
- Config: `C:\Users\Jan Clyde Talosig\.claude\CLAUDE.md`

---

## Output Directory

All graphify output is stored in `graphify-out/` at the project root:

```
MedDispenser/
└── graphify-out/
    ├── graph.json          # Machine-readable knowledge graph (315 KB)
    ├── graph.html          # Interactive visual explorer (open in browser)
    ├── GRAPH_REPORT.md     # Summary report (god nodes, communities, gaps)
    ├── manifest.json       # File manifest with metadata
    ├── .graphify_root      # Root marker
    ├── .graphify_labels.json
    └── cache/              # Internal cache (gitignored)
```

---

## Current Graph Statistics

| Metric | Value |
|--------|-------|
| **Files Analyzed** | 57 files |
| **Word Count** | ~94,616 words |
| **Nodes** | 337 |
| **Edges** | 463 |
| **Communities** | 36 (28 shown, 8 thin omitted) |
| **Extraction** | 89% EXTRACTED, 11% INFERRED, 0% AMBIGUOUS |
| **Built from Commit** | `d22ebba7` |

---

## God Nodes (Most Connected — Core Abstractions)

These are the most interconnected functions/files in your codebase. Changes here have the widest ripple effect:

| Rank | Node | Edges | File |
|------|------|-------|------|
| 1 | `stateMachineUpdate()` | 11 | `firmware/medbox_s3/state_machine.cpp` |
| 2 | `DEVICE_OPERATION_GUIDE.md` | 11 | `docs/DEVICE_OPERATION_GUIDE.md` |
| 3 | `apiClientSyncSchedules()` | 9 | `firmware/medbox_s3/api_client.cpp` |
| 4 | `_processCommand()` | 8 | `firmware/medbox_c3/uart_handler.cpp` |
| 5 | `_beginHttp()` | 8 | `firmware/medbox_s3/api_client.cpp` |
| 6 | `apiClientUpdate()` | 8 | `firmware/medbox_s3/api_client.cpp` |
| 7 | `_getTime()` | 8 | `firmware/medbox_s3/time_manager.cpp` |
| 8 | `wifiIsConnected()` | 8 | `firmware/medbox_s3/wifi_manager.cpp` |
| 9 | `ARCHITECTURE.md` | 8 | `docs/ARCHITECTURE.md` |
| 10 | `wifiManagerInit()` | 7 | `firmware/medbox_s3/wifi_manager.cpp` |

---

## Community Breakdown (Key Clusters)

| Community | Hub | Cohesion | Nodes | Description |
|-----------|-----|----------|-------|-------------|
| 0 | `api_client.cpp` | 0.08 | 28 | HTTP sync, command polling, heartbeat, notifications |
| 1 | `time_manager.cpp` | 0.16 | 19 | NTP sync, schedule evaluation, time functions |
| 2 | `wifi_manager.cpp` | 0.16 | 20 | Wi-Fi provisioning, captive portal, NVS credentials |
| 3 | `servo_controller.cpp` | 0.17 | 12 | Servo PWM control, dispense/hatch/home commands |
| 4 | `DEVICE_OPERATION_GUIDE.md` | 0.11 | 18 | Main documentation hub |
| 5 | `PIN_MAP.md` | 0.14 | 13 | Hardware wiring documentation |
| 6 | `data/app.js` | 0.25 | 11 | Wi-Fi captive portal frontend |
| 9 | `js/app.js` | 0.33 | 9 | Dashboard, schedule checker, device rendering |
| 10 | `buzzer.cpp` | 0.39 | 7 | Buzzer tone patterns |
| 12 | `js/medications.js` | 0.39 | 7 | Medication CRUD UI |

---

## Surprising Connections (Cross-Module Dependencies)

These are inferred connections the graph discovered that may not be immediately obvious:

| Source | → Target | Via |
|--------|----------|-----|
| `apiClientUpdate()` | `buzzerPatternStart()` | `api_client.cpp` → `buzzer.cpp` |
| `apiClientSyncSchedules()` | `scheduleAdd()` | `api_client.cpp` → `schedule_manager.cpp` |
| `apiClientSyncSchedules()` | `scheduleClearAll()` | `api_client.cpp` → `schedule_manager.cpp` |
| `apiClientSyncSchedules()` | `scheduleSaveToNVS()` | `api_client.cpp` → `schedule_manager.cpp` |
| `apiClientSyncSchedules()` | `timeSetEpoch()` | `api_client.cpp` → `time_manager.cpp` |

---

## How to Use Graphify

### Update the Graph (After Code Changes)
```bash
# From project root
graphify update .
```
This re-parses your codebase and rebuilds the knowledge graph. No API cost, runs 100% locally.

### Explore Visually
Open `graphify-out/graph.html` in your browser to interactively explore the knowledge graph with a force-directed node visualization.

### Query the Graph (Inside AI Assistants)
```
/graphify query "What breaks if I change stateMachineUpdate()?"
/graphify path "api_client.cpp" "servo_controller.cpp"
/graphify explain "apiClientSyncSchedules"
```

### Check Graph Freshness
```bash
git rev-parse HEAD
# Compare with "Built from commit" in GRAPH_REPORT.md
```

---

## Knowledge Gaps

The graph identified **92 isolated nodes** with ≤1 connection. These are likely:
- Frontend API wrapper functions (e.g., `Api.*` calls)
- Local variables and constants (e.g., `_triggeredSchedules`, `HW_COMMAND_LABELS`)
- These are normal for a mixed firmware + web project

---

## Integration Points

| AI Tool | Integration Method |
|---------|-------------------|
| Claude Code | Skill installed at `~/.claude/skills/graphify/SKILL.md` |
| Cursor | Add `graphify-out/GRAPH_REPORT.md` to context |
| Other Assistants | Point to `graphify-out/graph.json` as project map |

---

## File Locations

| File | Path |
|------|------|
| Graphify executable | `C:\Users\Jan Clyde Talosig\AppData\Roaming\Python\Python314\Scripts\graphify.exe` |
| Knowledge graph | `MedDispenser/graphify-out/graph.json` |
| Visual explorer | `MedDispenser/graphify-out/graph.html` |
| Graph report | `MedDispenser/graphify-out/GRAPH_REPORT.md` |
| Agent skill | `C:\Users\Jan Clyde Talosig\.claude\skills\graphify\SKILL.md` |
