# Building the firmware

This repo's firmware is plain ESPHome YAML — no C++ to manage. Everything is
driven by `build.sh`, which wraps the pinned ESPHome CLI in a local `.venv`.

## Toolchain (what's installed here)

| Component | Version | Where |
|---|---|---|
| [uv](https://docs.astral.sh/uv/) | 0.12.x | `~/.local/bin/uv` (single static binary, bootstraps its own Python) |
| CPython | 3.12.14 | uv-managed standalone build |
| ESPHome | 2026.8.0 (pinned in `requirements.txt`) | `.venv/` |
| xtensa toolchain | auto-downloaded by ESPHome on first compile | `.venv/.../esphome/frameworks/` |

Why `uv` instead of system Python / DinD:
- The container ships with no Python at all; `uv` installs a standalone CPython with no apt/root.
- The official `esphome/esphome` Docker image is **not** a pre-baked compiler image —
  the xtensa toolchain is still downloaded on first build. Docker-in-Docker would
  only add a privileged-daemon dependency for no speed benefit.
- If you do prefer the official image (e.g. to pin the exact runtime across machines):
  `docker run -it --rm -v "$PWD":/config esphome/esphome compile hp-server-psu.yaml`

## Setup (fresh machine/container)

```bash
curl -LsSf https://astral.sh/uv/install.sh | sh     # if uv isn't present
cp secrets.yaml.example secrets.yaml                 # then edit with real WiFi creds
./build.sh setup                                     # creates .venv, installs pinned ESPHome
```

> `secrets.yaml` is gitignored and **required** by `hp-server-psu.yaml`
> (`!secret wifi_ssid` / `!secret wifi_password`).

## Day-to-day commands

```bash
./build.sh config  [yaml]     # validate config only (fast)
./build.sh compile [yaml]     # build -> .esphome/build/<name>/.pioenvs/<name>/firmware.bin
./build.sh run     [yaml]     # build + flash (OTA if device is on the network, else serial)
./build.sh publish [yaml]     # compile + copy new .bin into firmware/
./build.sh clean   [yaml]     # remove build artifacts
```

Defaults to `hp-server-psu.yaml`. For the min/max variant pass
`hp-server-psu-MIN_MAX.yaml`.

## Flashing

- **OTA (preferred):** `./build.sh run` — device must be powered and on the network.
- **USB serial:** D1 Mini — pull GPIO15 LOW (flash mode), then `./build.sh run`
  over the serial port.
- **Web flasher:** upload `firmware/<name>.bin` to https://web.esphome.io/
  (fresh builds boot into the `HP Server PSU` / `123456789` captive-portal hotspot).

Note: `hp-server-psu-MIN_MAX.yaml` intentionally has no `ssid` — it compiles to an
**AP-only** image (hotspot only) meant to be configured post-flash via the
captive portal / improv serial.

## Gotchas

- First compile downloads the ESP8266 toolchain (~100 MB, a few minutes).
- Fonts are Google Fonts (`gfonts`) and are **fetched at compile time** — the build
  machine needs internet access.
- `i2c: frequency` is capped at 50kHz on purpose (see comment in the YAML) — don't raise it.
- `ota:` has no password; anyone on the LAN can reflash the device. Consider adding one.
