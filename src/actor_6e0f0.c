/* game_code_and_rodata 0x0806E0F0-0x0806EF5C (issue #64, module M18 batch 6).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806E0F0 0x0806EF5C src/actor_6e0f0.c --newpb
 *
 * Class-1 task bodies for a family of scenery/effect actors, each laid out as
 * the same three-function group:
 *
 *   <spawn/body>   installs Task.moveCallback/unk0C (the per-frame update + draw
 *                  hooks), the sprite table in Task.frameTable, Task.layer (draw
 *                  priority) and Task.updateCallback (the "still alive?" callback),
 *                  then runs an animation script of
 *                  `Task.frame = frame; TaskYieldTrampoline(delay);`.
 *   <alive check>  the 0x48-byte helper repeated eight times in this range:
 *                  `if (gTaskSlotTypes[t->unk44] == -1 || TaskHasSameSerial(...) != 1)
 *                       TaskFree(gCurTaskIdx);`  - i.e. kill this task when
 *                  the task it is attached to (Task.parent) is gone.
 *   <spawner>      sub_0806e6f8 / sub_0806e808 / sub_0806e9b4 allocate a task
 *                  of type 167/168/169 and seed its unk24/unk20 position.
 *
 * The tail (Task_IceBlock-sub_0806ef38) is the "cursor"/menu-ish task group:
 * sub_0806ed28 walks a 6-entry s16[6][2] table at 0x0873E5F8, and
 * sub_0806ee30 is a class-1 entry point that re-arms the running task from
 * the player record (Task.player) and the id at 0x020055C0.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "camera.h"
#include "player.h"
#include "effect.h"

/* RAM cells */
/* Not from actor.h: this file's view of gUnk_0873E5F8 differs (lesson 3.517). */
extern u16 gUnk_020055C0;

