[MedBox_Final_Project_Specification.md](https://github.com/user-attachments/files/31040068/MedBox_Final_Project_Specification.md)
# Modular Smart Medication Dispenser / MedBox
## Detailed Project Specification, Architecture, Mechanical Concept, Electronics, Firmware, IoT, and Expansion Plan

> **Project status:** Consolidated and finalized based on the design discussion so far.
>
> **Scope:** Commissioned prototype for a Wi-Fi-enabled medication reminder and automatic dispensing box intended for a client who has scheduled maintenance medicines.
>
> **Important scope note:** This document describes an engineering prototype architecture. It should not be represented as a medically certified dosing device. Reliable dispensing must be validated with the specific tablet/capsule types the client intends to use.

---

# 1. Project Overview

The MedBox is a modular medication reminder and dispensing system.

A user/client will use a web application to configure:

- medicine/type of medicine
- which medicine module contains it
- date
- time
- number of tablets/capsules per intake
- recurring schedule

At the configured medication time:

1. The system reaches the scheduled intake time.
2. The MedBox activates its local buzzer.
3. A push notification is sent to the user's phone through **ntfy**.
4. The system waits for the medicine taker to be in the vicinity.
5. A **proximity/IR sensor** on the main module detects the user.
6. The appropriate medication module opens its hatch.
7. The dispensing mechanism releases the configured amount of medicine.
8. The user takes the medicine from the pickup/hatch area.
9. The hatch closes.
10. The event is recorded as completed.

If the taker is not nearby, the system does **not** open the hatch. The reminder can repeat after the configured reminder interval; the current concept is **5 minutes**.

---

# 2. Core Product Concept

The MedBox is designed around one key principle:

> **One medication type = one medication module.**

The **main module is also a complete medication module**. It is not merely an electronics enclosure.

This means:

- The main module contains the first medication reservoir and dispensing mechanism.
- The main module also contains the system electronics and sensors.
- Optional expansion modules can be attached when additional medicine types are needed.
- Every medication module uses the same mechanical dispensing concept with **two servo motors**:
  - one servo for medicine dispensing
  - one servo for hatch opening/closing

Examples:

### One medication

```text
[ MAIN MODULE ]
Medicine A
```

### Two medications

```text
[ MAIN MODULE ] [ EXPANSION 2 ]
Medicine A       Medicine B
```

### Three medications

```text
[ MAIN MODULE ] [ EXPANSION 2 ] [ EXPANSION 3 ]
Medicine A       Medicine B       Medicine C
```

The system therefore does not force the user to install unnecessary expansion modules.

---

# 3. Finalized System Architecture

```text
                         ┌─────────────────────────┐
                         │       WEB APP           │
                         │   Netlify Hosted UI     │
                         │                         │
                         │ Medicine                 │
                         │ Module                   │
                         │ Dose                     │
                         │ Date                     │
                         │ Time                     │
                         │ Schedule                 │
                         └────────────┬────────────┘
                                      │
                                    HTTPS
                                      │
                                      ▼
                         ┌─────────────────────────┐
                         │        BACKEND          │
                         │                         │
                         │ Netlify Functions       │
                         │ Database                 │
                         │ Schedule/API logic       │
                         └───────┬─────────┬───────┘
                                 │         │
                                 │         └──────────► ntfy
                                 │                       │
                                 │                       ▼
                                 │                     Phone
                                 │
                              HTTPS sync
                                 │
                                 ▼
              ┌───────────────────────────────────────────┐
              │             MAIN MED MODULE              │
              │                                           │
              │ ESP32-S3 SuperMini                        │
              │   Main IoT / system brain                 │
              │                                           │
              │ ESP32-C3 SuperMini                        │
              │   Dedicated servo controller              │
              │                                           │
              │ Proximity / IR sensor                     │
              │ Buzzer                                    │
              │ USB-C power input                         │
              │                                           │
              │ Medicine reservoir                        │
              │ Spiral/rotary dispenser                   │
              │ Dispenser Servo                           │
              │ Hatch Servo                               │
              └──────────────────┬────────────────────────┘
                                 │
                                 │ module expansion
                                 ▼
                    ┌──────────────────────────┐
                    │   EXPANSION MODULE #2    │
                    │                          │
                    │ Medicine reservoir       │
                    │ Spiral/rotary dispenser  │
                    │ Dispenser Servo          │
                    │ Hatch Servo              │
                    └────────────┬─────────────┘
                                 │
                                 ▼
                    ┌──────────────────────────┐
                    │   EXPANSION MODULE #3    │
                    │                          │
                    │ Medicine reservoir       │
                    │ Spiral/rotary dispenser  │
                    │ Dispenser Servo          │
                    │ Hatch Servo              │
                    └──────────────────────────┘
```

---

# 4. Controller Responsibilities

## 4.1 ESP32-S3 SuperMini — Main Brain

The ESP32-S3 is the primary controller.

Responsibilities:

- Wi-Fi
- communication with the web/backend system
- medication schedule synchronization
- local schedule storage
- time management
- NTP synchronization when available
- optional RTC support
- medication event state machine
- user-presence detection
- buzzer control
- communication with the ESP32-C3
- ntfy-related backend/device integration as applicable
- error handling
- system status
- logging/synchronization

The S3 is the system's "brain."

It understands concepts such as:

```text
Medicine A
Module 1
07:00 AM
1 tablet
Monday-Friday
```

The S3 does not need to directly generate every servo movement itself.

---

## 4.2 ESP32-C3 SuperMini — Dedicated Servo Controller

The ESP32-C3 is intentionally retained as a separate controller.

Its role is specifically actuator/servo control.

Responsibilities:

- control the dispenser servo for each module
- control the hatch servo for each module
- receive commands from the ESP32-S3
- execute servo movement
- optionally report command reception/completion

Conceptually:

```text
ESP32-S3
   │
   │ UART / command interface
   ▼
ESP32-C3
   │
   ├── Main module dispenser servo
   ├── Main module hatch servo
   ├── Expansion #2 dispenser servo
   ├── Expansion #2 hatch servo
   ├── Expansion #3 dispenser servo
   └── Expansion #3 hatch servo
```

The C3 does not need to know about:

- medicine names
- users
- schedules
- Wi-Fi APIs
- ntfy messages
- database records

It acts as the servo/actuator layer.

---

# 5. Why Two ESP32s Are Used

The chosen architecture intentionally separates:

### High-level logic

Handled by ESP32-S3:

```text
Internet
Wi-Fi
Schedule
Medicine
Notifications
User presence
System state
```

### Low-level actuator control

Handled by ESP32-C3:

```text
Servo
Servo
Servo
Servo
Servo
Servo
```

This keeps the software responsibilities clean and allows the compact C3 to act as a dedicated motion controller while the S3 handles the IoT workload.

---

# 6. Medication Module Architecture

Every medication module has the same mechanical concept.

## 6.1 Main Module

The main module contains:

- ESP32-S3 SuperMini
- ESP32-C3 SuperMini
- USB-C input
- power distribution
- proximity/IR sensor
- buzzer
- first medicine reservoir
- first dispensing mechanism
- first dispenser servo
- first hatch servo

Thus, the main module is both:

1. **Medication Module #1**
2. **System Controller**

---

## 6.2 Expansion Module

Each expansion module contains:

- cylindrical medicine storage container
- mechanical gravity-feed path
- spiral/rotary metering dispenser
- dispenser servo
- hatch servo

Expansion modules intentionally avoid adding another microcontroller unless the design later proves that local intelligence is necessary.

The base design is:

```text
Expansion module =
mechanics + 2 servos
```

---

# 7. Main Module vs. Expansion Module

## Main Module

```text
┌────────────────────────────────────────┐
│              MAIN MODULE               │
│                                        │
│ ESP32-S3 SuperMini                     │
│ ESP32-C3 SuperMini                     │
│ Proximity / IR sensor                  │
│ Buzzer                                 │
│ USB-C power                            │
│                                        │
│ Medicine reservoir                     │
│ Spiral/rotary dispenser                │
│ Dispenser Servo                        │
│ Hatch Servo                            │
└────────────────────────────────────────┘
```

## Expansion Module

```text
┌─────────────────────────────┐
│      EXPANSION MODULE       │
│                             │
│ Medicine reservoir          │
│ Spiral/rotary dispenser     │
│ Dispenser Servo             │
│ Hatch Servo                 │
└─────────────────────────────┘
```

---

# 8. Power Architecture

## 8.1 Main Power Source

The intended power input is a **USB-C receptacle** on the main module.

The whole system is designed around a standard 5 V USB-C phone charger/power source.

The design should provide a dedicated power path for:

- ESP32-S3
- ESP32-C3
- servos
- other low-current peripherals

The important principle is:

> **The servos are powered from the main 5 V supply/servo power rail, not through the ESP32's 5 V pin as a servo power source.**

---

## 8.2 Recommended power topology

```text
                   USB-C INPUT
                     5 V
                      │
                Power Protection
                      │
                5 V Main Bus
                      │
          ┌───────────┴───────────┐
          │                       │
          ▼                       ▼
      ESP32-S3                SERVO POWER
      ESP32-C3                    │
      sensors                     │
      buzzer               ┌──────┼──────┐
                            ▼      ▼      ▼
                          Servo  Servo  Servo...
```

All grounds should share a common reference.

```text
ESP32 GND ───────────── Servo GND
```

---

## 8.3 USB-C power considerations

The charger should be treated as a **known power source**, not assumed to always provide a particular current simply because it uses USB-C.

For the prototype:

- use a known 5 V charger/source
- preferably use a supply with enough current headroom for the maximum number of simultaneous servo movements
- do not assume every USB-C charger automatically provides 3 A
- size wiring and traces for servo current
- consider adding a fuse or current-limiting protection to the servo branch
- consider bulk capacitance near the servo power rail to help with transient servo current demand

If the final system can contain several simultaneously active servos, use a supply with comfortable current margin.

---

# 9. USB-C Protection

The main USB-C receptacle requires appropriate protection.

The original plan discussed using diodes.

A conventional diode can be used for reverse-polarity protection where appropriate, but its forward-voltage drop should be considered because the system runs from a nominal 5 V rail.

For a more optimized PCB design, alternatives such as a MOSFET-based reverse-polarity protection circuit can be evaluated because they can reduce voltage drop.

Protection should be designed according to the actual fault being prevented.

Potential protection areas:

- reverse polarity
- accidental power injection
- overcurrent
- ESD/transient events
- hot-plugging

---

# 10. Important USB-C Rule

The design should use **one intentional external power input** for the system.

Avoid having multiple user-accessible power inputs that could allow two independent chargers to be connected to the same power rails simultaneously.

Preferred:

```text
ONE USB-C INPUT
      │
      ├── S3
      ├── C3
      └── Servo power
```

---

# 11. Servo Power and Servo Control

Servo control and servo power are separate concepts.

### Control

The ESP32-C3 generates the servo control signal.

### Power

The servo receives power directly from the dedicated 5 V servo rail.

Conceptually:

```text
ESP32-C3 GPIO ───── Signal ─────► Servo
5V SERVO BUS ────── 5V ─────────► Servo
Common GND ──────── GND ────────► Servo
```

This prevents the servo current from being routed through the microcontroller's logic power path.

---

# 12. Servo Count

Each medication module uses:

### Servo #1 — Dispenser

Controls the rotary/spiral medicine metering mechanism.

### Servo #2 — Hatch

Opens/closes access to the medication pickup area.

For three total medication modules:

```text
Module 1 = 2 servos
Module 2 = 2 servos
Module 3 = 2 servos

Total = 6 servos
```

---

# 13. Mechanical Dispensing Concept

The original screw/auger concept was reconsidered.

The finalized concept is a:

> **Cylindrical gravity-fed medicine reservoir + rotary/spiral metering dispenser**

The intended behavior is similar to a controlled egg-like or pocketed arrangement, but adapted for tablets/capsules.

---

# 14. Cylindrical Medicine Container

Medicine is stored vertically in a cylinder.

Conceptually:

```text
        CYLINDER
   ┌──────────────────┐
   │ ○ ○ ○ ○ ○ ○ ○ ○ │
   │ ○ ○ ○ ○ ○ ○ ○ ○ │
   │ ○ ○ ○ ○ ○ ○ ○ ○ │
   │ ○ ○ ○ ○ ○ ○ ○ ○ │
   │        ↓         │
   │        ↓         │
   └────────┬─────────┘
            ▼
      Metering section
```

Gravity feeds medicine toward the bottom.

This avoids the need for an active auger throughout the entire storage volume.

---

# 15. Spiral / Flower-Hopper Metering Mechanism

The bottom section uses a spiral/rotary structure that meters the medicine.

Conceptually:

```text
      STORAGE CYLINDER
             │
             ▼
      ┌─────────────┐
      │ FLOW /      │
      │ HOPPER      │
      │             │
      │  SPIRAL     │
      │    ↘        │
      │      ↘      │
      │        ↘    │
      └────────┬────┘
               ▼
             DROP
```

The spiral geometry is intended to:

- control the release point
- separate adjacent tablets/capsules
- prevent uncontrolled dumping
- meter a predictable amount per indexed servo movement
- provide a mechanical anti-double-feed function

---

# 16. Mechanical Anti-Double-Feed Principle

The preferred design is to let the geometry itself prevent multiple tablets from entering the dispensing point.

The design should attempt to make:

```text
One indexed movement → One intended dispensing event
```

rather than relying entirely on software timing.

The rotary section should act as both:

1. metering mechanism
2. physical gate against a second pill following immediately

---

# 17. Servo Indexing

The exact angular step is **not finalized yet**.

An early idea was:

```text
5° movement → one medicine
```

but this should not be assumed until the actual mechanical geometry is built and calibrated.

A better implementation is:

```text
Position 0 → closed/home
Position 1 → next dispensing pocket
Position 2 → next dispensing pocket
Position 3 → next dispensing pocket
```

The servo step should be derived from the physical pocket/spiral design.

If a rotor has equally spaced pockets:

```text
servo angle per pocket = 360° / number_of_pockets
```

For example, a 24-position rotor would provide:

```text
360° / 24 = 15° per position
```

The exact number of positions should be determined experimentally.

---

# 18. Firmware Abstraction for Dispensing

The firmware should not hard-code the idea that "5° means one pill."

Instead, high-level firmware should issue:

```cpp
dispensePill();
```

or:

```text
DISPENSE MODULE 1
```

The calibrated dispenser driver handles the required servo movement.

This allows the mechanical design to be refined without rewriting the entire software architecture.

---

# 19. Spring / Feed Concept

A spring-loaded medicine feed was considered, but the design has moved toward a **gravity-fed cylinder**.

Therefore the primary planned concept is:

```text
Vertical cylinder
      ↓
Gravity
      ↓
Spiral/rotary metering hopper
      ↓
Controlled release
```

A spring may still be used later if testing shows that certain pills/capsules fail to feed reliably, but it is not the current primary design.

---

# 20. Medicine Compatibility

The dispensing mechanism must be tested using the actual medication types expected by the client.

Different medicines have different:

- diameters
- thicknesses
- shapes
- surface friction
- capsule/tablet geometry
- tendency to bridge or jam

Examples:

```text
Round tablet      ●
Oval tablet       ◉
Caplet             ▬
Capsule           ◯━━◯
Irregular tablet  custom shape
```

The mechanism should therefore be designed around a specified acceptable size range or a defined medicine list.

Do not assume one geometry will automatically work reliably for every medicine.

---

# 21. Proximity / IR Sensor

A critical clarification:

> **The IR/proximity sensor is NOT a pill-drop sensor.**

Its only role is:

> **Detect whether the medicine taker is in the vicinity of the MedBox.**

The sensor is used to enforce the hatch-opening condition.

It is not used to verify whether a tablet was dispensed.

---

# 22. User-Presence Logic

At medication time:

```text
Medication time reached
        ↓
Reminder
        ↓
Check proximity sensor
        ↓
User nearby?
      /     \
    NO       YES
    │          │
    ▼          ▼
Keep        Open hatch
reminding      │
               ▼
           Dispense medicine
               │
               ▼
           User takes medicine
```

The hatch should not open simply because the schedule has reached its time.

The proximity condition must be satisfied first.

---

# 23. No Pill-Drop IR Confirmation

The project does **not** require an additional IR sensor in the dispensing chute to prove that a pill fell.

The sequence is therefore not:

```text
Dispense
↓
Pill sensor
↓
Confirm pill
```

Instead, the mechanism is expected to dispense according to its calibrated mechanical design.

Possible future enhancements may include weight sensing, optical confirmation, or other verification, but these are **not part of the current finalized base design**.

---

# 24. Hatch

Every medication module has a hatch controlled by its own hatch servo.

Purpose:

- restrict access to the medicine
- keep the medicine compartment closed when not being taken
- help reduce unnecessary exposure/contamination
- only expose the pickup area when the user is present

The main module and every expansion module use the same hatch concept.

---

# 25. Recommended Hatch State Handling

Although a pill confirmation sensor is not required, a future or preferred reliability enhancement is a hatch-position sensor.

Possible implementations:

- magnetic reed switch
- Hall-effect sensor
- limit switch

The purpose is to tell the controller whether the hatch is physically open or closed.

Conceptually:

```text
Open command
   ↓
Hatch servo
   ↓
Hatch position sensor
   ↓
OPEN confirmed
```

This is a mechanical-state check, not a pill-detection check.

---

# 26. Buzzer

The main module contains the local notification buzzer.

At the configured medication time:

```text
Buzzer ON
```

The buzzer can continue or repeat according to the firmware's reminder policy.

A future UI can allow settings such as:

- reminder sound duration
- repeat interval
- number of reminders
- quiet mode

The initial requirement is:

> **Reminder repeats after 5 minutes if the medication has not yet been taken/handled according to the system flow.**

---

# 27. Notification System — ntfy

The project intends to use **ntfy** for phone push notifications.

ntfy can be used as a notification channel so the user receives a message when medication is due.

Conceptually:

```text
Schedule reaches medication time
             ↓
        Backend / system
             ↓
          ntfy publish
             ↓
       User's phone
             ↓
       Push notification
```

Example notification:

```text
Medication Reminder

Medicine: Medicine A
Dose: 1 tablet
Time: 7:00 AM
Module: 1

Please take your scheduled medication.
```

---

# 28. ntfy Security

Do not expose a privileged ntfy publishing credential in browser-side JavaScript.

Preferred architecture:

```text
Web Browser
    ↓
Netlify Function / Backend
    ↓
ntfy API
    ↓
Phone
```

Credentials should remain server-side as environment/secrets.

The browser should not receive a token that lets anyone publish arbitrary messages to the private notification channel.

---

# 29. Netlify Architecture

Netlify is used primarily for the web/software side.

Recommended structure:

```text
Netlify
├── Frontend / Web UI
├── Server-side Functions
└── Database
```

The web application is used to configure medications and schedules.

The backend handles:

- storing medication data
- storing schedules
- serving API endpoints
- synchronizing device data
- triggering push notifications where applicable

---

# 30. WebSocket Decision

A WebSocket architecture is **not required for the base system**.

A simpler architecture is:

```text
Web UI
  ↓
HTTPS API
  ↓
Backend/database
  ↓
ESP32 schedule synchronization
```

The ESP32 should maintain its own local schedule after synchronization.

This is preferable to requiring a continuously active WebSocket connection for medication execution.

---

# 31. Local Device Operation

The ESP32 should not depend on the Internet at the exact medication time.

Recommended design:

```text
Internet available
      ↓
ESP32 synchronizes schedule
      ↓
Schedule stored locally
      ↓
Internet temporarily unavailable
      ↓
ESP32 still follows the saved schedule
```

This prevents a temporary Wi-Fi failure from automatically disabling the basic medication schedule.

---

# 32. Timekeeping

The preferred time architecture is:

### Primary

NTP synchronization while connected to Wi-Fi.

### Recommended reliability enhancement

Use an RTC such as a DS3231 for persistent local timekeeping.

Conceptually:

```text
Internet
   ↓
NTP sync
   ↓
ESP32 time
   +
RTC backup
```

The device should not rely exclusively on `millis()` for real calendar scheduling.

---

# 33. Medication Schedule Data

The web interface should allow at minimum:

- medication name/type
- module number
- dosage/number of pills
- time
- date
- recurring days
- start date
- end date
- enabled/disabled state

Example:

```text
Medicine:
Metformin

Module:
1

Dose:
1 tablet

Time:
07:00 AM

Days:
Monday
Tuesday
Wednesday
Thursday
Friday

Start:
08/15/2026

End:
08/22/2026

Enabled:
YES
```

---

# 34. Suggested Web UI

A medication entry screen can contain:

```text
MEDICATION

Medicine name:
[________________________]

Module:
[ Module 1 ▼ ]

Dose:
[ 1 ▼ ] tablet(s)

Time:
[ 07:00 AM ]

Days:
[ Mon ] [ Tue ] [ Wed ] [ Thu ] [ Fri ] [ Sat ] [ Sun ]

Start date:
[ MM/DD/YYYY ]

End date:
[ MM/DD/YYYY ]

[ SAVE MEDICATION ]
```

The dashboard can then show:

```text
TODAY'S MEDICATIONS

07:00 AM
Medicine A
1 tablet
Module 1

12:00 PM
Medicine B
2 tablets
Module 2

08:00 PM
Medicine C
1 tablet
Module 3
```

---

# 35. Suggested Database Structure

A practical starting structure is:

## PATIENT

```text
id
name
notification_topic
created_at
```

## MEDICATION

```text
id
patient_id
name
dosage
module_id
pills_per_dose
active
```

## SCHEDULE

```text
id
medication_id
time
days_of_week
start_date
end_date
enabled
```

## DISPENSE LOG

```text
id
medication_id
module_id
scheduled_time
actual_time
requested_quantity
status
error
```

The exact schema can be changed when implementing the application.

---

# 36. Example Event Log

The website can display:

```text
MEDICATION HISTORY

08/13 07:00
Medicine A
1 tablet
DISPENSED / COMPLETED

08/13 12:00
Medicine B
2 tablets
DISPENSED / COMPLETED

08/13 20:00
Medicine C
1 tablet
REMINDER PENDING
```

The base project can log the system's commanded event and completion state.

More advanced verification can be added later.

---

# 37. Medication Event State Machine

The firmware should be designed around explicit states rather than one large collection of unrelated `if` statements.

Recommended states:

```cpp
enum SystemState {
    IDLE,
    REMINDER,
    WAITING_FOR_USER,
    HATCH_OPENING,
    DISPENSING,
    USER_TAKING_MEDICINE,
    HATCH_CLOSING,
    COMPLETED,
    ERROR_STATE
};
```

Conceptual flow:

```text
IDLE
  ↓
REMINDER
  ↓
WAITING_FOR_USER
  ↓
HATCH_OPENING
  ↓
DISPENSING
  ↓
USER_TAKING_MEDICINE
  ↓
HATCH_CLOSING
  ↓
COMPLETED
  ↓
IDLE
```

---

# 38. Complete Medication Sequence

The finalized base sequence is:

```text
1. Scheduled medication time reached
           ↓
2. Activate buzzer
           ↓
3. Trigger ntfy notification
           ↓
4. Wait/check for user proximity
           ↓
5. If user is not present:
       continue reminder policy
       wait 5 minutes
       check again
           ↓
6. If user is present:
       open the appropriate module hatch
           ↓
7. Dispense the configured medication amount
           ↓
8. Allow user to retrieve medication
           ↓
9. Close hatch
           ↓
10. Record/log event as completed
```

---

# 39. Proximity Condition

The system should avoid the following behavior:

```text
07:00
↓
Hatch immediately opens
↓
Nobody present
```

Instead:

```text
07:00
↓
Reminder
↓
Check proximity
↓
No person
↓
Keep hatch closed
```

Then:

```text
Person enters vicinity
↓
Proximity detected
↓
Hatch permitted to open
```

This is part of the project's cleanliness/access-control concept.

---

# 40. Reminder Retry

Initial requirement:

> If the medication has not yet been taken/handled, the reminder repeats after approximately 5 minutes.

Conceptually:

```text
07:00
Reminder

07:00–07:05
Wait for user

07:05
Reminder again if still pending
```

Exact reminder persistence, maximum retries, and escalation rules are configurable future software requirements.

---

# 41. Servo Command Protocol

The S3 and C3 can communicate over UART.

Example high-level commands:

```text
DISPENSE,1
OPEN,1
CLOSE,1
```

where the last number identifies the medication module.

Examples:

```text
DISPENSE,1
OPEN,1
CLOSE,1

DISPENSE,2
OPEN,2
CLOSE,2

DISPENSE,3
OPEN,3
CLOSE,3
```

The C3 can reply with acknowledgements:

```text
OK,DISPENSE,1
OK,OPEN,1
OK,CLOSE,1
```

Additional error responses can be defined later.

---

# 42. Module Mapping

Example for three medicine modules:

```text
Module 1
  Dispenser Servo → Servo A
  Hatch Servo     → Servo B

Module 2
  Dispenser Servo → Servo C
  Hatch Servo     → Servo D

Module 3
  Dispenser Servo → Servo E
  Hatch Servo     → Servo F
```

The ESP32-C3 is responsible for controlling these actuator channels.

The final GPIO mapping should be selected only after accounting for:

- ESP32-C3 board pin availability
- boot/strapping constraints
- UART connection to the S3
- servo control pins
- any debugging pins
- PCB connector routing

---

# 43. Expansion Module Electrical Concept

The user clarified that expansion modules do not carry proximity sensors.

The current simplified module concept is therefore:

```text
Expansion module
├── 5V
├── GND
├── dispenser servo signal
└── hatch servo signal
```

Depending on the finalized connector and PCB routing, additional dedicated grounds/signals can be included for reliability.

Because the expansion module does not contain its own MCU, there is no need for a full local controller interface.

---

# 44. Mechanical Connection

The physical concept is:

> **Slide an additional medicine module beside the main module.**

The modules should have a physical locking mechanism so they do not move apart during use.

Possible mechanical approaches:

- slide rails
- dovetail profile
- tongue-and-groove
- keyed mechanical latch
- spring latch

The final design should combine:

1. mechanical alignment
2. electrical contact
3. secure retention
4. easy removal

---

# 45. Main Module as the Minimum Product

The smallest valid product configuration is:

```text
┌─────────────────────────┐
│       MAIN MODULE       │
│                         │
│ Medicine A              │
│ Dispenser               │
│ Hatch                   │
│ ESP32-S3                │
│ ESP32-C3                │
│ Proximity sensor        │
│ Buzzer                  │
│ USB-C                   │
└─────────────────────────┘
```

No expansion module is necessary for a user with only one medication type.

This is an important product design principle.

---

# 46. Expansion Product Model

The system scales linearly:

```text
1 medicine:
Main module

2 medicines:
Main + expansion

3 medicines:
Main + 2 expansions

4 medicines:
Main + 3 expansions
```

The exact maximum number of modules is a future design limit based on:

- GPIO availability
- servo supply capacity
- mechanical dimensions
- connector capacity
- current draw
- communication architecture

---

# 47. Why the Main Module Still Has Two Servos

The main module is not only the "controller box."

It also dispenses Medicine #1.

Therefore it has:

```text
Servo 1 = medicine dispenser
Servo 2 = hatch
```

This is identical in mechanical function to an expansion module.

The main module merely contains **additional electronics and sensors**.

---

# 48. Optional Future Enhancements

These are not mandatory for the base prototype but can be considered after the core mechanism works.

## 48.1 Hatch Position Sensor

Confirms physically open/closed state.

## 48.2 Low Medicine Level Detection

Could be estimated from a starting count:

```text
starting quantity
-
successful scheduled doses
=
estimated remaining quantity
```

Or later improved with a physical sensor.

## 48.3 Physical Acknowledge Button

Can require user interaction before dispensing.

Example:

```text
Reminder
↓
User presses TAKE
↓
Presence detected
↓
Hatch opens
```

This can provide an additional confirmation step.

## 48.4 Display

A small OLED/TFT can show:

```text
07:00 AM

Medicine A
1 tablet

Please take medicine
```

## 48.5 Local Controls

Future buttons can provide:

- manual setup
- Wi-Fi status
- module testing
- servo calibration
- maintenance mode

---

# 49. Safety and Reliability Considerations

Even though the base project is not using pill confirmation sensing, several protections are recommended.

At minimum:

- keep the hatch closed until the user is present
- use a controlled/mechanically indexed dispenser
- use a dedicated servo power rail
- provide power protection
- prevent accidental continuous servo activation
- have a firmware error state
- provide a maintenance/manual service mode
- prevent multiple medication schedules from issuing contradictory commands
- store the schedule locally on the device
- avoid depending exclusively on continuous Internet availability
- keep notification credentials out of frontend code

---

# 50. Recommended Prototype Development Order

Do not attempt the entire system at once.

Build in this order.

## Phase 1 — Mechanical dispenser

Build one module only.

Verify:

```text
Medicine storage
↓
Gravity feed
↓
Spiral/rotary mechanism
↓
One controlled drop
```

Test real tablets/capsules.

---

## Phase 2 — Two-servo module

Add:

```text
Dispenser servo
Hatch servo
```

Verify:

```text
servo movement
home position
dispensing position
hatch open
hatch close
```

---

## Phase 3 — ESP32-C3 Servo Controller

Implement:

```text
S3 → C3 command
C3 → servo movement
C3 → acknowledgement
```

Test all six servo outputs if building the three-module version.

---

## Phase 4 — Main ESP32-S3

Add:

- Wi-Fi
- local scheduling
- time
- buzzer
- proximity sensor
- device state machine

---

## Phase 5 — User Presence

Implement:

```text
scheduled time
↓
buzzer
↓
proximity
↓
hatch permission
```

---

## Phase 6 — Web Application

Implement:

- medicine entry
- module selection
- dosage
- schedule
- date
- time
- enabled/disabled

---

## Phase 7 — Backend

Implement:

- API
- database
- schedule storage
- device synchronization

---

## Phase 8 — ntfy

Implement:

```text
Medication event
↓
backend
↓
ntfy
↓
phone
```

---

## Phase 9 — Expansion

Add:

```text
Module 2
Module 3
```

and verify the same mechanism works for every module.

---

# 51. Recommended First MVP

The first working prototype should contain only:

```text
MAIN MODULE
├── ESP32-S3
├── ESP32-C3
├── USB-C
├── Proximity sensor
├── Buzzer
├── 1 medicine cylinder
├── 1 spiral/rotary dispenser
├── 1 dispenser servo
└── 1 hatch servo
```

The MVP sequence:

```text
Set test schedule
        ↓
Time reached
        ↓
Buzzer
        ↓
Person enters vicinity
        ↓
Proximity detected
        ↓
Hatch opens
        ↓
Dispenser servo indexes
        ↓
Medicine drops
        ↓
User retrieves medicine
        ↓
Hatch closes
        ↓
System returns to IDLE
```

Only once this works reliably should the cloud layer and additional modules be integrated.

---

# 52. Finalized Design Summary

## Controllers

### ESP32-S3 SuperMini

**Main brain / IoT**

- Wi-Fi
- web/API
- schedule
- local scheduling
- time
- ntfy integration
- system logic
- proximity logic
- buzzer
- S3↔C3 communication

### ESP32-C3 SuperMini

**Dedicated servo controller**

- all dispenser servos
- all hatch servos
- actuator command execution

---

## Main Module

Contains:

- ESP32-S3
- ESP32-C3
- USB-C power input
- power distribution
- proximity/IR sensor
- buzzer
- Medicine Module #1
- dispenser servo
- hatch servo
- cylindrical medicine reservoir
- spiral/rotary dispenser

---

## Expansion Module

Contains:

- cylindrical medicine reservoir
- spiral/rotary dispenser
- dispenser servo
- hatch servo
- mechanical/electrical attachment interface

No expansion MCU is required in the base design.

---

## Power

```text
USB-C 5V
   ↓
Protection
   ↓
5V system bus
   ├── ESP32-S3
   ├── ESP32-C3
   └── dedicated servo rail
```

Servos are not intended to be powered through the ESP32's logic power path.

---

## Medicine Dispensing

Final mechanical concept:

```text
Cylinder reservoir
       ↓
Gravity
       ↓
Spiral / flower-hopper style metering mechanism
       ↓
Indexed servo rotation
       ↓
Controlled medicine release
```

The exact servo angle and spiral geometry must be calibrated experimentally.

---

## Presence Detection

The proximity/IR sensor:

- detects the medicine taker's presence
- prevents the hatch from opening when nobody is nearby

It is **not** a medicine-drop confirmation sensor.

---

## Notifications

Use:

- local buzzer
- ntfy push notifications

Secure the ntfy publishing credentials on the backend.

---

## Internet Architecture

Preferred:

```text
Netlify Web App
      ↓
Netlify Functions/API
      ↓
Database
      ↓
ESP32 schedule synchronization
```

WebSockets are optional and not required for the base design.

---

# 53. Final Concept in One Diagram

```text
                              USER
                               │
                   ┌───────────┴───────────┐
                   │                       │
                   ▼                       ▼
                WEB APP                ntfy PUSH
                   │                       ▲
                   ▼                       │
             NETLIFY BACKEND               │
                   │                       │
                   │ schedule              │
                   ▼                       │
          ┌───────────────────────┐        │
          │     ESP32-S3          │────────┘
          │     MAIN BRAIN        │
          │                       │
          │ Wi-Fi                 │
          │ Schedule              │
          │ Time                  │
          │ Presence              │
          │ Buzzer                │
          │ System logic          │
          └──────────┬────────────┘
                     │ UART
                     ▼
          ┌───────────────────────┐
          │     ESP32-C3          │
          │   SERVO CONTROLLER    │
          └─────┬────────┬────────┘
                │        │
        ┌───────┘        └───────────┐
        ▼                            ▼
 MAIN MED MODULE                EXPANSION MODULES
 ┌─────────────────┐          ┌─────────────────────┐
 │ Cylinder        │          │ Cylinder             │
 │ Spiral dispenser│          │ Spiral dispenser     │
 │ Servo: dispense │          │ Servo: dispense      │
 │ Servo: hatch    │          │ Servo: hatch         │
 │ Proximity       │          └─────────────────────┘
 │ Buzzer           │
 └─────────────────┘

                USB-C 5V POWER
                       │
               ┌───────┴───────┐
               ▼               ▼
            LOGIC POWER    SERVO POWER
```

---

# 54. Final Engineering Principle

The most important design principle for this project is:

> **The electronics should supervise the medication process, while the mechanical dispenser should inherently control the physical dose as much as possible.**

That means:

- the S3 manages the schedule and system logic
- the C3 manages servo movement
- the proximity sensor enforces user presence
- the hatch controls access
- the spiral/rotary mechanism controls physical medicine release
- the backend manages schedules and notifications
- the local device retains enough schedule information to continue basic operation during temporary connectivity loss
- the expansion modules stay mechanically standardized

This keeps the system compact, modular, serviceable, and easier to expand.

---

# 55. Current Finalized Decisions

The following decisions are considered **final for the current design direction**:

- [x] ESP32-S3 SuperMini is the main/IoT brain.
- [x] ESP32-C3 SuperMini is used specifically for servo control.
- [x] Main module is also Medication Module #1.
- [x] Main module has the same two-servo medication mechanism as expansion modules.
- [x] Each medicine module has two servos:
  - dispenser
  - hatch
- [x] Expansion modules do not require their own microcontrollers.
- [x] System uses a USB-C 5 V power source.
- [x] Servo power is separated from the ESP32 logic power path.
- [x] Proximity/IR sensor is used to detect the medicine taker.
- [x] Proximity sensor is not used for pill-drop confirmation.
- [x] Buzzer provides a local medication reminder.
- [x] ntfy is used for push notifications.
- [x] Netlify is used for the web application/backend ecosystem.
- [x] Base architecture does not require WebSockets.
- [x] Device should store/synchronize schedules locally.
- [x] NTP is suitable for network time; an RTC is recommended for additional reliability.
- [x] Medication storage uses a cylindrical container concept.
- [x] Medicine is intended to gravity-feed downward.
- [x] Dispensing uses a spiral/rotary metering mechanism.
- [x] Mechanical anti-double-feed should be achieved through dispenser geometry as much as possible.
- [x] Main module and expansion modules share the same medicine-module concept.
- [x] Expansion is optional; one-medication users only need the main module.
- [x] First prototype should be built and validated as a single-module system before adding multiple modules and full cloud integration.

---

# 56. Items Still Requiring Engineering Validation

These are deliberately left open until prototyping:

- exact servo model
- exact number of dispensing positions
- exact servo angular step
- spiral geometry
- tablet/capsule size range
- anti-double-feed geometry
- maximum quantity per cylinder
- exact USB-C charger current requirement
- exact diode/protection circuit
- servo-rail fuse/current protection
- servo power filtering/capacitance
- exact ESP32-C3 GPIO assignment
- exact S3↔C3 UART pins
- module connector design
- mechanical slide/lock geometry
- hatch geometry
- proximity sensor model and range
- reminder persistence/retry rules
- whether a hatch position sensor will be included in the first prototype
- maximum supported number of expansion modules

These should be finalized from physical testing and PCB/mechanical constraints rather than guessed up front.
