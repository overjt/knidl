# ROM map — Kirby: Nightmare in Dream Land (USA, A7KE)

Layout analysis of `baserom.gba` (8 MiB, SHA-1 `37a476567d133c146fee6b5e2eb0b07a215da6b0`).
All addresses are GBA cart VMAs (`file offset = VMA - 0x08000000`); every claim below is
verifiable against `baserom.gba` at the cited address. Produced for issue #2.

## 1. Method

All disassembly ran inside the Docker toolchain image (per `AGENTS.md`):

```sh
docker run --rm -v $PWD:/src -w /src knidl-builder \
    arm-none-eabi-objdump -D -bbinary -marmv4t --adjust-vma=0x08000000 baserom.gba   # ARM view
docker run --rm -v $PWD:/src -w /src knidl-builder \
    arm-none-eabi-objdump -D -bbinary -marmv4t -Mforce-thumb \
    --adjust-vma=0x08000000 baserom.gba                                               # Thumb view
```

Raw dumps (~1.96M lines each) were written outside the repo and are not committed.
Classification heuristics: BL call-target census (both dumps), function-pointer tables
(words in `0x08xxxxxx`, bit 0 set = Thumb entry), lr-saving prologue scan
(`push {..,lr}` / `stmfd sp!, {..lr}`), entropy/zero-density per 64 KiB block,
ASCII scan, BIOS-LZ77 header scan, zero-run scan, pointer-run clustering.

## 2. Segment table

