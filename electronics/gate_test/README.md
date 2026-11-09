# Gate-only open/close test

Open `gate_test.ino` in Arduino IDE and upload to Arduino Uno. Disconnect the battery during upload. This sketch temporarily replaces the motor/sensor sketch and keeps motor-control pins LOW; ENA and ENB caps must be removed. It does not automatically cycle the gate or drive the robot.

Keep existing tested servo wiring: D9 signal; confirmed 4.8V pack positive to servo red; battery minus/common Arduino GND to servo brown/black. Do not increase the servo supply voltage when changing the motor battery later.

First test with the gate bar removed. Reconnect the tested supply after upload, open Serial Monitor at 115200, and send `n` to center, `o` to move to 120 degrees, and `c` to move to 60 degrees. Movement from center to either trial endpoint takes roughly 0.75 seconds. `+` / `-` change the target by 2 degrees. `x` stops the sweep but the servo continues holding its position; disconnect servo power if jammed/buzzing. Merely opening Serial Monitor resets many UNO boards; the sketch does not command servo motion on reset. The first servo command can move the shaft directly to center because its actual initial angle is unknown.

The 120/60 angles are trial settings, not measured gate positions. To fit the mechanism: send `n`, disconnect all power, fit the horn/bar roughly halfway between open/closed and make sure nothing binds, then restore power. Use `+` / `-` and read the angles printed on arrival. Enter the measured angles as OPEN_ANGLE and CLOSED_ANGLE in the sketch and reupload with the battery disconnected. Swap those angle values if opening direction is reversed. Avoid travel that presses into mechanical stops. Soft limits are 30..150 degrees; remount the horn rather than forcing a mechanism to match those limits.

Software checks: mock compilation and simulated commands/sweep checks only. No AVR compilation or hardware gate validation was possible here; Verify/Upload in Arduino IDE and confirm real mechanical clearances.
