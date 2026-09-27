/* game_code_and_rodata 0x0806D22C-0x0806E0F0 (issue #64, module M18 batch 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806D22C 0x0806E0F0 src/actor_6d22c.c --newpb
 *
 * Class-1 task bodies for a family of scripted set-piece actors: the
 * three-stage entrance at sub_0806d22c (three TaskSetMotionXFacing sweeps with the
 * position recomputed from the parent task each time), the four short
 * animation-table players sub_0806d65c/6e4/730/77c and their dispatch
 * wrappers sub_0806d554/564/574/5a4/5b8/5cc, the CreateChildTaskHere spawner
 * helpers sub_0806d4e4/d928/da3c, the eight-way "carried" body
 * sub_0806d7ec, the gTasks[].unk73-keyed body sub_0806daec (with its
 * per-frame mover sub_0806da74), the two-sprite draw callback sub_0806dca0,
 * and the two random-walk bodies sub_0806dd90 and sub_0806df98.
 */

#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u32 gUnk_0874CB90[];
extern u32 gUnk_0874C9D8[];
extern u32 gUnk_0874CA78[];
extern u32 gUnk_0874CA1C[];
extern u32 gUnk_0874CAD8[];
extern u16 gUnk_0873E620[];
extern u16 gUnk_0873E634[];
extern u16 gUnk_0873E700[];
extern u16 gUnk_0873E640[];
extern vs16 gTaskSlotTypes[];
extern u32 gUnk_0874C500[];
extern s16 gUnk_0873EB40[];
extern s16 gUnk_0873EB60[];
extern s16 gUnk_0873EB80[];
extern u32 gUnk_0874CC60[];
extern u32 gUnk_0873EBA0[];
extern u32 gUnk_0873EC20[];
extern s32 gUnk_0873ECA0[];
extern s32 gUnk_0873ECC0[];
extern u32 gUnk_08752E48[];
extern u32 gUnk_08752D20[];
extern u16 gUnk_0873ECD0[];
extern s16 gSpriteCameraX;
extern s16 gSpriteCameraY;
extern u32 gUnk_0874CC84[];
extern u32 gUnk_0874CC48[];

extern void TaskYieldTrampoline(u32 a);
extern void TaskExitTrampoline(void);
extern void ActorMove(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void TaskFaceLikeParent(void);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetFrameByFacing(u32 a);
extern void sub_0806cd30(void);
extern void TaskStop(void);
extern u8 TaskHasSameSerial(s32 i);
extern void TaskFree(s32 i);
extern s32 CreateChildTaskHere(u32 a, u32 b);
extern void TaskMoveRelativeToParent(void);
extern void ActorDrawWorldInView(void);
extern void TaskMove(void);
extern s32 RandomRange(s32 a);
extern void TaskSleepForever(void);
extern void ActorDestroy(void);
extern void sub_0806af78(void);
extern void sub_0806aaa4(void);
extern u8 ActorIsInView(void);
extern s32 TaskIsOnScreen(void);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
extern s32 RandomSpread(s32 a, s32 b, s32 c);
extern void TaskUpdatePixelPos(void);
extern void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void sub_0806e0f0(void);

void sub_0806d49c(void);
void sub_0806d5e0(void);
void sub_0806d65c(void);
void sub_0806d6e4(void);
void sub_0806d77c(void);
void sub_0806d9d4(void);
void sub_0806dca0(void);
void sub_0806de18(void);

void sub_0806d22c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 10;
    u = gCurTask;
    u->unk38 = gUnk_0874CB90;
    u->unk04 = (u32)sub_0806d49c;
    TaskFaceLikeParent();

    v = gCurTask;
    v->posX = (gTasks[v->unk44].unk48 + -v->facing * v->unk24) << 16;
    v->posY = (gTasks[v->unk44].unk4A + v->unk20) << 16;
    TaskSetMotionXFacing(0xFFFD0000, 0);
    w = gCurTask;
    w->unk58 = 0;
    w->unk60 = -0x2000;
    TaskSetFrameByFacing(0);
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    sub_0806cd30();
    TaskSetFrameByFacing(4);
    TaskYieldTrampoline(1);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(2);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(1);
    TaskStop();

    x = gCurTask;
    x->posX = (gTasks[x->unk44].unk48 + -x->facing * x->unk24) << 16;
    x->posY = (gTasks[x->unk44].unk4A + x->unk20) << 16;
    TaskSetMotionXFacing(0xFFFDC000, 0x1000);
    w = gCurTask;
    w->unk58 = -0x4000;
    w->unk60 = -0x2000;
    TaskSetFrameByFacing(0);
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(2);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(1);
    gCurTask->frame = -1;
    TaskYieldTrampoline(1);
    TaskStop();

    y = gCurTask;
    y->posX = (gTasks[y->unk44].unk48 + -y->facing * y->unk24) << 16;
    y->posY = (gTasks[y->unk44].unk4A + y->unk20) << 16;
    TaskSetMotionXFacing(0xFFFEE000, 0x1800);
    w = gCurTask;
    w->unk58 = -0x4000;
    w->unk60 = -0x2000;
    TaskSetFrameByFacing(0);
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    TaskSetFrameByFacing(6);
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0806d49c(void)
{
    if (gTaskSlotTypes[gCurTask->unk44] == -1
     || TaskHasSameSerial(gCurTask->unk44) != 1)
        TaskFree(gCurTaskIdx);
}

void sub_0806d4e4(u32 a, s32 b)
{
    s32 i;
    struct Task *p;

    switch (a)
    {
    case 1:
        i = CreateChildTaskHere(148, 0);
        break;
    case 0:
        i = CreateChildTaskHere(149, 0);
        break;
    case 2:
        i = CreateChildTaskHere(150, 0);
        break;
    case 4:
        i = CreateChildTaskHere(151, 0);
        break;
    case 3:
        i = CreateChildTaskHere(152, 0);
        break;
    case 5:
        i = CreateChildTaskHere(153, 0);
        break;
    }
    if (i != -1 && b > 0)
    {
        p = &gTasks[i];
        p->unk24 = b;
    }
}

void sub_0806d554(void)
{
    sub_0806d6e4();
    TaskExitTrampoline();
}

void sub_0806d564(void)
{
    sub_0806d65c();
    TaskExitTrampoline();
}

void sub_0806d574(void)
{
    sub_0806d77c();
    TaskExitTrampoline();
}

void sub_0806d584(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)TaskMoveRelativeToParent;
    t->posY = 0;
    t->posX = 0;
    t->unk04 = (u32)sub_0806d5e0;
}

