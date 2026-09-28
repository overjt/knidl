/* game_code_and_rodata_080653ec_0806ef5c 0x080C0DE8-0x080C1FFC
 * (issue #66, module M36 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080C0DE8 0x080C1FFC src/subgame_c0de8.c --newpb
 *
 * Tail of M36: the sub-game's presentation layer.
 *
 *   sub_080c0de8 .. BombRallyStartSign   fourteen near-identical class-4 sprite
 *       bodies, one per on-screen element; each sets Task.updateState/unk38 from
 *       its own gUnk_08755Exx animation script and walks a fixed 16.16
 *       position list with TaskYieldTrampoline.
 *   BombRallyResultsPlayer / BombRallyResultsPlayerPose    the two dispatch bodies (switch on
 *       Task.unk1C / Task.unk20).
 *   BombRallyResultsPlayerLives / BombRallyResultsPlayerLivesUpdate    slot placement: Task.posX/unk50 from
 *       gBombRallyResultsSlotX/gBombRallyResultsSlotY, and the per-slot horizontal offset
 *       switch over gBombRallyFinishOrder[Task.unk20].
 *   BombRallyMenuItem / BombRallyMenuItemUpdate    the results task.
 *   BombRallyMenuItemContinueUpdate / BombRallyMenuItemLevel / BombRallyMenuItemLevelUpdate   the three ranking markers.
 *   BombRallyShakeScreen   walks the 4-direction path script at gBombRallyBlastShake in
 *       6.0 steps, writing gBg3ScrollX/gBg3ScrollY (terminator 128).
 *   sub_080c1f88 / AirGrindInit / AirGrindMain   the score-record reset:
 *       four 60-byte records at gAirGrindCourse + 0x18, the player count into
 *       gAirGrind, and the final dispatcher hand-off.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "hud.h"
#include "room.h"

/* Not from subgame.h: this file's view of gAirGrind differs (lesson 3.517). */
extern u8 gBombRallySafeBeatsLeft;
extern s8 gBombRallySeats[];
extern u8 gBombRallyOutMask;
extern u8 gBombRallyOutCount;
extern u8 gBombRallyFinishOrder[];

