#include "global.h"
#include "camera.h"

/* The rooms' BG animation scripts and palette fades (room_bg_anims,
 * 0x08334EC0-0x0835CFD3, issue #167): 30 scripts (struct BgAnimCmd
 * arrays) and 14 palette fades (struct BgAnimPaletteFade) in 40 runs, in
 * address order.  LoadRoomBgAnims (src/camera_bg_anims.c) loads the
 * NULL-ended script list gRoomBgAnimScripts[RoomDef.bgAnimSet] into the
 * slots gBgAnims[]; UpdateBgAnims runs each script one 8-byte command
 * {op, arg, ptr} at a time:
 *   op 0  BgAnimCopyTiles(ptr), ptr a tile frame (struct BgAnimTileFrame:
 *         tile, byte size, tiles), then wait arg + 1 frames;
 *   op 1  BgAnimStartPaletteFade(slot, ptr), ptr a fade, then wait
 *         arg + 1 frames;
 *   op 2  wait arg + 1 frames;
 *   op 3  back to command 0 (the end of a looping script);
 *   op 5  SetCollisionTile(arg >> 8, arg & 0xFF, (u16)ptr): ptr is the
 *         new collision tile, a value (tools/split_config.json
 *         not_pointers);
 *   op 6  PlaySfx(arg);
 *   other BgAnimStop (op 4: the end of a one-shot script).
 * Nothing after a script's op 3 or op 4 is read.  A fade is the 16 bytes
 * BgAnimStartPaletteFade copies: BgAnimStepPaletteFade blends colorCount
 * colours from src to dst into gBgPaletteBank2[colorIndex], rate/256 more of
 * the way each frame.
 *
 * The tile frames and the palettes are assets: they stay baserom slices
 * in data/room_bg_anims.s (one section per data piece between the runs),
 * and this file only names them.
 * Each run (a script, a fade, or a script's last fade followed by the
 * script) is a named section .bg_anim_<address>: linker.ld lists the runs
 * between the data pieces of room_bg_anims inside ONE output section
 * (docs/data.md 5.2).  The records are not const: the scripts are the
 * struct BgAnimCmd * of gRoomBgAnimScripts' lists and of
 * BgAnim.script, a fade is a command's void * and
 * BgAnimStartPaletteFade's parameter, so the qualifier would not survive
 * -Werror; the section attribute is what places them in ROM. */

#define BG_ANIM(addr) __attribute__((section(".bg_anim_" #addr)))

/* Tile frames (struct BgAnimTileFrame, assets): labels of data/room_bg_anims.s. */
extern struct BgAnimTileFrame gRoomBgAnimSet1Script0Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script0Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script0Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script0Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script0Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script0Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script0Cmd6TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script0Cmd7TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script1Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script1Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script1Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script1Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script1Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script1Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script1Cmd6TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script1Cmd7TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script2Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script2Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script2Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script2Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script2Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script2Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script2Cmd6TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script2Cmd7TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script3Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script3Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script3Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script3Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script3Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script3Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script3Cmd6TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script3Cmd7TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script4Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script4Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script4Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script4Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script4Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script4Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script4Cmd6TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script4Cmd7TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script5Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script5Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script5Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script5Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script5Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script5Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script5Cmd6TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script5Cmd7TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script6Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script6Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script6Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script6Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script6Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script6Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script6Cmd6TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script6Cmd7TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script7Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script7Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script7Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script7Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script7Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script7Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script7Cmd6TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script7Cmd7TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script8Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script8Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script8Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script8Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script8Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script8Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script8Cmd6TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script8Cmd7TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script9Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script9Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script9Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script9Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script9Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script9Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script9Cmd6TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet1Script9Cmd7TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet2Script0Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet2Script0Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet2Script0Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet2Script0Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet2Script0Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet2Script0Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet3Script0Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet3Script0Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet3Script0Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet3Script0Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet3Script0Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet3Script0Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet4Script0Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet4Script0Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet4Script0Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet4Script0Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet4Script0Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet4Script0Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet4Script0Cmd6TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet4Script0Cmd7TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet5Script2Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet5Script2Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet5Script2Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet5Script2Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet5Script2Cmd6TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet5Script2Cmd7TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet5Script2Cmd8TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet5Script2Cmd9TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet5Script2Cmd10TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet6Script0Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet6Script0Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet6Script0Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet6Script0Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet6Script0Cmd6TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet6Script0Cmd7TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet6Script0Cmd8TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet6Script0Cmd9TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet6Script0Cmd10TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet7Script0Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet7Script0Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet7Script0Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet7Script0Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet8Script0Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet8Script0Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet8Script0Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet8Script0Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script0Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script0Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script0Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script0Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script0Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script0Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script1Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script1Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script1Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script1Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script1Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script1Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script2Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script2Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script2Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script2Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script2Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet10Script2Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet12Script0Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet12Script0Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet12Script0Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet12Script0Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet12Script0Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet12Script0Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script0Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script0Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script0Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script0Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script0Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script0Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script1Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script1Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script1Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script1Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script1Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script1Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script2Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script2Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script2Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script2Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script2Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet9Script2Cmd5TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet11Script1Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet11Script1Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet11Script1Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet11Script1Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet11Script1Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet11Script1Cmd6TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet11Script1Cmd7TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet11Script0Cmd0TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet11Script0Cmd1TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet11Script0Cmd2TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet11Script0Cmd3TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet11Script0Cmd4TileFrame;
extern struct BgAnimTileFrame gRoomBgAnimSet11Script0Cmd5TileFrame;

