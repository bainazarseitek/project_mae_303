# Complete connection schedule

For the CURRENT isolated two-motor test, use [robot_bench/README.md](robot_bench/README.md) and [its wiring picture](robot_bench/wiring-two-motors.png). It keeps the normal L298N signal map below, but temporarily uses the existing HW-140 for motor power with the servo/sensors disconnected. The following two-regulator schedule is the later complete build, not a prerequisite for that isolated bench test.

Use the recommended DRV8833 plan when the extra parts are installed; use the separately labelled L298N fallback while retaining that driver. Never combine their control tables. Pin names describe electrical connections, not physical left/right terminal order. Diagram symbols represent terminals, not a photo layout.

## Power distribution: both plans

| From | To | Notes |
|---|---|---|
| LiPo main + | Fuse input | Fuse near battery; select value with wire/load data |
| Fuse output | Main switch input | DC-rated switch |
| Switch output | BAT+ distribution | Only raw-battery positive rail |
| LiPo main - | GND distribution | Robust power terminal |
| BAT+ / GND | HW-140 IN+ / IN- | Existing regulator |
| BAT+ / GND | Motor buck IN+ / IN- | Second regulator, required for separate motor voltage |
| HW-140 OUT+ | 5V distribution | Fixed 5.0 V, checked unloaded and under load |
| HW-140 OUT- | GND distribution | Common reference, power return |
| Motor buck OUT+ | MOTOR+ distribution | <=motor rating; 6.0 V only for confirmed 6 V pair |
| Motor buck OUT- | GND distribution | Common reference, power return |
| 5V distribution | Servo red | Direct power branch, no Uno/breadboard path |
| GND distribution | Servo brown/black | Direct return branch |
| MOTOR+ distribution | Driver motor input | DRV8833 VIN/VM, or L298N VM/+12V |
| GND distribution | Driver GND | Direct power return |
| GND distribution | Uno GND | Reference lead, not motor return |
| 5V distribution | USB power adapter +5V | Mobile mode only; verified adapter/lead |
| GND distribution | USB power adapter GND | Mobile mode only |
| Adapter USB cable | Uno USB-B | Mobile; computer USB disconnected |
| Computer USB cable | Uno USB-B | Programming; mobile USB cable disconnected |
| Uno 5V / GND | Sensor 5V / GND rails | Low-current logic only; independent of servo power path |
| Uno 5V | L298N 5V logic | L298N fallback only; remove 5V-EN |

Leave Uno VIN, barrel jack and 3.3V unused. Do not connect HW-140 OUT+ to Uno 5V header or motor regulator OUT+. A single USB socket/source selects mobile versus programming power. Do not add a Y cable or join computer VBUS to regulator output. During desktop programming, sensor power comes from computer USB; servo/motor power remains battery-sourced when enabled.

Confirm both regulators permit common input/output negatives before using this ground schedule; see POWER.md for current-shunt variants. A basic 3S cell alarm plugs into the battery balance connector with the manufacturer's pin order; it does not carry motor current. An automatic cutoff requires its own rated module wiring, not an assumed connection through the Uno. Unplug the battery from the robot and use its specified main/balance connections for charging.

## Recommended driver: Pololu DRV8833 carrier #2130

| Uno / supply | Driver connection |
|---|---|
| D5 | AIN1 (left PWM) |
| D2 | AIN2 (left direction/second input) |
| D6 | BIN1 (right PWM) |
| D7 | BIN2 (right direction/second input) |
| D8 | SLP / nSLEEP; firmware keeps LOW while stopped |
| MOTOR+ | VIN / motor input, not raw battery |
| GND | GND |
| AOUT1 / AOUT2 | Left motor two leads |
| BOUT1 / BOUT2 | Right motor two leads |
| D4 | Unconnected/reserved |

No separate logic 5V input is required by this carrier. Leave fault/status output unconnected for now. Do not ground or alter current-sense resistors on a substitute board without its documentation. This plan relies on the carrier's documented biasing, decoupling and current capacity. Pin labels vary; verify the actual board. Set `ROBOT_DRIVER_DRV8833 1` in EVERY uploaded sketch.

Firmware uses one PWM-capable input and one direction input per motor. Positive commands use drive/coast; negative commands use drive/brake with inverted PWM, as specified by the DRV8833 truth table. Equal numbers in opposite directions need not give equal physical speed. Calibrate turns under load; this is not closed-loop speed control.

## Existing L298N fallback

| Uno / supply | Driver connection |
|---|---|
| D5 | ENA; remove ENA cap; add 10k pull-down to GND |
| D2 | IN1 |
| D4 | IN2 |
| D6 | ENB; remove ENB cap; add 10k pull-down to GND |
| D7 | IN3 |
| D8 | IN4 |
| Uno 5V | Driver 5V logic; remove 5V-EN jumper |
| MOTOR+ | VM/+12V terminal |
| GND | Driver GND |
| OUT1 / OUT2 | Left motor two leads |
| OUT3 / OUT4 | Right motor two leads |

Keep `ROBOT_DRIVER_DRV8833 0` in every uploaded sketch. Do not connect battery directly to VM for unidentified motors. The final fallback also uses a separate motor regulator; the previous shared 5 V test is superseded.

## Servo, start and all optional sensors: both plans

| Device | Terminal | Connection |
|---|---|---|
| Gate servo | Orange/yellow | Uno D9 |
| Gate servo | Red / brown | 5V distribution / GND distribution |
| Start button | Normally open contacts | D3 and GND |
| Left HC-SR04 | VCC / GND | Uno-powered 5V sensor rail / GND |
| Left HC-SR04 | TRIG / ECHO | A0 / A1 |
| Right HC-SR04 | VCC / GND | Same 5V sensor rail / GND |
| Right HC-SR04 | TRIG / ECHO | A2 / A3 |
| Reflective IR module | VCC / GND | Same 5V sensor rail / GND |
| Reflective IR module | OUT / S / DO | A4; confirmed white=1 |
| TCS3200/TCS230-style color module | VCC / GND | Same 5V sensor rail / GND |
| Color module | S0 / S1 / S2 / S3 | D10 / D11 / D12 / D13 |
| Color module | OUT | A5 |
| Color module | LED | Leave unconnected pending module-specific control confirmation |
| Bluetooth board | All | Leave disconnected until exact breakout and logic levels are identified |
| Unidentified square converter | All | Leave disconnected |

D0/D1 stay free for upload/Serial. A4/A5 are occupied by sensors, so the same pins cannot also be used for I2C. Servo uses Timer1; wheel PWM remains on D5/D6. Color control on D10 is ordinary digital output, not Timer1 PWM. The color module is not read by any existing sketch; optional connection is not functional integration. Only one VCC and one GND connection are needed on modules with duplicated power pins.

Connect low-current sensor power via labelled distribution/breadboard rails; check any rail split. Keep grounds common even if a sensor is not being read. Sensors stay optional and do not determine final timed-route motion. For sensor-only tests disable the battery motor/gate branch and use computer USB.

## Before power-on

Trace every named net, verify polarity and regulator outputs, inspect removed jumpers, check continuity with supplies disconnected, then test gate and wheels separately. The latest photo cannot confirm hidden terminals, solder joints, jumper state or actual wire destinations. See POWER.md for remaining electrical acceptance checks and INVENTORY.md for added parts.
