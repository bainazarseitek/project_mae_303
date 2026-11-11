# MAE 303 soccer robot

An Arduino Uno robot with two wheel motors, a servo-operated ball gate, and optional sensor diagnostics. The competition plan uses measured movement times; the existing route is a locked example, not a calibrated three-ball routine.

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
| `electronics/robot_bench` | Jogs, speed calibration, locked timed-route example | By command only | 90/0 degree targets, calibration lock |
| `electronics/gate_test` | Gate calibration | Disabled | Direct open=90, close=0 |
| `electronics/sensor_test` | Ultrasonic and IR diagnostics | Disabled | No commands |

All use Serial Monitor at **115200 baud**. Uploading replaces the previous sketch. Sensors do not influence the motor/route sketch. Assignment requirements for line following and obstacle avoidance remain unimplemented.

[RobotHardware.h](electronics/firmware/RobotHardware.h) is the shared hardware source. Run `python tools/prepare_electronics.py` after editing it to refresh the copies in each standalone sketch folder, diagrams and wire list. Each folder opens directly in Arduino IDE; install Servo by Arduino for gate/motor sketches. `ROBOT_DRIVER_DRV8833` defaults to `0`; set it to `1` at the top of **every sketch you upload** after fitting the recommended driver.

`references/Project_Soccer Robot_Fall 2026.pdf` is the assignment brief; `references/parts-photo.jpg` is the original photo. Rebuild the portable electronics ZIP using `python tools/prepare_electronics.py --package`. Development scripts require only Python's standard library.
