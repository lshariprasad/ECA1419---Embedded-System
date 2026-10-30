/*
  EXPERIMENT 3
  I2C-BASED HEALTH MONITORING NODE FOR PULSE AND TEMPERATURE ACQUISITION

  Board: Arduino UNO (ATmega328P)

  LM75AD temperature sensor (I2C):
    SDA -> Arduino A4
    SCL -> Arduino A5
    A0, A1, A2 -> GND (I2C address 0x48)
    VCC -> +5V, GND -> GND

  Heart Beat Sensor (older TEP Proteus model):
    OUT -> Arduino D2 (digital pulse output for this library model)
    VCC -> +5V
    GND -> GND
    Test Pin -> LOGICSTATE set HIGH in Proteus to simulate a finger

  LM016L 16x2 LCD in 4-bit parallel mode:
    VSS -> GND, VDD -> +5V, VEE -> contrast (GND for simple testing)
    RS -> D8, RW -> GND, E -> D9
    LCD D4 -> Arduino D4
    LCD D5 -> Arduino D5
    LCD D6 -> Arduino D6
    LCD D7 -> Arduino D7
    LCD D0-D3 unused

  NOTE:
  The LM75AD uses I2C. The LM016L shown in the schematic is a parallel LCD,
  not an I2C-backpack LCD. This sketch uses I2C for temperature acquisition
  and the parallel LCD interface for display.
  Educational prototype only; not a medical device.
*/

#include <Wire.h>
#include <LiquidCrystal.h>

// ---------------------- Pin assignments ----------------------
const uint8_t PULSE_PIN = 2;
const uint8_t LCD_RS_PIN = 8;
const uint8_t LCD_EN_PIN = 9;
const uint8_t LCD_D4_PIN = 4;
const uint8_t LCD_D5_PIN = 5;
const uint8_t LCD_D6_PIN = 6;
const uint8_t LCD_D7_PIN = 7;

// LM75AD address when A2=A1=A0=LOW
const uint8_t LM75_ADDRESS = 0x48;
const uint8_t LM75_TEMP_REGISTER = 0x00;

LiquidCrystal lcd(LCD_RS_PIN, LCD_EN_PIN,
                  LCD_D4_PIN, LCD_D5_PIN,
                  LCD_D6_PIN, LCD_D7_PIN);

// ---------------------- Scheduling ---------------------------
const unsigned long TEMP_UPDATE_MS = 1000UL;
const unsigned long LCD_UPDATE_MS = 500UL;
const unsigned long SERIAL_UPDATE_MS = 1000UL;
const unsigned long MIN_BEAT_INTERVAL_MS = 300UL;  // reject >200 BPM
const unsigned long MAX_BEAT_INTERVAL_MS = 2000UL; // allow down to 30 BPM
const unsigned long BEAT_TIMEOUT_MS = 3000UL;

unsigned long lastTempUpdate = 0;
unsigned long lastLcdUpdate = 0;
unsigned long lastSerialUpdate = 0;
unsigned long lastBeatTime = 0;

bool lastPulseLevel = false;
bool temperatureValid = false;
float temperatureC = 0.0f;
uint16_t bpm = 0;

// Moving average of recent beat-to-beat intervals
const uint8_t INTERVAL_SAMPLES = 4;
unsigned long beatIntervals[INTERVAL_SAMPLES] = {0};
uint8_t intervalIndex = 0;
uint8_t intervalsStored = 0;
unsigned long intervalSum = 0;

// ---------------------- LM75AD reading ----------------------
bool readLM75Temperature(float &tempC)
{
  Wire.beginTransmission(LM75_ADDRESS);
  Wire.write(LM75_TEMP_REGISTER);

  // Repeated start for register read
  if (Wire.endTransmission(false) != 0)
  {
    return false;
  }

  uint8_t received = Wire.requestFrom(LM75_ADDRESS, (uint8_t)2);
  if (received != 2 || Wire.available() < 2)
  {
    while (Wire.available()) (void)Wire.read();
    return false;
  }

  uint8_t msb = (uint8_t)Wire.read();
  uint8_t lsb = (uint8_t)Wire.read();

  // LM75A temperature data: signed 11-bit two's-complement value,
  // left-aligned in the 16-bit register, with 0.125 C resolution.
  uint16_t registerValue = ((uint16_t)msb << 8) | lsb;
  int16_t raw11 = (int16_t)((registerValue >> 5) & 0x07FF);

  // Sign-extend the 11-bit value.
  if (raw11 & 0x0400)
  {
    raw11 -= 0x0800;
  }

  tempC = (float)raw11 * 0.125f;
  return true;
}