extern s8 gUnk_08756560[];
extern s8 gUnk_08756564[];
extern u32 gBombRallyBombFrames[];
extern u32 gBombRallyBlastNearFrames[];
extern u32 gBombRallyBlastSideFrames[];
extern u32 gBombRallyBlastFarFrames[];
extern s32 gBombRallySeatScrollX[];
extern s32 gBombRallySeatScrollY[];
extern s16 gBombRallySeatBombX[];
extern s32 gBombRallyArcVelX[][3];
extern s32 *gUnk_08756D3C[][2];
extern u32 gBombRallyBombShadowFrames;
extern u32 gUnk_08755EFC[];
extern u32 gBombRallyResultsPoseFrames[];
extern u32 gBombRallyContinueItemFrames[];
extern u32 gBombRallyLevelItemFrames[];
extern u32 gBombRallyResultsPlayerStates[];
extern u32 gBombRallyResultsPlayerStateUpdates[];
extern s32 gBombRallyResultsSlotX[];
extern s32 gBombRallyResultsSlotY[];
extern u32 gBombRallyMenuItemStates[];
extern u32 gBombRallyMenuItemStateUpdates[];
extern u8 gBombRallyBlastShake[];
extern u32 gAirGrindPhases[];
extern u8 gAirGrind[];
extern s32 gAirGrindCourse[];
extern u8 gSubGamePhase;
extern u32 gBombRallyBombSmokeFrames[];
extern u32 gBombRallyStarFrames[];
extern u32 gBombRallyStarBurstStates[];
extern s16 gBombRallySeatShadowY[];
extern s32 *gUnk_0875716C[];
extern s32 gBombRallyPanStepX[][4];
extern s32 gBombRallyPanStepY[][4];
extern s32 gUnk_08756D74[];
extern s32 gUnk_08756DC8[];
extern s32 gUnk_08756E1C[];
extern s32 gUnk_08756E38[];
extern s32 gUnk_08756E54[][4];
extern s32 gUnk_08756EC4[][4];
extern s32 gUnk_08756F34[][4];
extern s32 gUnk_08756FA4[][4];
extern s16 gUnk_08756770[];
extern u16 gUnk_08756778[];
extern s8 gUnk_0875673C[];
extern s16 gBombRallySeatBombY[];
extern u8 gUnk_0875676C[];
extern u32 gBombRallyBombStates[];
extern u32 gBombRallyBombStateUpdates[];
extern u32 *gBombRallyBubblesFrames[];
extern u32 gUnk_0875674C[];
extern u32 gUnk_0875675C[];
extern u32 gUnk_08756528[];
extern u8 gBombRallyBeatFrames[];
extern u8 gBombRallyStartSpeeds[];
extern u8 gBombRallySpeedUpBeats[];
extern u8 gBombRallySafeBeats[];
extern u32 gBombRallyStates[];
extern u32 gBombRallyStateUpdates[];
extern u32 gBombRallyResultsStates[];
extern u32 gBombRallyResultsStateUpdates[];
extern u32 gBombRallyObjectVariants[];
extern u32 gUnk_08755DC0;
extern u16 gUnk_08756538[];
extern u8 gUnk_087565E0[];
extern u8 *gBombRallyBlastOdds[];
extern u8 *gUnk_087565F4[];
extern u32 *gBombRallyPlayerFrames[];
extern s16 gUnk_0875672C[];
extern s16 gUnk_08756734[];
extern s8 gUnk_08756740[];
extern s8 gUnk_08756744[];
extern s8 gUnk_08756748[];
extern u32 gBombRallyPlayerStates[];
extern u32 gBombRallyPlayerStateUpdates[];

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskSetEntry(void *a, u32 i);
extern void CreateBombRallyBurstStar(u32 a);
extern void BombRallyBombSmokeUpdate(void);
extern void BombRallyStartSignUpdate(void);
extern void BombRallyResultsPlayerUpdate(void);
extern void BombRallyMenuItemUpdate(void);
extern void PlaySfx(u32 a);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern u32 RandomRange(u32 range);
extern void SubGameReplay(s32 a);
extern void SubGameQuit(void);
extern void SubGameCheckEnd(void);

extern void BombRallyRoundUpdate(void);
extern void BombRallyEnterState(void);
extern u32 BombRallyKnockOutTurnPlayer(void);
extern u32 BombRallyIsMatchOver(void);
extern void BombRallyInitSpeed(void);
extern void BombRallyRestartSpeed(void);
extern void CreateBombRallyBomb(u32 a);
extern void BombRallySeatPlayers(void);
extern void BombRallyResultsUpdate(void);
extern void BombRallyResultsEnterState(void);
extern void CreateBombRallyResultsPoses(void);
extern void CreateBombRallyLivesIcons(void);
extern void CreateBombRallyPlaceLabels(void);
extern void CreateBombRallyContinueItems(u32 a);
extern void CreateBombRallyLevelItems(u32 a);
extern void BombRallyAwardLives(void);
extern void BombRallyPlayerUpdate(void);
extern u32 BombRallyPlayerJudgePress(void);
extern void BombRallyPlayerUpdatePose(void);
extern void BombRallyPlayerEnterState(void);
extern void BombRallyBombUpdate(void);
extern void BombRallyBombEnterState(void);
extern void CreateBombRallyStartSign(void);
extern void BombRallyBombPlaceAtSeat(u32 a);
extern void BombRallyScrollToSeat(u32 a);
extern void BombRallyPanToSeat(u32 a);
extern s32 BombRallyShakeScreen(s32 a, s32 b);
extern void BombRallyBombSetArcPos(s32 a, s32 b, s32 c, s32 d);
extern void BombRallyScrollAlongPass(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void BombRallyBombDrawShadow(s32 a, s32 b, s32 c, s32 d, s16 e);
/* NOTE: CreateBombRallyBombSmoke's third parameter is `u16` at its definition
   (src/subgame_bda2c.c) but the ROM's call sites here sign-extend the
   argument, so the declaration visible here is the wider `s32` - the
   original source had the same prototype mismatch. */
extern void CreateBombRallyBombSmoke(s32 a, s32 b, s32 c, s32 d);
extern void CreateBombRallyStarBurst(s32 a, s32 b, u32 c, u32 d);
extern void BombRallyPlaySfxIfPlayer0(u32 a);

void sub_080c0de8(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0x80 << 10, 0xFFFFF400);
    t = gCurTask;
    t->velY = 0xFFFF1000;
    t->accelY = 0xA0 << 3;
    TaskSetFrame(0);
    TaskYieldTrampoline(3);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    TaskStop();
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskExitTrampoline();
}