| # | ROM range (VMA) | Size | Content | Evidence |
|---|-----------------|------|---------|----------|
| 1 | `0x08000000-0x080000BF` | 0xC0 | **Cartridge header** | `0x08000000: ea00002e  b 0x080000C0` (ARM entry); title `AGB KIRBY DX` @0x080000A0; code `A7KE` @0x080000AC; maker `01` @0x080000B0; fixed `0x96` @0x080000B2; version `0x00` @0x080000BC; complement `0xC1` @0x080000BD |
| 2 | `0x080000C0-0x0800020C` | 0x14C | **crt0 + master ISR (ARM)** | `msr CPSR_fc` mode switches, stack loads `0x03007EC0`/`0x03007F60`, `bx` calls to Thumb init/main; ISR pushes `{r0-r3,lr}`, dispatches through table @`0x030004B0` |
| 3 | `0x08000210-0x08000233` | 0x24 | crt0/ISR literal pool | `0x08000214: 03007FFC`, `0x08000218: 08000311` (init|1), `0x0800021C: 08007301` (main|1), `0x0800022C: 030004B0`, `0x08000210: 89abcdef` |
| 4 | `0x08000234-0x080002E4` | 0xB1 | **Task/context-switch helpers (ARM)** | 4 small routines saving/restoring `sp`/`lr`/`r0` into IWRAM cells (`0x03004C94`, `0x03002470`, `0x030026F8`, `0x030025E0`, `0x0300248C`, `0x030025F0`); `bl 0x080CFDDC` @0x08000290 (the only ARM `bl` in the ROM) |
| 5 | `0x08000310-0x080006FE` | 0x3EE | **AgbInit (Thumb)** | Prologue `b5f0 464f 4646 b4c0` @0x08000310; epilogue `pop {r4-r7}; pop {r0}; bx r0` @0x080006F4-0x080006FE; performs the memory clears/copies listed in §4 |
| 6 | `0x08000700-0x080072FF` | 0x6C00 | Game code (Thumb): early subsystems | 160+ Thumb BL targets in this window; includes IRQ default handler `0x08001518`, `0x080010CC`, task entry `0x08005654` |
| 7 | `0x08007300-0x080CF9FF` | ~0xC8700 | **Game code + rodata (Thumb)** | ~2,650 Thumb BL targets, thumb-pointer tables throughout; interleaved rodata (pointer tables 122 runs ≥8 entries, e.g. 43-entry table @0x0803EC48) |
| 8 | `0x080CFA40-0x080CFA7E` | 0x3F | **SDK syscall wrappers (Thumb)** — named via SDK-order SWI table (`include/gba/syscall.h`, issue #27) | `svc N; bx lr` pairs: `0x080CFA50` ArcTan2 (`svc 0x0A`), `0x080CFA54` CpuFastSet (`svc 0x0C`), `0x080CFA58` CpuSet (`svc 0x0B`), `0x080CFA5C` Div (`svc 0x06`), `0x080CFA60` Mod (`svc 0x06`, returns remainder), `0x080CFA68` HuffUnComp (`svc 0x13`), `0x080CFA6C` LZ77UnCompVram (`svc 0x12`), `0x080CFA70` LZ77UnCompWram (`svc 0x11`), `0x080CFA74` MultiBoot mode=1 (`svc 0x25`), `0x080CFA7C` SoundDriverVSyncOff (`svc 0x28`); plus `0x080CFA40` (unidentified SDK helper, 0 callers) and `0x080CFA4C` (`bx lr` stub, referenced from `0x080CEA48`); `0x080CFA80` reset helper (`svc 1; svc 0`) |
| 9 | `0x080CFA9C-0x080CFDDB` | ~0x340 | **C library + SRAM driver (Thumb)** — SRAM driver **decompiled** (`src/agb_sram.c`, old_agbcc `-O1 -mthumb-interwork`) | SRAM driver `0x080CFA9C-0x080CFC2F`: `ReadSram_Core` `0x080CFA9C`, `ReadSram` `0x080CFAC0`, `WriteSram` `0x080CFB24`, `VerifySram_Core` `0x080CFB64`, `VerifySram` `0x080CFB94`, `WriteSramEx` `0x080CFBF8` (all byte-identical, linked from C); WAITCNT setup + stack-copy calls via `_call_via_r3` @`0x080CFC3C`; fn table `0x0872EA04` = {ReadSram_Core, ReadSram, VerifySram_Core, VerifySram} (no xrefs); remaining libc tail `0x080CFC30-0x080CFDDB` verbatim (call_via_rN, copy loops, division) |
| 10 | `0x080CFF00-0x080CFFFF` | 0x100 | lib rodata | small tables (sine-like `0x0700..0001` @0x080CFF00), `0x7FFF` blocks @0x080CFF20, reverb/FIR-style coefficient sets — bytes `e8 50 e0 c1 00 90 00 00` / `.. 08 00` / `.. 80 00` @0x080CFF3A — m4a-family sound tables |
| 11 | `0x080D0000-0x08120000` | 0x50000 | Level/map & object tables | entropy 4.4-5.4, 31-48% zeros, few pointers; `faff/0000/0100` pattern tables (e.g. file `0xD00C0`) |
| 12 | `0x08120000-0x08330000` | ~0x210000 | Level data / uncompressed graphics / palettes, with embedded table zones | pointer clusters @0x08120000 (1066), 0x08150000 (962), 0x081A0000 (1140), 0x08200000 (1206), 0x08250000 (1526) |
| 13 | `0x083356E0-0x0834EEE8`, `0x08350AF8-0x083A85D4` | ~0x1B290 | **Sound sample data (PCM)** | pointed to by the sample index @`0x087E1D58` (24 pointers into `0x0833-0x0834`, 339 into `0x0835-0x083A`); high entropy (~7.2), zero-pct ~6-8% |
| 14 | `0x083D0000-0x085C0000` | 0x1F0000 | Compressed graphics (LZ77/RLE-class) | entropy 7.0-7.8 uniformly, near-zero pointer density |
| 15 | `0x085C0000-0x0872E9F7` | ~0x16EA00 | **m4a songs / sequences** | song table @`0x0860B460` (0x338 bytes, 103 `(ptr,0)` entries; first → song header @`0x0870F504`, bytes `08 00 00 80` + track ptr `0x0860A418` = valid m4a header); tail pointers `0x0860ACB8`,`0x0872E800` @0x0872E9F0 |
| 16 | `0x0872E9F8-0x0872EA01` | 10 | `SRAM_V112` string | ASCII @0x0872E9F8 (`53 52 41 4D 5F 56 31 31 32 00`), save-type marker |
| 17 | `0x0872EA04-0x0872EA13` | 16 | SRAM driver function table | 4 Thumb ptrs `0x080CFA9D, 0x080CFAC1, 0x080CFB65, 0x080CFB95` |
| 18 | `0x08730000-0x08760000` | 0x30000 | **Asset metadata / index zone** | >30k in-ROM pointers; targets spread across segs 7,11,12,15 and self-referential @0x0873-0x0876 (2186+1176+1616+2129 self pointers) |
| 19 | `0x08760000-0x087E1D57` | ~0x181D58 | Song tail / misc audio data | song headers/tracks referenced from seg 15 (e.g. `0x0870F504`); moderate entropy 6.5-7.3 |
| 20 | `0x087E1D58-0x087E3087` | 0x1330 | **End-of-ROM index (sample sets)** | 1228 words; entries point into seg 13 (`0x083356E0...`, `0x08350AF8...`) and back into this table; first entries point @0x087E1F68+ (sub-tables) |
| 21 | `0x087E3088-0x087FFFFF` | 0x1CF78 | Zero padding | last non-zero byte @ file `0x7E3087` |

Notes:
- No meaningful ASCII strings exist in the game (text is charset-graphics based); the only
  real strings are the header title (seg 1) and `SRAM_V112` (seg 16).
- Boundaries inside `0x080D0000-0x083D0000` (segs 11-13) are soft: table zones and data
  blobs interleave. The boundary `0x080CFFFF|0x080D0000` (code|data) is sharp: last
  validated Thumb entry `0x080CFBF8`, lib rodata to `0x080CFFFF`, then table data.
- Three small gaps between the segments above were found during the `main_blob` split
  and are now explicit entries in `segments.txt`:
  `0x080CFDE4-0x080CFDE8` (4 B, veneer→IRQ table padding, bytes `55 56 00 08`),
  `0x0872EA02-0x0872EA04` (2 B, null padding after SRAM string),
  `0x0872EA14-0x08730000` (5612 B, data between SRAM fn table and asset metadata).

## 3. ARM vs Thumb mix

Measurement (BL-target census + pointer tables + prologue validation, restricted to the
code span `0x080000C0-0x080CFFFF`):

| Metric | Value |
|---|---|
| Validated Thumb function entries (BL targets ∪ bit0-set ROM pointer targets, plausible prologue) | **3,149** |
| Thumb entry span | `0x08000310-0x080CFF00` |
| Thumb code+inline-pool coverage (union of entry→next-entry intervals, 4 KiB cap) | **~824 KiB** (upper bound; includes interleaved rodata gaps) |
| ARM functions | 3 zones: crt0+ISR `0x080000C0-0x0800020C` (0x14C), task switch `0x08000234-0x080002E4` (0xB0), veneer `0x080CFDDC` (8) = **~0x204 bytes** |
| ARM `bl` instructions in the entire ROM | **1** (`0x08000290: bl 0x080CFDDC`) |
| Interworking | ARM→Thumb only via `bx reg` with odd literal (`0x08000311` init, `0x08007301` main, `0x08005655` veneer target); Thumb↔Thumb `bl` throughout |

**Conclusion: KNiDL is a Thumb-compiled game** (~99.9% of in-ROM code bytes are Thumb),
unlike its ARM-compiled successor KATAM. For this repo this means: default `agbcc`
(Thumb) for `src/`, `agbcc_arm` only for the crt0/ISR/task units, and `old_agbcc` is the
likely match for the SDK lib/SRAM units (`0x080CF9xx` zone).

## 4. Boot flow trace

```
BIOS -> 0x08000000: b 0x080000C0                     ; header entry, ARM state
0x080000C0: mov r0,#0x12 ; msr CPSR_fc,r0            ; -> IRQ mode
0x080000C8: ldr sp,=0x03007F60                       ; IRQ stack (lit @0x08000104)
0x080000CC: mov r0,#0x1F ; msr CPSR_fc,r0            ; -> System mode
0x080000D4: ldr sp,=0x03007EC0                       ; SYS stack (lit @0x08000100)
0x080000D8-0x080000E0: str 0x08000108 -> [0x03007FFC]; install ROM master ISR
0x080000E4-0x080000EC: mov lr,pc ; bx 0x08000311     ; -> AgbInit (Thumb) 0x08000310
0x080000F0-0x080000F8: mov lr,pc ; bx 0x08007301     ; -> main (Thumb) 0x08007300
0x080000FC: b 0x080000C0                             ; loop if main returns
```

`AgbInit` (`0x08000310-0x080006FE`) memory setup — all via SWI wrappers, no open-coded loops:

| Call | Wrapper | Operation |
|---|---|---|
| `CpuFastSet` @0x08000326 | `0x080CFA54` (`svc 0x0C`) | fill `[0x02000000]`, 0x10000 words (256 KiB) with 0 — **EWRAM clear** |
| @0x08000332 | same | fill `[0x03000010]`, 0x1E words (0x78 B) — IWRAM clear, skipping BIOS `0x03000000-0x0300000F` |
| @0x08000340 | same | fill `[0x0C000000]`, 0x60 words (0x180 B) — SRAM mirror |
| @0x08000350 | same | fill `[0x0E000000]`, 1 word — SRAM |
| @0x0800035E | same | fill `[0x0A000000]`, 1 word — SRAM mirror |
| `CpuSet` @0x0800037E | `0x080CFA58` (`svc 0x0B`) | copy `0x080CFDE8 -> 0x030004B0`, 28 halfwords (56 B) — **14-entry IRQ handler table** |
| @0x0800038A | same | copy `0x08000108 -> 0x03001030`, 160 halfwords (320 B) — **master ISR copied ROM→IWRAM** |
| @0x08000390 | — | `str 0x03001030 -> [0x03007FFC]` — repoint BIOS IRQ vector at the IWRAM ISR copy |

The remainder of AgbInit (`0x080003E8-0x080006F4`) initializes IO registers
(`0x04000000` block: DISPCNT, timers, sound regs at `0x04000080+`, KEYINPUT etc.
via masked halfword stores) and zeroes state cells in IWRAM/EWRAM.

Master ISR (runs from IWRAM `0x03001030`, source `0x08000108-0x0800020C`):
saves `SPSR,r1,r2,r3,lr` (`0x0800011C`), reads the `0x04000200` interrupt block,
computes a priority index (`ip` = 0/4/8/12/24/52 by bit tests `0x80/0x40/0x01/0x02/0x10/0x2000`
at `0x08000130-0x08000174`), tail-calls `[[0x030004B0 + ip]]` (`0x080001BC-0x080001D0`),
restores and `bx lr` (`0x080001EC-0x0800020C`), including an IRQ-stack guard
(`sp < 0x03007B80` → save to `[0x03000FA0]`, `sp = 0x03000FA0`).

## 5. Compiler-validation leaf candidates

Criteria: Thumb entry reached by `bl` or a bit0-set pointer; no external branch or
pointer into the interior; single return; ≤1 call; size 0x06-0x40. The scan found
**282** candidates; 12 selected below for agbcc codegen validation (sizes/return style
read from the ROM; all are plain compiler-shaped Thumb-1 with pc-relative literal pools):

| Entry | Size | Calls | Return | Shape (evidence) |
|---|---|---|---|---|
| `0x080022A0` | 0x08 | 0 | `bx lr` | literal store: `str 0 -> [0x03000B00]` |
| `0x080055B0` | 0x0E | 0 | `bx lr` | scaled-index field set: `strb r0,[base + ((idx*9)<<4) + 19]`, base `0x03002790` lit @0x080055C0 |
| `0x08009AA0` | 0x0E | 0 | `bx lr` | two global clears: byte `[0x02006030]`, word `[0x03000014]` |
| `0x08022F9C` | 0x08 | 0 | `bx lr` | literal store (pool @0x08022FA0) |
| `0x08001488` | 0x1C | 0 | `bx lr` | two masked IO halfword writes + word store (lits @0x080014A4+) |
| `0x08004968` | 0x1C | 0 | `bx lr` | same pattern family |
| `0x0800AAAC` | 0x1C | 0 | `bx lr` | same pattern family |
| `0x08010020` | 0x18 | 0 | `pop {r4,pc}`-style | four `strb` setters to globals (pool @0x08010038+) |
| `0x080089E0` | 0x18 | 1 | pop-ret | setter + one call |
| `0x08001A34` | 0x32 | 1 | pop-ret | globals init + `sub sp,#4` local (pool @0x08001A68+) |
| `0x080059A0` | 0x32 | 1 | pop-ret | same family, larger pool |
| `0x08022F50` | 0x2C | 1 | pop-ret | same family |

Suggested first target: `0x080022A0` / `0x080055B0` (pure Thumb leaf, no frame) to
confirm default-`agbcc` Thumb codegen (pool placement, `ldr pc` literals), then
`0x08001488` to validate IO-register idioms now that `main_blob` has been split into per-segment sections.

## 6. Notes / open questions

- SWI numbering follows the **SDK/libagbsyscall order**, not the retail-BIOS order: the two agree for 0x00-0x07, 0x10-0x1F and 0x25+, but differ for 0x08-0x0F (e.g. `0x0B` = CpuSet here, Sqrt on a retail BIOS; `0x0A` = ArcTan2 here, GetBiosChecksum on retail). `0x080CFA54`/`0x080CFA58` are the SDK's thunks for CpuFastSet/CpuSet in that order. Full verified table and thunk map: `include/gba/syscall.h` (issue #27; identification method in docs/lessons-learned.md §3.6).
- Segments 18-20 (asset metadata, sample index) are indexed but not yet named; their semantics belong to the graphics/sound loaders and should be resolved when those loaders are decompiled.
- One isolated Thumb `bl` pair was detected at `0x080D1B1E -> 0x080315E0` inside the seg-11 data zone (single occurrence; possibly a small code overlay or coincidence — investigate when decompiling the level/object table loaders).
- `0x08000210: 89abcdef` magic + task-switch helpers (seg 4) suggest the engine's cooperative task system; the IWRAM task pointer (`0x03004C94` etc.) cells should be named during crt0 extraction.

Generated summaries (not committed): strings/pointers/lz77/zeros/isa scans as described in §1.
