# AGENTS.md

## Project

Matching decompilation of Kirby: The Amazing Mirror's predecessor, **Kirby: Nightmare in Dream Land** (GBA, 2002). Goal is code that compiles to output matching the original ROM, not a rewrite or port.

- Language: C/C++.
- All code, comments, commit messages, and documentation must be in **English**.
- Before decompiling a new module, read `docs/decomp-loop.md` — the standard
  per-function loop (pick → m2c first pass → asmdiff iterate → decomp-permuter
  escalation → land + verify) including the subagent handoff contract — and
  `docs/lessons-learned.md` — pitfalls and validated workflow from previous
  modules (build-system gotchas, m2c/tooling, old_agbcc source shapes). Add new
  lessons there as they are discovered.

## Agent harness (optional)

A project-local harness for the Pi coding agent lives in `tools/pi_harness.py`,
`tools/pi-knidl.sh`, `.githooks/` and `.pi/`; see
[docs/pi-harness.md](docs/pi-harness.md). It is optional — every rule in this
file applies with or without it — but when it is active a `pre-commit` merge
gate rejects any commit whose staged snapshot does not rebuild the ROM
byte-for-byte, and `python3 tools/pi_harness.py next` is the authoritative
answer to "which module is next". Name modules by the frozen ids of
`docs/analysis/module-map.md` section 4; the `id` column of
`module-map.csv` is positional and renumbers as ranges land.

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

## Git / PR workflow (mandatory for agents)

