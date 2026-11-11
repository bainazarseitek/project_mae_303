#ifndef ROBOT_DRIVER_DRV8833
#define ROBOT_DRIVER_DRV8833 0 // Match installed driver in EVERY sketch.
#endif
#include "RobotHardware.h"
using namespace RobotHardware;

// UNO R3. Diagnostic sketch only: motors disabled, no servo commands.
// Test with the MOTOR/SERVO BATTERY DISCONNECTED; UNO and sensors use USB.
// Confirmed on the team's tested field: IR_WHITE == HIGH.
const bool ENABLE_IR = true;
const unsigned long PING_INTERVAL_MS = 75; // one transmitter at a time
const unsigned long ECHO_TIMEOUT_US = 25000;
unsigned long lastPing = 0;
bool sampleLeft = true;

float readDistance(byte trig, byte echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  unsigned long duration = pulseIn(echo, HIGH, ECHO_TIMEOUT_US);
  if (duration == 0) return -1; // timeout is UNKNOWN, never "clear path"
  float cm = duration / 58.0f;
  if (cm < 2 || cm > 400) return -1;
  return cm;
}

void setup() {
  beginMotors();
  digitalWrite(LEFT_TRIG, LOW); digitalWrite(RIGHT_TRIG, LOW);
  pinMode(LEFT_TRIG, OUTPUT); pinMode(LEFT_ECHO, INPUT);
  pinMode(RIGHT_TRIG, OUTPUT); pinMode(RIGHT_ECHO, INPUT);
  if (ENABLE_IR) pinMode(IR_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  printDriver();
  Serial.println(F("Sensor test. Motors OFF. Battery disconnected."));
  Serial.println(F("NA = no valid echo; it does NOT mean no obstacle."));
}

void loop() {
  unsigned long now = millis();
  if (now - lastPing < PING_INTERVAL_MS) return;
  lastPing = now;
  float cm = sampleLeft ? readDistance(LEFT_TRIG, LEFT_ECHO)
                        : readDistance(RIGHT_TRIG, RIGHT_ECHO);
  if (sampleLeft) Serial.print(F("LEFT_cm="));
  else Serial.print(F("RIGHT_cm="));
  if (cm < 0) Serial.print(F("NA"));
  else Serial.print(cm, 1);
  if (ENABLE_IR) {
    Serial.print(F("  IR_raw=")); Serial.print(digitalRead(IR_PIN));
  }
  Serial.println();
  sampleLeft = !sampleLeft;
}
