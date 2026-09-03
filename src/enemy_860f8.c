/* game_code_and_rodata 0x080860F8-0x08088000 (issue #80, module M23 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080860F8 0x08088000 src/enemy_860f8.c --newpb
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];
extern u8 gUnk_03005550[];

/* ROM tables */
extern s16 gUnk_08742150[];
extern s32 gUnk_08742088[];
extern s32 gUnk_08742090[];
extern s32 gUnk_08742098[];
extern s32 gUnk_087420A8[];
extern s32 gUnk_087420AC[];
extern s32 gUnk_087420F4[];
extern s32 gUnk_0874210C[];
extern s32 gUnk_087425B8[];
extern s32 gUnk_087425C0[];
extern s32 gUnk_087425C8[];
extern s32 gUnk_087425D8[];
extern s32 gUnk_087425DC[];
extern s32 gUnk_08742600[];
extern s32 gUnk_08742614[];
extern struct AnimCmd gUnk_087420C0[];
extern struct AnimCmd gUnk_087420D4[];
extern struct AnimCmd gUnk_08742144[];
extern struct AnimCmd gUnk_08742598[];
extern struct AnimCmd gUnk_087425A4[];
extern struct AnimCmd gUnk_08742634[];
extern u32 gUnk_0873F500[];
extern u32 gUnk_08742080[];
extern u32 gUnk_08742084[];
extern u32 gUnk_087420A0[];
extern u32 gUnk_087420A4[];
extern u32 gUnk_087420BC[];
extern u32 gUnk_087420E8[];
extern u32 gUnk_087420F0[];
extern u32 gUnk_08742100[];
extern u32 gUnk_08742104[];
extern u32 gUnk_08742108[];
extern u32 gUnk_0874212C[];
extern u32 gUnk_08742138[];
extern u32 gUnk_08742570[];
extern u32 gUnk_087425B0[];
extern u32 gUnk_087425B4[];
extern u32 gUnk_087425D0[];
extern u32 gUnk_087425D4[];
extern u32 gUnk_087425EC[];
extern u32 gUnk_087425F0[];
extern u32 gUnk_087425F8[];
extern u32 gUnk_087425FC[];
extern u32 gUnk_0874260C[];
extern u32 gUnk_08742610[];
extern u32 gUnk_0874263C[];
extern u32 gUnk_08742648[];
extern u32 gUnk_08742654[];
extern u32 gUnk_08742660[];
extern u32 gUnk_0874266C[];
extern u32 gUnk_08752560[];

/* Externals */
extern s32 sub_08063b38(void);
extern s32 sub_08063cd0(void);
extern s32 sub_08063d2c(void);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_080640fc(void);
extern u32 sub_08002ee8(u32 range);
extern u32 sub_08068e04(void);
extern u32 sub_080692fc(void);
extern u32 sub_08069b44(void);
extern u8 sub_0806443c(void);
extern void TaskYieldTrampoline(u32 frames);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_080031b8(s32 id);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, u32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_0806395c(u8 v);
extern void sub_080639b4(u32 v);
extern void sub_08063e14(void);
extern void sub_08063ff4(void);
extern void sub_0806421c(s16 t, s16 mag);
extern void sub_080645a4(s32 step, s32 limit);
extern void sub_0806523c(void);
extern void sub_080656b4(void);
extern void sub_080860d8(void);
void sub_08088024(void);

/* Defined below */
void sub_0808614c(void);
void sub_08086320(void);
void sub_0808659c(void);
void sub_080868dc(void);
void sub_08086a44(void);
void sub_08086c14(void);
void sub_08086f40(void);
s32 sub_08086f54(void);
void sub_08087118(void);
void sub_080872bc(void);
void sub_08087508(void);
void sub_08087848(void);
void sub_080879b0(void);
void sub_08087b44(void);
void sub_08087e84(void);

void sub_080860f8(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808614c;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08742080);
}

void sub_08086128(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_0808614c;
    sub_08002e98(t->unk14, 1, gUnk_08742080);
}

void sub_0808614c(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08742084);
    sub_08068e04();
    sub_08069b44();
}

void sub_08086170(void)
{
    struct Task *t;
    s16 *p;

    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_080061c0(gUnk_08742088[gUnk_03002490->unk74], 0x5A5A5A5A);
    p = &gUnk_03002490->unk4A;
    if (*p < gUnk_03002790[sub_08063b38()].unk4A)
    {
        t = gUnk_03002490;
        t->unk58 = gUnk_08742090[0];
        t->unk60 = -gUnk_08742098[0];
    }
    else
    {
        t = gUnk_03002490;
        t->unk58 = -gUnk_08742090[0];
        t->unk60 = gUnk_08742098[0];
    }
    t = gUnk_03002490;
    t->unk30 = 4;
    t->unk34 = 40;
    while (1)
    {
        if (gUnk_03002490->unk60 >= 0)
        {
            sub_08006338(4);
            TaskYieldTrampoline(6);
            sub_08006338(7);
            TaskYieldTrampoline(3);
            sub_08006338(5);
            TaskYieldTrampoline(6);
            sub_08006338(7);
            TaskYieldTrampoline(1);
        }
        else
        {
            sub_08006338(4);
            TaskYieldTrampoline(2);
            sub_08006338(7);
            TaskYieldTrampoline(2);
            sub_08006338(5);
            TaskYieldTrampoline(2);
            sub_08006338(7);
            TaskYieldTrampoline(2);
        }
    }
}

