# Start over: two motors continuously, normal L298N wiring

This is the CURRENT `robot_bench` behavior. It replaces the earlier 400ms jog/route example. It uses no servo, sensors, start button or autonomous route. Keep those devices disconnected for this diagnostic. The previous one-channel `motor_continuous_test` is a different sketch with a temporary pin swap; do not use its wiring here.

Current unresolved hardware fault and pending temporary ENA-cap test: [channel A investigation](../L298N_CHANNEL_A_DEBUG.md). The cap-on test is an exception to the normal wiring below and requires disconnecting D5 first. Do not confuse ENA with the separate 5V-EN regulator jumper.

Open `robot_bench.ino` together with its accompanying `RobotHardware.h` in Arduino IDE. Profile `ROBOT_DRIVER_DRV8833=0` is the default and matches the owned L298N. The shared header still implements the optional DRV8833 profile, but this reset-test diagram is L298N only. A successful compile or command message does not prove that motor VM is connected or adequate under load.

## Power for THIS bench test only

The newer battery is the TERANTY 3S LiPo, 11.1V nominal, reported 11.4V, up to 12.6V when full. The HW-140 with a numeric display is an adjustable power converter, not merely a voltmeter. A plain voltage meter does not reduce voltage and cannot replace it.

For the isolated two-motor test, with servo/sensor supply disconnected, use the existing HW-140 as the temporary motor regulator. This does not replace the two-regulator final design in POWER.md. Keep its output at the previously used conservative 5.0V setting for the initial low-voltage test, or below the verified motor rating if that is lower. This is not certification that the unidentified motors are 5V rated; identify their specification before loaded/extended use. Do not raise voltage to overcome L298N loss without knowing the motor rating.

| From | To |
|---|---|
| Battery main + | Fuse, then accessible DC switch, then HW-140 IN+ |
| Battery main - | HW-140 IN- |
| HW-140 OUT+ | L298N VM / +12V motor supply terminal |
| HW-140 OUT- | L298N GND, direct power return |
| UNO GND | L298N GND, reference wire |
| Computer USB | UNO USB-B |
| UNO 5V | L298N 5V logic input, 5V-EN cap REMOVED |

Set the regulator with the driver/motors disconnected: read OUT in VOLTS, not IN or AMPS; verify with a working multimeter. Then turn power off before connecting driver VM. Enable the regulator output when ready. A current limit that is too low can prevent startup; if it enters CC or the rail collapses, diagnose current/load before changing limits or voltage. Two motors can require more starting current than one.

Do not connect battery positive, converter IN+, or converter OUT+ to UNO 5V. Do not power motors from UNO. The bench return goes through HW-140 OUT-; do not add an external bridge between converter IN- and OUT- that bypasses a possible current shunt. Follow actual converter documentation.

## All six control wires — restore these, no channel swap

| UNO | L298N |
|---|---|
| D5 | ENA SIGNAL pin |
| D2 | IN1 |
| D4 | IN2 |
| D6 | ENB SIGNAL pin |
| D7 | IN3 |
| D8 | IN4 |

Remove ENA and ENB caps for Arduino-controlled enables. The signal pins align with the IN1..IN4 row on the common red board; confirm markings/continuity on the actual module. Keep 5V-EN removed for externally supplied 5V logic. Retain the final-design 10k pull-downs from ENA/ENB to GND for reset behavior. Never drive Arduino outputs while an enable cap ties those pins to 5V.

Left motor two leads -> OUT1/OUT2. Right motor two leads -> OUT3/OUT4. Neither motor lead connects directly to GND. Clamp bare copper, not insulation, in the screw terminals. Keep power wires short and suitably sized; do not run motor current through a breadboard or UNO ground pin.

## Upload and run

1. Disconnect USB and battery before changing wires. Keep gate and sensors disconnected, and raise both wheels / secure both bare motors so shafts are free.
2. Verify converter output with motor loads disconnected, switch off, then complete the power and six signal connections above.
3. Keep motor power off, connect UNO USB and upload `robot_bench.ino`. The `.h` file must remain in its folder. No Servo library is needed for this motor-only sketch.
4. Open Serial Monitor at 115200. It prints TWO-MOTOR CONTINUOUS BENCH TEST and is stopped on boot. Confirm the driver line says L298N.
5. Enable the verified motor supply, send `r`. Both motors stay commanded at duty 255/255, without an automatic timeout. With L298N and positive signs, this is static full enable, not pulsed speed control. No 1/2 selection remains.
6. Send `x` to stop both. `v` prints running/stopped status; `h` prints help. Unknown characters stop both; CR/LF/spaces are ignored. Losing USB/Serial connectivity does not guarantee a stop. Keep the physical switch accessible. If either motor hums/stalls, stop immediately rather than leaving it energized for a long measurement.
7. If a mounted wheel runs backward relative to robot forward, power off and swap only that motor's two leads, or change its SIGN constant and upload. This bench test initially checks rotation, not straight-line driving.

## Voltage measurements

Set meter to DC volts, black lead COM, red lead V/ohms (NOT A/mA). Choose a range above the measured rail, such as 20V for this 5V test. Never place a current-mode meter across supply or output terminals. Clip probes with power off and then start the test; turn power off before moving them.

1. HW-140 OUT+ to OUT-: approximately the selected 5.0V, both idle and with `r` active. A large fall suggests current limiting/supply/wiring trouble.
2. L298N VM to GND: approximately the converter output. Near zero means the motor-power connection is missing or sagging, regardless of driver LED/logic.
3. OUT1 to OUT2, then OUT3 to OUT4, while `r` active: nonzero differential voltage, generally below VM because of bridge losses. Negative sign means polarity/probe orientation; use magnitude for this check. No single exact voltage is guaranteed.
4. If one output remains near zero, inspect logic relative to driver GND. Normal positive-sign full-duty expectation is ENA=HIGH, IN1=HIGH, IN2=LOW and ENB=HIGH, IN3=HIGH, IN4=LOW (roughly 5/5/0V on each trio). Switch off before moving probes. If a motor stalls, measure with motor disconnected rather than maintaining a stall.

The changed command behavior is covered by production-header mock tests in `tests/bench/robot_bench_checks.cpp`. Hardware measurements still determine whether the converter/driver can start both motors.