void sub_0806d5a4(void)
{
    sub_0806d584();
    sub_0806d6e4();
    TaskExitTrampoline();
}

void sub_0806d5b8(void)
{
    sub_0806d584();
    sub_0806d65c();
    TaskExitTrampoline();
}

void sub_0806d5cc(void)
{
    sub_0806d584();
    sub_0806d77c();
    TaskExitTrampoline();
}

void sub_0806d5e0(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk24 <= 0)
    {
        t->unk04 = 0;
        t->unk00 = (u32)ActorMove;
        t->posX = t->unk48 << 16;
        t->posY = t->unk4A << 16;
    }
    else
    {
        if (gTaskSlotTypes[t->unk44] == -1 || TaskHasSameSerial(t->unk44) != 1)
            TaskFree(gCurTaskIdx);
    }
}

void sub_0806d65c(void)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    t = gCurTask;
    t->unk0C = (u32)ActorDrawWorldInView;
    t->unk38 = gUnk_0874C9D8;
    t->layer = 10;
    u = gCurTask;
    u->unk40 = 0;
    u->frame = 0;
    TaskYieldTrampoline(2);
    for (i = 0; i < 10; i++)
    {
        gCurTask->frame = gUnk_0873E620[i];
        TaskYieldTrampoline(1);
    }
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(4);
    for (i = 0; i < 6; i++)
    {
        gCurTask->frame = gUnk_0873E634[i];
        TaskYieldTrampoline(1);
    }
}

void sub_0806d6e4(void)
{
    struct Task *t;
    s32 i;

    t = gCurTask;
    t->unk0C = (u32)ActorDrawWorldInView;
    t->unk38 = gUnk_0874CA78;
    t->layer = 10;
    gCurTask->unk40 = 0;
    for (i = 0; i < 23; i++)
    {
        gCurTask->frame = gUnk_0873E700[i];
        TaskYieldTrampoline(1);
    }
}

void sub_0806d730(void)
{
    struct Task *t;
    s32 i;

    t = gCurTask;
    t->unk0C = (u32)ActorDrawWorldInView;
    t->unk38 = gUnk_0874CA1C;
    t->layer = 4;
    gCurTask->unk40 = 0;
    for (i = 0; i < 23; i++)
    {
        gCurTask->frame = gUnk_0873E640[i];
        TaskYieldTrampoline(1);
    }
}

