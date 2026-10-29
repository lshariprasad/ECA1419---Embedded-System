# Experiment 02 — FSM-Based Traffic Light Controller

**Lab title:** Formal Design of an Embedded Controller Using State Machine (Statechart) Modeling — Traffic Light Controller

## Objective
Design and simulate an Arduino UNO traffic-light controller using a finite-state machine (FSM) with RED, GREEN, and YELLOW states and timed transitions.

## Repository structure

```text
Exp_02_Traffic_Light_Controller/
├── README.md
├── LICENSE
├── .gitignore
├── code/
│   ├── TrafficLightController.ino
│   └── README.md
├── proteus/
│   ├── README.md
│   └── traffic_light_wiring_reference.md
└── output/
    ├── README.md
    ├── experiment_report.md
    ├── expected_state_sequence.txt
    ├── test_results_template.csv
    ├── fsm_statechart.svg
    ├── fsm_statechart.png
    └── traffic_light_circuit_reference.png
```

> **Important:** The ZIP includes the Arduino source, documentation, FSM diagram, test template, and the circuit screenshot available during preparation. It does **not** contain your native Proteus `.pdsprj` project because that project file was not uploaded here. Copy the real project and any associated schematic/dependency files from your computer into `proteus/` before publishing the repository.

## Hardware / Proteus components

- Arduino UNO (ATmega328P)
- One red LED, one yellow LED, one green LED
- Three 220 Ω current-limiting resistors
- GND terminal(s)

## Pin mapping

| Arduino UNO pin | Connection |
|---|---|
| D8 | 220 Ω resistor → RED LED anode |
| D9 | 220 Ω resistor → YELLOW LED anode |
| D10 | 220 Ω resistor → GREEN LED anode |
| GND | Common LED cathode return |

Each LED must have its **own series resistor**. The LED cathode (K) goes to GND; the anode (A) is driven from its Arduino digital pin through a 220 Ω resistor.

## FSM definition

The program starts in `STATE_RED` and follows this repeating sequence:

```text
RED (5 s) → GREEN (5 s) → YELLOW (2 s) → RED (repeat)
```

Durations are demonstration values chosen for the simulation and can be changed at the top of `code/TrafficLightController.ino`.

| Current state | Duration | RED output | YELLOW output | GREEN output | Next state |
|---|---:|---:|---:|---:|---|
| RED | 5000 ms | HIGH | LOW | LOW | GREEN |
| GREEN | 5000 ms | LOW | LOW | HIGH | YELLOW |
| YELLOW | 2000 ms | LOW | HIGH | LOW | RED |

## Build and simulate

1. Open `code/TrafficLightController.ino` in Arduino IDE.
2. Select **Tools → Board → Arduino AVR Boards → Arduino Uno**.
3. Click **Verify** and confirm compilation succeeds.
4. Select **Sketch → Export Compiled Binary**.
5. In Proteus, double-click the Arduino UNO and set **Program File** to the generated `.ino.standard.hex` file (or the compatible generated `.hex` file provided by your IDE version).
6. Verify the LED connections and common ground in `proteus/traffic_light_wiring_reference.md`.
7. Click **OK**, then **Run ▶**.
8. Capture screenshots of each state/sequence and add verified observations to `output/experiment_report.md` and `output/test_results_template.csv`.

## Expected result
Only one LED should be on at a time. RED remains on for 5 seconds, GREEN for 5 seconds, and YELLOW for 2 seconds; the sequence repeats. The Serial Monitor at **9600 baud** prints a message when the FSM enters each state.

## Verification status
The wiring reference screenshot is included as a record of the circuit arrangement shown during preparation. It is **not** evidence that the compiled program was successfully simulated. Replace or supplement it with screenshots captured after you run and verify the simulation on your machine.

## Notes
- The sketch uses `millis()` rather than blocking `delay()` for state timing.
- The elapsed-time calculation uses unsigned subtraction, which remains safe across `millis()` rollover.
- This is an educational model only; the simplified three-state sequence is not a certified road-traffic controller.
