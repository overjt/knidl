/* game_code_and_rodata 0x080673EC-0x080692FC (issue #65, module M17 batch 4).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080673EC 0x080692FC src/actor_673ec.c --newpb
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u8 gUnk_03001F30;
extern u16 gUnk_03002360;
extern s8 gUnk_0873E1F8[];
extern u32 gUnk_0873E2F0[];
extern u32 gUnk_0873E31C[];
extern vs16 gUnk_03004CA0[];
extern u32 gUnk_02007D00[];
extern struct PlayerState gUnk_03002170[];

extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_080062e0(s32 i);
extern s32 sub_08009ee8(s32 a, s32 b);
extern void sub_08064eb8(u32 a);
extern void sub_0800a008(u32 a, s32 b, s32 c);
extern void sub_080034f0(s32 a, s32 b);
extern void sub_0803d1c4(s32 i);
extern void sub_0803d2d4(s32 i);
extern void sub_08067108(void);
extern void sub_08068a8c(s32 i, u8 flag);
extern void sub_08068b88(s32 i, u32 b, u32 c, u32 d);
extern void sub_0806737c(void);
extern void sub_0803e2d4(void);
extern void sub_0803e080(void);
extern void sub_08026264(s16 x, s16 y);
extern void sub_08068690(void);
extern void sub_08068760(void);
extern u32 sub_080687a0(void);
extern void sub_080062c4(void);
extern void sub_080687e0(void);
extern void sub_08006138(void);
extern void sub_080687fc(void);
extern void sub_08068828(void);
extern void sub_0803d494(void);
extern void sub_08068840(void);
extern void sub_0806865c(s32 i);
extern void sub_0806896c(void);
extern void sub_08006338(s32 a);
extern void sub_080684a4(void);
extern void TaskYieldTrampoline(u32 a);
extern u16 gUnk_03001EA4;
extern s32 sub_08068a2c(s32 a, s32 b);
extern void sub_0803e1b8(u32 a, u32 b, s32 c);
extern void sub_080031b8(u32 a);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006364(s32 a);
extern u32 gUnk_0873CB1C[];
extern u8 gUnk_03005550;
extern s32 gUnk_0873E348[];
extern s32 gUnk_0873E388[];
extern void sub_080227a4(struct Task *t);
extern void sub_0801bcac(u32 *p);
extern u16 gUnk_0873E3C8[];
extern void sub_080261d4(u32 a);
extern void sub_080682a8(void);
extern void sub_08006148(void *fn, s32 i);
extern s32 sub_08064d34(u32 type, u8 keepPrio);

void sub_080675e4(void);

void sub_080673ec(void)
{
    s32 d;

    sub_08002e98(gUnk_03002490->unk14, 11, gUnk_0873E2F0);
}

void sub_08067408(void)
{
    sub_08002e98(gUnk_03002490->unk15, 11, gUnk_0873E31C);
    sub_0803e2d4();
    if ((gUnk_03002490->unk88->unk42 & 32) == 0)
        sub_0803e080();
    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
        sub_08026264(gUnk_03002490->unk48, gUnk_03002490->unk4A);
}

void sub_08067470(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08068690();
    while (sub_080687a0() == 0)
    {
        sub_08068760();
        TaskYieldTrampoline(1);
    }
    sub_080062c4();
    sub_080687e0();
    sub_08006138();
}

void sub_080674a8(void)
{
    struct Task *t;

    if (gUnk_03001F30 != 0)
        return;
    t = gUnk_03002490;
    if (t->unk30 <= 0)
        return;
    if (t->unk3C == -1)
        return;
    sub_080687fc();
}

void sub_080674d8(void)
{
    gUnk_03002490->unk15 = 1;
    sub_08068828();
    sub_08006138();
}

void sub_080674f4(void)
{
}

void sub_080674f8(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_0803d494;
    t->unk15 = 2;
    sub_080062c4();
    sub_08068840();
    sub_08006138();
}

void sub_08067520(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk34 <= 0)
    {
        sub_080062c4();
        sub_0806865c(gCurTaskIdx);
    }
    else
    {
        t->unk34--;
        sub_0806896c();
    }
}

void sub_08067550(void)
{
    struct Task *u;
    struct PlayerState *p;

    gUnk_03002490->unk15 = 3;
    gUnk_03002490->unk43 = gUnk_03002790[gUnk_03002490->unk44].unk43;
    if (gUnk_03001F30 == 0)
    {
        u = gUnk_03002490;
        p = u->unk88;
        if (p->unk06 == 1)
        {
            u->unk28 = 0x11C1;
            u->unk2C = 3;
        }
        else
        {
            u->unk28 = 0x133;
            u->unk2C = -8;
        }
        sub_08006338(*(s16 *)&gUnk_03002490->unk28);
        sub_080684a4();
    }
    else
    {
        sub_08006338(0x123B);
    }
    sub_08006138();
}
void sub_080675d8(void)
{
    sub_080675e4();
}

/* Follow the carried task's frame offsets out of gUnk_0873E1F8. */
void sub_080675e4(void)
{
    struct Task *t;
    struct Task *u;
    s8 *p;

    t = gUnk_03002490;
    u = &gUnk_03002790[t->unk44];
    if ((u16)(u->unk3C - 36) > 25)
        return;
    p = &gUnk_0873E1F8[u->unk3C * 4];
    t->unk48 = u->unk48 + *p * t->unk43;
    p++;
    t->unk4A = *p + u->unk4A;
    p++;
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
    if (gUnk_03001F30 == 0)
    {
        if (*p++ != 0)
            t->unk3C = t->unk28 + t->unk2C;
        else
            t->unk3C = t->unk28;
        sub_080684a4();
    }
    else
    {
        if (*p++ != 0)
            t->unk3C = 0x1243;
        else
            sub_08006338(0x123B);
    }
    gUnk_03002490->unk42 = *p;
}

