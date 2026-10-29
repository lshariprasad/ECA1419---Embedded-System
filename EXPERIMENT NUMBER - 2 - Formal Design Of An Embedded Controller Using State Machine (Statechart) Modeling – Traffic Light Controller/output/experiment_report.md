# Experiment 2 — Simulation Report

## Title
Formal Design of an Embedded Controller Using State Machine (Statechart) Modeling — Traffic Light Controller

## Objective
To design and simulate a timed traffic-light controller using a finite-state machine (FSM) on an Arduino UNO.

## Components
Arduino UNO, red LED, yellow LED, green LED, three 220 Ω resistors, and GND terminal(s).

## Pin configuration
- RED: D8
- YELLOW: D9
- GREEN: D10
- Common cathode return: GND

## Statechart
Refer to `fsm_statechart.png`.

## State transition table
| State | Entry output | Time | Transition |
|---|---|---:|---|
| RED | Red LED ON; yellow/green OFF | 5 s | Timer expiry → GREEN |
| GREEN | Green LED ON; red/yellow OFF | 5 s | Timer expiry → YELLOW |
| YELLOW | Yellow LED ON; red/green OFF | 2 s | Timer expiry → RED |

## Procedure
1. Connect the LEDs and their current-limiting resistors according to the pin table.
2. Compile `code/TrafficLightController.ino` for Arduino UNO.
3. Export the compiled HEX file and assign it to the Proteus Arduino component.
4. Start the simulation and observe the LEDs through several cycles.
5. Record actual test observations in `test_results_template.csv` and add screenshots below.

## Expected result
The controller starts with RED, then transitions to GREEN and YELLOW according to the configured timers. Only one LED is on at a time. The cycle repeats continuously. State-change messages should appear in the Serial Monitor at 9600 baud.

## Actual result
**Pending verification.** Fill this section only after running the simulation on your computer.

## Screenshots
Add verified simulation screenshots here, for example:
- `red_state.png`
- `green_state.png`
- `yellow_state.png`

## Conclusion
**To be completed after simulation.** State whether the observed LED sequence and timing matched the expected FSM behavior.
