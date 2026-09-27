#ifndef GUARD_SUBGAME_H
#define GUARD_SUBGAME_H

#include "gba/types.h"

/* subgame.h: the RAM cells and ROM tables of the sub-game framework and the
   three sub-games (M35-M37).  One declaration per symbol, with the type its
   consumers prove (issue #36 phase 2, docs/header-conventions.md). */

struct GfxDesc
{
    u16 unk00;
    u16 unk02;
    u32 unk04;
    u32 unk08;
    const void *unk0C;
};

/* per-player records of gAirGrindCourse, M37Course.players[4] (0x3C bytes) */
struct M37CoursePlayer
{
    /*0x00*/ s32 coursePos;
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ s32 unk20;
    /*0x24*/ s32 unk24;
    /*0x28*/ s32 unk28;
    /*0x2C*/ s32 unk2C;
    /*0x30*/ s32 unk30;
    /*0x34*/ s32 prevCoursePos;
    /*0x38*/ s32 unk38;
};

/* gAirGrindCourse, reached through gAirGrindCoursePtr (and directly by the
   0x080C5284-0x080C623C builder) */
struct M37Course
{
    /*0x000*/ s32 scrollPos;
    /*0x004*/ s32 unk004;
    /*0x008*/ s32 unk008;
    /*0x00C*/ s32 unk00C;
    /*0x010*/ s32 finishLine;
    /*0x014*/ s32 unk014;
    /*0x018*/ struct M37CoursePlayer players[4];
    /*0x108*/ s32 unk108;
    /*0x10C*/ s32 unk10C;
    /*0x110*/ s32 unk110;
};

/* per-player records, M37Game.players[4] (0x34 bytes) */
struct M37Player
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u16 unk02;
    /*0x04*/ u16 unk04;
    /*0x06*/ s16 unk06;
    /*0x08*/ s16 unk08;
    /*0x0A*/ s16 unk0A;
    /*0x0C*/ s16 unk0C;
    /*0x0E*/ u16 unk0E;
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ u8 unk20;
    /*0x21*/ u8 unk21;
    /*0x22*/ u8 pad22[2];
    /*0x24*/ s32 unk24;
    /*0x28*/ s32 unk28;
    /*0x2C*/ s32 unk2C;
    /*0x30*/ s32 unk30;
};

/* 16-byte object records, M37ObjSet.unk04[7] (sub_080c4664, sub_080c4790) */
struct M37Obj
{
    /*0x00*/ s16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ s32 unk4;
    /*0x08*/ s32 unk8;          /* 16.16; the ROM also reads its high half */
    /*0x0C*/ s32 unkC;          /* 16.16; the ROM also reads its high half */
};

/* M37Game.unk0EC (0xB8 bytes with the compiler's 2-byte tail pad): task
   type #96 variant 1's seven scrolling objects.  sub_080c4664 addresses an
   object as &set->unk04[i] off the set's own base (`lsls #4; adds #4`), so
   the records are a sub-struct, not flat fields; the two 16-colour rows
   AirGrindSetupRace fills end it exactly at M37Game.randomStates. */
struct M37ObjSet
{
    /*0x00*/ s32 unk00;         /* the scroll position last frame */
    /*0x04*/ struct M37Obj unk04[7];
    /*0x74*/ s16 unk74;         /* the last object's sprite id */
    /*0x76*/ u16 unk76[16];     /* M37Game + 0x162 */
    /*0x96*/ u16 unk96[16];     /* M37Game + 0x182 */
};

/* gAirGrind, the game's state; always used through gAirGrindPtr */
struct M37Game
{
    /*0x000*/ s32 level;       /* the level (M36's AirGrindInit copies gSubGameLevel) */
    /*0x004*/ s32 raceTimes[4];
    /*0x014*/ s32 unk014;
    /*0x018*/ s32 unk018;
    /*0x01C*/ struct M37Player players[4];
    /*0x0EC*/ struct M37ObjSet unk0EC;
    /*0x1A4*/ s32 randomStates[5];    /* five LCG streams (AirGrindRandom, AirGrindRandomRange) */
    /*0x1B8*/ s32 unk1B8;
    /*0x1BC*/ u16 skyLineColors[160]; /* per-scanline colour, HBlank DMA source */
    /*0x2FC*/ u16 backdropColor;
    /*0x2FE*/ u8 pad2FE[2];
    /*0x300*/ u32 frameCount;       /* frame counter */
    /*0x304*/ s16 unk304;       /* sub_080c4f60's OAM list: entry count */
    /*0x306*/ s16 unk306[160];  /* ... and entries */
    /*0x446*/ u16 localPlayer;       /* gLocalPlayer */
    /*0x448*/ u16 playerCount;       /* gLinkPlayerCount */
    /*0x44A*/ u8 pad44A[2];
    /*0x44C*/ s32 unk44C;       /* a task index into gTasks */
    /*0x450*/ u8 unk450;
    /*0x451*/ u8 unk451;
    /*0x452*/ u8 pad452[2];
};