/* Task body: the carried task's "thrown" arc. */
void sub_080676c0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    u = gUnk_03002490;
    u->unk00 = (u32)sub_0803d494;
    u->unk15 = 4;
    sub_080062c4();
    sub_08068a2c(-8, 512);
    if (gUnk_03001F30 == 0)
        sub_080675e4();
    t = gUnk_03002490;
    t->unk28 = gUnk_03002790[t->unk44].unk28;
    t->unk34 = 0;
    if (t->unk78 == 0)
        t->unk34 = 1;
    if (gUnk_03002490->unk28 != 3)
        TaskYieldTrampoline(28);
    else
        TaskYieldTrampoline(1);
    if (gCurTaskIdx == gUnk_03002360)
    {
        if (gUnk_03001F30 == 0)
        {
            if (gUnk_03002490->unk88->unk06 == 1)
                sub_080031b8(158);
            else if (gUnk_03001EA4 & 1)
                sub_080031b8(111);
            else
                sub_080031b8(112);
        }
        else
        {
            sub_080031b8(0x107);
        }
    }
    sub_0803e1b8(1, 96, gCurTaskIdx);
    v = gUnk_03002490;
    if (v->unk28 == 2)
        v->unk43 = -v->unk43;
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0x18000, 0x5A5A5A5A);
    sub_0800622c(0xFFFD8000, 0x2500, 0x30000);
    if (gUnk_03001F30 == 0 && gUnk_03002490->unk88->unk06 == 1)
    {
        sub_08006338(0x11C2);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
    }
    else
    {
        w = gUnk_03002490;
        w->unk2C = w->unk43;
        w->unk28 = 8;
        if (gUnk_03001F30 == 0)
            sub_08006364(0x133);
        else
            sub_08006338(0x123B);
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_0806896c();
            TaskYieldTrampoline(1);
        } while ((s16)(++gUnk_03002490->unk6C) <= 29);
    }
    x = gUnk_03002490;
    x->unk34 = x->unk34 + 1;
    sub_08006138();
}
void sub_08067908(void)
{
    sub_080227a4(gUnk_03002490);
    sub_0801bcac(gUnk_0873CB1C);
    if (gUnk_03005550 != 0)
        gUnk_03002490->unk54 = 0;
    if (gUnk_03002490->unk34 != 0)
    {
        sub_080062c4();
        sub_0806865c(gCurTaskIdx);
    }
}

