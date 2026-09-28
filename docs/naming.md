# Naming convention

How functions, globals and ROM tables get real names in this repository
(issue #155).  The style is pret's and katam's; the evidence rules decide
*whether* a symbol gets a name at all.  `tools/rename.py` applies a name
everywhere it lives and logs it in `docs/analysis/renames.csv`.

## 1. Principles

- **A wrong name is worse than no name.**  `sub_XXXXXXXX` and
  `gUnk_XXXXXXXX` say "unknown" honestly.  A name claims a role, and every
  later reader builds on that claim.  If the role is uncertain, the symbol
  stays unnamed.
- **Every name carries evidence.**  Each row of `docs/analysis/renames.csv`
  has an `evidence` column that a reviewer can check without trusting the
  author (section 4).
- **A name describes what is true on every path**, not the one caller or
  the one effect that happens to be easiest to see.  Check all callers, all
  writers and readers of a cell, and every exit of a state machine before
  using words like `Init`, `Create`, `Update`, `Draw`, `Clear`, `All` or
  `First` (katam `AGENTS.md` section 3.3 has the full list).
- **A name adds knowledge.**  Ordinals (`Func2`, `gTable3`) and vague words
  (`Data`, `Info`, `Handler`, `Thing`, `Value`) with no distinction behind
  them are not names; leave the placeholder.
- **Renames change no byte.**  A rename is an identifier change only: no
  code, prototype or type change rides along.  `make clean && make compare`
  proves every batch (section 6).

## 2. Style

| Symbol kind | Style | Examples |
|---|---|---|
| Functions | `PascalCase` | `MultiBootInit`, `TaskCreate` |
| Task-type bodies (entries of the task-type table `0x0872FF30`) | `Task_<Thing>` | `Task_TitleScreen` |
| Globals (RAM cells, ROM tables) | `g` + `PascalCase` | `gMultiBootParam`, `gTaskTypes` |
| File-local statics | `s` + `PascalCase` | `sLinkTimer` |
| Struct and union tags, typedefs | `PascalCase` | `struct Task`, `struct RoomDef` |
| Struct fields | `camelCase` | `posX`, `sleepFrames`, `frameTable` (section 2.2) |
| Macros, enum constants | `UPPER_CASE` | `REG_IME`, `TASK_CLASS_ACTOR` |
| Unknown fields / regions | `unk<off>` / `filler<off>` | `unk3C`, `filler6C` |
| Unknown parameters and locals | positional / register | `arg0`, `r4`, `sp00` |

Rules that follow from the table:

- **Families.**  Siblings get parallel names: one prefix per subsystem
  (`Link*`, `MultiBoot*`, `Camera*`, `Hud*`), then the verb or noun.  An
  underscore joins a family prefix only where the prefix is itself a unit
  (`Task_<Thing>`, the m4a `gMPlayInfo_BGM` style); otherwise it is plain
  PascalCase.
- **Task bodies: `Task_<Thing>`.**  The function a task-type table entry
  points at is entered by the ARM task switcher, not called, and it runs
  as a coroutine until it frees itself.  pret (`Task_*` in pokeemerald) and
  katam (`Task_InputRecorder`) use this prefix for exactly that role, so
  it marks the table-dispatched entry points and keeps them greppable.  A
  function that spawns a task of that type is `Create<Thing>` (katam's
  `CreateBonkers`); the task's per-frame callbacks and coroutine steps are
  `<Thing><Verb>` (katam's `BonkersStartWalk`, `BonkersWalk`).  katam's
  task *engine* (`TaskCreate(TaskMain, size, priority, flags, destructor)`)
  is a different design from this ROM's cooperative coroutines, so its
  engine names are evidence only where the shape really matches.
- **Public references keep their spelling.**  Code that is a known library
  keeps the library's names even where they break the table: the m4a
  engine (`m4aSongNumStart`, `ply_note`), the AGB SDK (`MultiBootMain`,
  `SoftReset`, the SWI thunks), pokeruby's `src/link.c` (`SerialCB`,
  `EnqueueSendCmd`), libgcc (`__divsi3`).  `tools/rename.py --allow-style`
  accepts such a name.
- **Hex in placeholders** follows the existing census: functions are
  `sub_` + lower-case hex (`sub_0806395c`), globals `gUnk_` + upper-case hex
  (`gUnk_03002170`).  The tool matches either case.
- Prefixes `sub_`, `gUnk_`, `unk_`, `loc_`, `nullsub` and a leading `_` are
  reserved for placeholders and compiler/library symbols; a real name never
  uses them.

### 2.1 The engine's vocabulary (run 1 of #155)

These words were fixed by run 1 and the families built on them; keep them.

- **Task** functions act on the running task `gCurTask` (`TaskMove`,
  `TaskSetFrame`, `TaskFaceNearestPlayer`); the same with a slot argument
  ends in `Slot` (`TaskStopSlot`, `ActorSetStateSlot`).  `Actor` functions
  act on the running task's `struct Actor` (`gActors[slot]`, M16-M18).
- A coroutine **sleeps** n frames (`TaskYieldTrampoline(n)`), is sent to a
  new state with `TaskSetEntry`, and ends with `TaskExitTrampoline`
  (`TaskFree(gCurTaskIdx)`).  A task is **freed** (`TaskFree`, `...OrFree`);
  an actor is **destroyed** (`ActorDestroy`, `...OrDestroy`) because that
  also runs its teardown and frees its attached task.
- **Facing** in a name means the sign follows `Task.facing` (1 = right).
- **World** / **Screen** coordinates: world pixels minus the sprite camera
  (`gSpriteCameraX/Y`) are screen pixels.  **InView** tests the view rect
  `gViewRect` widened by 64 px; **OnScreen** tests the screen widened by
  about 64 px.
- Motion: **position** (16.16, `Task.posX/posY`), **pixel position**
  (`pixelX/pixelY`), **velocity** (`velX/velY`), **acceleration**
  (`accelX/accelY`), **speed limit** (`speedLimitX/speedLimitY`).
  `Task.health` is **health** (actors and players).
- The player's action machine: `PlayerAction<Name>` is the enter coroutine
  in `gPlayerActions`, `PlayerAction<Name>Update` the per-frame handler it
  installs, `PlayerCheck<Name>` the check that requests it.
- Do not reuse a public name with a different shape: pokeruby's
  `BuildSendCmd` takes a command and ours does not, hence `FillSendCmd`.

### 2.2 Struct fields (run 2 of #155)

Fields follow the symbols' evidence rules, one struct at a time, with
`tools/rename_field.py` (section 6).  A field keeps `unk<off>` while its
role is not proven on every path; the offset comments (`/*0x14*/`) stay.

- **camelCase, the words of 2.1.**  Axis pairs end in `X`/`Y` (`posX`,
  `pixelY`, `velX`); a saved copy is `saved<Field>` (`Actor.savedFrame`),
  last frame's value `prev<Field>` (`PlayerState.prevPixelX`,
  `Actor.prevState`); a count is `<thing>Count` (`RoomDef.doorCount`); a
  flag that is only tested reads as a predicate (`onGround`, `running`,
  `mapsCompressed`); a callback slot is `<role>Callback`.
