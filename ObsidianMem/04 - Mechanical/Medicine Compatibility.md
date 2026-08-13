# Medicine Compatibility

## Status: NOT FINALIZED — requires physical testing

## Variables
- Diameter
- Thickness
- Shape (round, oval, caplet, capsule)
- Surface friction
- Tendency to bridge or jam
- Coating (smooth vs rough)

## Common Medicine Forms
```
Round tablet      ●     ~8-12mm diameter
Oval tablet       ◉     ~10-15mm long
Caplet            ▬     ~15-20mm long
Capsule          ◯━━◯   ~15-22mm long, ~5-8mm diameter
```

## Design Approach
1. Choose a target medication list from the client
2. Measure the largest tablet/capsule dimensions
3. Size the cylinder inner diameter with 2-3mm clearance
4. Size rotor pockets for the largest expected medicine + tolerance
5. Test with the actual medications

## Known Risks
- Capsules may orient unpredictably in cylinder
- Coated tablets may stick together in humidity
- Very small tablets may double-feed without proper pocket sizing
- Irregular shapes may not feed consistently

## Related
- [[Dispenser Mechanism]]
- [[Module Enclosure]]
