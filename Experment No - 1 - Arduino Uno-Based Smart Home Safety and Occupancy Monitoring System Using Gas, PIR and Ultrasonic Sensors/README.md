# Experiment 01 — Smart Home Safety and Occupancy Monitoring

An Arduino UNO educational prototype that monitors an MQ-2 gas-sensor signal, PIR motion, and ultrasonic distance. A buzzer warns when the gas reading exceeds a configurable threshold or when motion is detected with a nearby object.

<img width="1919" height="1019" alt="image" src="https://github.com/user-attachments/assets/7d638f59-0326-4b6f-ae5f-858e774e4a30" />


## Repository layout

```text
Exp_01_Smart_Home_Safety/
├── README.md
├── code/
│   ├── SmartHomeSafety.ino
│   └── README.md
├── proteus/
│   └── README.md
└── output/
    └── README.md
```

## Hardware and Proteus wiring

| Component pin | Arduino UNO connection |
|---|---|
| MQ-2 `OUT` | `A0` |
| MQ-2 `VCC` | `5V` |
| MQ-2 `GND` | `GND` |
| PIR `OUT` | `D2` |
| PIR `VCC` | `5V` |
| PIR `GND` | `GND` |
| Ultrasonic `TRIG` / `Trigger` | `D3` |
| Ultrasonic `ECHO` / `Echo` | `D4` |
| Ultrasonic `VCC` / `+5V` | `5V` |
| Ultrasonic `GND` | `GND` |
| Buzzer `+` | `D8` |
| Buzzer `-` | `GND` |

All component grounds must share the Arduino UNO GND. Verify the exact pin labels against the selected Proteus component models.

## Features

- Moving-average filtering for the gas sensor reading.
- Gas alarm hysteresis to reduce chatter near the threshold.
- PIR input debounce.
- Ultrasonic measurements at a controlled interval and bounded echo timeout.
- Distance hysteresis for nearby-object detection.
- Continuous buzzer for gas warnings and pulsed buzzer for occupancy warnings.
- Serial status report once per second at 9600 baud.
- No third-party Arduino libraries required.

## Build the Arduino firmware

1. Open `code/SmartHomeSafety.ino` in Arduino IDE.
2. Select **Tools → Board → Arduino AVR Boards → Arduino Uno**.
3. Click **Verify**.
4. To create a HEX file for Proteus, choose **Sketch → Export Compiled Binary**.
5. In Proteus, double-click the Arduino UNO component and set **Program File** to the generated `.ino.standard.hex` (or the appropriate generated `.hex` file).
6. Check that the Proteus MQ-2, PIR, and ultrasonic models each point to their own required model firmware files. These are distinct from the Arduino application HEX.

## Configuration

Edit these constants near the top of `SmartHomeSafety.ino` to tune the simulation:

- `GAS_ALARM_ON` (default 400 ADC counts)
- `GAS_ALARM_OFF` (default 350 ADC counts)
- `DISTANCE_ENTER_CM` (default 50 cm)
- `DISTANCE_EXIT_CM` (default 55 cm)

The thresholds are starting values, not calibrated gas-concentration or safety limits. Tune them to the chosen sensor model and observed readings.

## Troubleshooting

- **Proteus cannot open a sensor HEX file:** open that sensor component's properties and point `Program File` to the corresponding sensor-model HEX from its library package. Do not use the Arduino application HEX for the sensor model.
- **CPU-load warning:** all simulated microcontroller models consume simulation resources. Close unnecessary programs, reduce the number of actively simulated models where possible, and use a controlled measurement/reporting interval. The warning usually means the simulation cannot keep up with wall-clock time; it is not by itself proof of a logic error.
- **No gas alert:** check the reported filtered ADC value and tune `GAS_ALARM_ON` / `GAS_ALARM_OFF` for the model's simulated range.
- **No occupancy alert:** the logic requires stable PIR motion AND a valid ultrasonic echo with distance inside the threshold.

## Safety note

This is an educational prototype and simulation. It is not a certified gas detector, fire alarm, life-safety system, or security system. Do not rely on it for real-world emergency protection.
