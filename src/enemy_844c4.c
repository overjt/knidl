/* game_code_and_rodata 0x080844C4-0x08084D14 (issue #69, module M22 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080844C4 0x08084D14 src/enemy_844c4.c --newpb
 *
 * The body of the class-3 task #10 script whose entry (`sub_08084484`) is at
 * the end of src/enemy_82e68.c: `sub_080844c4` / `sub_080844f8` are the two
 * unk73 rows of `0x08741F70`, the six coroutine bodies hang off `0x08741F78`
 * and the six per-frame guards off `0x08741F90`, and `sub_0808451c` is the
 * per-frame hook - it keeps the low half of Task.unk24 (the 16.16 vertical
 * offset the draw helper reads) while Task.unk7A bit 0 says the object is
 * still attached, and or-s in 0x10000 while `gUnk_03005550[4]` (the room's
 * kind) is outside 1-4.
 *
 * `sub_08084bc0` / `sub_08084c0c` / `sub_08084c5c` / `sub_08084cb8` plus the
 * shared `sub_08084c84` are the four class-3 hook rows at `0x08742CF0` /
 * `0x08742D00`: each returns 1 when it has handed the task to a new state and
 * 0 otherwise, and all four open with the same `Task.unk73 == 1` bail-out.
 * `sub_08084cd4` is the class-3 task #14 entry; its script is in
 * src/enemy_84d14.c.
 *
 * `sub_08084a50` is a leaf the census could not propose (no `push`, lesson
 * 4.30): the anchor-table word at `0x08741FA0` points at it and it counts the
 * <Task.unk30, Task.unk34> pair down, restoring both when unk34 underflows.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern u8 gUnk_03005550[];

/* ROM tables */
extern u32 gUnk_0873F500[];
extern u32 gUnk_08741F78[];
extern s16 gUnk_08741FA8[];
extern s32 gUnk_08741FAC[];
extern s8 gUnk_08741FB4[];
extern u32 gUnk_08741F90[];
extern u32 gUnk_08752190[];
extern u32 gUnk_08752284[];
extern u32 gUnk_08741FB8[];

/* Externals */
extern void TaskYieldTrampoline(u32 frames);
extern void TaskDispatchTrampoline(void);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_08005654(s32 i);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_0806395c(s32 a);
extern void sub_080639b4(u32 *p);
extern void sub_08063e14(void);
extern s32 sub_08063c74(void);
extern u8 sub_080699a8(void);
extern void sub_08063ff4(void);
extern void sub_0806a0f0(s32 a);
extern void sub_080656b4(void);
extern void sub_080059d8(void);
extern void sub_08005d9c(void);
extern void sub_0806523c(void);
extern s32 sub_08064cdc(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern void sub_08068e04(void);
extern u8 sub_080692fc(void);
extern void sub_08069b44(void);

/* Defined below */
void sub_0808451c(void);
void sub_08084ae8(void);
void sub_08084b7c(void);
s32 sub_08084c84(void);

void sub_080844c4(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0808451c;
    t->unk7A = 1;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_08741F78);
}

void sub_080844f8(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0808451c;
    sub_08002e98(t->unk14, 6, gUnk_08741F78);
}

void sub_0808451c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    u8 r;

    r = sub_080692fc();
    t = gUnk_03002490;
    if ((t->unk7A & 1) != 0)
    {
        if ((u8)(gUnk_03005550[4] - 1) > 3)
            t->unk24 = (u16)t->unk24 | 0x10000;
        if ((gUnk_03002490->unk7A & 1) != 0)
            goto skip;
    }
    u = gUnk_03002490;
    u->unk24 = (u16)u->unk24;
skip:
    if (r == 0)
    {
        sub_08084c84();
        sub_08002e98(gUnk_03002490->unk15, 6, gUnk_08741F90);
    }
    v = gUnk_03002490;
    v->unk24 = (v->unk24 & 0xFFFF0000) | v->unk4A;
    sub_08068e04();
    sub_08069b44();
}

