/*
  EXPERIMENT 01 - SMART HOME SAFETY AND OCCUPANCY MONITORING
  Controller : Arduino UNO (ATmega328P)
  Sensors    : MQ-2 gas sensor, PIR motion sensor, ultrasonic sensor
  Output     : Buzzer

  Pin mapping:
    MQ-2 OUT          -> A0
    PIR OUT           -> D2
    Ultrasonic TRIG   -> D3
    Ultrasonic ECHO   -> D4
    Buzzer +          -> D8
    All VCC pins      -> 5V (as required by the module)
    All GND pins      -> Arduino GND (common ground)

  Notes:
    - No third-party Arduino libraries are required.
    - Thresholds are starting values for simulation, not calibrated safety limits.
    - This educational prototype is not a certified gas/fire or security alarm.
*/

#include <Arduino.h>

// --------------------------- Pin configuration ---------------------------
const uint8_t PIN_GAS     = A0;
const uint8_t PIN_PIR     = 2;
const uint8_t PIN_TRIG    = 3;
const uint8_t PIN_ECHO    = 4;
const uint8_t PIN_BUZZER  = 8;

// --------------------------- Tunable thresholds ---------------------------
// MQ-2 threshold is an ADC count (0..1023). Tune against the model's readings.
const uint16_t GAS_ALARM_ON  = 400;
const uint16_t GAS_ALARM_OFF = 350;  // lower threshold provides hysteresis

// Occupancy alert requires BOTH stable PIR motion and a nearby object.
const float DISTANCE_ENTER_CM = 50.0f;
const float DISTANCE_EXIT_CM  = 55.0f;  // hysteresis prevents rapid toggling

// --------------------------- Scheduling settings -------------------------
const unsigned long GAS_SAMPLE_MS      = 100UL;
const unsigned long PIR_SAMPLE_MS      = 20UL;
const unsigned long DISTANCE_SAMPLE_MS = 250UL;
const unsigned long REPORT_INTERVAL_MS = 1000UL;
const unsigned long ECHO_TIMEOUT_US    = 25000UL;

// --------------------------- Gas moving average ---------------------------
const uint8_t GAS_FILTER_SIZE = 5;
uint16_t gasWindow[GAS_FILTER_SIZE] = {0};
uint8_t gasWindowIndex = 0;
uint8_t gasSamplesStored = 0;
uint32_t gasWindowSum = 0;
uint16_t gasRaw = 0;
uint16_t gasAverage = 0;
bool gasAlarm = false;

// --------------------------- PIR input debounce --------------------------
const uint8_t PIR_STABLE_SAMPLES = 3;  // 3 x 20 ms = approximately 60 ms
bool pirCandidate = false;
bool motionDetected = false;
uint8_t pirConsistentSamples = 0;

// --------------------------- Distance state ------------------------------
float distanceCm = -1.0f;
bool distanceValid = false;
bool nearObject = false;
bool occupancyAlarm = false;

// --------------------------- Overall alarm state -------------------------
bool alarmActive = false;
bool buzzerOutput = false;

// Scheduling timestamps. Using millis() keeps the main loop non-blocking,
// apart from the bounded pulseIn() timeout used for ultrasonic echo timing.
unsigned long lastGasSample = 0;
unsigned long lastPirSample = 0;
unsigned long lastDistanceSample = 0;
unsigned long lastReport = 0;

// -------------------------------------------------------------------------
// Add a new MQ-2 reading to a small moving-average filter.
// -------------------------------------------------------------------------
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

  return (uint16_t)(gasWindowSum / gasSamplesStored);
}

// -------------------------------------------------------------------------
// Read the ultrasonic module. Returns -1 when no echo arrives before timeout.
// -------------------------------------------------------------------------
float readDistanceCm()
{
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  const unsigned long duration = pulseIn(PIN_ECHO, HIGH, ECHO_TIMEOUT_US);
  if (duration == 0UL)
  {
    return -1.0f;
  }

  return ((float)duration * 0.0343f) / 2.0f;
}

// -------------------------------------------------------------------------
// Apply gas alarm hysteresis to the filtered MQ-2 reading.
// -------------------------------------------------------------------------
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

