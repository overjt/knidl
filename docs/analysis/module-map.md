# Module map — bulk game code `0x08007300-0x080CFA4C`

Subsystem identification pass over the ~800 KiB of game code that is still
labeled assembly (issue #34).  Its job is to turn one undifferentiated
`game_code_and_rodata` segment into a set of **coherent, self-contained,
contiguous slices** that can each become a child issue of #35, with enough
evidence attached that whoever picks a slice up knows what they are looking at
before they read a single instruction.

Everything here is derived from the committed symbol database (#22), the call
graph (#22), the labeled asm (#25), the semantics recovered in #32, and
`baserom.gba`, by `tools/modmap.py` (`make modmap`).  The module table is
machine-readable in `docs/analysis/module-map.csv` and its regeneration is
checked in CI, exactly like `symbols.csv` and the split outputs.

Related documents: `docs/analysis/rom-map.md` (ROM layout, §2 seg 7 is this
range, §6 the task system), `docs/decomp-loop.md` (per-function workflow),
`docs/lessons-learned.md` §3 (agbcc idioms — §3.19-§3.93 come from #32 and are
properties of the compiler, not of that zone).

## 1. Scope and coverage accounting

| | Range | Bytes | Functions |
|---|---|---|---|
| Issue #34 range | `0x08007300-0x080CFA4C` | `0xC874C` (801.8 KiB) | 5,045 |
| Already landed (not clustered) | `main` + 4 m4a segments | `0x2468` (9.1 KiB) | 95 |
| **Still in asm, clustered here** | `0x080075B8-0x080CD89C` | **`0xC62E4` (792.7 KiB)** | **4,950** |

Coverage is 100% **by construction, and asserted**: the modules are the cells
of a partition of the range, every function entry in `symbols.csv` belongs to
exactly one, and `tools/modmap.py` fails if the pieces do not tile the range
end to end (`coverage hole/overlap at ...`).  There are therefore no "unknown"
leftovers — but there *are* modules whose *name* is a guess; those are marked
with a `?` and listed in §7.  A module is a claim about a range, not about a
name.

The 42 rows below cover the range as: 37 modules to decompile + 4 m4a segments
+ `main`, in address order.

## 2. Method

`tools/modmap.py`, in six steps (thresholds are constants at the top of the
file; the docstring is the authoritative description):

1. **Nodes** — every `symbols.csv` function in the range.  Sizes run to the
   next entry, so the nodes tile the range exactly.
2. **Edges** — `bl` edges from `callgraph.csv`.  `ptr` edges are deliberately
   *not* unioned into the same graph: a pointer table can point anywhere, and
   unioning them collapses unrelated code into one component.  They are used
   separately, as anchors.
3. **Boundary cost** — for each gap between adjacent functions, the number of
   *local* `bl` edges straddling it (`|caller - callee| <= 0x8000`).  Distant
   calls are excluded on purpose: with a median intra-range call distance of
   `0xEF84` they are calls into engine-wide helpers and carry no locality
   information at all.
4. **Anchors** — runs of consecutive ROM words holding Thumb function
   pointers.  A run whose targets span less than `0x6000` bytes is one
   module's state/behaviour table; splitting one is charged 20 units, which
   keeps table-dispatched families whole.
5. **Segmentation** — an exact dynamic program that minimises total boundary
   cost subject to a module byte size in `[0x3000, 0x8000]`, run a second time
   at `[0x800, 0x2000]` for the suggested batches inside each module, with the
   module boundaries constrained to be a subset of the batch boundaries.
6. **Evidence** — per module: exact literal-pool references (decoded from
   every `ldr rN, [pc, #imm]`, so the pool words are read, not guessed),
   bucketed into I/O registers, IWRAM/EWRAM cells, and far ROM data zones;
   anchor tables; task-type entry points; cross-module call traffic; calls into
   the decompiled early zone, named by role; and a difficulty score.

**Why contiguous modules.** The ROM was linked from translation units, so a
real module *is* an address range; and `tools/carve.py`, the tool that lands
verified C, only accepts address ranges. A clustering that produced
address-interleaved sets of functions would score better on modularity metrics
and be useless in practice.

**Does the seam detection actually work?** 850 of the 4,949 gaps between adjacent
functions are straddled by no local call whatsoever, i.e. the code really is laid out in
call-cohesive runs. Of the 36 internal module boundaries the DP chose, **22
cost nothing at all** and 26 cost three or fewer crossing calls. The expensive
ones are honest and visible: M07|M08 (104), M10|M11 (95), M17|M18 (81),
M11|M12 (56) are places where one subsystem exceeded the 32 KiB module cap and
had to be cut in the middle — those pairs should be read as one subsystem in
two issues, and the detail blocks say so.

### Data quirks handled

* **`caller = 0x00000000` edges.** 2,698 of them, but only **8** are `bl`
  edges (sites past `symdb.py`'s 4 KiB size cap; repaired here by interval
  containment). The other 2,690 are `ptr` edges whose site is a ROM data word
  outside any function — those are not defects, they are the pointer tables,
  and their site address is the most valuable anchor in this whole pass.
* **Sizes that do not tile.** Only two gaps ≥ 64 bytes exist in the range
  (`0x08048FE8` + `0x204`, `0x08038F2A` + `0xAE`), i.e. the DB tiles the range
  almost perfectly. That also means interleaved rodata is *inside* some
  function's nominal size: expect the last "function" before a table to be
  bigger than its code, exactly as in #32.
* **Pointer-run splitting.** A one-word hole is tolerated when grouping
  pointer runs (dispatch tables carry the occasional NULL or count word);
  without it the task-type table shatters into three runs.

## 3. What the clustering found

### 3.1 The ROM task-type table is a 266-entry index of gameplay subsystems

`0x0872FF30` holds 8-byte entries `{u8 class; u8 pad[3]; u32 entry}`.  #32
read the second word as a flag word (`struct TaskType.unk04`, see
`src/early_58e4.c`); it is in fact the **task body's Thumb entry point**.
Every one of the 266 entries is a bit-0-set pointer to a function accepted by
the symbol DB, and the table stops dead at the first word that is not
(`0x00130018` at `0x08730780`).  All 266 entry points land inside this issue's
range.

* Extent `0x0872FF30-0x0873077F` (`0x850` bytes) — note this **straddles the
  `segments.txt` boundary at `0x08730000`**, starting in
  `gap_sram_driver_fn_table_asset_metadata_index` and ending inside
  `asset_metadata_index`.  A future segments.txt refinement should carve it out.
* Class histogram (the priority group of `src/early_4fec.c`'s 5 groups):
  class 0 x3, class 1 x24, class 2 x38, class 3 x132, class 4 x69.
* The three class-0 types (#0, #1, #2) all live in M02 — the top-level
  game-mode tasks.  The 22 consecutive class-4 types #238-#259 all live in M03
  (the UI/menu bank).  Types #221-#236 are all in M08.  Type IDs are grouped
  by subsystem, so a module's task-type list is a real fingerprint.

This is the anchor the issue text hoped for, and it is now machine-extracted
(`tools/modmap.py --report`, `task_types()`).

### 3.2 The bulk code touches almost no hardware registers

Across 792 KiB, the literal pools reference the `0x04000000` block **20
times** in total (five modules; mostly a DMA0 descriptor and a couple of
`REG_BGxHOFS`).  Everything else goes through the IWRAM shadow cells that the
early zone flushes (`gUnk_03001ED8` = DISPCNT shadow, the eight 16.16 BG
scroll shadows, the copy-request ring at `0x03000B80`, …).  Consequence for
decompilation: bulk modules need `struct`/shadow-cell declarations far more
than they need `include/gba/io_reg.h`, and a module that *does* poke a register
directly (M33, M34, M37) is doing something unusual and worth a second look.

### 3.3 Indirect calls are the dominant control-flow idiom

`TaskYieldTrampoline` (`0x080CFDCC`) is the single most-called function in the
ROM: **11,063 call sites**.  Second and third are the early-zone sprite
helpers `0x08006338` (2,021) and `0x080062C4` (932).  Inside the bulk range the
hottest callee is `0x0806395C` (825) — a three-instruction accessor on the
current task in M17.  Add 3,288 of 5,045 functions that are reachable *only*
through a ROM pointer table, and the shape of this code is clear: thousands of
small per-state functions, dispatched from tables, each yielding to the
scheduler.  This is why the behaviour banks (M20-M32) score difficulty 1-2
despite being 200 KiB: the functions are tiny (means of `0x42`-`0x97`), take
`struct Task *` implicitly through `gUnk_03002490`, and repeat the same shapes.

### 3.4 The asset-metadata zone is the behaviour-table store

Of the 4,846 `ptr` edges, 2,562 have their site in `0x08730000-0x0875FFFF`
(`asset_metadata_index`).  136 runs of ≥4 entries sit fully inside a single
module and are listed per module below as **anchor tables**; they are the best
starting point for naming individual functions, because a table with N entries
indexed by a state variable gives every one of its targets an ordinal
(`state 0 handler`, `state 1 handler`, …).  A few tables cross module
boundaries because the subsystem exceeded the size cap; the biggest is
`0x08731FA8` (71 entries, `0x0801050C-0x0801A07C`), which is why M04 and M05
are one subsystem.

### 3.5 Two data-zone identifications worth recording

* `0x087E1D58`, which rom-map §2 seg 20 calls the "end-of-ROM index (sample
  sets)", is referenced **21 times from M07** (level/room build). Its leaves
  point into `0x0836xxxx` (`sound_samples_2`). So it is indexed by *level*
  code: per-room sample/bank selection, not a sound-engine-private table.
* The FIR/envelope coefficient tables at `0x080CFF00` (`lib_rodata_fir_tables`,
  rom-map §2 seg 10) have exactly one consumer, **M37** (`0x080C1FFC`), with 22
  pool refs into `lib_misc` and 7 into the tables. rom-map predicted a consumer
  at `0x080C2580-0x080C4FE8`; M37 contains that range.

## 4. Module table

Generated by `python3 tools/modmap.py --markdown - --details`.  Difficulty is a
1-6 heuristic (mean/max function size, distinct RAM cells, jump-table
dispatches, pool density) — a planning aid, not a promise.

| # | Range | Size | Fns | Anchors | Difficulty | Candidate subsystem |
|---|-------|------|-----|---------|------------|---------------------|
| M01 | `0x08007300-0x080075B7` | 0.7 KiB | 1 | 0 | - | **done** - main |
| M02 | `0x080075B8-0x0800B91F` | 16.9 KiB | 110 | 1 | *** | game mode + screen/asset loader |
| M03 | `0x0800B920-0x08010357` | 18.6 KiB | 83 | 0 | **** | menu / UI task bank |
| M04 | `0x08010358-0x08017667` | 28.8 KiB | 67 | 0 | *** | player-character state bodies? |
| M05 | `0x08017668-0x0801A8C7` | 12.6 KiB | 23 | 0 | *** | player-character driver? |
| M06 | `0x0801A8C8-0x08021B17` | 28.6 KiB | 56 | 0 | ***** | terrain / collision query (pure leaf) |
| M07 | `0x08021B18-0x0802969F` | 30.9 KiB | 154 | 1 | ****** | level / room builder + tilemap upload |
| M08 | `0x080296A0-0x08030803` | 28.3 KiB | 153 | 2 | *** | camera / BG scroll + tilemap streaming |
| M09 | `0x08030804-0x0803627F` | 22.6 KiB | 60 | 0 | ***** | stage manager A |
| M10 | `0x08036280-0x0803CD5F` | 26.7 KiB | 41 | 0 | **** | stage script runner |
| M11 | `0x0803CD60-0x080449C7` | 31.1 KiB | 123 | 4 | ***** | stage support library |
| M12 | `0x080449C8-0x08047FE7` | 13.5 KiB | 22 | 0 | *** | large actor bank A |
| M13 | `0x08047FE8-0x0804CC7B` | 19.1 KiB | 27 | 0 | *** | large actor bank B |
| M14 | `0x0804CC7C-0x08053AF3` | 27.6 KiB | 84 | 2 | *** | stage manager B |
| M15 | `0x08053AF4-0x0805AFAB` | 29.2 KiB | 86 | 1 | *** | link multiplayer mode |
| M16 | `0x0805AFAC-0x08062583` | 29.5 KiB | 88 | 1 | *** | actor / effect support library B |
| M17 | `0x08062584-0x080692FB` | 27.4 KiB | 244 | 2 | *** | struct Task field API (actor core) |
| M18 | `0x080692FC-0x08070EBF` | 30.9 KiB | 256 | 4 | * | player-state task bodies (actor core part 2) - **landed (#64)** |
| M19 | `0x08070EC0-0x08078B67` | 31.2 KiB | 218 | 8 | ** | actor bank C (11 class-3 tasks) |
| M20 | `0x08078B68-0x0807F043` | 25.2 KiB | 390 | 20 | ** | enemy/object behaviour bank 1 |
| M21 | `0x0807F044-0x08082E67` | 15.5 KiB | 188 | 10 | * | enemy/object behaviour bank 2 |
| M22 | `0x08082E68-0x080860F7` | 12.6 KiB | 119 | 8 | * | enemy/object behaviour bank 3 |
| M23 | `0x080860F8-0x0808CCE7` | 27.0 KiB | 285 | 14 | *** | enemy/object behaviour bank 4 |
| M24 | `0x0808CCE8-0x0809000B` | 12.8 KiB | 157 | 8 | * | enemy/object behaviour bank 5 |
| M25 | `0x0809000C-0x08093F63` | 15.8 KiB | 121 | 4 | * | enemy/object behaviour bank 6 |
| M26 | `0x08093F64-0x080988F7` | 18.4 KiB | 140 | 4 | ** | enemy/object behaviour bank 7 |
| M27 | `0x080988F8-0x0809BA43` | 12.3 KiB | 140 | 3 | * | enemy/object behaviour bank 8 |
| M28 | `0x0809BA44-0x080A158F` | 22.8 KiB | 198 | 5 | * | enemy/object behaviour bank 9 |
| M29 | `0x080A1590-0x080A5643` | 16.2 KiB | 221 | 5 | ** | enemy/object behaviour bank 10 |
| M30 | `0x080A5644-0x080AA337` | 19.2 KiB | 130 | 2 | * | enemy/object behaviour bank 11 |
| M31 | `0x080AA338-0x080AE3BB` | 16.1 KiB | 121 | 2 | ** | enemy/object behaviour bank 12 |
| M32 | `0x080AE3BC-0x080B2FE7` | 19.0 KiB | 129 | 5 | * | enemy/object behaviour bank 13 |
| M33 | `0x080B2FE8-0x080B6153` | 12.4 KiB | 108 | 5 | *** | HUD / overlay effects? |
| M34 | `0x080B6154-0x080B9D0B` | 14.9 KiB | 106 | 2 | *** | save file / SRAM records + options |
| M35 | `0x080B9D0C-0x080BDA2B` | 15.3 KiB | 193 | 4 | *** | game-mode flow + link lobby |
| M36 | `0x080BDA2C-0x080C1FFB` | 17.5 KiB | 117 | 4 | * | sub-game / bonus mode? |
| M37 | `0x080C1FFC-0x080C641F` | 17.0 KiB | 82 | 1 | **** | FIR-coefficient effect engine |
| M38 | `0x080C6420-0x080CD89B` | 29.1 KiB | 110 | 4 | **** | intro / cutscene / ending sequences? |
| M39 | `0x080CD89C-0x080CE51F` | 3.1 KiB | 40 | 0 | - | **done** - m4a_1 |
| M40 | `0x080CE520-0x080CEFB3` | 2.6 KiB | 31 | 0 | - | **done** - m4a_c1 |
| M41 | `0x080CEFB4-0x080CF587` | 1.5 KiB | 4 | 0 | - | **done** - m4a_cgb |
| M42 | `0x080CF588-0x080CFA4B` | 1.2 KiB | 19 | 0 | - | **done** - m4a_ctrl |


## 5. Suggested decompilation order

Ordering rule: **leaf-heavy and cheap first** (`ext_deps` ascending, difficulty
ascending, `module-map.csv` has both columns), with one deliberate exception —
M17/M18, the `struct Task` field API, go first regardless, because every other
module calls them and their signatures decide how `struct Task` is spelled in
`include/`.  Getting that wrong once costs a rewrite of every bank.

| Wave | Modules | Bytes | Why this wave |
|---|---|---|---|
| **1. Actor API** | M17, M18 | 58 KiB | Highest fan-in in the ROM (M17 is called from 24 modules). Establishes `struct Task` accessors, the field offsets every bank uses, and the `gUnk_03002490` idiom. M18 is difficulty 1. |
| **2. Behaviour banks** | M19-M32, M36 | 282 KiB | Difficulty 1-2, **zero BL callers** (nothing depends on them), hundreds of tiny table-dispatched functions with repeating shapes. The bulk of the byte count and the safest parallel work: any number of agents can take one bank each. |
| **3. Support libraries** | M06, M11, M16, M04, M05 | 130 KiB | Called by the engine modules; M06 is a pure leaf (no outgoing calls at all) and is the single best first target for anyone wanting an isolated slice. |
| **4. Engine / stage managers** | M07, M08, M09, M10, M12, M13, M14, M15 | 198 KiB | The level, camera and stage machinery. Highest difficulty scores, densest pools, most jump tables; M07/M08 and M10/M11 are each one subsystem split across two issues. |
| **5. Modes, UI, save, effects** | M02, M03, M33, M34, M35, M37, M38 | 124 KiB | Reachable from `AgbMain`; needs the game-state cells and the save format. M34 is the only SRAM user; M37 is the only FIR-table consumer. |

Within a wave, take the modules in `module-map.csv` order of
`(difficulty, ext_deps, size)`. That ordering, ignoring the wave grouping, is
below; the filing order of the child issues applies the wave first and this
ordering inside it:

| # | Module | Size | Fns | Difficulty | Modules it calls | Modules calling it | Task types |
|---|--------|------|-----|-----------|------------------|--------------------|------------|
| 1 | M36 sub-game / bonus mode? | 0x45D0 | 117 | 1 | 2 | 1 | 1 |
| 2 | M25 enemy/object behaviour bank 6 | 0x3F58 | 121 | 1 | 3 | 0 | 10 |
| 3 | M27 enemy/object behaviour bank 8 | 0x314C | 140 | 1 | 4 | 0 | 5 |
| 4 | M22 enemy/object behaviour bank 3 | 0x3290 | 119 | 1 | 4 | 0 | 8 |
| 5 | M24 enemy/object behaviour bank 5 | 0x3324 | 157 | 1 | 4 | 0 | 11 |
| 6 | M21 enemy/object behaviour bank 2 | 0x3E24 | 188 | 1 | 4 | 0 | 9 |
| 7 | M30 enemy/object behaviour bank 11 | 0x4CF4 | 130 | 1 | 4 | 0 | 12 |
| 8 | M32 enemy/object behaviour bank 13 | 0x4C2C | 129 | 1 | 5 | 0 | 15 |
| 9 | M28 enemy/object behaviour bank 9 | 0x5B4C | 198 | 1 | 5 | 0 | 7 |
| 10 | M18 actor core part 2 + class-1 task bodies | 0x7BC4 | 256 | 1 | 10 | 18 | 31 |
| 11 | M26 enemy/object behaviour bank 7 | 0x4994 | 140 | 2 | 3 | 0 | 7 |
| 12 | M29 enemy/object behaviour bank 10 | 0x40B4 | 221 | 2 | 4 | 0 | 9 |
| 13 | M20 enemy/object behaviour bank 1 | 0x64DC | 390 | 2 | 5 | 0 | 21 |
| 14 | M31 enemy/object behaviour bank 12 | 0x4084 | 121 | 2 | 7 | 0 | 18 |
| 15 | M19 actor bank C (11 class-3 tasks) | 0x7CA8 | 218 | 2 | 7 | 4 | 11 |
| 16 | M34 save file / SRAM records + options | 0x3BB8 | 106 | 3 | 2 | 12 | 0 |
| 17 | M05 player-character driver? | 0x3260 | 23 | 3 | 3 | 10 | 0 |
| 18 | M35 game-mode flow + link lobby | 0x3D20 | 193 | 3 | 3 | 3 | 2 |
| 19 | M23 enemy/object behaviour bank 4 | 0x6BF0 | 285 | 3 | 3 | 0 | 10 |
| 20 | M04 player-character state bodies? | 0x7310 | 67 | 3 | 3 | 1 | 2 |
| 21 | M08 camera / BG scroll + tilemap streaming | 0x7164 | 153 | 3 | 4 | 3 | 17 |
| 22 | M02 game mode + screen/asset loader | 0x4368 | 110 | 3 | 6 | 29 | 5 |
| 23 | M16 actor / effect support library B | 0x75D8 | 88 | 3 | 6 | 10 | 10 |
| 24 | M33 HUD / overlay effects? | 0x316C | 108 | 3 | 7 | 8 | 9 |
| 25 | M12 large actor bank A | 0x3620 | 22 | 3 | 7 | 1 | 0 |
| 26 | M13 large actor bank B | 0x4C94 | 27 | 3 | 7 | 4 | 0 |
| 27 | M15 link multiplayer mode | 0x74B8 | 86 | 3 | 7 | 0 | 1 |
| 28 | M17 struct Task field API (actor core) | 0x6D78 | 244 | 3 | 8 | 24 | 2 |
| 29 | M14 stage manager B | 0x6E78 | 84 | 3 | 9 | 5 | 1 |
| 30 | M37 FIR-coefficient effect engine | 0x4424 | 82 | 4 | 5 | 2 | 1 |
| 31 | M03 menu / UI task bank | 0x4A38 | 83 | 4 | 6 | 2 | 22 |
| 32 | M38 intro / cutscene / ending sequences? | 0x747C | 110 | 4 | 6 | 3 | 7 |
| 33 | M10 stage script runner | 0x6AE0 | 41 | 4 | 11 | 1 | 0 |
| 34 | M06 terrain / collision query (pure leaf) | 0x7250 | 56 | 5 | 2 | 11 | 0 |
| 35 | M11 stage support library | 0x7C68 | 123 | 5 | 9 | 18 | 0 |
| 36 | M09 stage manager A | 0x5A7C | 60 | 5 | 13 | 13 | 1 |
| 37 | M07 level / room builder + tilemap upload | 0x7B88 | 154 | 6 | 8 | 34 | 1 |

### Child issues

One issue per module, created from this map with
`python3 tools/modmap.py --issues <dir>` and filed in the order below (each is a
sub-issue of #35, so the numbering ascends with the recommended order):

| Order | Issue | Module | Range | Size | Wave |
|-------|-------|--------|-------|------|------|
| 1 | #64 | M18 player-state task bodies (actor core part 2) - landed | `0x080692FC-0x08070EBF` | 30.9 KiB | 1 |
| 2 | #65 | M17 struct Task field API (actor core) | `0x08062584-0x080692FB` | 27.4 KiB | 1 |
| 3 | #66 | M36 sub-game / bonus mode? | `0x080BDA2C-0x080C1FFB` | 17.5 KiB | 2 |
| 4 | #67 | M25 enemy/object behaviour bank 6 | `0x0809000C-0x08093F63` | 15.8 KiB | 2 |
| 5 | #68 | M27 enemy/object behaviour bank 8 | `0x080988F8-0x0809BA43` | 12.3 KiB | 2 |
| 6 | #69 | M22 enemy/object behaviour bank 3 | `0x08082E68-0x080860F7` | 12.6 KiB | 2 |
| 7 | #70 | M24 enemy/object behaviour bank 5 | `0x0808CCE8-0x0809000B` | 12.8 KiB | 2 |
| 8 | #71 | M21 enemy/object behaviour bank 2 | `0x0807F044-0x08082E67` | 15.5 KiB | 2 |
| 9 | #72 | M30 enemy/object behaviour bank 11 | `0x080A5644-0x080AA337` | 19.2 KiB | 2 |
| 10 | #73 | M32 enemy/object behaviour bank 13 | `0x080AE3BC-0x080B2FE7` | 19.0 KiB | 2 |
| 11 | #74 | M28 enemy/object behaviour bank 9 | `0x0809BA44-0x080A158F` | 22.8 KiB | 2 |
| 12 | #75 | M26 enemy/object behaviour bank 7 | `0x08093F64-0x080988F7` | 18.4 KiB | 2 |
| 13 | #76 | M29 enemy/object behaviour bank 10 | `0x080A1590-0x080A5643` | 16.2 KiB | 2 |
| 14 | #77 | M20 enemy/object behaviour bank 1 | `0x08078B68-0x0807F043` | 25.2 KiB | 2 |
| 15 | #78 | M31 enemy/object behaviour bank 12 | `0x080AA338-0x080AE3BB` | 16.1 KiB | 2 |
| 16 | #79 | M19 actor bank C (11 class-3 tasks) | `0x08070EC0-0x08078B67` | 31.2 KiB | 2 |
| 17 | #80 | M23 enemy/object behaviour bank 4 | `0x080860F8-0x0808CCE7` | 27.0 KiB | 2 |
| 18 | #81 | M05 player-character driver? | `0x08017668-0x0801A8C7` | 12.6 KiB | 3 |
| 19 | #82 | M04 player-character state bodies? | `0x08010358-0x08017667` | 28.8 KiB | 3 |
| 20 | #83 | M16 actor / effect support library B | `0x0805AFAC-0x08062583` | 29.5 KiB | 3 |
| 21 | #84 | M06 terrain / collision query (pure leaf) | `0x0801A8C8-0x08021B17` | 28.6 KiB | 3 |
| 22 | #85 | M11 stage support library | `0x0803CD60-0x080449C7` | 31.1 KiB | 3 |
| 23 | #86 | M08 camera / BG scroll + tilemap streaming | `0x080296A0-0x08030803` | 28.3 KiB | 4 |
| 24 | #87 | M12 large actor bank A | `0x080449C8-0x08047FE7` | 13.5 KiB | 4 |
| 25 | #88 | M13 large actor bank B | `0x08047FE8-0x0804CC7B` | 19.1 KiB | 4 |
| 26 | #89 | M15 link multiplayer mode | `0x08053AF4-0x0805AFAB` | 29.2 KiB | 4 |
| 27 | #90 | M14 stage manager B | `0x0804CC7C-0x08053AF3` | 27.6 KiB | 4 |
| 28 | #91 | M10 stage script runner | `0x08036280-0x0803CD5F` | 26.7 KiB | 4 |
| 29 | #92 | M09 stage manager A | `0x08030804-0x0803627F` | 22.6 KiB | 4 |
| 30 | #93 | M07 level / room builder + tilemap upload | `0x08021B18-0x0802969F` | 30.9 KiB | 4 |
| 31 | #94 | M34 save file / SRAM records + options | `0x080B6154-0x080B9D0B` | 14.9 KiB | 5 |
| 32 | #95 | M35 game-mode flow + link lobby | `0x080B9D0C-0x080BDA2B` | 15.3 KiB | 5 |
| 33 | #96 | M02 game mode + screen/asset loader | `0x080075B8-0x0800B91F` | 16.9 KiB | 5 |
| 34 | #97 | M33 HUD / overlay effects? | `0x080B2FE8-0x080B6153` | 12.4 KiB | 5 |
| 35 | #98 | M37 FIR-coefficient effect engine | `0x080C1FFC-0x080C641F` | 17.0 KiB | 5 |
| 36 | #99 | M03 menu / UI task bank | `0x0800B920-0x08010357` | 18.6 KiB | 5 |
| 37 | #100 | M38 intro / cutscene / ending sequences? | `0x080C6420-0x080CD89B` | 29.1 KiB | 5 |

### Sizing the child issues

#32 was 182 functions / 27 KiB and needed ~16 batches; these modules are
sized to the same order of magnitude on purpose (12-31 KiB, `[0x3000,
0x8000]`), so one module ≈ one #32-sized issue.  Each detail block below ends
with **suggested batches** — the fine-grained segmentation, 132 of them across
the range, each a contiguous run of functions at a cheap seam. Use them as the
batch boundaries inside an issue; they are what `tools/carve.py` will accept,
and start-adjacent carves keep `segments.txt` tidy (see #35's carve note).

## 6. Per-module detail

Cross-references use module ids. "Depends on" and "Called from" are `bl` call
counts; segment names (`early_*`, `sdk_libc`, …) are the already decompiled
early zone and the SDK tails. "Pool references" counts literal-pool words, so
`asset_metadata_index x252` means 252 pool words pointing into the asset zone.
### M02 `0x080075B8-0x0800B91F` - game mode + screen/asset loader

* **Size** 16.9 KiB (`0x4368`), 110 functions (15 reachable only through pointer tables), mean `0x9c`, largest `0x37c`, pool words 19.7% of bytes.
* **Difficulty** 3/6 - 161 distinct RAM cells, 7 jump-table dispatches, 4 functions >= `0x200`.
* **Seam cost** 0 in / 26 out (local `bl` edges crossing the boundary).
* **Why** class-0 task types #0-2; game-state cells; LZ77UnCompVram x21, HuffUnComp x6; called from AgbMain.
* **Task types** 5 (class 0 x3, class 4 x2): #0, #1, #2, #237, #265.
* **Anchor tables** `0x0873078C` 4 entries -> `0x080083B0-0x08008558`.
* **Calls into the decompiled early zone** BG/fade/blend reset x83, frame driver + RNG + blend x52, VRAM transfer queue + sprite buckets x42, sound/SE x13, SIO multi-play x10, SIO multiboot x9.
* **Named helpers** LZ77UnCompVram x21, CpuSet x13, TaskYieldTrampoline x13, HuffUnComp x6, Div x4, LZ77UnCompWram x2.
* **Called from** M03 x57, M34 x47, M07 x39, M38 x31, M01 x18.
* **Depends on** early_1fd0 x83, early_2b04 x52, sdk_swi_wrappers x46, early_1518 x42, M07 x18.
* **Pool references** IWRAM x333, EWRAM x316, compressed_graphics x75, VRAM x39, asset_metadata_index x25, game_code_and_rodata x9, song_tail_misc_audio x8, early_58e4 x2.
* **Known RAM cells touched** current game state (main dispatch) x25, DISPCNT shadow x20, requested/next game state x10, per-player keys pressed x8, BG3HOFS shadow (16.16) x7, BG3VOFS shadow (16.16) x7.
* **Suggested batches** `0x080075B8` (7 fns), `0x08007E04` (41 fns), `0x08009AA0` (62 fns).

### M03 `0x0800B920-0x08010357` - menu / UI task bank

* **Size** 18.6 KiB (`0x4a38`), 83 functions (43 reachable only through pointer tables), mean `0xe4`, largest `0x40c`, pool words 16.0% of bytes.
* **Difficulty** 4/6 - 93 distinct RAM cells, 2 jump-table dispatches, 7 functions >= `0x200`.
* **Seam cost** 26 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 22 class-4 task types #238-259; key auto-repeat + decimal digit buffer; called from AgbMain and the save module.
* **Task types** 22 (class 4 x22): #238, #239, #240, #241, #242, #243, #244, #245, #246, #247, #248, #249, #250, #251, #252, #253, #254, #255, #256, #257, #258, #259.
* **Calls into the decompiled early zone** frame driver + RNG + blend x66, VRAM transfer queue + sprite buckets x63, task position/draw x32, BG/fade/blend reset x27, SIO multiboot x7, sound/SE x6.
* **Named helpers** TaskYieldTrampoline x33, TaskDispatchTrampoline x18, LZ77UnCompWram x3.
* **Called from** M34 x4, M01 x2.
* **Depends on** early_2b04 x66, early_1518 x63, M02 x57, sdk_libc x51, game_code_early_080011ac_08002378_08003110 x33.
* **Pool references** IWRAM x304, EWRAM x262, asset_metadata_index x58, compressed_graphics x37, VRAM x24, game_code_and_rodata x17, early_58e4 x14, early_5c4c x10.
* **Known RAM cells touched** DISPCNT shadow x26, keys newly pressed x20, current game state (main dispatch) x11, BG3VOFS shadow (16.16) x10, decimal digit buffer, [5] = sign/flag x9, auto-repeat first delay (14) x7.
* **Suggested batches** `0x0800B920` (13 fns), `0x0800CA10` (37 fns), `0x0800EA0C` (33 fns).

### M04 `0x08010358-0x08017667` - player-character state bodies?

* **Size** 28.8 KiB (`0x7310`), 67 functions (64 reachable only through pointer tables), mean `0x1b7`, largest `0x7c0`, pool words 7.3% of bytes.
* **Difficulty** 3/6 - 15 distinct RAM cells, 0 jump-table dispatches, 22 functions >= `0x200`.
* **Seam cost** 0 in / 1 out (local `bl` edges crossing the boundary).
* **Why** 64/67 functions pointer-dispatched from the 71-entry table @0x08731FA8 (shared with the next module); TaskYieldTrampoline x1340, the densest in the ROM; sprite draw x261.
* **Task types** 2 (class 1 x1, class 2 x1): #91, #92.
* **Calls into the decompiled early zone** sprite draw/update x261, VRAM transfer queue + sprite buckets x11, sound/BGM x8, frame driver + RNG + blend x3, task position/draw x3, SIO multi-play x2.
* **Named helpers** TaskYieldTrampoline x1340, TaskDispatchTrampoline x6.
* **Called from** M05 x5.
* **Depends on** sdk_libc x1346, early_5d9c x261, early_1518 x11, M17 x8, early_3110 x8.
* **Pool references** IWRAM x105, asset_metadata_index x60, early_58e4 x48, early_5c4c x47, game_code_and_rodata x4, EWRAM x3, level_graphics_palettes x2, VRAM x1.
* **Known RAM cells touched** DISPCNT shadow x3, current game state (main dispatch) x2, per-player keys pressed x1.
* **Suggested batches** `0x08010358` (23 fns), `0x0801201C` (13 fns), `0x08013E38` (18 fns), `0x08015758` (13 fns).

### M05 `0x08017668-0x0801A8C7` - player-character driver?

* **Size** 12.6 KiB (`0x3260`), 23 functions (20 reachable only through pointer tables), mean `0x230`, largest `0xdfc`, pool words 7.5% of bytes.
* **Difficulty** 3/6 - 31 distinct RAM cells, 0 jump-table dispatches, 5 functions >= `0x200`.
* **Seam cost** 1 in / 0 out (local `bl` edges crossing the boundary).
* **Why** task type #3; 20/23 pointer-dispatched; called by every stage module, calls the bank above.
* **Calls into the decompiled early zone** sprite draw/update x71, VRAM transfer queue + sprite buckets x24, frame driver + RNG + blend x6, sound/BGM x2, sound/SE x2.
* **Named helpers** TaskYieldTrampoline x553, TaskDispatchTrampoline x3, LZ77UnCompWram x1.
* **Called from** M14 x21, M18 x10, M13 x8, M12 x7, M11 x6.
* **Depends on** sdk_libc x556, early_5d9c x71, early_1518 x24, game_code_early_080011ac_08002378_08003110 x20, early_2b04 x6.
* **Pool references** IWRAM x101, asset_metadata_index x25, early_58e4 x13, game_code_and_rodata x11, m4a_songs x11, EWRAM x10, VRAM x10, early_5c4c x4.
* **Known RAM cells touched** BLDALPHA hi shadow x2, BLDALPHA lo shadow x2, BG3HOFS shadow (16.16) x1, BG3VOFS shadow (16.16) x1, BLDCNT hi shadow x1, BLDCNT lo shadow x1.
* **Suggested batches** `0x08017668` (2 fns), `0x08018498` (5 fns), `0x08019000` (16 fns).

### M06 `0x0801A8C8-0x08021B17` - terrain / collision query (pure leaf)

* **Size** 28.6 KiB (`0x7250`), 56 functions (1 reachable only through pointer tables), mean `0x20a`, largest `0xb42`, pool words 20.5% of bytes.
* **Difficulty** 5/6 - 82 distinct RAM cells, 0 jump-table dispatches, 16 functions >= `0x200`.
* **Seam cost** 0 in / 16 out (local `bl` edges crossing the boundary).
* **Why** zero outgoing calls except ArcTan2 x3; 1303 pool refs into the 0x030054E0-0x030055B0 room descriptor; 334 into the 0x100-stride index tables @0x087328F0-0x087339F0.
* **Named helpers** ArcTan2 x3.
* **Called from** M07 x21, M18 x20, M17 x14, M14 x8, M19 x6.
* **Depends on** M07 x6, sdk_swi_wrappers x3, M02 x1.
* **Pool references** IWRAM x1303, asset_metadata_index x334, EWRAM x3.
* **Suggested batches** `0x0801A8C8` (7 fns), `0x0801BCAC` (17 fns), `0x0801DC88` (9 fns), `0x0801FC48` (23 fns).

### M07 `0x08021B18-0x0802969F` - level / room builder + tilemap upload

* **Size** 30.9 KiB (`0x7b88`), 154 functions (24 reachable only through pointer tables), mean `0xcd`, largest `0x87a`, pool words 21.3% of bytes.
* **Difficulty** 6/6 - 161 distinct RAM cells, 15 jump-table dispatches, 17 functions >= `0x200`.
* **Seam cost** 16 in / 104 out (local `bl` edges crossing the boundary).
* **Why** writes BG map bases 0x06002000-0x06008000; CpuSet+CpuFastSet x25; 15 jump tables; EWRAM working set 0x02005000-0x0200B080; indexes 0x087E1D58; drives the next module x195.
* **Task types** 1 (class 4 x1): #3.
* **Anchor tables** `0x08732614` 7 entries -> `0x08023634-0x08024DA4`.
* **Calls into the decompiled early zone** VRAM transfer queue + sprite buckets x25, sprite draw/update x12, sound/BGM x8, BG/fade/blend reset x4, sound/SE x4, task engine x3.
* **Named helpers** CpuSet x13, CpuFastSet x12, Div x2.
* **Called from** M19 x64, M18 x30, M10 x29, M11 x28, M25 x25.
* **Depends on** M08 x195, M02 x39, sdk_swi_wrappers x27, early_1518 x25, M06 x21.
* **Pool references** IWRAM x957, EWRAM x537, asset_metadata_index x57, game_code_and_rodata x52, sample_set_index x21, VRAM x16, level_graphics_palettes x3.
* **Known RAM cells touched** current game state (main dispatch) x6, per-player keys held x4, per-player keys pressed x4, DISPCNT shadow x1.
* **Suggested batches** `0x08021B18` (36 fns), `0x080238A4` (26 fns), `0x0802589E` (55 fns), `0x08027750` (37 fns).

### M08 `0x080296A0-0x08030803` - camera / BG scroll + tilemap streaming

* **Size** 28.3 KiB (`0x7164`), 153 functions (42 reachable only through pointer tables), mean `0xbd`, largest `0x844`, pool words 12.2% of bytes.
* **Difficulty** 3/6 - 82 distinct RAM cells, 1 jump-table dispatches, 10 functions >= `0x200`.
* **Seam cost** 104 in / 11 out (local `bl` edges crossing the boundary).
* **Why** all eight BGxHOFS/VOFS 16.16 shadows; 15 class-3 task types #221-236; task position/draw x18.
* **Task types** 17 (class 3 x15, class 4 x2): #4, #221, #222, #223, #224, #225, #226, #227, #228, #229, #230, #231, #232, #233, #234, #235, #236.
* **Anchor tables** `0x087328A0` 7 entries -> `0x0802D38C-0x0802E3AC`; `0x087328D8` 6 entries -> `0x08030254-0x08030604`.
* **Calls into the decompiled early zone** VRAM transfer queue + sprite buckets x19, task position/draw x18, sprite draw/update x14, frame driver + RNG + blend x13.
* **Named helpers** TaskYieldTrampoline x190, TaskDispatchTrampoline x18.
* **Called from** M07 x195, M09 x4, M33 x3.
* **Depends on** sdk_libc x208, early_1518 x19, early_58e4 x18, M09 x15, early_5d9c x14.
* **Pool references** IWRAM x765, EWRAM x67, asset_metadata_index x46, game_code_and_rodata x27, VRAM x23, early_58e4 x21, early_5d9c x11, compressed_graphics x6.
* **Known RAM cells touched** BG3VOFS shadow (16.16) x5, BG2VOFS shadow (16.16) x4, BG3HOFS shadow (16.16) x4, BG2HOFS shadow (16.16) x3, BG1HOFS shadow (16.16) x2, BG1VOFS shadow (16.16) x2.
* **Suggested batches** `0x080296A0` (50 fns), `0x0802B62C` (25 fns), `0x0802D38C` (30 fns), `0x0802F38C` (48 fns).

### M09 `0x08030804-0x0803627F` - stage manager A

* **Size** 22.6 KiB (`0x5a7c`), 60 functions (27 reachable only through pointer tables), mean `0x182`, largest `0x888`, pool words 10.5% of bytes.
* **Difficulty** 5/6 - 62 distinct RAM cells, 17 jump-table dispatches, 13 functions >= `0x200`.
* **Seam cost** 11 in / 3 out (local `bl` edges crossing the boundary).
* **Why** task type #5 (class 1); 17 jump tables; calls the stage support library x151 and the script runner x23.
* **Task types** 1 (class 1 x1): #5.
* **Calls into the decompiled early zone** sprite draw/update x89, frame driver + RNG + blend x7, sound/BGM x1, task engine x1.
* **Named helpers** TaskYieldTrampoline x159.
* **Called from** M14 x16, M08 x15, M13 x4, M18 x4, M07 x2.
* **Depends on** sdk_libc x159, M11 x151, early_5d9c x89, M10 x23, M07 x15.
* **Pool references** IWRAM x297, EWRAM x138, asset_metadata_index x74, game_code_and_rodata x39, VRAM x9, level_object_tables x2, sample_set_index x1.
* **Known RAM cells touched** current game state (main dispatch) x2, per-player keys held x1.
* **Suggested batches** `0x08030804` (33 fns), `0x08032688` (13 fns), `0x080343C0` (14 fns).

### M10 `0x08036280-0x0803CD5F` - stage script runner

* **Size** 26.7 KiB (`0x6ae0`), 41 functions (35 reachable only through pointer tables), mean `0x297`, largest `0x1000`, pool words 7.6% of bytes.
* **Difficulty** 4/6 - 39 distinct RAM cells, 21 jump-table dispatches, 14 functions >= `0x200`.
* **Seam cost** 3 in / 95 out (local `bl` edges crossing the boundary).
* **Why** 21 jump tables; 35/41 pointer-dispatched; task engine x14; calls the stage support library x266.
* **Calls into the decompiled early zone** sprite draw/update x167, task engine x14, sound/SE x8, VRAM transfer queue + sprite buckets x3, frame driver + RNG + blend x1, sound/BGM x1.
* **Named helpers** TaskYieldTrampoline x355, LZ77UnCompWram x1, TaskDispatchTrampoline x1.
* **Called from** M09 x23.
* **Depends on** sdk_libc x356, M11 x266, early_5d9c x167, M07 x29, M16 x27.
* **Pool references** IWRAM x393, asset_metadata_index x50, game_code_and_rodata x39, EWRAM x31, VRAM x2, level_object_tables x2.
* **Known RAM cells touched** DISPCNT shadow x4, current game state (main dispatch) x1, frames left to wait x1.
* **Suggested batches** `0x08036280` (6 fns), `0x080371F0` (10 fns), `0x08038FE8` (8 fns), `0x0803AFCC` (17 fns).

### M11 `0x0803CD60-0x080449C7` - stage support library

* **Size** 31.1 KiB (`0x7c68`), 123 functions (46 reachable only through pointer tables), mean `0x102`, largest `0x85e`, pool words 9.8% of bytes.
* **Difficulty** 5/6 - 50 distinct RAM cells, 18 jump-table dispatches, 16 functions >= `0x200`.
* **Seam cost** 95 in / 56 out (local `bl` edges crossing the boundary).
* **Why** fan-in from every stage module (266+171+165+159+151); hot leaves 0x08040B40 x195 / 0x080413A4 x165; link-aware SE gate 0x0803E34C; 43-entry rodata table @0x0803EC48.
* **Anchor tables** `0x0873B430` 11 entries -> `0x08041438-0x08042128`; `0x0873B46C` 12 entries -> `0x08042580-0x08044288`; `0x0873B4AC` 7 entries -> `0x080415C8-0x08042328`; `0x0873B500` 4 entries -> `0x08043A88-0x08044470`.
* **Calls into the decompiled early zone** sprite draw/update x132, VRAM transfer queue + sprite buckets x26, frame driver + RNG + blend x4, sound/SE x4, SIO multi-play x2, task engine x2.
* **Named helpers** TaskYieldTrampoline x136, CpuSet x1.
* **Called from** M10 x266, M13 x171, M12 x165, M14 x159, M09 x151.
* **Depends on** sdk_libc x136, early_5d9c x132, M07 x28, early_1518 x26, M16 x25.
* **Pool references** IWRAM x450, asset_metadata_index x52, EWRAM x43, game_code_and_rodata x28, level_object_tables x11, level_graphics_palettes x5, VRAM x3.
* **Known RAM cells touched** DISPCNT shadow x2, frames left to wait x1, per-player keys held x1, per-player keys pressed x1.
* **Suggested batches** `0x0803CD60` (38 fns), `0x0803EAF8` (41 fns), `0x08040A44` (24 fns), `0x080429FC` (20 fns).

### M12 `0x080449C8-0x08047FE7` - large actor bank A

* **Size** 13.5 KiB (`0x3620`), 22 functions (21 reachable only through pointer tables), mean `0x275`, largest `0x694`, pool words 7.8% of bytes.
* **Difficulty** 3/6 - 10 distinct RAM cells, 4 jump-table dispatches, 11 functions >= `0x200`.
* **Seam cost** 56 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 22 functions, 21 pointer-dispatched, mean 0x275; TaskYieldTrampoline x244; sprite draw x89.
* **Calls into the decompiled early zone** sprite draw/update x89, frame driver + RNG + blend x3, sound/SE x2.
* **Named helpers** TaskYieldTrampoline x244.
* **Called from** M11 x1.
* **Depends on** sdk_libc x244, M11 x165, early_5d9c x89, M16 x41, M07 x14.
* **Pool references** IWRAM x145, asset_metadata_index x46, EWRAM x18, game_code_and_rodata x12, level_graphics_palettes x2.
* **Known RAM cells touched** DISPCNT shadow x1.
* **Suggested batches** `0x080449C8` (13 fns), `0x0804676C` (9 fns).

### M13 `0x08047FE8-0x0804CC7B` - large actor bank B

* **Size** 19.1 KiB (`0x4c94`), 27 functions (23 reachable only through pointer tables), mean `0x2c2`, largest `0x1000`, pool words 7.6% of bytes.
* **Difficulty** 3/6 - 16 distinct RAM cells, 7 jump-table dispatches, 12 functions >= `0x200`.
* **Seam cost** 0 in / 1 out (local `bl` edges crossing the boundary).
* **Why** 27 functions, 23 pointer-dispatched, mean 0x2c2; TaskYieldTrampoline x317; palette-fade calls; level_graphics_palettes refs x24.
* **Calls into the decompiled early zone** sprite draw/update x144, VRAM transfer queue + sprite buckets x35, frame driver + RNG + blend x7, palette fade x2, task engine x2.
* **Named helpers** TaskYieldTrampoline x317.
* **Called from** M18 x7, M09 x4, M10 x3, M14 x1.
* **Depends on** sdk_libc x317, M11 x171, early_5d9c x144, M16 x76, early_1518 x35.
* **Pool references** IWRAM x216, asset_metadata_index x27, level_graphics_palettes x24, game_code_and_rodata x13, VRAM x9, EWRAM x8, level_object_tables x4.
* **Known RAM cells touched** DISPCNT shadow x1.
* **Suggested batches** `0x08047FE8` (10 fns), `0x08049F98` (13 fns), `0x0804B8A0` (4 fns).

### M14 `0x0804CC7C-0x08053AF3` - stage manager B

* **Size** 27.6 KiB (`0x6e78`), 84 functions (73 reachable only through pointer tables), mean `0x150`, largest `0xa4a`, pool words 10.5% of bytes.
* **Difficulty** 3/6 - 24 distinct RAM cells, 7 jump-table dispatches, 14 functions >= `0x200`.
* **Seam cost** 1 in / 0 out (local `bl` edges crossing the boundary).
* **Why** task type #6 (class 1); anchor tables @0x0873B664 (25) and @0x0873B77C (13); calls the stage support library x159.
* **Task types** 1 (class 1 x1): #6.
* **Anchor tables** `0x0873B664` 25 entries -> `0x0804E78C-0x08050630`; `0x0873B77C` 13 entries -> `0x080509EC-0x080537DC`.
* **Calls into the decompiled early zone** sprite draw/update x140, frame driver + RNG + blend x15, SIO multi-play x9, VRAM transfer queue + sprite buckets x8, task position/draw x4.
* **Named helpers** TaskYieldTrampoline x399, TaskDispatchTrampoline x9, Div x2.
* **Called from** M13 x15, M10 x13, M09 x2, M11 x2, M12 x2.
* **Depends on** sdk_libc x408, M11 x159, early_5d9c x140, M05 x21, M16 x18.
* **Pool references** IWRAM x373, asset_metadata_index x116, game_code_and_rodata x62, EWRAM x21, early_58e4 x16, early_5d9c x16, VRAM x4, early_5c4c x3.
* **Known RAM cells touched** current game state (main dispatch) x1.
* **Suggested batches** `0x0804CC7C` (10 fns), `0x0804E78C` (39 fns), `0x080506DC` (22 fns), `0x0805268C` (13 fns).

### M15 `0x08053AF4-0x0805AFAB` - link multiplayer mode

* **Size** 29.2 KiB (`0x74b8`), 86 functions (78 reachable only through pointer tables), mean `0x15b`, largest `0x670`, pool words 8.8% of bytes.
* **Difficulty** 3/6 - 24 distinct RAM cells, 5 jump-table dispatches, 17 functions >= `0x200`.
* **Seam cost** 0 in / 7 out (local `bl` edges crossing the boundary).
* **Why** SIO multi-play (early_6464) x162 - by far the heaviest link user in the bulk; task type #7 (class 1); 49-entry anchor table @0x0873B928.
* **Task types** 1 (class 1 x1): #7.
* **Anchor tables** `0x0873B928` 49 entries -> `0x08053B40-0x0805AE94`.
* **Calls into the decompiled early zone** sprite draw/update x163, SIO multi-play x162, VRAM transfer queue + sprite buckets x16, frame driver + RNG + blend x14, task engine x6, palette fade x2.
* **Named helpers** TaskYieldTrampoline x592, TaskDispatchTrampoline x51, CpuSet x1.
* **Depends on** sdk_libc x643, early_5d9c x163, early_6464 x162, game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654 x20, early_1518 x16.
* **Pool references** IWRAM x222, asset_metadata_index x92, early_58e4 x61, early_5d9c x50, game_code_and_rodata x50, EWRAM x30, VRAM x8, level_graphics_palettes x2.
* **Known RAM cells touched** DISPCNT shadow x3.
* **Suggested batches** `0x08053AF4` (12 fns), `0x08054330` (25 fns), `0x0805614C` (18 fns), `0x08057CE0` (17 fns), `0x08059C28` (14 fns).

### M16 `0x0805AFAC-0x08062583` - actor / effect support library B

* **Size** 29.5 KiB (`0x75d8`), 88 functions (53 reachable only through pointer tables), mean `0x156`, largest `0x924`, pool words 7.9% of bytes.
* **Difficulty** 3/6 - 38 distinct RAM cells, 3 jump-table dispatches, 16 functions >= `0x200`.
* **Seam cost** 7 in / 2 out (local `bl` edges crossing the boundary).
* **Why** fan-in 76+41+27+25+18 from the stage modules; TaskYieldTrampoline x1052; touches each BG shadow once.
* **Task types** 10 (class 3 x8, class 4 x2): #81, #82, #83, #84, #85, #86, #87, #88, #89, #90.
* **Anchor tables** `0x0873DBE4` 22 entries -> `0x0805B4D8-0x0805BE3C`.
* **Calls into the decompiled early zone** sprite draw/update x200, frame driver + RNG + blend x17, task position/draw x14, VRAM transfer queue + sprite buckets x11, BG/fade/blend reset x7, sound/BGM x3.
* **Named helpers** TaskYieldTrampoline x1052, TaskDispatchTrampoline x5, LZ77UnCompWram x1.
* **Called from** M13 x76, M12 x41, M10 x27, M11 x25, M14 x18.
* **Depends on** sdk_libc x1057, early_5d9c x200, early_2b04 x17, early_58e4 x14, early_1518 x11.
* **Pool references** IWRAM x254, asset_metadata_index x55, game_code_and_rodata x35, EWRAM x22, early_58e4 x8, early_5d9c x6, VRAM x3, compressed_graphics x2.
* **Known RAM cells touched** BG0HOFS shadow (16.16) x1, BG0VOFS shadow (16.16) x1, BG1HOFS shadow (16.16) x1, BG1VOFS shadow (16.16) x1, BG2HOFS shadow (16.16) x1, BG2VOFS shadow (16.16) x1.
* **Suggested batches** `0x0805AFAC` (53 fns), `0x0805CF3C` (27 fns), `0x0805EE90` (5 fns), `0x08060C2C` (3 fns).

### M17 `0x08062584-0x080692FB` - struct Task field API (actor core) - **landed (#65)**

The range is decompiled and carved out of the split asm, so it now appears in
`module-map.csv` as four `c_code` rows instead of one clusterable module; the
census below is the pre-decompilation one, kept for the record.

* **Landed as** `src/actor_62584.c` (`0x08062584-0x08063698`, 2 fns),
  `src/actor_63698.c` (`0x08063698-0x080653EC`, 107 fns),
  `src/actor_653ec.c` (`0x080653EC-0x080673EC`, 86 fns) and
  `src/actor_673ec.c` (`0x080673EC-0x080692FC`, 49 fns); shared data model in
  `include/task.h`. `make progress` reports 0 asm code bytes in the range.
* **What it turned out to be** the field API over `gUnk_03002490` (the running
  `struct Task *`) and the 64-entry `gUnk_03002790[]` table: spawn/free, 16.16
  position and velocity, `ArcTan2` aiming, rectangle/overlap tests,
  animation-script walking (`struct AnimCmd`), actor graphics upload, and the
  116-byte per-player record `gUnk_03002170[]` that `Task.unk88` points at.
  `Task.unk8C` is the 0x70-byte `struct Actor`, `Actor.unk44` its ROM
  `struct ActorDef`. The two large leaders (`0xa04`/`0x710`) are straight-line
  cutscene bodies, ~230 `unk3C = frame; TaskYieldTrampoline(n)` steps each.
* **Size** 27.4 KiB (`0x6d78`), 244 functions (49 reachable only through pointer tables), mean `0x72`, largest `0xa04`, pool words 11.6% of bytes.
* **Difficulty** 3/6 - 63 distinct RAM cells, 7 jump-table dispatches, 4 functions >= `0x200`.
* **Seam cost** 2 in / 81 out (local `bl` edges crossing the boundary).
* **Why** highest fan-in in the ROM (385+350+313+208+191+190...); 244 small accessors over gUnk_03002490 (current task); hot leaves 0x0806395C x825, 0x08063E14 x265.
* **Task types** 2 (class 2 x1, class 4 x1): #139, #174.
* **Anchor tables** `0x0873DF24` 5 entries -> `0x080658D8-0x08065C14`; `0x0873E2F0` 22 entries -> `0x08067470-0x08068460`.
* **Calls into the decompiled early zone** sprite draw/update x127, VRAM transfer queue + sprite buckets x21, frame driver + RNG + blend x10, task engine x7, SIO multi-play x2, sound/BGM x2.
* **Named helpers** TaskYieldTrampoline x301, TaskDispatchTrampoline x6, ArcTan2 x2, _call_via_r0 x2, _call_via_r1 x2, CpuSet x1.
* **Called from** M20 x385, M23 x350, M26 x313, M29 x208, M21 x191.
* **Depends on** sdk_libc x311, early_5d9c x127, early_1518 x21, game_code_early_080011ac_08002378_08003110 x17, M06 x14.
* **Pool references** IWRAM x511, EWRAM x58, asset_metadata_index x54, game_code_and_rodata x38, level_graphics_palettes x4, VRAM x2, gap_sram_driver_fn_table_asset_metadata_index x2, early_58e4 x1.
* **Known RAM cells touched** current game state (main dispatch) x3.
* **Batches as landed** `0x08062584` (2 fns), `0x08063698` (107 fns), `0x080653EC` (86 fns), `0x080673EC` (49 fns).

### M18 `0x080692FC-0x08070EBF` - player-state task bodies (actor core part 2) - **landed (#64)**

The range is decompiled and carved out of the split asm, so it now appears in
`module-map.csv` as ten `c_code` rows instead of one clusterable module; the
census below is the pre-decompilation one, kept for the record.

* **Landed as** `src/actor_692fc.c` (`0x080692FC-0x0806A344`, 32 fns),
  `src/actor_6a344.c` (`0x0806A344-0x0806AD18`, 37),
  `src/actor_6ad18.c` (`0x0806AD18-0x0806B2E4`, 19),
  `src/actor_6b2e4.c` (`0x0806B2E4-0x0806C2A4`, 39),
  `src/actor_6c2a4.c` (`0x0806C2A4-0x0806CD40`, 19),
  `src/actor_6cd40.c` (`0x0806CD40-0x0806D22C`, 7),
  `src/actor_6d22c.c` (`0x0806D22C-0x0806E0F0`, 31),
  `src/actor_6e0f0.c` (`0x0806E0F0-0x0806EF5C`, 29),
  `src/actor_6ef5c.c` (`0x0806EF5C-0x0806FF24`, 13) and
  `src/actor_6ff24.c` (`0x0806FF24-0x08070EC0`, 30); shared data model in
  `include/task.h`. `make progress` reports 0 asm code bytes in the range.
* **What it turned out to be** the class-1 task bodies the player's own tasks
  run, layered on the M17 field API. In address order: the input-probe
  wrappers and their six directional decoders (they snapshot the running
  task's state into a 6-byte stack record and dispatch through
  `Actor.unk54`); the "player is out of the level" bodies (warp-star and door
  exits, the level-clear dance); the screen-transition fades, which mask the
  `gUnk_03001ED8` DISPCNT shadow to `0xE0FF` and re-OR a BG-enable pattern
  while alternating a ROM window descriptor with a stack copy of
  `gUnk_03001270`; the "carried by / riding on another task" movement block
  with its two 16.16 position integrators and the `gUnk_03002170[]`
  player-record bookkeeping; the vehicle and star-ride state machines; a
  family of scripted set-piece and scenery/effect actors laid out as
  repeating `<body, alive-check, spawner>` triples; and finally the
  stage-enter and stage-leave sequences dispatched from the 26-entry anchor
  table at `0x0873FB08`, each a run of `Task.unk58 = <16.16 offset>;
  TaskYieldTrampoline(8);` steps separated by
  `while (gUnk_03002490->unk24 != K) TaskYieldTrampoline(1);` barriers.
* **Census defects found** seven, all curated in `tools/symdb.py` and all
  confirmed by the byte match. False positives `0x0806F0E2`, `0x0806FC3E`
  (the `.word 0xFFFFF000` bl-pair artifact, as in #32/#65), `0x0806FFF8` and
  `0x08070406` (the `b.n` that skips a mid-function literal pool), and
  `0x080706A8` (mid-function, no prologue); missed real functions
  `0x080694E0`, `0x0806ACF8`, `0x0806B40C`, `0x080702D8` and `0x08070454`,
  invisible to the prologue scan because `-fprologue-bugfix` drops the leaf
  `push {lr}`. The module's true function count is 258, not 256.
* **ROM data** ~110 tables named in `tools/split_config.json` `data_symbols`,
  clustering in `0x0873D3xx`, `0x0873E5xx-0x0873F9xx`,
  `0x0874C5xx-0x0874CFxx` and `0x087525xx-0x08752Exx`.

* **Size** 30.9 KiB (`0x7bc4`), 256 functions (148 reachable only through pointer tables), mean `0x7b`, largest `0x474`, pool words 12.1% of bytes.
* **Difficulty** 1/6 - 24 distinct RAM cells, 7 jump-table dispatches, 5 functions >= `0x200`.
* **Seam cost** 81 in / 19 out (local `bl` edges crossing the boundary).
* **Why** 31 task types (#140-172, 19 of class 1); 256 functions, 148 pointer-dispatched; _call_via_r0 x17; calls the Task API x137.
* **Task types** 31 (class 1 x19, class 3 x7, class 4 x5): #140, #141, #142, #143, #144, #145, #146, #147, #148, #149, #150, #151, #152, #153, #154, #155, #156, #157, #158, #159, #160, #161, #162, #163, #164, #166, #167, #168, #169, #171, #172.
* **Anchor tables** `0x0873E5BC` 15 entries -> `0x0806A3AC-0x0806B3C4`; `0x0873E670` 10 entries -> `0x0806A8F4-0x0806AA98`; `0x0873EAA0` 8 entries -> `0x0806BF54-0x0806C930`; `0x0873FB08` 26 entries -> `0x0806EF5C-0x08070E7C`.
* **Calls into the decompiled early zone** sprite draw/update x201, SIO multi-play x26, frame driver + RNG + blend x23, VRAM transfer queue + sprite buckets x6, task engine x4, sound/SE x2.
* **Named helpers** TaskYieldTrampoline x386, TaskDispatchTrampoline x24, _call_via_r0 x17.
* **Called from** M20 x80, M23 x63, M24 x48, M21 x44, M26 x40.
* **Depends on** sdk_libc x427, early_5d9c x201, M17 x137, M07 x30, early_6464 x26.
* **Pool references** IWRAM x473, asset_metadata_index x146, game_code_and_rodata x125, early_58e4 x19, EWRAM x9, VRAM x1, early_5c4c x1, level_graphics_palettes x1.
* **Known RAM cells touched** DISPCNT shadow x3, current game state (main dispatch) x2.
* **Batches as landed** `0x080692FC` (32 fns), `0x0806A344` (37), `0x0806AD18` (19), `0x0806B2E4` (39), `0x0806C2A4` (19), `0x0806CD40` (7), `0x0806D22C` (31), `0x0806E0F0` (29), `0x0806EF5C` (13), `0x0806FF24` (30).

### M19 `0x08070EC0-0x08078B67` - actor bank C (11 class-3 tasks)

* **Size** 31.2 KiB (`0x7ca8`), 218 functions (151 reachable only through pointer tables), mean `0x92`, largest `0x83a`, pool words 11.6% of bytes.
* **Difficulty** 2/6 - 38 distinct RAM cells, 5 jump-table dispatches, 16 functions >= `0x200`.
* **Seam cost** 19 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 8 anchor tables in 0x0873F-0x08740; TaskYieldTrampoline x691; sprite draw x172.
* **Task types** 11 (class 3 x11): #8, #74, #75, #76, #77, #78, #79, #97, #98, #99, #165.
* **Anchor tables** `0x0873FBAC` 8 entries -> `0x08071418-0x08071E80`; `0x0873FBD4` 20 entries -> `0x08071E74-0x08074420`; `0x0873FC3C` 21 entries -> `0x080720DC-0x08074628`; `0x0873FCA4` 17 entries -> `0x08074588-0x08074B60`; `0x087400B0` 13 entries -> `0x08076C88-0x08077224`; `0x08740100` 9 entries -> `0x0807728C-0x08077A64` (+2 more).
* **Calls into the decompiled early zone** sprite draw/update x172, frame driver + RNG + blend x48, VRAM transfer queue + sprite buckets x25, sound/SE x12, task position/draw x3, sound/BGM x2.
* **Named helpers** TaskYieldTrampoline x691, TaskDispatchTrampoline x21, CpuSet x4, LZ77UnCompWram x2, Div x1.
* **Called from** M09 x2, M18 x2, M07 x1, M17 x1.
* **Depends on** sdk_libc x712, early_5d9c x172, M17 x87, M07 x64, game_code_early_080011ac_08002378_08003110 x54.
* **Pool references** IWRAM x429, asset_metadata_index x90, game_code_and_rodata x85, EWRAM x79, VRAM x10, early_58e4 x10, level_graphics_palettes x3, m4a_songs x3.
* **Known RAM cells touched** DISPCNT shadow x3, per-player keys held x1, per-player keys pressed x1.
* **Suggested batches** `0x08070EC0` (42 fns), `0x08072D8C` (47 fns), `0x08074C0C` (13 fns), `0x080763E8` (63 fns), `0x08077AE0` (53 fns).

### M20 `0x08078B68-0x0807F043` - enemy/object behaviour bank 1

* **Size** 25.2 KiB (`0x64dc`), 390 functions (347 reachable only through pointer tables), mean `0x42`, largest `0x1c0`, pool words 15.4% of bytes.
* **Difficulty** 2/6 - 11 distinct RAM cells, 1 jump-table dispatches, 0 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 390 functions, mean 0x42, 347 pointer-dispatched, 20 anchor tables; no BL callers at all; 21 task types.
* **Task types** 21 (class 2 x2, class 3 x18, class 4 x1): #9, #12, #15, #16, #18, #19, #21, #22, #26, #29, #36, #37, #44, #45, #46, #48, #102, #134, #173, #216, #217.
* **Anchor tables** `0x08740630` 6 entries -> `0x08078BE0-0x080792F8`; `0x08740658` 4 entries -> `0x08078C80-0x08078CE4`; `0x08740670` 4 entries -> `0x08078D88-0x08078E38`; `0x087406C4` 10 entries -> `0x08078EEC-0x0807921C`; `0x08740700` 8 entries -> `0x080792DC-0x08079918`; `0x08740758` 24 entries -> `0x080795B4-0x0807A578` (+14 more).
* **Calls into the decompiled early zone** sprite draw/update x315, frame driver + RNG + blend x154.
* **Named helpers** TaskYieldTrampoline x313, TaskDispatchTrampoline x3.
* **Depends on** M17 x385, sdk_libc x316, early_5d9c x315, early_2b04 x154, M18 x80.
* **Pool references** IWRAM x540, asset_metadata_index x252, game_code_and_rodata x151, EWRAM x4, early_58e4 x3, level_graphics_palettes x1.
* **Suggested batches** `0x08078B68` (128 fns), `0x0807AB54` (103 fns), `0x0807C4A0` (44 fns), `0x0807D1BC` (41 fns), `0x0807DD70` (74 fns).

### M21 `0x0807F044-0x08082E67` - enemy/object behaviour bank 2

* **Size** 15.5 KiB (`0x3e24`), 188 functions (179 reachable only through pointer tables), mean `0x54`, largest `0x190`, pool words 13.9% of bytes.
* **Difficulty** 1/6 - 11 distinct RAM cells, 1 jump-table dispatches, 0 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 188 functions, mean 0x54, 179 pointer-dispatched, 10 anchor tables; 9 task types.
* **Task types** 9 (class 3 x8, class 4 x1): #27, #31, #32, #38, #39, #40, #42, #43, #175.
* **Anchor tables** `0x08741380` 22 entries -> `0x0807F044-0x0807FBCC`; `0x08741488` 27 entries -> `0x0807FD34-0x08080B28`; `0x08741544` 26 entries -> `0x08080E10-0x08081810`; `0x087415B8` 9 entries -> `0x080819F4-0x08081DB4`; `0x08741604` 8 entries -> `0x080820B8-0x08082548`; `0x08741640` 16 entries -> `0x08082718-0x08082C58` (+4 more).
* **Calls into the decompiled early zone** sprite draw/update x182, frame driver + RNG + blend x86.
* **Named helpers** TaskYieldTrampoline x116, ArcTan2 x1.
* **Depends on** M17 x191, early_5d9c x182, sdk_libc x116, early_2b04 x86, M18 x44.
* **Pool references** IWRAM x299, asset_metadata_index x141, game_code_and_rodata x86, EWRAM x1.
* **Suggested batches** `0x0807F044` (80 fns), `0x08080B70` (64 fns), `0x080820B8` (44 fns).

### M22 `0x08082E68-0x080860F7` - enemy/object behaviour bank 3

* **Size** 12.6 KiB (`0x3290`), 119 functions (112 reachable only through pointer tables), mean `0x6c`, largest `0x2b0`, pool words 13.2% of bytes.
* **Difficulty** 1/6 - 8 distinct RAM cells, 0 jump-table dispatches, 1 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 119 functions, mean 0x6c, 112 pointer-dispatched, 8 anchor tables; 8 task types.
* **Task types** 8 (class 2 x3, class 3 x4, class 4 x1): #10, #14, #17, #20, #105, #108, #176, #194.
* **Anchor tables** `0x0874176C` 18 entries -> `0x08082E68-0x08083A44`; `0x08741E7C` 6 entries -> `0x080840A4-0x080843F8`; `0x08741F70` 13 entries -> `0x080844C4-0x08084A74`; `0x08741FB8` 22 entries -> `0x08084D14-0x08085CD8`; `0x08742030` 8 entries -> `0x080858FC-0x08085C10`; `0x08742CF0` 4 entries -> `0x08084BC0-0x08084CB8` (+2 more).
* **Calls into the decompiled early zone** sprite draw/update x185, frame driver + RNG + blend x38.
* **Named helpers** TaskYieldTrampoline x188, TaskDispatchTrampoline x2, ArcTan2 x1.
* **Depends on** sdk_libc x190, early_5d9c x185, M17 x132, early_2b04 x38, M18 x29.
* **Pool references** IWRAM x211, asset_metadata_index x124, game_code_and_rodata x64, early_58e4 x1, early_5d9c x1.
* **Suggested batches** `0x08082E68` (44 fns), `0x080844C4` (24 fns), `0x08084D14` (51 fns).

### M23 `0x080860F8-0x0808CCE7` - enemy/object behaviour bank 4

* **Size** 27.0 KiB (`0x6bf0`), 285 functions (266 reachable only through pointer tables), mean `0x60`, largest `0x1f8`, pool words 14.0% of bytes.
* **Difficulty** 3/6 - 6 distinct RAM cells, 10 jump-table dispatches, 0 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 285 functions, mean 0x60, 266 pointer-dispatched, 14 anchor tables; sprite draw x511.
* **Task types** 10 (class 2 x1, class 3 x9): #24, #28, #30, #33, #34, #35, #47, #104, #218, #219.
* **Anchor tables** `0x08742064` 8 entries -> `0x080860F8-0x08086EFC`; `0x0874212C` 6 entries -> `0x08086C5C-0x08086EC8`; `0x08742570` 10 entries -> `0x080870C4-0x08088498`; `0x0874263C` 23 entries -> `0x08087B8C-0x0808835C`; `0x087426AC` 16 entries -> `0x080883DC-0x08088F4C`; `0x08742704` 12 entries -> `0x08088B58-0x08088F24` (+8 more).
* **Calls into the decompiled early zone** sprite draw/update x511, frame driver + RNG + blend x108.
* **Named helpers** TaskYieldTrampoline x340, TaskDispatchTrampoline x2, Div x1.
* **Depends on** early_5d9c x511, M17 x350, sdk_libc x342, early_2b04 x108, M18 x63.
* **Pool references** IWRAM x463, asset_metadata_index x239, game_code_and_rodata x175, early_58e4 x3, early_5d9c x2.
* **Suggested batches** `0x080860F8` (35 fns), `0x080870C4` (98 fns), `0x08089064` (73 fns), `0x0808AA68` (40 fns), `0x0808BC60` (39 fns).

### M24 `0x0808CCE8-0x0809000B` - enemy/object behaviour bank 5

* **Size** 12.8 KiB (`0x3324`), 157 functions (131 reachable only through pointer tables), mean `0x53`, largest `0x304`, pool words 13.0% of bytes.
* **Difficulty** 1/6 - 9 distinct RAM cells, 6 jump-table dispatches, 1 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 157 functions, mean 0x53, 131 pointer-dispatched, 8 anchor tables; 11 task types.
* **Task types** 11 (class 2 x5, class 3 x6): #11, #13, #23, #25, #41, #103, #106, #107, #109, #137, #215.
* **Anchor tables** `0x08743188` 17 entries -> `0x0808D558-0x0808E050`; `0x087431E4` 12 entries -> `0x0808E404-0x0808E8A0`; `0x08743224` 8 entries -> `0x0808E914-0x0808EBDC`; `0x08743294` 21 entries -> `0x0808F320-0x0808F974`; `0x087432F4` 6 entries -> `0x0808FA50-0x0808FC00`; `0x087434FC` 4 entries -> `0x0808EBE0-0x0808ECB4` (+2 more).
* **Calls into the decompiled early zone** sprite draw/update x269, frame driver + RNG + blend x49, SIO multi-play x7.
* **Named helpers** TaskYieldTrampoline x161, ArcTan2 x3, TaskDispatchTrampoline x2.
* **Depends on** early_5d9c x269, sdk_libc x163, M17 x142, early_2b04 x49, M18 x48.
* **Pool references** IWRAM x235, asset_metadata_index x96, game_code_and_rodata x74, early_58e4 x10, EWRAM x1, early_5d9c x1.
* **Known RAM cells touched** decimal digit buffer, [5] = sign/flag x1.
* **Suggested batches** `0x0808CCE8` (45 fns), `0x0808E070` (112 fns).

### M25 `0x0809000C-0x08093F63` - enemy/object behaviour bank 6

* **Size** 15.8 KiB (`0x3f58`), 121 functions (109 reachable only through pointer tables), mean `0x86`, largest `0x340`, pool words 12.8% of bytes.
* **Difficulty** 1/6 - 12 distinct RAM cells, 1 jump-table dispatches, 3 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 121 functions, mean 0x86, 109 pointer-dispatched, 4 anchor tables; 10 task types.
* **Task types** 10 (class 2 x3, class 3 x4, class 4 x3): #49, #50, #54, #110, #111, #177, #179, #180, #181, #200.
* **Anchor tables** `0x08743848` 23 entries -> `0x08090090-0x08090DE4`; `0x08743984` 15 entries -> `0x0809113C-0x0809191C`; `0x08743ADC` 27 entries -> `0x08091F9C-0x08093488`; `0x087441A4` 5 entries -> `0x08093C30-0x08093E54`.
* **Calls into the decompiled early zone** sprite draw/update x263, frame driver + RNG + blend x32.
* **Named helpers** TaskYieldTrampoline x206, TaskDispatchTrampoline x3, Div x1.
* **Depends on** early_5d9c x263, sdk_libc x209, M17 x184, M18 x34, early_2b04 x32.
* **Pool references** IWRAM x275, asset_metadata_index x89, game_code_and_rodata x66, EWRAM x19, level_graphics_palettes x9, early_58e4 x1.
* **Suggested batches** `0x0809000C` (36 fns), `0x0809113C` (28 fns), `0x08091F9C` (57 fns).

### M26 `0x08093F64-0x080988F7` - enemy/object behaviour bank 7

* **Size** 18.4 KiB (`0x4994`), 140 functions (100 reachable only through pointer tables), mean `0x86`, largest `0x404`, pool words 15.0% of bytes.
* **Difficulty** 2/6 - 15 distinct RAM cells, 4 jump-table dispatches, 4 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 140 functions, 100 pointer-dispatched, 4 anchor tables; sprite draw x339; 7 task types.
* **Task types** 7 (class 2 x3, class 3 x4): #51, #55, #56, #114, #133, #138, #214.
* **Anchor tables** `0x08744440` 23 entries -> `0x08094220-0x08095484`; `0x087444E4` 9 entries -> `0x0809595C-0x08096B7C`; `0x08744564` 12 entries -> `0x080959E8-0x08096D20`; `0x0874489C` 18 entries -> `0x08097694-0x080983A0`.
* **Calls into the decompiled early zone** sprite draw/update x339, frame driver + RNG + blend x39, sound/SE x7.
* **Named helpers** TaskYieldTrampoline x192.
* **Depends on** early_5d9c x339, M17 x313, sdk_libc x192, M18 x40, early_2b04 x39.
* **Pool references** IWRAM x300, asset_metadata_index x199, game_code_and_rodata x67, EWRAM x18, level_graphics_palettes x4, early_58e4 x1, early_5d9c x1.
* **Suggested batches** `0x08093F64` (48 fns), `0x080957BC` (47 fns), `0x080974F8` (45 fns).

### M27 `0x080988F8-0x0809BA43` - enemy/object behaviour bank 8

* **Size** 12.3 KiB (`0x314c`), 140 functions (122 reachable only through pointer tables), mean `0x5a`, largest `0x150`, pool words 13.9% of bytes.
* **Difficulty** 1/6 - 6 distinct RAM cells, 4 jump-table dispatches, 0 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 140 functions, mean 0x5a, 122 pointer-dispatched; anchor tables @0x08745630 (38) and @0x0874574C (48).
* **Task types** 5 (class 2 x3, class 3 x2): #52, #53, #115, #118, #119.
* **Anchor tables** `0x08745630` 38 entries -> `0x08098ED4-0x08099A7C`; `0x0874574C` 48 entries -> `0x0809A118-0x0809B408`; `0x08745AE4` 10 entries -> `0x0809B57C-0x0809B964`.
* **Calls into the decompiled early zone** sprite draw/update x270, frame driver + RNG + blend x36, sound/SE x3.
* **Named helpers** TaskYieldTrampoline x195.
* **Depends on** early_5d9c x270, sdk_libc x195, M17 x160, early_2b04 x36, M18 x29.
* **Pool references** IWRAM x208, game_code_and_rodata x70, asset_metadata_index x66, level_graphics_palettes x8, EWRAM x4.
* **Known RAM cells touched** decimal digit buffer, [5] = sign/flag x2.
* **Suggested batches** `0x080988F8` (53 fns), `0x08099B20` (87 fns).

### M28 `0x0809BA44-0x080A158F` - enemy/object behaviour bank 9

* **Size** 22.8 KiB (`0x5b4c`), 198 functions (135 reachable only through pointer tables), mean `0x76`, largest `0x35c`, pool words 11.2% of bytes.
* **Difficulty** 1/6 - 29 distinct RAM cells, 3 jump-table dispatches, 4 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 198 functions, 135 pointer-dispatched, 5 anchor tables; sprite draw x487.
* **Task types** 7 (class 2 x3, class 3 x3, class 4 x1): #57, #58, #59, #129, #130, #131, #132.
* **Anchor tables** `0x08745B1C` 5 entries -> `0x0809BA44-0x0809BBD4`; `0x08747AA4` 27 entries -> `0x0809C4D0-0x0809D7A4`; `0x08747BCC` 22 entries -> `0x0809DCBC-0x0809F120`; `0x08747C6C` 5 entries -> `0x0809F478-0x0809F508`; `0x0874844C` 21 entries -> `0x080A0B30-0x080A14E4`.
* **Calls into the decompiled early zone** sprite draw/update x487, frame driver + RNG + blend x35, VRAM transfer queue + sprite buckets x8, sound/SE x3, task engine x3, task position/draw x1.
* **Named helpers** TaskYieldTrampoline x452, TaskDispatchTrampoline x2.
* **Depends on** early_5d9c x487, sdk_libc x454, M17 x190, M18 x38, early_2b04 x35.
* **Pool references** IWRAM x353, game_code_and_rodata x85, asset_metadata_index x76, EWRAM x41, level_graphics_palettes x15, early_58e4 x6, VRAM x1, level_object_tables x1.
* **Suggested batches** `0x0809BA44` (13 fns), `0x0809C404` (58 fns), `0x0809DCBC` (74 fns), `0x0809FCA4` (53 fns).

### M29 `0x080A1590-0x080A5643` - enemy/object behaviour bank 10

* **Size** 16.2 KiB (`0x40b4`), 221 functions (153 reachable only through pointer tables), mean `0x4a`, largest `0x100`, pool words 15.4% of bytes.
* **Difficulty** 2/6 - 21 distinct RAM cells, 0 jump-table dispatches, 0 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 221 functions, mean 0x4a, 153 pointer-dispatched; 80-entry anchor table @0x08748624.
* **Task types** 9 (class 1 x1, class 2 x2, class 3 x4, class 4 x2): #61, #63, #112, #113, #127, #178, #182, #183, #193.
* **Anchor tables** `0x087484C4` 8 entries -> `0x080A1590-0x080A472C`; `0x08748624` 80 entries -> `0x080A3184-0x080A46C0`; `0x087489D4` 11 entries -> `0x080A4BA8-0x080A5188`; `0x08748A28` 4 entries -> `0x080A4F40-0x080A503C`; `0x08748A54` 11 entries -> `0x080A510C-0x080A55AC`.
* **Calls into the decompiled early zone** sprite draw/update x222, frame driver + RNG + blend x41, VRAM transfer queue + sprite buckets x6, sound/SE x2.
* **Named helpers** TaskYieldTrampoline x91, TaskDispatchTrampoline x1.
* **Depends on** early_5d9c x222, M17 x208, sdk_libc x92, early_2b04 x41, M18 x22.
* **Pool references** IWRAM x327, asset_metadata_index x102, game_code_and_rodata x78, EWRAM x66, early_58e4 x5, level_graphics_palettes x5, VRAM x1.
* **Suggested batches** `0x080A1590` (49 fns), `0x080A2814` (119 fns), `0x080A4808` (53 fns).

### M30 `0x080A5644-0x080AA337` - enemy/object behaviour bank 11

* **Size** 19.2 KiB (`0x4cf4`), 130 functions (110 reachable only through pointer tables), mean `0x97`, largest `0x5ac`, pool words 12.0% of bytes.
* **Difficulty** 1/6 - 30 distinct RAM cells, 2 jump-table dispatches, 7 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 130 functions, 110 pointer-dispatched; anchor tables @0x08748EB8 (53) and @0x08749150 (20); 12 task types.
* **Task types** 12 (class 2 x1, class 3 x3, class 4 x8): #65, #67, #184, #185, #186, #187, #188, #195, #196, #197, #198, #199.
* **Anchor tables** `0x08748EB8` 53 entries -> `0x080A5644-0x080A787C`; `0x08749150` 20 entries -> `0x080A7D98-0x080A9304`.
* **Calls into the decompiled early zone** sprite draw/update x198, frame driver + RNG + blend x44, VRAM transfer queue + sprite buckets x11, sound/SE x2, BG/fade/blend reset x1, sound/BGM x1.
* **Named helpers** TaskYieldTrampoline x233, TaskDispatchTrampoline x6, ArcTan2 x1.
* **Depends on** sdk_libc x239, early_5d9c x198, M17 x173, early_2b04 x44, M07 x16.
* **Pool references** IWRAM x337, EWRAM x93, asset_metadata_index x86, game_code_and_rodata x53, level_graphics_palettes x7, early_58e4 x4, VRAM x3.
* **Known RAM cells touched** BG2CNT shadow x2, BG3CNT shadow x2, MOSAIC lo shadow x1, per-player keys held x1.
* **Suggested batches** `0x080A5644` (68 fns), `0x080A7438` (12 fns), `0x080A7D98` (38 fns), `0x080A99A0` (12 fns).

### M31 `0x080AA338-0x080AE3BB` - enemy/object behaviour bank 12

* **Size** 16.1 KiB (`0x4084`), 121 functions (96 reachable only through pointer tables), mean `0x88`, largest `0x378`, pool words 14.3% of bytes.
* **Difficulty** 2/6 - 21 distinct RAM cells, 1 jump-table dispatches, 2 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 121 functions, 96 pointer-dispatched; 18 task types; Div x5.
* **Task types** 18 (class 2 x3, class 3 x11, class 4 x4): #60, #62, #120, #128, #136, #189, #190, #191, #192, #201, #202, #203, #204, #205, #206, #207, #208, #209.
* **Anchor tables** `0x087493F4` 25 entries -> `0x080AA338-0x080AB46C`; `0x08749B8C` 8 entries -> `0x080AC868-0x080ACC18`.
* **Calls into the decompiled early zone** sprite draw/update x151, frame driver + RNG + blend x22, task engine x5, task position/draw x1.
* **Named helpers** TaskYieldTrampoline x191, Div x5, TaskDispatchTrampoline x5.
* **Depends on** sdk_libc x196, M17 x169, early_5d9c x151, early_2b04 x22, game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654 x14.
* **Pool references** IWRAM x285, asset_metadata_index x112, game_code_and_rodata x78, EWRAM x76, early_5d9c x8, early_58e4 x6, level_graphics_palettes x2.
* **Known RAM cells touched** current game state (main dispatch) x3.
* **Suggested batches** `0x080AA338` (50 fns), `0x080AC30C` (21 fns), `0x080ACC9C` (22 fns), `0x080AD4B8` (28 fns).

### M32 `0x080AE3BC-0x080B2FE7` - enemy/object behaviour bank 13

* **Size** 19.0 KiB (`0x4c2c`), 129 functions (96 reachable only through pointer tables), mean `0x97`, largest `0x320`, pool words 11.9% of bytes.
* **Difficulty** 1/6 - 15 distinct RAM cells, 4 jump-table dispatches, 3 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 129 functions, 96 pointer-dispatched, 5 anchor tables; 15 task types; TaskYieldTrampoline x412.
* **Task types** 15 (class 2 x5, class 3 x8, class 4 x2): #64, #66, #80, #116, #117, #121, #122, #123, #124, #135, #210, #211, #212, #213, #220.
* **Anchor tables** `0x0874AD34` 4 entries -> `0x080AF114-0x080AF1C8`; `0x0874B1DC` 12 entries -> `0x080AF38C-0x080AFD9C`; `0x0874B540` 8 entries -> `0x080B0DE4-0x080B14AC`; `0x0874B5E4` 12 entries -> `0x080B1A1C-0x080B2228`; `0x0874C12C` 11 entries -> `0x080B2A74-0x080B2F34`.
* **Calls into the decompiled early zone** sprite draw/update x155, frame driver + RNG + blend x29, VRAM transfer queue + sprite buckets x10, sound/SE x4, task position/draw x3, sound/BGM x2.
* **Named helpers** TaskYieldTrampoline x412, Div x8, TaskDispatchTrampoline x7, _call_via_r0 x2.
* **Depends on** sdk_libc x421, M17 x179, early_5d9c x155, early_2b04 x29, game_code_early_080011ac_08002378_08003110 x17.
* **Pool references** IWRAM x230, asset_metadata_index x129, game_code_and_rodata x70, EWRAM x24, early_5d9c x8, level_graphics_palettes x6, early_58e4 x3, VRAM x2.
* **Suggested batches** `0x080AE3BC` (47 fns), `0x080B0338` (52 fns), `0x080B22F8` (30 fns).

### M33 `0x080B2FE8-0x080B6153` - HUD / overlay effects?

* **Size** 12.4 KiB (`0x316c`), 108 functions (69 reachable only through pointer tables), mean `0x75`, largest `0x380`, pool words 14.2% of bytes.
* **Difficulty** 3/6 - 58 distinct RAM cells, 5 jump-table dispatches, 4 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 9 task types (5 of class 4); LZ77UnCompVram x6 + CpuSet x3 + a DMA0 register pair; VRAM transfer queue x20; EWRAM x102.
* **Task types** 9 (class 2 x2, class 3 x2, class 4 x5): #68, #69, #70, #71, #72, #73, #125, #126, #170.
* **Anchor tables** `0x0873F5FC` 6 entries -> `0x080B404C-0x080B442C`; `0x0874C21C` 9 entries -> `0x080B3090-0x080B32D0`; `0x087560A0` 8 entries -> `0x080B4174-0x080B42F8`; `0x087560D0` 7 entries -> `0x080B4770-0x080B4E04`; `0x08756150` 10 entries -> `0x080B4B18-0x080B4DF8`.
* **Calls into the decompiled early zone** sprite draw/update x104, frame driver + RNG + blend x33, VRAM transfer queue + sprite buckets x20.
* **Named helpers** TaskYieldTrampoline x105, LZ77UnCompVram x6, CpuSet x3, _call_via_r0 x1.
* **Called from** M10 x6, M07 x5, M08 x4, M17 x4, M18 x3.
* **Depends on** sdk_libc x106, early_5d9c x104, M17 x59, early_2b04 x33, early_1518 x20.
* **Pool references** IWRAM x185, EWRAM x102, asset_metadata_index x69, game_code_and_rodata x42, VRAM x10, early_58e4 x5.
* **Suggested batches** `0x080B2FE8` (19 fns), `0x080B37EC` (32 fns), `0x080B447C` (37 fns), `0x080B4E40` (20 fns).

### M34 `0x080B6154-0x080B9D0B` - save file / SRAM records + options

* **Size** 14.9 KiB (`0x3bb8`), 106 functions (26 reachable only through pointer tables), mean `0x90`, largest `0x384`, pool words 16.9% of bytes.
* **Difficulty** 3/6 - 98 distinct RAM cells, 0 jump-table dispatches, 6 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** WriteSramEx x5, ReadSram x2, sram_driver x7 - the only SRAM user in the bulk; 98 RAM cells; direct REG_BG1HOFS/REG_BG2HOFS writes.
* **Anchor tables** `0x08756198` 9 entries -> `0x080B6154-0x080B6D04`; `0x08756270` 14 entries -> `0x080B9674-0x080B97F8`.
* **Calls into the decompiled early zone** sprite draw/update x11, SRAM save x7, frame driver + RNG + blend x5, BG/fade/blend reset x2, VRAM transfer queue + sprite buckets x2.
* **Named helpers** WriteSramEx x5, Div x4, TaskYieldTrampoline x4, Mod x2, ReadSram x2, TaskDispatchTrampoline x1.
* **Called from** M03 x29, M02 x13, M07 x9, M38 x5, M09 x3.
* **Depends on** M02 x47, early_5d9c x11, sram_driver x7, sdk_libc x6, sdk_swi_wrappers x6.
* **Pool references** IWRAM x366, EWRAM x239, compressed_graphics x19, asset_metadata_index x10, game_code_and_rodata x10, SRAM x6, lib_misc x1.
* **Known RAM cells touched** BG1HOFS shadow (16.16) x13, per-player keys pressed x8, BLDALPHA hi shadow x7, BLDALPHA lo shadow x7, BG2HOFS shadow (16.16) x6, BG3HOFS shadow (16.16) x6.
* **Suggested batches** `0x080B6154` (19 fns), `0x080B6F38` (35 fns), `0x080B8EA0` (52 fns).

### M35 `0x080B9D0C-0x080BDA2B` - game-mode flow + link lobby

* **Size** 15.3 KiB (`0x3d20`), 193 functions (96 reachable only through pointer tables), mean `0x51`, largest `0x1e4`, pool words 15.1% of bytes.
* **Difficulty** 3/6 - 52 distinct RAM cells, 6 jump-table dispatches, 0 functions >= `0x200`.
* **Seam cost** 0 in / 8 out (local `bl` edges crossing the boundary).
* **Why** requested/next game state x7, link session state, link-mode flag, number of linked players; called from AgbMain; 193 small functions.
* **Task types** 2 (class 3 x2): #93, #94.
* **Anchor tables** `0x087562FC` 27 entries -> `0x080BAD00-0x080BB4FC`; `0x08756378` 24 entries -> `0x080BBE20-0x080BD9B0`; `0x08756468` 12 entries -> `0x080BC8E0-0x080BCBF8`; `0x087564E4` 12 entries -> `0x080BD544-0x080BD7EC`.
* **Calls into the decompiled early zone** sprite draw/update x73, frame driver + RNG + blend x21, task position/draw x19, BG/fade/blend reset x18, VRAM transfer queue + sprite buckets x9, SIO multi-play x4.
* **Named helpers** TaskYieldTrampoline x60, TaskDispatchTrampoline x4, LZ77UnCompWram x1, _call_via_r0 x1.
* **Called from** M36 x5, M37 x5, M01 x1.
* **Depends on** early_5d9c x73, sdk_libc x65, early_2b04 x21, early_58e4 x19, early_1fd0 x18.
* **Pool references** IWRAM x379, asset_metadata_index x74, EWRAM x71, game_code_and_rodata x40, early_5c4c x8, early_58e4 x4, VRAM x1.
* **Known RAM cells touched** DISPCNT shadow x12, requested/next game state x7, frames left to wait x6, per-player keys pressed x5, link session state, high byte = command x4, BLDCNT lo shadow x3.
* **Suggested batches** `0x080B9D0C` (101 fns), `0x080BBC04` (92 fns).

### M36 `0x080BDA2C-0x080C1FFB` - sub-game / bonus mode?

* **Size** 17.5 KiB (`0x45d0`), 117 functions (91 reachable only through pointer tables), mean `0x98`, largest `0x314`, pool words 12.4% of bytes.
* **Difficulty** 1/6 - 22 distinct RAM cells, 0 jump-table dispatches, 5 functions >= `0x200`.
* **Seam cost** 8 in / 0 out (local `bl` edges crossing the boundary).
* **Why** 41-entry anchor table @0x08756668 covering 0x080BDAB4-0x080C1B30; reached only from the game-mode flow module; task type #95.
* **Task types** 1 (class 4 x1): #95.
* **Anchor tables** `0x08756668` 41 entries -> `0x080BDAB4-0x080C1B30`; `0x08756780` 6 entries -> `0x080BFDE8-0x080C0540`; `0x087571F8` 20 entries -> `0x080C0DE8-0x080C1B2C`; `0x08757270` 4 entries -> `0x080C1B94-0x080C1D84`.
* **Calls into the decompiled early zone** sprite draw/update x160, frame driver + RNG + blend x28, task position/draw x14, VRAM transfer queue + sprite buckets x10, sound/BGM x4, BG/fade/blend reset x2.
* **Named helpers** TaskYieldTrampoline x204, TaskDispatchTrampoline x17, Div x1.
* **Called from** M35 x4.
* **Depends on** sdk_libc x221, early_5d9c x160, early_2b04 x28, early_58e4 x14, early_1518 x10.
* **Pool references** IWRAM x319, asset_metadata_index x153, EWRAM x36, game_code_and_rodata x29, early_58e4 x9, early_5c4c x5, early_5d9c x1.
* **Known RAM cells touched** BG3VOFS shadow (16.16) x16, BG3HOFS shadow (16.16) x13, per-player keys pressed x8, DISPCNT shadow x2, frames left to wait x2, requested/next game state x2.
* **Suggested batches** `0x080BDA2C` (57 fns), `0x080BF994` (24 fns), `0x080C0DE8` (36 fns).

### M37 `0x080C1FFC-0x080C641F` - FIR-coefficient effect engine

* **Size** 17.0 KiB (`0x4424`), 82 functions (27 reachable only through pointer tables), mean `0xd4`, largest `0x6b8`, pool words 10.7% of bytes.
* **Difficulty** 4/6 - 60 distinct RAM cells, 2 jump-table dispatches, 8 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** the consumer rom-map section 2 seg 10 predicted: 22 pool refs into lib_misc + 7 into lib_rodata_fir_tables (0x080CFF00 tables); Div x10 / Mod x4 / __divsi3 x5; EWRAM x183.
* **Task types** 1 (class 3 x1): #96.
* **Anchor tables** `0x087572CC` 5 entries -> `0x080C21B0-0x080C46EC`.
* **Calls into the decompiled early zone** frame driver + RNG + blend x17, VRAM transfer queue + sprite buckets x10, sound/SE x7, sound/BGM x6, BG/fade/blend reset x5, sprite draw/update x5.
* **Named helpers** TaskYieldTrampoline x12, Div x10, __divsi3 x5, Mod x4, CpuSet x1, __umodsi3 x1.
* **Called from** M01 x1, M35 x1.
* **Depends on** sdk_libc x18, early_2b04 x17, sdk_swi_wrappers x15, game_code_early_080011ac_08002378_08003110 x12, early_1518 x10.
* **Pool references** EWRAM x183, IWRAM x135, game_code_and_rodata x29, lib_misc x22, asset_metadata_index x20, level_object_tables x9, VRAM x8, lib_rodata_fir_tables x7.
* **Known RAM cells touched** per-player keys pressed x9, requested/next game state x4, DISPCNT shadow x2, number of linked players x2, per-player keys held x2, 128 8-byte free-list entries x1.
* **Suggested batches** `0x080C1FFC` (19 fns), `0x080C3648` (46 fns), `0x080C5284` (17 fns).

### M38 `0x080C6420-0x080CD89B` - intro / cutscene / ending sequences?

* **Size** 29.1 KiB (`0x747c`), 110 functions (70 reachable only through pointer tables), mean `0x10f`, largest `0x918`, pool words 11.1% of bytes.
* **Difficulty** 4/6 - 83 distinct RAM cells, 2 jump-table dispatches, 17 functions >= `0x200`.
* **Seam cost** 0 in / 0 out (local `bl` edges crossing the boundary).
* **Why** TaskYieldTrampoline x826; 28 compressed_graphics refs; palette fade + BLD shadows; task types #260-264; called from AgbMain and the mode manager.
* **Task types** 7 (class 3 x2, class 4 x5): #100, #101, #260, #261, #262, #263, #264.
* **Anchor tables** `0x08757334` 10 entries -> `0x080C6D84-0x080C8EA8`; `0x087573F8` 11 entries -> `0x080C9114-0x080CA8F0`; `0x08758294` 23 entries -> `0x080CB5C4-0x080CD2F8`; `0x08758324` 4 entries -> `0x080CCF2C-0x080CD248`.
* **Calls into the decompiled early zone** sprite draw/update x187, frame driver + RNG + blend x67, VRAM transfer queue + sprite buckets x45, BG/fade/blend reset x35, task position/draw x26, sound/BGM x7.
* **Named helpers** TaskYieldTrampoline x826, TaskDispatchTrampoline x13, LZ77UnCompWram x8, CpuSet x1, Mod x1.
* **Called from** M02 x5, M01 x3, M03 x1.
* **Depends on** sdk_libc x839, early_5d9c x187, early_2b04 x67, early_1518 x45, early_1fd0 x35.
* **Pool references** IWRAM x379, EWRAM x83, asset_metadata_index x76, compressed_graphics x28, game_code_and_rodata x27, early_58e4 x21, VRAM x20, m4a_songs x12.
* **Known RAM cells touched** DISPCNT shadow x14, BG3HOFS shadow (16.16) x10, BG3VOFS shadow (16.16) x10, current game state (main dispatch) x7, BLDALPHA hi shadow x6, BLDALPHA lo shadow x6.
* **Suggested batches** `0x080C6420` (15 fns), `0x080C7E4C` (15 fns), `0x080C9114` (20 fns), `0x080CACF0` (18 fns), `0x080CB5C4` (37 fns), `0x080CD330` (5 fns).

## 7. Confidence, open questions, and what would settle them

The **ranges, sizes, seams, anchors, task types and call traffic are measured**
and reproducible. The **names are inference**, at three confidence levels:

**Solid (structure + several independent signals agree)**

* M17/M18 — `struct Task` field API. Read `0x0806395C`, `0x08063E14`,
  `0x080639B4`: three-instruction accessors on `*(struct Task **)0x03002490`.
* M20-M32 — enemy/object behaviour banks: no callers, table-dispatched, tiny
  functions, sprite + RNG + yield, ~136 anchor tables between them.
* M34 — save/records: the only `WriteSramEx`/`ReadSram` caller in the bulk.
* M15 — link multiplayer mode: 162 calls into `early_6464` (SIO MULTI-PLAY),
  five times more than the rest of the range put together.
* M03 — UI/menu bank: owns all 22 consecutive class-4 task types #238-#259 and
  the key auto-repeat + decimal-digit cells.
* M02 — game mode / screen loader: owns the three class-0 tasks and the
  `current game state` / `requested game state` cells, and does the
  LZ77/Huff decompression into VRAM.
* M37 — FIR-coefficient consumer (see §3.5).

**Plausible, marked `?` in the table**

* M04+M05 — "player character". The evidence is a 71-entry dispatch table
  (`0x08731FA8`) spanning both, the densest `TaskYieldTrampoline` use in the
  ROM (1,340 in M04 alone), 261 sprite-draw calls, and callers in every stage
  module. What does *not* fit a player controller is how little it reads the
  key cells (one reference). **To settle it:** decompile one dispatched body
  and see whether it reads `gUnk_0300100C`/`gUnk_03000F98[]` through a
  per-player struct, or trace which task type spawns it.
* M06 — "terrain/collision query". It is certainly a pure leaf over the IWRAM
  block `0x030054E0-0x030055B0` and the `0x100`-stride index tables at
  `0x087328F0-0x087339F0`, shared with M07. Whether that block is the room
  descriptor or something else is untested. **To settle it:** name the block's
  fields while decompiling M07, which builds it.
* M33 "HUD/overlay effects", M36 "sub-game", M38 "intro/cutscene/ending" — each
  rests on one strong hint (class-4 tasks + LZ77-to-VRAM; a 41-entry table
  reached only from the mode-flow module; compressed-graphics refs + fades).
* M07/M08's split into "builder" and "camera/scroll": the 104-edge seam says
  they are one subsystem; the BG-scroll shadows are all on the M08 side, which
  is why they are named separately.

**Structural only (named for what they are, not what they do)**

M09-M14, M16, M19, M21-M32 carry role names ("stage manager A", "bank 7").
Renaming them is expected as decompilation proceeds; that is what the
`MODULE_NAMES` dict in `tools/modmap.py` is for — update it there, re-run
`make modmap`, and the CSV, the table and this document's detail blocks stay in
sync.

Other loose ends this pass surfaced:

1. The task-type table straddles a `segments.txt` boundary (§3.1) and should be
   carved into its own segment with symbolic entry names — a natural companion
   to #36 (typed extraction of data zones).
2. `rom-map.md` §6 / `src/early_58e4.c` describe `TaskType.unk04` as a flag
   word; it is an entry point. Worth fixing in both places when the task
   subsystem is next touched.
3. 3,288 of 5,045 functions have **no BL caller at all** — they are only
   reachable through tables. Any future "dead code?" analysis must not treat
   `rom-pointer`-only evidence as suspicious.

## 8. Regenerating

```sh
make modmap                              # docs/analysis/module-map.csv (Docker)
python3 tools/modmap.py --report -       # full evidence dump (what §6 is built from)
python3 tools/modmap.py --markdown - --details   # §4 table + §6 detail blocks
```

CI re-runs `make modmap` whenever a baserom is available and fails on any diff
in `module-map.csv`, so the committed table cannot drift from the tool. Changing
a threshold (`MODULE_MAX`, `LOCAL_WINDOW`, …) re-cuts the modules and therefore
invalidates the `MODULE_NAMES` keys and any open child issues — treat the module
boundaries as frozen once the child issues exist.
