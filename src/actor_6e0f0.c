/* game_code_and_rodata 0x0806E0F0-0x0806EF5C (issue #64, module M18 batch 6).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806E0F0 0x0806EF5C pending/batch6/actor_6e0f0.c --newpb
 *
 * Class-1 task bodies for a family of scenery/effect actors, each laid out as
 * the same three-function group:
 *
 *   <spawn/body>   installs Task.unk00/unk0C (the per-frame update + draw
 *                  hooks), the sprite table in Task.unk38, Task.unk42 (draw
 *                  priority) and Task.unk04 (the "still alive?" callback),
 *                  then runs an animation script of
 *                  `Task.unk3C = frame; TaskYieldTrampoline(delay);`.
 *   <alive check>  the 0x48-byte helper repeated eight times in this range:
 *                  `if (gUnk_03004CA0[t->unk44] == -1 || sub_08065f2c(...) != 1)
 *                       sub_08005654(gCurTaskIdx);`  - i.e. kill this task when
 *                  the task it is attached to (Task.unk44) is gone.
 *   <spawner>      sub_0806e6f8 / sub_0806e808 / sub_0806e9b4 allocate a task
 *                  of type 167/168/169 and seed its unk24/unk20 position.
 *
 * The tail (sub_0806ec40-sub_0806ef38) is the "cursor"/menu-ish task group:
 * sub_0806ed28 walks a 6-entry s16[6][2] table at 0x0873E5F8, and
 * sub_0806ee30 is a class-1 entry point that re-arms the running task from
 * the player record (Task.unk88) and the id at 0x020055C0.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern u16 gUnk_020055C0;
extern u8 gUnk_03001F30;
extern s8 gUnk_030023B8;
extern vs16 gUnk_03004CA0[];

/* ROM tables */
extern s16 gUnk_0873E5F8[][2];
extern u32 gUnk_0873ECE0[];
extern u8 gUnk_0873FAE8[];
extern u32 gUnk_0873FB04[];
extern u32 gUnk_0873FB24[];
extern u32 gUnk_0874C718[];
extern u32 gUnk_0874C828[];
extern u32 gUnk_0874CB3C[];
extern u32 gUnk_0874CB7C[];
extern u32 gUnk_0874CBD0[];
extern u32 gUnk_0874CC38[];
extern u32 gUnk_0874CCA4[];
extern u32 gUnk_0874CFEC[];
extern u32 gUnk_08752548[];
extern u32 gUnk_08752D50[];

