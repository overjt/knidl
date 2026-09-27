#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u8 gObjPalette[];
extern u16 gFrameCount;
extern u16 gLocalPlayer;
extern u16 gPlayerCount;
extern struct Task *gCurTask;
extern u32 gUnk_0874CE90[];
extern u8 gUnk_08757368[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s16 f);
void BlendColors(s32 a, s32 b, s32 c, s32 d, void *e);
s32 PlaySfx(s32 id);
void TaskMoveRelativeToParent(void);
void TaskDrawScreen(void);
void TaskStop(void);
s32 IsOnScreen(s16 x, s16 y);
s32 PlayerLoadFrameTilesAndPalette(s32 mode);

void sub_0801a07c(void)
{
    struct Task *t = gCurTask;

    t->unk00 = (u32)TaskMoveRelativeToParent;
    t->unk0C = (u32)TaskDrawScreen;
    t->unk42 = 12;
    gCurTask->unk38 = gUnk_0874CE90;
    gCurTask->unk40 = 0;
    gCurTask->unk4C = 0;
    gCurTask->unk50 = 0;
    TaskStop();
    PlaySfx(0x242);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C = 0;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 16;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 1;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 17;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 2;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 18;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 3;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 19;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 4;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 20;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 5;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 21;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 6;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 11;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 7;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 12;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 8;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 13;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 9;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 14;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 10;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 15;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 5);
    TaskExitTrampoline();
}

void sub_0801a1ec(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *z;
    struct Task *q;
    s32 mode;
    s32 anim;
    s32 n;
    u16 dx;
    u16 dy;
    s16 x;
    s16 y;

    t = gCurTask;
    if (t->unk38 == NULL)
        return;
    if (t->unk3C == -1)
        return;
    mode = 0;
    if (t->unk3C > 49 && (gFrameCount & 3) == 0)
        mode = 73;
    u = gCurTask;
    dx = u->unk48;
    dy = u->unk4A;
    anim = PlayerLoadFrameTilesAndPalette(mode);
    v = gCurTask;
    if (v->unk3C > 49)
    {
        n = v->unk34 + 1;
        v->unk34 = n;
        if (n < 0)
            v->unk34 = 0;
        w = gCurTask;
        if (w->unk34 > 10)
            w->unk34 = 0;
        BlendColors(0x0859A0B0, 0x0859A0D0, gUnk_08757368[gCurTask->unk34], 16,
                     (void *)((u32)gObjPalette
                              + (((gCurTask->unk40 >> 12) + 1) << 5)));
    }
    if (gPlayerCount > 1)
        RequestCopy(2, (gLocalPlayer << 5) + 0x080DC628,
                     (u32)gObjPalette + ((gCurTask->unk40 >> 12) << 5), 22);
    x = dx;
    y = dy;
    if (IsOnScreen(x, y) == 0)
        return;
    q = gCurTask;
    QueueSprite(q->unk42, anim, q->unk3E, 0x800 | q->unk40, x, y);
}

void sub_0801a310(void)
{
    struct Task *t;
    struct Task *u;
    struct TaskGfx *g;
    u16 *p;
    u16 *q;
    u32 dst;
    u32 prio;

    t = gCurTask;
    if (t->unk38 == NULL)
        return;
    if (t->unk3C == -1)
        return;
    prio = t->unk40;
    dst = ((prio & 0x7FF) << 5) + 0x0600FE00;
    g = (struct TaskGfx *)t->unk38[t->unk3C];
    p = g->unk08;
    if (*p != 0xFFFF)
    {
        do
        {
            q = p + 1;
            RequestCopy(4, (u32)q, dst, *p);
            p = (u16 *)((u8 *)q + *p);
            dst += 0x400;
        } while (*p != 0xFFFF);
    }
    RequestCopy(2, (u32)(g->unk04 + 1), (u32)gObjPalette + ((prio >> 12) << 5),
                 *g->unk04);
    u = gCurTask;
    QueueSprite(u->unk42, g->unk00, u->unk3E, 0x800 | u->unk40, u->unk48, u->unk4A);
}
