# Proteus wiring reference

## Components

- Arduino UNO
- `LED-RED`, `LED-YELLOW`, `LED-GREEN` (or equivalent LED models)
- Three `RES` resistors set to `220R`
- GROUND terminal

## Wiring

1. Arduino D8 → one end of R1 (220 Ω); R1 other end → red LED anode.
2. Arduino D9 → one end of R2 (220 Ω); R2 other end → yellow LED anode.
3. Arduino D10 → one end of R3 (220 Ω); R3 other end → green LED anode.
4. Connect all LED cathodes to a common ground net, and place a GROUND terminal on that net.
5. Save the Proteus project. Make sure the program file for the Arduino component points to the compiled HEX created from `../code/TrafficLightController.ino`.

## Polarity reminder

Use the LED symbol's anode/cathode markers. The resistor may appear before or after the LED in a series branch, but every LED needs its own series resistor. Do not connect an LED directly to an Arduino output without a current-limiting resistor.
