# AGENTS.md

## Project

Matching decompilation of Kirby: The Amazing Mirror's predecessor, **Kirby: Nightmare in Dream Land** (GBA, 2002). Goal is code that compiles to output matching the original ROM, not a rewrite or port.

- Language: C/C++.
- All code, comments, commit messages, and documentation must be in **English**.
- Before decompiling a new module, read `docs/decomp-loop.md` — the standard
  per-function loop (pick → m2c first pass → asmdiff iterate → decomp-permuter
  escalation → land + verify) including the subagent handoff contract — and
  `docs/lessons-learned.md` — pitfalls and validated workflow from previous
  modules (build-system gotchas, m2c/tooling, old_agbcc source shapes). Add new
  lessons there as they are discovered.

## ROM handling

- Builds require a user-supplied, legally-dumped `baserom.gba`. The ROM is **never committed**; it must always be gitignored.
- Do not commit or link to ROM contents, extracted copyrighted assets, or other people's dumps.

## Builds

- All compilation happens in **Docker**; do not install toolchains (compiler, devkitARM, etc.) on the host machine.
- Do not assume host toolchains exist. If a `Dockerfile`/build script is missing or broken, fix or extend it rather than building natively.
- Commands:
  - `make image` — build the toolchain image (Debian 12 + `arm-none-eabi` binutils + pinned agbcc fork `jiangzhengwenjz/agbcc@new_newlib_pret`, commit `59b966e`).
  - `make` / `make all` — build `knidl.gba` (header from source + `baserom.gba` via `.incbin`) and patch the header with `tools/gbafix.py`.
  - `make compare` — build and verify SHA-1 against `knidl.sha1` (USA `A7KE`, SHA-1 `37a476567d133c146fee6b5e2eb0b07a215da6b0`).
  - `make progress` — parse `build/knidl.map` with `tools/calcrom.pl` into code/data byte counts and percentages.
  - `make check-headers` — compile-only smoke test of `include/gba/*.h` (`tools/header_smoke.c`) with agbcc + old_agbcc; never linked into the ROM.
  - `make clean` — remove `build/` and `knidl.gba`.
- Header fields for `gbafix`: title `AGB KIRBY DX`, code `A7KE`, maker `01`, version `0`. Internal ROM codes are `A7K*` (not `AKT*`).

## Git / PR workflow (mandatory for agents)

- `master` is the main branch and the ONLY valid PR base. `init` is a frozen bootstrap snapshot — never merge or push work into it (it may appear as origin/HEAD locally; ignore that).
- Work on a feature branch, open the PR against `master`, and wait for CI ("Build and verify") to pass.
- **Do NOT merge PRs.** The repo owner reviews and approves every merge personally. An agent's job ends with: PR open, CI green, a clear description (what/why, evidence of `make clean && make compare` OK), and a comment or summary pointing at anything a reviewer should double-check.
- Issues auto-close via "Closes #N" only when the owner merges to `master` — do not close issues manually.

## Conventions

