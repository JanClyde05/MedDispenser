# Dispenser Mechanism

## Status: FINALIZED (concept), NOT FINALIZED (geometry/dimensions)

## Concept
Cylindrical gravity-fed reservoir → spiral/rotary/flower-hopper metering mechanism → controlled release.

## How It Works
1. Medicine stored vertically in a cylinder
2. Gravity feeds tablets/capsules downward
3. Bottom section has a spiral/rotary rotor with pockets
4. Servo indexes the rotor by one pocket per dispense
5. One indexed movement = one dispensing event
6. Geometry physically prevents a second pill from following

## Key Design Goals
- One index step = one pill (calibrated per medication)
- Mechanical anti-double-feed through rotor pocket geometry
- No reliance on software timing for dose control

## Open Items
- Exact pocket count (determines step angle: 360°/N)
- Pocket dimensions (per tablet/capsule size)
- Spiral geometry (pitch, depth, wall clearance)
- Material (3D printed prototype)
- Step angle NOT FINALIZED — placeholder is 15° (24-pocket)

## Firmware Abstraction
The S3 issues `dispensePill()` / `DISPENSE,N`.
The physical angle is handled by the C3's `config.h` constants.

## Related
- [[Hatch Design]]
- [[Medicine Compatibility]]
- [[Servo Selection]]
