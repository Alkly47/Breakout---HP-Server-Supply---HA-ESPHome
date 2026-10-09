# Firmware Logic & Sensor Handling

Supplemental documentation for the internals of the ESP firmware — how data
flows from the PSU over I²C to Home Assistant, how errors are tracked, and the
per-sensor range guards. Companion to `BUILDING.md` (compile/flash workflow)
and `README.md` (feature overview).

## Architecture at a glance

- **MCU:** ESP8266 (D1 Mini), single core — the WiFi stack, I²C polling, OLED
  rendering, and the HA API all time-share one CPU. (Erratic ping RTT is normal
  for this class of device; sensor delivery is what matters.)
- **Time:** no RTC. In-memory wall clock, synced from Home Assistant every
  15 min (`time: homeassistant`). Energy integration itself uses the monotonic
  ms counter, not the wall clock.
- **Buses:**
  - PMBus PSU telemetry @ `0x5F`, PSU ROM @ `0x57` — I²C **50 kHz** (deliberate
    cap; >100 kHz causes corrupt data, faster lockups the bus and crashes the ESP)
  - ADS1115 ADC @ `0x48` (4 channels: 12V main current shunt, 12V main, 12V STBY, 5V STBY)
  - SSD1306 OLED @ `0x3C` (6 rotating pages, 5 s page interval)

## PMBus read path (every telemetry sensor)

Each PMBus sensor is a template sensor with its own 1 s poll. One read:

```
1. write  [regAddr, regChecksum]  to PSU          (regChecksum = two's-complement of regAddr + devAddr<<1)
2. read   3 bytes:  dataLo, dataHi, dataChecksum
3. validate transfer  ->  on failure: log, error++, return NAN
4. validate data checksum: (dataLo + dataHi + dataChecksum) mod 256 == 0
                     ->  on mismatch: log, error++, return NAN
5. range guard: raw 16-bit value < sensor's physical max
                     ->  on exceed: log, error++, return NAN
6. scale and publish (see register map)
```

A NAN publish takes the entity **unavailable in HA** — a gap you can diagnose,
never a wrong number.

### Register map

| Reg | Quantity | Scale | Notes |
|-----|----------|-------|-------|
| 0x08 | VIN | /32 V | AC input |
| 0x0A | IIN | /64 A | |
| 0x0C | PIN | W | |
| 0x0E | VOUT | /254.5 V | 12V rail (255 default, tuned to multimeter) |
| 0x10 | IOUT | /64 A | |
| 0x12 | POUT | W | |
| 0x1A | TEMP1 | /64 °C | intake air |
| 0x1C | TEMP2 | /64 °C | internal PSU |
| 0x1E | FAN_SPEED | rpm | |
| 0x36 | PEAK IOUT | /64 A | vendor register, resets on output cycle |

ROM @ `0x57` (part number, date code, manufacturer, PSU name, option kit, CT
barcode) is read via plain `read_bytes` on a 24 h interval — no checksummed
word protocol, static data only.

## Error tracking & recovery

Per-sensor state lives in two globals:

- `dev_fail[10]` — bool, sensor permanently disabled (stops polling)
- `chksum_err[10]` — uint8, accrued error count (I²C failures + checksum mismatches + range-guard rejections)

Rules:

- Any failure at steps 3–5 increments the sensor's count
- A successful read decrements the count (when `allow_error_recovery: true`)
  — intermittent bus noise can't ratchet a sensor into failure over time
- Count exceeds `fail_limit` (10) → sensor marked failed, polling stops
- **Slot 7 is shared** by `power_in` and the three rolling-average sensors
  (all read register 0x0C — same register, same failure mode)

Log lines to grep (tag `HP_PSU`):

```
I2C communication failed!          step 3
Mismatch checksum. addr: 0x..      step 4
Implausible raw value 0x....       step 5
Repeat Errors: fail_limit reached  step 3–5 escalation
```

## Sensor range handling (guards)

### Why guards exist