/* Task body: the carried task wobbling in the player's hands. */
void sub_08067950(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    gUnk_03002490->unk15 = 5;
    u = gUnk_03002490;
    u->unk43 = gUnk_03002790[u->unk44].unk43;
    v = gUnk_03002490;
    v->unk34 = 0;
    v->unk42 = 7;
    sub_080062c4();
    t = gUnk_03002490;
    t->unk4C = (t->unk43 * 3) << 19;
    t->unk50 = 0;
    while (gUnk_03002790[gUnk_03002490->unk44].unk3C == 34)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk42 = 12;
    if (gUnk_03002490->unk88->unk06 == 1)
    {
        sub_08006138();
        return;
    }
    while (1)
    {
        sub_080061c0(gUnk_0873E348[gUnk_03002490->unk34], 0x5A5A5A5A);
        w = gUnk_03002490;
        w->unk58 = gUnk_0873E388[w->unk34];
        TaskYieldTrampoline(2);
        x = gUnk_03002490;
        x->unk34++;
        if (x->unk34 > 14)
            x->unk34 = 0;
    }
}
void sub_08067a48(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *p;
    s32 i;

    if (gUnk_03001F30 == 0)
    {
        t = gUnk_03002490;
        p = t->unk88;
        if (p->unk06 == 1)
        {
            i = (s16)(gUnk_03002790[t->unk44].unk3C - 30);
            switch (i)
            {
            case 1:
                sub_08006338(0x173);
                break;
            case 0:
            case 2:
                sub_08006338(370);
                break;
            case 3:
            case 4:
                sub_08006338(0x171);
                break;
            }
        }
        else
        {
            sub_08006338(0x12D);
            sub_080684a4();
        }
    }
    else
    {
        sub_08006338(0x1243);
        u = gUnk_03002490;
        if (u->unk43 == 1)
            u->unk3E |= 0x8000;
        else
            u->unk3E &= 0x7FFF;
        sub_08006138();
    }
}
void sub_08067b24(void)
{
    struct Task *t;
    struct PlayerState *p;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_0803d494;
    t->unk15 = 6;
    sub_080062c4();
    t = gUnk_03002490;
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
    sub_08068a2c(-8, 512);
    t = gUnk_03002490;
    t->unk34 = 0;
    if (t->unk78 == 0)
        t->unk34 = 1;
    if (gUnk_03001F30 == 0)
    {
        if (gUnk_03002360 == gCurTaskIdx)
        {
            p = gUnk_03002490->unk88;
            if (p->unk06 == 1)
                sub_080031b8(158);
            else if (gUnk_03001EA4 & 1)
                sub_080031b8(111);
            else
                sub_080031b8(112);
        }
    }
    else
    {
        sub_080031b8(0x107);
    }
    sub_0803e1b8(1, 96, gCurTaskIdx);
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0x18000, 0x5A5A5A5A);
    sub_0800622c(0xFFFD8000, 0x2500, 0x30000);
    if (gUnk_03001F30 == 0)
    {
        t = gUnk_03002490;
        p = t->unk88;
        if (p->unk06 == 1)
        {
            sub_08006338(370);
            TaskYieldTrampoline(3);
            t = gUnk_03002490;
            t->unk3C++;
            TaskYieldTrampoline(4);
            t = gUnk_03002490;
            t->unk3C--;
            TaskYieldTrampoline(2);
            t = gUnk_03002490;
            t->unk3C--;
            TaskYieldTrampoline(4);
            t = gUnk_03002490;
            t->unk3C++;
            TaskYieldTrampoline(2);
            t = gUnk_03002490;
            t->unk3C++;
            TaskYieldTrampoline(4);
            t = gUnk_03002490;
            t->unk3C--;
            TaskYieldTrampoline(3);
            t = gUnk_03002490;
            t->unk3C--;
            TaskYieldTrampoline(5);
            t = gUnk_03002490;
            t->unk3C++;
            TaskYieldTrampoline(3);
        }
        else
        {
            t->unk2C = t->unk43;
            t->unk28 = 8;
            sub_08006364(0x133);
            gUnk_03002490->unk6C = 0;
            do
            {
                sub_0806896c();
                TaskYieldTrampoline(1);
                t = gUnk_03002490;
                t->unk6C++;
            } while ((s16)t->unk6C <= 29);
        }
    }
    else
    {
        sub_08006338(0x123B);
        TaskYieldTrampoline(6);
        sub_08006338(0x1241);
        TaskYieldTrampoline(26);
    }
    t = gUnk_03002490;
    t->unk34++;
    sub_08006138();
}

