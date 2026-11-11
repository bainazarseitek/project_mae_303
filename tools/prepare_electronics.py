"""Refresh standalone headers, SVG diagrams, CSV wire list and optional PNG/ZIP.

Run from any directory. Python standard library only. --render needs Edge on
Windows; all other outputs work without a browser. See electronics/REVIEW.md.
"""
from __future__ import annotations

import argparse
import csv
import html
from pathlib import Path
import re
import shutil
import struct
import subprocess
import time
import zipfile

ROOT = Path(__file__).resolve().parents[1]
ELECTRONICS = ROOT / "electronics"
SKETCHES = ("robot_bench", "gate_test", "sensor_test")

PROFILES = {
    "drv8833": [
        ("D5", "AIN1", "Left motor PWM"),
        ("D2", "AIN2", "Left direction / second input"),
        ("D6", "BIN1", "Right motor PWM"),
        ("D7", "BIN2", "Right direction / second input"),
        ("D8", "SLP / nSLEEP", "LOW while stopped; D4 is unused"),
    ],
    "l298n": [
        ("D5", "ENA", "Left PWM; remove cap, add 10k to GND"),
        ("D2", "IN1", "Left direction"),
        ("D4", "IN2", "Left direction"),
        ("D6", "ENB", "Right PWM; remove cap, add 10k to GND"),
        ("D7", "IN3", "Right direction"),
        ("D8", "IN4", "Right direction"),
    ],
}
SENSORS = [
    ("Left ultrasonic", "TRIG / ECHO", "A0 / A1"),
    ("Right ultrasonic", "TRIG / ECHO", "A2 / A3"),
    ("IR line sensor", "OUT / S / DO", "A4; white=1, background=0"),
    ("Color sensor", "S0 / S1", "D10 / D11"),
    ("Color sensor", "S2 / S3", "D12 / D13"),
    ("Color sensor", "OUT", "A5; no color-reading code yet"),
]
POWER = [
    ("LiPo main +", "Fuse IN", "Raw battery; up to 12.6 V"),
    ("Fuse OUT", "Main switch IN", "DC-rated fuse/switch/wires"),
    ("Main switch OUT", "BAT+ terminal", "Switched battery positive"),
    ("LiPo main -", "GND terminal", "Power ground"),
    ("BAT+", "HW-140 IN+", "Existing 5 V regulator"),
    ("GND", "HW-140 IN-", "Power return"),
    ("BAT+", "Motor regulator IN+", "New independent regulator"),
    ("GND", "Motor regulator IN-", "Power return"),
    ("HW-140 OUT+", "5V terminal", "Set and verify 5.0 V"),
    ("HW-140 OUT-", "GND terminal", "Common ground"),
    ("Motor regulator OUT+", "MOTOR+ terminal", "<=verified motor rating"),
    ("Motor regulator OUT-", "GND terminal", "Common ground"),
    ("5V terminal", "Servo red", "Direct servo power branch"),
    ("GND terminal", "Servo brown/black", "Direct return; not via Uno"),
    ("GND terminal", "Uno GND", "Signal reference"),
    ("5V terminal", "Mobile USB adapter +5V", "Only mobile USB source"),
    ("GND terminal", "Mobile USB adapter GND", "Only mobile USB source"),
    ("Mobile adapter USB", "Uno USB-B", "Unplug computer USB"),
    ("Computer USB", "Uno USB-B", "Programming; mobile USB unplugged"),
]


