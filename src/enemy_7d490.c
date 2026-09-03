/* game_code_and_rodata 0x0807D490-0x0807F044 (issue #77, module M20 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0807D490 0x0807F044 src/enemy_7d490.c --newpb
 *
 * The last third of enemy/object behaviour bank 1 (see src/enemy_78b68.c for
 * the bank's three-table script pattern).  Mostly "moving scenery": scripts
 * that drive Task.unk54/unk58 (the 16.16 x/y velocity pair) and Task.unk60
 * from ROM tables, then wait on the collision flag Task.unk7A:
 *   * the two class-8 platform intros `sub_0807d490` and `sub_0807d510`
 *     (the latter a seventeen-step cue script over `0x087410C0`);
 *   * task #216's rider `sub_0807d6c4`+`sub_0807d718`, whose per-frame
 *     handlers `sub_0807d82c` / `sub_0807d918` re-centre on the nearest
 *     player when `|sub_08063CD0()| <= 49` and `|sub_08063D18()| <= 15`;
 *   * the class-3 three-way branch pair `sub_0807dd70` / `sub_0807dddc`
 *     (`switch (Task.unk73)` with an empty `case 1`);
 *   * the `0x08741220` function-pointer table the three `sub_0807e244` /
 *     `sub_0807e3b0` hooks dispatch through;
 *   * the two 0x120/0xA8-byte cutscene coroutines `sub_0807e290` and
 *     `sub_0807e768`, which spawn a companion with `sub_08064B5C` and then
 *     bounce between velocity presets until Task.unk7A fires;
 *   * the swing/orbit loops `sub_0807ea84`, `sub_0807eb60`, `sub_0807ec4c`,
 *     `sub_0807ed20` and `sub_0807ef7c`, each an infinite eight-step ramp.
 *
 * `sub_0807daf0`, `sub_0807e428`, `sub_0807e5a0`, `sub_0807e904`,
 * `sub_0807ee44` and `sub_0807ef08` are dead exports: each is a copy of its
 * host's tail dispatch that nothing in the ROM references (curated in
 * tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s8 gUnk_02007FB8[];
extern struct Task *gUnk_03002490;

/* ROM tables */
extern struct AnimCmd gUnk_087412A8[];
extern struct AnimCmd gUnk_087412EC[];
extern u32 gUnk_0873F500[];
extern u32 gUnk_087410C0[];
extern u32 gUnk_087410D8[];
extern u32 gUnk_087411C0[];
extern u32 gUnk_087411CC[];
extern u32 gUnk_087411E0[];
extern u32 gUnk_087411F4[];
extern u32 gUnk_087411F8[];
extern u32 gUnk_087411FC[];
extern u32 gUnk_08741208[];
extern u32 gUnk_08741218[];
extern u32 gUnk_08741220[];
extern u32 gUnk_08741228[];
extern u32 gUnk_08741234[];
extern u32 gUnk_08741240[];
extern u32 gUnk_0874124C[];
extern u32 gUnk_08741258[];
extern u32 gUnk_08741264[];
extern u32 gUnk_08741268[];
extern u32 gUnk_0874126C[];
extern u32 gUnk_08741278[];
extern u32 gUnk_0874127C[];
extern u32 gUnk_08741280[];
extern u32 gUnk_08741288[];
extern u32 gUnk_08741290[];
extern u32 gUnk_08741294[];
extern u32 gUnk_08741298[];
extern u32 gUnk_087412A0[];
extern u32 gUnk_087412BC[];
extern u32 gUnk_087412CC[];
extern u32 gUnk_087412D0[];
extern u32 gUnk_087412D4[];
extern u32 gUnk_087412D8[];
extern u32 gUnk_087412DC[];
extern u32 gUnk_087412E0[];
extern u32 gUnk_087412E4[];
extern u32 gUnk_087412E8[];
extern u32 gUnk_08741300[];
extern u32 gUnk_08741308[];
extern u32 gUnk_0874130C[];
extern u32 gUnk_08741310[];
extern u32 gUnk_08741314[];
extern u32 gUnk_08741380[];
extern u32 gUnk_0874183C[];
extern u32 gUnk_08752108[];
extern u32 gUnk_08752194[];
extern u32 gUnk_08752234[];
extern u32 gUnk_0875230C[];
extern u32 gUnk_0875235C[];
extern u32 gUnk_0875237C[];
extern u32 gUnk_087523EC[];
extern u32 gUnk_08752438[];
extern u8 gUnk_08741214[];
extern u8 gUnk_08741216[];

