
/*
  ============================================================
  EXPERIMENT 01
  SMART HOME SAFETY AND OCCUPANCY MONITORING SYSTEM
  ============================================================

  Controller : Arduino UNO / ATmega328P
  Gas Sensor : MQ-2
  Motion     : PIR sensor
  Distance   : Ultrasonic sensor
  Alarm      : Buzzer

  PIN CONNECTIONS
  ------------------------------------------------------------
  MQ-2 OUT          -> A0
  PIR OUT           -> D2
  Ultrasonic TRIG   -> D3
  Ultrasonic ECHO   -> D4
  Buzzer Positive   -> D8
  Sensor VCC        -> 5V (check module requirements)
  All GND pins      -> Arduino GND

  FEATURES
  ------------------------------------------------------------
  1. Filtered MQ-2 analog readings
  2. Gas-alert threshold hysteresis
  3. PIR signal debouncing
  4. Scheduled ultrasonic measurements
  5. Distance hysteresis
  6. Continuous gas-warning alarm
  7. Pulsed occupancy-warning alarm
  8. Periodic Serial Monitor reports
  9. No third-party libraries

  IMPORTANT:
  Thresholds are simulation starting values, not calibrated
  gas-concentration limits or certified safety settings.
*/

// ======================== PIN CONFIGURATION =================

#include <Arduino.h>

const uint8_t PIN_GAS    = A0;
const uint8_t PIN_PIR    = 2;
const uint8_t PIN_TRIG   = 3;
const uint8_t PIN_ECHO   = 4;
const uint8_t PIN_BUZZER = 8;

// ======================== GAS THRESHOLDS ====================

// ADC readings range from 0 to 1023 on the Arduino UNO.
// Adjust these values after observing your Proteus model.

const uint16_t GAS_ALARM_ON  = 400;
const uint16_t GAS_ALARM_OFF = 350;

// ======================== DISTANCE SETTINGS =================

const float DISTANCE_ENTER_CM = 50.0;
const float DISTANCE_EXIT_CM  = 55.0;

// ======================== TIMING SETTINGS ====================

const unsigned long GAS_SAMPLE_MS      = 100;
const unsigned long PIR_SAMPLE_MS      = 20;
const unsigned long DISTANCE_SAMPLE_MS = 250;
const unsigned long REPORT_INTERVAL_MS = 1000;
const unsigned long ECHO_TIMEOUT_US    = 25000;

// ======================== GAS FILTER ========================

const uint8_t GAS_FILTER_SIZE = 5;

uint16_t gasWindow[GAS_FILTER_SIZE] = {0};

uint8_t gasWindowIndex = 0;
uint8_t gasSamplesStored = 0;

uint32_t gasWindowSum = 0;

uint16_t gasRaw = 0;
uint16_t gasAverage = 0;

bool gasAlarm = false;

// ======================== PIR DEBOUNCE ======================

const uint8_t PIR_STABLE_SAMPLES = 3;

bool pirCandidate = false;
bool motionDetected = false;

uint8_t pirConsistentSamples = 0;

// ======================== DISTANCE STATE ====================

float distanceCm = -1.0;

bool distanceValid = false;
bool nearObject = false;

bool occupancyAlarm = false;

// ======================== ALARM STATE =======================

bool alarmActive = false;
bool buzzerOutput = false;

// ======================== SCHEDULER =========================

unsigned long lastGasSample = 0;
unsigned long lastPirSample = 0;
unsigned long lastDistanceSample = 0;
unsigned long lastReport = 0;

// ============================================================
// GAS SENSOR MOVING-AVERAGE FILTER
// ============================================================

uint16_t updateGasAverage(uint16_t reading)
{
  if (gasSamplesStored < GAS_FILTER_SIZE)
  {
    gasWindow[gasWindowIndex] = reading;

    gasWindowSum += reading;
    gasSamplesStored++;
  }
  else
  {
    gasWindowSum -= gasWindow[gasWindowIndex];

    gasWindow[gasWindowIndex] = reading;
    gasWindowSum += reading;
  }

  gasWindowIndex++;

  if (gasWindowIndex >= GAS_FILTER_SIZE)
  {
    gasWindowIndex = 0;
  }

  if (gasSamplesStored == 0)
  {
    return reading;
  }

  return gasWindowSum / gasSamplesStored;
}

// ============================================================
// ULTRASONIC DISTANCE MEASUREMENT
// ============================================================

float readDistanceCm()
{
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(PIN_TRIG, LOW);

  unsigned long duration =
      pulseIn(PIN_ECHO, HIGH, ECHO_TIMEOUT_US);

  if (duration == 0)
  {
    return -1.0;
  }

  float distance =
      ((float)duration * 0.0343) / 2.0;

  return distance;
}

// ============================================================
// GAS ALARM WITH HYSTERESIS
// ============================================================

void updateGasAlarm()
{
  if (!gasAlarm && gasAverage >= GAS_ALARM_ON)
  {
    gasAlarm = true;
  }
  else if (gasAlarm && gasAverage <= GAS_ALARM_OFF)
  {
    gasAlarm = false;
  }
}

// ============================================================
// PIR INPUT DEBOUNCING
// ============================================================

