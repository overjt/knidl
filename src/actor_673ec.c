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

extern void sub_08002e98(u32 a, u32 b, u32 *c);
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
extern void sub_08068a2c(s32 a, s32 b);
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

void sub_080675e4(void);

void sub_080673ec(void)
{
    sub_08002e98(gUnk_03002490->unk14, 11, gUnk_0873E2F0);
}

void sub_08067408(void)
{
    struct Task *t;
    u8 *p;

    sub_08002e98(gUnk_03002490->unk15, 11, gUnk_0873E31C);
    sub_0803e2d4();
    p = gUnk_03002490->unk88;
    if ((*(u16 *)(p + 66) & 32) == 0)
        sub_0803e080();
    t = gUnk_03002490;
    p = t->unk88;
    if (gUnk_03002360 == *(s8 *)p)
        sub_08026264(t->unk48, t->unk4A);
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
    if (t->unk34 > 0)
    {
        t->unk34--;
        sub_0806896c();
    }
    else
    {
        sub_080062c4();
        sub_0806865c(gCurTaskIdx);
    }
}

void sub_08067550(void)
{
    struct Task *t;
    struct Task *u;
    u8 *p;

    gUnk_03002490->unk15 = 3;
    gUnk_03002490->unk43 = gUnk_03002790[gUnk_03002490->unk44].unk43;
    if (gUnk_03001F30 != 0)
    {
        sub_08006338(0x123B);
    }
    else
    {
        u = gUnk_03002490;
        p = u->unk88;
        if (p[6] == 1)
        {
            u->unk28 = 0x11C1;
            u->unk2C = 3;
        }
        else
        {
            u->unk28 = 0x133;
            u->unk2C = -8;
        }
        sub_08006338((s16)gUnk_03002490->unk28);
        sub_080684a4();
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
    t->unk48 = t->unk43 * *p + u->unk48;
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
    u8 *p;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_0803d494;
    t->unk15 = 4;
    sub_080062c4();
    sub_08068a2c(-8, 512);
    if (gUnk_03001F30 == 0)
        sub_080675e4();
    t = gUnk_03002490;
    t->unk28 = gUnk_03002790[t->unk44].unk28;
    t->unk34 = 0;
    if (t->unk78 == 0)
        t->unk34 = 1;
    if (gUnk_03002490->unk28 == 3)
        TaskYieldTrampoline(1);
    else
        TaskYieldTrampoline(28);
    if (gCurTaskIdx == gUnk_03002360)
    {
        if (gUnk_03001F30 == 0)
        {
            p = gUnk_03002490->unk88;
            if (p[6] == 1)
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
    t = gUnk_03002490;
    if (t->unk28 == 2)
        t->unk43 = -t->unk43;
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0x18000, 0x5A5A5A5A);
    sub_0800622c(0xFFFD8000, 0x2500, 0x30000);
    p = gUnk_03002490->unk88;
    if (gUnk_03001F30 == 0 && p[6] == 1)
    {
        sub_08006338(0x11C2);
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
        t = gUnk_03002490;
        t->unk2C = t->unk43;
        t->unk28 = 8;
        if (gUnk_03001F30 == 0)
            sub_08006364(0x133);
        else
            sub_08006338(0x123B);
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_0806896c();
            TaskYieldTrampoline(1);
            t = gUnk_03002490;
            t->unk6C++;
        } while ((s16)t->unk6C <= 29);
    }
    t = gUnk_03002490;
    t->unk34 = t->unk34 + 1;
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
    u8 *p;

    gUnk_03002490->unk15 = 5;
    t = gUnk_03002490;
    t->unk43 = gUnk_03002790[t->unk44].unk43;
    t = gUnk_03002490;
    t->unk34 = 0;
    t->unk42 = 7;
    sub_080062c4();
    t = gUnk_03002490;
    t->unk4C = (t->unk43 * 3) << 19;
    t->unk50 = 0;
    while (gUnk_03002790[t->unk44].unk3C == 34)
    {
        TaskYieldTrampoline(1);
        t = gUnk_03002490;
    }
    gUnk_03002490->unk42 = 12;
    p = gUnk_03002490->unk88;
    if (p[6] == 1)
    {
        sub_08006138();
        return;
    }
    while (1)
    {
        t = gUnk_03002490;
        sub_080061c0(gUnk_0873E348[t->unk34], 0x5A5A5A5A);
        t = gUnk_03002490;
        t->unk58 = gUnk_0873E388[t->unk34];
        TaskYieldTrampoline(2);
        t = gUnk_03002490;
        t->unk34++;
        if (t->unk34 > 14)
            t->unk34 = 0;
    }
}
