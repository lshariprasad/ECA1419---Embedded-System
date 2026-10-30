# Experiment 3 — I²C-Based Health Monitoring Node for Pulse and Temperature Acquisition

An Arduino UNO educational prototype that reads an LM75AD temperature sensor over I²C, detects digital pulse edges from a Proteus Heart Beat Sensor model, calculates an estimated beats-per-minute (BPM) value, and displays readings on an LM016L 16×2 LCD.

> **Important interface note:** In the supplied schematic, the LM75AD temperature sensor uses I²C (SDA/SCL), but the LM016L display is wired in 4-bit parallel mode. Therefore, the sensor link is I²C; the LCD link is not. If the assessment requires the display module itself to communicate over I²C, replace LM016L with an I²C-backpack LCD model and update the sketch accordingly.
>
> **Safety note:** This is a classroom simulation/prototype, not a medical device. Pulse values are estimated from a simulated digital signal and are not suitable for diagnosis or clinical decisions. The LM75AD measures local sensor temperature, not necessarily human body temperature.

## Repository structure

```text
Exp_03_I2C_Health_Monitoring/
├── README.md
├── LICENSE
├── .gitignore
├── code/
│   ├── HealthMonitoringNode.ino
│   └── README.md
├── proteus/
│   ├── README.md
│   └── wiring_reference.md
└── output/
    ├── README.md
    ├── current_schematic_reference.png
    ├── system_block_diagram.svg
    ├── test_results.csv
    ├── serial_output_template.txt
    └── experiment_report_template.md
```

## Hardware and pin map

| Device / signal | Arduino UNO connection |
|---|---|
| LM75AD SDA | A4 / SDA |
| LM75AD SCL | A5 / SCL |
| LM75AD A0, A1, A2 | GND, GND, GND (I²C address `0x48`) |
| LM75AD supply | VCC to +5 V; GND to GND (check the Proteus model's power pins) |
| Heart Beat Sensor OUT | D2 |
| Heart Beat Sensor VCC | +5 V |
| Heart Beat Sensor GND | GND |
| Heart Beat Sensor Test Pin | `LOGICSTATE` set HIGH for the TEP Proteus model, according to the model instructions |
| LM016L VSS / VDD / VEE | GND / +5 V / contrast (GND is a basic starting setting) |
| LM016L RS / RW / E | D8 / GND / D9 |
| LM016L D4 / D5 / D6 / D7 | D4 / D5 / D6 / D7 |
| LM016L D0–D3 | Not connected in 4-bit mode |

All grounds must be common. Do not connect an LED or sensor output directly to a supply if its model documentation specifies another stimulus method.

## Software

- Board: Arduino UNO / ATmega328P
- Sketch: `code/HealthMonitoringNode.ino`
- Arduino libraries: `Wire` and `LiquidCrystal` (included with the Arduino AVR core)
- I²C bus speed: 100 kHz
- Default LM75AD address: `0x48` when A2=A1=A0=LOW
- Serial Monitor: 9600 baud

## Build and simulation

1. Open `code/HealthMonitoringNode.ino` in Arduino IDE.
2. Select **Tools → Board → Arduino AVR Boards → Arduino Uno**.
3. Verify/compile the sketch.
4. Use **Sketch → Export Compiled Binary** to produce the `.ino.standard.hex` file.
5. In Proteus, double-click the Arduino UNO and assign that HEX file to its **Program File** property.
6. Assign the model-specific firmware to the Heart Beat Sensor component if its library requires a separate Program File. Do not use the Arduino HEX file for the sensor model.
7. Set the heartbeat model's `Test Pin` stimulus as required by that model (for the referenced TEP model, use a `LOGICSTATE` HIGH).
8. Run the simulation and verify both the LCD and Serial Monitor.

## Expected behaviour

- The LCD first shows a startup message.
- If an LM75AD responds at address `0x48`, temperature is shown in °C.
- Rising edges on D2 are timed. After valid beat intervals are detected, BPM is estimated from a moving average of up to four intervals.
- If no pulse edges arrive for the timeout interval, BPM returns to `--`.
- Serial diagnostics are printed once per second at 9600 baud.

The exact numeric output depends on the Proteus model stimulus and sensor configuration. Do not report simulated results as real patient measurements.

## Verification status

The `output/` directory contains the currently supplied schematic screenshot as a reference, not proof of a successful simulation. Fill in `output/test_results.csv` and `output/experiment_report_template.md` only after you have run and observed the circuit.
