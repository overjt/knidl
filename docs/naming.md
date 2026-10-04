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
| I/O registers kept as symbols (asm pools, lesson 3.523) | `gReg` + the GBATEK name in `PascalCase` | `gRegIme`, `gRegSound1CntL` (`REG_<NAME>` is `include/gba/io_reg.h`'s macro, which C code uses everywhere else) |
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

### 2.2.1 Per-family registers: alias macros (run 5 of #155)

`Task.unk18`-`unk34`, `unk46`, `unk6C`-`unk70` and `unk74` are named per
family by object-like alias macros in `include/task_vars.h`
(docs/header-conventions.md, "Per-family registers"; the owner's decision
D1): `<family><Role>` in lowerCamelCase (`fireLionHopCount`), or a shared
role alias `actor<Role>` when a shared engine helper's contract fixes the
role (`actorAnimDelay`, `actorSpawnArg`).  The role words are those of 2.1
and 2.2: a **Timer** counts frames to an event, a **Count** counts events,
a **Slot** is a task index a spawner returned (`gTasks[...]`), an
**AnimDelay** is what the animation-script helpers return; **Base** /
**Start** / **Target** / **Saved** prefix a saved coordinate or value.
`tools/task_alias.py` applies them (section 6.2) and logs one
`renames.csv` row of kind `alias` per macro (`Task.unk28` ->
`Task.actorAnimDelay`), whose evidence is the store or read that proves the
role.

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
  rows; the one-line re-arm copy a one-state row still has
  (`CallTableEntry(Task.state, 1, g<Row>States)`, `WheelieIdleEnterState`)
  is a dead export that nothing references, named by its shape (run 4).
- **Teleport**: the body hides the sprite, moves it and shows it again
  (`KabuTeleport`).
- **Float** / **Exhale** (King Dedede): a puffed-up flight that drifts
  after the player until a timer ends, and letting the held air out as a
  puff object before dropping (`KingDededeFloat`, `KingDededeExhale`);
  Exhale is not run 2's **Spit**, which spits a swallowed object out as a
  star.
- **Defeat**: the state that starts a mid-boss's defeat (removes the HP
  bar, marks the actor defeated, clears its score, then star-flashes and
  knocks the boss back); **Summon**: creates a helper actor that moves on
  its own (not a shot aimed at the player), then goes back to idle
  (`BugzzySummon`, `KrackoSummon`); Nightmare Wizard's three star attacks
  are named by their pose (`NightmareWizardOpenCloak`, `...OpenPalm`,
  `...Point`), because one Shoot would fit all three.
- A per-frame check whose whole body re-enters the state machine when the
  state has changed is `<State>Update` (`BonkersWalkUpdate`), as run 2's
  `ScarfyChaseUpdate`.
- Two species that share one script get a pair prefix
  (`SwordAndBladeKnightWalkInit`, as `gMrShineAndMrBrightDef`); a verb
  that fits two states of one family names neither of them.

Run 4 of #155 named the state bodies with these words, each defined by the
whole body (the agents' notes in the run-4 PR give every family's verb
list):

- **The state pair.**  `g<Enemy>States[N]` is state N's entry and
  `g<Enemy>StateUpdates[N]` the update it selects; when the entry has the
  verb V, the update is `<Enemy>VUpdate`.  The update table is indexed by
  `Task.updateState`, which each entry stores itself: an update is named
  after state N only when `g<Enemy>States[N]`'s body stores
  `updateState = N`.  Most families do; Fire Lion, Gip, Javelin Knight
  (state k stores k-1), Bubbles' slot 2 and Mr. Tick-Tock's 18 do not, and
  their updates are named from the entry that selects them or not at all.
- **Hop**: a loop of small jumps, each waiting for `Task.onGround`, with or
  without an X step (`CappyCappedHop`, `BonkersHop`, `JavelinKnightHop`);
  a **Jump** is one jump whose landing hands over to another state.
  **Land**: the state a landing enters, a recovery with no motion
  (`MetaKnightLand`, `BubblesLand`).  **BounceOffWall**: the state a
  mid-boss's wall hook enters during a rush: a shake, a hop backward, a
  landing and a recovery (`BugzzyBounceOffWall`).  **Ascend** / **Descend**
  (Mr. Shine & Mr. Bright), **Swoop** (one U-shaped dive and back up,
  `KrackoSwoop`), **Hover** (in place in the air on a velY wave), **Drift**
  (the parasol's swaying descent), **Spin** (Kabu's turning frames),
  **Discharge** (Sparky's widened attack box), **Explode** (sets an
  exploding death mode, then ActorDie), **Hide** (no sprite, no hit
  checks), **Sleep**, **Skid**, **Vanish** (plays a disappearing animation
  and destroys the actor).  **Stand** is a row word for a row that stays in
  place (`CappyStandHop`).
