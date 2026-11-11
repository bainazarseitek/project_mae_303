#include <Arduino.h>

// L298N ONLY. Temporary diagnostic wiring, not the normal robot pin map:
// D6 -> ENA signal, D7 -> IN1, D8 -> IN2. Motor -> OUT1/OUT2.
// Disconnect the old D5/D2/D4 wires from channel A. Leave channel B empty.
// Remove ENA cap. Remove 5V-EN if UNO provides driver logic 5V.
// Regulated, motor-rated supply -> VM/+12V; all control grounds common.
// Do NOT connect the raw 3S LiPo directly to unidentified low-voltage motors.
const byte TEST_EN = 6;
const byte TEST_IN1 = 7;
const byte TEST_IN2 = 8;

bool motorRunning = false;

void stopMotor() {
  digitalWrite(TEST_EN, LOW);
  digitalWrite(TEST_IN1, LOW);
  digitalWrite(TEST_IN2, LOW);
  motorRunning = false;
}

void runMotor() {
  stopMotor();
  digitalWrite(TEST_IN1, HIGH);
  digitalWrite(TEST_IN2, LOW);
  digitalWrite(TEST_EN, HIGH); // full duty, no PWM averaging for meter test
  motorRunning = true;
  Serial.println(F("RUNNING continuously at full duty. x stops drive. No timeout."));
}

void command(char c) {
  switch (c) {
    case '\n': case '\r': case ' ': return;
    case 'r': runMotor(); break;
    case 'x':
      stopMotor(); Serial.println(F("STOPPED. Wheel may coast.")); break;
    case 'h':
      Serial.println(F("r = continuous run; x = stop; h = help")); break;
    default:
      Serial.println(F("Unknown command. Use r to run or x to stop.")); break;
  }
}

void setup() {
  // All original motor-control pins LOW before OUTPUT mode.
  const byte pins[] = {2, 4, 5, 6, 7, 8};
  for (byte i = 0; i < sizeof(pins) / sizeof(pins[0]); ++i) {
    digitalWrite(pins[i], LOW);
    pinMode(pins[i], OUTPUT);
  }
  stopMotor();
  Serial.begin(115200);
  Serial.println(F("L298N OUT1/OUT2 continuous test. Stopped on boot."));
  Serial.println(F("TEMP WIRING: D6->ENA, D7->IN1, D8->IN2."));
  Serial.println(F("Disconnect servo; raise wheel; check motor supply rating first."));
  command('h');
}

void loop() {
  while (Serial.available()) command(Serial.read());
}
