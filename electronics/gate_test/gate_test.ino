#include <Servo.h>

// Gate-only bench test, Arduino UNO R3. Signal: D9.
// Separate verified servo supply, with common Arduino GND.
// Check mechanical clearance at 0 and 90 degrees before fitting the bar.
const int OPEN_ANGLE = 90;
const int CLOSED_ANGLE = 0;
const int CENTER_ANGLE = 90;
const int MIN_ANGLE = 0, MAX_ANGLE = 90;
const byte SERVO_PIN = 9;

Servo gate;
bool attached = false;
int targetAngle = CENTER_ANGLE;

void moveTo(int angle) {
  targetAngle = constrain(angle, MIN_ANGLE, MAX_ANGLE);
  // Command the endpoint directly: no software delay or gradual sweep.
  gate.write(targetAngle);
  if (!attached) {
    gate.attach(SERVO_PIN);
    attached = true;
  }
  Serial.print(F("Gate target: ")); Serial.println(targetAngle);
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
      Serial.println(F("Holding last target. x cannot stop direct servo travel; disconnect servo power if jammed."));
      break;
    case 'h':
      Serial.println(F("o open 90; c close 0; n 90; +/- adjust 2 deg; x hold last target"));
      break;
    default: Serial.println(F("Unknown command. Send h.")); break;
  }
}

void setup() {
  // Disable both motor channels. Remove ENA/ENB caps.
  const byte motorPins[] = {2, 4, 5, 6, 7, 8};
  for (byte i = 0; i < sizeof(motorPins) / sizeof(motorPins[0]); ++i) {
    digitalWrite(motorPins[i], LOW);
    pinMode(motorPins[i], OUTPUT);
  }
  Serial.begin(115200);
  Serial.println(F("Gate test ready. No movement until a command. Motors disabled."));
  Serial.println(F("Direct movement: open=90, closed=0. First test with bar OFF."));
  command('h');
}

void loop() {
  while (Serial.available()) command(Serial.read());
}
