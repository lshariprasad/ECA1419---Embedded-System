# Experiment 9 – Transaction Level Modeling (TLM) and SystemC-Based Production Line Simulator

![Platform](https://img.shields.io/badge/Platform-Proteus%20%7C%20Arduino%20UNO%20%7C%20SystemC-blue)
![Model](https://img.shields.io/badge/Model-SystemC%20TLM--2.0-green)
![Payload](https://img.shields.io/badge/TLM%20payload-555-orange)

## 1. Objectives

1. Study Transaction Level Modeling (TLM) and its role in system-level design and hardware/software co-simulation.
2. Develop a production-line simulator using SystemC.
3. Represent processing stations, conveyors, buffers and a control unit using modular design.
4. Create an initiator-to-target TLM transaction using a generic payload and `b_transport()`.
5. Transfer and process the required transaction data value **555**.

## 2. Project contents

```text
Exp_09_SystemC_TLM/
├── README.md
├── EVIDENCE_INDEX.md
├── reflection.md
├── code/
│   ├── arduino/
│   │   ├── production_line_controller.ino
│   │   └── README.md
│   ├── reference_model/
│   │   └── production_line_reference.py
│   └── systemc_tlm/
│       ├── main.cpp
│       ├── CMakeLists.txt
│       ├── run_systemc.ps1
│       └── README.md
├── proteus/
│   ├── production_line_running_capture.png
│   ├── production_line_full_schematic.png
│   └── README.md
└── output/
    ├── proteus_simulation_running.png
    ├── expected_tlm_execution_preview_NOT_SYSTEMC.png
    ├── reference_model_console_preview_NOT_SYSTEMC.png
    ├── reference_model_run_NOT_SYSTEMC.txt
    ├── source_static_check.txt
    └── README.md
```

## 3. System architecture

### Proteus / Arduino hardware-level demonstration

The Proteus schematic represents a three-stage production line:

- Arduino UNO R3 – production-line controller.
- Three DC motors – conveyor 1, conveyor 2 and conveyor 3.
- Two L293D H-bridge driver ICs – motor interfaces.
- Three `LOGICSTATE` inputs – simulated product-detection sensors.
- Three push buttons – START, STOP and RESET.
- Two 74HC595 shift registers – 10 status LEDs using three Arduino control pins.
- LM016L 16×2 LCD – line state and production status.
- Buzzer – fault alarm.

**Important distinction:** Proteus represents the circuit and controller behavior. The TLM initiator/target sockets, generic payload and `b_transport()` are implemented in the separate SystemC program in `code/systemc_tlm/`. The Proteus schematic does not execute SystemC TLM code.

### Latest Proteus evidence

The two latest user-supplied screenshots are included:

1. `proteus/production_line_running_capture.png` – the Proteus status bar visibly shows `ANIMATING` at the time of capture, and the LCD displays `SYSTEM RUNNING` / `SPEED: NORMAL`. Station/buffer indicator LEDs are visible.
2. `proteus/production_line_full_schematic.png` – a wider schematic view showing the Arduino UNO, both L293D drivers, three motors, buttons, sensor inputs, cascaded 74HC595 devices, LEDs, LCD and buzzer.

The running screenshot is also copied to `output/proteus_simulation_running.png`. These are user-supplied Proteus captures. The active/animating view is evidence that the simulation was running at capture time, but it is not a test report for every sensor, button, buffer-full and fault condition.

### SystemC TLM model

```text
ProductionLineInitiator
      |
      | simple_initiator_socket
      | generic payload: WRITE, data = 555
      | b_transport()
      v
ProductionLineTarget
      |
      +--> Conveyor 1 --> Processing Station 1 --> Buffer 1
      |
      +--> Conveyor 2 --> Inspection Station 2  --> Buffer 2
      |
      +--> Conveyor 3 --> Packaging Station 3    --> Buffer 3
      |
      +--> TLM response: TLM_OK_RESPONSE
```

The target validates command, address and payload length, reads the 32-bit data value, and models the product moving through all three stages. Each conveyor, station and buffer is represented as a SystemC module. Blocking waits represent modeled stage delays.

## 4. Proteus component list

| Component | Quantity | Purpose |
|---|---:|---|
| Arduino UNO R3 | 1 | Control unit |
| L293D | 2 | Conveyor motor drivers |
| DC MOTOR | 3 | Conveyors |
| 74HC595 | 2 | LED output expansion |
| LM016L | 1 | 16×2 LCD |
| LOGICSTATE | 3 | Simulated product sensors |
| BUTTON | 3 | Start, stop and reset |
| LED-GREEN | 3 | Station-active indication |
| LED-RED | 4 | Three buffer indicators and system fault |
| LED-YELLOW | 3 | Processing indication |
| RES | 10 | LED current limiting |
| POT-HG | 1 (10 kΩ) | LCD contrast |
| CAP | 2 (100 nF) | Supply decoupling |
| BUZZER | 1 | Fault alert |
| DC supplies | As required | Logic and motor supplies |

## 5. Wiring reference for the Arduino sketch

Check the actual Proteus net connections against this reference before compiling/loading the sketch.

### Arduino pin allocation

| Arduino pin | Connected function |
|---|---|
| D0 | Buzzer (no Serial use while buzzer is connected) |
| D2, D3, D4 | Product sensors 1, 2, 3 (active HIGH) |
| D5, D6, D9 | L293D enable pins for conveyors 1, 2, 3 |
| D7, D8, D10 | START, STOP, RESET buttons (active LOW to GND) |
| D11 | 74HC595 serial data |
| D12 | 74HC595 shift clock |
| D13 | 74HC595 storage/latch clock |
| A0 | LCD RS |
| A1 | LCD E |
| A2–A5 | LCD D4–D7 |

### L293D motor-driver connections

For fixed forward direction, the sketch expects the direction inputs to be strapped as follows:

| Driver | Enable | INx connections | Output pins |
|---|---|---|---|
| U1 channel 1 / Motor 1 | Pin 1 → D5 | Pin 2 → +5 V; pin 7 → GND | Pins 3 and 6 → Motor 1 |
| U1 channel 2 / Motor 2 | Pin 9 → D6 | Pin 10 → +5 V; pin 15 → GND | Pins 11 and 14 → Motor 2 |
| U2 channel 1 / Motor 3 | Pin 1 → D9 | Pin 2 → +5 V; pin 7 → GND | Pins 3 and 6 → Motor 3 |
| U2 channel 2 unused | Pin 9 → GND | Pins 10 and 15 → GND | Pins 11 and 14 unused |

For both L293D ICs: pin 16 (logic supply) → +5 V; pin 8 (motor supply) → selected motor supply; all ground pins → common GND. Motor-supply negative, Arduino GND and driver GND must be common. Do not connect motor supply voltage to Arduino 5 V. Confirm motor voltage/current ratings before real hardware use.

### 74HC595 connections

For both U3 and U4: pin 16 → +5 V, pin 8 → GND, pin 10 (MR) → +5 V, pin 13 (OE) → GND. Pin 11 (SH_CP) is common to Arduino D12 and pin 12 (ST_CP) is common to Arduino D13. Arduino D11 connects to U3 pin 14 (DS); U3 pin 9 (Q7S/Q7') connects to U4 pin 14 (DS).

| Register output | IC pin | LED | Colour | Function |
|---|---:|---|---|---|
| U3 Q0 | 15 | D1 | Green | Station 1 active |
| U3 Q1 | 1 | D2 | Green | Station 2 active |
| U3 Q2 | 2 | D3 | Green | Station 3 active |
| U3 Q3 | 3 | D4 | Red | Buffer 1 full |
| U3 Q4 | 4 | D5 | Red | Buffer 2 full |
| U3 Q5 | 5 | D6 | Red | Buffer 3 full |
| U3 Q6 | 6 | D7 | Yellow | Station 1 processing |
| U3 Q7 | 7 | D8 | Yellow | Station 2 processing |
| U4 Q0 | 15 | D9 | Yellow | Station 3 processing |
| U4 Q1 | 1 | D10 | Red | System fault |

For each LED: 74HC595 output → individual series resistor → LED anode; LED cathode → GND. Use an individual **1 kΩ** resistor per LED for a conservative shift-register load. The latest screenshot appears to show 220 Ω resistor labels; check and preferably change R1–R10 to 1 kΩ before connecting physical LEDs or hardware.

### LCD connections

| LM016L pin | Signal | Connection |
|---:|---|---|
| 1 | VSS | GND |
| 2 | VDD | +5 V |
| 3 | VEE | 10 kΩ potentiometer wiper |
| 4 | RS | Arduino A0 |
| 5 | RW | GND |
| 6 | E | Arduino A1 |
| 7–10 | D0–D3 | Not connected (4-bit mode) |
| 11–14 | D4–D7 | Arduino A2–A5 respectively |

10 kΩ contrast potentiometer: outer terminals to +5 V and GND; centre/wiper to LCD pin 3.

### Buttons, sensors and buzzer

- Each button is wired between its Arduino input pin and GND. The sketch enables `INPUT_PULLUP`, so a pressed button reads LOW.
- Each `LOGICSTATE` output connects to its corresponding sensor pin D2, D3 or D4. The sketch treats HIGH as product detected.
- The buzzer is connected to D0 and GND for the Proteus model. D0 is also the UNO hardware serial RX pin; the sketch does not use `Serial`. For a physical board, disconnect the buzzer during USB uploads. Use a transistor driver if buzzer current exceeds the GPIO rating.

## 6. Build and run the SystemC TLM program (Windows + MSYS2 UCRT64)

Prerequisites:

- MSYS2 UCRT64 GCC/G++
- MSYS2 UCRT64 CMake and Ninja
- SystemC installed in `C:/msys64/ucrt64`

Open PowerShell in the project root and run:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\code\systemc_tlm\run_systemc.ps1
```

The script configures CMake, builds the executable, runs the TLM simulation and writes the actual console output to `output\systemc_tlm_run.txt`. The expected runtime trace should show the initiator sending **555**, the target receiving it, all three conveyor/station/buffer stages handling the product, and the initiator receiving `TLM_OK_RESPONSE`.

**Verification status:** The SystemC source is included, but it has not been compiled or executed as part of preparing this ZIP. Run the script on the configured Windows/MSYS2 PC to generate the genuine runtime log.

## 7. Arduino/Proteus controller sketch

Open `code/arduino/production_line_controller.ino` in Arduino IDE, select Arduino UNO, compile, and load the HEX file into the Proteus Arduino UNO model. The sketch:

- starts/stops all three conveyor enables,
- reads the three simulated product sensors,
- counts a product when sensor 3 becomes active,
- displays line state and completed-product count on the LCD,
- updates the 10 LEDs through the cascaded 74HC595s,
- sounds the buzzer when all three sensor inputs are active together.

The supplied running capture displays `SYSTEM RUNNING` / `SPEED: NORMAL`. The bundled sketch uses the LCD strings `LINE: RUNNING` / `Done: ...`; the screenshots are evidence from the supplied Proteus session and are not proof that this exact sketch generated those exact LCD words.

## 8. Evidence and verification status

### Proteus evidence supplied

- `proteus/production_line_running_capture.png` is the latest supplied Proteus capture. Its status bar shows `ANIMATING`, and the LCD displays `SYSTEM RUNNING` / `SPEED: NORMAL`; several status LEDs are visibly illuminated.
- `proteus/production_line_full_schematic.png` shows the complete production-line circuit layout.
- `output/proteus_simulation_running.png` is a copy of the running capture for the output folder.

This is visual evidence of an active Proteus simulation view at the time of capture. It is not a test report for every START/STOP/RESET, sensor, buffer-full and fault condition. The latest images do not include the complete Simulation Errors panel, so this package does not claim that every diagnostic has been cleared.

### SystemC evidence supplied

- `output/expected_tlm_execution_preview_NOT_SYSTEMC.png` is a timing preview inferred from source waits. It is not a SystemC runtime capture.
- `output/reference_model_run_NOT_SYSTEMC.txt` and `output/reference_model_console_preview_NOT_SYSTEMC.png` are from a separate Python reference model, not proof of SystemC TLM socket execution.
- `output/source_static_check.txt` records static source checks only.
- `output/systemc_tlm_run.txt` is generated by `run_systemc.ps1` after the actual SystemC executable builds and runs successfully.

Do not present reference-model previews as actual SystemC output. Capture the real Windows terminal after running the script and keep the generated log in `output/`.

## 9. Concepts for viva

- **TLM:** models communication and functionality at transaction level rather than modeling every individual signal transition.
- **Initiator socket:** the interface used by the module that starts a transaction.
- **Target socket:** the interface that receives and services the transaction.
- **Generic payload:** the standard TLM object carrying command, address, data pointer, data length, streaming width and response status.
- **`b_transport()`:** blocking transport interface; the initiator waits until the target completes the transaction.
- **Transaction delay:** SystemC `wait()` calls in the target model the time taken by conveyor and processing stages.

## 10. Author / course

ECA1419 – Embedded Systems Laboratory  
B.Tech Electrical and Electronics Engineering  
SIMATS Engineering