void sub_08086274(void)
{
    struct Task *t = gUnk_03002490;
    s32 n;

    if (--t->unk34 != 0)
        return;
    n = t->unk30 - 1;
    t->unk30 = n;
    if (t->unk60 < 0)
    {
        if (n < 0)
            return;
        t->unk58 = -gUnk_08742090[0];
        t->unk60 = gUnk_08742098[0];
    }
    else
    {
        t->unk58 = gUnk_08742090[0];
        t->unk60 = -gUnk_08742098[0];
    }
    gUnk_03002490->unk34 = 40;
}

void sub_080862cc(void)
{
    gUnk_03002490->unk04 = (u32)sub_08086320;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087420A0);
}

void sub_080862fc(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_08086320;
    sub_08002e98(t->unk14, 1, gUnk_087420A0);
}

void sub_08086320(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087420A4);
    sub_08068e04();
    sub_08069b44();
}

void sub_08086344(void)
{
    struct Task *t;
    s16 *p;

    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_080061c0(gUnk_08742088[gUnk_03002490->unk74], 0x5A5A5A5A);
    p = &gUnk_03002490->unk4A;
    if (*p < gUnk_03002790[sub_08063b38()].unk4A)
    {
        t = gUnk_03002490;
        t->unk58 = gUnk_08742090[1];
        t->unk60 = -gUnk_08742098[1];
    }
    else
    {
        t = gUnk_03002490;
        t->unk58 = -gUnk_08742090[1];
        t->unk60 = gUnk_08742098[1];
    }
    gUnk_03002490->unk34 = 40;
    while (1)
    {
        if (gUnk_03002490->unk60 >= 0)
        {
            sub_08006338(4);
            TaskYieldTrampoline(6);
            sub_08006338(7);
            TaskYieldTrampoline(3);
            sub_08006338(5);
            TaskYieldTrampoline(6);
            sub_08006338(7);
            TaskYieldTrampoline(1);
        }
        else
        {
            sub_08006338(4);
            TaskYieldTrampoline(2);
            sub_08006338(7);
            TaskYieldTrampoline(2);
            sub_08006338(5);
            TaskYieldTrampoline(2);
            sub_08006338(7);
            TaskYieldTrampoline(2);
        }
    }
}

void sub_08086444(void)
{
    s32 d, a;

    if (--gUnk_03002490->unk34 == 20)
    {
        d = (s16)sub_08063d2c();
        a = d;
        if (d < 0)
            a = -d;
        if (a > 32)
        {
            if (d < 0)
            {
                if (gUnk_03002490->unk60 < 0)
                    gUnk_03002490->unk60 = 0xFFFFE000;
                else
                    gUnk_03002490->unk60 = 0xD00;
            }
            else
            {
                if (gUnk_03002490->unk60 < 0)
                    gUnk_03002490->unk60 = 0xFFFFF300;
                else
                    gUnk_03002490->unk60 = 0x2000;
            }
        }
    }
    if (gUnk_03002490->unk34 != 0)
        return;
    if (gUnk_03002490->unk60 < 0)
    {
        gUnk_03002490->unk58 = -gUnk_08742090[1];
        gUnk_03002490->unk60 = gUnk_08742098[1];
    }
    else
    {
        gUnk_03002490->unk58 = gUnk_08742090[1];
        gUnk_03002490->unk60 = -gUnk_08742098[1];
    }
    gUnk_03002490->unk34 = 40;
}

void sub_080864ec(void)
{
    struct Task *u;
    u32 r;

    gUnk_03002490->unk04 = (u32)sub_0808659c;
    if (sub_08063d2c() <= 31)
    {
        u = &gUnk_03002790[sub_08063b38()];
        gUnk_03002490->unk48 = (u16)u->unk48;
        gUnk_03002490->unk4C = u->unk4C;
    }
    else
    {
        r = sub_08002ee8(4);
        gUnk_03002490->unk48 =
            gUnk_087420AC[r] + (u16)gUnk_03002790[sub_08063b38()].unk48;
        gUnk_03002490->unk4C = (s16)gUnk_03002490->unk48 << 16;
    }
    gUnk_03002490->unk4A = 0;
    gUnk_03002490->unk50 = 0;
    gUnk_03002490->unk34 = 0;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, (u32 *)gUnk_087420A8);
}

