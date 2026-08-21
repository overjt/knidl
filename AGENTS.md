# AGENTS.md

## Project

Matching decompilation of Kirby: The Amazing Mirror's predecessor, **Kirby: Nightmare in Dream Land** (GBA, 2002). Goal is code that compiles to output matching the original ROM, not a rewrite or port.

- Language: C/C++.
- All code, comments, commit messages, and documentation must be in **English**.
- Before decompiling a new module, read `docs/lessons-learned.md` — pitfalls and
  validated workflow from previous modules (build-system gotchas, m2c/tooling,
  old_agbcc source shapes). Add new lessons there as they are discovered.

## ROM handling

- Builds require a user-supplied, legally-dumped `baserom.gba`. The ROM is **never committed**; it must always be gitignored.
- Do not commit or link to ROM contents, extracted copyrighted assets, or other people's dumps.

## Builds

- All compilation happens in **Docker**; do not install toolchains (compiler, devkitARM, etc.) on the host machine.
- Do not assume host toolchains exist. If a `Dockerfile`/build script is missing or broken, fix or extend it rather than building natively.
- Commands:
  - `make image` — build the toolchain image (Debian 12 + `arm-none-eabi` binutils + pinned agbcc fork `jiangzhengwenjz/agbcc@new_newlib_pret`, commit `59b966e`).
  - `make` / `make all` — build `knidl.gba` (header from source + `baserom.gba` via `.incbin`) and patch the header with `tools/gbafix.py`.
  - `make compare` — build and verify SHA-1 against `knidl.sha1` (USA `A7KE`, SHA-1 `37a476567d133c146fee6b5e2eb0b07a215da6b0`).
  - `make progress` — parse `build/knidl.map` with `tools/calcrom.pl` into code/data byte counts and percentages.
  - `make check-headers` — compile-only smoke test of `include/gba/*.h` (`tools/header_smoke.c`) with agbcc + old_agbcc; never linked into the ROM.
  - `make clean` — remove `build/` and `knidl.gba`.
- Header fields for `gbafix`: title `AGB KIRBY DX`, code `A7KE`, maker `01`, version `0`. Internal ROM codes are `A7K*` (not `AKT*`).

## Conventions

- pret-style layout: `src/` (decompiled C), `asm/` (hand-written assembly), `data/` (extracted blobs), `tools/`, `linker.ld`, `<game>.sha1`.
- The Nintendo logo and any copyrighted assets are `.incbin`'d from `baserom.gba` at build time, never committed.
- Compiler: agbcc family (validated in `docs/research/compiler-validation.md`, issue #7): default `agbcc` with `-O2 -mthumb-interwork` for `src/`; `old_agbcc` with `-O1 -mthumb-interwork` for SDK files (m4a, `0x080CF9xx` zone — confirmed byte-exact on `src/agb_sram.c`, issue #8); `agbcc_arm` only for ARM-mode units. Fork flags `-fhex-asm -f2003-patch -ffix-debug-line` are safe additions (no codegen change).

## Status

- `make compare` passes (ROM built from source matches baserom byte-for-byte).
- CI (`.github/workflows/build.yml`): toolchain image + baserom-free compile/tooling checks always run; `make compare` runs only when a `baserom.gba` is available (self-hosted runner, Actions cache, or `BASEROM_URL` secret) and fails closed on hash mismatch; otherwise skipped explicitly.
- Progress tracking: `make progress` (`tools/calcrom.pl`, vendored from katam, adapted to this repo's `build/` layout and custom section names).
- README.md / INSTALL.md follow pret conventions (ROM facts, Docker-only builds, no-affiliation and dump-your-own-cartridge disclaimers, no OSS license).
- ROM split into 30 address-pinned sections in `linker.ld` (boundaries from `docs/analysis/segments.txt`); each section is a per-segment `.incbin` slice in `data/`.
- Research docs with sources live in `docs/research/` (prior art, toolchain, tooling pipeline, ROM facts + bootstrap checklist).
- First C module decompiled (issue #8): SRAM driver `src/agb_sram.c` (`0x080CFA9C-0x080CFC2F`, old_agbcc `-O1`), linked from C; ROM remains byte-identical.
- Platform headers complete (issue #27): full I/O map (`include/gba/io_reg.h`), interrupt IDs + master-ISR dispatch order (`include/gba/interrupts.h`), SDK-order SWI numbers + thunk prototypes (`include/gba/syscall.h`), umbrella `include/gba/gba.h`; conventions in `docs/header-conventions.md`, guarded by `make check-headers` (agbcc + old_agbcc).
- ROM-wide symbol database (issue #22): `tools/symdb.py` + `tools/symdb_check.py` via `make symbols` (Docker); committed `docs/analysis/symbols.csv` (5,201 functions: 5,194 Thumb + 7 ARM) and `docs/analysis/callgraph.csv` (19,317 edges); validated against a fresh dual-view objdump disassembly (see `docs/analysis/rom-map.md` §7).
- ROM splitter (issue #23): `tools/split.py` + `tools/split_config.json` via `make split` extracts segments into labeled, byte-verified `asm/<segment>.s` (functions labeled from the symbol DB, symbolic literal pools, `asm/rom_syms.s` absolute symbols for unsplit targets) and removes the replaced `data/<segment>.s` incbin; usage and pitfalls in `docs/splitting.md` + `docs/lessons-learned.md` §4.
- All SDK/ARM segments around the code region converted from `.incbin` to labeled asm (issue #24): `task_switch_helpers`, `task_literals`, `sdk_swi_wrappers`, `sdk_reset_helper`, `sdk_libc` (`_call_via_r0..lr` exported; task trampolines decoded in rom-map §6), `interworking_veneer` (+ its literal-word gap), `irq_handler_table_14`, `lib_misc`, `lib_rodata_fir_tables`. Task-system IWRAM cells are named via config `data_symbols`, non-DB labels via `extra_labels`; hand names must always go through `tools/split_config.json` because CI re-checks split regeneration byte-for-byte.
- Whole Thumb game-code region split into per-function labeled asm (issue #25): `agb_init` (1 chunk), `game_code_early` (3 chunks; odd start = 1 data byte) and `game_code_and_rodata` (14 ~64 KiB chunks, 5,003 functions) live under `asm/<segment>/<segment>_NN.s` via the config's `chunk_bytes`. Chunks share the segment's linker section (ld concatenates them in address order); cross-chunk branches use global `loc_XXXXXXXX` labels; no `.incbin` remains below `0x080D0000`. objdump→gas hazards are auto-repaired per instruction (`-marmv4t`, error-line feedback, post-assemble byte-diff feedback — lessons §4.15–4.17); ROM stays byte-identical.
- Next milestones: decompile split/SDK modules to C using the validated per-zone compiler recipe (task system Thumb side, sound driver, then game code); grow `src/` one module at a time with `asmdiff.sh` on the module range.
