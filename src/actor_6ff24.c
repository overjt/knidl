/* game_code_and_rodata 0x0806FF24-0x08070EC0 (issue #64, module M18 batch 8).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806FF24 0x08070EC0 pending/batch8/actor_6ff24.c --newpb
 *
 * The tail of M18: the class-1 "player intro / transformation" task family
 * that the 26-entry anchor table at 0x0873FB08 dispatches, continuing the
 * batch-7 range.  Each state is a body + a per-frame helper pair:
 *
 *   sub_0807079c / sub_080708ec   state 1
 *   sub_08070930 / sub_08070a84   state 3
 *   sub_08070ac8 / sub_08070c0c   state 4
 *   sub_08070c54 / sub_08070d48   state 5
 *   sub_08070d90 / sub_08070e7c   state 6
 *
 * The bodies are all the same shape: set Task.unk15 (the state), unk24 (the
 * sub-step the helper advances on a unk7A bit), unk0C (the draw hook) and
 * unk42/unk43, kick a sound with TaskSetFrame, then walk Task.unk58 (a 16.16
 * vertical speed) through a table of steps with TaskYieldTrampoline, and
 * finally wait for the helper to reach unk24 == 2.
 *
 * Also here: sub_0806ff7c, the OAM draw routine for the family, and
 * sub_08070498 / sub_08070648, the entry points that re-seat the running
 * task from the player record (Task.unk88) and the id at 0x020055C0.
 *
 * NOTE for the coordinator - symbols.csv corrections this range needs:
 *   FALSE entries (pool-skip branches / mid-function): 0x0806FFF8,
 *   0x08070406, 0x080706A8.
 *   MISSING entries: 0x080702D8, 0x08070454.
 * See REPORT.md for the evidence.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s8 gUnk_02006094;
extern u16 gUnk_020055C0;
extern u16 gObjPalette[];
extern u8 gUnk_03001F30;
extern struct PlayerState gPlayerStates[];
extern u16 gSpriteCameraX;
extern u16 gLocalPlayer;

extern u16 gPlayerCount;
extern u16 gSpriteCameraY;
extern s16 gUnk_0300244C;

/* ROM tables */
extern s16 gUnk_0873D420[][3];
extern u8 gUnk_0873F5D4[];
extern s16 *gUnk_0873F950[];
extern u16 gUnk_0873FAB4[];
extern u8 gUnk_0873FAE8[];
extern u32 gUnk_0873FB44[];
extern u32 gUnk_0873FB60[];
extern s16 gUnk_0873FF98[];

/* The 0x0824A9E4 record sub_08070648 uploads from (two tile-count halfwords
   plus two source pointers).  Local to this file until it gets a header. */
struct GfxSrc
{
    /*0x00*/ u16 unk00;
    /*0x02*/ u16 unk02;
    /*0x04*/ u32 unk04;
    /*0x08*/ void *unk08;
    /*0x0C*/ void *unk0C;
};
extern struct GfxSrc gUnk_0824A9E4;

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void RequestCopy(u32 mode, void *src, void *dst, u32 size);
extern void QueueSprite(u32 a, s32 b, u32 c, u32 d, s16 e, s16 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(s32 a);
extern void TaskMove(void);
extern void TaskSleepForever(void);
extern void TaskSetEntry(u32 fn, u32 a);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern s32 IsOnScreen(s16 x, s16 y);
extern void sub_0801bcac(u8 *a);
extern u32 sub_08025e88(u32 i);
extern void RequestScreenShake(s32 a);
extern void SetCameraFocus(s32 x, s32 y);
extern s32 PlayerLoadFrameTilesAndPalette(s32 a);
extern void sub_0803d7c4(void);
extern void sub_0803db74(void);
extern void sub_0803ddc0(void);
extern void sub_08040808(u32 a);
extern void sub_08068a8c(u32 a, u8 flag);
extern void sub_08068b88(s32 i, u16 b, u8 c, u8 d);
extern void sub_0806d4e4(s32 a, s32 b);
extern void sub_0806ee30(void);
extern void sub_08070ec0(void);
extern void sub_08070ffc(void);

/* Defined below */
void sub_080700e8(void);
void sub_0807029c(void);
void sub_080702d8(void);
void sub_0807042c(void);
void sub_08070454(void);
void sub_0807073c(void);
void sub_08070334(void);
void sub_08070614(u32 a);

void sub_0806ff24(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk88->unk00 == gLocalPlayer)
        SetCameraFocus(t->unk48, t->unk4A);
    switch (gCurTask->unk24)
    {
    case 1:
    case 2:
        sub_08070334();
        break;
    case 3:
        sub_080702d8();
        break;
    }
}