/* The fades' palettes (assets): labels of data/room_bg_anims.s. */
extern u16 gRoomBgAnimSet5Script0Cmd1PaletteFadeSrc[];
extern u16 gRoomBgAnimSet5Script0Cmd1PaletteFadeDst[];
extern u16 gRoomBgAnimSet5Script0Cmd2PaletteFadeSrc[];
extern u16 gRoomBgAnimSet5Script0Cmd2PaletteFadeDst[];
extern u16 gRoomBgAnimSet5Script0Cmd3PaletteFadeSrc[];
extern u16 gRoomBgAnimSet5Script0Cmd3PaletteFadeDst[];
extern u16 gRoomBgAnimSet5Script1Cmd1PaletteFadeSrc[];
extern u16 gRoomBgAnimSet5Script1Cmd1PaletteFadeDst[];
extern u16 gRoomBgAnimSet5Script1Cmd2PaletteFadeSrc[];
extern u16 gRoomBgAnimSet5Script1Cmd2PaletteFadeDst[];
extern u16 gRoomBgAnimSet5Script1Cmd3PaletteFadeSrc[];
extern u16 gRoomBgAnimSet5Script1Cmd3PaletteFadeDst[];
extern u16 gRoomBgAnimSet6Script1Cmd1PaletteFadeSrc[];
extern u16 gRoomBgAnimSet6Script1Cmd1PaletteFadeDst[];
extern u16 gRoomBgAnimSet6Script1Cmd2PaletteFadeSrc[];
extern u16 gRoomBgAnimSet6Script1Cmd2PaletteFadeDst[];
extern u16 gRoomBgAnimSet6Script1Cmd3PaletteFadeSrc[];
extern u16 gRoomBgAnimSet6Script1Cmd3PaletteFadeDst[];
extern u16 gRoomBgAnimSet6Script1Cmd4PaletteFadeSrc[];
extern u16 gRoomBgAnimSet6Script1Cmd4PaletteFadeDst[];
extern u16 gRoomBgAnimSet6Script2Cmd1PaletteFadeSrc[];
extern u16 gRoomBgAnimSet6Script2Cmd1PaletteFadeDst[];
extern u16 gRoomBgAnimSet6Script2Cmd2PaletteFadeSrc[];
extern u16 gRoomBgAnimSet6Script2Cmd2PaletteFadeDst[];
extern u16 gRoomBgAnimSet6Script2Cmd3PaletteFadeSrc[];
extern u16 gRoomBgAnimSet6Script2Cmd3PaletteFadeDst[];
extern u16 gRoomBgAnimSet6Script2Cmd4PaletteFadeSrc[];
extern u16 gRoomBgAnimSet6Script2Cmd4PaletteFadeDst[];

