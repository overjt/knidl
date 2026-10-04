# Kirby: Nightmare in Dream Land (USA) matching decompilation.
#
# All compilation runs inside Docker (see Dockerfile). Host-side targets wrap
# the container; pass INSIDE_DOCKER=1 to run the real rules directly.

ROM       := knidl.gba
SHA1_FILE := knidl.sha1
IMAGE     := knidl-builder

# BUILD_DIR and ROM must be visible on BOTH sides of the INSIDE_DOCKER split:
# the host-side `clean` target expands them directly, and while they were
# defined only inside the container branch it ran `rm -rf  knidl.gba` with an
# EMPTY first argument - so `make clean` never removed build/ on the host and
# every "clean rebuild" silently reused stale objects.  That masked a real
# regression in this issue (#85): a shared-header change that broke M17/M18
# still reported a byte-identical ROM because those objects were never
# recompiled.
BUILD_DIR := build
ROM       := knidl.gba

ifeq ($(INSIDE_DOCKER),1)

# Every compile rule is a pipeline (cpp | agbcc | as).  Without pipefail the
# pipeline's status is `as`'s, so an agbcc error ("structure has no member
# named ...") was reported on stderr and then SWALLOWED: `as` happily
# assembled the truncated output, the object linked, and `make` exited 0 with
# a silently wrong ROM.  That is how issue #85 reached CI green locally and
# failed there.  bash + pipefail makes any stage's failure fail the rule.
SHELL       := /bin/bash
.SHELLFLAGS := -o pipefail -c


AS      := arm-none-eabi-as
LD      := arm-none-eabi-ld
OBJCOPY := arm-none-eabi-objcopy

# agbcc toolchain.  Per-file overrides are possible by adding rules like:
#   $(BUILD_DIR)/src/foo.o: CC    := old_agbcc
#   $(BUILD_DIR)/src/foo.o: CFLAGS := -O1 -mthumb-interwork
# before the generic pattern rule below.
CC      := agbcc
CPP     := cpp -P
# -fprologue-bugfix suppresses agbcc's spurious leaf `push {lr}` (it stops
# caching current_function_has_far_jump, see docs/lessons-learned.md 3.75).
# The whole game-code zone needs it; without it, leaf functions that branch
# gain a push/pop pair and old_agbcc coincidentally looks like a better fit.
CFLAGS  := -O2 -mthumb-interwork -fprologue-bugfix -Wimplicit -Wparentheses -Werror -fhex-asm

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

.PHONY: all compare check-headers check-data audit progress datastats shifttest boottest-roms boottest-run boottest-coverage-run boottest-coverage-report report symbols split modmap clean

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

check-headers: $(BUILD_DIR)/header_smoke_agbcc.o $(BUILD_DIR)/header_smoke_old_agbcc.o $(BUILD_DIR)/header_smoke_game.o
	@echo "header smoke check passed (agbcc + old_agbcc; game headers)"

$(BUILD_DIR)/header_smoke_agbcc.o: tools/header_smoke.c $(GBA_HEADERS)
	@mkdir -p $(dir $@)
	$(CPP) $(INCLUDE) $< | $(CC) $(CFLAGS) -o - - | \
	  { cat; printf '.text\n\t.align\t2, 0\n'; } | \
	  $(AS) -mcpu=arm7tdmi -o $@ -