- **The same word for the same thing** across structs: `ActorDef.score` is
  copied into `Actor.score`, `ActorDef.ability` into `Actor.ability`,
  `GfxHeader`, `TaskGfx` and the room objects' graphics descriptor all say
  `palette` / `tiles` / `tileCount` / `paletteBankCount`.
- **Per-type scratch stays unnamed.**  `Task.unk18`-`unk34`,
  `unk6C`-`unk70`, `unk46`, `unk73`/`unk74`/`unk76` and `unk82`/`unk84`
  mean different things in different task families (a variant, a timer, a
  child slot); they get a name only where every writer and reader agrees.
  A field that one or two families reuse for their own values may still
  be named after the engine's use (`Task.facing`, `Task.hitTimer`); say so
  in the evidence.
- **Local copies.**  A file that declares its own copy of a shared struct
  (the task engine's `struct Task` with `h10`/`b12`/`w4C` names,
  `src/early_5d9c.c`'s `struct Sprite`, the 17 `struct RoomDef` copies)
  gets the same name at the same offset; a copy whose member spans more
  than the field (an array, padding) keeps its span.
- **What the named `struct Task` looks like** (`include/task.h`): four
  callbacks `moveCallback`, `updateCallback` (the state's per-frame
  update), `lateUpdateCallback` (RunTasks phase 4, after every task has
  moved) and `drawCallback`; the coroutine's `sleepFrames`, `taskClass`,
  `skipMask`; the state machine `state` (`CallTableEntry(Task.state, n,
  states)`) and `updateState` (the update callback's handler index);
  `serial`, `parent`; the sprite `frameTable`, `frame`, `spriteFlags`,
  `tileWord`, `layer`, `facing`; the motion fields of 2.1; the actor and
  hit engine's `actorKind`, `hitTimer`, `health`, `onGround`,
  `waterFlags`, `hitKind`, `hitDirection`, `hitterSlot`, `hitterPlayer`;
  and `player`, the task's `struct PlayerState`.

### 2.3 Enemies and the abilities (run 2 of #155)

No string says which enemy a script is; the local sprite renders do
(section 4, `visual:`; lesson 4.126), corroborated by the enemy's
`ActorDef.ability` (the ability Kirby gets from it), its behaviour or
katam.  Names use the enemy's English name as the game's manual spells
it, in PascalCase (`WaddleDee`, `BrontoBurt`, `PoppyBrosJr`, `UFO`).

| Symbol | Name | Example |
|---|---|---|
| the task-type body a room object of that subtype runs | `Task_<Enemy>` | `Task_WaddleDee` |
| its frame table (`Task.frameTable`) | `g<Enemy>Frames` | `gRockyFrames` |
| its variant table (`CallTableEntry(Task.unk73, n, ...)`) | `g<Enemy>Variants` | `gSparkyVariants` |
| a variant row that installs the update hook, sets state 0 and enters the state table once | `<Enemy>[<Row>]Init` | `PengyInit`, `SirKibbleWalkInit` |
| the re-arm entry `CallTableEntry(Task.state, n, states)` / the per-frame hook | `<Enemy>[<Row>]EnterState` / `<Enemy>[<Row>]Update` | `ScarfyEnterState` |
| its state tables (`Task.state` / `Task.updateState`) | `g<Enemy>[<Row>]States` / `g<Enemy>[<Row>]StateUpdates` | `gScarfyStates` |
| a state body with a verb the code shows | `<Enemy><Verb>` | `ScarfyChase`, `PengyShoot` |
| a child object only that family spawns (a projectile, an effect) | `Task_<Enemy><Thing>` | `Task_WaddleDooBeam`, `Task_WhispyWoodsApple` |
| its ActorDef / graphics descriptor, where it has a label | `g<Enemy>Def` / `g<Enemy>Gfx` | `gBonkersDef`, `gUFOGfx` |

`<Row>` is left out when the row is the family's only state machine.  A
row word says what that row does (`SirKibbleStand...` stands between
throws, `SirKibbleWalk...` walks).  The verbs are defined by the body:
**Wait** (no motion; loops until its check picks the next state), **Walk**
(`TaskSetMotionXFacing` plus a walk-frame loop), **Fall** (only gravity
and the fall speed limit), **Shoot** (spawns projectile actors in a loop),
**Jump** (an upward velocity, then waits for `Task.onGround`).  Mid-bosses
and bosses use the same table (`Task_KingDedede`, `KingDededeEnterState`,
`gKingDededeStates`); effect tasks are named by what they show
(`Task_HitFlames`, `Task_StarFlash`), and a callback that frees a task
whose parent has died is `<Thing>CheckParent`.

Run 3 of #155 added these words, each defined by the whole body:

- **Idle** (a row word): the row only plays its animation and reacts to
  hits - no velocity or `TaskSetMotion*` call, no attack or spawn, no
  state change of its own (`WaddleDeeIdleInit`, `gPengyIdleStates`); a
  row that also hops, spawns or steers is not Idle.  Nothing re-arms these
  rows, so they have no EnterState.
- **Teleport**: the body hides the sprite, moves it and shows it again
  (`KabuTeleport`).
- **Float** / **Exhale** (King Dedede): a puffed-up flight that drifts
  after the player until a timer ends, and letting the held air out as a
  puff object before dropping (`KingDededeFloat`, `KingDededeExhale`);
  Exhale is not run 2's **Spit**, which spits a swallowed object out as a
  star.
- Two species that share one script get a pair prefix
  (`SwordAndBladeKnightWalkInit`, as `gMrShineAndMrBrightDef`); a verb
  that fits two states of one family names neither of them.

**A species identity needs three agreeing sources** (run 3): the local
render (`visual:`), the behaviour and `ActorDef.ability` from the code
(`code:`), and a text description from a public reference, cited by URL
(`public: https://wikirby.com/wiki/...`; text only, never images).  That
rule corrected run 2 twice: subtype 36 is Needlous, not Togezo (which
appears only in Kirby's Dream Land 3), and the BALL enemy is Bubbles
(Bounder is not in this game).  Sword Knight (purple) and Blade Knight
(green armour, red plume, magenta mask) were told apart by WiKirby's
colour text, the case run 2 left open.

Meta Knightmare's action machine (`gMetaKnightActions`,
`gMetaKnightActionHandlers`, dispatched instead of the player's while
`gMetaKnightmareMode` is set) follows the player's words:
`MetaKnightAction<Name>` / `MetaKnightAction<Name>Update`, named after the
Kirby twin at the same index when the body is its twin
(`MetaKnightActionWalk`), or after the input and motion for Meta Knight's
own four sword attacks (`MetaKnightActionDashSlash`).

The ability ids (`PlayerState.ability`, `ActorDef.ability`) are fixed by
the HUD ability pictures `gAbilityPictures[id]`, whose banners carry the
name: 0 NORMAL, 1 FIRE, 2 SPARK, 3 CUTTER, 4 SWORD, 5 BURNING, 6 LASER,
7 MIKE, 8 WHEEL, 9 HAMMER, 10 PARASOL, 11 SLEEP, 12 NEEDLE, 13 ICE,
14 FREEZE, 15 HI-JUMP, 16 BEAM, 17 STONE, 18 BALL, 19 TORNADO, 20 CRASH,
21 LIGHT, 22 BACKDROP, 23 THROW, 24 U.F.O., 25 STAR ROD, 26 WAIT.  The
ability moves are `PlayerAction<Ability>` / `PlayerAction<Ability>Update`
(`PlayerActionFire`, `PlayerActionHiJumpUpdate`, `PlayerActionStarRod`).
An enum for the ids would be a code change and waits for #36 phase 2.

### 2.4 Data records by position (run 3 of #155)

Some records have no identity but their slot in a table that a decompiled
consumer proves (docs/data.md 5): a stage's room list, a room's header,
the map and the doors only that header points at.  Such a record is named
after the slot, and the slot is its evidence (tag `slot:`, section 4).
The indices are the table's own, **0-based**, exactly as the code indexes
it (`gRoomTable[level][stage][room]`); no world name enters an
identifier.

| Record | Name | Example |
|---|---|---|
| a stage's room list `gRoomTable[L][S]` | `gLevel<L>Stage<S>Rooms` | `gLevel0Stage1Rooms` |
| a room header (`struct RoomDef`) `gRoomTable[L][S][R]` | `gLevel<L>Stage<S>Room<R>` | `gLevel0Stage1Room2` |
| a record that one RoomDef field alone points at | `gLevel<L>Stage<S>Room<R><Field>`, the field in PascalCase | `gLevel0Stage1Room2Doors`, `...Room2MetatileMap` |
| a BG animation set `gRoomBgAnimScripts[N]` and its script `[N][I]` | `gRoomBgAnimSet<N>` / `gRoomBgAnimSet<N>Script<I>` | `gRoomBgAnimSet3Script0` |
| an ActorDef bound by a named family through its kind table's slot | `g<Enemy>Def` | `gWaddleDeeDef` |
| a graphics descriptor in a kind's descriptor table (`gEnemyGfx`, `gMidBossGfx`, `gBossGfx`, `gMetaKnightsGfx`) | `g<Enemy>Gfx` | `gCappyGfx` |
| the palette and sprite-sheet tiles that descriptor alone points at | `g<Enemy>GfxPalette` / `g<Enemy>GfxTiles` | `gCappyGfxTiles` |

Rules: a record gets a position name only when that one slot is its only
referrer (or when every slot that shares it belongs to one named family,
as the three Poppy Bros. Jr. subtypes share `gPoppyBrosJrGfx`) (no second table, no code reference; checked over `data/`,
`asm/`, `src/` and `include/` before a batch); a record several slots
share (a stage's common tiles and palettes) keeps its placeholder, and so
does the target of a field that has no name yet (`RoomDef.unk10`).
Position names are applied in their own batches, and the progress figures
count them apart from the semantic names (a slot is an address-free
identity, not a role).

What the level indices are in the game is proven for 0-6 by the boss each
level's last stage spawns (room objects of kind 3, `gBossDefs[subtype]`,
whose identities run 2 fixed): 0 Whispy Woods, 1 Paint Roller, 2 Mr. Shine
& Mr. Bright, 3 Kracko, 4 Heavy Mole, 5 Meta Knight, 6 King Dedede - the
game's Levels 1-7 (Vegetable Valley, Ice Cream Island, Butter Building,
Grape Garden, Yogurt Yard, Orange Ocean, Rainbow Resort;
https://wikirby.com/wiki/Kirby:_Nightmare_in_Dream_Land).  Level 7's
stage 0 holds the Nightmare Power Orb and Nightmare Wizard rooms and its
stage 1 three rooms with Kracko, Whispy Woods and King Dedede; level 8
spawns no boss.  What levels 7 and 8 are in the game is not proven here.

## 3. Words with a fixed meaning

| Word | Means | Not |
|---|---|---|
| `Init` | puts a subsystem or record in its starting state, once per use | a per-frame reset |
| `Create` / `Spawn` | allocates a task/object and returns or registers it | only filling fields |
| `Free` / `Destroy` | releases the slot or record | hiding it |
| `Update` / `Step` | the per-frame work of a record | a one-shot setup |
| `Draw` | builds OAM/BG output | any function that also moves things |
| `Load` | copies or decompresses data into RAM/VRAM | computing it |
| `Get` / `Set` | a read / write of one value with no side effects | anything that allocates |
| `Is` / `Has` | returns a boolean | a count or a code |
| `Try` | may fail and says so in its return value | always succeeds |
| `Main` | the loop body of a game state or a task | a helper |

## 4. Evidence

The `evidence` column says why the name is right in terms a reviewer can
check.  Start it with one of these tags:

- `public:` a public reference, by file and function: `public: pokeemerald
  src/multiboot.c MultiBootInit, same function order and body shape`.
- `katam:` a katam function whose role **and** shape match (arguments,
  cells, callers' pattern): `katam: src/palette.c LoadBgPaletteWithTransformation
  - same (src, dst, len) args, same fade table`.  A katam name with only a
  similar role is not evidence.
- `role:` the caller or cell pattern that fixes the role: who calls it and
  with what, what it writes, which table dispatches it.  Cite a file, a table
  or a rom-map section: `role: installed in gUnk_030004B0[0], the serial
  slot of the master ISR's handler table (src/early_6464.c)`.
- `visual:` what a LOCAL render of the graphics the code loads shows
  (`visual: a red rock-dome creature with a headband (local render, not
  committed)`).  Renders live only in the gitignored `pending/` and are
  never committed, uploaded or attached anywhere (data policy, AGENTS.md;
  lesson 4.126).  A render is never enough alone: combine it with `code:`
  (the ability byte, the behaviour) or `katam:`.
- `code:` what the body itself does, read from the C: the cells it reads
  and writes, the loop it runs, what it returns (for example, code: the
  body is `while (1) TaskYieldTrampoline(0x7FFF)`).  Enough on its own only
  for a small function whose whole contract is visible.
- `hw:` a hardware register the code drives: `hw: writes REG_SIOCNT
  0x4003 (multi-play, 115200 bps, IRQ) and REG_RCNT 0`.
- `string:` a string or ID the code reads or compares (`"AGB  KIRBY"`).
- `slot:` the record's slot in a consumer-proven table, for a position
  name (section 2.4): `slot: gRoomTable[0][1][2].doors`.
- `doc:` a finding already written down in `docs/analysis/rom-map.md`,
  `docs/analysis/module-map.md` or a lesson, by section.

Several tags can be combined (`public: ...; role: ...`).  "The name is
obvious", "the model inferred it", a visual resemblance or a matching build
are not evidence.

## 5. When to keep `sub_` / `gUnk_`, and what stays unnamed by design

Keep the placeholder when:

- the role is known only from one caller of many, or the callers disagree;
- the function is a thin wrapper whose name would only restate its callee
  (`CallFoo`) and whose role in its callers is not settled;
- a cell is read and written by one function only and its meaning is local
  to that function's algorithm;
- a name would have to be an ordinal or a vague word (section 1).

Unnamed by design, for #37's audit:

- **struct fields whose role changes with the task family** (section 2.2)
  and the fields run 2 of #155 could not prove; the list of what is left is
  in #155;
- **assets** (graphics, palettes, tilemaps, samples, songs, level maps):
  their labels in `data/*.s` keep address names until a consumer's role
  gives them one (`docs/data.md`), and the data policy (AGENTS.md) still
  holds - naming a label never commits its bytes;
- **census placeholders that are not symbols in C**: `loc_XXXXXXXX` branch
  labels, split-only labels in `asm/rom_syms.s`, and the dead SDK exports
  that nothing references and no reference names;
- **unknown parameters and locals** (`arg0`, `r4`, `sp00`), which keep
  katam's positional form;
- segment names (`docs/analysis/segments.txt`) and source file names, which
  are not symbols; renaming them is not part of #155.

## 6. Applying names: `tools/rename.py`

```sh
tools/rename.py sub_08004968 MultiBootInit --kind function \
    --evidence "public: pokeemerald src/multiboot.c MultiBootInit"   # dry run
tools/rename.py --csv batch.csv            # dry run of a batch
tools/rename.py --csv batch.csv --write    # apply
make symbols && make split && make modmap  # regenerate (tools/rename.py --regen
make clean && make compare                 #  runs these five for you)
tools/rename.py --verify-diff master       # the branch is a pure rename
```

A batch CSV has the header `old,new,kind,evidence` (optional `issue`);
`kind` is `function`, `ram`, `io`, `rom` or `const` and is checked against
the address.  The tool refuses a name that is not a C identifier, is a
keyword, uses a placeholder prefix, breaks the style of section 2 (unless
`--allow-style`), or is already an identifier anywhere in the tree in any
case.  It updates `tools/symdb.py` (`KNOWN_SYMBOLS`, or `ARM_ENTRIES` for
ARM code), `tools/split_config.json` (`data_symbols`, `extra_labels`,
`abs_symbols`, `external_defined`, and the names quoted in the
`pointer_tables` / `not_pointers` reasons), every word-boundary use in
`src/`, `include/`, `tools/header_smoke.c` and the hand-written asm, and it
appends to `docs/analysis/renames.csv`.  It never edits generated files:
`docs/analysis/symbols.csv`, `asm/rom_syms.s`, the generated `asm/*.s` and
`data/*.s` come from `make symbols` and `make split`.

`--verify-diff REF` maps every name `renames.csv` gained since the git ref
back to its old name and compares the tree with the ref: the C and asm must
be identical outside comments (comment edits are listed for review), a
field rename is accepted only after `.`/`->` or at a member declarator of
its struct, and
the generated files, the config and `tools/symdb.py` identical except for
the new `KNOWN_SYMBOLS` entries.  It is the proof that a rename branch
changed nothing but names.

`docs/analysis/renames.csv` is the alias table.  The lessons, the rom-map
and the module-map keep the names of their time; a reader maps an old name
through it.

### 6.1 Fields: `tools/rename_field.py`

```sh
tools/rename_field.py Task unk43 facing --copies \
    --evidence "code: ..."                   # dry run (compiles in Docker)
tools/rename_field.py --csv fields.csv       # struct,old,new,evidence[,offset,copies]
tools/rename_field.py --csv fields.csv --write
make clean && make compare
tools/rename.py --verify-diff master         # covers field renames too
```

A word replace cannot rename a field (dozens of structs have an `unk14`),
so the tool renames the member in every definition of the struct, lets
gcc 12's `-fsyntax-only` (in the knidl-builder image) report each access
that now fails with its file, line, column and struct, renames exactly
those, and repeats until the tree compiles as before; the renamed tree's
error set must equal the original's (lesson 4.125).  `copies` renames the
member at the same offset in the local copies of the struct (section 2.2).
A qualified mention `Struct.old` in a comment of `src/` or `include/`
follows; prose such as `task->unk14` does not.  Each field rename is a
`renames.csv` row of kind `field`, written `Struct.old` -> `Struct.new`, one
per local copy with its own old name.  Field names never reach code
generation (lesson 3.516), and agbcc's `make compare` is still the proof.

Apply names in batches of about 50-100 and run `make clean && make compare`
after every batch.  gcc 2.95 hashes some RTL by symbol name (lessons 4.79,
4.86, 3.493), so a rename is not guaranteed to be codegen-neutral in
principle; if a batch breaks the match, bisect it, revert the one rename
that did it, and write the finding down as a lesson.  Never "fix" a broken
match with a code change.