void sub_0808659c(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087420BC);
    sub_08068e04();
    sub_08069b44();
}

void sub_080865c0(void)
{
    u8 k;

    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    gUnk_03002490->unk60 = 0x2500;
    gUnk_03002490->unk68 = 0x30000;
    sub_08006338(4);
    while (gUnk_03002490->unk34 == 0)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk28 = sub_080640dc(gUnk_087420D4);
    sub_080061c0(0xFFFF0000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0x40000;
    TaskYieldTrampoline(gUnk_087420E8[gUnk_03002490->unk74]);
    sub_080061c0(0xFFFE0000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0x30000;
    TaskYieldTrampoline(gUnk_087420E8[gUnk_03002490->unk74]);
    sub_080061c0(0xFFFD0000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(gUnk_087420E8[gUnk_03002490->unk74]);
    gUnk_03002490->unk28 = sub_080640dc(gUnk_087420C0);
    sub_080061c0(0xFFFE0000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087420E8[gUnk_03002490->unk74]);
    sub_080061c0(0xFFFF0000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087420E8[gUnk_03002490->unk74]);
    sub_080061c0(0xFFFF8000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087420E8[gUnk_03002490->unk74]);
    sub_080061c0(0x10000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFD0000;
    TaskYieldTrampoline(gUnk_087420E8[gUnk_03002490->unk74]);
    sub_080061c0(0x14000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087420E8[gUnk_03002490->unk74]);
    sub_080061c0(0x14000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087420E8[gUnk_03002490->unk74]);
    sub_080061c0(0x14000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087420E8[gUnk_03002490->unk74]);
    gUnk_03002490->unk28 = sub_080640dc(gUnk_087420D4);
    sub_080061c0(0x14000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0x8000;
    k = gUnk_03002490->unk74;
    if (k == 0)
    {
        TaskYieldTrampoline(16);
        gUnk_03002490->unk73 = k;
    }
    else
    {
        TaskYieldTrampoline(11);
        gUnk_03002490->unk73 = 1;
    }
    sub_08006138();
}

void sub_080867b8(void)
{
    if (gUnk_03002490->unk73 != 2)
    {
        sub_08006148(sub_080860d8, gCurTaskIdx);
        return;
    }
    if (gUnk_03002490->unk8C->unk2C != 0)
    {
        if (gUnk_03002490->unk28 == 0)
            gUnk_03002490->unk28 = sub_080640fc();
        gUnk_03002490->unk28--;
    }
    if (gUnk_03002490->unk34 != 0)
        return;
    if (sub_08063d2c() > 15)
        return;
    gUnk_03002490->unk34 = 1;
}

void sub_08086824(void)
{
    s8 k;

    gUnk_03002490->unk04 = (u32)sub_080868dc;
    k = sub_0806443c();
    if (k > 3)
        k -= 4;
    k -= 1;
    if (k < 0)
        k += 4;
    gUnk_03002490->unk2C = (k << 7) + 64;
    sub_0806421c(gUnk_03002490->unk2C,
                 gUnk_087420F4[gUnk_03002490->unk74] << 8 >> 16);
    gUnk_03002490->unk54 = gUnk_030023B4;
    gUnk_03002490->unk58 = gUnk_030023D4;
    gUnk_03002490->unk34 = 0;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087420F0);
}

void sub_080868b8(void)
{
    gUnk_03002490->unk04 = (u32)sub_080868dc;
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087420F0);
}

void sub_080868dc(void)
{
    s32 v;

    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08742100);
    if (sub_08086f54() != 0)
    {
        sub_0806421c((s16)gUnk_03002490->unk2C,
                     gUnk_087420F4[gUnk_03002490->unk74] << 8 >> 16);
        v = gUnk_03002490->unk54 = gUnk_030023B4;
        gUnk_03002490->unk58 = gUnk_030023D4;
        if (v > 0)
            gUnk_03002490->unk43 = 1;
        else if (v < 0)
            gUnk_03002490->unk43 = -1;
        gUnk_03002490->unk34 = 20;
        gUnk_03002490->unk28 = sub_080640dc(gUnk_087420C0);
    }
    gUnk_03002490->unk7A = 0;
    sub_08068e04();
    sub_08069b44();
}

void sub_08086984(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk7A = 0;
    while (1)
    {
        if (gUnk_03002490->unk34 == 0)
            gUnk_03002490->unk28 = sub_080640dc(gUnk_087420D4);
        TaskYieldTrampoline(1);
    }
}

void sub_080869b8(void)
{
    gUnk_03002490->unk34--;
    if (gUnk_03002490->unk8C->unk2C != 0)
    {
        if (gUnk_03002490->unk28 == 0)
            gUnk_03002490->unk28 = sub_080640fc();
        gUnk_03002490->unk28--;
    }
}

void sub_080869f0(void)
{
    gUnk_03002490->unk04 = (u32)sub_08086a44;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08742104);
}

void sub_08086a20(void)
{
    gUnk_03002490->unk04 = (u32)sub_08086a44;
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08742104);
}

