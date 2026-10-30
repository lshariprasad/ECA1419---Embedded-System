/*
  EXPERIMENT 4
  INDUSTRIAL HVAC CONTROL FIRMWARE
  ADC TEMPERATURE ACQUISITION + TIMER1 INTERRUPT PWM FAN CONTROL

  Target: Arduino UNO / ATmega328P / 16 MHz

  CONNECTIONS (matching the supplied Proteus schematic):
    LM35 pin 1 (+Vs)  -> Arduino 5V
    LM35 pin 2 (VOUT) -> Arduino A0
    LM35 pin 3 (GND)  -> Arduino GND

    Arduino D9 / OC1A -> R1 220 ohm -> IRLZ44N Gate
    R2 10k ohm: MOSFET Gate -> GND (pull-down)
    MOSFET Drain      -> DC motor/fan negative
    MOSFET Source     -> common GND
    Fan positive      -> external +12V supply
    External supply - -> common GND (Arduino GND + MOSFET Source)

    Flyback diode across a brushed DC motor:
      Cathode / stripe -> motor positive (+12V)
      Anode             -> motor negative / MOSFET Drain

  TIMER1 DESIGN:
    Fast PWM mode 14, TOP = ICR1 = 1999, prescaler = 8
    PWM frequency = 16 MHz / (8 * (1999 + 1)) = 1 kHz
    OC1A hardware output is Arduino D9.
    TIMER1_OVF interrupt occurs at the PWM period rate (~1 kHz).
    ISR creates a 100 ms control-task flag; ADC and control work
    are carried out in loop(), NOT inside the interrupt.

  IMPORTANT:
    - Timer1 is reserved by this firmware. Do not use Servo library
      or analogWrite() on D9/D10 in this project.
    - The LM35 conversion assumes a 5.00 V ADC reference. Adjust
      ADC_REFERENCE_MV if the actual AVcc/reference differs.
    - This is an educational prototype, not certified HVAC or
      personnel-protection firmware.
*/

#include <Arduino.h>
#include <avr/interrupt.h>

// ---------------- PIN ASSIGNMENTS ----------------
const uint8_t LM35_PIN = A0;
const uint8_t FAN_PWM_PIN = 9;  // OC1A, Timer1 hardware PWM

// ---------------- ADC / LM35 CONFIGURATION ----------------
const float ADC_REFERENCE_MV = 5000.0f;
const uint8_t ADC_SAMPLES = 8;
const float LM35_MV_PER_C = 10.0f;

// Low-pass filter: smaller alpha = smoother but slower response.
const float TEMP_FILTER_ALPHA = 0.25f;

// ---------------- TIMER1 / PWM CONFIGURATION ----------------
const uint16_t TIMER1_TOP = 1999;   // 1 kHz at 16 MHz / prescaler 8
const uint8_t CONTROL_TICK_DIVIDER = 100; // 100 x 1 ms = 100 ms

volatile bool controlTaskDue = false;
volatile uint8_t timer1TickDivider = 0;

// ---------------- FAN CONTROL LEVELS ----------------
enum FanStage : uint8_t {
  FAN_OFF = 0,
  FAN_LOW,
  FAN_MEDIUM,
  FAN_HIGH,
  FAN_MAX
};

FanStage fanStage = FAN_OFF;
uint8_t fanDutyPercent = 0;

// Thresholds use hysteresis to reduce rapid switching.
const float OFF_TO_LOW_C = 26.0f;
const float LOW_TO_OFF_C = 24.5f;
const float LOW_TO_MED_C = 31.0f;
const float MED_TO_LOW_C = 29.5f;
const float MED_TO_HIGH_C = 36.0f;
const float HIGH_TO_MED_C = 34.5f;
const float HIGH_TO_MAX_C = 41.0f;
const float MAX_TO_HIGH_C = 39.5f;

// ---------------- MEASUREMENTS / STATUS ----------------
uint16_t adcRawAverage = 0;
float temperatureInstantC = 0.0f;
float temperatureFilteredC = 0.0f;
bool temperatureInitialized = false;

unsigned long lastSerialReportMs = 0;
const unsigned long SERIAL_REPORT_INTERVAL_MS = 1000;

// Timer1 overflow ISR: runs once per PWM period (~1 ms).
// Keep this ISR short; no ADC reads, Serial, floating point or delays here.
ISR(TIMER1_OVF_vect) {
  timer1TickDivider++;
  if (timer1TickDivider >= CONTROL_TICK_DIVIDER) {
    timer1TickDivider = 0;
    controlTaskDue = true;
  }
}

// Configure Timer1 for 1 kHz Fast PWM, mode 14, output OC1A (D9).
void configureTimer1PWM() {
  pinMode(FAN_PWM_PIN, OUTPUT);
  digitalWrite(FAN_PWM_PIN, LOW);

  uint8_t oldSREG = SREG;
  cli();

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  ICR1 = TIMER1_TOP;
  OCR1A = 0;

  // Mode 14: WGM13:WGM10 = 1110 (Fast PWM, TOP = ICR1).
  // Initially disconnect OC1A so the fan remains OFF.
  TCCR1A = _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11); // prescaler 8

  // Clear any pending overflow flag and enable Timer1 overflow interrupt.
  TIFR1 = _BV(TOV1);
  TIMSK1 |= _BV(TOIE1);

  SREG = oldSREG;
}

