# Header conventions — `include/gba/` (issue #27) and the game headers (issue #36)

One page of rules so later agents extend the headers the same way instead
of inventing conflicting definitions. Read this before adding anything to
`include/`.  The platform headers (`include/gba/`) come first; the game's
subsystem headers (`include/*.h`) are the last section.

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
| `task.h`, `main.h` ... `ending.h` (repo root include/) | the game's structs, RAM cells, ROM tables and prototypes, one header per subsystem (last section) |

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

## Game headers — `include/*.h` (issue #36 phase 2)

Every RAM cell, ROM table and C function the game code shares is declared
**once**, in the header of its subsystem.  A source file includes the
headers it needs after `gba/gba.h`, `global.h` and `task.h`; it declares
nothing of its own except what the exceptions below keep.

| header | subsystem (the files whose symbols it owns) |
|---|---|
| `main.h` | AgbInit, AgbMain, the SRAM driver and the engine zone's main loop, interrupts, input, display shadows, sprites and fades |
| `task.h` | the task engine (structs, `gTasks`, `gCurTask`, `gTaskTypes`, the trampolines) |
| `link.h` | the SIO link driver, link-play sessions, the SDK MultiBoot library |
| `sound.h` | the BGM/SE front end and the m4a C driver (the m4a API itself stays in `gba/m4a_internal.h`) |
| `mode.h` | the game-state bodies, boot/title sequence, screen loaders (M02) |
| `hud.h` | the HUD (M02) and HUD/overlay effects (M33) |
| `menu.h` | the main menu and its tasks (M03) |
| `cutscene.h` | AgbMain state 7, the sequence director (M04), the cutscene bank (M19) |
| `collision.h` | the collision engine and hit tests (M06), the map queries |
| `room.h` | the level/room builder, doors, stage helpers (M07); the room table and RoomDefs |
| `camera.h` | the camera, BG streaming, map events, stage objects (M08) |
| `player.h` | the player (M05, M09-M14) |
| `effect.h` | the player effect objects and the effect spawner (M15, M16) |
| `actor.h` | the actor core (M17, M18); the actor definition tables |
| `enemy.h` | the enemy, mid-boss and boss banks (M20-M32) |
| `save.h` | the save file and M34's other subsystems |
| `subgame.h` | the sub-game framework and the three sub-games (M35-M37) |
| `ending.h` | the ending, credits, boot logo objects, game-over screen (M37-M38) |

Rules (run 1 applied them mechanically; follow them by hand from now on):

- **Where a symbol lives.**  A RAM cell belongs to the lowest subsystem
  that uses it (order: main, task, sound, link, room, collision, camera,
  actor, player, effect, save, hud, mode, menu, cutscene, enemy, subgame,
  ending), AgbInit and AgbMain voting only when nothing else uses the
  cell; a ROM table to the subsystem with the most consumers; a function
  to the subsystem of the file that defines it, grouped under a
  `/* src/<file>.c */` line in definition order.
- **Which type.**  The one the consumers compile to the same instructions
  with.  Run 1 tried every spelling a symbol had in every file that
  declared it and kept the type the most files accept byte-identically;
  a function's prototype is its definition's own line.  Signedness stays
  as the proven reads need it (fields the ROM reads signed stay unsigned
  in shared structs and the sites cast, as `task.h` does).  Tables defined
  in `src/data/` are `const` (docs/data.md §5.2).
- **Structs.**  A struct that a header's declarations need is defined in
  that header when every local copy of it has the same layout (or accepts
  the header's, byte-identically); a struct only pointed at is
  forward-declared (`struct RoomDef;`).  **An array of a struct must see
  the struct's definition**: agbcc gives an array declared with an
  incomplete element type byte alignment, and its halfword fields then
  load as two `ldrb` (lesson 3.518), so such a header includes the
  struct's home header.
- **Views.**  C allows one type per symbol in a translation unit, so a
  file whose code needs another type for a symbol (an array read as `u32
  []`, a `vu32` chain in AgbInit, a partial struct copy) cannot include
  the header that declares it: it keeps its own declarations of that
  header's symbols, spelled like the header wherever the file accepts
  that, under a one-line note `/* Not from room.h: this file's view of
  gRoomMap differs (lesson 3.517). */`.  A function whose calls in some
  file need another signature (lesson 3.428) stays out of the headers and
  keeps its per-file prototypes, under a note citing 3.428/3.517.  Never
  change code to fit a header; a new view goes to the owner's review.
- **Adding a symbol.**  Add the declaration to its owner header, in
  address order within its EWRAM/IWRAM/ROM block (a function: under its
  file's line), remove the local copies, and prove it with `make clean &&
  make compare`.  Declarations never move PRE or pool order (lessons
  3.515, 4.79); the types do, which is why every change is compared.
- `tools/header_smoke_game.c` includes every game header into one
  translation unit (`make check-headers`): a symbol declared twice with
  two types, a struct defined twice or a missing definition fails there.

### Per-family views of `struct Task` (#155 run 3)

Some `struct Task` fields hold a different thing in different task
families.  Where two meanings each hold on every path of their families,
`include/task.h` gives the field a **named union** (agbcc, gcc 2.95, has
no anonymous unions), and every access says which member it means:

| field | union | members, and who uses which |
|---|---|---|
| `0x80` | `u80` (`__attribute__((packed))`) | `nearestPlayer` (s8): the actor API's nearest-player index (sub_08063a9c, TaskFindNearestPlayer, TaskFree's clear, the #170 trail's history); `attackAbility` (s8): the ability id of the running attack for the player, its objects (#6) and effects (#7), read by ActorPlayHitSfx off the hitter |
| `0x8C` | `u8C` | `actor` (`struct Actor *`): the task's `&gActors[slot]` for every actor family; `parentTask` (`struct Task *`): `&gTasks[Task.parent]` for task types #6 Task_PlayerObject and #7 Task_PlayerEffect (their bodies bind it, sub_08056770 rebinds it with `parent`) |

Rules for a view:

- It changes no instruction: every translation unit's agbcc assembly is
  identical to the parent commit's (`pending/`'s whole-tree oracle,
  lesson 4.128), gcc 12's `-fsyntax-only` error set over the tree is
  unchanged, and `make compare` passes.  The accesses are respelled; the
  only other code change allowed is a cast the view makes unnecessary
  (`(struct Task *)t->unk8C` became `t->u8C.parentTask`: 247 casts to
  `struct Task *` and 2 to `struct Actor *` went away).
- **A union narrower than a word must be `packed`**: agbcc pads every
  union, like every struct, to a multiple of 4 bytes
  (`STRUCTURE_SIZE_BOUNDARY` 32), which moves every later field (lesson
  3.522); `packed` keeps a byte union at 1 byte and its `ldrb`, `packed,
  aligned(2)` a halfword union at 2.  `tools/header_smoke_game.c` checks
  the offsets of `u80`, `unk81`, `hitEffect`, `u8C` and `sizeof(struct
  Task)` with negative-size arrays, so a layout change fails `make
  check-headers` too.
- The task engine's local copies of `struct Task` (`src/early_4fec.c`,
  `early_58e4.c`, `early_5c4c.c`) keep their plain `u8`/`u32` members:
  the only accesses to them are InitTasks' clears (`w8C = 0`, `b80 |=
  0xFF` in `src/early_4fec.c`).
- A view is a type change, applied in its own commit (outside
  `tools/rename.py --verify-diff`, which proves renames only), and a new
  one goes to the owner first.
