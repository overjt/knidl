/* game_code_and_rodata 0x0806AD18-0x0806B2E4 (issue #64, module M18 batch 2b).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806AD18 0x0806B2E4 src/actor_6ad18.c --newpb
 *
 * The screen-transition half of the module. sub_0806ad18 nudges the carried
 * actor one frame either way; sub_0806adb0 and sub_0806af78 are the two
 * DISPCNT-shadow fades (gDispCnt masked to 0xE0FF and re-ORed with the
 * BG-enable pattern, alternating a ROM window descriptor with a copy of
 * gBgPalette on the stack); sub_0806aec0 plays the six-step
 * gUnk_0873E6A0/gUnk_0873E6D0 blend-and-offset table. sub_0806b070
 * dispatches through gUnk_0873E734[Task.unk76], sub_0806b098 is the
 * transition body proper, and sub_0806b12c is the entry point that saves and
 * restores Task.unk40 around it. The tail (0x0806B12C-0x0806B2E4) is the
 * small task-field setters and their gUnk_0873E7xx descriptor tables.
 */

#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern void TaskYieldTrampoline(u32 a);

extern vu16 gDispCnt; /* DISPCNT shadow */
extern u16 gBgPalette;
extern u16 gUnk_0873E69A;
extern u16 gUnk_0873E69C;

extern void RequestScreenShake(u32);
extern void SetRoomUpdateFlags(u32);
extern void sub_080670ac(u32);
extern void sub_080670d4(void);
extern void LoadBackdropColor(u16 *);
extern void PlaySfx(u32);
extern void TaskSetSkipMask(u32, u32);
extern void TaskSetMotionXFacing(u32, u32);
extern void TaskStop(void);
extern void TaskSetFrame(u32);
extern void CreateChildTaskHere(u32, u32);
extern void ActorPlaySfx(u32, u32);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void sub_0806d4e4(u32, u32);


extern u32 gUnk_0873E6A0[];
extern u32 gUnk_0873E6D0[];
extern u32 gUnk_0874CA78[];
extern u16 gUnk_0873E72E;
extern u16 gUnk_0873E730;
extern u32 gUnk_0873E734[];
extern u16 gGameState;

extern void StopBgm(void);
extern void sub_080668c8(void);
extern void sub_08067108(void);
extern void sub_0806d928(void);
extern void HudRemoveHpBar(void);
extern void CallTableEntry(u32, u32, void *);
extern void TaskSetEntry(void (*)(void), u32);
extern void ActorSetState(u32);
extern void sub_08066f78(void);
extern void ActorCheckHits(void);
extern void ActorReactToHit(void);
extern void sub_08069fc8(void);
extern void sub_0806a7a0(void);
void sub_0806b2ac(void);
extern u8 ActorCollideTerrain(void);
extern void ActorMove(void);
extern void sub_0806b2e4(void);
extern void sub_0806ed9c(void);

extern u32 gUnk_02007D00[];
extern u8 gUnk_0873E758[];
extern u8 gUnk_0873E77C[];
extern u8 gUnk_0873E78C[];
extern u8 gUnk_0873E798[];
extern u32 gUnk_0874C9D8[];

void sub_0806ad18(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk8C->extraFrame = 0;
    t->posY = (t->unk4A + 1) << 16;
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do
    {
        t = gCurTask;
        t->posY = (t->unk4A - 2) << 16;
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->posY = (t->unk4A + 2) << 16;
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->unk6C++;
    } while ((s16)t->unk6C <= 11);
    t = gCurTask;
    t->posY = (t->unk4A - 1) << 16;
    TaskYieldTrampoline(2);
    gCurTask->unk8C->extraFrame = 0xFFFF;
}

void sub_0806adb0(void)
{
    struct Task *t;
    u16 v;

    sub_080670ac(15);
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
    sub_080670d4();
}

void sub_0806ae94(void)
{
    PlaySfx(0x200);
    TaskSetSkipMask(14, gCurTaskIdx);
    sub_0806adb0();
    TaskSetSkipMask(0, gCurTaskIdx);
}

void sub_0806aec0(void)
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
        t->unk58 = gUnk_0873E6D0[(s16)t->unk6C];
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->unk6C++;
    } while ((s16)t->unk6C <= 5);
    TaskStop();
    t = gCurTask;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->unk38 = gUnk_0874CA78;
    t->unk40 = 0;
    RequestScreenShake(4);
    ActorPlaySfx(0x1F9, 0);
    sub_0806d4e4(1, 0);
    TaskYieldTrampoline(20);
}

void sub_0806af78(void)
{
    struct Task *t;
    u16 v;

    sub_080670ac(15);
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
    sub_080670d4();
}

void sub_0806b05c(void)
{
    PlaySfx(0x1FF);
    sub_0806af78();
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
    sub_080670ac(15);
    sub_080668c8();
    SetRoomUpdateFlags(2);
    ActorPlaySfx(0x1FD, 0);
    RequestScreenShake(4);
    sub_0806d928();
    sub_0806d4e4(1, 0);
    gCurTask->frame = 0xFFFF;
    sub_0806b070();
    TaskYieldTrampoline(24);
    sub_080670d4();
    sub_08067108();
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

void sub_0806b12c(void)
{
    gUnk_02007D00[9] = gCurTask->unk40;
    HudRemoveHpBar();
    sub_0806b0f0();
    sub_0806b05c();
    sub_0806b098();
    gCurTask->unk40 = gUnk_02007D00[9];
    sub_08066f78();
    CallTableEntry(gCurTask->unk76, 9, gUnk_0873E758);
}

void sub_0806b178(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk82 > 3)
        t->unk82 = 0;
    CallTableEntry(gCurTask->unk82, 4, gUnk_0873E77C);
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
    t->unk38 = gUnk_0874C9D8;
    t->unk40 = 0;
    ActorPlaySfx(212, 0);
    sub_08069fc8();
}

void sub_0806b1f4(void)
{
    struct Task *t;

    TaskStop();
    TaskSetFrame(0);
    t = gCurTask;
    t->unk38 = gUnk_0874C9D8;
    t->unk40 = 0;
    ActorPlaySfx(212, 0);
    sub_08069fc8();
}

void sub_0806b224(void)
{
    sub_0806a7a0();
}

void sub_0806b230(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk04 = 0;
    t->unk78 = 127;
    sub_0806ed9c();
}

s32 sub_0806b24c(void)
{
    ActorSetState(1);
    TaskSetEntry(sub_0806b2e4, gCurTaskIdx);
    return 1;
}

void sub_0806b26c(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->unk04 = (u32)sub_0806b2ac;
    t->unk78 = 1;
    t->unk08 = 0;
    CallTableEntry(t->unk14, 3, gUnk_0873E78C);
}

void sub_0806b2ac(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 3, gUnk_0873E798);
    if (gCurTask->unk14 != 2)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}
