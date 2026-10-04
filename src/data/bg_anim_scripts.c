#include "global.h"
#include "camera.h"

/* The rooms' BG animation scripts and palette fades (room_bg_anims,
 * 0x08334EC0-0x0835CFD3, issue #167): 30 scripts (struct BgAnimCmd
 * arrays) and 14 palette fades (struct BgAnimPaletteFade) in 40 runs, in
 * address order.  LoadRoomBgAnims (src/camera_2d01c.c) loads the
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
extern struct BgAnimTileFrame gUnk_08334EC0;
extern struct BgAnimTileFrame gUnk_08334FC4;
extern struct BgAnimTileFrame gUnk_083350C8;
extern struct BgAnimTileFrame gUnk_083351CC;
extern struct BgAnimTileFrame gUnk_083352D0;
extern struct BgAnimTileFrame gUnk_083353D4;
extern struct BgAnimTileFrame gUnk_083354D8;
extern struct BgAnimTileFrame gUnk_083355DC;
extern struct BgAnimTileFrame gUnk_08335728;
extern struct BgAnimTileFrame gUnk_083357AC;
extern struct BgAnimTileFrame gUnk_08335830;
extern struct BgAnimTileFrame gUnk_083358B4;
extern struct BgAnimTileFrame gUnk_08335938;
extern struct BgAnimTileFrame gUnk_083359BC;
extern struct BgAnimTileFrame gUnk_08335A40;
extern struct BgAnimTileFrame gUnk_08335AC4;
extern struct BgAnimTileFrame gUnk_08335B90;
extern struct BgAnimTileFrame gUnk_08335BD4;
extern struct BgAnimTileFrame gUnk_08335C18;
extern struct BgAnimTileFrame gUnk_08335C5C;
extern struct BgAnimTileFrame gUnk_08335CA0;
extern struct BgAnimTileFrame gUnk_08335CE4;
extern struct BgAnimTileFrame gUnk_08335D28;
extern struct BgAnimTileFrame gUnk_08335D6C;
extern struct BgAnimTileFrame gUnk_08335DF8;
extern struct BgAnimTileFrame gUnk_08335EFC;
extern struct BgAnimTileFrame gUnk_08336000;
extern struct BgAnimTileFrame gUnk_08336104;
extern struct BgAnimTileFrame gUnk_08336208;
extern struct BgAnimTileFrame gUnk_0833630C;
extern struct BgAnimTileFrame gUnk_08336410;
extern struct BgAnimTileFrame gUnk_08336514;
extern struct BgAnimTileFrame gUnk_08336660;
extern struct BgAnimTileFrame gUnk_08336764;
extern struct BgAnimTileFrame gUnk_08336868;
extern struct BgAnimTileFrame gUnk_0833696C;
extern struct BgAnimTileFrame gUnk_08336A70;
extern struct BgAnimTileFrame gUnk_08336B74;
extern struct BgAnimTileFrame gUnk_08336C78;
extern struct BgAnimTileFrame gUnk_08336D7C;
extern struct BgAnimTileFrame gUnk_08336EC8;
extern struct BgAnimTileFrame gUnk_08336FCC;
extern struct BgAnimTileFrame gUnk_083370D0;
extern struct BgAnimTileFrame gUnk_083371D4;
extern struct BgAnimTileFrame gUnk_083372D8;
extern struct BgAnimTileFrame gUnk_083373DC;
extern struct BgAnimTileFrame gUnk_083374E0;
extern struct BgAnimTileFrame gUnk_083375E4;
extern struct BgAnimTileFrame gUnk_08337730;
extern struct BgAnimTileFrame gUnk_083377B4;
extern struct BgAnimTileFrame gUnk_08337838;
extern struct BgAnimTileFrame gUnk_083378BC;
extern struct BgAnimTileFrame gUnk_08337940;
extern struct BgAnimTileFrame gUnk_083379C4;
extern struct BgAnimTileFrame gUnk_08337A48;
extern struct BgAnimTileFrame gUnk_08337ACC;
extern struct BgAnimTileFrame gUnk_08337B98;
extern struct BgAnimTileFrame gUnk_08337EFC;
extern struct BgAnimTileFrame gUnk_08338260;
extern struct BgAnimTileFrame gUnk_083385C4;
extern struct BgAnimTileFrame gUnk_08338928;
extern struct BgAnimTileFrame gUnk_08338C8C;
extern struct BgAnimTileFrame gUnk_08338FF0;
extern struct BgAnimTileFrame gUnk_08339354;
extern struct BgAnimTileFrame gUnk_08339700;
extern struct BgAnimTileFrame gUnk_08339784;
extern struct BgAnimTileFrame gUnk_08339808;
extern struct BgAnimTileFrame gUnk_0833988C;
extern struct BgAnimTileFrame gUnk_08339910;
extern struct BgAnimTileFrame gUnk_08339994;
extern struct BgAnimTileFrame gUnk_08339A18;
extern struct BgAnimTileFrame gUnk_08339A9C;
extern struct BgAnimTileFrame gUnk_08339B68;
extern struct BgAnimTileFrame gUnk_08339BAC;
extern struct BgAnimTileFrame gUnk_08339BF0;
extern struct BgAnimTileFrame gUnk_08339C34;
extern struct BgAnimTileFrame gUnk_08339C78;
extern struct BgAnimTileFrame gUnk_08339CBC;
extern struct BgAnimTileFrame gUnk_08339D00;
extern struct BgAnimTileFrame gUnk_08339D44;
extern struct BgAnimTileFrame gUnk_08339DD0;
extern struct BgAnimTileFrame gUnk_08339E54;
extern struct BgAnimTileFrame gUnk_08339ED8;
extern struct BgAnimTileFrame gUnk_08339F5C;
extern struct BgAnimTileFrame gUnk_08339FE0;
extern struct BgAnimTileFrame gUnk_0833A064;
extern struct BgAnimTileFrame gUnk_0833A120;
extern struct BgAnimTileFrame gUnk_0833A2E4;
extern struct BgAnimTileFrame gUnk_0833A4A8;
extern struct BgAnimTileFrame gUnk_0833A66C;
extern struct BgAnimTileFrame gUnk_0833A830;
extern struct BgAnimTileFrame gUnk_0833A9F4;
extern struct BgAnimTileFrame gUnk_0833ABF0;
extern struct BgAnimTileFrame gUnk_0833B3F4;
extern struct BgAnimTileFrame gUnk_0833BBF8;
extern struct BgAnimTileFrame gUnk_0833C3FC;
extern struct BgAnimTileFrame gUnk_0833CC00;
extern struct BgAnimTileFrame gUnk_0833D404;
extern struct BgAnimTileFrame gUnk_0833DC08;
extern struct BgAnimTileFrame gUnk_0833E40C;
extern struct BgAnimTileFrame gUnk_0833EC58;
extern struct BgAnimTileFrame gUnk_0833ED3C;
extern struct BgAnimTileFrame gUnk_0833EE20;
extern struct BgAnimTileFrame gUnk_0833EF04;
extern struct BgAnimTileFrame gUnk_0833EFE8;
extern struct BgAnimTileFrame gUnk_0833F0CC;
extern struct BgAnimTileFrame gUnk_0833F1B0;
extern struct BgAnimTileFrame gUnk_0833F294;
extern struct BgAnimTileFrame gUnk_0833F378;
extern struct BgAnimTileFrame gUnk_0833F4C4;
extern struct BgAnimTileFrame gUnk_0833F5A8;
extern struct BgAnimTileFrame gUnk_0833F68C;
extern struct BgAnimTileFrame gUnk_0833F770;
extern struct BgAnimTileFrame gUnk_0833F854;
extern struct BgAnimTileFrame gUnk_0833F938;
extern struct BgAnimTileFrame gUnk_0833FA1C;
extern struct BgAnimTileFrame gUnk_0833FB00;
extern struct BgAnimTileFrame gUnk_0833FBE4;
extern struct BgAnimTileFrame gUnk_08340940;
extern struct BgAnimTileFrame gUnk_083418E4;
extern struct BgAnimTileFrame gUnk_08342888;
extern struct BgAnimTileFrame gUnk_0834382C;
extern struct BgAnimTileFrame gUnk_083447F8;
extern struct BgAnimTileFrame gUnk_083448FC;
extern struct BgAnimTileFrame gUnk_08344A00;
extern struct BgAnimTileFrame gUnk_08344B04;
extern struct BgAnimTileFrame gUnk_08344C30;
extern struct BgAnimTileFrame gUnk_08345534;
extern struct BgAnimTileFrame gUnk_08345E38;
extern struct BgAnimTileFrame gUnk_0834673C;
extern struct BgAnimTileFrame gUnk_08347040;
extern struct BgAnimTileFrame gUnk_08347944;
extern struct BgAnimTileFrame gUnk_08348280;
extern struct BgAnimTileFrame gUnk_08348B84;
extern struct BgAnimTileFrame gUnk_08349488;
extern struct BgAnimTileFrame gUnk_08349D8C;
extern struct BgAnimTileFrame gUnk_0834A690;
extern struct BgAnimTileFrame gUnk_0834AF94;
extern struct BgAnimTileFrame gUnk_0834B8D0;
extern struct BgAnimTileFrame gUnk_0834C1D4;
extern struct BgAnimTileFrame gUnk_0834CAD8;
extern struct BgAnimTileFrame gUnk_0834D3DC;
extern struct BgAnimTileFrame gUnk_0834DCE0;
extern struct BgAnimTileFrame gUnk_0834E5E4;
extern struct BgAnimTileFrame gUnk_0834EF20;
extern struct BgAnimTileFrame gUnk_0834F3C4;
extern struct BgAnimTileFrame gUnk_0834F868;
extern struct BgAnimTileFrame gUnk_0834FD0C;
extern struct BgAnimTileFrame gUnk_083501B0;
extern struct BgAnimTileFrame gUnk_08350654;
extern struct BgAnimTileFrame gUnk_08350B30;
extern struct BgAnimTileFrame gUnk_083511B4;
extern struct BgAnimTileFrame gUnk_08351838;
extern struct BgAnimTileFrame gUnk_08351EBC;
extern struct BgAnimTileFrame gUnk_08352540;
extern struct BgAnimTileFrame gUnk_08352BC4;
extern struct BgAnimTileFrame gUnk_08353280;
extern struct BgAnimTileFrame gUnk_08353904;
extern struct BgAnimTileFrame gUnk_08353F88;
extern struct BgAnimTileFrame gUnk_0835460C;
extern struct BgAnimTileFrame gUnk_08354C90;
extern struct BgAnimTileFrame gUnk_08355314;
extern struct BgAnimTileFrame gUnk_083559D0;
extern struct BgAnimTileFrame gUnk_08356054;
extern struct BgAnimTileFrame gUnk_083566D8;
extern struct BgAnimTileFrame gUnk_08356D5C;
extern struct BgAnimTileFrame gUnk_083573E0;
extern struct BgAnimTileFrame gUnk_08357A64;
extern struct BgAnimTileFrame gUnk_08358120;
extern struct BgAnimTileFrame gUnk_083588A4;
extern struct BgAnimTileFrame gUnk_08359028;
extern struct BgAnimTileFrame gUnk_083597AC;
extern struct BgAnimTileFrame gUnk_08359F30;
extern struct BgAnimTileFrame gUnk_0835A6B4;
extern struct BgAnimTileFrame gUnk_0835AE38;
extern struct BgAnimTileFrame gUnk_0835B604;
extern struct BgAnimTileFrame gUnk_0835BA48;
extern struct BgAnimTileFrame gUnk_0835BE8C;
extern struct BgAnimTileFrame gUnk_0835C2D0;
extern struct BgAnimTileFrame gUnk_0835C714;
extern struct BgAnimTileFrame gUnk_0835CB58;

/* The fades' palettes (assets): labels of data/room_bg_anims.s. */
extern u16 gUnk_0833FD30[];
extern u16 gUnk_0833FDB0[];
extern u16 gUnk_0833FE40[];
extern u16 gUnk_0833FEC0[];
extern u16 gUnk_0833FF50[];
extern u16 gUnk_0833FFD0[];
extern u16 gUnk_08340088[];
extern u16 gUnk_083400C8[];
extern u16 gUnk_08340118[];
extern u16 gUnk_08340158[];
extern u16 gUnk_083401A8[];
extern u16 gUnk_083401E8[];
extern u16 gUnk_08340260[];
extern u16 gUnk_083402E0[];
extern u16 gUnk_08340370[];
extern u16 gUnk_083403F0[];
extern u16 gUnk_08340480[];
extern u16 gUnk_08340500[];
extern u16 gUnk_08340590[];
extern u16 gUnk_08340610[];
extern u16 gUnk_083406D0[];
extern u16 gUnk_08340710[];
extern u16 gUnk_08340760[];
extern u16 gUnk_083407A0[];
extern u16 gUnk_083407F0[];
extern u16 gUnk_08340830[];
extern u16 gUnk_08340880[];
extern u16 gUnk_083408C0[];