void sub_080c0e88(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0x80 << 10, 0xFFFFF400);
    t = gCurTask;
    t->velY = 0xF0 << 8;
    t->accelY = 0xFFFFFB00;
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskSetFrame(0);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskStop();
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskExitTrampoline();
}

void sub_080c0f54(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0xFFFE0000, 0xC0 << 4);
    t = gCurTask;
    t->velY = 0xF0 << 8;
    t->accelY = 0xFFFFFB00;
    TaskSetFrame(0);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 5);
    TaskStop();
    TaskSetFrame(7);
    TaskYieldTrampoline(2);
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void sub_080c0fe4(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0xFFFE0000, 0xC0 << 4);
    t = gCurTask;
    t->velY = 0xFFFF1000;
    t->accelY = 0xA0 << 3;
    TaskSetFrame(0);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    TaskStop();
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void sub_080c1068(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0xFFFF1000, 0xA0 << 3);
    t = gCurTask;
    t->velY = 0xFFFE0000;
    t->accelY = 0xC0 << 4;
    TaskSetFrame(0);
    TaskYieldTrampoline(3);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    TaskSetFrame(1);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskExitTrampoline();
}

void sub_080c1114(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0x80 << 10, 0xFFFFF400);
    t = gCurTask;
    t->velY = 0xFFFF1000;
    t->accelY = 0xA0 << 3;
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskSetFrame(0);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskStop();
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskExitTrampoline();
}

void sub_080c11cc(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0x80 << 10, 0xFFFFF400);
    t = gCurTask;
    t->velY = 0xF0 << 8;
    t->accelY = 0xFFFFFB00;
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 4);
    TaskStop();
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskExitTrampoline();
}

void sub_080c1260(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0xFFFE0000, 0xC0 << 4);
    t = gCurTask;
    t->velY = 0xF0 << 8;
    t->accelY = 0xFFFFFB00;
    TaskSetFrame(8);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskExitTrampoline();
}

void sub_080c1300(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0xFFFE0000, 0xC0 << 4);
    t = gCurTask;
    t->velY = 0xFFFF1000;
    t->accelY = 0xA0 << 3;
    TaskSetFrame(4);
    TaskYieldTrampoline(3);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskStop();
    TaskSetFrame(0);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void sub_080c1390(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0xFFFF1000, 0xA0 << 3);
    t = gCurTask;
    t->velY = 0xFFFE0000;
    t->accelY = 0xC0 << 4;
    TaskSetFrame(4);
    TaskYieldTrampoline(3);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    TaskStop();
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskExitTrampoline();
}

void sub_080c1424(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0x80 << 10, 0xFFFFF400);
    t = gCurTask;
    t->velY = 0xFFFF1000;
    t->accelY = 0xA0 << 3;
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    TaskStop();
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskExitTrampoline();
}

void sub_080c14b8(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0x80 << 10, 0xFFFFF400);
    t = gCurTask;
    t->velY = 0xF0 << 8;
    t->accelY = 0xFFFFFB00;
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskStop();
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskExitTrampoline();
}

void sub_080c1558(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0xFFFE0000, 0xC0 << 4);
    t = gCurTask;
    t->velY = 0xF0 << 8;
    t->accelY = 0xFFFFFB00;
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetFrame(8);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskStop();
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskExitTrampoline();
}

