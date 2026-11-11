# Continuous OUT1/OUT2 voltage test — L298N only

This separate sketch is for the temporary diagnostic wiring requested in chat:

| UNO | L298N |
|---|---|
| D6 | ENA signal pin, ENA cap removed |
| D7 | IN1 |
| D8 | IN2 |
| 5V | Logic 5V terminal, 5V-EN cap removed |
| GND | Driver/common GND |
| Verified motor-rated supply positive | VM/+12V |
| Motor supply negative | Driver GND |
| Motor leads | OUT1 and OUT2 |

Disconnect the original D5/D2/D4 wires from the driver, leave ENB/IN3/IN4 and OUT3/OUT4 unused, and disconnect the gate servo supply during this diagnostic. All changes are made with USB and battery power disconnected. Keep reset-state enable pull-downs described in CONNECTIONS.md. This is not a DRV8833 sketch and does not use RobotHardware.h.

1. Open this folder's `motor_continuous_test.ino` in Arduino IDE. Keep motor power off during upload. Select Arduino Uno and the actual port; upload.
2. Raise the wheel or secure the bare motor so its shaft is free. Check that motor VM receives a regulated supply within the motor rating. Do not connect the raw 11.4V/3S LiPo based on the driver's +12V terminal label.
3. Set the multimeter to DC VOLTS, above the expected motor voltage (e.g. 20V DC manual range for a low-voltage motor rail). Black lead in COM; red lead in V/ohms, NOT A/mA. Never bridge OUT1/OUT2 with a meter in current/continuity/resistance mode while powered.
4. Clip probes while all power is off: red to OUT1, black to OUT2; avoid shorting adjacent contacts. A motor may stay connected for a loaded reading if it rotates freely. For a no-load driver voltage check, disconnect motor wires with power off and measure across the empty output pair.
5. Restore USB and the verified motor supply. Open Serial Monitor at 115200. Boot is stopped. Send `r` to run continuously. The sketch drives ENA HIGH with fixed opposite input states, so the meter does not need to average PWM.
6. Read voltage across OUT1/OUT2. It should become nonzero; driver losses mean it can be lower than VM. A minus sign usually means reversed probes/polarity, not a failure. Exact output cannot be predicted without supply/current measurements.
7. Send `x` to stop drive. Wheels may coast. There is NO automatic timeout while running. If the motor hums/stalls or wiring/driver overheats, disconnect motor power immediately rather than waiting for software. Unknown commands and line endings do not stop this continuous run.
8. If output is near zero, send `x`, switch off, move probes to VM and GND, restore power and repeat `r` to check supply sag. To inspect logic with the meter, ENA/IN1/IN2 should be approximately 5V/5V/0V relative to GND while `r` is active. Switch off before moving connections. Supply/logic checks help distinguish missing VM, regulator current limiting, control-wire faults and bridge faults.

Commands: `r` run, `x` stop, `h` help. No motor movement on startup, no servo/sensor behavior, and no automatic route. Opening Serial Monitor usually resets UNO and stops this sketch; a lost Serial connection after starting does not guarantee a stop. Keep the physical power disconnect accessible.

Verification: mock C++ compile and command-state checks only; physical output voltage and operation require the team's bench test.