void sub_0806ff7c(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 gfx;
    s8 sign;
    u16 dx;
    u16 dy;
    s16 x;
    s16 y;

    t = gCurTask;
    if (t->unk38 == NULL)
        return;
    if (t->frame == -1)
        return;
    dx = t->unk48 - gSpriteCameraX;
    dy = t->unk4A - gSpriteCameraY;
    v = gTasks[t->unk44].unk18;
    t->unk18 = v;
    if (v <= -2)
        return;
    if (v == -1)
        t->unk40 |= 0xC00;
    else
        t->unk40 &= 0xF3FF;
    u = gCurTask;
    if (u->unk18 > 0)
    {
        sign = (u->unk3E & 0x8000) ? -1 : 1;
        u->unk3E &= 0x7FFF;
        gfx = DrawAffineSprite(PlayerLoadFrameTilesAndPalette(0),
                           (u16)gUnk_0873FF98[((s16 *)gCurTask)[13]] * sign,
                           gUnk_0873FF98[((s16 *)gCurTask)[13]], 0);
        if (sign < 0)
            gCurTask->unk3E |= 0x8000;
    }
    else
    {
        gfx = PlayerLoadFrameTilesAndPalette(0);
    }
    if (gPlayerCount > 1)
        sub_0803d7c4();
    sub_0803db74();
    x = dx;
    y = dy;
    if (IsOnScreen(x, y) == 0)
        return;
    QueueSprite(gCurTask->layer, gfx, gCurTask->unk3E,
                 gCurTask->unk40 | 0x800, x, y);
}

void sub_080700e8(void)
{
    struct PlayerState *p;
    struct Task *t;
    s16 *tbl;
    s32 k;
    s32 pri;

    tbl = gUnk_0873F950[(p = gCurTask->unk88)->unk0D];
    k = p->unk00;
    if (k == 1 || k == 2)
        if (gPlayerCount == 4)
            k += 3;
    TaskSetFrame(tbl[k]);
    t = gCurTask;
    t->unk28 = t->frame;
    switch (gCurTask->unk88->unk00)
    {
    case 0:
        pri = 7;
        break;
    case 1:
        pri = 7;
        break;
    case 2:
        pri = 6;
        break;
    case 3:
        pri = 6;
        break;
    }
    gCurTask->layer = pri;
}

void sub_08070174(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk0C = (u32)sub_0803ddc0;
    t->unk3E &= 0x7FFF;
    if (t->unk34 != 0)
    {
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    }
    else
    {
        switch (t->unk88->unk00)
        {
        case 0:
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            break;
        case 1:
            TaskSetMotionXFacing(0x12000, 0x5A5A5A5A);
            break;
        case 2:
            TaskSetMotionXFacing(0xE000, 0x5A5A5A5A);
            break;
        case 3:
            TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
            break;
        }
    }
    TaskSetMotionY(0xFFFC0000, 0x3000, 0x40000);
    sub_0807029c();
}

void sub_08070208(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)sub_08070454;
    t->taskClass = 4;
    gCurTask->unk08 = 0;
    sub_080700e8();
}

void sub_0807022c(void)
{
    while (gCurTask->unk24 != 3)
        TaskYieldTrampoline(1);
    sub_08070614(gCurTaskIdx);
    TaskSleepForever();
}

