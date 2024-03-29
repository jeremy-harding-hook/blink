TARGET_ELF := blink.elf
TARGET_BIN := blink.bin

BUILD_DIR_BASE = ./build
SRC_DIRS := ./src
EXTRA_LIB_DIRS := /usr/arm-none-eabi/include/ /usr/arm-none-eabi/lib/
CC = /usr/bin/arm-none-eabi-gcc
OBJCOPY = /usr/bin/arm-none-eabi-objcopy

ifndef mode
	mode = release
endif

MAKEFLAGS += "j $(nproc)"

COREFLAGS = -mcpu=cortex-m4 -DSTM32F4 -lopencm3_stm32f4 -mfloat-abi=hard \
		-fomit-frame-pointer -fno-exceptions -fno-asynchronous-unwind-tables \
		-fno-unwind-tables \
#			-nostdlib 

CFLAGS = $(COREFLAGS) -nostartfiles  \
		-g -Wall -std=c89 -pedantic -Wextra -Wconversion -Wcast-align=strict \
		-Wcast-qual -Wfloat-equal -Wundef -Wshadow -Wpointer-arith \
		-Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings \
		-Waggregate-return -Wunreachable-code \
		-ffreestanding \

LDFLAGS = $(COREFLAGS) -L/usr/arm-none-eabi/lib/arm/v5te/hard \
		  -L/usr/arm-none-eabi/lib -Tstm32f4.ld -Wl,--gc-sections
		   

# !!! IMPORTANT !!! 
# If you're desperately trying to figure out why division doesn't work, try
# -lgcc. However, be sure to remember that it's sloooow.

SPEC_FILE := --specs=nano.specs 

ifeq ($(mode), debug)
CFLAGS += -O0
else
CFLAGS += -O2
endif

ifeq ($(mode), debug)
BUILD_DIR := $(BUILD_DIR_BASE)/debug
else
BUILD_DIR := $(BUILD_DIR_BASE)/release
endif

mode_defined = 1
ifneq ($(mode), release)
ifneq ($(mode), debug)
mode_defined = 0
endif
endif

# Find all the C files we want to compile
# Note the single quotes around the * expressions. Make will incorrectly expand
# these otherwise.

SRCS := $(shell find $(SRC_DIRS) -name '*.c' -print)

# String substitution for every C file.
# As an example, hello.c turns into ./build/hello.c.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.c.o turns into ./build/hello.c.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header
# files
INC_DIRS := $(shell find $(SRC_DIRS) $(prune_command) -type d -print)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands
# this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS) $(EXTRA_LIB_DIRS))

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := $(INC_FLAGS) -MMD -MP

.PHONY:all
all: information $(BUILD_DIR)/$(TARGET_ELF)

information:
ifeq ($(mode_defined), 0)
	$(error Invalid build mode. Please use 'make mode=release' (default) 
		\ or 'make mode=debug')
else
	$(info  Building in "$(mode)" mode)
endif

# The final build step. Note the build outputs an .elf, but we're using objcopy
# to get a .bin.
$(BUILD_DIR)/$(TARGET_ELF): $(OBJS)
	$(CC) $(SPEC_FILE) $(OBJS) $(LDLIBS) -o $@ $(LDFLAGS)
	$(OBJCOPY) -O binary $(BUILD_DIR)/$(TARGET_ELF) $(BUILD_DIR)/$(TARGET_BIN)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(SPEC_FILE) $(CPPFLAGS) $(CFLAGS) $(LDLIBS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: flash
flash: all 
	st-flash write $(BUILD_DIR)/$(TARGET_BIN) 0x08000000

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want
# those errors to show up.
-include $(DEPS)
