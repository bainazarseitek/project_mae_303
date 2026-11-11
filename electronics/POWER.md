# Power, maximum voltage and wheel speed

## Decision

Keep the owned HW-140 at **5.0 V** for gate/logic. Add a separate **motor buck regulator**. For higher speed at rated voltage prefer a **DRV8833 carrier** to the lossy L298N. Confirm the original motor specifications or fit a documented matching pair. This is a proposed architecture, not a completed hardware test.

| Location | Setting / limit |
|---|---|
| LiPo battery | 11.1 V nominal, 2450 mAh, 30C; measured 11.4 V; allow for 12.6 V fully charged |
| Servo/USB rail | 5.0 V target; previously displayed 5.01 V. Exact servo maximum unknown; do not raise it for wheel speed |
| Original yellow motors | Maximum cannot be established from casing/photos. No claim that 6, 7.4 or 11.1 V is safe |
| Documented 3-6 V replacement motors | Motor regulator <=6.0 V; 6.0 V is the example model's upper rated supply |
| L298N VM with confirmed 6 V motors | <=6.0 V in this conservative plan; less reaches the motors. A higher rail is not authorized by a fixed-drop assumption |
| DRV8833 motor input | Same motor-rated rail; 6.0 V only for known 6 V motors. Carrier range 2.7-10.8 V; never raw 3S LiPo |
| HW-140 module capability | Supplier lists 5.5-30 V input, 0.5-30 V output, approximately 3 A continuous. Cooling/variant matter. 30 V is not a robot setting |
| Driver logic / sensors / Uno USB | 5 V. L298N external logic supply requires 5V-EN removed |

TowerPro specifies 4.8 V operation for genuine SG90. It does not establish a 6 V maximum for the photographed clone. Retain the reported approximately 5 V bench setting and check the actual servo before changing it. [TowerPro](https://towerpro.com.tw/product/sg90-analog/)

## Is 5 V enough?

5 V is the intended gate/logic rail. It must also supply sufficient current without sagging; an unloaded display reading proves neither current capacity nor transient stability.

For wheels, L298N loses appreciable voltage. ST specifies **1.8 V typical total bridge drop at 1 A**, up to 3.2 V at that condition. At that illustrative operating point, 5 V into VM leaves about 3.2 V across the motor while enabled. Actual drop varies with current and temperature. Do not assume a fixed 2 V loss and raise the rail to 8 V for a 6 V motor. [ST L298 datasheet](https://www.st.com/resource/en/datasheet/l298.pdf)

The previous PWM 140/255 is about 55% duty. Full PWM 255 removes off-time, not driver loss. PWM is not a safe substitute for regulating an unknown motor's supply: the motor still receives high-voltage pulses.

TI specifies about 0.36 ohm combined bridge on-resistance for DRV8833 at its stated conditions. At 1 A this suggests about 0.36 V drop, much less than the L298 example. Carrier, temperature and wiring still matter. [TI DRV8833 datasheet](https://www.ti.com/lit/ds/symlink/drv8833.pdf)

## Maximize usable speed

1. Identify original motors or fit a documented pair. Adafruit's TT example is rated 3-6 V; that example does not identify your motors. [Adafruit #3777](https://www.adafruit.com/product/3777)
2. Set motor regulator to the verified rating or lower, retaining 5 V for the gate.
3. Use low-loss driver, sufficient regulator current and short power wiring; fix drag/unreliable starts.
4. Increase PWM toward 255 after raised-wheel checks, then measure loaded field speed and straightness. Trim left/right PWM and retime turns.
5. If rated voltage/full duty is insufficient, change motor/gear ratio. Do not guess an overvoltage.

## Current and power budget

The example TT motor is reported at **1.5 A stall at 6 V**, about 3 A for a pair before margin. Startup can approach stall briefly. A reputable >=4 A continuous motor regulator is a provisional selection target for that pair; actual motors, thermal conditions and transients decide suitability. Do not deliberately hold a motor stalled.

The specified Pololu DRV8833 carrier is **1.2 A continuous / 2 A peak per channel**. Verify actual loaded currents stay below continuous rating and startups fit peak duration and thermal limits; otherwise choose a higher-current driver. Chip ratings do not establish the capacity of an arbitrary breakout. [Pololu specification](https://www.pololu.com/product/2130/specs)

HW-140 is assigned to gate/logic so wheel starts do not consume its capacity. Add actual servo peak, Uno and sensor currents, and validate under gate motion. Roughly 2 A capacity allowance for a small servo plus logic margin is a starting design allowance, not measured demand. Select CC from actual load/wiring tests; do not turn it fully up by default.

The label calculation 30C x 2.45Ah = 73.5 A is theoretical battery discharge capability, not a verified circuit rating. Jumpers, regulators, switches, driver and motors have much lower individual limits. Battery capacity does not increase motor voltage tolerance. Approximate regulator input power is output power divided by efficiency.

## Grounding, noise and battery management

Use star power distribution: each regulator input/return, driver return, servo return and Uno reference ground reaches a proper power terminal. Never route motor/servo return current through Uno or breadboard. Common ground provides a signal reference; it does not connect positive rails.

The ground schedule assumes regulators that permit common input/output negatives. Confirm that on the actual modules before assembly. Some current-limited converters put a measuring shunt in the negative path; bypassing it with another ground path can defeat current measurement/limiting. This is a general circuit concern, not a confirmed finding about this HW-140 variant. Verify CC using a known load after final grounding; if its manual prohibits common negatives, use a documented common-ground 5 V regulator instead. Do not depend on unverified CC for fault protection. [Current-shunt grounding reference](https://knowledge.ni.com/KnowledgeArticleDetails?id=kA03q000000YI4xCAG&l=en-US%2F1000)

Fit 100 nF ceramics across each motor. Provisional bulk starting points are 470-1000 uF near driver VM (16 V or greater rating at the <=6 V regulated rail), and 470 uF near the servo's 5 V connector. Follow selected carrier/regulator capacitor and stability requirements; verify with load tests. Observe electrolytic polarity. Capacitors cannot repair an undersized supply.

Use a fuse close to pack positive, accessible DC switch and rated wiring/connectors. Exact fuse/current-limit settings require load and wire data. L298N fallback needs 10k pull-downs on ENA/ENB to keep reset/floating inputs off; remove enable caps. The specified DRV8833 carrier has input/sleep biasing; verify substitutes.

Use a 3S LiPo balance charger and pack-appropriate per-cell monitoring/cutoff. HW-140 undervoltage shutdown is not per-cell battery protection. No battery monitoring or regulator control is implemented in firmware. Do not charge through this robot circuit.

## Before accepting the build

Record motor rating, motor-regulator model, both rail voltages unloaded and under simultaneous starts, loaded current, driver temperature, absence of resets/jitter, gate clearance/retention, chosen PWM, and measured route timings. A complete schematic cannot supply missing measurements.

Additional sources: [HW-140 technical sheet](https://m.media-amazon.com/images/I/81GjzpF4AmL.pdf), [manufacturer listing](https://www.szhwmake.com/prod_view.aspx?FId=t3%3A88%3A3&Id=202&TypeId=88), [Arduino USB power](https://support.arduino.cc/hc/en-us/articles/360018922259-What-power-supply-can-I-use-with-my-Arduino-board), [L298N jumper example](https://handsontec.com/dataspecs/L298N%20Motor%20Driver.pdf). Match actual board variants before applying specifications.
