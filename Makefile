# Kirby: Nightmare in Dream Land (USA) matching decompilation.
#
# All compilation runs inside Docker (see Dockerfile). Host-side targets wrap
# the container; pass INSIDE_DOCKER=1 to run the real rules directly.

ROM       := knidl.gba
SHA1_FILE := knidl.sha1
IMAGE     := knidl-builder

ifeq ($(INSIDE_DOCKER),1)

AS      := arm-none-eabi-as
LD      := arm-none-eabi-ld
OBJCOPY := arm-none-eabi-objcopy

BUILD_DIR := build

ASM_OBJS  := $(BUILD_DIR)/asm/rom_header.o

# Per-segment data objects, split from the former main_blob.
# Each .s file .incbin's its slice of baserom.gba; the linker script
# pins every section at its exact ROM VMA.
DATA_SRCS := $(wildcard data/*.s)
DATA_OBJS := $(patsubst %.s,$(BUILD_DIR)/%.o,$(DATA_SRCS))

ALL_OBJS  := $(ASM_OBJS) $(DATA_OBJS)

ELF := $(BUILD_DIR)/$(ROM:.gba=.elf)

.PHONY: all compare clean

all: $(ROM)

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) -mcpu=arm7tdmi -o $@ $<

$(ELF): $(ALL_OBJS) linker.ld
	$(LD) -T linker.ld -Map $(BUILD_DIR)/knidl.map -o $@ $(ALL_OBJS)

$(ROM): $(ELF)
	$(OBJCOPY) -O binary $< $@
	python3 tools/gbafix.py $@

compare: $(ROM)
	sha1sum -c $(SHA1_FILE)

clean:
	rm -rf $(BUILD_DIR) $(ROM)

else

DOCKER_RUN := docker run --rm -v $(CURDIR):/src -w /src $(IMAGE)

.PHONY: image all compare clean

image:
	docker build -t $(IMAGE) .

all: image
	$(DOCKER_RUN) make all INSIDE_DOCKER=1

compare: image
	$(DOCKER_RUN) make compare INSIDE_DOCKER=1

clean:
	rm -rf $(BUILD_DIR) $(ROM)

endif
