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

# agbcc toolchain.  Per-file overrides are possible by adding rules like:
#   $(BUILD_DIR)/src/foo.o: CC    := old_agbcc
#   $(BUILD_DIR)/src/foo.o: CFLAGS := -O1 -mthumb-interwork
# before the generic pattern rule below.
CC      := agbcc
CPP     := cpp -P
CFLAGS  := -O2 -mthumb-interwork -Wimplicit -Wparentheses -Werror -fhex-asm

INCLUDE := -I include

BUILD_DIR := build

# SDK library units are compiled with old_agbcc (docs/research/compiler-
# validation.md, issue #7): the 0x080CF9xx zone (agb_sram etc.) matches the
# old compiler's interwork epilogues (pop {rN}; bx rN) and bl _call_via_rN.
$(BUILD_DIR)/src/agb_sram.o: CC := old_agbcc
$(BUILD_DIR)/src/agb_sram.o: CFLAGS := -O1 -mthumb-interwork

ASM_OBJS  := $(BUILD_DIR)/asm/rom_header.o \
             $(BUILD_DIR)/asm/crt0.o

# Per-segment data objects, split from the former main_blob.
# Each .s file .incbin's its slice of baserom.gba; the linker script
# pins every section at its exact ROM VMA.
DATA_SRCS := $(wildcard data/*.s)
DATA_OBJS := $(patsubst %.s,$(BUILD_DIR)/%.o,$(DATA_SRCS))

# C objects compiled from src/. Empty by default — add .c files to src/ to
# grow this list organically. The link is unchanged until real objects appear.
SRC_SRCS  := $(wildcard src/**/*.c src/*.c)
SRC_OBJS  := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC_SRCS))

ALL_OBJS  := $(ASM_OBJS) $(DATA_OBJS) $(SRC_OBJS)

ELF := $(BUILD_DIR)/$(ROM:.gba=.elf)

.PHONY: all compare check-headers progress clean

all: $(ROM)

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) -mcpu=arm7tdmi -o $@ $<

# agbcc C compilation pipeline:
#   1. cpp          — standard C pre-processor (strips comments, expands macros)
#   2. $(CC)        — agbcc (GCC 2.x back-end), emits GAS assembly to stdout
#   3. echo/cat     — appends ".text\n\t.align\t2, 0" (required by agbcc output)
#   4. $(AS)        — assemble the resulting .s into an object file
$(BUILD_DIR)/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CPP) $(INCLUDE) $< | $(CC) $(CFLAGS) -o - - | \
	  { cat; printf '.text\n\t.align\t2, 0\n'; } | \
	  $(AS) -mcpu=arm7tdmi -o $@ -

# Header smoke test (issue #27): compile a TU that touches every
# include/gba/*.h header with both validated compilers.  Compile-only —
# the objects are never linked into the ROM.
GBA_HEADERS := $(wildcard include/gba/*.h)

check-headers: $(BUILD_DIR)/header_smoke_agbcc.o $(BUILD_DIR)/header_smoke_old_agbcc.o
	@echo "header smoke check passed (agbcc + old_agbcc)"

$(BUILD_DIR)/header_smoke_agbcc.o: tools/header_smoke.c $(GBA_HEADERS)
	@mkdir -p $(dir $@)
	$(CPP) $(INCLUDE) $< | $(CC) $(CFLAGS) -o - - | \
	  { cat; printf '.text\n\t.align\t2, 0\n'; } | \
	  $(AS) -mcpu=arm7tdmi -o $@ -

$(BUILD_DIR)/header_smoke_old_agbcc.o: tools/header_smoke.c $(GBA_HEADERS)
	@mkdir -p $(dir $@)
	$(CPP) $(INCLUDE) $< | old_agbcc -O1 -mthumb-interwork -o - - | \
	  { cat; printf '.text\n\t.align\t2, 0\n'; } | \
	  $(AS) -mcpu=arm7tdmi -o $@ -

$(ELF): $(ALL_OBJS) linker.ld
	$(LD) -T linker.ld -Map $(BUILD_DIR)/knidl.map -o $@ $(ALL_OBJS)

$(ROM): $(ELF)
	$(OBJCOPY) -O binary $< $@
	python3 tools/gbafix.py $@

compare: $(ROM)
	sha1sum -c $(SHA1_FILE)

# Progress report: parse the linker map into code/data byte counts and
# percentages (tools/calcrom.pl, vendored from katam/pret). Requires a full
# link, i.e. a baserom.gba must be present.
progress: $(ELF)
	perl tools/calcrom.pl $(BUILD_DIR)/knidl.map

clean:
	rm -rf $(BUILD_DIR) $(ROM)

else

DOCKER_RUN := docker run --rm -v $(CURDIR):/src -w /src $(IMAGE)

.PHONY: image all compare check-headers progress clean

image:
	docker build -t $(IMAGE) .

all: image
	$(DOCKER_RUN) make all INSIDE_DOCKER=1

compare: image
	$(DOCKER_RUN) make compare INSIDE_DOCKER=1

check-headers: image
	$(DOCKER_RUN) make check-headers INSIDE_DOCKER=1

progress: image
	$(DOCKER_RUN) make progress INSIDE_DOCKER=1

clean:
	rm -rf $(BUILD_DIR) $(ROM)

endif