void sub_08086a44(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08742108);
    sub_08068e04();
    sub_08069b44();
}

void sub_08086a68(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    gUnk_03002490->unk34 = 0;
    gUnk_03002490->unk30 = 384;
    do
    {
        switch (gUnk_03002490->unk34)
        {
        case 0:
            sub_08006338(4);
            TaskYieldTrampoline(6);
            sub_08006338(7);
            TaskYieldTrampoline(3);
            sub_08006338(5);
            TaskYieldTrampoline(6);
            sub_08006338(7);
            TaskYieldTrampoline(1);
            break;
        case 1:
            sub_08006338(4);
            TaskYieldTrampoline(1);
            break;
        case -1:
            sub_08006338(4);
            TaskYieldTrampoline(2);
            sub_08006338(7);
            TaskYieldTrampoline(2);
            sub_08006338(5);
            TaskYieldTrampoline(2);
            sub_08006338(7);
            TaskYieldTrampoline(2);
            break;
        }
    } while (gUnk_03002490->unk30 != 0);
    sub_08063e14();
    sub_08063ff4();
    sub_080061c0(0x6600, 0x5A5A5A5A);
    gUnk_03002490->unk60 = 0xFFFFE700;
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(2);
        sub_08006338(7);
        TaskYieldTrampoline(2);
        sub_08006338(5);
        TaskYieldTrampoline(2);
        sub_08006338(7);
        TaskYieldTrampoline(2);
    }
}

void sub_08086b68(void)
{
    if (gUnk_03002490->unk30 != 0)
    {
        gUnk_03002490->unk30--;
        sub_08063e14();
        gUnk_03002490->unk28++;
        if (gUnk_03002490->unk28 == 8)
        {
            sub_080645a4(gUnk_0874210C[gUnk_03002490->unk74],
                         gUnk_0874210C[gUnk_03002490->unk74 + 4]);
            gUnk_03002490->unk34 = gUnk_030023D4;
            gUnk_03002490->unk28 = 0;
        }
    }
}

void sub_08086bc0(void)
{
    gUnk_03002490->unk04 = (u32)sub_08086c14;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0874212C);
}

void sub_08086bf0(void)
{
    gUnk_03002490->unk04 = (u32)sub_08086c14;
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0874212C);
}

