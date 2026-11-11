#ifndef ROBOT_DRIVER_DRV8833
#define ROBOT_DRIVER_DRV8833 0 // 0: existing L298N; 1: recommended DRV8833
#endif
#include <Servo.h>
#include "RobotHardware.h"
using namespace RobotHardware;

// Flip either sign if that wheel runs backwards. PWM is not a voltage limit.
const int LEFT_SIGN = 1, RIGHT_SIGN = 1;
int LEFT_PWM = 140, RIGHT_PWM = 140; // ]/[ adjust both; v prints settings.
const unsigned long JOG_MS = 400;

// Calibrate without the gate bar fitted, then fit it at a known angle.
// Commands n / + / - set 90 degrees / increase 2 / decrease 2 degrees.
const bool GATE_CALIBRATED = false;

// Replace zeroes with measured milliseconds. TURN_MS may stay 0 (straight).
const unsigned long TO_BALL_MS = 0;
const unsigned long TURN_MS = 0;
const unsigned long TO_GOAL_MS = 0;
const unsigned long PUSH_MS = 0;
const bool TURN_RIGHT = true;
const bool ROUTE_CONFIGURED = false;

Servo gate;
int gateAngle = 90;
bool servoAttached = false;
bool jogging = false, running = false;
unsigned long jogStarted = 0, stepStarted = 0;
byte stepIndex = 0;

// kind: 'o' open, 'c' close, 'f' forward, 't' turn, 'x' motors off.
struct Step { char kind; unsigned long duration; };
const Step route[] = {
  {'o', 600}, {'f', TO_BALL_MS}, {'x', 250}, {'c', 600},
  {'t', TURN_MS}, {'x', 250}, {'f', TO_GOAL_MS}, {'x', 250},
  {'o', 600}, {'f', PUSH_MS}, {'x', 250}
};
const byte STEP_COUNT = sizeof(route) / sizeof(route[0]);

void drive(int left, int right) {
  driveWheels(left * LEFT_SIGN, right * RIGHT_SIGN);
}

void setGate(int angle) {
  gateAngle = constrain(angle, MIN_ANGLE, MAX_ANGLE);
  // Set pulse before attaching, rather than sweeping to an unknown endpoint.
  gate.write(gateAngle);
  if (!servoAttached) { gate.attach(SERVO_PIN); servoAttached = true; }
  Serial.print(F("Gate angle: ")); Serial.println(gateAngle);
}

void abortMotion() {
  running = false; jogging = false; stopMotors();
  // Stop means motors off. Servo continues holding its last position.
}

void applyStep() {
  stopMotors();
  switch (route[stepIndex].kind) {
    case 'o': setGate(OPEN_ANGLE); break;
    case 'c': setGate(CLOSED_ANGLE); break;
    case 'f': drive(LEFT_PWM, RIGHT_PWM); break;
    case 't':
      if (TURN_MS > 0) {
        if (TURN_RIGHT) drive(LEFT_PWM, -RIGHT_PWM);
        else drive(-LEFT_PWM, RIGHT_PWM);
      }
      break;
  }
  stepStarted = millis();
}

void startRoute() {
  if (!ROUTE_CONFIGURED || !GATE_CALIBRATED || OPEN_ANGLE == CLOSED_ANGLE ||
      OPEN_ANGLE < MIN_ANGLE || OPEN_ANGLE > MAX_ANGLE ||
      CLOSED_ANGLE < MIN_ANGLE || CLOSED_ANGLE > MAX_ANGLE ||
      TO_BALL_MS == 0 || TO_GOAL_MS == 0 || PUSH_MS == 0) {
    Serial.println(F("Route locked: calibrate angles/times and set both flags true."));
    return;
  }
  abortMotion(); running = true; stepIndex = 0; applyStep();
  Serial.println(F("Route started. x stops drive motors."));
}

void printSpeeds() {
  Serial.print(F("PWM left=")); Serial.print(LEFT_PWM);
  Serial.print(F(" right=")); Serial.println(RIGHT_PWM);
}

void command(char c) {
  if (c == '\n' || c == '\r' || c == ' ') return;
  if (c == 'x') { abortMotion(); Serial.println(F("Motors stopped.")); return; }
  if (running) return; // do not accept calibration commands during a route
  if (c == 'g') { startRoute(); return; }
  if (c == 'h') {
    Serial.println(F("1 left; 2 right; f forward; b back; l/r turn; x stop; [/] PWM; v speeds"));
    Serial.println(F("n servo 90; +/- servo 2 deg; o/c calibrated open/close; g route"));
    return;
  }
  abortMotion();
  switch (c) {
    case '[': case ']':
      LEFT_PWM = constrain(LEFT_PWM + (c == ']' ? 10 : -10), 0, 255);
      RIGHT_PWM = constrain(RIGHT_PWM + (c == ']' ? 10 : -10), 0, 255);
      printSpeeds();
      return;
    case 'v':
      printSpeeds();
      return;
    case '1': drive(LEFT_PWM, 0); break;
    case '2': drive(0, RIGHT_PWM); break;
    case 'f': drive(LEFT_PWM, RIGHT_PWM); break;
    case 'b': drive(-LEFT_PWM, -RIGHT_PWM); break;
    case 'l': drive(-LEFT_PWM, RIGHT_PWM); break;
    case 'r': drive(LEFT_PWM, -RIGHT_PWM); break;
    case 'n': setGate(90); return;
    case '+': setGate(gateAngle + 2); return;
    case '-': setGate(gateAngle - 2); return;
    case 'o': case 'c':
      if (GATE_CALIBRATED) setGate(c == 'o' ? OPEN_ANGLE : CLOSED_ANGLE);
      else Serial.println(F("Calibrate with n/+/- first; set GATE_CALIBRATED true."));
      return;
    default: Serial.println(F("Unknown command. Send h for help.")); return;
  }
  jogging = true; jogStarted = millis();
}

void setup() {
  beginMotors();
  pinMode(START_PIN, INPUT_PULLUP);
  stopMotors();
  Serial.begin(115200);
  printDriver();
  Serial.println(F("Ready. No movement on boot. Send h for commands."));
}

void loop() {
  while (Serial.available()) command(Serial.read());
  unsigned long now = millis();
  if (jogging && now - jogStarted >= JOG_MS) {
    jogging = false; stopMotors();
  }
  if (running && now - stepStarted >= route[stepIndex].duration) {
    stopMotors();
    if (++stepIndex >= STEP_COUNT) {
      running = false; Serial.println(F("Route finished."));
    } else applyStep();
  }

  // Optional debounced start button. Must be released before first start.
  // Holding the button cannot repeat a route after it finishes.
  static bool lastRaw = HIGH, stable = HIGH, armed = false;
  static unsigned long changedAt = 0;
  bool raw = digitalRead(START_PIN);
  if (raw != lastRaw) { lastRaw = raw; changedAt = now; }
  if (now - changedAt >= 40) {
    if (raw == HIGH) armed = true;
    if (raw != stable) {
      stable = raw;
      if (stable == LOW && armed) {
        armed = false;
        if (!running && !jogging) startRoute();
      }
    }
  }
}