/* Externals */
extern void TaskDispatchTrampoline(void);
extern void TaskYieldTrampoline(u32 a);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_080059d8(void);
extern void sub_080059fc(void);
extern void sub_08005654(s32 id);
extern void sub_08005ca0(void);
extern void sub_08006138(void);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_08006384(s32 a);
extern u16 sub_080064ac(s32 base, u8 scale, u8 amount);
extern s16 sub_080064dc(s32 base, u8 scale, u8 amount);
extern u32 sub_08025e88(s32 i);
extern void sub_08064a60(void);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern s32 sub_08064d6c(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern void sub_080651b4(void);
extern void sub_0806523c(void);
extern u8 sub_08065f2c(s32 i);
extern void sub_0806ff7c(void);
extern void sub_08070648(void);

/* Defined below */
void sub_0806e210(void);
void sub_0806e3dc(void);
void sub_0806e5f0(void);
void sub_0806e6b0(void);
void sub_0806e7c0(void);
void sub_0806e96c(void);
s32 sub_0806e9b4(u8 a, s16 x, s16 y);
void sub_0806ec88(void);
void sub_0806ed28(void);
void sub_0806ed9c(void);
void sub_0806ef1c(void);

void sub_0806e0f0(void)
{
    s32 i;

    if (gUnk_03004CA0[i = gUnk_03002490->unk44] == -1 || sub_08065f2c(i) != 1)
        sub_08005654(gCurTaskIdx);
}

void sub_0806e138(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059fc;
    t->unk0C = (u32)sub_080651b4;
    t->unk38 = gUnk_0874CBD0;
    t->unk42 = 10;
    gUnk_03002490->unk04 = (u32)sub_0806e210;
    gUnk_03002490->unk40 = 0;
    while (1)
    {
        gUnk_03002490->unk4C = sub_080064dc(-12, 1, 24) << 16;
        gUnk_03002490->unk50 = sub_080064ac(-12, 1, 24) << 16;
        gUnk_03002490->unk58 = 0xFFFF8000;
        sub_08006338(18);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        sub_08006338(14);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = 0xFFFF0000;
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk58 = 0xFFFE0000;
        sub_08006338(24);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(4);
    }
}

void sub_0806e210(void)
{
    s32 i;

    if (gUnk_03004CA0[i = gUnk_03002490->unk44] == -1 || sub_08065f2c(i) != 1)
        sub_08005654(gCurTaskIdx);
}

void sub_0806e258(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059fc;
    t->unk0C = (u32)sub_080651b4;
    t->unk38 = gUnk_0874CC38;
    t->unk42 = 10;
    gUnk_03002490->unk04 = (u32)sub_0806e3dc;
    gUnk_03002490->unk40 = 0;
    while (1)
    {
        gUnk_03002490->unk4C = sub_080064dc(-12, 1, 24) << 16;
        gUnk_03002490->unk50 = sub_080064ac(-12, 1, 24) << 16;
        gUnk_03002490->unk54 = 0x10000;
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk54 = 0xFFFF0000;
        gUnk_03002490->unk58 = 0xFFFF0000;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk54 = 0;
        gUnk_03002490->unk58 = 0x10000;
        TaskYieldTrampoline(1);
        sub_080062c4();
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk4C = sub_080064dc(-12, 1, 24) << 16;
        gUnk_03002490->unk50 = sub_080064ac(-12, 1, 24) << 16;
        gUnk_03002490->unk54 = 0x10000;
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk54 = 0xFFFF0000;
        gUnk_03002490->unk58 = 0xFFFF0000;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk54 = 0;
        gUnk_03002490->unk58 = 0x10000;
        TaskYieldTrampoline(1);
        sub_080062c4();
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(28);
        gUnk_03002490->unk4C = sub_080064dc(-12, 1, 24) << 16;
        gUnk_03002490->unk50 = sub_080064ac(-12, 1, 24) << 16;
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk4C = sub_080064dc(-12, 1, 24) << 16;
        gUnk_03002490->unk50 = sub_080064ac(-12, 1, 24) << 16;
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(2);
    }
}

void sub_0806e3dc(void)
{
    s32 i;

    if (gUnk_03004CA0[i = gUnk_03002490->unk44] == -1 || sub_08065f2c(i) != 1)
        sub_08005654(gCurTaskIdx);
}

void sub_0806e424(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059fc;
    if (gUnk_030023B8 != 7)
        t->unk0C = (u32)sub_080651b4;
    else
        t->unk0C = (u32)sub_08005ca0;
    u = gUnk_03002490;
    u->unk38 = gUnk_08752D50;
    u->unk42 = 10;
    gUnk_03002490->unk04 = (u32)sub_0806e5f0;
    gUnk_03002490->unk40 = 0;

    while (1)
    {
        gUnk_03002490->unk4C = sub_080064ac(-16, 1, 32) << 16;
        gUnk_03002490->unk50 = sub_080064ac(-16, 1, 32) << 16;
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C -= 1;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(30);
        gUnk_03002490->unk4C = sub_080064ac(-16, 1, 32) << 16;
        gUnk_03002490->unk50 = sub_080064ac(-16, 1, 32) << 16;
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C -= 1;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk4C = sub_080064ac(-16, 1, 32) << 16;
        gUnk_03002490->unk50 = sub_080064ac(-16, 1, 32) << 16;
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C -= 1;
        TaskYieldTrampoline(1);
    }
}

void sub_0806e5f0(void)
{
    s32 i;

    if (gUnk_03004CA0[i = gUnk_03002490->unk44] == -1 || sub_08065f2c(i) != 1)
        sub_08005654(gCurTaskIdx);
}

void sub_0806e638(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059fc;
    t->unk0C = (u32)sub_080651b4;
    t->unk38 = gUnk_0874CB3C;
    t->unk42 = 4;
    u = gUnk_03002490;
    u->unk04 = (u32)sub_0806e6b0;
    u->unk40 = 0;
    u->unk4C = 0;
    u->unk50 = 0;
    u->unk3C = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(1);
    } while ((s16)(++gUnk_03002490->unk6C) <= 6);
    TaskDispatchTrampoline();
}

