# Proteus schematic and simulation evidence

This folder contains the two latest user-supplied Proteus screenshots for Experiment 9.

| File | Description |
|---|---|
| `production_line_running_capture.png` | Proteus capture whose status bar visibly shows `ANIMATING`; the LCD reads `SYSTEM RUNNING` / `SPEED: NORMAL`, and multiple status LEDs are illuminated. |
| `production_line_full_schematic.png` | Wider schematic view of the Arduino UNO, both L293D drivers, three motors, three buttons, three LogicState inputs, two 74HC595 devices, ten LEDs, LM016L LCD, potentiometer and buzzer. |

These are the images supplied by the user. They are included as evidence without fabricating additional states. The running view demonstrates that the simulator was active at capture time; it is not a complete test matrix for every button, sensor and fault scenario. The complete Simulation Errors panel is not shown in these latest images.

**Hardware note:** The latest schematic screenshot appears to show 220 Ω LED resistors. For physical hardware, use an individually current-limiting resistor for each LED and preferably select 1 kΩ to limit the total load on each 74HC595. Keep the motor supply separate from the Arduino 5 V logic rail and share ground references.
