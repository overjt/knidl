/* game_code_and_rodata 0x0808AA68-0x0808CCE8 (issue #80, module M23 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0808AA68 0x0808CCE8 src/enemy_8aa68.c --newpb
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];

/* ROM tables */
extern s32 gUnk_087428E8[];
extern s32 gUnk_08742930[];
extern s8 gUnk_08732FF0[];
extern s8 gUnk_087336F0[];
extern s8 gUnk_087337F0[];
extern s8 gUnk_087339F0[];
extern struct AnimCmd gUnk_08742894[];
extern struct AnimCmd gUnk_087428A8[];
extern u32 gUnk_087343F0[];
extern u32 gUnk_087347F0[];
extern u32 gUnk_0873F500[];
extern u32 gUnk_087428C0[];
extern u32 gUnk_087428D8[];
extern u32 gUnk_087428E0[];
extern u32 gUnk_08742908[];
extern u32 gUnk_0874290C[];
extern u32 gUnk_08742910[];
extern u32 gUnk_08742918[];
extern u32 gUnk_08742920[];
extern u32 gUnk_08742928[];
extern u32 gUnk_08742940[];
extern u32 gUnk_08742948[];
extern u32 gUnk_08742978[];
extern u32 gUnk_087522B4[];
extern u32 gUnk_08752858[];
extern u32 gUnk_087528C8[];
extern u32 gUnk_08752BD4[];
extern u8 *gUnk_08742998[];
extern u8 gUnk_08732CF0[];
extern u8 gUnk_08742938[];

/* Externals */
extern s32 Div(s32 numerator, s32 denominator);
extern s32 sub_08021b18(u16 x, u16 y);
extern s32 sub_08063b38(void);
extern s32 sub_08063cd0(void);
extern s32 sub_08063d2c(void);
extern s32 sub_08063df4(void);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_080640fc(void);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern s8 sub_08064a38(void);
extern u16 sub_08064314(s32 prec);
extern u32 sub_08002ee8(u32 range);
extern u32 sub_08068e04(void);
extern u32 sub_080692fc(void);
extern u32 sub_08069b44(void);
extern u8 sub_08021c4c(s16 x, s16 y);
extern u8 sub_08064358(void);
extern void TaskDispatchTrampoline(void);
extern void TaskYieldTrampoline(u32 frames);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_080031b8(s32 id);
extern void sub_080059d8(void);
extern void sub_08005d9c(void);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, u32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_080062c4(void);
extern void sub_08006304(void);
extern void sub_08006338(s32 a);
extern void sub_080224b0(void);
extern void sub_0806395c(u8 v);
extern void sub_080639b4(u32 v);
extern void sub_08063e14(void);
extern void sub_08063fe0(void);
extern void sub_08063ff4(void);
extern void sub_08064680(s32 step, s32 limit, u16 dir);
extern void sub_0806523c(void);
extern void sub_080656b4(void);
extern void sub_0806a0f0(s32 a);
extern void sub_0806a344(void);
void sub_0808a84c(u8 *p, s32 b);

/* Defined below */
void sub_0808ab38(void);
void sub_0808ae48(void);
void sub_0808afd0(void);
void sub_0808b234(void);
void sub_0808b4d0(void);
void sub_0808b5b4(void);
void sub_0808b650(void);
void sub_0808b704(void);
void sub_0808b79c(void);
void sub_0808b830(void);
void sub_0808b8c4(void);
void sub_0808b99c(s32 a);
void sub_0808bb24(void);
void sub_0808bb5c(void);
void sub_0808be58(void);
void sub_0808c708(void);
s32 sub_0808c82c(void);
void sub_0808c8bc(void);
void sub_0808c934(void);
void sub_0808c980(void);
s32 sub_0808ca00(s32 a, s32 b);
s32 sub_0808cab8(s32 a, s32 b);
void sub_0808cc14(void);

void sub_0808aa68(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752858;
    gUnk_03002490->unk7A = 0;
    if (gUnk_03002490->unk73 == 5)
        sub_0808bb24();
    sub_080224b0();
    if (gUnk_03002490->unk7B == 3)
        sub_08002e98(gUnk_03002490->unk73, 6, gUnk_087428C0);
    sub_0808b5b4();
}

void sub_0808aad8(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808ab38;
    gUnk_03002490->unk7A = 0;
    gUnk_03002490->unk34 = 0;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087428D8);
}

void sub_0808ab14(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808ab38;
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087428D8);
}

void sub_0808ab38(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_087428E0);
    sub_08068e04();
    sub_08069b44();
    gUnk_03002490->unk7A = 0;
}

void sub_0808ab70(void)
{
    gUnk_03002490->unk15 = 0;
    while (gUnk_03002490->unk34 <= 0x6FF)
    {
        if (gUnk_03002490->unk28 == 0)
        {
            sub_08006338(4);
            TaskYieldTrampoline(1);
        }
        else
        {
            sub_08006338(8);
            TaskYieldTrampoline(4);
            sub_08006338(6);
            TaskYieldTrampoline(4);
            sub_08006338(4);
            TaskYieldTrampoline(4);
            sub_08006338(6);
            TaskYieldTrampoline(4);
        }
    }
    sub_0806395c(1);
    sub_08006138();
}

