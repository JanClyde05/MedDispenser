# MedBox — Complete Pin Mapping & Interconnect Specification

> **Last Updated:** 2026-08-22 — Pin assignments verified against actual hardware wiring. S3 UART pins corrected to GPIO 1 (TX) / GPIO 2 (RX).

> 📌 **STATUS: FINALIZED & VERIFIED** — Tested against ESP32-S3 SuperMini and ESP32-C3 SuperMini hardware. UART communication confirmed working (PING/PONG/DISPENSE/OPEN/CLOSE).

---

## 1. System Interconnect Summary

```
                    ┌─────────────────────────┐
                    │     USB-C 5V / 3A       │
                    └────────────┬────────────┘
                                 │
                   ┌─────────────┴─────────────┐
                   ▼                           ▼
        ┌─────────────────────┐     ┌─────────────────────┐
        │  5V LOGIC BUS       │     │  5V SERVO RAIL      │
        │ (ESP32-S3 & C3)     │     │ (100-470µF Cap)     │
        └──────────┬──────────┘     └──────────┬──────────┘
                   │                           │
  ┌────────────────┴──────────────┐            │
  │ ESP32-S3 SuperMini (Brain)    │            │
  │  • GPIO 1  (TX)  ─────────────┼──────────┐ │
  │  • GPIO 2  (RX)  ◄────────────┼────────┐ │ │
  │  • GPIO 5  ◄── IR Proximity   │        │ │ │
  │  • GPIO 6  ──► Active Buzzer  │        │ │ │
  └───────────────────────────────┘        │ │ │
                                           │ │ │
  ┌───────────────────────────────┐        │ │ │
  │ ESP32-C3 SuperMini (Motion)   │        │ │ │
  │  • GPIO 20 (RX0) ◄────────────┼────────┘ │ │
  │  • GPIO 21 (TX0) ─────────────┼──────────┘ │
  │  • GPIO 0  ──► M1 Dispenser   │            │
  │  • GPIO 1  ──► M1 Hatch       │            │
  │  • GPIO 3  ──► M2 Dispenser   │            │
  │  • GPIO 4  ──► M2 Hatch       │            │
  │  • GPIO 5  ──► M3 Dispenser   │            │
  │  • GPIO 6  ──► M3 Hatch       │            │
  └───────────────────────────────┘            │
                   │                           │
                   ▼                           ▼
        ┌──────────────────────────────────────────┐
        │ Common GND Bus (Shared System Reference) │
        └──────────────────────────────────────────┘
```

---

## 2. ESP32-S3 SuperMini (Main IoT Controller) Pin Map

| Board Pin Name | GPIO | Function / Peripheral | Physical Connection Target | Signal Direction | Notes |
|:--------------|:----:|:----------------------|:---------------------------|:----------------:|:------|
| **5V / VBUS** | — | 5V Main Power Input | USB-C 5V Power Rail | Input | Powers board internal 3.3V LDO regulator |
| **3V3** | — | 3.3V Regulated Output | IR Proximity Sensor VCC | Output | Max 300mA total board draw |
| **GND** | — | System Ground | Common GND Bus | Power Ref | **MUST share common ground** with C3 & Servos |
| **GPIO 1** | `GPIO 1` | UART Transmit (TX) | ESP32-C3 `GPIO 20` (RX0) | Output (3.3V) | Transmits ASCII move commands to C3 |
| **GPIO 2** | `GPIO 2` | UART Receive (RX) | ESP32-C3 `GPIO 21` (TX0) | Input (3.3V) | Receives status/ack responses from C3 |
| **GPIO 5** | `GPIO 5` | Proximity / IR Input | IR Obstacle / Proximity Sensor `OUT` | Input (Digital) | Active LOW (configurable via `PROXIMITY_ACTIVE` in `config.h`) |
| **GPIO 6** | `GPIO 6` | Active Buzzer Drive | Active Buzzer `+` terminal | Output (Digital) | Uses `digitalWrite(HIGH/LOW)` — NOT PWM `ledcWrite` |

### ⚠️ ESP32-S3 Strapping Pins (DO NOT USE)
| GPIO | Strapping Function | Risk if Connected |
|:----:|:-------------------|:------------------|
| `GPIO 0` | Boot Mode (Low = Download, High = SPI Boot) | Boot failure if pulled LOW during startup |
| `GPIO 45` | VDD_SPI Voltage Selector | Can brownout internal SPI flash |
| `GPIO 46` | Boot Log Verbosity / ROM Code | Prevents clean cold boot |

---

## 3. ESP32-C3 SuperMini (Servo Controller) Pin Map

| Board Pin Name | GPIO | Function / Servo Channel | Physical Connection Target | Wire Color | Signal Type |
|:--------------|:----:|:-------------------------|:---------------------------|:----------:|:-----------|
| **5V / VBUS** | — | 5V Logic Power | USB-C 5V Power Rail | Red | Power Input |
| **GND** | — | System Ground | Common GND Bus | Black | Common Ground |
| **RX0** | `GPIO 20` | UART0 Receive | ESP32-S3 `GPIO 1` (TX) | Yellow/Blue | Serial RX (115200 Baud) |
| **TX0** | `GPIO 21` | UART0 Transmit | ESP32-S3 `GPIO 2` (RX) | Green/White | Serial TX (115200 Baud) |
| **GPIO 0** | `GPIO 0` | Module 1 Dispenser Servo | M1 Dispenser Servo PWM Signal | Orange/Yellow | PWM (50 Hz, 500-2400 µs) |
| **GPIO 1** | `GPIO 1` | Module 1 Hatch Servo | M1 Hatch Servo PWM Signal | Orange/Yellow | PWM (50 Hz, 500-2400 µs) |
| **GPIO 3** | `GPIO 3` | Module 2 Dispenser Servo | M2 Dispenser Servo PWM Signal | Orange/Yellow | PWM (50 Hz, 500-2400 µs) |
| **GPIO 4** | `GPIO 4` | Module 2 Hatch Servo | M2 Hatch Servo PWM Signal | Orange/Yellow | PWM (50 Hz, 500-2400 µs) |
| **GPIO 5** | `GPIO 5` | Module 3 Dispenser Servo | M3 Dispenser Servo PWM Signal | Orange/Yellow | PWM (50 Hz, 500-2400 µs) |
| **GPIO 6** | `GPIO 6` | Module 3 Hatch Servo | M3 Hatch Servo PWM Signal | Orange/Yellow | PWM (50 Hz, 500-2400 µs) |

