# Project history

The `## Status` section of `AGENTS.md` grew one bullet per issue from the
bootstrap (#7, #8) to the end of #155 run 3 and #36 phase 2 run 3: what each
module run decompiled, the census rows it corrected, the cells and tables it
named and the lessons it added.  Issue #37 replaced that section with a short
summary of the current state and moved the bullets here **verbatim and in
their original order** (as they stood at master `d351d26`), so the record and
the cross-references into it are kept.  Read them as history: figures,
function names (`sub_*`, `gUnk_*`; `docs/analysis/renames.csv` maps each old
name to its current one), the "next milestones" bullet and the first bullets'
"current state" describe the tree when they were written, not today's.  The
current state is `AGENTS.md`'s `## Status`; the lessons are
`docs/lessons-learned.md`.

## Status bullets, issue by issue


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
- Bulk code clustered into a module map (issue #34): `docs/analysis/module-map.md` + `docs/analysis/module-map.csv` (`make modmap`, `tools/modmap.py`, CSV regeneration checked in CI) partition the remaining `0x080075B8-0x080CD89C` (792.7 KiB, 4,950 functions) into **37 contiguous candidate modules** of 12-31 KiB with per-module evidence (anchor tables, task types, call traffic, pool references, difficulty, suggested batches) and a five-wave decompile order; child issues of #35 are generated from it. Key findings: the ROM task-type table at `0x0872FF30` has **266 entries whose second word is the task body's entry point** (not a flag word — corrects rom-map §6 / `src/task_move.c`), seg 7 references the I/O block only 20 times in 792 KiB (everything goes through the early zone's IWRAM shadows), and 3,288 of 5,045 functions are reachable only through ROM pointer tables.
- Enemy/object behaviour bank 9 decompiled (issue #74): module M28
  `0x0809BA44-0x080A158F` (22.8 KiB) landed as `src/enemy_meta_knights.c`,
  `src/enemy_meta_knights_lineup.c`, `src/enemy_axe_knight.c`, `src/enemy_axe_knight_slash_loop_update.c`,
  `src/enemy_javelin_knight.c`, `src/enemy_javelin_knight_overlay.c`, `src/enemy_mace_knight_trident_knight.c`,
  `src/enemy_trident_knight_overlay.c`, `src/enemy_meta_knights_knight.c`, `src/enemy_meta_knights_knight_palette.c`,
  `src/enemy_king_dedede_damage.c`, `src/enemy_king_dedede_inhale_particles.c` and `src/enemy_king_dedede.c` (**all 204
  functions, no asm left in the range**; the last one, `sub_080A00EC`, the
  392-byte three-star burst stepper, fell to the address-reload phase mechanism
  in `docs/lessons-learned.md` 3.258 after 3.249-3.257 document the road there;
  since #154 it and the module's other pinned functions are plain C, lesson
  3.513).
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
  `0x08093F64-0x080988F7` (18.4 KiB) landed as `src/enemy_grand_wheelie.c`,
  `src/enemy_fire_lion.c`, `src/enemy_fire_lion_flame.c` and `src/enemy_phan_phan.c` (all 148
  functions; no asm left in the range).
  Four scripted enemies in the M22/M25 three-table shape plus two companions
  and a room-edge wanderer; eight census entries curated in `tools/symdb.py`
  and 131 ROM tables named via `split_config.json` `data_symbols`.
- Enemy/object behaviour bank 5 decompiled (issue #70): module M24
  `0x0808CCE8-0x0809000C` (12.8 KiB) landed as `src/enemy_broom_hatter.c`,
  `src/enemy_shotzo.c` and `src/enemy_shotzo_coner.c` (all 158 functions; no asm left
  in the range).  Six scripted enemies in the M22/M25/M26 three-table shape
  plus the bank's own ArcTan2 aiming library; seven census rows corrected in
  `tools/symdb.py` (three `0xFFFFF000`/rom-pointer phantoms removed, four
  hidden entries added) and 65 ROM tables named via `split_config.json`
  `data_symbols`.
- Enemy/object behaviour bank 4 decompiled (issue #80): module M23
  `0x080860F8-0x0808CCE8` (27.0 KiB) landed as `src/enemy_bronto_burt_twizzy.c`,
  `src/enemy_slippy.c` and `src/enemy_blipper_gip.c` (all 297 functions; no asm left
  in the range).  Fourteen scripted objects in the M22/M24/M25/M26 three-table
  shape plus two multi-state bosses (eleven and twelve states) and the shared
  nine-way walk probe `sub_08086f54`; sixteen census rows corrected in
  `tools/symdb.py` (fourteen hidden entries added, two graphics-blob phantoms
  removed) and 129 ROM tables named via `split_config.json` `data_symbols`.
- Enemy/object behaviour bank 2 decompiled (issue #71): module M21
  `0x0807F044-0x08082E67` (15.5 KiB) landed as `src/enemy_kabu.c`,
  `src/enemy_starman_poppy_bros_jr.c` and `src/enemy_poppy_bros_jr_wheelie.c` (all 200 functions; no asm left
  in the range).  Nine ROM task types in the M22/M24/M25/M26 three-table shape
  (eight class-3 dispatchers plus the class-4 coroutine #175), 21 entry/hook
  script pairs and a shared library of terrain probe, animation loops and
  facing flip; twenty-two census rows corrected in `tools/symdb.py` (five
  graphics-blob phantoms removed, seven prologue-filter misses and ten dead
  exports added) and 98 ROM tables named via `split_config.json`
  `data_symbols`.
- Enemy/object behaviour bank 1 decompiled (issue #77): module M20
  `0x08078B68-0x0807F044` (25.2 KiB) landed as `src/enemy_sparky.c`,
  `src/enemy_sword_and_blade_knight.c` and `src/enemy_rocky.c` (all 414 functions; no asm left
  in the range).  Twenty-one ROM task types in the M21/M22/M24/M25/M26
  three-table shape.  #77 read the bank as moving scenery; #155 run 2's
  local sprite renders and the subtypes' `ActorDef.ability` show it is
  enemies (Waddle Dee's rows, Rocky, Pengy, Sir Kibble, Cappy, Gordo, Cool
  Spook, Kabu, Bomber, Sparky, Scarfy, the two sword knights, Needlous (run 2 said Togezo; run 3 corrected it), UFO
  and the parasol), whose bodies drive Task.velX/velY and Task.accelY from
  ROM constants and wait on Task.onGround.  Twenty-four census rows corrected in `tools/symdb.py` (five
  prologue-filter misses and nineteen dead exports) and 121 ROM tables named
  via `split_config.json` `data_symbols`.
- Cutscene / ending-sequence bank decompiled (issue #79): module M19
  `0x08070EC0-0x08078B68` (31.2 KiB) landed as `src/cutscene_warp_star.c`,
  `src/cutscene_warp_star_flights.c`, `src/cutscene_nightmare_power_orb_escape.c`, `src/cutscene_cannon.c` and
  `src/cutscene_big_switch_room_particles.c` (all 220 functions; no asm left in the range).  Eleven
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
- Save file / SRAM records + options decompiled (issue #94): module
  M34 `0x080B6154-0x080B9D0B` (14.9 KiB) landed as `src/main_hblank_bands_in_blend.c`,
  `src/main_hblank_bands_out_blend.c`, `src/main_hblank_bands_out.c`, `src/main_hblank_bands_in_link_play.c`,
  `src/main_room_hblank_scroll.c`, `src/main_hblank_row_scroll.c`, `src/main_hblank_uniform_scroll.c`,
  `src/main_hblank_row_scroll_reverse.c`, `src/main_hblank_scroll.c`, `src/save_input_recorder_start.c`,
  `src/save_input_recorder_restore.c`, `src/save_input_recorder_frame.c`, `src/save_init_slots.c`,
  `src/save_completion_percent.c`, `src/save_write_slot.c`, `src/save_slot_checksum.c`,
  `src/save_store_progress.c`, `src/save_receive_link_slots.c`, `src/save_exchange_link_slots.c`,
  `src/save_merge_link_slots.c` and `src/player_life_request.c` (**all 105 functions, no asm
  left in the range**).  PR #130 landed 97; the last eight (five wavy-scroll
  drivers, the input recorder pair and the link-record copy, 3,252 bytes)
  had resisted the first run's pins and clobber sweeps at 3-532 bytes and
  all matched in the second run from plain, pin-free source (no `asm`, no
  `register` in them): the four-loop family's preheader order that lessons
  3.336-3.353 called unreachable is agbcc's second loop pass
  (`-frerun-loop-opt`) hoisting the cell address after the HImode `256`
  (lesson 3.465), and the reload-scratch residues were the pins' doing
  (3.466).  The range is three subsystems, not one: a wavy-scroll effect run
  from the per-frame hook `gUnk_0300003C` (nine drivers build the per-line
  HBlank DMA table `gUnk_020164A0`), the SRAM save file proper (four
  256-byte slots at `gUnk_0200E600` mirrored to `0x0E000200`, each
  checksummed by the 28-word additive sum seeded `0x97538642`) with the
  link-play records `gUnk_0200EA00[]` and the run-length input
  recorder/playback the staff credits use, and the link-play results screen
  (a class-1 task whose coroutines fill `0x080B9610-0x080B9D0C`).  One
  census row corrected in `tools/symdb.py` (`0x080B6B36`, the sixth
  `0xFFFFF0xx` pool-word phantom, lesson 4.40) and 41 RAM/ROM cells named
  via `split_config.json` `data_symbols`; new agbcc lessons 3.330-3.354,
  3.465-3.471 and 4.70, 4.110.
- Sub-game framework + reaction-duel sub-game decompiled (issue #95): module
  M35 `0x080B9D0C-0x080BDA2B` (15.3 KiB) landed as `src/subgame.c`,
  `src/subgame_quick_draw.c`, `src/subgame_quick_draw_results.c`, `src/subgame_quick_draw_objects.c` and
  `src/subgame_bomb_rally_main.c` (**all 196 functions, no asm left in the range**, no
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
  landed as `src/mode_hub_stage.c`, `src/mode_extra_mode_title.c`, `src/mode_extra_mode_title_sprites.c`,
  `src/mode_pause_boss_endurance.c`, `src/mode_gfx_loaders.c`, `src/mode_boot_sequence.c`,
  `src/hud_init_counters.c`, `src/hud_hp_bars.c`, `src/hud_draw.c`,
  `src/hud_tilemap.c` and `src/mode_hub_stage_init.c` (**all 109 functions, no asm left
  in the range**; its one zero-byte `asm("" ::: "r0")` clobber went in #154,
  no `register` pins).  It is what `AgbMain` dispatches into: the per-frame bodies of game
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
  (18.6 KiB) landed as `src/menu_main_save_slots.c`, `src/menu_file_select.c`,
  `src/menu_mode_list.c`, `src/menu_sound_test_link_play.c`, `src/menu_file_select_tasks.c`,
  `src/menu_panel_tasks.c`, `src/menu_sound_test_tasks.c`, `src/menu_link_play_tasks.c`,
  `src/menu_bg_scroll.c` and `src/cutscene_main.c` (**all 79 functions, no asm
  left in the range**; its one zero-byte `asm("" ::: "r6")` clobber went in
  #154, no `register` pins), so `0x080075B8-0x08017667` (M02-M04) is contiguous C.
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
  (28.3 KiB) landed as `src/camera_stream_scroll.c`, `src/camera_player_group.c`,
  `src/camera_draw_bg_map.c`, `src/camera_block_anim_clip.c`, `src/camera_scroll_lock.c`,
  `src/camera_hub.c`, `src/camera_bg_anims.c`, `src/camera_map_events.c`,
  `src/camera_door_signs.c`, `src/camera_warp_star_station_door.c`, `src/camera_stage_effect.c` and
  `src/camera_world_sprite_block_anims.c` (**all 151 functions, no asm left in the range**, no
  `asm` statements and no `register` pins).  It is one subsystem with M07
  (the level / room builder, then still asm), which calls it every frame: the
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
  `0x08021B18-0x0802969F` (30.9 KiB) landed as `src/collision_terrain_init.c`,
  `src/room_reset_level_load_room.c`, `src/room_task.c`, `src/room_hub.c`,
  `src/room_enter_exit.c`, `src/room_stage_helpers.c`, `src/room_doors.c`,
  `src/room_stop_pause.c`, `src/room_restart_point.c`, `src/room_bg_layout.c`,
  `src/room_spawn_door_objects.c` and `src/room_camera_init.c` (**156 of 157 functions**,
  no `asm` statements and no `register` pins; the one hole is
  `sub_08027a6c`, 956 bytes, parked at 234 differing bytes, which the
  final campaign landed from plain source as `src/room_hub_map.c`), so
  `0x08021B18-0x08030803` (M07+M08) is all C.  It is the half of the
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
  and the BG3 parallax layer.  `room_enter_exit.c` covers the part-3 loaders and
  the doors as one translation unit because the doors only match after them
  (lessons 4.79/4.86).  Six census rows corrected in `tools/symdb.py` (four
  dead exports added, the two long-jump phantoms inside `sub_08025024`
  removed) and 48 RAM/ROM cells named via `split_config.json`
  `data_symbols`; done by a four-agent fan-out after the coordinator matched
  59 cross-batch leaves, with seven handovers through `variants.sh`; new
  lessons 3.389-3.401 and 4.82-4.86.

- Breakable blocks + the player task decompiled (issue #92): module M09
  `0x08030804-0x0803627F` (22.6 KiB) landed as `src/player_break_blocks.c`,
  `src/player_block_anims.c`, `src/player_task.c`, `src/player_stand_walk.c`,
  `src/player_run_jump.c` and `src/player_fall_float.c` (**all 63 functions, no asm
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
  NULL entry 0, written in M11's style (`src/player_helpers.c`, `src/player_meta_knight_*.c`).  Three dead
  exports added in `tools/symdb.py` (so 63, not 60 functions), 35 RAM/ROM
  cells named via `split_config.json` `data_symbols`, `PlayerState.unk5C`
  named in `include/task.h`; done by a four-agent fan-out after the
  coordinator matched 24 leaves and family templates; the last function,
  `sub_08031f3c`, fell to two index spellings for two induction variables
  (lesson 3.409); new lessons 3.402-3.410 and 4.87-4.89.

- Player action bodies, part 2, decompiled (issue #91): module M10
  `0x08036280-0x0803CD5F` (26.7 KiB) landed as `src/player_duck_slide.c`,
  `src/player_ladder_inhale.c`, `src/player_hurt.c`, `src/player_die_enter_door.c`,
  `src/player_exit_door.c`, `src/player_water.c` and `src/player_share_item.c`
  (**all 39 functions, no asm left in the range**, no `asm` statements and
  no `register` pins), so `0x08021B18-0x08040B3F` (M07 through the start of
  M11) is C (M07's last function landed in the final campaign).  The census name "stage script
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
  `0x080449C8-0x08047FE7` (13.5 KiB) landed as `src/player_spark_cutter.c`,
  `src/player_sword.c`, `src/player_burning_laser.c`, `src/player_mike.c`,
  `src/player_wheel.c`, `src/player_hammer.c` and `src/player_sleep.c`
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
  `0x08047FE8-0x0804CC7B` (19.1 KiB) landed as `src/player_get_ability.c`,
  `src/player_ability_tiles.c`, `src/player_ice_freeze.c`, `src/player_hi_jump.c`,
  `src/player_beam_stone.c`, `src/player_tornado_crash.c`, `src/player_light.c`,
  `src/player_backdrop_hold.c` and `src/player_throw_hold.c` (**all 24 functions, no
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
  (27.6 KiB) landed as `src/player_ufo.c`, `src/player_backdrop_throw.c`,
  `src/player_ball.c`, `src/player_ball_roll.c`, `src/player_ball_jump.c`,
  `src/player_ball_helpers.c`, `src/player_star_rod.c`, `src/player_star_rod_flight.c`,
  `src/player_object_task.c`, `src/player_object_air_puff.c`, `src/player_object_laser_beam.c`,
  `src/player_object_ice_breath.c` and `src/player_object_ufo_shot.c` around PR #133's
  `src/player_throw_update.c` (**all 81 functions, no asm left in the range**,
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
  M15 `0x08053AF4-0x0805AFAB` (29.2 KiB) landed as `src/effect_skid_dust.c`,
  `src/effect_slide_dust.c`, `src/effect_death_star_ring_ability.c`, `src/effect_dance_star_burst.c`,
  `src/effect_hurt_flames_sparks.c`, `src/effect_ability_puffs.c`, `src/effect_fire_breath_spark_aura.c`,
  `src/effect_burning_flames_wheel.c`, `src/effect_mike_attack.c`, `src/effect_ice_breath_freeze_aura.c`,
  `src/effect_hi_jump_ball.c` and `src/effect_crash_blast.c` (**all 84 functions, no asm
  left in the range**, no `asm` statements and no `register` pins), so
  `0x08043654-0x080B566F` (the end of M11 through the head of M33) is one
  contiguous C run.  The census name "link multiplayer mode" was wrong: its
  "SIO multi-play x162" counted calls into `src/link_driver.c`, 149 of them
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
  `src/subgame_air_grind.c`, `src/subgame_air_grind_results_sky.c`, `src/subgame_air_grind_racer.c`,
  `src/subgame_air_grind_racer_steps.c`, `src/subgame_air_grind_effects.c`, `src/subgame_air_grind_scenery.c`,
  `src/subgame_air_grind_sprites.c`, `src/subgame_air_grind_course.c`, `src/subgame_air_grind_depth_scale.c` and
  `src/ending_main.c` around PR #133's `src/subgame_air_grind_half_depth.c` (**81 of 82
  functions** in C, no `asm` statements and no `register` pins; the one
  hole was the 1720-byte course renderer `sub_080c5b84`, parked at 22 bytes,
  which the final campaign landed from natural source at the end of
  `src/subgame_air_grind_course.c`), so `0x080B9D0C-0x080C641F` (M35, M36 and M37) is
  contiguous C.  The census name "FIR-coefficient effect
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
  `sub_080c59d8` and `sub_080c5b84` renders column by column;
  the sky is a 160-line HBlank-DMA backdrop gradient.  After the real seam
  at `0x080C6260` comes `AgbMain` state 11 (`src/ending_main.c`), two scenes
  directed by M38's task types #100/#101 before state 12.  Four census
  rows corrected in `tools/symdb.py` (two lesson 4.95 phantoms folded, two
  push-less entries added, so still 82 functions) and 51 ROM/RAM cells
  named via `split_config.json` `data_symbols`; the shared structs lived in
  a prepended `canon.h` instead of per-function copies (lesson 4.101); done
  by a four-agent fan-out with plain handovers and races; new lessons
  3.443-3.452 and 4.101-4.105.
- The ending, the staff credits and the game-over screen decompiled (issue
  #100): module M38 `0x080C6420-0x080CD89B` (29.1 KiB), the last unstarted
  bulk module, landed as `src/ending_final_results.c`, `src/ending_epilogue.c`,
  `src/ending_epilogue_kirby.c`, `src/ending_star_rod_return.c`, `src/boot_logo_init_objects.c`,
  `src/game_over_screen.c`, `src/game_over_tasks.c`, `src/game_over_player.c`,
  `src/game_over_choice.c`, `src/game_over_objects.c` and `src/ending_credits.c`
  (**109 of 110 functions**, no `asm` statements and no `register` pins;
  the one hole was the boot logo objects' 568-byte interpreter
  `sub_080caab8`, parked at 33 differing bytes, which the final campaign
  landed as `src/boot_logo_update_objects.c` with two commented zero-byte levers), so
  `0x080C6260-0x080CD89B` is all C.  The census
  name "intro / cutscene / ending sequences?" was half right: it is the
  ending and the game-over screen.  `AgbMain` state 11 (M37's
  `src/ending_main.c`) plays two scenes directed by task types #100 and #101
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
- Box-vs-terrain collision engine + actor-vs-collider hit tests decompiled
  (issue #84): module M06 `0x0801A8C8-0x08021B17` (28.6 KiB) is C in 21
  files - PR #131's `src/collision_collide_box.c`, `collision_collide_point.c`,
  `collision_collide_box_tile_edge.c`, `collision_probe_begin_end.c`, `collision_probe_box_top.c`,
  `collision_probe_point_stop.c`, `collision_probe_water_drift.c` and `collision_query_pixel.c` (28
  functions), and this run's `src/collision_hit_test_players_class10.c`, `src/collision_hit_test_helpers.c`,
  `src/collision_player_probe.c`, `src/collision_probe_wall_on_ground.c`, `src/collision_probe_floor_in_camera_bounds.c`,
  `src/collision_probe_floor.c`, `src/collision_probe_wall_in_air_ceiling.c`, `src/collision_probe_landing_in_camera_bounds.c`,
  `src/collision_probe_landing.c`, `src/collision_probe_no_slope_link.c`, `src/collision_probe_along_velocity.c`,
  `src/collision_probe_point_push_out_tile_edge.c` and `src/collision_probe_water.c` (26) - **54 of 55
  functions**, no `asm` statements and no `register` pins; the one hole is
  `sub_0801b24c` (1424 bytes, the third collider list's hit test), parked
  at 44 differing bytes, which the final campaign landed as
  `src/collision_hit_test_class20.c`, so `0x0801A8C8-0x08030803` (M06-M08) is all C.  The census name "terrain / collision query (pure leaf)"
  was half right: the range is the collision engine every actor and player
  runs - ten per-frame entry points that load a box, compute its
  room-relative corners and run a probe set picked by the x velocity and
  the on-ground flag `gUnk_03005530.unk6` (wall, ceiling, floor-follow and
  landing probes that push the probe point `gUnk_03005560/gUnk_03005570`
  through M06's cell queries and the per-tile-set tables
  `0x087328F0-0x08734FF0` and fill the result block `gUnk_03005530`) - plus
  the actor-vs-collider hit tests M17/M18's actors run against the three
  collider lists M05's `sub_0801a828` fills (attack box
  `gUnk_0300236C`, damage and `ArcTan2` knock-back); not a leaf, it calls
  M07's map lookups and M02's health counter.  Census: the long-jump
  phantom `0x0801ECBA` folded (`sub_0801e178`'s shared epilogue) and the
  empty dead export `0x08020698` added, so 55 functions; 19 RAM/ROM cells
  named via `split_config.json` `data_symbols`; done by a four-agent
  fan-out with handovers and races through `variants.sh`; new lessons
  3.458-3.464 (3.458: cse learns `a == 1` from a branch and swaps a later
  AND, so the ROM ANDs the constant; 3.459: a goto dispatch puts the tests
  first and the bodies after them) and 4.108-4.109.
- Straggler campaign over seven parked allocation-residue functions
  (issues #125, #97, #84 and M11's #85): six landed as C in one file each,
  `src/cutscene_fountain_sparkles.c`, `src/cutscene_fountain_blend.c` and `src/cutscene_fountain_sprite_draw.c`
  (M05's last three, so M05 has no asm left), `src/player_motion_x_preset.c` and
  `src/player_meta_knight_swim_update.c` (M11's last two, so M11 is all C) and
  `src/room_enemy_gfx.c` (M33's last, so M33 is all C); 5,168 of 6,592 bytes.
  Five are plain pin-free C with no `asm` and no `register`;
  `sub_0804335c` keeps one commented zero-code `do { } while (0)` (the
  lever its M10 twin uses) and two documented stand-in locals.  The old
  "register residues" were mostly source differences, re-derived from the
  listings in the plainest style as M34's closing run advised (3.468): a
  `u8` declaration of the `vu8` blend shadows (`sub_08019eec`, 17 bytes for
  weeks), a 1-D spelling of a 2-D table and `for (;;)` for `while (1)`
  (`sub_08018e14`, 286 bytes), one draw call whose attribute bits were a
  `u16` variable (`sub_0801a3e4`), cached mask locals that let regmove AND
  in place (`sub_08040b40`, `sub_0804335c`, correcting 4.62/4.63), and in
  `sub_080b5670` a missing third call argument, the compiler's own QImode
  byte copy behind the `[sp, #4]` slot (correcting 3.269) and a VRAM base
  that must be a dropped pointer local (3.258).  `sub_0801b24c` (M06,
  1424 bytes) stayed parked at 44 differing bytes, now from plain source,
  with its residue traced to 3.464's reaching register and a cse1/cse2
  path effect (3.478); the final campaign landed it (3.492).  No census row changed; 13
  ROM tables named via `split_config.json` `data_symbols`; new lessons
  3.472-3.478 and 4.111.
- The engine zone's last asm functions (issue #63, the backlog #32 left):
  13 of the 14 functions of `game_code_early` that #32 parked landed as C
  in `src/main_affine_sprite.c`, `src/link_sync_random.c`, `src/sound_play_sfx.c`,
  `src/link_setup_init.c`, `src/link_multiboot_main.c`, `src/task.c`,
  `src/task_frame_tiles.c`, `src/link_cmd_queue.c` and `src/link_do_recv.c`, so
  `0x080008E8-0x08007300` is C except `sub_08006d28` (SerialCB, 356 bytes,
  parked at 15 differing bytes; the final campaign landed it as
  `src/link_serial_cb.c`, rom-map §6.4).
  Twelve are plain source and `sub_08002378` keeps one commented zero-code
  stand-in conjunct.  All fourteen were redrafted from the listings (#32's
  candidates were lost), and every #32 diagnosis turned out to describe
  the old candidate: two "Group B" proofs (3.35, 3.73) now carry correction
  notes.  Several are library code with public references - MultiBootMain
  is an older revision of the SDK source pokeemerald ships, the link driver
  an older revision of pokeruby's `src/link.c` (EnqueueSendCmd,
  DequeueRecvCmds, SerialCB, DoRecv) - and the recurring causes were
  constant-versus-symbol spellings of I/O and EWRAM addresses (gcse PREs
  symbol loads, never CONST_INTs), one variable serving several roles, and
  the libraries' older revisions.  `include/gba/syscall.h` now declares
  `MultiBoot` as returning int.  No census row changed; done by a
  three-agent fan-out; new lessons 3.479-3.488 and 4.112.
- The final campaign over the last five game functions (issues #63, #84,
  #93, #98, #100): `sub_08027a6c` (M07, `src/room_hub_map.c`),
  `sub_08006d28` (SerialCB, `src/link_serial_cb.c`), `sub_0801b24c` (M06,
  `src/collision_hit_test_class20.c`), `sub_080c5b84` (M37, appended to
  `src/subgame_air_grind_course.c`, whose nine landed functions stay byte-identical)
  and `sub_080caab8` (M38, `src/boot_logo_update_objects.c`), 5,024 bytes.  Four are
  plain source, SerialCB with one commented zero-code stand-in (a dead
  `i = 4;` before a `break`), and `sub_080caab8` keeps two commented
  zero-byte `asm` levers at one store, approved by the owner's coordinator
  (its natural best is on #100).  The two shared "compiler questions" had
  source answers: SerialCB's doubled live length was jump.c's else-arm swap
  running before register allocation (lesson 3.491), `sub_0801b24c` had no
  doubling residue at all (a `p` local and one mask spelling, 3.492), and
  gcse's PRE slot order is bucket arithmetic over declaration order, insn
  count and the file's pool-label count (3.493, 3.494); `sub_08027a6c`
  fell to a `u16` value and a struct copy (3.489).  New lessons 3.489-3.494
  and 4.113, with correction notes on 3.452, 3.457, 3.464, 3.478, 3.487,
  4.79 and 4.105.  Three agents, about two hours from the census to the
  last landing.
- **The game code is complete**: everything from `AgbInit` (`0x08000310`)
  to `0x080CD89B`, the start of `m4a_1`, is byte-exact C.  The asm left in the ROM is
  the sound engine's hand-written core `m4a_1`, crt0 and the ARM task
  switcher, and the SDK stubs (SWI thunks, SoftReset, lib1funcs and the
  task trampolines, the interworking veneer).  The #35 close-out moved
  `MidiKeyToFreq` and `UnusedDummyFunc` (the head of pokeemerald's `m4a.c`,
  wrongly inside the asm core's segment) into `src/m4a_c1.c`, listed the
  by-design asm as excluded in `tools/calcrom.pl` and counted asm-split
  tables as data (lesson 4.114).  `make progress`: 847028 of 851204 code
  bytes in `src/` (99.5094%), **0 bytes of code remaining to be
  decompiled**; 184 of 8282 symbols have real names.
- Natural-C campaign (issue #154): the 133 functions in 45 files that
  matched only with `register ... asm("rN")` pins or zero-byte `asm("")`
  levers (mostly M28-M33) were redrafted as plain C that still matches byte
  for byte: **130 of 133 are clean**, pins 622 -> 10 and `asm("")` levers
  189 -> 9, with two commented zero-code stand-ins (`sub_080a22d4`'s
  `do { } while (0)`, `sub_080b55d8`'s volatile read); `sub_080109c8` keeps
  #82's two commented `volatile` placeholder re-reads.  M29-M32
  (`enemy_mr_shine_and_mr_bright.c` ... `enemy_nightmare_power_orb.c`), M28 and the tail have no pin or
  empty `asm` left; `room_object_gfx.c` lost its file-scope r9-r11 register
  globals.  The census had missed `BLOCK_CROSS_JUMP` (`global.h`'s
  `asm("");` macro): 13 sites, 9 of them in three unlisted functions, now 4,
  all in `sub_0809fe10`, where the ROM really duplicates the tails (a
  sanctioned pret idiom).  Still carrying: `sub_080b38f0` (actor_whispy_woods_items.c, 10
  pins + 6 levers; best plain attempt 7 bytes off, an r3/r4 local-alloc
  order), `sub_08091e18` (enemy_poppy_bros_sr.c, one lever; lesson 3.156: cse folds
  every use of the decremented value inside its `== 0` block, 9 bytes) and
  `sub_080caab8` (boot_logo_update_objects.c, #152's two approved levers, 3.494).  The
  pins described the old candidates, not the functions: 36 matched with
  their pins merely deleted, and the rest fell to wrong declarations
  (`u16` for `s16`, `vu8` for `u8` and back, `u32 []` headers), literals
  written at every use, table elements read twice, full arms that
  cross-jump, variable roles and loop shapes.  No header or cross-file
  prototype changed; done by four agents plus the coordinator; new lessons
  3.495-3.514 and 4.115, and correction notes on 34 older lessons
  (3.229-3.281 and 3.341, 3.370 among them).
- Data structure, phase 1 (issue #36): every data segment of the ROM is a
  structure-only `data/<segment>.s` that `make split` generates
  (`tools/split.py`'s new emitter; convention in `docs/data.md`): real
  labels, symbolic pointer words and `.incbin "baserom.gba", <offset>,
  <length>` slices, and no ROM value anywhere (`make check-data`, run in CI
  on every push).  The six data segments split to value lists before
  (IRQ table, veneer literal, `lib_misc`, `lib_rodata_fir_tables`, the m4a
  engine rodata and song table) moved from `asm/` to `data/`.  A word
  becomes a symbol only when it is proven a pointer: a function entry
  outside the asset segments (graphics, samples, songs), or a word of a
  config `pointer_tables` entry whose layout a decompiled consumer proves
  (the task-type table, `gMPlayTable`, the room table
  `gUnk_087E1D58[9][8]`, its 57 room lists and the 333 `struct RoomDef`
  headers they reach, the BG animation script lists) or whose element
  type the C declares, with the span to the next label as its extent
  (48 seg 18 arrays, counted apart).  Metrics (`make datastats`), before
  -> after: ROM data symbols defined by absolute address 2,277 -> **0**
  (of 5,065: 2,788 new labels for pointer targets); symbolic pointer-like
  words 63 -> **8,385** (2,522 function entries, 4,272 in proven tables,
  1,591 in next-label tables); not symbolic, points at code 13,882 ->
  11,154, points at data 50,718 -> 45,124 (mostly assets and seg 18's
  untyped tables).  Findings: seg 13 ("sound_samples_1/2") is level data
  (BG animation scripts, RoomDefs, maps, doors), seg 20
  (`sample_set_index`) is the room table plus a sprite pointer table from
  `0x087E2570`; the names stay until phase 2.  The SRAM table's pointers to
  the two `static` cores of `src/agb_sram.c` stay extracted
  (`not_pointers`).  New lessons 4.116-4.122.
- Names, run 1 (issue #155): `tools/rename.py` renames a function or
  cell everywhere its name lives (`tools/symdb.py` `KNOWN_SYMBOLS` /
  `ARM_ENTRIES`, `tools/split_config.json`, `src/`, `include/`, the
  hand-written asm) and appends its evidence to
  `docs/analysis/renames.csv`, the alias table from the old names the
  lessons, rom-map and module-map keep; `--verify-diff master` proves a
  branch is a pure rename.  The convention (pret/katam style, `Task_<Thing>`
  task bodies, evidence tags, what stays unnamed) is `docs/naming.md`.
  1,145 renames in 14 batches (765 functions, 342 RAM cells, 38 ROM
  tables; four of them corrected earlier names, among them
  `TaskDispatchTrampoline` -> `TaskExitTrampoline` and `gTaskFlagsTable`
  -> `gTaskResumeAddrs`): 178 of the engine zone's 182 functions (the task
  engine, motion, sprites, fades, sound front end, the SDK MultiBoot
  library, pokeruby's link driver, katam's link-setup code), 286 of the
  424 functions with 5+ callers, the actor API, the player's shared checks
  and 23 player actions, the collision engine, camera, room and BG
  streaming, the HUD and menu, the save file, the sub-games and the
  game-over screen, 29 of the 266 task bodies.  No rename changed a byte:
  agbcc's Thumb backend forces every symbol address into the constant pool
  at expand time, so gcse only hashes `.LCn` names (lesson 3.515).
  `make progress`: 185 -> 1,326 of 11,065 symbols documented (1.7% ->
  12.0%).  Done by four proposal agents by address zone plus the
  coordinator, who alone applied names; new lessons 3.515, 4.123-4.124.
- Names, run 2 (issue #155): struct fields, the enemies and the
  abilities.  `tools/rename_field.py` renames a struct field
  compiler-guided: it renames the member in every definition of the
  struct (and, with `copies`, in the local copies at the same offset: the
  task engine's `struct Task` with `h10`/`b12` names, `src/task_draw_world.c`'s
  `struct Sprite`, the 17 `struct RoomDef` copies), lets gcc 12's
  `-fsyntax-only` in the knidl-builder image report each access that now
  fails (file, line, column and struct), renames exactly those, and
  requires the final error set to equal the original's; `rename.py
  --verify-diff` checks field renames token by token (kind `field`,
  `Struct.old` -> `Struct.new` in `renames.csv`).  242 fields of 30 structs
  named (296 rows with the local copies, 34,096 uses in 247 files):
  `include/task.h` went from 213 to 69 `unkXX` of its 222 fields (`Task`
  38 of 58, `PlayerState` 40 of 72, `Actor` 32 of 43, `ActorDef`,
  `ActorSpawn`, `GfxHeader`, `TaskGfx`, `AnimCmd` all or nearly all), plus
  `RoomDef`, the room object list, the terrain results, the scroll lock,
  the save slot and the Air Grind / boot-logo records.  No local copy was
  layout-identical to `include/task.h`, so no copy was replaced by the
  header.  The enemies were identified from LOCAL sprite renders
  (`pending/`, never committed: lesson 4.126) of the room objects'
  graphics descriptors `gUnk_0873EEA0[subtype]`, corroborated by each
  subtype's `ActorDef.ability`: 30 kind-0 enemies (Waddle Dee ... UFO, and
  the parasol), the 8 mid-bosses, Meta Knight's four knights, the 9 bosses
  (Whispy Woods ... Nightmare), the stage objects and the pickups, named
  `Task_<Enemy>` with their frame, variant and state tables, state
  machines (`<Enemy>[<Row>]Init/EnterState/Update`), state verbs and child
  objects (`docs/naming.md` 2.3).  The HUD ability pictures carry the
  ability names as text, so the ids 1-25 are fixed (FIRE ... STAR ROD) and
  the 29 ability moves are `PlayerAction<Ability>` / `...Update`.  M20 is
  enemies, not moving scenery (file headers, rom-map and module-map
  corrected).  806 symbol renames (469 functions, 320 ROM tables, 17 RAM
  cells) in 16 batches; 141 of the 266 task bodies and 1,356 of 5,344
  functions have a real name.  `make progress`: 1,326 -> 2,132 of 11,065
  symbols documented (11.98% -> 19.27%).  No rename changed a byte (field
  names never reach codegen, lesson 3.516).  Done by four proposal agents
  (fields; actor records; kind-0 enemies; bosses and abilities) in five or
  six rounds each plus the coordinator, who alone applied names; new
  lessons 3.516, 4.125-4.127.
- Names, run 3 (issue #155): the task bodies and their families, the
  unidentified enemies, two per-family views of `struct Task` and the data
  records by position.  **248 of the 266 task bodies** have a name (141
  before); the 18 left are visual-only or unsettled effects (#79, #88,
  #99, #127, #145, #193, Nightmare's #202/#204/#206/#208-#213/#220, the
  link-play palette tasks #249/#250).  A species identity now needs three
  agreeing sources (a LOCAL render, the code and `ActorDef.ability`, and
  WiKirby text cited by URL; docs/naming.md 2.3): the ten open kind-0
  subtypes are Coconut, Bubbles, Slippy, Sword Knight, Blade Knight, Poppy
  Bros. Jr. on an apple and on a Maxim Tomato, Coner, Gip and the block
  star, and run 2's Togezo is Needlous (Togezo is only in Kirby's Dream
  Land 3).  Named on the way: the boss and mid-boss children (Heavy Mole's
  arms and missiles, Paint Roller's paintings, Kracko Jr., the Nightmare
  forms' stars and hands, Meta Knight's sword, cape and mask), the
  kind-0 Idle rows and verbs, the carried bodies, the goal game, the
  Star Rod piece, the hub's Warp Star Station / Museum / Arena (stage
  requests 12-14), Meta Knightmare's action machine and Boss Endurance,
  the level-intro cutscenes, the ending scenes, the three sub-games'
  internals (Quick Draw's opponents Waddle Doo, Wheelie, Chef Kawasaki,
  King Dedede and Meta Knight; Bomb Rally's bomb and Bubbles seat; Air
  Grind's racer), the menus, the terrain engine's entry points and the
  Ball / Star Rod flight sub-moves.  **Views** (a type change, its own
  commit, approved by the owner's coordinator): `Task.u8C` is `union {
  struct Actor *actor; struct Task *parentTask; }` (247 `(struct Task *)`
  and 2 `(struct Actor *)` casts gone) and `Task.u80` a packed `union {
  s8 nearestPlayer; s8 attackAbility; }`; all 311 files' agbcc assembly is
  identical to the parent commit's, and `tools/header_smoke_game.c`
  checks the layout (agbcc pads every union to 4 bytes: lesson 3.522,
  docs/header-conventions.md).  **Position names** (docs/naming.md 2.4,
  approved as a family of about 1,700): a record whose only identity is
  its slot in a consumer-proven table is named after the slot, 0-based
  (`gLevel0Stage1Room2Doors`): the 57 room lists, 333 RoomDefs and 1,295
  single-referrer RoomDef records, the kind tables' ActorDefs
  (`g<Enemy>Def`, `gChildActorDefs`), the kind-0 graphics descriptors
  with the palettes and tiles 55 descriptors alone point at, and the 12
  BG animation sets; level indices 0-6 are the game's Levels 1-7 by their
  bosses.  3,440 `renames.csv` rows (1,977 position names, 1,103
  functions, 291 ROM tables and 56 RAM cells by role, 13 fields) in 43
  batches; `include/task.h` has 58 `unkXX` fields (69 before); 2,448 of
  5,347 functions are named (1,356 before).  `make progress`: 4,333 ->
  7,743 of 34,084 symbols documented (12.71% -> 22.72%), of which 1,976
  are position names; the semantic names alone give 5,767 (16.92%).  (The
  README's old 2,134 of 11,067 predates #36's 23,000 data labels.)
  `tools/rename.py` now also renames the symbols in linker.ld's MATCHING
  asserts (lesson 4.147).  No rename changed a byte.  Done by eight
  proposal agents (A-D, then E-H) in 2-6 rounds each plus the coordinator,
  who alone applied names; new lessons 3.522 and 4.147-4.150.
- Data structure, phase 2, run 1 (issue #36): **shared headers**,
  the first **functional tables as C** and the seg 13/20 re-partition.
  Eighteen subsystem headers (`include/main.h`, `link.h`, `sound.h`,
  `mode.h`, `hud.h`, `menu.h`, `cutscene.h`, `collision.h`, `room.h`,
  `camera.h`, `player.h`, `effect.h`, `actor.h`, `enemy.h`, `save.h`,
  `subgame.h`, `ending.h` and the task engine's part of `task.h`; the
  conventions are `docs/header-conventions.md`'s last section) declare
  each of the 3,383 RAM cells and ROM tables once with the type its
  consumers prove (every candidate type was tried in every file, only
  byte-identical assembly counted), carry the 55 shared struct
  definitions, and hold the prototypes of 5,128 C functions (each the
  definition's own line).  Local declarations in `src/`: 12,768 data
  `extern`s -> 991 (75 files) and 18,724 prototypes -> 2,400; symbols
  with conflicting types 201 -> 73.  What stays local is a genuine view:
  a file whose code needs another type for a symbol keeps its own
  declarations of that header (C allows one type per translation unit),
  and 99 functions whose call sites need another signature keep their
  per-file prototypes (lessons 3.428, 3.517, 3.519); both carry a
  one-line note.  An array of a struct must see the struct's definition
  (agbcc gives it byte alignment otherwise, lesson 3.518).
  `tools/carve_data.py` (the data twin of `carve.py`: `c_data` rows,
  `.rodata` pins, `fnmatch.sh --rodata` to verify) moved four tables to
  `src/data/`: the task-type table `gTaskTypes[266]`, the six actor
  definition tables by actor kind, the room table `gRoomTable[9][8]` and
  its 57 stage room lists (763 pointer words; the RoomDefs and ActorDefs
  they point at stay structure-only).  The re-survey found **no PCM in
  seg 13** (all 100 samples are in `m4a_songs_2`) and a level-data zone
  `0x08334EC0-0x083D0148` spanning six segments, now `room_bg_anims`,
  `room_data`, `room_metatiles` and `room_bg3_maps` (with
  `level_graphics_palettes` and `compressed_graphics` trimmed); seg 20
  (`sample_set_index`) is `room_table`, `room_bg_anim_lists`,
  `room_lists` and `sprite_frame_lists` (23 per-sheet frame lists with no
  code reference).  `tools/resegment.py` re-partitions data segments
  (structure only; `make datastats` unchanged by the move); the old ->
  new names are `docs/data.md` §4.1.  Every file was verified by its
  agbcc assembly (all 306 identical to master, lesson 4.128) and the ROM
  by `make compare`.  New lessons 3.517-3.519 and 4.128-4.131.
- Data structure, phase 2, run 2 (issue #36): **the ROM's sections
  follow each other, and shifting is measured**.  `linker.ld` pins only
  the cartridge header; every other block is placed by ld after the
  previous one and followed by `ASSERT(!MATCHING || ADDR(.x) == <addr>)`,
  with `--defsym MATCHING=1` from the Makefile unless `MATCHING=0` is
  given (`tools/ldblocks.py` writes the blocks for carve.py,
  carve_data.py and resegment.py); order and sizes alone reproduce the
  ROM.  `tools/shiftcheck.py` (`make shifttest`, also in CI) relinks
  with 0x1000 bytes inserted at seven section boundaries and counts the
  pointer-like words that did not move; `tools/ptrcensus.py` sorts them
  into proven pointers, proven coincidences and unknowns, from providers
  `tools/census_*.py`.  After crt0 (the whole ROM): **56,092 unrelocated
  words -> 16,997, of which 16,568 are proven coincidences, 429 unknown
  and 0 proven pointers**; relocated 16,866 -> 55,961 plus 1,343
  unaligned m4a track operands (docs/data.md §8, per zone and per
  insertion point).  What changed: the code's last 14 raw ROM pointers
  (crt0's AgbInit/AgbMain words, 10 C pool constants, the SRAM table's two
  cores, now non-static; each file's agbcc assembly identical but for the
  pool word, lesson 3.520); SoundMainRAM's ARM mixer kept raw
  (`raw_ranges`: a false Thumb branch to another object, 4.134); the m4a
  song structure as a verified parse (`tools/m4a_struct.py`, config
  `"m4a"`: 330 headers, 701 tracks, 129 WaveData, 3,894 pointer words,
  1,343 unaligned); the BG animation scripts, ActorDef/ActorAux/GfxHeader
  records, seg 18's behaviour tables (the six mixed and seven short-span
  tables of §5.1), the sprite frame network (329 frame tables, 5,087
  TaskGfx records, the 20-byte tagged player records that fill seg 19's
  head, split.py's `targets.tagged`) and, format-only because no code
  reads them, the frame lists, GfxHeader trailers and sheet headers
  (`"proof": "format"`, 3,014 words, docs/data.md §5.3).  `make datastats`: symbolic
  pointer-like words 8,385 -> 47,465 (763 of them in C; +1,343
  unaligned), not symbolic 56,278 -> 17,198; 25,859 ROM data labels.  Seg 19 holds no
  songs (the player frame records, the frame lists and four separately
  linked GBA programs); new lessons 3.520 and 4.132-4.139.  Done by four
  proposal agents in scratch copies of the tree, in three rounds, plus the
  coordinator, who alone applied config, linker and tool changes.
- Data structure, phase 2, run 3 (issue #36): **the ROM is proven
  movable and runs moved**.  The census after crt0: 16,961 unrelocated
  words = 0 proven pointers, 16,898 proven coincidences (317 of them on
  format evidence only), 63 unreachable and **0 unknown** (429 before);
  every insertion point, from AgbInit on, is proven safe, and `make
  shifttest --strict` keeps it so.  New providers `tools/census_sheets.py`
  (the sprite sheets: consumer-read LZ77 sources and sized palettes, four
  raw `.tiles` pointers now symbols, nine unreached TaskGfx records and
  chained OAM streams format-only) and `tools/census_bounds.py` (consumer
  extents: the completion pictures, seg 18's value tables, the credits
  demos replayed, the wave table bounded by the camera clamp; and the
  regions nothing reads), and a fourth census class, **unreachable**,
  whose condition (a) `ptrcensus.py` checks mechanically over every byte
  offset of the ROM (docs/data.md §8.3).  Five `ActorDef.unk10` words were
  real pointers counted as value fields (lesson 4.141), now symbols.
  **`make boottest`** (mGBA 0.10.5 in its own image, `tools/boottest/`):
  knidl.gba and seven shifted images agree frame for frame over 14,066
  scripted frames (boot, title, menus, a new game, stage 1-1 with doors,
  an ability, the pause screen and a lost life, the intro story).  Its
  first run found what the shift test cannot see: the three task
  trampolines' ARM `b` back to crt0 were raw bytes, and every ROM shifted
  inside the game code crashed at frame 88; they are `ARM_ENTRIES` now,
  and `tools/branchcheck.py` checks every relative branch across an
  insertion point (4.142).  Segments renamed by content
  (`tools/resegment.py`, docs/data.md §4.1): `sram_id_string`,
  `air_grind_rodata`, `sprite_sheets`/`sprite_sheets_2`,
  `m4a_voicegroups`, `m4a_song_data`, `engine_rodata`, `game_rodata`,
  `actor_rodata`, `frame_tables`, `late_game_rodata`, `credits_demos`,
  `player_frame_records`, `player_frame_lists` and the four program
  images.  The 179 ActorDef/ActorAux records are C
  (`src/data/actor_records.c`, one named input section per run, all in
  one output section via the new `tools/ldgroup.py`, docs/data.md §5.2).
  CI steps that pipe into `tee` got `pipefail`.  New lessons 3.521 and
  4.140-4.146.  Done by four proposal agents in scratch copies of the
  tree plus the coordinator.
- Next milestones: (1) #36 is closed on evidence; what remains is
  readability: the BG animation scripts, the RoomDef headers, the frame
  tables and seg 18's behaviour tables as C with run 3's grouped design
  (docs/data.md §5.2, §7), and a boot-test script that reaches further
  (sub-games, bosses, the credits); (2) #155, the rest of the names: the
  ~2,900 functions still `sub_*` (mostly enemy and boss state bodies and
  one-caller helpers, named only where a verb or contract is proven), the
  18 task bodies left, the ~200 `gUnk_` RAM cells (many proven to be
  shared scratch), the per-family `Task` fields `unk18`-`unk34`,
  `unk46`, `unk74`, `unk76` (views need the owner) and the asset labels;
  then #37's final audit.
  The three functions #154 left pinned or levered are listed in its
  bullet above.

## After #37

The bullets below follow the same form for the issues closed after #37
moved the record here; `AGENTS.md`'s `## Status` stays the current state.

- Data readability (issue #167): **every functional record family is
  typed C**, on top of a new data layout, and no census or shift-test
  number moved.  `tools/split.py` writes one data file per zone: a data
  segment's config entry may name its `"zone"`, and the pieces a zone is
  cut into around C runs share `data/<zone>.s`, one `.section` each (the
  35 `actor_rodata*` files of run 3 became one); `carve_data.py` gained
  `--runs`, `--c-file`, `--section` and `--record-in-asset`, and the
  `.tail` sections and odd-start raw path went (#170's item 4).  As C
  (`src/data/`, each run a named section, each zone one output section
  via `tools/ldgroup.py`): the 30 BG animation scripts and 14 palette
  fades (40 runs) and the script lists, the 343 frame tables (one
  section), the 333 RoomDef headers (333 runs; their maps, block layers,
  block tables, doors and object lists stay `.incbin` assets), and seg
  18's 507 handler tables and scripts plus 62 terrain-handler tables and
  136 hit-reaction records (`struct ActorHandlers`/`ActorVt`, moved from
  `src/actor_collision.c` to `include/actor.h`) in 176 runs.  589 `c_data`
  rows, 17,622 pointer words in C; `make datastats`: symbolic words in
  data files 46,156 -> 29,884; `make shifttest` 16,961 unrelocated after
  crt0, 0 proven pointers, 0 unknown, before and after; `make boottest`
  7/7.  Found on the way: the `.word MasterIsr` inside the credits
  particles' frame rows (`gUnk_087404A0`) was four frame bytes (a
  `not_pointers` entry now), `tools/calcrom.pl` counted the C records as
  code and split.py's segment labels as documented symbols, and
  `tools/audit.py`'s census skipped labels defined in C.  Fields named
  from their consumers (`Unk02007D70Cmd {op, arg, ptr}`, the fades'
  `{src, dst, colorIndex, colorCount, rate}`,
  `RoomDef.blockMetatiles`/`driftObjectIndex`) and the 333 block tables
  by their slot.  New lessons 4.156-4.159.  Done by four proposal agents
  in scratch copies of the tree, one of them in two rounds, plus the
  coordinator, who wrote the zone emitter and applied every change.
- Tooling follow-ups from #37's audit (issue #170): `tools/split.py`
  writes every ARM pool load whose literal lies in another segment as
  `ldr rN, [pc, #:pc_g0:(<segment> + <off> - 8)]`, crt0's form (the task
  switch helpers' ten loads from `task_literals` and the interworking
  veneer's one); a boot test with 0x100 bytes inserted before
  `task_literals` crashed at frame 88 with the old numeric offsets and
  ran all 14,066 frames with the relocations (lesson 4.155).
  `tools/fnmatch.sh` defaults to the Makefile's recipe
  (`-fprologue-bugfix`; `--newpb` kept, `--nopb` the old default); the
  baserom-reading targets check for `baserom.gba` first and point at
  INSTALL.md; a bare host `make` builds the ROM (`.DEFAULT_GOAL` was the
  first rule, `image`); `.gitignore`'s inline comment had left
  `report.json` unignored; the four `gUnk_04*` I/O symbols are
  `gRegVcount`, `gRegSound1CntL`, `gRegDma1Sad` and `gRegIme`
  (`REG_<NAME>` is io_reg.h's macro).  Item 4, split.py's unused `.tail`
  and odd-start paths, went with #167.
- Names, run 4 (issue #155): the enemy and boss state bodies, by verb and
  by slot.  #167's C tables gave every state body its family and slot, so
  six proposal agents (bosses; mid-bosses, the knights and King Dedede;
  kind-0 enemies; the actor core and stage objects; then RAM cells and
  fields) named the bodies a defined verb fits, with one verb list per
  family (new words in docs/naming.md 2.3: Hop as one definition, Land,
  BounceOffWall, Ascend/Descend, Swoop, Hover, Drift, Vanish..., the
  `<Family>ReactToDamage`/`ReactToDefeat` hooks of `struct ActorVt`, the
  terrain handlers of `struct ActorHandlers`, the held player's states
  after the carried actor's words).  **State-table slot names** (approved
  by the owner's coordinator as a family of about 800): a function whose
  only referrer is one slot of one named state table is named after it,
  `<Family>State<N>` / `State<N>Update` / `Variant<N>` (662 functions,
  their own commits, counted apart); an update is named by its index only
  where its state stores `updateState = N`, because Fire Lion, Gip,
  Javelin Knight, Bubbles and Mr. Tick-Tock index their update tables
  otherwise (lesson 4.160).  The collider lists RegisterCollider fills for
  the box classes 0x10 and 0x20 are named by their class value
  (`gColliderClass10`, `HitTestColliderClass20`...), because no role word
  fits every registrant; their reading is in `include/collision.h`.
  **Task bodies:** 263 of 266 (248 before): #79, #99, #127, #145, #193,
  #202/#204/#206, #208, #210-#213, #249/#250 got role names with code
  evidence or three-source identities; #88, #209 and #220 stay open.
  **Fields:** 40 (`ActorHandlers`' six terrain callbacks, `ActorVt`'s
  reaction kinds and hooks, `Collider`, `AttackBox`, `BodyBox`, five
  `PlayerState` fields), and one more view, `Task.u76` = packed `union {
  subtype; doorIndex; unk76; }` (all 319 files' agbcc assembly identical
  to the parent commit's; lesson 3.525: the oracle cannot see which
  member an access names, so the census was reviewed access by access).
  A census of the other per-family fields (`unk18`-`unk34`, `unk46`,
  `unk74`) found 46-125 task types each using them as their own
  registers: no plain name and no view worth having.  1,545
  `renames.csv` rows (1,351 functions - 689 by role, 662 by slot -, 105
  ROM tables, 49 RAM cells, 40 fields); functions named 2,449 -> 3,800 of
  5,348; `gUnk_` RAM cells 173 -> 124; `unk*` fields 349 -> 313 (header)
  and 362 -> 322 (local copies).  `make progress`: 8,015 -> 9,520 of
  34,017 symbols documented (23.56% -> 27.99%), of which 2,971 are
  position names (2,309 data records, 662 state-table slots).  No rename
  changed a byte; `make shifttest` unchanged (16,961 unrelocated, 0
  proven pointers, 0 unknown).  New lessons 3.525, 4.160 and 4.161.
- The boot test's reach (issue #168): **four scripts instead of one,
  each checking its own scenes**.  `tools/boottest/boottest.c` gained
  script checkpoints (`mark <scene>`, `expect <cell> <op> <value>` on the
  reference's RAM, named from the ELF's `nm` list; a failed expectation
  fails the run), `--peek` for tuning and `--coverage` (single-stepped
  reference, mapped onto `symbols.csv` by `tools/boottest/coverage.py`,
  `make boottest-coverage`); the Makefile runs every script of
  `BOOTTEST_INPUT`.  New scripts: `subgames.txt` (Quick Draw, Bomb Rally
  and Air Grind in single-player mode, Kirby winning each), `gameover.txt`
  (three lives lost in stage 1-1, the game-over screen, CONTINUE) and
  `level1.txt` (stages 1-1 and 1-2 cleared through their goal games,
  the Warp Star, the mid-boss Poppy Bros. Sr., stage 1-3's first three
  rooms); `input.txt` got
  checkpoints, and its comments, which put every menu screen one key
  early since #162, were corrected.  43,513 frames at all seven
  shift-test points with no difference; executed code 847 -> 1,687 of
  5,348 functions (31.5%); CI's boot-test step about 60 -> 180 s.
  Out of reach, documented: the level 1 boss (behind stage 1-4, past
  #168's time-box), the
  ending and credits (the whole game, some 200,000 frames) and link play
  (linked cores).  New lessons 4.162-4.164.  Done by three proposal
  agents (the sub-games, the game over and the credits study, level 1)
  and the coordinator, who wrote the harness features.
- The two functions that kept zero-byte `asm("")` levers (issue #169):
  **`PoppyBrosSrHeadUpdate` is plain C; `BootLogoUpdateObjects` keeps
  its two levers, with the fold's cause measured**.  The Poppy Bros.
  Sr. head's animation walker (formerly `sub_08091e18`, 9 bytes after
  the strip test) now stops its timer explicitly at the end marker,
  `if (frame == -1) w->unk28 = 0; else { ... }`: the store is
  redundant on that path, so cse stores the decremented register
  (keeping it live into the frame test, the ROM's `subs r4` and `ldrsh`
  scratches 4, 5, 5, 4) and post-reload cse deletes it; written with
  the other arm first, the store follows a label and survives (+4
  bytes).  The owner's coordinator accepted it as plain C (meaningful,
  not dead), so its docs/audit.md section 3 row went.  For the boot
  logo's objects (40 bytes plain, 564 of 568) the instrumented compiler
  showed that combine folds the switch-off's `orrs` because gcse's
  reaching register for the old id has two sets; a probe with one set
  gives the ROM's code, a compiler without combine's first-scan
  recording changes five other files, and 13 new spellings (a
  bit-field id, id locals, range tests, a read-before-store loop) and
  13 compiler-flag variants fold or change the function; the owner's
  coordinator accepted that residue as final, so its two levers are a
  sanctioned exception (closes #169).  New lessons
  3.526, 3.527 and 4.165 (3.156 corrected, 3.494 amended).  Done by two
  proposal agents, the first resumed as the racer on the second
  function.
- The decomp.dev report (#164): `tools/gen_report.py` counts the
  asm-by-design zones (crt0, the ARM task switcher, the `m4a_1` core, SDK
  libc and the task trampolines, the SWI thunks, SoftReset, the
  interworking veneer) as matched/complete `[asm]` units, since their
  checked-in asm is their source and `make compare` verifies it; the
  module loop also treats module M265 (exactly the `m4a_1` core) as one of
  them.  The badge moves from 99.51% to 100% ("every code byte built from
  repository source"); `make progress` keeps the zones excluded.
- Asset extraction (#165): `tools/extract_assets.py` (`make assets`,
  `make assets-check`, docs/assets.md) decodes the census-proven graphics
  objects from `baserom.gba` into the gitignored `assets/` directory, the
  data policy's editable view (the tmc/mzm model), reusing the pointer
  census providers so every boundary, format and consumer citation is the
  census-proven one: 15,620 artifacts (palettes as JASC, 4bpp tiles and
  TaskGfx chunk streams, decoded LZ77 blobs, room and picture maps, OAM
  templates as JSON, 19 rendered pictures) plus a manifest.  `make
  assets-check` re-extracts into a temporary directory and compares byte for
  byte; CI runs it after `make compare`.  Rebased after #167-#176 by the
  coordinator: the extractor ran unchanged on the zone files and C records.
- Asset re-injection (#166): `tools/rebuild_assets.py` (`make assets-mod`,
  `assets-mod-check`, `assets-selftest`, docs/assets.md), the extractor's
  inverse, rebuilds the gitignored `knidl-mod.gba` from `baserom.gba` and
  the edited `assets/` tree: it re-derives every record's pristine original
  through `extract_assets`, leaves unchanged objects untouched (an unedited
  tree rebuilds `baserom.gba` byte for byte), re-encodes edited ones
  (JASC palettes to BGR555, tiles, TaskGfx chunk streams, a BIOS LZ77
  writer whose streams are decoded back before splicing, maps with their
  BG3 headers, OAM JSON) and splices each over its slot, fit or fail.
  `assets-selftest` re-encodes the whole corpus (14,386 objects byte-exact,
  1,091 LZ77 streams decoded back).  Rebased after #165 by the coordinator.
- Names, run 5 (issue #155): the family sweep and **per-family register
  aliases**.  The owner's decisions D1-D5, applied: (D1) `struct Task`'s
  registers `unk18`-`unk34`, `unk46`, `unk6C`-`unk70` and `unk74` (the
  last three added with the coordinator's approval) are named the way
  pret names `struct Task`'s `data[16]`: object-like alias macros in the
  new `include/task_vars.h`, one block per family, each line giving the
  member, the type the code reads it as and the role.  `tools/task_alias.py`
  applies (file, function, pointer, field, alias) site rows inside the
  named bodies and proves them twice: every translation unit's `cpp -P`
  output equals the parent's, and gcc 12 accepts the tree with each
  aliased member an anonymous union of the member and its aliases (so no
  alias sits on another struct); `rename.py --verify-diff` accepts the
  alias rows and their merge chains.  A role a shared helper's contract
  fixes is one shared alias without a family prefix (`actorAnimDelay`,
  `actorSpawnArg`, `actorDustTrailSlot`, `doorObjectKind`, the actor core's
  drown, defeat, freeze, carry, throw and swallow registers); the rest are
  `<family><Role>` with one word set across siblings (`<family>LoopCount`
  for the running state's loop counter).  965 aliases in 198 families
  cover 8,117 of the 10,456 register accesses.  (D2) the cells inside the palette shadow buffers
  are named by position (`gObjPaletteBank6Color6`, 24 cells).  (D3) the
  shared-scratch pattern is documented; no cell proved to be scratch.
  (D4) functional ROM records by position: the ActorDef field records
  (231), the player's frame table `gPlayerFrames` and its 2,590 records,
  58 records of named data tables, and 55 frame lists that only a
  descriptor's trailer reaches (format-only, counted apart); and, approved
  by the coordinator, slot names for the named dispatch tables
  (`CutsceneBeachActorScript10`, `WarpStarFlight5`, `PlayerDance3`,
  `ActorDefeat2`: 104 functions).  (D5) the engine API's positional
  parameters (`TaskSetMotionY(velY, accelY, speedLimitY)`, 18 functions,
  27 parameters; every unit's agbcc assembly identical).  Four proposal
  agents in two waves (mid-bosses, bosses, two enemy banks; then the
  player, the actor core and stage, the cutscenes and ending, the menus
  and sub-games, and RAM cells and fields) proposed names and aliases per
  family; the coordinator expanded the alias proposals from a gcc
  census of every `struct Task` member access (lesson 4.167).  Task bodies
  #209 and #220 got effect names (`Task_NightmareWizardDefeatFlash`,
  `Task_NightmarePowerOrbStreak`); #88 stays open.  Figures: functions
  named 3,800 -> 4,366 of 5,348; task bodies 263 -> 265 of 266; `make
  progress` 9,524 -> 13,102 of 34,017 symbols documented (27.99% ->
  38.52%), about 6,950 semantic and 6,156 position names; `gUnk_` RAM cells
  124 -> 84; functional ROM labels 5,747 -> 2,775; `unk*` fields 313 ->
  257 (header) and 322 -> 282 (local copies), the 14 register members now
  named per family.  No rename or alias changed a byte; `make shifttest`
  unchanged (16,961 unrelocated, 0 proven pointers, 0 unknown).  New
  lessons 4.166-4.170.
- Names, run 6 (issue #155): **named constants**, struct tags, locals and
  the long tail.  The owner's decisions D6-D8, applied: (D6) the magic
  numbers whose meaning the names prove are pret-style object-like
  `#define`s in 11 new headers `include/constants/*.h`, included by the
  subsystem headers that define their fields: task types (`TASK_<BODY>`,
  266), abilities (27, from the HUD banners), the player's and Meta
  Knight's actions and handlers (162), every state table's slots
  (`<FAMILY>_STATE_<VERB>`, 677, named after the slot's verb or by index)
  and every variant table's role-named slots (236), game states and stage
  requests, hit kinds and effects, actor kinds, camera modes, room entry
  modes and door kinds, and the sound and song ids every call site agrees
  on (1,473 constants, 3,980 literal sites, each constant logged with the
  consumer that proves it in the new `docs/analysis/constants.csv`).
  `tools/constants.py` writes the headers, respells the literals by a
  position rule per family (`--scan`: 0 literals left at those positions)
  or by site lists, and proves each family's commit with `--verify-cpp`
  (every unit's `cpp -P` tokens equal the parent's, integer literals
  compared by value), backed by the per-file assembly oracle; the state
  constants are placed only inside functions proven to run that table's
  machine.  (D7) 33 struct tags named after their proven cell or consumer
  (`Unk02007D70` -> `BgAnim`, the Air Grind records, `LinkSave` ->
  `InputRecording`), `tools/rename.py` kind `tag`.  (D8) 147 locals in
  functions named by role (child task slots and pointers after their
  TASK_ constant, player and level/stage loops), `tools/rename.py
  --locals`.  `tools/rename.py --verify-diff` now accepts constants,
  tags, locals and run 5's parameters, so the whole stacked branch
  verifies from `origin/master` in one pass.  Six proposal agents (four in
  wave 1 by subject, two in wave 2 on what wave 1 left, and one on the
  player's registers) proposed names with renders, code and WiKirby text:
  the bosses' and mid-bosses' states (new verbs Intro, DropIn, Follow,
  PickMove, FigureEight, FlyAway, Twist, Pounce, Dive ..., new row words
  Weave, Ambush, Leap, Asleep), the life-request screen in full, the
  ability effects (the second palette bank comes from the player's own
  frame records), the fountain cutscene and the ending's actors, the
  player's palette, blink and goal-game helpers, the Warp Star ride's
  tumble, the door signs; 126 fields (`PlayerState`, the input recording,
  the link records, Air Grind, the HUD bars, `AttackBox`, `BodyBox`); 86
  register aliases.  Figures: functions named 4,366 -> 4,659 of 5,348
  (4,034 by role, 625 by slot); task bodies 265 of 266 (#88 still draws a
  shape no source names); `make progress` 13,102 -> 13,443 of 34,017
  symbols documented (38.52% -> 39.52%); `gUnk_` RAM cells 84 -> 73;
  functional ROM labels 2,775 -> 2,739; `unk*` fields 257 -> 135 (header)
  and 282 -> 233 (local copies); register aliases 965 -> 1,051 in 198
  families (about 8,100 -> 9,000 of the 10,467 register accesses).  No
  change moved a byte: `make compare` after every batch, `make shifttest`
  unchanged (16,961 unrelocated, 0 proven pointers, 0 unknown).  New
  lessons 3.528-3.529 and 4.171-4.172.
- Names, run 7 (issue #155, closing it): **paired states, flag bits and
  a census with a reason per symbol**.  The owner's decisions R1-R3,
  applied: (R1) a verb that fits two states names both, each with the
  qualifier the code proves - a direction, speed, phase or entering
  condition, or a sequence number when the transitions prove the order
  (`GipClimbUp` / `GipClimbDown` / `GipClimbUpFromFloor` /
  `GipClimbDownFromLedge`, `MetaKnightJumpHigh` / `JumpLow`,
  `MrTickTockDashWindUp` / `DashStart` / `DashLoop`,
  `PoppyBrosSrHop1`-`Hop4`); (R2) a row no row word covers is named after
  what sets it apart from its siblings (`TwizzyHopToChase`,
  `SquishyJumpToWalk`, the Meta Knights' `FallIn` / `WalkInShort` rows),
  else it keeps its position name; (R3) flag words get a field name
  (`AttackBox.immunityFlags` / `attackFlags`, `BodyBox.bodyFlags`,
  `PlayerState.actionFlags` / `statusFlags`) and one constant per proven
  bit (38 bits: `ATTACK_BOX_*`, `BODY_BOX_*`, `PLAYER_ACTION_FLAG_*`,
  `PLAYER_STATUS_*`, `PLAYER_HIT_*`, `SPRITE_FLAG_FLIP_X`, `TASK_SKIP_*`;
  `tools/constants.py`'s `bits` positions), and, approved as a second
  form, clears and masks are spelled `&= ~FLAG` and `(A | B)` (262 sites,
  proven by the per-file assembly oracle; no site changed code).  Four
  proposal agents read the code run 6 had left: the actor core, the
  terrain probes, the cannon's launch kinds, the Warp Star, the link
  driver's `struct Link` (pokeruby's names) and the link-setup records,
  the ending and game-over variants, task body #88, every boss's and
  enemy's paired states, the ROM records several functions read, the RAM
  cells and every remaining field.  ROM records whose every use is one
  engine API's argument are named by kind and consumer
  (`gHeavyMoleYellowMissileFrames`, `gBubblesLandAttackBox`), and records
  one slot of a named record holds get position names.  `tools/task_alias.py`
  serves `PlayerState`'s per-action scratch too (`playerJumpPhaseTimer`).
  The census: `docs/analysis/unnamed.csv` gives every `sub_*`, `gUnk_` RAM
  cell and `unk*` field left its reason code (`pair`, `identity`,
  `unnamed-input`, `no-verb`, `restates`, `dead`, `two-meanings`,
  `never-accessed` ...), the functional ROM labels are classed by their
  referrers, and `make audit` fails on a placeholder without a reason.
  Figures (run 6 -> run 7): functions named 4,659 -> 4,937 of 5,348 (by
  role 4,034 -> 4,597, by slot 625 -> 340); `make progress` 13,443 ->
  14,208 of 34,017 symbols documented (39.52% -> 41.77%); task bodies 265
  of 266 (#88: identity); `gUnk_` RAM cells 73 -> 62; functional ROM labels
  2,739 -> 2,263; `unk*` fields 135 -> 92 (header) and 233 -> 141 (local
  copies); register aliases 1,051 -> 1,186 in 205 families (about 9,000 ->
  10,018 of the 10,455 register accesses); constants 1,473 -> 1,571 at
  3,980 -> 4,637 sites; struct tags 33 -> 36.  No change moved a byte:
  `make compare` after every batch, `make shifttest` unchanged.  New
  lessons 3.530 and 4.173-4.176.
