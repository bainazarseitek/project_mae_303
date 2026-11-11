#pragma once
#include <Arduino.h>

// Canonical hardware source. tools/prepare_electronics.py copies this header
// into each standalone sketch. Edit this file, then regenerate the copies.
// Supply voltage is set on physical regulators, never by this header.
#ifndef ROBOT_DRIVER_DRV8833
#define ROBOT_DRIVER_DRV8833 0
#endif
#if ROBOT_DRIVER_DRV8833 != 0 && ROBOT_DRIVER_DRV8833 != 1
#error "Select 0 for L298N or 1 for DRV8833"
#endif

namespace RobotHardware {
const byte LEFT_PWM_PIN = 5, LEFT_DIR_PIN = 2, LEFT_OTHER_PIN = 4;
const byte RIGHT_PWM_PIN = 6, RIGHT_DIR_PIN = 7, RIGHT_OTHER_PIN = 8;
const byte SLEEP_PIN = 8; // DRV8833 only; L298N uses this pin as IN4.
const byte SERVO_PIN = 9, START_PIN = 3;
const byte LEFT_TRIG = A0, LEFT_ECHO = A1;
const byte RIGHT_TRIG = A2, RIGHT_ECHO = A3;
const byte IR_PIN = A4, COLOR_OUT_PIN = A5;
const byte COLOR_S0 = 10, COLOR_S1 = 11, COLOR_S2 = 12, COLOR_S3 = 13;
const byte IR_WHITE = HIGH;
const int OPEN_ANGLE = 90, CLOSED_ANGLE = 0, CENTER_ANGLE = 90;
const int MIN_ANGLE = 0, MAX_ANGLE = 90;

inline void stopMotors() {
#if ROBOT_DRIVER_DRV8833
  digitalWrite(SLEEP_PIN, LOW);
#endif
  analogWrite(LEFT_PWM_PIN, 0);
  analogWrite(RIGHT_PWM_PIN, 0);
  digitalWrite(LEFT_DIR_PIN, LOW);
  digitalWrite(RIGHT_DIR_PIN, LOW);
#if !ROBOT_DRIVER_DRV8833
  digitalWrite(LEFT_OTHER_PIN, LOW);
  digitalWrite(RIGHT_OTHER_PIN, LOW);
#endif
}

inline void beginMotors() {
  // Low output latches before OUTPUT mode; external L298N EN pull-downs
  // keep the motors disabled before setup/reset as shown in the schematic.
  const byte pins[] = {2, 4, 5, 6, 7, 8};
  for (byte i = 0; i < sizeof(pins) / sizeof(pins[0]); ++i) {
    digitalWrite(pins[i], LOW);
    pinMode(pins[i], OUTPUT);
  }
  stopMotors();
}

struct MotorOutput {
  byte pwm, direction, other;
  constexpr MotorOutput(byte p, byte d, byte o) : pwm(p), direction(d), other(o) {}
};

constexpr int boundedSpeed(int speed) {
  return speed < -255 ? -255 : (speed > 255 ? 255 : speed);
}

constexpr MotorOutput motorOutput(int speed) {
#if ROBOT_DRIVER_DRV8833
  // pwm -> IN1, direction -> IN2. Positive: drive/coast PWM.
  // Negative: inverted PWM gives drive/brake. See DRV8833 truth table.
  return MotorOutput(boundedSpeed(speed) < 0 ? 255 + boundedSpeed(speed) : boundedSpeed(speed),
                     speed < 0 ? HIGH : LOW, LOW);
#else
  return MotorOutput(boundedSpeed(speed) < 0 ? -boundedSpeed(speed) : boundedSpeed(speed),
                     speed > 0 ? HIGH : LOW, speed < 0 ? HIGH : LOW);
#endif
}

inline void oneMotor(byte pwm, byte direction, byte other, int speed) {
  const MotorOutput out = motorOutput(speed);
  digitalWrite(direction, out.direction);
#if ROBOT_DRIVER_DRV8833
  (void)other;
#else
  digitalWrite(other, out.other);
#endif
  analogWrite(pwm, out.pwm);
}

inline void driveWheels(int left, int right) {
  stopMotors();
  oneMotor(LEFT_PWM_PIN, LEFT_DIR_PIN, LEFT_OTHER_PIN, left);
  oneMotor(RIGHT_PWM_PIN, RIGHT_DIR_PIN, RIGHT_OTHER_PIN, right);
#if ROBOT_DRIVER_DRV8833
  if (left != 0 || right != 0) {
    digitalWrite(SLEEP_PIN, HIGH);
    delayMicroseconds(1000); // Datasheet wake time before asserting drive.
  }
#endif
}

inline void printDriver() {
#if ROBOT_DRIVER_DRV8833
  Serial.println(F("Driver: DRV8833. Separate regulated motor rail required."));
#else
  Serial.println(F("Driver: L298N. Remove ENA/ENB/5V-EN caps; connect motor VM."));
#endif
}
} // namespace RobotHardware
