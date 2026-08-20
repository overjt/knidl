# Decomp Pipeline Tooling for GBA Matching Decompilation

Research notes for **Kirby: Nightmare in Dream Land** (GBA, 2002) matching decomp.
Date: 2026-08-20. All facts below were verified against the linked sources at research time.

---

## 0. Executive summary

- **splat does not support GBA.** Upstream splat only handles N64, PSX, PS2 and PSP
  ([README](https://github.com/ethteck/splat)), and `src/splat/platforms/` contains only
  `n64.py`, `psx.py`, `ps2.py`, `psp.py` — there is no `gba.py`. GBA matching decomps
  (pret's Pokémon repos, Fire Emblem 8, The Minish Cap, and crucially the sibling project
  **KATAM**) all use the **pret-style Makefile build** instead of a yaml splitter.
- **KATAM (Kirby & The Amazing Mirror decomp)** — the closest sibling, same engine lineage
  and same maintainer ecosystem — is pret-style: `Makefile` + `linker.ld` + `katam.sha1`,
  built with **agbcc** (`new_newlib_pret` branch) + **devkitARM** binutils, with
  `make compare`, `asmdiff.sh`, `calcrom.pl` ([repo](https://github.com/jiangzhengwenjz/katam)).
- **Recommended for this repo:** a pret/KATAM-style Makefile pipeline, compiled with agbcc
  inside a Debian-based Docker image (pattern taken from
  [zeldaret/tmc's devcontainer](https://github.com/zeldaret/tmc)), m2c + decomp.me (GBA
  presets: `agbcc`, `old_agbcc`, `agbcc_arm`, `agbccpp`, `gcc2.96`) for function work,
  asm-differ for diffs, and GitHub Actions for CI with ROM hash verification,
  clang-format checks and `make compare`.

---

## 1. ROM splitting: splat vs. the pret way

### 1.1 What splat is

- **splat** — <https://github.com/ethteck/splat> (PyPI package `splat64`). "A binary
  splitting tool to assist with decompilation and modding projects. Currently, only N64,
  PSX, PS2 and PSP binaries are supported."
- **Star Rod** — <https://github.com/z64a/star-rod> — is a suite of tools for editing
  Paper Mario 64 assets from that game's decompilation; it is a modding toolkit, not a
  general splitter, and has no GBA relevance. (The task brief mentioned "star-rod /
  decompals/splat"; in practice splat lives at `ethteck/splat` and the
  [decompals org](https://github.com/orgs/decompals/repositories) does not host it.)
- splat's yaml config (from
  [`test/basic_app/splat.yaml`](https://github.com/ethteck/splat/blob/main/test/basic_app/splat.yaml))
  has two parts, useful as a mental model even though we can't use splat on GBA:

  ```yaml
  options:
    base_path:    split
    platform:     n64          # <- no 'gba' platform exists
    compiler:     KMC
    basename:     basic_app
    target_path:  ../build/basic_app.bin
    elf_path:     build/basic_app_target.elf
    asm_path:     asm
    src_path:     src
    ld_script_path: basic_app.ld
    symbol_addrs_path:    generated.symbols.txt
    undefined_funcs_auto_path: undefined_funcs_auto.txt
    undefined_syms_auto_path:  undefined_syms_auto.txt
  segments:
    - name: header
      type: header
      start: 0x00
    - name: boot
      type: code
      start: 0x1000
      vram:  0x80000400
      bss_size: 0x80
      subsegments:
        - [0x1000, c, main]
        - [0x10F0, hasm, handwritten]
        - [0x1100, data, main]
        - [0x1110, .rodata, main]
  ```

  i.e. options (platform/compiler/paths/symbol files) + a `segments` list mapping ROM
  offsets to `c` / `asm` / `data` / `bin` / `bss` chunks. The
  [splat wiki](https://github.com/ethteck/splat/wiki) documents options and has an
  [Examples page](https://github.com/ethteck/splat/wiki/Examples) of projects using it.

**Verdict:** splat cannot split a GBA ROM without writing a custom platform module.
No maintained GBA fork was found. Don't fight this — GBA decomps solved splitting
differently.

### 1.2 What GBA decomps actually use (pret-style)

GBA matching projects use a **Makefile-driven build** that:

1. requires a user-supplied `baserom.gba` (gitignored, never committed);
2. verifies it by SHA-1 (`tmc.sha1` / `katam.sha1`, checked with
   `sha1sum -c` / `make compare`);
3. extracts binary/data assets from `baserom.gba` via Makefile implicit rules and small
  C tools (`gbagfx`, `mid2agb`, `aif2pcm`, `preproc`, `scaninc`, `gbafix`, `bin2c`);
4. compiles `src/**/*.c` with **agbcc**, assembles `asm/**/*.s` with devkitARM
  `arm-none-eabi-as`, links with `arm-none-eabi-ld` + a hand-written `linker.ld`
  (carts are memory-mapped at `0x08000000`, IWRAM `0x03000000`, EWRAM `0x02000000`).

Reference repos inspected:

| Repo | What it is | Evidence |
| --- | --- | --- |
| [pret/pokeemerald](https://github.com/pret/pokeemerald) | Pokémon Emerald (GBA) decomp, the canonical pret layout | root has `Makefile`, `asmdiff.sh`, `tools/`, `linker.ld`, `.sym` machinery |
| [jiangzhengwenjz/katam](https://github.com/jiangzhengwenjz/katam) | **Kirby & The Amazing Mirror (USA)** decomp — our sibling game | builds sha1 `274b102b6d940f46861a92b4e65f89a51815c12c`; `tools/` = `aif2pcm, bin2c, gbafix, gbagfx, mid2agb, preproc, scaninc`; uses **agbcc `new_newlib_pret` branch** + devkitARM; `make compare`; `asmdiff.sh`; `calcrom.pl` ([INSTALL.md](https://github.com/jiangzhengwenjz/katam/blob/master/INSTALL.md)) |
| [zeldaret/tmc](https://github.com/zeldaret/tmc) | The Minish Cap (GBA) decomp | `Toolchain.mk` (agbcc as `CC1`, devkitARM `arm-none-eabi-*` for `AS/LD/OBJCOPY`), `GBA.mk`, `linker.ld`, `tmc*.sha1`, `progress.py`, `genctx.sh`, `format.sh`, `.clang-format`, `.devcontainer/`, `Jenkinsfile` |
| [other GBA decomps by KATAM's author](https://github.com/jiangzhengwenjz?tab=repositories) | `fireemblem8u`, `csm3`, `totkol`, archived `zeldatmc` | all pret-family; none use splat; none currently have CI workflows |

**Answer to the task question:** KATAM does **not** use splat (or any yaml splitter).
It uses the pret-style Makefile + custom C tools approach, inherited from pret/pokémon
repos. Since Kirby: Nightmare in Dream Land is KATAM's direct predecessor (same
developer, same engine family), mirroring KATAM's setup is the lowest-risk path.

### 1.3 Asset tools (from katam `tools/` — reuse directly)

| Tool | Purpose |
| --- | --- |
| `gbagfx` | converts GBA graphics formats (4bpp/8bpp) to/from PNG (`libpng-dev` needed) |
| `mid2agb` | converts MIDI to the game's music (AGB) format |
| `aif2pcm` | converts AIC/AIFF samples to PCM/ADPCM |
| `preproc` | C-preprocesses `.s` assembly (charmaps, `#include`) |
| `scaninc` | scans `.s`/`.c` include dependencies |
| `gbafix` | fixes/checks the GBA cartridge header + computes Nintendo checksum |
| `bin2c` | binary blob → C array |

All live in katam `tools/` (C, built by `make_tools.mk`); pret originals:
<https://github.com/pret/pokeemerald/tree/master/tools>.

---

## 2. Toolchain & build system

### 2.1 Compiler: agbcc

- **pret/agbcc** — <https://github.com/pret/agbcc> — GCC 2.x-based old compiler
  ("C compiler") that matches what GBA-era games were built with. Standard for pret
  repos, tmc, katam, fireemblem8u.
- KATAM installs the **`new_newlib_pret` branch**: `git clone ... -b new_newlib_pret`,
  `sh build.sh && sh install.sh ../katam`
  ([INSTALL.md](https://github.com/jiangzhengwenjz/katam/blob/master/INSTALL.md)).
  This newer branch builds with newlib rather than the legacy patches; check whether KNL
  functions match better against `master` or `new_newlib_pret` agbcc (experiment on
  decomp.me first — see §4).
- `agbccpp` (a C++ fork) also exists as a decomp.me GBA compiler, and `gcc2.96` is
  available as a preset for games built with raw old GCC.

### 2.2 Assembler/linker: devkitARM binutils only

KATAM/tmc need only the binutils half of devkitARM (`arm-none-eabi-as`,
`arm-none-eabi-ld`, `arm-none-eabi-objcopy`, `arm-none-eabi-objdump`). Two equivalent
sources:

- [devkitPro devkitARM](https://devkitpro.org/wiki/Getting_Started) (`sudo (dkp-)pacman -S gba-dev`, then `DEVKITPRO`/`DEVKITARM` env vars — katam INSTALL.md);
- Debian/Ubuntu package `gcc-arm-none-eabi` + `binutils-arm-none-eabi` (what
  [tmc's Containerfile](https://github.com/zeldaret/tmc/blob/master/.devcontainer/Containerfile)
  and [pokeemerald CI](https://github.com/pret/pokeemerald/blob/master/.github/workflows/build.yml) use).

tmc's `Toolchain.mk` resolves this cleanly: `TOOLCHAIN_PATH` from `arm-none-eabi-gcc`
on `PATH`, else `$DEVKITARM/bin/`, else error. Copy this pattern so both Docker and
bare-metal devkitARM installs work.

### 2.3 ROM verification

- KATAM: `katam.sha1` + `make compare` (`COMPARE := 1` Makefile var; the check runs
  sha1 over the built ROM vs the stored hash).
- tmc: `SHA1 := sha1sum -c` against `tmc.sha1` (and per-variant `tmc_eu.sha1` etc.).
- pokeemerald CI: `COMPARE: 1` env, `make -j all syms` fails if the ROM differs.
- The GBA header (title/game code `B8KE`/`AGB KIRBY AM` in KATAM's case, set in the
  Makefile and validated by `gbafix`) is part of the match.

---

## 3. Disassembly-to-C helpers

### 3.1 m2c ("mips2c")

- Repo: <https://github.com/matt-kempster/m2c> — "A decompiler for 32-bit MIPS, **ARM**,
  PowerPC and SuperH assembly that produces C code, with partial support for C++."
- **Has a dedicated GBA target**: platforms include `arm` (ARM LE) and
  **`gba`: "ARM (with APCS, little endian)"**. Invocation:
  `python3 m2c.py --target gba-gcc-c -f <func> <asmfile>` (targets are
  `platform-compiler-language`).
- Input must be GNU `as`-format assembly — i.e. objdump output works. Context files can
  be passed with `--context` (cached as `*.m2c`).
- Online version: <https://simonsoftware.se/other/m2c.html>.
- decomp.me runs m2c server-side for GBA scratches (the platform advertises
  `has_decompiler: true` — see §4), giving you a first-pass C approximation in-browser.
- Workflow quote from the README: m2c "is often used in decompilation workflows with
  splat, asm-differ, and decomp-permuter."

### 3.2 Ghidra / IDA static analysis

- **Ghidra** (free) has built-in ARMv4T/Thumb support; the missing piece is the GBA
  memory map, provided by the **GhidraGBA** loader extension. Practical walkthroughs:
  - Starcube Labs, "Reverse Engineering a GBA Game" — <https://www.starcubelabs.com/reverse-engineering-gba>
  - wrongbaud, "A first look at Ghidra's Debugger — GBA Edition" (GhidraGBA + mGBA GDB
    stub) — <https://wrongbaud.github.io/posts/ghidra-debugger>
  - General GBA ROM RE writeup using GhidraGBA — <https://vidner.medium.com/reverse-engineer-game-boy-advance-rom-63a1fcd87bbe>
- **IDA Pro** users in GBA decomps: [jiangzhengwenjz/gba_ida_util](https://github.com/jiangzhengwenjz/gba_ida_util)
  (same author as KATAM) — IDC scripts + header for setting up GBA IDB projects
  (`idc/`, `header/` in the repo).
- **Dynamic analysis/debugging**: mGBA's GDB stub (`mgba` debugger + Ghidra/LLDB), used
  by the wrongbaud guide above. Also no$gba's debugger is popular in GBA RE circles.

### 3.3 binutils objdump flags (the GBA recipe)

From [pokeemerald's `asmdiff.sh`](https://github.com/pret/pokeemerald/blob/master/asmdiff.sh)
(identical approach in katam):

```sh
OBJDUMP="$DEVKITARM/bin/arm-none-eabi-objdump -D -bbinary -marmv4t -Mforce-thumb"

# GBA ROM is mapped at 0x08000000 on hardware
if [ $(($1)) -ge $((0x8000000)) ]; then
    OPTIONS="--adjust-vma=0x8000000 --start-address=$(($1)) --stop-address=$(($1 + $2))"
else
    OPTIONS="--start-address=$(($1)) --stop-address=$(($1 + $2))"
fi

$OBJDUMP $OPTIONS baserom.gba > baserom.dump
$OBJDUMP $OPTIONS myrom.gba   > myrom.dump
diff -u baserom.dump myrom.dump
```

- `-D -bbinary -marmv4t` — disassemble All, treat the ROM as a flat binary, ARMv4T.
- `-Mforce-thumb` — render as Thumb (most GBA game code is Thumb); drop it for ARM
  (`.arm`) functions.
- `--adjust-vma=0x8000000` — remap ROM addresses to the cart mapping so symbols line up.

### 3.4 decomp.me context ("ctx") import/export

- A decomp.me scratch takes an optional **Context** — preprocessed header soup that
  lets the compiler resolve types/globals ([decomp.me FAQ](https://www.decomp.me/faq)).
- Standard GBA way to generate it (from
  [zeldaret/tmc `genctx.sh`](https://github.com/zeldaret/tmc/blob/master/genctx.sh)):
  recursively `#include` every project header into one TU, then preprocess:
  ```sh
  echo "#include \"gba/types.h\"\n$headers" | cc -E -nostdinc -Iinclude -Itools/agbcc/include - > ctx.c
  ```
  Paste `ctx.c` into the scratch's Context field (or attach when creating via API).
- Export side: **objdiff and decomp-permuter can create/update scratches via decomp.me's
  API** (per the FAQ), so scratch round-tripping is scriptable.

---

## 4. decomp.me for GBA

### 4.1 Platform & presets

- Platform **`gba`** — "Game Boy Advance, ARMv4T, arch `arm32`, has decompiler: yes"
  ([API](https://www.decomp.me/api/compilers)).
- A **preset** = platform + compiler + flags (+ libs) for a specific project
  ([FAQ](https://www.decomp.me/faq)).
- **GBA compilers available** (from the API): `agbcc`, `old_agbcc`, `agbcc_arm`
  (ARM-mode codegen), `agbccpp`, `gcc2.96`. Exposed flag sets: `-O0..-O3`,
  `-fsigned-char`/`-funsigned-char`, `-fforce-addr`, `-gdwarf-*`; diff options include
  Levenshtein scoring, function-symbol display, and `--adjust-vma`.
  Browse existing GBA presets at <https://www.decomp.me/preset> (filter platform: GBA)
  and existing scratches by platform.

### 4.2 Workflow

1. New scratch at <https://www.decomp.me/new>: pick platform **GBA**, a preset/compiler
   (+flags), paste the **target assembly** in GNU as syntax (objdump/asm-differ output),
   and optionally the context from `genctx.sh`.
2. Hit **Decompile** — decomp.me runs **m2c** for the initial C approximation
   ([FAQ](https://www.decomp.me/faq)).
3. Iterate until the left/right assembly panes match; the page is a live compiler.
4. **Share** via the scratch URL; others can **fork** it into independent copies
   (FAQ). Scratches are listed/browsable by platform or preset.

### 4.3 Contribution flow

- Missing compiler? Compilers live in [decompme/compilers](https://github.com/decompme/compilers):
  Dockerfiles generated from jinja templates (`values.yaml`), built by GitHub Actions
  and pushed to GHCR; decomp.me pulls the images at deploy (repo README).
- New game preset: open an issue from the **compiler preset issue template** on
  decomp.me, or PR a preset (FAQ).
- An AI-assisted GBA decomp writeup using decomp.me scratches
  (<https://gambiconf.substack.com/p/development-journey-on-game-decompilation>)
  confirms the agbcc/GBA preset + scratch loop is the standard modern workflow.

---

## 5. Diff tooling (function-level and whole-ROM)

| Tool | What it does | When to use |
| --- | --- | --- |
| [pret `asmdiff.sh`](https://github.com/pret/pokeemerald/blob/master/asmdiff.sh) (also in katam/tmc) | raw `objdump -bbinary -marmv4t` diff of a ROM range between `baserom.gba` and the built ROM | quick whole-ROM / early-project checks; zero setup |
| [simonlindholm/asm-differ](https://github.com/simonlindholm/asm-differ) (`diff.py`) | colored function-level diff with scoring; **supports ARM32**; "default viewer used by decomp.me" | the day-to-day tool: `diff_settings.py` + `./diff.py -mwo func`, auto-rebuilds via make |
| [encounter/objdiff](https://github.com/encounter/objdiff) | local GUI/CLI comparing whole object files; **supports ARM (GBA, DS, 3DS)**; VS Code extension; "detailed progress reporting (powers decomp.dev)" | modern alternative; unit-level (not raw-ROM) diffs; progress metrics |
| [simonlindholm/decomp-permuter](https://github.com/simonlindholm/decomp-permuter) | "Automatically permutes C files to better match a target binary" — random + manual (`PERM_*` macros) modes; **supports MIPS, PowerPC and ARM32**; `import.py` sets up a perm dir from a makefile project; permuter@home distributes searches | when a function is *almost* matching; let it churn variants overnight |
| [WhenGryphonsFly/decomp-permuter-agbcc](https://github.com/WhenGryphonsFly/decomp-permuter-agbcc) | permuter fork tailored to agbcc/ARMv4T (upstream also ships `arm32_compile_example.sh`) | ready-made compile script for agbcc targets |

Notes:

- asm-differ usage: copy `diff.py` into the repo, write a `diff_settings.py` (arch
  `armv4t`, thumb base, objdump path from devkitARM/Docker image), run
  `./diff.py -mwo <symbol>` — `-m` re-runs make, `-w` watches, `-o` shows offsets.
- The **`.sym` symbol-file pattern**: pokeemerald's CI runs `make -j all syms` and
  commits generated `.sym` files to a dedicated `symbols` branch on every push
  ([build.yml](https://github.com/pret/pokeemerald/blob/master/.github/workflows/build.yml));
  diffs and tab-completion resolve symbols against them.
- **calcrom.pl** (pret heritage, present in katam and tmc) parses the linker map to
  report per-file code/data sizes and decomp percentages.

---

## 6. Progress tracking

| Layer | Tool | Notes |
| --- | --- | --- |
| In-repo stats | `calcrom.pl` (katam, tmc, pret) | map-file-based byte counts; zero infra |
| In-repo progress script | [`progress.py`](https://github.com/zeldaret/tmc/blob/master/progress.py) (tmc; descendant of zeldaret/oot's `tools/progress.py`) | outputs `csv`, `csv -m` (matching-only) and `shield-json` (shields.io badge JSON); runs in CI |
| Progress API | [decompals/frogress](https://github.com/decompals/frogress) — "An api for storing and retrieving decomp project progress" | projects POST periodic progress; frontends read it |
| Frontend | [decomp.dev](https://decomp.dev) (e.g. <https://decomp.dev/zeldaret/tp>) | per-unit code/data progress pages, history; "powered by objdiff" metrics |
| Badges | shields.io consuming `shield-json` or frogress endpoints | embedded in README; e.g. zeldaret READMEs show per-version badges |

No shared "progress-tooling" repo exists under decompals today (checked the org repo
list and GitHub search); the de-facto pattern is a per-project `progress.py` (copied
from zeldaret/oot / tmc) that feeds frogress + decomp.dev. ZeldaRET's own site
(<https://zelda.deco.mp>) renders the same data per game.

---

## 7. CI patterns in decomp repos

### 7.1 GitHub Actions without Docker (pret/pokeemerald)

[`.github/workflows/build.yml`](https://github.com/pret/pokeemerald/blob/master/.github/workflows/build.yml):

- `runs-on: ubuntu-latest`, env `COMPARE: 1`;
- checkout → checkout **agbcc** (`pret/agbcc`) into `agbcc/` → `apt install
  gcc-arm-none-eabi binutils-arm-none-eabi libpng-dev` → `./build.sh && ./install.sh`;
- `make -j all syms` (ROM-hash compare is part of the build) → optional modern build
  (`MODERN=1`, `COMPARE=0`);
- on push: copy `*.sym` and **auto-commit them to a `symbols` branch**
  (`EndBug/add-and-commit@v7`).

### 7.2 Jenkins + devcontainer (zeldaret/tmc)

- Real builds run on self-hosted **Jenkins** (agent label `tmc`): install agbcc, copy
  private baseroms, `make tools`, `make all -j`, then
  `python3 progress.py csv` / `csv -m` / `shield-json`, publishing CSVs to the website
  ([Jenkinsfile](https://github.com/zeldaret/tmc/blob/master/Jenkinsfile)).
- GitHub Actions is used only for **doxygen** (`doxygen.yml`) and **PR title lint**
  (`lint_pr.yml`).
- **Docker**: `.devcontainer/Containerfile` builds the whole toolchain image:

  ```dockerfile
  FROM debian
  RUN apt update && apt install -y --no-install-recommends \
      ca-certificates cmake g++ gcc gcc-arm-none-eabi git libpng-dev make \
      python3 python3-pycparser ssh-client
  RUN git clone https://github.com/pret/agbcc.git /agbcc
  RUN cd /agbcc && ./build.sh && ./install.sh /agbcc-install
  ENV AGBCC_PATH=/agbcc-install/tools/agbcc
  ```

  (This is exactly the "everything in Docker" recipe this repo's AGENTS.md requires.)

### 7.3 Docker-built compilers at scale (decomp.me)

[decompme/compilers](https://github.com/decompme/compilers): every decomp.me compiler is
a Docker image built by GitHub Actions and published to GHCR — a good model if we ever
pin a custom compiler image for CI reproducibility.

### 7.4 The checks that matter

1. **ROM hash verification** — stored `*.sha1` + `make compare`; CI fails on mismatch;
   `baserom.gba` always gitignored, never committed (katam/tmc/pret convention).
2. **Format checks** — `.clang-format` + wrapper `format.sh`
   ([tmc format.sh](https://github.com/zeldaret/tmc/blob/master/format.sh)):
   `clang-format -i -style=file`, then `clang-tidy --fix`, then final-newline fixup.
   CI can run `clang-format --dry-run -Werror` instead.
3. **Symbol checks** — `make syms` builds `.sym` files; pokeemerald commits them to a
   `symbols` branch for diff tooling.
4. **Build + compare** — the actual matching gate (`COMPARE=1`).
5. **Progress reporting** — `progress.py` → frogress/website on master pushes only.

---

## 8. Recommended pipeline for this project

**Stage 0 — bootstrap (repo skeleton).**

- Pret/KATAM-style layout: `src/`, `asm/`, `data/`, `include/`, `constants/`,
  `graphics/`, `sound/`, `tools/`, `linker.ld`, `Makefile`, `*.sha1`, `asmdiff.sh`,
  `.gitignore` with `baserom.gba`.
- `Dockerfile` derived from tmc's Containerfile (Debian + `gcc-arm-none-eabi` +
  `libpng-dev` + `python3` + agbcc built & installed), plus   `make_tools.mk` copied from
  katam's tool set (`gbafix`, `gbagfx`, `mid2agb`, `aif2pcm`, `preproc`,
  `scaninc`, `bin2c`). All builds run in Docker per AGENTS.md; no host toolchains.
- Verify the dumped KNL USA ROM sha1 at setup time (`sha1sum -c knl.sha1`).

**Stage 1 — compiler identification.**

- Before committing to a compiler, test on decomp.me: create GBA scratches with
  presets `agbcc`, `old_agbcc`, `agbcc_arm`, `gcc2.96` against a few easy KNL
  functions (using objdump-extracted target asm and a `genctx.sh`-style context).
- Default hypothesis: **agbcc `new_newlib_pret`** (what KATAM, the engine successor,
  uses). If KNL (2002, pre-KATAM toolchain) doesn't match, fall back to
  `master`-branch agbcc or `gcc2.96`, and record the winner in `AGENTS.md`.

**Stage 2 — function decomp loop.**

1. Map the ROM in Ghidra (+ GhidraGBA loader) or IDA (+ gba_ida_util); keep an IDB/Ghidra
   project outside git.
2. Extract target asm with `arm-none-eabi-objdump -D -bbinary -marmv4t
   [-Mforce-thumb] --adjust-vma=0x8000000 --start-address=... --stop-address=...`.
3. First-pass C with **m2c** (`--target gba-gcc-c`) locally or via decomp.me's
   Decompile button.
4. Open a decomp.me scratch (platform GBA, matching preset, context from `genctx.sh`);
   iterate there for collaboration, or locally with **asm-differ** (`diff.py -mwo`).
5. Near-matching functions → **decomp-permuter** (or the agbcc fork) with `import.py`.
6. Migrate matched functions from `asm/` to `src/` in the Makefile; `make compare`
   must stay green.

**Stage 3 — diff & progress infrastructure.**

- Vendor asm-differ (`diff.py` + `diff_settings.py`) in `tools/`; keep `asmdiff.sh` for
  raw ROM diffs; optionally adopt objdiff for unit-level diffs later.
- Add `calcrom.pl` immediately; add `progress.py` (tmc-style, csv + shield-json) once
  non-trivial src exists; upload to frogress and badge the README via shields.io /
  decomp.dev when ready for public tracking.

**Stage 4 — CI (GitHub Actions, Docker).**

- One workflow: checkout → build/pull the project Docker image → `make tools && make
  compare` (ROM hash gate; `baserom.gba` supplied as a CI secret/artifact, never
  committed) → `clang-format --dry-run -Werror` on `src/`+`include/` → `make syms` and
  commit `.sym` files to a `symbols` branch on push (pokeemerald pattern) → on master
  pushes only, run `progress.py` and POST to frogress.
- Copy tmc's `.clang-format` (GNU-90-ish, agbcc-compatible) as the starting style.

**Key decisions implied by the research:** no splat (GBA unsupported); pret-style
Makefile; agbcc-first compiler hypothesis validated via decomp.me; Docker image based
on tmc's Containerfile; decomp.me + m2c + asm-differ + permuter as the function
toolchain; pokeemerald-style GHA adapted to Docker.

---

## 9. Source list

- splat: <https://github.com/ethteck/splat>, wiki <https://github.com/ethteck/splat/wiki>,
  example yaml <https://github.com/ethteck/splat/blob/main/test/basic_app/splat.yaml>
- Star Rod (Paper Mario modding): <https://github.com/z64a/star-rod>
- KATAM: <https://github.com/jiangzhengwenjz/katam>, INSTALL
  <https://github.com/jiangzhengwenjz/katam/blob/master/INSTALL.md>
- zeldaret/tmc: <https://github.com/zeldaret/tmc> (Toolchain.mk, genctx.sh, format.sh,
  Jenkinsfile, progress.py, .devcontainer/Containerfile, tmc.sha1)
- pret/pokeemerald: <https://github.com/pret/pokeemerald> (asmdiff.sh,
  .github/workflows/build.yml, tools/)
- pret/agbcc: <https://github.com/pret/agbcc>
- m2c: <https://github.com/matt-kempster/m2c>, online <https://simonsoftware.se/other/m2c.html>
- decomp.me: site <https://www.decomp.me>, FAQ <https://www.decomp.me/faq>,
  compilers API <https://www.decomp.me/api/compilers>, compilers repo
  <https://github.com/decompme/compilers>
- asm-differ: <https://github.com/simonlindholm/asm-differ>
- objdiff: <https://github.com/encounter/objdiff>
- decomp-permuter: <https://github.com/simonlindholm/decomp-permuter> (USAGE.md,
  arm32_compile_example.sh); agbcc fork
  <https://github.com/WhenGryphonsFly/decomp-permuter-agbcc>
- frogress: <https://github.com/decompals/frogress>; decomp.dev <https://decomp.dev>
  (example: <https://decomp.dev/zeldaret/tp>); ZeldaRET site <https://zelda.deco.mp>
- Ghidra GBA workflow guides: <https://www.starcubelabs.com/reverse-engineering-gba>,
  <https://wrongbaud.github.io/posts/ghidra-debugger>,
  <https://vidner.medium.com/reverse-engineer-game-boy-advance-rom-63a1fcd87bbe>
- gba_ida_util: <https://github.com/jiangzhengwenjz/gba_ida_util>
- Decompedia intro (workflow context): <https://decomp.wiki/en/resources/decomp-intro>
- AI + decomp.me GBA workflow writeup:
  <https://gambiconf.substack.com/p/development-journey-on-game-decompilation>