void sub_080845b4(void)
{
    struct Task *t;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_08063e14();
    t = gUnk_03002490;
    t->unk30 = gUnk_08741FA8[t->unk74];
    sub_080061c0(gUnk_08741FAC[t->unk74], 0x5A5A5A5A);
    while (1)
    {
        sub_08006338(9);
        TaskYieldTrampoline(gUnk_08741FB4[gUnk_03002490->unk74]);
        u1 = gUnk_03002490;
        u1->unk3C++;
        TaskYieldTrampoline(gUnk_08741FB4[u1->unk74]);
        u2 = gUnk_03002490;
        u2->unk3C++;
        TaskYieldTrampoline(gUnk_08741FB4[u2->unk74]);
        u3 = gUnk_03002490;
        u3->unk3C++;
        TaskYieldTrampoline(gUnk_08741FB4[u3->unk74]);
        u4 = gUnk_03002490;
        u4->unk3C++;
        TaskYieldTrampoline(gUnk_08741FB4[u4->unk74]);
        u5 = gUnk_03002490;
        u5->unk3C--;
        TaskYieldTrampoline(gUnk_08741FB4[u5->unk74]);
        u6 = gUnk_03002490;
        u6->unk3C--;
        TaskYieldTrampoline(gUnk_08741FB4[u6->unk74]);
        u7 = gUnk_03002490;
        u7->unk3C--;
        TaskYieldTrampoline(gUnk_08741FB4[u7->unk74]);
    }
}

void sub_080846c4(void)
{
    if (--gUnk_03002490->unk30 == 0)
    {
        sub_0806395c(1);
        sub_08006148(sub_080844f8, gCurTaskIdx);
    }
}

void sub_080846f4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;

    gUnk_03002490->unk15 = 5;
    sub_080062c4();
    t = gUnk_03002490;
    if (t->unk74 != 0)
    {
        if (t->unk74 == 1)
            t->unk58 = -0x30000;
    }
    u = gUnk_03002490;
    u->unk60 = 0x1500;
    u->unk68 = 0x30000;
    while (1)
    {
        sub_08006338(9);
        TaskYieldTrampoline(gUnk_08741FB4[gUnk_03002490->unk74]);
        u1 = gUnk_03002490;
        u1->unk3C++;
        TaskYieldTrampoline(gUnk_08741FB4[u1->unk74]);
        u2 = gUnk_03002490;
        u2->unk3C++;
        TaskYieldTrampoline(gUnk_08741FB4[u2->unk74]);
        u3 = gUnk_03002490;
        u3->unk3C++;
        TaskYieldTrampoline(gUnk_08741FB4[u3->unk74]);
        u4 = gUnk_03002490;
        u4->unk3C++;
        TaskYieldTrampoline(gUnk_08741FB4[u4->unk74]);
        u5 = gUnk_03002490;
        u5->unk3C--;
        TaskYieldTrampoline(gUnk_08741FB4[u5->unk74]);
        u6 = gUnk_03002490;
        u6->unk3C--;
        TaskYieldTrampoline(gUnk_08741FB4[u6->unk74]);
        u7 = gUnk_03002490;
        u7->unk3C--;
        TaskYieldTrampoline(gUnk_08741FB4[u7->unk74]);
    }
}

void sub_080847f8(void)
{
}

void sub_080847fc(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 1;
    sub_080062c4();
    sub_08006338(5);
    TaskYieldTrampoline(4);
    t = gUnk_03002490;
    t->unk3C--;
    TaskYieldTrampoline(10);
    t = gUnk_03002490;
    t->unk3C++;
    TaskYieldTrampoline(20);
    t = gUnk_03002490;
    t->unk3C--;
    TaskYieldTrampoline(40);
    sub_0806395c(2);
    sub_08006138();
}

void sub_08084854(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_080844f8, gCurTaskIdx);
}

void sub_0808487c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;

    gUnk_03002490->unk15 = 2;
    sub_080062c4();
    t = gUnk_03002490;
    t->unk30 = 224;
    t->unk34 = 1;
    while (1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(26);
        u = gUnk_03002490;
        u->unk3C++;
        TaskYieldTrampoline(22);
        gUnk_03002490->unk46 = sub_08064cdc(194, 12, 0, 1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(26);
        w = gUnk_03002490;
        w->unk3C--;
        TaskYieldTrampoline(22);
    }
}

void sub_080848e4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    if (--t->unk30 == 0)
        t->unk34--;
    u = gUnk_03002490;
    if (u->unk34 == 0 && u->unk30 == 0)
    {
        sub_0806395c(3);
        sub_08006148(sub_080844f8, gCurTaskIdx);
    }
    v = gUnk_03002490;
    if (v->unk74 != 0 && v->unk34 != 0 && v->unk30 <= 119 && sub_08063c74() <= 0xFFF)
    {
        sub_0806395c(3);
        sub_08006148(sub_080844f8, gCurTaskIdx);
    }
}