# The subsystem headers (issue #36 phase 2) all in one TU: each symbol is
# declared once, so they must compile together.
GAME_HEADERS := $(wildcard include/*.h)

$(BUILD_DIR)/header_smoke_game.o: tools/header_smoke_game.c $(GAME_HEADERS) $(GBA_HEADERS)
	@mkdir -p $(dir $@)
	$(CPP) $(INCLUDE) $< | $(CC) $(CFLAGS) -o - - | \
	  { cat; printf '.text\n\t.align\t2, 0\n'; } | \
	  $(AS) -mcpu=arm7tdmi -o $@ -

$(BUILD_DIR)/header_smoke_old_agbcc.o: tools/header_smoke.c $(GBA_HEADERS)
	@mkdir -p $(dir $@)
	$(CPP) $(INCLUDE) $< | old_agbcc -O1 -mthumb-interwork -o - - | \
	  { cat; printf '.text\n\t.align\t2, 0\n'; } | \
	  $(AS) -mcpu=arm7tdmi -o $@ -

# MATCHING=1 (the default) turns on linker.ld's per-section address
# assertions, so a matching build fails at the first section that moved;
# a modified ROM links with `make MATCHING=0` (docs/data.md section 8).
MATCHING ?= 1

$(ELF): $(ALL_OBJS) linker.ld
	$(LD) --defsym MATCHING=$(MATCHING) -T linker.ld -Map $(BUILD_DIR)/knidl.map -o $@ $(ALL_OBJS)

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

# Data-structure metrics (issue #36, docs/data.md): ROM data symbols still
# defined by absolute address, and the pointer-like words of the data
# segments that are not yet symbolic.  Reads the committed data/ files and
# asm/rom_syms.s against baserom.gba; needs no build.
datastats: baserom.gba tools/datastats.py tools/split_config.json docs/analysis/segments.txt
	python3 tools/datastats.py --rom baserom.gba

# The shift test (issue #36, docs/data.md section 8): link the same objects
# again with padding inserted at a few section boundaries and count, per
# zone, the pointer-like words that did not move with their targets.
shifttest: $(ELF)
	python3 tools/shiftcheck.py --elf $(ELF) --json $(BUILD_DIR)/shifttest.json --objs $(ALL_OBJS)
	python3 tools/ptrcensus.py --elf $(ELF) --shift $(BUILD_DIR)/shifttest.json --by-target --strict --unknown $(BUILD_DIR)/ptrcensus_unknown.json --unreachable $(BUILD_DIR)/ptrcensus_unreachable.json

# The boot test (issue #36, docs/data.md section 8.4), in two halves because
# it needs two images: boottest-roms (knidl-builder) links one shifted ROM
# per insertion point into build/boottest/, as shiftcheck.py's image B, and
# writes the symbol list the scripts' checks name RAM cells with;
# boottest-run (knidl-boottest, tools/boottest/Dockerfile) runs the
# reference and every shifted ROM in lockstep in mGBA, once per script of
# BOOTTEST_INPUT (each from boot with an empty save), and fails at the first
# frame whose video, audio or RAM differs, or when a script's `expect` no
# longer holds on the reference.  BOOTTEST_AT picks the points (section
# names; default: shiftcheck.py's), BOOTTEST_INPUT the scripts (default:
# the CI set below), BOOTTEST_FRAMES the length (default: each script's
# plus 600 frames).
BOOTTEST_DIR   := $(BUILD_DIR)/boottest
BOOTTEST_SYMS  := $(BOOTTEST_DIR)/syms.txt
BOOTTEST_INPUT := tools/boottest/input.txt tools/boottest/subgames.txt tools/boottest/gameover.txt

$(BOOTTEST_SYMS): $(ELF)
	@mkdir -p $(BOOTTEST_DIR)
	arm-none-eabi-nm $(ELF) > $@

boottest-roms: $(ROM) $(BOOTTEST_SYMS)
	python3 tools/boottest.py --elf $(ELF) --out $(BOOTTEST_DIR) $(foreach s,$(BOOTTEST_AT),--at $(s)) --objs $(ALL_OBJS)

# BOOTTEST_RAM_ALLOW: RAM words allowed to differ, each with its evidence.
# gHBlankDmaCnt/gHBlankDmaSrc (0x03001184, 0x03001EF0): the fade driver
# sub_080b6154 (src/save_b6154.c) falls off its end without a return value
# on its last frame, so UpdateHBlankScroll (src/hud_b5840.c) takes the
# function's own address (still in r0) as the DMA count: the ROM's own
# undefined behaviour, layout-dependent by nature (docs/data.md 8.4).
BOOTTEST_RAM_ALLOW := 0x03001184,0x03001EF0

# no prerequisites: the emulator image has no toolchain to rebuild them.
# Every script runs even after one fails, so one run shows them all.
boottest-run:
	@status=0; for s in $(BOOTTEST_INPUT); do \
	  echo "knidl-boottest --input $$s"; \
	  knidl-boottest --input $$s --syms $(BOOTTEST_SYMS) --ram-allow $(BOOTTEST_RAM_ALLOW) $(if $(BOOTTEST_FRAMES),--frames $(BOOTTEST_FRAMES)) $(ROM) $$(cat $(BOOTTEST_DIR)/roms.txt) || status=1; \
	  echo; \
	done; exit $$status

# Execution coverage (docs/data.md 8.4): the reference alone, one
# instruction at a time, per script; the ranges go to
# build/boottest/coverage-<script>.txt (never committed) and
# tools/boottest/coverage.py maps them onto docs/analysis/symbols.csv.
BOOTTEST_COV := $(foreach s,$(BOOTTEST_INPUT),$(BOOTTEST_DIR)/coverage-$(basename $(notdir $(s))).txt)

boottest-coverage-run:
	@for s in $(BOOTTEST_INPUT); do \
	  echo "knidl-boottest --input $$s --coverage"; \
	  knidl-boottest --input $$s --syms $(BOOTTEST_SYMS) --coverage $(BOOTTEST_DIR)/coverage-$$(basename $$s .txt).txt $(ROM) || exit 1; \
	done

boottest-coverage-report:
	python3 tools/boottest/coverage.py --syms $(BOOTTEST_SYMS) $(BOOTTEST_COV)

# Data policy (AGENTS.md, docs/data.md): assets are never committed, and
# data/ may hold only labels, symbolic .words and .incbin slices of
# baserom.gba; needs no baserom, so CI runs it on every push.
check-data:
	python3 tools/check_data_policy.py

# The final audit (issue #37, docs/audit.md): .incbin only in data/*.s and
# the header logo, every raw directive in asm/ justified, the sanctioned asm
# list against tools/calcrom.pl, no raw address in src/ without a symbol,
# macro or reason, the code exceptions, and the placeholder census in
# docs/naming.md; needs no baserom, so CI runs it on every push.
audit:
	python3 tools/audit.py

# objdiff-schema progress report (report.json) for decomp.dev — derived
# from the repo's own ground truth (segments.txt / symbols.csv /
# module-map.csv), no baserom needed.  CI uploads the artifact.
report:
	python3 tools/gen_report.py

# ROM-wide function/symbol database (issue #22): regenerate
# docs/analysis/symbols.csv + callgraph.csv from baserom.gba and validate
# them (coverage + spot checks against a fresh dual-view disassembly).
symbols: baserom.gba tools/symdb.py tools/symdb_check.py docs/analysis/segments.txt
	python3 tools/symdb.py --rom baserom.gba --segments docs/analysis/segments.txt --out-dir docs/analysis
	python3 tools/symdb_check.py --rom baserom.gba --symbols docs/analysis/symbols.csv --callgraph docs/analysis/callgraph.csv --segments docs/analysis/segments.txt

# Subsystem clustering of the bulk game code (issue #34): regenerate
# docs/analysis/module-map.csv from the symbol DB + call graph + baserom.
# The narrative map lives in docs/analysis/module-map.md; pass --report to
# get the full per-module evidence dump that document is written from.
modmap: baserom.gba tools/modmap.py docs/analysis/segments.txt docs/analysis/symbols.csv docs/analysis/callgraph.csv
	python3 tools/modmap.py --rom baserom.gba

# Extract configured ROM ranges into labeled, byte-identical assembly
# (issue #23; see docs/splitting.md).  For each code segment in tools/
# split_config.json this writes asm/<name>.s and deletes the data/<name>.s
# incbin slice; each data segment becomes a structure-only data/<name>.s
# (labels, symbolic pointers, baserom .incbin slices: issue #36,
# docs/data.md).  asm/rom_syms.s is regenerated too.
split: baserom.gba tools/split.py tools/split_config.json docs/analysis/segments.txt docs/analysis/symbols.csv
	python3 tools/split.py --rom baserom.gba --config tools/split_config.json

clean:
	rm -rf $(BUILD_DIR) $(ROM)

else

DOCKER_RUN := docker run --rm -v $(CURDIR):/src -w /src $(IMAGE)

# The boot test's emulator image (mGBA + tools/boottest/boottest.c), kept
# apart from the toolchain image.
BOOTTEST_IMAGE := knidl-boottest

.PHONY: image boottest-image all compare check-headers check-data audit progress datastats shifttest boottest boottest-coverage symbols split modmap clean

image:
	docker build -t $(IMAGE) .

boottest-image:
	docker build -t $(BOOTTEST_IMAGE) tools/boottest

all: image
	$(DOCKER_RUN) make all INSIDE_DOCKER=1 $(if $(MATCHING),MATCHING=$(MATCHING))

compare: image
	$(DOCKER_RUN) make compare INSIDE_DOCKER=1

check-headers: image
	$(DOCKER_RUN) make check-headers INSIDE_DOCKER=1

progress: image
	$(DOCKER_RUN) make progress INSIDE_DOCKER=1

datastats: image
	$(DOCKER_RUN) make datastats INSIDE_DOCKER=1

check-data: image
	$(DOCKER_RUN) make check-data INSIDE_DOCKER=1

audit: image
	$(DOCKER_RUN) make audit INSIDE_DOCKER=1

shifttest: image
	$(DOCKER_RUN) make shifttest INSIDE_DOCKER=1

# The boot test (docs/data.md section 8.4): link the shifted ROMs in the
# toolchain image, then run them against knidl.gba in the emulator image.
BOOTTEST_VARS = $(if $(BOOTTEST_INPUT),BOOTTEST_INPUT="$(BOOTTEST_INPUT)") $(if $(BOOTTEST_FRAMES),BOOTTEST_FRAMES=$(BOOTTEST_FRAMES))

boottest: image boottest-image
	$(DOCKER_RUN) make boottest-roms INSIDE_DOCKER=1 $(if $(BOOTTEST_AT),BOOTTEST_AT="$(BOOTTEST_AT)")
	docker run --rm -v $(CURDIR):/src -w /src $(BOOTTEST_IMAGE) make boottest-run INSIDE_DOCKER=1 $(BOOTTEST_VARS)

# How much of the code the scripts execute (docs/data.md section 8.4).
boottest-coverage: image boottest-image
	$(DOCKER_RUN) make $(ROM) build/boottest/syms.txt INSIDE_DOCKER=1
	docker run --rm -v $(CURDIR):/src -w /src $(BOOTTEST_IMAGE) make boottest-coverage-run INSIDE_DOCKER=1 $(BOOTTEST_VARS)
	$(DOCKER_RUN) make boottest-coverage-report INSIDE_DOCKER=1 $(BOOTTEST_VARS)

# report.json generation only needs Python + the repo's ground-truth CSVs,
# so it runs directly on the host (no toolchain image required).
report:
	python3 tools/gen_report.py

symbols: image
	$(DOCKER_RUN) make symbols INSIDE_DOCKER=1

split: image
	$(DOCKER_RUN) make split INSIDE_DOCKER=1

modmap: image
	$(DOCKER_RUN) make modmap INSIDE_DOCKER=1

clean:
	rm -rf $(BUILD_DIR) $(ROM)

endif
