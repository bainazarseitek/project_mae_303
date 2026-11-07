# MAE 303 Soccer Robot

## Start here

1. Read [electronics/README.md](electronics/README.md) for the inventory, wiring and test sequence.
2. Open [electronics/wiring.png](electronics/wiring.png) beside your workspace.
3. Open [electronics/robot_bench/robot_bench.ino](electronics/robot_bench/robot_bench.ino) in Arduino IDE. Keep the sketch inside its matching `robot_bench` folder.
4. Start with USB-only Blink, then test one motor, the other motor, and finally the servo with the gate bar removed.

## Files

- `electronics/robot_bench/robot_bench.ino`: motor jog tests, servo calibration, optional start button, and a timed route that stays locked until calibrated.
- `electronics/README.md`: full setup and testing instructions.
- `electronics/wiring.png`: connection diagram.
- `electronics/wiring.svg`: editable diagram source.
- `electronics/soccer-electronics.zip`: portable copy of the electronics package.
- `references/Project_Soccer Robot_Fall 2026.pdf`: assignment brief.
- `references/parts-photo.jpg`: supplied photo of the available components.

## Current status

Hardware: UNO R3-style board, L298N-style driver, two geared motors, SG90-style servo, optional sensors/Bluetooth. Exact motor ratings, battery and blue power converter remain unconfirmed. Confirm these before finalizing battery wiring.

The diagram describes a USB-powered bench setup. The controller's software logic passed mock C++ simulation checks; actual Arduino compilation, upload and hardware tests remain to be done. Gate angles and route durations are deliberately not calibrated yet.

The code currently implements timed movement, not line following or obstacle avoidance. Confirm the assessed navigation requirements before treating it as the competition program.
