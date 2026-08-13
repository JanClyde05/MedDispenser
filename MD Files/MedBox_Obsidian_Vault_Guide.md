# MedBox — Obsidian Vault Guide

> How to use the `ObsidianMem/` vault as the project's knowledge base.

---

## Opening the Vault

1. Open Obsidian
2. Click **Open folder as vault**
3. Select `d:\Antigravity\Projects\Med Dispenser\ObsidianMem`
4. The vault loads with the index at `00 - Index.md`

---

## Vault Structure

```
ObsidianMem/
├── 00 - Index.md                  Master index / Map of Content
├── 01 - Architecture/             System design (4 notes)
├── 02 - Firmware/                 Firmware design (5 notes)
├── 03 - Hardware/                 Component selection (4 notes)
├── 04 - Mechanical/               Physical design (4 notes)
├── 05 - Web & IoT/               Software/cloud (5 notes)
├── 06 - Decision Log/            Decisions + open questions (2 notes)
├── 07 - Build Log/               Prototyping journal (add as you go)
└── Templates/                    Reusable note templates (2 templates)
```

Sections are numbered for sort order. Each section focuses on one engineering domain.

---

## How to Use

### Daily Reference
- Open `00 - Index.md` to navigate to any topic
- Use `Ctrl+O` (Quick Open) to jump to any note by name
- Use `[[wikilinks]]` in notes to cross-reference related topics

### Recording Decisions
1. Open `Templates/Decision Template.md`
2. Copy the content
3. Create a new note in `06 - Decision Log/`
4. Paste and fill in the template
5. Add a link from `Finalized Decisions.md` or `Open Questions.md`

### Build Logging
1. Open `Templates/Build Log Entry Template.md`
2. Copy the content
3. Add a new entry in `07 - Build Log/Build Log.md` or create a separate note
4. Fill in measurements, issues, photos

### Updating Notes
- When a decision changes, update the relevant note and `Finalized Decisions.md`
- Mark status as FINALIZED, RECOMMENDED, OPTIONAL, UNDECIDED, or REJECTED
- Add date stamps to significant changes

---

## Navigation Tips

| Action | Shortcut |
|--------|----------|
| Quick Open | `Ctrl+O` |
| Search | `Ctrl+Shift+F` |
| Back | `Alt+←` |
| Graph View | `Ctrl+G` |
| Toggle sidebar | `Ctrl+B` |

### Graph View
Press `Ctrl+G` to see the full knowledge graph. Notes with more `[[wikilinks]]` appear more connected. This helps visualize which subsystems are most interrelated.

---

## Relationship to Other Project Files

| Location | Purpose |
|----------|---------|
| `ObsidianMem/` | Living knowledge base (this vault) |
| `MedDispenser/` | Source code (firmware + web) |
| `MedDispenser/docs/` | In-repo quick-reference docs |
| `MD Files/` | Standalone specification & reference documents |

The Obsidian vault is the **thinking and decision layer**. The `MedDispenser/docs/` folder is the **developer quick-reference layer**. The `MD Files/` folder holds **formal reference documents**.