void sub_08070264(void)
{
    struct Task *t;
    s16 *row;

    row = gUnk_0873D420[gCurTask->unk88->unk0D];
    TaskSetFrame(row[1]);
    t = gCurTask;
    t->unk2C = 1;
    t->unk28 = 2;
}

void sub_0807029c(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk2C = -t->facing;
    t->unk28 = 1;
    t->frame = gUnk_0873FAB4[t->unk88->unk0D] + 13;
    t->unk30 = 13;
}

void sub_080702d8(void)
{
    struct Task *t;
    u8 k;
    s32 v;

    k = gCurTask->unk88->unk0D;
    if (k == 1 || k == 2 || (s8)k == 5 || (s8)k == 15 || (s8)k == 16
        || (s8)k == 17 || (s8)k == 19 || (s8)k == 22 || (s8)k == 23)
    {
        t = gCurTask;
        if (t->unk28 <= 0)
        {
            v = t->unk2C;
            t->frame += v;
            t->unk28 = 2;
            t->unk2C = -v;
        }
        gCurTask->unk28--;
    }
}

void sub_08070334(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    s32 d;
    s32 v;

    t = gCurTask;
    if (t->unk28 <= 0)
    {
        d = t->unk2C;
        t->frame += d;
        v = t->unk30 + d;
        t->unk30 = v;
        t->unk28 = 1;
        if (v > 15)
        {
            t->frame = gUnk_0873FAB4[t->unk88->unk0D];
            t->unk30 = 0;
        }
        u = gCurTask;
        if (u->unk30 < 0)
        {
            u->frame = gUnk_0873FAB4[u->unk88->unk0D] + 15;
            u->unk30 = 15;
        }
    }
    w = gCurTask;
    w->unk28--;
}

void sub_080703a8(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk7A & 1)
    {
        t->unk7A = 0;
        u = gCurTask;
        u->unk24++;
        switch (u->unk24)
        {
        case 1:
            if (u->unk34 == 1)
            {
                if (gUnk_02006094 == -1)
                    PlaySfx(153);
                gUnk_02006094 = 0;
            }
            RequestScreenShake(4);
            sub_0806d4e4(0, 0);
            sub_08070174();
            break;
        case 2:
            TaskSetMotionY(0xFFFD0000, 0x3000, 0x40000);
            sub_08070264();
            break;
        }
    }
    sub_0807042c();
}

void sub_0807042c(void)
{
    switch (gCurTask->unk24)
    {
    case 1:
        sub_08070334();
        break;
    case 2:
        sub_080702d8();
        break;
    }
}

void sub_08070454(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk48 = gTasks[t->unk44].unk48;
    t->unk4A = gTasks[t->unk44].unk4A;
}

void sub_08070498(u32 a, s32 b)
{
    struct PlayerState *p;
    struct Task *e;
    struct Task *s;
    struct Task *t;
    s16 *tbl;
    s32 k;

    p = &gPlayerStates[a];
    e = &gTasks[a];
    s = &gTasks[b];
    sub_08068a8c(a, 1);
    e->unk48 = s->unk48;
    e->unk4A = s->unk4A;
    e->posX = e->unk48 << 16;
    e->posY = e->unk4A << 16;
    e->taskClass = 4;
    e->unk44 = b;
    gUnk_020055C0 = b;
    if (gUnk_0300244C != 0)
    {
        if (gUnk_03001F30 == 0)
        {
            tbl = gUnk_0873F950[p->unk0D];
            k = p->unk00;
            if (k == 1 || k == 2)
                if (gPlayerCount == 4)
                    k += 3;
            e->frame = tbl[k];
            p->unk37 = 0;
            t = gCurTask;
            if (t->unk74 == 0)
            {
                if ((s8)gUnk_0873FAE8[sub_08025e88(gCurTaskIdx)] == 1)
                    e->unk3E &= 0x7FFF;
                else
                    e->unk3E |= 0x8000;
            }
            else
            {
                if ((s8)gUnk_0873FAE8[t->unk74] == 1)
                    e->unk3E &= 0x7FFF;
                else
                    e->unk3E |= 0x8000;
            }
        }
    }
    else
    {
        tbl = gUnk_0873F950[p->unk0D];
        k = p->unk00;
        if (k == 1 || k == 2)
            if (gPlayerCount == 4)
                k += 3;
        e->frame = tbl[k];
        p->unk37 = 0;
        e->unk3E &= 0x7FFF;
    }
    TaskSetEntry((u32)sub_0806ee30, a);
}

