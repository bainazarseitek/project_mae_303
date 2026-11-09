# Sensor wiring and test for the scoop-and-gate robot

This is a separate test sketch. It replaces the driving sketch temporarily; it does not add automatic obstacle avoidance to that sketch. Motors stay disabled and the servo receives no commands. Disconnect the motor/servo battery for this test. Upload `sensor_test/sensor_test.ino` with UNO and sensors powered through USB.

## Available sensors

The parts photo shows two HC-SR04-style ultrasonic modules (two round silver transducers on each) and one apparent three-pin IR reflective line sensor (narrow black board). The Bluetooth board is a communications module, not a ball/line sensor. Keep Bluetooth disconnected for now.

## Connections

Follow the names printed on each module, not a guessed left-to-right order.

| Module | Module pin | UNO pin |
|---|---|---|
| Ultrasonic left | VCC | USB-powered 5V rail |
| Ultrasonic left | GND | GND rail |
| Ultrasonic left | TRIG | A0 |
| Ultrasonic left | ECHO | A1 |
| Ultrasonic right | VCC | USB-powered 5V rail |
| Ultrasonic right | GND | GND rail |
| Ultrasonic right | TRIG | A2 |
| Ultrasonic right | ECHO | A3 |
| IR, only after label/rating confirmation | VCC / + | 5V rail, if compatible |
| IR, only after label/rating confirmation | GND / - | GND rail |
| IR, only after label/rating confirmation | OUT / S / DO | A4 |

A0-A4 are usable as digital pins on UNO R3. These choices leave motor D2/D4/D5/D6/D7/D8, servo D9 and optional start D3 unchanged. Analog pins are labelled A0, A1, etc. on the UNO. HC-SR04 ECHO can connect directly to UNO R3's 5V logic; this is not a wiring plan for a 3.3V controller.

Distribute UNO 5V and GND through separate low-current breadboard rails or a proper distribution connector. Connect each sensor to those same rails; do not stack multiple plugs into one UNO socket. Also retain the common ground to the motor driver. Keep the rail linked to UNO 5V separate from the motor/servo battery positive. Breadboard rails are only for the sensors/control power, not motor current. Check whether your breadboard rails have a break halfway.

If the L298N 5V logic input remains connected to UNO 5V, its separate 5V-EN regulator jumper must remain removed. Sensor signal connections need male-female jumpers.

## Test sequence

1. Disconnect USB and battery. Leave the servo and motors mechanically unloaded.
2. Connect only the first ultrasonic sensor (VCC/GND/TRIG/ECHO). The second sensor may remain disconnected; its reading will be NA.
3. Plug in UNO USB, select Arduino Uno and the correct port. Open and upload `sensor_test/sensor_test.ino`. Install no extra library; this sketch uses the Arduino core.
4. Open Serial Monitor at 115200. Hold a flat book in front of the sensor at roughly 10, 20 and 40 cm. LEFT_cm should follow the distance. Do not start with a small round ball: a flat target is the better initial check. NA means no usable echo, not zero distance or a clear path.
5. Unplug USB. Connect the second sensor and restore USB. RIGHT_cm should respond to a flat target in front of it.
6. For IR, first confirm the actual board's VCC/GND/output markings and voltage. With power off, connect to the mapped pins. Set ENABLE_IR=true and upload. Move the actual black field and white tape under the head at a small, consistent gap (start a few mm). Record which IR_raw value corresponds to each surface; output polarity varies between boards. Adjust its trimmer gently and check glossy/matte patches. Do not assume 0 always means white.
7. Before mounting, test ultrasonic readings with the pocket empty/full and the gate in both positions. If loaded balls/gate cause short readings, change mounting before using readings for obstacle stops.

The sketch fires one ultrasonic transmitter every 75ms, alternating the modules; it does not fire both together. This reduces their mutual interference, but another robot's ultrasound can still interfere. Measurements time out after 25ms. It rejects echoes outside 2-400cm and prints NA. Raw IR readings are for calibration, not yet landmark logic.

## Placement and simple match logic

Initial placement: one ultrasonic near the front left, one near the front right, facing into the robot's intended travel corridor. Arrange beams so the gate, chassis and captured balls do not block them. A sensor mounted above the balls may miss low walls or low parts of the opponent, so test against actual field obstacles. Make no assumption that a mounting position is successful until tested with three balls loaded. Put the IR sensor underneath facing the field at a small fixed gap.

For the first driving program, use a short predetermined collection route with expected line crossings as landmarks. Collect -> close gate -> travel toward the known opponent goal -> stop/slow in the approach zone -> open gate -> push balls over the goal boundary -> stop. A distance reading alone cannot identify a goal: it might be a wall, a ball, the gate or another robot. A missing echo is not permission to accelerate. Initial obstacle handling should stop/abort, not improvise a complex detour.

One downward sensor can detect a crossing but cannot tell whether a line is to the left or right. It does not provide robust centering by itself. These sensors cannot identify ball ownership/color or reliably count all captured balls. The collection sequence must rely on the known starting layout unless more sensing is added. Reopening the gate for another ball can release captured balls; test this exact sequence before planning an all-ball run.

Do not drive at full speed until both motors start reliably and the chassis holds its course. Confirm actual ball arrangement and goal geometry on the field before implementing the match route. Only the sensor diagnostics are implemented here.

The CAD's "LiPo Battery" is a placeholder, not a voltage specification. If replacing the current 4.8V pack with a LiPo, check cell count, motor ratings, servo rating, regulation and charger. Do not put the new pack voltage directly on UNO 5V or the servo. Keep the existing verified bench supply arrangement until the new supply is identified.

Sources:
- HC-SR04 module and connections: https://www.sparkfun.com/ultrasonic-distance-sensor-hc-sr04.html
- HC-SR04 timing reference: https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
- Reflective sensor example (not proof of the exact photographed model): https://www.sunrom.com/m/4594

Verification: C++ mock checks cover timing, alternating triggers, invalid-echo handling and disabled motor pins. No connected-hardware test or AVR build was performed here; verify/upload in Arduino IDE and test against the actual sensors.