// ---------------------- Pulse detection ----------------------
void clearBeatAverage()
{
  intervalIndex = 0;
  intervalsStored = 0;
  intervalSum = 0;
  for (uint8_t i = 0; i < INTERVAL_SAMPLES; i++)
  {
    beatIntervals[i] = 0;
  }
  bpm = 0;
}

void addBeatInterval(unsigned long intervalMs)
{
  if (intervalsStored == INTERVAL_SAMPLES)
  {
    intervalSum -= beatIntervals[intervalIndex];
  }
  else
  {
    intervalsStored++;
  }

  beatIntervals[intervalIndex] = intervalMs;
  intervalSum += intervalMs;
  intervalIndex = (intervalIndex + 1) % INTERVAL_SAMPLES;

  if (intervalSum > 0 && intervalsStored > 0)
  {
    unsigned long averageInterval = intervalSum / intervalsStored;
    bpm = (uint16_t)(60000UL / averageInterval);
  }
}

void updatePulseRate(unsigned long now)
{
  bool pulseLevel = (digitalRead(PULSE_PIN) == HIGH);

  // Count only rising edges to avoid counting both HIGH and LOW edges.
  if (pulseLevel && !lastPulseLevel)
  {
    if (lastBeatTime == 0)
    {
      lastBeatTime = now;
    }
    else
    {
      unsigned long intervalMs = now - lastBeatTime;

      if (intervalMs >= MIN_BEAT_INTERVAL_MS &&
          intervalMs <= MAX_BEAT_INTERVAL_MS)
      {
        addBeatInterval(intervalMs);
        lastBeatTime = now;
      }
      else if (intervalMs > MAX_BEAT_INTERVAL_MS)
      {
        // Long gap: restart averaging from this beat.
        clearBeatAverage();
        lastBeatTime = now;
      }
      // Intervals < MIN_BEAT_INTERVAL_MS are ignored as noise.
    }
  }

  lastPulseLevel = pulseLevel;

  if (lastBeatTime != 0 && (now - lastBeatTime > BEAT_TIMEOUT_MS))
  {
    clearBeatAverage();
    lastBeatTime = 0;
  }
}

// ---------------------- LCD display --------------------------
void updateLCD()
{
  lcd.setCursor(0, 0);
  if (temperatureValid)
  {
    lcd.print(F("Temp: "));
    lcd.print(temperatureC, 1);
    lcd.print((char)223);
    lcd.print(F("C     "));
  }
  else
  {
    lcd.print(F("Temp: SENSOR ERR"));
  }

  lcd.setCursor(0, 1);
  lcd.print(F("Pulse: "));
  if (bpm == 0)
  {
    lcd.print(F("-- BPM       "));
  }
  else
  {
    lcd.print(bpm);
    lcd.print(F(" BPM       "));
  }
}

// ---------------------- Setup --------------------------------
void setup()
{
  pinMode(PULSE_PIN, INPUT);

  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(100000UL); // standard I2C clock

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Health Monitor"));
  lcd.setCursor(0, 1);
  lcd.print(F("Starting..."));

  delay(800);

  unsigned long now = millis();
  lastTempUpdate = now - TEMP_UPDATE_MS;
  lastLcdUpdate = now - LCD_UPDATE_MS;
  lastSerialUpdate = now - SERIAL_UPDATE_MS;

  Serial.println(F("Experiment 3: I2C Health Monitoring Node"));
  Serial.println(F("LM75AD temperature + pulse sensor"));
  Serial.println(F("Check wiring and set heartbeat Test Pin HIGH in Proteus."));
}

// ---------------------- Main loop ----------------------------
void loop()
{
  unsigned long now = millis();

  // Read pulse transitions as frequently as possible.
  updatePulseRate(now);

  // Poll the LM75AD periodically to avoid unnecessary I2C traffic.
  if (now - lastTempUpdate >= TEMP_UPDATE_MS)
  {
    lastTempUpdate = now;
    temperatureValid = readLM75Temperature(temperatureC);
  }

  if (now - lastLcdUpdate >= LCD_UPDATE_MS)
  {
    lastLcdUpdate = now;
    updateLCD();
  }

  if (now - lastSerialUpdate >= SERIAL_UPDATE_MS)
  {
    lastSerialUpdate = now;

    Serial.print(F("Temperature: "));
    if (temperatureValid)
    {
      Serial.print(temperatureC, 2);
      Serial.println(F(" C"));
    }
    else
    {
      Serial.println(F("LM75AD not detected - check I2C and address pins"));
    }

    Serial.print(F("Pulse rate: "));
    if (bpm == 0)
    {
      Serial.println(F("waiting for valid beats"));
    }
    else
    {
      Serial.print(bpm);
      Serial.println(F(" BPM"));
    }
    Serial.println(F("------------------------------"));
  }
}