void sub_08086c14(void)
{
    if (gUnk_03002490->unk34 != 0)
    {
        if ((u8)sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08742138);
    }
    else
    {
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08742138);
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_08086c5c(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk34 = 1;
    gUnk_03002490->unk7A = 1;
    gUnk_03002490->unk28 = sub_080640dc(gUnk_08742144);
    while (gUnk_03002490->unk7A != 0)
    {
        if (sub_08063cd0() < 0)
        {
            if (-sub_08063cd0() <= 63)
                break;
        }
        else if (sub_08063cd0() <= 63)
            break;
        TaskYieldTrampoline(1);
    }
    sub_0806395c(1);
    sub_08006138();
}

void sub_08086ccc(void)
{
    if (gUnk_03002490->unk14 != 0)
    {
        sub_08006148(sub_08086bf0, gCurTaskIdx);
        return;
    }
    if (gUnk_03002490->unk8C->unk2C != 0)
    {
        if (gUnk_03002490->unk28 == 0)
            gUnk_03002490->unk28 = sub_080640fc();
        gUnk_03002490->unk28--;
    }
}

void sub_08086d18(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk34 = 0;
    sub_080031b8(187);
    gUnk_03002490->unk28 = sub_080640dc(gUnk_087420C0);
    gUnk_03002490->unk58 = 0xFFFD0000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk28 = sub_080640dc(gUnk_087420D4);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    sub_0806395c(2);
    sub_08006138();
}

void sub_08086da4(void)
{
    if (gUnk_03002490->unk14 != 1)
    {
        sub_08006148(sub_08086bf0, gCurTaskIdx);
        return;
    }
    if (gUnk_03002490->unk8C->unk2C != 0)
    {
        if (gUnk_03002490->unk28 == 0)
            gUnk_03002490->unk28 = sub_080640fc();
        gUnk_03002490->unk28--;
    }
}

void sub_08086df0(void)
{
    gUnk_03002490->unk15 = 2;
    gUnk_03002490->unk28 = sub_080640dc(gUnk_087420D4);
    switch (gUnk_03002490->unk74)
    {
    case 0:
        TaskYieldTrampoline(48);
        sub_080062c4();
        sub_08063e14();
        sub_080061c0(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(10);
        sub_080061c0(0x20000, 0x5A5A5A5A);
        gUnk_03002490->unk28 = sub_080640dc(gUnk_087420C0);
        TaskYieldTrampoline(10);
        sub_080061c0(0x30000, 0x5A5A5A5A);
        sub_08006138();
        break;
    case 1:
        TaskYieldTrampoline(32);
        sub_080062c4();
        sub_08063e14();
        sub_080061c0(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(8);
        sub_080061c0(0x20000, 0x5A5A5A5A);
        gUnk_03002490->unk28 = sub_080640dc(gUnk_087420C0);
        TaskYieldTrampoline(8);
        sub_080061c0(0x30000, 0x5A5A5A5A);
        sub_08006138();
        break;
    }
}

void sub_08086ec8(void)
{
    if (gUnk_03002490->unk8C->unk2C != 0)
    {
        if (gUnk_03002490->unk28 == 0)
            gUnk_03002490->unk28 = sub_080640fc();
        gUnk_03002490->unk28--;
    }
}

void sub_08086efc(void)
{
    gUnk_03002490->unk04 = (u32)sub_08086f40;
    sub_080639b4((u32)gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_08063e14();
    while (1)
    {
        sub_08006338(8);
        TaskYieldTrampoline(18);
        sub_08006338(9);
        TaskYieldTrampoline(10);
    }
}

void sub_08086f40(void)
{
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

s32 sub_08086f54(void)
{
    u8 n = 0;
    s16 v;

    if ((gUnk_03002490->unk7A & 1) == 0 && (gUnk_03005550[1] & 1) == 0
     && (gUnk_03005550[0] & 3) == 0)
        return 0;
    switch (gUnk_03005550[4])
    {
    case 0:
        if (gUnk_03002490->unk7A & 1)
            n = n + 1;
        if ((gUnk_03005550[1] & 1) == 0)
            break;
        n = n + 2;
        break;
    case 3:
        n = n + 3;
        break;
    case 4:
        n = n + 4;
        break;
    case 1:
        n = n + 5;
        break;
    case 2:
        n = n + 6;
        break;
    case 8:
        n = n + 7;
        break;
    case 7:
        n = n + 8;
        break;
    case 6:
        n = n + 9;
        break;
    case 5:
        n = n + 10;
        break;
    }
    if (gUnk_03005550[0] & 1)
        n = n + 11;
    if (gUnk_03005550[0] & 2)
        n = n + 22;
    v = gUnk_08742150[(n << 4) + (gUnk_03002490->unk2C >> 5)];
    if (v < 0)
        return 0;
    gUnk_03002490->unk2C = v;
    return 1;
}

void sub_0808705c(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752560;
    sub_08063e14();
    sub_08002e98(gUnk_03002490->unk73, 10, gUnk_08742570);
}

void sub_080870a4(void)
{
    sub_08002e98(gUnk_03002490->unk73, 10, gUnk_08742570);
}

void sub_080870c4(void)
{
    gUnk_03002490->unk04 = (u32)sub_08087118;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087425B0);
}

void sub_080870f4(void)
{
    gUnk_03002490->unk04 = (u32)sub_08087118;
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087425B0);
}

void sub_08087118(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087425B4);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808713c(void)
{
    s16 *p;

    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_080061c0(gUnk_087425B8[gUnk_03002490->unk74], 0x5A5A5A5A);
    p = &gUnk_03002490->unk4A;
    if (*p < gUnk_03002790[sub_08063b38()].unk4A)
    {
        gUnk_03002490->unk58 = gUnk_087425C0[0];
        gUnk_03002490->unk60 = -gUnk_087425C8[0];
    }
    else
    {
        gUnk_03002490->unk58 = -gUnk_087425C0[0];
        gUnk_03002490->unk60 = gUnk_087425C8[0];
    }
    gUnk_03002490->unk30 = 4;
    gUnk_03002490->unk34 = 40;
    while (1)
    {
        if (gUnk_03002490->unk60 >= 0)
        {
            sub_08006338(4);
            TaskYieldTrampoline(8);
            sub_08006338(5);
            TaskYieldTrampoline(8);
        }
        else
        {
            sub_08006338(4);
            TaskYieldTrampoline(4);
            sub_08006338(5);
            TaskYieldTrampoline(4);
        }
    }
}

void sub_08087210(void)
{
    struct Task *t = gUnk_03002490;
    s32 n;

    if (--t->unk34 != 0)
        return;
    n = t->unk30 - 1;
    t->unk30 = n;
    if (t->unk60 < 0)
    {
        if (n < 0)
            return;
        t->unk58 = -gUnk_087425C0[0];
        t->unk60 = gUnk_087425C8[0];
    }
    else
    {
        t->unk58 = gUnk_087425C0[0];
        t->unk60 = -gUnk_087425C8[0];
    }
    gUnk_03002490->unk34 = 40;
}

void sub_08087268(void)
{
    gUnk_03002490->unk04 = (u32)sub_080872bc;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087425D0);
}

void sub_08087298(void)
{
    gUnk_03002490->unk04 = (u32)sub_080872bc;
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087425D0);
}

void sub_080872bc(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087425D4);
    sub_08068e04();
    sub_08069b44();
}

