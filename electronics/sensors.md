# Optional sensors

The pin schedule in [CONNECTIONS.md](CONNECTIONS.md) is shared by both driver profiles; [sensor-wiring.png](sensor-wiring.png) shows it. Uno 5V powers sensor VCC; all grounds share the power reference. Motor/servo current stays off this low-current rail. Use labels, not a guessed pin order.

| Sensor | Signals | Status |
|---|---|---|
| Left HC-SR04-style | TRIG A0, ECHO A1 | NA readings unresolved |
| Right HC-SR04-style | TRIG A2, ECHO A3 | NA readings unresolved |
| Reflective IR | OUT A4 | Confirmed white=1, background=0 |
| TCS3200/TCS230-style color module | S0 D10, S1 D11, S2 D12, S3 D13, OUT A5 | Identified visually; no test code/hardware result |
| Bluetooth | Disconnected | Exact model, breakout and logic levels unknown |

## Test sketch

1. Set the correct `ROBOT_DRIVER_DRV8833` profile in `sensor_test/sensor_test.ino`. The tracked default is L298N (`0`).
2. Disable the battery motor/gate branch, connect Uno computer USB, and upload `sensor_test`. This replaces the motor/gate program and keeps wheel outputs disabled.
3. Open Serial Monitor at 115200. IR is now enabled by default. If the IR module is unplugged, set `ENABLE_IR=false`; a floating/pulled-up disconnected input is not a valid white-line result.
4. Keep IR head facing down at a repeatable small gap (start a few mm). Test actual field tape/background and adjust sensitivity gently. Confirmed polarity applies to this module/test, not every sensor.
5. Test one ultrasonic module at a time against a flat book at about 10, 20 and 40 cm. Check VCC/GND/TRIG/ECHO and board pin labels. Another disconnected module will report NA.

The HC-SR04 protocol uses a 10 us trigger, timed echo and distance in cm = echo_us/58. The sketch alternates transmitters every 75 ms, times out at 25 ms, and rejects outside 2-400 cm. NA means no usable echo or rejected range, not zero distance or an obstacle-free path. The trigger/echo plan is for 5 V Uno R3, not 3.3 V boards. [HC-SR04 datasheet](https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf)

The color module measures filtered light as pulse frequency; it requires channel-selection/readout code and close-range calibration, not a single HIGH/LOW read. Keep its LED-control pin unconnected until the actual module's control circuit is confirmed. No implemented sketch reads it. [Comparable TCS3200 module](https://www.waveshare.com/wiki/Color_Sensor)

One downward IR sensor detects white/background but does not directly locate the line left versus right. Ultrasonic distance cannot identify ball ownership, goals, or reliably count three balls. Connected sensors do not add navigation to the planned timed route.

For later integration, verify readings with the gate open/closed and the pocket empty/full, avoid blocked ultrasonic beams, and calibrate lighting/height on the field. A4/A5 are reserved by IR/color OUT and cannot simultaneously serve an I2C device. D0/D1 remain available for Serial/upload.