void sub_080c1608(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0xFFFE0000, 0xC0 << 4);
    t = gCurTask;
    t->velY = 0xFFFF1000;
    t->accelY = 0xA0 << 3;
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetFrame(10);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskStop();
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void sub_080c168c(void)
{
    struct Task *t;

    TaskSetMotionXFacing(0xFFFF1000, 0xA0 << 3);
    t = gCurTask;
    t->velY = 0xFFFE0000;
    t->accelY = 0xC0 << 4;
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskStop();
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskSetFrame(4);
    TaskYieldTrampoline(12);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void BombRallyStartSign(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawScreen;
    t->updateCallback = (u32)BombRallyStartSignUpdate;
    t->layer = 6;
    u = gCurTask;
    u->frameTable = gUnk_08755EFC;
    u->posX = 0xF0 << 15;
    u->posY = 0xE0 << 15;
    u->frame = 0;
    TaskYieldTrampoline(46);
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(16);
    gCurTask->frame = 1;
    TaskYieldTrampoline(30);
    TaskExitTrampoline();
}

void BombRallyStartSignUpdate(void)
{
}

void BombRallyResultsPlayer(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawScreen;
    t->updateCallback = (u32)BombRallyResultsPlayerUpdate;
    switch (t->unk1C) {
    case 0:
        t->state = t->unk1C;
        break;
    case 1:
        t->state = t->unk1C;
        break;
    case 2:
        t->state = t->unk1C;
        break;
    }
    CallTableEntry(gCurTask->state, 3, gBombRallyResultsPlayerStates);
    TaskSleepForever();
}

void BombRallyResultsPlayerUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 3, gBombRallyResultsPlayerStateUpdates);
}

void BombRallyResultsPlayerEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gBombRallyResultsPlayerStates);
}

void BombRallyResultsPlayerPose(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = 0;
    gCurTask->layer = 9;
    u = gCurTask;
    u->frameTable = gBombRallyResultsPoseFrames;
    u->tileWord = (u->unk18 << 12) | (0x80 << 4);
    if (gPlayerCount == 1) {
        u->posX = 0xF0 << 15;
        u->posY = 0xC0 << 15;
    } else {
        u->posX = gBombRallyResultsSlotX[u->unk18] << 16;
        u->posY = gBombRallyResultsSlotY[u->unk18] << 16;
    }
    v = gCurTask;
    v->frame = gBombRallyFinishOrder[v->unk20];
    TaskSleepForever();
}

void BombRallyResultsPlayerPoseUpdate(void)
{
}

void BombRallyResultsPlayerLives(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 n;

    t = gCurTask;
    t->updateState = 2;
    u = gCurTask;
    u->unk28 = 0;
    if (gPlayerCount == 1) {
        u->posX = 0xF0 << 15;
        u->posY = 0xC0 << 15;
    } else {
        u->posX = gBombRallyResultsSlotX[u->unk18] << 16;
        u->posY = gBombRallyResultsSlotY[u->unk18] << 16;
    }
    n = gBombRallyFinishOrder[gCurTask->unk20];
    switch (n) {
    case 3:
        gCurTask->unk2C = -16;
        break;
    case 2:
        gCurTask->unk2C = -8;
        break;
    case 0:
    case 1:
        gCurTask->unk2C = 0;
        break;
    }
    TaskSleepForever();
}

void BombRallyResultsPlayerLivesUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *z;
    s32 r;

    switch (gBombRallyFinishOrder[gCurTask->unk20]) {
    case 3:
        t = gCurTask;
        if (t->unk28 > 39) {
            if (t->unk28 == 40 && t->unk20 == gLocalPlayer)
                PlaySfx(220);
            QueueSprite(7, gUnk_08755EFC[6], 0, 0,
                         (s16)gCurTask->pixelX + gCurTask->unk2C + 32,
                         (s16)(gCurTask->pixelY - 24));
        }
        /* fall through */
    case 2:
        v = gCurTask;
        if (v->unk28 > 19) {
            if (v->unk28 == 20 && v->unk20 == gLocalPlayer)
                PlaySfx(220);
            QueueSprite(7, gUnk_08755EFC[6], 0, 0,
                         (s16)gCurTask->pixelX + gCurTask->unk2C + 16,
                         (s16)(gCurTask->pixelY - 24));
        }
        /* fall through */
    case 1:
        v = gCurTask;
        if (v->unk28 >= 0) {
            if (v->unk28 == 0 && v->unk20 == gLocalPlayer)
                PlaySfx(220);
            QueueSprite(7, gUnk_08755EFC[6], 0, 0,
                         (s16)gCurTask->pixelX + gCurTask->unk2C,
                         (s16)(gCurTask->pixelY - 24));
        }
        break;
    case 0:
        t = gCurTask;
        if (t->unk28 >= 0)
            QueueSprite(7, gUnk_08755EFC[7], 0, 0,
                         (s16)t->pixelX + t->unk2C,
                         (s16)(t->pixelY - 24));
        break;
    }
    z = gCurTask;
    if (z->unk28 <= 40)
        z->unk28++;
}

