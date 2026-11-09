# Bench progress — 7 November 2026

## Confirmed in this chat

- UNO sketch uploaded and Serial Monitor works at 115200 baud.
- Motor 1 ran successfully through L298N OUT1/OUT2.
- Motor 2 starts intermittently / hums, including when moved to the known-working OUT1/OUT2 channel. Cause not proven; investigate motor, attached leads, mechanical load and starting power.
- Team plans to replace the suspect motor and reconsider the battery later. No replacement battery voltage or motor rating has been confirmed.
- Current reported battery: four 1.2V AA cells in series, nominal 4.8V.
- Servo responds to the test commands. Physical gate angles have not been recorded.

## Saved sensor work

- `sensor-wiring.png` and `.svg`: proposed wiring.
- `sensors.md`: sensor identification, mounting, wiring and tests.
- `sensor_test/sensor_test.ino`: ultrasonic diagnostics; IR disabled pending pin/rating confirmation. This replaces, rather than extends, the motor/servo sketch when uploaded.
- Pin allocation: ultrasonic left TRIG/ECHO A0/A1; right A2/A3; provisional IR OUT A4. Sensor power through USB-powered 5V and common GND.
- No sensor hardware success has been reported yet. Only mock software checks were performed.

## Accepted match concept

Collect own balls in the CAD front pocket, lower the retaining gate, travel toward the opponent goal, open the gate, and push balls over the scoring boundary. Exact field route, release point and landmark detection remain uncalibrated. Sensors do not identify goal/ball ownership or reliably count all captured balls. Verify that reopening for another ball does not release previously captured balls.

## Current next task: physical gate

Update: `gate_test/gate_test.ino` now provides a separate gate-only program: `o` targets 120 degrees, `c` targets 60, `n` centers, `+`/`-` adjust and `x` stops the sweep while holding position. These are trial positions only; physical gate angles remain unknown. It commands no servo movement on boot and holds motor-control pins LOW. Mock compilation/simulation passed for startup, motor disable, sweep timing, open/close, stop and angle limits. Upload and physical testing remain for the team. Use this sketch for the immediate open/close test; the original sketch below remains available for later integration.

Use `robot_bench/robot_bench.ino`, not the sensor-only sketch. Disconnect the motor/servo battery before uploading. Keep the existing tested servo supply; do not connect a future higher-voltage motor pack directly to the servo.

1. With the bar removed and tested servo wiring in place (D9 signal, correct separate supply, common GND), send `n` to center at 90 degrees.
2. Disconnect USB and battery, secure servo to the mount and fit horn/bar approximately midway through the required open/closed motion. Do not force the shaft.
3. Restore power and use `+` / `-` to move in 2-degree increments. Record open/closed angles. Disconnect power immediately if mechanically stalled/buzzing.
4. Set OPEN_ANGLE and CLOSED_ANGLE to those measured values, then GATE_CALIBRATED=true. Keep ROUTE_CONFIGURED=false.
5. Upload with battery disconnected, restore power and test `o` / `c` with actual balls. Check retention, free entry and complete release.

The saved sketch currently has both gate angles at 90 degrees and calibration flags false; no physical calibration values should be invented. Motor reliability must be resolved before an autonomous run. Do not present mock software checks as hardware validation.

9 November compile note: the repository's sensor sketch uses `Serial.println(F("Sensor test. Motors OFF. Battery disconnected."));`. An extra `+` after `println` in a downloaded/edited Windows copy causes a compilation error; remove it or use the repository copy. Ultrasonic NA readings remain unresolved; no hardware success has been reported.
