# Arduino firmware

`HealthMonitoringNode.ino` is the Arduino UNO sketch for the Experiment 3 circuit.

## Required board

Arduino UNO (ATmega328P).

## Libraries

- `Wire.h` — I²C communication with the LM75AD.
- `LiquidCrystal.h` — 4-bit parallel control of the LM016L LCD.

Both are part of the standard Arduino AVR environment; no third-party library is required.

## Compile

1. Open the `.ino` file in Arduino IDE.
2. Select **Arduino Uno**.
3. Click Verify.
4. Select **Sketch → Export Compiled Binary**.
5. Load the generated `.ino.standard.hex` into the Arduino UNO's Program File property in Proteus.

The Heart Beat Sensor Proteus model may require its own separate model HEX. That is not the Arduino firmware and must be configured in the sensor component's Program File property.
