# Flashing Aura to a new device

Target: ESP32-2432S028R ("CYD", 2.8" ILI9341). Built and flashed with Arduino IDE; there is no CLI build.

## 1. One-time computer setup (skip if already done)

1. Install Arduino IDE 2.x.
2. Add ESP32 board support: Settings -> Additional boards manager URLs ->
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`, then Boards Manager -> install **esp32** (Espressif).
3. Library Manager -> install these exact versions:
   - ArduinoJson 7.4.1
   - HttpClient 2.2.0
   - TFT_eSPI 2.5.43
   - WiFiManager 2.0.17
   - XPT2046_Touchscreen 1.4
   - lvgl 9.2.2
4. Copy the repo's config files over the library defaults (**required**, or the screen/UI won't work):
   ```sh
   cd <path-to-Aura-repo>
   cp TFT_eSPI/User_Setup.h ~/Documents/Arduino/libraries/TFT_eSPI/User_Setup.h
   cp lvgl/src/lv_conf.h   ~/Documents/Arduino/libraries/lvgl/src/lv_conf.h
   ```
5. Copy the sketch folder:
   ```sh
   cp -R aura ~/Documents/Arduino/aura
   ```
   (Re-run after any `git pull`, or the IDE compiles stale code.)

## 2. Flash the device

1. Plug the CYD in over USB (use a data cable, not charge-only).
2. Open `~/Documents/Arduino/aura/weather.ino`.
3. Tools menu:
   - Board: **ESP32 Dev Module**
   - Partition Scheme: **Huge App (3MB No OTA/1MB SPIFFS)**
   - Port: the `/dev/cu.usbserial-*` (or `wchusbserial*`) entry
   - Everything else default
4. Click **Verify** (2-3 min first time), then **Upload** (1-2 min). Don't cancel mid-upload.
5. Optional: Serial Monitor at 115200 baud for debug output.

## 3. First boot on the new device

1. Grateful Dead splash shows for ~2 s.
2. Device starts a Wi-Fi AP named **Aura**. Join it from your phone/laptop.
3. Browse to `http://192.168.4.1`, enter your Wi-Fi credentials.
4. Device reboots, joins Wi-Fi, shows weather. Touch the screen for settings (location, brightness, language, units, 12/24h).

Wi-Fi credentials are stored on the device, so each new board needs step 3 once.

## Troubleshooting

| Symptom | Fix |
|---|---|
| No serial port appears | Install the CH340 (or CP210x) USB driver; try another cable/port |
| Upload hangs at "Connecting..." | Hold the **BOOT** button on the board until upload starts; or set Tools -> Upload Speed to 115200 |
| Blank/white or garbled screen | `TFT_eSPI/User_Setup.h` wasn't copied into the library folder (step 1.4) |
| Compile error about lvgl config | `lv_conf.h` wasn't copied (step 1.4) |
| Sketch too large | Partition Scheme isn't "Huge App" |
| Touch is mirrored/off | Some CYD variants differ; check the `XPT2046` setup in `weather.ino` |
| Want to re-enter Wi-Fi | Use the Wi-Fi reset in the device's settings screen; it returns to the "Aura" portal |

## Command line (no Arduino IDE)

```sh
brew install uv           # once, if you don't have uv (or: curl -LsSf https://astral.sh/uv/install.sh | sh)
make install-esptool       # uv tool install esptool (optional: `make flash` falls back to `uvx esptool`)
make flash                 # flashes installer/bin/aura-firmware.bin, port auto-detected
make flash PORT=/dev/cu.usbserial-0001 BAUD=115200   # override port / speed
make flash IMAGE=path/to/other.bin                   # flash a different image
make build                 # recompile the sketch first (needs arduino-cli + setup in section 1)
make erase                 # wipe flash, including saved Wi-Fi credentials
```

If it hangs at "Connecting...", hold BOOT, tap RST, release BOOT, or use `BAUD=115200`.
`write_flash` is spelled `write-flash` in newer esptool; both work.

## Browser flash (no Arduino IDE)

The `installer/` folder is a one-click web flasher built on [ESP Web Tools](https://esphome.github.io/esp-web-tools/) (Chrome/Edge only).

Rebuild the firmware after changing the sketch (takes ~3 min; needs the setup in section 1):

```sh
./scripts/build-firmware.sh   # writes installer/bin/aura-firmware.bin (merged image, flashed at offset 0)
git add installer && git commit -m "Rebuild firmware" && git push
```

Pushing changes under `installer/` triggers `.github/workflows/pages.yml`, which publishes to GitHub Pages
(one-time: repo Settings -> Pages -> Source: **GitHub Actions**).