void sub_08067d30(void)
{
    sub_080227a4(gUnk_03002490);
    sub_0801bcac(gUnk_0873CB1C);
    if (gUnk_03005550 != 0)
        gUnk_03002490->unk54 = 0;
    if (gUnk_03002490->unk34 != 0)
    {
        sub_080062c4();
        sub_0806865c(gCurTaskIdx);
    }
}

void sub_08067d78(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 7;
    t = gUnk_03002490;
    t->unk43 = gUnk_03002790[t->unk44].unk43;
    sub_08006138();
}

/* Track the carrier's frame through the 5-word gUnk_0873E3C8 table. */
void sub_08067db0(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *p;
    s32 i;

    t = gUnk_03002490;
    u = &gUnk_03002790[t->unk44];
    i = (u->unk3C - 30) * 5;
    if (gUnk_03001F30 == 0)
    {
        p = t->unk88;
        if (p->unk06 == 1)
            i += 75;
    }
    else
    {
        i += 150;
    }
    t = gUnk_03002490;
    t->unk48 = u->unk48 + u->unk43 * gUnk_0873E3C8[i];
    t->unk4A = u->unk4A + gUnk_0873E3C8[i + 1];
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
    t->unk43 = u->unk43 * (s16)gUnk_0873E3C8[i + 2];
    sub_08006338((s16)gUnk_0873E3C8[i + 3]);
    if (gUnk_03001F30 == 0)
        sub_080684a4();
    gUnk_03002490->unk42 = gUnk_0873E3C8[i + 4];
}

/* Task body: the carried task struggling in the player's hands. */
void sub_08067ea4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct PlayerState *p;
    s32 f;

    gUnk_03002490->unk15 = 8;
    gUnk_03002490->unk00 = (u32)sub_0803d494;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk43 = gUnk_03002790[gUnk_03002490->unk44].unk43;
    t = gUnk_03002490;
    u = &gUnk_03002790[t->unk44];
    t->unk4C = (u->unk48 + (t->unk43 << 4)) << 16;
    t->unk50 = (gUnk_03002790[t->unk44].unk4A - 24) << 16;
    sub_080061c0(0x50000, 0x5A5A5A5A);
    sub_0800622c(0xFFFD8000, 0x1200, 0x30000);
    if (gUnk_03001F30 == 0)
    {
        p = gUnk_03002490->unk88;
        if (p->unk06 == 1)
        {
            f = 370;
            while (1)
            {
                sub_08006338(0x171);
                TaskYieldTrampoline(4);
                sub_08006338(f);
                TaskYieldTrampoline(2);
                sub_08006338(0x173);
                TaskYieldTrampoline(4);
                sub_08006338(f);
                TaskYieldTrampoline(2);
            }
        }
        while (1)
        {
            sub_08006338(0x135);
            sub_080684a4();
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C = 0;
            do
            {
                w = gUnk_03002490;
                w->unk3C--;
                TaskYieldTrampoline(1);
                v = gUnk_03002490;
                v->unk6C++;
            } while ((s16)v->unk6C <= 14);
        }
    }
    sub_08006338(0x123C);
    x = gUnk_03002490;
    if (x->unk43 == 1)
        x->unk3E |= 0x8000;
    else
        x->unk3E &= 0x7FFF;
    sub_08006138();
    sub_08006138();
}

void sub_08068028(void)
{
    struct Task *t;

    sub_0801bcac(gUnk_0873CB1C);
    if (gUnk_03005550 != 0)
    {
        sub_08064d34(148, 0);
        sub_080031b8(153);
        sub_080261d4(2);
        t = gUnk_03002490;
        t->unk43 = -t->unk43;
        sub_080062c4();
        sub_08006148(sub_080682a8, gCurTaskIdx);
    }
    else if (gUnk_03002490->unk7A & 1)
    {
        sub_080062c4();
        sub_08006148(sub_080682a8, gCurTaskIdx);
    }
}