void sub_0808abf4(void)
{
    if (gUnk_03002490->unk14 != 0)
    {
        sub_08006148(sub_0808ab14, gCurTaskIdx);
        return;
    }
    gUnk_03002490->unk34++;
    if ((gUnk_03002490->unk34 & 7) == 0)
    {
        u8 c = sub_08064314(1) + 1;
        u16 d = (c & 0xF) >> 1;

        sub_08064680(gUnk_087428E8[gUnk_03002490->unk74],
                     gUnk_087428E8[gUnk_03002490->unk74 + 4], d);
        if (gUnk_030023B4 > 0)
            gUnk_03002490->unk43 = 1;
        if (gUnk_030023B4 < 0)
            gUnk_03002490->unk43 = -1;
        gUnk_03002490->unk28 = gUnk_030023B4 | gUnk_030023D4;
    }
    if (sub_08021c4c(gUnk_03002490->unk48, (u16)gUnk_03002490->unk4A - 8) != 1)
    {
        if (gUnk_03002490->unk58 < 0)
            gUnk_03002490->unk58 = -gUnk_03002490->unk58;
        gUnk_03002490->unk4A = (gUnk_03002490->unk4A & -16) + 8;
        gUnk_03002490->unk50 = (s16)gUnk_03002490->unk4A << 16;
    }
}

void sub_0808acdc(void)
{
    gUnk_03002490->unk15 = 1;
    while (1)
    {
        if (gUnk_03002490->unk28 == 0)
        {
            sub_08006338(4);
            TaskYieldTrampoline(1);
        }
        else
        {
            sub_08006338(6);
            TaskYieldTrampoline(6);
            sub_08006338(8);
            TaskYieldTrampoline(6);
        }
    }
}

void sub_0808ad20(void)
{
    gUnk_03002490->unk34++;
    if ((gUnk_03002490->unk34 & 7) == 0)
    {
        u8 c = sub_08064314(1) + 9;
        u16 d = (c & 0xF) >> 1;

        sub_08064680(gUnk_087428E8[gUnk_03002490->unk74],
                     gUnk_087428E8[gUnk_03002490->unk74 + 4], d);
        if (gUnk_03002490->unk54 > 0)
            gUnk_03002490->unk43 = 1;
        if (gUnk_03002490->unk54 < 0)
            gUnk_03002490->unk43 = -1;
        gUnk_03002490->unk28 = gUnk_030023B4 | gUnk_030023D4;
    }
    if (sub_08021c4c(gUnk_03002490->unk48, (u16)gUnk_03002490->unk4A - 8) != 1)
    {
        if (gUnk_03002490->unk58 < 0)
            gUnk_03002490->unk58 = -gUnk_03002490->unk58;
        gUnk_03002490->unk4A = (gUnk_03002490->unk4A & -16) + 8;
        gUnk_03002490->unk50 = (s16)gUnk_03002490->unk4A << 16;
    }
}

void sub_0808adec(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808ae48;
    gUnk_03002490->unk7A = 0;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08742908);
}

void sub_0808ae24(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_0808ae48;
    sub_08002e98(t->unk14, 1, gUnk_08742908);
}

void sub_0808ae48(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 1, gUnk_0874290C);
    sub_08068e04();
    sub_08069b44();
    gUnk_03002490->unk7A = 0;
}

void sub_0808ae80(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_080061c0(0x8000, 0x5A5A5A5A);
    gUnk_03002490->unk28 = sub_080640dc(gUnk_08742894);
    while (1)
    {
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk58 = 0x4000;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk58 = 0xFFFF8000;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk58 = 0xFFFFC000;
        TaskYieldTrampoline(10);
    }
}

void sub_0808aeec(void)
{
    if (gUnk_03002490->unk8C->unk2C != 0)
    {
        if (gUnk_03002490->unk28 == 0)
            gUnk_03002490->unk28 = sub_080640fc();
        gUnk_03002490->unk28--;
    }
    if (gUnk_03002490->unk73 == 3 || gUnk_03002490->unk73 == 4)
        sub_0808b4d0();
}

void sub_0808af34(void)
{
    s16 *p;

    gUnk_03002490->unk04 = (u32)sub_0808afd0;
    gUnk_03002490->unk7A = 0;
    sub_08063e14();
    gUnk_03002490->unk34 = gUnk_03002490->unk43;
    p = &gUnk_03002490->unk4A;
    if (*p < gUnk_03002790[sub_08063b38()].unk4A)
        sub_0806395c(1);
    else
        sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08742910);
}

void sub_0808afac(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808afd0;
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08742910);
}

void sub_0808afd0(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_08742918);
    sub_08068e04();
    sub_08069b44();
    gUnk_03002490->unk7A = 0;
}

void sub_0808b008(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk43 = gUnk_03002490->unk34;
    gUnk_03002490->unk58 = 0xFFFF8000;
    gUnk_03002490->unk28 = sub_080640dc(gUnk_087428A8);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk54 = 0xFFFF8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x4000;
        TaskYieldTrampoline(15);
        gUnk_03002490->unk54 = 0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0xFFFFC000;
        TaskYieldTrampoline(15);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_0806395c(1);
    sub_08006138();
}

void sub_0808b094(void)
{
    if (gUnk_03002490->unk14 == 0)
    {
        if ((u8)sub_08021c4c(gUnk_03002490->unk48,
                             (u16)gUnk_03002490->unk4A - 8) != 0)
            goto anim;
        gUnk_03002490->unk4A = (gUnk_03002490->unk4A & -16) + 8;
        gUnk_03002490->unk50 = (s16)gUnk_03002490->unk4A << 16;
        sub_0806395c(1);
    }
    sub_08006148(sub_0808afac, gCurTaskIdx);
    return;
anim:
    if (gUnk_03002490->unk8C->unk2C != 0)
    {
        if (gUnk_03002490->unk28 == 0)
            gUnk_03002490->unk28 = sub_080640fc();
        gUnk_03002490->unk28--;
    }
}