void sub_08084960(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 3;
    sub_08006338(5);
    TaskYieldTrampoline(4);
    t = gUnk_03002490;
    t->unk3C--;
    TaskYieldTrampoline(10);
    t = gUnk_03002490;
    t->unk3C++;
    TaskYieldTrampoline(20);
    t = gUnk_03002490;
    t->unk3C--;
    TaskYieldTrampoline(40);
    sub_0806395c(0);
    sub_08006138();
}

void sub_080849b4(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_080844f8, gCurTaskIdx);
}

void sub_080849dc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;

    gUnk_03002490->unk15 = 4;
    sub_080062c4();
    t = gUnk_03002490;
    t->unk60 = 0x1500;
    t->unk68 = 0x30000;
    t->unk30 = 224;
    t->unk34 = 1;
    while (1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(26);
        u = gUnk_03002490;
        u->unk3C++;
        TaskYieldTrampoline(22);
        gUnk_03002490->unk46 = sub_08064cdc(194, 12, 0, 1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(26);
        w = gUnk_03002490;
        w->unk3C--;
        TaskYieldTrampoline(22);
    }
}

void sub_08084a50(void)
{
    struct Task *t;
    s32 a;
    s32 b;

    t = gUnk_03002490;
    a = t->unk30;
    t->unk30 = a - 1;
    if (t->unk30 == 0)
    {
        b = t->unk34;
        t->unk34 = b - 1;
        if (t->unk34 < 0)
        {
            t->unk30 = a;
            t->unk34 = b;
        }
    }
}

void sub_08084a74(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;

    gUnk_03002490->unk04 = (u32)sub_08084ae8;
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_08063e14();
    while (1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(26);
        u = gUnk_03002490;
        u->unk3C++;
        TaskYieldTrampoline(22);
        gUnk_03002490->unk46 = sub_08064cdc(194, 12, 0, 1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(26);
        w = gUnk_03002490;
        w->unk3C--;
        TaskYieldTrampoline(22);
    }
}

void sub_08084ae8(void)
{
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

void sub_08084afc(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08005d9c;
    t->unk38 = gUnk_08752190;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk04 = (u32)sub_08084b7c;
    u->unk43 = (gUnk_03002790 + (s16)u->unk44)->unk43;
    sub_080061c0(0x2000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = -0x4000;
    sub_08006338(0);
    TaskYieldTrampoline(48);
    TaskDispatchTrampoline();
}

void sub_08084b7c(void)
{
    struct Task *t;

    t = gUnk_03002790 + (s16)gUnk_03002490->unk44;
    if (t->unk73 != 1 && t->unk14 != 2 && t->unk14 != 4)
        sub_08005654(gCurTaskIdx);
}

u8 sub_08084bc0(void)
{
    struct Task *t;
    s32 v;

    t = gUnk_03002490;
    if (t->unk73 != 1)
    {
        switch (t->unk14)
        {
        case 5:
            v = 0;
            break;
        case 4:
            v = 2;
            break;
        default:
            goto def;
        }
        sub_0806395c(v);
        sub_08006148(sub_080844f8, gCurTaskIdx);
        return 1;
def:
        sub_080062c4();
    }
    return 0;
}

u8 sub_08084c0c(void)
{
    struct Task *t;
    s32 v;

    t = gUnk_03002490;
    if (t->unk73 != 1)
    {
        switch (t->unk14)
        {
        case 0:
        case 3:
            v = 5;
            break;
        case 1:
        case 2:
            v = 4;
            break;
        default:
            goto out;
        }
        sub_0806395c(v);
        sub_08006148(sub_080844f8, gCurTaskIdx);
        return 1;
    }
out:
    return 0;
}

u8 sub_08084c5c(void)
{
    if (gUnk_03002490->unk73 == 1)
        return 0;
    sub_0806a0f0(-2);
    return 1;
}

s32 sub_08084c84(void)
{
    struct Task *t;
    u8 r;

    t = gUnk_03002490;
    if (t->unk73 == 1)
        return 0;
    if (t->unk54 != 0)
    {
        r = sub_080699a8();
        if (r != 0)
        {
            sub_08063ff4();
            return 0;
        }
    }
}

s32 sub_08084cb8(void)
{
    if (gUnk_03002490->unk73 != 1)
        sub_08063ff4();
    return 0;
}

void sub_08084cd4(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_08752284;
    sub_08002e98(u->unk73, 2, gUnk_08741FB8);
}
