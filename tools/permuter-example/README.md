# decomp-permuter scratch example (ReadSram_Core, SDK zone)

Working smoke-test/example directory for `tools/decomp-permuter`, wired to this
repo's toolchain. It proves the permuter runs inside the `knidl-builder`
Docker image and documents every file a per-function directory needs.

## Files

| File | Purpose |
|------|---------|
| `target.s` | Exact ROM assembly of the target function (`ReadSram_Core`, `0x080CFA9C`, 0x24 bytes incl. trailing pad halfword), hand-converted from objdump to unified-syntax gas. Assembled to `target.o`, it is the scoring reference. Byte-equality against `baserom.gba` is asserted by the run below. |
| `base.c` | A plausible but NON-matching first-pass C shape (score > 0) — the starting point a permuter run improves on. |
| `base.matching-reference.c` | The known-good source shape; scores **0**. Swap it in to sanity-check the whole pipeline deterministically. |
| `compile.sh` | Compiler pipeline in the permuter's contract (`$1 = input.c`, `$3 = output.o`). Mirrors the Makefile recipe for this zone: `cpp -P \| old_agbcc -O1 -mthumb-interwork \| append .text/.align \| as`. **Must keep `set -o pipefail`** — without it a crashed compiler masked by a succeeding assembler yields false zero scores. |
| `settings.toml` | `func_name` + `compiler_type = "gcc"` + `[weight_overrides]` tuned for old_agbcc (disables modern-GCC-oriented passes that mostly fail to compile under GCC 2.9). Do NOT set `objdump_command`: the scorer auto-detects ARM from the ELF header and adds its own flags. |

## Run it (no baserom needed for scoring)

```sh
mkdir -p build/scratch
cp -r tools/permuter-example build/scratch/fn && chmod +x build/scratch/fn/compile.sh
docker run --rm -v "$(pwd):/src" -w /src knidl-builder bash -c '
  cd build/scratch/fn &&
  arm-none-eabi-as -mcpu=arm7tdmi -o target.o target.s &&
  python3 /src/tools/decomp-permuter/permuter.py . --debug'
```

With `cp base.matching-reference.c base.c` first, this prints
`[ReadSram_Core] base score = 0`. With the shipped non-matching `base.c`,
it prints a nonzero score and the side-by-side diff.

## Permute

```sh
docker run --rm -v "$(pwd):/src" -w /src knidl-builder bash -c '
  cd build/scratch/fn &&
  timeout 300 python3 /src/tools/decomp-permuter/permuter.py . -j 4 --seed 1'
```

Improvements are written to `output-<score>-<n>/source.c`. See
`docs/decomp-loop.md` for where this fits in the full workflow.
