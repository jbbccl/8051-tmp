# Makefile for 8051 with SDCC

TARGET = main
SRC_DIR = src
BUILD_DIR = build
CFLAGS = --model-small --opt-code-speed -Iinc
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

.PHONY: clean flash