/* the results screen's state (AirGrindResults, sub_080c25c4, sub_080c2740) */
struct M37Results
{
    /*0x00*/ s8 unk00;          /* state */
    /*0x01*/ s8 unk01;
    /*0x02*/ s8 unk02;
    /*0x03*/ s8 unk03;
    /*0x04*/ u8 unk04[4];       /* the players, sorted by score */
    /*0x08*/ u8 unk08[4];       /* each player's index into unk04 */
    /*0x0C*/ u8 unk0C[4];       /* the places, ties shared */
    /*0x10*/ s32 unk10;         /* the winner's pulsing scale */
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;         /* frame timer */
    /*0x1C*/ s32 unk1C[4];
};

/* gAirGrindScript: a cursor into one of the u16-pair scripts gUnk_087572EC[]
   (sub_080c51c0 clears it, sub_080c51d4 steps it, 0x8000 = end, 0x9999 =
   loop) */
struct M37Script
{
    /*0x00*/ u8 scriptId;           /* script id, 0 = none */
    /*0x01*/ u8 step;           /* step */
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
};

/* four 40-byte records at gAirGrindPaletteFades (AirGrindStartPaletteFade fills one) */
struct M37Timer
{
    /*0x00*/ s32 unk00;         /* in use */
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ s32 unk20;
    /*0x24*/ s32 unk24;
};

/* EWRAM */
extern u8 gUnk_02004B5C;
extern s16 gUnk_020055EC;
extern u8 gQuickDrawBestTime;
extern u8 gUnk_020061DC;
extern s8 gBombRallySeats[];
extern u8 gSubGamePhase;
extern u8 gBombRallyOutMask;
extern u8 gBombRallyOutCount;
extern u8 gQuickDrawWins[];
extern u8 gBombRallyFinishOrder[];
extern u8 gUnk_0200B048;
extern u8 gQuickDrawRanking[];
extern struct M37Game gAirGrind;
extern struct M37Game *gAirGrindPtr;
extern struct M37Timer gAirGrindPaletteFades[4];
extern struct M37Results gAirGrindResults;
extern struct M37Course *gAirGrindCoursePtr;
extern u16 gAirGrindFrame;
extern s16 gUnk_02017180[4][256];
extern s16 gUnk_02017980[4][500];
extern u32 gUnk_02018920[];
extern s16 gUnk_02019140[4][500];
extern u32 gUnk_0201A0E0[4][256];
extern struct M37Course gAirGrindCourse;
extern s16 gUnk_0201B1F4;
extern s32 gUnk_0201B200[4][73];
extern s32 gUnk_0201B690[];
extern s16 gUnk_0201B7C0[4][256];
extern s32 gUnk_0201BFC0;

/* IWRAM */
extern u16 gUnk_03001510[];
extern struct M37Script gAirGrindScript;

