# Soccer robot: first electronics session

Use the included `wiring.svg` (and PNG) with `robot_bench/robot_bench.ino`. This is a USB bench setup, not a finalized battery wiring plan. Switch off and disconnect power before changing wires. Test motor commands with wheels raised and test the servo without the gate bar fitted first.

## Inventory from the photo

| Visible item | Quantity | Identification and use |
|---|---:|---|
| Arduino UNO R3-style board | 1 | Main controller; USB-B socket |
| Red L298N-style dual motor driver | 1 | Controls the two drive motors; black heatsink |
| Yellow TT-style geared DC motors | 2 | Left and right drive; exact rated voltage/current not readable |
| Blue SG90-style micro servo | 1 | Raises/lowers the gate; label/rating should be checked |
| HC-SR04-style ultrasonic modules | 2 | Optional distance measurements later; not wired in this first test |
| Bluetooth serial board | 1 | Appears HC-05/HC-06 family; exact variant unconfirmed; leave disconnected |
| Narrow three-pin IR board | 1 | Appears reflective line sensor; exact model/pin order unconfirmed |
| Square blue power board | 1 | Appears DC-DC related; chip, input/output and rating unconfirmed; leave disconnected |
| Wheel assembly | At least 1 visible | Confirm a second matching wheel and correct motor shaft fit |
| Metal caster | At least 1 partly visible | Lower right; confirm usable size and mounting |
| Servo horns and screws | 1 bag | White parts near top |
| Jumper wires / short red-black wires | Assorted | Signals / suitably sized power connections |
| Bag of brackets/fasteners | 1 | Exact contents unconfirmed |

No identifiable robot battery, USB cable, power switch, start button, multimeter, or regulated servo supply is visible. The green/silver wrapper near the wheel appears unrelated; do not treat it as a battery. Opaque packaging cannot be inventoried.

## What you need on the bench

- USB data cable with a USB-B end for UNO, and laptop with Arduino IDE.
- Multimeter to verify voltage and polarity.
- Motor supply matched to the actual motors, with enough current for both starting together. Ask the lab technician for an adjustable current-limited bench supply. Read the motor/seller rating first; do not assume every yellow motor is rated the same.
- Regulated servo supply matched to the servo rating (SG90-type is normally around 5V). A 2A-capable supply is a reasonable capacity allowance for one small servo; this is capacity, not a claim that it constantly draws 2A. Do not take servo power from the UNO or the L298N's small onboard regulator.
- Motor wires, soldering iron, heat-shrink, screwdriver, and a terminal distribution point. Route motor and servo current through proper power wiring, rather than thin signal jumpers/breadboard rails. Solder motor terminal connections with strain relief if wires are absent.
- For later untethered operation: suitable battery, correct charger, switch and correctly identified regulator. Battery selection remains open until motor and regulator ratings are known.

## Wiring

Use the printed labels on YOUR module. The diagram is functional, not a photograph of terminal positions. Confirm the usual L298N module arrangement before using it: ENA/ENB jumpers and a separate regulator jumper normally marked 5V-EN.

1. Disconnect all power. Remove ENA and ENB jumpers so Arduino can control motor speed. Remove 5V-EN to disable the module regulator for this particular wiring arrangement. Do not remove random jumpers if markings differ.
2. Wire UNO D5 -> ENA, D2 -> IN1, D4 -> IN2, D6 -> ENB, D7 -> IN3, D8 -> IN4.
3. Connect the left motor to OUT1/OUT2 and right motor to OUT3/OUT4. Neither motor lead connects directly to GND or UNO.
4. With 5V-EN removed, UNO 5V -> driver 5V logic input; UNO GND -> driver GND. UNO receives its power through USB.
5. Motor supply positive -> driver motor supply terminal (VM/+12V); motor supply negative -> driver GND. The +12V label does NOT mean these motors require 12V. The L298 loses a noticeable amount of voltage internally; if motors are weak, measure the supply/outputs and investigate current capability instead of raising voltage blindly.
6. Servo signal (usually orange/yellow) -> UNO D9. Servo red -> separate verified servo supply positive. Servo brown/black -> that supply negative.
7. Join servo supply negative to the same common GND as UNO and the driver. All grounds connect; the separate positive supply rails do NOT connect together. Leave UNO VIN, barrel jack and 3.3V unused in this bench setup.
8. Optional later: normally-open button between D3 and GND; no external resistor is needed with INPUT_PULLUP. For now use Serial Monitor.

Never connect the unknown blue power board by guessing. Verify its marking, input/output terminals and measured output first. Do not feed battery voltage into UNO 5V or the servo. Do not power the motors from UNO.

## Step-by-step session

