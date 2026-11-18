#!/usr/bin/env bash
# Build the Aura sketch and export a single merged image for ESP Web Tools (flash at offset 0).
# Requires: arduino-cli, esp32 core, and the libraries in README.md (with the repo's
# TFT_eSPI/User_Setup.h and lvgl/src/lv_conf.h already copied in, per FLASHING.md).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$ROOT/installer/bin/aura-firmware.bin"
FQBN="esp32:esp32:esp32:PartitionScheme=huge_app"

# arduino-cli wants the sketch folder name to match the .ino file name.
STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT
mkdir "$STAGE/weather"
cp "$ROOT"/aura/* "$STAGE/weather/"

arduino-cli compile --fqbn "$FQBN" --export-binaries --output-dir "$STAGE/out" "$STAGE/weather"

cp "$STAGE/out/weather.ino.merged.bin" "$OUT"
ls -l "$OUT"
shasum -a 256 "$OUT"