class Svg:
    def __init__(self, height: int):
        self.height = height
        self.items = [
            f'<svg xmlns="http://www.w3.org/2000/svg" width="1800" height="{height}" viewBox="0 0 1800 {height}">',
            '<style>text{font-family:Arial,sans-serif;fill:#17324d}.title{font-size:34px;font-weight:bold}.h{font-size:27px;font-weight:bold}.t{font-size:23px}.s{font-size:21px}</style>',
            f'<rect width="1800" height="{height}" fill="#f2f6fa"/>',
        ]

    def text(self, x, y, value, cls="t"):
        self.items.append(f'<text x="{x}" y="{y}" class="{cls}">{html.escape(value)}</text>')

    def box(self, x, y, w, h, color="#fff"):
        self.items.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="12" fill="{color}" stroke="#bfcedd" stroke-width="2"/>')

    def line(self, path, color="#247ab9", dash=False):
        extra = ' stroke-dasharray="12 8"' if dash else ""
        self.items.append(f'<path d="{path}" fill="none" stroke="{color}" stroke-width="5"{extra}/>')

    def table(self, y, headings, rows, widths=(520, 520, 660)):
        # Every row has a generous fixed height; cell content kept to one line.
        self.box(50, y, 1700, 60 + len(rows) * 48)
        xs = [75, 75 + widths[0], 75 + widths[0] + widths[1]]
        for x, label in zip(xs, headings):
            self.text(x, y + 39, label, "h")
        for i, row in enumerate(rows):
            for x, value in zip(xs, row):
                self.text(x, y + 88 + i * 48, value, "s")

    def save(self, path):
        path.write_text("\n".join(self.items + ["</svg>"]) + "\n", encoding="utf-8")


def power_diagram(profile):
    drv = profile == "drv8833"
    doc = Svg(2190)
    doc.text(50, 60, "SOCCER ROBOT / " + ("RECOMMENDED DRV8833 BUILD" if drv else "EXISTING L298N / TWO-RAIL BUILD"), "title")
    doc.text(50, 103, "11 November 2026 | Functional schematic: connect by printed terminal names, not board position.")
    doc.box(50, 130, 1700, 110, "#fff0cd")
    doc.text(75, 172, "Additional parts required: motor regulator" + (", DRV8833 carrier and verified motors." if drv else "; original motor ratings still unknown."), "h")
    doc.text(75, 212, "6.0 V is permitted ONLY for confirmed 6 V motors. Do not connect raw 3S battery to motor driver or servo.")
    doc.text(50, 295, "1 / BATTERY AND TWO INDEPENDENT REGULATED OUTPUTS", "h")
    doc.box(50, 325, 360, 200)
    for yy, txt in [(367, "TERANTY LiPo / 3S"), (407, "11.1 V / 2450 mAh / 30C"), (447, "Measured 11.4 V"), (487, "Up to 12.6 V charged")]:
        doc.text(75, yy, txt, "s")
    doc.box(610, 325, 500, 200)
    doc.text(635, 367, "FUSE → MAIN SWITCH → BAT+", "h")
    doc.text(635, 410, "Battery + enters fuse first.")
    doc.text(635, 450, "Battery − → GND terminal.")
    doc.text(635, 490, "Fuse/wires selected for actual load.", "s")
    doc.line("M410 388H610", "#d23d42")
    doc.box(1310, 325, 440, 200, "#e3ebf2")
    doc.text(1335, 367, "BATTERY MANAGEMENT", "h")
    doc.text(1335, 410, "3S cell monitor / cutoff")
    doc.text(1335, 450, "3S LiPo balance charger")
    doc.text(1335, 490, "Pack limits; no charging via robot", "s")
    # No implicit rail tie: power inputs and outputs are labelled explicitly.
    doc.box(50, 580, 650, 195)
    doc.text(75, 622, "HW-140 / EXISTING", "h")
    doc.text(75, 664, "IN+ ← BAT+     IN− ← GND")
    doc.text(75, 706, "OUT+ → 5V terminal; set CV to 5.0 V")
    doc.text(75, 748, "OUT− → GND; CC is current limit", "s")
    doc.box(1030, 580, 720, 195)
    doc.text(1055, 622, "MOTOR BUCK / ADD THIS PART", "h")
    doc.text(1055, 664, "IN+ ← BAT+     IN− ← GND")
    doc.text(1055, 706, "OUT+ → MOTOR+; no higher than motor rating")
    doc.text(1055, 748, "OUT− → GND; input rated above 12.6 V", "s")
    doc.line("M860 525V550H375V580 M860 550H1390V580", "#d23d42")
    doc.box(50, 815, 1700, 140, "#e3ebf2")
    doc.text(75, 857, "POWER RETURNS / COMMON GROUND", "h")
    doc.text(75, 897, "Battery −, both regulator IN−/OUT−, driver GND, servo brown, Uno GND and sensor GND share a terminal.")
    doc.text(75, 932, "Run motor/servo returns directly to power ground. Keep 5V OUT+, MOTOR+ and BAT+ separate.", "s")
    doc.text(50, 1010, "2 / EVERY LOAD HAS ITS OWN POWER CONNECTION", "h")
    rows = [
        ("Gate servo", "Red → 5V; brown → GND", "Orange/yellow → Uno D9"),
        ("Wheel driver", "MOTOR+ → " + ("VIN / VM" if drv else "VM / +12V"), "GND → power GND; never motor via Uno"),
        ("Driver logic", "No external logic 5V" if drv else "Uno 5V → driver 5V", "No logic VCC on specified carrier" if drv else "Remove 5V-EN; ENA/ENB caps off"),
        ("Uno / programming", "Computer USB → USB-B", "Mobile power cable disconnected"),
        ("Uno / mobile", "5V → USB power adapter", "Adapter → USB-B; computer disconnected"),
        ("Sensors", "Uno 5V → VCC; GND → GND", "Low-current rail; see sensor-wiring.png"),
    ]
    doc.table(1035, ("DEVICE", "POWER", "SIGNAL / CONDITION"), rows)
    doc.text(50, 1445, "3 / UNO SIGNALS → " + ("DRV8833" if drv else "L298N"), "h")
    doc.table(1470, ("UNO PIN", "DRIVER PIN", "PURPOSE"), PROFILES[profile])
    y = 1470 + 60 + len(PROFILES[profile]) * 48 + 45
    doc.text(75, y, ("AOUT1/AOUT2 → left motor; BOUT1/BOUT2 → right motor." if drv else "OUT1/OUT2 → left motor; OUT3/OUT4 → right motor."))
    doc.text(75, y + 37, "Each motor has TWO driver leads; neither lead goes directly to ground. Fit 100 nF across each motor.", "s")
    doc.text(75, y + 74, "Firmware: ROBOT_DRIVER_DRV8833 = " + ("1" if drv else "0") + " in EVERY uploaded sketch. Default firmware is L298N.", "s")
    doc.text(75, y + 111, "Gate: open=90°, close=0°; D3-to-GND start button. Wheel PWM D5/D6 controls speed, not regulator CV.", "s")
    doc.text(50, 2085, "Verify voltage/polarity under load. See CONNECTIONS.md for all sensor wires and POWER.md for current limits.", "s")
    doc.text(50, 2123, "L298N fallback: add 10k pull-downs ENA/ENB → GND. DRV8833: nSLEEP LOW disables outputs.", "s")
    doc.text(50, 2161, "No external power into Uno 5V header. No USB Y cable. No motor/servo power through breadboard. GND is common.", "s")
    doc.save(ELECTRONICS / ("wiring.svg" if drv else "wiring-l298n.svg"))


