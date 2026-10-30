# Firmware

Main source: `IndustrialHVACController.ino`.

## Target
- Arduino UNO / ATmega328P at 16 MHz
- LM35 output on A0
- Timer1 PWM output on D9 (OC1A)
- Serial diagnostics at 9600 baud

## Compile
Open the sketch in Arduino IDE, select **Arduino UNO**, click **Verify**, then use **Sketch → Export Compiled Binary**. Load the generated `.ino.standard.hex` into the Arduino component's **Program File** property in Proteus.

## Important Timer1 constraint
The sketch directly configures Timer1. Do not combine it with a library or another feature that uses Timer1, and do not use `analogWrite()` on D9/D10.

The LM35 conversion assumes a 5.00 V ADC reference and nominal 10 mV/°C output. Calibrate if AVcc differs materially from 5.00 V.
