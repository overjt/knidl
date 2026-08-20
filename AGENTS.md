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
- Next milestones: crt0/entry (`0x080000C0`) into `asm/crt0.s` (done), decompile more SDK/game modules using the validated per-zone compiler recipe.
