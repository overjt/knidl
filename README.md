# Kirby: Nightmare in Dream Land

[![CI](https://github.com/overjt/knidl/actions/workflows/build.yml/badge.svg)](https://github.com/overjt/knidl/actions/workflows/build.yml)
[![decomp.dev](https://decomp.dev/overjt/knidl?mode=shield)](https://decomp.dev/overjt/knidl)

A **matching decompilation** of *Kirby: Nightmare in Dream Land* (Game Boy
Advance, 2002), developed by HAL Laboratory and published by Nintendo.

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

## Building, verifying and testing

Docker and GNU make are the only requirements; every target below runs
inside the `knidl-builder` image (the boot test in its own `knidl-boottest`
image). Full instructions, including `baserom.gba` placement and hash
verification: [INSTALL.md](INSTALL.md).

```sh
make image      # build the knidl-builder toolchain image (Debian 12 + pinned agbcc)
make            # build knidl.gba from source + baserom.gba
make compare    # build and verify the SHA-1 (byte-for-byte match)
```

| Command | What it checks | Needs `baserom.gba` |
| --- | --- | --- |
| `make compare` | the built ROM's SHA-1 against `knidl.sha1` | yes |
| `make shifttest` | the shift test and pointer census: relinks with padding at section boundaries and proves every pointer-like word that did not move is not a pointer ([docs/data.md](docs/data.md) section 8) | yes |
| `make boottest` | the boot test: runs shifted ROMs against `knidl.gba` in mGBA, frame for frame (the first run builds the `knidl-boottest` image, also `make boottest-image`) | yes |
| `make audit` | the final audit: `.incbin` only where allowed, no unjustified raw bytes or addresses, the sanctioned asm and code-exception lists, the placeholder census ([docs/audit.md](docs/audit.md)) | no |
| `make check-data` | the data policy: `data/*.s` holds only labels, symbolic pointers and `.incbin` slices | no |
| `make check-headers` | compile-only smoke test of `include/gba/*.h` and the game headers | no |
| `make datastats` | data-structure metrics (symbolic vs raw pointer-like words) | yes |
| `make progress` | code/data/symbol percentages from the linker map | yes |
| `make symbols`, `make split`, `make modmap` | regenerate the committed symbol database, `asm/`/`data/` and the module map; a clean tree must show no diff afterwards (`git status --porcelain` empty) | yes |

## Progress

The project started as a full ROM split and was decompiled module by module
into `src/`, following the pret conventions (see [AGENTS.md](AGENTS.md) and
[docs/history.md](docs/history.md)).

<!-- Figures from `make progress` at the end of #167; refresh after any asm, naming or data change. -->

```
851196 total bytes of code
    847028 bytes of code in src (99.5103%)
    4168 bytes of code in asm (0.4897%)
        0 bytes of code remaining to be decompiled
        0 bytes in 0 functions in asm/nonmatching
        4168 bytes excluded from decompilation tracking

34017 total symbols
    8015 symbols documented (23.5617%)
    26002 symbols undocumented (76.4383%)

7537432 total bytes of data
    95640 bytes of data in src (1.2689%)
    7441560 bytes of data in data (98.7281%)
    232 bytes of data from asm (0.0031%)

7316608 bytes of data in 24725 baserom incbins (97.0703%)
```

- **Code: complete.** All of the game's code is byte-exact C: everything from
  `AgbInit` (`0x08000310`) to the sound engine's asm core, the sound engine's
  C driver and the SRAM driver. The 4,168 bytes of asm are kept by design and
  excluded from tracking: the ROM header, crt0 and the master interrupt
  handler, the ARM task switcher, the m4a engine's hand-scheduled core
  (`asm/m4a_1.s`, as in pret projects), the BIOS call thunks and `SoftReset`,
  libgcc's division routines with `_call_via_rN` and the task trampolines,
  and the interworking veneer. Each is justified in
  [docs/audit.md](docs/audit.md) section 2.
- **Code exceptions.** The C is plain C apart from the sites
  [docs/audit.md](docs/audit.md) section 3 lists: no register pin is left,
  and one function keeps zero-byte `asm("")` levers
  (`BootLogoUpdateObjects`); the rest are
  `BLOCK_CROSS_JUMP` tails, the SDK's own inline asm and documented
  zero-code stand-ins.
- **Data** (#36, closed; #167). The ROM's data is structure, not bytes:
  labeled, symbolic `data/*.s` files whose contents are `.incbin` slices of
  your own `baserom.gba`, and typed C tables in `src/data/` where a
  decompiled consumer proves the layout: since #167 every functional
  record family (the BG animation scripts, the frame tables, the RoomDef
  headers, the actor records and seg 18's handler tables). Assets are never committed
  ([docs/data.md](docs/data.md)). The shift test proves the ROM movable and
  the boot test runs it moved (see "For modders").
- **Names** (#155, open). 2,449 of the 5,348 functions and 248 of the 266
  task bodies have real names, each with its evidence in
  `docs/analysis/renames.csv` (convention: [docs/naming.md](docs/naming.md)).
  Of the 8,015 documented symbols, 5,706 have semantic names and 2,309 are
  position names (a data record named after its slot in a consumer-proven
  table). Most of the undocumented symbols are ROM data labels, 17,074 of
  them asset labels that stay unnamed by policy; the long tail (about 2,900
  `sub_*` functions, enemy and boss state bodies and one-caller helpers
  mostly) is #155's backlog. `make audit` keeps the census of what is left,
  and why, in [docs/naming.md](docs/naming.md) section 5.1.

## For modders

`make MATCHING=0` links without `linker.ld`'s per-section address asserts, so
a modified ROM builds: every section after the cartridge header is placed by
the linker right after the previous one. Moving code or data is safe as far as
it has been measured: the shift test ([docs/data.md](docs/data.md) section 8)
proves every insertion point after crt0 safe (no pointer-like word that fails
to move is a pointer or unexplained), and the boot test runs shifted images
frame for frame against the original through boot, menus, a new game and the
first stage. The data policy sets the limits: assets (graphics, audio, text,
level maps) are `.incbin` slices of your own `baserom.gba` and are never
committed, so editing them needs a build-time extraction step that does not
exist yet; functional tables are C only where a decompiled consumer proves
their layout, and stay structure-only (labels and symbolic pointers)
everywhere else.

## CI

Continuous integration (`.github/workflows/build.yml`) is designed to be
**green without a baserom**: it builds the full Docker toolchain image, syntax
checks the Python/Perl tooling, compiles `crt0` plus every file in `src/`, and
runs the checks that need no ROM (`make check-headers`, `make check-data`,
`make audit`). The ROM-dependent steps additionally run when a `baserom.gba`
is available on the runner: `make compare`, `make progress`, the regeneration
checks of `make symbols`, `make split` and `make modmap` (the committed
outputs must regenerate with no diff, `git diff --exit-code`),
`make datastats`, `make shifttest` and `make boottest`. The baserom can come
from any of these mechanisms:

- **Self-hosted runner** with `baserom.gba` placed in the runner user's home
  directory (`$HOME/baserom.gba`; files inside the workspace itself are
  removed by the checkout step, so the ROM must live outside it),
- **Actions cache** seeded by an earlier run, or
- a **`BASEROM_URL`** repository secret (optionally with `BASEROM_TOKEN`)
  pointing at a private artifact holding the USA ROM; the download is cached
  for subsequent runs.

If the baserom's SHA-1 does not match `37a476567d133c146fee6b5e2eb0b07a215da6b0`,
or the built ROM differs from it, CI **fails closed** — a mismatch can never
pass. When no baserom is available, the ROM-dependent steps are skipped
explicitly and visibly (a notice annotation and a step summary banner).

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