/* Externals */
extern s32 sub_08002ee8(s32 a);
extern s32 sub_080031b8(s32 id);
extern s32 sub_08063b38(void);
extern s32 sub_08063cbc(s32 i);
extern s32 sub_08063cd0(void);
extern s32 sub_08063d18(u32 i);
extern s32 sub_08063df4(void);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_08064188(s32 n);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064fc4(u8 cls, u32 sub, u8 p3, u8 p4, int x, int y, u16 prio);
extern s32 sub_0806956c();
extern s32 sub_080695bc();
extern s32 sub_08069b44(void);
extern u32 sub_08068e04(void);
extern u32 sub_080692fc(void);
extern void TaskDispatchTrampoline(void);
extern void TaskYieldTrampoline(u32 frames);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_080059d8(void);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, u32 i);
extern void sub_080061a8(s32 a, s32 b, s32 c);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_080261d4(s32 a);
extern void sub_08063908(u32 *def);
extern void sub_0806395c(u32 v);
extern void sub_080639b4(u32 *p);
extern void sub_08063ddc(u32 i);
extern void sub_08063e14(void);
extern void sub_08063ff4(void);
extern void sub_08064a60(void);
extern void sub_080651b4(void);
extern void sub_0806523c(void);
extern void sub_080656b4(void);
extern void sub_08065848(u32 p0, s32 idx);
extern void sub_0806a0f0(s32 a);
extern void sub_080b5540(s32 a, s32 b);

/* Forward declarations */
void sub_0807d718(void);
void sub_0807db0c(void);
void sub_0807db9c(void);
void sub_0807dd10(void);
void sub_0807df30(void);
void sub_0807e060(void);
void sub_0807e444(void);
void sub_0807e484(void);
void sub_0807e5bc(void);
void sub_0807e730(void);
void sub_0807e920(void);
void sub_0807ea60(void);
void sub_0807eb20(void);
void sub_0807ec10(void);
void sub_0807ecfc(void);
void sub_0807ee60(void);
void sub_0807ef24(void);
void sub_0807ef7c(void);

void sub_0807d490(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_080651b4;
    gUnk_03002490->unk42 = 8;
    t = gUnk_03002490;
    t->unk38 = gUnk_08752234;
    t->unk40 = (0xFFF & t->unk40) | 0xF000;
    sub_08064a60();
    sub_080061c0(0x30000, -0x5000);
    {
        struct Task *u = gUnk_03002490;

        u->unk60 = -0x4000;
        u->unk3C = 2;
    }
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    TaskDispatchTrampoline();
}

void sub_0807d510(void)
{
    u32 a;
    u32 b;
    s32 i;
    s32 j;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_080651b4;
    gUnk_03002490->unk42 = 8;
    {
        struct Task *t = gUnk_03002490;

        t->unk38 = gUnk_08752234;
        t->unk40 = (0xFFF & t->unk40) | 0xF000;
    }
    sub_08064a60();
    i = gUnk_03002490->unk74;
    j = i * 2;
    a = gUnk_087410C0[j];
    b = gUnk_087410C0[j + 1];
    sub_080062c4();
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(4);
    sub_080061c0(a, 0x5A5A5A5A);
    {
        struct Task *t = gUnk_03002490;

        t->unk58 = b;
        t->unk3C = -1;
    }
    TaskYieldTrampoline(1);
    sub_080062c4();
    gUnk_03002490->unk3C = -1;
    TaskYieldTrampoline(1);
    sub_080061c0(a, 0x5A5A5A5A);
    {
        struct Task *t = gUnk_03002490;

        t->unk58 = b;
        t->unk3C = 4;
    }
    TaskYieldTrampoline(1);
    sub_080062c4();
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    sub_080061c0(a, 0x5A5A5A5A);
    {
        struct Task *t = gUnk_03002490;

        t->unk58 = b;
        t->unk3C = 4;
    }
    TaskYieldTrampoline(1);
    sub_080062c4();
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    sub_080061c0(a, 0x5A5A5A5A);
    {
        struct Task *t = gUnk_03002490;

        t->unk58 = b;
        t->unk3C = 4;
    }
    TaskYieldTrampoline(1);
    sub_080061c0(0x30000, 0x5A5A5A5A);
    {
        struct Task *t = gUnk_03002490;

        t->unk58 = gUnk_087410D8[t->unk74];
        t->unk3C--;
    }
    TaskYieldTrampoline(1);
    sub_080061c0(0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(1);
    sub_080061c0(0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_0807d684(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752194;
    sub_08002e98(gUnk_03002490->unk73, 3, gUnk_087411C0);
}

void sub_0807d6c4(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807d718;
    sub_08063e14();
    gUnk_03002490->unk28 = 0;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 5, gUnk_087411CC);
}

void sub_0807d6fc(void)
{
    sub_08002e98(gUnk_03002490->unk14, 5, gUnk_087411CC);
}

void sub_0807d718(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 5, gUnk_087411E0);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807d748(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080061c0(0x2000, 0x5A5A5A5A);
        sub_08006338(4);
        TaskYieldTrampoline(8);
        sub_080061c0(0x5800, 0x5A5A5A5A);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0;
            t->unk3C++;
        }
        TaskYieldTrampoline(8);
        sub_080061c0(0x2000, 0x5A5A5A5A);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        sub_080061c0(0x4800, 0x5A5A5A5A);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0;
            t->unk3C++;
        }
        TaskYieldTrampoline(8);
    } while ((s16)++gUnk_03002490->unk6C <= 1);
    gUnk_03002490->unk54 = 0;
    sub_0806395c(1);
    sub_08006138();
}

