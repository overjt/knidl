#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* subgame_c1ffc.c (0x080C1FFC-0x080C243B, issue #98).
 *
 * Sub-game 2 (gUnk_02007FCC == 2), the race screen: phase 0 of M35's
 * framework (M36's AirGrindMain dispatches gSubGamePhase through
 * gUnk_087572CC; entry 0 is AirGrindRace).
 * 
 *   CreateAirGrindRacers / sub_080c2038 / sub_080c2078   spawn task type #96:
 *       variant 0 once per player (Task.unk1C = the player), variant 1 (its
 *       task index kept in M37Game.unk44C) and variant 2 (Task.unk18/unk1C/
 *       unk20 from the caller).
 *   AirGrindSetupRace   the state set-up: gAirGrindPtr = &gAirGrind,
 *       gAirGrindCoursePtr = &gAirGrindCourse, the linked-player count and mode
 *       cells, the four players' course records, the per-frame hook
 *       AirGrindBuildSky (gFrameCallback, called by the frame driver EndFrame)
 *       and the VBlank hook AirGrindSkyVBlankCallback (gVBlankCallback, called by the VBlank
 *       handler), and two 16-colour rows of gUnk_08609E40.
 *   AirGrindRace   the screen's task body: waits for the scroll position
 *       gAirGrindCoursePtr->unk000 to reach the two course lines unk00C and unk010
 *       (a sign sprite and songs 0x82B/0x82A/0x82C at each), then for all
 *       four players to pass unk010, and ends the screen (Task.unk18 = 2).
 *   AirGrindRaceUpdate   its per-frame callback: the frame counter gAirGrindFrame,
 *       the script cursor, the palette fades and M35's SubGameCheckEnd. */

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

extern struct M37Game gAirGrind;
extern struct M37Game *gAirGrindPtr;
extern struct M37Course gAirGrindCourse;
extern struct M37Course *gAirGrindCoursePtr;
extern u16 gAirGrindFrame;
extern u16 gLocalPlayer;
extern u16 gLinkPlayerCount;
extern u32 gFrameCallback;
extern u32 gVBlankCallback;
extern u16 gUnk_08609E40[];
extern u32 gUnk_08755FEC[];
extern u16 gUnk_0860A042[];
extern u32 gVBlankEndCallback;

void TaskYieldTrampoline(s32 frames);
void ClearHBlankIntr(void);
s32 PlayBgm(s32 songId);
void StopBgm(void);
void StopAllSfx(void);
s32 TaskCreateFrom(u32 type, s32 idx);                         /* spawn a task */
void TaskDrawScreen(void);
void TaskSleepForever(void);                                     /* end the running task */
void SubGameCheckEnd(void);
void AirGrindBuildSky(void);
void AirGrindSkyVBlankCallback(void);
void sub_080c4860(s32 y);
void AirGrindStepPaletteFades(void);                                  /* step the four palette fades gAirGrindPaletteFades[] */
void sub_080c495c(void);
s32 AirGrindStartPaletteFade(u16 *src, s32 pal, s32 period, s32 steps, s32 count, s32 repeat);
void AirGrindStopPaletteFade(s32 i);
void AirGrindSeedRandom(void);
void sub_080c51c0(void);
void sub_080c51d4(void);                                  /* step the gAirGrindScript script */
void AirGrindRaceUpdate(void);

void CreateAirGrindRacers(void)
{
    s32 i;
    s32 id;
    struct Task *t;

    for (i = 0; i < 4; i++) {
        id = TaskCreateFrom(96, 0);
        if (id != -1) {
            t = &gTasks[id];
            t->unk18 = id;
            t->unk1C = i;
            t->unk73 = 0;
        }
    }
}

void sub_080c2038(s32 unused)
{
    s32 id;
    struct Task *t;

    id = TaskCreateFrom(96, 32);
    if (id != -1) {
        t = &gTasks[id];
        t->unk73 = 1;
        gAirGrindPtr->unk44C = id;
    }
}

void sub_080c2078(s32 a, s32 b, s32 c)
{
    s32 id;
    struct Task *t;

    id = TaskCreateFrom(96, 32);
    if (id != -1) {
        t = &gTasks[id];
        t->unk18 = b;
        t->unk1C = a;
        t->unk73 = 2;
        t->unk20 = c;
    }
}

