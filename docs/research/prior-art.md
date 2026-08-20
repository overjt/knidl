# Prior Art Research: Matching Decompilation of Kirby: Nightmare in Dream Land

Research date: 2026-08-20. Scope: existing NiDL reverse-engineering efforts, the Kirby & The Amazing Mirror decompilation (`katam`), and pret (Pokémon) GBA decompilations as the canonical project template. All claims below are sourced; uncertain items are marked as such.

---

## 1. Existing Kirby: Nightmare in Dream Land (NiDL) work

**Bottom line: no public decompilation or disassembly project for NiDL exists.** Multiple targeted searches (GitHub, decomp.me, TCRF/Data Crystal, romhacking.net, GBAtemp, VG-Resource) found no repo attempting C matching or even a labeled ARM disassembly of NiDL. What exists is asset/cheat-level hacking:

### 1.1 No NiDL decompilation repo

- GitHub searches for "Nightmare in Dream Land decompilation/disassembly" surface only other Kirby projects: `huderlem/kirbydreamland` (GB, pret-style, https://github.com/huderlem/kirbydreamland), `Yoshifanatic1/Kirbys-Dream-Land-3-Disassembly` (SNES, https://github.com/Yoshifanatic1/Kirbys-Dream-Land-3-Disassembly), `AP-NJA/KRTDL-Decomp` and `ThePlayerRolo/KRTDLDecomp` (Wii), and `Kirby64Ret/kirby64` (N64). None cover NiDL.
- The curated list `CharlotteCross1998/awesome-game-decompilations` (https://github.com/CharlotteCross1998/awesome-game-decompilations) lists Kirby & The Amazing Mirror, Kirby Air Ride, Kirby 64, Epic Yarn, and Return to Dream Land — but **not** NiDL, consistent with no NiDL project existing.

### 1.2 decomp.me

- **decomp.me preset #4 is "Kirby and the Amazing Mirror" (compiler: agbcc)** with **2,422 scratches** as of this writing — a very large body of matching work for the successor game: https://decomp.me/preset/4. Example power user: `freshollie` with 1,676 scratches, e.g. `Task_SpecialStageUIMain` (https://decomp.me/u/freshollie).
- **No "Nightmare in Dream Land" preset was found** (site search + web searches). NiDL matching work on decomp.me is therefore effectively zero; we would create a preset (mirroring the KATAM preset's compiler settings, see §2.2) once the project starts.

### 1.3 TCRF (The Cutting Room Floor)

- NiDL has a well-developed TCRF page: https://tcrf.net/Kirby:_Nightmare_in_Dream_Land — documents **debug rooms / debug tileset** (largely lifted from Kirby's Dream Land 3's debug tileset), unused areas, regional differences, and unused music/sounds. A subpage https://tcrf.net/Kirby:_Nightmare_in_Dream_Land/Unused_Enemy_Palettes documents the palette-variant system, including concrete ROM offsets (e.g. palette pointer edits at ROM offset 0x255034 / 0x2563AC to force unused palettes).
- The KATAM TCRF page (https://tcrf.net/Kirby_&_the_Amazing_Mirror) repeatedly cross-references NiDL: an unused room uses "a similar tileset to Kirby: Nightmare in Dream Land's debug rooms", Kirby's unused state 0x1B "seems to be an unfinished version of the WAIT state from NiDL", and an unused inhale sound "is used in ... Kirby: Nightmare in Dream Land when Kirby successfully inhales an enemy". This is direct evidence of **engine/asset continuity between NiDL and KATAM** (see §2.7).
- Practical value for us: TCRF identifies debug menus/rooms and their RAM/ROM hooks — useful early targets for naming and for validating a decomp build.

### 1.4 Hacking tools, hacks, and codes (no source-level work)

- **MAPDELUXE**: a Chinese-language NiDL **level/map editor** (2016), circulated via Baidu cloud; a VG-Resource archive thread discusses translating its UI (https://archive.vg-resource.com/thread-29549-post-619875.html). Evidence the room/map format had community tooling, but the tool is closed-source and Windows-only.
- **KNDL-Rando** (`aquova/KNDL-Rando`, https://github.com/aquova/KNDL-Rando): randomizer that patches the US ROM's enemy-ability assignments (plus a Bizhawk Lua variant). Documents some ROM-level enemy data offsets.
- **"A More Modern Nightmare In Dream Land"** romhack (romhacking.net hack id 8289, https://www.romhacking.net/hacks/8289/; also https://neoromhacking.net/hackpage.php?id=8289): gameplay-modernization patch — binary patching only.
- Cheat databases: GameHacking.org pages for USA (https://gamehacking.org/game/5031) and Europe (https://gamehacking.org/game/5032); libretro-database CodeBreaker entries. These give quick RAM addresses (Kirby state, etc.) useful for debugging.
- Sprite ripping threads exist (e.g. GBAtemp "Kirby GBA Sprite Ripping Help", https://gbatemp.net/threads/kirby-gba-sprite-ripping-help.602366/); The Spriters Resource has a full NiDL sheet archive (https://www.spriters-resource.com/game_boy_advance/kirbynim/) — useful as graphics reference when rebuilding `graphics/`.
- **Data Crystal**: no NiDL wiki page exists (404 on https://datacrystal.tcrf.net/wiki/Kirby:_Nightmare_in_Dream_Land; searches return nothing Kirby-related).

---

## 2. Kirby & The Amazing Mirror decompilation (`katam`) — primary reference

### 2.1 Identity and status

- Repo: **https://github.com/jiangzhengwenjz/katam** — "WIP Decompilation of Kirby & The Amazing Mirror (USA)". Created 2020-01-11; still active (pushed 2026-08-09); 154 stars, 19 forks; language C; default branch `master`. Community on Discord (invite in README).
- Builds `katam.gba`, No-Intro #1692, **sha1 `274b102b6d940f46861a92b4e65f89a51815c12c`** (README + `katam.sha1`; verified against `make compare`).
- Progress level: far along. `src/` holds ~145 decompiled gameplay/engine modules (every common enemy, most bosses, Kirby, task/object/sprite systems, subgames); only ~26 `.s` files remain under `asm/` (mostly `code_080xxxxx.s`-style unidentified modules, m4a asm, libagbsyscall, crt0), and one `NONMATCHING` function. Large raw regions remain as `data/data_N.s` (e.g. `data_2.s` covers `0x08D60980`–`0x08D6DD04`, per its AGENTS.md).

### 2.2 Toolchain and compiler

- Compiler: **agbcc** (pret's GCC 2.x-based GBA compiler), installed from the fork **`jiangzhengwenjz/agbcc`, branch `new_newlib_pret`** (`git clone https://github.com/jiangzhengwenjz/agbcc -b new_newlib_pret`, then `sh build.sh && sh install.sh ../katam`, which vendors it into `tools/agbcc/`). INSTALL.md: https://github.com/jiangzhengwenjz/katam/blob/master/INSTALL.md.
- Two compiler binaries are used: default `tools/agbcc/bin/agbcc`, plus **`old_agbcc`** for special files (`src/m4a.c`, `src/agb_sram.c` — the latter also built `-O1`), mirroring pret's per-file compiler overrides.
- Standard agbcc flags: `-mthumb-interwork -Wimplicit -Wparentheses -Werror -O2 -fhex-asm -f2003-patch -ffix-debug-line`.
- Assembler/linker: **devkitARM** (arm-none-eabi binutils), `ASFLAGS := -mcpu=arm7tdmi`; on Linux, distro `binutils-arm-none-eabi` also suffices (no full devkitPRO needed for binutils).
- **No Docker**: the repo has no Dockerfile/devcontainer; builds are native (Linux/macOS/WSL/Cygwin). Our repo's Docker requirement is a local convention on top (see §4).
- A `MODERN=1` (modern GCC) path is stubbed but explicitly errors out: "Modern compilers are not supported yet in katam".

### 2.3 Build system (Makefile, verified from source)

Classic pret pipeline, adapted:

- C → ROM rule: `$(CPP) $(CPPFLAGS) $< | $(PREPROC) -i $< | $(CC1) $(CFLAGS) -o - - | cat - <(echo -e ".text\n\t.align\t2, 0") | $(AS) $(ASFLAGS) -o $@ -` — i.e. cpp → preproc (charmap) → agbcc → append `.text`/`.align 2` → gas.
- `baserom.gba` must be placed at repo root by the user (never committed; `*.gba` is gitignored in `.gitignore`). `make compare` runs the build then `sha1sum -c katam.sha1`.
- Header fixup via `tools/gbafix` with title `AGB KIRBY AM`, game code `B8KE`, maker `01`, revision `0`, language `ENGLISH`.
- `make_tools.mk` builds the vendored host tools (see §2.6); dependency scanning via `scaninc` with a `NODEP=1` escape hatch; `make syms` emits a sorted symbol file via objdump.
- **Four extra multiboot sub-ROMs** are built by separate sub-makes: `multi_boot/subgame_loaders` (incl. `speed_eaters.gba`) plus `unk_8D94B9C/unk_8E1FE28/unk_8E8490C` — the single-pak multiplayer downloads embedded in the ROM. NiDL similarly embeds multiplayer subgame images, so this machinery is directly relevant.
- Linker script `linker.ld` (47 KB, section-ordered, `SUBALIGN(4)`, IWRAM/EWRAM output sections with verified `ALIGN(16)`/`ALIGN(4)` markers).

### 2.4 Repository layout (from the git tree)

```
katam/
  AGENTS.md            # 113 KB of AI/reviewer conventions (matching discipline, naming, data conversion)
  INSTALL.md, README.md, Makefile, asmdiff.sh, calcrom.pl
  katam.sha1           # expected sha1 for `make compare`
  linker.ld
  audio_rules.mk, graphics_file_rules.mk, make_tools.mk
  src/                 # ~145 C files: engine (task, object, kirby, level, sprite_1/2, bg, palette,
  │                    #  trig, malloc_ewram, malloc_vram, save, agb_sram, m4a, m4a_tables, ...)
  │                    #  + one C file per enemy/boss/object (waddle_dee.c, gordo.c, dark_mind.c, ...)
  │                    #  + unidentified modules as code_08XXXXXX.c, multi_08XXXXXX.c
  │                    #  + src/data/room_tiled_bgs/*.c, src/data/pause_menu/*.c (generated-looking data)
  asm/                 # crt0.s, rom_header.inc, libagbsyscall.s, m4a_asm.s, macros.inc + macros/*.inc,
  │                    #  remaining un-decompiled code_08XXXXXX.s / named .s, nonmatching/*.inc
  data/                # data_1.s..data_23.s raw regions, object.s, chest.s, sound_data.s, multi_boot_images.s
  include/             # per-module headers (gordo.h, ...), functions.h, global.h, data.h,
  │                    #  constants/{kirby.h, object_types.h, songs.h, languages.h}, gba/* (pret/libagbsyscall headers)
  constants/           # .inc constants for asm (gba_constants.inc, misc_constants.inc, multi_sio_constants.inc)
  graphics/            # 213 files: png sources per category (rooms/, pause_menu/, wave_ride/, speed_eaters/, ...)
  sound/               # 447 song .s under sound/songs (+ midi/), direct_sound_samples (226),
  │                    #  programmable_wave_samples, MPlayDef.s, song_table.inc, voice_groups.inc
  multi_boot/          # 4 sub-ROM source trees (subgame_loaders + 3 unk_ ROMs)
  scripts/sound/       # SoundDumper.cs (+ python helpers: parse_song_table.py, rename_song.py, ...)
  tools/               # aif2pcm, bin2c, gbafix, gbagfx, mid2agb, preproc, scaninc (all pret-derived, vendored)
```

### 2.5 Symbol/naming conventions (from katam AGENTS.md §3.2 and code)

| Symbol kind | Style | Example |
|---|---|---|
| Functions / named types | PascalCase | `CreateBonkers`, `struct ObjectBase` |
| Global variables | `g` + PascalCase | `gNumKirbys` |
| Static variables | `s` + PascalCase | `sAreaMapRoomInfos` |
| Known locals/params | camelCase | `objBase` |
| Constants / enums | SCREAMING_SNAKE_CASE | `SE_BOSS_GROUND_POUND_ATTACK` |
| Header guards | `GUARD_<FILE>_H` | `GUARD_WADDLE_DEE_H` |
| Unknown struct member (known type) | `unk` + hex offset | `unk3C` |
| Unknown struct region | `filler` + hex offset | `filler6` |
| Unknown parameter / local | `arg0`, `sp00`, `r4` | |

Unidentified symbols stay as `sub_08XXXXXX` / `gUnk_08XXXXXX` until evidence justifies a rename; the AGENTS.md (https://github.com/jiangzhengwenjz/katam/blob/master/AGENTS.md) is a 113 KB rulebook covering: never guess names, preserve original bugs, separate binary from semantic evidence, one coherent change per PR, matching tricks as last resort, data-conversion endpoint verification, and a strict `NONMATCHING` policy.

**NONMATCHING workflow** (katam-specific, differs from pret): the default build keeps known-matching assembly; a `#ifdef NONMATCHING` branch compiles readable best-effort C instead:

```c
#ifndef NONMATCHING
NAKED bool32 sub_08153184(void) { asm(".include \"asm/nonmatching/sub_08153184.inc\""); }
#else
bool32 sub_08153184(void) { /* readable C */ }
#endif
```

Nonmatching assembly lives in `asm/nonmatching/*.inc`; the NONMATCHING build must still compile and link (validated separately from `make compare`).

### 2.6 Tools and scripts

- Vendored pret tools (each with own Makefile, built by `make_tools.mk`): **gbagfx** (png↔4bpp/8bpp/lz/rl/pal), **gbafix** (header + checksum), **mid2agb** (midi → m4a song asm), **aif2pcm** (samples), **preproc** (charmap preprocessor for C/asm strings), **scaninc** (dependency scanner), plus **bin2c** (binary → C array).
- `asmdiff.sh`: objdump-based diff of `baserom.gba` vs built ROM at a given address/length (uses `arm-none-eabi-objdump -D -bbinary -marmv4t -Mforce-thumb`).
- `calcrom.pl`: pret's progress calculator.
- `scripts/sound/`: `SoundDumper.cs` (C#) + python helpers to round-trip the m4a song table to MIDI and back.

### 2.7 Reusability for the NiDL predecessor — concrete assessment

NiDL (2002, HAL Laboratory) is KATAM's (2004) direct predecessor on the same engine family. Cross-evidence: TCRF's KATAM page documents NiDL-derived debug-room tilesets, an unfinished NiDL "WAIT" state, and NiDL inhale sounds; KATAM reuses NiDL music arrangements in its prototype (https://tcrf.net/Proto:Kirby_%26_the_Amazing_Mirror/en). (The Sonic Advance decomp `SAT-R/sa2` also asserts its engine shares DNA with the KATAM engine, crediting the same dev team — Dimps — https://github.com/SAT-R/sa2.)

**Directly reusable (knowledge/patterns, not bytes):**
- Entire pret-style build skeleton: Makefile pipeline, `make_tools.mk`, `asmdiff.sh`, `calcrom.pl`, gbafix/gbagfx/preproc/scaninc/mid2agb/aif2pcm/bin2c — copy nearly verbatim.
- agbcc toolchain choice + the `jiangzhengwenjz/agbcc@new_newlib_pret` fork with `old_agbcc` fallback for stubborn files (sound driver, SRAM).
- The **m4a/MusicPlayer sound driver** layer (`src/m4a.c`, `m4a_tables.c`, `asm/m4a_asm.s`, `sound/*.inc`, `scripts/sound/`): NiDL uses the same driver family; structure, table formats, and tooling transfer with address changes.
- Engine-core architectures and naming vocabulary: task system (`TaskCreate`, `TASK_USE_EWRAM`), object system (`struct ObjectBase`/`Object2`, per-enemy `CreateXxx` files), sprite/bg/palette managers, EWRAM/VRAM allocators, trig — KATAM's recovered structs are strong priors for NiDL's equivalents (verify per-field).
- `multi_boot` sub-ROM build machinery (NiDL embeds multiplayer subgames too).
- The katam AGENTS.md conventions (naming table, NONMATCHING policy, data-conversion rules) — our workspace conventions already derive from this style; adopt and trim.
- decomp.me workflow: reuse KATAM preset #4's compiler settings (agbcc + the flags above) for an NiDL preset.

**Not reusable / must be redone:** all addresses, symbols, linker layout (different ROM), room/level data formats (KATAM's interconnected-map metroidvania data vs NiDL's linear Kirby's-Adventure-style stages), the specific enemy/object set (NiDL's roster differs), graphics and sound assets, and any KATAM-specific systems (4-player SIO link, mirror-world map logic).

---

## 3. pret Pokémon GBA decompilations — canonical template

Repos (all verified): **pokeemerald** https://github.com/pret/pokeemerald, **pokeruby** https://github.com/pret/pokeruby, **pokefirered** https://github.com/pret/pokefirered. Umbrella: https://pret.github.io/. Status: matching-complete (pokeruby since ~2018, pokeemerald 2020, pokefirered 2021).

### 3.1 Top-level layout (verified from repo listings)

```
pokeemerald/
  Makefile  INSTALL.md  README.md  asmdiff.sh / asmdiff.ps1
  build_tools.sh  make_tools.mk  audio_rules.mk  graphics_file_rules.mk
  json_data_rules.mk  map_data_rules.mk  charmap.txt
  ld_script.ld  ld_script_modern.ld
  rom.sha1                       # expected hash(es) for `make compare`
  sym_bss.txt  sym_common.txt  sym_ewram.txt   # symbols for preproc when assembling data
  src/  asm/  data/  include/ (functions.h, global.h, constants/)  constants/ (.inc for asm)
  graphics/  sound/  tools/  libagbsyscall/  docs/  .github/workflows/build.yml (CI)
```

- pokeruby additionally shows **multi-version/multi-region support**: separate `ruby.sha1`, `sapphire.sha1`, `ruby_rev1/rev2.sha1`, `*_de*.sha1` (German) with per-version symbol files (`sym_bss_rev10.txt`, ...) and `compare-ruby.sh`/`compare-sapphire.sh`; pokefirered similarly ships rev1 and "switch" (NSO) variants. Relevant if we ever target NiDL JP (`Hoshi no Kirby: Yume no Izumi Deluxe`) or EU.
- Note on symbols: GBA pret repos use `include/functions.h` + `include/global.h` + `sym_*.txt`; the `symbols.txt`/`globals.txt` pair familiar from pret's Game Boy Color projects (pokecrystal) is **not** the GBA convention.
- `libagbsyscall/`: Nintendo SDK syscall stubs assembled separately (katam ships the same as `asm/libagbsyscall.s`).

### 3.2 baserom handling and verification workflow

- User places `baserom.gba` at the repo root; it is gitignored (never committed). `INSTALL.md` builds **agbcc** from https://github.com/pret/agbcc (`sh build.sh && sh install.sh ../pokeemerald`).
- `make` builds tools then the ROM; **`make compare`** additionally verifies the sha1 (e.g. pokeemerald `f3ae088181bf583e55daf962a92bb46f4f1d07b7`, pokeruby `f28b6ffc97847e94a6c21a63cacf633ee5c8df1e`, pokefirered `41cb23d8dccc8ebd7c649cd8fbb58eeace6e2fdc`) from `rom.sha1`. Modern (`MODERN=1`) builds use `ld_script_modern.ld` and are not expected to match.
- CI: `.github/workflows/build.yml` builds and compares on every push.

### 3.3 Nonmatchings workflow

pret GBA repos keep it simple: a function lives in `asm/*.s` until it is decompiled to **matching** C in `src/`, at which point the `.s` is deleted in the same PR — there is no long-lived nonmatching-C directory. (katam's `NONMATCHING` ifdef, §2.5, is an extension of this.) Progress is tracked by `calcrom.pl` (percentage of bytes decompiled) and community tooling; decomp.me is the standard scratch/iteration medium (2,400+ scratches exist for KATAM, §1.2).

### 3.4 Why this matters for us

pokeemerald/pokeruby/pokefirered are the longest-running, most-contributed GBA matching projects: their Makefile/pipeline shape, tool vendoring, symbol files, charmap, CI, and compare discipline are the de facto standard that katam (and we) inherit. They are the best reference for onboarding docs (`INSTALL.md` with per-OS tabs) and for handling multi-version ROMs.

---

## 4. What we should copy or reuse (recommendations)

1. **Scaffold the repo as a katam fork-in-spirit, pret in letter.** Copy the directory contract from §2.4/§3.1: `src/ asm/ data/ include/ constants/ graphics/ sound/ tools/ scripts/ multi_boot/`, root `Makefile`, `make_tools.mk`, `audio_rules.mk`, `graphics_file_rules.mk`, `asmdiff.sh`, `calcrom.pl`, `linker.ld`, `<name>.sha1`, `INSTALL.md`, `README.md`. Rename targets to our ROM (e.g. `kindl.gba`); set the real header title/game code from `baserom.gba` when available (do not guess).
2. **Toolchain**: agbcc from `jiangzhengwenjz/agbcc@new_newlib_pret` (built in Docker, installed into `tools/agbcc`), binutils via devkitARM. Keep `old_agbcc` available for driver/SRAM files. Pin versions.
3. **Docker**: neither pret nor katam ship Docker; our AGENTS.md mandates it. Base the image on the official **`devkitpro/devkitarm`** Docker image (https://hub.docker.com/r/devkitpro/devkitarm) plus `git`, `python3`, `perl`, `libpng`; build agbcc and host tools inside the image (or as a build stage). Run builds via a wrapper script so nobody compiles on the host.
4. **baserom policy**: identical to pret/katam — user-supplied `baserom.gba` at root, gitignored (`*.gba` in `.gitignore`), verified by `make compare` against `<name>.sha1` containing the USA sha1 we compute from the dump. Never commit the ROM or extracted copyrighted assets.
5. **Naming conventions**: adopt the katam table in §2.5 verbatim (PascalCase functions, `g`/`s` prefixes, `sub_08…`/`gUnk_08…` unknowns, `unkNN`/`fillerNN` members, `GUARD_*_H` guards) so that cross-referencing katam symbol discussions stays friction-free.
6. **NONMATCHING policy**: implement katam's `#ifndef NONMATCHING` + `asm/nonmatching/*.inc` pattern from day one, plus the rule that the NONMATCHING configuration must build.
7. **Reuse katam engine knowledge**: start NiDL RE by mapping the m4a driver, task/object/sprite/bg systems against katam's recovered sources (`src/m4a.c`, `task.c`, `object.c`, `sprite_1.c`/`sprite_2.c`, `bg.c`, `malloc_*.c`) as hypotheses, verifying each struct field against NiDL's own code. Port the `scripts/sound/` m4a dump/rebuild tooling.
8. **decomp.me**: request/create an NiDL preset matching KATAM preset #4's settings (agbcc, `-mthumb-interwork -O2 …`), and standardize on per-function scratches before committing to `src/`.
9. **Prior-art data sources to mine**: TCRF NiDL page (debug rooms, unused content, palette-ROM offsets), GameHacking/CodeBreaker codes (RAM map hints), KNDL-Rando (enemy data offsets), Spriters Resource (graphics reference), MAPDELUXE's existence (level-format prior art; closed-source, do not incorporate its code).
10. **Docs**: keep an `INSTALL.md` with per-OS instructions modeled on pokeemerald's, a `CONTRIBUTING`/AGENTS conventions doc derived from katam's, and CI (`.github/workflows/build.yml`) running the Docker build + `make compare`.

---

## Sources

- katam repo / README / INSTALL / Makefile / .gitignore / AGENTS.md / tree: https://github.com/jiangzhengwenjz/katam (files fetched 2026-08-20)
- jiangzhengwenjz/agbcc branch `new_newlib_pret`: https://github.com/jiangzhengwenjz/agbcc
- pret repos: https://github.com/pret/pokeemerald, https://github.com/pret/pokeruby, https://github.com/pret/pokefirered, https://pret.github.io/, https://github.com/pret/agbcc
- decomp.me KATAM preset: https://decomp.me/preset/4 ; user scratches: https://decomp.me/u/freshollie
- TCRF: https://tcrf.net/Kirby:_Nightmare_in_Dream_Land , /Unused_Enemy_Palettes , https://tcrf.net/Kirby_&_the_Amazing_Mirror , https://tcrf.net/Proto:Kirby_&_the_Amazing_Mirror/en , https://tcrf.net/Kirby's_Adventure
- NiDL hacking: https://github.com/aquova/KNDL-Rando , https://www.romhacking.net/hacks/8289/ , https://gamehacking.org/game/5031 , https://gamehacking.org/game/5032 , https://archive.vg-resource.com/thread-29549-post-619875.html (MAPDELUXE), https://gbatemp.net/threads/kirby-gba-sprite-ripping-help.602366/ , https://www.spriters-resource.com/game_boy_advance/kirbynim/
- Other Kirby decomps (non-NiDL): https://github.com/huderlem/kirbydreamland , https://github.com/Yoshifanatic1/Kirbys-Dream-Land-3-Disassembly , https://github.com/Kirby64Ret/kirby64 , https://github.com/ThePlayerRolo/KRTDLDecomp , https://github.com/CharlotteCross1998/awesome-game-decompilations
- Sonic Advance (engine note): https://github.com/SAT-R/sa2
- Docker: https://hub.docker.com/r/devkitpro/devkitarm