// -------------------------------------------------------------------------
// Debounce PIR state. A new state is accepted only after repeated samples.
// -------------------------------------------------------------------------
void updatePirState()
{
  const bool rawMotion = (digitalRead(PIN_PIR) == HIGH);

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

// -------------------------------------------------------------------------
// Update near-object state using separate enter/exit thresholds.
// -------------------------------------------------------------------------
void updateNearObjectState()
{
  if (!distanceValid)
  {
    // Fail safely for this educational logic: no valid echo means no
    // distance-confirmed occupancy alert for this sample.
    nearObject = false;
    return;
  }

  if (!nearObject && distanceCm <= DISTANCE_ENTER_CM)
  {
    nearObject = true;
  }
  else if (nearObject && distanceCm >= DISTANCE_EXIT_CM)
  {
    nearObject = false;
  }
}

// -------------------------------------------------------------------------
// Buzzer behavior:
//   Gas alert                 -> continuous buzzer
//   Motion + nearby object    -> pulsed buzzer
//   No active alarm           -> buzzer OFF
// -------------------------------------------------------------------------
void updateBuzzer(unsigned long now)
{
  alarmActive = gasAlarm || occupancyAlarm;

  if (gasAlarm)
  {
    buzzerOutput = true;
  }
  else if (occupancyAlarm)
  {
    buzzerOutput = ((now % 500UL) < 250UL);
  }
  else
  {
    buzzerOutput = false;
  }

  digitalWrite(PIN_BUZZER, buzzerOutput ? HIGH : LOW);
}

// -------------------------------------------------------------------------
// Periodic status message. Low-rate output reduces serial/log overhead.
// -------------------------------------------------------------------------
void printStatus(unsigned long now)
{
  Serial.println(F("\n========== SMART HOME SAFETY =========="));

  Serial.print(F("Uptime (ms): "));
  Serial.println(now);

  Serial.print(F("Gas raw: "));
  Serial.print(gasRaw);
  Serial.print(F(" | filtered: "));
  Serial.print(gasAverage);
  Serial.print(F(" | alert: "));
  Serial.println(gasAlarm ? F("YES") : F("NO"));

  Serial.print(F("PIR motion: "));
  Serial.println(motionDetected ? F("DETECTED") : F("NOT DETECTED"));

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
  Serial.println((distanceValid && nearObject) ? F("YES") : F("NO / UNKNOWN"));

  Serial.print(F("Occupancy alert: "));
  Serial.println(occupancyAlarm ? F("YES") : F("NO"));

  Serial.print(F("BUZZER: "));
  Serial.println(buzzerOutput ? F("ON") : F("OFF"));

  Serial.print(F("SYSTEM STATUS: "));
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

  Serial.println(F("======================================"));
}

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
  Serial.println(F("\nSmart Home Safety & Occupancy Monitor"));
  Serial.println(F("Arduino UNO | MQ-2 + PIR + Ultrasonic"));
  Serial.println(F("Status messages every 1 second"));

  const unsigned long now = millis();
  lastGasSample = now;
  lastPirSample = now;
  lastDistanceSample = now;
  lastReport = now;
}

void loop()
{
  const unsigned long now = millis();

  // Sample MQ-2 at a controlled rate and update filtered gas alarm state.
  if (now - lastGasSample >= GAS_SAMPLE_MS)
  {
    lastGasSample = now;
    gasRaw = (uint16_t)analogRead(PIN_GAS);
    gasAverage = updateGasAverage(gasRaw);
    updateGasAlarm();
  }

  // Debounce PIR without a blocking delay.
  if (now - lastPirSample >= PIR_SAMPLE_MS)
  {
    lastPirSample = now;
    updatePirState();
  }

  // The ultrasonic read is limited to 4 times per second to reduce workload.
  if (now - lastDistanceSample >= DISTANCE_SAMPLE_MS)
  {
    lastDistanceSample = now;
    distanceCm = readDistanceCm();
    distanceValid = (distanceCm >= 0.0f);
    updateNearObjectState();
  }

  occupancyAlarm = motionDetected && distanceValid && nearObject;
  updateBuzzer(now);

  if (now - lastReport >= REPORT_INTERVAL_MS)
  {
    lastReport = now;
    printStatus(now);
  }
}