void AirGrindSetupRace(void)
{
    s32 i;

    gAirGrindPtr = &gAirGrind;
    gAirGrindCoursePtr = &gAirGrindCourse;
    gAirGrindPtr->localPlayer = gLocalPlayer;
    gAirGrindPtr->playerCount = gLinkPlayerCount;
    gAirGrindFrame = 0;
    StopBgm();
    StopAllSfx();
    AirGrindSeedRandom();
    sub_080c51c0();
    for (i = 0; i < 4; i++) {
        gAirGrindCoursePtr->players[i].coursePos = gAirGrindCoursePtr->scrollPos;
        gAirGrindCoursePtr->players[i].unk04 = 0;
    }
    gAirGrindPtr->unk014 = -1;
    gAirGrindPtr->frameCount = 0;
    gFrameCallback = (u32)AirGrindBuildSky;
    gVBlankCallback = (u32)AirGrindSkyVBlankCallback;
    sub_080c495c();
    for (i = 0; i < 16; i++) {
        gAirGrindPtr->unk0EC.unk76[i] = gUnk_08609E40[32 + i];
        gAirGrindPtr->unk0EC.unk96[i] = gUnk_08609E40[64 + i];
    }
}

void AirGrindRace(void)
{
    s32 n;
    s32 i;

    AirGrindSetupRace();
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->frameTable = gUnk_08755FEC;
    gCurTask->frame = 0xFFFF;
    PlayBgm(0x82B);
    CreateAirGrindRacers();
    sub_080c2038(0);
    gCurTask->updateCallback = (u32)AirGrindRaceUpdate;
    while (gAirGrindCoursePtr->scrollPos < gAirGrindCoursePtr->unk00C - 240)
        TaskYieldTrampoline(1);
    sub_080c4860(gAirGrindCoursePtr->unk00C);
    while (1) {
        if (gAirGrindCoursePtr->scrollPos >= gAirGrindCoursePtr->unk00C)
            break;
        TaskYieldTrampoline(1);
    }
    gCurTask->pixelX = 144;
    gCurTask->pixelY = 80;
    gCurTask->unk34 = AirGrindStartPaletteFade(gUnk_0860A042, 241, 10, 8, 15, 0);
    gCurTask->frame = 0;
    PlayBgm(0x82A);
    while (gAirGrindCoursePtr->scrollPos < gAirGrindCoursePtr->unk00C + 240)
        TaskYieldTrampoline(1);
    gCurTask->lateUpdateCallback = 0;
    gCurTask->frame = 0xFFFF;
    AirGrindStopPaletteFade(gCurTask->unk34);
    if (gFrameCallback != 0 && gAirGrind.level != 2) {
        while (gAirGrindPtr->frameCount <= 0x4AF)
            TaskYieldTrampoline(1);
        AirGrindStartPaletteFade(&gAirGrindPtr->unk0EC.unk76[1], 161, 256, 2, 6, 1);
    }
    while (gAirGrindCoursePtr->scrollPos < gAirGrindCoursePtr->finishLine - 240)
        TaskYieldTrampoline(1);
    sub_080c4860(gAirGrindCoursePtr->finishLine);
    while (1) {
        if (gAirGrindCoursePtr->scrollPos >= gAirGrindCoursePtr->finishLine)
            break;
        TaskYieldTrampoline(1);
    }
    while (gAirGrindCoursePtr->players[0].coursePos < gAirGrindCoursePtr->finishLine)
        TaskYieldTrampoline(1);
    gCurTask->pixelX = 112;
    gCurTask->pixelY = 80;
    gCurTask->unk34 = AirGrindStartPaletteFade(gUnk_0860A042, 241, 10, 8, 15, 0);
    gCurTask->frame = 1;
    PlayBgm(0x82C);
    while (1) {
        n = 0;
        for (i = 0; i < 4; i++)
            if (gAirGrindCoursePtr->players[i].coursePos > gAirGrindCoursePtr->finishLine)
                n++;
        if (n > 3)
            break;
        TaskYieldTrampoline(1);
    }
    TaskYieldTrampoline(180);
    StopBgm();
    StopAllSfx();
    ClearHBlankIntr();
    gVBlankEndCallback = 0;
    gCurTask->unk18 = 2;
    TaskSleepForever();
}

void AirGrindRaceUpdate(void)
{
    gAirGrindFrame++;
    sub_080c51d4();
    AirGrindStepPaletteFades();
    SubGameCheckEnd();
}
