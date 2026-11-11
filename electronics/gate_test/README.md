# Gate-only test

Open `gate_test.ino`, select the driver profile at the top (`0` existing L298N; `1` DRV8833), and upload to Arduino Uno. Switch battery-fed motor/gate power off during upload. Uno stays powered by computer USB. The selected driver is disabled by this program.

Servo: orange signal to D9, red to HW-140 OUT+ at verified 5.0 V, brown to power GND. Regulator OUT- and Uno GND share the common power ground. Never connect the LiPo directly to the servo.

With the bar removed, enable gate power and open Serial Monitor at 115200 baud. `o` commands 90 degrees, `c` commands 0 degrees, `n` commands 90. `+`/`-` adjust 2 degrees within 0..90. Commands set the endpoint immediately; physical travel speed is set by servo/load/supply. No motion is commanded on boot. These endpoints are user-requested, not verified mechanical limits.

`x` retains the last target; it cannot stop physical travel halfway. Disconnect servo power if jammed or buzzing. Check mechanical clearance before fitting the bar and test retention/release with actual balls.

Angles/pins come from `../firmware/RobotHardware.h`, copied into this folder by `python tools/prepare_electronics.py` from the repository root. Edit the canonical file and regenerate to keep the motor and gate sketches consistent. See `../README.md` for commissioning and `../REVIEW.md` for verification results.
