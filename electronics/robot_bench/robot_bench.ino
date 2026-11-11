#ifndef ROBOT_DRIVER_DRV8833
#define ROBOT_DRIVER_DRV8833 0 // 0: existing L298N; 1: recommended DRV8833
#endif
#include "RobotHardware.h"
using namespace RobotHardware;

// START-OVER TEST: two motors only. No servo, sensors or automatic route.
// NORMAL L298N wiring, NOT the temporary swapped-channel test:
// D5->ENA, D2->IN1, D4->IN2; D6->ENB, D7->IN3, D8->IN4.
// Remove ENA/ENB caps and 5V-EN when UNO provides logic 5V.
// Battery -> HW-140 IN; verified motor-rated regulator OUT -> driver VM.
// Never connect the raw 3S LiPo directly to unidentified low-voltage motors.
// Full duty is intentional for DC voltage measurements. It does NOT limit VM.
const int LEFT_SIGN = 1, RIGHT_SIGN = 1;
const int TEST_DUTY = 255;
bool motorsRunning = false;

void stopTest() {
  stopMotors();
  motorsRunning = false;
}

void runBoth() {
  driveWheels(TEST_DUTY * LEFT_SIGN, TEST_DUTY * RIGHT_SIGN);
  motorsRunning = true;
  Serial.println(F("BOTH motors RUNNING continuously at full duty. Send x to stop."));
}

void printStatus() {
  if (motorsRunning) Serial.println(F("Status: RUNNING, both motors, duty 255/255."));
  else Serial.println(F("Status: STOPPED."));
}

void command(char c) {
  switch (c) {
    case '\n': case '\r': case ' ': return;
    case 'r': runBoth(); break;
    case 'x':
      stopTest(); Serial.println(F("BOTH motors STOPPED. Wheels may coast.")); break;
    case 'v': printStatus(); break;
    case 'h':
      Serial.println(F("r = BOTH motors continuously; x = STOP; v = status; h = help"));
      break;
    default:
      stopTest();
      Serial.println(F("Unknown command: motors stopped. Use r / x / v / h."));
      break;
  }
}

void setup() {
  beginMotors();
  stopTest();
  Serial.begin(115200);
  printDriver();
  Serial.println(F("TWO-MOTOR CONTINUOUS BENCH TEST. Stopped on boot."));
  Serial.println(F("Disconnect servo/sensors. Raise wheels. No automatic timeout."));
  command('h');
}

void loop() {
  while (Serial.available()) command(Serial.read());
}
