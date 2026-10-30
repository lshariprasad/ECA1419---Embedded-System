# Proteus wiring reference

This wiring map matches the intended circuit discussed for Experiment 4. Confirm actual MOSFET symbol pin numbers / pin labels in Proteus before wiring; graphical orientation varies by symbol.

| Part / pin | Connect to |
|---|---|
| LM35 pin 1 (+Vs) | Arduino 5V |
| LM35 pin 2 (VOUT) | Arduino A0 |
| LM35 pin 3 (GND) | Arduino GND |
| Arduino D9 / OC1A | R1 = 220 Ω, then MOSFET Gate |
| R2 = 10 kΩ | MOSFET Gate to common GND (pull-down) |
| MOSFET Drain | Fan / motor negative |
| MOSFET Source | Common GND |
| Fan / motor positive | External supply +12 V (only if the motor is rated for 12 V) |
| Supply negative | Common GND |
| Diode cathode (banded end) | Fan / motor positive |
| Diode anode | Fan / motor negative / MOSFET Drain |

## Checks before running

1. Verify IRLZ44N Gate, Drain and Source by its datasheet / Proteus pin labels; do not infer pin functions from a rotated symbol alone.
2. Confirm 12 V is applied only to the motor branch, not to Arduino 5 V or an I/O pin.
3. Join external supply negative, Arduino GND and MOSFET Source to one common reference.
4. Confirm the diode is across the motor in reverse bias during normal operation: cathode to motor positive, anode to motor negative.
5. Check the fan model type. A brushed DC motor and a brushless 2-wire / 3-wire fan may need different simulation models and suppression / drive arrangements.
6. Verify the MOSFET is a suitable logic-level device at 5 V gate drive and that its current, thermal and safe-operating-area ratings are sufficient.

## Expected control input and output

- LM35 analog output → A0.
- Timer1 Fast PWM output → D9 / OC1A.
- PWM frequency configured by the firmware: 1 kHz (16 MHz clock, prescaler 8, TOP 1999).
- Temperature-control task is scheduled every 100 ms by the Timer1 overflow ISR; ADC conversion and state updates execute in the main loop.
