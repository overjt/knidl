# INSTALL

How to set up, build and verify the *Kirby: Nightmare in Dream Land* (USA)
matching decompilation from a clean clone. Everything compiles inside Docker;
nothing is installed on your host except Docker itself.

## Requirements

| Tool | Notes |
| --- | --- |
| [Docker](https://docs.docker.com/get-docker/) | Docker Engine 20+ or Docker Desktop (Linux/macOS/Windows via WSL2) |
| GNU make | Any reasonably recent version |
| git | to clone this repository |
| ~2 GB free disk | the toolchain image is about 0.7 GB, the optional boot-test image about 0.15 GB, the clone about 70 MB and `build/` about 25 MB |

No ARM toolchain, devkitARM, or agbcc installation on your host is needed —
or wanted: matching builds must use the pinned compiler inside the image.

## 1. Clone

```sh
git clone https://github.com/overjt/knidl.git && cd knidl
```

(The repository's old name, `overjt/gba_kirby_nightmare_recomp`, redirects
here.)

## 2. Provide `baserom.gba`

You must dump **your own legitimately owned cartridge** of the USA release
(*Kirby: Nightmare in Dream Land*, USA, code `A7KE`). Copy the dump to the
repository root and rename it:

```
baserom.gba
```

Expected file (dumping hardware such as a cart dumper or a homebrew dumper on
a GBA/DS produces it directly):

| | |
| --- | --- |
| Size | 8,388,608 bytes (8 MiB) |
| SHA-1 | `37a476567d133c146fee6b5e2eb0b07a215da6b0` |

Verify before building (run from the repo root):

```sh
# Linux
echo "37a476567d133c146fee6b5e2eb0b07a215da6b0  baserom.gba" | sha1sum -c -

# macOS
echo "37a476567d133c146fee6b5e2eb0b07a215da6b0  baserom.gba" | shasum -a 1 -c -
```

A different hash means a wrong-region dump, a bad/introed scene dump, or a
corrupt file — the build will not match. See `docs/research/rom-and-bootstrap.md`
section 1 for the full list of known dumps and their hashes. **Do not** ask
for or share ROM images; `baserom.gba` is gitignored and must stay that way.

## 3. Build the toolchain image (once)

```sh
make image
```

This builds the `knidl-builder` Docker image: Debian 12 + `arm-none-eabi`
binutils + the pinned agbcc fork (`jiangzhengwenjz/agbcc` branch
`new_newlib_pret`, commit `59b966e`). It takes a few minutes the first time
(it compiles agbcc); later runs are cached. Every other `make` target runs
inside this image and depends on `make image`, so it is (re)built first when
needed.

## 4. Build and verify the ROM

```sh
make compare
```

This compiles and assembles everything inside the container, links
`knidl.gba` and checks it against `knidl.sha1`; the last lines are:

```
sha1sum -c knidl.sha1
knidl.gba: OK
```

`OK` means the built ROM is byte-for-byte identical to the target retail ROM.
The code is all built from `src/` (C) and `asm/` (the assembly that stays
assembly by design, docs/audit.md section 2); the data and the Nintendo logo
are labeled, structure-only `.incbin` slices of your own `baserom.gba`, read
at build time and never committed (docs/data.md). `make` alone builds
`knidl.gba` without the check.

## 5. Optional checks

These are the checks CI runs. None of them modifies a committed file when the
tree is in order.

```sh
# Regeneration: the committed outputs must regenerate with no diff.
make symbols      # docs/analysis/symbols.csv + callgraph.csv
make split        # asm/ and data/ from tools/split_config.json
make modmap       # docs/analysis/module-map.csv
git status --porcelain    # must print nothing

# Checks that need no baserom.gba
make check-headers   # compile-only smoke test of include/gba/*.h and the game headers
make check-data      # data/ holds only labels, symbolic pointers and .incbin slices
make audit           # the final audit (docs/audit.md)

# Reports and the movability tests (need baserom.gba)
make progress     # code/data/symbol percentages (tools/calcrom.pl)
make datastats    # data-structure metrics (docs/data.md section 6)
make shifttest    # shift test + pointer census (docs/data.md section 8)
make boottest     # boot test: shifted ROMs against knidl.gba in mGBA
```

`make boottest` runs in a second image, `knidl-boottest` (mGBA built from a
release tag, `tools/boottest/Dockerfile`). The first run builds it, which
takes a few minutes; `make boottest-image` builds it on its own. The test
plays the scripted input `tools/boottest/input.txt` (14,066 frames) on
`knidl.gba` and on one shifted ROM per insertion point, and fails at the first frame whose video, audio or
RAM differs. Never commit its output: screenshots, frame dumps and captures
are assets.

`make MATCHING=0` links without `linker.ld`'s per-section address asserts, for
a modified ROM (README.md, "For modders").

## Troubleshooting

- **`make` fails with `baserom.gba: No such file or directory`** — step 2 was
  skipped; place your dump at the repo root, named exactly `baserom.gba`.
- **`make compare` reports a SHA-1 mismatch for `baserom.gba`/`knidl.gba`** —
  your dump has the wrong hash (wrong region, bad dump). Re-dump your own
  USA cartridge; never patch the expected hash to "make it pass".
- **A source change seems to have no effect, or a clean rebuild looks
  suspicious** — run `make clean` (it removes `build/` and `knidl.gba`; check
  with `ls build`, which should fail) and then `make compare` again. Bare
  `make` on the host proves nothing about the match; `make compare` does.
- **`git status` shows changes after `make symbols`/`make split`/`make modmap`**
  — the committed outputs and the tools disagree (or your `baserom.gba` is not
  the expected dump). Do not commit the regenerated files; check the hash in
  step 2 first.
- **Docker commands hang (macOS, after the host slept)** — restart Docker
  Desktop or OrbStack, check `docker image inspect knidl-builder`, then re-run
  `make compare` before anything else.
- **Docker permission errors on Linux** — add your user to the `docker` group
  (`sudo usermod -aG docker $USER`, then re-login), or prefix the `make`
  commands with `sudo` (not recommended).
- **Windows** — use Docker Desktop with the WSL2 backend and build from inside
  a WSL2 shell (a plain `cmd`/PowerShell works too, but the paths in errors
  are clearer from WSL2).
- **CI without a baserom** — the GitHub Actions workflow is intentionally
  green without `baserom.gba` (toolchain build, compilable-object checks,
  `make check-headers`, `make check-data`, `make audit`) and skips the
  ROM-dependent steps visibly. See README.md (CI) for wiring a baserom via a
  self-hosted runner, the Actions cache, or a `BASEROM_URL` secret.
