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

# m4a C driver (issue #53): old_agbcc like the SRAM driver but at -O2 —
# verified byte-exact via tools/fnmatch.sh --old2 (loop strength reduction,
# pool AND masks and the dead ident-lock stores only reproduce at -O2).
$(BUILD_DIR)/src/m4a_c1.o: CC := old_agbcc
$(BUILD_DIR)/src/m4a_c1.o: CFLAGS := -O2 -mthumb-interwork

# m4a C driver part 2, CGB/PSG side (issue #54): same recipe as part 1.
$(BUILD_DIR)/src/m4a_cgb.o: CC := old_agbcc
$(BUILD_DIR)/src/m4a_cgb.o: CFLAGS := -O2 -mthumb-interwork

# m4a C driver part 3, track controls + memacc/xcmd handlers (issue #55):
# same recipe as parts 1-2 (one translation unit upstream).
$(BUILD_DIR)/src/m4a_ctrl.o: CC := old_agbcc
$(BUILD_DIR)/src/m4a_ctrl.o: CFLAGS := -O2 -mthumb-interwork

# game_code_early tail (issue #32): despite sitting in the middle of the
# agbcc -O2 game-code zone, the translation unit starting around 0x08001CC8 is
# old_agbcc -O2.  Fingerprint: its leaf functions end in a bare `bx lr`, while
# agbcc unconditionally emits `push {lr}` / `pop {r0}; bx r0` for every
# function.  Verified byte-exact via tools/fnmatch.sh --old2.
$(BUILD_DIR)/src/early_1fd0.o: CC := old_agbcc
$(BUILD_DIR)/src/early_1fd0.o: CFLAGS := -O2 -mthumb-interwork

# Same old_agbcc -O2 unit, further along (issue #32): link input dispatch,
# frame driver, wait helpers, 12-bit LCG rand, decimal split, colour blend.
$(BUILD_DIR)/src/early_2b04.o: CC := old_agbcc
$(BUILD_DIR)/src/early_2b04.o: CFLAGS := -O2 -mthumb-interwork

# 0x0800293C-0x08002B04 (issue #32): back in the DEFAULT agbcc -O2 recipe —
# the old_agbcc unit does not run contiguously through the zone.  Verified
# byte-exact with tools/fnmatch.sh and no flag.

# Sound/SE subsystem, 0x08003110-0x08003888 (issue #32): old_agbcc -O2 again.
# Fingerprint: sub_08003194 / sub_0800374c / sub_080037f8 / sub_0800381c are
# leaves ending in a bare `bx lr`; under the default recipe sub_08003194 is 19
# bytes off with an extra push/pop.
$(BUILD_DIR)/src/early_3110.o: CC := old_agbcc
$(BUILD_DIR)/src/early_3110.o: CFLAGS := -O2 -mthumb-interwork
$(BUILD_DIR)/src/early_3484.o: CC := old_agbcc
$(BUILD_DIR)/src/early_3484.o: CFLAGS := -O2 -mthumb-interwork

# SIO multi-play / MultiBoot link driver, 0x08003964-0x08004000 (issue #32).
$(BUILD_DIR)/src/early_3964.o: CC := old_agbcc
$(BUILD_DIR)/src/early_3964.o: CFLAGS := -O2 -mthumb-interwork

# SIO link driver continued, 0x08004000-0x08004734 (issue #32): the SIO IRQ
# handler, handshake steps and the 32-bit normal-mode bulk transfer halves.
$(BUILD_DIR)/src/early_4000.o: CC := old_agbcc
$(BUILD_DIR)/src/early_4000.o: CFLAGS := -O2 -mthumb-interwork

# AGB SDK MultiBoot client library + link session sequencer (issue #32).
$(BUILD_DIR)/src/early_4734.o: CC := old_agbcc
$(BUILD_DIR)/src/early_4734.o: CFLAGS := -O2 -mthumb-interwork
$(BUILD_DIR)/src/early_4d6c.o: CC := old_agbcc
$(BUILD_DIR)/src/early_4d6c.o: CFLAGS := -O2 -mthumb-interwork

# Cooperative task engine, 0x08004FEC-0x08005654 (issue #32).
$(BUILD_DIR)/src/early_4fec.o: CC := old_agbcc
$(BUILD_DIR)/src/early_4fec.o: CFLAGS := -O2 -mthumb-interwork
$(BUILD_DIR)/src/early_55b0.o: CC := old_agbcc
$(BUILD_DIR)/src/early_55b0.o: CFLAGS := -O2 -mthumb-interwork

# All of asm/ is assembled into the ROM: hand-written files (rom_header.s,
# crt0.s), split-generated segment files (asm/<segment>.s, see tools/
# split.py / docs/splitting.md), chunked code segments (issue #25:
# asm/<segment>/<segment>_NN.s, one file per ~64 KiB at function
# boundaries) and asm/rom_syms.s (absolute symbols for every DB function
# not defined by a real label, so split files can reference not-yet-split
# code symbolically).  $(sort) keeps the link order deterministic; within
# a chunk directory the zero-padded suffixes make alphabetical order equal
# address order, which ld's input-section concatenation requires.
ASM_SRCS  := $(sort $(wildcard asm/*.s) $(wildcard asm/*/*.s))
ASM_OBJS  := $(patsubst %.s,$(BUILD_DIR)/%.o,$(ASM_SRCS))

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

.PHONY: all compare check-headers progress symbols split clean

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

# ROM-wide function/symbol database (issue #22): regenerate
# docs/analysis/symbols.csv + callgraph.csv from baserom.gba and validate
# them (coverage + spot checks against a fresh dual-view disassembly).
symbols: baserom.gba tools/symdb.py tools/symdb_check.py docs/analysis/segments.txt
	python3 tools/symdb.py --rom baserom.gba --segments docs/analysis/segments.txt --out-dir docs/analysis
	python3 tools/symdb_check.py --rom baserom.gba --symbols docs/analysis/symbols.csv --callgraph docs/analysis/callgraph.csv --segments docs/analysis/segments.txt

# Extract configured ROM ranges into labeled, byte-identical assembly
# (issue #23; see docs/splitting.md).  For each segment in tools/
# split_config.json this writes asm/<name>.s, deletes the data/<name>.s
# incbin slice, and regenerates asm/rom_syms.s.
split: baserom.gba tools/split.py tools/split_config.json docs/analysis/segments.txt docs/analysis/symbols.csv
	python3 tools/split.py --rom baserom.gba --config tools/split_config.json

clean:
	rm -rf $(BUILD_DIR) $(ROM)

else

DOCKER_RUN := docker run --rm -v $(CURDIR):/src -w /src $(IMAGE)

.PHONY: image all compare check-headers progress symbols split clean

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

symbols: image
	$(DOCKER_RUN) make symbols INSIDE_DOCKER=1

split: image
	$(DOCKER_RUN) make split INSIDE_DOCKER=1

clean:
	rm -rf $(BUILD_DIR) $(ROM)

endif
