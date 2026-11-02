// Athlete Reaction Time Sensor - Arduino side
// Five illuminated buttons. A random LED lights up; the athlete presses that
// button as fast as possible. Each result is sent over USB serial as CSV:
//     elapsed_ms,button_number,reaction_ms
// MATLAB starts a session by sending the character 'S'.

const uint8_t BUTTON_PINS[5] = {2, 3, 4, 5, 6};    // button -> pin, other side -> GND
const uint8_t LED_PINS[5]    = {8, 9, 10, 11, 12}; // LED in each button (use a resistor)

const int  NUM_ROUNDS    = 20;     // rounds per session
const long MIN_DELAY_MS  = 1000;   // random wait before each light
const long MAX_DELAY_MS  = 3000;
const long DEBOUNCE_MS   = 30;

void allLedsOff() {
  for (int i = 0; i < 5; i++) digitalWrite(LED_PINS[i], LOW);
}

bool anyButtonPressed() {
  for (int i = 0; i < 5; i++) {
    if (digitalRead(BUTTON_PINS[i]) == LOW) return true;
  }
  return false;
}

void runSession() {
  unsigned long sessionStart = millis();

  for (int round = 0; round < NUM_ROUNDS; round++) {
    // Make sure nothing is being held before the next round
    while (anyButtonPressed()) {}
    allLedsOff();

    delay(random(MIN_DELAY_MS, MAX_DELAY_MS + 1));

    int target = random(5);                  // pick a random button
    digitalWrite(LED_PINS[target], HIGH);
    unsigned long lightOn = micros();

    // Wait for the correct button (wrong buttons are ignored)
    while (digitalRead(BUTTON_PINS[target]) == HIGH) {}
    unsigned long reaction = micros() - lightOn;

    digitalWrite(LED_PINS[target], LOW);
    delay(DEBOUNCE_MS);

    Serial.print(millis() - sessionStart);
    Serial.print(',');
    Serial.print(target + 1);
    Serial.print(',');
    Serial.println(reaction / 1000.0, 3);    // reaction time in ms
  }

  Serial.println("DONE");
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 5; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    pinMode(LED_PINS[i], OUTPUT);
  }
  allLedsOff();
  randomSeed(analogRead(A0));                // A0 left unconnected for noise
}

void loop() {
  if (Serial.available() > 0 && Serial.read() == 'S') {
    runSession();
  }
}