def sensor_diagram():
    doc = Svg(1210)
    doc.text(50, 60, "SOCCER ROBOT / OPTIONAL SENSORS AND GATE SIGNAL", "title")
    doc.text(50, 105, "Shared map for both driver profiles. Signals use Uno 5 V logic; follow printed module labels.")
    doc.box(50, 135, 1700, 105, "#e3ebf2")
    doc.text(75, 177, "SENSOR POWER: Uno 5V → all identified sensor VCC; Uno GND → all sensor GND.", "h")
    doc.text(75, 215, "Use a low-current distribution rail. Servo red comes from HW-140 5 V DIRECTLY, not this sensor rail.", "s")
    doc.table(280, ("DEVICE", "SIGNAL PIN", "UNO PIN / RESULT"), SENSORS, (500, 400, 800))
    doc.box(50, 700, 1700, 380)
    for yy, txt in [
        (745, "Gate orange/yellow → D9; gate red → HW-140 5 V; gate brown → power ground."),
        (790, "Normally-open start button → D3 and GND; no external resistor (INPUT_PULLUP)."),
        (835, "Color LED control left unconnected pending module confirmation; color-reading code is not implemented."),
        (880, "Bluetooth and unidentified square converter: leave disconnected until their ratings/pins are identified."),
        (925, "D0/D1 reserved for Serial/upload. A4/A5 occupied: do not also connect I2C peripherals there."),
        (970, "sensor_test: battery motor/gate branch OFF; computer USB powers Uno/sensors. IR enabled by default."),
        (1015, "Ultrasonic NA = no valid echo, not a clear path. Color and ultrasonic hardware remain unverified."),
        (1060, "robot_bench uses motor/gate timing only. Connected sensors do not provide navigation or obstacle avoidance."),
    ]:
        doc.text(75, yy, txt, "s")
    doc.text(50, 1140, "Gate motion and wheel PWM are independent. Servo Timer1 does not affect wheel PWM on D5/D6.", "s")
    doc.text(50, 1180, "Read CONNECTIONS.md and POWER.md before assembling. Both driver profiles preserve these sensor assignments.", "s")
    doc.save(ELECTRONICS / "sensor-wiring.svg")