void BombRallyResultsPlayerPlace(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = 1;
    gCurTask->layer = 7;
    u = gCurTask;
    u->frameTable = gUnk_08755EFC;
    if (gPlayerCount == 1) {
        u->posX = 0xF0 << 15;
        u->posY = 0x90 << 15;
    } else {
        u->posX = gBombRallyResultsSlotX[u->unk18] << 16;
        u->posY = (gBombRallyResultsSlotY[u->unk18] - 24) << 16;
    }
    v = gCurTask;
    v->frame = gBombRallyFinishOrder[v->unk20] + 2;
    TaskSleepForever();
}

void BombRallyResultsPlayerPlaceUpdate(void)
{
}

void BombRallyMenuItem(void)
{
    struct Task *t;
    s32 n;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawScreen;
    t->updateCallback = (u32)BombRallyMenuItemUpdate;
    n = t->unk74;
    if (n == 0)
        t->state = n;
    else
        t->state = 1;
    CallTableEntry(gCurTask->state, 2, gBombRallyMenuItemStates);
    TaskSleepForever();
}

void BombRallyMenuItemUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 2, gBombRallyMenuItemStateUpdates);
}

void BombRallyMenuItemContinue(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    t = gCurTask;
    t->frameTable = gBombRallyContinueItemFrames;
    t->frame = t->unk18;
    t->unk28 = 0;
    if (t->unk18 == t->unk1C) {
        t->posX = 0xDC << 15;
        t->posY = 0xFE << 15;
        t->layer = 3;
    } else {
        t->posX = 0x82 << 16;
        t->posY = 0x89 << 16;
        t->layer = 4;
    }
    TaskSleepForever();
}

void BombRallyMenuItemContinueUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 n;

    t = gCurTask;
    u = &gTasks[t->parent];
    if (u->unk18 == 4)
        n = 0;
    else
        n = t->unk28 + 1;
    t->unk28 = n;
    v = gCurTask;
    if (u->unk2C != v->unk18) {
        if ((s16)v->pixelY <= 136) {
            v->posY += 0x80 << 9;
            v->pixelY = v->posY >> 16;
            v->posX += 0x80 << 10;
            v->pixelX = v->posX >> 16;
            if (u->unk30 == 0)
                v->layer = 4;
            else
                v->layer = 3;
        } else {
            v->layer = 4;
        }
        w = gCurTask;
        w->frame = w->unk18 << 1;
    } else {
        if ((s16)v->pixelY > 127) {
            v->posY += 0xFFFF0000;
            v->pixelY = v->posY >> 16;
            v->posX += 0xFFFE0000;
            v->pixelX = v->posX >> 16;
            v->frame = v->unk18 << 1;
            if (u->unk30 == 0)
                v->layer = 3;
            else
                v->layer = 4;
        } else {
            v->frame = (v->unk18 << 1) + ((v->unk28 >> 1) & 1);
            v->layer = 3;
        }
    }
    if (u->unk28 != 0)
        TaskFree(gCurTaskIdx);
}