/* ---- 0x083356E0-0x08335728: gRoomBgAnimSet1Script0, section .bg_anim_083356e0 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script0[] BG_ANIM(083356e0) = {
    { 0, 8, &gUnk_08334EC0 }, /* copy tiles */
    { 0, 8, &gUnk_08334FC4 }, /* copy tiles */
    { 0, 8, &gUnk_083350C8 }, /* copy tiles */
    { 0, 8, &gUnk_083351CC }, /* copy tiles */
    { 0, 8, &gUnk_083352D0 }, /* copy tiles */
    { 0, 8, &gUnk_083353D4 }, /* copy tiles */
    { 0, 8, &gUnk_083354D8 }, /* copy tiles */
    { 0, 8, &gUnk_083355DC }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08335B48-0x08335B90: gRoomBgAnimSet1Script1, section .bg_anim_08335b48 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script1[] BG_ANIM(08335b48) = {
    { 0, 8, &gUnk_08335728 }, /* copy tiles */
    { 0, 8, &gUnk_083357AC }, /* copy tiles */
    { 0, 8, &gUnk_08335830 }, /* copy tiles */
    { 0, 8, &gUnk_083358B4 }, /* copy tiles */
    { 0, 8, &gUnk_08335938 }, /* copy tiles */
    { 0, 8, &gUnk_083359BC }, /* copy tiles */
    { 0, 8, &gUnk_08335A40 }, /* copy tiles */
    { 0, 8, &gUnk_08335AC4 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08335DB0-0x08335DF8: gRoomBgAnimSet1Script2, section .bg_anim_08335db0 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script2[] BG_ANIM(08335db0) = {
    { 0, 8, &gUnk_08335B90 }, /* copy tiles */
    { 0, 8, &gUnk_08335BD4 }, /* copy tiles */
    { 0, 8, &gUnk_08335C18 }, /* copy tiles */
    { 0, 8, &gUnk_08335C5C }, /* copy tiles */
    { 0, 8, &gUnk_08335CA0 }, /* copy tiles */
    { 0, 8, &gUnk_08335CE4 }, /* copy tiles */
    { 0, 8, &gUnk_08335D28 }, /* copy tiles */
    { 0, 8, &gUnk_08335D6C }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08336618-0x08336660: gRoomBgAnimSet1Script3, section .bg_anim_08336618 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script3[] BG_ANIM(08336618) = {
    { 0, 8, &gUnk_08335DF8 }, /* copy tiles */
    { 0, 8, &gUnk_08335EFC }, /* copy tiles */
    { 0, 8, &gUnk_08336000 }, /* copy tiles */
    { 0, 8, &gUnk_08336104 }, /* copy tiles */
    { 0, 8, &gUnk_08336208 }, /* copy tiles */
    { 0, 8, &gUnk_0833630C }, /* copy tiles */
    { 0, 8, &gUnk_08336410 }, /* copy tiles */
    { 0, 8, &gUnk_08336514 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08336E80-0x08336EC8: gRoomBgAnimSet1Script4, section .bg_anim_08336e80 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script4[] BG_ANIM(08336e80) = {
    { 0, 8, &gUnk_08336660 }, /* copy tiles */
    { 0, 8, &gUnk_08336764 }, /* copy tiles */
    { 0, 8, &gUnk_08336868 }, /* copy tiles */
    { 0, 8, &gUnk_0833696C }, /* copy tiles */
    { 0, 8, &gUnk_08336A70 }, /* copy tiles */
    { 0, 8, &gUnk_08336B74 }, /* copy tiles */
    { 0, 8, &gUnk_08336C78 }, /* copy tiles */
    { 0, 8, &gUnk_08336D7C }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x083376E8-0x08337730: gRoomBgAnimSet1Script5, section .bg_anim_083376e8 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script5[] BG_ANIM(083376e8) = {
    { 0, 8, &gUnk_08336EC8 }, /* copy tiles */
    { 0, 8, &gUnk_08336FCC }, /* copy tiles */
    { 0, 8, &gUnk_083370D0 }, /* copy tiles */
    { 0, 8, &gUnk_083371D4 }, /* copy tiles */
    { 0, 8, &gUnk_083372D8 }, /* copy tiles */
    { 0, 8, &gUnk_083373DC }, /* copy tiles */
    { 0, 8, &gUnk_083374E0 }, /* copy tiles */
    { 0, 8, &gUnk_083375E4 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08337B50-0x08337B98: gRoomBgAnimSet1Script6, section .bg_anim_08337b50 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script6[] BG_ANIM(08337b50) = {
    { 0, 8, &gUnk_08337730 }, /* copy tiles */
    { 0, 8, &gUnk_083377B4 }, /* copy tiles */
    { 0, 8, &gUnk_08337838 }, /* copy tiles */
    { 0, 8, &gUnk_083378BC }, /* copy tiles */
    { 0, 8, &gUnk_08337940 }, /* copy tiles */
    { 0, 8, &gUnk_083379C4 }, /* copy tiles */
    { 0, 8, &gUnk_08337A48 }, /* copy tiles */
    { 0, 8, &gUnk_08337ACC }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x083396B8-0x08339700: gRoomBgAnimSet1Script7, section .bg_anim_083396b8 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script7[] BG_ANIM(083396b8) = {
    { 0, 8, &gUnk_08337B98 }, /* copy tiles */
    { 0, 8, &gUnk_08337EFC }, /* copy tiles */
    { 0, 8, &gUnk_08338260 }, /* copy tiles */
    { 0, 8, &gUnk_083385C4 }, /* copy tiles */
    { 0, 8, &gUnk_08338928 }, /* copy tiles */
    { 0, 8, &gUnk_08338C8C }, /* copy tiles */
    { 0, 8, &gUnk_08338FF0 }, /* copy tiles */
    { 0, 8, &gUnk_08339354 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08339B20-0x08339B68: gRoomBgAnimSet1Script8, section .bg_anim_08339b20 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script8[] BG_ANIM(08339b20) = {
    { 0, 8, &gUnk_08339700 }, /* copy tiles */
    { 0, 8, &gUnk_08339784 }, /* copy tiles */
    { 0, 8, &gUnk_08339808 }, /* copy tiles */
    { 0, 8, &gUnk_0833988C }, /* copy tiles */
    { 0, 8, &gUnk_08339910 }, /* copy tiles */
    { 0, 8, &gUnk_08339994 }, /* copy tiles */
    { 0, 8, &gUnk_08339A18 }, /* copy tiles */
    { 0, 8, &gUnk_08339A9C }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08339D88-0x08339DD0: gRoomBgAnimSet1Script9, section .bg_anim_08339d88 ---- */
struct BgAnimCmd gRoomBgAnimSet1Script9[] BG_ANIM(08339d88) = {
    { 0, 8, &gUnk_08339B68 }, /* copy tiles */
    { 0, 8, &gUnk_08339BAC }, /* copy tiles */
    { 0, 8, &gUnk_08339BF0 }, /* copy tiles */
    { 0, 8, &gUnk_08339C34 }, /* copy tiles */
    { 0, 8, &gUnk_08339C78 }, /* copy tiles */
    { 0, 8, &gUnk_08339CBC }, /* copy tiles */
    { 0, 8, &gUnk_08339D00 }, /* copy tiles */
    { 0, 8, &gUnk_08339D44 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0833A0E8-0x0833A120: gRoomBgAnimSet2Script0, section .bg_anim_0833a0e8 ---- */
struct BgAnimCmd gRoomBgAnimSet2Script0[] BG_ANIM(0833a0e8) = {
    { 0, 8, &gUnk_08339DD0 }, /* copy tiles */
    { 0, 8, &gUnk_08339E54 }, /* copy tiles */
    { 0, 8, &gUnk_08339ED8 }, /* copy tiles */
    { 0, 8, &gUnk_08339F5C }, /* copy tiles */
    { 0, 8, &gUnk_08339FE0 }, /* copy tiles */
    { 0, 8, &gUnk_0833A064 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0833ABB8-0x0833ABF0: gRoomBgAnimSet3Script0, section .bg_anim_0833abb8 ---- */
struct BgAnimCmd gRoomBgAnimSet3Script0[] BG_ANIM(0833abb8) = {
    { 0, 7, &gUnk_0833A120 }, /* copy tiles */
    { 0, 7, &gUnk_0833A2E4 }, /* copy tiles */
    { 0, 7, &gUnk_0833A4A8 }, /* copy tiles */
    { 0, 7, &gUnk_0833A66C }, /* copy tiles */
    { 0, 7, &gUnk_0833A830 }, /* copy tiles */
    { 0, 7, &gUnk_0833A9F4 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0833EC10-0x0833EC58: gRoomBgAnimSet4Script0, section .bg_anim_0833ec10 ---- */
struct BgAnimCmd gRoomBgAnimSet4Script0[] BG_ANIM(0833ec10) = {
    { 0, 10, &gUnk_0833ABF0 }, /* copy tiles */
    { 0, 10, &gUnk_0833B3F4 }, /* copy tiles */
    { 0, 10, &gUnk_0833BBF8 }, /* copy tiles */
    { 0, 10, &gUnk_0833C3FC }, /* copy tiles */
    { 0, 10, &gUnk_0833CC00 }, /* copy tiles */
    { 0, 10, &gUnk_0833D404 }, /* copy tiles */
    { 0, 10, &gUnk_0833DC08 }, /* copy tiles */
    { 0, 10, &gUnk_0833E40C }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0833F45C-0x0833F4C4: gRoomBgAnimSet5Script2, section .bg_anim_0833f45c ---- */
struct BgAnimCmd gRoomBgAnimSet5Script2[] BG_ANIM(0833f45c) = {
    { 2, 292, NULL }, /* wait */
    { 6, 247, NULL }, /* sound effect */
    { 0, 4, &gUnk_0833EC58 }, /* copy tiles */
    { 0, 4, &gUnk_0833ED3C }, /* copy tiles */
    { 0, 4, &gUnk_0833EE20 }, /* copy tiles */
    { 0, 4, &gUnk_0833EF04 }, /* copy tiles */
    { 0, 4, &gUnk_0833EFE8 }, /* copy tiles */
    { 0, 4, &gUnk_0833F0CC }, /* copy tiles */
    { 0, 4, &gUnk_0833F1B0 }, /* copy tiles */
    { 0, 4, &gUnk_0833F294 }, /* copy tiles */
    { 0, 4, &gUnk_0833F378 }, /* copy tiles */
    { 5, 0x0905, (void *)0x10 }, /* collision tile */
    { 4, 0, NULL }, /* stop */
};

/* ---- 0x0833FCC8-0x0833FD30: gRoomBgAnimSet6Script0, section .bg_anim_0833fcc8 ---- */
struct BgAnimCmd gRoomBgAnimSet6Script0[] BG_ANIM(0833fcc8) = {
    { 2, 464, NULL }, /* wait */
    { 6, 247, NULL }, /* sound effect */
    { 0, 4, &gUnk_0833F4C4 }, /* copy tiles */
    { 0, 4, &gUnk_0833F5A8 }, /* copy tiles */
    { 0, 4, &gUnk_0833F68C }, /* copy tiles */
    { 0, 4, &gUnk_0833F770 }, /* copy tiles */
    { 0, 4, &gUnk_0833F854 }, /* copy tiles */
    { 0, 4, &gUnk_0833F938 }, /* copy tiles */
    { 0, 4, &gUnk_0833FA1C }, /* copy tiles */
    { 0, 4, &gUnk_0833FB00 }, /* copy tiles */
    { 0, 4, &gUnk_0833FBE4 }, /* copy tiles */
    { 5, 0x0902, (void *)0x10 }, /* collision tile */
    { 4, 0, NULL }, /* stop */
};

/* ---- 0x0833FE30-0x0833FE40: gUnk_0833FE30, section .bg_anim_0833fe30 ---- */
struct BgAnimPaletteFade gUnk_0833FE30 BG_ANIM(0833fe30) = {
    .src = gUnk_0833FD30,
    .dst = gUnk_0833FDB0,
    .colorIndex = 0,
    .colorCount = 64,
    .rate = 874,
};

/* ---- 0x0833FF40-0x0833FF50: gUnk_0833FF40, section .bg_anim_0833ff40 ---- */
struct BgAnimPaletteFade gUnk_0833FF40 BG_ANIM(0833ff40) = {
    .src = gUnk_0833FE40,
    .dst = gUnk_0833FEC0,
    .colorIndex = 0,
    .colorCount = 64,
    .rate = 874,
};

/* ---- 0x08340050-0x08340088: gUnk_08340050, gRoomBgAnimSet5Script0, section .bg_anim_08340050 ---- */
struct BgAnimPaletteFade gUnk_08340050 BG_ANIM(08340050) = {
    .src = gUnk_0833FF50,
    .dst = gUnk_0833FFD0,
    .colorIndex = 0,
    .colorCount = 64,
    .rate = 874,
};
struct BgAnimCmd gRoomBgAnimSet5Script0[] BG_ANIM(08340050) = {
    { 2, 65, NULL }, /* wait */
    { 1, 75, &gUnk_0833FE30 }, /* palette fade */
    { 1, 75, &gUnk_0833FF40 }, /* palette fade */
    { 1, 75, &gUnk_08340050 }, /* palette fade */
    { 4, 0, NULL }, /* stop */
};

/* ---- 0x08340108-0x08340118: gUnk_08340108, section .bg_anim_08340108 ---- */
struct BgAnimPaletteFade gUnk_08340108 BG_ANIM(08340108) = {
    .src = gUnk_08340088,
    .dst = gUnk_083400C8,
    .colorIndex = 64,
    .colorCount = 32,
    .rate = 874,
};

/* ---- 0x08340198-0x083401A8: gUnk_08340198, section .bg_anim_08340198 ---- */
struct BgAnimPaletteFade gUnk_08340198 BG_ANIM(08340198) = {
    .src = gUnk_08340118,
    .dst = gUnk_08340158,
    .colorIndex = 64,
    .colorCount = 32,
    .rate = 874,
};

/* ---- 0x08340228-0x08340260: gUnk_08340228, gRoomBgAnimSet5Script1, section .bg_anim_08340228 ---- */
struct BgAnimPaletteFade gUnk_08340228 BG_ANIM(08340228) = {
    .src = gUnk_083401A8,
    .dst = gUnk_083401E8,
    .colorIndex = 64,
    .colorCount = 32,
    .rate = 874,
};
struct BgAnimCmd gRoomBgAnimSet5Script1[] BG_ANIM(08340228) = {
    { 2, 65, NULL }, /* wait */
    { 1, 75, &gUnk_08340108 }, /* palette fade */
    { 1, 75, &gUnk_08340198 }, /* palette fade */
    { 1, 75, &gUnk_08340228 }, /* palette fade */
    { 4, 0, NULL }, /* stop */
};

/* ---- 0x08340360-0x08340370: gUnk_08340360, section .bg_anim_08340360 ---- */
struct BgAnimPaletteFade gUnk_08340360 BG_ANIM(08340360) = {
    .src = gUnk_08340260,
    .dst = gUnk_083402E0,
    .colorIndex = 0,
    .colorCount = 64,
    .rate = 656,
};

/* ---- 0x08340470-0x08340480: gUnk_08340470, section .bg_anim_08340470 ---- */
struct BgAnimPaletteFade gUnk_08340470 BG_ANIM(08340470) = {
    .src = gUnk_08340370,
    .dst = gUnk_083403F0,
    .colorIndex = 0,
    .colorCount = 64,
    .rate = 656,
};

/* ---- 0x08340580-0x08340590: gUnk_08340580, section .bg_anim_08340580 ---- */
struct BgAnimPaletteFade gUnk_08340580 BG_ANIM(08340580) = {
    .src = gUnk_08340480,
    .dst = gUnk_08340500,
    .colorIndex = 0,
    .colorCount = 64,
    .rate = 656,
};

/* ---- 0x08340690-0x083406D0: gUnk_08340690, gRoomBgAnimSet6Script1, section .bg_anim_08340690 ---- */
struct BgAnimPaletteFade gUnk_08340690 BG_ANIM(08340690) = {
    .src = gUnk_08340590,
    .dst = gUnk_08340610,
    .colorIndex = 0,
    .colorCount = 64,
    .rate = 656,
};
struct BgAnimCmd gRoomBgAnimSet6Script1[] BG_ANIM(08340690) = {
    { 2, 64, NULL }, /* wait */
    { 1, 100, &gUnk_08340360 }, /* palette fade */
    { 1, 100, &gUnk_08340470 }, /* palette fade */
    { 1, 100, &gUnk_08340580 }, /* palette fade */
    { 1, 100, &gUnk_08340690 }, /* palette fade */
    { 4, 0, NULL }, /* stop */
};

/* ---- 0x08340750-0x08340760: gUnk_08340750, section .bg_anim_08340750 ---- */
struct BgAnimPaletteFade gUnk_08340750 BG_ANIM(08340750) = {
    .src = gUnk_083406D0,
    .dst = gUnk_08340710,
    .colorIndex = 64,
    .colorCount = 32,
    .rate = 656,
};

/* ---- 0x083407E0-0x083407F0: gUnk_083407E0, section .bg_anim_083407e0 ---- */
struct BgAnimPaletteFade gUnk_083407E0 BG_ANIM(083407e0) = {
    .src = gUnk_08340760,
    .dst = gUnk_083407A0,
    .colorIndex = 64,
    .colorCount = 32,
    .rate = 656,
};

/* ---- 0x08340870-0x08340880: gUnk_08340870, section .bg_anim_08340870 ---- */
struct BgAnimPaletteFade gUnk_08340870 BG_ANIM(08340870) = {
    .src = gUnk_083407F0,
    .dst = gUnk_08340830,
    .colorIndex = 64,
    .colorCount = 32,
    .rate = 656,
};

/* ---- 0x08340900-0x08340940: gUnk_08340900, gRoomBgAnimSet6Script2, section .bg_anim_08340900 ---- */
struct BgAnimPaletteFade gUnk_08340900 BG_ANIM(08340900) = {
    .src = gUnk_08340880,
    .dst = gUnk_083408C0,
    .colorIndex = 64,
    .colorCount = 32,
    .rate = 656,
};
struct BgAnimCmd gRoomBgAnimSet6Script2[] BG_ANIM(08340900) = {
    { 2, 64, NULL }, /* wait */
    { 1, 100, &gUnk_08340750 }, /* palette fade */
    { 1, 100, &gUnk_083407E0 }, /* palette fade */
    { 1, 100, &gUnk_08340870 }, /* palette fade */
    { 1, 100, &gUnk_08340900 }, /* palette fade */
    { 4, 0, NULL }, /* stop */
};

/* ---- 0x083447D0-0x083447F8: gRoomBgAnimSet7Script0, section .bg_anim_083447d0 ---- */
struct BgAnimCmd gRoomBgAnimSet7Script0[] BG_ANIM(083447d0) = {
    { 0, 4, &gUnk_08340940 }, /* copy tiles */
    { 0, 4, &gUnk_083418E4 }, /* copy tiles */
    { 0, 4, &gUnk_08342888 }, /* copy tiles */
    { 0, 4, &gUnk_0834382C }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08344C08-0x08344C30: gRoomBgAnimSet8Script0, section .bg_anim_08344c08 ---- */
struct BgAnimCmd gRoomBgAnimSet8Script0[] BG_ANIM(08344c08) = {
    { 0, 8, &gUnk_083447F8 }, /* copy tiles */
    { 0, 8, &gUnk_083448FC }, /* copy tiles */
    { 0, 8, &gUnk_08344A00 }, /* copy tiles */
    { 0, 8, &gUnk_08344B04 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08348248-0x08348280: gRoomBgAnimSet10Script0, section .bg_anim_08348248 ---- */
struct BgAnimCmd gRoomBgAnimSet10Script0[] BG_ANIM(08348248) = {
    { 0, 2, &gUnk_08344C30 }, /* copy tiles */
    { 0, 2, &gUnk_08345534 }, /* copy tiles */
    { 0, 2, &gUnk_08345E38 }, /* copy tiles */
    { 0, 2, &gUnk_0834673C }, /* copy tiles */
    { 0, 2, &gUnk_08347040 }, /* copy tiles */
    { 0, 2, &gUnk_08347944 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0834B898-0x0834B8D0: gRoomBgAnimSet10Script1, section .bg_anim_0834b898 ---- */
struct BgAnimCmd gRoomBgAnimSet10Script1[] BG_ANIM(0834b898) = {
    { 0, 2, &gUnk_08348280 }, /* copy tiles */
    { 0, 2, &gUnk_08348B84 }, /* copy tiles */
    { 0, 2, &gUnk_08349488 }, /* copy tiles */
    { 0, 2, &gUnk_08349D8C }, /* copy tiles */
    { 0, 2, &gUnk_0834A690 }, /* copy tiles */
    { 0, 2, &gUnk_0834AF94 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0834EEE8-0x0834EF20: gRoomBgAnimSet10Script2, section .bg_anim_0834eee8 ---- */
struct BgAnimCmd gRoomBgAnimSet10Script2[] BG_ANIM(0834eee8) = {
    { 0, 2, &gUnk_0834B8D0 }, /* copy tiles */
    { 0, 2, &gUnk_0834C1D4 }, /* copy tiles */
    { 0, 2, &gUnk_0834CAD8 }, /* copy tiles */
    { 0, 2, &gUnk_0834D3DC }, /* copy tiles */
    { 0, 2, &gUnk_0834DCE0 }, /* copy tiles */
    { 0, 2, &gUnk_0834E5E4 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08350AF8-0x08350B30: gRoomBgAnimSet12Script0, section .bg_anim_08350af8 ---- */
struct BgAnimCmd gRoomBgAnimSet12Script0[] BG_ANIM(08350af8) = {
    { 0, 8, &gUnk_0834EF20 }, /* copy tiles */
    { 0, 8, &gUnk_0834F3C4 }, /* copy tiles */
    { 0, 8, &gUnk_0834F868 }, /* copy tiles */
    { 0, 8, &gUnk_0834FD0C }, /* copy tiles */
    { 0, 8, &gUnk_083501B0 }, /* copy tiles */
    { 0, 8, &gUnk_08350654 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08353248-0x08353280: gRoomBgAnimSet9Script0, section .bg_anim_08353248 ---- */
struct BgAnimCmd gRoomBgAnimSet9Script0[] BG_ANIM(08353248) = {
    { 0, 3, &gUnk_08350B30 }, /* copy tiles */
    { 0, 3, &gUnk_083511B4 }, /* copy tiles */
    { 0, 3, &gUnk_08351838 }, /* copy tiles */
    { 0, 3, &gUnk_08351EBC }, /* copy tiles */
    { 0, 3, &gUnk_08352540 }, /* copy tiles */
    { 0, 3, &gUnk_08352BC4 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x08355998-0x083559D0: gRoomBgAnimSet9Script1, section .bg_anim_08355998 ---- */
struct BgAnimCmd gRoomBgAnimSet9Script1[] BG_ANIM(08355998) = {
    { 0, 3, &gUnk_08353280 }, /* copy tiles */
    { 0, 3, &gUnk_08353904 }, /* copy tiles */
    { 0, 3, &gUnk_08353F88 }, /* copy tiles */
    { 0, 3, &gUnk_0835460C }, /* copy tiles */
    { 0, 3, &gUnk_08354C90 }, /* copy tiles */
    { 0, 3, &gUnk_08355314 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x083580E8-0x08358120: gRoomBgAnimSet9Script2, section .bg_anim_083580e8 ---- */
struct BgAnimCmd gRoomBgAnimSet9Script2[] BG_ANIM(083580e8) = {
    { 0, 3, &gUnk_083559D0 }, /* copy tiles */
    { 0, 3, &gUnk_08356054 }, /* copy tiles */
    { 0, 3, &gUnk_083566D8 }, /* copy tiles */
    { 0, 3, &gUnk_08356D5C }, /* copy tiles */
    { 0, 3, &gUnk_083573E0 }, /* copy tiles */
    { 0, 3, &gUnk_08357A64 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0835B5BC-0x0835B604: gRoomBgAnimSet11Script1, section .bg_anim_0835b5bc ---- */
struct BgAnimCmd gRoomBgAnimSet11Script1[] BG_ANIM(0835b5bc) = {
    { 0, 2, &gUnk_08358120 }, /* copy tiles */
    { 0, 3, &gUnk_083588A4 }, /* copy tiles */
    { 0, 4, &gUnk_08359028 }, /* copy tiles */
    { 0, 6, &gUnk_083597AC }, /* copy tiles */
    { 0, 6, &gUnk_08359F30 }, /* copy tiles */
    { 0, 6, &gUnk_083597AC }, /* copy tiles */
    { 0, 8, &gUnk_0835A6B4 }, /* copy tiles */
    { 0, 10, &gUnk_0835AE38 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};

/* ---- 0x0835CF9C-0x0835CFD4: gRoomBgAnimSet11Script0, section .bg_anim_0835cf9c ---- */
struct BgAnimCmd gRoomBgAnimSet11Script0[] BG_ANIM(0835cf9c) = {
    { 0, 8, &gUnk_0835B604 }, /* copy tiles */
    { 0, 8, &gUnk_0835BA48 }, /* copy tiles */
    { 0, 8, &gUnk_0835BE8C }, /* copy tiles */
    { 0, 8, &gUnk_0835C2D0 }, /* copy tiles */
    { 0, 8, &gUnk_0835C714 }, /* copy tiles */
    { 0, 8, &gUnk_0835CB58 }, /* copy tiles */
    { 3, 0, NULL }, /* loop */
};
