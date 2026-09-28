/* game_code_and_rodata 0x0806C2A4-0x0806CD40 (issue #64, module M18 batch 4).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806C2A4 0x0806CD40 src/actor_6c2a4.c --newpb
 *
 * Class-1 task bodies for the vehicle/ride actors: the launch-and-fall pair
 * (sub_0806c2a4 / sub_0806c930 set Task.velX/unk58 from the sign in unk43
 * and hand control to ActorMove), the star-ride state machine
 * (sub_0806c4a0 / sub_0806c5d4 / sub_0806c770 - a nine-step animation
 * switch over Task.unk46, the gUnk_0873EAC0 speed table and the
 * gUnk_0873EAF0 drift table), and the short spawn-effect bodies that only
 * walk Task.frame through a gfx list (gUnk_0874C520 / gUnk_0874CBC8) before
 * TaskExitTrampoline.  CreateStarFlash and CreateDustTrail are the two helper
 * spawners that fix up Task.facing (facing) on the task they created.
 *
 * sub_0806c770 was the hardest function in M18: instruction-identical to the
 * ROM but 34 bytes of a three-way register rotation, caused by a preference
 * exclusion in the allocator rather than a wrong shape.  Its `ka`, `kb`,
 * `tbl` and `p` locals are load-bearing - see the commit message and
 * docs/lessons-learned.md.
 */

#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "actor.h"

extern void TaskSetEntry(void *fn, s32 i);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void RegisterCollider(u8 a, s16 x, s16 y, void *p);
extern void ActorSetState(u8 v);
extern void ActorSetTerrainHandlers(u32 v);

extern void ActorSetTerrainBox(u32 v);

/* Not from collision.h: this file's view of gTerrainResult differs (lesson
   3.517). */
extern u8 gTerrainResult[];
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
extern u32 IsWorldPosOnScreen(s16 x, s16 y);
extern void PlaySfx(u32 a);
extern void RequestScreenShake(u32 a);
extern void TaskBreakBlocks(void *p, s16 v);
extern void ActorCollideTerrain(void);
extern s16 RandomSpreadFacing(s32 a, u32 b, u32 c);
extern s32 RandomSpread(s32 a, u32 b, u32 c);

void sub_0806c2a4(void)
{
    struct Task *t;
    s32 v;
    s32 w;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->updateCallback = (u32)sub_0806c30c;
    t->lateUpdateCallback = 0;
    t->unk34 = 0;
    v = t->velX >> 1;
    if (v < 0)
        v = -v;
    t->velX = t->facing * v;
    w = t->velY >> 1;
    if (w < 0)
        w = -w;
    t->velY = -w;
    t->hitKind = 0;
    gCurTask->u80.attackAbility = 0;
    TaskYieldTrampoline(12);
    gCurTask->unk34 = 1;
    TaskSleepForever();
}

void sub_0806c30c(void)
{
    if (gCurTask->unk34 != 0)
        sub_0806b8bc();
}

void sub_0806c324(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->lateUpdateCallback = (u32)sub_0806c384;
    ActorSetTerrainHandlers((u32)gUnk_0873F8F4);
    sub_0806ba9c();
    u = gCurTask;
    u->unk18 = 0;
    u->layer = 6;
    while (sub_0806baec(18) == 0)
    {
        sub_0806bc28();
        TaskYieldTrampoline(1);
    }
    sub_0806bc9c();
    ActorSetState(5);
    TaskSleepForever();
}

void sub_0806c384(void)
{
    struct Task *t;

    sub_0806b878();
    sub_0806b938();
    t = gCurTask;
    if (t->unk18 == 1)
        sub_0806b8bc();
    else if (t->state != 4)
        TaskSetEntry(sub_0806bf38, gCurTaskIdx);
    sub_0806be84();
}

void sub_0806c3c4(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->updateCallback = (u32)sub_0806c418;
    t->lateUpdateCallback = (u32)sub_0806c490;
    t->posY = 0;
    t->posX = 0;
    t->unk20 = t->pixelX;
    t->unk1C = t->pixelY;
    t->taskClass = 1;
    ActorSetTerrainHandlers((u32)gUnk_0873F8F4);
    TaskSleepForever();
}

void sub_0806c418(void)
{
    struct Task *t;

    gCurTask->health = 127;
    sub_0806b878();
    sub_0806b410();
    t = gCurTask;
    if (t->actorKind == 1)
        RegisterCollider((u8)gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873F86C);
    else
        RegisterCollider((u8)gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873F858);
}

void sub_0806c490(void)
{
    sub_0806b848();
    sub_0806be84();
}