void sub_080872e0(void)
{
    s16 *p;

    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_080061c0(gUnk_087425B8[gUnk_03002490->unk74], 0x5A5A5A5A);
    p = &gUnk_03002490->unk4A;
    if (*p < gUnk_03002790[sub_08063b38()].unk4A)
    {
        gUnk_03002490->unk58 = gUnk_087425C0[1];
        gUnk_03002490->unk60 = -gUnk_087425C8[1];
    }
    else
    {
        gUnk_03002490->unk58 = -gUnk_087425C0[1];
        gUnk_03002490->unk60 = gUnk_087425C8[1];
    }
    gUnk_03002490->unk34 = 40;
    while (1)
    {
        if (gUnk_03002490->unk60 >= 0)
        {
            sub_08006338(4);
            TaskYieldTrampoline(8);
            sub_08006338(5);
            TaskYieldTrampoline(8);
        }
        else
        {
            sub_08006338(4);
            TaskYieldTrampoline(4);
            sub_08006338(5);
            TaskYieldTrampoline(4);
        }
    }
}

void sub_080873b0(void)
{
    s32 d, a;

    if (--gUnk_03002490->unk34 == 20)
    {
        d = (s16)sub_08063d2c();
        a = d;
        if (d < 0)
            a = -d;
        if (a > 32)
        {
            if (d < 0)
            {
                if (gUnk_03002490->unk60 < 0)
                    gUnk_03002490->unk60 = 0xFFFFE000;
                else
                    gUnk_03002490->unk60 = 0xD00;
            }
            else
            {
                if (gUnk_03002490->unk60 < 0)
                    gUnk_03002490->unk60 = 0xFFFFF300;
                else
                    gUnk_03002490->unk60 = 0x2000;
            }
        }
    }
    if (gUnk_03002490->unk34 != 0)
        return;
    if (gUnk_03002490->unk60 < 0)
    {
        gUnk_03002490->unk58 = -gUnk_087425C0[1];
        gUnk_03002490->unk60 = gUnk_087425C8[1];
    }
    else
    {
        gUnk_03002490->unk58 = gUnk_087425C0[1];
        gUnk_03002490->unk60 = -gUnk_087425C8[1];
    }
    gUnk_03002490->unk34 = 40;
}

void sub_08087458(void)
{
    struct Task *u;
    u32 r;

    gUnk_03002490->unk04 = (u32)sub_08087508;
    if (sub_08063d2c() <= 31)
    {
        u = &gUnk_03002790[sub_08063b38()];
        gUnk_03002490->unk48 = (u16)u->unk48;
        gUnk_03002490->unk4C = u->unk4C;
    }
    else
    {
        r = sub_08002ee8(4);
        gUnk_03002490->unk48 =
            gUnk_087425DC[r] + (u16)gUnk_03002790[sub_08063b38()].unk48;
        gUnk_03002490->unk4C = (s16)gUnk_03002490->unk48 << 16;
    }
    gUnk_03002490->unk4A = 0;
    gUnk_03002490->unk50 = 0;
    gUnk_03002490->unk34 = 0;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, (u32 *)gUnk_087425D8);
}

