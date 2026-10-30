# Experiment 4 — Industrial HVAC Control Firmware

**ADC Temperature Acquisition and Timer-Interrupt PWM Fan Control**

<img width="1919" height="1019" alt="Screenshot 2026-10-30 083638" src="https://github.com/user-attachments/assets/ff3f7d86-0f09-4631-8312-137b0c279b92" />

<img width="1919" height="1021" alt="Screenshot 2026-10-30 083723" src="https://github.com/user-attachments/assets/067dd92a-96df-42f7-b88d-aab73767b109" />

<img width="1919" height="1020" alt="Screenshot 2026-10-30 083741" src="https://github.com/user-attachments/assets/e7e59d47-52bd-49a7-8b11-5d7a9979852b" />

<img width="1919" height="1019" alt="Screenshot 2026-10-30 083804" src="https://github.com/user-attachments/assets/1559c1cd-5bd7-41fc-9068-c9a48578159d" />


This repository contains an Arduino UNO prototype that reads an LM35 analog temperature sensor and controls a DC fan through an IRLZ44N N-channel MOSFET. Timer1 provides hardware PWM on D9 and an overflow interrupt schedules the temperature-control task.

## Repository contents

```text
Exp_04_Industrial_HVAC_Control/
├── README.md
├── LICENSE
├── .gitignore
├── code/
│   ├── IndustrialHVACController.ino
│   └── README.md
├── proteus/
│   ├── README.md
│   └── wiring_reference.md
└── output/
    ├── README.md
    ├── schematic_reference.png
    ├── system_block_diagram.svg
    ├── test_results.csv
    ├── serial_output_template.txt
    └── experiment_report_template.md
```

## Hardware / simulation components

- Arduino UNO / ATmega328P, 16 MHz
- LM35 analog temperature sensor
- IRLZ44N logic-level N-channel MOSFET
- DC motor / fan with an appropriately rated external supply (12 V in the current schematic)
- 220 Ω MOSFET gate series resistor
- 10 kΩ MOSFET gate pull-down resistor
- Flyback diode across a brushed DC motor, with cathode at motor positive and anode at motor negative / MOSFET drain

## Pin connections

| Signal / component | Connection |
|---|---|
| LM35 pin 1 (+Vs) | Arduino 5V |
| LM35 pin 2 (VOUT) | Arduino A0 |
| LM35 pin 3 (GND) | Arduino GND |
| Arduino D9 / OC1A | 220 Ω series resistor → MOSFET Gate |
| 10 kΩ pull-down | MOSFET Gate → GND |
| MOSFET Drain | Fan / motor negative |
| MOSFET Source | Common GND |
| Fan / motor positive | External +12 V supply (verify fan rating) |
| External supply negative | Common GND with Arduino and MOSFET Source |
| Flyback diode cathode (band) | Motor positive |
| Flyback diode anode | Motor negative / MOSFET Drain |

**Power and safety:** Do not power the fan from an Arduino GPIO pin or the Arduino 5 V pin. Use a supply rated for the motor's voltage and current. Join the external supply negative to Arduino GND. Verify MOSFET pin orientation from the exact Proteus model / datasheet. The flyback-diode recommendation applies to a brushed inductive DC motor; a brushless fan may contain internal electronics and may require a different driver/interface. Never assume a generic diode is adequate for every fan.

## Firmware design

- Reads the LM35 through the UNO's 10-bit ADC (A0).
- Averages eight ADC samples per acquisition.
- Converts the measured voltage using the LM35 nominal 10 mV/°C scale and a configured 5.00 V ADC reference.
- Applies a first-order low-pass filter.
- Uses five fan stages with hysteresis to reduce rapid toggling.
- Configures Timer1 Fast PWM mode 14, `ICR1 = 1999`, prescaler 8, giving a nominal PWM frequency of 1 kHz at 16 MHz.
- Uses Timer1 overflow interrupts to schedule control work at 100 ms intervals. ADC reads and control calculations occur in `loop()`, not inside the ISR.
- Outputs hardware PWM on D9 / OC1A.

### Example temperature stages

| Stage | Entry / transition condition | PWM duty |
|---|---|---:|
| OFF → LOW | Temperature ≥ 26.0 °C | 30% |
| LOW → OFF | Temperature < 24.5 °C | 0% |
| LOW → MEDIUM | Temperature ≥ 31.0 °C | 50% |
| MEDIUM → LOW | Temperature < 29.5 °C | 30% |
| MEDIUM → HIGH | Temperature ≥ 36.0 °C | 75% |
| HIGH → MEDIUM | Temperature < 34.5 °C | 50% |
| HIGH → MAX | Temperature ≥ 41.0 °C | 100% |
| MAX → HIGH | Temperature < 39.5 °C | 75% |

The stage thresholds are example educational settings and can be changed in the sketch to meet the instructor's requirements.

## Build and simulate

1. Open `code/IndustrialHVACController.ino` in Arduino IDE.
2. Select **Arduino UNO** as the board.
3. Click **Verify**.
4. Use **Sketch → Export Compiled Binary** to generate a HEX file.
5. Open the actual Proteus project and double-click the Arduino UNO.
6. Set the Arduino **Program File** to the exported `.ino.standard.hex` file.
7. Confirm the LM35, MOSFET, fan, supply polarity, common ground and diode orientation against `proteus/wiring_reference.md`.
8. Run the simulation and record observed results in `output/test_results.csv`.

## Timer1 resource note

This firmware takes ownership of Timer1. Do not add libraries or code that reconfigure Timer1 (for example, the standard Servo library) or use `analogWrite()` on D9/D10 at the same time. The D9 PWM output is generated directly by the Timer1 hardware.

## Validation status

The included schematic image is a **reference screenshot**, not an editable Proteus project. The native Proteus `.pdsprj` file and its associated project files have not been included yet. Copy your actual Proteus project files into `proteus/` before publishing. Test-results and report files are templates; enter only measurements observed in your own simulation or hardware test.

## Limitations

This is a teaching prototype, not certified HVAC, industrial safety, or personnel-protection firmware. ADC calibration, thermal sensor placement, motor driver thermal/current ratings, EMC, fault handling and independent safety interlocks would be required for a real industrial deployment.