- pret-style layout: `src/` (decompiled C), `asm/` (hand-written assembly), `data/` (extracted blobs), `tools/`, `linker.ld`, `<game>.sha1`.
- The Nintendo logo and any copyrighted assets are `.incbin`'d from `baserom.gba` at build time, never committed.
- Compiler: agbcc family (validated in `docs/research/compiler-validation.md`, issue #7): default `agbcc` with `-O2 -mthumb-interwork` for `src/`; `old_agbcc` with `-O1 -mthumb-interwork` for SDK files (m4a, `0x080CF9xx` zone — confirmed byte-exact on `src/agb_sram.c`, issue #8); `agbcc_arm` only for ARM-mode units. Fork flags `-fhex-asm -f2003-patch -ffix-debug-line` are safe additions (no codegen change).

## Status

- `make compare` passes (ROM built from source matches baserom byte-for-byte).
- CI (`.github/workflows/build.yml`): toolchain image + baserom-free compile/tooling checks always run; `make compare` runs only when a `baserom.gba` is available (self-hosted runner, Actions cache, or `BASEROM_URL` secret) and fails closed on hash mismatch; otherwise skipped explicitly.
- Progress tracking: `make progress` (`tools/calcrom.pl`, vendored from katam, adapted to this repo's `build/` layout and custom section names).
- README.md / INSTALL.md follow pret conventions (ROM facts, Docker-only builds, no-affiliation and dump-your-own-cartridge disclaimers, no OSS license).
- ROM split into 30 address-pinned sections in `linker.ld` (boundaries from `docs/analysis/segments.txt`); each section is a per-segment `.incbin` slice in `data/`.
- Research docs with sources live in `docs/research/` (prior art, toolchain, tooling pipeline, ROM facts + bootstrap checklist).
- First C module decompiled (issue #8): SRAM driver `src/agb_sram.c` (`0x080CFA9C-0x080CFC2F`, old_agbcc `-O1`), linked from C; ROM remains byte-identical.
- First game-side C module decompiled (issue #28): `AgbInit` `src/agb_init.c` (`0x08000310-0x080008E7`, default agbcc `-O2 -mthumb-interwork`), including its post-epilogue pool-skip branch and 121-word literal pool; the old `agb_init`/`game_code_early` boundary at `0x080006FF` was an analysis artifact (it split the final `bx r0` and orphaned the pool) and is now `0x080008E8`. The ~90 IWRAM/EWRAM cells it initializes are named `gUnk_<addr>` via `split_config.json` `data_symbols`. Matching shapes documented in `docs/lessons-learned.md` §3.6-§3.11.
- Game main loop decompiled (issue #33): `AgbMain` `src/main.c` (`0x08007300-0x080075B7`, default agbcc `-O2 -mthumb-interwork`), the 23-state dispatch loop at the start of `game_code_and_rodata` (boot flow in rom-map §4). The symbol formerly called `main` is now `AgbMain` (no `__gccmain` call in the ROM proves the original name wasn't `main`, lesson 3.13); the crt0 ARM entry `0x080000C0` was renamed `Start`. Its 6 RAM cells are `gUnk_<addr>` data_symbols.
- Per-function decompilation tooling (post-#28): `tools/fnmatch.sh <start> <end> <file.c> [--old]` byte-verifies a candidate C file against the ROM without touching the build (auto stand-ins, pool-resolving diff); `tools/carve.py <start> <end> <name> [--write]` lands a verified range as a `c_code` segment (rewrites segments.txt/split_config.json/linker.ld with validation). Workflow: `docs/decomp-loop.md` §3/§5.
- Platform headers complete (issue #27): full I/O map (`include/gba/io_reg.h`), interrupt IDs + master-ISR dispatch order (`include/gba/interrupts.h`), SDK-order SWI numbers + thunk prototypes (`include/gba/syscall.h`), umbrella `include/gba/gba.h`; conventions in `docs/header-conventions.md`, guarded by `make check-headers` (agbcc + old_agbcc).
- ROM-wide symbol database (issue #22): `tools/symdb.py` + `tools/symdb_check.py` via `make symbols` (Docker); committed `docs/analysis/symbols.csv` and `docs/analysis/callgraph.csv` (5,241 functions / 19,364 edges as of #31); validated against a fresh dual-view objdump disassembly (see `docs/analysis/rom-map.md` §7).
- ROM splitter (issue #23): `tools/split.py` + `tools/split_config.json` via `make split` extracts segments into labeled, byte-verified `asm/<segment>.s` (functions labeled from the symbol DB, symbolic literal pools, `asm/rom_syms.s` absolute symbols for unsplit targets) and removes the replaced `data/<segment>.s` incbin; usage and pitfalls in `docs/splitting.md` + `docs/lessons-learned.md` §4.
- All SDK/ARM segments around the code region converted from `.incbin` to labeled asm (issue #24): `task_switch_helpers`, `task_literals`, `sdk_swi_wrappers`, `sdk_reset_helper`, `sdk_libc` (`_call_via_r0..lr` exported; task trampolines decoded in rom-map §6), `interworking_veneer` (+ its literal-word gap), `irq_handler_table_14`, `lib_misc`, `lib_rodata_fir_tables`. Task-system IWRAM cells are named via config `data_symbols`, non-DB labels via `extra_labels`; hand names must always go through `tools/split_config.json` because CI re-checks split regeneration byte-for-byte.
- Whole Thumb game-code region split into per-function labeled asm (issue #25): `game_code_early` (2 chunks; `agb_init` decompiled to `src/agb_init.c` in #28) and `game_code_and_rodata` (14 ~64 KiB chunks, 5,003 functions) live under `asm/<segment>/<segment>_NN.s` via the config's `chunk_bytes`. Chunks share the segment's linker section (ld concatenates them in address order); cross-chunk branches use global `loc_XXXXXXXX` labels; no `.incbin` remains below `0x080D0000`. objdump→gas hazards are auto-repaired per instruction (`-marmv4t`, error-line feedback, post-assemble byte-diff feedback — lessons §4.15–4.17); ROM stays byte-identical.
- Decomp-permuter vendored + standard loop documented (issue #26): `tools/decomp-permuter/` (simonlindholm/decomp-permuter@`2795247`, own MIT LICENSE kept; Dockerfile gained the required `toml` pip dep) verified inside `knidl-builder` on a scratch example (`tools/permuter-example/`, scorer reaches 0 against ROM-extracted target asm); per-function workflow + subagent handoff contract in `docs/decomp-loop.md`, old_agbcc-specific pitfalls in lessons §2.9–2.11.
- m4a/mp2k sound engine located and fully labeled (issue #31): engine code `0x080CD89C-0x080CFA4B` (asm core + C driver halves, boundaries/evidence in `docs/analysis/rom-map.md` §8), 91 canonical names in the symbol DB (94 entries in the range; 3 tiny bx-r3 shims stay sub_*) (incl. 10 dead SDK exports via the new `EXTRA_THUMB_ENTRIES`/`curated` evidence mechanism in `tools/symdb.py`), engine RAM cells named via `data_symbols` (`gSoundInfo` `0x030056D0`, `SOUND_INFO_PTR` `0x03007FF0`, players/tracks, `gSoundMainRAM_Buffer` `0x03007150`), engine rodata tables mapped at `0x0860A140-0x0860B797` (byte-identical to pokeemerald's — same engine revision). Decompilation proceeds in child issues per rom-map §8.5.
- Bulk code clustered into a module map (issue #34): `docs/analysis/module-map.md` + `docs/analysis/module-map.csv` (`make modmap`, `tools/modmap.py`, CSV regeneration checked in CI) partition the remaining `0x080075B8-0x080CD89C` (792.7 KiB, 4,950 functions) into **37 contiguous candidate modules** of 12-31 KiB with per-module evidence (anchor tables, task types, call traffic, pool references, difficulty, suggested batches) and a five-wave decompile order; child issues of #35 are generated from it. Key findings: the ROM task-type table at `0x0872FF30` has **266 entries whose second word is the task body's entry point** (not a flag word — corrects rom-map §6 / `src/early_58e4.c`), seg 7 references the I/O block only 20 times in 792 KiB (everything goes through the early zone's IWRAM shadows), and 3,288 of 5,045 functions are reachable only through ROM pointer tables.
- Enemy/object behaviour bank 9 decompiled (issue #74): module M28
  `0x0809BA44-0x080A158F` (22.8 KiB) landed as `src/enemy_9ba44.c`,
  `src/enemy_9c028.c`, `src/enemy_9c0a8.c`, `src/enemy_9cb90.c`,
  `src/enemy_9cc24.c`, `src/enemy_9d994.c`, `src/enemy_9da1c.c`,
  `src/enemy_9f2f4.c`, `src/enemy_9f37c.c`, `src/enemy_9f9dc.c`,
  `src/enemy_9fbd0.c`, `src/enemy_a00ec.c` and `src/enemy_a0274.c` (**all 204
  functions, no asm left in the range**; the last one, `sub_080A00EC`, the
  392-byte three-star burst stepper, fell to the address-reload phase mechanism
  in `docs/lessons-learned.md` 3.258 after 3.249-3.257 document the road there).
  Despite the census name this is NOT one
  behaviour bank: seven ROM task types share the range, and three of them are a
  four-lane actor spawner (#57), a six-variant enemy family whose variant picks
  both the script and the OAM palette bank (#58) and the player's damage/death
  -and-retry coroutine (#59, including the three-star burst and the
  `gUnk_02006190[]` save block).  Eight census rows corrected in
  `tools/symdb.py` (one pool-skip-branch phantom removed, seven hidden entries
  added) and 80 ROM/RAM cells named via `split_config.json` `data_symbols`;
  `tools/addsyms.py` is new and folds the linker's `undefined reference to
  gUnk_<addr>` wall back into that map.
- Enemy/object behaviour bank 7 decompiled (issue #75): module M26
  `0x08093F64-0x080988F7` (18.4 KiB) landed as `src/enemy_93f64.c`,
  `src/enemy_957bc.c`, `src/enemy_970c4.c` and `src/enemy_974c8.c` (all 148
  functions; no asm left in the range).
  Four scripted enemies in the M22/M25 three-table shape plus two companions
  and a room-edge wanderer; eight census entries curated in `tools/symdb.py`
  and 131 ROM tables named via `split_config.json` `data_symbols`.
- Enemy/object behaviour bank 5 decompiled (issue #70): module M24
  `0x0808CCE8-0x0809000C` (12.8 KiB) landed as `src/enemy_8cce8.c`,
  `src/enemy_8e404.c` and `src/enemy_8f41c.c` (all 158 functions; no asm left
  in the range).  Six scripted enemies in the M22/M25/M26 three-table shape
  plus the bank's own ArcTan2 aiming library; seven census rows corrected in
  `tools/symdb.py` (three `0xFFFFF000`/rom-pointer phantoms removed, four
  hidden entries added) and 65 ROM tables named via `split_config.json`
  `data_symbols`.
- Enemy/object behaviour bank 4 decompiled (issue #80): module M23
  `0x080860F8-0x0808CCE8` (27.0 KiB) landed as `src/enemy_860f8.c`,
  `src/enemy_88000.c` and `src/enemy_8aa68.c` (all 297 functions; no asm left
  in the range).  Fourteen scripted objects in the M22/M24/M25/M26 three-table
  shape plus two multi-state bosses (eleven and twelve states) and the shared
  nine-way walk probe `sub_08086f54`; sixteen census rows corrected in
  `tools/symdb.py` (fourteen hidden entries added, two graphics-blob phantoms
  removed) and 129 ROM tables named via `split_config.json` `data_symbols`.
- Enemy/object behaviour bank 2 decompiled (issue #71): module M21
  `0x0807F044-0x08082E67` (15.5 KiB) landed as `src/enemy_7f044.c`,
  `src/enemy_80b70.c` and `src/enemy_820b8.c` (all 200 functions; no asm left
  in the range).  Nine ROM task types in the M22/M24/M25/M26 three-table shape
  (eight class-3 dispatchers plus the class-4 coroutine #175), 21 entry/hook
  script pairs and a shared library of terrain probe, animation loops and
  facing flip; twenty-two census rows corrected in `tools/symdb.py` (five
  graphics-blob phantoms removed, seven prologue-filter misses and ten dead
  exports added) and 98 ROM tables named via `split_config.json`
  `data_symbols`.
- Enemy/object behaviour bank 1 decompiled (issue #77): module M20
  `0x08078B68-0x0807F044` (25.2 KiB) landed as `src/enemy_78b68.c`,
  `src/enemy_7aa5c.c` and `src/enemy_7d3b0.c` (all 414 functions; no asm left
  in the range).  Twenty-one ROM task types in the M21/M22/M24/M25/M26
  three-table shape, but the bank is **moving scenery** rather than enemies:
  the bodies drive the 16.16 velocity pair Task.unk54/unk58 and the gravity
  cell Task.unk60 from ROM constants and wait on the collision flag
  Task.unk7A.  Twenty-four census rows corrected in `tools/symdb.py` (five
  prologue-filter misses and nineteen dead exports) and 121 ROM tables named
  via `split_config.json` `data_symbols`.
- Cutscene / ending-sequence bank decompiled (issue #79): module M19
  `0x08070EC0-0x08078B68` (31.2 KiB) landed as `src/actor_70ec0.c`,
  `src/actor_72d8c.c`, `src/actor_74c0c.c`, `src/actor_763e8.c` and
  `src/actor_77ae0.c` (all 220 functions; no asm left in the range).  Eleven
  class-3 ROM task types (#8, #74-#79, #97-#99, #165) that run the game's
  non-interactive sequences - warp-star intro, stage-clear pose, goal-game
  walk, four-ring sparkle and the end credits.  Unlike the enemy banks these
  are NOT the three-table shape: each type dispatches one anchor table into a
  linear TaskYieldTrampoline script.  Three module-local records were
  identified (the 0x087401E4 script table with its forward/reverse step lists,
  the eight credits particles at 0x03000FE0 and the 24-entry animation rows at
  0x08740320/0x087404A0); eight census rows corrected in `tools/symdb.py`
  (three non-functions removed - a pool-skip branch, a phantom pool `bl` and a
  shared epilogue - and five hidden entries added) and 63 ROM/RAM cells named
  via `split_config.json` `data_symbols`.
- Save file / SRAM records + options partially decompiled (issue #94): module
  M34 `0x080B6154-0x080B9D0B` (14.9 KiB) landed as `src/save_b6a90.c`,
  `src/save_b63a4.c`, `src/save_b6c40.c`, `src/save_b6e44.c`,
  `src/save_b6290.c`, `src/save_b6d04.c`, `src/save_b6f38.c`, `src/save_b72bc.c`, `src/save_b77d4.c`,
  `src/save_b79b8.c`, `src/save_b7a9c.c`, `src/save_b7df4.c`,
  `src/save_b7e14.c`, `src/save_b8888.c`, `src/save_b8918.c` and
  `src/save_b8ea0.c` (**97 of 105 functions byte-matched**,
  `make clean && make compare` ROM-identical around six asm holes).  The range is three subsystems, not one: an HBlank
  wavy-scroll effect (`gUnk_0300003C` is the HBlank callback cell), the SRAM
  save file proper (four 256-byte slots at `gUnk_0200E600` mirrored to
  `0x0E000200`, each checksummed by the 28-word additive sum seeded
  `0x97538642`), and the link-play results screen (a class-1 task whose
  coroutines fill `0x080B9610-0x080B9D0C`).  One census row corrected in
  `tools/symdb.py` (`0x080B6B36`, the sixth `0xFFFFF0xx` pool-word phantom,
  lesson 4.40) and 40 RAM/ROM cells named via `split_config.json`
  `data_symbols`; new agbcc lessons 3.330-3.354 and 4.70.
- Sub-game framework + reaction-duel sub-game decompiled (issue #95): module
  M35 `0x080B9D0C-0x080BDA2B` (15.3 KiB) landed as `src/subgame_b9d0c.c`,
  `src/subgame_ba774.c`, `src/subgame_bb528.c`, `src/subgame_bc0cc.c` and
  `src/subgame_bd9e8.c` (**all 196 functions, no asm left in the range**, no
  `register`/`asm` pins).  The census name "game-mode flow + link lobby" was
  half right: the range is the framework `AgbMain` runs for its sub-game
  state - `gUnk_02007FCC` selects one of three sub-games (0 = the duel here,
  1 = M36's bomb-pass, 2 = M37's) and indexes four per-game tables at
  `0x087562A8-0x087562E3`, `gUnk_02007D2C` is the game/results phase,
  `sub_080ba150` the SIO handshake and task type #93 the controller - plus a
  complete reaction-duel sub-game (a seven-state round controller, a
  seven-state results screen and task type #94, its sprite objects).  Three
  hidden census entries added in `tools/symdb.py` (so 196, not 193
  functions) and 62 ROM/RAM cells named via `split_config.json`
  `data_symbols`; new agbcc lessons 3.361-3.366 and 4.71.
- Game-state bodies, boot/title sequence, screen loaders, pause screen and
  HUD decompiled (issue #96): module M02 `0x080075B8-0x0800B91F` (16.9 KiB)
  landed as `src/mode_075b8.c`, `src/mode_07b68.c`, `src/mode_082d0.c`,
  `src/mode_08664.c`, `src/gfx_08b8c.c`, `src/boot_091ac.c`,
  `src/hud_099fc.c`, `src/hud_0a130.c`, `src/hud_0aad0.c`,
  `src/hud_0b318.c` and `src/mode_0b44c.c` (**all 109 functions, no asm left
  in the range**; one zero-byte `asm("" ::: "r0")` clobber, no `register`
  pins).  It is what `AgbMain` dispatches into: the per-frame bodies of game
  states 5, 8/17/18/19, 9 and 20, which loop until the stage-request byte
  `gUnk_03002438` asks for a state change, the pause screen (request 5) or a
  lost life (6); the extra modes' title screen (state 13, which in
  single-pak link play first sends a multiboot image staged at
  `0x02020000`); the boot logo (state 1, task type #0) and the title screen
  / nine-scene intro story (state 3, task types #1, #2 and #237); the
  LZ77/Huffman screen loaders; and the HUD (lives `gUnk_02007D48[]`,
  health `gUnk_02005588[]`, score `gUnk_02006020[]`, a clock, all drawn into
  the tilemap buffer `gUnk_02005600`).  Nine census rows corrected in
  `tools/symdb.py` (five phantoms removed, four hidden entries added) and
  108 RAM/ROM cells named via `split_config.json` `data_symbols`; done by a
  four-agent fan-out with per-function declarations and mid-run handovers;
  new lessons 3.367-3.372 and 4.72-4.75.

- Main menu, its sprite tasks, the BG scroll animator and the stage
  sequence state decompiled (issue #99): module M03 `0x0800B920-0x08010357`
  (18.6 KiB) landed as `src/menu_0b920.c`, `src/menu_0c09c.c`,
  `src/menu_0ca10.c`, `src/menu_0d450.c`, `src/menutask_0daf8.c`,
  `src/menutask_0e314.c`, `src/menutask_0ea0c.c`, `src/menutask_0f180.c`,
  `src/bgscroll_0fcbc.c` and `src/mode_100ac.c` (**all 79 functions, no asm
  left in the range**; one zero-byte `asm("" ::: "r6")` clobber, no
  `register` pins), so `0x080075B8-0x08017667` (M02-M04) is contiguous C.
  `AgbMain` state 4 is the main menu: `sub_0800b920` dispatches on the menu
  screen `gUnk_020060D0` (file select, file menu, two-way choices, the mode
  list whose rows pick M35-M37's sub-game `gUnk_02007FCC`, erase, the sound
  test, the link-play connection screen) until the player starts a game
  (state 5 or 13) or backs out to the title.  All 22 class-4 task types
  #238-#259 are its sprites and background effects, among them #257, a BG
  scroll animator for up to eight BG/axis scrolls (`gUnk_02004B74`,
  `gUnk_02006070[2][4]`, `gUnk_020061B0[2][4]`, shadow pointer tables
  `0x08731DA0`/`0x08731DB0`).  State 7 (`sub_080100ac`) plays the stage's
  scripted sequence through M04's director, task type #91.  Six census rows
  corrected in `tools/symdb.py` (five phantoms removed, one hidden leaf
  added) and 83 RAM/ROM cells named via `split_config.json`
  `data_symbols`; done by a four-agent fan-out after the coordinator seeded
  the cross-batch leaves; new lessons 3.373-3.378 and 4.76-4.78.

- Camera, BG map streaming, the type-#4 map events and the stage objects
  #221-#236 decompiled (issue #86): module M08 `0x080296A0-0x08030803`
  (28.3 KiB) landed as `src/camera_296a0.c`, `src/camera_29c74.c`,
  `src/bgmap_2a9cc.c`, `src/bgmap_2b2f0.c`, `src/camera_2b4bc.c`,
  `src/camera_2c42c.c`, `src/camera_2d01c.c`, `src/camtask_2d38c.c`,
  `src/obj_2eac8.c`, `src/obj_2f62c.c`, `src/obj_30238.c` and
  `src/obj_306b4.c` (**all 151 functions, no asm left in the range**, no
  `asm` statements and no `register` pins).  It is one subsystem with M07
  (the level / room builder, still asm), which calls it every frame: the
  camera (mode `gUnk_030055C0`, pixel position `gUnk_03005604`, 16.16
  target `gUnk_03005614`/`gUnk_03005634`, camera and room bounds
  `gUnk_030055F8`/`gUnk_03005628`, per-player cameras for link play, a
  scroll lock `gUnk_03005680` and a screen shake `gUnk_03005670`) that
  writes the BG1-BG3 16.16 scroll shadows; the tilemap streaming of the
  room's metatile map `gUnk_03005660` into the BG maps at `0x06001800`,
  `0x06002000` and `0x06003000`; task type #4, seven scripted map events
  (anchor table `0x087328A0`); the M07-placed stage objects #221-#235, each
  a spawner plus a `TaskYieldTrampoline` body and its callbacks; and #236,
  six sprite effects (anchor table `0x087328D8`).  Four census rows
  corrected in `tools/symdb.py` (three phantoms removed, one dead export
  added) and 73 RAM/ROM cells named via `split_config.json`
  `data_symbols`; done by a four-agent fan-out after the coordinator matched
  59 cross-batch leaves and twins, with a canonical `types.txt` of shared
  cell spellings and three mid-run handovers; new lessons 3.379-3.388 and
  4.79-4.81 (4.79: a function can match alone and not inside its carve
  file, because gcse hashes pool-label addresses).

- Level / room builder decompiled (issue #93): module M07
  `0x08021B18-0x0802969F` (30.9 KiB) landed as `src/terrain_21b18.c`,
  `src/level_2296c.c`, `src/roomtask_23618.c`, `src/level_23948.c`,
  `src/level_242d0.c`, `src/stage_261c0.c`, `src/door_26b60.c`,
  `src/stage_270d0.c`, `src/stage_273a0.c`, `src/room_27e28.c`,
  `src/room_28320.c` and `src/camera_28b8c.c` (**156 of 157 functions**,
  no `asm` statements and no `register` pins; the one hole is
  `sub_08027a6c`, 956 bytes, parked at 234 differing bytes with its best
  source on #93), so `0x08021B18-0x08030803` (M07+M08) is C except that
  one function.  It is the half of the
  level engine that decides which room is on screen and drives M08's camera:
  the room table `gUnk_087E1D58[level][stage][room]` gives the room header
  `gUnk_030055EC` (`struct RoomDef`, 0x58 bytes: BGM, compressed maps,
  tiles, palettes, BG map, origins, door records, object list); one loader
  per game state loads the room, resets the camera and the players and
  spawns task type #3 (class 4), whose body dispatches `Task.unk14` through
  the anchor table `0x08732614` into seven room-task variants gated by the
  per-frame flags `gUnk_03005624`; `sub_08024e40` finds the door under the
  player and `sub_08025024` (a 9-way `switch` on the door kind) enters it and
  raises M02's stage request `gUnk_03002438`.  The rest continue M06's map
  queries, hold the stage helpers the whole game calls and start the camera
  and the BG3 parallax layer.  `level_242d0.c` covers the part-3 loaders and
  the doors as one translation unit because the doors only match after them
  (lessons 4.79/4.86).  Six census rows corrected in `tools/symdb.py` (four
  dead exports added, the two long-jump phantoms inside `sub_08025024`
  removed) and 48 RAM/ROM cells named via `split_config.json`
  `data_symbols`; done by a four-agent fan-out after the coordinator matched
  59 cross-batch leaves, with seven handovers through `variants.sh`; new
  lessons 3.389-3.401 and 4.82-4.86.

- Breakable blocks + the player task decompiled (issue #92): module M09
  `0x08030804-0x0803627F` (22.6 KiB) landed as `src/block_30804.c`,
  `src/block_318b4.c`, `src/player_32688.c`, `src/player_337f4.c`,
  `src/player_343c0.c` and `src/player_34f8c.c` (**all 63 functions, no asm
  left in the range**, no `asm` statements and no `register` pins).  The
  census name "stage manager A" was wrong twice over: the first half is the
  breakable-block system (the per-metatile block layers `gUnk_02008160[]`
  and `gUnk_02004CA0[]`, the attack hit-box scans, the break test
  `sub_0803111c` and spawner `sub_08031374`, the 64 animated block records
  `gUnk_020061F0[]` and the three per-frame stage hooks M08 installs in
  `gUnk_030004A0`), the second is the player: task type #5 (class 1, one
  task per player record `gUnk_03002170[i]`), its callbacks, and the
  action machine M10-M14 continue - "enter" coroutines `gUnk_0873A748[62]`
  indexed by `PlayerState.unk02` and per-frame handlers `gUnk_0873A840[57]`
  indexed by `Task.unk15`, both dispatched through `sub_08002e98` with a
  NULL entry 0, written in M11's style (`src/stage_*.c`).  Three dead
  exports added in `tools/symdb.py` (so 63, not 60 functions), 35 RAM/ROM
  cells named via `split_config.json` `data_symbols`, `PlayerState.unk5C`
  named in `include/task.h`; done by a four-agent fan-out after the
  coordinator matched 24 leaves and family templates; the last function,
  `sub_08031f3c`, fell to two index spellings for two induction variables
  (lesson 3.409); new lessons 3.402-3.410 and 4.87-4.89.

- Player action bodies, part 2, decompiled (issue #91): module M10
  `0x08036280-0x0803CD5F` (26.7 KiB) landed as `src/player_36280.c`,
  `src/player_36c94.c`, `src/player_37ed8.c`, `src/player_3919c.c`,
  `src/player_39c24.c`, `src/player_3aa64.c` and `src/player_3bde8.c`
  (**all 39 functions, no asm left in the range**, no `asm` statements and
  no `register` pins), so `0x08021B18-0x08040B3F` (M07 through the start of
  M11) is C except M07's `sub_08027a6c`.  The census name "stage script
  runner" was wrong: the range is the second half of M09's player action
  machine - "enter" coroutines 10-21 and 23-28 of `gUnk_0873A748[62]` and
  per-frame handlers 9-25 of `gUnk_0873A840[57]` - among them the player's
  death (action 17, which raises the stage request `gUnk_03002438 = 6`),
  the door entry and walk (actions 20/21), a block-breaking attack
  (handler 13) and a health hand-over to the partner player (action 19),
  plus the spark, knock-back and motion helpers M09 calls; several bodies
  are twins of M11's `gUnk_03001F30` copies.  Five census rows corrected in
  `tools/symdb.py` (four long-jump phantoms inside the 4368-byte
  `sub_08037ed8` and the 3612-byte `sub_08039c24` removed, the hidden
  handler-17 leaf added, so 39, not 41 functions) and 20 ROM tables named
  via `split_config.json` `data_symbols`; done by a four-agent fan-out after
  the coordinator matched 17 templates, with three handovers and a
  two-agent race on the last function through `variants.sh`; new lessons
  3.411-3.415 and 4.90-4.91.

- Player action bodies, part 3, decompiled (issue #87): module M12
  `0x080449C8-0x08047FE7` (13.5 KiB) landed as `src/player_449c8.c`,
  `src/player_44d04.c`, `src/player_455c8.c`, `src/player_45d34.c`,
  `src/player_46330.c`, `src/player_46c00.c` and `src/player_474e8.c`
  (**all 21 functions, no asm left in the range**, no `asm` statements and
  no `register` pins), so `0x08043654-0x08047FE7` (the end of M11 and all
  of M12) is contiguous C.  The census name "large actor bank A" was
  wrong: the range is the third part of M09's player action machine -
  "enter" coroutines 34-43 of `gUnk_0873A748[62]` and per-frame handlers
  30-40 of `gUnk_0873A840[57]`, each enter body k followed by its handler
  k - 3 (handler 30 belongs to M11's action 33).  All are mode 13 and none
  switches on the ability: each action is one move, mostly with a ground
  and an air form, that installs the player's collider record
  `gUnk_020060E0[]` and block hit-box set `gUnk_02005550[]` from ROM
  templates - among them two sibling ground/air attacks, a dash and a spin
  with bounce-off states driven by the collision block `gUnk_03005550`, a
  three-charge move that plays a song per charge and drops the ability
  with the last one, and a palette blend of `gUnk_081BE6BC[player]`.  One
  census row corrected in `tools/symdb.py` (`0x08044A72`, a lesson 4.40
  phantom inside `sub_080449c8`'s own jump table, so 21, not 22 functions)
  and 34 ROM tables named via `split_config.json` `data_symbols`; done by
  a three-agent fan-out after the coordinator matched 13 templates, with
  no handovers; new lessons 3.416-3.420 and 4.93-4.94.

- Player action bodies, part 4, decompiled (issue #88): module M13
  `0x08047FE8-0x0804CC7B` (19.1 KiB) landed as `src/player_47fe8.c`,
  `src/player_49738.c`, `src/player_49b48.c`, `src/player_49f98.c`,
  `src/player_4a54c.c`, `src/player_4ab70.c`, `src/player_4b5b4.c`,
  `src/player_4b858.c` and `src/player_4c64c.c` (**all 24 functions, no
  asm left in the range**, no `asm` statements and no `register` pins; two
  zero-code `do { } while (0)` priority levers), so
  `0x08043654-0x0804CC7B` (the end of M11, M12 and M13) is contiguous C.
  The census name "large actor bank B" was wrong, as M12's was: 22 of the
  24 functions are entries of M09's two player action tables - "enter"
  coroutines 29, 44-48 and 50-54 of `gUnk_0873A748[62]` and per-frame
  handlers 26, 41-45 and 47-51 of `gUnk_0873A840[57]`, each enter body k
  followed by its handler k - 3 - and the other two are the ability
  sprite-tile loaders `sub_08049738`/`sub_08049a58` that M09, M10, M14 and
  M18 call (`void (void)`, as they already declared them).  Among the
  actions: the ability get (action 29, mode 19: the stage freeze, the HUD
  roulette, the new ability's tiles and a 25-way pose `switch` on the
  ability), the twins of M11's actions 32-33 (44-45), a rolling move (50),
  a screen-wide blast that drops the ability (51), a six-move stance (53,
  `loop: switch` over eight states) and a charged three-way move (54).
  Three census rows corrected in `tools/symdb.py` (two long-jump targets
  inside the 5276-byte `sub_08047fe8`, which the census capped at 0x1000,
  and the loop head of `sub_0804b858`, so 24, not 27 functions) and 31 ROM
  tables named via `split_config.json` `data_symbols`; done by a
  three-agent fan-out after the coordinator matched ten templates, with
  two handovers and a two-agent race on the last function; new lessons
  3.421-3.427 and 4.95-4.96.

- Player action bodies, part 5, the action 49/58 sub-action tables and
  task type #6 decompiled (issue #90): module M14 `0x0804CC7C-0x08053AF3`
  (27.6 KiB) landed as `src/player_4cc7c.c`, `src/player_4dc08.c`,
  `src/player_4e5a4.c`, `src/player_4e78c.c`, `src/player_4ee08.c`,
  `src/player_4f614.c`, `src/player_4f948.c`, `src/player_4ffdc.c`,
  `src/plobj_507bc.c`, `src/plobj_509ec.c`, `src/plobj_514f8.c`,
  `src/plobj_5239c.c` and `src/plobj_52f6c.c` around PR #133's
  `src/sub_0804e3a0.c` (**all 81 functions, no asm left in the range**,
  no `asm` statements and no `register` pins), so
  `0x08043654-0x08053AF3` (the end of M11 through M14) is contiguous C.
  The census name "stage manager B" was half right: the range holds the
  last asm entries of M09's two action tables - "enter" coroutines 30,
  31, 49 and 55-58 of `gUnk_0873A748[62]` and per-frame handlers 27, 28
  (PR #133), 46 and 52-55 of `gUnk_0873A840[57]`, enter k followed by
  its handler k - 3 - which completes both tables; two move sets one
  level down (action 49 dispatches `Task.unk73` through nine sub-actions
  `gUnk_0873B664` and its handler through nine sub-handlers
  `gUnk_0873B688`, action 58 through four each, `gUnk_0873B6AC`/
  `gUnk_0873B6BC`, with their helpers); and task type #6 (class 1, body
  `sub_080507bc`), the objects the player's actions spawn through
  `sub_08053940`/`sub_08053a44` (`s32 (s8 player, u8 variant, s32 arg)`;
  `Task.unk18 = variant << 24 | arg`), 13 variant bodies
  `gUnk_0873B77C[]` each switching on the sub-state `Task.unk18 & 15` and
  followed by its own callbacks.  Four census rows corrected in
  `tools/symdb.py` (the long-jump exit `0x0804D6C6` and the `b.n` arm
  tails `0x0804F6FA`/`0x08051008` folded, the sub-table entry
  `0x0804F5BC` added) plus one M38 phantom (`0x080CCAA6`, #100) the
  reshuffled spot check surfaced, and 79 ROM tables named via
  `split_config.json` `data_symbols`; done by a four-agent fan-out after
  the coordinator matched 21 cross-file helpers and representatives, with
  three handovers and a two-agent race on the last function; new lessons
  3.428-3.434 and 4.97-4.98.

- Task type #7, the player's effect objects, decompiled (issue #89): module
  M15 `0x08053AF4-0x0805AFAB` (29.2 KiB) landed as `src/effect_53af4.c`,
  `src/effect_54330.c`, `src/effect_54a80.c`, `src/effect_55460.c`,
  `src/effect_55b24.c`, `src/effect_56448.c`, `src/effect_56dd4.c`,
  `src/effect_57494.c`, `src/effect_57ce0.c`, `src/effect_58810.c`,
  `src/effect_59570.c` and `src/effect_5a358.c` (**all 84 functions, no asm
  left in the range**, no `asm` statements and no `register` pins), so
  `0x08043654-0x080B566F` (the end of M11 through the head of M33) is one
  contiguous C run.  The census name "link multiplayer mode" was wrong: its
  "SIO multi-play x162" counted calls into `src/early_6464.c`, 149 of them
  the random-range helpers `sub_080064ac`/`sub_080064dc`.  The range is
  task type #7 (class 1, body `sub_08053af4`): the effect objects the
  player's actions and M11's movement code spawn through M16's
  `sub_0805afac`/`sub_0805b088` (`Task.unk18 = variant << 24 | arg`), 49
  variant bodies `gUnk_0873B928[]` (motion/draw hooks, an animation table
  and a yield script, most switching on the sub-state `Task.unk18 & 15`)
  each followed by the 34 callbacks only they install (kill tests on the
  player's mode `PlayerState.unk04`, end flags in `Task.unk28`, hit tests,
  draw and palette callbacks).  Fourteen census rows corrected in
  `tools/symdb.py` (eight `0xFFFFF000` pool-word phantoms folded, six
  push-less companion leaves added, so 84, not 86 functions) and 50 ROM/RAM
  cells named via `split_config.json` `data_symbols`; done by a four-agent
  fan-out after the coordinator matched 23 functions (the body, the shared
  callbacks, every push-less companion and the first variants); the last
  register residues were agbcc's `local_alloc` sorting a block of exactly
  three quantities wrongly (lesson 3.435, found with an instrumented
  compiler, 4.100) and one pseudo per user variable (3.436); new lessons
  3.435-3.442 and 4.99-4.100.

- Sub-game 2 of M35's framework and `AgbMain` state 11 decompiled (issue
  #98): module M37 `0x080C1FFC-0x080C641F` (17.0 KiB) landed as
  `src/subgame_c1ffc.c`, `src/subgame_c243c.c`, `src/subgame_c2ff8.c`,
  `src/subgame_c3648.c`, `src/subgame_c3f44.c`, `src/subgame_c4630.c`,
  `src/subgame_c4d08.c`, `src/subgame_c5284.c`, `src/subgame_c623c.c` and
  `src/mode_c6260.c` around PR #133's `src/sub_080c6258.c` (**81 of 82
  functions** in C, no `asm` statements and no `register` pins; the one
  hole is the 1720-byte course renderer `sub_080c5b84`, parked at 22 bytes
  with its best source on #98), so `0x080B9D0C-0x080C5B83` (M35, M36 and
  most of M37) is contiguous C.  The census name "FIR-coefficient effect
  engine" was wrong: the `0x080CFE2C-0x080D0600` tables it was named after
  are this module's rodata.  The range is game 2 of the sub-game framework
  (`gUnk_02007FCC == 2`), a four-player race along four scrolling lanes:
  M36's `sub_080c1fdc` dispatches the phase `gUnk_02007D2C` through
  `gUnk_087572CC` (the race `sub_080c21b0`, the results `sub_080c243c`);
  task type #96 (class 3, body `sub_080c2ff8`) runs the racers (held A on a
  course segment accelerates, computer racers from per-level AI tables),
  seven scrolling background objects and nine kinds of effect sprites; the
  state is the 0x454-byte record `gUnk_02016C40` behind the pointer cell
  `gUnk_02017094` and the course record `gUnk_0201B0E0` behind
  `gUnk_0201716C`, which M35 fills through the course builder
  `sub_080c59d8` and `sub_080c5b84` (asm) renders column by column;
  the sky is a 160-line HBlank-DMA backdrop gradient.  After the real seam
  at `0x080C6260` comes `AgbMain` state 11 (`src/mode_c6260.c`), two scenes
  directed by M38's task types #100/#101 before state 12.  Four census
  rows corrected in `tools/symdb.py` (two lesson 4.95 phantoms folded, two
  push-less entries added, so still 82 functions) and 51 ROM/RAM cells
  named via `split_config.json` `data_symbols`; the shared structs lived in
  a prepended `canon.h` instead of per-function copies (lesson 4.101); done
  by a four-agent fan-out with plain handovers and races; new lessons
  3.443-3.452 and 4.101-4.105.
- The ending, the staff credits and the game-over screen decompiled (issue
  #100): module M38 `0x080C6420-0x080CD89B` (29.1 KiB), the last unstarted
  bulk module, landed as `src/results_c6420.c`, `src/ending_c6c64.c`,
  `src/ending_c7e4c.c`, `src/ending_c9004.c`, `src/boot_caa3c.c`,
  `src/gameover_cacf0.c`, `src/gameover_cb354.c`, `src/gameover_cb64c.c`,
  `src/gameover_cbed4.c`, `src/gameover_ccd4c.c` and `src/credits_cd330.c`
  (**109 of 110 functions**, no `asm` statements and no `register` pins;
  the one hole is the boot logo objects' 568-byte interpreter
  `sub_080caab8`, parked at 33 differing bytes with its best sources on
  #100), so `0x080C6260-0x080CD89B` is C except that function.  The census
  name "intro / cutscene / ending sequences?" was half right: it is the
  ending and the game-over screen.  `AgbMain` state 11 (M37's
  `src/mode_c6260.c`) plays two scenes directed by task types #100 and #101
  (class 3, variants `gUnk_08757330[11]` / `gUnk_087573F4[12]`, linear
  `TaskYieldTrampoline` scripts that end the scene by clearing
  `gUnk_02008018`); state 12 runs the staff credits `sub_080cd330` -
  recorded demos of the game, one per scene of `gUnk_087583CC[]`, played
  back by M34's recorder under a BG0 text layer streamed from the 14 LZ77
  pages `gUnk_087583B4[]` - and the final score screen `sub_080c6420`,
  then returns to state 0; state 22 is the game-over / continue screen
  `sub_080cacf0` with its objects, task types #260-#264 (class 4; #263
  halves the score, the price of a continue; #264's six variants
  `gUnk_08758294[]` include three M17-style state machines of sub-states on
  `Task.unk14` and per-frame handlers on `Task.unk15`).  The rest are the
  boot logo's 115 script-driven objects at `gUnk_02030000`, two picture
  screens M02 and M03 show, and the VRAM score/clock drawers.  Three census
  rows corrected in `tools/symdb.py` (the 4.40 phantom `0x080CD5AE` folded,
  the push-less entries `0x080CB6D8` and `0x080CD828` added, so 110
  functions) and 77 RAM/ROM cells named via `split_config.json`
  `data_symbols`; done by a four-agent fan-out after the coordinator matched
  46 leaves and representatives, with plain handovers and two races through
  `variants.sh`; new lessons 3.453-3.457 and 4.106-4.107 (4.106: the
  "23-entry" anchor table `0x08758294` is five dispatch tables back to
  back).
- Next milestones: decompile split/SDK modules to C using the validated per-zone compiler recipe (task system Thumb side, sound driver, then game code); grow `src/` one module at a time with `asmdiff.sh` on the module range.
