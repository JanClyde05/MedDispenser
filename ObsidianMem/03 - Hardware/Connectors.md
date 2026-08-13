# Connectors

## Status: NOT FINALIZED

## Purpose
Electrical + mechanical connection between main module and expansion modules.

## Signals Per Expansion Module
1. 5V (servo power)
2. GND
3. Dispenser servo signal
4. Hatch servo signal

Minimum: 4 pins. May include additional GND for reliability.

## Connector Candidates

| Connector | Pins | Pitch | Notes |
|-----------|------|-------|-------|
| JST XH | 4-5 | 2.5mm | Common, keyed, affordable |
| JST PH | 4-5 | 2.0mm | Smaller, common in hobby electronics |
| Pogo pins | 4+ | custom | No cable, direct contact, elegant |
| Magnetic pogo | 4+ | custom | Premium, easy alignment, costly |
| Board-to-board | 4+ | various | PCB stacking, compact |

## Considerations
- Must carry servo current (~1A peak per module)
- Should be mechanically robust (repeated connect/disconnect)
- Should be keyed or polarized to prevent miswiring
- Should align with mechanical locking mechanism

## Related
- [[Expansion Concept]]
- [[Power Architecture]]