/* Task body: the carried task struggling, mirrored variant. */
void sub_080680ac(void)
{
    struct Task *t;
    struct PlayerState *p;
    s32 f;

    gUnk_03002490->unk15 = 9;
    gUnk_03002490->unk00 = (u32)sub_0803d494;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk43 = -gUnk_03002790[gUnk_03002490->unk44].unk43;
    t = gUnk_03002490;
    t->unk4C = gUnk_03002790[t->unk44].unk48 << 16;
    t->unk50 = (gUnk_03002790[t->unk44].unk4A - 40) << 16;
    sub_080061c0(0x50000, 0x5A5A5A5A);
    sub_0800622c(0xFFFD8000, 0x1200, 0x30000);
    if (gUnk_03001F30 == 0)
    {
        p = gUnk_03002490->unk88;
        if (p->unk06 == 1)
        {
            f = 370;
            while (1)
            {
                sub_08006338(0x171);
                TaskYieldTrampoline(4);
                sub_08006338(f);
                TaskYieldTrampoline(2);
                sub_08006338(0x173);
                TaskYieldTrampoline(4);
                sub_08006338(f);
                TaskYieldTrampoline(2);
            }
        }
        while (1)
        {
            sub_08006338(0x135);
            sub_080684a4();
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C = 0;
            do
            {
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(1);
            } while ((s16)(++gUnk_03002490->unk6C) <= 14);
        }
    }
    sub_08006338(0x123C);
    t = gUnk_03002490;
    if (t->unk43 == 1)
        t->unk3E |= 0x8000;
    else
        t->unk3E &= 0x7FFF;
    sub_08006138();
    sub_08006138();
}

