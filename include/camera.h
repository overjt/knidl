#ifndef GUARD_CAMERA_H
#define GUARD_CAMERA_H

#include "gba/types.h"
#include "room.h"

/* camera.h: the RAM cells and ROM tables of the camera, the BG map streaming,
   the map events and the stage objects #221-#236 (M08).  One declaration per
   symbol, with the type its consumers prove (issue #36 phase 2,
   docs/header-conventions.md). */

struct Unk02007D70
{
    /*0x00*/ u16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ struct Unk02007D70Cmd *unk4;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ u16 *unkC;
    /*0x10*/ u16 *unk10;
    /*0x14*/ u16 unk14;
    /*0x16*/ u16 unk16;
    /*0x18*/ u32 unk18;
};

struct Unk02007D70Cmd
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ void *unk4;
};

struct Unk03004B00
{
    /*0x00*/ u32 unk0;
    /*0x04*/ u32 unk4;
    /*0x08*/ u32 unk8;
};

/* EWRAM */
extern u8 gBlockAnimHookId;
extern u8 gUnk_02005E10[];
extern struct Unk02007D70 gBgAnims[];
extern u8 gUnk_02007FC4;
extern struct Unk020061F0 gBg1BreakingBlocks[];
extern s32 gUnk_02016C30;

/* IWRAM */
/* Actor-vs-player hit test cells (M17's src/actor_673ec.c widths). */
extern s16 gViewRect[]; /* camera rectangle: left, right, top, bottom */
extern struct Unk03004B00 gUnk_03004B00;

/* ROM */
extern u16 gUnk_080D71A0[];
extern u8 gUnk_085A0638[];
extern u8 gUnk_085A0C38[][32];
extern u8 gUnk_085A12F8[];
extern u8 gUnk_085A1BF8[];
extern u8 gUnk_085A24F8[];
extern u8 gUnk_085A2DF8[][32];
extern u16 gUnk_087323C6[][2];
extern s8 gUnk_08732428[][3];
extern s8 gUnk_087324A6[][3];
extern u16 *gScreenShakePatterns[];
extern void (*gMapEventVariants[])(void);
extern u8 gUnk_087328BC[];
extern s16 gUnk_087328C0[][2];
extern void (*gUnk_087328D8[])(void);
extern u32 gUnk_0874CD54[];
extern u32 gUnk_0874CD68[];
extern u32 gUnk_0874CDE0[];
extern u32 gUnk_08752548[];
extern u32 gUnk_087558BC[];
extern u32 gUnk_087558C0[];
extern u32 gUnk_087558C4[];
extern u32 gUnk_087558D0[];
extern u32 gUnk_087558DC[];
extern u32 gUnk_087558E8[];
extern u32 gUnk_087558EC[];
extern u32 gUnk_087558FC[];
extern u32 gUnk_08755930[];
extern u32 gUnk_0875593C[];
extern u32 gUnk_08755944[];
extern u32 gUnk_08755948[];
extern u32 gUnk_08755978[];
extern u32 gUnk_0875597C[];
extern u32 gUnk_0875599C[];
extern u32 gUnk_087559A4[];
extern u32 gUnk_087559C0[];
extern u32 gUnk_087559DC[];
extern struct Unk02007D70Cmd **gRoomBgAnimScripts[];

#endif /* GUARD_CAMERA_H */
