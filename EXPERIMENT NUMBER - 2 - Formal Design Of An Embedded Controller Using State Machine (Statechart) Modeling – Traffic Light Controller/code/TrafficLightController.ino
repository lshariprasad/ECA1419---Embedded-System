/*
  EXPERIMENT 2
  FORMAL DESIGN OF AN EMBEDDED CONTROLLER USING FSM / STATECHART
  TRAFFIC LIGHT CONTROLLER

  Board: Arduino UNO

  Proteus connections (matching the provided schematic):
    D8  -> 220 ohm resistor -> RED LED anode
    D9  -> 220 ohm resistor -> YELLOW LED anode
    D10 -> 220 ohm resistor -> GREEN LED anode
    All LED cathodes -> common GND

  State sequence:
    RED (5 s) -> GREEN (5 s) -> YELLOW (2 s) -> RED ...

  Uses millis() instead of delay(), so the FSM remains non-blocking.
*/

// -------------------- Output pin definitions --------------------
const uint8_t RED_LED_PIN    = 8;
const uint8_t YELLOW_LED_PIN = 9;
const uint8_t GREEN_LED_PIN  = 10;

// -------------------- State timing (milliseconds) ----------------
const unsigned long RED_DURATION_MS    = 5000UL;
const unsigned long GREEN_DURATION_MS  = 5000UL;
const unsigned long YELLOW_DURATION_MS = 2000UL;

// -------------------- FSM state definitions ----------------------
enum TrafficState : uint8_t {
  STATE_RED,
  STATE_GREEN,
  STATE_YELLOW
};

TrafficState currentState = STATE_RED;
unsigned long stateEnteredAt = 0;

// Return the duration assigned to the current state.
unsigned long durationForState(TrafficState state) {
  switch (state) {
    case STATE_RED:    return RED_DURATION_MS;
    case STATE_GREEN:  return GREEN_DURATION_MS;
    case STATE_YELLOW: return YELLOW_DURATION_MS;
    default:           return RED_DURATION_MS;
  }
}

// Display the current state on the Serial Monitor.
void printState(TrafficState state) {
  Serial.print(F("Traffic light state: "));

  switch (state) {
    case STATE_RED:
      Serial.println(F("RED - STOP"));
      break;
    case STATE_GREEN:
      Serial.println(F("GREEN - GO"));
      break;
    case STATE_YELLOW:
      Serial.println(F("YELLOW - PREPARE TO STOP"));
      break;
    default:
      Serial.println(F("UNKNOWN"));
      break;
  }
}

// Apply the output pattern for the specified state.
void applyOutputs(TrafficState state) {
  // Turn every LED off before enabling the selected one.
  // This prevents overlapping states during transitions.
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  switch (state) {
    case STATE_RED:
      digitalWrite(RED_LED_PIN, HIGH);
      break;

    case STATE_GREEN:
      digitalWrite(GREEN_LED_PIN, HIGH);
      break;

    case STATE_YELLOW:
      digitalWrite(YELLOW_LED_PIN, HIGH);
      break;

    default:
      // Fail-safe fallback: all LEDs remain off.
      break;
  }
}

// Enter a new state and reset its timer.
void enterState(TrafficState nextState, unsigned long now) {
  currentState = nextState;
  stateEnteredAt = now;
  applyOutputs(currentState);
  printState(currentState);
}

// Calculate the next state in the finite-state machine.
TrafficState nextStateFor(TrafficState state) {
  switch (state) {
    case STATE_RED:    return STATE_GREEN;
    case STATE_GREEN:  return STATE_YELLOW;
    case STATE_YELLOW: return STATE_RED;
    default:           return STATE_RED;
  }
}

void setup() {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  Serial.begin(9600);
  Serial.println(F("===================================="));
  Serial.println(F("FSM TRAFFIC LIGHT CONTROLLER"));
  Serial.println(F("Arduino UNO - Experiment 2"));
  Serial.println(F("===================================="));

  // Initialize the FSM in RED state.
  enterState(STATE_RED, millis());
}

void loop() {
  const unsigned long now = millis();

  // Unsigned subtraction keeps the timer safe across millis() rollover.
  if (now - stateEnteredAt >= durationForState(currentState)) {
    enterState(nextStateFor(currentState), now);
  }
}