- `master` is the main branch and the ONLY valid PR base. `init` is a frozen bootstrap snapshot — never merge or push work into it (it may appear as origin/HEAD locally; ignore that).
- Work on a feature branch, open the PR against `master`, and wait for CI ("Build and verify") to pass.
- **Do NOT merge PRs.** The repo owner reviews and approves every merge personally. An agent's job ends with: PR open, CI green, a clear description (what/why, evidence of `make clean && make compare` OK), and a comment or summary pointing at anything a reviewer should double-check.
- Issues auto-close via "Closes #N" only when the owner merges to `master` — do not close issues manually.

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
- First game-side C module decompiled (issue #28): `AgbInit` `src/agb_init.c` (`0x08000310-0x080008E7`, default agbcc `-O2 -mthumb-interwork`), including its post-epilogue pool-skip branch and 121-word literal pool; the old `agb_init`/`game_code_early` boundary at `0x080006FF` was an analysis artifact (it split the final `bx r0` and orphaned the pool) and is now `0x080008E8`. The ~90 IWRAM/EWRAM cells it initializes are named `gUnk_<addr>` via `split_config.json` `data_symbols`. Matching shapes documented in `docs/lessons-learned.md` §3.6-§3.11.
- Game main loop decompiled (issue #33): `AgbMain` `src/main.c` (`0x08007300-0x080075B7`, default agbcc `-O2 -mthumb-interwork`), the 23-state dispatch loop at the start of `game_code_and_rodata` (boot flow in rom-map §4). The symbol formerly called `main` is now `AgbMain` (no `__gccmain` call in the ROM proves the original name wasn't `main`, lesson 3.13); the crt0 ARM entry `0x080000C0` was renamed `Start`. Its 6 RAM cells are `gUnk_<addr>` data_symbols.
- Per-function decompilation tooling (post-#28): `tools/fnmatch.sh <start> <end> <file.c> [--old]` byte-verifies a candidate C file against the ROM without touching the build (auto stand-ins, pool-resolving diff); `tools/carve.py <start> <end> <name> [--write]` lands a verified range as a `c_code` segment (rewrites segments.txt/split_config.json/linker.ld with validation). Workflow: `docs/decomp-loop.md` §3/§5.
- Platform headers complete (issue #27): full I/O map (`include/gba/io_reg.h`), interrupt IDs + master-ISR dispatch order (`include/gba/interrupts.h`), SDK-order SWI numbers + thunk prototypes (`include/gba/syscall.h`), umbrella `include/gba/gba.h`; conventions in `docs/header-conventions.md`, guarded by `make check-headers` (agbcc + old_agbcc).
- ROM-wide symbol database (issue #22): `tools/symdb.py` + `tools/symdb_check.py` via `make symbols` (Docker); committed `docs/analysis/symbols.csv` and `docs/analysis/callgraph.csv` (5,241 functions / 19,364 edges as of #31); validated against a fresh dual-view objdump disassembly (see `docs/analysis/rom-map.md` §7).
- ROM splitter (issue #23): `tools/split.py` + `tools/split_config.json` via `make split` extracts segments into labeled, byte-verified `asm/<segment>.s` (functions labeled from the symbol DB, symbolic literal pools, `asm/rom_syms.s` absolute symbols for unsplit targets) and removes the replaced `data/<segment>.s` incbin; usage and pitfalls in `docs/splitting.md` + `docs/lessons-learned.md` §4.
- All SDK/ARM segments around the code region converted from `.incbin` to labeled asm (issue #24): `task_switch_helpers`, `task_literals`, `sdk_swi_wrappers`, `sdk_reset_helper`, `sdk_libc` (`_call_via_r0..lr` exported; task trampolines decoded in rom-map §6), `interworking_veneer` (+ its literal-word gap), `irq_handler_table_14`, `lib_misc`, `lib_rodata_fir_tables`. Task-system IWRAM cells are named via config `data_symbols`, non-DB labels via `extra_labels`; hand names must always go through `tools/split_config.json` because CI re-checks split regeneration byte-for-byte.
- Whole Thumb game-code region split into per-function labeled asm (issue #25): `game_code_early` (2 chunks; `agb_init` decompiled to `src/agb_init.c` in #28) and `game_code_and_rodata` (14 ~64 KiB chunks, 5,003 functions) live under `asm/<segment>/<segment>_NN.s` via the config's `chunk_bytes`. Chunks share the segment's linker section (ld concatenates them in address order); cross-chunk branches use global `loc_XXXXXXXX` labels; no `.incbin` remains below `0x080D0000`. objdump→gas hazards are auto-repaired per instruction (`-marmv4t`, error-line feedback, post-assemble byte-diff feedback — lessons §4.15–4.17); ROM stays byte-identical.
- Decomp-permuter vendored + standard loop documented (issue #26): `tools/decomp-permuter/` (simonlindholm/decomp-permuter@`2795247`, own MIT LICENSE kept; Dockerfile gained the required `toml` pip dep) verified inside `knidl-builder` on a scratch example (`tools/permuter-example/`, scorer reaches 0 against ROM-extracted target asm); per-function workflow + subagent handoff contract in `docs/decomp-loop.md`, old_agbcc-specific pitfalls in lessons §2.9–2.11.
- m4a/mp2k sound engine located and fully labeled (issue #31): engine code `0x080CD89C-0x080CFA4B` (asm core + C driver halves, boundaries/evidence in `docs/analysis/rom-map.md` §8), 91 canonical names in the symbol DB (94 entries in the range; 3 tiny bx-r3 shims stay sub_*) (incl. 10 dead SDK exports via the new `EXTRA_THUMB_ENTRIES`/`curated` evidence mechanism in `tools/symdb.py`), engine RAM cells named via `data_symbols` (`gSoundInfo` `0x030056D0`, `SOUND_INFO_PTR` `0x03007FF0`, players/tracks, `gSoundMainRAM_Buffer` `0x03007150`), engine rodata tables mapped at `0x0860A140-0x0860B797` (byte-identical to pokeemerald's — same engine revision). Decompilation proceeds in child issues per rom-map §8.5.
- Bulk code clustered into a module map (issue #34): `docs/analysis/module-map.md` + `docs/analysis/module-map.csv` (`make modmap`, `tools/modmap.py`, CSV regeneration checked in CI) partition the remaining `0x080075B8-0x080CD89C` (792.7 KiB, 4,950 functions) into **37 contiguous candidate modules** of 12-31 KiB with per-module evidence (anchor tables, task types, call traffic, pool references, difficulty, suggested batches) and a five-wave decompile order; child issues of #35 are generated from it. Key findings: the ROM task-type table at `0x0872FF30` has **266 entries whose second word is the task body's entry point** (not a flag word — corrects rom-map §6 / `src/early_58e4.c`), seg 7 references the I/O block only 20 times in 792 KiB (everything goes through the early zone's IWRAM shadows), and 3,288 of 5,045 functions are reachable only through ROM pointer tables.
- Next milestones: decompile split/SDK modules to C using the validated per-zone compiler recipe (task system Thumb side, sound driver, then game code); grow `src/` one module at a time with `asmdiff.sh` on the module range.
