# Servo Selection

## Status: NOT FINALIZED

## Requirements
- 5V operation (from servo power rail)
- Sufficient torque for dispenser indexing and hatch operation
- Small form factor (compact enclosure)
- Available and affordable for prototyping

## Candidates

| Model | Torque | Speed | Size | Price | Notes |
|-------|--------|-------|------|-------|-------|
| SG90 | 1.8 kg·cm | 0.1s/60° | 22×11×23 mm | ~$1-2 | Common, may lack torque |
| MG90S | 2.2 kg·cm | 0.1s/60° | 22×12×30 mm | ~$3-4 | Metal gears, more reliable |
| SG92R | 2.5 kg·cm | 0.1s/60° | 23×12×29 mm | ~$2-3 | Upgraded SG90 |
| MG996R | 13 kg·cm | 0.15s/60° | 40×20×43 mm | ~$5-7 | Large — may not fit |

## Considerations
- Dispenser servo needs consistent indexing — metal gears recommended
- Hatch servo needs moderate torque — SG90 class may suffice
- Current draw varies significantly between models
- SG90-class: ~500 mA peak, MG996R: ~900 mA peak

## Recommendation
Start prototyping with **MG90S** (metal gear, compact, sufficient torque).
Validate with actual dispenser mechanism before committing.

## Related
- [[Power Architecture]]
- [[GPIO Map]]
- [[Dispenser Mechanism]]
