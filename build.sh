#!/usr/bin/env bash
# Build helper for the HP Server PSU ESPHome project.
#
# Usage:
#   ./build.sh setup                 (re)create .venv and install pinned deps (needs `uv`)
#   ./build.sh config  [yaml]       validate config only (default: hp-server-psu.yaml)
#   ./build.sh compile [yaml]       compile firmware -> .esphome/build/<name>/<name>.bin
#   ./build.sh run     [yaml]        compile + flash (OTA if device is on network, else serial)
#   ./build.sh publish [yaml]       compile + copy new .bin into firmware/
#   ./build.sh clean   [yaml]       remove build artifacts
#
# Examples:
#   ./build.sh compile hp-server-psu-MIN_MAX.yaml
#   ./build.sh publish
set -euo pipefail
cd "$(dirname "$0")"

YAML="${2:-hp-server-psu.yaml}"
CMD="${1:-config}"
ESPHOME="$PWD/.venv/bin/esphome"

if [ "$CMD" != "setup" ] && [ ! -x "$ESPHOME" ]; then
  echo "error: .venv missing — run: $0 setup" >&2
  exit 1
fi

case "$CMD" in
  setup)
    command -v uv >/dev/null || { echo "error: uv not found (install: curl -LsSf https://astral.sh/uv/install.sh | sh)" >&2; exit 1; }
    uv python install 3.12
    uv venv .venv --python 3.12
    uv pip install --python .venv/bin/python -r requirements.txt
    echo "setup complete: $ESPHOME"
    ;;
  config|compile|run|clean)
    "$ESPHOME" "$CMD" "$YAML"
    ;;
  publish)
    "$ESPHOME" compile "$YAML"
    NAME=$("$ESPHOME" config "$YAML" 2>/dev/null | grep -m1 -E '^[[:space:]]+name:' | awk '{print $2}')
    [ -n "$NAME" ] || { echo "error: could not determine device name from $YAML" >&2; exit 1; }
    SRC=".esphome/build/$NAME/.pioenvs/$NAME/firmware.bin"
    [ -f "$SRC" ] || { echo "error: $SRC not found" >&2; exit 1; }
    cp "$SRC" "firmware/$NAME.bin"
    echo "published: firmware/$NAME.bin"
    ;;
  *)
    echo "unknown command: $CMD (see header of this script)" >&2
    exit 1
    ;;
esac
