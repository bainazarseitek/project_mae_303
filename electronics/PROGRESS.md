# Bench progress - updated 11 November 2026

## Confirmed results

- UNO upload and Serial Monitor work at 115200 baud.
- IR line sensor works on the actual tested surfaces: white = 1, background = 0. Signal is assigned to A4. Keep the mounting height fixed and retest on the competition field.
- One yellow motor ran through L298N OUT1/OUT2. Another motor was unreliable through the driver but ran when connected directly to a battery. This does not establish whether the cause is motor starting torque, supply, wiring or driver.
- During troubleshooting, the team confirmed that L298N VM/+12V was not connected. Logic power and control wires alone cannot power the motors. Successful operation after completing motor-power wiring has not yet been reported.
- Earlier servo commands worked. Clearance and ball retention/release with the new 0/90 degree settings remain unconfirmed.
- Ultrasonic NA readings remain unresolved; no successful sensor hardware test has been reported.

## Battery and regulator

- The old pack was four 1.2V AA cells, nominal 4.8V. The team has replaced it with a TERANTY pack labelled 2450mAh / 30C.
- The new battery measured 11.4V. This is consistent with a standard 3S LiPo (11.1V nominal, up to 12.6V fully charged); its printed label lists both 7.4V and 11.1V variants.
- The photographed regulator is an HW-140 adjustable buck-boost module. The team reported a 5.01V reading while setting its output; the handheld multimeter was unreliable. Verify that the regulator display indicates OUT in volts, with output enabled, and confirm with a working meter when available.
- CV sets output voltage; CC sets the current limit. Neither adjustment controls steering.
- Keep the gate servo supply at 5V. Do not connect the LiPo directly to the servo or UNO 5V. Keep UNO USB-powered for bench testing.
- Servo wiring: orange signal to D9, red to regulator OUT+, brown to OUT-. Connect OUT- to UNO GND and L298N GND; all control grounds are common.
- L298N logic: UNO 5V to driver 5V with 5V-EN removed. Remove ENA/ENB caps when their pins are driven by Arduino PWM. Never join the enabled onboard regulator output to UNO USB-powered 5V.
- L298N VM/+12V is the motor-power input, not a requirement to supply exactly 12V. Motor rated voltage remains unknown: do not connect the 11.4V LiPo directly based on that terminal label.
- A shared 5V regulator output to both servo and L298N VM was suggested only as a low-voltage motor test. Motor startup/current limiting may disturb the servo, and L298N voltage loss may prevent motor startup. Hardware success with that arrangement has not been confirmed. Do not raise a shared output while the servo is connected.
- Different motor and servo supply voltages require separate regulators/supplies, with common grounds. The current wiring diagram shows the servo regulator; final motor supply selection is still pending.

## Gate code changed

`gate_test/gate_test.ino` now commands open = 90 degrees and closed = 0 degrees directly, removing the previous 25ms-per-degree sweep. Physical speed depends on servo, supply and load.

- `o`: open 90; `c`: close 0; `n`: 90; `+`/`-`: adjust 2 degrees within 0..90.
- No commanded movement on boot. Motor-control pins stay LOW.
- `x` retains the last target; it cannot stop direct travel halfway. Disconnect servo power if jammed.
- Test mechanical clearance with the bar removed first. These are requested endpoints, not verified physical calibration.
- `robot_bench` has NOT yet received these gate settings: it still has both angles at 90 and its calibration/route flags false.

## Sensors

- `sensor_test/sensor_test.ino` reads ultrasonic sensors and optionally the IR sensor. IR is still disabled in the tracked sketch (`ENABLE_IR=false`); change to true to reproduce the IR test.
- Pin map: left ultrasonic TRIG/ECHO A0/A1; right A2/A3; IR OUT A4. Sensors use USB-powered UNO 5V and common GND.
- A newly photographed TCS3200/TCS230-style color module was identified. Proposed unused-pin map: S0 D10, S1 D11, S2 D12, S3 D13, OUT A5, VCC 5V, GND common. LED control remains unconnected pending module-specific confirmation. No color-reading code or hardware test is implemented.
- Uploading any test sketch replaces the previous sketch. `gate_test` and `sensor_test` deliberately disable wheel motors; use `robot_bench` and commands `1` / `2` for individual 400ms motor tests.
- Earlier compile issue: an extra `+` after `Serial.println` in a separate Windows copy caused an error. The repository sensor sketch does not contain that typo.

## Agreed competition approach

The team wants a timed route using wheel motors and the gate servo: collect three balls, retain them, travel to the opponent goal, open the gate and push all balls across. Sensors are intended to remain connected for demonstration but not control the final route.

This final route is not implemented or calibrated. Measure forward and turn durations on the actual field with the final supply, fixed PWM values, repeatable starting pose and realistic ball load. Confirm whether the gate stays open during all collection or must cycle without losing captured balls. The start method is still undecided; the existing bench sketch supports an optional D3-to-GND button.

The assignment calls for autonomous line following and obstacle avoidance. A timed route without sensor feedback may not satisfy those assessment requirements.

## Verification and next steps

- Git and Python 3.12 installed; local `.venv` contains pypdf for reading the assignment PDF. No Python runtime dependencies are required by the robot firmware.
- All three sketches compiled for Arduino Uno using Arduino AVR Boards 1.8.8 and Servo 1.3.0 on 9 November. The revised direct-movement gate sketch also compiled successfully. Compilation does not validate hardware behavior.
- Updated `wiring.svg` and rendered/visually reviewed `wiring.png` for battery/regulator/servo wiring, motor control pins and sensor assignments. The portable ZIP is an older snapshot; use the live repository files.
- Next: identify motor voltage rating, finalize motor power, confirm both motors start reliably, verify gate clearance/retention/release, measure the field route, then implement the final sequence.