/* ROM */
extern u8 gUnk_080CFE2C[][4];
extern s32 gUnk_080CFE3C[][3];
extern u8 gUnk_080CFE60[11][3];
extern u8 gUnk_080CFE81[11][3];
extern u8 gUnk_080CFEA2[11][3];
extern u8 gUnk_080CFEC3[11][3];
extern u8 gUnk_080CFEE4[];
extern u8 gUnk_080CFEE9[];
extern s8 gUnk_080CFF01[];
extern u16 gUnk_080CFF1C[];
extern s16 gUnk_080CFF52[];
extern s16 gUnk_080CFF60[];
extern s16 gUnk_080CFF70[];
extern u8 gUnk_080CFF76[4][4][2]; /* OBJ shape/size -> {width, height} */
extern u16 gUnk_080D0198[];
extern s16 gUnk_080D0398[];
extern s16 gUnk_080D059A[];
extern s16 gUnk_080D075A[];
extern s16 gUnk_080D0760[];
extern s16 gUnk_080D0766[];
extern u16 gUnk_08609D42[][16];
extern u16 gUnk_08609E40[];
extern u16 gUnk_08609F40[][16];
extern u16 gUnk_0860A042[];
extern u32 gUnk_087559E4[];
extern u32 gUnk_087559F4[];
extern u32 gUnk_08755A04[];
extern u32 gUnk_08755A14[];
extern u32 gUnk_08755A24[];
extern u32 gUnk_08755A34[];
extern u32 gUnk_08755A5C[];
extern u32 gUnk_08755A68[];
extern u32 gUnk_08755A78[];
extern u32 gUnk_08755A7C[];
extern u32 gUnk_08755A88[];
extern u32 gUnk_08755AA4[];
extern u32 gUnk_08755AB8[];
extern u32 gUnk_08755AC8[];
extern u32 gUnk_08755AD8[];
extern u32 gUnk_08755ADC[];
extern u32 gUnk_08755AF0[];
extern u32 gUnk_08755B18[];
extern u32 gUnk_08755B40[];
extern u32 gUnk_08755B68[];
extern u32 gUnk_08755B90[];
extern u32 gUnk_08755BAC[];
extern u32 gUnk_08755DC0;
extern u32 gUnk_08755E00[];
extern u32 gUnk_08755E0C[];
extern u32 gUnk_08755E44[];
extern u32 gUnk_08755E7C[];
extern u32 gUnk_08755EB4;
extern u32 gUnk_08755EB8[];
extern u32 gUnk_08755EC4[];
extern u32 gUnk_08755EFC[];
extern u32 gUnk_08755F1C[];
extern u32 gUnk_08755F2C[];
extern u32 gUnk_08755F3C[];
extern u16 *gUnk_08755F54[];
extern u32 gUnk_08755FA8[];
extern u32 gUnk_08755FBC[];
extern u32 gUnk_08755FC4[];
extern u32 gUnk_08755FEC[];
extern u32 gUnk_0875602C[];
extern u32 gUnk_0875603C[];
extern u32 gUnk_087562A8[][2];
extern u16 gUnk_087562C0[];
extern s32 (*const gSubGameInitHooks[])(void);
extern void *gSubGameBodies[];
extern u16 gUnk_087562E4[];
extern u32 gQuickDrawPhases[];
extern s16 gUnk_087562F0[];
extern s16 gUnk_087562F6[];
extern u32 gQuickDrawStates[];
extern u32 gQuickDrawStateUpdates[];
extern u32 gUnk_08756334[];
extern u32 gUnk_08756350[];
extern s16 gUnk_0875636C[];
extern u32 gUnk_08756378[];
extern u32 gUnk_08756394[];
extern u32 gQuickDrawObjectKinds[];
extern u16 gUnk_087563D8[];
extern u16 gUnk_08756410[];
extern s16 gUnk_08756448[];
extern s16 gUnk_08756450[];
extern s16 gUnk_08756458[];
extern s16 gUnk_08756460[];
extern u32 gUnk_08756468[];
extern u32 gUnk_08756480[];
extern u16 gUnk_08756498[];
extern u32 gUnk_087564A0[];
extern s16 gUnk_087564B0[];
extern struct GfxDesc *const gUnk_087564D0[];
extern u32 gUnk_087564E4[];
extern u32 gUnk_087564FC[];
extern u16 gUnk_08756514[];
extern u16 gUnk_0875651C[];
extern u32 gUnk_08756528[];
extern u16 gUnk_08756538[];
extern s32 gUnk_08756540[];
extern s32 gUnk_08756550[];
extern s8 gUnk_08756560[];
extern s8 gUnk_08756564[];
extern u32 gBombRallyPhases[];
extern u8 gUnk_08756570[];
extern u8 gUnk_087565E0[];
extern u8 *gUnk_087565F4[];
extern u8 *gUnk_08756650[];
extern u8 gUnk_0875665C[];
extern u8 gUnk_0875665F[];
extern u8 gUnk_08756662[];
extern u32 gBombRallyStates[];
extern u32 gBombRallyStateUpdates[];
extern u32 gBombRallyResultsStates[];
extern u32 gBombRallyResultsStateUpdates[];
extern u32 gBombRallyObjectVariants[];
extern u32 gBombRallyPlayerStates[];
extern u32 gBombRallyPlayerStateUpdates[];
extern u32 *gUnk_0875670C[];
extern u32 *gUnk_0875671C[];
extern s16 gUnk_0875672C[];
extern s16 gUnk_08756734[];
extern s8 gUnk_0875673C[];
extern s8 gUnk_08756740[];
extern s8 gUnk_08756744[];
extern s8 gUnk_08756748[];
extern u32 gUnk_0875674C[];
extern u32 gUnk_0875675C[];
extern u8 gUnk_0875676C[];
extern s16 gUnk_08756770[];
extern u16 gUnk_08756778[];
extern u32 gUnk_08756780[];
extern u32 gUnk_0875678C[];
extern s16 gUnk_08756798[];
extern s16 gUnk_087567A0[];
extern s32 gUnk_087567A8[][3];
extern s32 *gUnk_08756D3C[][2];
extern s32 gUnk_08756D74[];
extern s32 gUnk_08756DC8[];
extern s32 gUnk_08756E1C[];
extern s32 gUnk_08756E38[];
extern s32 gUnk_08756E54[][4];
extern s32 gUnk_08756EC4[][4];
extern s32 gUnk_08756F34[][4];
extern s32 gUnk_08756FA4[][4];
extern s16 gUnk_08757014[];
extern s32 *gUnk_0875716C[];
extern s32 gUnk_08757178[][4];
extern s32 gUnk_087571B8[][4];
extern u32 gUnk_087571F8[];
extern u32 gUnk_08757238[];
extern u32 gUnk_08757244[];
extern s32 gUnk_08757250[];
extern s32 gUnk_08757260[];
extern u32 gUnk_08757270[];
extern u32 gUnk_08757278[];
extern u8 gUnk_08757280[];
extern u32 gAirGrindPhases[];
extern void (*gUnk_087572D4[])(void);
extern u32 gUnk_087572E0[];
extern u16 *gUnk_087572EC[];
extern s32 *gUnk_08757300[];
extern s32 *gUnk_08757310[];
extern u16 *gUnk_08757320[];

#endif /* GUARD_SUBGAME_H */