/* ROM tables */
extern s16 gUnk_0873E5F8[][2];
extern u32 gUnk_0873ECE0[];
extern u8 gUnk_0873FAE8[];
extern u32 gUnk_0873FB04[];
extern u32 gUnk_0873FB24[];
extern u32 gUnk_0874CB3C[];
extern u32 gUnk_0874CB7C[];
extern u32 gUnk_0874CBD0[];
extern u32 gUnk_0874CC38[];
extern u32 gUnk_0874CCA4[];
extern u32 gWarpStarFrames[];

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskSetPosXFacing(s32 a);
extern u16 RandomSpread(s32 base, u8 scale, u8 amount);
extern s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);
extern void TaskFaceLikeParent(void);
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);
extern s32 CreateChildTaskAt(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern void ActorDrawWorldInView(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern u8 TaskHasSameSerial(s32 i);
extern void sub_0806ff7c(void);
extern void sub_08070648(void);

/* Defined below */
void HitFlamesCheckParent(void);
void HitSparksCheckParent(void);
void WarpStarSparkleCheckParent(void);
void AbilityReleaseFlashCheckParent(void);
void sub_0806e7c0(void);
void sub_0806e96c(void);
s32 sub_0806e9b4(u8 a, s16 x, s16 y);
void sub_0806ec88(void);
void sub_0806ed28(void);
void sub_0806ed9c(void);
void sub_0806ef1c(void);

void HitFrostCheckParent(void)
{
    s32 i;

    if (gTaskSlotTypes[i = gCurTask->parent] == -1 || TaskHasSameSerial(i) != 1)
        TaskFree(gCurTaskIdx);
}

void Task_HitFlames(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->frameTable = gUnk_0874CBD0;
    t->layer = 10;
    gCurTask->updateCallback = (u32)HitFlamesCheckParent;
    gCurTask->tileWord = 0;
    while (1)
    {
        gCurTask->posX = RandomSpreadFacing(-12, 1, 24) << 16;
        gCurTask->posY = RandomSpread(-12, 1, 24) << 16;
        gCurTask->velY = 0xFFFF8000;
        TaskSetFrame(18);
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        TaskSetFrame(14);
        TaskYieldTrampoline(2);
        gCurTask->velY = 0xFFFF0000;
        gCurTask->frame += 2;
        TaskYieldTrampoline(6);
        gCurTask->frame += 2;
        TaskYieldTrampoline(6);
        gCurTask->velY = 0xFFFE0000;
        TaskSetFrame(24);
        TaskYieldTrampoline(2);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(4);
    }
}

void HitFlamesCheckParent(void)
{
    s32 i;

    if (gTaskSlotTypes[i = gCurTask->parent] == -1 || TaskHasSameSerial(i) != 1)
        TaskFree(gCurTaskIdx);
}

void Task_HitSparks(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->frameTable = gUnk_0874CC38;
    t->layer = 10;
    gCurTask->updateCallback = (u32)HitSparksCheckParent;
    gCurTask->tileWord = 0;
    while (1)
    {
        gCurTask->posX = RandomSpreadFacing(-12, 1, 24) << 16;
        gCurTask->posY = RandomSpread(-12, 1, 24) << 16;
        gCurTask->velX = 0x10000;
        gCurTask->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->velX = 0xFFFF0000;
        gCurTask->velY = 0xFFFF0000;
        TaskYieldTrampoline(1);
        gCurTask->velX = 0;
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(1);
        TaskStop();
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(10);
        gCurTask->posX = RandomSpreadFacing(-12, 1, 24) << 16;
        gCurTask->posY = RandomSpread(-12, 1, 24) << 16;
        gCurTask->velX = 0x10000;
        gCurTask->frame = 2;
        TaskYieldTrampoline(1);
        gCurTask->velX = 0xFFFF0000;
        gCurTask->velY = 0xFFFF0000;
        TaskYieldTrampoline(1);
        gCurTask->velX = 0;
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(1);
        TaskStop();
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(28);
        gCurTask->posX = RandomSpreadFacing(-12, 1, 24) << 16;
        gCurTask->posY = RandomSpread(-12, 1, 24) << 16;
        gCurTask->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(4);
        gCurTask->posX = RandomSpreadFacing(-12, 1, 24) << 16;
        gCurTask->posY = RandomSpread(-12, 1, 24) << 16;
        gCurTask->frame = 3;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(2);
    }
}

void HitSparksCheckParent(void)
{
    s32 i;

    if (gTaskSlotTypes[i = gCurTask->parent] == -1 || TaskHasSameSerial(i) != 1)
        TaskFree(gCurTaskIdx);
}

void Task_WarpStarSparkle(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    if (gUnk_030023B8 != 7)
        t->drawCallback = (u32)ActorDrawWorldInView;
    else
        t->drawCallback = (u32)TaskDrawScreen;
    u = gCurTask;
    u->frameTable = gWarpStarFrames;
    u->layer = 10;
    gCurTask->updateCallback = (u32)WarpStarSparkleCheckParent;
    gCurTask->tileWord = 0;

    while (1)
    {
        gCurTask->posX = RandomSpread(-16, 1, 32) << 16;
        gCurTask->posY = RandomSpread(-16, 1, 32) << 16;
        gCurTask->frame = 5;
        TaskYieldTrampoline(1);
        gCurTask->frame += 1;
        TaskYieldTrampoline(1);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 6;
        TaskYieldTrampoline(1);
        gCurTask->frame -= 1;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(30);
        gCurTask->posX = RandomSpread(-16, 1, 32) << 16;
        gCurTask->posY = RandomSpread(-16, 1, 32) << 16;
        gCurTask->frame = 5;
        TaskYieldTrampoline(1);
        gCurTask->frame += 1;
        TaskYieldTrampoline(1);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 6;
        TaskYieldTrampoline(1);
        gCurTask->frame -= 1;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(4);
        gCurTask->posX = RandomSpread(-16, 1, 32) << 16;
        gCurTask->posY = RandomSpread(-16, 1, 32) << 16;
        gCurTask->frame = 5;
        TaskYieldTrampoline(1);
        gCurTask->frame += 1;
        TaskYieldTrampoline(1);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 6;
        TaskYieldTrampoline(1);
        gCurTask->frame -= 1;
        TaskYieldTrampoline(1);
    }
}

void WarpStarSparkleCheckParent(void)
{
    s32 i;

    if (gTaskSlotTypes[i = gCurTask->parent] == -1 || TaskHasSameSerial(i) != 1)
        TaskFree(gCurTaskIdx);
}

void Task_AbilityReleaseFlash(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->frameTable = gUnk_0874CB3C;
    t->layer = 4;
    u = gCurTask;
    u->updateCallback = (u32)AbilityReleaseFlashCheckParent;
    u->tileWord = 0;
    u->posX = 0;
    u->posY = 0;
    u->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame += 1;
        TaskYieldTrampoline(1);
    } while ((s16)(++gCurTask->unk6C) <= 6);
    TaskExitTrampoline();
}

void AbilityReleaseFlashCheckParent(void)
{
    s32 i;

    if (gTaskSlotTypes[i = gCurTask->parent] == -1 || TaskHasSameSerial(i) != 1)
        TaskFree(gCurTaskIdx);
}

s32 sub_0806e6f8(s16 x, s16 y)
{
    struct Task *t;
    s32 i;

    i = CreateChildTaskAt(167, 0, 0, 0);
    if (i != -1)
    {
        t = &gTasks[i];
        t->unk24 = x;
        t->unk20 = y;
    }
    return i;
}

void sub_0806e73c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gUnk_0874CCA4;
    u->updateCallback = (u32)sub_0806e7c0;
    u->tileWord = 0;
    TaskFaceLikeParent();
    while (1)
    {
        TaskSetPosXFacing(gCurTask->unk24);
        gCurTask->posY = gCurTask->unk20 << 16;
        TaskSetMotionXFacing(0xFFFD0000, 0x5A5A5A5A);
        TaskSetFrame(0);
        TaskYieldTrampoline(2);
        TaskSetFrame(2);
        TaskYieldTrampoline(3);
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
    }
}