void sub_0807d82c(void)
{
    gUnk_03002490->unk2C = sub_08063b38();
    if ((sub_08063cbc(gUnk_03002490->unk2C) < 0
             ? -sub_08063cbc(gUnk_03002490->unk2C)
             : sub_08063cbc(gUnk_03002490->unk2C))
        <= 49)
    {
        if ((sub_08063d18(gUnk_03002490->unk2C) < 0
                 ? -sub_08063d18(gUnk_03002490->unk2C)
                 : sub_08063d18(gUnk_03002490->unk2C))
            <= 15)
        {
            struct Task *t = gUnk_03002490;
            s32 n = --t->unk28;

            if (n <= 0)
            {
                t->unk30 = n < 0 ? 4 : 3;
                if (sub_08002ee8(gUnk_03002490->unk30) == 0)
                {
                    sub_08063ddc(gUnk_03002490->unk2C);
                    sub_0806395c(2);
                }
                gUnk_03002490->unk28 = 60;
            }
        }
    }
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_0807d6fc, gCurTaskIdx);
}

void sub_0807d8f8(void)
{
    gUnk_03002490->unk15 = 1;
    sub_0807dd10();
    sub_0806395c(0);
    sub_08006138();
}

void sub_0807d918(void)
{
    gUnk_03002490->unk2C = sub_08063b38();
    if ((sub_08063cbc(gUnk_03002490->unk2C) < 0
             ? -sub_08063cbc(gUnk_03002490->unk2C)
             : sub_08063cbc(gUnk_03002490->unk2C))
        <= 49)
    {
        if ((sub_08063d18(gUnk_03002490->unk2C) < 0
                 ? -sub_08063d18(gUnk_03002490->unk2C)
                 : sub_08063d18(gUnk_03002490->unk2C))
            <= 15)
            sub_08063ddc(gUnk_03002490->unk2C);
    }
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_0807d6fc, gCurTaskIdx);
}

void sub_0807d9ac(void)
{
    gUnk_03002490->unk7A = 0;
    gUnk_03002490->unk15 = 2;
    sub_080061c0(0x18000, 0);
    {
        struct Task *t = gUnk_03002490;

        t->unk58 = -0x2E800;
        t->unk60 = 0x2000;
    }
    sub_08006338(14);
    TaskYieldTrampoline(24);
    sub_08006338(12);
    sub_080062c4();
    TaskYieldTrampoline(16);
    sub_0806395c(4);
    sub_08006138();
}

void sub_0807da08(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_0807d6fc, gCurTaskIdx);
}

void sub_0807da30(void)
{
    gUnk_03002490->unk15 = 3;
    sub_08006338(12);
    sub_080062c4();
    TaskYieldTrampoline(16);
    sub_0806395c(4);
    sub_08006138();
}

void sub_0807da5c(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_0807d6fc, gCurTaskIdx);
}

