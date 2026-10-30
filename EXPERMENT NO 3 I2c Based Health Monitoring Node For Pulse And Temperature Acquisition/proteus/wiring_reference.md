# Proteus wiring reference

## Arduino UNO ↔ LM75AD

| LM75AD pin | Connection |
|---|---|
| SDA | Arduino A4 / SDA |
| SCL | Arduino A5 / SCL |
| A0 | GND |
| A1 | GND |
| A2 | GND |
| VCC | +5 V (if exposed by the model) |
| GND | GND (if exposed by the model) |
| OS | Leave unconnected for this sketch |

With A2=A1=A0=LOW, the sketch uses 7-bit address `0x48`. If the address pins are configured differently, update `LM75_ADDRESS` in the sketch to match the resulting address. Check the exact Proteus model pinout; some models hide supply pins and use global power rails.

## Arduino UNO ↔ Heart Beat Sensor model

| Heart Beat Sensor pin | Connection |
|---|---|
| OUT | Arduino D2 |
| VCC | +5 V |
| GND | GND |
| Test Pin | LOGICSTATE set HIGH for the referenced TEP simulation model, following that model's instructions |

The firmware expects a digital pulse waveform on D2 and counts rising edges. If the model's OUT pin provides an analog waveform instead, this sketch and input circuit need to be adapted.

## Arduino UNO ↔ LM016L (parallel 4-bit mode)

| LM016L pin | Connection |
|---|---|
| VSS | GND |
| VDD | +5 V |
| VEE | Contrast input; GND is a simple initial test setting (a potentiometer is preferable for adjustment) |
| RS | Arduino D8 |
| RW | GND |
| E | Arduino D9 |
| D0–D3 | Not connected |
| D4 | Arduino D4 |
| D5 | Arduino D5 |
| D6 | Arduino D6 |
| D7 | Arduino D7 |

## Important assessment note

LM016L is a standard parallel LCD. It is not an I²C LCD merely because the temperature sensor uses I²C. If the assessment explicitly requires I²C communication with the display module as well, use an LCD with an I²C backpack/model (commonly based on PCF8574) and a different display implementation in the Arduino sketch.