void sub_0806e7c0(void)
{
    s32 i;

    if (gTaskSlotTypes[i = gCurTask->parent] == -1 || TaskHasSameSerial(i) != 1)
        TaskFree(gCurTaskIdx);
}

s32 sub_0806e808(s16 x, s16 y)
{
    struct Task *t;
    s32 i;

    i = CreateChildTaskAt(168, 0, 0, 0);
    if (i != -1)
    {
        t = &gTasks[i];
        t->unk24 = x;
        t->unk20 = y;
    }
    return i;
}

void sub_0806e84c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gUnk_0874C718;
    u->updateCallback = (u32)sub_0806e96c;
    u->tileWord = 0;
    TaskFaceLikeParent();
    gCurTask->facing = -gCurTask->facing;
    while (1)
    {
        gCurTask->posX = (RandomSpreadFacing(-8, 1, 16)
            + gCurTask->unk24 * gCurTask->facing) << 16;
        gCurTask->posY = (RandomSpread(-8, 1, 16)
            + gCurTask->unk20) << 16;
        TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
        TaskSetFrame(0);
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
    }
}

void sub_0806e96c(void)
{
    s32 i;

    if (gTaskSlotTypes[i = gCurTask->parent] == -1 || TaskHasSameSerial(i) != 1)
        TaskFree(gCurTaskIdx);
}

s32 sub_0806e9b4(u8 a, s16 x, s16 y)
{
    struct Task *t;
    s32 i;

    i = CreateChildTaskHere(169, 0);
    if (i != -1)
    {
        t = &gTasks[i];
        t->variant = a;
        t->unk24 = x;
        t->unk20 = y;
    }
    return i;
}

void sub_0806e9fc(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gUnk_0874C828;
    u->pixelX += u->unk24;
    u->pixelY += u->unk20;
    u->posX = u->pixelX << 16;
    u->posY = u->pixelY << 16;
    CallTableEntry(u->variant, 3, gUnk_0873ECE0);
    TaskSleepForever();
}