void sub_0806d77c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk0C = (u32)ActorDrawWorldInView;
    t->unk38 = gUnk_0874CAD8;
    t->layer = 10;
    u = gCurTask;
    u->unk40 = 0;
    u->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
}

void sub_0806d7ec(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 n;
    s32 a;
    s32 b;
    s32 k;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)ActorDrawWorldInView;
    t->layer = 8;
    gCurTask->unk38 = gUnk_0874C500;
    n = RandomRange(8);
    u = gCurTask;
    u->unk28 = n;
    u->posX = (u->unk48 + gUnk_0873EB40[n]) << 16;
    k = 4;
    u->posY = (u->unk4A + ((gUnk_0873EB40 + 8)[n] + k)) << 16;
    a = gUnk_0873EB60[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    u->unk54 = b;
    a = (gUnk_0873EB60 + 8)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    u->unk58 = b;
    u->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    v = gCurTask;
    a = gUnk_0873EB80[v->unk28];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    v->unk54 = b;
    a = (gUnk_0873EB80 + 8)[v->unk28];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    v->unk58 = b;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    w = gCurTask;
    w->unk54 = 0;
    w->unk58 = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    TaskExitTrampoline();
}

void sub_0806d928(void)
{
    s32 i;
    s32 j;
    struct Task *p;

    for (i = 0; i < 8; i++)
    {
        j = CreateChildTaskHere(155, 0);
        if (j != -1)
        {
            p = &gTasks[j];
            p->unk73 = i;
        }
    }
}

void sub_0806d95c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 i;
    s32 k;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 10;
    u = gCurTask;
    u->unk38 = gUnk_0874CC60;
    u->unk04 = (u32)sub_0806d9d4;
    u->unk24 = 2;
    u->frame = 0;
    k = u->unk73 * 4;
    for (i = 0; i < 4; i++)
    {
        v = gCurTask;
        v->unk54 = gUnk_0873EBA0[k + i];
        v->unk58 = gUnk_0873EC20[k + i];
        TaskYieldTrampoline(6);
    }
    TaskSleepForever();
}

void sub_0806d9d4(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk24 <= 0)
    {
        if (t->frame > 7)
        {
            ActorDestroy();
        }
        else
        {
            t->frame++;
            t->unk24 = 2;
        }
    }
    else
    {
        t->unk24--;
    }
}

void sub_0806da04(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = 0;
    t->unk0C = 0;
    sub_0806af78();
    TaskExitTrampoline();
}

void sub_0806da20(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = 0;
    t->unk0C = 0;
    sub_0806aaa4();
    TaskExitTrampoline();
}

void sub_0806da3c(u32 a, u32 b)
{
    s32 i;
    struct Task *p;

    i = CreateChildTaskHere(156, 0);
    if (i != -1)
    {
        p = &gTasks[i];
        p->unk73 = a;
        p->unk74 = b;
    }
}

void sub_0806da74(void)
{
    struct Task *t;
    s32 m;
    s32 j;

    t = gCurTask;
    if (t->unk73 != 0)
    {
        m = t->unk74;
        j = m * 2;
        t->unk48 += gUnk_0873ECA0[m * 2];
        t->unk4A += gUnk_0873ECA0[j + 1];
        t->posX = t->unk48 << 16;
        t->posY = t->unk4A << 16;
        if (t->unk73 == 1)
        {
            t->unk54 = gUnk_0873ECC0[m * 2];
            t->unk58 = gUnk_0873ECC0[j + 1];
        }
    }
}

