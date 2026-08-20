# Kirby: Nightmare in Dream Land — ROM Facts & Repo Bootstrap Research

Research notes for bootstrapping a matching decompilation of **Kirby: Nightmare in Dream Land**
(GBA, 2002; JP title: *Hoshi no Kirby: Yume no Izumi Deluxe*; developer: HAL Laboratory; publisher: Nintendo).
All hashes below come from the No-Intro DAT-o-MATIC database and Gekkio's Game Boy hardware database,
cross-checked against each other.

---

## 1. Regional releases, ROM identifiers, and hashes

There are exactly **three GBA retail revisions** (one per region, chip revision `-0`). No `Rev 1`/`v1.1`
exists for any region. The other clones in No-Intro's parent/clone group are Wii U Virtual Console
releases ([x140](https://datomatic.no-intro.org/index.php?page=show_record&s=23&n=x140) USA-VC,
[x336](https://datomatic.no-intro.org/index.php?page=show_record&s=23&n=x336) EUR-VC,
[x388](https://datomatic.no-intro.org/index.php?page=show_record&s=23&n=x388) JP-VC), which are not
separate cartridge revisions for our purposes.

Retail dates ([TCRF](https://tcrf.net/Kirby:_Nightmare_in_Dream_Land), [WiKirby](https://wikirby.com/wiki/Kirby:_Nightmare_in_Dream_Land)):
Japan 2002-10-25, North America 2002-12-02, Europe 2003-09-26, Australia 2004-01-27.

### Hash table (No-Intro "Nintendo - Game Boy Advance" DAT)

| Region / version | No-Intro record | Internal code | Cart serial | Size | CRC32 | MD5 | SHA-1 | SHA-256 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| **USA (Rev 0)** | [0759](https://datomatic.no-intro.org/index.php?page=show_record&s=23&n=0759) | `A7KE` | AGB-A7KE-USA | 8,388,608 B (8 MiB) | `20ef3f64` | `35ae64b0f27e60107c14ab956f6cdf70` | `37a476567d133c146fee6b5e2eb0b07a215da6b0` | `caa4e11b6102257939297710dc4b49f6cec307de67838b2e7b7627e81b155db8` |
| **Japan (Rev 0)** — Hoshi no Kirby - Yume no Izumi Deluxe | [0666](https://datomatic.no-intro.org/index.php?page=show_record&s=23&n=0666) | `A7KJ` | AGB-A7KJ-JPN | 8,388,608 B (8 MiB) | `c55a3c2c` | `b3f6ea2a0876d9b706ef7f79a0479a32` | `d0e1d2578344e881780a71b9910562da4b123964` | `4d306911d2d7bad548aaffe13b1d30670a3fd97f9f9d760c247cd2ae3c431c36` |
| **Europe (En,Fr,De,Es,It) (Rev 0)** — parent of the No-Intro clone group | [1160](https://datomatic.no-intro.org/index.php?page=show_record&s=23&n=1160) | `A7KP` | AGB-A7KP-EUR | 16,777,216 B (16 MiB) | `3b7a7477` | `6dd162a103fb55ddf99b6e688019354f` | `39b00beee4558e6738859cfa250e4e0fcaae626e` | `c9e7ede4f74b291cd0dd1414e7579c613775c0be7836a919609f6354e3e8698a` (from [gbhwdb](https://gbhwdb.gekkio.fi/cartridges/AGB-A7KP-0/3615retro-1.html)) |

Note on internal ROM codes: the game's 4-letter header codes are **A7K\*** (A7KJ/A7KE/A7KP), *not*
"AKTE/AKTJ" — worth pinning early so scripts and docs use the right identifiers. (For comparison,
the successor *Kirby & The Amazing Mirror* uses `B8K*`, internal title `AGB KIRBY AM`, per
[Data Crystal](https://datacrystal.tcrf.net/wiki/Kirby_%26_the_Amazing_Mirror).)

### Known bad / scene dumps to avoid (No-Intro "bad" flags)

- USA introed scene dump (Mode7, 2002-11-26): CRC32 `5086113b`, SHA-1 `a3d14cfde55ec71535e46b45f6a53963fbf09fac`.
- USA bad dump (drewby, 2022): CRC32 `1e3ae4fe`, SHA-1 `c2abb5287403738e0ffa1b6dccd28137e70339d1`.
- Japan introed scene dump (Eurasia, 2002-10-23): CRC32 `7fe05a37`, SHA-1 `468ac882cadff2b0736a56fcaecaaddb6e2f8fca`; plus an older GoodTools bad JP dump CRC32 `e016866e`.
Sources: DAT-o-MATIC records 0759 / 0666 (dump sections listed per record, linked above).

### Save type and cartridge hardware

- **Emulated save type (header/GBA convention): `SRAM_V112`** — 32 KiB (256 Kbit) SRAM-class save
  mapped at `0x0E000000-0x0E00FFFF`. Listed as `Sram_v112` on all three DAT-o-MATIC records, and
  "256K SRAM (SRAM_V112)" in the FlashGBX report for the European dump.
- Physical cartridges: US/EU boards are **AGB-E11-01** with a Fujitsu **MB85R256(A) FRAM** (32 KiB,
  battery-less FRAM behaving as SRAM); Japanese carts exist in both SRAM (AGB-E06-01/-10/-20) and
  FRAM (AGB-E11) board variants — same ROM either way. Sources:
  [gbhwdb AGB-A7KP-0](https://gbhwdb.gekkio.fi/cartridges/AGB-A7KP-0/3615retro-1.html),
  [gbhwdb AGB-A7KE-0](https://gbhwdb.gekkio.fi/cartridges/AGB-A7KE-0),
  DAT-o-MATIC 0666 dump comments ("Note: FRAM board" / "Note: SRAM board").
- ROM chips: USA/Japan Macronix MX23L6406 (64 Mbit = 8 MiB); Europe MX23L12806 (128 Mbit = 16 MiB,
  doubled by the MULTI5 localization).
- Header checksum (complement byte 0xBD) of the European dump: `0xB6` (FlashGBX report). USA/Japan
  values should be read from the actual baserom at bootstrap.

### Recommended target ROM for matching

**`Kirby - Nightmare in Dream Land (USA).gba` — SHA-1 `37a476567d133c146fee6b5e2eb0b07a215da6b0`** (8 MiB).

Rationale (consistent with decomp convention and the sister project):

1. pret and ZeldaRET-style projects default to the first NTSC/US retail build
   (pokeruby → Ruby 1.0; [zeldaret/oot](https://github.com/zeldaret/oot) → build-table convention).
2. The closest sister project, the WIP **Kirby & The Amazing Mirror decomp**
   ([jiangzhengwenjz/katam](https://github.com/jiangzhengwenjz/katam)), targets the **USA** ROM
   (`katam.gba` SHA-1 `274b102b6d940f46861a92b4e65f89a51815c12c`), so tooling and symbol knowledge
   transfer best against the US build of this game.
3. The hacking scene targets the USA ROM (e.g., the
   ["A More Modern Nightmare In Dream Land" hack](https://www.romhacking.net/hacks/8289/) and
   [GameHacking.org codes](https://gamehacking.org/game/5031) are built against SHA-1 `37a4765...`),
   so cheats/diffs are directly usable during development.
4. It is half the size of the European ROM (8 MiB vs 16 MiB), which is also the earliest-shipped
   English code base (US 2002-12-02; EU 2003-09-26 is a later, recompiled MULTI5 build).

Japan (Rev 0) is the earliest code base overall (2002-10-25) and can be added later as a second
target if the project wants per-region builds (sm64/OoT style `VERSION=` switch).

---

## 2. High-level ROM structure and hacking-scene knowledge

### GBA header (first 0xC0 bytes, [GBATEK](https://problemkaputt.de/gbatek.htm#gbacartheader))

| Offset | Field | KNiDL value |
| --- | --- | --- |
| 0x00 | ARM entry point (`B` branch instruction; CPU boots in ARM state) | branch to crt0 |
| 0x04-0x9F | Nintendo logo (156 bytes, validated by BIOS) | standard |
| 0xA0 | Game title (12 bytes) | `AGB KIRBY DX` (verified on EUR dump; expected same on USA — confirm from baserom) |
| 0xAC | Game code (4 bytes) | `A7KE` (USA) / `A7KJ` (JP) / `A7KP` (EUR) |
| 0xB0 | Maker code | `01` (Nintendo) |
| 0xB3 | Fixed value | `0x96` |
| 0xBC | Software version | `0x00` (all regions, Rev 0) |
| 0xBD | Complement check | `0xB6` on EUR (compute per dump) |

Source for the string/code values: FlashGBX parsed header in the
[gbhwdb European cartridge entry](https://gbhwdb.gekkio.fi/cartridges/AGB-A7KP-0/3615retro-1.html).

### ISA (ARM vs Thumb)

- Every GBA ROM starts in **ARM state** (entry branch at 0x00); games freely mix ARM and Thumb per
  function. No public ISA-mix measurement exists for KNiDL — record one at bootstrap (e.g., count
  `bx r?`/interworking vs `arm-none-eabi-objdump -d` section stats).
- Evidence from the engine family: **katam compiles all game C in ARM mode** (`-mthumb-interwork`,
  no `-mthumb`, with `old_agbcc` used for a few files like `agb_sram.c`), so the sibling Kirby GBA
  engine is predominantly ARM. KNiDL is expected to be similar, but do not assume — measure.

### What the hacking scene has documented

- **TCRF article** ([Kirby: Nightmare in Dream Land](https://tcrf.net/Kirby:_Nightmare_in_Dream_Land)):
  the game has **unused areas, unused graphics, unused music/sounds, debugging functions, and
  regional differences**. Highlights useful for decomp naming/structure:
  - **Debug rooms** (rooms 0x149-0x153): a debug hub (room `149`) with doors to test rooms for
    slopes, breakable blocks, currents, ice/spikes, cannons, minibosses, tile surfaces. Accessed
    with a JPN GameShark code (`BE954637 6CD3E2EB` / `4BCF913D 9633BB38`).
  - **HAL Room** still exists in room `09` of stage 1-2-1 (carried over from Kirby's Adventure).
  - Unused ability placeholder icon ("fiyah" spelling), unused debug tiles **borrowed from
    Kirby's Dream Land 3's debug tileset** (including KDL3 ability-name text tiles).
  - **Regional differences**: three distinct title screens (JP/US/EU); the European version adds a
    **language selector** on the title screen, moves the copyright text, and updates
    "1993-2002" → "1993-2003". No prototype ROM dumps are known; the
    [TCRF prerelease article](https://tcrf.net/Prerelease:Kirby:_Nightmare_in_Dream_Land) covers
    E3 2002 screenshots/trailer only (early ability designs, merged Fire/Burning and Ice/Freeze
    abilities, unused Meta Knight arena tileset).
- **Data Crystal**: there is **no page for KNiDL** (404), unlike
  [Kirby & The Amazing Mirror](https://datacrystal.tcrf.net/wiki/Kirby_%26_the_Amazing_Mirror)
  (which documents internal names, per-region CRCs, and a RAM map). Expect to build RAM/ROM maps
  from scratch; the Amazing Mirror pages and katam's symbolization are the nearest references.
- **Editors/tools**: no dedicated KNiDL level editor is documented; Amazing Mirror has the
  [KatAM Object Editor](https://www.romhacking.net/utilities/1793) (object/entity placement), whose
  format work is a useful analogy for KNiDL's room/object formats. KNiDL hacks (e.g.,
  [A More Modern Nightmare](https://www.romhacking.net/hacks/8289/)) patch the USA ROM directly.
- **Disassemblies**: a complete Kirby's Dream Land (GB) disassembly exists
  ([huderlem/kirbydreamland](https://github.com/huderlem/kirbydreamland)), but **no public KNiDL
  disassembly/decomp** — this project appears to be first.

---

## 3. Repo bootstrap conventions in matching-decomp projects

Surveyed from [pret/pokeruby](https://github.com/pret/pokeruby),
[pret/pokeemerald](https://github.com/pret/pokeemerald),
[jiangzhengwenjz/katam](https://github.com/jiangzhengwenjz/katam) (closest sibling),
[n64decomp/sm64](https://github.com/n64decomp/sm64), and
[zeldaret/oot](https://github.com/zeldaret/oot).

### baserom handling

- ROM **never committed**; user supplies a legally dumped copy. pret/katam convention: user places
  it at repo root as `baserom.gba` (katam INSTALL.md, first line: "you must put a ... ROM (with
  SHA1: `274b...`) in the root directory ... and name it `baserom.gba`"). sm64 uses per-version
  `baserom.<VERSION>.z64`. `baserom.gba` goes in `.gitignore` alongside build outputs
  (`build/`, `*.gba`, tool binaries).
- **Hash verification**: `make compare` runs `sha1sum -c <rom>.sha1` (katam: `katam.sha1`; pokeemerald:
  `rom.sha1`; both Makefiles define `SHA1 := sha1sum|shasum -c`). sm64 instead hashes the baserom
  during extraction and fails with a mismatch message. Verify the **baserom** hash at extraction
  time *and* the **built ROM** hash on `make compare`.

### README & progress

- README states: project name + "disassembly/decompilation of ...", the exact built ROM(s) with
  SHA-1 (pokeruby lists `pokeruby.gba sha1: f28b...`; katam lists `katam.gba sha1: 274b...`), a link
  to INSTALL.md, and contact/Discord. OoT adds a table of all buildable versions with MD5s and a
  "work in progress" warning diff-block.
- **Badges**: CI build status (GitHub Actions/Travis), progress badge (ZeldaRET uses a
  shields.io endpoint JSON at `zelda.deco.mp`; pret computes per-category percentages with
  `calcrom.pl` — katam ships `calcrom.pl` too), contributors and Discord badges.
- **Licensing disclaimers**: pret and katam ship **no LICENSE file at all** (checked: 404), i.e.
  "all rights reserved" by default; sm64/OoT READMEs carry the disclaimer that the repo "does not
  include any of the assets necessary to build the ROM — a prior copy of the game is required."
  Adopt both: no OSS license + explicit "no ROM, no copyrighted assets; dump your own cartridge"
  statement (mirrors this repo's AGENTS.md rules).

### INSTALL/CONTRIBUTING conventions

- `INSTALL.md` (pret/katam): prerequisites table per OS, devkitPro/devkitARM setup, agbcc clone +
  `build.sh`/`install.sh`, `make -j$(nproc)`, `make compare`, `NODEP=1` tip, macOS `gmake` notes.
- pret repos have **no CONTRIBUTING.md**; contribution rules live in INSTALL docs and community
  style (katam instead has a detailed `AGENTS.md` for AI-assisted work: evidence-first naming,
  one-coherent-change PRs, matching tricks as last resort). Keep an AGENTS.md like this repo
  already does.

### Symbol & nonmatching conventions

- Naming (katam AGENTS.md, pret-derived): functions/types PascalCase (`CreateBonkers`); globals
  `g`+PascalCase (`gNumKirbys`); statics `s`+PascalCase; locals camelCase; constants
  SCREAMING_SNAKE_CASE; header guards `GUARD_<FILE>_H`; unknown struct members `unk<hexOffset>`
  (`unk3C`) / `filler<hexOffset>`; unknown params `arg0/arg1`; **unknown symbols stay
  address-derived** (`sub_08XXXXXX`, `gUnk_08XXXXXX`) until evidence justifies a name — never
  invent vague names.
- Nonmatching flow: unmatched functions live in `asm/*.s`; when a C version matches byte-for-byte
  the `.s` file is deleted and the function moves to `src/*.c` (classic pret flow; katam keeps
  `asm/`, `src/`, `data/`, `include/`, `constants/`, `graphics/`, `sound/`, `tools/`,
  `linker.ld`, `asmdiff.sh`). sm64's alternative: `NON_MATCHING=1` builds functional-but-unmatching
  C from `asm/non_matchings/`. Diffing: `asmdiff.sh` style objdump-based diffs +
  [decomp.me](https://decomp.me) scratch links in PRs.
- Toolchain for matching: **agbcc** (GCC 2.9-ish ARM fork) — katam uses
  `jiangzhengwenjz/agbcc` branch `new_newlib_pret`; per-file exceptions use `old_agbcc`
  (e.g., `agb_sram.c`, `task.c`). "modern" builds (devkitARM `arm-none-eabi-gcc`) are non-matching
  by definition. All compilation in Docker per this repo's AGENTS.md (see sm64's Docker install
  section for the bind-mount build pattern).

### Bootstrap checklist for this repo

1. [ ] `.gitignore`: `baserom.gba`, `*.gba`, `build/`, tool binaries, `.DS_Store`.
2. [ ] `README.md`: project blurb, target ROM + SHA-1 (`37a476567d133c146fee6b5e2eb0b07a215da6b0`),
       build/compare instructions, "not affiliated with Nintendo/HAL; no ROM or assets included;
       you must dump your own cartridge" disclaimer, badges (CI, progress, Discord).
3. [ ] `INSTALL.md`: baserom.gba placement + hash check, Docker toolchain instructions (per
       AGENTS.md: all builds in Docker; devkitARM + agbcc fork inside the image).
4. [ ] `Makefile`: GBA header block (`TITLE := AGB KIRBY DX`, `GAME_CODE := A7KE`, `MAKER_CODE := 01`,
       `REVISION := 0`), `COMPARE`/`NODEP`/`MODERN` flags, `SHA1 := sha1sum -c` with `knid.sha1`,
       extraction rules pulling assets/unknown data from `baserom.gba`.
5. [ ] `knid.sha1` (or equivalent) with the built-ROM SHA-1; baserom SHA-1 pinned in Makefile/INSTALL.
6. [ ] `linker.ld` mirroring the USA ROM's memory layout (derive from crt0 + first survey).
7. [ ] Directory skeleton: `src/`, `asm/`, `data/`, `include/`, `constants/`, `graphics/`, `sound/`,
       `tools/` (vendor `gbafix`-style header writer, `gbagfx`, `scaninc`/`preproc` equivalents as needed).
8. [ ] `Dockerfile` + build script (Debian + `binutils-arm-none-eabi`, `libpng-dev`, devkitARM,
       agbcc built from the pinned fork).
9. [ ] `AGENTS.md` update once build commands exist (record `make compare` etc.), following the
       katam AGENTS.md evidence/naming rules.
10. [ ] GitHub Actions CI: build + `make compare` inside the Docker image (ROM hash check will fail
        closed without a baserom, so CI builds tools/tests only or uses a cached baserom secret).
11. [ ] Progress tracking: `calcrom.pl`-style percentages; badge endpoint.
12. [ ] First technical task: dump symbols/ISA mix (arm-none-eabi-objdump), map header entry → crt0,
        locate SRAM save routine (32 KiB SRAM at 0x0E000000), and split the `Sram_v112` driver early.

### Source links

- No-Intro DAT-o-MATIC records: [USA 0759](https://datomatic.no-intro.org/index.php?page=show_record&s=23&n=0759),
  [Japan 0666](https://datomatic.no-intro.org/index.php?page=show_record&s=23&n=0666),
  [Europe 1160](https://datomatic.no-intro.org/index.php?page=show_record&s=23&n=1160);
  mirrored on [spludlow's No-Intro index](https://data.spludlow.co.uk/no-intro/no-intro/Nintendo%20-%20Game%20Boy%20Advance/).
- Gekkio gbhwdb cartridges: [AGB-A7KE-0 (USA)](https://gbhwdb.gekkio.fi/cartridges/AGB-A7KE-0),
  [AGB-A7KP-0 (EUR)](https://gbhwdb.gekkio.fi/cartridges/AGB-A7KP-0/3615retro-1.html).
- [TCRF: Kirby: Nightmare in Dream Land](https://tcrf.net/Kirby:_Nightmare_in_Dream_Land) and
  [TCRF prerelease article](https://tcrf.net/Prerelease:Kirby:_Nightmare_in_Dream_Land).
- [Data Crystal: Kirby & The Amazing Mirror](https://datacrystal.tcrf.net/wiki/Kirby_%26_the_Amazing_Mirror) (no KNiDL page exists).
- [WiKirby article](https://wikirby.com/wiki/Kirby:_Nightmare_in_Dream_Land) (dates, language select).
- Decomp conventions: [pret/pokeruby](https://github.com/pret/pokeruby) (README/INSTALL),
  [pret/pokeemerald](https://github.com/pret/pokeemerald) (`rom.sha1` compare),
  [jiangzhengwenjz/katam](https://github.com/jiangzhengwenjz/katam) (README, INSTALL.md, Makefile, AGENTS.md, calcrom.pl, asmdiff.sh),
  [n64decomp/sm64](https://github.com/n64decomp/sm64) (baserom extraction, Docker, NON_MATCHING),
  [zeldaret/oot](https://github.com/zeldaret/oot) (version table, progress badges).
- [GBATEK — GBA cartridge header](https://problemkaputt.de/gbatek.htm#gbacartheader).