void sub_0806e6b0(void)
{
    s32 i;

    if (gUnk_03004CA0[i = gUnk_03002490->unk44] == -1 || sub_08065f2c(i) != 1)
        sub_08005654(gCurTaskIdx);
}

s32 sub_0806e6f8(s16 x, s16 y)
{
    struct Task *t;
    s32 i;

    i = sub_08064d6c(167, 0, 0, 0);
    if (i != -1)
    {
        t = &gUnk_03002790[i];
        t->unk24 = x;
        t->unk20 = y;
    }
    return i;
}

void sub_0806e73c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059fc;
    t->unk0C = (u32)sub_080651b4;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874CCA4;
    u->unk04 = (u32)sub_0806e7c0;
    u->unk40 = 0;
    sub_08064a60();
    while (1)
    {
        sub_08006384(gUnk_03002490->unk24);
        gUnk_03002490->unk50 = gUnk_03002490->unk20 << 16;
        sub_080061c0(0xFFFD0000, 0x5A5A5A5A);
        sub_08006338(0);
        TaskYieldTrampoline(2);
        sub_08006338(2);
        TaskYieldTrampoline(3);
        sub_08006338(4);
        TaskYieldTrampoline(2);
    }
}

void sub_0806e7c0(void)
{
    s32 i;

    if (gUnk_03004CA0[i = gUnk_03002490->unk44] == -1 || sub_08065f2c(i) != 1)
        sub_08005654(gCurTaskIdx);
}

s32 sub_0806e808(s16 x, s16 y)
{
    struct Task *t;
    s32 i;

    i = sub_08064d6c(168, 0, 0, 0);
    if (i != -1)
    {
        t = &gUnk_03002790[i];
        t->unk24 = x;
        t->unk20 = y;
    }
    return i;
}

void sub_0806e84c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059fc;
    t->unk0C = (u32)sub_080651b4;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874C718;
    u->unk04 = (u32)sub_0806e96c;
    u->unk40 = 0;
    sub_08064a60();
    gUnk_03002490->unk43 = -gUnk_03002490->unk43;
    while (1)
    {
        gUnk_03002490->unk4C = (sub_080064dc(-8, 1, 16)
            + gUnk_03002490->unk24 * gUnk_03002490->unk43) << 16;
        gUnk_03002490->unk50 = (sub_080064ac(-8, 1, 16)
            + gUnk_03002490->unk20) << 16;
        sub_080061c0(0x30000, 0x5A5A5A5A);
        sub_08006338(0);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
    }
}

void sub_0806e96c(void)
{
    s32 i;

    if (gUnk_03004CA0[i = gUnk_03002490->unk44] == -1 || sub_08065f2c(i) != 1)
        sub_08005654(gCurTaskIdx);
}

s32 sub_0806e9b4(u8 a, s16 x, s16 y)
{
    struct Task *t;
    s32 i;

    i = sub_08064d34(169, 0);
    if (i != -1)
    {
        t = &gUnk_03002790[i];
        t->unk73 = a;
        t->unk24 = x;
        t->unk20 = y;
    }
    return i;
}

void sub_0806e9fc(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_080651b4;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874C828;
    u->unk48 += u->unk24;
    u->unk4A += u->unk20;
    u->unk4C = u->unk48 << 16;
    u->unk50 = u->unk4A << 16;
    sub_08002e98(u->unk73, 3, gUnk_0873ECE0);
    sub_08006138();
}

