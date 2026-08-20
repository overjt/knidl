# AGENTS.md

## Project

Matching decompilation of Kirby: The Amazing Mirror's predecessor, **Kirby: Nightmare in Dream Land** (GBA, 2002). Goal is code that compiles to output matching the original ROM, not a rewrite or port.

- Language: C/C++.
- All code, comments, commit messages, and documentation must be in **English**.

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
  - `make clean` — remove `build/` and `knidl.gba`.
- Header fields for `gbafix`: title `AGB KIRBY DX`, code `A7KE`, maker `01`, version `0`. Internal ROM codes are `A7K*` (not `AKT*`).

## Conventions

- pret-style layout: `src/` (decompiled C), `asm/` (hand-written assembly), `data/` (extracted blobs), `tools/`, `linker.ld`, `<game>.sha1`.
- The Nintendo logo and any copyrighted assets are `.incbin`'d from `baserom.gba` at build time, never committed.
- Compiler: agbcc family (validated in `docs/research/compiler-validation.md`, issue #7): default `agbcc` with `-O2 -mthumb-interwork` for `src/`; `old_agbcc` reserved for SDK files (m4a, agb_sram, `0x080CF9xx` zone); `agbcc_arm` only for ARM-mode units. Fork flags `-fhex-asm -f2003-patch -ffix-debug-line` are safe additions (no codegen change).

## Status

- `make compare` passes (ROM built from source matches baserom byte-for-byte).
- ROM split into 30 address-pinned sections in `linker.ld` (boundaries from `docs/analysis/segments.txt`); each section is a per-segment `.incbin` slice in `data/`.
- Research docs with sources live in `docs/research/` (prior art, toolchain, tooling pipeline, ROM facts + bootstrap checklist).
- Next milestones: extract crt0/entry (`0x080000C0`) into `asm/crt0.s`, then decompile first C modules (validate agbcc codegen against leaf functions).