void updatePirState()
{
  bool rawMotion =
      (digitalRead(PIN_PIR) == HIGH);

  if (rawMotion == pirCandidate)
  {
    if (pirConsistentSamples < PIR_STABLE_SAMPLES)
    {
      pirConsistentSamples++;
    }
  }
  else
  {
    pirCandidate = rawMotion;
    pirConsistentSamples = 1;
  }

  if (pirConsistentSamples >= PIR_STABLE_SAMPLES)
  {
    motionDetected = pirCandidate;
  }
}

// ============================================================
// NEARBY-OBJECT DETECTION WITH DISTANCE HYSTERESIS
// ============================================================

void updateNearObjectState()
{
  if (!distanceValid)
  {
    nearObject = false;
    return;
  }

  if (!nearObject &&
      distanceCm <= DISTANCE_ENTER_CM)
  {
    nearObject = true;
  }
  else if (nearObject &&
           distanceCm >= DISTANCE_EXIT_CM)
  {
    nearObject = false;
  }
}

// ============================================================
// BUZZER CONTROL
// ============================================================

void updateBuzzer(unsigned long now)
{
  alarmActive = gasAlarm || occupancyAlarm;

  // Gas alert takes priority and activates the buzzer
  // continuously.

  if (gasAlarm)
  {
    buzzerOutput = true;
  }
  // Occupancy-only alert produces an intermittent alarm.
  else if (occupancyAlarm)
  {
    buzzerOutput = ((now % 500) < 250);
  }
  else
  {
    buzzerOutput = false;
  }

  digitalWrite(
      PIN_BUZZER,
      buzzerOutput ? HIGH : LOW
  );
}

// ============================================================
// SERIAL STATUS REPORT
// ============================================================

void printStatus(unsigned long now)
{
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("     SMART HOME SAFETY MONITOR"));
  Serial.println(F("========================================"));

  Serial.print(F("Uptime: "));
  Serial.print(now);
  Serial.println(F(" ms"));

  Serial.print(F("Gas raw reading: "));
  Serial.println(gasRaw);

  Serial.print(F("Gas filtered: "));
  Serial.println(gasAverage);

  Serial.print(F("Gas alert: "));
  Serial.println(gasAlarm ? F("YES") : F("NO"));

  Serial.print(F("PIR motion: "));
  Serial.println(
      motionDetected
          ? F("DETECTED")
          : F("NOT DETECTED")
  );

  Serial.print(F("Distance: "));

  if (distanceValid)
  {
    Serial.print(distanceCm, 1);
    Serial.println(F(" cm"));
  }
  else
  {
    Serial.println(F("NO ECHO"));
  }

  Serial.print(F("Nearby object: "));
  Serial.println(
      (distanceValid && nearObject)
          ? F("YES")
          : F("NO / UNKNOWN")
  );

  Serial.print(F("Occupancy alert: "));
  Serial.println(
      occupancyAlarm ? F("YES") : F("NO")
  );

  Serial.print(F("Buzzer: "));
  Serial.println(
      buzzerOutput ? F("ON") : F("OFF")
  );

  Serial.print(F("System status: "));

  if (gasAlarm && occupancyAlarm)
  {
    Serial.println(F("MULTIPLE ALERTS"));
  }
  else if (gasAlarm)
  {
    Serial.println(F("GAS WARNING"));
  }
  else if (occupancyAlarm)
  {
    Serial.println(F("OCCUPANCY WARNING"));
  }
  else
  {
    Serial.println(F("NORMAL"));
  }

  Serial.println(F("========================================"));
}

// ============================================================
// SETUP
// ============================================================

void setup()
{
  pinMode(PIN_GAS, INPUT);
  pinMode(PIN_PIR, INPUT);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  pinMode(PIN_BUZZER, OUTPUT);

  digitalWrite(PIN_TRIG, LOW);
  digitalWrite(PIN_BUZZER, LOW);

  Serial.begin(9600);

  Serial.println();
  Serial.println(F("Smart Home Safety System"));
  Serial.println(F("Arduino UNO"));
  Serial.println(F("MQ-2 + PIR + Ultrasonic"));
  Serial.println(F("System initialized"));

  unsigned long now = millis();

  lastGasSample = now;
  lastPirSample = now;
  lastDistanceSample = now;
  lastReport = now;
}

// ============================================================
// MAIN LOOP
// ============================================================

void loop()
{
  unsigned long now = millis();

  // 1. SAMPLE GAS SENSOR

  if (now - lastGasSample >= GAS_SAMPLE_MS)
  {
    lastGasSample = now;

    gasRaw = analogRead(PIN_GAS);

    gasAverage = updateGasAverage(gasRaw);

    updateGasAlarm();
  }

  // 2. READ AND DEBOUNCE PIR

  if (now - lastPirSample >= PIR_SAMPLE_MS)
  {
    lastPirSample = now;

    updatePirState();
  }

  // 3. MEASURE ULTRASONIC DISTANCE

  if (now - lastDistanceSample >=
      DISTANCE_SAMPLE_MS)
  {
    lastDistanceSample = now;

    distanceCm = readDistanceCm();

    distanceValid = (distanceCm >= 0.0);

    updateNearObjectState();
  }

  // 4. CONFIRM OCCUPANCY ALERT
  // Both motion and a valid nearby-object measurement
  // must be present.

  occupancyAlarm =
      motionDetected &&
      distanceValid &&
      nearObject;

  // 5. UPDATE ALARM OUTPUT

  updateBuzzer(now);

  // 6. REPORT STATUS ONCE PER SECOND

  if (now - lastReport >= REPORT_INTERVAL_MS)
  {
    lastReport = now;

    printStatus(now);
  }
}