void sub_08087508(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087425EC);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808752c(void)
{
    u8 k;

    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    gUnk_03002490->unk60 = 0x2500;
    gUnk_03002490->unk68 = 0x30000;
    sub_08006338(4);
    while (gUnk_03002490->unk34 == 0)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk28 = sub_080640dc(gUnk_087425A4);
    sub_080061c0(0xFFFF0000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0x40000;
    TaskYieldTrampoline(gUnk_087425F0[gUnk_03002490->unk74]);
    sub_080061c0(0xFFFE0000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0x30000;
    TaskYieldTrampoline(gUnk_087425F0[gUnk_03002490->unk74]);
    sub_080061c0(0xFFFD0000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(gUnk_087425F0[gUnk_03002490->unk74]);
    gUnk_03002490->unk28 = sub_080640dc(gUnk_08742598);
    sub_080061c0(0xFFFE0000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087425F0[gUnk_03002490->unk74]);
    sub_080061c0(0xFFFF0000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087425F0[gUnk_03002490->unk74]);
    sub_080061c0(0xFFFF8000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087425F0[gUnk_03002490->unk74]);
    sub_080061c0(0x10000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFD0000;
    TaskYieldTrampoline(gUnk_087425F0[gUnk_03002490->unk74]);
    sub_080061c0(0x14000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(gUnk_087425F0[gUnk_03002490->unk74]);
    sub_080061c0(0x14000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(gUnk_087425F0[gUnk_03002490->unk74]);
    sub_080061c0(0x14000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(gUnk_087425F0[gUnk_03002490->unk74]);
    gUnk_03002490->unk28 = sub_080640dc(gUnk_087425A4);
    sub_080061c0(0x14000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0x8000;
    k = gUnk_03002490->unk74;
    if (k == 0)
    {
        TaskYieldTrampoline(16);
        gUnk_03002490->unk73 = k;
    }
    else
    {
        TaskYieldTrampoline(11);
        gUnk_03002490->unk73 = 1;
    }
    sub_08006138();
}

void sub_08087724(void)
{
    if (gUnk_03002490->unk73 != 2)
    {
        sub_08006148(sub_080870a4, gCurTaskIdx);
        return;
    }
    if (gUnk_03002490->unk8C->unk2C != 0)
    {
        if (gUnk_03002490->unk28 == 0)
            gUnk_03002490->unk28 = sub_080640fc();
        gUnk_03002490->unk28--;
    }
    if (gUnk_03002490->unk34 != 0)
        return;
    if (sub_08063d2c() > 15)
        return;
    gUnk_03002490->unk34 = 1;
}

void sub_08087790(void)
{
    s8 k;

    gUnk_03002490->unk04 = (u32)sub_08087848;
    k = sub_0806443c() - 1;
    if (k > 3)
        k -= 4;
    k -= 1;
    if (k < 0)
        k += 4;
    gUnk_03002490->unk2C = (k << 7) + 64;
    sub_0806421c(gUnk_03002490->unk2C,
                 gUnk_08742600[gUnk_03002490->unk74] << 8 >> 16);
    gUnk_03002490->unk54 = gUnk_030023B4;
    gUnk_03002490->unk58 = gUnk_030023D4;
    gUnk_03002490->unk34 = 0;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087425F8);
}

void sub_08087824(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_08087848;
    sub_08002e98(t->unk14, 1, gUnk_087425F8);
}

void sub_08087848(void)
{
    s32 v;

    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087425FC);
    if (sub_08086f54() != 0)
    {
        sub_0806421c((s16)gUnk_03002490->unk2C,
                     gUnk_08742600[gUnk_03002490->unk74] << 8 >> 16);
        v = gUnk_03002490->unk54 = gUnk_030023B4;
        gUnk_03002490->unk58 = gUnk_030023D4;
        if (v > 0)
            gUnk_03002490->unk43 = 1;
        else if (v < 0)
            gUnk_03002490->unk43 = -1;
        gUnk_03002490->unk34 = 20;
        gUnk_03002490->unk28 = sub_080640dc(gUnk_08742598);
    }
    gUnk_03002490->unk7A = 0;
    sub_08068e04();
    sub_08069b44();
}

void sub_080878f0(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk7A = 0;
    while (1)
    {
        if (gUnk_03002490->unk34 == 0)
            gUnk_03002490->unk28 = sub_080640dc(gUnk_087425A4);
        TaskYieldTrampoline(1);
    }
}

void sub_08087924(void)
{
    gUnk_03002490->unk34--;
    if (gUnk_03002490->unk8C->unk2C != 0)
    {
        if (gUnk_03002490->unk28 == 0)
            gUnk_03002490->unk28 = sub_080640fc();
        gUnk_03002490->unk28--;
    }
}

void sub_0808795c(void)
{
    gUnk_03002490->unk04 = (u32)sub_080879b0;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_0874260C);
}

void sub_0808798c(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_080879b0;
    sub_08002e98(t->unk14, 1, gUnk_0874260C);
}

void sub_080879b0(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08742610);
    sub_08068e04();
    sub_08069b44();
}

void sub_080879d4(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    gUnk_03002490->unk34 = 0;
    gUnk_03002490->unk30 = 384;
    do
    {
        switch (gUnk_03002490->unk34)
        {
        case 0:
            sub_08006338(4);
            TaskYieldTrampoline(8);
            sub_08006338(5);
            TaskYieldTrampoline(8);
            break;
        case 1:
            sub_08006338(4);
            TaskYieldTrampoline(1);
            break;
        case -1:
            sub_08006338(4);
            TaskYieldTrampoline(4);
            sub_08006338(5);
            TaskYieldTrampoline(4);
            break;
        }
    } while (gUnk_03002490->unk30 != 0);
    sub_08063e14();
    sub_08063ff4();
    sub_080061c0(0x6600, 0x5A5A5A5A);
    gUnk_03002490->unk60 = 0xFFFFE700;
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(4);
        sub_08006338(5);
        TaskYieldTrampoline(4);
    }
}

void sub_08087a98(void)
{
    if (gUnk_03002490->unk30 != 0)
    {
        gUnk_03002490->unk30--;
        sub_08063e14();
        gUnk_03002490->unk28++;
        if (gUnk_03002490->unk28 == 8)
        {
            sub_080645a4(gUnk_08742614[gUnk_03002490->unk74],
                         gUnk_08742614[gUnk_03002490->unk74 + 4]);
            gUnk_03002490->unk34 = gUnk_030023D4;
            gUnk_03002490->unk28 = 0;
        }
    }
}

void sub_08087af0(void)
{
    gUnk_03002490->unk04 = (u32)sub_08087b44;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0874263C);
}

void sub_08087b20(void)
{
    gUnk_03002490->unk04 = (u32)sub_08087b44;
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0874263C);
}

void sub_08087b44(void)
{
    if (gUnk_03002490->unk34 != 0)
    {
        if ((u8)sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08742648);
    }
    else
    {
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08742648);
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_08087b8c(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk34 = 1;
    gUnk_03002490->unk7A = 1;
    gUnk_03002490->unk28 = sub_080640dc(gUnk_08742634);
    while (gUnk_03002490->unk7A != 0)
    {
        if (sub_08063cd0() < 0)
        {
            if (-sub_08063cd0() <= 63)
                break;
        }
        else if (sub_08063cd0() <= 63)
            break;
        TaskYieldTrampoline(1);
    }
    sub_0806395c(1);
    sub_08006138();
}

void sub_08087bfc(void)
{
    if (gUnk_03002490->unk14 != 0)
    {
        sub_08006148(sub_08087b20, gCurTaskIdx);
        return;
    }
    if (gUnk_03002490->unk8C->unk2C != 0)
    {
        if (gUnk_03002490->unk28 == 0)
            gUnk_03002490->unk28 = sub_080640fc();
        gUnk_03002490->unk28--;
    }
}

void sub_08087c48(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk34 = 0;
    sub_080031b8(187);
    gUnk_03002490->unk28 = sub_080640dc(gUnk_08742598);
    gUnk_03002490->unk58 = 0xFFFD0000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk28 = sub_080640dc(gUnk_087425A4);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    sub_0806395c(2);
    sub_08006138();
}

void sub_08087cd4(void)
{
    if (gUnk_03002490->unk14 != 1)
    {
        sub_08006148(sub_08087b20, gCurTaskIdx);
        return;
    }
    if (gUnk_03002490->unk8C->unk2C != 0)
    {
        if (gUnk_03002490->unk28 == 0)
            gUnk_03002490->unk28 = sub_080640fc();
        gUnk_03002490->unk28--;
    }
}

void sub_08087d20(void)
{
    gUnk_03002490->unk15 = 2;
    gUnk_03002490->unk28 = sub_080640dc(gUnk_087425A4);
    switch (gUnk_03002490->unk74)
    {
    case 0:
        TaskYieldTrampoline(48);
        sub_080062c4();
        sub_08063e14();
        sub_080061c0(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(10);
        sub_080061c0(0x20000, 0x5A5A5A5A);
        gUnk_03002490->unk28 = sub_080640dc(gUnk_08742598);
        TaskYieldTrampoline(10);
        sub_080061c0(0x30000, 0x5A5A5A5A);
        sub_08006138();
        break;
    case 1:
        TaskYieldTrampoline(32);
        sub_080062c4();
        sub_08063e14();
        sub_080061c0(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(8);
        sub_080061c0(0x20000, 0x5A5A5A5A);
        gUnk_03002490->unk28 = sub_080640dc(gUnk_08742598);
        TaskYieldTrampoline(8);
        sub_080061c0(0x30000, 0x5A5A5A5A);
        sub_08006138();
        break;
    }
}

void sub_08087df8(void)
{
    if (gUnk_03002490->unk8C->unk2C != 0)
    {
        if (gUnk_03002490->unk28 == 0)
            gUnk_03002490->unk28 = sub_080640fc();
        gUnk_03002490->unk28--;
    }
}

void sub_08087e2c(void)
{
    gUnk_03002490->unk04 = (u32)sub_08087e84;
    gUnk_03002490->unk7A = 1;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08742654);
}

void sub_08087e60(void)
{
    gUnk_03002490->unk04 = (u32)sub_08087e84;
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08742654);
}

void sub_08087e84(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08742660);
    sub_08068e04();
    sub_08069b44();
}

void sub_08087eb4(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_08006338(6);
    TaskYieldTrampoline(24);
    sub_08063e14();
    sub_08006338(6);
    TaskYieldTrampoline(24);
    sub_0806395c(1);
    sub_08006138();
}

void sub_08087ef0(void)
{
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_08087e60, gCurTaskIdx);
}

void sub_08087f18(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk7A = 0;
    sub_080031b8(187);
    sub_0800622c(0xFFFE0000, 0x800, 0x30000);
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08006338(4);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
    } while ((s16)++gUnk_03002490->unk6C <= 3);
    sub_08006338(4);
    sub_08006138();
}

void sub_08087f88(void)
{
}

void sub_08087f8c(void)
{
    gUnk_03002490->unk15 = 2;
    gUnk_03002490->unk60 = 0x800;
    gUnk_03002490->unk68 = 0x30000;
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
    }
}

void sub_08087fc8(void)
{
}

void sub_08087fcc(void)
{
    gUnk_03002490->unk04 = (u32)sub_08088024;
    gUnk_03002490->unk7A = 1;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_0874266C);
}