void sub_0807da84(void)
{
    gUnk_03002490->unk15 = 4;
    gUnk_03002490->unk58 = 0x80000;
    sub_08006338(12);
    sub_08006138();
}

void sub_0807daa8(void)
{
}

void sub_0807daac(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807db0c;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087411F4);
}

void sub_0807daf0(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087411F4);
}

void sub_0807db0c(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087411F8);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807db30(void)
{
    gUnk_03002490->unk15 = 0;
    while (1)
        sub_0807dd10();
}

void sub_0807db44(void)
{
}

void sub_0807db48(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807db9c;
    sub_08063e14();
    gUnk_03002490->unk28 = 0;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087411FC);
}

void sub_0807db80(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087411FC);
}

void sub_0807db9c(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08741208);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807dbcc(void)
{
    gUnk_03002490->unk15 = 0;
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0;
        t->unk6C = 0;
    }
    do
    {
        sub_08063e14();
        sub_08006338(4);
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
    } while ((s16)++gUnk_03002490->unk6C <= 1);
    sub_0806395c(1);
    sub_08006138();
}

void sub_0807dc78(void)
{
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_0807db80, gCurTaskIdx);
}

void sub_0807dca0(void)
{
    gUnk_03002490->unk15 = 1;
    sub_0807dd10();
    sub_0806395c(0);
    sub_08006138();
}

void sub_0807dcc0(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_0807db80, gCurTaskIdx);
}

void sub_0807dce8(void)
{
    gUnk_03002490->unk15 = 2;
    gUnk_03002490->unk58 = 0x80000;
    sub_08006338(12);
    sub_08006138();
}

void sub_0807dd0c(void)
{
}

void sub_0807dd10(void)
{
    sub_08006338(12);
    TaskYieldTrampoline(100);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(60);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(18);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(6);
}

s32 sub_0807dd70(void)
{
    s32 r;

    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0;
        t->unk58 = 0;
    }
    sub_080261d4(1);
    sub_080031b8(163);
    r = 0;
    switch (gUnk_03002490->unk73)
    {
    case 0:
        sub_0806395c(1);
        sub_08006148(sub_0807d6fc, gCurTaskIdx);
        r = 1;
        break;
    case 1:
        break;
    case 2:
        sub_0806395c(1);
        sub_08006148(sub_0807db80, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 sub_0807dddc(void)
{
    s32 r = 0;

    switch (gUnk_03002490->unk73)
    {
    case 0:
        sub_0806395c(4);
        sub_08006148(sub_0807d6fc, gCurTaskIdx);
        r = 1;
        break;
    case 1:
        break;
    case 2:
        sub_0806395c(2);
        sub_08006148(sub_0807db80, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 sub_0807de30(void)
{
    s32 r = 0;

    if (gUnk_03002490->unk73 == 0)
    {
        sub_0806395c(3);
        sub_08006148(sub_0807d6fc, gCurTaskIdx);
        r = 1;
    }
    return r;
}

s32 sub_0807de64(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 == 2)
        t->unk54 = 0;
    else
        sub_08063ff4();
    return 0;
}

s32 sub_0807de88(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_0807de98(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_0875230C;
    sub_08063e14();
    {
        struct Task *t = gUnk_03002490;

        t->unk2C = 0;
        sub_08002e98(t->unk73, 3, gUnk_08741228);
    }
}

void sub_0807dee4(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807df30;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08741234);
}

void sub_0807df14(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08741234);
}

void sub_0807df30(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08741240);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807df60(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    t = gUnk_03002490;
    t->unk28 = gUnk_08741216[t->unk74];
    sub_0807e484();
}

void sub_0807df8c(void)
{
    struct Task *t = gUnk_03002490;

    if (--t->unk28 == 0)
    {
        if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 63)
        {
            if (sub_08002ee8(gUnk_08741214[gUnk_03002490->unk74]) == 0)
                sub_0806395c(2);
            else
                sub_0806395c(1);
        }
        else
        {
            sub_0806395c(1);
        }
        gUnk_03002490->unk54 = 0;
        sub_08006148(sub_0807df14, gCurTaskIdx);
    }
}

void sub_0807e014(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807e060;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0874124C);
}

void sub_0807e044(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0874124C);
}

