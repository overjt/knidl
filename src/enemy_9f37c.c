
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "camera.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern s32 PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
extern void ActorSetState(u16 v);
extern u32 ActorCheckHits(void);
extern u32 ActorReactToHit(void);

void Task_TridentKnightTrident(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    u8 *p;
    s32 z;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    p = &t->layer;
    z = 0;
    *p = 9;
    u = gCurTask;
    u->frameTable = gTridentKnightTridentFrames;
    u->updateCallback = (u32)sub_0809f3e0;
    TaskFaceLikeParent();
    v = gCurTask;
    v->unk28 = z;
    CallTableEntry(v->variant, 5, gUnk_08747C6C);
    w = gCurTask;
    w->accelY = 148 << 6;
    w->speedLimitY = 192 << 10;
    TaskSleepForever();
}

void sub_0809f3e0(void)
{
    struct Task *t;
    struct Task *u;
    s32 vy;
    s32 n;
    u16 h;

    t = gCurTask;
    if (t->pixelY > gViewRect[2] + 196)
    {
        ActorDestroy();
        return;
    }
    vy = t->velY;
    if (vy < 0)
    {
        if (abs(t->velX) >= -vy)
            TaskSetFrame(8);
        else
            TaskSetFrame(4);
    }
    else
    {
        if (abs(t->velX) >= vy)
            TaskSetFrame(10);
        else
            TaskSetFrame(6);
    }
    u = gCurTask;
    n = u->unk28 + 1;
    u->unk28 = n;
    if ((n & 2) != 0)
    {
        h = u->frame;
        u->frame = (0xFE & h) + ((h + 1) & 1);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0809f478(void)
{
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFE0000;
}

void sub_0809f49c(void)
{
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFE0000;
}

void sub_0809f4c0(void)
{
    TaskSetMotionXFacing(208 << 9, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFE0000;
}

void sub_0809f4e4(void)
{
    TaskSetMotionXFacing(136 << 10, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFE0000;
}

void sub_0809f508(void)
{
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFF0000;
}

u8 sub_0809f52c(void)
{
    switch (gCurTask->variant)
    {
    case 1:
    case 2:
    case 3:
    zero:
        return 0;
    case 0:
        TaskStopY();
        gCurTask->updateState = 0;
        if (gCurTask->state == 0)
            goto zero;
        sub_0809f90c();
        gCurTask->unk28 = 1;
        ActorSetState(0);
        TaskSetEntry(sub_0809e8b0, gCurTaskIdx);
        return 1;
    }
}

u8 sub_0809f588(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s8 *p;
    s32 vx;

    t = gCurTask;
    switch (t->variant)
    {
    case 2:
    case 3:
        p = &t->facing;
        *p = -*p;
        u = gCurTask;
        vx = -u->velX;
        u->velX = vx;
        if (u->facing == 1 && vx > 0)
            goto zero;
        break;
    case 0:
        if (t->state == 0)
            sub_0809ec84();
        else
            TaskStopX();
        return 0;
    case 1:
        sub_0809f930();
        return 0;
    default:
        goto end;
    }
    v = gCurTask;
    if (v->facing == -1 && v->velX < 0)
    {
    zero:
        gCurTask->unk28 = 0;
    }
    else
    {
        gCurTask->unk28 = 1;
    }
    return 0;
end:
    ;
}

u8 sub_0809f618(void)
{
    return 0;
}

void sub_0809f61c(void)
{
    struct Task *t;
    struct Task *u;
    u8 *p;
    s32 z;

    switch (gCurTask->variant)
    {
    case 0:
        TaskYieldTrampoline(30);
        break;
    case 1:
        TaskYieldTrampoline(60);
        break;
    case 2:
        TaskYieldTrampoline(120);
        break;
    case 3:
        TaskYieldTrampoline(180);
        break;
    case 4:
        TaskYieldTrampoline(240);
        break;
    case 5:
        t = gCurTask;
        t->moveCallback = (u32)TaskMove;
        t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
        p = &t->layer;
        z = 0;
        *p = 11;
        u = gCurTask;
        u->frameTable = gUnk_087535A8;
        u->tileWord = z;
        PlaySfx(142 << 1);
        gCurTask->frame = z;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        break;
    }
    sub_0809f818(0);
    TaskExitTrampoline();
}

void sub_0809f7e4(void)
{
    sub_0809f818(-1);
    TaskExitTrampoline();
}

void sub_0809f7f8(void)
{
    gCurTask->unk24 = 18;
}

void sub_0809f808(void)
{
    sub_0809f818(0);
    sub_0809f874();
}

void sub_0809f818(s32 v)
{
    switch (gCurTask->unk6E)
    {
    case 0:
        gUnk_02007D00[0] = v;
        break;
    case 1:
        gUnk_02007D00[1] = v;
        break;
    case 2:
        gUnk_02007D00[2] = v;
        break;
    case 3:
        gUnk_02007D00[3] = v;
        break;
    default:
        while (1)
            ;
    }
}

void sub_0809f874(void)
{
    switch (gCurTask->unk6E)
    {
    case 0:
        gUnk_02007D00[5] |= 1;
        break;
    case 1:
        gUnk_02007D00[5] |= 2;
        break;
    case 2:
        gUnk_02007D00[5] |= 4;
        break;
    case 3:
        gUnk_02007D00[5] |= 8;
        break;
    default:
        while (1)
            ;
    }
}

void TaskFaceScreenCenter(void)
{
    TaskGetScreenPos();
    if (gUnk_030023B4 <= 119)
        gCurTask->facing = 1;
    else
        gCurTask->facing = -1;
    TaskUpdateFlip();
}

void sub_0809f90c(void)
{
    TaskFaceNearestPlayer();
    TaskUpdateFlip();
}

void sub_0809f91c(void)
{
    gCurTask->facing = -gCurTask->facing;
}

void sub_0809f930(void)
{
    struct Task *t;
    s8 f;

    f = gCurTask->facing;
    TaskFaceNearestPlayer();
    TaskUpdateFlip();
    gCurTask->facing = -f;
    t = gCurTask;
    t->velX = -t->velX;
}

void sub_0809f960(void)
{
    TaskFaceNearestPlayer();
    TaskTurnAroundAndReverseX();
}

void sub_0809f970(void)
{
    struct Task *t;

    gCurTask->facing = -gCurTask->facing;
    t = gCurTask;
    t->velX = -t->velX;
    TaskUpdateFlip();
}

s32 sub_0809f994(void)
{
    struct Task *t;
    struct Actor *a;
    s16 *p;
    s8 *q;
    s32 h;

    t = gCurTask;
    p = &t->pixelX;
    h = *p;
    a = t->u8C.actor;
    q = (s8 *)a->terrainBox;
    if (h < q[4] + 24)
    {
        if (t->velX < 0)
            return 1;
    }
    else if (h > 288 - q[5])
    {
        if (t->velX > 0)
            return 1;
    }
    return 0;
}