/* ---- 0x083356E0-0x08335728: gRoomBgAnimSet1Script0, section .bg_anim_083356e0 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script0[] BG_ANIM(083356e0) = {
    { 0, 8, &gRoomBgAnimSet1Script0Cmd0TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script0Cmd1TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script0Cmd2TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script0Cmd3TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script0Cmd4TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script0Cmd5TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script0Cmd6TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script0Cmd7TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08335B48-0x08335B90: gRoomBgAnimSet1Script1, section .bg_anim_08335b48 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script1[] BG_ANIM(08335b48) = {
    { 0, 8, &gRoomBgAnimSet1Script1Cmd0TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script1Cmd1TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script1Cmd2TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script1Cmd3TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script1Cmd4TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script1Cmd5TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script1Cmd6TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script1Cmd7TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08335DB0-0x08335DF8: gRoomBgAnimSet1Script2, section .bg_anim_08335db0 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script2[] BG_ANIM(08335db0) = {
    { 0, 8, &gRoomBgAnimSet1Script2Cmd0TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script2Cmd1TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script2Cmd2TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script2Cmd3TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script2Cmd4TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script2Cmd5TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script2Cmd6TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script2Cmd7TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08336618-0x08336660: gRoomBgAnimSet1Script3, section .bg_anim_08336618 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script3[] BG_ANIM(08336618) = {
    { 0, 8, &gRoomBgAnimSet1Script3Cmd0TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script3Cmd1TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script3Cmd2TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script3Cmd3TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script3Cmd4TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script3Cmd5TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script3Cmd6TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script3Cmd7TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08336E80-0x08336EC8: gRoomBgAnimSet1Script4, section .bg_anim_08336e80 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script4[] BG_ANIM(08336e80) = {
    { 0, 8, &gRoomBgAnimSet1Script4Cmd0TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script4Cmd1TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script4Cmd2TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script4Cmd3TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script4Cmd4TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script4Cmd5TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script4Cmd6TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script4Cmd7TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x083376E8-0x08337730: gRoomBgAnimSet1Script5, section .bg_anim_083376e8 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script5[] BG_ANIM(083376e8) = {
    { 0, 8, &gRoomBgAnimSet1Script5Cmd0TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script5Cmd1TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script5Cmd2TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script5Cmd3TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script5Cmd4TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script5Cmd5TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script5Cmd6TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script5Cmd7TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08337B50-0x08337B98: gRoomBgAnimSet1Script6, section .bg_anim_08337b50 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script6[] BG_ANIM(08337b50) = {
    { 0, 8, &gRoomBgAnimSet1Script6Cmd0TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script6Cmd1TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script6Cmd2TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script6Cmd3TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script6Cmd4TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script6Cmd5TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script6Cmd6TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script6Cmd7TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x083396B8-0x08339700: gRoomBgAnimSet1Script7, section .bg_anim_083396b8 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script7[] BG_ANIM(083396b8) = {
    { 0, 8, &gRoomBgAnimSet1Script7Cmd0TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script7Cmd1TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script7Cmd2TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script7Cmd3TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script7Cmd4TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script7Cmd5TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script7Cmd6TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script7Cmd7TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08339B20-0x08339B68: gRoomBgAnimSet1Script8, section .bg_anim_08339b20 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script8[] BG_ANIM(08339b20) = {
    { 0, 8, &gRoomBgAnimSet1Script8Cmd0TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script8Cmd1TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script8Cmd2TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script8Cmd3TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script8Cmd4TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script8Cmd5TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script8Cmd6TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script8Cmd7TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08339D88-0x08339DD0: gRoomBgAnimSet1Script9, section .bg_anim_08339d88 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script9[] BG_ANIM(08339d88) = {
    { 0, 8, &gRoomBgAnimSet1Script9Cmd0TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script9Cmd1TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script9Cmd2TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script9Cmd3TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script9Cmd4TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script9Cmd5TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script9Cmd6TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet1Script9Cmd7TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0833A0E8-0x0833A120: gRoomBgAnimSet2Script0, section .bg_anim_0833a0e8 ---- */