The data checksum is a sum-mod-256 over 3 bytes. A corrupted I²C read that
*preserves the byte sum* passes it with probability 1/256 and would publish a
physically impossible value. Historical incident (2024 firmware): a truncated
read produced an all-ones low word → `0xFFFF/64 = 1023.984375 A` on the input
current sensor. That firmware also **returned values on checksum failure**
(log-only); current firmware rejects, and the guards close the residual 1/256
hole.

### Design

Each sensor has a post-checksum cap at a value the PSU **cannot physically
report** (generous margin, far below the 16-bit max of 65535):

| Sensor | Reg | Cap (raw) | ≈ Physical | Rationale |
|--------|-----|-----------|------------|-----------|
| Current In | 0x0A | 2048 | 32 A | worst-case input current ×2 |
| Peak Amps Out | 0x36 | 16000 | 250 A | 1.6 kW @ 12V ≈ 133 A |
| Current Out | 0x10 | 16000 | 250 A | as above |
| Fan Speed | 0x1E | 30000 | rpm | PSU fans top out ~18–20k |
| Intake / PSU Temp | 0x1A/0x1C | 12000 | 187 °C | thermal shutdown ~105–110 °C |
| Power In / Out | 0x0C/0x12 | 5000 | W | largest server PSU ≈ 1.6–2 kW |
| 3× Power In Avg | 0x0C | 5000 | W | same register as Power In |
| Voltage Out | 0x0E | 8000 | 31 V | 12V rail |
| Voltage In | 0x08 | 12800 | 400 V | AC input max 264 V |

Because caps sit well below both the physical limit *and* the 16-bit max, a
genuine reading can never trip one: **cap hit = corruption, not measurement.**

### Why not 3-of-2 voting / range-edge discounting

The usual multi-sensor discount rules (vote 2-of-3, discard a channel that
sits within ~10% of its sensing range) require redundant measurement paths per
quantity. This design has exactly one path per quantity (one register, no
duplicate sensor), so there is nothing to vote on and no known true range to
apply an edge rule against. The physical-max guard is the single-path
equivalent: it converts the failure mode from "impossible value published" to
"gap + logged error," which is the best available with one register.

### Known limitation: temperature sign

PMBus temperature registers are signed (SWORD), but the read path masks them
unsigned. A genuine *negative* temperature wraps to a huge raw value and is
rejected by the cap (sensor unavailable) rather than displaying ~1000 °C.
Irrelevant in a server room; fix with an `int16_t` cast if cold-soak testing.

## Power & energy chain

- **W:** raw register value, 1:1 (PMBus WORD registers at 1 W resolution)
- **kWh:** `total_daily_energy` integrates the *published* power state on-device:
  - method `right` (default): `ΔE_Wh = elapsed_h × P_new` per power publish
  - power publishes are gated by `or: [delta 0.5 W, throttle 30 s]` → steady-state
    steps ≈ 30 s; full elapsed time is always accounted (no lost seconds)
  - `× 0.001` filter → kWh; midnight reset (HA time, DST-aware); counter
    persisted to flash across reboots (`restore` defaults true in 2026.x)
- **Rolling averages (01/05/15 min):** read PIN directly at 1 s (bypassing the
  power filter so the window is true 1 s samples) → `sliding_window_moving_average`
  (60/300/900 samples) → 1 s output, delta/throttle gate to HA. Partial-window
  output for the first window period after boot.
- **Loss points:** `PSU Loss` = PIN − POUT (W); `PSU Loss Percent` (guarded
  divide); `Total Daily Energy Loss` = daily input − output (kWh).

## HA behavior cheat sheet

| Condition | What you see |
|-----------|--------------|
| Transient I²C/checksum/range error | entity briefly unavailable; log line; count accrues |
| `fail_limit` reached | entity unavailable, polling stops; `dev_fail` set |
| PSU output cycled | peak amps resets; (MIN_MAX build) min/max records reset |
| Reboot | wall clock invalid until HA sync; energy counter restored from flash |
| Offline > 15 min | clock drifts (crystal-based, small); re-syncs on reconnect; midnight reset has day-change catch-up |
