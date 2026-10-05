/* Kirby: Nightmare in Dream Land (USA) - ROM layout.
 *
 * The sections follow each other without pinned addresses (issue #36,
 * docs/data.md section 8): ld places each one right after the previous
 * one, aligned to its own input sections, so the order and the sizes alone
 * give the layout.  Only the cartridge header keeps a fixed address (the
 * hardware reads it at 0x08000000, and the branch at its first word jumps
 * to the 0xC0 bytes after it).
 *
 * Every other block is followed by an ASSERT of the address the original
 * ROM has.  The Makefile links with --defsym MATCHING=1 by default (what
 * `make compare` and CI use), so a matching build still fails loudly at
 * the first section that moved; `make MATCHING=0` drops the assertions for
 * a modified ROM.  tools/shiftcheck.py (make shifttest) links with
 * MATCHING=0 and padding inserted to measure which pointers still do not
 * move with their targets.  New blocks are written by tools/ldblocks.py
 * (carve.py, carve_data.py, resegment.py).
 */

MEMORY
{
    /* 32 MiB, the GBA maximum; the matching image ends at 0x08800000
     * (the assertion at the end). */
    ROM : ORIGIN = 0x08000000, LENGTH = 32M
}

SECTIONS
{
    /* GBA cartridge header (0x08000000-0x080000BF), built from source in
     * asm/rom_header.s. The Nintendo logo is copyrighted and is extracted
     * from baserom.gba via .incbin instead of being committed. */
    .rom_header 0x08000000 : {
        KEEP(*(.rom_header))
    } > ROM

    /* ===== Segments split from the former main_blob =====
     * One section per docs/analysis/segments.txt row, in address order;
     * the assertion after each block is its segments.txt start.  A group
     * (tools/ldgroup.py) lists several rows, the data pieces of one
     * data/<zone>.s and the C runs between them, in one block. */

    /* crt0 + master ISR (ARM code) — assembled from asm/crt0.s */
    .crt0_master_isr : {
        KEEP(*(.crt0_master_isr))
    } > ROM
    ASSERT(!MATCHING || ADDR(.crt0_master_isr) == 0x080000C0, "MATCHING: .crt0_master_isr moved")

    /* crt0/ISR literal pool — assembled from asm/crt0.s */
    .crt0_literals : {
        KEEP(*(.crt0_literals))
    } > ROM
    ASSERT(!MATCHING || ADDR(.crt0_literals) == 0x08000210, "MATCHING: .crt0_literals moved")

    /* Task/context-switch helpers (ARM code) */
    .task_switch_helpers : {
        KEEP(*(.task_switch_helpers))
    } > ROM
    ASSERT(!MATCHING || ADDR(.task_switch_helpers) == 0x08000234, "MATCHING: .task_switch_helpers moved")

    /* Task helper literal pool */
    .task_literals : {
        KEEP(*(.task_literals))
    } > ROM
    ASSERT(!MATCHING || ADDR(.task_literals) == 0x080002E8, "MATCHING: .task_literals moved")

    /* AgbInit (Thumb) — decompiled to C (src/agb_init.c, issue #28).
     * The section spans 0x08000310-0x080008E7: the function body, the
     * compiler's pool-skip branch at 0x0700 and its literal pool. */
    .agb_init : {
        build/src/agb_init.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.agb_init) == 0x08000310, "MATCHING: .agb_init moved")

    /* Game code: early subsystems (Thumb) */
    /* main_fade — decompiled to C (src/main_fade.c, carved by tools/carve.py) */
    .main_fade : {
        build/src/main_fade.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_fade) == 0x080008E8, "MATCHING: .main_fade moved")

    /* main_end_frame — decompiled to C (src/main_end_frame.c, carved by tools/carve.py) */
    .main_end_frame : {
        build/src/main_end_frame.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_end_frame) == 0x08000DE4, "MATCHING: .main_end_frame moved")

    /* main_vblank_intr — decompiled to C (src/main_vblank_intr.c, carved by tools/carve.py) */
    .main_vblank_intr : {
        build/src/main_vblank_intr.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_vblank_intr) == 0x080010CC, "MATCHING: .main_vblank_intr moved")

    /* main_frame_io — decompiled to C (src/main_frame_io.c, carved by tools/carve.py) */
    .main_frame_io : {
        build/src/main_frame_io.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_frame_io) == 0x080011AC, "MATCHING: .main_frame_io moved")

    /* main_copy_queue — decompiled to C (src/main_copy_queue.c, carved by tools/carve.py) */
    .main_copy_queue : {
        build/src/main_copy_queue.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_copy_queue) == 0x08001518, "MATCHING: .main_copy_queue moved")

    /* main_build_oam — decompiled to C (src/main_build_oam.c, carved by tools/carve.py) */
    .main_build_oam : {
        build/src/main_build_oam.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_build_oam) == 0x08001B08, "MATCHING: .main_build_oam moved")

    /* main_affine_sprite — decompiled to C (src/main_affine_sprite.c, carved by tools/carve.py) */
    .main_affine_sprite : {
        build/src/main_affine_sprite.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_affine_sprite) == 0x08001CC8, "MATCHING: .main_affine_sprite moved")

    /* main_fade_requests — decompiled to C (src/main_fade_requests.c, carved by tools/carve.py) */
    .main_fade_requests : {
        build/src/main_fade_requests.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_fade_requests) == 0x08001FD0, "MATCHING: .main_fade_requests moved")

    /* link_sync_random — decompiled to C (src/link_sync_random.c, carved by tools/carve.py) */
    .link_sync_random : {
        build/src/link_sync_random.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_sync_random) == 0x08002378, "MATCHING: .link_sync_random moved")

    /* link_sync_clock — decompiled to C (src/link_sync_clock.c, carved by tools/carve.py) */
    .link_sync_clock : {
        build/src/link_sync_clock.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_sync_clock) == 0x08002668, "MATCHING: .link_sync_clock moved")

    /* link_disconnect — decompiled to C (src/link_disconnect.c, carved by tools/carve.py) */
    .link_disconnect : {
        build/src/link_disconnect.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_disconnect) == 0x0800293C, "MATCHING: .link_disconnect moved")

    /* link_run_frames — decompiled to C (src/link_run_frames.c, carved by tools/carve.py) */
    .link_run_frames : {
        build/src/link_run_frames.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_run_frames) == 0x08002B04, "MATCHING: .link_run_frames moved")

    /* sound_bgm — decompiled to C (src/sound_bgm.c, carved by tools/carve.py) */
    .sound_bgm : {
        build/src/sound_bgm.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.sound_bgm) == 0x08003110, "MATCHING: .sound_bgm moved")

    /* sound_play_sfx — decompiled to C (src/sound_play_sfx.c, carved by tools/carve.py) */
    .sound_play_sfx : {
        build/src/sound_play_sfx.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.sound_play_sfx) == 0x080031B8, "MATCHING: .sound_play_sfx moved")

    /* sound_control — decompiled to C (src/sound_control.c, carved by tools/carve.py) */
    .sound_control : {
        build/src/sound_control.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.sound_control) == 0x08003484, "MATCHING: .sound_control moved")

    /* link_setup_init — decompiled to C (src/link_setup_init.c, carved by tools/carve.py) */
    .link_setup_init : {
        build/src/link_setup_init.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_setup_init) == 0x08003888, "MATCHING: .link_setup_init moved")

    /* link_setup — decompiled to C (src/link_setup.c, carved by tools/carve.py) */
    .link_setup : {
        build/src/link_setup.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_setup) == 0x08003964, "MATCHING: .link_setup moved")

    /* link_setup_intr_block — decompiled to C (src/link_setup_intr_block.c, carved by tools/carve.py) */
    .link_setup_intr_block : {
        build/src/link_setup_intr_block.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_setup_intr_block) == 0x08004000, "MATCHING: .link_setup_intr_block moved")

    /* link_block_main — decompiled to C (src/link_block_main.c, carved by tools/carve.py) */
    .link_block_main : {
        build/src/link_block_main.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_block_main) == 0x08004734, "MATCHING: .link_block_main moved")

    /* link_multiboot_main — decompiled to C (src/link_multiboot_main.c, carved by tools/carve.py) */
    .link_multiboot_main : {
        build/src/link_multiboot_main.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_multiboot_main) == 0x08004984, "MATCHING: .link_multiboot_main moved")

    /* link_multiboot — decompiled to C (src/link_multiboot.c, carved by tools/carve.py) */
    .link_multiboot : {
        build/src/link_multiboot.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_multiboot) == 0x08004D6C, "MATCHING: .link_multiboot moved")

    /* task_init — decompiled to C (src/task_init.c, carved by tools/carve.py) */
    .task_init : {
        build/src/task_init.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.task_init) == 0x08004FEC, "MATCHING: .task_init moved")

    /* task_run — decompiled to C (src/task_run.c, carved by tools/carve.py) */
    .task_run : {
        build/src/task_run.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.task_run) == 0x08005228, "MATCHING: .task_run moved")

    /* task_skip_mask — decompiled to C (src/task_skip_mask.c, carved by tools/carve.py) */
    .task_skip_mask : {
        build/src/task_skip_mask.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.task_skip_mask) == 0x080055B0, "MATCHING: .task_skip_mask moved")

    /* task — decompiled to C (src/task.c, carved by tools/carve.py) */
    .task : {
        build/src/task.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.task) == 0x08005654, "MATCHING: .task moved")

    /* task_move — decompiled to C (src/task_move.c, carved by tools/carve.py) */
    .task_move : {
        build/src/task_move.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.task_move) == 0x080058E4, "MATCHING: .task_move moved")

    /* task_frame_tiles — decompiled to C (src/task_frame_tiles.c, carved by tools/carve.py) */
    .task_frame_tiles : {
        build/src/task_frame_tiles.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.task_frame_tiles) == 0x08005ACC, "MATCHING: .task_frame_tiles moved")

    /* task_draw_screen — decompiled to C (src/task_draw_screen.c, carved by tools/carve.py) */
    .task_draw_screen : {
        build/src/task_draw_screen.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.task_draw_screen) == 0x08005C4C, "MATCHING: .task_draw_screen moved")

    /* task_draw_world — decompiled to C (src/task_draw_world.c, carved by tools/carve.py) */
    .task_draw_world : {
        build/src/task_draw_world.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.task_draw_world) == 0x08005D9C, "MATCHING: .task_draw_world moved")

    /* link_driver — decompiled to C (src/link_driver.c, carved by tools/carve.py) */
    .link_driver : {
        build/src/link_driver.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_driver) == 0x08006464, "MATCHING: .link_driver moved")

    /* link_cmd_queue — decompiled to C (src/link_cmd_queue.c, carved by tools/carve.py) */
    .link_cmd_queue : {
        build/src/link_cmd_queue.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_cmd_queue) == 0x08006AC8, "MATCHING: .link_cmd_queue moved")

    /* link_vsync — decompiled to C (src/link_vsync.c, carved by tools/carve.py) */
    .link_vsync : {
        build/src/link_vsync.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_vsync) == 0x08006CD4, "MATCHING: .link_vsync moved")

    /* link_timer3_intr — decompiled to C (src/link_timer3_intr.c, carved by tools/carve.py) */
    .link_timer3_intr : {
        build/src/link_timer3_intr.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_timer3_intr) == 0x08006D18, "MATCHING: .link_timer3_intr moved")

    /* link_serial_cb — decompiled to C (src/link_serial_cb.c, carved by tools/carve.py) */
    .link_serial_cb : {
        build/src/link_serial_cb.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_serial_cb) == 0x08006D28, "MATCHING: .link_serial_cb moved")

    /* link_start_transfer — decompiled to C (src/link_start_transfer.c, carved by tools/carve.py) */
    .link_start_transfer : {
        build/src/link_start_transfer.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_start_transfer) == 0x08006E8C, "MATCHING: .link_start_transfer moved")

    /* link_do_recv — decompiled to C (src/link_do_recv.c, carved by tools/carve.py) */
    .link_do_recv : {
        build/src/link_do_recv.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_do_recv) == 0x08006E9C, "MATCHING: .link_do_recv moved")

    /* link_send_connect — decompiled to C (src/link_send_connect.c, carved by tools/carve.py) */
    .link_send_connect : {
        build/src/link_send_connect.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.link_send_connect) == 0x08007004, "MATCHING: .link_send_connect moved")

    /* Game code + rodata (Thumb) — bulk of the game logic */
    /* main — decompiled to C (src/main.c, carved by tools/carve.py) */
    .main : {
        build/src/main.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main) == 0x08007300, "MATCHING: .main moved")

    /* mode_hub_stage — decompiled to C (src/mode_hub_stage.c, carved by tools/carve.py) */
    .mode_hub_stage : {
        build/src/mode_hub_stage.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.mode_hub_stage) == 0x080075B8, "MATCHING: .mode_hub_stage moved")

    /* mode_extra_mode_title — decompiled to C (src/mode_extra_mode_title.c, carved by tools/carve.py) */
    .mode_extra_mode_title : {
        build/src/mode_extra_mode_title.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.mode_extra_mode_title) == 0x08007B68, "MATCHING: .mode_extra_mode_title moved")

    /* mode_extra_mode_title_sprites — decompiled to C (src/mode_extra_mode_title_sprites.c, carved by tools/carve.py) */
    .mode_extra_mode_title_sprites : {
        build/src/mode_extra_mode_title_sprites.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.mode_extra_mode_title_sprites) == 0x080082D0, "MATCHING: .mode_extra_mode_title_sprites moved")

    /* mode_pause_boss_endurance — decompiled to C (src/mode_pause_boss_endurance.c, carved by tools/carve.py) */
    .mode_pause_boss_endurance : {
        build/src/mode_pause_boss_endurance.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.mode_pause_boss_endurance) == 0x08008664, "MATCHING: .mode_pause_boss_endurance moved")

    /* mode_gfx_loaders — decompiled to C (src/mode_gfx_loaders.c, carved by tools/carve.py) */
    .mode_gfx_loaders : {
        build/src/mode_gfx_loaders.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.mode_gfx_loaders) == 0x08008B8C, "MATCHING: .mode_gfx_loaders moved")

    /* mode_boot_sequence — decompiled to C (src/mode_boot_sequence.c, carved by tools/carve.py) */
    .mode_boot_sequence : {
        build/src/mode_boot_sequence.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.mode_boot_sequence) == 0x080091AC, "MATCHING: .mode_boot_sequence moved")

    /* hud_init_counters — decompiled to C (src/hud_init_counters.c, carved by tools/carve.py) */
    .hud_init_counters : {
        build/src/hud_init_counters.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hud_init_counters) == 0x080099FC, "MATCHING: .hud_init_counters moved")

    /* hud_hp_bars — decompiled to C (src/hud_hp_bars.c, carved by tools/carve.py) */
    .hud_hp_bars : {
        build/src/hud_hp_bars.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hud_hp_bars) == 0x0800A130, "MATCHING: .hud_hp_bars moved")

    /* hud_draw — decompiled to C (src/hud_draw.c, carved by tools/carve.py) */
    .hud_draw : {
        build/src/hud_draw.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hud_draw) == 0x0800AAD0, "MATCHING: .hud_draw moved")

    /* hud_tilemap — decompiled to C (src/hud_tilemap.c, carved by tools/carve.py) */
    .hud_tilemap : {
        build/src/hud_tilemap.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hud_tilemap) == 0x0800B318, "MATCHING: .hud_tilemap moved")

    /* mode_hub_stage_init — decompiled to C (src/mode_hub_stage_init.c, carved by tools/carve.py) */
    .mode_hub_stage_init : {
        build/src/mode_hub_stage_init.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.mode_hub_stage_init) == 0x0800B44C, "MATCHING: .mode_hub_stage_init moved")

    /* menu_main_save_slots — decompiled to C (src/menu_main_save_slots.c, carved by tools/carve.py) */
    .menu_main_save_slots : {
        build/src/menu_main_save_slots.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menu_main_save_slots) == 0x0800B920, "MATCHING: .menu_main_save_slots moved")

    /* menu_file_select — decompiled to C (src/menu_file_select.c, carved by tools/carve.py) */
    .menu_file_select : {
        build/src/menu_file_select.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menu_file_select) == 0x0800C09C, "MATCHING: .menu_file_select moved")

    /* menu_mode_list — decompiled to C (src/menu_mode_list.c, carved by tools/carve.py) */
    .menu_mode_list : {
        build/src/menu_mode_list.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menu_mode_list) == 0x0800CA10, "MATCHING: .menu_mode_list moved")

    /* menu_sound_test_link_play — decompiled to C (src/menu_sound_test_link_play.c, carved by tools/carve.py) */
    .menu_sound_test_link_play : {
        build/src/menu_sound_test_link_play.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menu_sound_test_link_play) == 0x0800D450, "MATCHING: .menu_sound_test_link_play moved")

    /* menu_file_select_tasks — decompiled to C (src/menu_file_select_tasks.c, carved by tools/carve.py) */
    .menu_file_select_tasks : {
        build/src/menu_file_select_tasks.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menu_file_select_tasks) == 0x0800DAF8, "MATCHING: .menu_file_select_tasks moved")

    /* menu_panel_tasks — decompiled to C (src/menu_panel_tasks.c, carved by tools/carve.py) */
    .menu_panel_tasks : {
        build/src/menu_panel_tasks.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menu_panel_tasks) == 0x0800E314, "MATCHING: .menu_panel_tasks moved")

    /* menu_sound_test_tasks — decompiled to C (src/menu_sound_test_tasks.c, carved by tools/carve.py) */
    .menu_sound_test_tasks : {
        build/src/menu_sound_test_tasks.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menu_sound_test_tasks) == 0x0800EA0C, "MATCHING: .menu_sound_test_tasks moved")

    /* menu_link_play_tasks — decompiled to C (src/menu_link_play_tasks.c, carved by tools/carve.py) */
    .menu_link_play_tasks : {
        build/src/menu_link_play_tasks.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menu_link_play_tasks) == 0x0800F180, "MATCHING: .menu_link_play_tasks moved")

    /* menu_bg_scroll — decompiled to C (src/menu_bg_scroll.c, carved by tools/carve.py) */
    .menu_bg_scroll : {
        build/src/menu_bg_scroll.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menu_bg_scroll) == 0x0800FCBC, "MATCHING: .menu_bg_scroll moved")

    /* cutscene_main — decompiled to C (src/cutscene_main.c, carved by tools/carve.py) */
    .cutscene_main : {
        build/src/cutscene_main.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_main) == 0x080100AC, "MATCHING: .cutscene_main moved")

    /* cutscene_director_duel — decompiled to C (src/cutscene_director_duel.c, carved by tools/carve.py) */
    .cutscene_director_duel : {
        build/src/cutscene_director_duel.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_director_duel) == 0x08010358, "MATCHING: .cutscene_director_duel moved")

    /* cutscene_actor_particles — decompiled to C (src/cutscene_actor_particles.c, carved by tools/carve.py) */
    .cutscene_actor_particles : {
        build/src/cutscene_actor_particles.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_actor_particles) == 0x080109C8, "MATCHING: .cutscene_actor_particles moved")

    /* cutscene_scenes — decompiled to C (src/cutscene_scenes.c, carved by tools/carve.py) */
    .cutscene_scenes : {
        build/src/cutscene_scenes.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_scenes) == 0x08010B38, "MATCHING: .cutscene_scenes moved")

    /* cutscene_fountain_kirby_king_dedede — decompiled to C (src/cutscene_fountain_kirby_king_dedede.c, carved by tools/carve.py) */
    .cutscene_fountain_kirby_king_dedede : {
        build/src/cutscene_fountain_kirby_king_dedede.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_fountain_kirby_king_dedede) == 0x08017668, "MATCHING: .cutscene_fountain_kirby_king_dedede moved")

    /* cutscene_fountain_actor_scripts — decompiled to C (src/cutscene_fountain_actor_scripts.c, carved by tools/carve.py) */
    .cutscene_fountain_actor_scripts : {
        build/src/cutscene_fountain_actor_scripts.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_fountain_actor_scripts) == 0x08018B84, "MATCHING: .cutscene_fountain_actor_scripts moved")

    /* cutscene_fountain_sparkles — decompiled to C (src/cutscene_fountain_sparkles.c, carved by tools/carve.py) */
    .cutscene_fountain_sparkles : {
        build/src/cutscene_fountain_sparkles.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_fountain_sparkles) == 0x08018E14, "MATCHING: .cutscene_fountain_sparkles moved")

    /* cutscene_fountain_power_orb_star_rod — decompiled to C (src/cutscene_fountain_power_orb_star_rod.c, carved by tools/carve.py) */
    .cutscene_fountain_power_orb_star_rod : {
        build/src/cutscene_fountain_power_orb_star_rod.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_fountain_power_orb_star_rod) == 0x08019000, "MATCHING: .cutscene_fountain_power_orb_star_rod moved")

    /* cutscene_fountain_blend — decompiled to C (src/cutscene_fountain_blend.c, carved by tools/carve.py) */
    .cutscene_fountain_blend : {
        build/src/cutscene_fountain_blend.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_fountain_blend) == 0x08019EEC, "MATCHING: .cutscene_fountain_blend moved")

    /* cutscene_fountain_kirby_draw — decompiled to C (src/cutscene_fountain_kirby_draw.c, carved by tools/carve.py) */
    .cutscene_fountain_kirby_draw : {
        build/src/cutscene_fountain_kirby_draw.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_fountain_kirby_draw) == 0x0801A07C, "MATCHING: .cutscene_fountain_kirby_draw moved")

    /* cutscene_fountain_sprite_draw — decompiled to C (src/cutscene_fountain_sprite_draw.c, carved by tools/carve.py) */
    .cutscene_fountain_sprite_draw : {
        build/src/cutscene_fountain_sprite_draw.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_fountain_sprite_draw) == 0x0801A3E4, "MATCHING: .cutscene_fountain_sprite_draw moved")

    /* collision_collider_lists — decompiled to C (src/collision_collider_lists.c, carved by tools/carve.py) */
    .collision_collider_lists : {
        build/src/collision_collider_lists.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_collider_lists) == 0x0801A76C, "MATCHING: .collision_collider_lists moved")

    /* collision_hit_test_players_class10 — decompiled to C (src/collision_hit_test_players_class10.c, carved by tools/carve.py) */
    .collision_hit_test_players_class10 : {
        build/src/collision_hit_test_players_class10.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_hit_test_players_class10) == 0x0801A8C8, "MATCHING: .collision_hit_test_players_class10 moved")

    /* collision_hit_test_class20 — decompiled to C (src/collision_hit_test_class20.c, carved by tools/carve.py) */
    .collision_hit_test_class20 : {
        build/src/collision_hit_test_class20.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_hit_test_class20) == 0x0801B24C, "MATCHING: .collision_hit_test_class20 moved")

    /* collision_hit_test_helpers — decompiled to C (src/collision_hit_test_helpers.c, carved by tools/carve.py) */
    .collision_hit_test_helpers : {
        build/src/collision_hit_test_helpers.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_hit_test_helpers) == 0x0801B7DC, "MATCHING: .collision_hit_test_helpers moved")

    /* collision_player_probe — decompiled to C (src/collision_player_probe.c, carved by tools/carve.py) */
    .collision_player_probe : {
        build/src/collision_player_probe.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_player_probe) == 0x0801BAA4, "MATCHING: .collision_player_probe moved")

    /* collision_collide_box — decompiled to C (src/collision_collide_box.c, carved by tools/carve.py) */
    .collision_collide_box : {
        build/src/collision_collide_box.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_collide_box) == 0x0801BCAC, "MATCHING: .collision_collide_box moved")

    /* collision_collide_point — decompiled to C (src/collision_collide_point.c, carved by tools/carve.py) */
    .collision_collide_point : {
        build/src/collision_collide_point.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_collide_point) == 0x0801C30C, "MATCHING: .collision_collide_point moved")

    /* collision_collide_box_tile_edge — decompiled to C (src/collision_collide_box_tile_edge.c, carved by tools/carve.py) */
    .collision_collide_box_tile_edge : {
        build/src/collision_collide_box_tile_edge.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_collide_box_tile_edge) == 0x0801C444, "MATCHING: .collision_collide_box_tile_edge moved")

    /* collision_probe_begin_end — decompiled to C (src/collision_probe_begin_end.c, carved by tools/carve.py) */
    .collision_probe_begin_end : {
        build/src/collision_probe_begin_end.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_probe_begin_end) == 0x0801C51C, "MATCHING: .collision_probe_begin_end moved")

    /* collision_probe_wall_on_ground — decompiled to C (src/collision_probe_wall_on_ground.c, carved by tools/carve.py) */
    .collision_probe_wall_on_ground : {
        build/src/collision_probe_wall_on_ground.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_probe_wall_on_ground) == 0x0801C690, "MATCHING: .collision_probe_wall_on_ground moved")

    /* collision_probe_box_top — decompiled to C (src/collision_probe_box_top.c, carved by tools/carve.py) */
    .collision_probe_box_top : {
        build/src/collision_probe_box_top.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_probe_box_top) == 0x0801C8DC, "MATCHING: .collision_probe_box_top moved")

    /* collision_probe_floor_in_camera_bounds — decompiled to C (src/collision_probe_floor_in_camera_bounds.c, carved by tools/carve.py) */
    .collision_probe_floor_in_camera_bounds : {
        build/src/collision_probe_floor_in_camera_bounds.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_probe_floor_in_camera_bounds) == 0x0801C930, "MATCHING: .collision_probe_floor_in_camera_bounds moved")

    /* collision_probe_floor — decompiled to C (src/collision_probe_floor.c, carved by tools/carve.py) */
    .collision_probe_floor : {
        build/src/collision_probe_floor.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_probe_floor) == 0x0801D394, "MATCHING: .collision_probe_floor moved")

    /* collision_probe_wall_in_air_ceiling — decompiled to C (src/collision_probe_wall_in_air_ceiling.c, carved by tools/carve.py) */
    .collision_probe_wall_in_air_ceiling : {
        build/src/collision_probe_wall_in_air_ceiling.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_probe_wall_in_air_ceiling) == 0x0801D9C8, "MATCHING: .collision_probe_wall_in_air_ceiling moved")

    /* collision_probe_landing_in_camera_bounds — decompiled to C (src/collision_probe_landing_in_camera_bounds.c, carved by tools/carve.py) */
    .collision_probe_landing_in_camera_bounds : {
        build/src/collision_probe_landing_in_camera_bounds.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_probe_landing_in_camera_bounds) == 0x0801E178, "MATCHING: .collision_probe_landing_in_camera_bounds moved")

    /* collision_probe_landing — decompiled to C (src/collision_probe_landing.c, carved by tools/carve.py) */
    .collision_probe_landing : {
        build/src/collision_probe_landing.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_probe_landing) == 0x0801ECD0, "MATCHING: .collision_probe_landing moved")

    /* collision_probe_no_slope_link — decompiled to C (src/collision_probe_no_slope_link.c, carved by tools/carve.py) */
    .collision_probe_no_slope_link : {
        build/src/collision_probe_no_slope_link.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_probe_no_slope_link) == 0x0801F540, "MATCHING: .collision_probe_no_slope_link moved")

    /* collision_probe_along_velocity — decompiled to C (src/collision_probe_along_velocity.c, carved by tools/carve.py) */
    .collision_probe_along_velocity : {
        build/src/collision_probe_along_velocity.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_probe_along_velocity) == 0x0801FF84, "MATCHING: .collision_probe_along_velocity moved")

    /* collision_probe_point_stop — decompiled to C (src/collision_probe_point_stop.c, carved by tools/carve.py) */
    .collision_probe_point_stop : {
        build/src/collision_probe_point_stop.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_probe_point_stop) == 0x0802069C, "MATCHING: .collision_probe_point_stop moved")

    /* collision_probe_point_push_out_tile_edge — decompiled to C (src/collision_probe_point_push_out_tile_edge.c, carved by tools/carve.py) */
    .collision_probe_point_push_out_tile_edge : {
        build/src/collision_probe_point_push_out_tile_edge.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_probe_point_push_out_tile_edge) == 0x080207A0, "MATCHING: .collision_probe_point_push_out_tile_edge moved")

    /* collision_probe_water_drift — decompiled to C (src/collision_probe_water_drift.c, carved by tools/carve.py) */
    .collision_probe_water_drift : {
        build/src/collision_probe_water_drift.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_probe_water_drift) == 0x08021130, "MATCHING: .collision_probe_water_drift moved")

    /* collision_probe_water — decompiled to C (src/collision_probe_water.c, carved by tools/carve.py) */
    .collision_probe_water : {
        build/src/collision_probe_water.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_probe_water) == 0x0802136C, "MATCHING: .collision_probe_water moved")

    /* collision_query_pixel — decompiled to C (src/collision_query_pixel.c, carved by tools/carve.py) */
    .collision_query_pixel : {
        build/src/collision_query_pixel.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_query_pixel) == 0x080214E0, "MATCHING: .collision_query_pixel moved")

    /* collision_terrain_init — decompiled to C (src/collision_terrain_init.c, carved by tools/carve.py) */
    .collision_terrain_init : {
        build/src/collision_terrain_init.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.collision_terrain_init) == 0x08021B18, "MATCHING: .collision_terrain_init moved")

    /* room_reset_level_load_room — decompiled to C (src/room_reset_level_load_room.c, carved by tools/carve.py) */
    .room_reset_level_load_room : {
        build/src/room_reset_level_load_room.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_reset_level_load_room) == 0x0802296C, "MATCHING: .room_reset_level_load_room moved")

    /* room_task — decompiled to C (src/room_task.c, carved by tools/carve.py) */
    .room_task : {
        build/src/room_task.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_task) == 0x08023618, "MATCHING: .room_task moved")

    /* room_hub — decompiled to C (src/room_hub.c, carved by tools/carve.py) */
    .room_hub : {
        build/src/room_hub.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_hub) == 0x08023948, "MATCHING: .room_hub moved")

    /* room_enter_exit — decompiled to C (src/room_enter_exit.c, carved by tools/carve.py) */
    .room_enter_exit : {
        build/src/room_enter_exit.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_enter_exit) == 0x080242D0, "MATCHING: .room_enter_exit moved")

    /* room_stage_helpers — decompiled to C (src/room_stage_helpers.c, carved by tools/carve.py) */
    .room_stage_helpers : {
        build/src/room_stage_helpers.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_stage_helpers) == 0x080261C0, "MATCHING: .room_stage_helpers moved")

    /* room_doors — decompiled to C (src/room_doors.c, carved by tools/carve.py) */
    .room_doors : {
        build/src/room_doors.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_doors) == 0x08026B60, "MATCHING: .room_doors moved")

    /* room_stop_pause — decompiled to C (src/room_stop_pause.c, carved by tools/carve.py) */
    .room_stop_pause : {
        build/src/room_stop_pause.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_stop_pause) == 0x080270D0, "MATCHING: .room_stop_pause moved")

    /* room_restart_point — decompiled to C (src/room_restart_point.c, carved by tools/carve.py) */
    .room_restart_point : {
        build/src/room_restart_point.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_restart_point) == 0x080273A0, "MATCHING: .room_restart_point moved")

    /* room_hub_map — decompiled to C (src/room_hub_map.c, carved by tools/carve.py) */
    .room_hub_map : {
        build/src/room_hub_map.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_hub_map) == 0x08027A6C, "MATCHING: .room_hub_map moved")

    /* room_bg_layout — decompiled to C (src/room_bg_layout.c, carved by tools/carve.py) */
    .room_bg_layout : {
        build/src/room_bg_layout.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_bg_layout) == 0x08027E28, "MATCHING: .room_bg_layout moved")

    /* room_spawn_door_objects — decompiled to C (src/room_spawn_door_objects.c, carved by tools/carve.py) */
    .room_spawn_door_objects : {
        build/src/room_spawn_door_objects.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_spawn_door_objects) == 0x08028320, "MATCHING: .room_spawn_door_objects moved")

    /* room_camera_init — decompiled to C (src/room_camera_init.c, carved by tools/carve.py) */
    .room_camera_init : {
        build/src/room_camera_init.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_camera_init) == 0x08028B8C, "MATCHING: .room_camera_init moved")

    /* camera_stream_scroll — decompiled to C (src/camera_stream_scroll.c, carved by tools/carve.py) */
    .camera_stream_scroll : {
        build/src/camera_stream_scroll.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_stream_scroll) == 0x080296A0, "MATCHING: .camera_stream_scroll moved")

    /* camera_player_group — decompiled to C (src/camera_player_group.c, carved by tools/carve.py) */
    .camera_player_group : {
        build/src/camera_player_group.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_player_group) == 0x08029C74, "MATCHING: .camera_player_group moved")

    /* camera_draw_bg_map — decompiled to C (src/camera_draw_bg_map.c, carved by tools/carve.py) */
    .camera_draw_bg_map : {
        build/src/camera_draw_bg_map.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_draw_bg_map) == 0x0802A9CC, "MATCHING: .camera_draw_bg_map moved")

    /* camera_block_anim_clip — decompiled to C (src/camera_block_anim_clip.c, carved by tools/carve.py) */
    .camera_block_anim_clip : {
        build/src/camera_block_anim_clip.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_block_anim_clip) == 0x0802B2F0, "MATCHING: .camera_block_anim_clip moved")

    /* camera_scroll_lock — decompiled to C (src/camera_scroll_lock.c, carved by tools/carve.py) */
    .camera_scroll_lock : {
        build/src/camera_scroll_lock.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_scroll_lock) == 0x0802B4BC, "MATCHING: .camera_scroll_lock moved")

    /* camera_hub — decompiled to C (src/camera_hub.c, carved by tools/carve.py) */
    .camera_hub : {
        build/src/camera_hub.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_hub) == 0x0802C42C, "MATCHING: .camera_hub moved")

    /* camera_bg_anims — decompiled to C (src/camera_bg_anims.c, carved by tools/carve.py) */
    .camera_bg_anims : {
        build/src/camera_bg_anims.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_bg_anims) == 0x0802D01C, "MATCHING: .camera_bg_anims moved")

    /* camera_map_events — decompiled to C (src/camera_map_events.c, carved by tools/carve.py) */
    .camera_map_events : {
        build/src/camera_map_events.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_map_events) == 0x0802D38C, "MATCHING: .camera_map_events moved")

    /* camera_door_signs — decompiled to C (src/camera_door_signs.c, carved by tools/carve.py) */
    .camera_door_signs : {
        build/src/camera_door_signs.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_door_signs) == 0x0802EAC8, "MATCHING: .camera_door_signs moved")

    /* camera_warp_star_station_door — decompiled to C (src/camera_warp_star_station_door.c, carved by tools/carve.py) */
    .camera_warp_star_station_door : {
        build/src/camera_warp_star_station_door.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_warp_star_station_door) == 0x0802F62C, "MATCHING: .camera_warp_star_station_door moved")

    /* camera_stage_effect — decompiled to C (src/camera_stage_effect.c, carved by tools/carve.py) */
    .camera_stage_effect : {
        build/src/camera_stage_effect.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_stage_effect) == 0x08030238, "MATCHING: .camera_stage_effect moved")

    /* camera_world_sprite_block_anims — decompiled to C (src/camera_world_sprite_block_anims.c, carved by tools/carve.py) */
    .camera_world_sprite_block_anims : {
        build/src/camera_world_sprite_block_anims.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_world_sprite_block_anims) == 0x080306B4, "MATCHING: .camera_world_sprite_block_anims moved")

    /* player_break_blocks — decompiled to C (src/player_break_blocks.c, carved by tools/carve.py) */
    .player_break_blocks : {
        build/src/player_break_blocks.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_break_blocks) == 0x08030804, "MATCHING: .player_break_blocks moved")

    /* player_block_anims — decompiled to C (src/player_block_anims.c, carved by tools/carve.py) */
    .player_block_anims : {
        build/src/player_block_anims.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_block_anims) == 0x080318B4, "MATCHING: .player_block_anims moved")

    /* player_task — decompiled to C (src/player_task.c, carved by tools/carve.py) */
    .player_task : {
        build/src/player_task.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_task) == 0x08032688, "MATCHING: .player_task moved")

    /* player_stand_walk — decompiled to C (src/player_stand_walk.c, carved by tools/carve.py) */
    .player_stand_walk : {
        build/src/player_stand_walk.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_stand_walk) == 0x080337F4, "MATCHING: .player_stand_walk moved")

    /* player_run_jump — decompiled to C (src/player_run_jump.c, carved by tools/carve.py) */
    .player_run_jump : {
        build/src/player_run_jump.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_run_jump) == 0x080343C0, "MATCHING: .player_run_jump moved")

    /* player_fall_float — decompiled to C (src/player_fall_float.c, carved by tools/carve.py) */
    .player_fall_float : {
        build/src/player_fall_float.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_fall_float) == 0x08034F8C, "MATCHING: .player_fall_float moved")

    /* player_duck_slide — decompiled to C (src/player_duck_slide.c, carved by tools/carve.py) */
    .player_duck_slide : {
        build/src/player_duck_slide.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_duck_slide) == 0x08036280, "MATCHING: .player_duck_slide moved")

    /* player_ladder_inhale — decompiled to C (src/player_ladder_inhale.c, carved by tools/carve.py) */
    .player_ladder_inhale : {
        build/src/player_ladder_inhale.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_ladder_inhale) == 0x08036C94, "MATCHING: .player_ladder_inhale moved")

    /* player_hurt — decompiled to C (src/player_hurt.c, carved by tools/carve.py) */
    .player_hurt : {
        build/src/player_hurt.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_hurt) == 0x08037ED8, "MATCHING: .player_hurt moved")

    /* player_die_enter_door — decompiled to C (src/player_die_enter_door.c, carved by tools/carve.py) */
    .player_die_enter_door : {
        build/src/player_die_enter_door.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_die_enter_door) == 0x0803919C, "MATCHING: .player_die_enter_door moved")

    /* player_exit_door — decompiled to C (src/player_exit_door.c, carved by tools/carve.py) */
    .player_exit_door : {
        build/src/player_exit_door.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_exit_door) == 0x08039C24, "MATCHING: .player_exit_door moved")

    /* player_water — decompiled to C (src/player_water.c, carved by tools/carve.py) */
    .player_water : {
        build/src/player_water.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_water) == 0x0803AA64, "MATCHING: .player_water moved")

    /* player_share_item — decompiled to C (src/player_share_item.c, carved by tools/carve.py) */
    .player_share_item : {
        build/src/player_share_item.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_share_item) == 0x0803BDE8, "MATCHING: .player_share_item moved")

    /* player_helpers — decompiled to C (src/player_helpers.c, carved by tools/carve.py) */
    .player_helpers : {
        build/src/player_helpers.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_helpers) == 0x0803CD60, "MATCHING: .player_helpers moved")

    /* player_motion_x_preset — decompiled to C (src/player_motion_x_preset.c, carved by tools/carve.py) */
    .player_motion_x_preset : {
        build/src/player_motion_x_preset.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_motion_x_preset) == 0x08040B40, "MATCHING: .player_motion_x_preset moved")

    /* player_meta_knight_actions — decompiled to C (src/player_meta_knight_actions.c, carved by tools/carve.py) */
    .player_meta_knight_actions : {
        build/src/player_meta_knight_actions.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_meta_knight_actions) == 0x080413A4, "MATCHING: .player_meta_knight_actions moved")

    /* player_meta_knight_swim_update — decompiled to C (src/player_meta_knight_swim_update.c, carved by tools/carve.py) */
    .player_meta_knight_swim_update : {
        build/src/player_meta_knight_swim_update.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_meta_knight_swim_update) == 0x0804335C, "MATCHING: .player_meta_knight_swim_update moved")

    /* player_meta_knight_slash — decompiled to C (src/player_meta_knight_slash.c, carved by tools/carve.py) */
    .player_meta_knight_slash : {
        build/src/player_meta_knight_slash.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_meta_knight_slash) == 0x08043654, "MATCHING: .player_meta_knight_slash moved")

    /* player_spark_cutter — decompiled to C (src/player_spark_cutter.c, carved by tools/carve.py) */
    .player_spark_cutter : {
        build/src/player_spark_cutter.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_spark_cutter) == 0x080449C8, "MATCHING: .player_spark_cutter moved")

    /* player_sword — decompiled to C (src/player_sword.c, carved by tools/carve.py) */
    .player_sword : {
        build/src/player_sword.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_sword) == 0x08044D04, "MATCHING: .player_sword moved")

    /* player_burning_laser — decompiled to C (src/player_burning_laser.c, carved by tools/carve.py) */
    .player_burning_laser : {
        build/src/player_burning_laser.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_burning_laser) == 0x080455C8, "MATCHING: .player_burning_laser moved")

    /* player_mike — decompiled to C (src/player_mike.c, carved by tools/carve.py) */
    .player_mike : {
        build/src/player_mike.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_mike) == 0x08045D34, "MATCHING: .player_mike moved")

    /* player_wheel — decompiled to C (src/player_wheel.c, carved by tools/carve.py) */
    .player_wheel : {
        build/src/player_wheel.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_wheel) == 0x08046330, "MATCHING: .player_wheel moved")

    /* player_hammer — decompiled to C (src/player_hammer.c, carved by tools/carve.py) */
    .player_hammer : {
        build/src/player_hammer.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_hammer) == 0x08046C00, "MATCHING: .player_hammer moved")

    /* player_sleep — decompiled to C (src/player_sleep.c, carved by tools/carve.py) */
    .player_sleep : {
        build/src/player_sleep.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_sleep) == 0x080474E8, "MATCHING: .player_sleep moved")

    /* player_get_ability — decompiled to C (src/player_get_ability.c, carved by tools/carve.py) */
    .player_get_ability : {
        build/src/player_get_ability.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_get_ability) == 0x08047FE8, "MATCHING: .player_get_ability moved")

    /* player_ability_tiles — decompiled to C (src/player_ability_tiles.c, carved by tools/carve.py) */
    .player_ability_tiles : {
        build/src/player_ability_tiles.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_ability_tiles) == 0x08049738, "MATCHING: .player_ability_tiles moved")

    /* player_ice_freeze — decompiled to C (src/player_ice_freeze.c, carved by tools/carve.py) */
    .player_ice_freeze : {
        build/src/player_ice_freeze.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_ice_freeze) == 0x08049B48, "MATCHING: .player_ice_freeze moved")

    /* player_hi_jump — decompiled to C (src/player_hi_jump.c, carved by tools/carve.py) */
    .player_hi_jump : {
        build/src/player_hi_jump.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_hi_jump) == 0x08049F98, "MATCHING: .player_hi_jump moved")

    /* player_beam_stone — decompiled to C (src/player_beam_stone.c, carved by tools/carve.py) */
    .player_beam_stone : {
        build/src/player_beam_stone.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_beam_stone) == 0x0804A54C, "MATCHING: .player_beam_stone moved")

    /* player_tornado_crash — decompiled to C (src/player_tornado_crash.c, carved by tools/carve.py) */
    .player_tornado_crash : {
        build/src/player_tornado_crash.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_tornado_crash) == 0x0804AB70, "MATCHING: .player_tornado_crash moved")

    /* player_light — decompiled to C (src/player_light.c, carved by tools/carve.py) */
    .player_light : {
        build/src/player_light.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_light) == 0x0804B5B4, "MATCHING: .player_light moved")

    /* player_backdrop_hold — decompiled to C (src/player_backdrop_hold.c, carved by tools/carve.py) */
    .player_backdrop_hold : {
        build/src/player_backdrop_hold.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_backdrop_hold) == 0x0804B858, "MATCHING: .player_backdrop_hold moved")

    /* player_throw_hold — decompiled to C (src/player_throw_hold.c, carved by tools/carve.py) */
    .player_throw_hold : {
        build/src/player_throw_hold.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_throw_hold) == 0x0804C64C, "MATCHING: .player_throw_hold moved")

    /* player_ufo — decompiled to C (src/player_ufo.c, carved by tools/carve.py) */
    .player_ufo : {
        build/src/player_ufo.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_ufo) == 0x0804CC7C, "MATCHING: .player_ufo moved")

    /* player_backdrop_throw — decompiled to C (src/player_backdrop_throw.c, carved by tools/carve.py) */
    .player_backdrop_throw : {
        build/src/player_backdrop_throw.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_backdrop_throw) == 0x0804DC08, "MATCHING: .player_backdrop_throw moved")

    /* player_throw_update — decompiled to C (src/player_throw_update.c, carved by tools/carve.py) */
    .player_throw_update : {
        build/src/player_throw_update.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_throw_update) == 0x0804E3A0, "MATCHING: .player_throw_update moved")

    /* player_ball — decompiled to C (src/player_ball.c, carved by tools/carve.py) */
    .player_ball : {
        build/src/player_ball.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_ball) == 0x0804E5A4, "MATCHING: .player_ball moved")

    /* player_ball_roll — decompiled to C (src/player_ball_roll.c, carved by tools/carve.py) */
    .player_ball_roll : {
        build/src/player_ball_roll.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_ball_roll) == 0x0804E78C, "MATCHING: .player_ball_roll moved")

    /* player_ball_jump — decompiled to C (src/player_ball_jump.c, carved by tools/carve.py) */
    .player_ball_jump : {
        build/src/player_ball_jump.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_ball_jump) == 0x0804EE08, "MATCHING: .player_ball_jump moved")

    /* player_ball_helpers — decompiled to C (src/player_ball_helpers.c, carved by tools/carve.py) */
    .player_ball_helpers : {
        build/src/player_ball_helpers.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_ball_helpers) == 0x0804F614, "MATCHING: .player_ball_helpers moved")

    /* player_star_rod — decompiled to C (src/player_star_rod.c, carved by tools/carve.py) */
    .player_star_rod : {
        build/src/player_star_rod.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_star_rod) == 0x0804F948, "MATCHING: .player_star_rod moved")

    /* player_star_rod_flight — decompiled to C (src/player_star_rod_flight.c, carved by tools/carve.py) */
    .player_star_rod_flight : {
        build/src/player_star_rod_flight.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_star_rod_flight) == 0x0804FFDC, "MATCHING: .player_star_rod_flight moved")

    /* player_object_task — decompiled to C (src/player_object_task.c, carved by tools/carve.py) */
    .player_object_task : {
        build/src/player_object_task.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_object_task) == 0x080507BC, "MATCHING: .player_object_task moved")

    /* player_object_air_puff — decompiled to C (src/player_object_air_puff.c, carved by tools/carve.py) */
    .player_object_air_puff : {
        build/src/player_object_air_puff.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_object_air_puff) == 0x080509EC, "MATCHING: .player_object_air_puff moved")

    /* player_object_laser_beam — decompiled to C (src/player_object_laser_beam.c, carved by tools/carve.py) */
    .player_object_laser_beam : {
        build/src/player_object_laser_beam.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_object_laser_beam) == 0x080514F8, "MATCHING: .player_object_laser_beam moved")

    /* player_object_ice_breath — decompiled to C (src/player_object_ice_breath.c, carved by tools/carve.py) */
    .player_object_ice_breath : {
        build/src/player_object_ice_breath.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_object_ice_breath) == 0x0805239C, "MATCHING: .player_object_ice_breath moved")

    /* player_object_ufo_shot — decompiled to C (src/player_object_ufo_shot.c, carved by tools/carve.py) */
    .player_object_ufo_shot : {
        build/src/player_object_ufo_shot.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_object_ufo_shot) == 0x08052F6C, "MATCHING: .player_object_ufo_shot moved")

    /* effect_skid_dust — decompiled to C (src/effect_skid_dust.c, carved by tools/carve.py) */
    .effect_skid_dust : {
        build/src/effect_skid_dust.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_skid_dust) == 0x08053AF4, "MATCHING: .effect_skid_dust moved")

    /* effect_slide_dust — decompiled to C (src/effect_slide_dust.c, carved by tools/carve.py) */
    .effect_slide_dust : {
        build/src/effect_slide_dust.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_slide_dust) == 0x08054330, "MATCHING: .effect_slide_dust moved")

    /* effect_death_star_ring_ability — decompiled to C (src/effect_death_star_ring_ability.c, carved by tools/carve.py) */
    .effect_death_star_ring_ability : {
        build/src/effect_death_star_ring_ability.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_death_star_ring_ability) == 0x08054A80, "MATCHING: .effect_death_star_ring_ability moved")

    /* effect_dance_star_burst — decompiled to C (src/effect_dance_star_burst.c, carved by tools/carve.py) */
    .effect_dance_star_burst : {
        build/src/effect_dance_star_burst.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_dance_star_burst) == 0x08055460, "MATCHING: .effect_dance_star_burst moved")

    /* effect_hurt_flames_sparks — decompiled to C (src/effect_hurt_flames_sparks.c, carved by tools/carve.py) */
    .effect_hurt_flames_sparks : {
        build/src/effect_hurt_flames_sparks.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_hurt_flames_sparks) == 0x08055B24, "MATCHING: .effect_hurt_flames_sparks moved")

    /* effect_ability_puffs — decompiled to C (src/effect_ability_puffs.c, carved by tools/carve.py) */
    .effect_ability_puffs : {
        build/src/effect_ability_puffs.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_ability_puffs) == 0x08056448, "MATCHING: .effect_ability_puffs moved")

    /* effect_fire_breath_spark_aura — decompiled to C (src/effect_fire_breath_spark_aura.c, carved by tools/carve.py) */
    .effect_fire_breath_spark_aura : {
        build/src/effect_fire_breath_spark_aura.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_fire_breath_spark_aura) == 0x08056DD4, "MATCHING: .effect_fire_breath_spark_aura moved")

    /* effect_burning_flames_wheel — decompiled to C (src/effect_burning_flames_wheel.c, carved by tools/carve.py) */
    .effect_burning_flames_wheel : {
        build/src/effect_burning_flames_wheel.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_burning_flames_wheel) == 0x08057494, "MATCHING: .effect_burning_flames_wheel moved")

    /* effect_mike_attack — decompiled to C (src/effect_mike_attack.c, carved by tools/carve.py) */
    .effect_mike_attack : {
        build/src/effect_mike_attack.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_mike_attack) == 0x08057CE0, "MATCHING: .effect_mike_attack moved")

    /* effect_ice_breath_freeze_aura — decompiled to C (src/effect_ice_breath_freeze_aura.c, carved by tools/carve.py) */
    .effect_ice_breath_freeze_aura : {
        build/src/effect_ice_breath_freeze_aura.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_ice_breath_freeze_aura) == 0x08058810, "MATCHING: .effect_ice_breath_freeze_aura moved")

    /* effect_hi_jump_ball — decompiled to C (src/effect_hi_jump_ball.c, carved by tools/carve.py) */
    .effect_hi_jump_ball : {
        build/src/effect_hi_jump_ball.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_hi_jump_ball) == 0x08059570, "MATCHING: .effect_hi_jump_ball moved")

    /* effect_crash_blast — decompiled to C (src/effect_crash_blast.c, carved by tools/carve.py) */
    .effect_crash_blast : {
        build/src/effect_crash_blast.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_crash_blast) == 0x0805A358, "MATCHING: .effect_crash_blast moved")

    /* effect_goal_game_dance — decompiled to C (src/effect_goal_game_dance.c, carved by tools/carve.py) */
    .effect_goal_game_dance : {
        build/src/effect_goal_game_dance.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_goal_game_dance) == 0x0805AFAC, "MATCHING: .effect_goal_game_dance moved")

    /* player_dance — decompiled to C (src/player_dance.c, carved by tools/carve.py) */
    .player_dance : {
        build/src/player_dance.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_dance) == 0x08062584, "MATCHING: .player_dance moved")

    /* actor — decompiled to C (src/actor.c, carved by tools/carve.py) */
    .actor : {
        build/src/actor.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor) == 0x08063698, "MATCHING: .actor moved")

    /* actor_helpers — decompiled to C (src/actor_helpers.c, carved by tools/carve.py) */
    .actor_helpers : {
        build/src/actor_helpers.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_helpers) == 0x080653EC, "MATCHING: .actor_helpers moved")

    /* actor_held_player — decompiled to C (src/actor_held_player.c, carved by tools/carve.py) */
    .actor_held_player : {
        build/src/actor_held_player.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_held_player) == 0x080673EC, "MATCHING: .actor_held_player moved")

    /* actor_collision — decompiled to C (src/actor_collision.c, carved by tools/carve.py) */
    .actor_collision : {
        build/src/actor_collision.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_collision) == 0x080692FC, "MATCHING: .actor_collision moved")

    /* actor_defeat — decompiled to C (src/actor_defeat.c, carved by tools/carve.py) */
    .actor_defeat : {
        build/src/actor_defeat.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_defeat) == 0x0806A344, "MATCHING: .actor_defeat moved")

    /* actor_boss_defeat — decompiled to C (src/actor_boss_defeat.c, carved by tools/carve.py) */
    .actor_boss_defeat : {
        build/src/actor_boss_defeat.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_boss_defeat) == 0x0806AD18, "MATCHING: .actor_boss_defeat moved")

    /* actor_attached — decompiled to C (src/actor_attached.c, carved by tools/carve.py) */
    .actor_attached : {
        build/src/actor_attached.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_attached) == 0x0806B2E4, "MATCHING: .actor_attached moved")

    /* actor_attached_throw_star_flash — decompiled to C (src/actor_attached_throw_star_flash.c, carved by tools/carve.py) */
    .actor_attached_throw_star_flash : {
        build/src/actor_attached_throw_star_flash.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_attached_throw_star_flash) == 0x0806C2A4, "MATCHING: .actor_attached_throw_star_flash moved")

    /* actor_dust — decompiled to C (src/actor_dust.c, carved by tools/carve.py) */
    .actor_dust : {
        build/src/actor_dust.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_dust) == 0x0806CD40, "MATCHING: .actor_dust moved")

    /* actor_burst_effects — decompiled to C (src/actor_burst_effects.c, carved by tools/carve.py) */
    .actor_burst_effects : {
        build/src/actor_burst_effects.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_burst_effects) == 0x0806D22C, "MATCHING: .actor_burst_effects moved")

    /* actor_hit_effects — decompiled to C (src/actor_hit_effects.c, carved by tools/carve.py) */
    .actor_hit_effects : {
        build/src/actor_hit_effects.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_hit_effects) == 0x0806E0F0, "MATCHING: .actor_hit_effects moved")

    /* player_warp_star_ride — decompiled to C (src/player_warp_star_ride.c, carved by tools/carve.py) */
    .player_warp_star_ride : {
        build/src/player_warp_star_ride.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_warp_star_ride) == 0x0806EF5C, "MATCHING: .player_warp_star_ride moved")

    /* player_meta_knight_warp_star_ride — decompiled to C (src/player_meta_knight_warp_star_ride.c, carved by tools/carve.py) */
    .player_meta_knight_warp_star_ride : {
        build/src/player_meta_knight_warp_star_ride.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_meta_knight_warp_star_ride) == 0x0806FF24, "MATCHING: .player_meta_knight_warp_star_ride moved")

    /* cutscene_warp_star — decompiled to C (src/cutscene_warp_star.c, carved by tools/carve.py) */
    .cutscene_warp_star : {
        build/src/cutscene_warp_star.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_warp_star) == 0x08070EC0, "MATCHING: .cutscene_warp_star moved")

    /* cutscene_warp_star_flights — decompiled to C (src/cutscene_warp_star_flights.c, carved by tools/carve.py) */
    .cutscene_warp_star_flights : {
        build/src/cutscene_warp_star_flights.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_warp_star_flights) == 0x08072D8C, "MATCHING: .cutscene_warp_star_flights moved")

    /* cutscene_nightmare_power_orb_escape — decompiled to C (src/cutscene_nightmare_power_orb_escape.c, carved by tools/carve.py) */
    .cutscene_nightmare_power_orb_escape : {
        build/src/cutscene_nightmare_power_orb_escape.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_nightmare_power_orb_escape) == 0x08074C0C, "MATCHING: .cutscene_nightmare_power_orb_escape moved")

    /* cutscene_cannon — decompiled to C (src/cutscene_cannon.c, carved by tools/carve.py) */
    .cutscene_cannon : {
        build/src/cutscene_cannon.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_cannon) == 0x080763E8, "MATCHING: .cutscene_cannon moved")

    /* cutscene_big_switch_room_particles — decompiled to C (src/cutscene_big_switch_room_particles.c, carved by tools/carve.py) */
    .cutscene_big_switch_room_particles : {
        build/src/cutscene_big_switch_room_particles.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.cutscene_big_switch_room_particles) == 0x08077AE0, "MATCHING: .cutscene_big_switch_room_particles moved")

    /* enemy_sparky — decompiled to C (src/enemy_sparky.c, carved by tools/carve.py) */
    .enemy_sparky : {
        build/src/enemy_sparky.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_sparky) == 0x08078B68, "MATCHING: .enemy_sparky moved")

    /* enemy_sword_and_blade_knight — decompiled to C (src/enemy_sword_and_blade_knight.c, carved by tools/carve.py) */
    .enemy_sword_and_blade_knight : {
        build/src/enemy_sword_and_blade_knight.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_sword_and_blade_knight) == 0x0807AA5C, "MATCHING: .enemy_sword_and_blade_knight moved")

    /* enemy_rocky — decompiled to C (src/enemy_rocky.c, carved by tools/carve.py) */
    .enemy_rocky : {
        build/src/enemy_rocky.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_rocky) == 0x0807D3B0, "MATCHING: .enemy_rocky moved")

    /* enemy_kabu — decompiled to C (src/enemy_kabu.c, carved by tools/carve.py) */
    .enemy_kabu : {
        build/src/enemy_kabu.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_kabu) == 0x0807F044, "MATCHING: .enemy_kabu moved")

    /* enemy_starman_poppy_bros_jr — decompiled to C (src/enemy_starman_poppy_bros_jr.c, carved by tools/carve.py) */
    .enemy_starman_poppy_bros_jr : {
        build/src/enemy_starman_poppy_bros_jr.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_starman_poppy_bros_jr) == 0x08080B70, "MATCHING: .enemy_starman_poppy_bros_jr moved")

    /* enemy_poppy_bros_jr_wheelie — decompiled to C (src/enemy_poppy_bros_jr_wheelie.c, carved by tools/carve.py) */
    .enemy_poppy_bros_jr_wheelie : {
        build/src/enemy_poppy_bros_jr_wheelie.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_poppy_bros_jr_wheelie) == 0x080820B8, "MATCHING: .enemy_poppy_bros_jr_wheelie moved")

    /* enemy_flamer — decompiled to C (src/enemy_flamer.c, carved by tools/carve.py) */
    .enemy_flamer : {
        build/src/enemy_flamer.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_flamer) == 0x08082E68, "MATCHING: .enemy_flamer moved")

    /* enemy_noddy — decompiled to C (src/enemy_noddy.c, carved by tools/carve.py) */
    .enemy_noddy : {
        build/src/enemy_noddy.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_noddy) == 0x080844C4, "MATCHING: .enemy_noddy moved")

    /* enemy_waddle_doo — decompiled to C (src/enemy_waddle_doo.c, carved by tools/carve.py) */
    .enemy_waddle_doo : {
        build/src/enemy_waddle_doo.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_waddle_doo) == 0x08084D14, "MATCHING: .enemy_waddle_doo moved")

    /* enemy_bronto_burt_twizzy — decompiled to C (src/enemy_bronto_burt_twizzy.c, carved by tools/carve.py) */
    .enemy_bronto_burt_twizzy : {
        build/src/enemy_bronto_burt_twizzy.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_bronto_burt_twizzy) == 0x080860F8, "MATCHING: .enemy_bronto_burt_twizzy moved")

    /* enemy_slippy — decompiled to C (src/enemy_slippy.c, carved by tools/carve.py) */
    .enemy_slippy : {
        build/src/enemy_slippy.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_slippy) == 0x08088000, "MATCHING: .enemy_slippy moved")

    /* enemy_blipper_gip — decompiled to C (src/enemy_blipper_gip.c, carved by tools/carve.py) */
    .enemy_blipper_gip : {
        build/src/enemy_blipper_gip.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_blipper_gip) == 0x0808AA68, "MATCHING: .enemy_blipper_gip moved")

    /* enemy_broom_hatter — decompiled to C (src/enemy_broom_hatter.c, carved by tools/carve.py) */
    .enemy_broom_hatter : {
        build/src/enemy_broom_hatter.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_broom_hatter) == 0x0808CCE8, "MATCHING: .enemy_broom_hatter moved")

    /* enemy_shotzo — decompiled to C (src/enemy_shotzo.c, carved by tools/carve.py) */
    .enemy_shotzo : {
        build/src/enemy_shotzo.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_shotzo) == 0x0808E404, "MATCHING: .enemy_shotzo moved")

    /* enemy_shotzo_coner — decompiled to C (src/enemy_shotzo_coner.c, carved by tools/carve.py) */
    .enemy_shotzo_coner : {
        build/src/enemy_shotzo_coner.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_shotzo_coner) == 0x0808F41C, "MATCHING: .enemy_shotzo_coner moved")

    /* enemy_bonkers — decompiled to C (src/enemy_bonkers.c, carved by tools/carve.py) */
    .enemy_bonkers : {
        build/src/enemy_bonkers.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_bonkers) == 0x0809000C, "MATCHING: .enemy_bonkers moved")

    /* enemy_poppy_bros_sr — decompiled to C (src/enemy_poppy_bros_sr.c, carved by tools/carve.py) */
    .enemy_poppy_bros_sr : {
        build/src/enemy_poppy_bros_sr.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_poppy_bros_sr) == 0x0809113C, "MATCHING: .enemy_poppy_bros_sr moved")

    /* enemy_bugzzy — decompiled to C (src/enemy_bugzzy.c, carved by tools/carve.py) */
    .enemy_bugzzy : {
        build/src/enemy_bugzzy.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_bugzzy) == 0x08091F9C, "MATCHING: .enemy_bugzzy moved")

    /* enemy_grand_wheelie — decompiled to C (src/enemy_grand_wheelie.c, carved by tools/carve.py) */
    .enemy_grand_wheelie : {
        build/src/enemy_grand_wheelie.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_grand_wheelie) == 0x08093F64, "MATCHING: .enemy_grand_wheelie moved")

    /* enemy_fire_lion — decompiled to C (src/enemy_fire_lion.c, carved by tools/carve.py) */
    .enemy_fire_lion : {
        build/src/enemy_fire_lion.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_fire_lion) == 0x080957BC, "MATCHING: .enemy_fire_lion moved")

    /* enemy_fire_lion_flame — decompiled to C (src/enemy_fire_lion_flame.c, carved by tools/carve.py) */
    .enemy_fire_lion_flame : {
        build/src/enemy_fire_lion_flame.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_fire_lion_flame) == 0x080970C4, "MATCHING: .enemy_fire_lion_flame moved")

    /* enemy_phan_phan — decompiled to C (src/enemy_phan_phan.c, carved by tools/carve.py) */
    .enemy_phan_phan : {
        build/src/enemy_phan_phan.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_phan_phan) == 0x080974C8, "MATCHING: .enemy_phan_phan moved")

    /* enemy_mr_frosty — decompiled to C (src/enemy_mr_frosty.c, carved by tools/carve.py) */
    .enemy_mr_frosty : {
        build/src/enemy_mr_frosty.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_mr_frosty) == 0x080988F8, "MATCHING: .enemy_mr_frosty moved")

    /* enemy_mr_tick_tock — decompiled to C (src/enemy_mr_tick_tock.c, carved by tools/carve.py) */
    .enemy_mr_tick_tock : {
        build/src/enemy_mr_tick_tock.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_mr_tick_tock) == 0x08099B20, "MATCHING: .enemy_mr_tick_tock moved")

    /* enemy_meta_knights — decompiled to C (src/enemy_meta_knights.c, carved by tools/carve.py) */
    .enemy_meta_knights : {
        build/src/enemy_meta_knights.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_meta_knights) == 0x0809BA44, "MATCHING: .enemy_meta_knights moved")

    /* enemy_meta_knights_lineup — decompiled to C (src/enemy_meta_knights_lineup.c, carved by tools/carve.py) */
    .enemy_meta_knights_lineup : {
        build/src/enemy_meta_knights_lineup.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_meta_knights_lineup) == 0x0809C028, "MATCHING: .enemy_meta_knights_lineup moved")

    /* enemy_axe_knight — decompiled to C (src/enemy_axe_knight.c, carved by tools/carve.py) */
    .enemy_axe_knight : {
        build/src/enemy_axe_knight.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_axe_knight) == 0x0809C0A8, "MATCHING: .enemy_axe_knight moved")

    /* enemy_axe_knight_slash_loop_update — decompiled to C (src/enemy_axe_knight_slash_loop_update.c, carved by tools/carve.py) */
    .enemy_axe_knight_slash_loop_update : {
        build/src/enemy_axe_knight_slash_loop_update.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_axe_knight_slash_loop_update) == 0x0809CB90, "MATCHING: .enemy_axe_knight_slash_loop_update moved")

    /* enemy_javelin_knight — decompiled to C (src/enemy_javelin_knight.c, carved by tools/carve.py) */
    .enemy_javelin_knight : {
        build/src/enemy_javelin_knight.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_javelin_knight) == 0x0809CC24, "MATCHING: .enemy_javelin_knight moved")

    /* enemy_javelin_knight_overlay — decompiled to C (src/enemy_javelin_knight_overlay.c, carved by tools/carve.py) */
    .enemy_javelin_knight_overlay : {
        build/src/enemy_javelin_knight_overlay.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_javelin_knight_overlay) == 0x0809D994, "MATCHING: .enemy_javelin_knight_overlay moved")

    /* enemy_mace_knight_trident_knight — decompiled to C (src/enemy_mace_knight_trident_knight.c, carved by tools/carve.py) */
    .enemy_mace_knight_trident_knight : {
        build/src/enemy_mace_knight_trident_knight.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_mace_knight_trident_knight) == 0x0809DA1C, "MATCHING: .enemy_mace_knight_trident_knight moved")

    /* enemy_trident_knight_overlay — decompiled to C (src/enemy_trident_knight_overlay.c, carved by tools/carve.py) */
    .enemy_trident_knight_overlay : {
        build/src/enemy_trident_knight_overlay.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_trident_knight_overlay) == 0x0809F2F4, "MATCHING: .enemy_trident_knight_overlay moved")

    /* enemy_meta_knights_knight — decompiled to C (src/enemy_meta_knights_knight.c, carved by tools/carve.py) */
    .enemy_meta_knights_knight : {
        build/src/enemy_meta_knights_knight.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_meta_knights_knight) == 0x0809F37C, "MATCHING: .enemy_meta_knights_knight moved")

    /* enemy_meta_knights_knight_palette — decompiled to C (src/enemy_meta_knights_knight_palette.c, carved by tools/carve.py) */
    .enemy_meta_knights_knight_palette : {
        build/src/enemy_meta_knights_knight_palette.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_meta_knights_knight_palette) == 0x0809F9DC, "MATCHING: .enemy_meta_knights_knight_palette moved")

    /* enemy_king_dedede_damage — decompiled to C (src/enemy_king_dedede_damage.c, carved by tools/carve.py) */
    .enemy_king_dedede_damage : {
        build/src/enemy_king_dedede_damage.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_king_dedede_damage) == 0x0809FBD0, "MATCHING: .enemy_king_dedede_damage moved")

    /* enemy_king_dedede_inhale_particles — decompiled to C (src/enemy_king_dedede_inhale_particles.c, carved by tools/carve.py) */
    .enemy_king_dedede_inhale_particles : {
        build/src/enemy_king_dedede_inhale_particles.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_king_dedede_inhale_particles) == 0x080A00EC, "MATCHING: .enemy_king_dedede_inhale_particles moved")

    /* enemy_king_dedede — decompiled to C (src/enemy_king_dedede.c, carved by tools/carve.py) */
    .enemy_king_dedede : {
        build/src/enemy_king_dedede.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_king_dedede) == 0x080A0274, "MATCHING: .enemy_king_dedede moved")

    /* enemy_mr_shine_and_mr_bright — decompiled to C (src/enemy_mr_shine_and_mr_bright.c, carved by tools/carve.py) */
    .enemy_mr_shine_and_mr_bright : {
        build/src/enemy_mr_shine_and_mr_bright.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_mr_shine_and_mr_bright) == 0x080A1590, "MATCHING: .enemy_mr_shine_and_mr_bright moved")

    /* enemy_meta_knight — decompiled to C (src/enemy_meta_knight.c, carved by tools/carve.py) */
    .enemy_meta_knight : {
        build/src/enemy_meta_knight.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_meta_knight) == 0x080A5644, "MATCHING: .enemy_meta_knight moved")

    /* enemy_meta_knight_sword_hit_box — decompiled to C (src/enemy_meta_knight_sword_hit_box.c, carved by tools/carve.py) */
    .enemy_meta_knight_sword_hit_box : {
        build/src/enemy_meta_knight_sword_hit_box.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_meta_knight_sword_hit_box) == 0x080A78A0, "MATCHING: .enemy_meta_knight_sword_hit_box moved")

    /* enemy_kracko_jr — decompiled to C (src/enemy_kracko_jr.c, carved by tools/carve.py) */
    .enemy_kracko_jr : {
        build/src/enemy_kracko_jr.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_kracko_jr) == 0x080A7998, "MATCHING: .enemy_kracko_jr moved")

    /* enemy_kracko_jr_transform_update — decompiled to C (src/enemy_kracko_jr_transform_update.c, carved by tools/carve.py) */
    .enemy_kracko_jr_transform_update : {
        build/src/enemy_kracko_jr_transform_update.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_kracko_jr_transform_update) == 0x080A860C, "MATCHING: .enemy_kracko_jr_transform_update moved")

    /* enemy_kracko — decompiled to C (src/enemy_kracko.c, carved by tools/carve.py) */
    .enemy_kracko : {
        build/src/enemy_kracko.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_kracko) == 0x080A87C8, "MATCHING: .enemy_kracko moved")

    /* enemy_kracko_jr_clamp_to_view — decompiled to C (src/enemy_kracko_jr_clamp_to_view.c, carved by tools/carve.py) */
    .enemy_kracko_jr_clamp_to_view : {
        build/src/enemy_kracko_jr_clamp_to_view.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_kracko_jr_clamp_to_view) == 0x080A932C, "MATCHING: .enemy_kracko_jr_clamp_to_view moved")

    /* enemy_kracko_cloud_lightning — decompiled to C (src/enemy_kracko_cloud_lightning.c, carved by tools/carve.py) */
    .enemy_kracko_cloud_lightning : {
        build/src/enemy_kracko_cloud_lightning.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_kracko_cloud_lightning) == 0x080A93EC, "MATCHING: .enemy_kracko_cloud_lightning moved")

    /* enemy_nightmare_wizard_heavy_mole — decompiled to C (src/enemy_nightmare_wizard_heavy_mole.c, carved by tools/carve.py) */
    .enemy_nightmare_wizard_heavy_mole : {
        build/src/enemy_nightmare_wizard_heavy_mole.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_nightmare_wizard_heavy_mole) == 0x080AA338, "MATCHING: .enemy_nightmare_wizard_heavy_mole moved")

    /* enemy_nightmare_power_orb — decompiled to C (src/enemy_nightmare_power_orb.c, carved by tools/carve.py) */
    .enemy_nightmare_power_orb : {
        build/src/enemy_nightmare_power_orb.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_nightmare_power_orb) == 0x080AE3BC, "MATCHING: .enemy_nightmare_power_orb moved")

    /* actor_whispy_woods_items — decompiled to C (src/actor_whispy_woods_items.c, carved by tools/carve.py) */
    .actor_whispy_woods_items : {
        build/src/actor_whispy_woods_items.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_whispy_woods_items) == 0x080B2FE8, "MATCHING: .actor_whispy_woods_items moved")

    /* room_object_gfx — decompiled to C (src/room_object_gfx.c, carved by tools/carve.py) */
    .room_object_gfx : {
        build/src/room_object_gfx.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_object_gfx) == 0x080B4EA8, "MATCHING: .room_object_gfx moved")

    /* room_spawn_objects — decompiled to C (src/room_spawn_objects.c, carved by tools/carve.py) */
    .room_spawn_objects : {
        build/src/room_spawn_objects.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_spawn_objects) == 0x080B5024, "MATCHING: .room_spawn_objects moved")

    /* room_enemy_gfx — decompiled to C (src/room_enemy_gfx.c, carved by tools/carve.py) */
    .room_enemy_gfx : {
        build/src/room_enemy_gfx.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_enemy_gfx) == 0x080B5670, "MATCHING: .room_enemy_gfx moved")

    /* room_gfx_spawns — decompiled to C (src/room_gfx_spawns.c, carved by tools/carve.py) */
    .room_gfx_spawns : {
        build/src/room_gfx_spawns.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_gfx_spawns) == 0x080B5840, "MATCHING: .room_gfx_spawns moved")

    /* main_hblank_bands_in_blend — decompiled to C (src/main_hblank_bands_in_blend.c, carved by tools/carve.py) */
    .main_hblank_bands_in_blend : {
        build/src/main_hblank_bands_in_blend.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_hblank_bands_in_blend) == 0x080B6154, "MATCHING: .main_hblank_bands_in_blend moved")

    /* main_hblank_bands_out_blend — decompiled to C (src/main_hblank_bands_out_blend.c, carved by tools/carve.py) */
    .main_hblank_bands_out_blend : {
        build/src/main_hblank_bands_out_blend.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_hblank_bands_out_blend) == 0x080B6290, "MATCHING: .main_hblank_bands_out_blend moved")

    /* main_hblank_bands_out — decompiled to C (src/main_hblank_bands_out.c, carved by tools/carve.py) */
    .main_hblank_bands_out : {
        build/src/main_hblank_bands_out.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_hblank_bands_out) == 0x080B63A4, "MATCHING: .main_hblank_bands_out moved")

    /* main_hblank_bands_in_link_play — decompiled to C (src/main_hblank_bands_in_link_play.c, carved by tools/carve.py) */
    .main_hblank_bands_in_link_play : {
        build/src/main_hblank_bands_in_link_play.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_hblank_bands_in_link_play) == 0x080B6474, "MATCHING: .main_hblank_bands_in_link_play moved")

    /* main_room_hblank_scroll — decompiled to C (src/main_room_hblank_scroll.c, carved by tools/carve.py) */
    .main_room_hblank_scroll : {
        build/src/main_room_hblank_scroll.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_room_hblank_scroll) == 0x080B6A90, "MATCHING: .main_room_hblank_scroll moved")

    /* main_hblank_row_scroll — decompiled to C (src/main_hblank_row_scroll.c, carved by tools/carve.py) */
    .main_hblank_row_scroll : {
        build/src/main_hblank_row_scroll.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_hblank_row_scroll) == 0x080B6B08, "MATCHING: .main_hblank_row_scroll moved")

    /* main_hblank_uniform_scroll — decompiled to C (src/main_hblank_uniform_scroll.c, carved by tools/carve.py) */
    .main_hblank_uniform_scroll : {
        build/src/main_hblank_uniform_scroll.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_hblank_uniform_scroll) == 0x080B6C40, "MATCHING: .main_hblank_uniform_scroll moved")

    /* main_hblank_row_scroll_reverse — decompiled to C (src/main_hblank_row_scroll_reverse.c, carved by tools/carve.py) */
    .main_hblank_row_scroll_reverse : {
        build/src/main_hblank_row_scroll_reverse.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_hblank_row_scroll_reverse) == 0x080B6D04, "MATCHING: .main_hblank_row_scroll_reverse moved")

    /* main_hblank_scroll — decompiled to C (src/main_hblank_scroll.c, carved by tools/carve.py) */
    .main_hblank_scroll : {
        build/src/main_hblank_scroll.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main_hblank_scroll) == 0x080B6E44, "MATCHING: .main_hblank_scroll moved")

    /* save_input_recorder_start — decompiled to C (src/save_input_recorder_start.c, carved by tools/carve.py) */
    .save_input_recorder_start : {
        build/src/save_input_recorder_start.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_input_recorder_start) == 0x080B6F38, "MATCHING: .save_input_recorder_start moved")

    /* save_input_recorder_restore — decompiled to C (src/save_input_recorder_restore.c, carved by tools/carve.py) */
    .save_input_recorder_restore : {
        build/src/save_input_recorder_restore.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_input_recorder_restore) == 0x080B72BC, "MATCHING: .save_input_recorder_restore moved")

    /* save_input_recorder_frame — decompiled to C (src/save_input_recorder_frame.c, carved by tools/carve.py) */
    .save_input_recorder_frame : {
        build/src/save_input_recorder_frame.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_input_recorder_frame) == 0x080B75A4, "MATCHING: .save_input_recorder_frame moved")

    /* save_init_slots — decompiled to C (src/save_init_slots.c, carved by tools/carve.py) */
    .save_init_slots : {
        build/src/save_init_slots.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_init_slots) == 0x080B77D4, "MATCHING: .save_init_slots moved")

    /* save_completion_percent — decompiled to C (src/save_completion_percent.c, carved by tools/carve.py) */
    .save_completion_percent : {
        build/src/save_completion_percent.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_completion_percent) == 0x080B79B8, "MATCHING: .save_completion_percent moved")

    /* save_write_slot — decompiled to C (src/save_write_slot.c, carved by tools/carve.py) */
    .save_write_slot : {
        build/src/save_write_slot.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_write_slot) == 0x080B7A9C, "MATCHING: .save_write_slot moved")

    /* save_slot_checksum — decompiled to C (src/save_slot_checksum.c, carved by tools/carve.py) */
    .save_slot_checksum : {
        build/src/save_slot_checksum.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_slot_checksum) == 0x080B7DF4, "MATCHING: .save_slot_checksum moved")

    /* save_store_progress — decompiled to C (src/save_store_progress.c, carved by tools/carve.py) */
    .save_store_progress : {
        build/src/save_store_progress.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_store_progress) == 0x080B7E14, "MATCHING: .save_store_progress moved")

    /* save_receive_link_slots — decompiled to C (src/save_receive_link_slots.c, carved by tools/carve.py) */
    .save_receive_link_slots : {
        build/src/save_receive_link_slots.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_receive_link_slots) == 0x080B8694, "MATCHING: .save_receive_link_slots moved")

    /* save_exchange_link_slots — decompiled to C (src/save_exchange_link_slots.c, carved by tools/carve.py) */
    .save_exchange_link_slots : {
        build/src/save_exchange_link_slots.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_exchange_link_slots) == 0x080B8888, "MATCHING: .save_exchange_link_slots moved")

    /* save_merge_link_slots — decompiled to C (src/save_merge_link_slots.c, carved by tools/carve.py) */
    .save_merge_link_slots : {
        build/src/save_merge_link_slots.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_merge_link_slots) == 0x080B8918, "MATCHING: .save_merge_link_slots moved")

    /* player_life_request — decompiled to C (src/player_life_request.c, carved by tools/carve.py) */
    .player_life_request : {
        build/src/player_life_request.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_life_request) == 0x080B8EA0, "MATCHING: .player_life_request moved")

    /* subgame — decompiled to C (src/subgame.c, carved by tools/carve.py) */
    .subgame : {
        build/src/subgame.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame) == 0x080B9D0C, "MATCHING: .subgame moved")

    /* subgame_quick_draw — decompiled to C (src/subgame_quick_draw.c, carved by tools/carve.py) */
    .subgame_quick_draw : {
        build/src/subgame_quick_draw.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_quick_draw) == 0x080BA774, "MATCHING: .subgame_quick_draw moved")

    /* subgame_quick_draw_results — decompiled to C (src/subgame_quick_draw_results.c, carved by tools/carve.py) */
    .subgame_quick_draw_results : {
        build/src/subgame_quick_draw_results.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_quick_draw_results) == 0x080BB528, "MATCHING: .subgame_quick_draw_results moved")

    /* subgame_quick_draw_objects — decompiled to C (src/subgame_quick_draw_objects.c, carved by tools/carve.py) */
    .subgame_quick_draw_objects : {
        build/src/subgame_quick_draw_objects.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_quick_draw_objects) == 0x080BC0CC, "MATCHING: .subgame_quick_draw_objects moved")

    /* subgame_bomb_rally_main — decompiled to C (src/subgame_bomb_rally_main.c, carved by tools/carve.py) */
    .subgame_bomb_rally_main : {
        build/src/subgame_bomb_rally_main.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_bomb_rally_main) == 0x080BD9E8, "MATCHING: .subgame_bomb_rally_main moved")

    /* subgame_bomb_rally — decompiled to C (src/subgame_bomb_rally.c, carved by tools/carve.py) */
    .subgame_bomb_rally : {
        build/src/subgame_bomb_rally.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_bomb_rally) == 0x080BDA2C, "MATCHING: .subgame_bomb_rally moved")

    /* subgame_bomb_rally_bomb — decompiled to C (src/subgame_bomb_rally_bomb.c, carved by tools/carve.py) */
    .subgame_bomb_rally_bomb : {
        build/src/subgame_bomb_rally_bomb.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_bomb_rally_bomb) == 0x080BF994, "MATCHING: .subgame_bomb_rally_bomb moved")

    /* subgame_bomb_rally_star_burst — decompiled to C (src/subgame_bomb_rally_star_burst.c, carved by tools/carve.py) */
    .subgame_bomb_rally_star_burst : {
        build/src/subgame_bomb_rally_star_burst.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_bomb_rally_star_burst) == 0x080C0DE8, "MATCHING: .subgame_bomb_rally_star_burst moved")

    /* subgame_air_grind — decompiled to C (src/subgame_air_grind.c, carved by tools/carve.py) */
    .subgame_air_grind : {
        build/src/subgame_air_grind.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_air_grind) == 0x080C1FFC, "MATCHING: .subgame_air_grind moved")

    /* subgame_air_grind_results_sky — decompiled to C (src/subgame_air_grind_results_sky.c, carved by tools/carve.py) */
    .subgame_air_grind_results_sky : {
        build/src/subgame_air_grind_results_sky.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_air_grind_results_sky) == 0x080C243C, "MATCHING: .subgame_air_grind_results_sky moved")

    /* subgame_air_grind_racer — decompiled to C (src/subgame_air_grind_racer.c, carved by tools/carve.py) */
    .subgame_air_grind_racer : {
        build/src/subgame_air_grind_racer.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_air_grind_racer) == 0x080C2FF8, "MATCHING: .subgame_air_grind_racer moved")

    /* subgame_air_grind_racer_steps — decompiled to C (src/subgame_air_grind_racer_steps.c, carved by tools/carve.py) */
    .subgame_air_grind_racer_steps : {
        build/src/subgame_air_grind_racer_steps.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_air_grind_racer_steps) == 0x080C3648, "MATCHING: .subgame_air_grind_racer_steps moved")

    /* subgame_air_grind_effects — decompiled to C (src/subgame_air_grind_effects.c, carved by tools/carve.py) */
    .subgame_air_grind_effects : {
        build/src/subgame_air_grind_effects.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_air_grind_effects) == 0x080C3F44, "MATCHING: .subgame_air_grind_effects moved")

    /* subgame_air_grind_scenery — decompiled to C (src/subgame_air_grind_scenery.c, carved by tools/carve.py) */
    .subgame_air_grind_scenery : {
        build/src/subgame_air_grind_scenery.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_air_grind_scenery) == 0x080C4630, "MATCHING: .subgame_air_grind_scenery moved")

    /* subgame_air_grind_sprites — decompiled to C (src/subgame_air_grind_sprites.c, carved by tools/carve.py) */
    .subgame_air_grind_sprites : {
        build/src/subgame_air_grind_sprites.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_air_grind_sprites) == 0x080C4D08, "MATCHING: .subgame_air_grind_sprites moved")

    /* subgame_air_grind_course — decompiled to C (src/subgame_air_grind_course.c, carved by tools/carve.py) */
    .subgame_air_grind_course : {
        build/src/subgame_air_grind_course.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_air_grind_course) == 0x080C5284, "MATCHING: .subgame_air_grind_course moved")

    /* subgame_air_grind_depth_scale — decompiled to C (src/subgame_air_grind_depth_scale.c, carved by tools/carve.py) */
    .subgame_air_grind_depth_scale : {
        build/src/subgame_air_grind_depth_scale.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_air_grind_depth_scale) == 0x080C623C, "MATCHING: .subgame_air_grind_depth_scale moved")

    /* subgame_air_grind_half_depth — decompiled to C (src/subgame_air_grind_half_depth.c, carved by tools/carve.py) */
    .subgame_air_grind_half_depth : {
        build/src/subgame_air_grind_half_depth.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_air_grind_half_depth) == 0x080C6258, "MATCHING: .subgame_air_grind_half_depth moved")

    /* ending_main — decompiled to C (src/ending_main.c, carved by tools/carve.py) */
    .ending_main : {
        build/src/ending_main.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.ending_main) == 0x080C6260, "MATCHING: .ending_main moved")

    /* ending_final_results — decompiled to C (src/ending_final_results.c, carved by tools/carve.py) */
    .ending_final_results : {
        build/src/ending_final_results.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.ending_final_results) == 0x080C6420, "MATCHING: .ending_final_results moved")

    /* ending_epilogue — decompiled to C (src/ending_epilogue.c, carved by tools/carve.py) */
    .ending_epilogue : {
        build/src/ending_epilogue.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.ending_epilogue) == 0x080C6C64, "MATCHING: .ending_epilogue moved")

    /* ending_epilogue_kirby — decompiled to C (src/ending_epilogue_kirby.c, carved by tools/carve.py) */
    .ending_epilogue_kirby : {
        build/src/ending_epilogue_kirby.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.ending_epilogue_kirby) == 0x080C7E4C, "MATCHING: .ending_epilogue_kirby moved")

    /* ending_star_rod_return — decompiled to C (src/ending_star_rod_return.c, carved by tools/carve.py) */
    .ending_star_rod_return : {
        build/src/ending_star_rod_return.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.ending_star_rod_return) == 0x080C9004, "MATCHING: .ending_star_rod_return moved")

    /* boot_logo_init_objects — decompiled to C (src/boot_logo_init_objects.c, carved by tools/carve.py) */
    .boot_logo_init_objects : {
        build/src/boot_logo_init_objects.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.boot_logo_init_objects) == 0x080CAA3C, "MATCHING: .boot_logo_init_objects moved")

    /* boot_logo_update_objects — decompiled to C (src/boot_logo_update_objects.c, carved by tools/carve.py) */
    .boot_logo_update_objects : {
        build/src/boot_logo_update_objects.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.boot_logo_update_objects) == 0x080CAAB8, "MATCHING: .boot_logo_update_objects moved")

    /* game_over_screen — decompiled to C (src/game_over_screen.c, carved by tools/carve.py) */
    .game_over_screen : {
        build/src/game_over_screen.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.game_over_screen) == 0x080CACF0, "MATCHING: .game_over_screen moved")

    /* game_over_tasks — decompiled to C (src/game_over_tasks.c, carved by tools/carve.py) */
    .game_over_tasks : {
        build/src/game_over_tasks.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.game_over_tasks) == 0x080CB354, "MATCHING: .game_over_tasks moved")

    /* game_over_player — decompiled to C (src/game_over_player.c, carved by tools/carve.py) */
    .game_over_player : {
        build/src/game_over_player.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.game_over_player) == 0x080CB64C, "MATCHING: .game_over_player moved")

    /* game_over_choice — decompiled to C (src/game_over_choice.c, carved by tools/carve.py) */
    .game_over_choice : {
        build/src/game_over_choice.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.game_over_choice) == 0x080CBED4, "MATCHING: .game_over_choice moved")

    /* game_over_objects — decompiled to C (src/game_over_objects.c, carved by tools/carve.py) */
    .game_over_objects : {
        build/src/game_over_objects.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.game_over_objects) == 0x080CCD4C, "MATCHING: .game_over_objects moved")

    /* ending_credits — decompiled to C (src/ending_credits.c, carved by tools/carve.py) */
    .ending_credits : {
        build/src/ending_credits.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.ending_credits) == 0x080CD330, "MATCHING: .ending_credits moved")

    /* m4a sound engine: asm core (pret m4a_1.s equivalent; hand-scheduled
     * SDK asm, stays asm forever — issue #52). SoundMainRAM's ROM image
     * embeds an ARM-mode mixer loop (see rom-map section 8.2). */
    .m4a_1 : {
        KEEP(*(.m4a_1))
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_1) == 0x080CD89C, "MATCHING: .m4a_1 moved")

    /* m4a sound engine: C driver (pret m4a.c equivalent; to be decompiled
     * in issues #53-#55) */
    /* m4a_c1 — decompiled to C (src/m4a_c1.c, carved by tools/carve.py) */
    .m4a_c1 : {
        build/src/m4a_c1.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_c1) == 0x080CE4B8, "MATCHING: .m4a_c1 moved")

    /* m4a_cgb — decompiled to C (src/m4a_cgb.c, carved by tools/carve.py) */
    .m4a_cgb : {
        build/src/m4a_cgb.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_cgb) == 0x080CEFB4, "MATCHING: .m4a_cgb moved")

    /* m4a_ctrl — decompiled to C (src/m4a_ctrl.c, carved by tools/carve.py) */
    .m4a_ctrl : {
        build/src/m4a_ctrl.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_ctrl) == 0x080CF588, "MATCHING: .m4a_ctrl moved")

    /* SDK syscall wrappers (Thumb) */
    .sdk_swi_wrappers : {
        KEEP(*(.sdk_swi_wrappers))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sdk_swi_wrappers) == 0x080CFA4C, "MATCHING: .sdk_swi_wrappers moved")

    /* SDK SoftReset (Thumb) and its literal pool */
    .sdk_reset_helper : {
        KEEP(*(.sdk_reset_helper))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sdk_reset_helper) == 0x080CFA80, "MATCHING: .sdk_reset_helper moved")

    /* SDK SRAM driver (libagbsrd, "SRAM_V112") — decompiled to C
     * (src/agb_sram.c, compiled with old_agbcc). */
    .sram_driver : {
        build/src/agb_sram.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.sram_driver) == 0x080CFA9C, "MATCHING: .sram_driver moved")

    /* C library: call_via_rN, copy loops, division routines (verbatim) */
    .sdk_libc : {
        KEEP(*(.sdk_libc))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sdk_libc) == 0x080CFC30, "MATCHING: .sdk_libc moved")

    /* Interworking veneer (ARM) */
    .interworking_veneer : {
        KEEP(*(.interworking_veneer))
    } > ROM
    ASSERT(!MATCHING || ADDR(.interworking_veneer) == 0x080CFDDC, "MATCHING: .interworking_veneer moved")

    /* Alignment padding between veneer and IRQ table */
    .gap_interworking_veneer_irq_handler_table_14 : {
        KEEP(*(.gap_interworking_veneer_irq_handler_table_14))
    } > ROM
    ASSERT(!MATCHING || ADDR(.gap_interworking_veneer_irq_handler_table_14) == 0x080CFDE4, "MATCHING: .gap_interworking_veneer_irq_handler_table_14 moved")

    /* 14-entry IRQ handler table */
    .irq_handler_table_14 : {
        KEEP(*(.irq_handler_table_14))
    } > ROM
    ASSERT(!MATCHING || ADDR(.irq_handler_table_14) == 0x080CFDE8, "MATCHING: .irq_handler_table_14 moved")

    /* The SRAM id string "AGB  KIRBY" (gSramIdString), WriteSramEx(.., 0x0E000000, 10) src/save_write_slot.c:29 (#36 phase 2 run 3; formerly the head of lib_misc) */
    .sram_id_string : {
        KEEP(*(.sram_id_string))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sram_id_string) == 0x080CFE20, "MATCHING: .sram_id_string moved")

    /* Air Grind (M37) rodata (#36 phase 2 run 3; formerly lib_misc, lib_rodata_fir_tables and the head of level_object_tables, not sound-driver or FIR tables, #98): gAirGrindLocalPlayerSlots-gUnk_080CFFDA (src/subgame_air_grind_results_sky.c ... src/subgame_air_grind_sprites.c), palette gUnk_080D0198, sine table gAirGrindSineTable, gAirGrindDepthScales-gUnk_080D0766 (src/subgame_air_grind_course.c); 0x080D0768-0x080D0788 has no consumer */
    .air_grind_rodata : {
        KEEP(*(.air_grind_rodata))
    } > ROM
    ASSERT(!MATCHING || ADDR(.air_grind_rodata) == 0x080CFE2C, "MATCHING: .air_grind_rodata moved")

    /* Sprite sheets (asset, #36 phase 2 run 3): OAM template streams, tiles, palettes and struct TaskGfx records the frame tables reach; its head holds a RequestCopyList palette and the LZ77 streams gUnk_080D07C8/gUnk_080D1B78, its last 12 bytes the stage-count table gLevelStageCounts */
    .sprite_sheets : {
        KEEP(*(.sprite_sheets))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sprite_sheets) == 0x080D0788, "MATCHING: .sprite_sheets moved")

    /* Room BG animations (#36 phase 2): 30 scripts, 181 tile frames, 14 palette fades, 28 palettes (LoadRoomBgAnims, src/camera_bg_anims.c) (ldgroup.py: 80 rows in one output section) */
    .room_bg_anims : {
        KEEP(*(.room_bg_anims))
        build/src/data/bg_anim_scripts.o(.bg_anim_083356e0)
        KEEP(*(.room_bg_anims_08335728))
        build/src/data/bg_anim_scripts.o(.bg_anim_08335b48)
        KEEP(*(.room_bg_anims_08335b90))
        build/src/data/bg_anim_scripts.o(.bg_anim_08335db0)
        KEEP(*(.room_bg_anims_08335df8))
        build/src/data/bg_anim_scripts.o(.bg_anim_08336618)
        KEEP(*(.room_bg_anims_08336660))
        build/src/data/bg_anim_scripts.o(.bg_anim_08336e80)
        KEEP(*(.room_bg_anims_08336ec8))
        build/src/data/bg_anim_scripts.o(.bg_anim_083376e8)
        KEEP(*(.room_bg_anims_08337730))
        build/src/data/bg_anim_scripts.o(.bg_anim_08337b50)
        KEEP(*(.room_bg_anims_08337b98))
        build/src/data/bg_anim_scripts.o(.bg_anim_083396b8)
        KEEP(*(.room_bg_anims_08339700))
        build/src/data/bg_anim_scripts.o(.bg_anim_08339b20)
        KEEP(*(.room_bg_anims_08339b68))
        build/src/data/bg_anim_scripts.o(.bg_anim_08339d88)
        KEEP(*(.room_bg_anims_08339dd0))
        build/src/data/bg_anim_scripts.o(.bg_anim_0833a0e8)
        KEEP(*(.room_bg_anims_0833a120))
        build/src/data/bg_anim_scripts.o(.bg_anim_0833abb8)
        KEEP(*(.room_bg_anims_0833abf0))
        build/src/data/bg_anim_scripts.o(.bg_anim_0833ec10)
        KEEP(*(.room_bg_anims_0833ec58))
        build/src/data/bg_anim_scripts.o(.bg_anim_0833f45c)
        KEEP(*(.room_bg_anims_0833f4c4))
        build/src/data/bg_anim_scripts.o(.bg_anim_0833fcc8)
        KEEP(*(.room_bg_anims_0833fd30))
        build/src/data/bg_anim_scripts.o(.bg_anim_0833fe30)
        KEEP(*(.room_bg_anims_0833fe40))
        build/src/data/bg_anim_scripts.o(.bg_anim_0833ff40)
        KEEP(*(.room_bg_anims_0833ff50))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340050)
        KEEP(*(.room_bg_anims_08340088))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340108)
        KEEP(*(.room_bg_anims_08340118))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340198)
        KEEP(*(.room_bg_anims_083401a8))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340228)
        KEEP(*(.room_bg_anims_08340260))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340360)
        KEEP(*(.room_bg_anims_08340370))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340470)
        KEEP(*(.room_bg_anims_08340480))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340580)
        KEEP(*(.room_bg_anims_08340590))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340690)
        KEEP(*(.room_bg_anims_083406d0))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340750)
        KEEP(*(.room_bg_anims_08340760))
        build/src/data/bg_anim_scripts.o(.bg_anim_083407e0)
        KEEP(*(.room_bg_anims_083407f0))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340870)
        KEEP(*(.room_bg_anims_08340880))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340900)
        KEEP(*(.room_bg_anims_08340940))
        build/src/data/bg_anim_scripts.o(.bg_anim_083447d0)
        KEEP(*(.room_bg_anims_083447f8))
        build/src/data/bg_anim_scripts.o(.bg_anim_08344c08)
        KEEP(*(.room_bg_anims_08344c30))
        build/src/data/bg_anim_scripts.o(.bg_anim_08348248)
        KEEP(*(.room_bg_anims_08348280))
        build/src/data/bg_anim_scripts.o(.bg_anim_0834b898)
        KEEP(*(.room_bg_anims_0834b8d0))
        build/src/data/bg_anim_scripts.o(.bg_anim_0834eee8)
        KEEP(*(.room_bg_anims_0834ef20))
        build/src/data/bg_anim_scripts.o(.bg_anim_08350af8)
        KEEP(*(.room_bg_anims_08350b30))
        build/src/data/bg_anim_scripts.o(.bg_anim_08353248)
        KEEP(*(.room_bg_anims_08353280))
        build/src/data/bg_anim_scripts.o(.bg_anim_08355998)
        KEEP(*(.room_bg_anims_083559d0))
        build/src/data/bg_anim_scripts.o(.bg_anim_083580e8)
        KEEP(*(.room_bg_anims_08358120))
        build/src/data/bg_anim_scripts.o(.bg_anim_0835b5bc)
        KEEP(*(.room_bg_anims_0835b604))
        build/src/data/bg_anim_scripts.o(.bg_anim_0835cf9c)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_bg_anims) == 0x08334EC0, "MATCHING: .room_bg_anims moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script0 == 0x083356E0, "MATCHING: bg_anim_083356e0 moved")
    ASSERT(!MATCHING || room_bg_anims_08335728 == 0x08335728, "MATCHING: room_bg_anims_08335728 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script1 == 0x08335B48, "MATCHING: bg_anim_08335b48 moved")
    ASSERT(!MATCHING || room_bg_anims_08335b90 == 0x08335B90, "MATCHING: room_bg_anims_08335b90 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script2 == 0x08335DB0, "MATCHING: bg_anim_08335db0 moved")
    ASSERT(!MATCHING || room_bg_anims_08335df8 == 0x08335DF8, "MATCHING: room_bg_anims_08335df8 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script3 == 0x08336618, "MATCHING: bg_anim_08336618 moved")
    ASSERT(!MATCHING || room_bg_anims_08336660 == 0x08336660, "MATCHING: room_bg_anims_08336660 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script4 == 0x08336E80, "MATCHING: bg_anim_08336e80 moved")
    ASSERT(!MATCHING || room_bg_anims_08336ec8 == 0x08336EC8, "MATCHING: room_bg_anims_08336ec8 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script5 == 0x083376E8, "MATCHING: bg_anim_083376e8 moved")
    ASSERT(!MATCHING || room_bg_anims_08337730 == 0x08337730, "MATCHING: room_bg_anims_08337730 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script6 == 0x08337B50, "MATCHING: bg_anim_08337b50 moved")
    ASSERT(!MATCHING || room_bg_anims_08337b98 == 0x08337B98, "MATCHING: room_bg_anims_08337b98 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script7 == 0x083396B8, "MATCHING: bg_anim_083396b8 moved")
    ASSERT(!MATCHING || room_bg_anims_08339700 == 0x08339700, "MATCHING: room_bg_anims_08339700 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script8 == 0x08339B20, "MATCHING: bg_anim_08339b20 moved")
    ASSERT(!MATCHING || room_bg_anims_08339b68 == 0x08339B68, "MATCHING: room_bg_anims_08339b68 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script9 == 0x08339D88, "MATCHING: bg_anim_08339d88 moved")
    ASSERT(!MATCHING || room_bg_anims_08339dd0 == 0x08339DD0, "MATCHING: room_bg_anims_08339dd0 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet2Script0 == 0x0833A0E8, "MATCHING: bg_anim_0833a0e8 moved")
    ASSERT(!MATCHING || room_bg_anims_0833a120 == 0x0833A120, "MATCHING: room_bg_anims_0833a120 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet3Script0 == 0x0833ABB8, "MATCHING: bg_anim_0833abb8 moved")
    ASSERT(!MATCHING || room_bg_anims_0833abf0 == 0x0833ABF0, "MATCHING: room_bg_anims_0833abf0 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet4Script0 == 0x0833EC10, "MATCHING: bg_anim_0833ec10 moved")
    ASSERT(!MATCHING || room_bg_anims_0833ec58 == 0x0833EC58, "MATCHING: room_bg_anims_0833ec58 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet5Script2 == 0x0833F45C, "MATCHING: bg_anim_0833f45c moved")
    ASSERT(!MATCHING || room_bg_anims_0833f4c4 == 0x0833F4C4, "MATCHING: room_bg_anims_0833f4c4 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet6Script0 == 0x0833FCC8, "MATCHING: bg_anim_0833fcc8 moved")
    ASSERT(!MATCHING || room_bg_anims_0833fd30 == 0x0833FD30, "MATCHING: room_bg_anims_0833fd30 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet5Script0Cmd1PaletteFade == 0x0833FE30, "MATCHING: bg_anim_0833fe30 moved")
    ASSERT(!MATCHING || room_bg_anims_0833fe40 == 0x0833FE40, "MATCHING: room_bg_anims_0833fe40 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet5Script0Cmd2PaletteFade == 0x0833FF40, "MATCHING: bg_anim_0833ff40 moved")
    ASSERT(!MATCHING || room_bg_anims_0833ff50 == 0x0833FF50, "MATCHING: room_bg_anims_0833ff50 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet5Script0Cmd3PaletteFade == 0x08340050, "MATCHING: bg_anim_08340050 moved")
    ASSERT(!MATCHING || room_bg_anims_08340088 == 0x08340088, "MATCHING: room_bg_anims_08340088 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet5Script1Cmd1PaletteFade == 0x08340108, "MATCHING: bg_anim_08340108 moved")
    ASSERT(!MATCHING || room_bg_anims_08340118 == 0x08340118, "MATCHING: room_bg_anims_08340118 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet5Script1Cmd2PaletteFade == 0x08340198, "MATCHING: bg_anim_08340198 moved")
    ASSERT(!MATCHING || room_bg_anims_083401a8 == 0x083401A8, "MATCHING: room_bg_anims_083401a8 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet5Script1Cmd3PaletteFade == 0x08340228, "MATCHING: bg_anim_08340228 moved")
    ASSERT(!MATCHING || room_bg_anims_08340260 == 0x08340260, "MATCHING: room_bg_anims_08340260 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet6Script1Cmd1PaletteFade == 0x08340360, "MATCHING: bg_anim_08340360 moved")
    ASSERT(!MATCHING || room_bg_anims_08340370 == 0x08340370, "MATCHING: room_bg_anims_08340370 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet6Script1Cmd2PaletteFade == 0x08340470, "MATCHING: bg_anim_08340470 moved")
    ASSERT(!MATCHING || room_bg_anims_08340480 == 0x08340480, "MATCHING: room_bg_anims_08340480 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet6Script1Cmd3PaletteFade == 0x08340580, "MATCHING: bg_anim_08340580 moved")
    ASSERT(!MATCHING || room_bg_anims_08340590 == 0x08340590, "MATCHING: room_bg_anims_08340590 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet6Script1Cmd4PaletteFade == 0x08340690, "MATCHING: bg_anim_08340690 moved")
    ASSERT(!MATCHING || room_bg_anims_083406d0 == 0x083406D0, "MATCHING: room_bg_anims_083406d0 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet6Script2Cmd1PaletteFade == 0x08340750, "MATCHING: bg_anim_08340750 moved")
    ASSERT(!MATCHING || room_bg_anims_08340760 == 0x08340760, "MATCHING: room_bg_anims_08340760 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet6Script2Cmd2PaletteFade == 0x083407E0, "MATCHING: bg_anim_083407e0 moved")
    ASSERT(!MATCHING || room_bg_anims_083407f0 == 0x083407F0, "MATCHING: room_bg_anims_083407f0 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet6Script2Cmd3PaletteFade == 0x08340870, "MATCHING: bg_anim_08340870 moved")
    ASSERT(!MATCHING || room_bg_anims_08340880 == 0x08340880, "MATCHING: room_bg_anims_08340880 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet6Script2Cmd4PaletteFade == 0x08340900, "MATCHING: bg_anim_08340900 moved")
    ASSERT(!MATCHING || room_bg_anims_08340940 == 0x08340940, "MATCHING: room_bg_anims_08340940 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet7Script0 == 0x083447D0, "MATCHING: bg_anim_083447d0 moved")
    ASSERT(!MATCHING || room_bg_anims_083447f8 == 0x083447F8, "MATCHING: room_bg_anims_083447f8 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet8Script0 == 0x08344C08, "MATCHING: bg_anim_08344c08 moved")
    ASSERT(!MATCHING || room_bg_anims_08344c30 == 0x08344C30, "MATCHING: room_bg_anims_08344c30 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet10Script0 == 0x08348248, "MATCHING: bg_anim_08348248 moved")
    ASSERT(!MATCHING || room_bg_anims_08348280 == 0x08348280, "MATCHING: room_bg_anims_08348280 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet10Script1 == 0x0834B898, "MATCHING: bg_anim_0834b898 moved")
    ASSERT(!MATCHING || room_bg_anims_0834b8d0 == 0x0834B8D0, "MATCHING: room_bg_anims_0834b8d0 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet10Script2 == 0x0834EEE8, "MATCHING: bg_anim_0834eee8 moved")
    ASSERT(!MATCHING || room_bg_anims_0834ef20 == 0x0834EF20, "MATCHING: room_bg_anims_0834ef20 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet12Script0 == 0x08350AF8, "MATCHING: bg_anim_08350af8 moved")
    ASSERT(!MATCHING || room_bg_anims_08350b30 == 0x08350B30, "MATCHING: room_bg_anims_08350b30 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet9Script0 == 0x08353248, "MATCHING: bg_anim_08353248 moved")
    ASSERT(!MATCHING || room_bg_anims_08353280 == 0x08353280, "MATCHING: room_bg_anims_08353280 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet9Script1 == 0x08355998, "MATCHING: bg_anim_08355998 moved")
    ASSERT(!MATCHING || room_bg_anims_083559d0 == 0x083559D0, "MATCHING: room_bg_anims_083559d0 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet9Script2 == 0x083580E8, "MATCHING: bg_anim_083580e8 moved")
    ASSERT(!MATCHING || room_bg_anims_08358120 == 0x08358120, "MATCHING: room_bg_anims_08358120 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet11Script1 == 0x0835B5BC, "MATCHING: bg_anim_0835b5bc moved")
    ASSERT(!MATCHING || room_bg_anims_0835b604 == 0x0835B604, "MATCHING: room_bg_anims_0835b604 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet11Script0 == 0x0835CF9C, "MATCHING: bg_anim_0835cf9c moved")

    /* Room records (#36 phase 2, #167): 333 rooms, each a metatile map, block layer, block table, doors and object list, then its RoomDef header (C: src/data/room_defs.c) (ldgroup.py: 666 rows in one output section) */
    .room_data : {
        KEEP(*(.room_data))
        build/src/data/room_defs.o(.room_def_0835d08c)
        KEEP(*(.room_data_0835d0e4))
        build/src/data/room_defs.o(.room_def_0835d300)
        KEEP(*(.room_data_0835d358))
        build/src/data/room_defs.o(.room_def_0835d4f0)
        KEEP(*(.room_data_0835d548))
        build/src/data/room_defs.o(.room_def_0835ddc0)
        KEEP(*(.room_data_0835de18))
        build/src/data/room_defs.o(.room_def_0835e2a0)
        KEEP(*(.room_data_0835e2f8))
        build/src/data/room_defs.o(.room_def_0835e70c)
        KEEP(*(.room_data_0835e764))
        build/src/data/room_defs.o(.room_def_0835e924)
        KEEP(*(.room_data_0835e97c))
        build/src/data/room_defs.o(.room_def_0835ef1c)
        KEEP(*(.room_data_0835ef74))
        build/src/data/room_defs.o(.room_def_0835f1a4)
        KEEP(*(.room_data_0835f1fc))
        build/src/data/room_defs.o(.room_def_0835f544)
        KEEP(*(.room_data_0835f59c))
        build/src/data/room_defs.o(.room_def_0835fc24)
        KEEP(*(.room_data_0835fc7c))
        build/src/data/room_defs.o(.room_def_0835ffe4)
        KEEP(*(.room_data_0836003c))
        build/src/data/room_defs.o(.room_def_083601dc)
        KEEP(*(.room_data_08360234))
        build/src/data/room_defs.o(.room_def_08360690)
        KEEP(*(.room_data_083606e8))
        build/src/data/room_defs.o(.room_def_08360b38)
        KEEP(*(.room_data_08360b90))
        build/src/data/room_defs.o(.room_def_08360f38)
        KEEP(*(.room_data_08360f90))
        build/src/data/room_defs.o(.room_def_083613a0)
        KEEP(*(.room_data_083613f8))
        build/src/data/room_defs.o(.room_def_0836179c)
        KEEP(*(.room_data_083617f4))
        build/src/data/room_defs.o(.room_def_08361cb8)
        KEEP(*(.room_data_08361d10))
        build/src/data/room_defs.o(.room_def_08362104)
        KEEP(*(.room_data_0836215c))
        build/src/data/room_defs.o(.room_def_08362788)
        KEEP(*(.room_data_083627e0))
        build/src/data/room_defs.o(.room_def_08362ad0)
        KEEP(*(.room_data_08362b28))
        build/src/data/room_defs.o(.room_def_083630a0)
        KEEP(*(.room_data_083630f8))
        build/src/data/room_defs.o(.room_def_083634e0)
        KEEP(*(.room_data_08363538))
        build/src/data/room_defs.o(.room_def_08363a0c)
        KEEP(*(.room_data_08363a64))
        build/src/data/room_defs.o(.room_def_08363e04)
        KEEP(*(.room_data_08363e5c))
        build/src/data/room_defs.o(.room_def_08364014)
        KEEP(*(.room_data_0836406c))
        build/src/data/room_defs.o(.room_def_083643bc)
        KEEP(*(.room_data_08364414))
        build/src/data/room_defs.o(.room_def_0836463c)
        KEEP(*(.room_data_08364694))
        build/src/data/room_defs.o(.room_def_08364a38)
        KEEP(*(.room_data_08364a90))
        build/src/data/room_defs.o(.room_def_08365558)
        KEEP(*(.room_data_083655b0))
        build/src/data/room_defs.o(.room_def_083657c0)
        KEEP(*(.room_data_08365818))
        build/src/data/room_defs.o(.room_def_08366340)
        KEEP(*(.room_data_08366398))
        build/src/data/room_defs.o(.room_def_0836663c)
        KEEP(*(.room_data_08366694))
        build/src/data/room_defs.o(.room_def_08366974)
        KEEP(*(.room_data_083669cc))
        build/src/data/room_defs.o(.room_def_08366b98)
        KEEP(*(.room_data_08366bf0))
        build/src/data/room_defs.o(.room_def_08366d1c)
        KEEP(*(.room_data_08366d74))
        build/src/data/room_defs.o(.room_def_08367274)
        KEEP(*(.room_data_083672cc))
        build/src/data/room_defs.o(.room_def_0836753c)
        KEEP(*(.room_data_08367594))
        build/src/data/room_defs.o(.room_def_083677f4)
        KEEP(*(.room_data_0836784c))
        build/src/data/room_defs.o(.room_def_08367a50)
        KEEP(*(.room_data_08367aa8))
        build/src/data/room_defs.o(.room_def_08367e8c)
        KEEP(*(.room_data_08367ee4))
        build/src/data/room_defs.o(.room_def_083684d4)
        KEEP(*(.room_data_0836852c))
        build/src/data/room_defs.o(.room_def_08368710)
        KEEP(*(.room_data_08368768))
        build/src/data/room_defs.o(.room_def_08368d30)
        KEEP(*(.room_data_08368d88))
        build/src/data/room_defs.o(.room_def_083692d8)
        KEEP(*(.room_data_08369330))
        build/src/data/room_defs.o(.room_def_08369be0)
        KEEP(*(.room_data_08369c38))
        build/src/data/room_defs.o(.room_def_0836a0a8)
        KEEP(*(.room_data_0836a100))
        build/src/data/room_defs.o(.room_def_0836a318)
        KEEP(*(.room_data_0836a370))
        build/src/data/room_defs.o(.room_def_0836a534)
        KEEP(*(.room_data_0836a58c))
        build/src/data/room_defs.o(.room_def_0836a6dc)
        KEEP(*(.room_data_0836a734))
        build/src/data/room_defs.o(.room_def_0836a89c)
        KEEP(*(.room_data_0836a8f4))
        build/src/data/room_defs.o(.room_def_0836ac58)
        KEEP(*(.room_data_0836acb0))
        build/src/data/room_defs.o(.room_def_0836aee4)
        KEEP(*(.room_data_0836af3c))
        build/src/data/room_defs.o(.room_def_0836b160)
        KEEP(*(.room_data_0836b1b8))
        build/src/data/room_defs.o(.room_def_0836b44c)
        KEEP(*(.room_data_0836b4a4))
        build/src/data/room_defs.o(.room_def_0836b7b4)
        KEEP(*(.room_data_0836b80c))
        build/src/data/room_defs.o(.room_def_0836b968)
        KEEP(*(.room_data_0836b9c0))
        build/src/data/room_defs.o(.room_def_0836bc70)
        KEEP(*(.room_data_0836bcc8))
        build/src/data/room_defs.o(.room_def_0836c0b0)
        KEEP(*(.room_data_0836c108))
        build/src/data/room_defs.o(.room_def_0836c50c)
        KEEP(*(.room_data_0836c564))
        build/src/data/room_defs.o(.room_def_0836ca98)
        KEEP(*(.room_data_0836caf0))
        build/src/data/room_defs.o(.room_def_0836cda8)
        KEEP(*(.room_data_0836ce00))
        build/src/data/room_defs.o(.room_def_0836d19c)
        KEEP(*(.room_data_0836d1f4))
        build/src/data/room_defs.o(.room_def_0836d334)
        KEEP(*(.room_data_0836d38c))
        build/src/data/room_defs.o(.room_def_0836d4fc)
        KEEP(*(.room_data_0836d554))
        build/src/data/room_defs.o(.room_def_0836dadc)
        KEEP(*(.room_data_0836db34))
        build/src/data/room_defs.o(.room_def_0836dd44)
        KEEP(*(.room_data_0836dd9c))
        build/src/data/room_defs.o(.room_def_0836e09c)
        KEEP(*(.room_data_0836e0f4))
        build/src/data/room_defs.o(.room_def_0836e3f4)
        KEEP(*(.room_data_0836e44c))
        build/src/data/room_defs.o(.room_def_0836e660)
        KEEP(*(.room_data_0836e6b8))
        build/src/data/room_defs.o(.room_def_0836e824)
        KEEP(*(.room_data_0836e87c))
        build/src/data/room_defs.o(.room_def_0836eed8)
        KEEP(*(.room_data_0836ef30))
        build/src/data/room_defs.o(.room_def_0836f0a4)
        KEEP(*(.room_data_0836f0fc))
        build/src/data/room_defs.o(.room_def_0836f270)
        KEEP(*(.room_data_0836f2c8))
        build/src/data/room_defs.o(.room_def_0836f634)
        KEEP(*(.room_data_0836f68c))
        build/src/data/room_defs.o(.room_def_0836f7e4)
        KEEP(*(.room_data_0836f83c))
        build/src/data/room_defs.o(.room_def_0836f9b0)
        KEEP(*(.room_data_0836fa08))
        build/src/data/room_defs.o(.room_def_0836fd44)
        KEEP(*(.room_data_0836fd9c))
        build/src/data/room_defs.o(.room_def_08370000)
        KEEP(*(.room_data_08370058))
        build/src/data/room_defs.o(.room_def_083702fc)
        KEEP(*(.room_data_08370354))
        build/src/data/room_defs.o(.room_def_083704e4)
        KEEP(*(.room_data_0837053c))
        build/src/data/room_defs.o(.room_def_08370684)
        KEEP(*(.room_data_083706dc))
        build/src/data/room_defs.o(.room_def_08370af4)
        KEEP(*(.room_data_08370b4c))
        build/src/data/room_defs.o(.room_def_08370e08)
        KEEP(*(.room_data_08370e60))
        build/src/data/room_defs.o(.room_def_0837108c)
        KEEP(*(.room_data_083710e4))
        build/src/data/room_defs.o(.room_def_08371388)
        KEEP(*(.room_data_083713e0))
        build/src/data/room_defs.o(.room_def_08371728)
        KEEP(*(.room_data_08371780))
        build/src/data/room_defs.o(.room_def_0837181c)
        KEEP(*(.room_data_08371874))
        build/src/data/room_defs.o(.room_def_08371d90)
        KEEP(*(.room_data_08371de8))
        build/src/data/room_defs.o(.room_def_08372040)
        KEEP(*(.room_data_08372098))
        build/src/data/room_defs.o(.room_def_08372240)
        KEEP(*(.room_data_08372298))
        build/src/data/room_defs.o(.room_def_083723b0)
        KEEP(*(.room_data_08372408))
        build/src/data/room_defs.o(.room_def_08372518)
        KEEP(*(.room_data_08372570))
        build/src/data/room_defs.o(.room_def_08372680)
        KEEP(*(.room_data_083726d8))
        build/src/data/room_defs.o(.room_def_08372a78)
        KEEP(*(.room_data_08372ad0))
        build/src/data/room_defs.o(.room_def_08372d58)
        KEEP(*(.room_data_08372db0))
        build/src/data/room_defs.o(.room_def_0837301c)
        KEEP(*(.room_data_08373074))
        build/src/data/room_defs.o(.room_def_083736a8)
        KEEP(*(.room_data_08373700))
        build/src/data/room_defs.o(.room_def_08373914)
        KEEP(*(.room_data_0837396c))
        build/src/data/room_defs.o(.room_def_08373ac4)
        KEEP(*(.room_data_08373b1c))
        build/src/data/room_defs.o(.room_def_0837410c)
        KEEP(*(.room_data_08374164))
        build/src/data/room_defs.o(.room_def_083742cc)
        KEEP(*(.room_data_08374324))
        build/src/data/room_defs.o(.room_def_083747d0)
        KEEP(*(.room_data_08374828))
        build/src/data/room_defs.o(.room_def_08374b28)
        KEEP(*(.room_data_08374b80))
        build/src/data/room_defs.o(.room_def_08374c74)
        KEEP(*(.room_data_08374ccc))
        build/src/data/room_defs.o(.room_def_08374dbc)
        KEEP(*(.room_data_08374e14))
        build/src/data/room_defs.o(.room_def_0837531c)
        KEEP(*(.room_data_08375374))
        build/src/data/room_defs.o(.room_def_083761d0)
        KEEP(*(.room_data_08376228))
        build/src/data/room_defs.o(.room_def_08376b24)
        KEEP(*(.room_data_08376b7c))
        build/src/data/room_defs.o(.room_def_08376d94)
        KEEP(*(.room_data_08376dec))
        build/src/data/room_defs.o(.room_def_08376f18)
        KEEP(*(.room_data_08376f70))
        build/src/data/room_defs.o(.room_def_08377144)
        KEEP(*(.room_data_0837719c))
        build/src/data/room_defs.o(.room_def_08377334)
        KEEP(*(.room_data_0837738c))
        build/src/data/room_defs.o(.room_def_08377af0)
        KEEP(*(.room_data_08377b48))
        build/src/data/room_defs.o(.room_def_08377fbc)
        KEEP(*(.room_data_08378014))
        build/src/data/room_defs.o(.room_def_08378a80)
        KEEP(*(.room_data_08378ad8))
        build/src/data/room_defs.o(.room_def_08378eb0)
        KEEP(*(.room_data_08378f08))
        build/src/data/room_defs.o(.room_def_08379218)
        KEEP(*(.room_data_08379270))
        build/src/data/room_defs.o(.room_def_08379b84)
        KEEP(*(.room_data_08379bdc))
        build/src/data/room_defs.o(.room_def_08379e60)
        KEEP(*(.room_data_08379eb8))
        build/src/data/room_defs.o(.room_def_0837a0cc)
        KEEP(*(.room_data_0837a124))
        build/src/data/room_defs.o(.room_def_0837a52c)
        KEEP(*(.room_data_0837a584))
        build/src/data/room_defs.o(.room_def_0837ab88)
        KEEP(*(.room_data_0837abe0))
        build/src/data/room_defs.o(.room_def_0837ad38)
        KEEP(*(.room_data_0837ad90))
        build/src/data/room_defs.o(.room_def_0837af10)
        KEEP(*(.room_data_0837af68))
        build/src/data/room_defs.o(.room_def_0837b7cc)
        KEEP(*(.room_data_0837b824))
        build/src/data/room_defs.o(.room_def_0837bb9c)
        KEEP(*(.room_data_0837bbf4))
        build/src/data/room_defs.o(.room_def_0837bc90)
        KEEP(*(.room_data_0837bce8))
        build/src/data/room_defs.o(.room_def_0837bfa8)
        KEEP(*(.room_data_0837c000))
        build/src/data/room_defs.o(.room_def_0837c288)
        KEEP(*(.room_data_0837c2e0))
        build/src/data/room_defs.o(.room_def_0837c424)
        KEEP(*(.room_data_0837c47c))
        build/src/data/room_defs.o(.room_def_0837c520)
        KEEP(*(.room_data_0837c578))
        build/src/data/room_defs.o(.room_def_0837c698)
        KEEP(*(.room_data_0837c6f0))
        build/src/data/room_defs.o(.room_def_0837d968)
        KEEP(*(.room_data_0837d9c0))
        build/src/data/room_defs.o(.room_def_0837de3c)
        KEEP(*(.room_data_0837de94))
        build/src/data/room_defs.o(.room_def_0837e598)
        KEEP(*(.room_data_0837e5f0))
        build/src/data/room_defs.o(.room_def_0837eb34)
        KEEP(*(.room_data_0837eb8c))
        build/src/data/room_defs.o(.room_def_0837eca8)
        KEEP(*(.room_data_0837ed00))
        build/src/data/room_defs.o(.room_def_0837ee60)
        KEEP(*(.room_data_0837eeb8))
        build/src/data/room_defs.o(.room_def_0837f028)
        KEEP(*(.room_data_0837f080))
        build/src/data/room_defs.o(.room_def_0837f73c)
        KEEP(*(.room_data_0837f794))
        build/src/data/room_defs.o(.room_def_0837fa44)
        KEEP(*(.room_data_0837fa9c))
        build/src/data/room_defs.o(.room_def_0837fdc8)
        KEEP(*(.room_data_0837fe20))
        build/src/data/room_defs.o(.room_def_083800d0)
        KEEP(*(.room_data_08380128))
        build/src/data/room_defs.o(.room_def_083805bc)
        KEEP(*(.room_data_08380614))
        build/src/data/room_defs.o(.room_def_08380758)
        KEEP(*(.room_data_083807b0))
        build/src/data/room_defs.o(.room_def_08380cfc)
        KEEP(*(.room_data_08380d54))
        build/src/data/room_defs.o(.room_def_083811a8)
        KEEP(*(.room_data_08381200))
        build/src/data/room_defs.o(.room_def_08381ccc)
        KEEP(*(.room_data_08381d24))
        build/src/data/room_defs.o(.room_def_083821bc)
        KEEP(*(.room_data_08382214))
        build/src/data/room_defs.o(.room_def_083825d8)
        KEEP(*(.room_data_08382630))
        build/src/data/room_defs.o(.room_def_083828c4)
        KEEP(*(.room_data_0838291c))
        build/src/data/room_defs.o(.room_def_0838328c)
        KEEP(*(.room_data_083832e4))
        build/src/data/room_defs.o(.room_def_08383b28)
        KEEP(*(.room_data_08383b80))
        build/src/data/room_defs.o(.room_def_083840c0)
        KEEP(*(.room_data_08384118))
        build/src/data/room_defs.o(.room_def_083847b4)
        KEEP(*(.room_data_0838480c))
        build/src/data/room_defs.o(.room_def_08384adc)
        KEEP(*(.room_data_08384b34))
        build/src/data/room_defs.o(.room_def_08384cb0)
        KEEP(*(.room_data_08384d08))
        build/src/data/room_defs.o(.room_def_08384e3c)
        KEEP(*(.room_data_08384e94))
        build/src/data/room_defs.o(.room_def_08385964)
        KEEP(*(.room_data_083859bc))
        build/src/data/room_defs.o(.room_def_08385fcc)
        KEEP(*(.room_data_08386024))
        build/src/data/room_defs.o(.room_def_08386348)
        KEEP(*(.room_data_083863a0))
        build/src/data/room_defs.o(.room_def_08386700)
        KEEP(*(.room_data_08386758))
        build/src/data/room_defs.o(.room_def_08386bc0)
        KEEP(*(.room_data_08386c18))
        build/src/data/room_defs.o(.room_def_08386f80)
        KEEP(*(.room_data_08386fd8))
        build/src/data/room_defs.o(.room_def_08387308)
        KEEP(*(.room_data_08387360))
        build/src/data/room_defs.o(.room_def_08387498)
        KEEP(*(.room_data_083874f0))
        build/src/data/room_defs.o(.room_def_083879e8)
        KEEP(*(.room_data_08387a40))
        build/src/data/room_defs.o(.room_def_083881d4)
        KEEP(*(.room_data_0838822c))
        build/src/data/room_defs.o(.room_def_0838875c)
        KEEP(*(.room_data_083887b4))
        build/src/data/room_defs.o(.room_def_08388970)
        KEEP(*(.room_data_083889c8))
        build/src/data/room_defs.o(.room_def_08388bb8)
        KEEP(*(.room_data_08388c10))
        build/src/data/room_defs.o(.room_def_0838928c)
        KEEP(*(.room_data_083892e4))
        build/src/data/room_defs.o(.room_def_083893dc)
        KEEP(*(.room_data_08389434))
        build/src/data/room_defs.o(.room_def_08389a60)
        KEEP(*(.room_data_08389ab8))
        build/src/data/room_defs.o(.room_def_08389f2c)
        KEEP(*(.room_data_08389f84))
        build/src/data/room_defs.o(.room_def_0838a420)
        KEEP(*(.room_data_0838a478))
        build/src/data/room_defs.o(.room_def_0838a668)
        KEEP(*(.room_data_0838a6c0))
        build/src/data/room_defs.o(.room_def_0838a85c)
        KEEP(*(.room_data_0838a8b4))
        build/src/data/room_defs.o(.room_def_0838a9d0)
        KEEP(*(.room_data_0838aa28))
        build/src/data/room_defs.o(.room_def_0838af20)
        KEEP(*(.room_data_0838af78))
        build/src/data/room_defs.o(.room_def_0838b394)
        KEEP(*(.room_data_0838b3ec))
        build/src/data/room_defs.o(.room_def_0838b744)
        KEEP(*(.room_data_0838b79c))
        build/src/data/room_defs.o(.room_def_0838bb74)
        KEEP(*(.room_data_0838bbcc))
        build/src/data/room_defs.o(.room_def_0838bd4c)
        KEEP(*(.room_data_0838bda4))
        build/src/data/room_defs.o(.room_def_0838bf0c)
        KEEP(*(.room_data_0838bf64))
        build/src/data/room_defs.o(.room_def_0838c068)
        KEEP(*(.room_data_0838c0c0))
        build/src/data/room_defs.o(.room_def_0838c574)
        KEEP(*(.room_data_0838c5cc))
        build/src/data/room_defs.o(.room_def_0838cb58)
        KEEP(*(.room_data_0838cbb0))
        build/src/data/room_defs.o(.room_def_0838d190)
        KEEP(*(.room_data_0838d1e8))
        build/src/data/room_defs.o(.room_def_0838d7fc)
        KEEP(*(.room_data_0838d854))
        build/src/data/room_defs.o(.room_def_0838dbec)
        KEEP(*(.room_data_0838dc44))
        build/src/data/room_defs.o(.room_def_0838dd58)
        KEEP(*(.room_data_0838ddb0))
        build/src/data/room_defs.o(.room_def_0838dec4)
        KEEP(*(.room_data_0838df1c))
        build/src/data/room_defs.o(.room_def_0838e038)
        KEEP(*(.room_data_0838e090))
        build/src/data/room_defs.o(.room_def_0838e2c8)
        KEEP(*(.room_data_0838e320))
        build/src/data/room_defs.o(.room_def_0838e478)
        KEEP(*(.room_data_0838e4d0))
        build/src/data/room_defs.o(.room_def_0838e688)
        KEEP(*(.room_data_0838e6e0))
        build/src/data/room_defs.o(.room_def_0838e818)
        KEEP(*(.room_data_0838e870))
        build/src/data/room_defs.o(.room_def_0838eee4)
        KEEP(*(.room_data_0838ef3c))
        build/src/data/room_defs.o(.room_def_0838f788)
        KEEP(*(.room_data_0838f7e0))
        build/src/data/room_defs.o(.room_def_0838f914)
        KEEP(*(.room_data_0838f96c))
        build/src/data/room_defs.o(.room_def_0838fcf4)
        KEEP(*(.room_data_0838fd4c))
        build/src/data/room_defs.o(.room_def_083900ac)
        KEEP(*(.room_data_08390104))
        build/src/data/room_defs.o(.room_def_083902b0)
        KEEP(*(.room_data_08390308))
        build/src/data/room_defs.o(.room_def_0839059c)
        KEEP(*(.room_data_083905f4))
        build/src/data/room_defs.o(.room_def_08390ba8)
        KEEP(*(.room_data_08390c00))
        build/src/data/room_defs.o(.room_def_08391138)
        KEEP(*(.room_data_08391190))
        build/src/data/room_defs.o(.room_def_08391c40)
        KEEP(*(.room_data_08391c98))
        build/src/data/room_defs.o(.room_def_08391d9c)
        KEEP(*(.room_data_08391df4))
        build/src/data/room_defs.o(.room_def_08392308)
        KEEP(*(.room_data_08392360))
        build/src/data/room_defs.o(.room_def_08392778)
        KEEP(*(.room_data_083927d0))
        build/src/data/room_defs.o(.room_def_08392aa8)
        KEEP(*(.room_data_08392b00))
        build/src/data/room_defs.o(.room_def_08392f0c)
        KEEP(*(.room_data_08392f64))
        build/src/data/room_defs.o(.room_def_08393430)
        KEEP(*(.room_data_08393488))
        build/src/data/room_defs.o(.room_def_083935ac)
        KEEP(*(.room_data_08393604))
        build/src/data/room_defs.o(.room_def_08393760)
        KEEP(*(.room_data_083937b8))
        build/src/data/room_defs.o(.room_def_083938c0)
        KEEP(*(.room_data_08393918))
        build/src/data/room_defs.o(.room_def_08393a7c)
        KEEP(*(.room_data_08393ad4))
        build/src/data/room_defs.o(.room_def_08393c2c)
        KEEP(*(.room_data_08393c84))
        build/src/data/room_defs.o(.room_def_08393dc8)
        KEEP(*(.room_data_08393e20))
        build/src/data/room_defs.o(.room_def_08393ef4)
        KEEP(*(.room_data_08393f4c))
        build/src/data/room_defs.o(.room_def_08394458)
        KEEP(*(.room_data_083944b0))
        build/src/data/room_defs.o(.room_def_08394c58)
        KEEP(*(.room_data_08394cb0))
        build/src/data/room_defs.o(.room_def_083950f8)
        KEEP(*(.room_data_08395150))
        build/src/data/room_defs.o(.room_def_08395298)
        KEEP(*(.room_data_083952f0))
        build/src/data/room_defs.o(.room_def_08395450)
        KEEP(*(.room_data_083954a8))
        build/src/data/room_defs.o(.room_def_0839570c)
        KEEP(*(.room_data_08395764))
        build/src/data/room_defs.o(.room_def_083959c8)
        KEEP(*(.room_data_08395a20))
        build/src/data/room_defs.o(.room_def_08395c90)
        KEEP(*(.room_data_08395ce8))
        build/src/data/room_defs.o(.room_def_08395f34)
        KEEP(*(.room_data_08395f8c))
        build/src/data/room_defs.o(.room_def_083961e4)
        KEEP(*(.room_data_0839623c))
        build/src/data/room_defs.o(.room_def_083964ac)
        KEEP(*(.room_data_08396504))
        build/src/data/room_defs.o(.room_def_08396754)
        KEEP(*(.room_data_083967ac))
        build/src/data/room_defs.o(.room_def_08396a00)
        KEEP(*(.room_data_08396a58))
        build/src/data/room_defs.o(.room_def_08396cb8)
        KEEP(*(.room_data_08396d10))
        build/src/data/room_defs.o(.room_def_0839706c)
        KEEP(*(.room_data_083970c4))
        build/src/data/room_defs.o(.room_def_08397358)
        KEEP(*(.room_data_083973b0))
        build/src/data/room_defs.o(.room_def_08397508)
        KEEP(*(.room_data_08397560))
        build/src/data/room_defs.o(.room_def_08397680)
        KEEP(*(.room_data_083976d8))
        build/src/data/room_defs.o(.room_def_08397804)
        KEEP(*(.room_data_0839785c))
        build/src/data/room_defs.o(.room_def_08397970)
        KEEP(*(.room_data_083979c8))
        build/src/data/room_defs.o(.room_def_08397b54)
        KEEP(*(.room_data_08397bac))
        build/src/data/room_defs.o(.room_def_08397cfc)
        KEEP(*(.room_data_08397d54))
        build/src/data/room_defs.o(.room_def_08397ed0)
        KEEP(*(.room_data_08397f28))
        build/src/data/room_defs.o(.room_def_0839804c)
        KEEP(*(.room_data_083980a4))
        build/src/data/room_defs.o(.room_def_08398220)
        KEEP(*(.room_data_08398278))
        build/src/data/room_defs.o(.room_def_083983d8)
        KEEP(*(.room_data_08398430))
        build/src/data/room_defs.o(.room_def_083985d0)
        KEEP(*(.room_data_08398628))
        build/src/data/room_defs.o(.room_def_0839875c)
        KEEP(*(.room_data_083987b4))
        build/src/data/room_defs.o(.room_def_08398858)
        KEEP(*(.room_data_083988b0))
        build/src/data/room_defs.o(.room_def_08398ddc)
        KEEP(*(.room_data_08398e34))
        build/src/data/room_defs.o(.room_def_08399208)
        KEEP(*(.room_data_08399260))
        build/src/data/room_defs.o(.room_def_083995ec)
        KEEP(*(.room_data_08399644))
        build/src/data/room_defs.o(.room_def_08399cb8)
        KEEP(*(.room_data_08399d10))
        build/src/data/room_defs.o(.room_def_0839a214)
        KEEP(*(.room_data_0839a26c))
        build/src/data/room_defs.o(.room_def_0839a4c8)
        KEEP(*(.room_data_0839a520))
        build/src/data/room_defs.o(.room_def_0839a774)
        KEEP(*(.room_data_0839a7cc))
        build/src/data/room_defs.o(.room_def_0839aa40)
        KEEP(*(.room_data_0839aa98))
        build/src/data/room_defs.o(.room_def_0839ad18)
        KEEP(*(.room_data_0839ad70))
        build/src/data/room_defs.o(.room_def_0839b260)
        KEEP(*(.room_data_0839b2b8))
        build/src/data/room_defs.o(.room_def_0839bb28)
        KEEP(*(.room_data_0839bb80))
        build/src/data/room_defs.o(.room_def_0839bed4)
        KEEP(*(.room_data_0839bf2c))
        build/src/data/room_defs.o(.room_def_0839c0d0)
        KEEP(*(.room_data_0839c128))
        build/src/data/room_defs.o(.room_def_0839c57c)
        KEEP(*(.room_data_0839c5d4))
        build/src/data/room_defs.o(.room_def_0839cb34)
        KEEP(*(.room_data_0839cb8c))
        build/src/data/room_defs.o(.room_def_0839cd48)
        KEEP(*(.room_data_0839cda0))
        build/src/data/room_defs.o(.room_def_0839d254)
        KEEP(*(.room_data_0839d2ac))
        build/src/data/room_defs.o(.room_def_0839d5b4)
        KEEP(*(.room_data_0839d60c))
        build/src/data/room_defs.o(.room_def_0839d8bc)
        KEEP(*(.room_data_0839d914))
        build/src/data/room_defs.o(.room_def_0839dafc)
        KEEP(*(.room_data_0839db54))
        build/src/data/room_defs.o(.room_def_0839ded4)
        KEEP(*(.room_data_0839df2c))
        build/src/data/room_defs.o(.room_def_0839e190)
        KEEP(*(.room_data_0839e1e8))
        build/src/data/room_defs.o(.room_def_0839e590)
        KEEP(*(.room_data_0839e5e8))
        build/src/data/room_defs.o(.room_def_0839e944)
        KEEP(*(.room_data_0839e99c))
        build/src/data/room_defs.o(.room_def_0839ebb4)
        KEEP(*(.room_data_0839ec0c))
        build/src/data/room_defs.o(.room_def_0839ee14)
        KEEP(*(.room_data_0839ee6c))
        build/src/data/room_defs.o(.room_def_0839f3d8)
        KEEP(*(.room_data_0839f430))
        build/src/data/room_defs.o(.room_def_0839f684)
        KEEP(*(.room_data_0839f6dc))
        build/src/data/room_defs.o(.room_def_0839fc40)
        KEEP(*(.room_data_0839fc98))
        build/src/data/room_defs.o(.room_def_083a0058)
        KEEP(*(.room_data_083a00b0))
        build/src/data/room_defs.o(.room_def_083a0370)
        KEEP(*(.room_data_083a03c8))
        build/src/data/room_defs.o(.room_def_083a06dc)
        KEEP(*(.room_data_083a0734))
        build/src/data/room_defs.o(.room_def_083a0aa0)
        KEEP(*(.room_data_083a0af8))
        build/src/data/room_defs.o(.room_def_083a0d2c)
        KEEP(*(.room_data_083a0d84))
        build/src/data/room_defs.o(.room_def_083a0f00)
        KEEP(*(.room_data_083a0f58))
        build/src/data/room_defs.o(.room_def_083a142c)
        KEEP(*(.room_data_083a1484))
        build/src/data/room_defs.o(.room_def_083a1644)
        KEEP(*(.room_data_083a169c))
        build/src/data/room_defs.o(.room_def_083a17cc)
        KEEP(*(.room_data_083a1824))
        build/src/data/room_defs.o(.room_def_083a1b64)
        KEEP(*(.room_data_083a1bbc))
        build/src/data/room_defs.o(.room_def_083a1fd8)
        KEEP(*(.room_data_083a2030))
        build/src/data/room_defs.o(.room_def_083a24f0)
        KEEP(*(.room_data_083a2548))
        build/src/data/room_defs.o(.room_def_083a2578)
        KEEP(*(.room_data_083a25d0))
        build/src/data/room_defs.o(.room_def_083a2694)
        KEEP(*(.room_data_083a26ec))
        build/src/data/room_defs.o(.room_def_083a291c)
        KEEP(*(.room_data_083a2974))
        build/src/data/room_defs.o(.room_def_083a2ac4)
        KEEP(*(.room_data_083a2b1c))
        build/src/data/room_defs.o(.room_def_083a32bc)
        KEEP(*(.room_data_083a3314))
        build/src/data/room_defs.o(.room_def_083a3344)
        KEEP(*(.room_data_083a339c))
        build/src/data/room_defs.o(.room_def_083a34e4)
        KEEP(*(.room_data_083a353c))
        build/src/data/room_defs.o(.room_def_083a360c)
        KEEP(*(.room_data_083a3664))
        build/src/data/room_defs.o(.room_def_083a38fc)
        KEEP(*(.room_data_083a3954))
        build/src/data/room_defs.o(.room_def_083a3a74)
        KEEP(*(.room_data_083a3acc))
        build/src/data/room_defs.o(.room_def_083a3ff4)
        KEEP(*(.room_data_083a404c))
        build/src/data/room_defs.o(.room_def_083a407c)
        KEEP(*(.room_data_083a40d4))
        build/src/data/room_defs.o(.room_def_083a421c)
        KEEP(*(.room_data_083a4274))
        build/src/data/room_defs.o(.room_def_083a433c)
        KEEP(*(.room_data_083a4394))
        build/src/data/room_defs.o(.room_def_083a45dc)
        KEEP(*(.room_data_083a4634))
        build/src/data/room_defs.o(.room_def_083a46fc)
        KEEP(*(.room_data_083a4754))
        build/src/data/room_defs.o(.room_def_083a4d80)
        KEEP(*(.room_data_083a4dd8))
        build/src/data/room_defs.o(.room_def_083a4e08)
        KEEP(*(.room_data_083a4e60))
        build/src/data/room_defs.o(.room_def_083a4fa8)
        KEEP(*(.room_data_083a5000))
        build/src/data/room_defs.o(.room_def_083a50d0)
        KEEP(*(.room_data_083a5128))
        build/src/data/room_defs.o(.room_def_083a5354)
        KEEP(*(.room_data_083a53ac))
        build/src/data/room_defs.o(.room_def_083a5480)
        KEEP(*(.room_data_083a54d8))
        build/src/data/room_defs.o(.room_def_083a5be8)
        KEEP(*(.room_data_083a5c40))
        build/src/data/room_defs.o(.room_def_083a5c70)
        KEEP(*(.room_data_083a5cc8))
        build/src/data/room_defs.o(.room_def_083a5e10)
        KEEP(*(.room_data_083a5e68))
        build/src/data/room_defs.o(.room_def_083a5f3c)
        KEEP(*(.room_data_083a5f94))
        build/src/data/room_defs.o(.room_def_083a61e4)
        KEEP(*(.room_data_083a623c))
        build/src/data/room_defs.o(.room_def_083a633c)
        KEEP(*(.room_data_083a6394))
        build/src/data/room_defs.o(.room_def_083a6ae0)
        KEEP(*(.room_data_083a6b38))
        build/src/data/room_defs.o(.room_def_083a6b68)
        KEEP(*(.room_data_083a6bc0))
        build/src/data/room_defs.o(.room_def_083a6d08)
        KEEP(*(.room_data_083a6d60))
        build/src/data/room_defs.o(.room_def_083a6e24)
        KEEP(*(.room_data_083a6e7c))
        build/src/data/room_defs.o(.room_def_083a70a0)
        KEEP(*(.room_data_083a70f8))
        build/src/data/room_defs.o(.room_def_083a71e4)
        KEEP(*(.room_data_083a723c))
        build/src/data/room_defs.o(.room_def_083a77c4)
        KEEP(*(.room_data_083a781c))
        build/src/data/room_defs.o(.room_def_083a784c)
        KEEP(*(.room_data_083a78a4))
        build/src/data/room_defs.o(.room_def_083a7adc)
        KEEP(*(.room_data_083a7b34))
        build/src/data/room_defs.o(.room_def_083a7c28)
        KEEP(*(.room_data_083a7c80))
        build/src/data/room_defs.o(.room_def_083a8364)
        KEEP(*(.room_data_083a83bc))
        build/src/data/room_defs.o(.room_def_083a85d4)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_data) == 0x0835CFD4, "MATCHING: .room_data moved")
    ASSERT(!MATCHING || gLevel7Stage1Room0 == 0x0835D08C, "MATCHING: room_def_0835d08c moved")
    ASSERT(!MATCHING || room_data_0835d0e4 == 0x0835D0E4, "MATCHING: room_data_0835d0e4 moved")
    ASSERT(!MATCHING || gLevel7Stage1Room1 == 0x0835D300, "MATCHING: room_def_0835d300 moved")
    ASSERT(!MATCHING || room_data_0835d358 == 0x0835D358, "MATCHING: room_data_0835d358 moved")
    ASSERT(!MATCHING || gLevel7Stage1Room2 == 0x0835D4F0, "MATCHING: room_def_0835d4f0 moved")
    ASSERT(!MATCHING || room_data_0835d548 == 0x0835D548, "MATCHING: room_data_0835d548 moved")
    ASSERT(!MATCHING || gLevel0Stage0Room0 == 0x0835DDC0, "MATCHING: room_def_0835ddc0 moved")
    ASSERT(!MATCHING || room_data_0835de18 == 0x0835DE18, "MATCHING: room_data_0835de18 moved")
    ASSERT(!MATCHING || gLevel0Stage0Room1 == 0x0835E2A0, "MATCHING: room_def_0835e2a0 moved")
    ASSERT(!MATCHING || room_data_0835e2f8 == 0x0835E2F8, "MATCHING: room_data_0835e2f8 moved")
    ASSERT(!MATCHING || gLevel0Stage0Room2 == 0x0835E70C, "MATCHING: room_def_0835e70c moved")
    ASSERT(!MATCHING || room_data_0835e764 == 0x0835E764, "MATCHING: room_data_0835e764 moved")
    ASSERT(!MATCHING || gLevel0Stage0Room3 == 0x0835E924, "MATCHING: room_def_0835e924 moved")
    ASSERT(!MATCHING || room_data_0835e97c == 0x0835E97C, "MATCHING: room_data_0835e97c moved")
    ASSERT(!MATCHING || gLevel0Stage1Room0 == 0x0835EF1C, "MATCHING: room_def_0835ef1c moved")
    ASSERT(!MATCHING || room_data_0835ef74 == 0x0835EF74, "MATCHING: room_data_0835ef74 moved")
    ASSERT(!MATCHING || gLevel0Stage1Room1 == 0x0835F1A4, "MATCHING: room_def_0835f1a4 moved")
    ASSERT(!MATCHING || room_data_0835f1fc == 0x0835F1FC, "MATCHING: room_data_0835f1fc moved")
    ASSERT(!MATCHING || gLevel0Stage1Room2 == 0x0835F544, "MATCHING: room_def_0835f544 moved")
    ASSERT(!MATCHING || room_data_0835f59c == 0x0835F59C, "MATCHING: room_data_0835f59c moved")
    ASSERT(!MATCHING || gLevel0Stage1Room3 == 0x0835FC24, "MATCHING: room_def_0835fc24 moved")
    ASSERT(!MATCHING || room_data_0835fc7c == 0x0835FC7C, "MATCHING: room_data_0835fc7c moved")
    ASSERT(!MATCHING || gLevel0Stage1Room4 == 0x0835FFE4, "MATCHING: room_def_0835ffe4 moved")
    ASSERT(!MATCHING || room_data_0836003c == 0x0836003C, "MATCHING: room_data_0836003c moved")
    ASSERT(!MATCHING || gLevel0Stage1Room5 == 0x083601DC, "MATCHING: room_def_083601dc moved")
    ASSERT(!MATCHING || room_data_08360234 == 0x08360234, "MATCHING: room_data_08360234 moved")
    ASSERT(!MATCHING || gLevel0Stage2Room0 == 0x08360690, "MATCHING: room_def_08360690 moved")
    ASSERT(!MATCHING || room_data_083606e8 == 0x083606E8, "MATCHING: room_data_083606e8 moved")
    ASSERT(!MATCHING || gLevel0Stage2Room1 == 0x08360B38, "MATCHING: room_def_08360b38 moved")
    ASSERT(!MATCHING || room_data_08360b90 == 0x08360B90, "MATCHING: room_data_08360b90 moved")
    ASSERT(!MATCHING || gLevel0Stage2Room2 == 0x08360F38, "MATCHING: room_def_08360f38 moved")
    ASSERT(!MATCHING || room_data_08360f90 == 0x08360F90, "MATCHING: room_data_08360f90 moved")
    ASSERT(!MATCHING || gLevel0Stage2Room3 == 0x083613A0, "MATCHING: room_def_083613a0 moved")
    ASSERT(!MATCHING || room_data_083613f8 == 0x083613F8, "MATCHING: room_data_083613f8 moved")
    ASSERT(!MATCHING || gLevel0Stage3Room0 == 0x0836179C, "MATCHING: room_def_0836179c moved")
    ASSERT(!MATCHING || room_data_083617f4 == 0x083617F4, "MATCHING: room_data_083617f4 moved")
    ASSERT(!MATCHING || gLevel0Stage3Room1 == 0x08361CB8, "MATCHING: room_def_08361cb8 moved")
    ASSERT(!MATCHING || room_data_08361d10 == 0x08361D10, "MATCHING: room_data_08361d10 moved")
    ASSERT(!MATCHING || gLevel0Stage3Room2 == 0x08362104, "MATCHING: room_def_08362104 moved")
    ASSERT(!MATCHING || room_data_0836215c == 0x0836215C, "MATCHING: room_data_0836215c moved")
    ASSERT(!MATCHING || gLevel0Stage3Room3 == 0x08362788, "MATCHING: room_def_08362788 moved")
    ASSERT(!MATCHING || room_data_083627e0 == 0x083627E0, "MATCHING: room_data_083627e0 moved")
    ASSERT(!MATCHING || gLevel0Stage4Room0 == 0x08362AD0, "MATCHING: room_def_08362ad0 moved")
    ASSERT(!MATCHING || room_data_08362b28 == 0x08362B28, "MATCHING: room_data_08362b28 moved")
    ASSERT(!MATCHING || gLevel1Stage0Room0 == 0x083630A0, "MATCHING: room_def_083630a0 moved")
    ASSERT(!MATCHING || room_data_083630f8 == 0x083630F8, "MATCHING: room_data_083630f8 moved")
    ASSERT(!MATCHING || gLevel1Stage0Room1 == 0x083634E0, "MATCHING: room_def_083634e0 moved")
    ASSERT(!MATCHING || room_data_08363538 == 0x08363538, "MATCHING: room_data_08363538 moved")
    ASSERT(!MATCHING || gLevel1Stage0Room2 == 0x08363A0C, "MATCHING: room_def_08363a0c moved")
    ASSERT(!MATCHING || room_data_08363a64 == 0x08363A64, "MATCHING: room_data_08363a64 moved")
    ASSERT(!MATCHING || gLevel1Stage0Room3 == 0x08363E04, "MATCHING: room_def_08363e04 moved")
    ASSERT(!MATCHING || room_data_08363e5c == 0x08363E5C, "MATCHING: room_data_08363e5c moved")
    ASSERT(!MATCHING || gLevel1Stage1Room0 == 0x08364014, "MATCHING: room_def_08364014 moved")
    ASSERT(!MATCHING || room_data_0836406c == 0x0836406C, "MATCHING: room_data_0836406c moved")
    ASSERT(!MATCHING || gLevel1Stage1Room1 == 0x083643BC, "MATCHING: room_def_083643bc moved")
    ASSERT(!MATCHING || room_data_08364414 == 0x08364414, "MATCHING: room_data_08364414 moved")
    ASSERT(!MATCHING || gLevel1Stage1Room2 == 0x0836463C, "MATCHING: room_def_0836463c moved")
    ASSERT(!MATCHING || room_data_08364694 == 0x08364694, "MATCHING: room_data_08364694 moved")
    ASSERT(!MATCHING || gLevel1Stage1Room3 == 0x08364A38, "MATCHING: room_def_08364a38 moved")
    ASSERT(!MATCHING || room_data_08364a90 == 0x08364A90, "MATCHING: room_data_08364a90 moved")
    ASSERT(!MATCHING || gLevel1Stage1Room4 == 0x08365558, "MATCHING: room_def_08365558 moved")
    ASSERT(!MATCHING || room_data_083655b0 == 0x083655B0, "MATCHING: room_data_083655b0 moved")
    ASSERT(!MATCHING || gLevel1Stage2Room0 == 0x083657C0, "MATCHING: room_def_083657c0 moved")
    ASSERT(!MATCHING || room_data_08365818 == 0x08365818, "MATCHING: room_data_08365818 moved")
    ASSERT(!MATCHING || gLevel1Stage2Room1 == 0x08366340, "MATCHING: room_def_08366340 moved")
    ASSERT(!MATCHING || room_data_08366398 == 0x08366398, "MATCHING: room_data_08366398 moved")
    ASSERT(!MATCHING || gLevel1Stage2Room2 == 0x0836663C, "MATCHING: room_def_0836663c moved")
    ASSERT(!MATCHING || room_data_08366694 == 0x08366694, "MATCHING: room_data_08366694 moved")
    ASSERT(!MATCHING || gLevel1Stage2Room3 == 0x08366974, "MATCHING: room_def_08366974 moved")
    ASSERT(!MATCHING || room_data_083669cc == 0x083669CC, "MATCHING: room_data_083669cc moved")
    ASSERT(!MATCHING || gLevel1Stage2Room4 == 0x08366B98, "MATCHING: room_def_08366b98 moved")
    ASSERT(!MATCHING || room_data_08366bf0 == 0x08366BF0, "MATCHING: room_data_08366bf0 moved")
    ASSERT(!MATCHING || gLevel1Stage2Room5 == 0x08366D1C, "MATCHING: room_def_08366d1c moved")
    ASSERT(!MATCHING || room_data_08366d74 == 0x08366D74, "MATCHING: room_data_08366d74 moved")
    ASSERT(!MATCHING || gLevel1Stage3Room0 == 0x08367274, "MATCHING: room_def_08367274 moved")
    ASSERT(!MATCHING || room_data_083672cc == 0x083672CC, "MATCHING: room_data_083672cc moved")
    ASSERT(!MATCHING || gLevel1Stage3Room1 == 0x0836753C, "MATCHING: room_def_0836753c moved")
    ASSERT(!MATCHING || room_data_08367594 == 0x08367594, "MATCHING: room_data_08367594 moved")
    ASSERT(!MATCHING || gLevel1Stage3Room2 == 0x083677F4, "MATCHING: room_def_083677f4 moved")
    ASSERT(!MATCHING || room_data_0836784c == 0x0836784C, "MATCHING: room_data_0836784c moved")
    ASSERT(!MATCHING || gLevel1Stage3Room3 == 0x08367A50, "MATCHING: room_def_08367a50 moved")
    ASSERT(!MATCHING || room_data_08367aa8 == 0x08367AA8, "MATCHING: room_data_08367aa8 moved")
    ASSERT(!MATCHING || gLevel1Stage3Room4 == 0x08367E8C, "MATCHING: room_def_08367e8c moved")
    ASSERT(!MATCHING || room_data_08367ee4 == 0x08367EE4, "MATCHING: room_data_08367ee4 moved")
    ASSERT(!MATCHING || gLevel1Stage3Room5 == 0x083684D4, "MATCHING: room_def_083684d4 moved")
    ASSERT(!MATCHING || room_data_0836852c == 0x0836852C, "MATCHING: room_data_0836852c moved")
    ASSERT(!MATCHING || gLevel1Stage3Room6 == 0x08368710, "MATCHING: room_def_08368710 moved")
    ASSERT(!MATCHING || room_data_08368768 == 0x08368768, "MATCHING: room_data_08368768 moved")
    ASSERT(!MATCHING || gLevel1Stage4Room0 == 0x08368D30, "MATCHING: room_def_08368d30 moved")
    ASSERT(!MATCHING || room_data_08368d88 == 0x08368D88, "MATCHING: room_data_08368d88 moved")
    ASSERT(!MATCHING || gLevel1Stage4Room1 == 0x083692D8, "MATCHING: room_def_083692d8 moved")
    ASSERT(!MATCHING || room_data_08369330 == 0x08369330, "MATCHING: room_data_08369330 moved")
    ASSERT(!MATCHING || gLevel1Stage4Room2 == 0x08369BE0, "MATCHING: room_def_08369be0 moved")
    ASSERT(!MATCHING || room_data_08369c38 == 0x08369C38, "MATCHING: room_data_08369c38 moved")
    ASSERT(!MATCHING || gLevel1Stage4Room3 == 0x0836A0A8, "MATCHING: room_def_0836a0a8 moved")
    ASSERT(!MATCHING || room_data_0836a100 == 0x0836A100, "MATCHING: room_data_0836a100 moved")
    ASSERT(!MATCHING || gLevel1Stage4Room4 == 0x0836A318, "MATCHING: room_def_0836a318 moved")
    ASSERT(!MATCHING || room_data_0836a370 == 0x0836A370, "MATCHING: room_data_0836a370 moved")
    ASSERT(!MATCHING || gLevel1Stage4Room5 == 0x0836A534, "MATCHING: room_def_0836a534 moved")
    ASSERT(!MATCHING || room_data_0836a58c == 0x0836A58C, "MATCHING: room_data_0836a58c moved")
    ASSERT(!MATCHING || gLevel1Stage4Room6 == 0x0836A6DC, "MATCHING: room_def_0836a6dc moved")
    ASSERT(!MATCHING || room_data_0836a734 == 0x0836A734, "MATCHING: room_data_0836a734 moved")
    ASSERT(!MATCHING || gLevel1Stage5Room0 == 0x0836A89C, "MATCHING: room_def_0836a89c moved")
    ASSERT(!MATCHING || room_data_0836a8f4 == 0x0836A8F4, "MATCHING: room_data_0836a8f4 moved")
    ASSERT(!MATCHING || gLevel2Stage0Room0 == 0x0836AC58, "MATCHING: room_def_0836ac58 moved")
    ASSERT(!MATCHING || room_data_0836acb0 == 0x0836ACB0, "MATCHING: room_data_0836acb0 moved")
    ASSERT(!MATCHING || gLevel2Stage0Room1 == 0x0836AEE4, "MATCHING: room_def_0836aee4 moved")
    ASSERT(!MATCHING || room_data_0836af3c == 0x0836AF3C, "MATCHING: room_data_0836af3c moved")
    ASSERT(!MATCHING || gLevel2Stage0Room2 == 0x0836B160, "MATCHING: room_def_0836b160 moved")
    ASSERT(!MATCHING || room_data_0836b1b8 == 0x0836B1B8, "MATCHING: room_data_0836b1b8 moved")
    ASSERT(!MATCHING || gLevel2Stage0Room3 == 0x0836B44C, "MATCHING: room_def_0836b44c moved")
    ASSERT(!MATCHING || room_data_0836b4a4 == 0x0836B4A4, "MATCHING: room_data_0836b4a4 moved")
    ASSERT(!MATCHING || gLevel2Stage0Room4 == 0x0836B7B4, "MATCHING: room_def_0836b7b4 moved")
    ASSERT(!MATCHING || room_data_0836b80c == 0x0836B80C, "MATCHING: room_data_0836b80c moved")
    ASSERT(!MATCHING || gLevel2Stage0Room5 == 0x0836B968, "MATCHING: room_def_0836b968 moved")
    ASSERT(!MATCHING || room_data_0836b9c0 == 0x0836B9C0, "MATCHING: room_data_0836b9c0 moved")
    ASSERT(!MATCHING || gLevel2Stage1Room0 == 0x0836BC70, "MATCHING: room_def_0836bc70 moved")
    ASSERT(!MATCHING || room_data_0836bcc8 == 0x0836BCC8, "MATCHING: room_data_0836bcc8 moved")
    ASSERT(!MATCHING || gLevel2Stage1Room1 == 0x0836C0B0, "MATCHING: room_def_0836c0b0 moved")
    ASSERT(!MATCHING || room_data_0836c108 == 0x0836C108, "MATCHING: room_data_0836c108 moved")
    ASSERT(!MATCHING || gLevel2Stage1Room2 == 0x0836C50C, "MATCHING: room_def_0836c50c moved")
    ASSERT(!MATCHING || room_data_0836c564 == 0x0836C564, "MATCHING: room_data_0836c564 moved")
    ASSERT(!MATCHING || gLevel2Stage1Room3 == 0x0836CA98, "MATCHING: room_def_0836ca98 moved")
    ASSERT(!MATCHING || room_data_0836caf0 == 0x0836CAF0, "MATCHING: room_data_0836caf0 moved")
    ASSERT(!MATCHING || gLevel2Stage1Room4 == 0x0836CDA8, "MATCHING: room_def_0836cda8 moved")
    ASSERT(!MATCHING || room_data_0836ce00 == 0x0836CE00, "MATCHING: room_data_0836ce00 moved")
    ASSERT(!MATCHING || gLevel2Stage1Room5 == 0x0836D19C, "MATCHING: room_def_0836d19c moved")
    ASSERT(!MATCHING || room_data_0836d1f4 == 0x0836D1F4, "MATCHING: room_data_0836d1f4 moved")
    ASSERT(!MATCHING || gLevel2Stage1Room6 == 0x0836D334, "MATCHING: room_def_0836d334 moved")
    ASSERT(!MATCHING || room_data_0836d38c == 0x0836D38C, "MATCHING: room_data_0836d38c moved")
    ASSERT(!MATCHING || gLevel2Stage1Room7 == 0x0836D4FC, "MATCHING: room_def_0836d4fc moved")
    ASSERT(!MATCHING || room_data_0836d554 == 0x0836D554, "MATCHING: room_data_0836d554 moved")
    ASSERT(!MATCHING || gLevel2Stage2Room0 == 0x0836DADC, "MATCHING: room_def_0836dadc moved")
    ASSERT(!MATCHING || room_data_0836db34 == 0x0836DB34, "MATCHING: room_data_0836db34 moved")
    ASSERT(!MATCHING || gLevel2Stage2Room1 == 0x0836DD44, "MATCHING: room_def_0836dd44 moved")
    ASSERT(!MATCHING || room_data_0836dd9c == 0x0836DD9C, "MATCHING: room_data_0836dd9c moved")
    ASSERT(!MATCHING || gLevel2Stage2Room2 == 0x0836E09C, "MATCHING: room_def_0836e09c moved")
    ASSERT(!MATCHING || room_data_0836e0f4 == 0x0836E0F4, "MATCHING: room_data_0836e0f4 moved")
    ASSERT(!MATCHING || gLevel2Stage2Room3 == 0x0836E3F4, "MATCHING: room_def_0836e3f4 moved")
    ASSERT(!MATCHING || room_data_0836e44c == 0x0836E44C, "MATCHING: room_data_0836e44c moved")
    ASSERT(!MATCHING || gLevel2Stage2Room4 == 0x0836E660, "MATCHING: room_def_0836e660 moved")
    ASSERT(!MATCHING || room_data_0836e6b8 == 0x0836E6B8, "MATCHING: room_data_0836e6b8 moved")
    ASSERT(!MATCHING || gLevel2Stage3Room0 == 0x0836E824, "MATCHING: room_def_0836e824 moved")
    ASSERT(!MATCHING || room_data_0836e87c == 0x0836E87C, "MATCHING: room_data_0836e87c moved")
    ASSERT(!MATCHING || gLevel2Stage3Room1 == 0x0836EED8, "MATCHING: room_def_0836eed8 moved")
    ASSERT(!MATCHING || room_data_0836ef30 == 0x0836EF30, "MATCHING: room_data_0836ef30 moved")
    ASSERT(!MATCHING || gLevel2Stage3Room2 == 0x0836F0A4, "MATCHING: room_def_0836f0a4 moved")
    ASSERT(!MATCHING || room_data_0836f0fc == 0x0836F0FC, "MATCHING: room_data_0836f0fc moved")
    ASSERT(!MATCHING || gLevel2Stage3Room3 == 0x0836F270, "MATCHING: room_def_0836f270 moved")
    ASSERT(!MATCHING || room_data_0836f2c8 == 0x0836F2C8, "MATCHING: room_data_0836f2c8 moved")
    ASSERT(!MATCHING || gLevel2Stage3Room4 == 0x0836F634, "MATCHING: room_def_0836f634 moved")
    ASSERT(!MATCHING || room_data_0836f68c == 0x0836F68C, "MATCHING: room_data_0836f68c moved")
    ASSERT(!MATCHING || gLevel2Stage3Room5 == 0x0836F7E4, "MATCHING: room_def_0836f7e4 moved")
    ASSERT(!MATCHING || room_data_0836f83c == 0x0836F83C, "MATCHING: room_data_0836f83c moved")
    ASSERT(!MATCHING || gLevel2Stage4Room0 == 0x0836F9B0, "MATCHING: room_def_0836f9b0 moved")
    ASSERT(!MATCHING || room_data_0836fa08 == 0x0836FA08, "MATCHING: room_data_0836fa08 moved")
    ASSERT(!MATCHING || gLevel2Stage4Room1 == 0x0836FD44, "MATCHING: room_def_0836fd44 moved")
    ASSERT(!MATCHING || room_data_0836fd9c == 0x0836FD9C, "MATCHING: room_data_0836fd9c moved")
    ASSERT(!MATCHING || gLevel2Stage4Room2 == 0x08370000, "MATCHING: room_def_08370000 moved")
    ASSERT(!MATCHING || room_data_08370058 == 0x08370058, "MATCHING: room_data_08370058 moved")
    ASSERT(!MATCHING || gLevel2Stage4Room3 == 0x083702FC, "MATCHING: room_def_083702fc moved")
    ASSERT(!MATCHING || room_data_08370354 == 0x08370354, "MATCHING: room_data_08370354 moved")
    ASSERT(!MATCHING || gLevel2Stage4Room4 == 0x083704E4, "MATCHING: room_def_083704e4 moved")
    ASSERT(!MATCHING || room_data_0837053c == 0x0837053C, "MATCHING: room_data_0837053c moved")
    ASSERT(!MATCHING || gLevel2Stage5Room0 == 0x08370684, "MATCHING: room_def_08370684 moved")
    ASSERT(!MATCHING || room_data_083706dc == 0x083706DC, "MATCHING: room_data_083706dc moved")
    ASSERT(!MATCHING || gLevel2Stage5Room1 == 0x08370AF4, "MATCHING: room_def_08370af4 moved")
    ASSERT(!MATCHING || room_data_08370b4c == 0x08370B4C, "MATCHING: room_data_08370b4c moved")
    ASSERT(!MATCHING || gLevel2Stage5Room2 == 0x08370E08, "MATCHING: room_def_08370e08 moved")
    ASSERT(!MATCHING || room_data_08370e60 == 0x08370E60, "MATCHING: room_data_08370e60 moved")
    ASSERT(!MATCHING || gLevel2Stage5Room3 == 0x0837108C, "MATCHING: room_def_0837108c moved")
    ASSERT(!MATCHING || room_data_083710e4 == 0x083710E4, "MATCHING: room_data_083710e4 moved")
    ASSERT(!MATCHING || gLevel2Stage5Room4 == 0x08371388, "MATCHING: room_def_08371388 moved")
    ASSERT(!MATCHING || room_data_083713e0 == 0x083713E0, "MATCHING: room_data_083713e0 moved")
    ASSERT(!MATCHING || gLevel2Stage5Room5 == 0x08371728, "MATCHING: room_def_08371728 moved")
    ASSERT(!MATCHING || room_data_08371780 == 0x08371780, "MATCHING: room_data_08371780 moved")
    ASSERT(!MATCHING || gLevel2Stage5Room6 == 0x0837181C, "MATCHING: room_def_0837181c moved")
    ASSERT(!MATCHING || room_data_08371874 == 0x08371874, "MATCHING: room_data_08371874 moved")
    ASSERT(!MATCHING || gLevel2Stage5Room7 == 0x08371D90, "MATCHING: room_def_08371d90 moved")
    ASSERT(!MATCHING || room_data_08371de8 == 0x08371DE8, "MATCHING: room_data_08371de8 moved")
    ASSERT(!MATCHING || gLevel2Stage5Room8 == 0x08372040, "MATCHING: room_def_08372040 moved")
    ASSERT(!MATCHING || room_data_08372098 == 0x08372098, "MATCHING: room_data_08372098 moved")
    ASSERT(!MATCHING || gLevel2Stage5Room9 == 0x08372240, "MATCHING: room_def_08372240 moved")
    ASSERT(!MATCHING || room_data_08372298 == 0x08372298, "MATCHING: room_data_08372298 moved")
    ASSERT(!MATCHING || gLevel2Stage6Room0 == 0x083723B0, "MATCHING: room_def_083723b0 moved")
    ASSERT(!MATCHING || room_data_08372408 == 0x08372408, "MATCHING: room_data_08372408 moved")
    ASSERT(!MATCHING || gLevel2Stage6Room1 == 0x08372518, "MATCHING: room_def_08372518 moved")
    ASSERT(!MATCHING || room_data_08372570 == 0x08372570, "MATCHING: room_data_08372570 moved")
    ASSERT(!MATCHING || gLevel2Stage6Room2 == 0x08372680, "MATCHING: room_def_08372680 moved")
    ASSERT(!MATCHING || room_data_083726d8 == 0x083726D8, "MATCHING: room_data_083726d8 moved")
    ASSERT(!MATCHING || gLevel3Stage0Room0 == 0x08372A78, "MATCHING: room_def_08372a78 moved")
    ASSERT(!MATCHING || room_data_08372ad0 == 0x08372AD0, "MATCHING: room_data_08372ad0 moved")
    ASSERT(!MATCHING || gLevel3Stage0Room1 == 0x08372D58, "MATCHING: room_def_08372d58 moved")
    ASSERT(!MATCHING || room_data_08372db0 == 0x08372DB0, "MATCHING: room_data_08372db0 moved")
    ASSERT(!MATCHING || gLevel3Stage0Room2 == 0x0837301C, "MATCHING: room_def_0837301c moved")
    ASSERT(!MATCHING || room_data_08373074 == 0x08373074, "MATCHING: room_data_08373074 moved")
    ASSERT(!MATCHING || gLevel3Stage0Room3 == 0x083736A8, "MATCHING: room_def_083736a8 moved")
    ASSERT(!MATCHING || room_data_08373700 == 0x08373700, "MATCHING: room_data_08373700 moved")
    ASSERT(!MATCHING || gLevel3Stage0Room4 == 0x08373914, "MATCHING: room_def_08373914 moved")
    ASSERT(!MATCHING || room_data_0837396c == 0x0837396C, "MATCHING: room_data_0837396c moved")
    ASSERT(!MATCHING || gLevel3Stage0Room5 == 0x08373AC4, "MATCHING: room_def_08373ac4 moved")
    ASSERT(!MATCHING || room_data_08373b1c == 0x08373B1C, "MATCHING: room_data_08373b1c moved")
    ASSERT(!MATCHING || gLevel3Stage1Room0 == 0x0837410C, "MATCHING: room_def_0837410c moved")
    ASSERT(!MATCHING || room_data_08374164 == 0x08374164, "MATCHING: room_data_08374164 moved")
    ASSERT(!MATCHING || gLevel3Stage1Room1 == 0x083742CC, "MATCHING: room_def_083742cc moved")
    ASSERT(!MATCHING || room_data_08374324 == 0x08374324, "MATCHING: room_data_08374324 moved")
    ASSERT(!MATCHING || gLevel3Stage1Room2 == 0x083747D0, "MATCHING: room_def_083747d0 moved")
    ASSERT(!MATCHING || room_data_08374828 == 0x08374828, "MATCHING: room_data_08374828 moved")
    ASSERT(!MATCHING || gLevel3Stage1Room3 == 0x08374B28, "MATCHING: room_def_08374b28 moved")
    ASSERT(!MATCHING || room_data_08374b80 == 0x08374B80, "MATCHING: room_data_08374b80 moved")
    ASSERT(!MATCHING || gLevel3Stage1Room4 == 0x08374C74, "MATCHING: room_def_08374c74 moved")
    ASSERT(!MATCHING || room_data_08374ccc == 0x08374CCC, "MATCHING: room_data_08374ccc moved")
    ASSERT(!MATCHING || gLevel3Stage1Room5 == 0x08374DBC, "MATCHING: room_def_08374dbc moved")
    ASSERT(!MATCHING || room_data_08374e14 == 0x08374E14, "MATCHING: room_data_08374e14 moved")
    ASSERT(!MATCHING || gLevel3Stage2Room0 == 0x0837531C, "MATCHING: room_def_0837531c moved")
    ASSERT(!MATCHING || room_data_08375374 == 0x08375374, "MATCHING: room_data_08375374 moved")
    ASSERT(!MATCHING || gLevel3Stage2Room1 == 0x083761D0, "MATCHING: room_def_083761d0 moved")
    ASSERT(!MATCHING || room_data_08376228 == 0x08376228, "MATCHING: room_data_08376228 moved")
    ASSERT(!MATCHING || gLevel3Stage2Room2 == 0x08376B24, "MATCHING: room_def_08376b24 moved")
    ASSERT(!MATCHING || room_data_08376b7c == 0x08376B7C, "MATCHING: room_data_08376b7c moved")
    ASSERT(!MATCHING || gLevel3Stage2Room3 == 0x08376D94, "MATCHING: room_def_08376d94 moved")
    ASSERT(!MATCHING || room_data_08376dec == 0x08376DEC, "MATCHING: room_data_08376dec moved")
    ASSERT(!MATCHING || gLevel3Stage2Room4 == 0x08376F18, "MATCHING: room_def_08376f18 moved")
    ASSERT(!MATCHING || room_data_08376f70 == 0x08376F70, "MATCHING: room_data_08376f70 moved")
    ASSERT(!MATCHING || gLevel3Stage2Room5 == 0x08377144, "MATCHING: room_def_08377144 moved")
    ASSERT(!MATCHING || room_data_0837719c == 0x0837719C, "MATCHING: room_data_0837719c moved")
    ASSERT(!MATCHING || gLevel3Stage2Room6 == 0x08377334, "MATCHING: room_def_08377334 moved")
    ASSERT(!MATCHING || room_data_0837738c == 0x0837738C, "MATCHING: room_data_0837738c moved")
    ASSERT(!MATCHING || gLevel3Stage3Room0 == 0x08377AF0, "MATCHING: room_def_08377af0 moved")
    ASSERT(!MATCHING || room_data_08377b48 == 0x08377B48, "MATCHING: room_data_08377b48 moved")
    ASSERT(!MATCHING || gLevel3Stage3Room1 == 0x08377FBC, "MATCHING: room_def_08377fbc moved")
    ASSERT(!MATCHING || room_data_08378014 == 0x08378014, "MATCHING: room_data_08378014 moved")
    ASSERT(!MATCHING || gLevel3Stage3Room2 == 0x08378A80, "MATCHING: room_def_08378a80 moved")
    ASSERT(!MATCHING || room_data_08378ad8 == 0x08378AD8, "MATCHING: room_data_08378ad8 moved")
    ASSERT(!MATCHING || gLevel3Stage3Room3 == 0x08378EB0, "MATCHING: room_def_08378eb0 moved")
    ASSERT(!MATCHING || room_data_08378f08 == 0x08378F08, "MATCHING: room_data_08378f08 moved")
    ASSERT(!MATCHING || gLevel3Stage3Room4 == 0x08379218, "MATCHING: room_def_08379218 moved")
    ASSERT(!MATCHING || room_data_08379270 == 0x08379270, "MATCHING: room_data_08379270 moved")
    ASSERT(!MATCHING || gLevel3Stage4Room0 == 0x08379B84, "MATCHING: room_def_08379b84 moved")
    ASSERT(!MATCHING || room_data_08379bdc == 0x08379BDC, "MATCHING: room_data_08379bdc moved")
    ASSERT(!MATCHING || gLevel3Stage4Room1 == 0x08379E60, "MATCHING: room_def_08379e60 moved")
    ASSERT(!MATCHING || room_data_08379eb8 == 0x08379EB8, "MATCHING: room_data_08379eb8 moved")
    ASSERT(!MATCHING || gLevel3Stage4Room2 == 0x0837A0CC, "MATCHING: room_def_0837a0cc moved")
    ASSERT(!MATCHING || room_data_0837a124 == 0x0837A124, "MATCHING: room_data_0837a124 moved")
    ASSERT(!MATCHING || gLevel3Stage4Room3 == 0x0837A52C, "MATCHING: room_def_0837a52c moved")
    ASSERT(!MATCHING || room_data_0837a584 == 0x0837A584, "MATCHING: room_data_0837a584 moved")
    ASSERT(!MATCHING || gLevel3Stage4Room4 == 0x0837AB88, "MATCHING: room_def_0837ab88 moved")
    ASSERT(!MATCHING || room_data_0837abe0 == 0x0837ABE0, "MATCHING: room_data_0837abe0 moved")
    ASSERT(!MATCHING || gLevel3Stage4Room5 == 0x0837AD38, "MATCHING: room_def_0837ad38 moved")
    ASSERT(!MATCHING || room_data_0837ad90 == 0x0837AD90, "MATCHING: room_data_0837ad90 moved")
    ASSERT(!MATCHING || gLevel3Stage4Room6 == 0x0837AF10, "MATCHING: room_def_0837af10 moved")
    ASSERT(!MATCHING || room_data_0837af68 == 0x0837AF68, "MATCHING: room_data_0837af68 moved")
    ASSERT(!MATCHING || gLevel3Stage5Room0 == 0x0837B7CC, "MATCHING: room_def_0837b7cc moved")
    ASSERT(!MATCHING || room_data_0837b824 == 0x0837B824, "MATCHING: room_data_0837b824 moved")
    ASSERT(!MATCHING || gLevel3Stage5Room1 == 0x0837BB9C, "MATCHING: room_def_0837bb9c moved")
    ASSERT(!MATCHING || room_data_0837bbf4 == 0x0837BBF4, "MATCHING: room_data_0837bbf4 moved")
    ASSERT(!MATCHING || gLevel3Stage5Room2 == 0x0837BC90, "MATCHING: room_def_0837bc90 moved")
    ASSERT(!MATCHING || room_data_0837bce8 == 0x0837BCE8, "MATCHING: room_data_0837bce8 moved")
    ASSERT(!MATCHING || gLevel3Stage5Room3 == 0x0837BFA8, "MATCHING: room_def_0837bfa8 moved")
    ASSERT(!MATCHING || room_data_0837c000 == 0x0837C000, "MATCHING: room_data_0837c000 moved")
    ASSERT(!MATCHING || gLevel3Stage5Room4 == 0x0837C288, "MATCHING: room_def_0837c288 moved")
    ASSERT(!MATCHING || room_data_0837c2e0 == 0x0837C2E0, "MATCHING: room_data_0837c2e0 moved")
    ASSERT(!MATCHING || gLevel3Stage5Room5 == 0x0837C424, "MATCHING: room_def_0837c424 moved")
    ASSERT(!MATCHING || room_data_0837c47c == 0x0837C47C, "MATCHING: room_data_0837c47c moved")
    ASSERT(!MATCHING || gLevel3Stage5Room6 == 0x0837C520, "MATCHING: room_def_0837c520 moved")
    ASSERT(!MATCHING || room_data_0837c578 == 0x0837C578, "MATCHING: room_data_0837c578 moved")
    ASSERT(!MATCHING || gLevel3Stage5Room7 == 0x0837C698, "MATCHING: room_def_0837c698 moved")
    ASSERT(!MATCHING || room_data_0837c6f0 == 0x0837C6F0, "MATCHING: room_data_0837c6f0 moved")
    ASSERT(!MATCHING || gLevel3Stage6Room0 == 0x0837D968, "MATCHING: room_def_0837d968 moved")
    ASSERT(!MATCHING || room_data_0837d9c0 == 0x0837D9C0, "MATCHING: room_data_0837d9c0 moved")
    ASSERT(!MATCHING || gLevel4Stage0Room0 == 0x0837DE3C, "MATCHING: room_def_0837de3c moved")
    ASSERT(!MATCHING || room_data_0837de94 == 0x0837DE94, "MATCHING: room_data_0837de94 moved")
    ASSERT(!MATCHING || gLevel4Stage0Room1 == 0x0837E598, "MATCHING: room_def_0837e598 moved")
    ASSERT(!MATCHING || room_data_0837e5f0 == 0x0837E5F0, "MATCHING: room_data_0837e5f0 moved")
    ASSERT(!MATCHING || gLevel4Stage0Room2 == 0x0837EB34, "MATCHING: room_def_0837eb34 moved")
    ASSERT(!MATCHING || room_data_0837eb8c == 0x0837EB8C, "MATCHING: room_data_0837eb8c moved")
    ASSERT(!MATCHING || gLevel4Stage0Room3 == 0x0837ECA8, "MATCHING: room_def_0837eca8 moved")
    ASSERT(!MATCHING || room_data_0837ed00 == 0x0837ED00, "MATCHING: room_data_0837ed00 moved")
    ASSERT(!MATCHING || gLevel4Stage0Room4 == 0x0837EE60, "MATCHING: room_def_0837ee60 moved")
    ASSERT(!MATCHING || room_data_0837eeb8 == 0x0837EEB8, "MATCHING: room_data_0837eeb8 moved")
    ASSERT(!MATCHING || gLevel4Stage1Room0 == 0x0837F028, "MATCHING: room_def_0837f028 moved")
    ASSERT(!MATCHING || room_data_0837f080 == 0x0837F080, "MATCHING: room_data_0837f080 moved")
    ASSERT(!MATCHING || gLevel4Stage1Room1 == 0x0837F73C, "MATCHING: room_def_0837f73c moved")
    ASSERT(!MATCHING || room_data_0837f794 == 0x0837F794, "MATCHING: room_data_0837f794 moved")
    ASSERT(!MATCHING || gLevel4Stage1Room2 == 0x0837FA44, "MATCHING: room_def_0837fa44 moved")
    ASSERT(!MATCHING || room_data_0837fa9c == 0x0837FA9C, "MATCHING: room_data_0837fa9c moved")
    ASSERT(!MATCHING || gLevel4Stage1Room3 == 0x0837FDC8, "MATCHING: room_def_0837fdc8 moved")
    ASSERT(!MATCHING || room_data_0837fe20 == 0x0837FE20, "MATCHING: room_data_0837fe20 moved")
    ASSERT(!MATCHING || gLevel4Stage1Room4 == 0x083800D0, "MATCHING: room_def_083800d0 moved")
    ASSERT(!MATCHING || room_data_08380128 == 0x08380128, "MATCHING: room_data_08380128 moved")
    ASSERT(!MATCHING || gLevel4Stage1Room5 == 0x083805BC, "MATCHING: room_def_083805bc moved")
    ASSERT(!MATCHING || room_data_08380614 == 0x08380614, "MATCHING: room_data_08380614 moved")
    ASSERT(!MATCHING || gLevel4Stage1Room6 == 0x08380758, "MATCHING: room_def_08380758 moved")
    ASSERT(!MATCHING || room_data_083807b0 == 0x083807B0, "MATCHING: room_data_083807b0 moved")
    ASSERT(!MATCHING || gLevel4Stage2Room0 == 0x08380CFC, "MATCHING: room_def_08380cfc moved")
    ASSERT(!MATCHING || room_data_08380d54 == 0x08380D54, "MATCHING: room_data_08380d54 moved")
    ASSERT(!MATCHING || gLevel4Stage2Room1 == 0x083811A8, "MATCHING: room_def_083811a8 moved")
    ASSERT(!MATCHING || room_data_08381200 == 0x08381200, "MATCHING: room_data_08381200 moved")
    ASSERT(!MATCHING || gLevel4Stage2Room2 == 0x08381CCC, "MATCHING: room_def_08381ccc moved")
    ASSERT(!MATCHING || room_data_08381d24 == 0x08381D24, "MATCHING: room_data_08381d24 moved")
    ASSERT(!MATCHING || gLevel4Stage2Room3 == 0x083821BC, "MATCHING: room_def_083821bc moved")
    ASSERT(!MATCHING || room_data_08382214 == 0x08382214, "MATCHING: room_data_08382214 moved")
    ASSERT(!MATCHING || gLevel4Stage2Room4 == 0x083825D8, "MATCHING: room_def_083825d8 moved")
    ASSERT(!MATCHING || room_data_08382630 == 0x08382630, "MATCHING: room_data_08382630 moved")
    ASSERT(!MATCHING || gLevel4Stage2Room5 == 0x083828C4, "MATCHING: room_def_083828c4 moved")
    ASSERT(!MATCHING || room_data_0838291c == 0x0838291C, "MATCHING: room_data_0838291c moved")
    ASSERT(!MATCHING || gLevel4Stage3Room0 == 0x0838328C, "MATCHING: room_def_0838328c moved")
    ASSERT(!MATCHING || room_data_083832e4 == 0x083832E4, "MATCHING: room_data_083832e4 moved")
    ASSERT(!MATCHING || gLevel4Stage3Room1 == 0x08383B28, "MATCHING: room_def_08383b28 moved")
    ASSERT(!MATCHING || room_data_08383b80 == 0x08383B80, "MATCHING: room_data_08383b80 moved")
    ASSERT(!MATCHING || gLevel4Stage3Room2 == 0x083840C0, "MATCHING: room_def_083840c0 moved")
    ASSERT(!MATCHING || room_data_08384118 == 0x08384118, "MATCHING: room_data_08384118 moved")
    ASSERT(!MATCHING || gLevel4Stage3Room3 == 0x083847B4, "MATCHING: room_def_083847b4 moved")
    ASSERT(!MATCHING || room_data_0838480c == 0x0838480C, "MATCHING: room_data_0838480c moved")
    ASSERT(!MATCHING || gLevel4Stage3Room4 == 0x08384ADC, "MATCHING: room_def_08384adc moved")
    ASSERT(!MATCHING || room_data_08384b34 == 0x08384B34, "MATCHING: room_data_08384b34 moved")
    ASSERT(!MATCHING || gLevel4Stage3Room5 == 0x08384CB0, "MATCHING: room_def_08384cb0 moved")
    ASSERT(!MATCHING || room_data_08384d08 == 0x08384D08, "MATCHING: room_data_08384d08 moved")
    ASSERT(!MATCHING || gLevel4Stage3Room6 == 0x08384E3C, "MATCHING: room_def_08384e3c moved")
    ASSERT(!MATCHING || room_data_08384e94 == 0x08384E94, "MATCHING: room_data_08384e94 moved")
    ASSERT(!MATCHING || gLevel4Stage4Room0 == 0x08385964, "MATCHING: room_def_08385964 moved")
    ASSERT(!MATCHING || room_data_083859bc == 0x083859BC, "MATCHING: room_data_083859bc moved")
    ASSERT(!MATCHING || gLevel4Stage4Room1 == 0x08385FCC, "MATCHING: room_def_08385fcc moved")
    ASSERT(!MATCHING || room_data_08386024 == 0x08386024, "MATCHING: room_data_08386024 moved")
    ASSERT(!MATCHING || gLevel4Stage4Room2 == 0x08386348, "MATCHING: room_def_08386348 moved")
    ASSERT(!MATCHING || room_data_083863a0 == 0x083863A0, "MATCHING: room_data_083863a0 moved")
    ASSERT(!MATCHING || gLevel4Stage4Room3 == 0x08386700, "MATCHING: room_def_08386700 moved")
    ASSERT(!MATCHING || room_data_08386758 == 0x08386758, "MATCHING: room_data_08386758 moved")
    ASSERT(!MATCHING || gLevel4Stage4Room4 == 0x08386BC0, "MATCHING: room_def_08386bc0 moved")
    ASSERT(!MATCHING || room_data_08386c18 == 0x08386C18, "MATCHING: room_data_08386c18 moved")
    ASSERT(!MATCHING || gLevel4Stage4Room5 == 0x08386F80, "MATCHING: room_def_08386f80 moved")
    ASSERT(!MATCHING || room_data_08386fd8 == 0x08386FD8, "MATCHING: room_data_08386fd8 moved")
    ASSERT(!MATCHING || gLevel4Stage4Room6 == 0x08387308, "MATCHING: room_def_08387308 moved")
    ASSERT(!MATCHING || room_data_08387360 == 0x08387360, "MATCHING: room_data_08387360 moved")
    ASSERT(!MATCHING || gLevel4Stage4Room7 == 0x08387498, "MATCHING: room_def_08387498 moved")
    ASSERT(!MATCHING || room_data_083874f0 == 0x083874F0, "MATCHING: room_data_083874f0 moved")
    ASSERT(!MATCHING || gLevel4Stage5Room0 == 0x083879E8, "MATCHING: room_def_083879e8 moved")
    ASSERT(!MATCHING || room_data_08387a40 == 0x08387A40, "MATCHING: room_data_08387a40 moved")
    ASSERT(!MATCHING || gLevel4Stage5Room1 == 0x083881D4, "MATCHING: room_def_083881d4 moved")
    ASSERT(!MATCHING || room_data_0838822c == 0x0838822C, "MATCHING: room_data_0838822c moved")
    ASSERT(!MATCHING || gLevel4Stage5Room2 == 0x0838875C, "MATCHING: room_def_0838875c moved")
    ASSERT(!MATCHING || room_data_083887b4 == 0x083887B4, "MATCHING: room_data_083887b4 moved")
    ASSERT(!MATCHING || gLevel4Stage5Room3 == 0x08388970, "MATCHING: room_def_08388970 moved")
    ASSERT(!MATCHING || room_data_083889c8 == 0x083889C8, "MATCHING: room_data_083889c8 moved")
    ASSERT(!MATCHING || gLevel4Stage5Room4 == 0x08388BB8, "MATCHING: room_def_08388bb8 moved")
    ASSERT(!MATCHING || room_data_08388c10 == 0x08388C10, "MATCHING: room_data_08388c10 moved")
    ASSERT(!MATCHING || gLevel4Stage6Room0 == 0x0838928C, "MATCHING: room_def_0838928c moved")
    ASSERT(!MATCHING || room_data_083892e4 == 0x083892E4, "MATCHING: room_data_083892e4 moved")
    ASSERT(!MATCHING || gLevel4Stage6Room1 == 0x083893DC, "MATCHING: room_def_083893dc moved")
    ASSERT(!MATCHING || room_data_08389434 == 0x08389434, "MATCHING: room_data_08389434 moved")
    ASSERT(!MATCHING || gLevel5Stage0Room0 == 0x08389A60, "MATCHING: room_def_08389a60 moved")
    ASSERT(!MATCHING || room_data_08389ab8 == 0x08389AB8, "MATCHING: room_data_08389ab8 moved")
    ASSERT(!MATCHING || gLevel5Stage0Room1 == 0x08389F2C, "MATCHING: room_def_08389f2c moved")
    ASSERT(!MATCHING || room_data_08389f84 == 0x08389F84, "MATCHING: room_data_08389f84 moved")
    ASSERT(!MATCHING || gLevel5Stage0Room2 == 0x0838A420, "MATCHING: room_def_0838a420 moved")
    ASSERT(!MATCHING || room_data_0838a478 == 0x0838A478, "MATCHING: room_data_0838a478 moved")
    ASSERT(!MATCHING || gLevel5Stage0Room3 == 0x0838A668, "MATCHING: room_def_0838a668 moved")
    ASSERT(!MATCHING || room_data_0838a6c0 == 0x0838A6C0, "MATCHING: room_data_0838a6c0 moved")
    ASSERT(!MATCHING || gLevel5Stage0Room4 == 0x0838A85C, "MATCHING: room_def_0838a85c moved")
    ASSERT(!MATCHING || room_data_0838a8b4 == 0x0838A8B4, "MATCHING: room_data_0838a8b4 moved")
    ASSERT(!MATCHING || gLevel5Stage0Room5 == 0x0838A9D0, "MATCHING: room_def_0838a9d0 moved")
    ASSERT(!MATCHING || room_data_0838aa28 == 0x0838AA28, "MATCHING: room_data_0838aa28 moved")
    ASSERT(!MATCHING || gLevel5Stage1Room0 == 0x0838AF20, "MATCHING: room_def_0838af20 moved")
    ASSERT(!MATCHING || room_data_0838af78 == 0x0838AF78, "MATCHING: room_data_0838af78 moved")
    ASSERT(!MATCHING || gLevel5Stage1Room1 == 0x0838B394, "MATCHING: room_def_0838b394 moved")
    ASSERT(!MATCHING || room_data_0838b3ec == 0x0838B3EC, "MATCHING: room_data_0838b3ec moved")
    ASSERT(!MATCHING || gLevel5Stage1Room2 == 0x0838B744, "MATCHING: room_def_0838b744 moved")
    ASSERT(!MATCHING || room_data_0838b79c == 0x0838B79C, "MATCHING: room_data_0838b79c moved")
    ASSERT(!MATCHING || gLevel5Stage1Room3 == 0x0838BB74, "MATCHING: room_def_0838bb74 moved")
    ASSERT(!MATCHING || room_data_0838bbcc == 0x0838BBCC, "MATCHING: room_data_0838bbcc moved")
    ASSERT(!MATCHING || gLevel5Stage1Room4 == 0x0838BD4C, "MATCHING: room_def_0838bd4c moved")
    ASSERT(!MATCHING || room_data_0838bda4 == 0x0838BDA4, "MATCHING: room_data_0838bda4 moved")
    ASSERT(!MATCHING || gLevel5Stage1Room5 == 0x0838BF0C, "MATCHING: room_def_0838bf0c moved")
    ASSERT(!MATCHING || room_data_0838bf64 == 0x0838BF64, "MATCHING: room_data_0838bf64 moved")
    ASSERT(!MATCHING || gLevel5Stage1Room6 == 0x0838C068, "MATCHING: room_def_0838c068 moved")
    ASSERT(!MATCHING || room_data_0838c0c0 == 0x0838C0C0, "MATCHING: room_data_0838c0c0 moved")
    ASSERT(!MATCHING || gLevel5Stage2Room0 == 0x0838C574, "MATCHING: room_def_0838c574 moved")
    ASSERT(!MATCHING || room_data_0838c5cc == 0x0838C5CC, "MATCHING: room_data_0838c5cc moved")
    ASSERT(!MATCHING || gLevel5Stage2Room1 == 0x0838CB58, "MATCHING: room_def_0838cb58 moved")
    ASSERT(!MATCHING || room_data_0838cbb0 == 0x0838CBB0, "MATCHING: room_data_0838cbb0 moved")
    ASSERT(!MATCHING || gLevel5Stage2Room2 == 0x0838D190, "MATCHING: room_def_0838d190 moved")
    ASSERT(!MATCHING || room_data_0838d1e8 == 0x0838D1E8, "MATCHING: room_data_0838d1e8 moved")
    ASSERT(!MATCHING || gLevel5Stage2Room3 == 0x0838D7FC, "MATCHING: room_def_0838d7fc moved")
    ASSERT(!MATCHING || room_data_0838d854 == 0x0838D854, "MATCHING: room_data_0838d854 moved")
    ASSERT(!MATCHING || gLevel5Stage2Room4 == 0x0838DBEC, "MATCHING: room_def_0838dbec moved")
    ASSERT(!MATCHING || room_data_0838dc44 == 0x0838DC44, "MATCHING: room_data_0838dc44 moved")
    ASSERT(!MATCHING || gLevel5Stage2Room5 == 0x0838DD58, "MATCHING: room_def_0838dd58 moved")
    ASSERT(!MATCHING || room_data_0838ddb0 == 0x0838DDB0, "MATCHING: room_data_0838ddb0 moved")
    ASSERT(!MATCHING || gLevel5Stage2Room6 == 0x0838DEC4, "MATCHING: room_def_0838dec4 moved")
    ASSERT(!MATCHING || room_data_0838df1c == 0x0838DF1C, "MATCHING: room_data_0838df1c moved")
    ASSERT(!MATCHING || gLevel5Stage2Room7 == 0x0838E038, "MATCHING: room_def_0838e038 moved")
    ASSERT(!MATCHING || room_data_0838e090 == 0x0838E090, "MATCHING: room_data_0838e090 moved")
    ASSERT(!MATCHING || gLevel5Stage2Room8 == 0x0838E2C8, "MATCHING: room_def_0838e2c8 moved")
    ASSERT(!MATCHING || room_data_0838e320 == 0x0838E320, "MATCHING: room_data_0838e320 moved")
    ASSERT(!MATCHING || gLevel5Stage2Room9 == 0x0838E478, "MATCHING: room_def_0838e478 moved")
    ASSERT(!MATCHING || room_data_0838e4d0 == 0x0838E4D0, "MATCHING: room_data_0838e4d0 moved")
    ASSERT(!MATCHING || gLevel5Stage2Room10 == 0x0838E688, "MATCHING: room_def_0838e688 moved")
    ASSERT(!MATCHING || room_data_0838e6e0 == 0x0838E6E0, "MATCHING: room_data_0838e6e0 moved")
    ASSERT(!MATCHING || gLevel5Stage2Room11 == 0x0838E818, "MATCHING: room_def_0838e818 moved")
    ASSERT(!MATCHING || room_data_0838e870 == 0x0838E870, "MATCHING: room_data_0838e870 moved")
    ASSERT(!MATCHING || gLevel5Stage3Room0 == 0x0838EEE4, "MATCHING: room_def_0838eee4 moved")
    ASSERT(!MATCHING || room_data_0838ef3c == 0x0838EF3C, "MATCHING: room_data_0838ef3c moved")
    ASSERT(!MATCHING || gLevel5Stage3Room1 == 0x0838F788, "MATCHING: room_def_0838f788 moved")
    ASSERT(!MATCHING || room_data_0838f7e0 == 0x0838F7E0, "MATCHING: room_data_0838f7e0 moved")
    ASSERT(!MATCHING || gLevel5Stage3Room2 == 0x0838F914, "MATCHING: room_def_0838f914 moved")
    ASSERT(!MATCHING || room_data_0838f96c == 0x0838F96C, "MATCHING: room_data_0838f96c moved")
    ASSERT(!MATCHING || gLevel5Stage3Room3 == 0x0838FCF4, "MATCHING: room_def_0838fcf4 moved")
    ASSERT(!MATCHING || room_data_0838fd4c == 0x0838FD4C, "MATCHING: room_data_0838fd4c moved")
    ASSERT(!MATCHING || gLevel5Stage3Room4 == 0x083900AC, "MATCHING: room_def_083900ac moved")
    ASSERT(!MATCHING || room_data_08390104 == 0x08390104, "MATCHING: room_data_08390104 moved")
    ASSERT(!MATCHING || gLevel5Stage3Room5 == 0x083902B0, "MATCHING: room_def_083902b0 moved")
    ASSERT(!MATCHING || room_data_08390308 == 0x08390308, "MATCHING: room_data_08390308 moved")
    ASSERT(!MATCHING || gLevel5Stage4Room0 == 0x0839059C, "MATCHING: room_def_0839059c moved")
    ASSERT(!MATCHING || room_data_083905f4 == 0x083905F4, "MATCHING: room_data_083905f4 moved")
    ASSERT(!MATCHING || gLevel5Stage4Room1 == 0x08390BA8, "MATCHING: room_def_08390ba8 moved")
    ASSERT(!MATCHING || room_data_08390c00 == 0x08390C00, "MATCHING: room_data_08390c00 moved")
    ASSERT(!MATCHING || gLevel5Stage4Room2 == 0x08391138, "MATCHING: room_def_08391138 moved")
    ASSERT(!MATCHING || room_data_08391190 == 0x08391190, "MATCHING: room_data_08391190 moved")
    ASSERT(!MATCHING || gLevel5Stage4Room3 == 0x08391C40, "MATCHING: room_def_08391c40 moved")
    ASSERT(!MATCHING || room_data_08391c98 == 0x08391C98, "MATCHING: room_data_08391c98 moved")
    ASSERT(!MATCHING || gLevel5Stage4Room4 == 0x08391D9C, "MATCHING: room_def_08391d9c moved")
    ASSERT(!MATCHING || room_data_08391df4 == 0x08391DF4, "MATCHING: room_data_08391df4 moved")
    ASSERT(!MATCHING || gLevel5Stage5Room0 == 0x08392308, "MATCHING: room_def_08392308 moved")
    ASSERT(!MATCHING || room_data_08392360 == 0x08392360, "MATCHING: room_data_08392360 moved")
    ASSERT(!MATCHING || gLevel5Stage5Room1 == 0x08392778, "MATCHING: room_def_08392778 moved")
    ASSERT(!MATCHING || room_data_083927d0 == 0x083927D0, "MATCHING: room_data_083927d0 moved")
    ASSERT(!MATCHING || gLevel5Stage5Room2 == 0x08392AA8, "MATCHING: room_def_08392aa8 moved")
    ASSERT(!MATCHING || room_data_08392b00 == 0x08392B00, "MATCHING: room_data_08392b00 moved")
    ASSERT(!MATCHING || gLevel5Stage5Room3 == 0x08392F0C, "MATCHING: room_def_08392f0c moved")
    ASSERT(!MATCHING || room_data_08392f64 == 0x08392F64, "MATCHING: room_data_08392f64 moved")
    ASSERT(!MATCHING || gLevel5Stage5Room4 == 0x08393430, "MATCHING: room_def_08393430 moved")
    ASSERT(!MATCHING || room_data_08393488 == 0x08393488, "MATCHING: room_data_08393488 moved")
    ASSERT(!MATCHING || gLevel5Stage5Room5 == 0x083935AC, "MATCHING: room_def_083935ac moved")
    ASSERT(!MATCHING || room_data_08393604 == 0x08393604, "MATCHING: room_data_08393604 moved")
    ASSERT(!MATCHING || gLevel5Stage5Room6 == 0x08393760, "MATCHING: room_def_08393760 moved")
    ASSERT(!MATCHING || room_data_083937b8 == 0x083937B8, "MATCHING: room_data_083937b8 moved")
    ASSERT(!MATCHING || gLevel5Stage5Room7 == 0x083938C0, "MATCHING: room_def_083938c0 moved")
    ASSERT(!MATCHING || room_data_08393918 == 0x08393918, "MATCHING: room_data_08393918 moved")
    ASSERT(!MATCHING || gLevel5Stage5Room8 == 0x08393A7C, "MATCHING: room_def_08393a7c moved")
    ASSERT(!MATCHING || room_data_08393ad4 == 0x08393AD4, "MATCHING: room_data_08393ad4 moved")
    ASSERT(!MATCHING || gLevel5Stage5Room9 == 0x08393C2C, "MATCHING: room_def_08393c2c moved")
    ASSERT(!MATCHING || room_data_08393c84 == 0x08393C84, "MATCHING: room_data_08393c84 moved")
    ASSERT(!MATCHING || gLevel5Stage5Room10 == 0x08393DC8, "MATCHING: room_def_08393dc8 moved")
    ASSERT(!MATCHING || room_data_08393e20 == 0x08393E20, "MATCHING: room_data_08393e20 moved")
    ASSERT(!MATCHING || gLevel5Stage6Room0 == 0x08393EF4, "MATCHING: room_def_08393ef4 moved")
    ASSERT(!MATCHING || room_data_08393f4c == 0x08393F4C, "MATCHING: room_data_08393f4c moved")
    ASSERT(!MATCHING || gLevel6Stage0Room0 == 0x08394458, "MATCHING: room_def_08394458 moved")
    ASSERT(!MATCHING || room_data_083944b0 == 0x083944B0, "MATCHING: room_data_083944b0 moved")
    ASSERT(!MATCHING || gLevel6Stage0Room1 == 0x08394C58, "MATCHING: room_def_08394c58 moved")
    ASSERT(!MATCHING || room_data_08394cb0 == 0x08394CB0, "MATCHING: room_data_08394cb0 moved")
    ASSERT(!MATCHING || gLevel6Stage0Room2 == 0x083950F8, "MATCHING: room_def_083950f8 moved")
    ASSERT(!MATCHING || room_data_08395150 == 0x08395150, "MATCHING: room_data_08395150 moved")
    ASSERT(!MATCHING || gLevel6Stage0Room3 == 0x08395298, "MATCHING: room_def_08395298 moved")
    ASSERT(!MATCHING || room_data_083952f0 == 0x083952F0, "MATCHING: room_data_083952f0 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room0 == 0x08395450, "MATCHING: room_def_08395450 moved")
    ASSERT(!MATCHING || room_data_083954a8 == 0x083954A8, "MATCHING: room_data_083954a8 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room1 == 0x0839570C, "MATCHING: room_def_0839570c moved")
    ASSERT(!MATCHING || room_data_08395764 == 0x08395764, "MATCHING: room_data_08395764 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room2 == 0x083959C8, "MATCHING: room_def_083959c8 moved")
    ASSERT(!MATCHING || room_data_08395a20 == 0x08395A20, "MATCHING: room_data_08395a20 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room3 == 0x08395C90, "MATCHING: room_def_08395c90 moved")
    ASSERT(!MATCHING || room_data_08395ce8 == 0x08395CE8, "MATCHING: room_data_08395ce8 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room4 == 0x08395F34, "MATCHING: room_def_08395f34 moved")
    ASSERT(!MATCHING || room_data_08395f8c == 0x08395F8C, "MATCHING: room_data_08395f8c moved")
    ASSERT(!MATCHING || gLevel6Stage1Room5 == 0x083961E4, "MATCHING: room_def_083961e4 moved")
    ASSERT(!MATCHING || room_data_0839623c == 0x0839623C, "MATCHING: room_data_0839623c moved")
    ASSERT(!MATCHING || gLevel6Stage1Room6 == 0x083964AC, "MATCHING: room_def_083964ac moved")
    ASSERT(!MATCHING || room_data_08396504 == 0x08396504, "MATCHING: room_data_08396504 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room7 == 0x08396754, "MATCHING: room_def_08396754 moved")
    ASSERT(!MATCHING || room_data_083967ac == 0x083967AC, "MATCHING: room_data_083967ac moved")
    ASSERT(!MATCHING || gLevel6Stage1Room8 == 0x08396A00, "MATCHING: room_def_08396a00 moved")
    ASSERT(!MATCHING || room_data_08396a58 == 0x08396A58, "MATCHING: room_data_08396a58 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room9 == 0x08396CB8, "MATCHING: room_def_08396cb8 moved")
    ASSERT(!MATCHING || room_data_08396d10 == 0x08396D10, "MATCHING: room_data_08396d10 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room10 == 0x0839706C, "MATCHING: room_def_0839706c moved")
    ASSERT(!MATCHING || room_data_083970c4 == 0x083970C4, "MATCHING: room_data_083970c4 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room11 == 0x08397358, "MATCHING: room_def_08397358 moved")
    ASSERT(!MATCHING || room_data_083973b0 == 0x083973B0, "MATCHING: room_data_083973b0 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room12 == 0x08397508, "MATCHING: room_def_08397508 moved")
    ASSERT(!MATCHING || room_data_08397560 == 0x08397560, "MATCHING: room_data_08397560 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room13 == 0x08397680, "MATCHING: room_def_08397680 moved")
    ASSERT(!MATCHING || room_data_083976d8 == 0x083976D8, "MATCHING: room_data_083976d8 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room14 == 0x08397804, "MATCHING: room_def_08397804 moved")
    ASSERT(!MATCHING || room_data_0839785c == 0x0839785C, "MATCHING: room_data_0839785c moved")
    ASSERT(!MATCHING || gLevel6Stage1Room15 == 0x08397970, "MATCHING: room_def_08397970 moved")
    ASSERT(!MATCHING || room_data_083979c8 == 0x083979C8, "MATCHING: room_data_083979c8 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room16 == 0x08397B54, "MATCHING: room_def_08397b54 moved")
    ASSERT(!MATCHING || room_data_08397bac == 0x08397BAC, "MATCHING: room_data_08397bac moved")
    ASSERT(!MATCHING || gLevel6Stage1Room17 == 0x08397CFC, "MATCHING: room_def_08397cfc moved")
    ASSERT(!MATCHING || room_data_08397d54 == 0x08397D54, "MATCHING: room_data_08397d54 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room18 == 0x08397ED0, "MATCHING: room_def_08397ed0 moved")
    ASSERT(!MATCHING || room_data_08397f28 == 0x08397F28, "MATCHING: room_data_08397f28 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room19 == 0x0839804C, "MATCHING: room_def_0839804c moved")
    ASSERT(!MATCHING || room_data_083980a4 == 0x083980A4, "MATCHING: room_data_083980a4 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room20 == 0x08398220, "MATCHING: room_def_08398220 moved")
    ASSERT(!MATCHING || room_data_08398278 == 0x08398278, "MATCHING: room_data_08398278 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room21 == 0x083983D8, "MATCHING: room_def_083983d8 moved")
    ASSERT(!MATCHING || room_data_08398430 == 0x08398430, "MATCHING: room_data_08398430 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room22 == 0x083985D0, "MATCHING: room_def_083985d0 moved")
    ASSERT(!MATCHING || room_data_08398628 == 0x08398628, "MATCHING: room_data_08398628 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room23 == 0x0839875C, "MATCHING: room_def_0839875c moved")
    ASSERT(!MATCHING || room_data_083987b4 == 0x083987B4, "MATCHING: room_data_083987b4 moved")
    ASSERT(!MATCHING || gLevel6Stage1Room24 == 0x08398858, "MATCHING: room_def_08398858 moved")
    ASSERT(!MATCHING || room_data_083988b0 == 0x083988B0, "MATCHING: room_data_083988b0 moved")
    ASSERT(!MATCHING || gLevel6Stage2Room0 == 0x08398DDC, "MATCHING: room_def_08398ddc moved")
    ASSERT(!MATCHING || room_data_08398e34 == 0x08398E34, "MATCHING: room_data_08398e34 moved")
    ASSERT(!MATCHING || gLevel6Stage2Room1 == 0x08399208, "MATCHING: room_def_08399208 moved")
    ASSERT(!MATCHING || room_data_08399260 == 0x08399260, "MATCHING: room_data_08399260 moved")
    ASSERT(!MATCHING || gLevel6Stage2Room2 == 0x083995EC, "MATCHING: room_def_083995ec moved")
    ASSERT(!MATCHING || room_data_08399644 == 0x08399644, "MATCHING: room_data_08399644 moved")
    ASSERT(!MATCHING || gLevel6Stage2Room3 == 0x08399CB8, "MATCHING: room_def_08399cb8 moved")
    ASSERT(!MATCHING || room_data_08399d10 == 0x08399D10, "MATCHING: room_data_08399d10 moved")
    ASSERT(!MATCHING || gLevel6Stage3Room0 == 0x0839A214, "MATCHING: room_def_0839a214 moved")
    ASSERT(!MATCHING || room_data_0839a26c == 0x0839A26C, "MATCHING: room_data_0839a26c moved")
    ASSERT(!MATCHING || gLevel6Stage3Room1 == 0x0839A4C8, "MATCHING: room_def_0839a4c8 moved")
    ASSERT(!MATCHING || room_data_0839a520 == 0x0839A520, "MATCHING: room_data_0839a520 moved")
    ASSERT(!MATCHING || gLevel6Stage3Room2 == 0x0839A774, "MATCHING: room_def_0839a774 moved")
    ASSERT(!MATCHING || room_data_0839a7cc == 0x0839A7CC, "MATCHING: room_data_0839a7cc moved")
    ASSERT(!MATCHING || gLevel6Stage3Room3 == 0x0839AA40, "MATCHING: room_def_0839aa40 moved")
    ASSERT(!MATCHING || room_data_0839aa98 == 0x0839AA98, "MATCHING: room_data_0839aa98 moved")
    ASSERT(!MATCHING || gLevel6Stage3Room4 == 0x0839AD18, "MATCHING: room_def_0839ad18 moved")
    ASSERT(!MATCHING || room_data_0839ad70 == 0x0839AD70, "MATCHING: room_data_0839ad70 moved")
    ASSERT(!MATCHING || gLevel6Stage3Room5 == 0x0839B260, "MATCHING: room_def_0839b260 moved")
    ASSERT(!MATCHING || room_data_0839b2b8 == 0x0839B2B8, "MATCHING: room_data_0839b2b8 moved")
    ASSERT(!MATCHING || gLevel6Stage3Room6 == 0x0839BB28, "MATCHING: room_def_0839bb28 moved")
    ASSERT(!MATCHING || room_data_0839bb80 == 0x0839BB80, "MATCHING: room_data_0839bb80 moved")
    ASSERT(!MATCHING || gLevel6Stage4Room0 == 0x0839BED4, "MATCHING: room_def_0839bed4 moved")
    ASSERT(!MATCHING || room_data_0839bf2c == 0x0839BF2C, "MATCHING: room_data_0839bf2c moved")
    ASSERT(!MATCHING || gLevel6Stage4Room1 == 0x0839C0D0, "MATCHING: room_def_0839c0d0 moved")
    ASSERT(!MATCHING || room_data_0839c128 == 0x0839C128, "MATCHING: room_data_0839c128 moved")
    ASSERT(!MATCHING || gLevel6Stage4Room2 == 0x0839C57C, "MATCHING: room_def_0839c57c moved")
    ASSERT(!MATCHING || room_data_0839c5d4 == 0x0839C5D4, "MATCHING: room_data_0839c5d4 moved")
    ASSERT(!MATCHING || gLevel6Stage4Room3 == 0x0839CB34, "MATCHING: room_def_0839cb34 moved")
    ASSERT(!MATCHING || room_data_0839cb8c == 0x0839CB8C, "MATCHING: room_data_0839cb8c moved")
    ASSERT(!MATCHING || gLevel6Stage4Room4 == 0x0839CD48, "MATCHING: room_def_0839cd48 moved")
    ASSERT(!MATCHING || room_data_0839cda0 == 0x0839CDA0, "MATCHING: room_data_0839cda0 moved")
    ASSERT(!MATCHING || gLevel6Stage5Room0 == 0x0839D254, "MATCHING: room_def_0839d254 moved")
    ASSERT(!MATCHING || room_data_0839d2ac == 0x0839D2AC, "MATCHING: room_data_0839d2ac moved")
    ASSERT(!MATCHING || gLevel6Stage5Room1 == 0x0839D5B4, "MATCHING: room_def_0839d5b4 moved")
    ASSERT(!MATCHING || room_data_0839d60c == 0x0839D60C, "MATCHING: room_data_0839d60c moved")
    ASSERT(!MATCHING || gLevel6Stage5Room2 == 0x0839D8BC, "MATCHING: room_def_0839d8bc moved")
    ASSERT(!MATCHING || room_data_0839d914 == 0x0839D914, "MATCHING: room_data_0839d914 moved")
    ASSERT(!MATCHING || gLevel6Stage5Room3 == 0x0839DAFC, "MATCHING: room_def_0839dafc moved")
    ASSERT(!MATCHING || room_data_0839db54 == 0x0839DB54, "MATCHING: room_data_0839db54 moved")
    ASSERT(!MATCHING || gLevel6Stage5Room4 == 0x0839DED4, "MATCHING: room_def_0839ded4 moved")
    ASSERT(!MATCHING || room_data_0839df2c == 0x0839DF2C, "MATCHING: room_data_0839df2c moved")
    ASSERT(!MATCHING || gLevel6Stage5Room5 == 0x0839E190, "MATCHING: room_def_0839e190 moved")
    ASSERT(!MATCHING || room_data_0839e1e8 == 0x0839E1E8, "MATCHING: room_data_0839e1e8 moved")
    ASSERT(!MATCHING || gLevel6Stage5Room6 == 0x0839E590, "MATCHING: room_def_0839e590 moved")
    ASSERT(!MATCHING || room_data_0839e5e8 == 0x0839E5E8, "MATCHING: room_data_0839e5e8 moved")
    ASSERT(!MATCHING || gLevel6Stage5Room7 == 0x0839E944, "MATCHING: room_def_0839e944 moved")
    ASSERT(!MATCHING || room_data_0839e99c == 0x0839E99C, "MATCHING: room_data_0839e99c moved")
    ASSERT(!MATCHING || gLevel6Stage5Room8 == 0x0839EBB4, "MATCHING: room_def_0839ebb4 moved")
    ASSERT(!MATCHING || room_data_0839ec0c == 0x0839EC0C, "MATCHING: room_data_0839ec0c moved")
    ASSERT(!MATCHING || gLevel6Stage6Room0 == 0x0839EE14, "MATCHING: room_def_0839ee14 moved")
    ASSERT(!MATCHING || room_data_0839ee6c == 0x0839EE6C, "MATCHING: room_data_0839ee6c moved")
    ASSERT(!MATCHING || gLevel7Stage0Room0 == 0x0839F3D8, "MATCHING: room_def_0839f3d8 moved")
    ASSERT(!MATCHING || room_data_0839f430 == 0x0839F430, "MATCHING: room_data_0839f430 moved")
    ASSERT(!MATCHING || gLevel7Stage0Room1 == 0x0839F684, "MATCHING: room_def_0839f684 moved")
    ASSERT(!MATCHING || room_data_0839f6dc == 0x0839F6DC, "MATCHING: room_data_0839f6dc moved")
    ASSERT(!MATCHING || gLevel7Stage0Room2 == 0x0839FC40, "MATCHING: room_def_0839fc40 moved")
    ASSERT(!MATCHING || room_data_0839fc98 == 0x0839FC98, "MATCHING: room_data_0839fc98 moved")
    ASSERT(!MATCHING || gLevel7Stage2Room0 == 0x083A0058, "MATCHING: room_def_083a0058 moved")
    ASSERT(!MATCHING || room_data_083a00b0 == 0x083A00B0, "MATCHING: room_data_083a00b0 moved")
    ASSERT(!MATCHING || gLevel7Stage2Room1 == 0x083A0370, "MATCHING: room_def_083a0370 moved")
    ASSERT(!MATCHING || room_data_083a03c8 == 0x083A03C8, "MATCHING: room_data_083a03c8 moved")
    ASSERT(!MATCHING || gLevel7Stage2Room2 == 0x083A06DC, "MATCHING: room_def_083a06dc moved")
    ASSERT(!MATCHING || room_data_083a0734 == 0x083A0734, "MATCHING: room_data_083a0734 moved")
    ASSERT(!MATCHING || gLevel7Stage2Room3 == 0x083A0AA0, "MATCHING: room_def_083a0aa0 moved")
    ASSERT(!MATCHING || room_data_083a0af8 == 0x083A0AF8, "MATCHING: room_data_083a0af8 moved")
    ASSERT(!MATCHING || gLevel7Stage2Room4 == 0x083A0D2C, "MATCHING: room_def_083a0d2c moved")
    ASSERT(!MATCHING || room_data_083a0d84 == 0x083A0D84, "MATCHING: room_data_083a0d84 moved")
    ASSERT(!MATCHING || gLevel7Stage2Room5 == 0x083A0F00, "MATCHING: room_def_083a0f00 moved")
    ASSERT(!MATCHING || room_data_083a0f58 == 0x083A0F58, "MATCHING: room_data_083a0f58 moved")
    ASSERT(!MATCHING || gLevel7Stage2Room6 == 0x083A142C, "MATCHING: room_def_083a142c moved")
    ASSERT(!MATCHING || room_data_083a1484 == 0x083A1484, "MATCHING: room_data_083a1484 moved")
    ASSERT(!MATCHING || gLevel7Stage2Room7 == 0x083A1644, "MATCHING: room_def_083a1644 moved")
    ASSERT(!MATCHING || room_data_083a169c == 0x083A169C, "MATCHING: room_data_083a169c moved")
    ASSERT(!MATCHING || gLevel7Stage2Room8 == 0x083A17CC, "MATCHING: room_def_083a17cc moved")
    ASSERT(!MATCHING || room_data_083a1824 == 0x083A1824, "MATCHING: room_data_083a1824 moved")
    ASSERT(!MATCHING || gLevel7Stage2Room9 == 0x083A1B64, "MATCHING: room_def_083a1b64 moved")
    ASSERT(!MATCHING || room_data_083a1bbc == 0x083A1BBC, "MATCHING: room_data_083a1bbc moved")
    ASSERT(!MATCHING || gLevel7Stage2Room10 == 0x083A1FD8, "MATCHING: room_def_083a1fd8 moved")
    ASSERT(!MATCHING || room_data_083a2030 == 0x083A2030, "MATCHING: room_data_083a2030 moved")
    ASSERT(!MATCHING || gLevel8Stage0Room0 == 0x083A24F0, "MATCHING: room_def_083a24f0 moved")
    ASSERT(!MATCHING || room_data_083a2548 == 0x083A2548, "MATCHING: room_data_083a2548 moved")
    ASSERT(!MATCHING || gLevel8Stage0Room1 == 0x083A2578, "MATCHING: room_def_083a2578 moved")
    ASSERT(!MATCHING || room_data_083a25d0 == 0x083A25D0, "MATCHING: room_data_083a25d0 moved")
    ASSERT(!MATCHING || gLevel8Stage0Room2 == 0x083A2694, "MATCHING: room_def_083a2694 moved")
    ASSERT(!MATCHING || room_data_083a26ec == 0x083A26EC, "MATCHING: room_data_083a26ec moved")
    ASSERT(!MATCHING || gLevel8Stage0Room3 == 0x083A291C, "MATCHING: room_def_083a291c moved")
    ASSERT(!MATCHING || room_data_083a2974 == 0x083A2974, "MATCHING: room_data_083a2974 moved")
    ASSERT(!MATCHING || gLevel8Stage0Room4 == 0x083A2AC4, "MATCHING: room_def_083a2ac4 moved")
    ASSERT(!MATCHING || room_data_083a2b1c == 0x083A2B1C, "MATCHING: room_data_083a2b1c moved")
    ASSERT(!MATCHING || gLevel8Stage1Room0 == 0x083A32BC, "MATCHING: room_def_083a32bc moved")
    ASSERT(!MATCHING || room_data_083a3314 == 0x083A3314, "MATCHING: room_data_083a3314 moved")
    ASSERT(!MATCHING || gLevel8Stage1Room1 == 0x083A3344, "MATCHING: room_def_083a3344 moved")
    ASSERT(!MATCHING || room_data_083a339c == 0x083A339C, "MATCHING: room_data_083a339c moved")
    ASSERT(!MATCHING || gLevel8Stage1Room2 == 0x083A34E4, "MATCHING: room_def_083a34e4 moved")
    ASSERT(!MATCHING || room_data_083a353c == 0x083A353C, "MATCHING: room_data_083a353c moved")
    ASSERT(!MATCHING || gLevel8Stage1Room3 == 0x083A360C, "MATCHING: room_def_083a360c moved")
    ASSERT(!MATCHING || room_data_083a3664 == 0x083A3664, "MATCHING: room_data_083a3664 moved")
    ASSERT(!MATCHING || gLevel8Stage1Room4 == 0x083A38FC, "MATCHING: room_def_083a38fc moved")
    ASSERT(!MATCHING || room_data_083a3954 == 0x083A3954, "MATCHING: room_data_083a3954 moved")
    ASSERT(!MATCHING || gLevel8Stage1Room5 == 0x083A3A74, "MATCHING: room_def_083a3a74 moved")
    ASSERT(!MATCHING || room_data_083a3acc == 0x083A3ACC, "MATCHING: room_data_083a3acc moved")
    ASSERT(!MATCHING || gLevel8Stage2Room0 == 0x083A3FF4, "MATCHING: room_def_083a3ff4 moved")
    ASSERT(!MATCHING || room_data_083a404c == 0x083A404C, "MATCHING: room_data_083a404c moved")
    ASSERT(!MATCHING || gLevel8Stage2Room1 == 0x083A407C, "MATCHING: room_def_083a407c moved")
    ASSERT(!MATCHING || room_data_083a40d4 == 0x083A40D4, "MATCHING: room_data_083a40d4 moved")
    ASSERT(!MATCHING || gLevel8Stage2Room2 == 0x083A421C, "MATCHING: room_def_083a421c moved")
    ASSERT(!MATCHING || room_data_083a4274 == 0x083A4274, "MATCHING: room_data_083a4274 moved")
    ASSERT(!MATCHING || gLevel8Stage2Room3 == 0x083A433C, "MATCHING: room_def_083a433c moved")
    ASSERT(!MATCHING || room_data_083a4394 == 0x083A4394, "MATCHING: room_data_083a4394 moved")
    ASSERT(!MATCHING || gLevel8Stage2Room4 == 0x083A45DC, "MATCHING: room_def_083a45dc moved")
    ASSERT(!MATCHING || room_data_083a4634 == 0x083A4634, "MATCHING: room_data_083a4634 moved")
    ASSERT(!MATCHING || gLevel8Stage2Room5 == 0x083A46FC, "MATCHING: room_def_083a46fc moved")
    ASSERT(!MATCHING || room_data_083a4754 == 0x083A4754, "MATCHING: room_data_083a4754 moved")
    ASSERT(!MATCHING || gLevel8Stage3Room0 == 0x083A4D80, "MATCHING: room_def_083a4d80 moved")
    ASSERT(!MATCHING || room_data_083a4dd8 == 0x083A4DD8, "MATCHING: room_data_083a4dd8 moved")
    ASSERT(!MATCHING || gLevel8Stage3Room1 == 0x083A4E08, "MATCHING: room_def_083a4e08 moved")
    ASSERT(!MATCHING || room_data_083a4e60 == 0x083A4E60, "MATCHING: room_data_083a4e60 moved")
    ASSERT(!MATCHING || gLevel8Stage3Room2 == 0x083A4FA8, "MATCHING: room_def_083a4fa8 moved")
    ASSERT(!MATCHING || room_data_083a5000 == 0x083A5000, "MATCHING: room_data_083a5000 moved")
    ASSERT(!MATCHING || gLevel8Stage3Room3 == 0x083A50D0, "MATCHING: room_def_083a50d0 moved")
    ASSERT(!MATCHING || room_data_083a5128 == 0x083A5128, "MATCHING: room_data_083a5128 moved")
    ASSERT(!MATCHING || gLevel8Stage3Room4 == 0x083A5354, "MATCHING: room_def_083a5354 moved")
    ASSERT(!MATCHING || room_data_083a53ac == 0x083A53AC, "MATCHING: room_data_083a53ac moved")
    ASSERT(!MATCHING || gLevel8Stage3Room5 == 0x083A5480, "MATCHING: room_def_083a5480 moved")
    ASSERT(!MATCHING || room_data_083a54d8 == 0x083A54D8, "MATCHING: room_data_083a54d8 moved")
    ASSERT(!MATCHING || gLevel8Stage4Room0 == 0x083A5BE8, "MATCHING: room_def_083a5be8 moved")
    ASSERT(!MATCHING || room_data_083a5c40 == 0x083A5C40, "MATCHING: room_data_083a5c40 moved")
    ASSERT(!MATCHING || gLevel8Stage4Room1 == 0x083A5C70, "MATCHING: room_def_083a5c70 moved")
    ASSERT(!MATCHING || room_data_083a5cc8 == 0x083A5CC8, "MATCHING: room_data_083a5cc8 moved")
    ASSERT(!MATCHING || gLevel8Stage4Room2 == 0x083A5E10, "MATCHING: room_def_083a5e10 moved")
    ASSERT(!MATCHING || room_data_083a5e68 == 0x083A5E68, "MATCHING: room_data_083a5e68 moved")
    ASSERT(!MATCHING || gLevel8Stage4Room3 == 0x083A5F3C, "MATCHING: room_def_083a5f3c moved")
    ASSERT(!MATCHING || room_data_083a5f94 == 0x083A5F94, "MATCHING: room_data_083a5f94 moved")
    ASSERT(!MATCHING || gLevel8Stage4Room4 == 0x083A61E4, "MATCHING: room_def_083a61e4 moved")
    ASSERT(!MATCHING || room_data_083a623c == 0x083A623C, "MATCHING: room_data_083a623c moved")
    ASSERT(!MATCHING || gLevel8Stage4Room5 == 0x083A633C, "MATCHING: room_def_083a633c moved")
    ASSERT(!MATCHING || room_data_083a6394 == 0x083A6394, "MATCHING: room_data_083a6394 moved")
    ASSERT(!MATCHING || gLevel8Stage5Room0 == 0x083A6AE0, "MATCHING: room_def_083a6ae0 moved")
    ASSERT(!MATCHING || room_data_083a6b38 == 0x083A6B38, "MATCHING: room_data_083a6b38 moved")
    ASSERT(!MATCHING || gLevel8Stage5Room1 == 0x083A6B68, "MATCHING: room_def_083a6b68 moved")
    ASSERT(!MATCHING || room_data_083a6bc0 == 0x083A6BC0, "MATCHING: room_data_083a6bc0 moved")
    ASSERT(!MATCHING || gLevel8Stage5Room2 == 0x083A6D08, "MATCHING: room_def_083a6d08 moved")
    ASSERT(!MATCHING || room_data_083a6d60 == 0x083A6D60, "MATCHING: room_data_083a6d60 moved")
    ASSERT(!MATCHING || gLevel8Stage5Room3 == 0x083A6E24, "MATCHING: room_def_083a6e24 moved")
    ASSERT(!MATCHING || room_data_083a6e7c == 0x083A6E7C, "MATCHING: room_data_083a6e7c moved")
    ASSERT(!MATCHING || gLevel8Stage5Room4 == 0x083A70A0, "MATCHING: room_def_083a70a0 moved")
    ASSERT(!MATCHING || room_data_083a70f8 == 0x083A70F8, "MATCHING: room_data_083a70f8 moved")
    ASSERT(!MATCHING || gLevel8Stage5Room5 == 0x083A71E4, "MATCHING: room_def_083a71e4 moved")
    ASSERT(!MATCHING || room_data_083a723c == 0x083A723C, "MATCHING: room_data_083a723c moved")
    ASSERT(!MATCHING || gLevel8Stage6Room0 == 0x083A77C4, "MATCHING: room_def_083a77c4 moved")
    ASSERT(!MATCHING || room_data_083a781c == 0x083A781C, "MATCHING: room_data_083a781c moved")
    ASSERT(!MATCHING || gLevel8Stage6Room1 == 0x083A784C, "MATCHING: room_def_083a784c moved")
    ASSERT(!MATCHING || room_data_083a78a4 == 0x083A78A4, "MATCHING: room_data_083a78a4 moved")
    ASSERT(!MATCHING || gLevel8Stage6Room2 == 0x083A7ADC, "MATCHING: room_def_083a7adc moved")
    ASSERT(!MATCHING || room_data_083a7b34 == 0x083A7B34, "MATCHING: room_data_083a7b34 moved")
    ASSERT(!MATCHING || gLevel8Stage6Room3 == 0x083A7C28, "MATCHING: room_def_083a7c28 moved")
    ASSERT(!MATCHING || room_data_083a7c80 == 0x083A7C80, "MATCHING: room_data_083a7c80 moved")
    ASSERT(!MATCHING || gLevel8Stage7Room0 == 0x083A8364, "MATCHING: room_def_083a8364 moved")
    ASSERT(!MATCHING || room_data_083a83bc == 0x083A83BC, "MATCHING: room_data_083a83bc moved")
    ASSERT(!MATCHING || gLevel8Stage7Room1 == 0x083A85D4, "MATCHING: room_def_083a85d4 moved")

    /* Room metatile tables (#36 phase 2): 23 LZ77 RoomDef.metatileTiles streams */
    .room_metatiles : {
        KEEP(*(.room_metatiles))
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_metatiles) == 0x083A862C, "MATCHING: .room_metatiles moved")

    /* Room BG3 maps (#36 phase 2): 32 struct BgMap, 25 LZ77 and 7 raw */
    .room_bg3_maps : {
        KEEP(*(.room_bg3_maps))
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_bg3_maps) == 0x083B5538, "MATCHING: .room_bg3_maps moved")

    /* Compressed graphics (LZ77/RLE-class) */
    .compressed_graphics : {
        KEEP(*(.compressed_graphics))
    } > ROM
    ASSERT(!MATCHING || ADDR(.compressed_graphics) == 0x083D0148, "MATCHING: .compressed_graphics moved")

    /* Sprite sheets, part 2 (asset, #36 phase 2 run 3; formerly m4a_songs, no m4a data): OAM streams, raw and LZ77 tiles, palettes, TaskGfx records; opens with the tail of compressed_graphics' LZ77 stream gCutsceneSheet2Tiles */
    .sprite_sheets_2 : {
        KEEP(*(.sprite_sheets_2))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sprite_sheets_2) == 0x085C0000, "MATCHING: .sprite_sheets_2 moved")

    /* m4a engine rodata tables (issue #51): gMPlayJumpTableTemplate..gXcmdTable */
    .m4a_engine_rodata : {
        KEEP(*(.m4a_engine_rodata))
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_engine_rodata) == 0x0860A140, "MATCHING: .m4a_engine_rodata moved")

    /* m4a voicegroups (asset, #36 phase 2 run 3; formerly m4a_song_tracks): the five voicegroups and five 4-bit CGB waves tools/m4a_struct.py parses */
    .m4a_voicegroups : {
        KEEP(*(.m4a_voicegroups))
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_voicegroups) == 0x0860A418, "MATCHING: .m4a_voicegroups moved")

    /* gMPlayTable + gSongTable (issue #51) */
    .m4a_song_table : {
        KEEP(*(.m4a_song_table))
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_song_table) == 0x0860B430, "MATCHING: .m4a_song_table moved")

    /* m4a song data (asset, #36 phase 2 run 3; formerly m4a_songs_2): song headers, track streams, ToneData and all 129 WaveData PCM samples (tools/m4a_struct.py) */
    .m4a_song_data : {
        KEEP(*(.m4a_song_data))
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_song_data) == 0x0860C678, "MATCHING: .m4a_song_data moved")

    /* SRAM_V112 identifier string */
    .sram_v112_string : {
        KEEP(*(.sram_v112_string))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sram_v112_string) == 0x0872E9F8, "MATCHING: .sram_v112_string moved")

    /* Null padding after SRAM string */
    .gap_sram_v112_string_sram_driver_fn_table : {
        KEEP(*(.gap_sram_v112_string_sram_driver_fn_table))
    } > ROM
    ASSERT(!MATCHING || ADDR(.gap_sram_v112_string_sram_driver_fn_table) == 0x0872EA02, "MATCHING: .gap_sram_v112_string_sram_driver_fn_table moved")

    /* SRAM driver function table (4 Thumb pointers) */
    .sram_driver_fn_table : {
        KEEP(*(.sram_driver_fn_table))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sram_driver_fn_table) == 0x0872EA04, "MATCHING: .sram_driver_fn_table moved")

    /* Engine-zone rodata (#36 phase 2 run 3; formerly gap_sram_driver_fn_table_asset_metadata_index): 256 zero bytes, sprite half-dimensions gUnk_0872EB14, gBootSignature, the SE table gSfxTable, gCosTable; gTaskTypes follows */
    .engine_rodata : {
        KEEP(*(.engine_rodata))
    } > ROM
    ASSERT(!MATCHING || ADDR(.engine_rodata) == 0x0872EA14, "MATCHING: .engine_rodata moved")

    /* task_types - functional table in C (src/data/task_types.c, carved by tools/carve_data.py) */
    .task_types : {
        build/src/data/task_types.o(.rodata)
    } > ROM
    ASSERT(!MATCHING || ADDR(.task_types) == 0x0872FF30, "MATCHING: .task_types moved")

    /* Game rodata in link order, M02-M18 (#36 phase 2 run 3; formerly asset_metadata_index): mode, menu, player, collision, room, camera and terrain tables, blocks, player actions, effects, actor core (ldgroup.py: 43 rows in one output section) */
    .game_rodata : {
        KEEP(*(.game_rodata))
        build/src/data/game_tables.o(.game_tbl_0873078c)
        KEEP(*(.game_rodata_0873079c))
        build/src/data/game_tables.o(.game_tbl_08731fa8)
        KEEP(*(.game_rodata_087320c4))
        build/src/data/game_tables.o(.game_tbl_08732614)
        KEEP(*(.game_rodata_08732630))
        build/src/data/game_tables.o(.game_tbl_087328a0)
        KEEP(*(.game_rodata_087328bc))
        build/src/data/game_tables.o(.game_tbl_087328d8)
        KEEP(*(.game_rodata_087328f0))
        build/src/data/game_tables.o(.game_tbl_0873a748)
        KEEP(*(.game_rodata_0873a924))
        build/src/data/game_tables.o(.game_tbl_0873b42c)
        KEEP(*(.game_rodata_0873b510))
        build/src/data/game_tables.o(.game_tbl_0873b664)
        KEEP(*(.game_rodata_0873b6cc))
        build/src/data/game_tables.o(.game_tbl_0873b77c)
        KEEP(*(.game_rodata_0873b7b0))
        build/src/data/game_tables.o(.game_tbl_0873b928)
        KEEP(*(.game_rodata_0873b9ec))
        build/src/data/game_tables.o(.game_tbl_0873dbe4)
        KEEP(*(.game_rodata_0873dc3c))
        build/src/data/game_tables.o(.game_tbl_0873dea0)
        KEEP(*(.game_rodata_0873dea8))
        build/src/data/game_tables.o(.game_tbl_0873dedc)
        KEEP(*(.game_rodata_0873df14))
        build/src/data/game_tables.o(.game_tbl_0873df24)
        KEEP(*(.game_rodata_0873df38))
        build/src/data/game_tables.o(.game_tbl_0873e280)
        build/src/data/game_tables.o(.game_tbl_0873e284)
        KEEP(*(.game_rodata_0873e284))
        build/src/data/game_tables.o(.game_tbl_0873e2f0)
        KEEP(*(.game_rodata_0873e348))
        build/src/data/game_tables.o(.game_tbl_0873e5bc)
        KEEP(*(.game_rodata_0873e5f8))
        build/src/data/game_tables.o(.game_tbl_0873e670)
        KEEP(*(.game_rodata_0873e698))
        build/src/data/game_tables.o(.game_tbl_0873e734)
        KEEP(*(.game_rodata_0873e7a4))
        build/src/data/game_tables.o(.game_tbl_0873eaa0)
        KEEP(*(.game_rodata_0873eac0))
        build/src/data/game_tables.o(.game_tbl_0873ece0)
    } > ROM
    ASSERT(!MATCHING || ADDR(.game_rodata) == 0x08730780, "MATCHING: .game_rodata moved")
    ASSERT(!MATCHING || gExtraModeTitleSpriteStates == 0x0873078C, "MATCHING: game_tbl_0873078c moved")
    ASSERT(!MATCHING || game_rodata_0873079c == 0x0873079C, "MATCHING: game_rodata_0873079c moved")
    ASSERT(!MATCHING || gCutsceneStarts == 0x08731FA8, "MATCHING: game_tbl_08731fa8 moved")
    ASSERT(!MATCHING || game_rodata_087320c4 == 0x087320C4, "MATCHING: game_rodata_087320c4 moved")
    ASSERT(!MATCHING || gRoomTaskVariants == 0x08732614, "MATCHING: game_tbl_08732614 moved")
    ASSERT(!MATCHING || game_rodata_08732630 == 0x08732630, "MATCHING: game_rodata_08732630 moved")
    ASSERT(!MATCHING || gMapEventVariants == 0x087328A0, "MATCHING: game_tbl_087328a0 moved")
    ASSERT(!MATCHING || game_rodata_087328bc == 0x087328BC, "MATCHING: game_rodata_087328bc moved")
    ASSERT(!MATCHING || gStageEffectStates == 0x087328D8, "MATCHING: game_tbl_087328d8 moved")
    ASSERT(!MATCHING || game_rodata_087328f0 == 0x087328F0, "MATCHING: game_rodata_087328f0 moved")
    ASSERT(!MATCHING || gPlayerActions == 0x0873A748, "MATCHING: game_tbl_0873a748 moved")
    ASSERT(!MATCHING || game_rodata_0873a924 == 0x0873A924, "MATCHING: game_rodata_0873a924 moved")
    ASSERT(!MATCHING || gMetaKnightActions == 0x0873B42C, "MATCHING: game_tbl_0873b42c moved")
    ASSERT(!MATCHING || game_rodata_0873b510 == 0x0873B510, "MATCHING: game_rodata_0873b510 moved")
    ASSERT(!MATCHING || gPlayerBallVariants == 0x0873B664, "MATCHING: game_tbl_0873b664 moved")
    ASSERT(!MATCHING || game_rodata_0873b6cc == 0x0873B6CC, "MATCHING: game_rodata_0873b6cc moved")
    ASSERT(!MATCHING || gPlayerObjectVariants == 0x0873B77C, "MATCHING: game_tbl_0873b77c moved")
    ASSERT(!MATCHING || game_rodata_0873b7b0 == 0x0873B7B0, "MATCHING: game_rodata_0873b7b0 moved")
    ASSERT(!MATCHING || gPlayerEffectVariants == 0x0873B928, "MATCHING: game_tbl_0873b928 moved")
    ASSERT(!MATCHING || game_rodata_0873b9ec == 0x0873B9EC, "MATCHING: game_rodata_0873b9ec moved")
    ASSERT(!MATCHING || gPlayerGoalGameStates == 0x0873DBE4, "MATCHING: game_tbl_0873dbe4 moved")
    ASSERT(!MATCHING || game_rodata_0873dc3c == 0x0873DC3C, "MATCHING: game_rodata_0873dc3c moved")
    ASSERT(!MATCHING || gUnk_0873DEA0 == 0x0873DEA0, "MATCHING: game_tbl_0873dea0 moved")
    ASSERT(!MATCHING || game_rodata_0873dea8 == 0x0873DEA8, "MATCHING: game_rodata_0873dea8 moved")
    ASSERT(!MATCHING || gPlayerDances == 0x0873DEDC, "MATCHING: game_tbl_0873dedc moved")
    ASSERT(!MATCHING || game_rodata_0873df14 == 0x0873DF14, "MATCHING: game_rodata_0873df14 moved")
    ASSERT(!MATCHING || gPaletteAnimVariants == 0x0873DF24, "MATCHING: game_tbl_0873df24 moved")
    ASSERT(!MATCHING || game_rodata_0873df38 == 0x0873DF38, "MATCHING: game_rodata_0873df38 moved")
    ASSERT(!MATCHING || gInhalableStarStates == 0x0873E280, "MATCHING: game_tbl_0873e280 moved")
    ASSERT(!MATCHING || gInhalableStarStateUpdates == 0x0873E284, "MATCHING: game_tbl_0873e284 moved")
    ASSERT(!MATCHING || game_rodata_0873e284 == 0x0873E288, "MATCHING: game_rodata_0873e284 moved")
    ASSERT(!MATCHING || gHeldPlayerStates == 0x0873E2F0, "MATCHING: game_tbl_0873e2f0 moved")
    ASSERT(!MATCHING || game_rodata_0873e348 == 0x0873E348, "MATCHING: game_rodata_0873e348 moved")
    ASSERT(!MATCHING || gActorDefeats == 0x0873E5BC, "MATCHING: game_tbl_0873e5bc moved")
    ASSERT(!MATCHING || game_rodata_0873e5f8 == 0x0873E5F8, "MATCHING: game_rodata_0873e5f8 moved")
    ASSERT(!MATCHING || gActorDefeatFrozenStates == 0x0873E670, "MATCHING: game_tbl_0873e670 moved")
    ASSERT(!MATCHING || game_rodata_0873e698 == 0x0873E698, "MATCHING: game_rodata_0873e698 moved")
    ASSERT(!MATCHING || gUnk_0873E734 == 0x0873E734, "MATCHING: game_tbl_0873e734 moved")
    ASSERT(!MATCHING || game_rodata_0873e7a4 == 0x0873E7A4, "MATCHING: game_rodata_0873e7a4 moved")
    ASSERT(!MATCHING || gActorAttachedStates == 0x0873EAA0, "MATCHING: game_tbl_0873eaa0 moved")
    ASSERT(!MATCHING || game_rodata_0873eac0 == 0x0873EAC0, "MATCHING: game_rodata_0873eac0 moved")
    ASSERT(!MATCHING || gLandingImpactVariants == 0x0873ECE0, "MATCHING: game_tbl_0873ece0 moved")

    /* actor_defs - functional table in C (src/data/actor_defs.c, carved by tools/carve_data.py) */
    .actor_defs : {
        build/src/data/actor_defs.o(.rodata)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_defs) == 0x0873ECEC, "MATCHING: .actor_defs moved")

    /* Actor and enemy rodata, M17-M32 (#36 phase 2 run 3; formerly asset_metadata_index_0873eea0): behaviour tables, ActorDef/ActorAux records, HitBoxSets (ldgroup.py: 305 rows in one output section) */
    .actor_rodata : {
        KEEP(*(.actor_rodata))
        build/src/data/actor_records.o(.actor_rec_0873f2b8)
        KEEP(*(.actor_rodata_0873f4c8))
        build/src/data/actor_handlers.o(.actor_tbl_0873f5fc)
        build/src/data/actor_records.o(.actor_rec_0873f664)
        KEEP(*(.actor_rodata_0873f6e8))
        build/src/data/actor_records.o(.actor_rec_0873f8ec)
        build/src/data/actor_handlers.o(.actor_tbl_0873f8f4)
        build/src/data/actor_handlers.o(.actor_tbl_0873f910)
        build/src/data/actor_handlers.o(.actor_tbl_0873f92c)
        KEEP(*(.actor_rodata_0873f92c))
        build/src/data/actor_tables.o(.actor_tbl_0873fb04)
        KEEP(*(.actor_rodata_0873fb7c))
        build/src/data/actor_tables.o(.actor_tbl_0873fbac)
        KEEP(*(.actor_rodata_0873fcf8))
        build/src/data/actor_tables.o(.actor_tbl_087400b0)
        KEEP(*(.actor_rodata_087400e4))
        build/src/data/actor_tables.o(.actor_tbl_08740100)
        KEEP(*(.actor_rodata_08740124))
        build/src/data/actor_tables.o(.actor_tbl_087402bc)
        KEEP(*(.actor_rodata_08740320))
        build/src/data/actor_tables.o(.actor_tbl_08740630)
        KEEP(*(.actor_rodata_08740648))
        build/src/data/actor_tables.o(.actor_tbl_08740658)
        KEEP(*(.actor_rodata_08740668))
        build/src/data/actor_tables.o(.actor_tbl_08740670)
        KEEP(*(.actor_rodata_08740680))
        build/src/data/actor_tables.o(.actor_tbl_087406c4)
        KEEP(*(.actor_rodata_087406ec))
        build/src/data/actor_tables.o(.actor_tbl_08740700)
        KEEP(*(.actor_rodata_08740720))
        build/src/data/actor_tables.o(.actor_tbl_08740758)
        KEEP(*(.actor_rodata_087407bc))
        build/src/data/actor_tables.o(.actor_tbl_087407e4)
        KEEP(*(.actor_rodata_0874080c))
        build/src/data/actor_tables.o(.actor_tbl_08740810)
        KEEP(*(.actor_rodata_08740824))
        build/src/data/actor_tables.o(.actor_tbl_08740960)
        KEEP(*(.actor_rodata_0874099c))
        build/src/data/actor_tables.o(.actor_tbl_087409fc)
        KEEP(*(.actor_rodata_08740a80))
        build/src/data/actor_tables.o(.actor_tbl_08740ac8)
        KEEP(*(.actor_rodata_08740b08))
        build/src/data/actor_tables.o(.actor_tbl_08740b84)
        build/src/data/actor_records.o(.actor_rec_08740bd4)
        KEEP(*(.actor_rodata_08740de4))
        build/src/data/actor_handlers.o(.actor_tbl_08740e54)
        KEEP(*(.actor_rodata_08740fb0))
        build/src/data/actor_tables.o(.actor_tbl_08741088)
        KEEP(*(.actor_rodata_08741094))
        build/src/data/actor_tables.o(.actor_tbl_087410ac)
        KEEP(*(.actor_rodata_087410c0))
        build/src/data/actor_records.o(.actor_rec_087410e4)
        KEEP(*(.actor_rodata_0874113c))
        build/src/data/actor_handlers.o(.actor_tbl_087411b4)
        build/src/data/actor_tables.o(.actor_tbl_087411c0)
        KEEP(*(.actor_rodata_08741214))
        build/src/data/actor_tables.o(.actor_tbl_08741220)
        KEEP(*(.actor_rodata_08741298))
        build/src/data/actor_tables.o(.actor_tbl_087412bc)
        KEEP(*(.actor_rodata_087412ec))
        build/src/data/actor_tables.o(.actor_tbl_08741300)
        KEEP(*(.actor_rodata_08741318))
        build/src/data/actor_tables.o(.actor_tbl_08741380)
        KEEP(*(.actor_rodata_087413d8))
        build/src/data/actor_tables.o(.actor_tbl_08741488)
        KEEP(*(.actor_rodata_087414b0))
        build/src/data/actor_tables.o(.actor_tbl_087414b4)
        KEEP(*(.actor_rodata_087414f8))
        build/src/data/actor_tables.o(.actor_tbl_08741544)
        KEEP(*(.actor_rodata_087415ac))
        build/src/data/actor_tables.o(.actor_tbl_087415b8)
        KEEP(*(.actor_rodata_087415e4))
        build/src/data/actor_tables.o(.actor_tbl_08741604)
        KEEP(*(.actor_rodata_08741628))
        build/src/data/actor_tables.o(.actor_tbl_08741640)
        KEEP(*(.actor_rodata_08741684))
        build/src/data/actor_tables.o(.actor_tbl_0874176c)
        build/src/data/actor_records.o(.actor_rec_087417b8)
        KEEP(*(.actor_rodata_08741aa4))
        build/src/data/actor_handlers.o(.actor_tbl_08741b64)
        KEEP(*(.actor_rodata_08741d64))
        build/src/data/actor_tables.o(.actor_tbl_08741e64)
        KEEP(*(.actor_rodata_08741e70))
        build/src/data/actor_tables.o(.actor_tbl_08741e7c)
        KEEP(*(.actor_rodata_08741e94))
        build/src/data/actor_records.o(.actor_rec_08741eb4)
        KEEP(*(.actor_rodata_08741f0c))
        build/src/data/actor_handlers.o(.actor_tbl_08741f4c)
        build/src/data/actor_tables.o(.actor_tbl_08741f70)
        KEEP(*(.actor_rodata_08741fa8))
        build/src/data/actor_tables.o(.actor_tbl_08741fb8)
        KEEP(*(.actor_rodata_08742010))
        build/src/data/actor_tables.o(.actor_tbl_08742030)
        KEEP(*(.actor_rodata_08742050))
        build/src/data/actor_tables.o(.actor_tbl_08742064)
        KEEP(*(.actor_rodata_08742088))
        build/src/data/actor_tables.o(.actor_tbl_087420a0)
        KEEP(*(.actor_rodata_087420ac))
        build/src/data/actor_tables.o(.actor_tbl_087420bc)
        KEEP(*(.actor_rodata_087420c0))
        build/src/data/actor_tables.o(.actor_tbl_087420f0)
        KEEP(*(.actor_rodata_087420f4))
        build/src/data/actor_tables.o(.actor_tbl_08742100)
        KEEP(*(.actor_rodata_0874210c))
        build/src/data/actor_tables.o(.actor_tbl_0874212c)
        KEEP(*(.actor_rodata_08742144))
        build/src/data/actor_tables.o(.actor_tbl_08742570)
        KEEP(*(.actor_rodata_08742598))
        build/src/data/actor_tables.o(.actor_tbl_087425b0)
        KEEP(*(.actor_rodata_087425b8))
        build/src/data/actor_tables.o(.actor_tbl_087425d0)
        KEEP(*(.actor_rodata_087425dc))
        build/src/data/actor_tables.o(.actor_tbl_087425ec)
        KEEP(*(.actor_rodata_087425f0))
        build/src/data/actor_tables.o(.actor_tbl_087425f8)
        KEEP(*(.actor_rodata_08742600))
        build/src/data/actor_tables.o(.actor_tbl_0874260c)
        KEEP(*(.actor_rodata_08742614))
        build/src/data/actor_tables.o(.actor_tbl_0874263c)
        KEEP(*(.actor_rodata_0874269c))
        build/src/data/actor_tables.o(.actor_tbl_087426ac)
        KEEP(*(.actor_rodata_087426ec))
        build/src/data/actor_tables.o(.actor_tbl_08742704)
        KEEP(*(.actor_rodata_08742734))
        build/src/data/actor_tables.o(.actor_tbl_0874273c)
        KEEP(*(.actor_rodata_0874276c))
        build/src/data/actor_tables.o(.actor_tbl_08742798)
        KEEP(*(.actor_rodata_087427b0))
        build/src/data/actor_tables.o(.actor_tbl_087427b4)
        KEEP(*(.actor_rodata_08742814))
        build/src/data/actor_tables.o(.actor_tbl_087428c0)
        KEEP(*(.actor_rodata_087428e8))
        build/src/data/actor_tables.o(.actor_tbl_08742908)
        KEEP(*(.actor_rodata_08742930))
        build/src/data/actor_tables.o(.actor_tbl_08742940)
        KEEP(*(.actor_rodata_08742998))
        build/src/data/actor_records.o(.actor_rec_087429e8)
        KEEP(*(.actor_rodata_08742bf8))
        build/src/data/actor_handlers.o(.actor_tbl_08742cf0)
        KEEP(*(.actor_rodata_08742ea4))
        build/src/data/actor_records.o(.actor_rec_08742fbc)
        KEEP(*(.actor_rodata_0874306c))
        build/src/data/actor_handlers.o(.actor_tbl_087430ec)
        KEEP(*(.actor_rodata_0874313c))
        build/src/data/actor_tables.o(.actor_tbl_08743188)
        KEEP(*(.actor_rodata_087431cc))
        build/src/data/actor_tables.o(.actor_tbl_087431e4)
        KEEP(*(.actor_rodata_08743214))
        build/src/data/actor_tables.o(.actor_tbl_08743224)
        KEEP(*(.actor_rodata_08743248))
        build/src/data/actor_tables.o(.actor_tbl_08743284)
        KEEP(*(.actor_rodata_087432ec))
        build/src/data/actor_tables.o(.actor_tbl_087432f4)
        build/src/data/actor_records.o(.actor_rec_0874330c)
        KEEP(*(.actor_rodata_08743414))
        build/src/data/actor_handlers.o(.actor_tbl_087434c4)
        KEEP(*(.actor_rodata_08743588))
        build/src/data/actor_tables.o(.actor_tbl_08743600)
        KEEP(*(.actor_rodata_08743614))
        build/src/data/actor_tables.o(.actor_tbl_0874362c)
        build/src/data/actor_records.o(.actor_rec_08743644)
        KEEP(*(.actor_rodata_0874369c))
        build/src/data/actor_handlers.o(.actor_tbl_087436e4)
        KEEP(*(.actor_rodata_08743734))
        build/src/data/actor_tables.o(.actor_tbl_08743848)
        KEEP(*(.actor_rodata_087438a4))
        build/src/data/actor_tables.o(.actor_tbl_08743984)
        KEEP(*(.actor_rodata_087439c0))
        build/src/data/actor_tables.o(.actor_tbl_08743adc)
        KEEP(*(.actor_rodata_08743b48))
        build/src/data/actor_records.o(.actor_rec_08743b4c)
        KEEP(*(.actor_rodata_08743bd0))
        build/src/data/actor_records.o(.actor_rec_0874402c)
        build/src/data/actor_handlers.o(.actor_tbl_0874407c)
        KEEP(*(.actor_rodata_0874407c))
        build/src/data/actor_tables.o(.actor_tbl_08744170)
        KEEP(*(.actor_rodata_0874417c))
        build/src/data/actor_tables.o(.actor_tbl_087441a4)
        KEEP(*(.actor_rodata_087441bc))
        build/src/data/actor_tables.o(.actor_tbl_087441cc)
        build/src/data/actor_records.o(.actor_rec_087441d8)
        KEEP(*(.actor_rodata_0874425c))
        build/src/data/actor_handlers.o(.actor_tbl_087442c8)
        KEEP(*(.actor_rodata_0874433c))
        build/src/data/actor_tables.o(.actor_tbl_08744440)
        KEEP(*(.actor_rodata_0874449c))
        build/src/data/actor_tables.o(.actor_tbl_087444e4)
        KEEP(*(.actor_rodata_08744510))
        build/src/data/actor_tables.o(.actor_tbl_08744564)
        KEEP(*(.actor_rodata_08744598))
        build/src/data/actor_tables.o(.actor_tbl_0874489c)
        KEEP(*(.actor_rodata_087448e4))
        build/src/data/actor_records.o(.actor_rec_08744964)
        KEEP(*(.actor_rodata_087449e8))
        build/src/data/actor_records.o(.actor_rec_0874531c)
        build/src/data/actor_handlers.o(.actor_tbl_087453bc)
        KEEP(*(.actor_rodata_087453bc))
        build/src/data/actor_records.o(.actor_rec_08745530)
        KEEP(*(.actor_rodata_08745588))
        build/src/data/actor_handlers.o(.actor_tbl_087455c8)
        KEEP(*(.actor_rodata_08745618))
        build/src/data/actor_tables.o(.actor_tbl_08745630)
        KEEP(*(.actor_rodata_087456cc))
        build/src/data/actor_tables.o(.actor_tbl_0874574c)
        build/src/data/actor_records.o(.actor_rec_08745810)
        KEEP(*(.actor_rodata_08745868))
        build/src/data/actor_records.o(.actor_rec_08745a2c)
        KEEP(*(.actor_rodata_08745a34))
        build/src/data/actor_handlers.o(.actor_tbl_08745a3c)
        KEEP(*(.actor_rodata_08745aa4))
        build/src/data/actor_tables.o(.actor_tbl_08745ae4)
        KEEP(*(.actor_rodata_08745b0c))
        build/src/data/actor_tables.o(.actor_tbl_08745b1c)
        build/src/data/actor_records.o(.actor_rec_08745b30)
        KEEP(*(.actor_rodata_08745bb4))
        build/src/data/actor_handlers.o(.actor_tbl_08745c90)
        KEEP(*(.actor_rodata_08745cd4))
        build/src/data/actor_handlers.o(.actor_tbl_08745ce0)
        KEEP(*(.actor_rodata_08745cec))
        build/src/data/actor_tables.o(.actor_tbl_08747aa4)
        KEEP(*(.actor_rodata_08747b10))
        build/src/data/actor_tables.o(.actor_tbl_08747bcc)
        KEEP(*(.actor_rodata_08747c28))
        build/src/data/actor_tables.o(.actor_tbl_08747c6c)
        build/src/data/actor_records.o(.actor_rec_08747c80)
        KEEP(*(.actor_rodata_08747ebc))
        build/src/data/actor_records.o(.actor_rec_08748198)
        build/src/data/actor_handlers.o(.actor_tbl_087481a0)
        build/src/data/actor_tables.o(.actor_tbl_08748264)
        KEEP(*(.actor_rodata_08748268))
        build/src/data/actor_tables.o(.actor_tbl_0874844c)
        KEEP(*(.actor_rodata_087484a4))
        build/src/data/actor_tables.o(.actor_tbl_087484c4)
        KEEP(*(.actor_rodata_087484e4))
        build/src/data/actor_tables.o(.actor_tbl_08748624)
        build/src/data/actor_records.o(.actor_rec_08748764)
        KEEP(*(.actor_rodata_087487e8))
        build/src/data/actor_records.o(.actor_rec_08748918)
        build/src/data/actor_handlers.o(.actor_tbl_08748930)
        KEEP(*(.actor_rodata_08748930))
        build/src/data/actor_tables.o(.actor_tbl_087489b4)
        KEEP(*(.actor_rodata_087489c0))
        build/src/data/actor_tables.o(.actor_tbl_087489d4)
        KEEP(*(.actor_rodata_08748a00))
        build/src/data/actor_tables.o(.actor_tbl_08748a28)
        KEEP(*(.actor_rodata_08748a38))
        build/src/data/actor_tables.o(.actor_tbl_08748a54)
        KEEP(*(.actor_rodata_08748a80))
        build/src/data/actor_records.o(.actor_rec_08748ab0)
        KEEP(*(.actor_rodata_08748bb8))
        build/src/data/actor_handlers.o(.actor_tbl_08748cf8)
        KEEP(*(.actor_rodata_08748d28))
        build/src/data/actor_tables.o(.actor_tbl_08748eb8)
        KEEP(*(.actor_rodata_08748f8c))
        build/src/data/actor_tables.o(.actor_tbl_08749150)
        KEEP(*(.actor_rodata_087491a0))
        build/src/data/actor_tables.o(.actor_tbl_087493f4)
        KEEP(*(.actor_rodata_08749458))
        build/src/data/actor_records.o(.actor_rec_08749514)
        KEEP(*(.actor_rodata_08749598))
        build/src/data/actor_records.o(.actor_rec_08749ae8)
        build/src/data/actor_handlers.o(.actor_tbl_08749b08)
        KEEP(*(.actor_rodata_08749b08))
        build/src/data/actor_tables.o(.actor_tbl_08749b8c)
        KEEP(*(.actor_rodata_08749bac))
        build/src/data/actor_tables.o(.actor_tbl_08749be4)
        build/src/data/actor_records.o(.actor_rec_08749bec)
        KEEP(*(.actor_rodata_08749c70))
        build/src/data/actor_handlers.o(.actor_tbl_08749cd4)
        KEEP(*(.actor_rodata_08749cec))
        build/src/data/actor_tables.o(.actor_tbl_08749d10)
        KEEP(*(.actor_rodata_08749d1c))
        build/src/data/actor_tables.o(.actor_tbl_0874ad34)
        KEEP(*(.actor_rodata_0874ad44))
        build/src/data/actor_tables.o(.actor_tbl_0874ad74)
        KEEP(*(.actor_rodata_0874b1a8))
        build/src/data/actor_tables.o(.actor_tbl_0874b1dc)
        KEEP(*(.actor_rodata_0874b20c))
        build/src/data/actor_records.o(.actor_rec_0874b2dc)
        KEEP(*(.actor_rodata_0874b38c))
        build/src/data/actor_records.o(.actor_rec_0874b4c8)
        KEEP(*(.actor_rodata_0874b4d8))
        build/src/data/actor_handlers.o(.actor_tbl_0874b4e0)
        KEEP(*(.actor_rodata_0874b51c))
        build/src/data/actor_tables.o(.actor_tbl_0874b540)
        KEEP(*(.actor_rodata_0874b560))
        build/src/data/actor_tables.o(.actor_tbl_0874b5e4)
        KEEP(*(.actor_rodata_0874b614))
        build/src/data/actor_records.o(.actor_rec_0874b940)
        KEEP(*(.actor_rodata_0874bb7c))
        build/src/data/actor_handlers.o(.actor_tbl_0874bf58)
        KEEP(*(.actor_rodata_0874bff8))
        build/src/data/actor_tables.o(.actor_tbl_0874c12c)
        build/src/data/actor_records.o(.actor_rec_0874c158)
        KEEP(*(.actor_rodata_0874c184))
        build/src/data/actor_records.o(.actor_rec_0874c1fc)
        build/src/data/actor_handlers.o(.actor_tbl_0874c204)
        build/src/data/actor_tables.o(.actor_tbl_0874c21c)
        KEEP(*(.actor_rodata_0874c240))
        build/src/data/actor_tables.o(.actor_tbl_0874c254)
        KEEP(*(.actor_rodata_0874c260))
        build/src/data/actor_records.o(.actor_rec_0874c378)
        KEEP(*(.actor_rodata_0874c3d0))
        build/src/data/actor_handlers.o(.actor_tbl_0874c418)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_rodata) == 0x0873EEA0, "MATCHING: .actor_rodata moved")
    ASSERT(!MATCHING || gAbilityStarDef == 0x0873F2B8, "MATCHING: actor_rec_0873f2b8 moved")
    ASSERT(!MATCHING || actor_rodata_0873f4c8 == 0x0873F4C8, "MATCHING: actor_rodata_0873f4c8 moved")
    ASSERT(!MATCHING || gAbilityStarTerrainHandlers == 0x0873F5FC, "MATCHING: actor_tbl_0873f5fc moved")
    ASSERT(!MATCHING || gInhalableStarDef == 0x0873F664, "MATCHING: actor_rec_0873f664 moved")
    ASSERT(!MATCHING || actor_rodata_0873f6e8 == 0x0873F6E8, "MATCHING: actor_rodata_0873f6e8 moved")
    ASSERT(!MATCHING || gUnk_0873F8EC == 0x0873F8EC, "MATCHING: actor_rec_0873f8ec moved")
    ASSERT(!MATCHING || gNullTerrainHandlers == 0x0873F8F4, "MATCHING: actor_tbl_0873f8f4 moved")
    ASSERT(!MATCHING || gActorDrownTerrainHandlers == 0x0873F910, "MATCHING: actor_tbl_0873f910 moved")
    ASSERT(!MATCHING || gUnk_0873F92C == 0x0873F92C, "MATCHING: actor_tbl_0873f92c moved")
    ASSERT(!MATCHING || actor_rodata_0873f92c == 0x0873F950, "MATCHING: actor_rodata_0873f92c moved")
    ASSERT(!MATCHING || gPlayerWarpStarRideStates == 0x0873FB04, "MATCHING: actor_tbl_0873fb04 moved")
    ASSERT(!MATCHING || actor_rodata_0873fb7c == 0x0873FB7C, "MATCHING: actor_rodata_0873fb7c moved")
    ASSERT(!MATCHING || gWarpStarStates == 0x0873FBAC, "MATCHING: actor_tbl_0873fbac moved")
    ASSERT(!MATCHING || actor_rodata_0873fcf8 == 0x0873FCF8, "MATCHING: actor_rodata_0873fcf8 moved")
    ASSERT(!MATCHING || gPlayerCannonStates == 0x087400B0, "MATCHING: actor_tbl_087400b0 moved")
    ASSERT(!MATCHING || actor_rodata_087400e4 == 0x087400E4, "MATCHING: actor_rodata_087400e4 moved")
    ASSERT(!MATCHING || gCannonStates == 0x08740100, "MATCHING: actor_tbl_08740100 moved")
    ASSERT(!MATCHING || actor_rodata_08740124 == 0x08740124, "MATCHING: actor_rodata_08740124 moved")
    ASSERT(!MATCHING || gCannonFuseStates == 0x087402BC, "MATCHING: actor_tbl_087402bc moved")
    ASSERT(!MATCHING || actor_rodata_08740320 == 0x08740320, "MATCHING: actor_rodata_08740320 moved")
    ASSERT(!MATCHING || gWaddleDeeVariants == 0x08740630, "MATCHING: actor_tbl_08740630 moved")
    ASSERT(!MATCHING || actor_rodata_08740648 == 0x08740648, "MATCHING: actor_rodata_08740648 moved")
    ASSERT(!MATCHING || gWaddleDeeWalkStates == 0x08740658, "MATCHING: actor_tbl_08740658 moved")
    ASSERT(!MATCHING || actor_rodata_08740668 == 0x08740668, "MATCHING: actor_rodata_08740668 moved")
    ASSERT(!MATCHING || gWaddleDeePaceStates == 0x08740670, "MATCHING: actor_tbl_08740670 moved")
    ASSERT(!MATCHING || actor_rodata_08740680 == 0x08740680, "MATCHING: actor_rodata_08740680 moved")
    ASSERT(!MATCHING || gWaddleDeeJumpStates == 0x087406C4, "MATCHING: actor_tbl_087406c4 moved")
    ASSERT(!MATCHING || actor_rodata_087406ec == 0x087406EC, "MATCHING: actor_rodata_087406ec moved")
    ASSERT(!MATCHING || gWaddleDeeIdleStates == 0x08740700, "MATCHING: actor_tbl_08740700 moved")
    ASSERT(!MATCHING || actor_rodata_08740720 == 0x08740720, "MATCHING: actor_rodata_08740720 moved")
    ASSERT(!MATCHING || gPengyStates == 0x08740758, "MATCHING: actor_tbl_08740758 moved")
    ASSERT(!MATCHING || actor_rodata_087407bc == 0x087407BC, "MATCHING: actor_rodata_087407bc moved")
    ASSERT(!MATCHING || gSparkyJumpStates == 0x087407E4, "MATCHING: actor_tbl_087407e4 moved")
    ASSERT(!MATCHING || actor_rodata_0874080c == 0x0874080C, "MATCHING: actor_rodata_0874080c moved")
    ASSERT(!MATCHING || gSparkyStandStates == 0x08740810, "MATCHING: actor_tbl_08740810 moved")
    ASSERT(!MATCHING || actor_rodata_08740824 == 0x08740824, "MATCHING: actor_rodata_08740824 moved")
    ASSERT(!MATCHING || gScarfyStates == 0x08740960, "MATCHING: actor_tbl_08740960 moved")
    ASSERT(!MATCHING || actor_rodata_0874099c == 0x0874099C, "MATCHING: actor_rodata_0874099c moved")
    ASSERT(!MATCHING || gSwordAndBladeKnightWalkStates == 0x087409FC, "MATCHING: actor_tbl_087409fc moved")
    ASSERT(!MATCHING || actor_rodata_08740a80 == 0x08740A80, "MATCHING: actor_rodata_08740a80 moved")
    ASSERT(!MATCHING || gNeedlousStates == 0x08740AC8, "MATCHING: actor_tbl_08740ac8 moved")
    ASSERT(!MATCHING || actor_rodata_08740b08 == 0x08740B08, "MATCHING: actor_rodata_08740b08 moved")
    ASSERT(!MATCHING || gUFOStates == 0x08740B84, "MATCHING: actor_tbl_08740b84 moved")
    ASSERT(!MATCHING || gWaddleDeeDef == 0x08740BD4, "MATCHING: actor_rec_08740bd4 moved")
    ASSERT(!MATCHING || actor_rodata_08740de4 == 0x08740DE4, "MATCHING: actor_rodata_08740de4 moved")
    ASSERT(!MATCHING || gWaddleDeeTerrainHandlers == 0x08740E54, "MATCHING: actor_tbl_08740e54 moved")
    ASSERT(!MATCHING || actor_rodata_08740fb0 == 0x08740FB0, "MATCHING: actor_rodata_08740fb0 moved")
    ASSERT(!MATCHING || gPengyIceBreathVariants == 0x08741088, "MATCHING: actor_tbl_08741088 moved")
    ASSERT(!MATCHING || actor_rodata_08741094 == 0x08741094, "MATCHING: actor_rodata_08741094 moved")
    ASSERT(!MATCHING || gUFOLaserVariants == 0x087410AC, "MATCHING: actor_tbl_087410ac moved")
    ASSERT(!MATCHING || actor_rodata_087410c0 == 0x087410C0, "MATCHING: actor_rodata_087410c0 moved")
    ASSERT(!MATCHING || gPengyIceBreathDef == 0x087410E4, "MATCHING: actor_rec_087410e4 moved")
    ASSERT(!MATCHING || actor_rodata_0874113c == 0x0874113C, "MATCHING: actor_rodata_0874113c moved")
    ASSERT(!MATCHING || gUFOLaserHitReactions == 0x087411B4, "MATCHING: actor_tbl_087411b4 moved")
    ASSERT(!MATCHING || gRockyVariants == 0x087411C0, "MATCHING: actor_tbl_087411c0 moved")
    ASSERT(!MATCHING || actor_rodata_08741214 == 0x08741214, "MATCHING: actor_rodata_08741214 moved")
    ASSERT(!MATCHING || gSirKibbleEnterStates == 0x08741220, "MATCHING: actor_tbl_08741220 moved")
    ASSERT(!MATCHING || actor_rodata_08741298 == 0x08741298, "MATCHING: actor_rodata_08741298 moved")
    ASSERT(!MATCHING || gGordoVariants == 0x087412BC, "MATCHING: actor_tbl_087412bc moved")
    ASSERT(!MATCHING || actor_rodata_087412ec == 0x087412EC, "MATCHING: actor_rodata_087412ec moved")
    ASSERT(!MATCHING || gCoolSpookVariants == 0x08741300, "MATCHING: actor_tbl_08741300 moved")
    ASSERT(!MATCHING || actor_rodata_08741318 == 0x08741318, "MATCHING: actor_rodata_08741318 moved")
    ASSERT(!MATCHING || gKabuVariants == 0x08741380, "MATCHING: actor_tbl_08741380 moved")
    ASSERT(!MATCHING || actor_rodata_087413d8 == 0x087413D8, "MATCHING: actor_rodata_087413d8 moved")
    ASSERT(!MATCHING || gTwisterVariants == 0x08741488, "MATCHING: actor_tbl_08741488 moved")
    ASSERT(!MATCHING || actor_rodata_087414b0 == 0x087414B0, "MATCHING: actor_rodata_087414b0 moved")
    ASSERT(!MATCHING || gHotHeadVariants == 0x087414B4, "MATCHING: actor_tbl_087414b4 moved")
    ASSERT(!MATCHING || actor_rodata_087414f8 == 0x087414F8, "MATCHING: actor_rodata_087414f8 moved")
    ASSERT(!MATCHING || gStarmanVariants == 0x08741544, "MATCHING: actor_tbl_08741544 moved")
    ASSERT(!MATCHING || actor_rodata_087415ac == 0x087415AC, "MATCHING: actor_rodata_087415ac moved")
    ASSERT(!MATCHING || gPoppyBrosJrVariants == 0x087415B8, "MATCHING: actor_tbl_087415b8 moved")
    ASSERT(!MATCHING || actor_rodata_087415e4 == 0x087415E4, "MATCHING: actor_rodata_087415e4 moved")
    ASSERT(!MATCHING || gPoppyBrosJrRideVariants == 0x08741604, "MATCHING: actor_tbl_08741604 moved")
    ASSERT(!MATCHING || actor_rodata_08741628 == 0x08741628, "MATCHING: actor_rodata_08741628 moved")
    ASSERT(!MATCHING || gWheelieVariants == 0x08741640, "MATCHING: actor_tbl_08741640 moved")
    ASSERT(!MATCHING || actor_rodata_08741684 == 0x08741684, "MATCHING: actor_rodata_08741684 moved")
    ASSERT(!MATCHING || gFlamerVariants == 0x0874176C, "MATCHING: actor_tbl_0874176c moved")
    ASSERT(!MATCHING || gRockyDef == 0x087417B8, "MATCHING: actor_rec_087417b8 moved")
    ASSERT(!MATCHING || actor_rodata_08741aa4 == 0x08741AA4, "MATCHING: actor_rodata_08741aa4 moved")
    ASSERT(!MATCHING || gRockyTerrainHandlers == 0x08741B64, "MATCHING: actor_tbl_08741b64 moved")
    ASSERT(!MATCHING || actor_rodata_08741d64 == 0x08741D64, "MATCHING: actor_rodata_08741d64 moved")
    ASSERT(!MATCHING || gSirKibbleCutterVariants == 0x08741E64, "MATCHING: actor_tbl_08741e64 moved")
    ASSERT(!MATCHING || actor_rodata_08741e70 == 0x08741E70, "MATCHING: actor_rodata_08741e70 moved")
    ASSERT(!MATCHING || gHotHeadFireVariants == 0x08741E7C, "MATCHING: actor_tbl_08741e7c moved")
    ASSERT(!MATCHING || actor_rodata_08741e94 == 0x08741E94, "MATCHING: actor_rodata_08741e94 moved")
    ASSERT(!MATCHING || gSirKibbleCutterDef == 0x08741EB4, "MATCHING: actor_rec_08741eb4 moved")
    ASSERT(!MATCHING || actor_rodata_08741f0c == 0x08741F0C, "MATCHING: actor_rodata_08741f0c moved")
    ASSERT(!MATCHING || gSirKibbleCutterHitReactions == 0x08741F4C, "MATCHING: actor_tbl_08741f4c moved")
    ASSERT(!MATCHING || gNoddyVariants == 0x08741F70, "MATCHING: actor_tbl_08741f70 moved")
    ASSERT(!MATCHING || actor_rodata_08741fa8 == 0x08741FA8, "MATCHING: actor_rodata_08741fa8 moved")
    ASSERT(!MATCHING || gChillyVariants == 0x08741FB8, "MATCHING: actor_tbl_08741fb8 moved")
    ASSERT(!MATCHING || actor_rodata_08742010 == 0x08742010, "MATCHING: actor_rodata_08742010 moved")
    ASSERT(!MATCHING || gParasolWaddleDooStates == 0x08742030, "MATCHING: actor_tbl_08742030 moved")
    ASSERT(!MATCHING || actor_rodata_08742050 == 0x08742050, "MATCHING: actor_rodata_08742050 moved")
    ASSERT(!MATCHING || gBrontoBurtVariants == 0x08742064, "MATCHING: actor_tbl_08742064 moved")
    ASSERT(!MATCHING || actor_rodata_08742088 == 0x08742088, "MATCHING: actor_rodata_08742088 moved")
    ASSERT(!MATCHING || gBrontoBurtWeaveStates == 0x087420A0, "MATCHING: actor_tbl_087420a0 moved")
    ASSERT(!MATCHING || actor_rodata_087420ac == 0x087420AC, "MATCHING: actor_rodata_087420ac moved")
    ASSERT(!MATCHING || gBrontoBurtSwoopStateUpdates == 0x087420BC, "MATCHING: actor_tbl_087420bc moved")
    ASSERT(!MATCHING || actor_rodata_087420c0 == 0x087420C0, "MATCHING: actor_rodata_087420c0 moved")
    ASSERT(!MATCHING || gBrontoBurtDiagonalStates == 0x087420F0, "MATCHING: actor_tbl_087420f0 moved")
    ASSERT(!MATCHING || actor_rodata_087420f4 == 0x087420F4, "MATCHING: actor_rodata_087420f4 moved")
    ASSERT(!MATCHING || gBrontoBurtDiagonalStateUpdates == 0x08742100, "MATCHING: actor_tbl_08742100 moved")
    ASSERT(!MATCHING || actor_rodata_0874210c == 0x0874210C, "MATCHING: actor_rodata_0874210c moved")
    ASSERT(!MATCHING || gBrontoBurtTakeOffStates == 0x0874212C, "MATCHING: actor_tbl_0874212c moved")
    ASSERT(!MATCHING || actor_rodata_08742144 == 0x08742144, "MATCHING: actor_rodata_08742144 moved")
    ASSERT(!MATCHING || gTwizzyVariants == 0x08742570, "MATCHING: actor_tbl_08742570 moved")
    ASSERT(!MATCHING || actor_rodata_08742598 == 0x08742598, "MATCHING: actor_rodata_08742598 moved")
    ASSERT(!MATCHING || gTwizzyWaveStates == 0x087425B0, "MATCHING: actor_tbl_087425b0 moved")
    ASSERT(!MATCHING || actor_rodata_087425b8 == 0x087425B8, "MATCHING: actor_rodata_087425b8 moved")
    ASSERT(!MATCHING || gTwizzyWeaveStates == 0x087425D0, "MATCHING: actor_tbl_087425d0 moved")
    ASSERT(!MATCHING || actor_rodata_087425dc == 0x087425DC, "MATCHING: actor_rodata_087425dc moved")
    ASSERT(!MATCHING || gTwizzySwoopStateUpdates == 0x087425EC, "MATCHING: actor_tbl_087425ec moved")
    ASSERT(!MATCHING || actor_rodata_087425f0 == 0x087425F0, "MATCHING: actor_rodata_087425f0 moved")
    ASSERT(!MATCHING || gTwizzyDiagonalStates == 0x087425F8, "MATCHING: actor_tbl_087425f8 moved")
    ASSERT(!MATCHING || actor_rodata_08742600 == 0x08742600, "MATCHING: actor_rodata_08742600 moved")
    ASSERT(!MATCHING || gTwizzyChaseStates == 0x0874260C, "MATCHING: actor_tbl_0874260c moved")
    ASSERT(!MATCHING || actor_rodata_08742614 == 0x08742614, "MATCHING: actor_rodata_08742614 moved")
    ASSERT(!MATCHING || gTwizzyTakeOffStates == 0x0874263C, "MATCHING: actor_tbl_0874263c moved")
    ASSERT(!MATCHING || actor_rodata_0874269c == 0x0874269C, "MATCHING: actor_rodata_0874269c moved")
    ASSERT(!MATCHING || gTwizzyHoverStates == 0x087426AC, "MATCHING: actor_tbl_087426ac moved")
    ASSERT(!MATCHING || actor_rodata_087426ec == 0x087426EC, "MATCHING: actor_rodata_087426ec moved")
    ASSERT(!MATCHING || gUnk_08742704 == 0x08742704, "MATCHING: actor_tbl_08742704 moved")
    ASSERT(!MATCHING || actor_rodata_08742734 == 0x08742734, "MATCHING: actor_rodata_08742734 moved")
    ASSERT(!MATCHING || gBubblesVariants == 0x0874273C, "MATCHING: actor_tbl_0874273c moved")
    ASSERT(!MATCHING || actor_rodata_0874276c == 0x0874276C, "MATCHING: actor_rodata_0874276c moved")
    ASSERT(!MATCHING || gGlunkVariants == 0x08742798, "MATCHING: actor_tbl_08742798 moved")
    ASSERT(!MATCHING || actor_rodata_087427b0 == 0x087427B0, "MATCHING: actor_rodata_087427b0 moved")
    ASSERT(!MATCHING || gSlippyVariants == 0x087427B4, "MATCHING: actor_tbl_087427b4 moved")
    ASSERT(!MATCHING || actor_rodata_08742814 == 0x08742814, "MATCHING: actor_rodata_08742814 moved")
    ASSERT(!MATCHING || gBlipperVariants == 0x087428C0, "MATCHING: actor_tbl_087428c0 moved")
    ASSERT(!MATCHING || actor_rodata_087428e8 == 0x087428E8, "MATCHING: actor_rodata_087428e8 moved")
    ASSERT(!MATCHING || gBlipperWaveStates == 0x08742908, "MATCHING: actor_tbl_08742908 moved")
    ASSERT(!MATCHING || actor_rodata_08742930 == 0x08742930, "MATCHING: actor_rodata_08742930 moved")
    ASSERT(!MATCHING || gGipVariants == 0x08742940, "MATCHING: actor_tbl_08742940 moved")
    ASSERT(!MATCHING || actor_rodata_08742998 == 0x08742998, "MATCHING: actor_rodata_08742998 moved")
    ASSERT(!MATCHING || gNoddyDef == 0x087429E8, "MATCHING: actor_rec_087429e8 moved")
    ASSERT(!MATCHING || actor_rodata_08742bf8 == 0x08742BF8, "MATCHING: actor_rodata_08742bf8 moved")
    ASSERT(!MATCHING || gNoddyTerrainHandlers == 0x08742CF0, "MATCHING: actor_tbl_08742cf0 moved")
    ASSERT(!MATCHING || actor_rodata_08742ea4 == 0x08742EA4, "MATCHING: actor_rodata_08742ea4 moved")
    ASSERT(!MATCHING || gChillyFreezeDef == 0x08742FBC, "MATCHING: actor_rec_08742fbc moved")
    ASSERT(!MATCHING || actor_rodata_0874306c == 0x0874306C, "MATCHING: actor_rodata_0874306c moved")
    ASSERT(!MATCHING || gWaddleDooBeamTerrainHandlers == 0x087430EC, "MATCHING: actor_tbl_087430ec moved")
    ASSERT(!MATCHING || actor_rodata_0874313c == 0x0874313C, "MATCHING: actor_rodata_0874313c moved")
    ASSERT(!MATCHING || gBroomHatterVariants == 0x08743188, "MATCHING: actor_tbl_08743188 moved")
    ASSERT(!MATCHING || actor_rodata_087431cc == 0x087431CC, "MATCHING: actor_rodata_087431cc moved")
    ASSERT(!MATCHING || gLaserBallVariants == 0x087431E4, "MATCHING: actor_tbl_087431e4 moved")
    ASSERT(!MATCHING || actor_rodata_08743214 == 0x08743214, "MATCHING: actor_rodata_08743214 moved")
    ASSERT(!MATCHING || gCoconutVariants == 0x08743224, "MATCHING: actor_tbl_08743224 moved")
    ASSERT(!MATCHING || actor_rodata_08743248 == 0x08743248, "MATCHING: actor_rodata_08743248 moved")
    ASSERT(!MATCHING || gShotzoVariants == 0x08743284, "MATCHING: actor_tbl_08743284 moved")
    ASSERT(!MATCHING || actor_rodata_087432ec == 0x087432EC, "MATCHING: actor_rodata_087432ec moved")
    ASSERT(!MATCHING || gConerVariants == 0x087432F4, "MATCHING: actor_tbl_087432f4 moved")
    ASSERT(!MATCHING || gBroomHatterDef == 0x0874330C, "MATCHING: actor_rec_0874330c moved")
    ASSERT(!MATCHING || actor_rodata_08743414 == 0x08743414, "MATCHING: actor_rodata_08743414 moved")
    ASSERT(!MATCHING || gBroomHatterTerrainHandlers == 0x087434C4, "MATCHING: actor_tbl_087434c4 moved")
    ASSERT(!MATCHING || actor_rodata_08743588 == 0x08743588, "MATCHING: actor_rodata_08743588 moved")
    ASSERT(!MATCHING || gLaserBallLaserVariants == 0x08743600, "MATCHING: actor_tbl_08743600 moved")
    ASSERT(!MATCHING || actor_rodata_08743614 == 0x08743614, "MATCHING: actor_rodata_08743614 moved")
    ASSERT(!MATCHING || gShotzoCannonballVariants == 0x0874362C, "MATCHING: actor_tbl_0874362c moved")
    ASSERT(!MATCHING || gLaserBallLaserDef == 0x08743644, "MATCHING: actor_rec_08743644 moved")
    ASSERT(!MATCHING || actor_rodata_0874369c == 0x0874369C, "MATCHING: actor_rodata_0874369c moved")
    ASSERT(!MATCHING || gLaserBallLaserTerrainHandlers == 0x087436E4, "MATCHING: actor_tbl_087436e4 moved")
    ASSERT(!MATCHING || actor_rodata_08743734 == 0x08743734, "MATCHING: actor_rodata_08743734 moved")
    ASSERT(!MATCHING || gBonkersVariants == 0x08743848, "MATCHING: actor_tbl_08743848 moved")
    ASSERT(!MATCHING || actor_rodata_087438a4 == 0x087438A4, "MATCHING: actor_rodata_087438a4 moved")
    ASSERT(!MATCHING || gPoppyBrosSrVariants == 0x08743984, "MATCHING: actor_tbl_08743984 moved")
    ASSERT(!MATCHING || actor_rodata_087439c0 == 0x087439C0, "MATCHING: actor_rodata_087439c0 moved")
    ASSERT(!MATCHING || gBugzzyVariants == 0x08743ADC, "MATCHING: actor_tbl_08743adc moved")
    ASSERT(!MATCHING || actor_rodata_08743b48 == 0x08743B48, "MATCHING: actor_rodata_08743b48 moved")
    ASSERT(!MATCHING || gBonkersDef == 0x08743B4C, "MATCHING: actor_rec_08743b4c moved")
    ASSERT(!MATCHING || actor_rodata_08743bd0 == 0x08743BD0, "MATCHING: actor_rodata_08743bd0 moved")
    ASSERT(!MATCHING || gUnk_0874402C == 0x0874402C, "MATCHING: actor_rec_0874402c moved")
    ASSERT(!MATCHING || gBonkersTerrainHandlers == 0x0874407C, "MATCHING: actor_tbl_0874407c moved")
    ASSERT(!MATCHING || actor_rodata_0874407c == 0x08744118, "MATCHING: actor_rodata_0874407c moved")
    ASSERT(!MATCHING || gBonkersNutVariants == 0x08744170, "MATCHING: actor_tbl_08744170 moved")
    ASSERT(!MATCHING || actor_rodata_0874417c == 0x0874417C, "MATCHING: actor_rodata_0874417c moved")
    ASSERT(!MATCHING || gPoppyBrosSrBombVariants == 0x087441A4, "MATCHING: actor_tbl_087441a4 moved")
    ASSERT(!MATCHING || actor_rodata_087441bc == 0x087441BC, "MATCHING: actor_rodata_087441bc moved")
    ASSERT(!MATCHING || gBugzzyLadybugVariants == 0x087441CC, "MATCHING: actor_tbl_087441cc moved")
    ASSERT(!MATCHING || gBonkersNutDef == 0x087441D8, "MATCHING: actor_rec_087441d8 moved")
    ASSERT(!MATCHING || actor_rodata_0874425c == 0x0874425C, "MATCHING: actor_rodata_0874425c moved")
    ASSERT(!MATCHING || gBonkersNutTerrainHandlers == 0x087442C8, "MATCHING: actor_tbl_087442c8 moved")
    ASSERT(!MATCHING || actor_rodata_0874433c == 0x0874433C, "MATCHING: actor_rodata_0874433c moved")
    ASSERT(!MATCHING || gGrandWheelieVariants == 0x08744440, "MATCHING: actor_tbl_08744440 moved")
    ASSERT(!MATCHING || actor_rodata_0874449c == 0x0874449C, "MATCHING: actor_rodata_0874449c moved")
    ASSERT(!MATCHING || gFireLionStates == 0x087444E4, "MATCHING: actor_tbl_087444e4 moved")
    ASSERT(!MATCHING || actor_rodata_08744510 == 0x08744510, "MATCHING: actor_rodata_08744510 moved")
    ASSERT(!MATCHING || gFireLionStateUpdates == 0x08744564, "MATCHING: actor_tbl_08744564 moved")
    ASSERT(!MATCHING || actor_rodata_08744598 == 0x08744598, "MATCHING: actor_rodata_08744598 moved")
    ASSERT(!MATCHING || gPhanPhanStates == 0x0874489C, "MATCHING: actor_tbl_0874489c moved")
    ASSERT(!MATCHING || actor_rodata_087448e4 == 0x087448E4, "MATCHING: actor_rodata_087448e4 moved")
    ASSERT(!MATCHING || gGrandWheelieDef == 0x08744964, "MATCHING: actor_rec_08744964 moved")
    ASSERT(!MATCHING || actor_rodata_087449e8 == 0x087449E8, "MATCHING: actor_rodata_087449e8 moved")
    ASSERT(!MATCHING || gGrandWheelieAuxFrames4To9 == 0x0874531C, "MATCHING: actor_rec_0874531c moved")
    ASSERT(!MATCHING || gGrandWheelieTerrainHandlers == 0x087453BC, "MATCHING: actor_tbl_087453bc moved")
    ASSERT(!MATCHING || actor_rodata_087453bc == 0x08745458, "MATCHING: actor_rodata_087453bc moved")
    ASSERT(!MATCHING || gGrandWheelieMiniWheelieDef == 0x08745530, "MATCHING: actor_rec_08745530 moved")
    ASSERT(!MATCHING || actor_rodata_08745588 == 0x08745588, "MATCHING: actor_rodata_08745588 moved")
    ASSERT(!MATCHING || gGrandWheelieMiniWheelieTerrainHandlers == 0x087455C8, "MATCHING: actor_tbl_087455c8 moved")
    ASSERT(!MATCHING || actor_rodata_08745618 == 0x08745618, "MATCHING: actor_rodata_08745618 moved")
    ASSERT(!MATCHING || gMrFrostyVariants == 0x08745630, "MATCHING: actor_tbl_08745630 moved")
    ASSERT(!MATCHING || actor_rodata_087456cc == 0x087456CC, "MATCHING: actor_rodata_087456cc moved")
    ASSERT(!MATCHING || gMrTickTockVariants == 0x0874574C, "MATCHING: actor_tbl_0874574c moved")
    ASSERT(!MATCHING || gMrFrostyDef == 0x08745810, "MATCHING: actor_rec_08745810 moved")
    ASSERT(!MATCHING || actor_rodata_08745868 == 0x08745868, "MATCHING: actor_rodata_08745868 moved")
    ASSERT(!MATCHING || gUnk_08745A2C == 0x08745A2C, "MATCHING: actor_rec_08745a2c moved")
    ASSERT(!MATCHING || actor_rodata_08745a34 == 0x08745A34, "MATCHING: actor_rodata_08745a34 moved")
    ASSERT(!MATCHING || gMrFrostyTerrainHandlers == 0x08745A3C, "MATCHING: actor_tbl_08745a3c moved")
    ASSERT(!MATCHING || actor_rodata_08745aa4 == 0x08745AA4, "MATCHING: actor_rodata_08745aa4 moved")
    ASSERT(!MATCHING || gMrFrostyIceCubeVariants == 0x08745AE4, "MATCHING: actor_tbl_08745ae4 moved")
    ASSERT(!MATCHING || actor_rodata_08745b0c == 0x08745B0C, "MATCHING: actor_rodata_08745b0c moved")
    ASSERT(!MATCHING || gMrTickTockNoteVariants == 0x08745B1C, "MATCHING: actor_tbl_08745b1c moved")
    ASSERT(!MATCHING || gMrFrostyIceCubeDef == 0x08745B30, "MATCHING: actor_rec_08745b30 moved")
    ASSERT(!MATCHING || actor_rodata_08745bb4 == 0x08745BB4, "MATCHING: actor_rodata_08745bb4 moved")
    ASSERT(!MATCHING || gMrFrostyIceCubeTerrainHandlers == 0x08745C90, "MATCHING: actor_tbl_08745c90 moved")
    ASSERT(!MATCHING || actor_rodata_08745cd4 == 0x08745CD4, "MATCHING: actor_rodata_08745cd4 moved")
    ASSERT(!MATCHING || gMrTickTockNoteHitReactions == 0x08745CE0, "MATCHING: actor_tbl_08745ce0 moved")
    ASSERT(!MATCHING || actor_rodata_08745cec == 0x08745CEC, "MATCHING: actor_rodata_08745cec moved")
    ASSERT(!MATCHING || gAxeKnightVariants == 0x08747AA4, "MATCHING: actor_tbl_08747aa4 moved")
    ASSERT(!MATCHING || actor_rodata_08747b10 == 0x08747B10, "MATCHING: actor_rodata_08747b10 moved")
    ASSERT(!MATCHING || gMaceKnightVariants == 0x08747BCC, "MATCHING: actor_tbl_08747bcc moved")
    ASSERT(!MATCHING || actor_rodata_08747c28 == 0x08747C28, "MATCHING: actor_rodata_08747c28 moved")
    ASSERT(!MATCHING || gTridentKnightTridentVariants == 0x08747C6C, "MATCHING: actor_tbl_08747c6c moved")
    ASSERT(!MATCHING || gUnk_08747C80 == 0x08747C80, "MATCHING: actor_rec_08747c80 moved")
    ASSERT(!MATCHING || actor_rodata_08747ebc == 0x08747EBC, "MATCHING: actor_rodata_08747ebc moved")
    ASSERT(!MATCHING || gUnk_08748198 == 0x08748198, "MATCHING: actor_rec_08748198 moved")
    ASSERT(!MATCHING || gAxeKnightTerrainHandlers == 0x087481A0, "MATCHING: actor_tbl_087481a0 moved")
    ASSERT(!MATCHING || gKingDededeVariants == 0x08748264, "MATCHING: actor_tbl_08748264 moved")
    ASSERT(!MATCHING || actor_rodata_08748268 == 0x08748268, "MATCHING: actor_rodata_08748268 moved")
    ASSERT(!MATCHING || gKingDededeStates == 0x0874844C, "MATCHING: actor_tbl_0874844c moved")
    ASSERT(!MATCHING || actor_rodata_087484a4 == 0x087484A4, "MATCHING: actor_rodata_087484a4 moved")
    ASSERT(!MATCHING || gKingDededeDefeatedStates == 0x087484C4, "MATCHING: actor_tbl_087484c4 moved")
    ASSERT(!MATCHING || actor_rodata_087484e4 == 0x087484E4, "MATCHING: actor_rodata_087484e4 moved")
    ASSERT(!MATCHING || gMrShineAndMrBrightStates == 0x08748624, "MATCHING: actor_tbl_08748624 moved")
    ASSERT(!MATCHING || gKingDededeDef == 0x08748764, "MATCHING: actor_rec_08748764 moved")
    ASSERT(!MATCHING || actor_rodata_087487e8 == 0x087487E8, "MATCHING: actor_rodata_087487e8 moved")
    ASSERT(!MATCHING || gKingDededeAux == 0x08748918, "MATCHING: actor_rec_08748918 moved")
    ASSERT(!MATCHING || gKingDededeTerrainHandlers == 0x08748930, "MATCHING: actor_tbl_08748930 moved")
    ASSERT(!MATCHING || actor_rodata_08748930 == 0x087489A4, "MATCHING: actor_rodata_08748930 moved")
    ASSERT(!MATCHING || gKingDededeStarVariants == 0x087489B4, "MATCHING: actor_tbl_087489b4 moved")
    ASSERT(!MATCHING || actor_rodata_087489c0 == 0x087489C0, "MATCHING: actor_rodata_087489c0 moved")
    ASSERT(!MATCHING || gKingDededeAirPuffVariants == 0x087489D4, "MATCHING: actor_tbl_087489d4 moved")
    ASSERT(!MATCHING || actor_rodata_08748a00 == 0x08748A00, "MATCHING: actor_rodata_08748a00 moved")
    ASSERT(!MATCHING || gMrShineFallingStarStates == 0x08748A28, "MATCHING: actor_tbl_08748a28 moved")
    ASSERT(!MATCHING || actor_rodata_08748a38 == 0x08748A38, "MATCHING: actor_rodata_08748a38 moved")
    ASSERT(!MATCHING || gMrBrightFireballStates == 0x08748A54, "MATCHING: actor_tbl_08748a54 moved")
    ASSERT(!MATCHING || actor_rodata_08748a80 == 0x08748A80, "MATCHING: actor_rodata_08748a80 moved")
    ASSERT(!MATCHING || gKingDededeStarDef == 0x08748AB0, "MATCHING: actor_rec_08748ab0 moved")
    ASSERT(!MATCHING || actor_rodata_08748bb8 == 0x08748BB8, "MATCHING: actor_rodata_08748bb8 moved")
    ASSERT(!MATCHING || gKingDededeStarHitReactions == 0x08748CF8, "MATCHING: actor_tbl_08748cf8 moved")
    ASSERT(!MATCHING || actor_rodata_08748d28 == 0x08748D28, "MATCHING: actor_rodata_08748d28 moved")
    ASSERT(!MATCHING || gMetaKnightVariants == 0x08748EB8, "MATCHING: actor_tbl_08748eb8 moved")
    ASSERT(!MATCHING || actor_rodata_08748f8c == 0x08748F8C, "MATCHING: actor_rodata_08748f8c moved")
    ASSERT(!MATCHING || gKrackoVariants == 0x08749150, "MATCHING: actor_tbl_08749150 moved")
    ASSERT(!MATCHING || actor_rodata_087491a0 == 0x087491A0, "MATCHING: actor_rodata_087491a0 moved")
    ASSERT(!MATCHING || gNightmareWizardVariants == 0x087493F4, "MATCHING: actor_tbl_087493f4 moved")
    ASSERT(!MATCHING || actor_rodata_08749458 == 0x08749458, "MATCHING: actor_rodata_08749458 moved")
    ASSERT(!MATCHING || gMetaKnightDef == 0x08749514, "MATCHING: actor_rec_08749514 moved")
    ASSERT(!MATCHING || actor_rodata_08749598 == 0x08749598, "MATCHING: actor_rodata_08749598 moved")
    ASSERT(!MATCHING || gMetaKnightAux == 0x08749AE8, "MATCHING: actor_rec_08749ae8 moved")
    ASSERT(!MATCHING || gMetaKnightTerrainHandlers == 0x08749B08, "MATCHING: actor_tbl_08749b08 moved")
    ASSERT(!MATCHING || actor_rodata_08749b08 == 0x08749B6C, "MATCHING: actor_rodata_08749b08 moved")
    ASSERT(!MATCHING || gMetaKnightSwordStates == 0x08749B8C, "MATCHING: actor_tbl_08749b8c moved")
    ASSERT(!MATCHING || actor_rodata_08749bac == 0x08749BAC, "MATCHING: actor_rodata_08749bac moved")
    ASSERT(!MATCHING || gNightmareWizardStarStates == 0x08749BE4, "MATCHING: actor_tbl_08749be4 moved")
    ASSERT(!MATCHING || gMetaKnightSwordDef == 0x08749BEC, "MATCHING: actor_rec_08749bec moved")
    ASSERT(!MATCHING || actor_rodata_08749c70 == 0x08749C70, "MATCHING: actor_rodata_08749c70 moved")
    ASSERT(!MATCHING || gKrackoStarmanHitReactions == 0x08749CD4, "MATCHING: actor_tbl_08749cd4 moved")
    ASSERT(!MATCHING || actor_rodata_08749cec == 0x08749CEC, "MATCHING: actor_rodata_08749cec moved")
    ASSERT(!MATCHING || gPaintRollerStates == 0x08749D10, "MATCHING: actor_tbl_08749d10 moved")
    ASSERT(!MATCHING || actor_rodata_08749d1c == 0x08749D1C, "MATCHING: actor_rodata_08749d1c moved")
    ASSERT(!MATCHING || gNightmarePowerOrbStateUpdates == 0x0874AD34, "MATCHING: actor_tbl_0874ad34 moved")
    ASSERT(!MATCHING || actor_rodata_0874ad44 == 0x0874AD44, "MATCHING: actor_rodata_0874ad44 moved")
    ASSERT(!MATCHING || gUnk_0874AD74 == 0x0874AD74, "MATCHING: actor_tbl_0874ad74 moved")
    ASSERT(!MATCHING || actor_rodata_0874b1a8 == 0x0874B1A8, "MATCHING: actor_rodata_0874b1a8 moved")
    ASSERT(!MATCHING || gNightmarePowerOrbStarVariants == 0x0874B1DC, "MATCHING: actor_tbl_0874b1dc moved")
    ASSERT(!MATCHING || actor_rodata_0874b20c == 0x0874B20C, "MATCHING: actor_rodata_0874b20c moved")
    ASSERT(!MATCHING || gPaintRollerDef == 0x0874B2DC, "MATCHING: actor_rec_0874b2dc moved")
    ASSERT(!MATCHING || actor_rodata_0874b38c == 0x0874B38C, "MATCHING: actor_rodata_0874b38c moved")
    ASSERT(!MATCHING || gPaintRollerAux == 0x0874B4C8, "MATCHING: actor_rec_0874b4c8 moved")
    ASSERT(!MATCHING || actor_rodata_0874b4d8 == 0x0874B4D8, "MATCHING: actor_rodata_0874b4d8 moved")
    ASSERT(!MATCHING || gPaintRollerHitReactions == 0x0874B4E0, "MATCHING: actor_tbl_0874b4e0 moved")
    ASSERT(!MATCHING || actor_rodata_0874b51c == 0x0874B51C, "MATCHING: actor_rodata_0874b51c moved")
    ASSERT(!MATCHING || gPaintRollerPaintingStates == 0x0874B540, "MATCHING: actor_tbl_0874b540 moved")
    ASSERT(!MATCHING || actor_rodata_0874b560 == 0x0874B560, "MATCHING: actor_rodata_0874b560 moved")
    ASSERT(!MATCHING || gHeavyMoleArmStates == 0x0874B5E4, "MATCHING: actor_tbl_0874b5e4 moved")
    ASSERT(!MATCHING || actor_rodata_0874b614 == 0x0874B614, "MATCHING: actor_rodata_0874b614 moved")
    ASSERT(!MATCHING || gPaintRollerPaintingDef == 0x0874B940, "MATCHING: actor_rec_0874b940 moved")
    ASSERT(!MATCHING || actor_rodata_0874bb7c == 0x0874BB7C, "MATCHING: actor_rodata_0874bb7c moved")
    ASSERT(!MATCHING || gPaintRollerPaintingTerrainHandlers == 0x0874BF58, "MATCHING: actor_tbl_0874bf58 moved")
    ASSERT(!MATCHING || actor_rodata_0874bff8 == 0x0874BFF8, "MATCHING: actor_rodata_0874bff8 moved")
    ASSERT(!MATCHING || gWhispyWoodsVariants == 0x0874C12C, "MATCHING: actor_tbl_0874c12c moved")
    ASSERT(!MATCHING || gWhispyWoodsDef == 0x0874C158, "MATCHING: actor_rec_0874c158 moved")
    ASSERT(!MATCHING || actor_rodata_0874c184 == 0x0874C184, "MATCHING: actor_rodata_0874c184 moved")
    ASSERT(!MATCHING || gWhispyWoodsAux == 0x0874C1FC, "MATCHING: actor_rec_0874c1fc moved")
    ASSERT(!MATCHING || gWhispyWoodsHitReactions == 0x0874C204, "MATCHING: actor_tbl_0874c204 moved")
    ASSERT(!MATCHING || gWhispyWoodsAppleVariants == 0x0874C21C, "MATCHING: actor_tbl_0874c21c moved")
    ASSERT(!MATCHING || actor_rodata_0874c240 == 0x0874C240, "MATCHING: actor_rodata_0874c240 moved")
    ASSERT(!MATCHING || gWhispyWoodsAirPuffVariants == 0x0874C254, "MATCHING: actor_tbl_0874c254 moved")
    ASSERT(!MATCHING || actor_rodata_0874c260 == 0x0874C260, "MATCHING: actor_rodata_0874c260 moved")
    ASSERT(!MATCHING || gWhispyWoodsAppleDef == 0x0874C378, "MATCHING: actor_rec_0874c378 moved")
    ASSERT(!MATCHING || actor_rodata_0874c3d0 == 0x0874C3D0, "MATCHING: actor_rodata_0874c3d0 moved")
    ASSERT(!MATCHING || gWhispyWoodsAppleTerrainHandlers == 0x0874C418, "MATCHING: actor_tbl_0874c418 moved")

    /* Sprite frame tables (#36 phase 2 run 3): the struct TaskGfx * arrays the code installs as Task.frameTable, the player's gPlayerFrames among them; functional table in C (src/data/frame_tables.c, carved by tools/carve_data.py) */
    .frame_tables : {
        build/src/data/frame_tables.o(.frame_tables)
    } > ROM
    ASSERT(!MATCHING || ADDR(.frame_tables) == 0x0874C44C, "MATCHING: .frame_tables moved")

    /* Rodata of M33-M38 in link order (#36 phase 2 run 3): pickups, HUD effects, save, sub-games, ending, boot logo objects, game over, credits (ldgroup.py: 45 rows in one output section) */
    .late_game_rodata : {
        build/src/data/late_game_tables.o(.late_tbl_0875607c)
        KEEP(*(.late_game_rodata))
        build/src/data/late_game_tables.o(.late_tbl_087560a0)
        KEEP(*(.late_game_rodata_087560c0))
        build/src/data/late_game_tables.o(.late_tbl_087560d0)
        KEEP(*(.late_game_rodata_087560ec))
        build/src/data/late_game_tables.o(.late_tbl_08756150)
        KEEP(*(.late_game_rodata_08756178))
        build/src/data/late_game_tables.o(.late_tbl_08756198)
        KEEP(*(.late_game_rodata_087561c4))
        build/src/data/late_game_tables.o(.late_tbl_08756270)
        KEEP(*(.late_game_rodata_087562a8))
        build/src/data/late_game_tables.o(.late_tbl_087562cc)
        build/src/data/late_game_tables.o(.late_tbl_087562d8)
        KEEP(*(.late_game_rodata_087562e4))
        build/src/data/late_game_tables.o(.late_tbl_087562e8)
        KEEP(*(.late_game_rodata_087562f0))
        build/src/data/late_game_tables.o(.late_tbl_087562fc)
        KEEP(*(.late_game_rodata_0875636c))
        build/src/data/late_game_tables.o(.late_tbl_08756378)
        KEEP(*(.late_game_rodata_087563d8))
        build/src/data/late_game_tables.o(.late_tbl_08756468)
        KEEP(*(.late_game_rodata_08756498))
        build/src/data/late_game_tables.o(.late_tbl_087564e4)
        KEEP(*(.late_game_rodata_08756514))
        build/src/data/late_game_tables.o(.late_tbl_08756568)
        KEEP(*(.late_game_rodata_08756570))
        build/src/data/late_game_tables.o(.late_tbl_08756668)
        KEEP(*(.late_game_rodata_0875670c))
        build/src/data/late_game_tables.o(.late_tbl_08756780)
        KEEP(*(.late_game_rodata_08756798))
        build/src/data/late_game_tables.o(.late_tbl_087571f8)
        KEEP(*(.late_game_rodata_08757250))
        build/src/data/late_game_tables.o(.late_tbl_08757270)
        KEEP(*(.late_game_rodata_08757280))
        build/src/data/late_game_tables.o(.late_tbl_087572cc)
        KEEP(*(.late_game_rodata_087572e0))
        build/src/data/late_game_tables.o(.late_tbl_08757330)
        KEEP(*(.late_game_rodata_0875735c))
        build/src/data/late_game_tables.o(.late_tbl_087573f4)
        KEEP(*(.late_game_rodata_08757424))
        build/src/data/late_game_tables.o(.late_tbl_08758294)
        KEEP(*(.late_game_rodata_087582f4))
        build/src/data/late_game_tables.o(.late_tbl_08758324)
        KEEP(*(.late_game_rodata_08758334))
    } > ROM
    ASSERT(!MATCHING || ADDR(.late_game_rodata) == 0x0875607C, "MATCHING: .late_game_rodata moved")
    ASSERT(!MATCHING || late_game_rodata == 0x08756084, "MATCHING: late_game_rodata moved")
    ASSERT(!MATCHING || gPickupStates == 0x087560A0, "MATCHING: late_tbl_087560a0 moved")
    ASSERT(!MATCHING || late_game_rodata_087560c0 == 0x087560C0, "MATCHING: late_game_rodata_087560c0 moved")
    ASSERT(!MATCHING || gAbilityStarStates == 0x087560D0, "MATCHING: late_tbl_087560d0 moved")
    ASSERT(!MATCHING || late_game_rodata_087560ec == 0x087560EC, "MATCHING: late_game_rodata_087560ec moved")
    ASSERT(!MATCHING || gStarRodPieceHoverStates == 0x08756150, "MATCHING: late_tbl_08756150 moved")
    ASSERT(!MATCHING || late_game_rodata_08756178 == 0x08756178, "MATCHING: late_game_rodata_08756178 moved")
    ASSERT(!MATCHING || gHBlankScrollEffects == 0x08756198, "MATCHING: late_tbl_08756198 moved")
    ASSERT(!MATCHING || late_game_rodata_087561c4 == 0x087561C4, "MATCHING: late_game_rodata_087561c4 moved")
    ASSERT(!MATCHING || gPlayerLifeRequestStates == 0x08756270, "MATCHING: late_tbl_08756270 moved")
    ASSERT(!MATCHING || late_game_rodata_087562a8 == 0x087562A8, "MATCHING: late_game_rodata_087562a8 moved")
    ASSERT(!MATCHING || gSubGameInitHooks == 0x087562CC, "MATCHING: late_tbl_087562cc moved")
    ASSERT(!MATCHING || gSubGameBodies == 0x087562D8, "MATCHING: late_tbl_087562d8 moved")
    ASSERT(!MATCHING || late_game_rodata_087562e4 == 0x087562E4, "MATCHING: late_game_rodata_087562e4 moved")
    ASSERT(!MATCHING || gQuickDrawPhases == 0x087562E8, "MATCHING: late_tbl_087562e8 moved")
    ASSERT(!MATCHING || late_game_rodata_087562f0 == 0x087562F0, "MATCHING: late_game_rodata_087562f0 moved")
    ASSERT(!MATCHING || gQuickDrawStates == 0x087562FC, "MATCHING: late_tbl_087562fc moved")
    ASSERT(!MATCHING || late_game_rodata_0875636c == 0x0875636C, "MATCHING: late_game_rodata_0875636c moved")
    ASSERT(!MATCHING || gQuickDrawResultsStates == 0x08756378, "MATCHING: late_tbl_08756378 moved")
    ASSERT(!MATCHING || late_game_rodata_087563d8 == 0x087563D8, "MATCHING: late_game_rodata_087563d8 moved")
    ASSERT(!MATCHING || gQuickDrawPlayerStates == 0x08756468, "MATCHING: late_tbl_08756468 moved")
    ASSERT(!MATCHING || late_game_rodata_08756498 == 0x08756498, "MATCHING: late_game_rodata_08756498 moved")
    ASSERT(!MATCHING || gQuickDrawOpponentStates == 0x087564E4, "MATCHING: late_tbl_087564e4 moved")
    ASSERT(!MATCHING || late_game_rodata_08756514 == 0x08756514, "MATCHING: late_game_rodata_08756514 moved")
    ASSERT(!MATCHING || gBombRallyPhases == 0x08756568, "MATCHING: late_tbl_08756568 moved")
    ASSERT(!MATCHING || late_game_rodata_08756570 == 0x08756570, "MATCHING: late_game_rodata_08756570 moved")
    ASSERT(!MATCHING || gBombRallyStates == 0x08756668, "MATCHING: late_tbl_08756668 moved")
    ASSERT(!MATCHING || late_game_rodata_0875670c == 0x0875670C, "MATCHING: late_game_rodata_0875670c moved")
    ASSERT(!MATCHING || gBombRallyBombStates == 0x08756780, "MATCHING: late_tbl_08756780 moved")
    ASSERT(!MATCHING || late_game_rodata_08756798 == 0x08756798, "MATCHING: late_game_rodata_08756798 moved")
    ASSERT(!MATCHING || gBombRallyStarBurstStates == 0x087571F8, "MATCHING: late_tbl_087571f8 moved")
    ASSERT(!MATCHING || late_game_rodata_08757250 == 0x08757250, "MATCHING: late_game_rodata_08757250 moved")
    ASSERT(!MATCHING || gBombRallyMenuItemStates == 0x08757270, "MATCHING: late_tbl_08757270 moved")
    ASSERT(!MATCHING || late_game_rodata_08757280 == 0x08757280, "MATCHING: late_game_rodata_08757280 moved")
    ASSERT(!MATCHING || gAirGrindPhases == 0x087572CC, "MATCHING: late_tbl_087572cc moved")
    ASSERT(!MATCHING || late_game_rodata_087572e0 == 0x087572E0, "MATCHING: late_game_rodata_087572e0 moved")
    ASSERT(!MATCHING || gEndingEpilogueVariants == 0x08757330, "MATCHING: late_tbl_08757330 moved")
    ASSERT(!MATCHING || late_game_rodata_0875735c == 0x0875735C, "MATCHING: late_game_rodata_0875735c moved")
    ASSERT(!MATCHING || gEndingStarRodReturnVariants == 0x087573F4, "MATCHING: late_tbl_087573f4 moved")
    ASSERT(!MATCHING || late_game_rodata_08757424 == 0x08757424, "MATCHING: late_game_rodata_08757424 moved")
    ASSERT(!MATCHING || gGameOverObjectVariants == 0x08758294, "MATCHING: late_tbl_08758294 moved")
    ASSERT(!MATCHING || late_game_rodata_087582f4 == 0x087582F4, "MATCHING: late_game_rodata_087582f4 moved")
    ASSERT(!MATCHING || gUnk_08758324 == 0x08758324, "MATCHING: late_tbl_08758324 moved")
    ASSERT(!MATCHING || late_game_rodata_08758334 == 0x08758334, "MATCHING: late_game_rodata_08758334 moved")

    /* The 14 recorded input demos of the staff credits' rows 1 and 2 (gCreditsDemoRecordings, CreditsMain src/ending_credits.c; InputRecorderStart) */
    .credits_demos : {
        KEEP(*(.credits_demos))
    } > ROM
    ASSERT(!MATCHING || ADDR(.credits_demos) == 0x08758448, "MATCHING: .credits_demos moved")

    /* Player frame records {oam | 1, palette, tiles, palette2, tiles2} (PlayerLoadFrameTilesAndPalette, src/player_helpers.c) (#36 phase 2 run 3) */
    .player_frame_records : {
        KEEP(*(.player_frame_records))
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_frame_records) == 0x08759DC8, "MATCHING: .player_frame_records moved")

    /* The player sheets' frame lists, format-only (docs/data.md 5.3) (#36 phase 2 run 3) */
    .player_frame_lists : {
        KEEP(*(.player_frame_lists))
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_frame_lists) == 0x08769250, "MATCHING: .player_frame_lists moved")

    /* Separately linked GBA program (asset): the single-pak link client, MultiBootInitWithParams(gUnk_0876B1FC, gUnk_0876F690) src/menu_sound_test_link_play.c, ExtraModeTitleLoadMultiBootImage src/mode_extra_mode_title.c */
    .multiboot_program : {
        KEEP(*(.multiboot_program))
    } > ROM
    ASSERT(!MATCHING || ADDR(.multiboot_program) == 0x0876B1FC, "MATCHING: .multiboot_program moved")

    /* Separately linked GBA program (asset): sub-game 0 (Quick Draw), sent to the other players by ExtraModeTitleSendModeData (src/mode_extra_mode_title.c) */
    .quick_draw_program : {
        KEEP(*(.quick_draw_program))
    } > ROM
    ASSERT(!MATCHING || ADDR(.quick_draw_program) == 0x0876F690, "MATCHING: .quick_draw_program moved")

    /* Separately linked GBA program (asset): sub-game 1 (Bomb Rally), sent to the other players by ExtraModeTitleSendModeData (src/mode_extra_mode_title.c) */
    .bomb_rally_program : {
        KEEP(*(.bomb_rally_program))
    } > ROM
    ASSERT(!MATCHING || ADDR(.bomb_rally_program) == 0x087954C0, "MATCHING: .bomb_rally_program moved")

    /* Separately linked GBA program (asset): sub-game 2 (Air Grind), sent to the other players by ExtraModeTitleSendModeData (src/mode_extra_mode_title.c) */
    .air_grind_program : {
        KEEP(*(.air_grind_program))
    } > ROM
    ASSERT(!MATCHING || ADDR(.air_grind_program) == 0x087C0A4C, "MATCHING: .air_grind_program moved")

    /* room_table - functional table in C (src/data/room_table.c, carved by tools/carve_data.py) */
    .room_table : {
        build/src/data/room_table.o(.rodata)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_table) == 0x087E1D58, "MATCHING: .room_table moved")

    /* room_bg_anim_lists - functional table in C (src/data/room_bg_anim_lists.c, carved by tools/carve_data.py) */
    .room_bg_anim_lists : {
        build/src/data/room_bg_anim_lists.o(.rodata)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_bg_anim_lists) == 0x087E1E78, "MATCHING: .room_bg_anim_lists moved")

    /* room_lists - functional table in C (src/data/room_lists.c, carved by tools/carve_data.py) */
    .room_lists : {
        build/src/data/room_lists.o(.rodata)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_lists) == 0x087E1F58, "MATCHING: .room_lists moved")

    /* Sprite frame lists (#36 phase 2): 23 per-sheet lists of frame pointers, one per extended GfxHeader trailer; no code reference found */
    .sprite_frame_lists : {
        KEEP(*(.sprite_frame_lists))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sprite_frame_lists) == 0x087E2570, "MATCHING: .sprite_frame_lists moved")

    /* Zero padding to ROM end */
    .zero_padding : {
        KEEP(*(.zero_padding))
    } > ROM
    ASSERT(!MATCHING || ADDR(.zero_padding) == 0x087E3088, "MATCHING: .zero_padding moved")
    ASSERT(!MATCHING || ADDR(.zero_padding) + SIZEOF(.zero_padding) == 0x08800000, "MATCHING: the ROM does not end at 0x08800000")

    /DISCARD/ : {
        *(*)
    }
}
