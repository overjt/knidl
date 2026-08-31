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

The census heuristics are now committed as a reproducible generator,
`tools/symdb.py` (run via `make symbols`, Docker-wrapped): it decodes
`baserom.gba` directly (no intermediate dumps), emits the machine-readable
symbol database and call graph (section 7), and is validated by
`tools/symdb_check.py` against a fresh dual-view objdump disassembly.

## 2. Segment table

| # | ROM range (VMA) | Size | Content | Evidence |
|---|-----------------|------|---------|----------|
| 1 | `0x08000000-0x080000BF` | 0xC0 | **Cartridge header** | `0x08000000: ea00002e  b 0x080000C0` (ARM entry); title `AGB KIRBY DX` @0x080000A0; code `A7KE` @0x080000AC; maker `01` @0x080000B0; fixed `0x96` @0x080000B2; version `0x00` @0x080000BC; complement `0xC1` @0x080000BD |
| 2 | `0x080000C0-0x0800020C` | 0x14C | **crt0 + master ISR (ARM)** | `msr CPSR_fc` mode switches, stack loads `0x03007EC0`/`0x03007F60`, `bx` calls to Thumb AgbInit/AgbMain; ISR pushes `{r0-r3,lr}`, dispatches through table @`0x030004B0` |
| 3 | `0x08000210-0x08000233` | 0x24 | crt0/ISR literal pool | `0x08000214: 03007FFC`, `0x08000218: 08000311` (init|1), `0x0800021C: 08007301` (AgbMain|1), `0x0800022C: 030004B0`, `0x08000210: 89abcdef` |
| 4 | `0x08000234-0x080002E4` | 0xB1 | **Task/context-switch helpers (ARM)** — split to `asm/task_switch_helpers.s` (#24); their literal pools `0x080002E5-0x0800030F` are the separate `task_literals` segment (`asm/task_literals.s`), all ten words named cells | 4 small routines saving/restoring `sp`/`lr`/`r0` into IWRAM cells (`0x03004C94`, `0x03002470`, `0x030026F8`, `0x030025E0`, `0x0300248C`, `0x030025F0`; named via `tools/split_config.json` `data_symbols`, semantics in §6); `bl 0x080CFDDC` @0x08000290 (the only ARM `bl` in the ROM) |
| 5 | `0x08000310-0x080008E7` | 0x5D8 | **AgbInit (Thumb)** — **decompiled** (`src/agb_init.c`, agbcc `-O2 -mthumb-interwork`, issue #28) | Prologue `b5f0 464f 4646 b4c0` @0x08000310; epilogue `pop {r4-r7}; pop {r0}; bx r0` @0x080006F4-0x080006FE; performs the memory clears/copies listed in §4. The compiler's pool-skip branch (`b 0x080008E8`) sits at 0x08000700 followed by the 121-word literal pool to 0x080008E7 — the census entry `sub_08000700` is that branch (pointer-referenced from the data table @0x08369198), not a real function |
| 6 | `0x080008E8-0x080072FF` | 0x6A18 | **Game code (Thumb): early subsystems** — decompiled in issue #32: **168 of 182 functions (92%), 21232 of 27160 bytes (78%)** now in C, `make clean && make compare` byte-identical. **Recipe: `agbcc -O2 -mthumb-interwork -fprologue-bugfix` for the WHOLE zone** (lesson 3.75) — the flag suppresses agbcc's spurious leaf `push {lr}`; an earlier reading of that prologue as an `old_agbcc` fingerprint was wrong (lesson 3.18, superseded). Subsystems, in address order: **palette fade** `0x080008E8` (`early_08e8.c`, fade engine over the 512-colour buffer `0x03001270` -> `0x03001A90`); **frame tick** `0x08000DE4` (`early_0de4.c`, fade update, music volume ramp, VBlank spin on `0x03001EC4`, A+B+Start+Select `SoftReset(0x1C)`, play-time clock `0x03000498[4]`); **VBlank handler** `0x080010CC` (`early_10cc.c`); **OAM/palette flush, key reader, IO shadow flush, copy-queue ring `0x03000B80-0x03000F7B`, HBlank/VCount setters** `0x080011AC` (`early_11ac.c`); **default IRQ handler + VRAM transfer queue + sprite buckets** `0x08001518` (`early_1518.c`); **OAM shadow builder** `0x08001B08` (`early_1b08.c`) — copied to IWRAM `0x03001F40` and run from RAM, verified position-independent (zero `bl`, pool inside the copied bytes); **BG/fade/blend resets + fade variants + boot header check** `0x08001FD0` (`early_1fd0.c`); **SIO link handshake** `0x08002668`/`0x0800293C` (`early_2668.c`, `early_293c.c`); **link input dispatch, frame driver, wait helpers, 12-bit LCG rand, decimal split, colour blend** `0x08002B04` (`early_2b04.c`); **sound/SE subsystem** `0x08003110`+`0x08003484` (`early_3110.c`, `early_3484.c`, BGM play/stop/fade, volume, four-slot SE table; ROM table `gSeSongTable` at `0x0872EB38`, 8-byte entries indexed by `songId-100`); **SIO MultiBoot driver** `0x08003964`+`0x08004000` (`early_3964.c`, `early_4000.c`, IRQ handler, handshake, 32-bit bulk transfer; `0x0200EBF0` is an SDK `MultiBootParam`); **AGB SDK MultiBoot client library** `0x08004734`+`0x08004D6C` (`early_4734.c`, `early_4d6c.c` — the code pokeemerald ships as `multiboot.c`); **cooperative task engine** `0x08004FEC`-`0x08005654` (`early_4fec.c`, `early_5228.c`, `early_55b0.c`: 64 slots of 0x90 bytes at `0x03002790` in 5 priority groups, resume PC/SP consumed by the ARM switcher at `0x08000234`); **task position/draw helpers** `0x080058E4`+`0x08005C4C` (`early_58e4.c`, `early_5c4c.c`; `Task.unk44` is the parent index; ROM task-type table at `0x0872FF30`); **sprite draw/update family + on-screen tests + task idle loop** `0x08005D9C` (`early_5d9c.c`); **SIO MULTI-PLAY link driver** `0x08006464`+`0x08006CD4`+`0x08006D18`+`0x08007004` (`early_6464.c`, `early_6cd4.c`, `early_6d18.c`, `early_6e8c.c`, `early_7004.c`; `0x03004DA0` is a 0x4D4-byte session block with a `u16[4][30]` send ring at +0x1C and a `u16[4][4][30]` receive ring at +0x110). **14 functions remain in asm**, each with a diagnosed reason rather than a bare skip — see §6.4 |
| 7 | `0x08007300-0x080CFA4B` | ~0xC8700 | **Game code + rodata (Thumb)** | ~2,650 Thumb BL targets, thumb-pointer tables throughout; interleaved rodata (pointer tables 122 runs ≥8 entries, e.g. 43-entry table @0x0803EC48). Tail (`0x080CF94C-0x080CFA4B`) is the m4a/mp2k XCMD handler block, named in #29 via the 12-entry jump table @`0x0860A3E8` (matches katam/pokeemerald `gXcmdTable` 1:1, `ply_xxx` at indices 0 AND 3) + per-handler `MusicPlayerTrack` field offsets: `ply_xxx` `0x080CF94C`, `ply_xwave` `0x080CF960`, `ply_xtype` `0x080CF9A8`, `ply_xatta` `0x080CF9BC`, `ply_xdeca` `0x080CF9D0`, `ply_xsust` `0x080CF9E4`, `ply_xrele` `0x080CF9F8`, `ply_xiecv` `0x080CFA0C`, `ply_xiecl` `0x080CFA18`, `ply_xleng` `0x080CFA24`, `ply_xswee` `0x080CFA38`. The old `0x080CFA40` segment boundary cut `ply_xswee` in half (its tail was seg 8's "unidentified SDK helper" `gUnk_080cfa40`, 0 BL callers because XCMD handlers are table-dispatched only); boundary moved to `0x080CFA4C` in #29. **The whole m4a/mp2k engine occupies the region tail `0x080CD89C-0x080CFA4B` and is fully named in the symbol DB (issue #31) — see §8; carved out of the `game_code_and_rodata` segment into dedicated segments `m4a_1` (asm core) + `m4a` (C driver) in #52**. **The remaining bulk `0x080075B8-0x080CD89C` is clustered into 37 candidate modules in `docs/analysis/module-map.md` (issue #34) — see §9** |
| 8 | `0x080CFA4C-0x080CFA9B` | 0x50 | **SDK syscall wrappers + SoftReset (Thumb)** — named via SDK-order SWI table (`include/gba/syscall.h`, issue #27; finalized in #29). **Stays named asm forever, by design**: agbcc cannot emit a bare `svc N; bx lr` thunk from C, and the reset-helper segment starts at the odd address `0x080CFA7F` so the splitter emits it as raw data (lessons 4.2/4.3/4.14) — pret projects (katam `asm/libagbsyscall.s`) keep both as named asm and we do the same. Segment start moved from `0x080CFA40` to `0x080CFA4C` in #29 (the old boundary cut m4a's `ply_xswee` in half, see seg 7) | `svc N; bx lr` pairs: `0x080CFA50` ArcTan2 (`svc 0x0A`), `0x080CFA54` CpuFastSet (`svc 0x0C`), `0x080CFA58` CpuSet (`svc 0x0B`), `0x080CFA5C` Div (`svc 0x06`), `0x080CFA60` Mod (`svc 0x06`, returns remainder), `0x080CFA68` HuffUnComp (`svc 0x13`), `0x080CFA6C` LZ77UnCompVram (`svc 0x12`), `0x080CFA70` LZ77UnCompWram (`svc 0x11`), `0x080CFA74` MultiBoot mode=1 (`svc 0x25`), `0x080CFA7C` SoundDriverVSyncOff (`svc 0x28`); `0x080CFA4C` DummyFunc (`bx lr` stub, referenced from `0x080CEA48`); `0x080CFA80` **SoftReset** (IME=0, clear `0x03007FFA`, sp=`0x03007F00`, `svc 1; svc 0` = RegisterRamReset(r0) then reset — identical to katam's `SoftReset`; 2 BL callers `0x08000FF8`/`0x08008C40` kept as raw `.short` pairs, and the label is a `split_config.json` `extra_labels` data label, NOT a symbols.csv function: it carries no Thumb mark, so resolving the BLs against it would make ld insert an interworking veneer and shift every later section) |
| 9 | `0x080CFA9C-0x080CFDDB` | ~0x340 | **C library + SRAM driver (Thumb)** — SRAM driver **decompiled** (`src/agb_sram.c`, old_agbcc `-O1 -mthumb-interwork`); libc tail split to `asm/sdk_libc.s` (#24), fully named in #30. **The libc tail stays named asm forever, by design**: `_call_via_r0..lr` are gcc interworking shims reached by register-allocation-dependent `bl _call_via_rN` (lesson 3.4, all 15 variants exported); `__divsi3`/`__umodsi3`/`_div0` are libgcc routines that are hand-written *assembly* in gcc's own source tree (`lib1funcs.asm` — no C input produces them; the ROM bytes match the gcc 2.9 Thumb shapes instruction-for-instruction); the three trampolines are SDK glue with raw ARM branch words. No memcpy/memset copy loops exist in this range | SRAM driver `0x080CFA9C-0x080CFC2F`: `ReadSram_Core` `0x080CFA9C`, `ReadSram` `0x080CFAC0`, `WriteSram` `0x080CFB24`, `VerifySram_Core` `0x080CFB64`, `VerifySram` `0x080CFB94`, `WriteSramEx` `0x080CFBF8` (all byte-identical, linked from C); libc tail `0x080CFC30-0x080CFDDB`: `_call_via_r0..r7` (+ `_call_via_r8/r9/sl/fp/ip/sp/lr`), `__divsi3`/`__umodsi3`/`_div0`, three Thumb->ARM task trampolines `TaskSwitchTrampoline` `0x080CFDC4` / `TaskYieldTrampoline` `0x080CFDCC` / `TaskDispatchTrampoline` `0x080CFDD4` (`bx pc; nop; ARM b 0x08000234/58/88` — named in #30 after the task-helper semantics, see §6); the former `sub_080cfcfc` was a false positive — it is the `pop {pc}` tail of `__divsi3`'s `Ldiv0` path (`push {lr}; bl __div0; mov r0, #0; pop {pc}`, exactly gcc 2.9 `lib1funcs.asm`), whose only "rom-pointer" was a coincidental PCM word at `0x086DA494` inside `m4a_songs` (curated out via `tools/symdb.py` `FALSE_POSITIVES`, #30); fn table `0x0872EA04` = {ReadSram_Core, ReadSram, VerifySram_Core, VerifySram} (no xrefs) |
| 10 | `0x080CFF00-0x080CFFFF` | 0x100 | lib rodata — split to `asm/lib_rodata_fir_tables.s` (#24) | FIR/envelope-style coefficient tables consumed with signed relative indexing by **game code** at `0x080B7B14` and `0x080C2580-0x080C4FE8` (pool literals `0x080CFE2C`, `0x080CFE60`, `0x080CFEE4`, `0x080CFF52`, `0x080CFF70`, ... — several byte-granular); symmetric byte ramp peaking at 0x10 @0x080CFF00, `0x7FFF` saturation block @0x080CFF20. **NOT m4a tables**: the m4a engine (§8) never references this range — the earlier "m4a-family sound tables / mixer at 0x080C2xxx" hypothesis is corrected by issue #31 |
| 11 | `0x080D0000-0x08120000` | 0x50000 | Level/map & object tables | entropy 4.4-5.4, 31-48% zeros, few pointers; `faff/0000/0100` pattern tables (e.g. file `0xD00C0`) |
| 12 | `0x08120000-0x08330000` | ~0x210000 | Level data / uncompressed graphics / palettes, with embedded table zones | pointer clusters @0x08120000 (1066), 0x08150000 (962), 0x081A0000 (1140), 0x08200000 (1206), 0x08250000 (1526) |
| 13 | `0x083356E0-0x0834EEE8`, `0x08350AF8-0x083A85D4` | ~0x1B290 | **Sound sample data (PCM)** | pointed to by the sample index @`0x087E1D58` (24 pointers into `0x0833-0x0834`, 339 into `0x0835-0x083A`); high entropy (~7.2), zero-pct ~6-8% |
| 14 | `0x083D0000-0x085C0000` | 0x1F0000 | Compressed graphics (LZ77/RLE-class) | entropy 7.0-7.8 uniformly, near-zero pointer density |
| 15 | `0x085C0000-0x0872E9F7` | ~0x16EA00 | **m4a songs / sequences + engine rodata** — engine tables **extracted** (#51): `asm/m4a_engine_rodata.s` (`0x0860A140-0x0860A418`) + `asm/m4a_song_table.s` (`0x0860B430-0x0860C678`); song data stays `.incbin` (`data/m4a_songs.s` / `m4a_song_tracks.s` / `m4a_songs_2.s`, extraction is #36) | engine rodata block `0x0860A140-0x0860A418` (§8.3: gMPlayJumpTableTemplate, gScaleTable/gFreqTable/gCgb*/gNoiseTable, gPcmSamplesPerVBlankTable, gCgb3Vol, gClockTable, gXcmdTable — 12 entries ending `0x0860A418`); gMPlayTable @`0x0860B430`; song table @`0x0860B460`: **579** `(header, u16 ms, u16 me)` entries, 0x1218 bytes, ending at the first song header `0x0860C678` (an empty 0-track header used by 250 filler entries; all 579 header ptrs are ≥ `0x0860C678`, max `0x0872E9EC`; the pre-#51 "0x338 bytes / 103 entries" figure undercounted — entries continue uniformly to `0x0860C678`); first real song header @`0x0870F504`, bytes `08 00 00 80` + track ptr `0x0860A418` = valid m4a header; tail pointers `0x0860ACB8`,`0x0872E800` @0x0872E9F0 |
| 16 | `0x0872E9F8-0x0872EA01` | 10 | `SRAM_V112` string | ASCII @0x0872E9F8 (`53 52 41 4D 5F 56 31 31 32 00`), save-type marker |
| 17 | `0x0872EA04-0x0872EA13` | 16 | SRAM driver function table | 4 Thumb ptrs `0x080CFA9D, 0x080CFAC1, 0x080CFB65, 0x080CFB95` |
| 18 | `0x08730000-0x08760000` | 0x30000 | **Asset metadata / index zone** | >30k in-ROM pointers; targets spread across segs 7,11,12,15 and self-referential @0x0873-0x0876 (2186+1176+1616+2129 self pointers) |
| 19 | `0x08760000-0x087E1D57` | ~0x181D58 | Song tail / misc audio data **+ multiboot child images** | song headers/tracks referenced from seg 15 (e.g. `0x0870F504`); moderate entropy 6.5-7.3. Contains multiboot child-program images: the link/multiboot sender at `0x08007C5C-0x08007E5x` loads blob pointers `0x087954C0` and `0x087C0A4C` (pools @`0x08007CDC/0x08007CEC`) plus `0x0876B1FC`/`0x0876F690` (@`0x08007E40/0x08007CD8`); each image embeds its own copy of the m4a driver — Thumb code clusters with `SOUND_INFO_PTR`/`ID_NUMBER` literals at `0x08777800+`, `0x0879F2E0+`, `0x087CA834+` are those embedded drivers, NOT the main game's (whose engine is §8) |
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
  `0x080CFDE4-0x080CFDE8` (4 B — **not padding**: it is the interworking veneer's
  literal word, `0x08005655` = Thumb pointer to the task dispatcher `0x08005654`,
  emitted as `.word sub_08005654+1`),
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
| Interworking | ARM→Thumb only via `bx reg` with odd literal (`0x08000311` AgbInit, `0x08007301` AgbMain, `0x08005655` veneer target); Thumb↔Thumb `bl` throughout |

**Conclusion: KNiDL is a Thumb-compiled game** (~99.9% of in-ROM code bytes are Thumb),
unlike its ARM-compiled successor KATAM. For this repo this means: default `agbcc`
(Thumb) for `src/`, `agbcc_arm` only for the crt0/ISR/task units, and `old_agbcc` is the
likely match for the SDK lib/SRAM units (`0x080CF9xx` zone).

The committed symbol database (section 7) applies the same census method with a
corrected BL decoder and records **5,194 Thumb entries** (plus the 7 ARM functions
covering the three ARM zones above) — a superset of the 3,149-entry floor measured
here: the ad-hoc scan undercounted BL targets (its decoder dropped `bl` pairs whose
suffix halfword is even, i.e. whenever `target - site - 4` has bit 1 clear) and missed
pointer-only entries with non-push leaf shapes.

## 4. Boot flow trace

```
BIOS -> 0x08000000: b 0x080000C0                     ; header entry, ARM state
0x080000C0: mov r0,#0x12 ; msr CPSR_fc,r0            ; -> IRQ mode
0x080000C8: ldr sp,=0x03007F60                       ; IRQ stack (lit @0x08000104)
0x080000CC: mov r0,#0x1F ; msr CPSR_fc,r0            ; -> System mode
0x080000D4: ldr sp,=0x03007EC0                       ; SYS stack (lit @0x08000100)
0x080000D8-0x080000E0: str 0x08000108 -> [0x03007FFC]; install ROM master ISR
0x080000E4-0x080000EC: mov lr,pc ; bx 0x08000311     ; -> AgbInit (Thumb) 0x08000310
0x080000F0-0x080000F8: mov lr,pc ; bx 0x08007301     ; -> AgbMain (Thumb) 0x08007300
0x080000FC: b 0x080000C0                             ; loop if AgbMain returns
```

Naming note (issue #33): the crt0 ARM entry at `0x080000C0` is `Start`; the
Thumb C main loop at `0x08007300` is `AgbMain` (SDK/pret convention). The ROM's
prologue has no `__gccmain` call, which gcc 2.9 inserts into any function
literally named `main` — proof the original source did not use that name.

`AgbInit` (`0x08000310-0x080008E7`, **decompiled to `src/agb_init.c`**, issue #28) memory setup — all via SWI wrappers, no open-coded loops:

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

`AgbMain` (`0x08007300-0x080075B7`, **decompiled to `src/main.c`**, issue #33)
is the game's top-level state machine: an infinite loop dispatching on the u16
game state `gUnk_030023D8` through a 23-entry jump table at `0x08007328`.
Boot order of operations:

1. `gUnk_030023D8 = 0`, then `sub_080b7800()` — one-time init before the loop.
2. **State 0**: `sub_0800b44c()` → state 1.
3. **State 1**: if `gUnk_03000B00 == 0` (cell zeroed by AgbInit) call
   `sub_080091ac()` → state 3.
4. **State 3**: `sub_080096e0()` → state 4, and `gUnk_03002150 = 3`
   (`gUnk_03002150` holds the "return-to" state consumed by state 7).
5. **State 4**: `sub_0800b920(); sub_0800b4a8(); sub_0800b514();` (no state
   change — those callees advance `gUnk_030023D8` themselves).
6. **State 5**: normal frame pump — `sub_0800b5dc()` then
   `while (state == 5) { sub_0800b5dc(); sub_08007624(); }`; if the flag
   `gUnk_02007FC0` is set instead, park in state 7 with return-state 5.
7. **State 7**: if `gUnk_02007FC0` call `sub_080100ac()`, clear the flag,
   then `state = gUnk_03002150` (resume).

Other states: 2→3 re-entry; 6 (as 5 but via `sub_0800b628()` → 8);
8/9/17/18/19 busy-pump `sub_0800791c()`/`sub_0800783c()` while the state
holds; 10 (`gUnk_03001F30` selects `sub_0805b110()` vs `sub_0800b628()`) → 5;
11 `sub_080c6260()` → 12; 12 (`sub_080cd330()` unless `gUnk_03001F30 == 1` or
`gUnk_03002150 == 20`, then `sub_080c6420()`) → 0; 13 `sub_08007f9c()`;
14/15/16 `sub_080ba354()`; 20 clears `gUnk_02005588[0..3]`, sets
`gUnk_02007D48[0..3] = 1`, runs `sub_080022bc(); sub_08022f50();`, pumps
`sub_08008a00()` → return-state 20; 21 `sub_0800b4a8()`, sets `gUnk_02007FC0`
→ 5; 22 `sub_080cacf0()`. States > 22 spin on the dispatch read.

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

## 6.4 game_code_early: the 14 functions still in asm (issue #32)

Every one is structurally correct C whose remaining diff is register allocation
or an optimizer-placement decision, diagnosed from `agbcc -da` dumps rather than
guessed. Ranges, byte deltas and the specific blocker:

| Range | Fn | Δ | Blocker |
|---|---|---|---|
| `0x08001CC8-0x08001FD0` | affine-object emitter | 8 | `regmove` only re-targets to operand 2 when the chain root is a hard-register copy (lesson 3.35) — unreachable here |
| `0x08002378-0x08002668` | link state machine | ~20 | the ROM's `&gUnk_0300244C` preheader load is a GCSE/PRE insertion; ours lands one block later (3.55) |
| `0x080031B8-0x08003484` | SE slot allocator | 36 | pure register permutation over the last third |
| `0x08003888-0x08003964` | LinkInit | 4 | missing `*movhi_insn` scratch+copy; cse rewrote the constant into a copy from the byte-store value register |
| `0x08004984-0x08004D6C` | MultiBootMain | — | gcc keeps `0x04000120` in a callee-saved register across two loops where the ROM re-loads the pool word |
| `0x08005654-0x080058E4` | TaskFree/TaskCreate | 28+4 | the ROM re-materialises SImode −1 at three sites while keeping only r4/r5 callee-saved |
| `0x08005ACC-0x08005C4C` | on-screen test + gfx upload | 9+6+2 | a 16-bit load feeding a commutative SImode op proves ≥2 uses (3.73) |
| `0x08006AC8-0x08006CD4` | link ring push/pop | 8+140 | hoisted zero lands after the giv init; giv-discovery order decides which counter spills |
| `0x08006D28-0x08006E8C` | LinkVBlankIntr | 153 | three-way register rotation driven by a counter whose priority (refs 5 / live 292) puts it last |
| `0x08006E9C-0x08007004` | LinkReceiveStep | 271 | the ROM carries three extra base pseudos in high registers and re-loads the count per iteration |

Several were diagnosed before the 3.75 recipe correction and are tuned for
`--old2`; re-tuning them under `--newpb` is the obvious next attempt. That
correction already closed `sub_08005228` (the task engine's per-frame run loop)
with no source change at all.

## 6. Notes / open questions

- SWI numbering follows the **SDK/libagbsyscall order**, not the retail-BIOS order: the two agree for 0x00-0x07, 0x10-0x1F and 0x25+, but differ for 0x08-0x0F (e.g. `0x0B` = CpuSet here, Sqrt on a retail BIOS; `0x0A` = ArcTan2 here, GetBiosChecksum on retail). `0x080CFA54`/`0x080CFA58` are the SDK's thunks for CpuFastSet/CpuSet in that order. Full verified table and thunk map: `include/gba/syscall.h` (issue #27; identification method in docs/lessons-learned.md §3.12).
- Segments 18-20 (asset metadata, sample index) are indexed but not yet named; their semantics belong to the graphics/sound loaders and should be resolved when those loaders are decompiled. Partly answered by the module map (§9): seg 18 is the behaviour-table store (2,562 pointer-table entries dispatching seg-7 functions, plus the 266-entry task-type table at its head), and seg 20 is indexed by the level/room builder.
- One isolated Thumb `bl` pair was detected at `0x080D1B1E -> 0x080315E0` inside the seg-11 data zone (single occurrence; possibly a small code overlay or coincidence — investigate when decompiling the level/object table loaders).
- Cooperative task system (seg 4, split in #24). Behavioral reading of the four ARM
  helpers (names are inferences from control flow, to be confirmed when the Thumb-side
  scheduler is decompiled):
  - `sub_08000234` switch-to-task: saves the caller's `sp` to **`gTaskBaseSp`**
    (`0x03004C94`), installs the task stack (`sp = r2`), restores callee-saved regs,
    forces bit 0 of `r1` and jumps (`bx r1`) into the Thumb task entry.
  - `sub_08000258` yield-back: saves current `sp`/`lr`/`r0` to **`gTaskSavedSp`**
    (`0x03002470`), **`gTaskSavedLr`** (`0x030026F8`), **`gTaskSavedR0`**
    (`0x030025E0`), restores `sp` from `gTaskBaseSp`, returns via a popped word.
  - `sub_08000288` dispatcher call: passes `*gCurTaskIdx` (`0x0300248C`) as `r0`
    through the ARM veneer to the game-side routine `0x08005654`, then restores the
    base stack as above.
  - `sub_080002A8` halt check: if `gTaskSavedR0 != -1` return; else if
    `gTaskFlagsTable[gCurTaskIdx]` (`0x030025F0 + idx*4`) is nonzero, loop forever.
- The three Thumb->ARM trampolines at the tail of `sdk_libc` each decode as
  `bx pc; nop; ARM b <target>` — i.e. they let Thumb code reach task helpers 1-3.
  Named in #30 after the helper semantics above: `TaskSwitchTrampoline`
  `0x080CFDC4` (ARM word `0xEAFCC119` -> `0x08000234`, 1 BL caller),
  `TaskYieldTrampoline` `0x080CFDCC` (`0xEAFCC120` -> `0x08000258`, 11,063 BL
  sites — the game-wide cooperative yield), `TaskDispatchTrampoline`
  `0x080CFDD4` (`0xEAFCC12A` -> `0x08000288`, 232 BL sites). The trailing
  ARM branch words are emitted as raw halfwords in `asm/sdk_libc.s`; their decode
  is documented here.
- `lib_misc[0]` (`0x080CFE20`, named `gSramIdString`) is the ASCII string
  `"AGB  KIRBY"`; save-init code at `0x080B7AF8` writes those 10 bytes to SRAM
  (`0x0E000000`) via `WriteSramEx`. The rest of `lib_misc`/`lib_rodata_fir_tables`
  are coefficient tables consumed by **game code** at `0x080B7B14` and
  `0x080C2580-0x080C4FE8`: those functions load window bases (e.g. `0x080CFE60`,
  `0x080CFEE4`, `0x080CFF52`) into pools and index them with signed offsets
  (`ldrsh` index + byte load), which explains pointer-like values landing
  mid-table. Ascending 24-bit step triplets (`fd770a`, `fd811e`, ...) look like
  pitch/step-increment ladders. **They are NOT m4a driver tables** — the m4a
  engine (mapped in §8, issue #31) never references this range; canonical
  per-table names await decompilation of their game-side consumers.

Generated summaries (not committed): strings/pointers/lz77/zeros/isa scans as described in §1.

## 7. Symbol database (committed, machine-readable)

**Location:** `docs/analysis/symbols.csv` (one record per function) and
`docs/analysis/callgraph.csv` (caller → callee edges). Regenerate and validate
with `make symbols` (Docker-wrapped; requires `baserom.gba`). Generation is
deterministic — regenerating on an unmodified ROM reproduces both files
byte-for-byte.

**symbols.csv** columns:

| column | meaning |
|---|---|
| `vma` | entry address (GBA cart VMA; file offset = vma − 0x08000000) |
| `size` | upper bound: distance to the next accepted entry, capped at 4 KiB (includes trailing literal pools / padding) |
| `isa` | `thumb` or `arm` |
| `evidence` | how the entry was discovered, strongest first: `bl-target` (reached by a decoded `bl`), `rom-pointer` (referenced by a word in the ROM; bit 0 set for Thumb, clear for ARM), `prologue-scan` (curated ARM-zone split), `curated` (hand-verified entry with no in-ROM reference — dead m4a SDK exports from `EXTRA_THUMB_ENTRIES`, issue #31). Combined values use `+`. |
| `name` | `sub_08xxxxxx` for unknowns; canonical SDK/m4a/libc names where previously validated in this repo (`asm/crt0.s`, `src/agb_sram.c`, `asm/sdk_libc.s`) or canonical from sibling projects (SWI thunks per GBATEK numbering) |

Current contents: 5,241 functions (5,234 Thumb + 7 ARM across the three ARM zones
of §3), 19,364 call-graph edges (+9 rows/edges in #29: the curated m4a XCMD
handlers, accepted via the KNOWN_SYMBOLS bypass — they are table-dispatched only
and open with `ldr r0, [r1, #0x40]`, which the strict pointer-candidate prologue
filter rejects by design; +31 rows in #31: the m4a engine map of §8 — 21
previously-hidden entries with bl/pointer evidence accepted via KNOWN_SYMBOLS,
plus 10 dead SDK exports injected via the curated `EXTRA_THUMB_ENTRIES` list
with evidence kind `curated`). In `callgraph.csv`,
`caller` `0x00000000` means the reference site is outside any known function
(rodata table); `site` is the address of the `bl` pair / pointer word, and
`count` the number of such sites (for `bl` edges, aggregated per caller/callee).

**Method** (tools/symdb.py): candidates = BL-target census (Thumb `bl` pairs
decoded at every even offset of the §3 code span — ARMv4T encoding
`hw1[10:0]:hw2[10:0]:0`, sign at bit 22; the one ARM `bl` is decoded from the
`arm_code` segments only) ∪ bit0-set ROM pointers into the span (whole-ROM
word scan). Candidates are validated by prologue shape: pointer-only
candidates must start with `push {.., lr}` or an immediate terminator; BL
targets additionally pass with any body reaching an unconditional terminator
within 0x100 bytes. Validation harness `tools/symdb_check.py` re-checks
structure, coverage (≥ 3,149 Thumb + the 3 ARM zones) and ~20 deterministic
random entries against a fresh dual-view objdump disassembly.

**Known limitations** (fine to consume, fix later):
- Functions reached only by fallthrough or intra-function `b` (never `bl`,
  never a pointer) are absent — e.g. the IRQ helper at `0x0800151C` behind the
  `bx lr` default handler at `0x08001518`.
- Sizes are next-entry upper bounds; decompiled modules must derive real
  end addresses from their own analysis.
- ~35 BL targets whose shape validates nowhere were rejected; most look like
  data, a few may be real oddly-shaped leaves.
- Of the three `bx pc` Thumb→ARM trampolines at `0x080CFDC4/0x080CFDCC/0x080CFDD4`
  (ARM branches back to task helpers 1-3, see §6), all three carry `bl-target`
  evidence in the database; the ARM helpers themselves carry `prologue-scan`
  evidence from the curated §3 split. (An earlier revision of this section listed
  wrong addresses for the second/third trampoline and claimed only the first was
  BL-reachable — corrected in #24 against `docs/analysis/callgraph.csv`, which
  records hundreds of `bl 0x080CFDCC/DD4` call sites.)

## 8. m4a/mp2k sound engine map (issue #31)

The game uses Nintendo's m4a (a.k.a. mp2k/"Sappy") sound engine. The engine
code occupies the tail of `game_code_and_rodata`:
**`0x080CD89C-0x080CFA4B`**, immediately followed by `DummyFunc` (`0x080CFA4C`,
the m4a no-op used for the default CGB hooks) at the head of
`sdk_swi_wrappers`. Everything below is named in `docs/analysis/symbols.csv`
(generated from `tools/symdb.py` KNOWN_SYMBOLS; per-function evidence lives in
the KNOWN_SYMBOLS comments). Game-side sound glue (e.g. `sub_080cd330`, called
once from AgbMain, and the sample-set users of seg 20) is NOT part of the
engine; the engine's first byte is `umul3232H32` at `0x080CD89C`.

### 8.1 Identification method

- **Hard anchors:** every m4a build references the BIOS sound-info pointer
  `SOUND_INFO_PTR = 0x03007FF0` and the magic `ID_NUMBER = 0x68736D53`
  ("Smsh") from literal pools. In the main code span these literals cluster
  exclusively in `0x080CD918-0x080CF7D0`.
- **Jump table:** `MPlayJumpTableCopy` (`0x080CDD3C`, `movs r1, #36`) copies
  the 36-entry `gMPlayJumpTableTemplate` @`0x0860A140`, whose slots map
  sequence commands 0xB1..0xCF one-for-one to handlers — the same layout as
  pokeemerald's template, including ply_fine defaults in the slots that
  `MPlayExtender` fills at runtime (ply_memacc [8], ply_xcmd [28]) and the
  tail entries [31]=TrackStop, [32]=FadeOutBody, [33]=TrkVolPitSet,
  [34]=RealClearChain, [35]=SoundMainBTM.
- **Byte-identical tables:** `gScaleTable` (`0x0860A1D0`), `gFreqTable`
  (`0x0860A284`) and `gClockTable` (`0x0860A3B4`) match pokeemerald's tables
  byte-for-byte — same engine revision.
- **Function order** matches pokeemerald `m4a_1.s` + `m4a.c`, so pret names
  carry over directly; every function was additionally verified by body shape
  (register/field offsets against `m4a_internal.h` structs).

### 8.2 Code layout

| Range | Content |
|---|---|
| `0x080CD89C-0x080CE51F` | **asm core** (pret `m4a_1.s` equivalent; hand-scheduled, stays asm): `umul3232H32`, `SoundMain`, `SoundMainRAM` (ROM image `0x080CD930-0x080CDD2F`; `m4aSoundInit` CpuSet-copies 0x400 bytes to IWRAM `0x03007150` = `gSoundMainRAM_Buffer`, and `SoundMain` tail-jumps to `0x03007151`; contains an embedded ARM-mode inner mixer loop entered via `adr r1; bx r1` at `0x080CD936`), `SoundMainBTM`, `RealClearChain`, `ply_fine`, `MPlayJumpTableCopy`, byte-fetch helpers (`ld_r3_r2`/`chk_adr_r2`/`ld_r3_tp_adr_i`/`ld_r3_tp_adr` — descriptive names; the check variants zero r3 for implausible addresses, a HAL/SDK hardening absent from pokeemerald), `ply_goto/patt/pend/rept/prio/tempo/keysh/voice/volu/pan/bend/bendr/lfodl/modt/tune/port`, `m4aSoundVSync` (`0x080CDF00`, DMA1/2 FIFO restart), `MPlayMain` (`0x080CDF4C`, pointer-installed into `soundInfo->func` by MPlayOpen — pool word `0x080CDF4D` @`0x080CED10`), `TrackStop`, `ChnVolSetAsm`, `ply_note` (`0x080CE228`; `SoundInit` stores `soundInfo->plynote = 0x080CE229`), `ply_endtie`, `ClearModM_asm`, `ply_lfos`, `ply_mod`, `MidiKeyToFreq`. Three tiny `bx r3` call shims stay `sub_080cdcce`/`sub_080cdd72`/`sub_080ce1a4` (no canonical names). |
| `0x080CE520-0x080CFA4B` | **C driver** (pret `m4a.c` equivalent; compiler verdict from #53: **`old_agbcc -O2 -mthumb-interwork`**, NOT the -O1 SDK-zone default — see lesson 3.15): part 1 `0x080CE520-0x080CEFB3` **decompiled** (`src/m4a_c1.c`, issue #53, byte-exact): `MPlayContinue`/`MPlayFadeOut` (internal bodies; the public `m4aMPlayContinue` `0x080CE740` / `m4aMPlayFadeOut` `0x080CE778` are thin wrappers), `m4aSoundInit` (called from AgbInit), `m4aSoundMain`, `m4aSongNumStart/StartOrChange/StartOrContinue/Stop/Continue`, `m4aMPlayAllStop/AllContinue/FadeOutTemporarily/FadeIn/ImmInit`, `MPlayExtender`, `MusicPlayerJumpTableCopy` (`0x080CE930`, dead 4-byte `swi 0x2A; bx lr` BIOS thunk from `asm("swi 0x2A")`, exactly as in katam's m4a.c — carved out of MPlayExtender's old 0x11C size in #53), `ClearChain`, `Clear64byte`, `SoundInit`, `SampleFreqSet`, `m4aSoundMode`, `SoundClear`, `m4aSoundVSyncOff/On`, `MPlayOpen`, `MPlayStart`, `m4aMPlayStop`, `FadeOutBody`, `TrkVolPitSet`. Part 2 `0x080CEFB4-0x080CF587` **decompiled** (`src/m4a_cgb.c`, issue #54, byte-exact): `MidiKeyToCgbFreq`, `CgbOscOff`, `CgbModVol`, `CgbSound`, plus the dead-export `m4aMPlayTempoControl` the linker kept at `0x080CF554`. Part 3 `0x080CF588-0x080CFA4B` **decompiled** (`src/m4a_ctrl.c`, issue #55, byte-exact): `m4aMPlayVolumeControl/PitchControl/PanpotControl`, `ClearModM` (the C twin of asm-core `ClearModM_asm` `0x080CE468`), `m4aMPlayModDepthSet/LFOSpeedSet`, `ply_memacc` (18-op bounds-checked jump table, conditional ops tail-call `ply_goto` through `gMPlayJumpTable[1]` via `_call_via_r2`), `ply_xcmd` (`gXcmdTable` dispatch, pool `0x080CF948` = `0x0860A3E8`) and the twelve XCMD handlers `ply_xxx..ply_xswee`. The whole driver zone is one upstream translation unit: all three parts use the same `old_agbcc -O2 -mthumb-interwork` recipe. Eleven functions here are/were dead SDK exports with zero in-ROM references (whole-object linking), injected into the symbol DB via `EXTRA_THUMB_ENTRIES` with evidence `curated`. Source shapes are pokeruby's `src/libs/m4a.c` generation (internal bodies + thin wrappers, ident locks present in source — in parts 2-3 the lock STORES survive in the ROM because gcc cannot disprove aliasing between `track[]` writes and `mplayInfo->ident`; in part 1 they were optimized away, lesson 3.15 — `NUM_MUSIC_PLAYERS`/`MAX_LINES` read as absolute-symbol addresses — `gNumMusicPlayers`=4/`gMaxLines`=0 via `split_config.json` `abs_symbols`); struct layouts are katam's `gba/m4a.h` variant (`PCM_DMA_BUF_SIZE` 1584, `pcmSamplesPerVBlank` s32 at offset 16, `ExtVolPit` at 60, SoundInfo 0xFB0). This build's bodies differ from pokeruby in: countdown-free ascending player loops, `m4aSoundInit` mode `0x0095F700`, no pokemon-cries code. |

### 8.3 Engine rodata (extracted in #51: `asm/m4a_engine_rodata.s` `0x0860A140-0x0860A418` + `asm/m4a_song_table.s` `0x0860B430-0x0860C678`, labels via `split_config.json` `extra_labels`; pointer tables resolve symbolically as `.word <fn>+1`)

| VMA | Symbol (pret name) | Notes |
|---|---|---|
| `0x0860A140` | `gMPlayJumpTableTemplate` | 36 Thumb pointers (see §8.1; [30] = `SampleFreqSet` as in pokeemerald) |
| `0x0860A1D0` | `gScaleTable` | byte-identical to pokeemerald |
| `0x0860A284` | `gFreqTable` | 12 words, byte-identical to pokeemerald |
| `0x0860A2B4` | `gPcmSamplesPerVBlankTable` | halfwords, indexed by freq-1 in `SampleFreqSet` |
| `0x0860A2CC` | `gCgbScaleTable` | used by `MidiKeyToCgbFreq` (pool `0x080CF054`) |
| `0x0860A350` | `gCgbFreqTable` | used by `MidiKeyToCgbFreq` (pool `0x080CF058`) |
| `0x0860A368` | `gNoiseTable` | used by `MidiKeyToCgbFreq` (pool `0x080CEFE8`) |
| `0x0860A3A4` | `gCgb3Vol` | identified in #51 vs pokeemerald `m4a_tables.c`: 16 NR32 channel-3 output-level bytes `00 00 60×4 40×4 80×4 20 20` (mute/25%/50%/75%/100%), indexed by 4-bit volume in `CgbSound` (pool `0x080CF508`) |
| `0x0860A3B4` | `gClockTable` | 49 gate-time bytes + 3 pad, byte-identical to pokeemerald |
| `0x0860A3E8` | `gXcmdTable` | 12 Thumb pointers (#29), ends `0x0860A418` (the #51 issue text's `0x0860A410` was off by two entries) |
| `0x0860B430` | `gMPlayTable` | 4 players × (info, tracks, count): BGM 8 tracks, SE1-SE3 6 tracks |
| `0x0860B460` | `gSongTable` | **579** `(header, u16 ms, u16 me)` entries, 0x1218 bytes, ends `0x0860C678` — corrected in #51 from "103 entries / ends 0x0860B797": entries continue uniformly (ms/me player indices 0-3) up to the first song header `0x0860C678` (an empty 0-track header, the filler target of 250 entries), and all 579 header pointers are ≥ `0x0860C678` |

### 8.4 RAM map (named via `tools/split_config.json` `data_symbols`)

| Address | Symbol | Source of the address |
|---|---|---|
| `0x030056D0` | `gSoundInfo` | `m4aSoundInit` pool `0x080CE5BC`; SoundInfo is 0xFB0 bytes (CpuSet fill word `0x050003EC`) |
| `0x03006680` | `gMPlayJumpTable` | `SoundInit` pool `0x080CEA4C` / `MPlayExtender` pool `0x080CE8F4` |
| `0x03006710` | `gCgbChans` | `m4aSoundInit` pool `0x080CE5C0` |
| `0x03006810/50/90/E0` | `gMPlayInfo_BGM/SE1/SE2/SE3` | `gMPlayTable[i].info` |
| `0x030068D0` | `gMPlayMemAccArea` | `m4aSoundInit` pool `0x080CE5D0` |
| `0x03006930/6BB0/6D90/6F70` | `gMPlayTrack_BGM/SE1/SE2/SE3` | `gMPlayTable[i].track` (track = 0x50 bytes) |
| `0x03007150` | `gSoundMainRAM_Buffer` | CpuSet dest pool `0x080CE5B4`; `SoundMain` jumps to `0x03007151` (pool `0x080CD920`) |
| `0x03007FF0` | `SOUND_INFO_PTR` | BIOS-defined sound-info pointer cell |

### 8.5 Decompilation plan (child issues)

Split follows pret precedent: the asm core stays hand-written asm forever
(`m4a_1.s` is not compiler output); the C driver decompiles to `m4a.c` with
**`old_agbcc -O2 -mthumb-interwork`** (verdict from #53 — the -O1 SDK-zone
guess was wrong for this zone, see lesson 3.15; validate per function with
`./tools/fnmatch.sh <start> <end> src/m4a_<x>.c --old2` before landing).
Suggested chunks: (a)
engine rodata extraction (§8.3, tables only — song data stays `.incbin` for
issue #36) — **done in #51**, (b) carve the asm core `0x080CD89C-0x080CE51F` into a dedicated
`asm/m4a_1.s`-style unit — **done in #52** (segments `m4a_1`
`0x080CD89C-0x080CE520` and `m4a` `0x080CE520-0x080CFA4C` carved out of
`game_code_and_rodata`, which now ends at `0x080CD89C`; the `m4a` segment
is the split-asm staging unit that (c)-(e) decompile), (c) `m4a.c` part 1 `0x080CE520-0x080CEFB3`
(init/song-number/MPlay API) — **done in #53** (`src/m4a_c1.c` +
`include/gba/m4a_internal.h`; the `m4a` split segment now covers only
`0x080CEFB4-0x080CFA4C`), (d) `m4a.c` part 2 `0x080CEFB4-0x080CF587`
(CGB: MidiKeyToCgbFreq/CgbOscOff/CgbModVol/CgbSound) — **done in #54**
(`src/m4a_cgb.c`; the split segment now covers only `0x080CF588-0x080CFA4C`),
(e) `m4a.c` part 3 `0x080CF588-0x080CFA4B` (track controls,
ply_memacc/ply_xcmd/ply_x*) — **done in #55** (`src/m4a_ctrl.c`; the last
`m4a` staging segment is gone, replaced by the `.m4a_ctrl` C section).
The engine is now fully decompiled or deliberately asm (`m4a_1.s` core).

## 9. Module map of the bulk game code (issue #34)

The seg-7 remainder `0x080075B8-0x080CD89C` (`0xC62E4`, 792.7 KiB, 4,950
functions) is clustered into **37 contiguous candidate modules** of 12-31 KiB
in `docs/analysis/module-map.md`, with the machine-readable table in
`docs/analysis/module-map.csv` (regenerate with `make modmap`,
`tools/modmap.py`; CI checks the CSV like it checks `symbols.csv`). The map
carries, per module, the VMA range, function/pointer-dispatch counts, anchor
tables, task types, cross-module call traffic, literal-pool references
(IWRAM/EWRAM/IO/far-ROM), a difficulty score and suggested batch boundaries;
child issues of #35 are created from it. Findings that belong in this document:

- **The ROM task-type table at `0x0872FF30` has 266 entries, and the second
  word of each 8-byte entry is the task body's Thumb entry point**, not a flag
  word: all 266 point at functions accepted by the symbol DB, the table stops
  at the first word that does not (`0x00130018` @`0x08730780`), and all 266
  entry points lie in seg 7. Class (priority-group) histogram: class 0 x3,
  1 x24, 2 x38, 3 x132, 4 x69. This supersedes the "u8 class, u32 flags"
  reading in §6 / `src/early_58e4.c` (`struct TaskType.unk04`), which came from
  #32 before the table was censused. Extent `0x0872FF30-0x0873077F` **straddles
  the `segments.txt` boundary at `0x08730000`** (starts in
  `gap_sram_driver_fn_table_asset_metadata_index`, ends inside
  `asset_metadata_index`) and deserves its own segment (companion to #36).
- **M17 (`0x08062584-0x080692FB`) is the task/actor field API and the player
  record.** Decompiled in #65. `gUnk_03002490` is a `struct Task *` holding the
  currently-running task; `gUnk_03002790[]` is the 64-entry, 0x90-byte task
  table (the `struct Task` first mapped in #32). `Task.unk88` is a context
  pointer that for actor tasks points into **`gUnk_03002170[]`, a 116-byte
  per-player record** (`sub_08064EB8` sets it); `Task.unk8C` points at the
  0x70-byte `struct Actor`, and `Actor.unk44` at its ROM `struct ActorDef`.
  Newly identified ROM tables referenced from this range:
  `0x0873E1E8` / `0x0873E220` (s16 pairs indexed by animation frame,
  `sub_08066DCC`); `0x0873E1F8` (4 s8 per frame - x offset, y offset, a flag,
  and the value stored into `Task.unk42`; `sub_080675E4`); `0x0873E3C8` (5 u16
  per state, indexed by `(Task.unk3C - 30) * 5` - x offset, y offset, facing
  multiplier, animation id and `Task.unk42`; `sub_08067DB0`); `0x08752BA8` (the
  `Task.unk38` graphics-descriptor table, one `struct TaskGfx *` per frame); and
  the 26-entry jump table at `0x080684D0` (switch over `PlayerState.unk0D`,
  values 0-25, adding a per-character constant to `Task.unk3C`). `0x02007D00`
  is a 5-word EWRAM "pending spawn" record: flag, task index (or -1), x, y,
  facing (`sub_080685EC`, `sub_0806865C`, `sub_08068950`, `sub_08068840`).
- **M18 (`0x080692FC-0x08070EBF`) is the player's own class-1 task bodies.**
  Decompiled in #64, in ten files (`docs/analysis/module-map.md` §6). It sits
  directly on top of M17's field API and is where the player-facing state
  machines live: input probing and the six directional decoders, the
  carried/riding movement block, the vehicle and star-ride state machines, the
  screen-transition fades, and the stage-enter/leave sequences. Four anchor
  tables drive it: `0x0873E5BC` (15 entries -> `0x0806A3AC-0x0806B3C4`),
  `0x0873E670` (10 -> `0x0806A8F4-0x0806AA98`), `0x0873EAA0`
  (8 -> `0x0806BF54-0x0806C930`) and `0x0873FB08`
  (26 -> `0x0806EF5C-0x08070E7C`). The range named **101 new cells** in
  `split_config.json` `data_symbols`: 73 in `0x0873xxxx` (per-state descriptor
  rows - several are 2-D, e.g. `gUnk_0873EAD8` is `u16[][4]` and
  `gUnk_0873D3B8` is `s16[][2]`, which is visible because the compiler leaves
  the `+ K` as a separate add instead of folding it into the load offset),
  19 in `0x0874Cxxx` and 4 in `0x08752xxx` (sprite/graphics descriptor lists
  reached through `Task.unk38`), plus `gUnk_0824A9E4` and four RAM cells.
  Transitions drive the display through the early zone as predicted: the only
  hardware-ish cell here is the DISPCNT shadow `gUnk_03001ED8`, masked to
  `0xE0FF` and re-ORed with a BG-enable pattern.
- **M36 (`0x080BDA2C-0x080C1FFB`) is the four-slot bomb-pass sub-game.**
  Decompiled in #66, in three files (`docs/analysis/module-map.md` §6). It is
  started by M35 (game-mode flow) as task type #95 and dispatched entirely
  through the 41-entry anchor table `0x08756668` on `Task.unk14`, with a second
  table `0x08756670` on `Task.unk15` and `0x087572CC` on `gUnk_02007D2C` for the
  results screen. The mode is a round-robin over four slots: `sub_080be04c`
  shuffles the four player ids into `gUnk_02006A10[]` (rotated so the local
  player `gUnk_03002360` lands at a fixed index) and picks the starter with
  `sub_08002ee8(gUnk_030023AC)`, i.e. over the number of players; `Task.unk34`
  is the slot whose turn it is and walks forwards while `Task.unk28 <= 2` and backwards otherwise,
  `Task.unk2C` is the speed level 0-6 that indexes the seven-entry beat-length
  table `gUnk_08756570`, and `Task.unk1C` counts beats until the level steps up
  (the limits are the three parallel byte tables at
  `0x0875665C`/`0x0875665F`/`0x08756662`, indexed by the mode selector
  `gUnk_02006168`, an `s8` that also picks the row of the pointer table
  `gUnk_08756650` and is copied into the score block by `sub_080c1f9c`).
  `sub_080bf0ac` is the button-timing judgement: five-byte records at
  `0x087565F4`, four thresholds compared against `Task.unk30`, result 2/1/0 into
  `Task.unk20`. `sub_080c061c` evaluates the hand-off flight as a 16.16 parabola
  `p0 + v*t + (a*t*t)/2` into `Task.unk4C`/`unk50`, with `p0` from
  `gUnk_08756798`/`gUnk_087567A0`, `v` from `gUnk_087567A8[c][b]` and the two
  coefficient rows from the pointer-pair table `gUnk_08756D3C[c][0..1]`.
  Elimination order is written into `gUnk_0200B044[]` (3 = still in) with the
  running rank in `gUnk_0200AFF0` and the out-mask `gUnk_0200AF10`, and
  `sub_080c1f9c` resets the four 60-byte score records at `gUnk_0201B0E0 + 0x18`
  and copies the mode selector into `gUnk_02016C40`.
- **M36's rodata lives at `0x08755DC0-0x087572CC`, inside the
  `asset_metadata_index` data segment, and is not code-adjacent.** The 81 cells
  the range needed are therefore ordinary `split_config.json` `data_symbols`
  absolutes, not `extra_labels`: 62 in `0x0875xxxx` (anchor tables, animation
  scripts reached through `Task.unk38`, the beat/threshold/coefficient tables
  above; several are 2-D - `gUnk_087567A8` is `s32[][3]`, `gUnk_08757178` and
  `gUnk_087571B8` are `s32[][4]`, `gUnk_08756D3C` is `s32 *[][2]`) plus 9 EWRAM
  cells in `0x0200xxxx`/`0x0201xxxx`. Two more census entries were fixed while
  reading the range: `0x080C0AFC` and `0x080C1F18` are false positives (words
  inside the compressed sound-sample blobs at `0x087D2CB8`/`0x087D3F04` that
  disassemble as a `bl`), and `0x080C05F0` (four `bl` callers) and `0x080C1820`
  (a dead export with its own pool) are real Thumb entries the scan missed -
  the two blind spots this document already records, seen once more.

- **The prologue scan and the `bl` scan each have a systematic blind spot in
  this zone, and #64 found seven instances.** Two artifacts account for all of
  them. (a) `.word 0xFFFFF000` in a literal pool always disassembles as
  `bl <pc + 0xFFE>`, inventing a function ~4 KiB further on: `0x08063DFE` and
  `0x080643A2` (#65), `0x0806F0E2` and `0x0806FC3E` (#64). For any
  `bl-target`-only symbol `S`, check whether the word at `S - 0x1002` is
  `0xFFFFF000` before believing it. A `b.n` that skips a mid-function pool
  (lessons §3.6) reads the same way: `0x0806FFF8`, `0x08070406`. (b) Because
  the game-code zone is compiled `-fprologue-bugfix`, a leaf function has **no
  `push {lr}`**, so the prologue scan cannot propose one and it is silently
  merged into its predecessor's size unless something points at it:
  `0x080694E0`, `0x0806ACF8`, `0x0806B40C`, `0x080702D8` and `0x08070454`
  were all recovered this way, three of them live code with real callers.
  Both classes are curated in `tools/symdb.py` (`FALSE_POSITIVES` /
  `EXTRA_THUMB_ENTRIES`) with the evidence in the comment. The practical rule:
  **a size in `symbols.csv` that does not tile with the next entry is a
  signal**, and the cheap check on any suspicious entry is whether its first
  instruction could run with the registers a caller would have set.
- **Seg 7 barely touches hardware.** Across 792 KiB its literal pools reference
  the `0x04000000` I/O block only 20 times; all display/DMA/scroll work goes
  through the early zone's IWRAM shadow cells. 3,288 of its 5,045 functions
  have no `bl` caller at all and are reachable only through ROM pointer tables
  (2,562 `ptr` edges originate in `asset_metadata_index`, seg 18) — for this
  region, `rom-pointer`-only evidence is the norm, not a red flag.
- **Seg 20 (`0x087E1D58`) is indexed by level code**, not only by the sound
  engine: the level/room builder module (`0x08021B18-0x0802969F`) loads it 21
  times, and its leaves point into seg 13 (`0x0836xxxx`) — per-room sample-bank
  selection.
- **The `lib_misc`/`lib_rodata_fir_tables` consumer predicted in §2 seg 10 is
  one module**, `0x080C1FFC-0x080C641F` (22 pool refs into `lib_misc`, 7 into
  the FIR tables, and it contains the `0x080C2580-0x080C4FE8` functions named
  there); the `gSramIdString` consumer at `0x080B7AF8` is the save module
  `0x080B6154-0x080B9D0B`, the only `WriteSramEx`/`ReadSram` caller in seg 7.
