#ifndef GUARD_PLAYER_H
#define GUARD_PLAYER_H

#include "gba/types.h"

/* player.h: the RAM cells and ROM tables of the player: animation bank and
   collision registry (M05), breakable blocks and the player task (M09), the
   action machine and its objects (M10-M14).  One declaration per symbol, with
   the type its consumers prove (issue #36 phase 2,
   docs/header-conventions.md). */

struct Unk020061F0;

/* A hit-box set: unk0 & 0x8000 = mirror with the task's facing, unk0 & 0xFFF
   = the attack id passed to CanBreakBlock; unk2/unk3 = (x, y) offset of the
   set; unk4 = the boxes, {y0, y1, x0, x1} each (x mirrored as -x1..-x0),
   terminated by y0 == 127. */
struct HitBoxSet
{
    /*0x00*/ u16 unk0;
    /*0x02*/ s8 offsetX;
    /*0x03*/ s8 offsetY;
    /*0x04*/ s8 (*boxes)[4];
};

/* 16-byte per-slot record, 3 slots per player, at gUnk_02007E90.
   unk00/unk04 are 16.16 (x,y) offsets whose high halves are read directly,
   unk08 is the 16.16 y-delta, unk0C a down-counter, unk0D a frame id. */
struct M04Spark
{
    /*0x00*/ s32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ u8 unk0C;
    /*0x0D*/ u8 unk0D;
    /*0x0E*/ u16 unk0E;
};

struct M11Buf { u8 unk00[4]; u8 unk04[4]; };

struct M11R20 { u32 w[5]; };

/* M11's per-player records */
struct M11R8 { u8 unk00; u8 unk01; u8 offsetX; u8 offsetY; u8 *boxes; };

/* gUnk_0873B510[]: a palette fade, src/dst palettes and the blend step */
struct M12Fade
{
    /*0x00*/ u16 *unk0;
    /*0x04*/ u16 *unk4;
    /*0x08*/ s32 unk8;
};

struct Unk02005E00
{
    /*0x00*/ s32 unk00;
    /*0x04*/ u8 unk04[4];
    /*0x08*/ u8 unk08[4];
};

/* one step of a player's knock-back script: {dx, dy, flags} with
   flags & 15 = frames to hold, & 64 = mirror dx with the facing,
   & 128 = sound; a zero flags byte ends the script */
struct Unk0873A994
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 filler5[3];
};

/* EWRAM */
extern u16 gUnk_02000028;
extern u8 gBlockCursorPlayer; /*   the player that hit it */
extern u8 gUnk_02004B48; /*   hit-box id bit 11 */
extern s16 gUnk_02004B6C[]; /*   y (pixels) */
extern struct M11R8 gPlayerHitBoxSets[];
extern u8 gUnk_020055C4;
extern struct Unk02005E00 gUnk_02005E00;
extern u16 gBlockCursorIndex; /*   map index */
extern struct M11R20 gPlayerBodyBoxes[];
extern u16 gUnk_02006174; /*   the block kind (hit-box id low byte) */
extern s16 gBlockCursorTile; /*   the metatile's collision byte */
extern u8 gUnk_020061E0;
extern struct M11Buf gUnk_02006A80[];
extern u16 gBlockCursorX; /* the block CanBreakBlock accepted: x */
extern struct M04Spark gUnk_02007E90[][3];
extern u16 gUnk_02007F60[];
extern struct Unk020061F0 gUnk_02007FD0;
extern u16 gBlockCursorY; /*   y */
extern u16 gPlayerBubbleTimers[];
extern u16 gUnk_0200B060[];
extern u32 gUnk_02020000[]; /* decompression buffer */

/* IWRAM */
extern u16 gUnk_03001490[];
extern u8 gUnk_030023B0;

