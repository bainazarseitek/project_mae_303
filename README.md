# MAE 303 Soccer Robot

## Start here

1. Read [electronics/README.md](electronics/README.md) for the inventory, wiring and test sequence.
2. Open [electronics/wiring.png](electronics/wiring.png) beside your workspace.
3. Open [electronics/robot_bench/robot_bench.ino](electronics/robot_bench/robot_bench.ino) in Arduino IDE. Keep the sketch inside its matching `robot_bench` folder.
4. Start with USB-only Blink, then test one motor, the other motor, and finally the servo with the gate bar removed.

## Files

- `electronics/gate_test/gate_test.ino`: separate gate-only open/close test; motors disabled.
- `electronics/sensor_test/sensor_test.ino`: separate ultrasonic diagnostics; motors disabled, IR optional after identification.
- `electronics/sensors.md`: sensor wiring, tests and placement notes.
- `electronics/sensor-wiring.png` and `.svg`: sensor connection diagram.
- `electronics/PROGRESS.md`: confirmed bench results and outstanding hardware checks.

- `electronics/robot_bench/robot_bench.ino`: motor jog tests, servo calibration, optional start button, and a timed route that stays locked until calibrated.
- `electronics/README.md`: full setup and testing instructions.
- `electronics/wiring.png`: connection diagram.
- `electronics/wiring.svg`: editable diagram source.
- `electronics/soccer-electronics.zip`: portable copy of the electronics package.
- `references/Project_Soccer Robot_Fall 2026.pdf`: assignment brief.
- `references/parts-photo.jpg`: supplied photo of the available components.

## Current status

Hardware: UNO R3-style board, L298N-style driver, two geared motors, SG90-style servo, optional sensors/Bluetooth. The replacement LiPo measured 11.4V; the HW-140 regulator was adjusted to a reported 5.01V for the servo. Exact motor ratings and final motor supply remain unconfirmed.

The diagram describes a USB-powered UNO bench setup with regulated servo power. All three sketches compiled for Arduino Uno; selected hardware results are recorded in electronics/PROGRESS.md. Gate-only endpoints are now 90/0 degrees with direct movement, but physical clearance and route timings remain unverified.

The code currently implements timed movement, not line following or obstacle avoidance. Confirm the assessed navigation requirements before treating it as the competition program.

## Latest bench update (11 November 2026)

See [electronics/PROGRESS.md](electronics/PROGRESS.md) for confirmed results and open work. The IR sensor reports white=1/background=0. The new battery measured 11.4V, and the HW-140 regulator was adjusted to a reported 5.01V for the servo. Motor voltage ratings and final motor-power wiring remain unresolved.

The gate-only sketch now uses direct open=90/close=0 commands. These settings are not yet integrated into `robot_bench`, and physical gate clearance still needs checking. All three sketches have compiled for Arduino Uno. The team plans a timed motor/servo route; final route timings and sequence are not implemented. Sensor-based line following and obstacle avoidance remain absent.

Use the current files rather than the older `soccer-electronics.zip` snapshot. The wiring diagram shows the servo power arrangement; read the progress notes before selecting a motor supply.
