# INSTALL

How to set up and build the *Kirby: Nightmare in Dream Land* (USA) matching
decompilation. Everything compiles inside Docker; nothing is installed on
your host except Docker itself.

## Requirements

| Tool | Notes |
| --- | --- |
| [Docker](https://docs.docker.com/get-docker/) | Docker Engine 20+ or Docker Desktop (Linux/macOS/Windows via WSL2) |
| GNU make | Any reasonably recent version |
| git | to clone this repository |
| ~2 GB free disk | for the toolchain image and build tree |

No ARM toolchain, devkitARM, or agbcc installation on your host is needed —
or wanted: matching builds must use the pinned compiler inside the image.

## 1. Clone

```sh
git clone https://github.com/overjt/gba_kirby_nightmare_recomp.git
cd gba_kirby_nightmare_recomp
```

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
`new_newlib_pret`, commit `59b966e`). It takes a few minutes the first time;
later runs are cached.

## 4. Build the ROM

```sh
make
```

This compiles/assembles everything inside the container and produces `knidl.gba`
(the header is written from source; the Nintendo logo and not-yet-decompiled
segments are `.incbin`'d from your `baserom.gba` at build time).

## 5. Verify the match

```sh
make compare
```

Builds if needed, then checks the built ROM against `knidl.sha1`:

```
37a476567d133c146fee6b5e2eb0b07a215da6b0  knidl.gba
knidl.gba: OK
```

`OK` means the built ROM is byte-for-byte identical to the target retail ROM.

## 6. Check decompilation progress

```sh
make progress
```

Parses `build/knidl.map` (via `tools/calcrom.pl`, vendored from katam/pret)
and prints code/data byte counts and percentages, plus how much data is still
coming straight from `baserom.gba` via `.incbin`.

## Troubleshooting

- **`make` fails with `baserom.gba: No such file or directory`** — step 2 was
  skipped; place your dump at the repo root, named exactly `baserom.gba`.
- **`make compare` reports a SHA-1 mismatch for `baserom.gba`/`knidl.gba`** —
  your dump has the wrong hash (wrong region, bad dump). Re-dump your own
  USA cartridge; never patch the expected hash to "make it pass".
- **Docker permission errors on Linux** — add your user to the `docker` group
  (`sudo usermod -aG docker $USER`, then re-login), or prefix the `make`
  commands with `sudo` (not recommended).
- **Windows** — use Docker Desktop with the WSL2 backend and build from inside
  a WSL2 shell (a plain `cmd`/PowerShell works too, but the paths in errors
  are clearer from WSL2).
- **CI without a baserom** — the GitHub Actions workflow is intentionally
  green without `baserom.gba` (toolchain build + compilable-object checks) and
  skips `make compare` visibly. See README.md (CI) for wiring a baserom via a
  self-hosted runner, the Actions cache, or a `BASEROM_URL` secret.