1. Leave everything disconnected except UNO USB. Install Arduino IDE from https://www.arduino.cc/en/software . Choose Arduino AVR Boards -> Arduino Uno and the board's port. Upload File -> Examples -> 01.Basics -> Blink. Confirm the onboard LED blinks. This proves cable/upload work.
2. Open `robot_bench/robot_bench.ino`. If Servo.h is missing, install the Servo library by Arduino through Library Manager. Verify, then upload. Open Serial Monitor at 115200 baud. It should print Ready; send `h`. Booting does not start a route or command the servo.
3. Disconnect USB, complete the L298N logic/signal wiring above, and attach ONE motor first. Verify supply polarity/voltage with a meter. Reconnect USB, then switch on motor supply. Raise the motor/wheel. Send `1`: left motor runs for 400ms and stops. Send `x` for an earlier stop. If that wheel's positive direction is backwards, switch its two motor leads with all power off, OR change LEFT_SIGN to -1 and reupload (choose one method).
4. Switch power off, attach the second motor, restore power. Send `2`; correct RIGHT_SIGN if needed. Now `f` must turn both wheels in the robot's forward direction. Motor direction depends on how the motors are physically mounted.
5. Switch power off before adding the servo. Keep the gate bar off. Connect signal and separate servo supply as above; check common GND. Restore power, send `n` to center at 90 degrees. Use `+` and `-` to move 2 degrees at a time (software limits 30..150). Never force the shaft. If the servo buzzes against a stop or wiring is hot, disconnect its power. `x` stops wheels but does not remove servo power or holding torque.
6. At a known angle, switch off power and fit the servo horn/bar so its mechanical travel is free. Restore power, use small adjustments to find open and closed positions. Record the printed angles. Change OPEN_ANGLE, CLOSED_ANGLE and GATE_CALIBRATED=true in the sketch and reupload. Test `o` and `c`. If necessary, remount the horn; do not force the mechanism to reach arbitrary angles.
7. Secure electronics to the printed chassis with insulation/standoffs, no loose terminals against metal. Fit wheels/servo. Send `f` on the floor and adjust LEFT_PWM/RIGHT_PWM separately until short runs are straight. The jog lasts only 400ms. Do not drag a tether during full field tests.
8. Measure and enter TO_BALL_MS, TO_GOAL_MS, PUSH_MS and optionally TURN_MS. Zero turn means no turn; TURN_RIGHT chooses direction. Keep ROUTE_CONFIGURED=false until angles, wiring and times have been checked. Values are milliseconds: 1000 is one second. Time is calibrated experimentally; distance alone cannot determine it reliably.
9. Set ROUTE_CONFIGURED=true, upload, place at repeatable start and send `g`. Sequence: open -> drive to ball -> stop -> close -> turn if needed -> drive to goal -> stop -> open -> push -> stop. Send `x` to abort drive motion. It never repeats automatically. First try the route with wheels raised. The actual drive motor power switch is the reliable way to remove motor power; Serial Monitor is a bench convenience.
10. For untethered starts, use the optional D3 button once the mobile power arrangement is verified. Release the button after powering up, then press once. A held button does not repeat the route. Keep the USB power arrangement during bench work; do not improvise a battery connection to UNO.

Timing alone can miss after slip, battery changes or contact. This sketch is for building and measuring the simple mechanism; it does not yet implement line following or obstacle avoidance. Do not present disconnected sensors as functional features.

## Commands (Serial Monitor, 115200)

| Command | Result |
|---|---|
| h | Help |
| 1 / 2 | Left / right motor forward, 400ms |
| f / b | Both motors forward / back, 400ms |
| l / r | Turn in place left / right, 400ms |
| x | Abort route and stop wheels; servo keeps holding |
| n | Servo to 90 degrees, bar off for first test |
| + / - | Servo +2 / -2 degrees |
| o / c | Open / close after calibration flag is enabled |
| g | Run route once, only after both flags and timing are configured |

## Troubleshooting

- Upload fails: check data cable, UNO selection and port; leave Bluetooth disconnected from D0/D1.
- No motor motion: check external motor supply, common GND, 5V logic input, ENA/ENB jumper removal, and motor terminal wiring. If a motor only hums, stop and inspect supply current limit/load rather than leaving it stalled.
- UNO restarts when servo moves: servo power is inadequate, shared wiring drops voltage, or it is mechanically stalled. Give servo its own adequate supply and short power wiring; keep grounds common.
- One wheel backwards: change only that wheel's SIGN or swap its two leads with power removed.
- Servo moves but wheels change speed unexpectedly: keep motor PWM on D5/D6 as provided; the Servo library uses a timer that affects PWM on D9/D10 on UNO R3.
- Driver gets excessively hot: stop; inspect motor load/current, supply voltage and wiring. The heatsink does not guarantee the module can deliver its advertised peak current continuously.

## Documentation checked

- UNO R3: https://docs.arduino.cc/hardware/uno-rev3/
- PWM pins: https://support.arduino.cc/hc/en-us/articles/9350537961500-Use-PWM-output-with-Arduino
- Arduino upload procedure: https://support.arduino.cc/hc/en-us/articles/4733418441116-Upload-a-sketch-in-Arduino-IDE
- Common L298N module guide (confirm your board matches): https://handsontec.com/dataspecs/L298N%20Motor%20Driver.pdf
- L298 chip data: https://www.st.com/resource/en/datasheet/l298.pdf
- Genuine SG90 reference (clones may differ): https://towerpro.com.tw/product/sg90-analog/

This photo cannot verify pin labels, supply ratings or component authenticity. Close-up photographs of the L298N labels, blue power module front/back, motor markings and battery label are needed to finalize the portable power plan.

## Verification performed

The diagram was rendered and visually checked. The sketch was compiled as C++ against mocked Arduino/Servo interfaces, and a simulation checked idle startup, locked uncalibrated route, automatic jog stop, manual stop, servo angle limits, route completion and route abort. This is not an AVR build or a hardware test. Arduino IDE/AVR tools and a connected UNO were not available here; use Verify/Upload and the bench sequence above on your computer.
