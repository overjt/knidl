# Kirby: Nightmare in Dream Land

[![CI](https://github.com/overjt/knidl/actions/workflows/build.yml/badge.svg)](https://github.com/overjt/knidl/actions/workflows/build.yml)
[![decomp.dev](https://decomp.dev/overjt/knidl?mode=shield)](https://decomp.dev/overjt/knidl)

A work-in-progress **matching decompilation** of *Kirby: Nightmare in Dream Land*
(Game Boy Advance, 2002), developed by HAL Laboratory and published by Nintendo.

The goal is C/C++ source that compiles into a **byte-for-byte identical** copy of
the original ROM — not a port, rewrite, or recreation. All compilation happens
inside a pinned Docker toolchain (agbcc family), so no toolchain needs to be
installed on your machine. See [INSTALL.md](INSTALL.md) to get started.

## ROM

The repository builds the following USA retail ROM:

| | |
| --- | --- |
| Built file | `knidl.gba` (8 MiB) |
| Internal title | `AGB KIRBY DX` |
| Game code | `A7KE` (USA) |
| Maker code | `01` (Nintendo) |
| Version | `0` (Rev 0, no other retail revision exists) |
| SHA-1 | `37a476567d133c146fee6b5e2eb0b07a215da6b0` |

`make compare` verifies the built ROM byte-for-byte against this SHA-1.
You must supply your own legally dumped cartridge image as `baserom.gba`
(see [INSTALL.md](INSTALL.md)); the same hash is expected.

## Building

Docker is the only requirement:

```sh
make image    # build the knidl-builder toolchain image (Debian 12 + pinned agbcc)
make          # build knidl.gba from source + baserom.gba
make compare  # verify the built ROM SHA-1 (byte-for-byte match)
make progress # print code/data decompilation percentages
make symbols  # regenerate + validate the ROM-wide function symbol database
make split    # extract configured ROM ranges into labeled, byte-identical asm
```

Full instructions, including `baserom.gba` placement and hash verification:
[INSTALL.md](INSTALL.md).

## Progress

The project started as a full ROM split (30 address-pinned segments in
`linker.ld`) and was decompiled module by module into `src/`, following the
pret conventions (see `AGENTS.md`). **All of the game's code is now matching
C**: everything from `AgbInit` (`0x08000310`) to the sound engine's asm core
is byte-exact C, and so is the sound engine's C driver. The only code left in
assembly is kept there by design: crt0 and the ARM task switcher, the m4a
engine's hand-scheduled core (`asm/m4a_1.s`, as in pret projects), the BIOS
call thunks and the libgcc routines. Each one is justified in
`docs/analysis/rom-map.md` section 2 and excluded in `tools/calcrom.pl`.

```sh
make progress
```

```
851204 total bytes of code
    847028 bytes of code in src (99.5094%)
    4176 bytes of code in asm (0.4906%)
        0 bytes of code remaining to be decompiled
        0 bytes in 0 functions in asm/nonmatching
        4176 bytes excluded from decompilation tracking

8282 total symbols
    184 symbols documented (2.2217%)
    8098 symbols undocumented (97.7783%)

7537436 total bytes of data
    16 bytes of data in src (0.0002%)
    7531236 bytes of data in data (99.9177%)
    6184 bytes of data from asm (0.0820%)

7531388 bytes of data in 19 baserom incbins (99.9198%)
```

(Output from the current tree; run `make progress` for live values.)

What is left:

- **Data.** The ROM's data is still `.incbin`'d from `baserom.gba` at build
  time and is never committed. Issue #36 turns the structured tables into
  labeled, typed symbols without committing their contents.
- **Names.** Most functions and globals still have address-based names
  (`sub_08XXXXXX`, `gUnk_XXXXXXXX`), shown as "symbols undocumented" above.

## CI

Continuous integration (`.github/workflows/build.yml`) is designed to be
**green without a baserom**: it builds the full Docker toolchain image, syntax
checks the Python/Perl tooling, and compiles `crt0` plus every file in `src/`.
The byte-for-byte `make compare` step additionally runs when a `baserom.gba`
is available on the runner, by any of these mechanisms:

- **Self-hosted runner** with `baserom.gba` placed in the runner user's home
  directory (`$HOME/baserom.gba`; files inside the workspace itself are
  removed by the checkout step, so the ROM must live outside it),
- **Actions cache** seeded by an earlier run, or
- a **`BASEROM_URL`** repository secret (optionally with `BASEROM_TOKEN`)
  pointing at a private artifact holding the USA ROM; the download is cached
  for subsequent runs.

If the baserom's SHA-1 does not match `37a476567d133c146fee6b5e2eb0b07a215da6b0`,
or the built ROM differs from it, CI **fails closed** — a mismatch can never
pass. When no baserom is available, the compare step is skipped explicitly and
visibly (a notice annotation and a step summary banner).

A second workflow (`.github/workflows/report.yml`) generates the objdiff-schema
progress report and publishes it as the `A7KE_report` artifact for
[decomp.dev](https://decomp.dev) — see [docs/decomp-dev.md](docs/decomp-dev.md).

## Disclaimer

This project is **not affiliated with, endorsed by, or connected to** Nintendo
or HAL Laboratory. *Kirby: Nightmare in Dream Land* and its characters,
names, and assets are trademarks/copyright of Nintendo / HAL Laboratory.

No ROM image, Nintendo logo, or any other copyrighted game asset is included
in, or distributed from, this repository. The Nintendo logo used by the build
is extracted from your own `baserom.gba` at compile time. **You must dump your
own legitimately owned cartridge** to build; do not ask for or share ROMs here.

## Licensing

There is no open-source license attached to this repository: all rights are
reserved (the convention used by pret-style decompilation projects). Do not
reuse the code without explicit permission.

## See also

- [INSTALL.md](INSTALL.md) — setup and build instructions
- [katam](https://github.com/jiangzhengwenjz/katam) — matching decompilation of
  *Kirby & The Amazing Mirror* (the closest sibling project)
- [pret/pokeemerald](https://github.com/pret/pokeemerald) — the reference
  GBA decompilation project this repo's conventions follow
- `docs/research/` — research notes (ROM facts, toolchain validation, prior art)
