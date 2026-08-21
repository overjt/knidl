# Header conventions — `include/gba/` (issue #27)

One page of rules so later agents extend the platform headers the same way
instead of inventing conflicting definitions. Read this before adding
anything to `include/gba/`.

## Layout

| File | Contents |
|---|---|
| `gba/types.h` | `u8..u64`, `s8..s64`, `vu8..vu64`, `vs8..vs64`, `f32/f64`, `bool8/16/32` |
| `gba/defines.h` | memory-map constants, `IWRAM_DATA`/`EWRAM_DATA`, `INTR_CHECK`/`INTR_VECTOR`, display constants, `RGB` |
| `gba/io_reg.h` | the full I/O register map (`0x04000000` block) + register field/bit macros |
| `gba/interrupts.h` | `INTR_FLAG_*` interrupt IDs + master-ISR dispatch order notes |
| `gba/syscall.h` | SWI numbers (SDK order, see below) + prototypes for the ROM's thunks |
| `gba/agb_sram.h` | SRAM driver prototypes (`src/agb_sram.c`) |
| `gba/gba.h` | umbrella including all of the above |
| `global.h` (repo root include/) | game-level helpers: `ARRAY_COUNT`, `min/max/abs`, `BLOCK_CROSS_JUMP`, `asm_comment` |

New `gba/*.h` headers must be added to `gba/gba.h` and included (and used)
by `tools/header_smoke.c`, then verified with `make check-headers`.

## C dialect (GCC 2.x safety)

- C89/C90 only, plus the GCC 2.x extensions already in use
  (`__attribute__((...))`, inline `asm` strings in `global.h`).
- No C99: no `//` comments, no mixed declarations/statements, no
  `<stdint.h>` type usage beyond what `gba/types.h` already wraps (the
  pinned agbcc fork's newlib ships `stdint.h`, which is why it may be
  included there — and nowhere else).
- `long long` works on both validated compilers (verified: `u64` in
  `gba/types.h`, used by `REG_SIOMLT_RECV`), but prefer 32-bit types
  in new code — the ROM's ABI is 32-bit.

## Naming

- Registers: `REG_OFFSET_<NAME>` (offset from `REG_BASE`), `REG_ADDR_<NAME>`
  (absolute address), `REG_<NAME>` (volatile lvalue of the natural width).
  Split 32-bit registers additionally get `_L`/`_H` 16-bit aliases
  (e.g. `REG_BG2X_L`, `REG_DMA3CNT_H`).
- Register fields: `<REG>_<FIELD>` object macros (`DISPCNT_OBJ_ON`,
  `TIMER_64CLK`, `WAITCNT_SRAM_8`), values encoded in the macro name
  (`WAITCNT_WS0_N_3`, `BLDCNT_EFFECT_BLEND`).
- Interrupts: `INTR_FLAG_<SOURCE>` (IE/IF bit) in `interrupts.h`; keep the
  pret names (`INTR_FLAG_KEYPAD`, `INTR_FLAG_GAMEPAK`).
- Syscalls: `SWI_<NAME>` numbers plus plain-function prototypes
  (`CpuSet`, `Div`, ...) in `syscall.h`; thunk entry addresses are pinned
  in comments and must not be renumbered without ROM evidence.

## Macro policy

- Object macros everywhere; function-like macros only where pret uses
  them: `REG_TMCNT(n)`, `REG_SIOMULTI(n)`, `WIN_RANGE(a, b)`,
  `WIN_RANGE2(a, b)`, `BGCNT_PRIORITY(n)`, `BGCNT_CHARBASE(n)`,
  `BGCNT_SCREENBASE(n)`, `BLDALPHA_BLEND(a, b)`, `RGB(r, g, b)`,
  `ARRAY_COUNT(a)`. Do not introduce new function-like macros without a
  pret precedent; write a static inline-free helper function instead
  (or a `#define` that expands to an expression).
- Every macro that reads hardware expands to a `volatile` lvalue via the
  `vu*` types; never cast a register address to a plain type.

## Volatile / MMIO discipline

- All MMIO goes through `REG_*` from `io_reg.h`; do not open-code
  `(*(vu16 *)0x04000xxx)` outside that file.
- Never cache `REG_*` values across accesses that must observe hardware
  state (read each time through the macro; the `volatile` in the expansion
  prevents unwanted reordering).
- `src/agb_sram.c` codegen depends on the exact expansion of
  `REG_WAITCNT` — keep `REG_BASE + REG_OFFSET_WAITCNT` folding intact
  (`make compare` guards this).

## SWI numbering (important)

This ROM uses the **SDK/libagbsyscall SWI order**, not the retail-BIOS
order (they differ for 0x08–0x0F; full table and evidence in
`gba/syscall.h`). Verified in-ROM entries: 0x00/0x01 (reset pair),
0x06 (Div), 0x0A (ArcTan2), 0x0B (CpuSet), 0x0C (CpuFastSet),
0x11/0x12/0x13 (LZ77UnCompWram/Vram, HuffUnComp), 0x25 (MultiBoot),
0x28 (SoundDriverVSyncOff). When writing SWI thunks or checking m2c
output, always confirm semantics from a call site (BL census) rather
than assuming the retail mapping.

## Adding a new definition

1. Evidence first: an I/O store/load or SWI thunk observed in the ROM
   (cite `docs/analysis/rom-map.md`) or a GBATEK/pret reference for
   unused-but-mapped hardware.
2. One definition site only — no duplicate macros across headers;
   `tools/header_smoke.c`'s direct includes catch umbrella omissions.
3. Extend `tools/header_smoke.c` to touch the new macro/prototype.
4. Run `make check-headers` (both compilers) and a full
   `make clean && make compare` if any existing macro expansion changed.
5. If a new platform fact was learned (address, numbering, ABI quirk),
   record it in `docs/analysis/rom-map.md` and `docs/lessons-learned.md`.