def wire_list():
    with (ELECTRONICS / "connections.csv").open("w", newline="", encoding="utf-8-sig") as out:
        writer = csv.writer(out)
        writer.writerow(("profile", "from", "to", "condition"))
        for row in POWER:
            writer.writerow(("both", *row))
        for name, rows in PROFILES.items():
            for pin, target, note in rows:
                writer.writerow((name, "Uno " + pin, name.upper() + " " + target, note))
            outputs = ("AOUT1", "AOUT2", "BOUT1", "BOUT2") if name == "drv8833" else ("OUT1", "OUT2", "OUT3", "OUT4")
            for source, target in zip(outputs, ("Left motor lead 1", "Left motor lead 2", "Right motor lead 1", "Right motor lead 2")):
                writer.writerow((name, source, target, "No direct ground connection"))
            writer.writerow((name, "MOTOR+", "VIN/VM" if name == "drv8833" else "VM/+12V", "Regulated; <=motor rating"))
            writer.writerow((name, "Power GND", "Driver GND", "Direct power return"))
        writer.writerow(("l298n", "Uno 5V", "Driver 5V", "5V-EN removed"))
        for enable in ("ENA", "ENB"):
            writer.writerow(("l298n", enable, "GND via 10k", "Enable caps removed; reset pull-down"))
        writer.writerow(("both", "Uno D9", "Servo orange/yellow", "Signal only"))
        writer.writerow(("both", "Uno D3", "Start button contact 1", "INPUT_PULLUP"))
        writer.writerow(("both", "GND", "Start button contact 2", "Normally open"))
        for device in ("Left ultrasonic", "Right ultrasonic", "IR line sensor", "Color sensor"):
            writer.writerow(("both", "Uno 5V", device + " VCC", "Low-current logic power"))
            writer.writerow(("both", "GND", device + " GND", "Common reference"))
        for device, pins, destinations in SENSORS:
            for source, destination in zip(pins.split(" / "), destinations.split(";", 1)[0].split(" / ")):
                if source in ("S", "DO"):
                    continue
                writer.writerow(("both", device + " " + source, "Uno " + destination, "Optional; see sensor status"))
        writer.writerow(("both", "Color LED", "Unconnected", "Control not verified"))


def verify():
    source = (ELECTRONICS / "firmware" / "RobotHardware.h").read_bytes()
    for name in SKETCHES:
        assert (ELECTRONICS / name / "RobotHardware.h").read_bytes() == source, name
        text = (ELECTRONICS / name / (name + ".ino")).read_text(encoding="utf-8-sig")
        assert '#include "RobotHardware.h"' in text
        assert "#define ROBOT_DRIVER_DRV8833 0" in text, "Tracked default must match owned L298N"
    # All used signal pins are unique within each driver profile, including
    # sensor, gate and start functions. D0/D1 are deliberately unallocated.
    common = ["D3", "D9", "D10", "D11", "D12", "D13", "A0", "A1", "A2", "A3", "A4", "A5"]
    for profile, rows in PROFILES.items():
        used = common + [row[0] for row in rows]
        assert len(set(used)) == len(used), f"Pin conflict in {profile}"
    # Check canonical pin constants match the wiring tables, not just copies.
    header = source.decode()
    expected = {"LEFT_PWM_PIN": "5", "LEFT_DIR_PIN": "2", "LEFT_OTHER_PIN": "4", "RIGHT_PWM_PIN": "6", "RIGHT_DIR_PIN": "7", "RIGHT_OTHER_PIN": "8", "SLEEP_PIN": "8", "SERVO_PIN": "9", "START_PIN": "3", "LEFT_TRIG": "A0", "LEFT_ECHO": "A1", "RIGHT_TRIG": "A2", "RIGHT_ECHO": "A3", "IR_PIN": "A4", "COLOR_OUT_PIN": "A5"}
    for name, value in expected.items():
        assert re.search(r"\b" + name + r"\s*=\s*" + value + r"\b", header), name
    # Local links in electronics/root Markdown should not point to absent files.
    for path in [ROOT / "README.md", *ELECTRONICS.rglob("*.md")]:
        for target in re.findall(r"\]\(([^)]+)\)", path.read_text(encoding="utf-8-sig")):
            if "://" not in target and not target.startswith("#"):
                assert (path.parent / target.split("#")[0]).exists(), (path, target)
    print("Verified: standalone headers, driver defaults, pin allocations and local document links.")


