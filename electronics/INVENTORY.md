# Inventory and required additions

Evidence: original and latest bench photos, detailed battery/module photos, and reported tests. A visual type match does not establish exact ratings. Quantities cover the whole conversation.

| Owned item | Quantity | Status / uncertainty |
|---|---:|---|
| Uno R3-style board | 1 | Visible; upload and Serial Monitor work |
| L298N-style motor driver | 1 | Visible; jumper/terminal labels must be checked |
| Yellow TT-style gearmotors | 2 | One visible in latest photo; rated voltage, RPM, gear ratio and startup/stall current unknown; one unreliable through driver |
| SG90-style blue gate servo | 1 | Visible; actual clone voltage/current limits unknown |
| TERANTY LiPo | 1 | 11.1 V, 2450 mAh, 30C; measured 11.4 V; design for 12.6 V charged |
| HW-140 buck-boost regulator | 1 | Identified; reported 5.01 V output, no loaded validation |
| Three-pin reflective IR module | 1 | White=1/background=0 confirmed; exact model unknown |
| HC-SR04-style ultrasonic modules | 2 | Earlier photo; NA readings unresolved |
| TCS3200/TCS230-style color module | 1 | Separate photos; no color test implemented |
| HC-05/HC-06-style Bluetooth board | 1 | Breakout and logic levels unknown; leave disconnected |
| Small square blue power board | 1 | Original photo; separate from HW-140; unidentified, do not treat as verified second regulator |
| Breadboard and jumpers | Assorted | Logic only; no motor/servo supply current |
| USB cable and battery connector/lead | At least 1 each | Used/visible; verify polarity/current rating |
| Wheels, caster, horns, brackets/fasteners | Assorted | Confirm matching wheel pair, mounts and secure connections |
| Chassis, scoop/pocket, gate | Team CAD/planned | Mechanical clearance, retention/release unverified |
| Multimeter | 1 reported | Unreliable; repair/replace |
| Old four-cell AA pack | 1 reported | Superseded, not in target architecture |

## Additions for the recommended target

| Add or verify | Selection |
|---|---|
| Motor buck regulator | Input >=12.6 V with margin; output adjustable to chosen motor rating; real thermal/startup capacity for both motors. >=4 A continuous is a provisional selection target for the example pair |
| Lower-loss dual driver | Example: Pololu DRV8833 #2130, 1.2 A continuous / 2 A peak per channel. Check actual load and peak duration. Generic boards may have different capability |
| Documented matching motors, if originals cannot be identified | Example Adafruit #3777 TT pair, 3-6 V, measured stall 1.5 A each at 6 V. Not proof that your existing motors have those ratings |
| Fuse/holder, DC switch, power distribution | Select from measured current, wiring/connector capacity and startup transients; fuse near battery positive |
| 3S balance charger and cell monitor/cutoff | Correct chemistry/cell count and pack-specified cell limits; label does not prove internal protection |
| Normally-open start button | D3 to GND, internal pull-up |
| Regulated 5 V to USB power adapter/lead | Verified polarity, into Uno USB-B; no Y cable or simultaneous USB sources |
| Suppression capacitors and decoupling | 100 nF across each motor; see POWER.md for provisional bulk values |
| Power wiring, standoffs, solder/strain relief | Sized for current; keep high-current paths off breadboard |
| Working meter | Voltage/polarity/load checks; current characterization with appropriate equipment |
| L298N reset pull-downs if retained | 10k from ENA to GND and ENB to GND, with jumper caps removed |

Two regulated output rails let wheel voltage change without exposing the servo. A second regulator is required for that arrangement; a lower-loss driver is optional for basic operation and recommended for speed. Confirm/replace unknown motors before accepting 6 V as their setting.
