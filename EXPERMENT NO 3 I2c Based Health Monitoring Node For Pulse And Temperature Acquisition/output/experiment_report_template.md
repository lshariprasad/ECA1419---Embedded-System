# Experiment 3 — I²C-Based Health Monitoring Node for Pulse and Temperature Acquisition

## Aim
To design and simulate an embedded health-monitoring node that acquires temperature from an LM75AD sensor over I²C and estimates pulse rate from a simulated digital pulse signal.

## Components
- Arduino UNO
- LM75AD temperature sensor
- Heart Beat Sensor Proteus model
- LM016L 16×2 LCD (parallel 4-bit interface in the supplied schematic)
- Required Proteus power and ground terminals / stimulus component

## Connections
Refer to `../proteus/wiring_reference.md`.

## Principle of operation
The Arduino reads the LM75AD temperature register over the I²C bus. The heartbeat input is monitored on D2; rising-edge intervals are averaged and converted to an estimated BPM. Temperature and pulse estimates are displayed on the LM016L LCD, and diagnostic information is sent through the serial interface.

## Procedure
1. Assemble the circuit in Proteus using the wiring reference.
2. Compile `../code/HealthMonitoringNode.ino` for Arduino UNO.
3. Load the generated HEX file into the Arduino UNO component.
4. Configure the Heart Beat Sensor model's own Program File and Test Pin stimulus as required by its library.
5. Start the simulation and observe the LCD and Serial Monitor.
6. Record actual readings and outcomes in `test_results.csv`.

## Observations
| Parameter | Observed value | Notes |
|---|---|---|
| LM75AD temperature | To be recorded | °C |
| Pulse rate | To be recorded | BPM |
| I²C address response | To be recorded | Expected address 0x48 when A2:A0=000 |

## Result
To be completed after the simulation has been run and verified. Do not claim success before recording actual results.

## Limitations
- The Proteus heartbeat model produces simulated signals; the displayed BPM is not a clinical measurement.
- LM75AD measures its own sensor temperature, not necessarily human body temperature.
- The shown LM016L is a parallel LCD. It does not itself communicate over I²C. If the lab requires the display module to use I²C, replace it with an I²C-backpack LCD and revise the firmware.

## Reflection
To be completed by the student after testing.