void BombRallyMenuItemLevel(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    t = gCurTask;
    t->updateState = 1;
    u = gCurTask;
    u->unk34 = 0;
    u->frameTable = gBombRallyLevelItemFrames;
    u->unk6C = 0;
    do {
        v = gCurTask;
        if (v->unk18 == v->unk1C) {
            v->layer = (u8)v->unk6C + 3;
            w = gCurTask;
            w->frame = w->unk18 << 1;
            w->posX = (((s16)w->unk6C * 5) << 18) + (0xC8 << 15);
            w->posY = (((s16)w->unk6C * 5) << 17) + (0xF4 << 15);
        }
        x = gCurTask;
        x->unk1C++;
        if (x->unk1C == 3)
            x->unk1C = 0;
        y = gCurTask;
        y->unk6C++;
    } while ((s16)y->unk6C <= 2);
    TaskSleepForever();
}

void BombRallyMenuItemLevelUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    s32 n;

    t = gCurTask;
    u = &gTasks[t->parent];
    if (u->unk18 == 4)
        n = 0;
    else
        n = t->unk34 + 1;
    t->unk34 = n;
    v = gCurTask;
    v->unk28 = u->unk2C;
    v->unk6C = 0;
    do {
        w = gCurTask;
        if (w->unk18 == w->unk28) {
            w->unk2C = (((s16)w->unk6C * 5) << 1) + 122;
            w->unk30 = (s16)w->unk6C + 3;
        }
        x = gCurTask;
        x->unk28++;
        if (x->unk28 == 3)
            x->unk28 = 0;
        y = gCurTask;
        y->unk6C++;
    } while ((s16)y->unk6C <= 2);
    z = gCurTask;
    if ((s16)z->pixelY > z->unk2C) {
        z->posY += 0xFFFE0000;
        z->pixelY = z->posY >> 16;
        z->posX += 0xFFFC0000;
        z->pixelX = z->posX >> 16;
        z->frame = z->unk18 << 1;
        if (u->unk30 == 0)
            z->layer = z->unk30;
    } else if ((s16)z->pixelY < z->unk2C) {
        z->posY += 0x80 << 10;
        z->pixelY = z->posY >> 16;
        z->posX += 0x80 << 11;
        z->pixelX = z->posX >> 16;
        z->frame = z->unk18 << 1;
        if (u->unk30 == 0)
            z->layer = z->unk30;
    } else {
        if (z->unk18 == u->unk2C)
            z->frame = (z->unk18 << 1) + ((z->unk34 >> 1) & 1);
        gCurTask->layer = gCurTask->unk30;
    }
    if (u->unk28 != 1)
        TaskFree(gCurTaskIdx);
}

s32 BombRallyShakeScreen(s32 a, s32 b)
{
    u16 x;
    u16 y;
    u8 *p;
    u8 *base;

    base = gBombRallyBlastShake;
    p = base + b * 2;
    if (p[0] == 128) {
        gBg3ScrollX = gBombRallySeatScrollX[a];
        gBg3ScrollY = gBombRallySeatScrollY[a];
        return b;
    }
    x = (s8)p[0];
    y = (s8)p[1];
    switch (a) {
    case 0:
        y = ((u32)(y << 16) + 0xFFFA0000) >> 16;
        break;
    case 1:
        x = ((u32)(x << 16) + (0xC0 << 11)) >> 16;
        break;
    case 2:
        y = ((u32)(y << 16) + (0xC0 << 11)) >> 16;
        break;
    case 3:
        x = ((u32)(x << 16) + 0xFFFA0000) >> 16;
        break;
    }
    gBg3ScrollX = gBombRallySeatScrollX[a] + (x << 16);
    gBg3ScrollY = gBombRallySeatScrollY[a] + (y << 16);
    return b + 1;
}
void sub_080c1f88(void)
{
    u8 *p;

    p = gAirGrind;
    *(u16 *)(p + 772) = 0;
}

void AirGrindInit(void)
{
    s32 i;
    s32 zero;
    s32 j;
    u8 *p;
    s8 *q;
    s32 *r;

    p = gAirGrind;
    q = &gSubGameLevel;
    r = gAirGrindCourse;
    zero = 0;
    j = 51;
    for (i = 3; i >= 0; i--) {
        r[j] = zero;
        j -= 15;
    }
    *(s32 *)p = *q;
    p[1104] = 0;
    p[1105] = 0;
}

void AirGrindMain(void)
{
    CallTableEntry(gSubGamePhase, 2, gAirGrindPhases);
    TaskSleepForever();
}