void sub_0808b120(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk43 = gUnk_03002490->unk34;
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk58 = 0x8000;
    sub_08006338(12);
    TaskYieldTrampoline(6);
    sub_08006338(8);
    TaskYieldTrampoline(4);
    sub_08006338(4);
    TaskYieldTrampoline(3);
    sub_08006338(10);
    TaskYieldTrampoline(3);
    sub_08006338(22);
    TaskYieldTrampoline(3);
    sub_08006338(19);
    TaskYieldTrampoline(119);
    sub_080062c4();
    sub_08006338(13);
    TaskYieldTrampoline(6);
    sub_0806395c(0);
    sub_08006138();
}

void sub_0808b1a8(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_0808afac, gCurTaskIdx);
}

void sub_0808b1d0(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808b234;
    gUnk_03002490->unk2C = 1;
    gUnk_03002490->unk7A = 0;
    sub_08063e14();
    gUnk_03002490->unk34 = 1;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08742920);
}

void sub_0808b210(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808b234;
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08742920);
}

void sub_0808b234(void)
{
    if (gUnk_03002490->unk2C != 0)
    {
        if ((u8)sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 2, gUnk_08742928);
    }
    else
    {
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_08742928);
    }
    sub_08068e04();
    sub_08069b44();
    gUnk_03002490->unk7A = 0;
}

void sub_0808b28c(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk30 = 192;
    sub_080061c0(0x8000, 0x5A5A5A5A);
    gUnk_03002490->unk60 = 0xFFFFCD00;
    gUnk_03002490->unk68 = 0x30000;
    while (1)
    {
        sub_08006338(12);
        TaskYieldTrampoline(6);
        sub_08006338(10);
        TaskYieldTrampoline(5);
        sub_08006338(8);
        TaskYieldTrampoline(5);
        sub_08006338(6);
        TaskYieldTrampoline(6);
        sub_08006338(4);
        TaskYieldTrampoline(10);
    }
}

void sub_0808b2fc(void)
{
    if (gUnk_03002490->unk58 < 0
        && (u8)sub_08021c4c(gUnk_03002490->unk48,
                            (u16)gUnk_03002490->unk4A - 8) == 0)
    {
        gUnk_03002490->unk4A = (gUnk_03002490->unk4A & -16) + 8;
        gUnk_03002490->unk50 = (s16)gUnk_03002490->unk4A << 16;
        sub_0806395c(0);
        sub_08006148(sub_0808ae24, gCurTaskIdx);
    }
    else
    {
        sub_0808b4d0();
    }
}

void sub_0808b368(void)
{
    gUnk_03002490->unk15 = 1;
    if (gUnk_03002490->unk73 == 3)
        gUnk_03002490->unk2C = 0;
    else
        gUnk_03002490->unk2C = 1;
    sub_08063e14();
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk34 = 90;
    sub_080061c0(gUnk_08742930[gUnk_03002490->unk74], 0x5A5A5A5A);
    sub_0800622c(0xFFFC0000, 0x1500, 0x30000);
    while (gUnk_03002490->unk58 < 0)
    {
        sub_08006338(8);
        TaskYieldTrampoline(3);
        sub_08006338(10);
        TaskYieldTrampoline(3);
        sub_08006338(13);
        TaskYieldTrampoline(2);
    }
    sub_08006338(10);
    TaskYieldTrampoline(3);
    sub_08006338(8);
    TaskYieldTrampoline(3);
    sub_08006338(22);
    sub_08006138();
}

void sub_0808b468(void)
{
    if (gUnk_03002490->unk34 > 0 && gUnk_03002490->unk58 > 0
        && (u8)sub_08021c4c(gUnk_03002490->unk48,
                            (u16)gUnk_03002490->unk4A
                                + ((s8 *)gUnk_03002490->unk8C->unk50)[2]) != 0)
    {
        gUnk_03002490->unk58 >>= 2;
        sub_0806395c(0);
        sub_08006148(sub_0808b210, gCurTaskIdx);
    }
}

void sub_0808b4d0(void)
{
    if (--gUnk_03002490->unk30 <= 0)
    {
        gUnk_03002490->unk30 = 192;
        sub_08063ff4();
    }
    if ((u8)sub_08021c4c(gUnk_03002490->unk48, gUnk_03002490->unk4A) == 0)
    {
        sub_0806395c(1);
        sub_08006148(sub_0808b210, gCurTaskIdx);
        return;
    }
    if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) > 64)
        return;
    if (--gUnk_03002490->unk34 > 0)
        return;
    if (sub_08002ee8(3) == 0)
    {
        gUnk_03002490->unk34 = 16;
        return;
    }
    if (sub_08063cd0() < 0)
    {
        if (-sub_08063cd0() <= 32)
            goto one;
        goto zero;
    }
    if (sub_08063cd0() > 32)
        goto zero;
one:
    gUnk_03002490->unk74 = 1;
    goto done;
zero:
    gUnk_03002490->unk74 = 0;
done:
    sub_0806395c(1);
    sub_08006148(sub_0808b210, gCurTaskIdx);
}