def render():
    browser = Path(r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe")
    if not browser.exists():
        raise SystemExit("Edge not found; render the SVGs with an SVG-capable browser.")
    out_dir = ROOT / "build" / "rendered"
    out_dir.mkdir(parents=True, exist_ok=True)
    for name, height in (("wiring", 2190), ("wiring-l298n", 2190), ("sensor-wiring", 1210)):
        target = out_dir / (name + "-" + str(time.time_ns()) + ".png")
        profile = ROOT / "build" / ("edge-" + name + "-" + str(time.time_ns()))
        result = subprocess.run([str(browser), "--headless", "--disable-gpu", "--hide-scrollbars", "--force-device-scale-factor=1", "--no-first-run", "--user-data-dir=" + str(profile), "--screenshot=" + str(target), "--window-size=1800," + str(height), (ELECTRONICS / (name + ".svg")).as_uri()], capture_output=True, timeout=45, creationflags=0x08000000)
        if result.returncode:
            raise RuntimeError(result.stderr.decode(errors="replace")[-1000:])
        deadline = time.monotonic() + 20
        while not target.exists() and time.monotonic() < deadline:
            time.sleep(0.2)
        data = target.read_bytes()
        assert data[:8] == b"\x89PNG\r\n\x1a\n"
        assert struct.unpack(">II", data[16:24]) == (1800, height)
        shutil.copyfile(target, ELECTRONICS / (name + ".png"))
        print("Rendered", name + ".png")


def package():
    destination = ELECTRONICS / "soccer-electronics.zip"
    files = [ROOT / "README.md", *ELECTRONICS.rglob("*"), ROOT / "tools" / "prepare_electronics.py", *(ROOT / "tests").rglob("*"), *(ROOT / "references").glob("*")]
    with zipfile.ZipFile(destination, "w", zipfile.ZIP_DEFLATED) as archive:
        for path in sorted(set(files)):
            if path.is_file() and path != destination:
                archive.write(path, path.relative_to(ROOT))
    with zipfile.ZipFile(destination) as archive:
        assert archive.testzip() is None, "Archive CRC failure"
        for name in SKETCHES:
            relative = f"electronics/{name}/{name}.ino"
            assert archive.read(relative) == (ROOT / relative).read_bytes()
            relative = f"electronics/{name}/RobotHardware.h"
            assert archive.read(relative) == (ROOT / relative).read_bytes()
        for name in ("wiring.png", "wiring-l298n.png", "sensor-wiring.png", "connections.csv", "POWER.md"):
            assert archive.read("electronics/" + name) == (ELECTRONICS / name).read_bytes()
    print("Packaged", destination.name)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--render", action="store_true")
    parser.add_argument("--package", action="store_true")
    parser.add_argument("--check", action="store_true", help="Verify generated headers and local references only")
    args = parser.parse_args()
    if args.check:
        verify()
        return
    source = ELECTRONICS / "firmware" / "RobotHardware.h"
    for name in SKETCHES:
        shutil.copyfile(source, ELECTRONICS / name / source.name)
    power_diagram("drv8833")
    power_diagram("l298n")
    sensor_diagram()
    wire_list()
    if args.render:
        render()
    verify()
    if args.package:
        package()


if __name__ == "__main__":
    main()
