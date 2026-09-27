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
| Struct fields | `camelCase` | `unk10` until named (run 2 of #155) |
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
- `code:` what the body itself does, read from the C: the cells it reads
  and writes, the loop it runs, what it returns (for example, code: the
  body is `while (1) TaskYieldTrampoline(0x7FFF)`).  Enough on its own only
  for a small function whose whole contract is visible.
- `hw:` a hardware register the code drives: `hw: writes REG_SIOCNT
  0x4003 (multi-play, 115200 bps, IRQ) and REG_RCNT 0`.
- `string:` a string or ID the code reads or compares (`"AGB  KIRBY"`).
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

- **struct fields** (`Task.unkXX`, `PlayerState.unkXX`, `RoomDef.unkXX`):
  they touch `include/task.h` and hundreds of sites and come in run 2 of
  #155, one struct at a time, with the same evidence rules;
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

`docs/analysis/renames.csv` is the alias table.  The lessons, the rom-map
and the module-map keep the names of their time; a reader maps an old name
through it.

Apply names in batches of about 50-100 and run `make clean && make compare`
after every batch.  gcc 2.95 hashes some RTL by symbol name (lessons 4.79,
4.86, 3.493), so a rename is not guaranteed to be codegen-neutral in
principle; if a batch breaks the match, bisect it, revert the one rename
that did it, and write the finding down as a lesson.  Never "fix" a broken
match with a code change.
