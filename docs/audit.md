# The final audit (`make audit`)

Issue #37 closes the project's migration campaigns with one reproducible
check of what the source still holds that is not plain, named C.
`tools/audit.py` runs it (`make audit`, and CI on every push); it reads the
committed tree only, so it needs no baserom and no build, and it takes about
two seconds.  Every item it finds is either fixed or listed with its reason:
in the file itself (`@ raw:` / `raw:` comments), in the two lists of this
document, or in the placeholder census it generates into
[`docs/naming.md`](naming.md) section 5.

```sh
make audit                      # check (exit 1 on any problem)
python3 tools/audit.py --list   # also print every listed item
python3 tools/audit.py --write  # regenerate docs/naming.md's census
```

## 1. What it checks

| check | rule | where the reasons live |
|---|---|---|
| `.incbin` | `baserom.gba` slices only in the structure-only `data/*.s` (docs/data.md) and the Nintendo logo in `asm/rom_header.s`; none in the rest of `asm/`, in `src/` or in `include/` | AGENTS.md data policy |
| raw directives in `asm/` | every `.byte`, `.short`/`.hword`/`.2byte`, `.inst` and number-operand `.word`/`.4byte`/`.long` is an instruction or a symbol, or carries a same-line `@ raw: <reason>` (tools/split.py emits it for generated files) | the line itself |
| the sanctioned asm | every file in `asm/` has a `@ Stays asm:` header comment; section 2 lists exactly the files in `asm/`, and its "excluded" rows equal `tools/calcrom.pl`'s `%decomp_excluded_asm` | the file header, section 2 |
| raw addresses in C | no ROM (0x08-0x0D), SRAM (0x0E), EWRAM (0x02), IWRAM (0x03), I/O (0x04), palette (0x05), VRAM (0x06) or OAM (0x07) address as a bare number in `src/` or the game headers `include/*.h`: a symbol, a region macro (`include/gba/defines.h`) or a `REG_*` macro (`include/gba/io_reg.h`), or a `raw:` comment on the same line or the line above; the last argument of `CpuSet`/`CpuFastSet` is a control word, not an address | the comment |
| code exceptions | every `register ... asm("rN")` pin, `asm(...)` statement, `BLOCK_CROSS_JUMP` and `while (0)` in `src/` is a row of section 3, and every row still points at its site | section 3 |
| placeholders | `sub_*` functions, `gUnk_*` RAM, I/O and ROM symbols, `unk*` struct fields and `loc_*` labels counted by kind, category and zone, each category with its reason; since #155 run 7 every `sub_*`, `gUnk_` RAM cell and `unk*` field must have a reason row in `docs/analysis/unnamed.csv` (no row, a stale row or an unknown code fails) and the functional ROM labels are classed by their referrers; since #183 the asset labels are classed by `tools/name_assets.py` from their owner chains (named by their owner, or a computed reason; a label only one named function's code mentions needs a row, and one the docs/naming.md 2.5 rules would still name fails); the generated table in docs/naming.md section 5 must be current | docs/naming.md section 5.1 |
| source file names | every row of `docs/analysis/file-renames.csv` (#182) has its new `src/*.c` and not its old one; no old file name is left anywhere else in the tracked tree; every prefix is a game header's name or `boot_` / `game_over_`, and a row whose prefix is not its header's carries a deviation; every other `src/*.c` is `main.c`, `agb_init.c`, `agb_sram.c` or `m4a_*.c` (`tools/rename_tu.py --check`) | docs/naming.md section 8, the table's `deviation` and `evidence` columns |

## 2. The sanctioned asm

The only assembly left in the ROM, each file with why it stays asm.  "excluded"
rows are code that `tools/calcrom.pl` excludes from "remaining to be
decompiled" (`%decomp_excluded_asm`; `make progress` counts them as "bytes
excluded from decompilation tracking"); the audit checks that the two lists
agree.  "data" and "symbols" rows hold no code.

| file | counted as | why it stays asm |
|---|---|---|
| `asm/rom_header.s` | excluded | the cartridge header: the entry branch, the Nintendo logo (an `.incbin` slice of the user's baserom), the title, codes and checksum; header fields are data by nature, written as asm as in pret's `rom_header.s` |
| `asm/crt0.s` | excluded | crt0 (`Start`) and the master interrupt handler (`MasterIsr`, copied to IWRAM `0x03001030`): ARM mode switches, stack set-up and the IRQ dispatch prologue, which no C compiler emits; every pret GBA project keeps `crt0.s` in asm |
| `asm/task_switch_helpers.s` | excluded | the ARM coroutine switch (`TaskSwitch`, `TaskYield`, `TaskExit` and the task-done check): it saves and swaps `sp`, which no C source can express (rom-map section 6) |
| `asm/task_literals.s` | data | the task switch helpers' literal pool (named IWRAM cells); segment kind `pool` |
| `asm/m4a_1.s` | excluded | the m4a sound engine's hand-scheduled asm core with its ARM mixer `SoundMainRAM` (copied to IWRAM); pret's projects keep `m4a_1.s` in asm too (rom-map section 8) |
| `asm/sdk_swi_wrappers.s` | excluded | the BIOS call thunks: bare `svc N; bx lr`, which agbcc cannot emit (#29) |
| `asm/sdk_reset_helper.s` | excluded | `SoftReset`: the same SWI-thunk shape (#29) |
| `asm/sdk_libc.s` | excluded | libgcc's `lib1funcs.asm` routines (`__divsi3`, `__umodsi3`, `_div0`) and `_call_via_rN`, hand-written asm in gcc's own tree, plus the Thumb-to-ARM task trampolines (#30) |
| `asm/interworking_veneer.s` | excluded | the linker-style `ldr ip, [pc]; bx ip` ARM veneer |
| `asm/rom_syms.s` | symbols | no bytes: the absolute symbols of the RAM and I/O cells and of the functions no split file defines, generated by `make split` |

## 3. Code exceptions

The C that is not plain C.  The kinds the audit finds mechanically are
`pin` (`register ... asm("rN")`), `lever` (an empty `asm("")` statement),
`inline-asm` (a non-empty `asm(...)` statement), `alias` (a declaration
with an `asm("name")` label), `cross-jump` (`BLOCK_CROSS_JUMP`) and
`zero-code` (a `do ... while (0)` that emits no code of its own);
`stand-in` rows are documented source shapes the audit cannot recognise
(a volatile re-read, a dead store, an always-true conjunct, a stand-in
local), and it checks only that the lines still lie in the named function.
A row names `file:line` or `file:first-last`.

| site | function | kind | reason |
|---|---|---|---|
| `src/actor_whispy_woods_items.c:679-680` | `WhispyWoodsLeavesFillTrail` | zero-code | #37: two nested `do { } while (0)` around the `unk70` store weight its address's use x3 (1 + 3 = 4 refs), which ranks the address above the `&gCurTask` pool value in local allocation, as the ROM's r3/r4 show (lesson 3.524); replaced #154's ten pins and six levers |
| `src/boot_logo_update_objects.c:137-142` | `BootLogoUpdateObjects` | lever | #152's two approved zero-byte levers (`sub_080caab8` before #155): an opaque `0xFFFF` and a live mask at one store (lessons 3.457, 3.494).  Sanctioned and final since #169, which measured why the plain store folds: gcse's reaching register for the old id has two sets, so combine uses its first scan's union (0xFFFF); with one set the plain source gives the ROM's `orrs`, and the only route left is unnatural (lesson 3.527).  The plain store is 40 bytes off (#100) |
| `src/enemy_king_dedede_damage.c:189-216` | `KingDededeReactToDamage` | cross-jump | `BLOCK_CROSS_JUMP` (`include/global.h`, a pret idiom) at four tails that the ROM really duplicates instead of cross-jumping (#154) |
| `src/link_block_main.c:56` | (file scope) | alias | the `MultiBoot` SWI thunk declared int-returning under a local name, as the ROM keeps the untruncated result (lesson 3.481) |
| `src/link_multiboot.c:45` | (file scope) | alias | the same alias in the second MultiBoot unit |
| `src/link_multiboot.c:199-213` | `MultiBootWaitCycles` | inline-asm | the SDK's own inline asm: pokeemerald's `src/multiboot.c` `MultiBootWaitCycles` is the same asm |
| `src/m4a_c1.c:293` | `MusicPlayerJumpTableCopy` | inline-asm | a dead SDK export that is one `swi 0x2A` in inline asm, as in katam's SDK |
| `src/camera_bg_anims.c:256` | `UpdateBgAnims` | zero-code | its loop notes weight the body's references one loop level deeper, which puts the slot pointer in r4 and the command pointer in r5 as in the ROM (comment at the function) |
| `src/enemy_mr_shine_and_mr_bright.c:847` | `MrShineAndMrBrightPickGroundMove` | zero-code | #154's commented stand-in: the loop note ranks `acc` (r2) above `r` (r3) in global allocation |
| `src/player_hurt.c:728` | `PlayerActionHurtUpdate` | zero-code | an empty loop whose loop-end note stops cse1 from following a jump into the block (comment at the site) |
| `src/player_water.c:307` | `PlayerActionSwimUpdate` | zero-code | counts case 1's references one loop level deeper, so the key mask wins its register ahead of the switch value (lessons 3.383, 3.412) |
| `src/player_throw_hold.c:105` | `PlayerActionThrowHold` | zero-code | #88's priority lever around one call (lesson 3.424) |
| `src/player_throw_hold.c:242-243` | `PlayerActionThrowHoldUpdate` | zero-code | #88's nested priority levers around one store |
| `src/player_meta_knight_swim_update.c:85` | `MetaKnightActionSwimUpdate` | zero-code | the lever of its M10 twin `PlayerActionSwimUpdate` (lessons 3.383, 3.412) |
| `src/actor_whispy_woods_items.c:664` | `WhispyWoodsLeavesFillTrail` | stand-in | #37: an unused read of `unk70`, zero code, which computes the `unk70` address first (`adds r3, #112` before the first `ldrh`, lesson 3.524) |
| `src/room_spawn_objects.c:384` | `AllocObjTilesAndPalettes` | stand-in | #154's volatile read: the ROM loads the palette cursor twice, and a plain read lets cse reuse the first load |
| `src/cutscene_actor_particles.c:51-54` | `sub_080109c8` | stand-in | #82's two commented `volatile` placeholder re-reads (the ROM re-reads `unk00` and `unk04`) |
| `src/player_share_item.c:476-479` | `sub_0803c9b4` | stand-in | the twin of `sub_080109c8`'s two `volatile` re-reads |
| `src/link_sync_random.c:139` | `LinkSyncRandom` | stand-in | the always-true conjunct `gUnk_03001EFC == 0`, zero code, which keeps the branch two-way until cse1 (lesson 3.488) |
| `src/link_serial_cb.c:134` | `SerialCB` | stand-in | the dead `i = 4;` before a `break`, which keeps jump.c's else-arm swap out of the passes before register allocation (lessons 3.490, 3.491) |
| `src/player_meta_knight_swim_update.c:55-56` | `MetaKnightActionSwimUpdate` | stand-in | the `m` and `tp` locals, stand-ins for an address copy gcse cannot place here (the function's header comment) |