void sub_0807e060(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08741258);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807e090(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    t = gUnk_03002490;
    t->unk28 = gUnk_08741216[t->unk74];
    sub_080061c0(gUnk_08741218[t->unk74], 0x5A5A5A5A);
    while (1)
    {
        sub_08006338(7);
        TaskYieldTrampoline(12);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(12);
        sub_08006338(8);
        TaskYieldTrampoline(12);
        sub_08006338(6);
        TaskYieldTrampoline(12);
    }
}

void sub_0807e100(void)
{
    struct Task *t = gUnk_03002490;

    if (--t->unk28 == 0)
    {
        if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 63)
        {
            if (sub_08002ee8(gUnk_08741214[gUnk_03002490->unk74]) == 0)
                sub_0806395c(2);
            else
                sub_0806395c(1);
        }
        else
        {
            sub_0806395c(1);
        }
        gUnk_03002490->unk54 = 0;
        sub_08006148(sub_0807e044, gCurTaskIdx);
    }
}

void sub_0807e188(void)
{
    struct ActorSpawn spawn;

    gUnk_03002490->unk15 = 1;
    sub_08063e14();
    sub_08006338(5);
    TaskYieldTrampoline(6);
    sub_08006338(9);
    TaskYieldTrampoline(40);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    spawn.unk00 = 3;
    spawn.unk04 = 105;
    spawn.unk08 = 0;
    spawn.unk09 = 0;
    spawn.unk0C = 16;
    spawn.unk0E = 0;
    spawn.unk0A = 0;
    {
        struct Task *t;
        s32 id = sub_08064b5c(&spawn, 0);

        t = gUnk_03002490;
        t->unk46 = id;
        t->unk28 = 88;
    }
    do
        TaskYieldTrampoline(1);
    while (gUnk_03002490->unk28 != 0);
    sub_08006338(11);
    TaskYieldTrampoline(4);
    sub_08006338(9);
    TaskYieldTrampoline(4);
    sub_08006338(5);
    TaskYieldTrampoline(12);
    sub_0806395c(0);
    sub_08006138();
}

void sub_0807e244(void)
{
    {
        struct Task *t = gUnk_03002490;

        if (t->unk7A != 0 && t->unk28 > 0)
            t->unk28--;
    }
    {
        struct Task *t = gUnk_03002490;

        if (t->unk14 != 1)
            sub_08006148((void *)gUnk_08741220[t->unk73], gCurTaskIdx);
    }
}

void sub_0807e290(void)
{
    struct ActorSpawn spawn;

    gUnk_03002490->unk15 = 2;
    {
        struct Task *t;
        s32 r = sub_08063df4();

        t = gUnk_03002490;
        t->unk30 = r;
        if (r == 1)
            t->unk43 = 255;
        else
            t->unk43 = 1;
    }
    sub_08006338(5);
    TaskYieldTrampoline(6);
    sub_08006338(9);
    TaskYieldTrampoline(40);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    spawn.unk00 = 3;
    spawn.unk04 = 105;
    spawn.unk08 = 0;
    spawn.unk09 = 1;
    spawn.unk0C = 16;
    spawn.unk0E = 0;
    spawn.unk0A = 0;
    gUnk_03002490->unk46 = sub_08064b5c(&spawn, 0);
    TaskYieldTrampoline(30);
    sub_08006338(11);
    TaskYieldTrampoline(4);
    sub_08006338(9);
    TaskYieldTrampoline(4);
    sub_08006338(5);
    TaskYieldTrampoline(12);
    {
        struct Task *t = gUnk_03002490;

        if (t->unk7A != 0)
        {
            struct Task *u;

            t->unk7A = 0;
            u = gUnk_03002490;
            u->unk58 = -0x40000;
            u->unk60 = 0x4000;
            while (gUnk_03002490->unk7A == 0)
                TaskYieldTrampoline(1);
        }
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk58 = 0;
        t->unk60 = 0;
        t->unk43 = t->unk30;
    }
    sub_0806395c(0);
    sub_08006138();
}

void sub_0807e3b0(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 != 2)
        sub_08006148((void *)gUnk_08741220[t->unk73], gCurTaskIdx);
}

void sub_0807e3e4(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807e444;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741264);
}

void sub_0807e428(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741264);
}

void sub_0807e444(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08741268);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807e468(void)
{
    gUnk_03002490->unk15 = 0;
    sub_0807e484();
}

void sub_0807e480(void)
{
}

void sub_0807e484(void)
{
    sub_08006338(5);
    while (1)
    {
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(8);
    }
}