- **Hooks of the hit-reaction record** (`struct ActorVt`, called by
  `ActorReactToDamage` / `ActorReactToDefeat`): `<Family>ReactToDamage` /
  `<Family>ReactToDefeat`.  **Terrain handlers** (`struct ActorHandlers`,
  called by `ActorCollideTerrain`): the record is `g<Family>TerrainHandlers`
  and its functions `<Family>Land`, `<Family>StartFall`,
  `<Family>EnterWater`, `<Family>BounceOffWall`, after the slot that calls
  them.  A boss's defeat filter (`Actor.defeatSweepCallback`) is
  `<Boss>DefeatSweepFilter`, its slot in the per-boss Star Rod table
  `<Boss>DropStarRodPiece`.  A late hook (`Task.lateUpdateCallback`) is
  `<State>LateUpdate`.
- **Carried and held.**  The words of the actor a player carries
  (`ActorAttached<Move>Held` / `Flight` / `BounceOff`) are reused, with the
  captor's move as the row word, for the player a boss holds
  (`HeldPlayerBackdropHeld`, `HeldPlayerThrowFlightForward`).

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
| a state-table slot (run 4): the function in `g<Family>[<Row>]States[N]`, `...StateUpdates[N]` or `...Variants[N]` | `<Family>[<Row>]State<N>` / `<Family>[<Row>]State<N>Update` / `<Family>[<Row>]Variant<N>` | `BugzzyState3`, `BugzzyState3Update` |

