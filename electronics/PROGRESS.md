# Bench progress - 11 November 2026

## Reported hardware results

- Uno upload/Serial Monitor work at 115200 baud.
- IR module works: white=1, background=0. Height and field surface must remain consistent.
- One wheel motor ran on L298N OUT1/OUT2; another was intermittent/hummed through the driver but turned directly from a battery. Root cause not proven.
- Later troubleshooting established driver VM/+12V was not connected. Logic/control alone cannot power the wheels. Successful operation after completing motor-power wiring has not been reported.
- Servo responded to earlier tests. New 90/0 degree endpoints, bar clearance and ball retention/release remain unverified.
- Ultrasonic readings remain NA/unresolved. Color module identified visually; no color-readout result.
- Battery changed from the old 4.8 V AA pack to user-specified 11.1 V, 2450 mAh, 30C TERANTY LiPo, measured at 11.4 V.
- HW-140 was installed and displayed 5.01 V while being set up. Multimeter was reported unreliable; no confirmed loaded-rail test exists.

## Current architecture decision

Start-over update: the team requested an isolated two-motor continuous test. Current `robot_bench` now starts BOTH motors with `r`, stops BOTH with `x`, and uses full duty 255/255. It has no 400ms jogs, servo commands, start button or route. It is stopped on boot. Normal L298N wiring is restored: D5/D2/D4 -> ENA/IN1/IN2 and D6/D7/D8 -> ENB/IN3/IN4. The previous D6/D7/D8-to-channel-A swap applies only to the separate old `motor_continuous_test`.

For THIS test only, with servo/sensors disconnected, the HW-140 temporarily supplies driver VM at the initial 5.0V test setting (or lower if actual motor rating requires it). UNO remains USB-powered and provides L298N logic 5V with 5V-EN removed. The initial setting does not establish original motor voltage ratings or guarantee startup through L298N. See [current test instructions](robot_bench/README.md).

Both output pairs are under investigation: the team reports working motors but failure at OUT1/OUT2 across motor swaps, and a later reading of 0.16V across that pair during a temporary continuous test. Supply/logic readings and root cause are still unconfirmed. Do not record either channel as repaired yet.

Older single-regulator/shared motor-and-servo suggestions were provisional tests. They are superseded by [CONNECTIONS.md](CONNECTIONS.md): fixed 5 V gate/logic branch plus independent motor regulator. Do not increase servo voltage for wheel speed.

The team can add parts but has no specifications for the original motors. Recommended higher-efficiency target: documented motor pair, motor buck regulator, DRV8833 carrier, fused/switched power distribution and appropriate 3S charging/monitoring. Use 6 V only for a verified 6 V pair. Existing L298N fallback is fully documented, and firmware still defaults to that currently owned driver.

Latest photo shows Uno, L298N, HW-140, IR sensor, servo, breadboard and one TT motor. Hidden terminals/wires cannot prove actual connectivity or jumper states. The original square blue converter is a different, still-unidentified item; it is not automatically a usable second regulator.

## Firmware refactor

- Shared pin/angle/driver implementation in `firmware/RobotHardware.h`, with generated local copies for standalone Arduino IDE sketch folders.
- Gate sketch uses requested direct open=90/close=0 endpoints, limits 0..90. The motor-only sketch no longer controls the gate; the shared header retains its constants for later integration.
- L298N profile remains default. DRV8833 profile uses D5/D2 left, D6/D7 right, D8 sleep, no D4 connection. Every uploaded sketch must select the installed driver.
- Current motor commands are `r` continuous BOTH at 255/255, `x` stop, `v` status and `h` help. No automatic timeout; unknown characters stop. Gate/route/jog controls were removed from this sketch for the requested reset.
- IR now enabled by default in the sensor sketch; white=HIGH documented. Ultrasonic behavior remains diagnostic. Color/Bluetooth unused.
- Preparation tool refreshes header copies, SVGs, CSV schedule, optional PNG rendering and current portable ZIP.

## Competition intent and remaining work

Team plans to collect three balls, retain them behind the gate, travel to the opponent goal, open the gate and push all balls across using timed movement. Sensors may remain connected but will not control that route. The current sketch is only a continuous motor diagnostic, not a competition sequence.

Still needed: exact route order/timings, stable start pose/button, calibrated wheel duty/direction, verified motor ratings, regulator and driver load tests, physical gate clearance and no loss of captured balls when reopening. There is no active route in the current sketch. The assignment calls for line following and obstacle avoidance; a timing-only program may not satisfy those assessment requirements.

See [REVIEW.md](REVIEW.md) for software/diagram checks. Compilation and desktop checks are separate from hardware validation.