s32 sub_0807e4cc(void)
{
    struct Task *t;

    sub_08063e14();
    sub_08006338((s16)gUnk_03002490->unk3C);
    t = gUnk_03002490;
    sub_080061a8(-t->unk54, -t->unk5C, t->unk64);
    return 0;
}

s32 sub_0807e4fc(void)
{
    sub_0800622c(0, 0x2500, 0x30000);
    return 0;
}

s32 sub_0807e514(void)
{
    sub_08006244();
    return 0;
}

s32 sub_0807e520(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_0807e530(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    sub_08002e98(gUnk_03002490->unk73, 3, gUnk_0874126C);
}

void sub_0807e568(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_0807e5bc;
    t->unk38 = gUnk_0875235C;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741278);
}

void sub_0807e5a0(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741278);
}

void sub_0807e5bc(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 1, gUnk_0874127C);
    sub_08068e04();
    {
        struct Task *t = gUnk_03002490;

        if (t->unk7C == 3)
        {
            gUnk_03002490->unk46 = sub_08064fc4(0, 8, 1, 0, t->unk48, t->unk4A,
                                                t->unk40);
            sub_080b5540(gCurTaskIdx, gUnk_03002490->unk46);
        }
    }
    sub_08069b44();
}

void sub_0807e640(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk3C = 4;
    while (1)
    {
        gUnk_03002490->unk28 = 40;
        sub_08063e14();
        sub_080061c0(0x4000, 0x5A5A5A5A);
        while (--gUnk_03002490->unk28 >= 0)
        {
            gUnk_03002490->unk7A = 0;
            {
                struct Task *u = gUnk_03002490;

                u->unk58 = -0x10000;
                u->unk60 = 0x1000;
            }
            while (gUnk_03002490->unk7A == 0)
                TaskYieldTrampoline(1);
            gUnk_03002490->unk3E ^= 0x8000;
        }
    }
}

void sub_0807e6d0(void)
{
}

void sub_0807e6d4(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_0807e730;
    t->unk38 = gUnk_0875237C;
    sub_08063908(gUnk_0874183C);
    sub_0806395c(1);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08741280);
}

void sub_0807e714(void)
{
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08741280);
}

void sub_0807e730(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_08741288);
    if (gUnk_03002490->unk15 != 1)
        sub_08068e04();
    sub_08069b44();
}

void sub_0807e768(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08006338(4);
    while (1)
    {
        gUnk_03002490->unk28 = 40;
        sub_08063e14();
        sub_080061c0(0x4000, 0x5A5A5A5A);
        sub_08006338((s16)gUnk_03002490->unk3C);
        while (--gUnk_03002490->unk28 >= 0)
        {
            gUnk_03002490->unk7A = 0;
            {
                struct Task *u = gUnk_03002490;

                u->unk58 = -0x10000;
                u->unk60 = 0x1000;
            }
            while (gUnk_03002490->unk7A == 0)
                TaskYieldTrampoline(1);
            if ((s16)gUnk_03002490->unk3C == 6)
                sub_08006338(4);
            else
                sub_08006338(6);
        }
    }
}

void sub_0807e810(void)
{
}

void sub_0807e814(void)
{
    gUnk_03002490->unk15 = 1;
    sub_08063e14();
    gUnk_03002490->unk7A = 0;
    sub_080061c0(-0x8000, 0);
    {
        struct Task *t = gUnk_03002490;

        t->unk58 = -0x20000;
        t->unk60 = 0x2000;
    }
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
    }
}

void sub_0807e884(void)
{
    if (gUnk_03002490->unk7A != 0)
    {
        sub_0806395c(0);
        sub_08006148(sub_0807e714, gCurTaskIdx);
    }
}

void sub_0807e8b8(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_0807e920;
    t->unk38 = gUnk_0875235C;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741290);
}

void sub_0807e904(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741290);
}

void sub_0807e920(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08741294);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807e950(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk3C = 4;
    while (1)
    {
        gUnk_03002490->unk7A = 0;
        {
            struct Task *t = gUnk_03002490;

            t->unk58 = -0x10000;
            t->unk60 = 0x1000;
        }
        while (gUnk_03002490->unk7A == 0)
            TaskYieldTrampoline(1);
        gUnk_03002490->unk3E ^= 0x8000;
    }
}

void sub_0807e9b0(void)
{
}

