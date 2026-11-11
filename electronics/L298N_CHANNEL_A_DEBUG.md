# L298N channel A investigation — 11 November 2026

## Current status

OUT1/OUT2 is not driving the motor. The team reports approximately **0.16V across OUT1/OUT2**, versus approximately **4.8V across OUT3/OUT4**, during the start-over continuous test. The fault reportedly stays with OUT1/OUT2 when motors are exchanged, including after replacing the driver. The cause is **not established**. Do not record the motor, Arduino or driver as definitively defective or repaired.

The immediate question is whether channel A is receiving a valid enable and direction combination. Enable pin selection/wiring is under investigation. The proposed hardware-enable test below has been explained but **its result has not been reported**.

## Intended firmware and baseline connections

Use the CURRENT `robot_bench/robot_bench.ino` with its local `RobotHardware.h`, profile `ROBOT_DRIVER_DRV8833=0`. Startup should print `Driver: L298N` and `TWO-MOTOR CONTINUOUS BENCH TEST`. The team's uploaded binary/version has not been independently inspected.

- `r`: both motors continuously at full duty 255/255; no automatic timeout.
- `x`: stop both; wheels may coast.
- `v`: status; `h`: help.
- No individual 1/2 jogs or gate/sensor/route behavior in this sketch.

| Arduino | Normal L298N control connection |
|---|---|
| D5 | ENA signal, ENA cap removed |
| D2 | IN1 |
| D4 | IN2 |
| D6 | ENB signal, ENB cap removed |
| D7 | IN3 |
| D8 | IN4 |

UNO 5V supplies L298N logic 5V with the separate **5V-EN regulator cap removed**. UNO GND, driver GND and HW-140 OUT- form the common reference. The team confirmed connecting HW-140 OUT+ to VM/+12V and OUT- to driver GND. Converter output is intended to be the initial 5.0V motor-test setting, not raw 3S battery voltage; exact loaded VM/logic measurements remain outstanding.

Servo/sensors are excluded from this test. Battery is the reported TERANTY 3S LiPo, 11.1V nominal / 12.6V full; it supplies HW-140 IN, not unidentified motors directly. See [current two-motor setup](robot_bench/README.md).

The old diagnostic that moved D6/D7/D8 to ENA/IN1/IN2 is a separate test. It must not be mixed into this baseline.

## Reported observations — not independent measurements

| Observation | Reported value/result | Measurement limits |
|---|---|---|
| Swap motors between output pairs | Failure follows OUT1/OUT2 | Team report; not a proof of motor health under all loads |
| Replace motor driver | Channel A symptom persists | Exact board variant/jumper states not independently verified |
| Differential OUT1-to-OUT2 during continuous test | About 0.16V | Full probe configuration and load state not independently verified |
| Differential OUT3-to-OUT4 during same reset session | About 4.8V | Output voltage is not proof of regulator current margin or both-motor operation |
| Earlier ENA reading | About 1.5V | Exact pin, battery state and loaded/unloaded conditions unclear |
| Earlier IN1/IN2 message | Ambiguous (`in1,in 2V`) | No reliable separate IN1 and IN2 readings established; do not interpret as both equal to 2V |
| Arduino D5 with driver connection removed | About 5V | Team clarified motor battery was OFF; USB supplied UNO. This is a valid unloaded GPIO check, not a loaded-drive test |
| ENA two-pin header, later report | One pin about 0V; the other about 5V | Exact pins, probe reference and connection/command state not established |
| Connection to the pin described as 5V | OUT1/OUT2 still about 0V | Does not establish that the ENA control input was actually driven |

On the common red module, the ENA jumper bridges a 5V supply pin and the ENA signal. A pin reading 5V is **not automatically the signal pin**. Connecting D5 to the supply half may leave ENA uncontrolled and can cause output contention when D5 is driven LOW. The physical board/pin must be identified, not chosen by voltage alone.

Unloaded D5 reaching 5V shows it can output a HIGH without the driver attached. It does not exclude a weakened GPIO, a driver-side load/short, poor contact, wrong pin or reference-ground problem. Conversely, a low OUT1/OUT2 differential alone does not prove a damaged output bridge: disabled ENA or equal direction inputs can also produce it.

## Next test — pending, no result yet

Temporarily enable channel A with its cap, removing the D5 wire first. This removes the external ENA signal wire from the immediate test without changing the two direction inputs.

1. Disconnect USB and all battery power. Raise the wheels or secure the motor. If the motor hums/stalls during any powered test, switch off motor power immediately.
2. Disconnect **D5 from the driver entirely** and isolate the loose wire. Do not leave D5 attached to either ENA pin.
3. Refit the **ENA cap across its two pins**. This is only the channel-A enable cap, NOT the separate 5V-EN regulator cap.
4. Keep **5V-EN removed** because UNO still supplies driver logic 5V. Do not refit the regulator cap while UNO 5V is connected.
5. Keep D2->IN1 and D4->IN2, using the normal baseline. Keep channel B wiring unchanged if present. Use `robot_bench`, not gate/sensor firmware or the old swapped-channel sketch.
6. Restore UNO USB first, then the verified regulated motor supply. Send `r`. Observe whether the OUT1/OUT2 motor now turns.
7. If it does not turn, stop motor power. For further measurements, disconnect a stalled motor while all power is off rather than maintaining a stall. Use DC VOLTS, black meter lead COM and red lead V/ohms, never A/mA across output/supply pairs.
8. Position probes with power off, restore power and send `r` for each reading. Relative to driver GND, expected logic is IN1 about 5V and IN2 about 0V; both ENA header pins should be about logic 5V with the cap fitted. Also measure logic 5V->GND and VM->GND. Measure motor output differentially: red OUT1, black OUT2.

| Result | Next interpretation/check |
|---|---|
| Channel A turns with ENA cap and D5 disconnected | Bridge can drive in this condition. Focus on original ENA signal wire/pin selection or loaded D5 drive, not replacing motors |
| Still fails; IN1/IN2 are not approximately HIGH/LOW | Check D2/D4, input contacts, actual uploaded firmware and common reference |
| Still fails; VM or logic 5V missing/sagging | Correct power/connection/current-limit problem first; do not increase voltage blindly |
| Still fails; enable, direction, supply and terminals all verified | Output connection or damaged channel A becomes more likely; not yet proven by current evidence |

This cap configuration is a **temporary diagnostic exception** to the normal cap-off diagram. To restore normal PWM control, power off, remove the ENA cap, then reconnect D5 to the verified ENA signal. Never connect D5 while the cap ties that node to logic 5V. With the cap installed, software cannot deassert ENA; `x` sets equal direction inputs to stop drive, but the physical motor-power switch remains the decisive disconnect.

## Record next

Fill in readings with wiring, probe reference, command, battery/regulator state and motor connected/disconnected noted:

- ENA-cap test motor result: **not reported**.
- Driver VM to GND while `r`: **not reported**.
- Driver logic 5V to GND while `r`: **not reported**.
- IN1 to GND while `r`: **not established**.
- IN2 to GND while `r`: **not established**.
- OUT1 to OUT2 with ENA cap, D5 disconnected: **not reported**.
- Photo/continuity confirmation of ENA signal versus supply pin: **not established**.

No firmware changes or physical repair are claimed by this report. It records team observations and the pending diagnostic procedure.
