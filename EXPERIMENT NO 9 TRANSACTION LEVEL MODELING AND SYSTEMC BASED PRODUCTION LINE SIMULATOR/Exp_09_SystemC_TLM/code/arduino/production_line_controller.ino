#include <LiquidCrystal.h>

// -----------------------------------------------------------------------------
// EXPERIMENT 9 - PROTEUS PRODUCTION LINE CONTROLLER
// Arduino UNO + 2 x L293D + 2 x 74HC595 + LM016L LCD
//
// Assumed Proteus wiring:
// D2-D4  : three active-HIGH product sensors (LOGICSTATE)
// D5,D6,D9: L293D enable pins for conveyor motors 1, 2 and 3 (PWM)
// D7,D8,D10: START, STOP and RESET buttons, each wired to GND when pressed
// D11,D12,D13: 74HC595 DATA, CLOCK and LATCH
// A0-A5 : LCD RS, E, D4, D5, D6, D7
// D0    : buzzer (avoid Serial while connected)
// -----------------------------------------------------------------------------

const byte BUZZER_PIN = 0;  // D0/RX: no Serial use while buzzer is connected

const byte SENSOR_1_PIN = 2;
const byte SENSOR_2_PIN = 3;
const byte SENSOR_3_PIN = 4;

const byte MOTOR_1_EN = 5;
const byte MOTOR_2_EN = 6;
const byte MOTOR_3_EN = 9;

const byte START_BUTTON_PIN = 7;
const byte STOP_BUTTON_PIN  = 8;
const byte RESET_BUTTON_PIN = 10;

const byte SR_DATA_PIN  = 11;
const byte SR_CLOCK_PIN = 12;
const byte SR_LATCH_PIN = 13;

// LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

const byte MOTOR_SPEED = 170;  // PWM range: 0-255
const unsigned long DEBOUNCE_MS = 35;
const unsigned long LED_BLINK_MS = 300;
const unsigned long LCD_REFRESH_MS = 250;

struct ButtonState {
  byte pin;
  bool lastReading;
  bool stableState;
  unsigned long lastChange;
};

ButtonState startButton = {START_BUTTON_PIN, HIGH, HIGH, 0};
ButtonState stopButton  = {STOP_BUTTON_PIN, HIGH, HIGH, 0};
ButtonState resetButton = {RESET_BUTTON_PIN, HIGH, HIGH, 0};

bool lineRunning = false;
bool lastSensor3 = false;
unsigned long completedProducts = 0;
unsigned long lastLcdRefresh = 0;

bool buttonPressed(ButtonState &button, unsigned long now) {
  bool reading = digitalRead(button.pin);

  if (reading != button.lastReading) {
    button.lastChange = now;
    button.lastReading = reading;
  }

  if ((now - button.lastChange) >= DEBOUNCE_MS &&
      reading != button.stableState) {
    button.stableState = reading;
    return (button.stableState == LOW);
  }

  return false;
}

void updateMotors() {
  byte pwm = lineRunning ? MOTOR_SPEED : 0;
  analogWrite(MOTOR_1_EN, pwm);
  analogWrite(MOTOR_2_EN, pwm);
  analogWrite(MOTOR_3_EN, pwm);
}

void updateShiftRegisterLeds(bool sensor1, bool sensor2, bool sensor3,
                             bool fault, unsigned long now) {
  const bool blinkOn = ((now / LED_BLINK_MS) % 2) == 0;

  // U3 outputs: Q0-Q2 green station indicators, Q3-Q5 red buffer-full,
  // Q6-Q7 yellow processing indicators.
  byte u3 = 0;
  if (lineRunning) u3 |= (1 << 0);  // D1: Station 1 active - green
  if (lineRunning) u3 |= (1 << 1);  // D2: Station 2 active - green
  if (lineRunning) u3 |= (1 << 2);  // D3: Station 3 active - green
  if (sensor1)    u3 |= (1 << 3);   // D4: Buffer 1 full - red
  if (sensor2)    u3 |= (1 << 4);   // D5: Buffer 2 full - red
  if (sensor3)    u3 |= (1 << 5);   // D6: Buffer 3 full - red
  if (lineRunning && sensor1 && blinkOn) u3 |= (1 << 6); // D7 yellow
  if (lineRunning && sensor2 && blinkOn) u3 |= (1 << 7); // D8 yellow

  // U4 Q0: D9 yellow Station 3 processing; Q1: D10 red system fault.
  byte u4 = 0;
  if (lineRunning && sensor3 && blinkOn) u4 |= (1 << 0);
  if (fault) u4 |= (1 << 1);

  // U4 is the far register in the cascade, so transmit its byte first.
  digitalWrite(SR_LATCH_PIN, LOW);
  shiftOut(SR_DATA_PIN, SR_CLOCK_PIN, MSBFIRST, u4);
  shiftOut(SR_DATA_PIN, SR_CLOCK_PIN, MSBFIRST, u3);
  digitalWrite(SR_LATCH_PIN, HIGH);
}

void updateLcd(unsigned long now) {
  if (now - lastLcdRefresh < LCD_REFRESH_MS) return;
  lastLcdRefresh = now;

  lcd.setCursor(0, 0);
  if (lineRunning) {
    lcd.print("LINE: RUNNING   ");
  } else {
    lcd.print("LINE: STOPPED   ");
  }

  lcd.setCursor(0, 1);
  lcd.print("Done: ");
  lcd.print(completedProducts);
  lcd.print("        ");
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(SENSOR_1_PIN, INPUT);
  pinMode(SENSOR_2_PIN, INPUT);
  pinMode(SENSOR_3_PIN, INPUT);

  pinMode(MOTOR_1_EN, OUTPUT);
  pinMode(MOTOR_2_EN, OUTPUT);
  pinMode(MOTOR_3_EN, OUTPUT);

  pinMode(START_BUTTON_PIN, INPUT_PULLUP);
  pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

  pinMode(SR_DATA_PIN, OUTPUT);
  pinMode(SR_CLOCK_PIN, OUTPUT);
  pinMode(SR_LATCH_PIN, OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PRODUCTION LINE");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");

  updateMotors();
  updateShiftRegisterLeds(false, false, false, false, millis());
  delay(800);
  lcd.clear();
}

void loop() {
  const unsigned long now = millis();

  if (buttonPressed(startButton, now)) {
    lineRunning = true;
  }

  if (buttonPressed(stopButton, now)) {
    lineRunning = false;
  }

  if (buttonPressed(resetButton, now)) {
    lineRunning = false;
    completedProducts = 0;
    lastSensor3 = false;
  }

  const bool sensor1 = (digitalRead(SENSOR_1_PIN) == HIGH);
  const bool sensor2 = (digitalRead(SENSOR_2_PIN) == HIGH);
  const bool sensor3 = (digitalRead(SENSOR_3_PIN) == HIGH);

  // Count a completed product on the rising edge of sensor 3.
  if (lineRunning && sensor3 && !lastSensor3) {
    ++completedProducts;
  }
  lastSensor3 = sensor3;

  // Simple jam demonstration: all three detection inputs active together.
  const bool fault = sensor1 && sensor2 && sensor3;

  updateMotors();
  digitalWrite(BUZZER_PIN, (lineRunning && fault) ? HIGH : LOW);
  updateShiftRegisterLeds(sensor1, sensor2, sensor3, fault, now);
  updateLcd(now);
}