s32 sub_0807e9b4(void)
{
    struct Task *t = gUnk_03002490;

    t->unk54 = -t->unk54;
    return 0;
}

s32 sub_0807e9c8(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_0807e9d8(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    {
        struct Task *t = gUnk_03002490;

        t->unk38 = gUnk_08752108;
        t->unk7A = 0;
    }
    sub_08063e14();
    gUnk_03002490->unk28 = sub_080640dc(gUnk_087412A8);
    sub_08002e98(gUnk_03002490->unk73, 4, gUnk_087412BC);
}

void sub_0807ea30(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807ea60;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087412CC);
}

void sub_0807ea60(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087412D0);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807ea84(void)
{
    gUnk_03002490->unk15 = 0;
    while (1)
    {
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x4000;
        TaskYieldTrampoline(8);
    }
}

void sub_0807ead4(void)
{
    gUnk_03002490->unk28 = sub_08064188(gUnk_03002490->unk28);
}

void sub_0807eaf0(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807eb20;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087412D4);
}

void sub_0807eb20(void)
{
    if ((u8)sub_0806956c() == 1)
    {
        struct Task *t = gUnk_03002490;

        t->unk58 = -t->unk58;
        t->unk7A = 0;
    }
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087412D8);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807eb60(void)
{
    gUnk_03002490->unk15 = 0;
    {
        struct Task *t = gUnk_03002490;

        t->unk58 = gUnk_08741298[t->unk74];
    }
    while (1)
    {
        gUnk_03002490->unk54 = -0x10000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk54 = -0x8000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk54 = 0x8000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk54 = 0x10000;
        TaskYieldTrampoline(2);
    }
}

void sub_0807ebc4(void)
{
    gUnk_03002490->unk28 = sub_08064188(gUnk_03002490->unk28);
}

void sub_0807ebe0(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807ec10;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087412DC);
}

void sub_0807ec10(void)
{
    if ((u8)sub_080695bc() == 1)
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -t->unk54;
    }
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087412E0);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807ec4c(void)
{
    gUnk_03002490->unk15 = 0;
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = gUnk_087412A0[t->unk74];
    }
    while (1)
    {
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = 0x10000;
        TaskYieldTrampoline(2);
    }
}

void sub_0807ecb0(void)
{
    gUnk_03002490->unk28 = sub_08064188(gUnk_03002490->unk28);
}

void sub_0807eccc(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807ecfc;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087412E4);
}

void sub_0807ecfc(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087412E8);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807ed20(void)
{
    gUnk_03002490->unk15 = 0;
    while (1)
    {
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk58 = -0xC000;
        TaskYieldTrampoline(96);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk58 = 0;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk58 = 0xC000;
        TaskYieldTrampoline(96);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk58 = 0;
        TaskYieldTrampoline(16);
    }
}

void sub_0807ed98(void)
{
    gUnk_03002490->unk28 = sub_08064188(gUnk_03002490->unk28);
}

void sub_0807edb4(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_087523EC;
    sub_08065848(0, 0);
    gUnk_03002490->unk43 = 255;
    gUnk_03002490->unk28 = sub_080640dc(gUnk_087412EC);
    sub_08002e98(gUnk_03002490->unk73, 2, gUnk_08741300);
}

void sub_0807ee14(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807ee60;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741308);
}

void sub_0807ee44(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741308);
}

void sub_0807ee60(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_0874130C);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807ee84(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080061c0(0x8000, 0x5A5A5A5A);
    sub_0807ef7c();
}

void sub_0807eea8(void)
{
    gUnk_03002490->unk28 = sub_08064188(gUnk_03002490->unk28);
}

void sub_0807eec4(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807ef24;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741310);
}

void sub_0807ef08(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08741310);
}

void sub_0807ef24(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08741314);
    sub_08068e04();
    sub_08069b44();
}

void sub_0807ef48(void)
{
    gUnk_03002490->unk15 = 0;
    sub_0807ef7c();
}

void sub_0807ef60(void)
{
    gUnk_03002490->unk28 = sub_08064188(gUnk_03002490->unk28);
}

void sub_0807ef7c(void)
{
    while (1)
    {
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x10000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(8);
    }
}

void sub_0807efec(void)
{
    gUnk_02007FB8[0]--;
}

void sub_0807effc(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752438;
    sub_08063e14();
    sub_08002e98(gUnk_03002490->unk73, 4, gUnk_08741380);
}