struct BgAnimCmd gRoomBgAnimSet2Script0[] BG_ANIM(0833a0e8) = {
    { 0, 8, &gRoomBgAnimSet2Script0Cmd0TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet2Script0Cmd1TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet2Script0Cmd2TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet2Script0Cmd3TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet2Script0Cmd4TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet2Script0Cmd5TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0833ABB8-0x0833ABF0: gRoomBgAnimSet3Script0, section .bg_anim_0833abb8 ---- */
struct BgAnimCmd gRoomBgAnimSet3Script0[] BG_ANIM(0833abb8) = {
    { 0, 7, &gRoomBgAnimSet3Script0Cmd0TileFrame }, /* copy tiles */
    { 0, 7, &gRoomBgAnimSet3Script0Cmd1TileFrame }, /* copy tiles */
    { 0, 7, &gRoomBgAnimSet3Script0Cmd2TileFrame }, /* copy tiles */
    { 0, 7, &gRoomBgAnimSet3Script0Cmd3TileFrame }, /* copy tiles */
    { 0, 7, &gRoomBgAnimSet3Script0Cmd4TileFrame }, /* copy tiles */
    { 0, 7, &gRoomBgAnimSet3Script0Cmd5TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0833EC10-0x0833EC58: gRoomBgAnimSet4Script0, section .bg_anim_0833ec10 ---- */
struct BgAnimCmd gRoomBgAnimSet4Script0[] BG_ANIM(0833ec10) = {
    { 0, 10, &gRoomBgAnimSet4Script0Cmd0TileFrame }, /* copy tiles */
    { 0, 10, &gRoomBgAnimSet4Script0Cmd1TileFrame }, /* copy tiles */
    { 0, 10, &gRoomBgAnimSet4Script0Cmd2TileFrame }, /* copy tiles */
    { 0, 10, &gRoomBgAnimSet4Script0Cmd3TileFrame }, /* copy tiles */
    { 0, 10, &gRoomBgAnimSet4Script0Cmd4TileFrame }, /* copy tiles */
    { 0, 10, &gRoomBgAnimSet4Script0Cmd5TileFrame }, /* copy tiles */
    { 0, 10, &gRoomBgAnimSet4Script0Cmd6TileFrame }, /* copy tiles */
    { 0, 10, &gRoomBgAnimSet4Script0Cmd7TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0833F45C-0x0833F4C4: gRoomBgAnimSet5Script2, section .bg_anim_0833f45c ---- */
struct BgAnimCmd gRoomBgAnimSet5Script2[] BG_ANIM(0833f45c) = {
    { 2, 292, NULL }, /* wait */
    { 6, 247, NULL }, /* sound effect */
    { 0, 4, &gRoomBgAnimSet5Script2Cmd2TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet5Script2Cmd3TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet5Script2Cmd4TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet5Script2Cmd5TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet5Script2Cmd6TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet5Script2Cmd7TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet5Script2Cmd8TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet5Script2Cmd9TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet5Script2Cmd10TileFrame }, /* copy tiles */
    { 5, 0x0905, (void *)0x10 }, /* collision tile */
    { 4, 0, NULL }, /* stop */
};

/* ---- 0x0833FCC8-0x0833FD30: gRoomBgAnimSet6Script0, section .bg_anim_0833fcc8 ---- */
struct BgAnimCmd gRoomBgAnimSet6Script0[] BG_ANIM(0833fcc8) = {
    { 2, 464, NULL }, /* wait */
    { 6, 247, NULL }, /* sound effect */
    { 0, 4, &gRoomBgAnimSet6Script0Cmd2TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet6Script0Cmd3TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet6Script0Cmd4TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet6Script0Cmd5TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet6Script0Cmd6TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet6Script0Cmd7TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet6Script0Cmd8TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet6Script0Cmd9TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet6Script0Cmd10TileFrame }, /* copy tiles */
    { 5, 0x0902, (void *)0x10 }, /* collision tile */
    { 4, 0, NULL }, /* stop */
};

/* ---- 0x0833FE30-0x0833FE40: gRoomBgAnimSet5Script0Cmd1PaletteFade, section .bg_anim_0833fe30 ---- */
struct BgAnimPaletteFade gRoomBgAnimSet5Script0Cmd1PaletteFade BG_ANIM(0833fe30) = {
    .src = gRoomBgAnimSet5Script0Cmd1PaletteFadeSrc,
    .dst = gRoomBgAnimSet5Script0Cmd1PaletteFadeDst,
    .colorIndex = 0,
    .colorCount = 64,
    .rate = 874,
};

/* ---- 0x0833FF40-0x0833FF50: gRoomBgAnimSet5Script0Cmd2PaletteFade, section .bg_anim_0833ff40 ---- */
struct BgAnimPaletteFade gRoomBgAnimSet5Script0Cmd2PaletteFade BG_ANIM(0833ff40) = {
    .src = gRoomBgAnimSet5Script0Cmd2PaletteFadeSrc,
    .dst = gRoomBgAnimSet5Script0Cmd2PaletteFadeDst,
    .colorIndex = 0,
    .colorCount = 64,
    .rate = 874,
};

/* ---- 0x08340050-0x08340088: gRoomBgAnimSet5Script0Cmd3PaletteFade, gRoomBgAnimSet5Script0, section .bg_anim_08340050 ---- */
struct BgAnimPaletteFade gRoomBgAnimSet5Script0Cmd3PaletteFade BG_ANIM(08340050) = {
    .src = gRoomBgAnimSet5Script0Cmd3PaletteFadeSrc,
    .dst = gRoomBgAnimSet5Script0Cmd3PaletteFadeDst,
    .colorIndex = 0,
    .colorCount = 64,
    .rate = 874,
};
struct BgAnimCmd gRoomBgAnimSet5Script0[] BG_ANIM(08340050) = {
    { 2, 65, NULL }, /* wait */
    { 1, 75, &gRoomBgAnimSet5Script0Cmd1PaletteFade }, /* palette fade */
    { 1, 75, &gRoomBgAnimSet5Script0Cmd2PaletteFade }, /* palette fade */
    { 1, 75, &gRoomBgAnimSet5Script0Cmd3PaletteFade }, /* palette fade */
    { 4, 0, NULL }, /* stop */
};

/* ---- 0x08340108-0x08340118: gRoomBgAnimSet5Script1Cmd1PaletteFade, section .bg_anim_08340108 ---- */
struct BgAnimPaletteFade gRoomBgAnimSet5Script1Cmd1PaletteFade BG_ANIM(08340108) = {
    .src = gRoomBgAnimSet5Script1Cmd1PaletteFadeSrc,
    .dst = gRoomBgAnimSet5Script1Cmd1PaletteFadeDst,
    .colorIndex = 64,
    .colorCount = 32,
    .rate = 874,
};

/* ---- 0x08340198-0x083401A8: gRoomBgAnimSet5Script1Cmd2PaletteFade, section .bg_anim_08340198 ---- */
struct BgAnimPaletteFade gRoomBgAnimSet5Script1Cmd2PaletteFade BG_ANIM(08340198) = {
    .src = gRoomBgAnimSet5Script1Cmd2PaletteFadeSrc,
    .dst = gRoomBgAnimSet5Script1Cmd2PaletteFadeDst,
    .colorIndex = 64,
    .colorCount = 32,
    .rate = 874,
};

/* ---- 0x08340228-0x08340260: gRoomBgAnimSet5Script1Cmd3PaletteFade, gRoomBgAnimSet5Script1, section .bg_anim_08340228 ---- */
struct BgAnimPaletteFade gRoomBgAnimSet5Script1Cmd3PaletteFade BG_ANIM(08340228) = {
    .src = gRoomBgAnimSet5Script1Cmd3PaletteFadeSrc,
    .dst = gRoomBgAnimSet5Script1Cmd3PaletteFadeDst,
    .colorIndex = 64,
    .colorCount = 32,
    .rate = 874,
};
struct BgAnimCmd gRoomBgAnimSet5Script1[] BG_ANIM(08340228) = {
    { 2, 65, NULL }, /* wait */
    { 1, 75, &gRoomBgAnimSet5Script1Cmd1PaletteFade }, /* palette fade */
    { 1, 75, &gRoomBgAnimSet5Script1Cmd2PaletteFade }, /* palette fade */
    { 1, 75, &gRoomBgAnimSet5Script1Cmd3PaletteFade }, /* palette fade */
    { 4, 0, NULL }, /* stop */
};

/* ---- 0x08340360-0x08340370: gRoomBgAnimSet6Script1Cmd1PaletteFade, section .bg_anim_08340360 ---- */
struct BgAnimPaletteFade gRoomBgAnimSet6Script1Cmd1PaletteFade BG_ANIM(08340360) = {
    .src = gRoomBgAnimSet6Script1Cmd1PaletteFadeSrc,
    .dst = gRoomBgAnimSet6Script1Cmd1PaletteFadeDst,
    .colorIndex = 0,
    .colorCount = 64,
    .rate = 656,
};

/* ---- 0x08340470-0x08340480: gRoomBgAnimSet6Script1Cmd2PaletteFade, section .bg_anim_08340470 ---- */
struct BgAnimPaletteFade gRoomBgAnimSet6Script1Cmd2PaletteFade BG_ANIM(08340470) = {
    .src = gRoomBgAnimSet6Script1Cmd2PaletteFadeSrc,
    .dst = gRoomBgAnimSet6Script1Cmd2PaletteFadeDst,
    .colorIndex = 0,
    .colorCount = 64,
    .rate = 656,
};

/* ---- 0x08340580-0x08340590: gRoomBgAnimSet6Script1Cmd3PaletteFade, section .bg_anim_08340580 ---- */
struct BgAnimPaletteFade gRoomBgAnimSet6Script1Cmd3PaletteFade BG_ANIM(08340580) = {
    .src = gRoomBgAnimSet6Script1Cmd3PaletteFadeSrc,
    .dst = gRoomBgAnimSet6Script1Cmd3PaletteFadeDst,
    .colorIndex = 0,
    .colorCount = 64,
    .rate = 656,
};

/* ---- 0x08340690-0x083406D0: gRoomBgAnimSet6Script1Cmd4PaletteFade, gRoomBgAnimSet6Script1, section .bg_anim_08340690 ---- */
struct BgAnimPaletteFade gRoomBgAnimSet6Script1Cmd4PaletteFade BG_ANIM(08340690) = {
    .src = gRoomBgAnimSet6Script1Cmd4PaletteFadeSrc,
    .dst = gRoomBgAnimSet6Script1Cmd4PaletteFadeDst,
    .colorIndex = 0,
    .colorCount = 64,
    .rate = 656,
};
struct BgAnimCmd gRoomBgAnimSet6Script1[] BG_ANIM(08340690) = {
    { 2, 64, NULL }, /* wait */
    { 1, 100, &gRoomBgAnimSet6Script1Cmd1PaletteFade }, /* palette fade */
    { 1, 100, &gRoomBgAnimSet6Script1Cmd2PaletteFade }, /* palette fade */
    { 1, 100, &gRoomBgAnimSet6Script1Cmd3PaletteFade }, /* palette fade */
    { 1, 100, &gRoomBgAnimSet6Script1Cmd4PaletteFade }, /* palette fade */
    { 4, 0, NULL }, /* stop */
};

/* ---- 0x08340750-0x08340760: gRoomBgAnimSet6Script2Cmd1PaletteFade, section .bg_anim_08340750 ---- */
struct BgAnimPaletteFade gRoomBgAnimSet6Script2Cmd1PaletteFade BG_ANIM(08340750) = {
    .src = gRoomBgAnimSet6Script2Cmd1PaletteFadeSrc,
    .dst = gRoomBgAnimSet6Script2Cmd1PaletteFadeDst,
    .colorIndex = 64,
    .colorCount = 32,
    .rate = 656,
};

/* ---- 0x083407E0-0x083407F0: gRoomBgAnimSet6Script2Cmd2PaletteFade, section .bg_anim_083407e0 ---- */
struct BgAnimPaletteFade gRoomBgAnimSet6Script2Cmd2PaletteFade BG_ANIM(083407e0) = {
    .src = gRoomBgAnimSet6Script2Cmd2PaletteFadeSrc,
    .dst = gRoomBgAnimSet6Script2Cmd2PaletteFadeDst,
    .colorIndex = 64,
    .colorCount = 32,
    .rate = 656,
};

/* ---- 0x08340870-0x08340880: gRoomBgAnimSet6Script2Cmd3PaletteFade, section .bg_anim_08340870 ---- */
struct BgAnimPaletteFade gRoomBgAnimSet6Script2Cmd3PaletteFade BG_ANIM(08340870) = {
    .src = gRoomBgAnimSet6Script2Cmd3PaletteFadeSrc,
    .dst = gRoomBgAnimSet6Script2Cmd3PaletteFadeDst,
    .colorIndex = 64,
    .colorCount = 32,
    .rate = 656,
};

/* ---- 0x08340900-0x08340940: gRoomBgAnimSet6Script2Cmd4PaletteFade, gRoomBgAnimSet6Script2, section .bg_anim_08340900 ---- */
struct BgAnimPaletteFade gRoomBgAnimSet6Script2Cmd4PaletteFade BG_ANIM(08340900) = {
    .src = gRoomBgAnimSet6Script2Cmd4PaletteFadeSrc,
    .dst = gRoomBgAnimSet6Script2Cmd4PaletteFadeDst,
    .colorIndex = 64,
    .colorCount = 32,
    .rate = 656,
};
struct BgAnimCmd gRoomBgAnimSet6Script2[] BG_ANIM(08340900) = {
    { 2, 64, NULL }, /* wait */
    { 1, 100, &gRoomBgAnimSet6Script2Cmd1PaletteFade }, /* palette fade */
    { 1, 100, &gRoomBgAnimSet6Script2Cmd2PaletteFade }, /* palette fade */
    { 1, 100, &gRoomBgAnimSet6Script2Cmd3PaletteFade }, /* palette fade */
    { 1, 100, &gRoomBgAnimSet6Script2Cmd4PaletteFade }, /* palette fade */
    { 4, 0, NULL }, /* stop */
};

/* ---- 0x083447D0-0x083447F8: gRoomBgAnimSet7Script0, section .bg_anim_083447d0 ---- */
struct BgAnimCmd gRoomBgAnimSet7Script0[] BG_ANIM(083447d0) = {
    { 0, 4, &gRoomBgAnimSet7Script0Cmd0TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet7Script0Cmd1TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet7Script0Cmd2TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet7Script0Cmd3TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08344C08-0x08344C30: gRoomBgAnimSet8Script0, section .bg_anim_08344c08 ---- */
struct BgAnimCmd gRoomBgAnimSet8Script0[] BG_ANIM(08344c08) = {
    { 0, 8, &gRoomBgAnimSet8Script0Cmd0TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet8Script0Cmd1TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet8Script0Cmd2TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet8Script0Cmd3TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08348248-0x08348280: gRoomBgAnimSet10Script0, section .bg_anim_08348248 ---- */
struct BgAnimCmd gRoomBgAnimSet10Script0[] BG_ANIM(08348248) = {
    { 0, 2, &gRoomBgAnimSet10Script0Cmd0TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script0Cmd1TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script0Cmd2TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script0Cmd3TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script0Cmd4TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script0Cmd5TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0834B898-0x0834B8D0: gRoomBgAnimSet10Script1, section .bg_anim_0834b898 ---- */
struct BgAnimCmd gRoomBgAnimSet10Script1[] BG_ANIM(0834b898) = {
    { 0, 2, &gRoomBgAnimSet10Script1Cmd0TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script1Cmd1TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script1Cmd2TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script1Cmd3TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script1Cmd4TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script1Cmd5TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0834EEE8-0x0834EF20: gRoomBgAnimSet10Script2, section .bg_anim_0834eee8 ---- */
struct BgAnimCmd gRoomBgAnimSet10Script2[] BG_ANIM(0834eee8) = {
    { 0, 2, &gRoomBgAnimSet10Script2Cmd0TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script2Cmd1TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script2Cmd2TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script2Cmd3TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script2Cmd4TileFrame }, /* copy tiles */
    { 0, 2, &gRoomBgAnimSet10Script2Cmd5TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08350AF8-0x08350B30: gRoomBgAnimSet12Script0, section .bg_anim_08350af8 ---- */
struct BgAnimCmd gRoomBgAnimSet12Script0[] BG_ANIM(08350af8) = {
    { 0, 8, &gRoomBgAnimSet12Script0Cmd0TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet12Script0Cmd1TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet12Script0Cmd2TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet12Script0Cmd3TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet12Script0Cmd4TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet12Script0Cmd5TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08353248-0x08353280: gRoomBgAnimSet9Script0, section .bg_anim_08353248 ---- */
struct BgAnimCmd gRoomBgAnimSet9Script0[] BG_ANIM(08353248) = {
    { 0, 3, &gRoomBgAnimSet9Script0Cmd0TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script0Cmd1TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script0Cmd2TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script0Cmd3TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script0Cmd4TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script0Cmd5TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08355998-0x083559D0: gRoomBgAnimSet9Script1, section .bg_anim_08355998 ---- */
struct BgAnimCmd gRoomBgAnimSet9Script1[] BG_ANIM(08355998) = {
    { 0, 3, &gRoomBgAnimSet9Script1Cmd0TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script1Cmd1TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script1Cmd2TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script1Cmd3TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script1Cmd4TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script1Cmd5TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x083580E8-0x08358120: gRoomBgAnimSet9Script2, section .bg_anim_083580e8 ---- */
struct BgAnimCmd gRoomBgAnimSet9Script2[] BG_ANIM(083580e8) = {
    { 0, 3, &gRoomBgAnimSet9Script2Cmd0TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script2Cmd1TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script2Cmd2TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script2Cmd3TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script2Cmd4TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet9Script2Cmd5TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0835B5BC-0x0835B604: gRoomBgAnimSet11Script1, section .bg_anim_0835b5bc ---- */
struct BgAnimCmd gRoomBgAnimSet11Script1[] BG_ANIM(0835b5bc) = {
    { 0, 2, &gRoomBgAnimSet11Script1Cmd0TileFrame }, /* copy tiles */
    { 0, 3, &gRoomBgAnimSet11Script1Cmd1TileFrame }, /* copy tiles */
    { 0, 4, &gRoomBgAnimSet11Script1Cmd2TileFrame }, /* copy tiles */
    { 0, 6, &gRoomBgAnimSet11Script1Cmd3TileFrame }, /* copy tiles */
    { 0, 6, &gRoomBgAnimSet11Script1Cmd4TileFrame }, /* copy tiles */
    { 0, 6, &gRoomBgAnimSet11Script1Cmd3TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet11Script1Cmd6TileFrame }, /* copy tiles */
    { 0, 10, &gRoomBgAnimSet11Script1Cmd7TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0835CF9C-0x0835CFD4: gRoomBgAnimSet11Script0, section .bg_anim_0835cf9c ---- */
struct BgAnimCmd gRoomBgAnimSet11Script0[] BG_ANIM(0835cf9c) = {
    { 0, 8, &gRoomBgAnimSet11Script0Cmd0TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet11Script0Cmd1TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet11Script0Cmd2TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet11Script0Cmd3TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet11Script0Cmd4TileFrame }, /* copy tiles */
    { 0, 8, &gRoomBgAnimSet11Script0Cmd5TileFrame }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};
