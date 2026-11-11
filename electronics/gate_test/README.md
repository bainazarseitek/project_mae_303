# Gate-only open/close test

Open `gate_test.ino` in Arduino IDE and upload to Arduino Uno. Disconnect the battery during upload. This sketch replaces the motor/sensor sketch and keeps motor-control pins LOW; ENA and ENB caps must be removed.

Keep existing tested servo wiring: D9 signal; verified 4.8V servo supply; supply negative connected to Arduino GND. Do not connect a higher-voltage motor battery directly to the servo.

Open Serial Monitor at 115200 baud. Send `o` for open at 90 degrees, `c` for closed at 0 degrees, or `n` for 90 degrees. Each command sets the endpoint immediately, without a software sweep. Actual travel speed depends on the servo, supply and load. No movement is commanded on boot; the first command moves directly toward the requested position.

`+` and `-` adjust the target by 2 degrees within 0..90. `x` retains the last target; it cannot stop the servo halfway through direct travel. Disconnect servo power if the mechanism jams or buzzes.

Test with the bar removed first, then check mechanical clearance before operating the gate. The requested 0/90 endpoints still need physical verification. Swap OPEN_ANGLE and CLOSED_ANGLE if the mechanism moves in the opposite direction.

Validation: compiled successfully for Arduino Uno using Arduino AVR Boards 1.8.8 and Servo 1.3.0. Physical speed and gate clearance require hardware testing.
