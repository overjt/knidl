# Compiler validation — agbcc vs old_agbcc vs gcc2.96

Empirical validation of the matching compiler for KNiDL (issue #7), using the
leaf-function candidates from `docs/analysis/rom-map.md` §5. All local codegen
runs used the pinned toolchain from the `knidl-builder` Docker image
(`jiangzhengwenjz/agbcc@new_newlib_pret`, commit `59b966e`); all remote runs
used decomp.me GBA presets. Date: 2026-08-20.

## 1. Method

1. Disassembled 6 Thumb leaf candidates from `baserom.gba` (candidates 1-7, 12
   of the rom-map table; `agbcc_arm` was not exercised on the Thumb leaves —
   the ROM has no ARM-mode C candidates outside crt0/task units, see §5).
2. First-pass C with m2c (`m2c.py --target gba-gcc-c`), then hand-iteration
   against the ROM bytes (struct/array shapes for pool-offset stores, extern
   symbols for `0x03xxxxxx` bases so the pool word is a symbol reference).
3. Local byte-exact harness (inside Docker): `cpp -P` → `agbcc|old_agbcc
   -O? -mthumb-interwork` → `as` → `objcopy -O binary -j .text`, then `cmp`
   against the `baserom.gba` slice. The `bl` word is masked on both sides for
   `fn_08001A34` (the only candidate with a call).
4. decomp.me scratches (platform GBA) for the three candidate presets with the
   same C sources; scores below are as reported by decomp.me.

Literal-pool corrections found during decode (halfword-order slips in the
rom-map candidate table, now fixed there): `0x080055B0` base is `0x03002790`
(not `0x03009027`); `0x08009AA0` clears byte `[0x02006030]` and word
`[0x03000014]` (not `0x02003060`/`0x03001400`).

## 2. Functions and C sources

| Entry | Size | Shape |
|---|---|---|
| `0x080022A0` | 0x0C | single literal store `*(s32 *)0x03000B00 = 0` |
| `0x080055B0` | 0x14 | array-of-0x90 store: `gArr[arg1].unk13 = arg0`, `gArr = 0x03002790` |
| `0x08009AA0` | 0x18 | two global clears (byte `0x02006030`, word `0x03000014`) |
| `0x08001488` | 0x34 | two masked IO halfword RMWs + `irqTable->handler = 0x08001519` (struct member at +0xC) |
| `0x08010020` | 0x28 | four `strb` setters to globals |
| `0x08001A34` | 0x50 | globals init + `CpuFastSet` call + volatile halfword copy |

Key source-shape findings (m2c first pass → matching form):

- Struct member at constant offset (`str r0,[r1,#12]`, `strb r0,[r2,#19]`)
  must be written as a struct field access, not folded pointer arithmetic —
  agbcc folds `+const` into the pool otherwise (`0x030027A3` instead of
  `0x03002790` with `strb [r2,#19]`).
- The array-index store `0x080055B0` matches with `extern struct Unk gArr[]`
  (stride 0x90) + defsym `gArr=0x03002790`; the pool then holds the symbol
  value and scheduling keeps the `ldr` first.
- The `0x08001A34` epilogue `strh r4,[r0]; ldrh r0,[r0]; strh r0,[r1]`
  matches a chained assignment `*(vu16 *)0x03000B1C = (*(vu16 *)0x03001A80 = 0);`.

## 3. Local byte-exact matrix (Docker, pinned fork `59b966e`)

`MATCH` = assembled `.text` byte-identical to the `baserom.gba` slice
(`bl` masked for `fn_08001A34`).

| Function | agbcc -O1 | agbcc -O2 | agbcc -O3 | old_agbcc -O1 | old_agbcc -O2 | old_agbcc -O3 |
|---|---|---|---|---|---|---|
| `fn_08010020` | MATCH | MATCH | MATCH | MATCH | MATCH | MATCH |
| `fn_08009AA0` | MATCH | MATCH | MATCH | MATCH | MATCH | MATCH |
| `fn_080055B0` | MATCH | MATCH | MATCH | MATCH | MATCH | MATCH |
| `fn_08001A34` | mismatch | MATCH | MATCH | mismatch | MATCH | MATCH |
| `fn_08001488` | mismatch | **MATCH** | **MATCH** | mismatch | mismatch | mismatch |
| `fn_080022A0` | ~ | ~ | ~ | ~ | ~ | ~ (96%, see §6) |

`-fsigned-char` vs default (unsigned char): no difference on any candidate
(none of the leaves does char sign-extension), as expected for GCC 2.x ARM
where `char` is unsigned by default in both presets.

## 4. decomp.me scratches

| Function | Preset | Flags | Score | URL |
|---|---|---|---|---|
| `fn_08009AA0` | agbcc | `-O2 -mthumb-interwork` | **0**/1000 | https://decomp.me/scratch/uGyFe |
| `fn_080022A0` | agbcc | `-O2 -mthumb-interwork` | 20/500 (96%) | https://decomp.me/scratch/3zIvR |
| `fn_080022A0` | old_agbcc | `-O2 -mthumb-interwork` | 20/500 (96%) | https://decomp.me/scratch/foRWS |
| `fn_080022A0` | gcc2.96 | `-O2 -mthumb-interwork` | 615/500 | https://decomp.me/scratch/R4ORq |
| `fn_080055B0` | agbcc | `-O2 -mthumb-interwork` | **0**/900 | https://decomp.me/scratch/MeWtR |
| `fn_080055B0` | old_agbcc | `-O2 -mthumb-interwork` | **0**/900 | https://decomp.me/scratch/n7cA0 |
| `fn_080055B0` | gcc2.96 | `-O2 -mthumb-interwork` | 720/900 | https://decomp.me/scratch/IqpxU |
| `fn_08001488` | agbcc | `-O2 -mthumb-interwork` | **0**/2000 | https://decomp.me/scratch/fyMiH |
| `fn_08001488` | old_agbcc | `-O2 -mthumb-interwork` | 310/2000 | https://decomp.me/scratch/fXLKb |
| `fn_08001488` | gcc2.96 | `-O2 -mthumb-interwork` | 3505/2000 | https://decomp.me/scratch/605SF |
| `fn_08010020` | agbcc | `-O2 -mthumb-interwork` | **0**/1600 | https://decomp.me/scratch/uV0jZ |
| `fn_08001A34` | agbcc | `-O2 -mthumb-interwork` | **0**/3200 | https://decomp.me/scratch/49p5I |
| `fn_08001A34` | gcc2.96 | `-O2 -mthumb-interwork` | 5905/3200 | https://decomp.me/scratch/a03oL |

## 5. Conclusion

**The project compiler is `agbcc` (the `new_newlib_pret` fork) with
`-O2 -mthumb-interwork`.**

Evidence:

- 5 of 6 leaves are byte-identical (local) and 0-score (decomp.me) under
  `agbcc -O2 -mthumb-interwork`. Acceptance criterion met.
- `fn_08001488` is the discriminating function: it matches only new `agbcc`
  (-O2/-O3); `old_agbcc` fails it in every configuration (extra/other literal
  scheduling around the two halfword RMWs). So despite KNiDL shipping in 2002
  (before KATAM), its game code was built with the new-agbcc codegen, not the
  old one.
- `gcc2.96` is ruled out everywhere (large diffs on every leaf).
- `old_agbcc` remains plausible for the SDK library units (`0x080CF9xx` zone:
  m4a/agb_sram family) following KATAM's per-file layout; to be confirmed when
  those units are decompiled.
- Recommended default CFLAGS (KATAM-parity, fork flags included):
  `-O2 -mthumb-interwork -Wimplicit -Wparentheses -Werror -fhex-asm -f2003-patch -ffix-debug-line`.
  `-fhex-asm`/`-ffix-debug-line` are cosmetic (asm hex syntax, debug lines);
  `-f2003-patch` was verified not to change codegen on the leaves.

ARM-mode units (crt0/ISR/task switch, `0x080000C0-0x080002F0`) stay in
`asm/` for now; `agbcc_arm` will be validated when a C-shaped ARM unit is
found (none exists today — the ROM's only ARM code outside crt0 is 3 tiny
task-switch helpers).

## 6. Open nuance: `fn_080022A0`

The 20/500 residue is purely register allocation: target uses
`ldr r2 / movs r1 / str r1,[r2]`, every natural C form tried (global extern,
cast store, params, chained, register local, const-pointer, duplicate stores;
all opt levels; both compilers; fork flags) yields `ldr r1 / movs r0 /
str r0,[r1]`. Instruction sequence, order and pool are identical. This is a
source-shape question for the decomp phase (possibly an aggregate write or an
inlined helper shape), not a compiler question — both agbcc-family compilers
produce the same 96% result and gcc2.96 is far away.