void sub_08070614(u32 a)
{
    struct PlayerState *p;

    sub_08068b88(a, 0, 1, 0);
    p = gCurTask->unk88;
    if (p->unk0D == 25)
        p->unk37 = 3;
    sub_08040808(a);
}

void sub_08070648(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 i;

    t = gCurTask;
    t->unk0C = (u32)sub_08070ec0;
    t->unk04 = (u32)sub_0807073c;
    t->unk08 = 0;
    t->taskClass = 4;
    TaskStop();
    u = gCurTask;
    u->unk3E &= 0x7FFF;
    RequestCopy(2, gUnk_0824A9E4.unk08,
                 &gObjPalette[u->unk40 >> 12], gUnk_0824A9E4.unk00 << 5);
    RequestCopy(3, gUnk_0824A9E4.unk0C,
                 (u16 *)(0x06010000 + ((gCurTask->unk40 & 0xFFF) << 5)),
                 gUnk_0824A9E4.unk02 << 5);
    v = gCurTask;
    v->unk44 = gUnk_020055C0;
    if (gTasks[i = v->unk44].unk74 == 0)
        gCurTask->facing = gUnk_0873FAE8[sub_08025e88(i)];
    else
        v->facing = gUnk_0873FAE8[gTasks[i].unk74];
    w = gCurTask;
    w->unk14 = 2;
    CallTableEntry(gCurTask->unk14, 7, gUnk_0873FB44);
}

void sub_0807073c(void)
{
    CallTableEntry(gCurTask->unk15, 7, gUnk_0873FB60);
}

void sub_08070758(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    CallTableEntry(t->unk14, 7, gUnk_0873FB44);
}

void sub_0807077c(void)
{
    gCurTask->unk15 = 2;
    sub_08070ffc();
    TaskSleepForever();
}

void sub_08070798(void)
{
}

void sub_0807079c(void)
{
    struct Task *t;

    gCurTask->unk15 = 1;
    t = gCurTask;
    t->unk24 = 1;
    t->unk34 = 0;
    t->unk0C = (u32)sub_0803ddc0;
    t->facing = 1;
    TaskSetFrame(0x11E4);
    TaskSetMotionXFacing(0x6000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFF8000;
    TaskSetFrame(0x11E5);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11E6);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11E7);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11E8);
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0x8000;
    TaskSetFrame(0x11E9);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11EA);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11EB);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11EC);
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x40000;
    while (gCurTask->unk24 != 2)
        TaskYieldTrampoline(1);
    TaskStop();
    sub_08070614(gCurTaskIdx);
    TaskSleepForever();
}

void sub_080708ec(void)
{
    struct Task *t;
    struct Task *u;

    sub_0801bcac(gUnk_0873F5D4);
    t = gCurTask;
    if (t->unk7A & 1)
    {
        t->unk7A = 0;
        u = gCurTask;
        u->unk24++;
        if (u->unk24 == 1)
        {
            RequestScreenShake(4);
            sub_0806d4e4(0, 0);
        }
    }
}

