#ifndef ROBOT_DRIVER_DRV8833
#define ROBOT_DRIVER_DRV8833 0 // Match installed driver in EVERY sketch.
#endif
#include <Servo.h>
#include "RobotHardware.h"
using namespace RobotHardware;

// Gate-only bench test, Arduino UNO R3. Signal: D9.
// Separate verified servo supply, with common Arduino GND.
// Check mechanical clearance at 0 and 90 degrees before fitting the bar.

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
  beginMotors();
  Serial.begin(115200);
  printDriver();
  Serial.println(F("Gate test ready. No movement until a command. Motors disabled."));
  Serial.println(F("Direct movement: open=90, closed=0. First test with bar OFF."));
  command('h');
}

void loop() {
  while (Serial.available()) command(Serial.read());
}
