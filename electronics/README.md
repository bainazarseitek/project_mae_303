# Electronics assembly and calibration

Updated 11 November 2026. [CONNECTIONS.md](CONNECTIONS.md) is the wiring reference; [POWER.md](POWER.md) explains voltage and speed. Obscured wire ends in the latest photo cannot establish actual connections: trace each wire with power disconnected.

Current start-over bench session: [two motors only](robot_bench/README.md), with [its own wiring picture](robot_bench/wiring-two-motors.png). Use this before the full assembly below. Gate and sensors are disconnected; the existing HW-140 temporarily regulates motor power and UNO stays on computer USB.

## Choose the driver once

Recommended target: [wiring.png](wiring.png), a documented 6 V motor pair, DRV8833 carrier, separate motor regulator, existing HW-140 at 5 V, and USB power for the Uno. These additions are proposed; the photographed robot still has an L298N.

To retain the L298N use [wiring-l298n.png](wiring-l298n.png). It uses the same two regulated branches and existing control pins. It loses more voltage, so generally gives less speed at the same supply. Do not authorize higher voltage by assuming a fixed driver drop.

| Item | Existing L298N | Recommended DRV8833 |
|---|---|---|
| Setting in every sketch | `ROBOT_DRIVER_DRV8833 0` | `ROBOT_DRIVER_DRV8833 1` |
| Motor rail | Separate regulator, no higher than verified motor rating | Same; 6.0 V only for verified 6 V motors |
| Logic supply | Uno 5V to driver 5V; remove 5V-EN | No separate logic 5V on specified carrier |
| Control wires | D5/D2/D4 and D6/D7/D8 | D5/D2, D6/D7, D8 sleep; D4 unused |

## Assembly order

1. Disconnect USB and battery. Install a matching battery connector, fuse near battery positive, accessible main switch, and positive/negative distribution terminals. Coordinate fuse and wire ratings with actual startup/load current and the weakest connector; an exact fuse rating is not established.
2. Connect switched battery power to BOTH regulator inputs. Keep positive outputs separate. With loads disconnected, meter-check HW-140 at **5.0 V** and the motor regulator at the documented motor rating or lower. Original motor maximum is unknown.
3. Servo: red directly to 5 V distribution, brown to its ground return, orange to D9. No servo/motor current through Uno or breadboard.
4. Driver: follow CONNECTIONS.md, including motor-power input. L298N `5V` is logic power; `VM/+12V` is motor power, not an instruction to supply 12 V.
5. Connect one motor across each output pair. Neither lead goes directly to GND. Fit 100 nF ceramic capacitors across each motor, short power leads and strain relief.
6. Add start button and optional sensors at reserved pins. Leave Bluetooth disconnected until identified. See POWER.md for local decoupling and motor-driver reset behavior.
7. Programming: computer USB alone powers Uno. Mobile operation: HW-140 5 V to a verified USB power adapter/lead into the Uno USB-B socket. Physically use ONE USB source; no Y cable. Never feed external power into the Uno 5V header. Its 5V header supplies only low-current logic/sensors here.

## Test order

1. Verify disconnected-load polarity and regulator voltages using a working meter. Display-only adjustment was a provisional observation, not final validation.
2. Select Uno/port, verify Blink and install Servo by Arduino. Set the correct driver profile in every sketch before uploading. Disable motor/gate supply while uploading.
3. Gate: upload `gate_test`, bar removed. Send `o` (90), `c` (0), `n` (90), `+`/`-` (2 degrees). Test clearance before fitting bar. `x` cannot stop direct servo travel; remove servo power if jammed.
4. Wheels: upload current `robot_bench`, wheels raised, using its normal wiring table. `r` runs BOTH motors continuously at full duty; `x` stops both. There are no `1`/`2` jog commands. Correct `LEFT_SIGN`/`RIGHT_SIGN` or swap that motor's leads with power off. Gate/sensors remain disconnected for this start-over test.
5. Verify voltage under simultaneous starts, current limits, driver heat, lack of Uno resets and servo jitter. If a motor hums, check load/current limit before increasing CV. Do not hold motors stalled for current measurements.
6. The current test uses duty 255/255 to avoid PWM averaging during voltage measurements. This is full duty, not voltage regulation. `v` prints running/stopped status. Stop any stalled/humming motor immediately; do not leave it powered merely to take a reading.
7. Autonomous route: not present in the current motor-only sketch. First establish reliable motor power and physical gate behavior, then measure actual route timings with the final supply, load, wheels and surface before implementing the competition sequence.

| Current robot_bench command | Action |
|---|---|
| `r` | Run BOTH motors continuously, full duty, no timeout |
| `x` | Stop BOTH; wheels may coast |
| `v` | Report running/stopped status |
| `h` | Help |

Old jog, speed adjustment, gate and route commands are not accepted by this start-over sketch. Unknown characters stop both motors. CR/LF/spaces are ignored. Use the separate gate sketch for gate actions.

Use a suitable 3S LiPo balance charger and cell monitor/cutoff. HW-140 undervoltage behavior is not per-cell battery protection. The main switch removes battery-fed motor/gate power; USB may keep Uno logic on. Timing-only driving may miss the assignment's sensor-based requirements.