/* ROM */
extern u8 gUnk_080D07C8[];
extern u32 gUnk_080D2148[];
extern u16 gPlayerPalettes[][16]; /* per-player palettes (M03 spelling) */
extern u32 gUnk_080DC728[];
extern u8 gUnk_080DCA28[];
extern u32 gUnk_080DCC28[];
extern u32 gUnk_080DCC48[];
extern u8 gUnk_080DCC68[];
extern u32 gUnk_081AC358[];
extern u8 gUnk_081AC378[];
extern u8 gUnk_081BBD70[];
extern u8 gUnk_081BC050[];
extern u8 gUnk_081BE45C[];
extern u8 gUnk_081BE6BC[]; /* per-player palettes, 128 bytes each */
extern u8 gUnk_081BFE38[];
extern u8 gUnk_081CC328[];
extern u8 gUnk_081CF260[];
extern u8 gUnk_081D5B04[];
extern u8 gUnk_081DCDFC[];
extern u8 gUnk_081E1D0C[];
extern u8 gUnk_081E43B4[];
extern u8 gUnk_081EFD60[];
extern u8 gUnk_081F1AE0[];
extern u16 gUnk_081F59F0[];
extern u8 gUnk_081F6CEC[];
extern u8 gUnk_08200D08[];
extern u16 gUnk_08203098[];
extern u16 gUnk_082030B8[];
extern u8 gUnk_082036D8[];
extern u8 gUnk_08204B78[];
extern u8 gUnk_08204B98[];
extern u8 gUnk_082181F0[];
extern u16 gUnk_08226254[];
extern u32 gNightmarePowerOrbGfx[];
extern u32 gUnk_085E24D8[];
extern u32 gUnk_085E26E8[];
extern u16 gUnk_085E2920[];
extern u16 gUnk_085E2A20[];
extern u16 gUnk_085E2B20[];
extern s16 gUnk_08732150[][16];
extern s16 gUnk_08732190[];
extern s16 gUnk_087321A6[];
extern s16 gUnk_087321B2[];
extern u32 gUnk_087321C0[];
extern u32 gUnk_087321EC[];
extern u16 gUnk_0873A458[];
extern u16 *gUnk_0873A47C[]; /* animation script per block kind */
extern s8 gUnk_0873A494[];
extern s8 gUnk_0873A5D4[];
extern u16 gUnk_0873A6D4[][3];
extern u16 gUnk_0873A6EC[][3][3];
extern s8 gUnk_0873A734[][2];
extern void (*gPlayerActions[])(void);
extern void (*gPlayerActionHandlers[])(void);
extern s16 gUnk_0873A924[][16];
extern u32 gUnk_0873A964[];
extern struct Unk0873A994 *gUnk_0873A994[];
extern u16 gUnk_0873AEBC[][2];
extern u16 gUnk_0873AF0C[][2];
extern s8 gUnk_0873AF20[][2];
extern u8 gUnk_0873AF30[][2];
extern u8 gUnk_0873AF3A[][2];
extern u8 gUnk_0873AF42[];
extern u16 gUnk_0873AF58[][2];
extern u32 gUnk_0873AF6C[];
extern u32 gPlayerMotionXPresets[];
extern u32 gPlayerMotionYPresets[];
extern void (*gUnk_0873B42C[])(void);
extern void (*gUnk_0873B4A4[])(void);
extern struct M12Fade gUnk_0873B510[];
extern u16 gUnk_0873B534[][32];
extern u8 gUnk_0873B634[];
extern s16 gUnk_0873B654[];
extern u8 gUnk_0873B65E[];
extern void (*gUnk_0873B664[])(void); /* enter 49's sub-actions [9], indexed by Task.variant */
extern void (*gUnk_0873B688[])(void); /* handler 46's per-frame sub-handlers [9] */
extern void (*gUnk_0873B6AC[])(void); /* enter 58's sub-actions [4] */
extern void (*gUnk_0873B6BC[])(void); /* handler 55's per-frame sub-handlers [4] */
extern u16 gUnk_0873B6CC[][2];
extern u8 gUnk_0873B6DC[];
extern u16 gUnk_0873B6E8[][6];
extern u16 gUnk_0873B724[][4];
extern void (*gPlayerObjectVariants[])(void); /* task type #6's 13 variants, indexed by Task.unk18 >> 24 */
extern u16 gUnk_0873B7B0[][2];
extern u16 gUnk_0873B7C0[][2][6];
extern u8 gUnk_0873B808[][6][5];
extern u16 gUnk_0873B862[][2];
extern u8 gUnk_0873B872[][8];
extern u8 gUnk_0873B88A[][5];
extern u16 gUnk_0873B8C6[][2][8];
extern u32 gPlayerDefaultBodyBox[]; /* stored to PlayerState.bodyBox as (u32)gPlayerDefaultBodyBox */
extern u32 gUnk_0873BD14[];
extern u32 gUnk_0873BD28[];
extern u32 gUnk_0873BD3C[];
extern u32 gUnk_0873BD50[];
extern u32 gUnk_0873BD64[]; /* collider row passed to RegisterCollider (4th arg) */
extern u32 gUnk_0873BD78[];
extern u32 gUnk_0873BD8C[];
extern u32 gUnk_0873BDA0[];
extern u32 gUnk_0873BDB4[];
extern u32 gUnk_0873BDD4[];
extern u32 gUnk_0873BDE8[];
extern u32 gUnk_0873BDFC[];
extern u32 gUnk_0873BE10[];
extern u32 gUnk_0873BE24[];
extern u32 gUnk_0873BE38[];
extern u32 gUnk_0873BE4C[];
extern u32 gUnk_0873BE60[];
extern u32 gUnk_0873BE74[];
extern u32 gUnk_0873BE88[];
extern u8 gUnk_0873BE9C[];
extern u8 gUnk_0873BEB0[];
extern u8 gUnk_0873BEC4[];
extern u32 gUnk_0873BED8[]; /* collider row passed to RegisterCollider (4th arg) */
extern u8 gUnk_0873BEEC[];
extern u32 gUnk_0873BF00[];
extern u32 gUnk_0873BF14[];
extern u32 gUnk_0873BF28[];
extern u32 gUnk_0873BF3C[];
extern u32 gUnk_0873BF64[];
extern u32 gUnk_0873BF84[];
extern u32 gUnk_0873BF98[];
extern u32 gUnk_0873BFD8[];
extern u32 gUnk_0873C060[];
extern u32 gUnk_0873C074[];
extern u32 gUnk_0873C0C0[];
extern u32 gUnk_0873C128[];
extern u32 gUnk_0873C1B0[];
extern u32 gUnk_0873C1C4[];
extern u32 gUnk_0873C1EC[];
extern u32 gUnk_0873C214[]; /* collider row passed to RegisterCollider (4th arg) */
extern u32 gUnk_0873C228[];
extern u32 gUnk_0873C264[];
extern u32 gUnk_0873C278[];
extern u32 gUnk_0873C28C[];
extern u32 gUnk_0873C2A0[];
extern u32 gUnk_0873C2C8[];
extern u32 gUnk_0873C2DC[];
extern u32 gUnk_0873C304[];
extern u32 gUnk_0873C318[];
extern u32 gUnk_0873C358[];
extern u32 gUnk_0873C36C[];
extern u32 gUnk_0873CA54[];
extern u32 gUnk_0873CA68[];
extern u32 gUnk_0873CA7C[];
extern u32 gUnk_0873CA90[];
extern u32 gUnk_0873CAA4[];
extern u32 gPlayerDefaultTerrainBox[];
extern u32 gUnk_0873CB24[];
extern u32 gUnk_0873CB2C[];
extern u32 gUnk_0873CB34[];
extern u32 gUnk_0873CB3C[];
extern s8 gUnk_0873CB44[]; /* collision box passed to sub_0802205c / sub_0801c230 */
extern s8 gUnk_0873CB4C[];
extern s8 gUnk_0873CB54[];
extern s8 gUnk_0873CB5C[];
extern s8 gUnk_0873CB64[];
extern s8 gUnk_0873CB6C[];
extern u32 gUnk_0873CB84[];
extern u32 gUnk_0873CB94[];
extern u32 gUnk_0873CBA4[];
extern u32 gUnk_0873CBAC[];
extern u32 gUnk_0873CBB4[];
extern u32 gUnk_0873CBDC[];
extern u32 gUnk_0873CBEC[];
extern u32 gUnk_0873CBFC[];
extern u32 gUnk_0873CC0C[];
extern u32 gUnk_0873CC1C[];
extern u32 gUnk_0873CC2C[];
extern u32 gUnk_0873CC3C[];
extern u32 gUnk_0873CC44[];
extern struct HitBoxSet gUnk_0873CC54;
extern u32 gUnk_0873CC64[]; /* hit-box set, passed as (struct HitBoxSet *) */
extern u32 gUnk_0873CC74[];
extern u32 gUnk_0873CC84[];
extern u32 gUnk_0873CCA4[];
extern u32 gUnk_0873CCAC[];
extern u32 gUnk_0873CCB4[];
extern u32 gUnk_0873CCFC[];
extern u32 gUnk_0873CD04[];
extern u32 gUnk_0873CD44[];
extern u32 gUnk_0873CDAC[];
extern u32 gUnk_0873CDB4[];
extern u32 gUnk_0873CDBC[];
extern u32 gUnk_0873CDF4[];
extern u32 gUnk_0873CDFC[];
extern u32 gUnk_0873CE64[];
extern u32 gUnk_0873CEEC[];
extern u32 gUnk_0873CEF4[];
extern u32 gUnk_0873CF1C[];
extern u32 gUnk_0873CF4C[]; /* hit-box set, passed as (struct HitBoxSet *) */
extern u32 gUnk_0873CF5C[];
extern u32 gUnk_0873CF6C[];
extern u32 gUnk_0873CF7C[];
extern u32 gUnk_0873CF94[];
extern u32 gUnk_0873CF9C[];
extern u32 gUnk_0873D03C[];
extern u32 gUnk_0873D044[];
extern u32 gUnk_0873D04C[];
extern u8 gAbilityBButtonActions[];
extern u16 gUnk_0873D0F8[][5];
extern s16 gUnk_0873D206[];
extern s16 gUnk_0873D210[];
extern s16 gUnk_0873D2E0[];
extern u16 gUnk_0873D2E8[];
extern u16 gUnk_0873D31C[];
extern u16 gUnk_0873D350[];
extern s16 gUnk_0873D3B8[][2];
extern u16 gUnk_0873D4BC[][5];
extern s16 gUnk_0873D5C0[];
extern s16 gUnk_0873D5CA[][2];
extern u16 gPlayerDoorAnims[][7];
extern u16 gUnk_0873D79E[];
extern s16 gUnk_0873D7E4[][3];
extern s16 gUnk_0873D880[];
extern u16 gUnk_0873D8B4[];
extern u16 gUnk_0873D908[];
extern u32 gUnk_0873D986[];
extern s16 gUnk_0873D9DA[4][4];
extern u16 gUnk_0873D9FA[][2];
extern u16 gUnk_0873DA62[][2];
extern s16 gUnk_0873DACA[][2];
extern u16 gUnk_0873DADE[][2];
extern u16 gUnk_0873DB0A[];
extern s16 gUnk_0873DB34[];
extern u16 gUnk_0873DB44[][2];
extern u32 gUnk_0874C478[];
extern u32 gUnk_0874C4E4[];
extern u32 gUnk_0874C650[];
extern u32 gUnk_0874C7CC[];
extern u32 gUnk_0874CE90[];
extern u32 gUnk_0874CFEC[];
extern u32 gUnk_08751990[];
extern u32 gUnk_087519CC[];
extern u32 gUnk_087519E8[];
extern u32 gUnk_08751A28[];
extern u32 gUnk_08751A98[];
extern u32 gUnk_08751AF8[];
extern u32 gUnk_08751B40[];
extern u32 gUnk_08751BB0[];
extern u32 gUnk_08751BF4[];
extern u32 gUnk_08751CA4[];
extern u32 gUnk_08751E5C[];
extern u32 gUnk_0875204C[];
extern u32 gUnk_087520A8[];
extern u32 gUnk_08755068[];
extern u32 gUnk_08755378[];
extern u32 gUnk_087553DC[];
extern u32 gUnk_087553FC[];
extern u32 gUnk_08755440[];
extern u32 gUnk_0875546C[];
extern u32 gUnk_08755484[];
extern u8 gUnk_08757368[];

#endif /* GUARD_PLAYER_H */