void sub_0806ea70(void)
{
    gUnk_03002490->unk3C = 16;
    TaskYieldTrampoline(1);
    sub_0806e9b4(1, 0, 0);
    sub_0806e9b4(2, 0, 0);
    gUnk_03002490->unk3C += 1;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 1;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 1;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 1;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 1;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 1;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 1;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

void sub_0806eb04(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk3C = 0;
    t->unk54 = 0xFFFA0000;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    u = gUnk_03002490;
    u->unk54 = 0xFFFE0000;
    u->unk5C = 0x1000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_0806eba4(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk3C = 1;
    t->unk54 = 0x60000;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    u = gUnk_03002490;
    u->unk54 = 0x20000;
    u->unk5C = 0xFFFFF000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_0806ec40(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059fc;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 7;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874CB7C;
    u->unk04 = (u32)sub_0806ec88;
    u->unk40 = 0;
    u->unk4C = 0;
    u->unk50 = 0;
    sub_08006138();
}

void sub_0806ec88(void)
{
    s32 i;
    s32 j;

    if (gUnk_03004CA0[i = gUnk_03002490->unk44] != -1 && sub_08065f2c(i) == 1)
    {
        if ((s8)gUnk_03002790[j = gUnk_03002490->unk44].unk7C == 4
            && (u16)(gUnk_03002790[j].unk82 - 2) <= 1)
        {
            sub_08005654(gCurTaskIdx);
        }
        else
        {
            gUnk_03002490->unk7A = 0;
            sub_0806ed28();
        }
    }
    else
    {
        sub_08005654(gCurTaskIdx);
    }
}

void sub_0806ed28(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    s32 v;
    s32 i;

    t = gUnk_03002490;
    v = t->unk70;
    i = (s16)v >> 1;
    if (i <= 5)
    {
        if (t->unk20 <= 0)
        {
            t->unk70 = v + 1;
            if (t->unk1C != 0)
                t->unk3C = 0xFFFF;
            else
                t->unk3C = 4;
            u = gUnk_03002490;
            u->unk20 = gUnk_0873E5F8[i][u->unk1C];
            u->unk1C ^= 1;
        }
        w = gUnk_03002490;
        w->unk20 -= 1;
    }
    else
    {
        sub_08005654(gCurTaskIdx);
    }
}

void sub_0806ed9c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_080651b4;
    t->unk38 = gUnk_08752548;
    t->unk42 = 10;
    gUnk_03002490->unk40 = 0;
    sub_08006338(0);
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 1;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 1;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 1;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 1;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 1;
    TaskYieldTrampoline(1);
}

void sub_0806ee1c(void)
{
    sub_0806ed9c();
    TaskDispatchTrampoline();
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

    p = gUnk_03002490->unk88;
    p->unk05 = p->unk04;
    q = gUnk_03002490->unk88;
    q->unk04 = 20;
    if (gUnk_03001F30 == 1)
        sub_08070648();
    t = gUnk_03002490;
    t->unk0C = (u32)sub_0806ff7c;
    t->unk04 = (u32)sub_0806ef1c;
    t->unk08 = 0;
    t->unk12 = 4;
    gUnk_03002490->unk38 = gUnk_0874CFEC;
    sub_080062c4();
    u = gUnk_03002490;
    u->unk3E &= 0x7FFF;
    u->unk88->unk42 &= 0xFFEF;
    u->unk44 = gUnk_020055C0;
    if (gUnk_03002790[u->unk44].unk74 == 0)
        gUnk_03002490->unk43 = gUnk_0873FAE8[sub_08025e88(u->unk44)];
    else
        u->unk43 = gUnk_0873FAE8[gUnk_03002790[u->unk44].unk74];
    v = gUnk_03002490;
    v->unk14 = 2;
    sub_08002e98(gUnk_03002490->unk14, 8, gUnk_0873FB04);
}

void sub_0806ef1c(void)
{
    sub_08002e98(gUnk_03002490->unk15, 8, gUnk_0873FB24);
}

void sub_0806ef38(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    sub_08002e98(t->unk14, 8, gUnk_0873FB04);
}