// Set hardware PWM duty on D9. Zero percent disconnects OC1A and drives LOW.
void setFanDuty(uint8_t dutyPercent) {
  if (dutyPercent > 100) dutyPercent = 100;
  fanDutyPercent = dutyPercent;

  if (dutyPercent == 0) {
    // Disconnect compare output and force pin low for a true fan-OFF state.
    TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0));
    OCR1A = 0;
    digitalWrite(FAN_PWM_PIN, LOW);
    return;
  }

  // One count above TOP gives continuous HIGH (100% duty) in Fast PWM.
  uint32_t compareValue =
      ((uint32_t)(TIMER1_TOP + 1U) * dutyPercent) / 100U;

  if (compareValue > (uint32_t)(TIMER1_TOP + 1U)) {
    compareValue = TIMER1_TOP + 1U;
  }

  OCR1A = (uint16_t)compareValue;

  // Non-inverting OC1A output; preserve WGM bits.
  TCCR1A = (TCCR1A & ~_BV(COM1A0)) | _BV(COM1A1);
}

// Read multiple ADC samples and convert LM35 voltage to degrees Celsius.
float acquireTemperatureC() {
  uint32_t adcSum = 0;

  for (uint8_t i = 0; i < ADC_SAMPLES; i++) {
    adcSum += analogRead(LM35_PIN);
  }

  adcRawAverage = (uint16_t)(adcSum / ADC_SAMPLES);

  // LM35: 10 mV per degree C. temperature = mV / 10.
  float voltageMv =
      ((float)adcRawAverage * ADC_REFERENCE_MV) / 1023.0f;

  return voltageMv / LM35_MV_PER_C;
}

// Update filtered temperature, then apply staged control with hysteresis.
void updateFanControl(float temperatureC) {
  switch (fanStage) {
    case FAN_OFF:
      if (temperatureC >= OFF_TO_LOW_C) {
        fanStage = FAN_LOW;
      }
      break;

    case FAN_LOW:
      if (temperatureC < LOW_TO_OFF_C) {
        fanStage = FAN_OFF;
      } else if (temperatureC >= LOW_TO_MED_C) {
        fanStage = FAN_MEDIUM;
      }
      break;

    case FAN_MEDIUM:
      if (temperatureC < MED_TO_LOW_C) {
        fanStage = FAN_LOW;
      } else if (temperatureC >= MED_TO_HIGH_C) {
        fanStage = FAN_HIGH;
      }
      break;

    case FAN_HIGH:
      if (temperatureC < HIGH_TO_MED_C) {
        fanStage = FAN_MEDIUM;
      } else if (temperatureC >= HIGH_TO_MAX_C) {
        fanStage = FAN_MAX;
      }
      break;

    case FAN_MAX:
      if (temperatureC < MAX_TO_HIGH_C) {
        fanStage = FAN_HIGH;
      }
      break;
  }

  uint8_t requestedDuty = 0;

  switch (fanStage) {
    case FAN_OFF:    requestedDuty = 0;   break;
    case FAN_LOW:    requestedDuty = 30;  break;
    case FAN_MEDIUM: requestedDuty = 50;  break;
    case FAN_HIGH:   requestedDuty = 75;  break;
    case FAN_MAX:    requestedDuty = 100; break;
  }

  if (requestedDuty != fanDutyPercent) {
    setFanDuty(requestedDuty);
  }
}

const char* fanStageName(FanStage stage) {
  switch (stage) {
    case FAN_OFF:    return "OFF";
    case FAN_LOW:    return "LOW";
    case FAN_MEDIUM: return "MEDIUM";
    case FAN_HIGH:   return "HIGH";
    case FAN_MAX:    return "MAXIMUM";
    default:         return "UNKNOWN";
  }
}

void printStatus(unsigned long nowMs) {
  Serial.print(F("Time_ms="));
  Serial.print(nowMs);
  Serial.print(F(" | ADC="));
  Serial.print(adcRawAverage);
  Serial.print(F(" | Temperature_C="));
  Serial.print(temperatureFilteredC, 2);
  Serial.print(F(" | Fan_stage="));
  Serial.print(fanStageName(fanStage));
  Serial.print(F(" | PWM_Duty_percent="));
  Serial.println(fanDutyPercent);
}

void setup() {
  pinMode(LM35_PIN, INPUT);
  Serial.begin(9600);

  configureTimer1PWM();
  setFanDuty(0);

  Serial.println(F("========================================"));
  Serial.println(F("EXPERIMENT 4 - INDUSTRIAL HVAC CONTROL"));
  Serial.println(F("ADC Temperature + Timer1 Interrupt PWM"));
  Serial.println(F("PWM output: D9 / OC1A, frequency: 1 kHz"));
  Serial.println(F("Fan starts OFF; temperature controls duty."));
  Serial.println(F("========================================"));
}

void loop() {
  bool runControlTask = false;

  // Atomically consume the one-byte task flag.
  noInterrupts();
  if (controlTaskDue) {
    controlTaskDue = false;
    runControlTask = true;
  }
  interrupts();

  if (runControlTask) {
    temperatureInstantC = acquireTemperatureC();

    if (!temperatureInitialized) {
      temperatureFilteredC = temperatureInstantC;
      temperatureInitialized = true;
    } else {
      temperatureFilteredC +=
          TEMP_FILTER_ALPHA *
          (temperatureInstantC - temperatureFilteredC);
    }

    updateFanControl(temperatureFilteredC);
  }

  unsigned long now = millis();
  if (now - lastSerialReportMs >= SERIAL_REPORT_INTERVAL_MS) {
    lastSerialReportMs = now;

    if (temperatureInitialized) {
      printStatus(now);
    } else {
      Serial.println(F("Waiting for first ADC temperature sample..."));
    }
  }
}
