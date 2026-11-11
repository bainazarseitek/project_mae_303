#include "RobotHardware.h"
using namespace RobotHardware;

// Exhaustively check the actual production mapping: its effective signed duty
// must equal the requested duty, including saturation outside the valid range.
// A naive reverse analogWrite(abs(speed)) on the one-PWM DRV8833 wiring fails.
template<int Speed> struct Check {
  static constexpr MotorOutput out = motorOutput(Speed);
#if ROBOT_DRIVER_DRV8833
  static_assert((out.direction == HIGH ? int(out.pwm) - 255 : int(out.pwm)) == boundedSpeed(Speed), "DRV8833 signed duty mismatch");
#else
  static_assert((out.direction == HIGH ? int(out.pwm) : -int(out.pwm)) == boundedSpeed(Speed), "L298N signed duty mismatch");
  static_assert(!(out.direction == HIGH && out.other == HIGH), "Unexpected braking direction pair");
#endif
  static constexpr int count = 1 + Check<Speed - 1>::count;
};
template<> struct Check<-301> { static constexpr int count = 0; };
static_assert(Check<300>::count == 601, "Check every speed from -300 to 300");
static_assert(motorOutput(0).pwm == 0 && motorOutput(0).direction == LOW, "Zero must not drive");
static_assert(OPEN_ANGLE == 90 && CLOSED_ANGLE == 0, "Gate calibration defaults drifted");
static_assert(LEFT_PWM_PIN == 5 && RIGHT_PWM_PIN == 6, "Wheel PWM must avoid Servo Timer1");
