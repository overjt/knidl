/* game_code_and_rodata 0x0806AD18-0x0806B2E4 (issue #64, module M18 batch 2b).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806AD18 0x0806B2E4 src/actor_6ad18.c --newpb
 *
 * The screen-transition half of the module. sub_0806ad18 nudges the carried
 * actor one frame either way; sub_0806adb0 and BossDefeatScreenFlash are the two
 * DISPCNT-shadow fades (gDispCnt masked to 0xE0FF and re-ORed with the
 * BG-enable pattern, alternating a ROM window descriptor with a copy of
 * gBgPalette on the stack); ActorDefeatMidBoss plays the six-step
 * gUnk_0873E6A0/gUnk_0873E6D0 blend-and-offset table. sub_0806b070
 * dispatches through gUnk_0873E734[Task.unk76], sub_0806b098 is the
 * transition body proper, and ActorDefeatBoss is the entry point that saves and
 * restores Task.tileWord around it. The tail (0x0806B12C-0x0806B2E4) is the
 * small task-field setters and their gUnk_0873E7xx descriptor tables.
 */

#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "sound.h"
#include "hud.h"
#include "room.h"
#include "actor.h"

/* Not from main.h: this file's view of gBgPalette differs (lesson 3.517). */
extern vu16 gDispCnt; /* DISPCNT shadow */
extern u16 gBgPalette;

extern void RequestScreenShake(u32);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void LoadBackdropColor(u16 *);
extern void PlaySfx(u32);
extern void TaskSetMotionXFacing(u32, u32);
extern void TaskStop(void);
extern void TaskSetFrame(u32);

extern void CallTableEntry(u32, u32, void *);
extern void TaskSetEntry(void (*)(void), u32);
extern void ActorSetState(u32);
extern void ActorCheckHits(void);
extern void ActorReactToHit(void);
extern u8 ActorCollideTerrain(void);

void sub_0806ad18(void)
{
    struct Task *t;

    t = gCurTask;
    t->u8C.actor->extraFrame = 0;
    t->posY = (t->pixelY + 1) << 16;
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do
    {
        t = gCurTask;
        t->posY = (t->pixelY - 2) << 16;
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->posY = (t->pixelY + 2) << 16;
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->unk6C++;
    } while ((s16)t->unk6C <= 11);
    t = gCurTask;
    t->posY = (t->pixelY - 1) << 16;
    TaskYieldTrampoline(2);
    gCurTask->u8C.actor->extraFrame = 0xFFFF;
}

void sub_0806adb0(void)
{
    struct Task *t;
    u16 v;

    FreezeStage(15);
    SetRoomUpdateFlags(2);
    RequestScreenShake(5);
    v = gBgPalette;
    gCurTask->unk6C = 0;
    do
    {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1000;
        LoadBackdropColor(&gUnk_0873E69A);
        TaskYieldTrampoline(3);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
        LoadBackdropColor(&v);
        TaskYieldTrampoline(3);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1000;
        LoadBackdropColor(&gUnk_0873E69C);
        TaskYieldTrampoline(3);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
        LoadBackdropColor(&v);
        TaskYieldTrampoline(3);
        t = gCurTask;
        t->unk6C++;
    } while ((s16)t->unk6C <= 1);
    RequestScreenShake(0);
    LoadBackdropColor(&v);
    ThawStage();
}

void sub_0806ae94(void)
{
    PlaySfx(0x200);
    TaskSetSkipMask(14, gCurTaskIdx);
    sub_0806adb0();
    TaskSetSkipMask(0, gCurTaskIdx);
}

void ActorDefeatMidBoss(void)
{
    struct Task *t;

    TaskStop();
    TaskSetFrame(0);
    CreateChildTaskHere(142, 0);
    gCurTask->unk6C = 0;
    do
    {
        TaskSetMotionXFacing(gUnk_0873E6A0[(s16)gCurTask->unk6C], 0x5A5A5A5A);
        t = gCurTask;
        t->velY = gUnk_0873E6D0[(s16)t->unk6C];
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->unk6C++;
    } while ((s16)t->unk6C <= 5);
    TaskStop();
    t = gCurTask;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->frameTable = gUnk_0874CA78;
    t->tileWord = 0;
    RequestScreenShake(4);
    ActorPlaySfx(0x1F9, 0);
    CreateBurstEffect(1, 0);
    TaskYieldTrampoline(20);
}

