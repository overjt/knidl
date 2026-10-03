# Diffing Guide — Kirby: Nightmare in Dream Land (GBA)

This document covers both diff tools installed in the repo:

| Tool | Best for |
|------|----------|
| [`asmdiff.sh`](../asmdiff.sh) | Quick ROM-range sanity checks; zero Python dependency |
| [`tools/asm-differ/diff.py`](../tools/asm-differ/diff.py) | Day-to-day function-level work; color, scoring, watch mode |

> [!IMPORTANT]
> Neither tool requires a host ARM toolchain.  Both route `arm-none-eabi-objdump`
> through the `knidl-builder` Docker image.  Run `make image` once before using
> either tool.

---

## 1. `asmdiff.sh` — raw ROM range diff

### Syntax

```sh
./asmdiff.sh <start> <length> [--arm]
```

| Argument | Description |
|----------|-------------|
| `start`  | ROM address — decimal or hex (`0x08001234`). Pass the mapped address (≥ `0x08000000`) and `--adjust-vma` is applied automatically; pass a raw file offset for byte-only checks. |
| `length` | Byte count — decimal or hex. |
| `--arm`  | Disassemble as ARM (32-bit) instead of Thumb (default). |

### How it works

1. Runs `arm-none-eabi-objdump -D -bbinary -marmv4t [-Mforce-thumb]` on both
   `baserom.gba` and `knidl.gba` for the specified range inside Docker.
2. Pipes both outputs through `diff -u`.
3. An **empty diff** means the built ROM is byte-for-byte identical in that range
   to the base ROM — the goal for every matched section.

### Examples

```sh
# Sanity check the entire CRT0 stub (Thumb, mapped address):
./asmdiff.sh 0x080000C0 0x40

# Check an ARM-mode function:
./asmdiff.sh 0x080000C0 0x40 --arm

# Check a raw file offset (no VMA remapping):
./asmdiff.sh 0xC0 0x40

# Verify the whole ROM matches (slow, but definitive):
./asmdiff.sh 0x08000000 0x800000
```

A clean build should produce **no output** for any range (same as `make compare`).

---

## 2. `tools/asm-differ/diff.py` — function-level colored diff

### Prerequisites

The `knidl-builder` image has asm-differ's Python dependencies (the
`Dockerfile` installs `colorama`, `watchdog`, `levenshtein` and `cxxfilt`)
and the objdump it needs, so run it there.  `diff.py` and
`diff_settings.py` are both in `tools/asm-differ/`; run `diff.py` from the
**repo root**.

asm-differ compares a function's object with the same object from a
matching build, which it looks for under `expected/` (`.gitignore`d:
`expected/build/` matches the `build/` rule).  Seed it once from a
matching build:

```sh
make compare                     # knidl.gba: OK
cp -R build expected/
```

### Common invocations

```sh
# Diff a C function by name (its object is found through build/knidl.map):
docker run --rm -it -v "$PWD":/src -w /src knidl-builder \
    python3 tools/asm-differ/diff.py -o AgbMain

# Same, rebuilding before the diff and watching for changes:
docker run --rm -it -v "$PWD":/src -w /src -e INSIDE_DOCKER=1 knidl-builder \
    python3 tools/asm-differ/diff.py -mwo AgbMain

# Binary mode: baserom.gba against knidl.gba by FILE OFFSET
# (VMA - 0x08000000), e.g. AgbMain's first 0x20 bytes:
docker run --rm -it -v "$PWD":/src -w /src knidl-builder \
    python3 tools/asm-differ/diff.py 0x7300 0x7320
```

| Flag | Meaning |
|------|---------|
| `-o` | Diff the function's object against `expected/` (by symbol name). |
| `-m` | Run `make` before diffing (inside the container, with `INSIDE_DOCKER=1`). |
| `-w` | Watch mode — re-diffs automatically when source files change. |
| `--arm` | Disassemble as ARM instead of Thumb (`diff_settings.py`'s own flag). |
| `--no-pager`, `--format plain` | Print once, without colour (for logs). |

Binary mode cannot look a symbol up: asm-differ's GNU map parser wants
the overlay `load address` lines a flat GBA link does not write, so a
symbol name without `-o` fails with "Failed to find "load address" in map
file".  The asm units (crt0, the task switcher, `m4a_1`) are not in a
`.text` section, so `-o` cannot find them either; use `./asmdiff.sh`
(§1) or binary mode for those.

### Thumb vs ARM mode

The default is **Thumb**: `diff_settings.py` passes `-marmv4t
-Mforce-thumb` to objdump (asm-differ's `armel` arch).  For an
**ARM-mode** range, pass `--arm`:

```sh
python3 tools/asm-differ/diff.py --arm 0xC0 0x110
```

### How it maps onto Docker

Outside the container, `diff_settings.py` checks for `arm-none-eabi-objdump` on the host `PATH`:

- **Found** (e.g. inside the container, or devkitARM installed natively):
  `objdump_executable = "arm-none-eabi-objdump"`.
- **Not found**: falls back to `tools/asm-differ/docker-objdump.sh`, a thin
  wrapper that runs the container-side objdump via `docker run`.

The `-m` rebuild flag calls `make` on the host, which itself calls
`docker run … make INSIDE_DOCKER=1` — the full Docker chain fires automatically.

---

## 3. Workflow tips

### Typical function decomp loop

```
1.  ./asmdiff.sh 0x08XXXXXX <size>      # the target range (empty diff = matching)
2.  cp -R build expected/                # from a matching build, once
3.  Write/iterate C in src/ (or verify alone: ./tools/fnmatch.sh, docs/decomp-loop.md §3)
4.  make                                 # rebuild (Docker)
5.  python3 tools/asm-differ/diff.py -o <function>   # function diff (in the container)
6.  make compare                         # final byte-level check
```

### Checking the linker map

`build/knidl.map` is generated by `make`.  Symbol names in the map are used
by asm-differ for address → name resolution.  If a symbol isn't found, pass
its address directly.

### Quick sanity after `make compare`

```sh
./asmdiff.sh 0x08000000 0x800000
```

Should produce **zero output** — the built ROM is identical to the base ROM.
