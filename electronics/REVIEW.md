# Refactor review - 11 November 2026

## Findings addressed

| Finding | Resolution |
|---|---|
| Old guides still referenced unidentified battery/regulator and 60/120 gate settings | Replaced by battery-specific power plan, owned/additional inventory and direct 90/0 endpoints |
| Gate-only and motor sketch disagreed on gate limits/positions | Canonical RobotHardware.h shared via generated local copies |
| Driver logic 5V confused with motor VM/+12V | Separate explicit connections, power branches and L298N diagram |
| Raising one shared regulator threatened the gate supply | Target uses fixed 5 V and independent regulated motor rail |
| 5 V or a bigger battery assumed to guarantee wheel speed | POWER.md covers load-dependent bridge drop, PWM duty, current and rating limits |
| Unidentified yellow motor treated as a known 6 V model | Unknown maximum explicit; documented replacement required if originals cannot be identified |
| Sensor pins and driver upgrade could conflict | Separate driver tables preserve all servo/start/sensor assignments; unused D4 on DRV8833 |
| Multiple standalone sketches could accidentally drive the upgraded bridge | Every sketch selects the driver and calls the same motor-disable implementation |
| IR test succeeded but tracked code disabled it | IR enabled by default; confirmed white=1 documented |
| ZIP/diagram snapshots could go stale | Reproducible preparation/render/package tool; live files included in rebuilt archive |

## Review boundaries

This is a proposed electrical architecture based on photos and reported results. Physical wiring, motor/servo authenticity, actual motor maximum voltage, battery health, jumpers, converter load performance, driver current/heat and mechanical gate behavior have not been measured here. A 3S full battery is 12.6 V; never raw-feed the proposed low-voltage loads.

Firmware has no voltage/current measurement, regulator setting control, battery cutoff, wheel encoders, color classification or sensor-based navigation. `x` disables motor drive and retains servo target; wheels may coast. It is not a physical emergency power disconnect. Gate angles are commanded, not measured. DRV8833 positive and negative PWM use different decay modes; loaded turn timings require calibration.

Confirm common-negative compatibility for the actual regulator variants and test current limiting after grounding. Converter shunt topology is not established from the photo; the generic ground plan cannot certify its CC behavior. A cell alarm requires human response; it is not an automatic battery cutoff.

The DRV8833 alternative requires fitting the specified carrier/compatible rated board and selecting profile 1 in every uploaded sketch. Default profile 0 matches owned L298N. Recommended motor rail 6 V is conditional on a documented 6 V pair, not certified for the photographed motors. Initial PWM stays 140 until commissioning.

## Verification record

All checks below passed on 11 November 2026. They do not validate physical wiring or motion.

- All three sketches compiled for Arduino Uno with Arduino AVR Boards 1.8.8 and Servo 1.3.0, for BOTH driver profiles (six builds). Final project code produced no warnings; the Arduino core can emit its own unused-parameter warnings on clean builds.
- Compile-time checks exercised the actual production signed-duty conversion for every integer command from -300 through +300, for both profiles: zero, saturation, forward and reverse included. These checks catch inverted reverse duty errors; they are not a time-domain motor simulation.
- Shared header copies are byte-identical. Pin allocations are unique within each profile, avoid D0/D1 and preserve wheel PWM D5/D6 independently of Servo Timer1. Canonical constants match the connection tables.
- Both gate and motor sketch angle defaults are 90/0 with 0..90 limits. The calibration flags and zero route times were reviewed to keep the example locked. New speed commands are bounded and do not start a motor by themselves.
- Three SVGs rendered to PNG and were visually inspected for readable connections and unclipped text. All local Markdown links resolve. The CSV wire schedule is regenerated from the diagram tables.
- Portable ZIP rebuilt from current code, diagrams and documentation; CRC and bundled sketch/header/diagram contents checked against live files. No environment/cache/credentials are included.

Reproduce header/pin/link checks: `python tools/prepare_electronics.py --check` from repository root. Regenerate/render/package: `python tools/prepare_electronics.py --render --package` (Edge required for PNG rendering on Windows).

Driver conversion checks use `tests/firmware/driver_checks.cpp` with the installed AVR C++ compiler. Compile with `-std=gnu++11 -mmcu=atmega328p -Wall -Wextra -Werror -ftemplate-depth=700 -Itests/firmware -Ielectronics/firmware -DROBOT_DRIVER_DRV8833=0 -c tests/firmware/driver_checks.cpp -o build/driver-checks-0.o`; repeat for profile 1. A passing compile establishes the assertions, not hardware behavior.

For real sketch builds use Arduino CLI: `compile --fqbn arduino:avr:uno --warnings all --build-property compiler.cpp.extra_flags=-DROBOT_DRIVER_DRV8833=0 electronics/robot_bench`, repeating for the other two folders and profile 1. Do not upload a profile that does not match the fitted driver.
