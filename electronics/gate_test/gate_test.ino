#include <Servo.h>

// Gate-only bench test, Arduino UNO R3. Signal wire: D9.
// Servo uses the existing tested 4.8V supply with common GND.
// Trial angles, NOT calibrated positions for the printed mechanism.
// First test with the gate bar removed. Swap these if direction is reversed.
const int OPEN_ANGLE = 120;
const int CLOSED_ANGLE = 60;
const int CENTER_ANGLE = 90;
const int MIN_ANGLE = 30, MAX_ANGLE = 150;
const byte SERVO_PIN = 9;
const unsigned long STEP_MS = 25;

Servo gate;
bool attached = false;
int currentAngle = CENTER_ANGLE, targetAngle = CENTER_ANGLE;
unsigned long lastStep = 0;

void printAngle() {
  Serial.print(F("Gate angle: ")); Serial.println(currentAngle);
}

void moveTo(int angle) {
  targetAngle = constrain(angle, MIN_ANGLE, MAX_ANGLE);
  if (!attached) {
    // First servo command centers it; start with the bar removed.
    gate.write(currentAngle);
    gate.attach(SERVO_PIN);
    attached = true;
  }
  lastStep = millis();
  Serial.print(F("Target angle: ")); Serial.println(targetAngle);
}

void command(char c) {
  switch (c) {
    case '\n': case '\r': case ' ': return;
    case 'o': moveTo(OPEN_ANGLE); break;
    case 'c': moveTo(CLOSED_ANGLE); break;
    case 'n': moveTo(CENTER_ANGLE); break;
    case '+': moveTo(targetAngle + 2); break;
    case '-': moveTo(targetAngle - 2); break;
    case 'x':
      targetAngle = currentAngle; // stop sweep, continue holding this position
      Serial.println(F("Sweep stopped; servo still holds. Disconnect supply if jammed."));
      printAngle();
      break;
    case 'h':
      Serial.println(F("o open; c close; n center; +/- adjust 2 deg; x stop sweep"));
      break;
    default: Serial.println(F("Unknown command. Send h.")); break;
  }
}

void setup() {
  // Disable both motor channels. Remove ENA/ENB caps as in the bench wiring.
  const byte motorPins[] = {2, 4, 5, 6, 7, 8};
  for (byte i = 0; i < sizeof(motorPins) / sizeof(motorPins[0]); ++i) {
    digitalWrite(motorPins[i], LOW);
    pinMode(motorPins[i], OUTPUT);
  }
  Serial.begin(115200);
  Serial.println(F("Gate test ready. No movement until a command. Motors disabled."));
  Serial.println(F("Trial positions: open=120, closed=60. First test with bar OFF."));
  command('h');
}

void loop() {
  while (Serial.available()) command(Serial.read());
  unsigned long now = millis();
  if (attached && currentAngle != targetAngle && now - lastStep >= STEP_MS) {
    lastStep = now;
    currentAngle += targetAngle > currentAngle ? 1 : -1;
    gate.write(currentAngle);
    if (currentAngle == targetAngle) printAngle();
  }
}
