# Makefile for 8051 with SDCC

TARGET = main
SRC_DIR = 8051/src
BUILD_DIR = 8051/build
CFLAGS = --model-small --opt-code-speed -I8051/inc
SRCS = $(shell find $(SRC_DIR) -name '*.c')
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
RELS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.rel)

$(BUILD_DIR)/%.rel: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	sdcc $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(TARGET).ihx: $(RELS)
	sdcc $(CFLAGS) -o $@ $(RELS)

flash: $(BUILD_DIR)/$(TARGET).ihx
	.venv/bin/stcgal -P stc89 -p /dev/ttyUSB0 $(BUILD_DIR)/$(TARGET).ihx

clean:
	rm -rf $(BUILD_DIR)

# ── ESP32 ──────────────────────────────
esp:
	cd esp32 && platformio run

esp-upload:
	cd esp32 && platformio run -t upload

esp-monitor:
	cd esp32 && platformio device monitor

esp-clean:
	cd esp32 && platformio run -t clean

esp-db:
	cd esp32 && platformio run -t compiledb
	cd esp32 && python3 -c "\
import json;\
f='compile_commands.json';\
bad={'-fno-rtti','-mfix-esp32-psram-cache-issue','-mlongcalls','-fstrict-volatile-bitfields','-fno-tree-switch-conversion'};\
d=json.load(open(f));\
[e.update({'command':' '.join(p[:1]+['-w']+p[1:])})for e in d if (p:=[x for x in e['command'].split()if x not in bad])];\
json.dump(d,open(f,'w'),indent=2)"

.PHONY: clean flash esp esp-upload esp-monitor esp-clean esp-db web web-build web-pack web-dev web-install web-all

# ── Web UI (Vite) ──────────────────────
web-install:
	cd web && pnpm install

web-dev:
	cd web && pnpm dev

web-build:
	cd web && pnpm build

web-pack:
	python3 web/convert.py

web: web-build web-pack

web-all: web-install web