void sub_08068224(void)
{
    struct Task *t;

    sub_0801bcac(gUnk_0873CB1C);
    if (gUnk_03005550 != 0)
    {
        sub_08064d34(148, 0);
        sub_080031b8(153);
        sub_080261d4(2);
        t = gUnk_03002490;
        t->unk43 = -t->unk43;
        sub_080062c4();
        sub_08006148(sub_080682a8, gCurTaskIdx);
    }
    else if (gUnk_03002490->unk7A & 1)
    {
        sub_080062c4();
        sub_08006148(sub_080682a8, gCurTaskIdx);
    }
}
void sub_080682a8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_0803d494;
    t->unk15 = 10;
    sub_080062c4();
    sub_08068a2c(-8, 512);
    u = gUnk_03002490;
    u->unk34 = 0;
    if (u->unk78 == 0)
        u->unk34 = 1;
    if (gUnk_03001F30 == 0)
    {
        if (gUnk_03002360 == gCurTaskIdx)
        {
            if (gUnk_03002490->unk88->unk06 == 1)
                sub_080031b8(158);
            else if (gUnk_03001EA4 & 1)
                sub_080031b8(111);
            else
                sub_080031b8(112);
        }
    }
    else
    {
        sub_080031b8(0x107);
    }
    sub_0803e1b8(1, 96, gCurTaskIdx);
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0x18000, 0x5A5A5A5A);
    sub_0800622c(0xFFFD8000, 0x2500, 0x30000);
    if (gUnk_03001F30 == 0)
    {
        if (gUnk_03002490->unk88->unk06 == 1)
        {
            sub_08006338(370);
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
        else
        {
            v = gUnk_03002490;
            v->unk2C = v->unk43;
            v->unk28 = 8;
            sub_08006364(0x133);
            gUnk_03002490->unk6C = 0;
            do
            {
                sub_0806896c();
                TaskYieldTrampoline(1);
            } while ((s16)(++gUnk_03002490->unk6C) <= 14);
        }
    }
    else
    {
        sub_08006338(0x123B);
        TaskYieldTrampoline(6);
        sub_08006338(0x1241);
        TaskYieldTrampoline(7);
    }
    w = gUnk_03002490;
    w->unk34 = w->unk34 + 1;
    sub_08006138();
}
void sub_08068460(void)
{
    sub_080227a4(gUnk_03002490);
    sub_0801bcac(gUnk_0873CB1C);
    if (gUnk_03005550 != 0)
        gUnk_03002490->unk54 = 0;
    if (gUnk_03002490->unk34 != 0)
        sub_0806865c(gCurTaskIdx);
}
void sub_080684a4(void)
{
    s32 d;

    switch (gUnk_03002490->unk88->unk0D)
    {
    case 0:
    case 7:
    case 11:
    case 20:
    case 21:
    case 24:
        d = 0;
        break;
    case 1:
        d = 0x143;
        break;
    case 2:
        d = 0x234;
        break;
    case 3:
        d = 0x2B3;
        break;
    case 4:
        d = 0x384;
        break;
    case 5:
        d = 0x495;
        break;
    case 6:
        d = 0x522;
        break;
    case 8:
        d = 0x5CB;
        break;
    case 9:
        d = 0x69C;
        break;
    case 10:
        d = 0x79C;
        break;
    case 12:
        d = 0x838;
        break;
    case 13:
        d = 0x8C4;
        break;
    case 14:
        d = 0x950;
        break;
    case 15:
        d = 0x9F5;
        break;
    case 16:
        d = 0xA87;
        break;
    case 17:
        d = 0xB0A;
        break;
    case 18:
        d = 0xB94;
        break;
    case 19:
        d = 0xC8F;
        break;
    case 22:
        d = 0xD60;
        break;
    case 23:
        d = 0xE38;
        break;
    case 25:
        d = 0xF3A;
        break;
    default:
        while (1)
            ;
    }
    gUnk_03002490->unk3C += d;
}
void sub_080685ec(s32 i, s32 j, u8 c)
{
    struct Task *t;
    struct Task *u;

    t = &gUnk_03002790[i];
    u = &gUnk_03002790[j];
    t->unk44 = j;
    t->unk12 = 4;
    t->unk14 = c;
    t->unk72 = gUnk_03002490->unk72;
    if (u->unk72 == 2)
        sub_08068a8c(i, 1);
    else
        sub_08068a8c(i, 0);
    gUnk_02007D00[0] = 0;
    gUnk_02007D00[1] = i;
    sub_08006148(sub_0806737c, i);
}
void sub_0806865c(s32 i)
{
    struct Task *t;

    t = &gUnk_03002790[i];
    sub_08068b88(i, 0, 0, 1);
    gUnk_02007D00[1] = -1;
    t->unk72 = 0;
}
void sub_08068690(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    u = &gUnk_03002790[t->unk44];
    t->unk4C = (t->unk48 - u->unk48) << 16;
    t->unk50 = (t->unk4A - u->unk4A) << 16;
    t->unk28 = 8;
    t->unk30 = 2;
    if (gUnk_03001F30 == 0)
    {
        if (t->unk88->unk06 == 1)
        {
            t->unk43 = u->unk43;
            gUnk_03002490->unk2C = 1;
            sub_08006338(0x171);
        }
        else
        {
            t->unk43 = -u->unk43;
            v = gUnk_03002490;
            v->unk2C = -v->unk43;
            v->unk3C = 0x133;
            sub_080684a4();
        }
    }
    else
    {
        t->unk43 = u->unk43;
        sub_08006338(0x123B);
    }
}
void sub_08068760(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;

    t = gUnk_03002490;
    if (t->unk4C <= 0)
        t->unk5C = 10752;
    else
        t->unk5C = -10752;
    u = gUnk_03002490;
    v = u->unk50;
    if (v < 0)
        v = -v;
    v >>= 3;
    if (u->unk50 <= 0)
        u->unk58 = v;
    else
        u->unk58 = -v;
}
u32 sub_080687a0(void)
{
    struct Task *t;
    struct Task *u;
    s32 d;

    t = gUnk_03002490;
    u = &gUnk_03002790[t->unk44];
    d = u->unk48 - t->unk48;
    if (d < 0)
        d = -d;
    if (d <= 23)
        return 1;
    else
        return 0;
}
void sub_080687e0(void)
{
    gUnk_02007D00[0] = 1;
    gUnk_03002490->unk3C = 0xFFFF;
}
void sub_080687fc(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk28 <= 0)
    {
        t->unk3C += t->unk2C;
        t->unk28 = 8;
        t->unk30--;
    }
    else
    {
        t->unk28--;
    }
}
void sub_08068828(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk44 = t->unk18;
    t->unk4C = 0;
    t->unk50 = 0;
}
void sub_08068840(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk48 = gUnk_02007D00[2];
    t->unk4A = gUnk_02007D00[3];
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
    t->unk43 = gUnk_02007D00[4];
    u = gUnk_03002490;
    u->unk2C = -u->unk43;
    u->unk28 = 8;
    if (gUnk_03001F30 == 0)
        u->unk3C = 0x133;
    else
        sub_08006338(0x123B);
    sub_080061c0(0x8000, 0x5A5A5A5A);
    sub_0800622c(0xFFFD0000, 0x2000, 0x30000);
    gUnk_03002490->unk7A = 0;
    if (sub_08068a2c(-8, 256) != 0)
        sub_0803e1b8(1, 96, gCurTaskIdx);
    v = gUnk_03002490;
    if (v->unk78 != 0)
        v->unk34 = 48;
    else
        v->unk34 = 32;
}
void sub_08068920(s32 i, u8 c)
{
    struct Task *t;

    t = &gUnk_03002790[i];
    t->unk18 = gCurTaskIdx;
    t->unk14 = c;
    sub_08006148(sub_080673ec, i);
}
void sub_08068950(s16 x, s16 y, s16 d)
{
    gUnk_02007D00[2] = x;
    gUnk_02007D00[3] = y;
    gUnk_02007D00[4] = d;
}
void sub_0806896c(void)
{
    struct Task *t;
    struct Task *u;

    if (gUnk_03001F30 != 0)
        return;
    t = gUnk_03002490;
    if (t->unk28 <= 0)
    {
        t->unk3C += t->unk2C;
        t->unk28 = 8;
        if (t->unk3C > 0x135)
            t->unk3C = 294;
        u = gUnk_03002490;
        if (u->unk3C <= 0x125)
            u->unk3C = 0x135;
    }
    else
    {
        t->unk28--;
    }
}
void sub_080689c8(s32 i, u8 d)
{
    struct Task *t;
    struct PlayerState *p;

    t = &gUnk_03002790[i];
    p = &gUnk_03002170[i];
    t->unk43 = d;
    t->unk7A = 0;
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
    t->unk72 = 0;
    sub_080062e0(i);
    sub_08068b88(i, 6, 0, 0);
    p->unk14 = 4;
}
s32 sub_08068a2c(s32 a, s32 b)
{
    struct Task *t;
    s32 r;

    r = sub_08009ee8(a, gCurTaskIdx);
    if (gUnk_03002490->unk88->unk0D != 0)
    {
        sub_08064eb8(0);
        sub_0800a008(0, -1, gUnk_03002490->unk88->unk00);
    }
    t = gUnk_03002490;
    if (t->unk78 == 0)
        t->unk82 = b;
    return r;
}
void sub_08068a8c(s32 i, u8 flag)
{
    struct Task *t;
    struct PlayerState *p;
    u8 a;
    u8 b;

    t = &gUnk_03002790[i];
    p = &gUnk_03002170[i];
    if (p->unk2C != -1)
        sub_080034f0(p->unk2C, p->unk2E);
    t->unk08 = 0;
    t->unk04 = 0;
    t->unk15 = 0;
    t->unk10 = 0;
    t->unk76 = 0;
    t->unk73 = 0;
    t->unk7C = 0;
    if (p->unk40 & 1)
    {
        gUnk_03002490->unk88->unk26 = 0;
        p->unk24 = 0;
        p->unk2B = 0;
        p->unk29 = 0;
        p->unk28 = 0;
        p->unk40 &= 0xFFFE;
        t->unk13 = 0;
    }
    p->unk42 &= 0xFFEF;
    t->unk60 = 0;
    t->unk5C = 0;
    t->unk58 = 0;
    t->unk54 = 0;
    t->unk68 = 0x80000000;
    t->unk64 = 0x80000000;
    a = p->unk04;
    b = p->unk45;
    if (flag != 0)
        sub_0803d1c4(i);
    else
        sub_0803d2d4(i);
    p->unk05 = a;
    p->unk04 = 16;
    gUnk_03002170[i].unk45 = b;
    p->unk16 = 255;
    sub_08067108();
}