void sub_0806c4a0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    s32 i;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->updateCallback = (u32)sub_0806c5d4;
    t->lateUpdateCallback = (u32)sub_0806c770;
    t->layer = 11;
    if (gUnk_0300244C != 0 && gCurTask->u8C.actor->terrainBox == 0)
        ActorSetTerrainBox((u32)gUnk_0873F894);
    u = gCurTask;
    i = (s16)u->unk70 - 3;
    u->unk70 = i;
    u->unk24 = 0;
    TaskSetMotionXFacing(gUnk_0873EAC0[i].unk00, 0x5A5A5A5A);
    v = gCurTask;
    v->velY = gUnk_0873EAC0[i].unk04;
    v->hitKind = 0;
    gCurTask->u80.attackAbility = 0;
    w = gCurTask;
    w->unk46 = 0;
    w->unk34 = 0;
    w->unk20 = 0;
    if (w->actorKind != 1)
    {
        while (1)
        {
        gCurTask->spriteFlags |= 0x8000;
        CreateChildTaskHere(158, 0);
        gCurTask->frame = 2;
        TaskYieldTrampoline(4);
        gCurTask->spriteFlags &= 0x7FFF;
        CreateChildTaskHere(158, 0);
        TaskYieldTrampoline(4);
        gCurTask->spriteFlags &= 0x7FFF;
        CreateChildTaskHere(158, 0);
        gCurTask->frame = 3;
        TaskYieldTrampoline(4);
        gCurTask->spriteFlags |= 0x8000;
        CreateChildTaskHere(158, 0);
        TaskYieldTrampoline(4);
        }
    }
    while (1)
    {
        CreateChildTaskHere(158, 0);
        TaskYieldTrampoline(4);
    }
}

void sub_0806c5d4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->health = 127;
    if (t->actorKind == 1)
        TaskBreakBlocks(gUnk_0873F8DC, t->parent);
    else
        TaskBreakBlocks(gUnk_0873F8CC, t->parent);
    ActorCollideTerrain();
    if ((*(u32 *)gTerrainResult & 0x00FFFFFF) != 0)
    {
        if (gTerrainResult[1] != 0)
            gCurTask->unk30 = 0;
        if (gTerrainResult[2] != 0)
            gCurTask->unk30 = 1;
        if (gTerrainResult[0] != 0)
            gCurTask->unk30 = 2;
        if (gTerrainResult[4] != 0)
            gCurTask->unk30 = 2;
        PlaySfx(237);
        RequestScreenShake(2);
        gCurTask->lateUpdateCallback = 0;
        ActorSetState(7);
        TaskSetEntry(sub_0806bf38, gCurTaskIdx);
    }
    else
    {
        u = gCurTask;
        if ((u->waterFlags & 1) != 0)
        {
            if ((u->velX & 0xFFFF0000) != 0)
                TaskSetMotionXFacing(0x5A5A5A5A, 0xFFFFE800);
            gCurTask->accelY = 0x1000;
        }
        v = gCurTask;
        if (v->actorKind != 1)
        {
            v->pixelX += gUnk_0873EAF0[v->unk24 * 2];
            v->pixelY += (&gUnk_0873EAF0[1])[v->unk24 * 2];
            v->unk24 = (v->unk24 + 1) & 15;
        }
        if (v->actorKind == 1)
            RegisterCollider((u8)gCurTaskIdx, gCurTask->pixelX,
                         gCurTask->pixelY, gUnk_0873F86C);
        else
            RegisterCollider((u8)gCurTaskIdx, v->pixelX, v->pixelY, gUnk_0873F858);
    }
}

void sub_0806c770(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct Task *p;
    u32 *tbl;
    s16 dx;
    s16 dy;
    u32 k;
    u32 ka;
    u32 kb;

    switch (gCurTask->unk46)
    {
    case 0:
        t = gCurTask;
        dx = t->posX >> 16;
        dy = t->posY >> 16;
        if (t->actorKind == 1)
            k = 8;
        else
            k = 0;
        if (t->facing == 1)
            t->unk30 = dx - (ka = k + 8);
        else
            t->unk30 = dx + (kb = k + 8);
        u = gCurTask;
        u->unk1C = dy;
        u->unk2C = -1;
        break;
    case 1:
        gCurTask->unk2C = 1;
        break;
    case 2:
        gCurTask->unk2C = 2;
        break;
    case 3:
        gCurTask->unk2C = 3;
        break;
    case 4:
        gCurTask->unk2C = 4;
        break;
    case 5:
        gCurTask->unk2C = 5;
        break;
    case 6:
        gCurTask->unk2C = 6;
        break;
    case 7:
        gCurTask->unk2C = 7;
        break;
    case 8:
        v = gCurTask;
        v->unk2C = -1;
        v->unk34 = 0;
        v->unk20 = 0;
        break;
    }
    w = gCurTask;
    if (w->unk2C != -1)
    {
        if (w->unk46 != 0)
        {
            if (w->facing == 1)
                w->unk34 = w->unk34 - 2;
            else
                w->unk34 = w->unk34 + 2;
            x = gCurTask;
            switch ((s16)x->unk70)
            {
            case 0:
                x->unk20 = x->unk20 + 2;
                break;
            case 2:
                x->unk20 = x->unk20 - 2;
                break;
            }
        }
        y = gCurTask;
        if (IsWorldPosOnScreen((s16)(y->unk30 + y->unk34),
                         (s16)(y->unk1C + y->unk20)) != 0)
        {
            tbl = gUnk_0874CC84;
            z = gCurTask;
            QueueSprite(z->layer, tbl[z->unk2C], 0, 0,
                         z->unk30 + z->unk34 - gSpriteCameraX,
                         (s16)(z->unk1C + z->unk20 - gSpriteCameraY));
        }
    }
    p = gCurTask;
    p->unk46++;
    if (p->unk46 > 8)
        p->unk46 = 0;
}

