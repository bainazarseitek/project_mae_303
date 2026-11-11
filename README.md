# MAE 303 soccer robot

An Arduino Uno robot with two wheel motors, a servo-operated ball gate, and optional sensor diagnostics. The current start-over test runs both wheel motors continuously by command; the competition route is not implemented.

Start here for the current two-motor-only bench session: [instructions](electronics/robot_bench/README.md) and [exact wiring picture](electronics/robot_bench/wiring-two-motors.png). This isolated L298N test uses the existing HW-140 temporarily for motor power, with gate/sensors disconnected. The full build below is a later two-regulator architecture.

## Build references

1. [Electronics setup](electronics/README.md): assembly and testing.
2. [Inventory](electronics/INVENTORY.md): owned parts and additions needed.
3. [Power and speed](electronics/POWER.md): voltage limits and driver selection.
4. [Complete connections](electronics/CONNECTIONS.md): all power and signal wires.
5. [Recommended schematic](electronics/wiring.png), [existing L298N schematic](electronics/wiring-l298n.png), [sensor schematic](electronics/sensor-wiring.png).
6. [Progress](electronics/PROGRESS.md) and [review](electronics/REVIEW.md): reported results and remaining checks.

## Hardware decision

The TERANTY lithium-polymer battery is labelled **11.1 V, 2450 mAh, 30C**, previously measured at 11.4 V. Design for **12.6 V fully charged**. Keep the owned HW-140 at **5.0 V** for the gate/logic branch; wheel power belongs on an independent regulated rail.

For higher speed, the recommended target replaces the lossy L298N with a **DRV8833 carrier** and adds a **motor regulator**. Use **6.0 V only with confirmed 6 V-rated motors**, or replace the unidentified motors with a documented matching pair. Their maximum cannot be established from photographs. The complete L298N fallback retains existing hardware; firmware defaults to that profile.

## Sketches

| Folder | Purpose | Wheel motors | Gate |
|---|---|---|---|
| `electronics/robot_bench` | Start-over continuous BOTH-motor test | `r` runs both at full duty; `x` stops | No commands |
| `electronics/gate_test` | Gate calibration | Disabled | Direct open=90, close=0 |
| `electronics/sensor_test` | Ultrasonic and IR diagnostics | Disabled | No commands |

All use Serial Monitor at **115200 baud**. Uploading replaces the previous sketch. `robot_bench` now has no `1`/`2` jogs, servo actions or autonomous route. Assignment requirements for line following and obstacle avoidance remain unimplemented.

[RobotHardware.h](electronics/firmware/RobotHardware.h) is the shared hardware source. Run `python tools/prepare_electronics.py` after editing it to refresh the copies in each standalone sketch folder, diagrams and wire list. Each folder opens directly in Arduino IDE; install Servo by Arduino for the gate sketch. The motor-only `robot_bench` needs no Servo library. `ROBOT_DRIVER_DRV8833` defaults to `0`; set it to `1` at the top of **every sketch you upload** after fitting the recommended driver.

`references/Project_Soccer Robot_Fall 2026.pdf` is the assignment brief; `references/parts-photo.jpg` is the original photo. Rebuild the portable electronics ZIP using `python tools/prepare_electronics.py --package`. Development scripts require only Python's standard library.
