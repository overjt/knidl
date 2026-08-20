# Toolchain & Build Research: Kirby: Nightmare in Dream Land (GBA, 2002) Matching Decompilation

Research-only document. No code or build files were changed while producing this.
Date: 2026-08-20. All sources cited inline.

---

## 1. Executive summary

* **Compiler recommendation for NiDL:** start with **`agbcc` (GCC "2.9-arm-000512")**, the GCC-based compiler reconstructed by pret from the Pokémon GBA games and used by the Kirby & The Amazing Mirror decompilation ([jiangzhengwenjz/katam](https://github.com/jiangzhengwenjz/katam)), whose engine was built directly on NiDL's engine ([Kirby Wiki](https://kirby.fandom.com/wiki/Kirby_%26_The_Amazing_Mirror), [WiKirby](https://wikirby.com/wiki/Kirby_%26_The_Amazing_Mirror)). Keep **`old_agbcc`** available for Nintendo-SDK files (m4a / MusicPlayer2000, `agb_sram`), mirroring katam and Klonoa: Empire of Dreams ([kl-eod-decomp README](https://github.com/Dream-Atelier/kl-eod-decomp#readme)). Test a handful of simple functions on [decomp.me](https://decomp.me) before committing the toolchain.
* **Docker strategy:** single pinned Linux base image providing only host build deps + `arm-none-eabi` binutils; build the pinned agbcc fork **from source inside the image** (pret CI does exactly this on ubuntu-latest; see [pret/pokeemerald build.yml](https://github.com/pret/pokeemerald/blob/master/.github/workflows/build.yml)). This follows the repo's "all compilation in Docker" rule and is the pattern with the most community mileage.
* **Build system:** pret-style Makefile pipeline (`cpp | agbcc | arm-none-eabi-as`), GNU `ld` with a symbol-pinned `linker.ld` (explicit `MEMORY` regions, `SUBALIGN(4)`, per-symbol `. = 0x...` pins), `objcopy -O binary`, `gbafix` for the header, and `make compare` = `sha1sum -c` against a committed `<game>.sha1`. Unit-level feedback via objdiff/asm-differ/decomp.me; source fishing via decomp-permuter(-agbcc).

---

## 2. Compiler identification

### 2.1 The GBA compiler landscape of the era

Commercial GBA games (2001–2004) were typically built with one of ([GambiConf blog, "Starting a Decompilation Project from Zero"](https://gambiconf.substack.com/p/starting-a-decompilation-project), [abahbob, "Starting A Decomp Project"](https://abahbob.com/starting-a-decomp-project)):

1. **GCC-based toolchains** — the GNU ARM cross-compilers shipped with/alongside Nintendo's official AGB SDK (the "2.9-arm-000512" era snapshot, plus Cygnus `arm-elf-gcc 2.95/2.96/3.0` variants). pret's `agbcc` is a source reconstruction of this compiler family, recovered from the Pokémon GBA games ([pret/agbcc](https://github.com/pret/agbcc); `gcc/version.c` contains `char *version_string = "2.9-arm-000512";`).
2. **ARM Developer Suite (ADS) 1.1/1.2** — ARM's proprietary compiler (armcc/armasm/armlink + scatter files). ADS 1.2 was released November 2001. Community-maintained list of GBA games "probably compiled with ARM ADS 1.2": [Normmatt's gist](https://gist.github.com/Normmatt/e41587c133307fe3d0a1021374ba9e44) — it is dominated by Western-licensed titles (Dragon Ball Z Legacy of Goku, Bratz, Winx Club, GBA Video carts, etc.). **No Kirby title appears on it.**
3. **Metrowerks CodeWarrior for AGB (MWCC ARM)** — announced/released April 2002 ([Game Developer mag announcement](https://www.gamedeveloper.com/game-platforms/metrowerks-announces-codewarrior-tools-for-agb), cited by the GambiConf article). Usable within NiDL's timeframe but with no documented Japanese-first-party usage in the matching scene; decomp.me exposes MWCC presets for NDS/PS2/PSP/GC-Wii but **not for GBA** (see [decomp.me `compilers.py`](https://github.com/decompme/decomp.me/blob/main/backend/coreapp/compilers.py)).
4. **Old devkit-family GCC** (`arm-elf-gcc 2.9x`, pre-devkitPro "Devkit Advance") — e.g. Golden Sun was built with `gcc 2.96` (GS1) and `gcc 3.0` (GS2), preserved in [PascalPixel/alchemy-gcc](https://github.com/PascalPixel/alchemy-gcc).

The consensus heuristic from the community: *"The vast majority of GBA games were built using GNU... The alternative is ADS, which is not commonly used for GBA and has very little tooling"* ([abahbob](https://abahbob.com/starting-a-decomp-project)); *"many GBA games used agbcc (a fork of GCC 2.95 [era])"* ([GambiConf](https://gambiconf.substack.com/p/starting-a-decompilation-project)).

### 2.2 NiDL-specific evidence

Direct public compiler identification for NiDL does not exist yet (no public NiDL decomp project was found as of Aug 2026; nothing under GitHub topics/repos matching "Kirby Nightmare decomp"). The evidence is therefore circumstantial but strong:

| Evidence | Detail | Source |
|---|---|---|
| **Engine lineage** | KatAM's engine "was built upon Kirby: Nightmare in Dream Land's engine" (Kirby Wiki); WiKirby: KatAM has "physics, assets, and graphics borrowed from its predecessor". NiDL was developed by HAL Laboratory; KatAM produced by HAL, developed by Flagship/Dimps. | [Kirby Wiki](https://kirby.fandom.com/wiki/Kirby_%26_The_Amazing_Mirror), [WiKirby](https://wikirby.com/wiki/Kirby_%26_The_Amazing_Mirror) |
| **Successor game matches on agbcc** | The WIP KatAM decompilation builds a matching USA ROM (`sha1 274b102b...`) using a fork of pret's agbcc: `git clone https://github.com/jiangzhengwenjz/agbcc -b new_newlib_pret`. Its CFLAGS are `-mthumb-interwork -Wimplicit -Wparentheses -Werror -O$(O_LEVEL) -fhex-asm -f2003-patch -ffix-debug-line`, with `m4a.o` and `agb_sram.o` built with `old_agbcc` (Makefile lines ~120–296). | [katam INSTALL.md](https://github.com/jiangzhengwenjz/katam/blob/master/INSTALL.md), [katam Makefile](https://github.com/jiangzhengwenjz/katam/blob/master/Makefile), [zhade's agbcc fork](https://github.com/jiangzhengwenjz/agbcc/tree/new_newlib_pret) |
| **decomp.me activity** | decomp.me preset #4 is "Kirby and the Amazing Mirror" (2,422 scratches as of this writing); KatAM scratches (e.g. `katam-lava_wall-sub_0811C5D8`) are worked with the GBA agbcc-family compilers exposed by decomp.me. | [decomp.me/preset/4](https://decomp.me/preset/4), [example scratch](https://decomp.me/scratch/moVJJ) |
| **Same-family contemporaries** | Sonic Advance 2 (Dimps, 2002–03 — the studio also behind KatAM per the SA2 decomp README) builds with `tools/agbcc/bin/agbcc` + `old_agbcc` + `-fhex-asm`. Minish Cap (Capcom/Flagship, 2004) uses stock pret/agbcc. Klonoa EoD (2001) matches on an agbcc fork. | [SAT-R/sa2 Makefile](https://github.com/SAT-R/sa2), [zeldaret/tmc INSTALL.md](https://github.com/zeldaret/tmc/blob/master/INSTALL.md), [kl-eod-decomp](https://github.com/Dream-Atelier/kl-eod-decomp#readme) |
| **Not ADS** | No Kirby game in the known ADS 1.2 list; ADS/Wine tooling for GBA matching is rare and Windows-locked. | [Normmatt gist](https://gist.github.com/Normmatt/e41587c133307fe3d0a1021374ba9e44), [abahbob](https://abahbob.com/starting-a-decomp-project) |

**Caveats to keep honest:** engine reuse does not *prove* the same compiler — HAL compiled NiDL one to two years before KatAM, and a studio can change toolchains between projects (Golden Sun GS1→GS2 went 2.96→3.0). The recommendation below therefore includes a cheap empirical validation step.

### 2.3 One ROM, several compilers (plan for boundaries)

Matching GBA projects routinely need **more than one compiler configuration** in one build, because the SDK libraries were compiled separately from game code:

| Project | File(s) | Compiler | Flags |
|---|---|---|---|
| katam | most `src/*.c` | `agbcc` (zhade fork) | `-mthumb-interwork -Wimplicit -Wparentheses -Werror -O2 -fhex-asm -f2003-patch -ffix-debug-line` |
| katam | `m4a.c`, `agb_sram.c` | `old_agbcc` | (per-file overrides, Makefile ~L294–296) |
| kl-eod | `src/*.c` (except m4a) | `agbcc` (custom fork) | `-mthumb-interwork -O2 -fhex-asm -fprologue-bugfix` |
| kl-eod | `m4a.c` | `old_agbcc` | `-mthumb-interwork -O2` |
| kl-eod | `m4a_1.c` | `old_agbcc -ftst` | `-mthumb-interwork -O2 -ftst` |

Quote: *"MusicPlayer2000 (m4a.c) was originally compiled with an older GCC (old_agbcc) as part of Nintendo's GBA SDK. The two compilers differ in register allocation — for example, old_agbcc assigns the first literal pool load to r2, while agbcc assigns it to r3."* ([kl-eod README](https://github.com/Dream-Atelier/kl-eod-decomp#readme)). NiDL ships the same m4a sound driver (its soundtrack is ripped in `.minigsf` form, the m4a/"Sappy" GSF format — [JeraldDude/kirby-nightmare-in-dreamland-midi](https://github.com/JeraldDude/kirby-nightmare-in-dreamland-midi), [Zophar GSF page](https://www.zophar.net/music/gameboy-advance-gsf/kirby-nightmare-in-dream-land)), so expect the same `old_agbcc` boundary for the sound driver.

pret/agbcc's `build.sh` produces **three compiler binaries** plus `libgcc.a`/`libc.a` ([build.sh](https://github.com/pret/agbcc/blob/master/build.sh)):

* `agbcc` — GCC `2.9-arm-000512`, **Thumb** output (the GBA default; `gcc/thumb.md` machine description);
* `old_agbcc` — same tree built with `-DOLD_COMPILER`, matching the older SDK-variant codegen (used for m4a/SDK files);
* `agbcc_arm` — an **ARM-mode** variant (built from the `gcc_arm/` tree) for the rare ARM-state translation units.

### 2.4 Recommended candidates, in test order

1. **`agbcc` (GCC 2.9-arm-000512), stock pret or zhade's `new_newlib_pret` fork** — primary candidate for HAL game code. The zhade fork carries codegen patches in `gcc/thumb.c`, `gcc/thumb.md`, `gcc/regmove.c`, `gcc/reload1.c`, etc. (85 commits ahead of pret master) that were needed to match KatAM; since the engines are siblings, **fork first, stock pret agbcc second** if in doubt, or start from stock and switch when a systematic mismatch pattern appears.
2. **`old_agbcc`** — for the m4a sound driver and other SDK objects (as in katam/kl-eod).
3. **`agbcc_arm`** — only if ARM-mode C units are found (rare).
4. **Fallback experiments** (only if 1–3 systematically fail): `arm-elf-gcc 2.96` (decomp.me exposes `gcc2.96`; Golden Sun precedent), ADS 1.2 (Windows/Wine; Normmatt list methodology), MWCC ARM (no community GBA preset exists — would require original media).

**Empirical validation step (cheap, do it first):** pick 5–10 leaf functions on decomp.me (GBA platform), try the `AGBCC` preset, and check tell-tale codegen (register allocation like the r2/r3 literal-pool rule above, prologue shape, `-fhex-asm`-style hex immediates). The GambiConf article describes exactly this bootstrap: rule out compilers by date, then *"since it enabled the match for simple functions with clean C code, I stuck with it."* decomp.me's GBA compiler set is: `agbcc`, `old_agbcc`, `agbcc_arm`, `agbccpp` (C++ port), `gcc2.96` ([decomp.me compilers.py](https://github.com/decompme/decomp.me/blob/main/backend/coreapp/compilers.py) L285–316).

### 2.5 Community usage survey (what matching projects actually use)

| Project (game, year, dev) | Compiler | Source |
|---|---|---|
| pret pokeruby / pokeemerald / pokefirered / pokepinballrs (Pokémon, 2002–04, Game Freak) | agbcc / old_agbcc | [pret.github.io](https://pret.github.io/) |
| **katam** (Kirby & The Amazing Mirror, 2004, HAL/Flagship/Dimps) | agbcc fork (`new_newlib_pret`) + old_agbcc for m4a | [INSTALL.md](https://github.com/jiangzhengwenjz/katam/blob/master/INSTALL.md) |
| **sa2** (Sonic Advance 1&2, 2001–03, Dimps) | agbcc + old_agbcc, `-fhex-asm` | [SAT-R/sa2](https://github.com/SAT-R/sa2) |
| **tmc** (Zelda: Minish Cap, 2004, Capcom/Flagship) | stock pret/agbcc | [zeldaret/tmc INSTALL.md](https://github.com/zeldaret/tmc/blob/master/INSTALL.md) |
| **kl-eod** (Klonoa: Empire of Dreams, 2001, Now Production) | custom agbcc fork (`-fprologue-bugfix`, `-fhex-asm`, `-ftst`) + old_agbcc | [README](https://github.com/Dream-Atelier/kl-eod-decomp#readme) |
| **mother3** (Mother 3, 2006, Brownie Brown) | **agbccpp** — C++ port of agbcc (binary `agbcp`), [notyourav/agbcc](https://github.com/notyourav/agbcc) branch `cp` | [mother3 INSTALL.md](https://github.com/Kurausukun/mother3/blob/master/INSTALL.md), [decomp.me compilers.py](https://github.com/decompme/decomp.me/blob/main/backend/coreapp/compilers.py) |
| **goldensun-decomp / alchemy** (Golden Sun, 2001, Camelot) | gcc 2.96 (GS1), gcc 3.0 (GS2), plus agbcc parts | [PascalPixel/alchemy-gcc](https://github.com/PascalPixel/alchemy-gcc) |
| Winx Club (2004, powerDevs?) — the minority case | ARM ADS 1.2 (armasm/armlink scatter files, Wine/Windows) | [abahbob](https://abahbob.com/starting-a-decomp-project) |
| mksc-decompilation (Mario Kart: Super Circuit), hhg, hmfomt, wariowareinc, aw2bhr, rmz3, totkol, csm3, bkgr, fe7 | pret-style builds; the GBA ecosystem standardizes on GNU/agbcc-family | [GitHub topic survey](https://github.com/topics/decompilation) |

decomp.wiki's GBA page lists exactly two compilers for the platform: **agbcc** and **agbccpp** ([decomp.wiki/platforms/game-boy-advance](https://decomp.wiki/platforms/game-boy-advance)).

*(Note: the dispatch brief mentioned a "momo3" GBA decomp; no such repository could be located — the closest surveyed equivalents are the mmzret/rmz3 and XOlifreX/mksc projects above.)*

---

## 3. Reproducible builds in Docker

### 3.1 What actually needs pinning

A GBA matching build needs surprisingly little:

* **Host toolchain** (modern gcc/g++, make) — only to compile agbcc itself and the repo's C tools (gbafix, gbagfx, preproc...).
* **agbcc fork** — built from source; pinned by **git commit/branch**. This is the component that must never float.
* **`arm-none-eabi` binutils** (`as`, `ar`, `ld`, `objcopy`) — from Debian/Ubuntu's `binutils-arm-none-eabi` package or from devkitARM (`gba-dev`). katam's INSTALL offers both routes; pret CI uses the apt package and installs `gcc-arm-none-eabi` *only for the non-matching MODERN build* ([pret CI build.yml](https://github.com/pret/pokeemerald/blob/master/.github/workflows/build.yml)).
* **libpng-dev** (for gbagfx), **python3** (helper scripts), **git**.

Caveat: agbcc (GCC-2.9-era C) breaks on very new host compilers; pret tracks fixes (e.g. [issue #73 "Build failure with GCC14"](https://github.com/pret/agbcc/issues/73)) and zhade's fork carries "fix compilation on gcc 14 and 15" (commit `84d56fc8`, 2025-05-25). **Pin the base distro image tag** so host-GCC drift can't break builds, and/or use a fork with the modern-host fixes.

### 3.2 Strategy options observed in the wild

**Option A — single-stage distro image, compiler built from source (pret CI parity). Simplest; recommended.**

Pret's own CI is effectively the spec: ubuntu runner → apt install `gcc-arm-none-eabi binutils-arm-none-eabi libpng-dev` → checkout `pret/agbcc` → `./build.sh && ./install.sh ../` → `make compare`. A Docker equivalent:

```dockerfile
# Dockerfile — Option A (adapted from pret's CI + katam INSTALL prerequisites)
FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential binutils-arm-none-eabi libpng-dev \
        git python3 ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Pin the compiler fork by repo URL + commit (example: zhade's katam-tested fork)
ARG AGGCC_REPO=https://github.com/jiangzhengwenjz/agbcc
ARG AGGCC_REF=new_newlib_pret
RUN git clone "$AGGCC_REPO" /opt/agbcc && cd /opt/agbcc \
    && git checkout "$AGGCC_REF" \
    && ./build.sh && ./install.sh /opt/installed
ENV PATH=/opt/installed/tools/agbcc/bin:$PATH

WORKDIR /work
# usage: docker run -v "$PWD":/work <image> make compare -j"$(nproc)"
```

Precedents: [pret pokeemerald build.yml](https://github.com/pret/pokeemerald/blob/master/.github/workflows/build.yml) (apt set + build agbcc + `make compare`), [LuisBoto/PokedecompDocker](https://github.com/LuisBoto/PokedecompDocker) (debian:bookworm + `build-essential binutils-arm-none-eabi git libpng-dev` + clone agbcc).

**Option B — multi-stage image with prebuilt toolchain binaries (fast CI startup).**

[laqieer/gba-disasm-decomp-tools](https://github.com/laqieer/gba-disasm-decomp-tools) builds project tools *and* agbcc in a builder stage, then copies `/tools` into a slim final image (`FROM frolvlad/alpine-glibc`), setting `PATH=/usr/local/bin/agbcc/bin`. Good for agent-driven loops that spin containers up and down; slightly more maintenance than A.

**Option C — N64/zeldaret style: image provides only host deps; `make setup` compiles submoduled toolchains inside the container.**

zeldaret/oot ships `FROM ubuntu:24.04` with just `binutils-mips-linux-gnu build-essential python3 git ...`, and its Makefile runs `make setup` (compiles the IDO/GCC toolchains from pinned submodules) then the build; toolchain selection is a Makefile variable (`COMPILER ?= ido`, `ORIG_COMPILER` runs the original SGI compiler under QEMU) ([oot Dockerfile](https://github.com/zeldaret/oot/blob/main/Dockerfile), [oot Makefile](https://github.com/zeldaret/oot/blob/main/Makefile)). The GBA analogue pins the agbcc fork as a **git submodule** (kl-eod does this for both its agbcc fork and Luvdis: `tools/agbcc/  agbcc compiler (git submodule)` — [README](https://github.com/Dream-Atelier/kl-eod-decomp#readme); mother3 pins notyourav/agbcc `cp` similarly). This keeps the compiler version reviewable in every PR that changes it.

**Option D — full bit-reproducibility:** pin the base image by digest (`ubuntu:24.04@sha256:...`) on top of A or C. No surveyed GBA project does digest pinning yet; distro-tag pinning is the community norm.

### 3.3 Recommendation for this repository

Option A as the base (smallest cognitive load, exact pret-CI parity), plus the Option C convention of vendoring the compiler fork as a submodule *pinned by commit* so the Dockerfile stays dependency-only:

* Base image: `ubuntu:24.04` (or `debian:12`) by tag.
* agbcc fork cloned/submoduled at a recorded commit, built during image build (or by an idempotent `make setup` run inside the container with a named volume cache).
* devkitARM is **not** required for matching builds — the distro `binutils-arm-none-eabi` covers `as/ar/objcopy` (katam INSTALL and pret CI both support this path). Modern devkitARM's GCC only matters for the non-matching `MODERN=1` convenience build.

---

## 4. Build system, linker script, and the compare loop

### 4.1 The pret-style pipeline (what NiDL should replicate)

Canonical pipeline from pret/katam ([katam Makefile](https://github.com/jiangzhengwenjz/katam/blob/master/Makefile), [kl-eod README](https://github.com/Dream-Atelier/kl-eod-decomp#readme), [abahbob](https://abahbob.com/starting-a-decomp-project)):

1. **Compile:** `cpp -E -I tools/agbcc/include -nostdinc -iquote include ... file.c | tools/agbcc/bin/agbcc -mthumb-interwork -Wimplicit -Wparentheses -Werror -O2 -fhex-asm ... -o - - | arm-none-eabi-as -mcpu=arm7tdmi -o build/file.o` (katam Makefile L120–130, L309; decomp.me uses the identical split: `/usr/bin/cpp -E ... | agbcc $COMPILER_FLAGS | arm-none-eabi-as -mcpu=arm7tdmi` — [compilers.py L288](https://github.com/decompme/decomp.me/blob/main/backend/coreapp/compilers.py)).
2. **Link:** `arm-none-eabi-ld -T linker.ld ... -L tools/agbcc/lib -lgcc -lc` (katam L123–124).
3. **ELF→ROM:** `objcopy -O binary` then **`gbafix`** (header/game-code/ checksum fix; kl-eod sets `MAKER_CODE`/`TITLE`/`GAME_CODE` in `config.mk` — [kl-eod config.mk](https://github.com/Dream-Atelier/kl-eod-decomp/blob/main/config.mk)).
4. **Compare:** `make compare` runs `sha1sum -c <game>.sha1` and the build fails on mismatch (katam ships `katam.sha1` containing `274b102b... katam.gba`; katam's full compare validates **six** checksums — main ROM plus embedded multiboot payloads: `payload.gba`, `speed_eaters.gba`, and three unnamed ones — see [katam AGENTS.md §11.2](https://github.com/jiangzhengwenjz/katam/blob/master/AGENTS.md)). The `baserom.gba` is user-supplied and gitignored; asset extraction (gbagfx) regenerates graphics from it.

### 4.2 Linker script / symbol ordering (katam as template)

katam's `linker.ld` demonstrates the standard incremental-decomp technique ([linker.ld](https://github.com/jiangzhengwenjz/katam/blob/master/linker.ld)):

```ld
MEMORY {
    ewram (w!x) : ORIGIN = 0x02000000, LENGTH = 256K
    iwram (w!x) : ORIGIN = 0x03000000, LENGTH = 32K
    rom   (rx)  : ORIGIN = 0x08000000, LENGTH = 32M
}
SECTIONS {
    ewram (NOLOAD) : ALIGN(4) {
        gEwramHeap = 0;
        . = 0x00020080; data/sound_data.o(ewram_data);
        . = 0x00020EE0; gKirbys = .;          /* absolute symbol pins */
        ...
    }
    /* .text / .rodata ALIGN(4) : SUBALIGN(4), IWRAM with explicit ALIGN(16) markers */
}
```

Rules the katam project codified in its AGENTS.md (worth adopting wholesale):

* Preserve **exact order of input objects** in `linker.ld`; every new C file is a new input-section boundary and can shift addresses.
* `.rodata ALIGN(4) : SUBALIGN(4)` — 4-byte subalignment per input section, but the compiler/assembler still decides intra-section symbol alignment; `.align 2` in ARM GAS means 2^2 = 4 bytes (don't "fix" it to 4).
* IWRAM/EWRAM output sections use explicit `ALIGN(16)`/`ALIGN(4)` placement markers that are part of the reproduced memory map.
* Data goes in typed C when its type, boundary, mutability (`const` → `.rodata` vs writable → `.data`) and owner are proven; raw ranges stay as assembly/`incbin` until then.

### 4.3 Thumb vs ARM

* GBA game code is overwhelmingly **Thumb**: C is compiled with `-mthumb-interwork` and assembled by `arm-none-eabi-as -mcpu=arm7tdmi` (every surveyed Makefile). agbcc's default output is Thumb (`thumb.md`); the `agbcc_arm` binary exists for ARM-state translation units.
* **crt0 / startup / IWRAM-copy routines are handwritten ARM assembly** (`bx` interworking at 0x08000000, ROM header bytes).
* The **modern convenience build** (`MODERN=1`) uses real `arm-none-eabi-gcc` with `-mthumb -mthumb-interwork -mabi=apcs-gnu -mtune=arm7tdmi -march=armv4t -fno-toplevel-reorder` (katam Makefile L130) — useful for tooling and mods, never expected to match.

### 4.4 Function-level compare loop (the daily workflow)

1. **Build object diff:** [objdiff](https://github.com/encounter/objdiff/) (unit-level, used by kl-eod and modern projects) or [asm-differ](https://github.com/simonlindholm/asm-differ) (side-by-side, listed on [decomp.wiki](https://decomp.wiki/platforms/game-boy-advance)); katam also ships `asmdiff.sh`.
2. **Scratch on decomp.me:** GBA preset (for KatAM work: preset #4); compilers available: `agbcc`, `old_agbcc`, `agbcc_arm`, `agbccpp`, `gcc2.96`.
3. **Permuter:** [decomp-permuter-agbcc](https://github.com/WhenGryphonsFly/decomp-permuter-agbcc) — random source permutations to fish for matches; example config from that README: `compiler_command = "tools/agbcc/bin/agbcc -mthumb-interwork -Wimplicit -Wparentheses -Werror -O2 -fhex-asm -fprologue-bugfix -o /dev/stdout | arm-none-eabi-as -mcpu=arm7tdmi"`.
4. **Non-matching parking:** the `NONMATCHING` macro pattern — default build includes the verified original `.s` via `NAKED` + `asm(".include ...")`, while `#ifdef NONMATCHING` compiles the readable best-effort C; `make compare` always validates the matching configuration (katam AGENTS.md §7, pret convention).
5. **Progress metrics:** `calcrom.pl` (pret's per-module decompiled-byte stats, present in katam).
6. **Programmatic assist:** asmlift / m2c decompilers, LLM loops with compiler instrumentation (`-finstrument-src-locs` emitting `@ src:file.c:LINE` comments — [GambiConf article](https://gambiconf.substack.com/p/starting-a-decompilation-project)), plus the cautionary tale from the same article: an agent forking the compiler to "force" matches is a known failure mode — the oracle must only change with human-reviewed justification.

### 4.5 Verification discipline (summary for this repo's conventions)

* `make -j$(nproc) compare` is the authoritative gate (ROM sha1 equality, transitively validating baserom, assets, codegen, and linker layout).
* `NODEP=1` only as an iteration shortcut after C/asm-only edits; dependency-aware clean rebuild before trusting results (katam INSTALL/AGENTS.md).
* `NONMATCHING=1` full build must still compile and link (readability check).
* Record the target ROM early: choose the No-Intro revision (as katam does — README links the Datomatic record and pins `sha1 274b102b6d940f46861a92b4e65f89a51815c12c`), commit `<name>.sha1`, gitignore `baserom.gba` (already mandated by this repo's AGENTS.md).

---

## 5. Source list

* pret/agbcc — https://github.com/pret/agbcc (build.sh, install.sh, gcc/version.c `2.9-arm-000512`)
* zhade's compiler fork — https://github.com/jiangzhengwenjz/agbcc/tree/new_newlib_pret
* katam decompilation — https://github.com/jiangzhengwenjz/katam (README, INSTALL.md, Makefile, linker.ld, AGENTS.md, katam.sha1)
* zeldaret/tmc — https://github.com/zeldaret/tmc (INSTALL.md: agbcc + arm-none-eabi toolchain)
* Dream-Atelier/kl-eod-decomp — https://github.com/Dream-Atelier/kl-eod-decomp (README: compiler boundaries, submodule layout, objdiff/decomp.yaml)
* Kurausukun/mother3 — https://github.com/Kurausukun/mother3 (INSTALL.md: agbccpp / notyourav/agbcc `cp`)
* PascalPixel/alchemy-gcc — https://github.com/PascalPixel/alchemy-gcc (Golden Sun gcc 2.96/3.0)
* SAT-R/sa2 — https://github.com/SAT-R/sa2 (Makefile: agbcc/old_agbcc)
* pret/pokeemerald CI — https://github.com/pret/pokeemerald/blob/master/.github/workflows/build.yml
* decomp.me — https://decomp.me (preset/4 "Kirby and the Amazing Mirror"; backend compiler list: https://github.com/decompme/decomp.me/blob/main/backend/coreapp/compilers.py)
* decomp.wiki GBA page — https://decomp.wiki/platforms/game-boy-advance
* Normmatt, "GBA Games compiled with ADS" — https://gist.github.com/Normmatt/e41587c133307fe3d0a1021374ba9e44
* abahbob, "Starting A Decomp Project" (Winx Club / ADS 1.2, pret pipeline anatomy) — https://abahbob.com/starting-a-decomp-project
* Macabeus, "Starting a Decompilation Project from Zero" (GambiConf blog; compiler dating, agbcc fork flags, objdiff/splat-style workflow) — https://gambiconf.substack.com/p/starting-a-decompilation-project
* Kirby Wiki / WiKirby on KatAM↔NiDL engine lineage — https://kirby.fandom.com/wiki/Kirby_%26_The_Amazing_Mirror , https://wikirby.com/wiki/Kirby_%26_The_Amazing_Mirror , https://wikirby.com/wiki/Kirby:_Nightmare_in_Dream_Land
* laqieer/gba-disasm-decomp-tools (multi-stage Docker) — https://github.com/laqieer/gba-disasm-decomp-tools
* LuisBoto/PokedecompDocker — https://github.com/LuisBoto/PokedecompDocker
* zeldaret/oot Dockerfile/Makefile (N64-style pinned setup) — https://github.com/zeldaret/oot
* WhenGryphonsFly/decomp-permuter-agbcc — https://github.com/WhenGryphonsFly/decomp-permuter-agbcc
* encounter/objdiff — https://github.com/encounter/objdiff ; simonlindholm/asm-differ — https://github.com/simonlindholm/asm-differ