void sub_0808b5b4(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808b8c4;
    sub_080062c4();
    if (gUnk_03002490->unk7A == 0)
    {
        sub_08063e14();
        sub_08006338(20);
        sub_0800622c(0, 0x2500, 0x30000);
        sub_08006138();
    }
    if (sub_08002ee8(8) == 0)
        gUnk_03002490->unk43 = -gUnk_03002490->unk43;
    switch (gUnk_08742938[sub_08002ee8(8)])
    {
    case 0:
        sub_0808b650();
        break;
    case 1:
        sub_0808b704();
        break;
    case 2:
        sub_0808b79c();
        break;
    case 3:
        sub_0808b830();
        break;
    }
}

void sub_0808b650(void)
{
    gUnk_03002490->unk7A = 0;
    sub_0800622c(0xFFFC0000, 0x4000, 0x30000);
    sub_08006338(22);
    TaskYieldTrampoline(3);
    sub_08006338(21);
    TaskYieldTrampoline(3);
    sub_0808b99c(2);
    sub_08006338(22);
    TaskYieldTrampoline(3);
    sub_08006338(21);
    TaskYieldTrampoline(3);
    sub_0808b99c(3);
    sub_08006338(20);
    TaskYieldTrampoline(4);
    sub_0808b99c(2);
    sub_08006338(21);
    TaskYieldTrampoline(4);
    sub_0808b99c(3);
    sub_08006338(18);
    TaskYieldTrampoline(3);
    sub_08006338(8);
    TaskYieldTrampoline(3);
    sub_08006338(9);
    TaskYieldTrampoline(3);
    sub_08006338(19);
    sub_08006138();
}

void sub_0808b704(void)
{
    gUnk_03002490->unk7A = 0;
    sub_0800622c(0xFFFE0000, 0x4000, 0x30000);
    sub_0808b99c(0);
    sub_08006338(22);
    TaskYieldTrampoline(4);
    sub_0808b99c(1);
    sub_08006338(18);
    TaskYieldTrampoline(4);
    sub_08006338(9);
    TaskYieldTrampoline(4);
    sub_08006338(5);
    TaskYieldTrampoline(4);
    sub_0808b99c(0);
    sub_08006338(4);
    TaskYieldTrampoline(4);
    sub_08006338(8);
    TaskYieldTrampoline(4);
    sub_08006338(19);
    TaskYieldTrampoline(4);
    sub_08006338(23);
    sub_08006138();
}

void sub_0808b79c(void)
{
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0x10000, 0x5A5A5A5A);
    sub_0800622c(0xFFFE0000, 0x4000, 0x30000);
    sub_08006338(18);
    TaskYieldTrampoline(4);
    sub_08006338(16);
    TaskYieldTrampoline(4);
    sub_08006338(6);
    TaskYieldTrampoline(4);
    sub_08006338(4);
    TaskYieldTrampoline(4);
    sub_08006338(15);
    TaskYieldTrampoline(4);
    sub_08006338(17);
    TaskYieldTrampoline(4);
    sub_08006338(7);
    TaskYieldTrampoline(4);
    sub_08006338(9);
    sub_08006138();
}

void sub_0808b830(void)
{
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0xFFFF0000, 0x5A5A5A5A);
    sub_0800622c(0xFFFE0000, 0x4000, 0x30000);
    sub_08006338(9);
    TaskYieldTrampoline(4);
    sub_08006338(7);
    TaskYieldTrampoline(4);
    sub_08006338(17);
    TaskYieldTrampoline(4);
    sub_08006338(15);
    TaskYieldTrampoline(4);
    sub_08006338(4);
    TaskYieldTrampoline(4);
    sub_08006338(6);
    TaskYieldTrampoline(4);
    sub_08006338(16);
    TaskYieldTrampoline(4);
    sub_08006338(18);
    sub_08006138();
}