void sub_0806c930(void)
{
    struct Task *t;
    s32 v;
    s32 w;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->updateCallback = (u32)sub_0806c9e8;
    t->lateUpdateCallback = 0;
    t->unk34 = 0;
    v = t->velX >> 1;
    if (v < 0)
        v = -v;
    t->velX = t->facing * v;
    switch (t->unk30)
    {
    case 0:
        w = t->velY >> 1;
        if (w < 0)
            w = -w;
        t->velY = w;
        break;
    case 1:
        w = t->velY >> 1;
        if (w < 0)
            w = -w;
        t->velY = -w;
        break;
    case 2:
        v = t->velX >> 1;
        if (v < 0)
            v = -v;
        t->velX = -t->facing * v;
        TaskSetMotionY(0xFFFE0000, 0x4000, 0x20000);
        break;
    }
    gCurTask->hitKind = 0;
    gCurTask->u80.attackAbility = 0;
    TaskYieldTrampoline(12);
    gCurTask->unk34 = 1;
    TaskSleepForever();
}

void sub_0806c9e8(void)
{
    if (gCurTask->unk34 != 0)
        sub_0806b8bc();
}

void Task_ActorSplash(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gUnk_0874C520;
    u->frame = 11;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame = 13;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

s16 CreateStarFlash(u8 kind, s32 dx, s32 dy)
{
    struct Task *t;
    s32 i;
    u16 r;

    switch (kind)
    {
    case 0:
        r = CreateChildTaskAtOffsetFacing(141, (s16)dx, (s16)dy, 0);
        break;
    case 1:
        i = CreateChildTaskHere(142, 0);
        r = i;
        if ((s16)i != -1)
        {
            t = &gTasks[(s16)i];
            t->unk18 = gCurTask->facing * dx;
            t->unk1C = dy;
        }
        break;
    }
    return r;
}

void Task_StarFlash(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gUnk_0874CBC8;
    TaskFaceLikeParent();
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void Task_StarFlashOnParent(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = 0;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gUnk_0874CBC8;
    u->updateCallback = (u32)StarFlashFollowParent;
    TaskFaceLikeParent();
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    } while (++*(s16 *)&gCurTask->unk6C <= 7);
    TaskExitTrampoline();
}

void StarFlashFollowParent(void)
{
    struct Task *t;
    struct Task *p;
    s32 i;

    if (gTaskSlotTypes[i = gCurTask->parent] != -1 && TaskHasSameSerial(i) == 1)
    {
        t = gCurTask;
        p = &gTasks[t->parent];
        t->pixelX = p->pixelX + t->unk18
                 + gUnk_0873EB30[*(s16 *)&t->unk6C] * t->facing;
        t->pixelY = p->pixelY + t->unk1C + gUnk_0873EB38[*(s16 *)&t->unk6C];
    }
    else
    {
        TaskFree(gCurTaskIdx);
    }
}

s16 CreateDustTrail(u8 flag, u16 vx, s32 c, s32 d)
{
    struct Task *p;
    s32 i;
    u16 r;

    i = CreateChildTaskAtOffsetFacing(143, (s16)c, (s16)d, 0);
    r = i;
    if ((s16)i != -1)
    {
        p = &gTasks[(s16)i];
        p->unk18 = vx;
        if (flag == 0)
        {
            if (gCurTask->facing == 1)
                p->facing = 0xFF;
            else
                p->facing = 1;
        }
        else
        {
            p->facing = gCurTask->facing;
        }
        p->unk1C = gCurTask->facing * c;
        p->unk20 = d;
    }
    return r;
}

void CreateDustPuff(void)
{
    CreateChildTaskHere(144, 0);
}