### ⚠️ ESP32-C3 Strapping Pins (DO NOT USE)
| GPIO | Strapping Function | Risk if Connected |
|:----:|:-------------------|:------------------|
| `GPIO 2` | Boot Mode (SPI Boot vs UART Download) | **CRITICAL:** High/Low floating voltage during boot bricks startup |
| `GPIO 8` | Direct Boot State Check | Causes random boot loops |
| `GPIO 9` | Boot Button / Strapping Pin | Internal pull-up; external servo pull down forces boot mode |

---

## 4. Peripherals & Actuators Connection Breakout

### 4.1 Module 1 (Main Box Dispenser & Hatch)
| Device | Pin / Terminal | Connects To | Wire Color Code |
|:-------|:---------------|:------------|:----------------|
| **M1 Dispenser Servo** | Pin 1 (GND) | Common GND Bus | Brown / Black |
| | Pin 2 (VCC) | 5V Dedicated Servo Rail | Red |
| | Pin 3 (Signal) | ESP32-C3 `GPIO 0` | Orange / Yellow |
| **M1 Hatch Servo** | Pin 1 (GND) | Common GND Bus | Brown / Black |
| | Pin 2 (VCC) | 5V Dedicated Servo Rail | Red |
| | Pin 3 (Signal) | ESP32-C3 `GPIO 1` | Orange / Yellow |

### 4.2 Module 2 (Expansion Unit #1)
| Device | Pin / Terminal | Connects To | Wire Color Code |
|:-------|:---------------|:------------|:----------------|
| **M2 Dispenser Servo** | Pin 1 (GND) | Common GND Bus | Brown / Black |
| | Pin 2 (VCC) | 5V Dedicated Servo Rail | Red |
| | Pin 3 (Signal) | ESP32-C3 `GPIO 3` | Orange / Yellow |
| **M2 Hatch Servo** | Pin 1 (GND) | Common GND Bus | Brown / Black |
| | Pin 2 (VCC) | 5V Dedicated Servo Rail | Red |
| | Pin 3 (Signal) | ESP32-C3 `GPIO 4` | Orange / Yellow |

### 4.3 Module 3 (Expansion Unit #2)
| Device | Pin / Terminal | Connects To | Wire Color Code |
|:-------|:---------------|:------------|:----------------|
| **M3 Dispenser Servo** | Pin 1 (GND) | Common GND Bus | Brown / Black |
| | Pin 2 (VCC) | 5V Dedicated Servo Rail | Red |
| | Pin 3 (Signal) | ESP32-C3 `GPIO 5` | Orange / Yellow |
| **M3 Hatch Servo** | Pin 1 (GND) | Common GND Bus | Brown / Black |
| | Pin 2 (VCC) | 5V Dedicated Servo Rail | Red |
| | Pin 3 (Signal) | ESP32-C3 `GPIO 6` | Orange / Yellow |

### 4.4 Proximity Sensor & Buzzer
| Component | Sensor Pin | Connects To | Notes |
|:----------|:-----------|:------------|:------|
| **IR Proximity Sensor** | VCC | ESP32-S3 `3V3` Pin | Or 5V depending on module model (e.g. FC-51) |
| | GND | Common GND Bus | Shared Ground |
| | OUT / DO | ESP32-S3 `GPIO 5` | Digital Input |
| **Active Buzzer** | Positive (`+`) | ESP32-S3 `GPIO 6` | Direct `digitalWrite(HIGH/LOW)` — NOT PWM |
| | Negative (`-`) | Common GND Bus | Shared Ground |

---

## 5. Expansion Module Interconnect Bus (4-Pin Connector)

Each expansion module connects to the main controller chassis via a standardized 4-pin plug/socket (e.g. JST-XH 2.54mm or Header Pins):

```
Expansion Bus Connector (Female Header on Main Unit / Male Plug on Expansion Module):
┌──────────────────────────────────────────┐
│  [1] +5V_SERVO  │  (Red)     ──► Servo Power Rail (5V 3A)
│  [2] GND        │  (Black)   ──► Common System Ground
│  [3] DISP_PWM   │  (Yellow)  ──► ESP32-C3 GPIO (3 or 5)
│  [4] HATCH_PWM  │  (White)   ──► ESP32-C3 GPIO (4 or 6)
└──────────────────────────────────────────┘
```

---

## 6. Power Distribution Wiring & Filtering Rules

1. **Dedicated Servo Rail:** Never feed motor supply power through the 3.3V or 5V output pin of either ESP32 board. Wire servo VCC directly to the main 5V power bus.
2. **Decoupling Capacitor:** Solder a **470 µF / 16V Low-ESR Electrolytic Capacitor** across the 5V Servo Rail and GND near the servo connectors to absorb current surges during motor start.
3. **Common Ground:** All GND pins from the power supply, ESP32-S3, ESP32-C3, proximity sensor, buzzer, and all 6 servos **MUST BE CONNECTED TOGETHER** to establish a zero-volt logic reference.