void sub_0808b8c4(void)
{
    if ((u8)sub_08021c4c(gUnk_03002490->unk48,
                         (u16)gUnk_03002490->unk4A
                             + ((s8 *)gUnk_03002490->unk8C->unk50)[2]) != 0)
    {
        switch (gUnk_03002490->unk73)
        {
        case 0:
            sub_080062c4();
            sub_08006148(sub_0808aad8, gCurTaskIdx);
            break;
        case 1:
            sub_080062c4();
            sub_08006148(sub_0808adec, gCurTaskIdx);
            break;
        case 2:
            sub_080062c4();
            sub_08006148(sub_0808af34, gCurTaskIdx);
            break;
        case 3:
        case 4:
            gUnk_03002490->unk58 >>= 2;
            gUnk_03002490->unk34 = 90;
            sub_0806395c(0);
            gUnk_03002490->unk04 = (u32)sub_0808b234;
            sub_08006148(sub_0808b28c, gCurTaskIdx);
            break;
        }
    }
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

void sub_0808b99c(s32 a)
{
    gUnk_03002490->unk46 = sub_08064d34(218, 1);
    gUnk_03002790[gUnk_03002490->unk46].unk73 = a;
}

void sub_0808b9d0(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 12;
    gUnk_03002490->unk38 = gUnk_087528C8;
    gUnk_03002490->unk43 = sub_08064a38();
    t = gUnk_03002490;
    switch (t->unk73)
    {
    case 0:
        t->unk4C = (t->unk48 - t->unk43 * 12) << 16;
        sub_080061c0(0xFFFE0000, 0x5A5A5A5A);
        gUnk_03002490->unk58 = 0xFFFE0000;
        gUnk_03002490->unk60 = 0x8000;
        break;
    case 1:
        t->unk4C = (t->unk48 + t->unk43 * 12) << 16;
        sub_080061c0(0x20000, 0x5A5A5A5A);
        gUnk_03002490->unk58 = 0xFFFE0000;
        gUnk_03002490->unk60 = 0x8000;
        break;
    case 2:
        t->unk4C = (t->unk48 - t->unk43 * 8) << 16;
        t->unk50 = (t->unk4A - 12) << 16;
        sub_080061c0(0xFFFE0000, 0x5A5A5A5A);
        gUnk_03002490->unk58 = 0xFFFD8000;
        gUnk_03002490->unk60 = 0x8000;
        break;
    case 3:
        t->unk4C = (t->unk48 + t->unk43 * 8) << 16;
        t->unk50 = (t->unk4A - 12) << 16;
        sub_080061c0(0x20000, 0x5A5A5A5A);
        gUnk_03002490->unk58 = 0xFFFD8000;
        gUnk_03002490->unk60 = 0x8000;
        break;
    }
    sub_08006338(0);
    TaskYieldTrampoline(2);
    sub_08006338(1);
    TaskYieldTrampoline(4);
    TaskDispatchTrampoline();
}

void sub_0808bb24(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808bb5c;
    sub_080639b4((u32)gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_08063e14();
    sub_08006338(4);
    sub_08006138();
}

void sub_0808bb5c(void)
{
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

s32 sub_0808bb70(void)
{
    if (gUnk_03002490->unk73 != 5)
    {
        if ((u8)sub_08021c4c(gUnk_03002490->unk48, gUnk_03002490->unk4A) == 0)
        {
            sub_08006148(sub_0808b5b4, gCurTaskIdx);
            return 1;
        }
        switch (gUnk_03002490->unk73)
        {
        case 0:
            gUnk_03002490->unk58 = -gUnk_03002490->unk58;
        case 1:
            return 0;
        case 2:
            sub_0806395c(0);
            sub_08006148(sub_0808afac, gCurTaskIdx);
            return 1;
        case 3:
        case 4:
            gUnk_03002490->unk58 = 0;
            return 0;
        }
    }
}

s32 sub_0808bc18(void)
{
    if (gUnk_03002490->unk73 != 5)
    {
        if ((u8)sub_08021c4c(gUnk_03002490->unk48, gUnk_03002490->unk4A) == 0)
        {
            sub_08006148(sub_0808b5b4, gCurTaskIdx);
            return 1;
        }
    }
}

s32 sub_0808bc60(void)
{
    if (gUnk_03002490->unk73 != 5)
    {
        if ((u8)sub_08021c4c(gUnk_03002490->unk48, gUnk_03002490->unk4A) == 0)
            goto stop;
        switch (gUnk_03002490->unk73)
        {
        case 0:
            gUnk_03002490->unk54 = -gUnk_03002490->unk54;
        case 2:
        zero:
            return 0;
        case 3:
            gUnk_03002490->unk30 = 192;
        case 1:
        stop:
            sub_08063ff4();
            goto zero;
        case 4:
            if (gUnk_03002490->unk14 == 0)
                gUnk_03002490->unk30 = 192;
            sub_08063ff4();
            return 0;
        }
    }
}

s32 sub_0808bd04(void)
{
    u8 r;

    if (gUnk_03002490->unk73 != 5)
    {
        r = sub_08021c4c(gUnk_03002490->unk48, gUnk_03002490->unk4A);
        if (r == 0)
        {
            gUnk_03002490->unk58 = r;
            return 0;
        }
        switch (gUnk_03002490->unk73)
        {
        case 0:
            gUnk_03002490->unk58 = -gUnk_03002490->unk58;
        case 1:
            return 0;
        case 2:
            sub_0806395c(1);
            sub_08006148(sub_0808afac, gCurTaskIdx);
            return 1;
        case 3:
        case 4:
            sub_08006148(sub_0808adec, gCurTaskIdx);
            return 1;
        }
    }
}

void sub_0808bdb4(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752BD4;
    sub_08002e98(gUnk_03002490->unk73, 2, gUnk_08742940);
}

void sub_0808bdf4(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808be58;
    gUnk_03002490->unk28 = 120;
    if (sub_0808c82c() != 0)
        sub_0806395c(3);
    else
        sub_0806395c(1);
    sub_08002e98(gUnk_03002490->unk14, 12, gUnk_08742948);
}

void sub_0808be3c(void)
{
    sub_08002e98(gUnk_03002490->unk14, 12, gUnk_08742948);
}

void sub_0808be58(void)
{
    if (gUnk_03002490->unk18 != 0)
    {
        if ((u8)sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 8, gUnk_08742978);
    }
    else
    {
        sub_08002e98(gUnk_03002490->unk15, 8, gUnk_08742978);
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_0808bea0(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk18 = 1;
    gUnk_03002490->unk7A = 1;
    sub_080062c4();
    sub_08063e14();
    if (gUnk_03002490->unk14 == 2)
        gUnk_03002490->unk43 = -gUnk_03002490->unk43;
    sub_0806395c(0);
    sub_080061c0(0x10000, 0x5A5A5A5A);
    gUnk_03002490->unk2C = 16;
    while (1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(8);
        sub_08006338(7);
        TaskYieldTrampoline(6);
        sub_08006338(4);
        TaskYieldTrampoline(8);
        sub_08006338(5);
        TaskYieldTrampoline(6);
    }
}

void sub_0808bf1c(void)
{
    if (--gUnk_03002490->unk28 == 0)
    {
        sub_0806395c(11);
        sub_08006148(sub_0808be3c, gCurTaskIdx);
        return;
    }
    if (--gUnk_03002490->unk2C != 0)
        return;
    if (sub_08063cd0() < 0)
    {
        if (-sub_08063cd0() <= 63)
            goto near;
        goto far;
    }
    if (sub_08063cd0() > 63)
        goto far;
near:
    sub_08063e14();
    gUnk_03002490->unk43 = -gUnk_03002490->unk43;
    sub_08006304();
    sub_080061c0(0x10000, 0x5A5A5A5A);
    goto done;
far:
    sub_08063e14();
    sub_08006304();
    sub_080061c0(0x10000, 0x5A5A5A5A);
done:
    gUnk_03002490->unk2C = 16;
}

void sub_0808bfc4(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk18 = 1;
    gUnk_03002490->unk7A = 0;
    sub_080062c4();
    do
    {
        sub_08006338(8);
        TaskYieldTrampoline(29);
        sub_0808c8bc();
    } while (gUnk_03002490->unk14 == 3);
    sub_08006138();
}

void sub_0808c004(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_0808be3c, gCurTaskIdx);
}

void sub_0808c02c(void)
{
    gUnk_03002490->unk15 = 2;
    gUnk_03002490->unk18 = 1;
    gUnk_03002490->unk7A = 0;
    sub_080062c4();
    do
    {
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk58 = 0xFFFF0000;
            sub_08006338(8);
            TaskYieldTrampoline(3);
            sub_08006338(9);
            TaskYieldTrampoline(3);
            sub_08006338(10);
            TaskYieldTrampoline(4);
            gUnk_03002490->unk58 = 0xFFFF8000;
            sub_08006338(11);
            TaskYieldTrampoline(3);
            sub_08006338(12);
            TaskYieldTrampoline(3);
            gUnk_03002490->unk58 = 0xFFFFC000;
            sub_08006338(11);
            TaskYieldTrampoline(3);
            sub_08006338(10);
            TaskYieldTrampoline(3);
            sub_080062c4();
            sub_08006338(9);
            TaskYieldTrampoline(6);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 2);
        sub_0808c8bc();
    } while (gUnk_03002490->unk14 == 4);
    sub_08006138();
}

void sub_0808c0fc(void)
{
    sub_0806395c(4);
    gUnk_03002490->unk15 = 2;
    gUnk_03002490->unk18 = 1;
    gUnk_03002490->unk7A = 0;
    sub_080062c4();
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk58 = 0xFFFE0000;
        sub_08006338(8);
        TaskYieldTrampoline(2);
        sub_08006338(9);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = 0xFFFF0000;
        sub_08006338(10);
        TaskYieldTrampoline(2);
        sub_08006338(11);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = 0xFFFF8000;
        sub_08006338(12);
        TaskYieldTrampoline(2);
        sub_08006338(11);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = 0xFFFFC000;
        sub_08006338(10);
        TaskYieldTrampoline(2);
        sub_08006338(9);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_0806395c(3);
    sub_08006138();
}

void sub_0808c1d0(void)
{
    if (gUnk_03002490->unk14 != 4)
        sub_08006148(sub_0808be3c, gCurTaskIdx);
    if (gUnk_03002490->unk43 == 1)
    {
        if (sub_0808ca00(12, ((s8 *)gUnk_03002490->unk8C->unk50)[3]) < 0)
        {
            sub_0806395c(8);
            sub_08006148(sub_0808be3c, gCurTaskIdx);
        }
    }
    else if (sub_0808cab8(12, ((s8 *)gUnk_03002490->unk8C->unk50)[3]) < 0)
    {
        sub_0806395c(8);
        sub_08006148(sub_0808be3c, gCurTaskIdx);
    }
}

void sub_0808c260(void)
{
    gUnk_03002490->unk15 = 3;
    gUnk_03002490->unk18 = 1;
    gUnk_03002490->unk7A = 0;
    sub_080062c4();
    do
    {
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk58 = 0x10000;
            sub_08006338(16);
            TaskYieldTrampoline(4);
            sub_08006338(13);
            TaskYieldTrampoline(6);
            gUnk_03002490->unk58 = 0x8000;
            sub_08006338(16);
            TaskYieldTrampoline(4);
            sub_08006338(13);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk58 = 0x4000;
            sub_08006338(13);
            TaskYieldTrampoline(4);
            sub_08006338(16);
            TaskYieldTrampoline(2);
            sub_080062c4();
            sub_08006338(16);
            TaskYieldTrampoline(2);
            sub_08006338(13);
            TaskYieldTrampoline(4);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 2);
        sub_0808c8bc();
    } while (gUnk_03002490->unk14 == 5);
    sub_08006138();
}

void sub_0808c32c(void)
{
    sub_0806395c(5);
    gUnk_03002490->unk15 = 3;
    gUnk_03002490->unk18 = 0;
    gUnk_03002490->unk7A = 0;
    sub_080062c4();
    gUnk_03002490->unk43 = -gUnk_03002490->unk43;
    gUnk_03002490->unk58 = 0x20000;
    sub_08006338(16);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk18 = 1;
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk58 = 0x20000;
        sub_08006338(16);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 0x10000;
        sub_08006338(13);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 0x8000;
        sub_08006338(16);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 0x4000;
        sub_08006338(13);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_0806395c(3);
    sub_08006138();
}

void sub_0808c3e8(void)
{
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_0808be3c, gCurTaskIdx);
    if (gUnk_03002490->unk43 == 1)
    {
        if (sub_0808ca00(12, ((s8 *)gUnk_03002490->unk8C->unk50)[3]) < 0)
        {
            sub_0806395c(9);
            sub_08006148(sub_0808be3c, gCurTaskIdx);
        }
    }
    else if (sub_0808cab8(12, ((s8 *)gUnk_03002490->unk8C->unk50)[3]) < 0)
    {
        sub_0806395c(9);
        sub_08006148(sub_0808be3c, gCurTaskIdx);
    }
}

void sub_0808c478(void)
{
    gUnk_03002490->unk15 = 4;
    gUnk_03002490->unk18 = 0;
    gUnk_03002490->unk7A = 0;
    sub_080062c4();
    sub_080061c0(0x10000, 0x5A5A5A5A);
    sub_08006338(4);
    TaskYieldTrampoline(10);
    sub_0806395c(1);
    sub_08006138();
}

void sub_0808c4bc(void)
{
    if (gUnk_03002490->unk14 != 8)
        sub_08006148(sub_0808be3c, gCurTaskIdx);
}

void sub_0808c4e4(void)
{
    gUnk_03002490->unk15 = 5;
    gUnk_03002490->unk18 = 1;
    gUnk_03002490->unk7A = 0;
    sub_080062c4();
    sub_08006338(4);
    gUnk_03002490->unk43 = -gUnk_03002490->unk43;
    sub_080061c0(0x8000, 0x5A5A5A5A);
    gUnk_03002490->unk60 = 0x1500;
    gUnk_03002490->unk68 = 0x30000;
    sub_08006138();
}

void sub_0808c538(void)
{
}

void sub_0808c53c(void)
{
    gUnk_03002490->unk15 = 6;
    gUnk_03002490->unk18 = 0;
    gUnk_03002490->unk7A = 0;
    sub_080062c4();
    if (sub_08063df4() == gUnk_03002490->unk43)
    {
        sub_0806395c(3);
        sub_08006138();
    }
    sub_08006338(8);
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080061c0(0xFFFF0000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080061c0(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 7);
    gUnk_03002490->unk18 = 1;
    gUnk_03002490->unk43 = -gUnk_03002490->unk43;
    sub_0808c980();
    while (1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(4);
        sub_08006338(7);
        TaskYieldTrampoline(4);
    }
}

void sub_0808c5e8(void)
{
    if (gUnk_03002490->unk14 != 10)
        sub_08006148(sub_0808be3c, gCurTaskIdx);
}

void sub_0808c610(void)
{
    gUnk_03002490->unk15 = 7;
    gUnk_03002490->unk18 = 0;
    gUnk_03002490->unk7A = 1;
    sub_080062c4();
    sub_08063e14();
    sub_08006338(14);
    TaskYieldTrampoline(16);
    sub_080031b8(233);
    sub_0808c934();
    sub_08006338(15);
    TaskYieldTrampoline(12);
    sub_08006338(14);
    TaskYieldTrampoline(8);
    sub_08006338(7);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk28 = 120;
    sub_0806395c(1);
    sub_08006138();
}

void sub_0808c684(void)
{
    if (gUnk_03002490->unk14 != 11)
        sub_08006148(sub_0808be3c, gCurTaskIdx);
}

void sub_0808c6ac(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808c708;
    sub_080639b4((u32)gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_08063e14();
    while (1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(8);
        sub_08006338(7);
        TaskYieldTrampoline(6);
        sub_08006338(4);
        TaskYieldTrampoline(8);
        sub_08006338(5);
        TaskYieldTrampoline(6);
    }
}

void sub_0808c708(void)
{
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

s32 sub_0808c71c(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        sub_0806395c(0);
        sub_08006148(sub_0808be3c, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808c74c(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        sub_0806395c(7);
        sub_08006148(sub_0808be3c, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808c77c(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        sub_0806a0f0(-2);
        return 1;
    }
}

s32 sub_0808c79c(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        switch (gUnk_03002490->unk14)
        {
        case 0:
            sub_0806395c(6);
            sub_08006148(sub_0808be3c, gCurTaskIdx);
            return 1;
        case 9:
        case 10:
            sub_0806395c(3);
            sub_08006148(sub_0808be3c, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_0808c7ec(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        if (gUnk_03002490->unk14 != 4)
        {
            gUnk_03002490->unk58 = 0;
            return 0;
        }
        sub_0806395c(10);
        sub_08006148(sub_0808be3c, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808c82c(void)
{
    s32 r;

    r = sub_0808ca00(12, 0);
    if (r >= 0)
    {
        gUnk_03002490->unk48 = (u16)gUnk_03002490->unk48
            + (r - ((s8 *)gUnk_03002490->unk8C->unk50)[5]);
        gUnk_03002490->unk4C = (s16)gUnk_03002490->unk48 << 16;
        gUnk_03002490->unk43 = 1;
        return 1;
    }
    r = sub_0808cab8(12, 0);
    if (r >= 0)
    {
        gUnk_03002490->unk48 = (u16)gUnk_03002490->unk48
            - (((s8 *)gUnk_03002490->unk8C->unk50)[4] + r);
        gUnk_03002490->unk4C = (s16)gUnk_03002490->unk48 << 16;
        gUnk_03002490->unk43 = -1;
        return 1;
    }
    return 0;
}

void sub_0808c8bc(void)
{
    sub_0808a84c(gUnk_08742998[(u16)sub_08064314(1)], 100);
    switch (gUnk_030023D4)
    {
    case 0:
        sub_0806395c(4);
        break;
    case 1:
        sub_0806395c(5);
        break;
    case 2:
        sub_0806395c(9);
        break;
    case 3:
        sub_0806395c(10);
        break;
    case 4:
        sub_0806395c(3);
        break;
    }
}

void sub_0808c934(void)
{
    struct ActorSpawn sp;
    u8 zero;

    sp.unk00 = 34;
    sp.unk04 = 137;
    sp.unk08 = gUnk_03002490->unk73;
    sp.unk09 = gUnk_03002490->unk74;
    zero = 0;
    sp.unk0C = 6;
    sp.unk0E = -6;
    sp.unk0A = zero;
    gUnk_03002490->unk46 = sub_08064b5c(&sp, 0);
}

void sub_0808c980(void)
{
    s32 a;
    s32 d;

    sub_080061c0(0x20000, 0x5A5A5A5A);
    a = (sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) >> 1;
    d = -sub_08063d2c() + 32;
    if (d < 0)
        d = -d;
    if (a == 0)
        a = 1;
    d = Div(d << 16, a);
    if ((u8)sub_08064358() == 2)
        d = -d;
    d -= a << 16;
    if (d > 0)
        d = 0;
    if (d < (s32)0xFFFD0000)
        d = 0xFFFD0000;
    sub_0800622c(d, 0x2000, 0x30000);
}

s32 sub_0808ca00(s32 a, s32 b)
{
    struct Task *t;
    s16 *pa;
    s32 x;
    s32 p;
    s32 q;
    s32 i;
    s32 v;
    s32 w;
    s32 u;

    t = gUnk_03002490;
    pa = &t->unk48;
    x = (s8)a;
    p = (u16)*pa + x;
    q = (u16)t->unk4A + (s8)b;
    i = sub_08021b18(p, q);
    if (gUnk_087339F0[i] == 0 || gUnk_087336F0[i] != 0
     || gUnk_087337F0[i] != 0 || gUnk_08732CF0[i] != 0
     || gUnk_08732FF0[i] != 0)
        goto minus1;
    u = ((p << 16) >> 16) & 15;
    w = (u << 4) | u;
    v = (s8)((u8 *)gUnk_087347F0[i])[w];
    if (v < 0)
        v = -v;
    return x - v;
minus1:
    return -1;
}

s32 sub_0808cab8(s32 a, s32 b)
{
    struct Task *t;
    s16 *pa;
    s32 x;
    s32 p;
    s32 q;
    s32 i;
    s32 v;
    s32 w;
    s32 u;

    t = gUnk_03002490;
    pa = &t->unk48;
    x = (s8)a;
    p = (u16)*pa - x;
    q = (u16)t->unk4A + (s8)b;
    i = sub_08021b18(p, q);
    if (gUnk_087339F0[i] == 0 || gUnk_087336F0[i] != 0
     || gUnk_087337F0[i] != 0 || gUnk_08732CF0[i] != 0
     || gUnk_08732FF0[i] != 0)
        goto minus1;
    u = ((p << 16) >> 16) & 15;
    w = (u << 4) | u;
    v = (s8)((u8 *)gUnk_087343F0[i])[w];
    if (v < 0)
        v = -v;
    return x - v;
minus1:
    return -1;
}

void sub_0808cb70(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 12;
    t = gUnk_03002490;
    t->unk38 = gUnk_087522B4;
    t->unk40 = (t->unk40 & 0xFFF) | 0xF000;
    t->unk04 = (u32)sub_0808cc14;
    t->unk30 = 0;
    t->unk34 = 0;
    t->unk6C = 0;
    do
    {
        sub_08006338(0);
        TaskYieldTrampoline(1);
        sub_08006338(-1);
        TaskYieldTrampoline(1);
        sub_08006338(1);
        TaskYieldTrampoline(1);
        sub_08006338(-1);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 59);
    sub_08063fe0();
}

void sub_0808cc14(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    if (gUnk_03002490->unk34 <= 0)
    {
        sub_080031b8(185);
        gUnk_03002490->unk34 = 5;
    }
    t = gUnk_03002490;
    t->unk34--;
    u = &gUnk_03002790[t->unk44];
    if (u->unk7C != 0 || u->unk7A == 0)
    {
        sub_08006148(sub_0806a344, gCurTaskIdx);
        return;
    }
    n = t->unk30;
    if (n == 0)
    {
        gUnk_03002490->unk46 = sub_08064d34(215, 1);
        gUnk_03002790[gUnk_03002490->unk46].unk14 = 0;
        gUnk_03002490->unk30++;
    }
    else if (n == 10)
    {
        gUnk_03002490->unk46 = sub_08064d34(215, 1);
        gUnk_03002790[gUnk_03002490->unk46].unk14 = 1;
        gUnk_03002490->unk30++;
    }
    else if (n > 19)
        t->unk30 = 0;
    else
        t->unk30 = n + 1;
    sub_08068e04();
    sub_08069b44();
}