void sub_08070930(void)
{
    struct Task *t;

    gCurTask->unk15 = 3;
    t = gCurTask;
    t->unk24 = 1;
    t->unk34 = 0;
    t->unk0C = (u32)sub_0803ddc0;
    t->layer = 7;
    TaskSetFrame(0x11E4);
    TaskSetMotionXFacing(0xC000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFF8000;
    TaskSetFrame(0x11E5);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11E6);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11E7);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11E8);
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0x8000;
    TaskSetFrame(0x11E9);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11EA);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11EB);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11EC);
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x40000;
    while (gCurTask->unk24 != 2)
        TaskYieldTrampoline(1);
    TaskStop();
    sub_08070614(gCurTaskIdx);
    TaskSleepForever();
}

void sub_08070a84(void)
{
    struct Task *t;
    struct Task *u;

    sub_0801bcac(gUnk_0873F5D4);
    t = gCurTask;
    if (t->unk7A & 1)
    {
        t->unk7A = 0;
        u = gCurTask;
        u->unk24++;
        if (u->unk24 == 1)
        {
            RequestScreenShake(4);
            sub_0806d4e4(0, 0);
        }
    }
}

void sub_08070ac8(void)
{
    struct Task *t;

    gCurTask->unk15 = 4;
    t = gCurTask;
    t->unk24 = 0;
    t->unk34 = 0;
    t->unk0C = (u32)sub_0803ddc0;
    t->layer = 7;
    gCurTask->facing = 1;
    TaskSetFrame(0x11EC);
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(0xFFFF4000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0xFFFD0000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0xFFFEC000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0xFFFF4000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0xC000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0x14000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0x30000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0x40000;
    while (gCurTask->unk24 != 2)
        TaskYieldTrampoline(1);
    TaskStop();
    sub_08070614(gCurTaskIdx);
    TaskSleepForever();
}

void sub_08070c0c(void)
{
    struct Task *t;
    struct Task *u;

    sub_0801bcac(gUnk_0873F5D4);
    t = gCurTask;
    if (t->unk7A & 1)
    {
        t->unk7A = 0;
        u = gCurTask;
        u->unk24++;
        if (u->unk24 == 1)
        {
            RequestScreenShake(4);
            sub_0806d4e4(0, 0);
        }
    }
    sub_0807042c();
}

void sub_08070c54(void)
{
    struct Task *t;

    gCurTask->unk15 = 5;
    t = gCurTask;
    t->unk24 = 0;
    t->unk34 = 0;
    t->unk0C = (u32)sub_0803ddc0;
    t->layer = 7;
    gCurTask->facing = 1;
    TaskSetFrame(0x11EC);
    TaskSetMotionXFacing(0xFFFEF000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFD0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x30000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x40000;
    while (gCurTask->unk24 != 2)
        TaskYieldTrampoline(1);
    TaskStop();
    sub_08070614(gCurTaskIdx);
    TaskSleepForever();
}

void sub_08070d48(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    SetCameraFocus(t->unk48, t->unk4A);
    sub_0801bcac(gUnk_0873F5D4);
    u = gCurTask;
    if (u->unk7A & 1)
    {
        u->unk7A = 0;
        v = gCurTask;
        v->unk24++;
    }
}

void sub_08070d90(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->unk15 = 6;
    t = gCurTask;
    t->unk24 = 1;
    t->unk34 = 0;
    t->unk0C = (u32)sub_0803ddc0;
    t->layer = 7;
    u = gCurTask;
    u->unk3E &= 0x7FFF;
    TaskSetFrame(0x11E4);
    TaskSetMotionXFacing(0x9000, 0x5A5A5A5A);
    gCurTask->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x40000;
    while (gCurTask->unk24 != 2)
        TaskYieldTrampoline(1);
    TaskStop();
    sub_08070614(gCurTaskIdx);
    TaskSleepForever();
}

void sub_08070e7c(void)
{
    struct Task *t;
    struct Task *u;

    sub_0801bcac(gUnk_0873F5D4);
    t = gCurTask;
    if (t->unk7A & 1)
    {
        t->unk7A = 0;
        u = gCurTask;
        u->unk24++;
        if (u->unk24 == 2)
        {
            RequestScreenShake(4);
            sub_0806d4e4(0, 0);
        }
    }
}