Rules: a record gets a position name only when that one slot is its only
referrer (or when every slot that shares it belongs to one named family,
as the three Poppy Bros. Jr. subtypes share `gPoppyBrosJrGfx`) (no second table, no code reference; checked over `data/`,
`asm/`, `src/` and `include/` before a batch); a record several slots
share (a stage's common tiles and palettes) keeps its placeholder, and so
does the target of a field that has no name yet (`RoomDef.unk10`).
Position names are applied in their own batches, and the progress figures
count them apart from the semantic names (a slot is an address-free
identity, not a role).

**State-table slots** (run 4 of #155) extend the rule to functions.  A
family's state tables are consumer-proven (`CallTableEntry(Task.state, n,
g<Family>States)` in its EnterState, `Task.updateState` in its Update, the
variant table in its `Task_<Family>` body; `src/data/actor_tables.c`
names the consumer above each table).  A `sub_*` whose ONLY referrer, in
the whole tree, is one slot of one such named table, and whose body
proves no verb, is named after that slot: `g<Family>[<Row>]States[N]` ->
`<Family>[<Row>]State<N>`, `...StateUpdates[N]` ->
`<Family>[<Row>]State<N>Update` (state N's per-frame update, the pair of
2.3's `<Enemy><Verb>` / `<Enemy><Verb>Update`), `...Variants[N]` ->
`<Family>[<Row>]Variant<N>`.  `N` is the table's 0-based index in decimal,
as the code indexes it.  The evidence is `slot: g<Table>[N], <file>`.  A
function that two tables, two families or two slots share, or that code
also calls directly, keeps its placeholder; a body that proves a verb takes
the verb, never the slot (the verb batches come first), and a verb proven
later renames the slot name again (a `renames.csv` chain row).  An update
slot gets `State<N>Update` only where state N's body stores
`updateState = N` (2.3, the state pair); the update tables of the
families that index them otherwise keep their placeholders.

**Dispatch-table slots** (run 5, approved by the owner's coordinator).
The state-table rule covers the other named dispatch tables whose consumer
is proven: a `sub_*` whose only referrer is one slot of `gCutsceneActors`
(the cutscene actor's scripts; CreateCutsceneActor stores the index in
`cutsceneActorScript`), `gWarpStarFlights` / `gWarpStarFlightUpdates`,
`gWarpStarCameraPaths`, `gPlayerDances`, `gActorDefeats` or
`gActorExplodeDefeatsByEffect` is `CutsceneActorScript<N>`,
`WarpStarFlight<N>` / `WarpStarFlight<N>Update` (the update only where
flight N's body stores `updateState = N`, lesson 4.160),
`WarpStarCameraPath<N>`, `PlayerDance<N>`, `ActorDefeat<N>` or
`ActorExplodeDefeat<N>`.  A cutscene script whose every
`CreateCutsceneActor(N, ...)` call sits in one named scene (the level-intro
scenes `Cutscene<Scene>Start` and their helpers) carries the scene:
`Cutscene<Scene>ActorScript<N>` (`CutsceneBeachActorScript10`); N stays the
table index.  Same rules as the state-table slots: counted apart, their own
commits, and a verb proven later replaces them.

**RAM position names** (run 5 of #155, the owner's decision D2).  Some
RAM cells are not variables at all but addresses inside a larger buffer
that the code passes to a copy, fade or blend routine: the cells inside the
palette shadow buffers `gBgPalette` (0x03001270, 256 BG colours) and
`gObjPalette` (0x03001470, 256 OBJ colours).  Each user copies or blends a
run of colours from that address on, so the address's only identity is its
position.  Such a cell is named after its position, 0-based and in
decimal: `gBgPaletteBank<N>` / `gObjPaletteBank<N>` for the first colour of
16-colour bank N, `g<Bg|Obj>PaletteBank<N>Color<C>` for colour C inside
it (`gObjPaletteBank6Color6` is `gObjPalette` + 0xCC).  The evidence is
`slot: <buffer> + <offset>`.  These are renames, never respellings: the C
keeps the symbol and does not become `&gBgPalette[0x20]`, because gcc's
cse may derive one `symbol+offset` from another and change the code.  They
are counted with the position names.

**Functional ROM records by position** (run 5, the owner's decision D4).
The rule of this section covers any family size: the player's frame
records and frame lists (seg 19, `player_frame_records`,
`player_frame_lists`, docs/data.md) and the `*_rodata_*` records that a C
table in `src/data/` reaches by slot are named after that slot, by a
script, in their own commits.  A format-only chain (no code reads it;
#161's `"proof": "format"`) may be named the same way; the census counts
it apart.  Asset labels stay unnamed (section 5).

**Class-value names** (run 4).  When the code sorts records by a value and
no role word is true for every record in a group, the group is named by
that value as the code writes it, which claims no role: the collider lists
RegisterCollider fills for the box classes 0x10 and 0x20 are
`gColliderClass10` / `gColliderClass10Count` / `HitTestColliderClass10` and
`gColliderClass20` / `...Count` / `HitTestColliderClass20` (class 0x00 is
the players' bodies, `gPlayerColliders`).  What each group holds is written
next to its declaration (`include/collision.h`), so a later run can give it
a role name if one word ever fits every member.

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

**Shared scratch** (run 5, the owner's decision D3).  A cell proven to be
shared scratch - several unrelated users, and no value survives from one
user to the next (every reader is preceded by its own writer on every
path) - may take `gScratch<Shape>` (`gScratchBuffer`, `gScratchWord`), or
`g<Subsystem>Scratch<Shape>` when one subsystem owns it and others borrow
it.  The evidence lists every writer and reader.  A cell that holds two
encodings gets one name only if one noun covers both (the stage's and the
hub's arrival codes are both an arrival code); otherwise it stays `gUnk_`,
with the reason in the census.

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

### 5.1 What stays unnamed: the audit's census

`make audit` (`tools/audit.py`, docs/audit.md) counts every placeholder
left in the tree by kind and category and gives each category its reason;
the table below is generated from the committed tree
(`python3 tools/audit.py --write`), and the audit fails when it is stale.
The functions are counted from `docs/analysis/symbols.csv`, the RAM and
I/O cells from `tools/split_config.json`'s `data_symbols`, the ROM labels
from `data/*.s` (a segment with `"asset": true` holds assets), the fields
from the struct definitions in `include/*.h` (header) and `src/*.c` (local
copies and module-local records).

<!-- audit:placeholders:begin (generated by tools/audit.py --write; do not edit) -->

| kind | placeholder | count | reason |
|---|---|---:|---|
| function | `sub_*` | 1050 | tracked by #155: role not settled (mostly enemy and boss state bodies and one-caller helpers, docs/naming.md section 5) |
| function | `sub_*` | 5 | tracked by #155: engine-zone helpers whose role is not settled |
| function | `sub_*` | 8 | runtime and library code with no upstream name: the m4a `bx r3` shims, the task-done hang helper, the ARM halves of the task trampolines and the veneer (docs/analysis/rom-map.md sections 6 and 8) |
| RAM cell | `gUnk_02*`, `gUnk_03*` | 87 | tracked by #155: role not proven; many are proven shared scratch or hold two encodings |
| I/O register | `gUnk_04*` | 0 | none left: the four I/O registers kept as symbols (the m4a_1 and SoftReset asm pools, and early_4734.c's IME, where REG_IME changes the allocation, lesson 3.523) are named gRegVcount, gRegSound1CntL, gRegDma1Sad and gRegIme (#170); the rest of the C spells REG_* |
| ROM label | `gUnk_08*` | 2779 | tracked by #155: functional data whose consumer does not settle a name |
| ROM label | `gUnk_08*` | 17074 | asset label, unnamed by policy until a consumer gives it a role (docs/naming.md section 5, docs/data.md) |
| ROM label | (named) | 5193 | documented by position: the record's slot in a consumer-proven table (docs/naming.md section 2.4) |
| ROM label | (named) | 55 | documented by position in a format-only chain: a slot no code reads, such as the frame list a graphics descriptor's trailer word points at (docs/naming.md section 2.4, docs/data.md 5.3) |
| struct field | `unk*` | 14 | per-family registers, named per family by the aliases in include/task_vars.h (docs/header-conventions.md; the member keeps its unk name, lessons and history cite it); `unk76` is u76's member for the player's bits |
| struct field | `unk*` | 257 | tracked by #155: the field's role is not proven |
| struct field | `unk*` | 282 | local struct copies and module-local records: tracked by #155 (tools/rename_field.py `copies`) |
| label | `loc_*` | 0 | none left: the code is C |

Named for comparison: 493 RAM cells by role and 24 by position, 2948 ROM labels by role and 5248 by position.

Functions by zone (the #34 module map, docs/analysis/module-map.md):

| zone | content | functions | `sub_*` |
|---|---|---:|---:|
| crt0 | cartridge header, crt0, master ISR and the ARM task switch | 6 | 1 |
| engine | the engine zone: AgbInit, tasks, sprites, fades, sound front end, link | 184 | 5 |
| M01 | AgbMain | 1 | 0 |
| M02 | game-state bodies, title, screen loaders, pause, HUD | 109 | 17 |
| M03 | main menu and its sprite tasks | 79 | 4 |
| M04 | scripted-sequence director and scripts | 65 | 4 |
| M05 | player animation bank and collision registry | 23 | 8 |
| M06 | collision engine and hit tests | 55 | 7 |
| M07 | level / room builder | 157 | 54 |
| M08 | camera, BG streaming, map events, stage objects | 151 | 32 |
| M09 | breakable blocks and the player task | 63 | 11 |
| M10 | player action bodies, part 2 | 39 | 4 |
| M11 | player mode machine and stage services | 121 | 30 |
| M12 | player action bodies, part 3 | 21 | 0 |
| M13 | player action bodies, part 4 | 24 | 0 |
| M14 | player action bodies, part 5, and task type #6 | 82 | 9 |
| M15 | the player's effect objects (task type #7) | 84 | 48 |
| M16 | effect spawner (task types #81-#90) | 89 | 22 |
| M17 | actor core | 245 | 52 |
| M18 | actor core, part 2 | 256 | 51 |
| M19 | cutscenes and ending sequences | 220 | 45 |
| M20 | enemies, bank 1 | 414 | 33 |
| M21 | enemies, bank 2 | 200 | 22 |
| M22 | enemies, bank 3 | 125 | 12 |
| M23 | enemies, bank 4, and two bosses | 297 | 88 |
| M24 | enemies, bank 5 | 158 | 27 |
| M25 | bosses | 121 | 8 |
| M26 | enemies, bank 7 | 148 | 40 |
| M27 | mid-bosses | 145 | 13 |
| M28 | enemies, bank 9, and the player's death sequence | 204 | 73 |
| M29 | enemies, bank 10 | 226 | 75 |
| M30 | enemies, bank 11 | 131 | 27 |
| M31 | enemies, bank 12 | 123 | 37 |
| M32 | enemies, bank 13 | 135 | 47 |
| M33 | HUD effects | 110 | 18 |
| M34 | wavy scroll, save file, input recorder | 105 | 61 |
| M35 | sub-game framework and Quick Draw | 196 | 9 |
| M36 | Bomb Rally | 117 | 2 |
| M37 | Air Grind and game state 11 | 82 | 6 |
| M38 | ending, staff credits, game over | 110 | 54 |
| m4a | the m4a sound engine (asm core and C driver) | 95 | 3 |
| sdk | SDK stubs: SWI thunks, SoftReset, SRAM driver, lib1funcs, trampolines, veneer | 32 | 4 |
| all | | 5348 | 1063 |

Register aliases (include/task_vars.h): 934 in 197 families: AbilityReleaseFlash 1, AbilityStar 3, Actor 38, AirGrind 6, AirGrindDoorSign 1, ArenaDoorSign 1, Blipper 9, BombRally 10, BombRallyDoorSign 1, BombRallyObject 3, Bomber 2, Bonkers 10, BonkersHammerHitBox 1, BonkersNut 1, BossDoorSign 3, BrontoBurt 9, BroomHatter 4, Bubbles 2, Bugzzy 9, BugzzyAfterimage 3, BugzzyLadybug 5, Burst 1, Cannon 4, CannonFuse 8, CannonFuseSpark 2, CannonSmoke 3, Cappy 2, Chilly 3, ChillyFreeze 4, CutsceneActor 11, CutsceneDirector 1, DashFireTrail 2, DashFlame 2, DoorObject 1, DoorOpening 3, DustBurst 2, DustTrail 3, EndingEpilogue 12, EndingStarRodReturn 8, EraseConfirmDialog 3, EraseFileWipe 4, ExplosionScreenFlash 1, FileMenuHighlight 4, FileMenuSlot 4, FileSelectCursor 4, FileSelectSlot 2, FileSelectSlotLabel 1, FireLion 11, Flamer 12, FlamerFlame 1, GameOverChoice 2, GameOverObject 2, GameOverPalette 4, GameOverPlayer 5, GameOverSprite 1, Gip 5, Glunk 2, GlunkShot 1, GoalGameBigTrailStar 1, GoalGameHelperKirby 1, GoalGameLaunchStars 1, GoalGameSign 1, GoalGameSmallTrailStar 1, GrandWheelie 17, GrandWheelieMiniWheelie 2, HalveScore 2, HeavyMole 5, HeavyMoleArm 7, HeavyMoleEye 1, HeavyMoleRedMissile 1, HeavyMoleSmoke 3, HeavyMoleYellowMissile 1, HitFrost 2, HotHead 7, HotHeadFire 3, IceBlock 3, ImpactStar 1, InhalableStar 1, IntroStoryPicture 1, Kabu 13, KingDedede 16, KingDededeStar 2, Kracko 19, KrackoCloud 4, KrackoJrOrbs 3, KrackoLightningMiddle 1, KrackoLightningTop 1, KrackoStarman 1, LandingImpact 2, LaserBall 12, LevelDoorSign 1, LinkPlayCable 4, LinkPlayColorCycle 4, LinkPlayConsole 4, LinkPlayPalettePulse 6, LinkPlayPlayerList 9, MaceKnightMace 2, MapEvent 20, MenuBackground 4, MenuBgPaletteCycle 7, MenuScreenTitle 3, MetaKnight 10, MetaKnightCape 1, MetaKnightMask 1, MetaKnightMaskHalf 2, MetaKnightSword 4, MetaKnightSwordHitBox 1, MetaKnights 11, MetaKnightsKnight 16, ModeListCursor 4, ModePlayerCountPanel 3, MrFrosty 10, MrFrostyIceCube 4, MrShineAndMrBright 19, MrShineAndMrBrightAttack 6, MrTickTock 14, MrTickTockNote 1, MrTickTockRing 2, MuseumAbilitySign 1, Needlous 5, NightmarePowerOrb 6, NightmarePowerOrbEscape 1, NightmarePowerOrbEscapeStar 5, NightmarePowerOrbIntroScroll 1, NightmarePowerOrbStar 1, NightmarePowerOrbStarAfterimage 1, NightmarePowerOrbStarTrail 1, NightmarePowerOrbStarTrailDown 1, NightmarePowerOrbStarTrailUp 1, NightmareWizard 11, NightmareWizardCloakHands 2, NightmareWizardHitBox 1, NightmareWizardPalmTornado 1, NightmareWizardPointTornado 1, NightmareWizardStar 3, Noddy 4, NormalExtraPanel 3, PaintRoller 3, PaintRollerPainting 3, PaletteAnim 13, Parasol 5, Pengy 5, PengyIceBreath 1, PengyIceBreathSparkle 1, PhanPhan 6, PhanPhanApple 1, Player 39, PlayerCountPanel 3, PlayerEffect 2, PlayerObject 2, PoppyBrosJr 14, PoppyBrosSr 11, PoppyBrosSrBomb 1, PoppyBrosSrHand 8, PoppyBrosSrHead 3, QuickDraw 16, QuickDrawDoorSign 1, RingStar 1, Rocky 3, RoomParticles 3, Scarfy 8, Shotzo 15, ShotzoCannonball 2, SirKibble 3, Slippy 4, SoundTestCursors 3, SoundTestPulse 5, Sparky 4, Squishy 4, StageDoorSign 4, StageEffect 1, StarFlashOnParent 3, StarRodPiece 2, Starman 11, SubGame 1, SwordAndBladeKnight 8, SwordAndBladeKnightSlash 1, TitlePalette 1, TitleSprites 4, TridentKnightTrident 1, Twister 7, Twizzy 11, UFO 8, WaddleDee 4, WaddleDoo 5, WaddleDooBeam 2, WarpStar 12, WarpStarStationDoorSign 1, WarpStarStationDoorSparkle 1, WarpStarStationLevelSign 1, WarpStarStationNumber 1, WarpStarTrailStar 3, Wheelie 10, WhispyWoods 5, WhispyWoodsAirPuff 3, WhispyWoodsApple 4, WhispyWoodsLeaves 4.

`unk*` fields by header struct: `LinkSave` 33, `PlayerState` 21, `M37Player` 19, `Task` 17, `M37CoursePlayer` 13, `AttackBox` 10, `M37Game` 9, `Unk03005530` 9, `BodyBox` 8, `LinkRec` 8, `Unk03005550` 8, `M37Course` 7, `Actor` 6, `Door` 6, `M04Spark` 6, `GfxDesc` 5, `GfxSrc` 5, `HudBar` 5, `M37Obj` 5, `M37ObjSet` 5, `RoomDef` 5, `M19Frame` 4, `M19Particle` 4, `M19Script` 4, `M37Results` 4, `Unk03005670` 4, `M12Fade` 3, `SaveSlot` 3, `Unk02005E00` 3, `Unk03004B00` 3, `Unk03005680` 3, `Unk0873A994` 3, `ActorDef` 2, `BgMap` 2, `M11Buf` 2, `M11R8` 2, `M37Script` 2, `Unk02004B90` 2, `Unk020060A0` 2, `Unk0873EAC0` 2, `ActorHandlers` 1, `ActorSpawn` 1, `GfxHeader` 1, `HitBoxSet` 1, `M38LogoObj` 1, `Unk02007D70` 1, `Unk0873EEA0` 1.

<!-- audit:placeholders:end -->

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

### 6.2 Register aliases: `tools/task_alias.py`

```sh
tools/task_alias.py --defs defs.csv --sites sites.csv          # dry run
tools/task_alias.py --defs defs.csv --sites sites.csv --write  # apply
tools/task_alias.py --verify-cpp HEAD    # every unit preprocesses the same
tools/task_alias.py --verify-types       # gcc 12: aliases only on struct Task
make clean && make compare
tools/rename.py --verify-diff master     # covers the aliases too
tools/task_alias.py --list               # the aliases per family
```

`defs.csv` (`family,header,alias,field,type,role,evidence`) gives one row
per alias; `header` describes a new family's block.  `sites.csv`
(`file,function,pointer,field,alias`) says where each alias is used: inside
the body of FUNCTION, every `POINTER->FIELD` / `POINTER.FIELD` outside
comments becomes `POINTER->ALIAS`, and a row that matches nothing is an
error.  The tool writes `include/task_vars.h` (one block per family, the
defines in offset order) and appends the `renames.csv` rows.

Apply names in batches of about 50-100 and run `make clean && make compare`
after every batch.  gcc 2.95 hashes some RTL by symbol name (lessons 4.79,
4.86, 3.493), so a rename is not guaranteed to be codegen-neutral in
principle; if a batch breaks the match, bisect it, revert the one rename
that did it, and write the finding down as a lesson.  Never "fix" a broken
match with a code change.
