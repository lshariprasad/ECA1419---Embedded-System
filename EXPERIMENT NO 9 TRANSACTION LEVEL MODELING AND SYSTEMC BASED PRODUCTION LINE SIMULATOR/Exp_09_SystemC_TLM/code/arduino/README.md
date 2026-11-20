# Arduino UNO controller

Open `production_line_controller.ino` in Arduino IDE, install/select the Arduino AVR Boards package, select **Arduino Uno**, then use Verify. The sketch requires the built-in `LiquidCrystal` library.

The pin map in the sketch must match your Proteus wiring. In particular, D0 is assigned to the buzzer, so do not use Serial while it is connected. If the selected buzzer needs more current than a GPIO can safely supply, use a transistor driver.

This package does not contain a claimed Arduino compile result; compile with Arduino IDE on the user's Windows machine.