void BossDefeatScreenFlash(void)
{
    struct Task *t;
    u16 v;

    FreezeStage(15);
    SetRoomUpdateFlags(2);
    RequestScreenShake(5);
    v = gBgPalette;
    gCurTask->unk6C = 0;
    do
    {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1000;
        LoadBackdropColor(&gUnk_0873E72E);
        TaskYieldTrampoline(4);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
        LoadBackdropColor(&v);
        TaskYieldTrampoline(3);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1000;
        LoadBackdropColor(&gUnk_0873E730);
        TaskYieldTrampoline(4);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
        LoadBackdropColor(&v);
        TaskYieldTrampoline(3);
        t = gCurTask;
        t->unk6C++;
    } while ((s16)t->unk6C <= 1);
    RequestScreenShake(0);
    LoadBackdropColor(&v);
    ThawStage();
}

void sub_0806b05c(void)
{
    PlaySfx(0x1FF);
    BossDefeatScreenFlash();
}

void sub_0806b070(void)
{
    void (*f)(void);

    f = (void (*)(void))gUnk_0873E734[gCurTask->unk76];
    if (f != NULL)
        f();
}

void sub_0806b098(void)
{
    FreezeStage(15);
    sub_080668c8();
    SetRoomUpdateFlags(2);
    ActorPlaySfx(0x1FD, 0);
    RequestScreenShake(4);
    CreateStarRing();
    CreateBurstEffect(1, 0);
    gCurTask->frame = 0xFFFF;
    sub_0806b070();
    TaskYieldTrampoline(24);
    ThawStage();
    DisablePause();
}

s32 sub_0806b0f0(void)
{
    switch (gCurTask->unk76)
    {
    case 1:
    case 3:
    case 4:
    case 5:
    case 6:
        if (gGameState != 20)
            StopBgm();
        break;
    case 2:
        break;
    default:
        StopBgm();
        break;
    }
}

void ActorDefeatBoss(void)
{
    gUnk_02007D00[9] = gCurTask->tileWord;
    HudRemoveHpBar();
    sub_0806b0f0();
    sub_0806b05c();
    sub_0806b098();
    gCurTask->tileWord = gUnk_02007D00[9];
    LoadStarRodPieceGfx();
    CallTableEntry(gCurTask->unk76, 9, gUnk_0873E758);
}

void sub_0806b178(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->hitEffect > 3)
        t->hitEffect = 0;
    CallTableEntry(gCurTask->hitEffect, 4, gUnk_0873E77C);
}

void sub_0806b1a8(void)
{
    TaskStop();
    TaskSetFrame(0);
    ActorPlaySfx(212, 0);
    sub_08069fc8();
}

void sub_0806b1c4(void)
{
    struct Task *t;

    TaskStop();
    TaskSetFrame(0);
    t = gCurTask;
    t->frameTable = gUnk_0874C9D8;
    t->tileWord = 0;
    ActorPlaySfx(212, 0);
    sub_08069fc8();
}

void sub_0806b1f4(void)
{
    struct Task *t;

    TaskStop();
    TaskSetFrame(0);
    t = gCurTask;
    t->frameTable = gUnk_0874C9D8;
    t->tileWord = 0;
    ActorPlaySfx(212, 0);
    sub_08069fc8();
}

void sub_0806b224(void)
{
    ActorDefeatFrozen();
}

void sub_0806b230(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = 0;
    t->health = 127;
    PlaySmokeRingAnim();
}

s32 ActorDrownLand(void)
{
    ActorSetState(1);
    TaskSetEntry(ActorDrownEnterState, gCurTaskIdx);
    return 1;
}

void ActorDrownInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->updateCallback = (u32)ActorDrownUpdate;
    t->health = 1;
    t->lateUpdateCallback = 0;
    CallTableEntry(t->state, 3, gActorDrownStates);
}

void ActorDrownUpdate(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gActorDrownStateUpdates);
    if (gCurTask->state != 2)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}
