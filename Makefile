# Aura firmware: build and flash from the command line.
#   make build              compile and write installer/bin/aura-firmware.bin (~3 min)
#   make flash              flash that image to a connected board (auto-detects the port)
#   make flash PORT=/dev/cu.usbserial-0001 BAUD=115200
#   make flash IMAGE=other.bin
# `flash` uses esptool: an installed one if found, otherwise `uvx esptool` (needs uv).
# Install options: uv tool install esptool   (or: make install-esptool)

IMAGE ?= installer/bin/aura-firmware.bin
BAUD  ?= 460800
PORT  ?= $(firstword $(wildcard /dev/cu.usbserial-* /dev/cu.wchusbserial* /dev/cu.SLAB_USBtoUART* /dev/ttyUSB* /dev/ttyACM*))

ESPTOOL := $(shell command -v esptool 2>/dev/null || command -v esptool.py 2>/dev/null)
ifeq ($(ESPTOOL),)
ifneq ($(shell command -v uvx 2>/dev/null),)
ESPTOOL := uvx esptool
endif
endif
NO_ESPTOOL := esptool not found. Install uv (brew install uv), then: make install-esptool

.PHONY: build flash port erase install-esptool

build:
	./scripts/build-firmware.sh

install-esptool:
	uv tool install esptool

port:
	@test -n "$(PORT)" || { echo "No serial port found. Plug in the board (data cable) or pass PORT=/dev/..."; exit 1; }
	@echo $(PORT)

flash:
	@test -f "$(IMAGE)" || { echo "$(IMAGE) not found. Run 'make build' or pass IMAGE=..."; exit 1; }
	@test -n "$(ESPTOOL)" || { echo "$(NO_ESPTOOL)"; exit 1; }
	@test -n "$(PORT)" || { echo "No serial port found. Plug in the board (data cable) or pass PORT=/dev/..."; exit 1; }
	@echo "Flashing $(IMAGE) to $(PORT) at $(BAUD) baud"
	$(ESPTOOL) --chip esp32 --port $(PORT) --baud $(BAUD) write_flash 0x0 $(IMAGE)

erase:
	@test -n "$(ESPTOOL)" || { echo "$(NO_ESPTOOL)"; exit 1; }
	@test -n "$(PORT)" || { echo "No serial port found."; exit 1; }
	$(ESPTOOL) --chip esp32 --port $(PORT) erase_flash