void sub_0806daec(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInView;
    t->unk38 = gUnk_08752E48;
    t->layer = 10;
    gCurTask->unk40 = 0;
    sub_0806da74();
    u = gCurTask;
    switch (u->unk73)
    {
    case 0:
        u->frame = 8;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        break;
    case 1:
        u->unk6E = 0;
        do
        {
            gCurTask->frame = 0;
            TaskYieldTrampoline(1);
            gCurTask->unk6C = 0;
            do
            {
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while (++*(s16 *)&gCurTask->unk6C <= 6);
        } while (++*(s16 *)&gCurTask->unk6E <= 8);
        break;
    case 2:
        u->frame = 33;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame = 15;
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while (++*(s16 *)&gCurTask->unk6C <= 5);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        break;
    case 3:
        u->frame = 24;
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while (++*(s16 *)&gCurTask->unk6C <= 5);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        break;
    }
    TaskExitTrampoline();
}

void sub_0806dca0(void)
{
    struct Task *p;
    struct Task *t;
    struct Task *u;
    u32 *tbl;

    p = gCurTask;
    if (p->unk38 == NULL)
        return;
    if (p->frame == -1)
        return;
    if (ActorIsInView() == 0)
        return;
    if (TaskIsOnScreen() == 0)
        return;
    t = gCurTask;
    tbl = t->unk38;
    QueueSprite(t->layer, tbl[t->frame], t->unk3E, t->unk40,
                 t->unk48 - gSpriteCameraX,
                 (s16)(t->unk4A - gSpriteCameraY));
    u = gCurTask;
    QueueSprite(u->layer - 1, tbl[7], u->unk3E, u->unk40,
                 u->unk48 + u->unk24 - gSpriteCameraX,
                 (s16)(u->unk4A + u->unk20 - gSpriteCameraY));
}

void sub_0806dd90(void)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)sub_0806dca0;
    t->unk38 = gUnk_08752D20;
    t->layer = 10;
    u = gCurTask;
    u->unk04 = (u32)sub_0806de18;
    u->unk40 = 0;
    u->unk24 = 0;
    u->unk20 = 0;
    while (1)
    {
        for (i = 0; i < 8; i++)
        {
            gCurTask->frame = gUnk_0873ECD0[i];
            TaskYieldTrampoline(3);
            gCurTask->unk24 = (u16)RandomSpread(-12, 1, 24);
            gCurTask->unk20 = (u16)RandomSpread(-12, 1, 24);
        }
    }
}

void sub_0806de18(void)
{
    if (gTaskSlotTypes[gCurTask->unk44] == -1
     || TaskHasSameSerial(gCurTask->unk44) != 1)
        TaskFree(gCurTaskIdx);
}

void sub_0806de60(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)TaskUpdatePixelPos;
    t->unk0C = (u32)ActorDrawWorldInView;
    t->unk38 = gUnk_0874CC84;
    t->layer = 10;
    u = gCurTask;
    u->unk40 = 0;
    u->posX = gTasks[u->unk44].posX;
    u->posY = gTasks[u->unk44].posY;
    u->frame = 0;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void ActorUpdateAttachedEffect(void)
{
    struct Actor *a;

    a = gCurTask->unk8C;
    if (a->unk10 == -1)
        return;
    if (*(s16 *)&a->unk12 == -2)
        return;
    if (*(s16 *)&a->unk12 <= 0)
    {
        TaskFree(a->unk10);
        a->unk10 = 0xFFFF;
        a->unk12 = 0xFFFE;
    }
    a->unk12--;
}

void ActorAttachEffect(s32 a, s32 b)
{
    struct Actor *p;
    s32 c;

    p = gCurTask->unk8C;
    if (p->unk10 != -1)
    {
        TaskFree(p->unk10);
        p->unk10 = 0xFFFF;
    }
    switch (a)
    {
    case 1:
        c = CreateChildTaskHere(160, 0);
        break;
    case 2:
        c = CreateChildTaskHere(161, 0);
        break;
    case 3:
        c = CreateChildTaskHere(159, 0);
        break;
    case 0:
    default:
        c = -1;
        break;
    }
    p->unk10 = c;
    if (b == 1)
        p->unk12 = 60;
    else
        p->unk12 = 0xFFFE;
}

void sub_0806df98(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)TaskMoveRelativeToParent;
    t->unk0C = (u32)ActorDrawWorldInView;
    t->unk38 = gUnk_0874CC48;
    t->layer = 6;
    u = gCurTask;
    u->unk04 = (u32)sub_0806e0f0;
    u->unk40 = 0;
    u->unk6E = 0;
    do
    {
        gCurTask->posX = RandomSpread(-12, 1, 24) << 16;
        gCurTask->posY = RandomSpread(-12, 1, 24) << 16;
        TaskSetMotion(0x4000, 0xFFFFF900, 0x5A5A5A5A, 0xFFFFC000, 0xFFFFF000,
                     0x5A5A5A5A);
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        } while (++*(s16 *)&gCurTask->unk6C <= 4);
        gCurTask->posX = RandomSpread(-12, 1, 24) << 16;
        gCurTask->posY = RandomSpread(-12, 1, 24) << 16;
        TaskSetMotion(0xFFFFC000, 0x700, 0x5A5A5A5A, 0xFFFFC000, 0xFFFFF000,
                     0x5A5A5A5A);
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        } while (++*(s16 *)&gCurTask->unk6C <= 4);
    } while (++*(s16 *)&gCurTask->unk6E <= 1);
    gCurTask->frame = 0xFFFF;
    TaskSleepForever();
}