void sub_0806ea70(void)
{
    gCurTask->frame = 16;
    TaskYieldTrampoline(1);
    sub_0806e9b4(1, 0, 0);
    sub_0806e9b4(2, 0, 0);
    gCurTask->frame += 1;
    TaskYieldTrampoline(2);
    gCurTask->frame += 1;
    TaskYieldTrampoline(2);
    gCurTask->frame += 1;
    TaskYieldTrampoline(2);
    gCurTask->frame += 1;
    TaskYieldTrampoline(2);
    gCurTask->frame += 1;
    TaskYieldTrampoline(2);
    gCurTask->frame += 1;
    TaskYieldTrampoline(2);
    gCurTask->frame += 1;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void sub_0806eb04(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->frame = 0;
    t->velX = 0xFFFA0000;
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    u = gCurTask;
    u->velX = 0xFFFE0000;
    u->accelX = 0x1000;
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0806eba4(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->frame = 1;
    t->velX = 0x60000;
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    u = gCurTask;
    u->velX = 0x20000;
    u->accelX = 0xFFFFF000;
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void Task_IceBlock(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 7;
    u = gCurTask;
    u->frameTable = gUnk_0874CB7C;
    u->updateCallback = (u32)sub_0806ec88;
    u->tileWord = 0;
    u->posX = 0;
    u->posY = 0;
    TaskSleepForever();
}

void sub_0806ec88(void)
{
    s32 i;
    s32 j;

    if (gTaskSlotTypes[i = gCurTask->parent] != -1 && TaskHasSameSerial(i) == 1)
    {
        if ((s8)gTasks[j = gCurTask->parent].hitKind == 4
            && (u16)(gTasks[j].unk82 - 2) <= 1)
        {
            TaskFree(gCurTaskIdx);
        }
        else
        {
            gCurTask->onGround = 0;
            sub_0806ed28();
        }
    }
    else
    {
        TaskFree(gCurTaskIdx);
    }
}

void sub_0806ed28(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    s32 v;
    s32 i;

    t = gCurTask;
    v = t->unk70;
    i = (s16)v >> 1;
    if (i <= 5)
    {
        if (t->unk20 <= 0)
        {
            t->unk70 = v + 1;
            if (t->unk1C != 0)
                t->frame = 0xFFFF;
            else
                t->frame = 4;
            u = gCurTask;
            u->unk20 = gUnk_0873E5F8[i][u->unk1C];
            u->unk1C ^= 1;
        }
        w = gCurTask;
        w->unk20 -= 1;
    }
    else
    {
        TaskFree(gCurTaskIdx);
    }
}

void sub_0806ed9c(void)
{
    struct Task *t;

    t = gCurTask;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->frameTable = gUnk_08752548;
    t->layer = 10;
    gCurTask->tileWord = 0;
    TaskSetFrame(0);
    TaskYieldTrampoline(1);
    gCurTask->frame += 1;
    TaskYieldTrampoline(1);
    gCurTask->frame += 1;
    TaskYieldTrampoline(1);
    gCurTask->frame += 1;
    TaskYieldTrampoline(1);
    gCurTask->frame += 1;
    TaskYieldTrampoline(1);
    gCurTask->frame += 1;
    TaskYieldTrampoline(1);
}

void sub_0806ee1c(void)
{
    sub_0806ed9c();
    TaskExitTrampoline();
}

void sub_0806ee2c(void)
{
}

void sub_0806ee30(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct PlayerState *p;
    struct PlayerState *q;

    p = gCurTask->player;
    p->prevMode = p->mode;
    q = gCurTask->player;
    q->mode = 20;
    if (gUnk_03001F30 == 1)
        sub_08070648();
    t = gCurTask;
    t->drawCallback = (u32)sub_0806ff7c;
    t->updateCallback = (u32)sub_0806ef1c;
    t->lateUpdateCallback = 0;
    t->taskClass = 4;
    gCurTask->frameTable = gUnk_0874CFEC;
    TaskStop();
    u = gCurTask;
    u->spriteFlags &= 0x7FFF;
    u->player->unk42 &= 0xFFEF;
    u->parent = gUnk_020055C0;
    if (gTasks[u->parent].unk74 == 0)
        gCurTask->facing = gUnk_0873FAE8[sub_08025e88(u->parent)];
    else
        u->facing = gUnk_0873FAE8[gTasks[u->parent].unk74];
    v = gCurTask;
    v->state = 2;
    CallTableEntry(gCurTask->state, 8, gUnk_0873FB04);
}

void sub_0806ef1c(void)
{
    CallTableEntry(gCurTask->updateState, 8, gUnk_0873FB24);
}

void sub_0806ef38(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    CallTableEntry(t->state, 8, gUnk_0873FB04);
}
