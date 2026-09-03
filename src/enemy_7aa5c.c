/* game_code_and_rodata 0x0807AA5C-0x0807D3B0 (issue #77, module M20 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0807AA5C 0x0807D3B0 src/enemy_7aa5c.c --newpb
 *
 * The middle third of enemy/object behaviour bank 1 (see src/enemy_78b68.c for
 * the bank's three-table script pattern).  Twelve more scripts, among them:
 *   * the class-6 "swinging platform" pair `sub_0807aab8`+`sub_0807aae4` with
 *     its five-state `sub_0807a8fc` dispatch and `sub_0807aa5c`'s two-table
 *     velocity ramp;
 *   * task #12's eight-state script (`sub_0807b300`+`sub_0807b32c`) whose hook
 *     packs Task.unk4A into the low half of Task.unk24 and ORs 0x10000 in
 *     when the four-player flag `gUnk_03005550[4]` is out of range;
 *   * the eight-state class-8 rider `sub_0807b3f8`..`sub_0807b8ec`, which
 *     probes for a partner with `sub_08063eb0` over a stack `struct PointPair`
 *     and reacts through the shared `sub_0807b294` state entry;
 *   * the class-4 "conveyor" script `sub_0807c684`+`sub_0807c6b0` with the
 *     `sub_0807c5ac` box test (`struct Rect` + sub_08063BD4) and the
 *     `sub_0807c530` aim helper that clamps into `0x08740B3C`/`0x08740B60`;
 *   * `sub_0807c828`, which walks a sixteen-entry cue ring through
 *     `sub_0807c508` (`15 & (rand + Task.unk24)`);
 *   * the class-1 lift `sub_0807cc68`+`sub_0807cc9c` and the `sub_0807cbf4`
 *     spin-up (a 512-step angle from `sub_080642FC`).
 *
 * `sub_0807b888`, `sub_0807bd60`, `sub_0807c3d8`, `sub_0807cadc`,
 * `sub_0807ccec`, `sub_0807cdec`, `sub_0807cf64` and `sub_0807d0d8` are dead
 * exports: each is a copy of its host's tail dispatch that nothing in the ROM
 * references (curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s16 gUnk_0300244C;
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern s8 gUnk_02007FB8[];
extern struct Task *gUnk_03002490;
extern u8 gUnk_03005550[];
extern vs16 gUnk_03004CA0[];

/* ROM tables */
extern s16 gUnk_0874094C[];
extern s16 gUnk_08740B3C[];
extern s16 gUnk_08740B60[];
extern struct AnimCmd gUnk_08740854[];
extern struct AnimCmd gUnk_0874093C[];
extern struct AnimCmd gUnk_0874099C[];
extern struct AnimCmd gUnk_087409C0[];
extern struct AnimCmd gUnk_08740A98[];
extern struct AnimCmd gUnk_08740AAC[];
extern u32 gUnk_0825B350[];
extern u32 gUnk_0873F500[];
extern u32 gUnk_0873F720[];
extern u32 gUnk_08740864[];
extern u32 gUnk_087408AC[];
extern u32 gUnk_08740934[];
extern u32 gUnk_08740950[];
extern u32 gUnk_08740960[];
extern u32 gUnk_08740978[];
extern u32 gUnk_08740990[];
extern u32 gUnk_087409E4[];
extern u32 gUnk_087409FC[];
extern u32 gUnk_08740A1C[];
extern u32 gUnk_08740A3C[];
extern u32 gUnk_08740A40[];
extern u32 gUnk_08740A44[];
extern u32 gUnk_08740A5C[];
extern u32 gUnk_08740A74[];
extern u32 gUnk_08740A78[];
extern u32 gUnk_08740A80[];
extern u32 gUnk_08740A88[];
extern u32 gUnk_08740A90[];
extern u32 gUnk_08740AC8[];
extern u32 gUnk_08740AE0[];
extern u32 gUnk_08740AF8[];
extern u32 gUnk_08740AFC[];
extern u32 gUnk_08740B00[];
extern u32 gUnk_08740B08[];
extern u32 gUnk_08740B84[];
extern u32 gUnk_08740B94[];
extern u32 gUnk_08740BA4[];
extern u32 gUnk_08740BA8[];
extern u32 gUnk_08740BAC[];
extern u32 gUnk_08740BBC[];
extern u32 gUnk_08740BC0[];
extern u32 gUnk_08740BC4[];
extern u32 gUnk_08740BC8[];
extern u32 gUnk_08740BCC[];
extern u32 gUnk_08740BD0[];
extern u32 gUnk_08740E38[];
extern u32 gUnk_08740F50[];
extern u32 gUnk_08740F5C[];
extern u32 gUnk_08740FA4[];
extern u32 gUnk_08741088[];
extern u32 gUnk_0874108C[];
extern u32 gUnk_08741090[];
extern u32 gUnk_08741094[];
extern u32 gUnk_087410A0[];
extern u32 gUnk_087410AC[];
extern u32 gUnk_087410B0[];
extern u32 gUnk_087410B8[];
extern u32 gUnk_0874CB5C[];
extern u32 gUnk_08752234[];
extern u32 gUnk_08752680[];
extern u32 gUnk_087529D8[];
extern u32 gUnk_08752A24[];
extern u32 gUnk_08752B08[];
extern u32 gUnk_08752B4C[];
extern u32 gUnk_08752B8C[];
extern u32 gUnk_08752BA8[];

/* Externals */
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern s32 sub_08002ee8(s32 a);
extern s32 sub_080031b8(s32 id);
extern s32 sub_08021a40(s32 x, s32 y);
extern s32 sub_08063a2c(void);
extern s32 sub_08063bd4(struct PointPair *p);
extern s32 sub_08063cd0(void);
extern s32 sub_08063d2c(void);
extern s32 sub_08063eb0(struct PointPair *box, s32 i);
extern s32 sub_080640a8(u32 *p);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_0806415c(s32 n);
extern s32 sub_08064188(s32 n);
extern s32 sub_0806425c(s16 x0, s16 y0, s16 x1, s16 y1, s32 mode);
extern s32 sub_08064314(s32 prec);
extern s32 sub_08064984();
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064d34(u32 a, u32 b);
extern s32 sub_08066338();
extern s32 sub_08069b44(void);
extern u16 sub_080642fc(s32 a, s32 b);
extern u32 sub_08068e04(void);
extern u32 sub_080692fc(void);
extern u32 sub_0806a344(void);
extern u32 sub_0806bb7c(void);
extern u8 sub_08069604(void);
extern u8 sub_080699a8(void);
extern void TaskDispatchTrampoline(void);
extern void TaskYieldTrampoline(u32 frames);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_080059d8(void);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, u32 i);
extern void sub_0800617c(s16 a);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_08006214(void);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_0806395c(u32 v);
extern void sub_080639a4(u32 *p);
extern void sub_080639b4(u32 *p);
extern void sub_08063d7c(void);
extern void sub_08063e14(void);
extern void sub_08063fe0(void);
extern void sub_08063ff4(void);
extern void sub_080640c8(void);
extern void sub_0806421c(s16 t, s16 mag);
extern void sub_080645a4(s32 step, s32 limit);
extern void sub_08064680(s32 step, s32 limit, u16 dir);
extern void sub_08064a60(void);
extern void sub_0806523c(void);
extern void sub_080656b4(void);
extern void sub_08065760(u32 i, u32 p1, u8 p2);
extern void sub_08065848(u32 p0, s32 idx);
extern void sub_0806a0f0(s32 a);
extern void sub_0806ee2c(void);

/* Forward declarations */
extern s32 sub_0807a8d4();
extern s32 sub_0807a8fc();
extern s32 sub_0807a968();
extern s32 sub_0807aa0c();
void sub_0807aae4(void);
void sub_0807b32c(void);
void sub_0807b3dc(void);
void sub_0807b844(void);
void sub_0807b918(void);
void sub_0807bd14(void);
void sub_0807c0ac(void);
void sub_0807c0fc(void);
void sub_0807c394(void);
s32 sub_0807c5ac(struct Rect *r);
void sub_0807c6b0(void);
void sub_0807ca98(void);
void sub_0807cc9c(void);
void sub_0807cd9c(void);
void sub_0807cf20(void);
void sub_0807cff0(void);
void sub_0807d094(void);
void sub_0807d230(void);
void sub_0807d29c(void);
extern s32 sub_0807d3b0();

void sub_0807aa5c(void)
{
    s32 i;

    for (i = 0; i <= 16; i += 2)
    {
        gUnk_03002490->unk58 = gUnk_08740864[i] * gUnk_03002490->unk28;
        TaskYieldTrampoline(gUnk_08740864[i + 1]);
    }
    gUnk_03002490->unk28 = -gUnk_03002490->unk28;
    while (1)
    {
        for (i = 0; i <= 32; i += 2)
        {
            gUnk_03002490->unk58 = gUnk_087408AC[i] * gUnk_03002490->unk28;
            TaskYieldTrampoline(gUnk_087408AC[i + 1]);
        }
        gUnk_03002490->unk28 = -gUnk_03002490->unk28;
    }
}

void sub_0807aab8(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807aae4;
    sub_0807a8d4();
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_08740960);
}

void sub_0807aae4(void)
{
    sub_08002e98(gUnk_03002490->unk15, 6, gUnk_08740978);
    if (gUnk_03004CA0[gCurTaskIdx] != -1 && gUnk_03002490->unk14 != 0)
    {
        sub_08068e04();
        sub_08069b44();
        sub_0807aa0c();
    }
}

void sub_0807ab38(void)
{
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_08740960);
}

void sub_0807ab54(void)
{
    gUnk_03002490->unk15 = 2;
    sub_0807a8fc();
    sub_0807aa5c();
}

void sub_0807ab70(void)
{
    gUnk_03002490->unk34 = sub_0806415c(gUnk_03002490->unk34);
}

void sub_0807ab8c(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    gUnk_03002490->unk3C = 0xFFFF;
    sub_08006138();
}

void sub_0807abb4(void)
{
    if (sub_08064984(10) != 0)
    {
        sub_0806395c(1);
        sub_08006148(sub_0807ab38, gCurTaskIdx);
    }
}

void sub_0807abdc(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk34 = sub_080640dc(gUnk_08740854);
    sub_0807a968();
    sub_08006138();
}

void sub_0807ac08(void)
{
    gUnk_03002490->unk34 = sub_0806415c(gUnk_03002490->unk34);
    if ((sub_08063d2c() < 0 ? -sub_08063d2c() : sub_08063d2c()) <= 4)
    {
        sub_0806395c(2);
        sub_08006148(sub_0807ab38, gCurTaskIdx);
    }
}

void sub_0807ac58(void)
{
    struct Task *t;
    s32 v;

    gUnk_03002490->unk15 = 3;
    sub_080062c4();
    gUnk_03002490->unk38 = gUnk_08752680;
    sub_080639a4(gUnk_08740F50);
    t = gUnk_03002490;
    t->unk28 = 2;
    t->unk2C = 0;
    v = -t->unk43;
    t->unk30 = v;
    t->unk54 = v * gUnk_08740934[0];
    t->unk6C = 0;
    do
    {
        sub_08006338(4);
        TaskYieldTrampoline(2);
        sub_08006338(7);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 8;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
        sub_08006338(5);
        TaskYieldTrampoline(2);
        sub_08006338(6);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 8;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
        sub_08006338(6);
        TaskYieldTrampoline(2);
        sub_08006338(5);
        TaskYieldTrampoline(2);
    } while ((s16)++gUnk_03002490->unk6C <= 1);
    sub_08006338(6);
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 8;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 9;
    TaskYieldTrampoline(1);
    sub_08006338(5);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 8;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 9;
    TaskYieldTrampoline(1);
    sub_08006338(5);
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(12);
    sub_080062c4();
    sub_0806395c(4);
    sub_08006138();
}

void sub_0807adcc(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 != 3)
    {
        sub_08006148(sub_0807ab38, gCurTaskIdx);
    }
    else if (--t->unk28 <= 0)
    {
        t->unk2C ^= 1;
        t->unk54 = gUnk_08740934[t->unk2C] * t->unk30;
        t->unk28 = 2;
    }
}

void sub_0807ae1c(void)
{
    struct Task *t;
    s32 i;
    s32 j;

    gUnk_03002490->unk15 = 4;
    sub_080062c4();
    t = gUnk_03002490;
    t->unk28 = gUnk_0874094C[t->unk74];
    i = t->unk74;
    j = i * 2;
    t->unk2C = gUnk_08740950[j];
    t->unk30 = gUnk_08740950[j + 1];
    gUnk_03002490->unk34 = sub_080640dc(gUnk_0874093C);
    sub_08006138();
}

void sub_0807ae7c(void)
{
    struct Task *t;
    s32 n;

    gUnk_03002490->unk34 = sub_0806415c(gUnk_03002490->unk34);
    t = gUnk_03002490;
    n = t->unk28 - 1;
    t->unk28 = n;
    if (n <= 0)
    {
        sub_0806395c(5);
        sub_08006148(sub_0807ab38, gCurTaskIdx);
    }
    else if ((n & 7) == 0)
    {
        sub_080645a4(t->unk2C, t->unk30);
    }
}

void sub_0807aecc(void)
{
    gUnk_03002490->unk15 = 5;
    sub_080062c4();
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080061c0(0x20000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        sub_080061c0(-0x20000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
    } while ((s16)++gUnk_03002490->unk6C <= 5);
    sub_08006214();
    sub_080639a4(gUnk_08740F5C);
    sub_0806a344();
}

void sub_0807af3c(void)
{
    gUnk_03002490->unk34 = sub_0806415c(gUnk_03002490->unk34);
}

void sub_0807af58(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_087529D8;
    sub_08002e98(gUnk_03002490->unk73, 3, gUnk_08740990);
}

void sub_0807af98(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752A24;
    sub_08002e98(gUnk_03002490->unk73, 3, gUnk_08740990);
}

s32 sub_0807afd8(void)
{
    if (gUnk_03002490->unk73 != 0)
        return 0;
    sub_0806395c(7);
    sub_08006148(sub_0807b3dc, gCurTaskIdx);
    return 1;
}

s32 sub_0807b010(void)
{
    if (gUnk_03002490->unk73 != 0)
        return 0;
    sub_0806395c(0);
    sub_08006148(sub_0807b3dc, gCurTaskIdx);
    return 1;
}

s32 sub_0807b048(void)
{
    sub_0806a0f0(-2);
    return 1;
}

s32 sub_0807b058(void)
{
    sub_080062c4();
    gUnk_03002490->unk1C = 1;
    return 0;
}

s32 sub_0807b070(void)
{
    sub_080062c4();
    gUnk_03002490->unk1C = 1;
    return 0;
}

void sub_0807b088(void)
{
    if (gUnk_03002490->unk54 != 0 && (u8)sub_080699a8() != 0)
    {
        sub_080062c4();
        gUnk_03002490->unk1C = 1;
    }
}

void sub_0807b0b4(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk1C == 0 && --t->unk30 <= 0)
    {
        t->unk30 = 3;
        if (t->unk34 <= 4)
        {
            t->unk34++;
            t->unk54 = gUnk_087409E4[t->unk34] * t->unk2C;
        }
    }
}

void sub_0807b0f0(void)
{
    struct Task *t = gUnk_03002490;

    t->unk2C = t->unk43;
    t->unk30 = 1;
    t->unk34 = 0;
    sub_080062c4();
    sub_080031b8(196);
}

void sub_0807b11c(void)
{
    struct Task *t = gUnk_03002490;

    t->unk8C->unk2C = 0;
    t->unk34 = 0;
    t->unk30 = 0;
    t->unk2C = 0;
    t->unk28 = 0;
    t->unk18 = 0;
    t->unk1C = 0;
    sub_0806395c(0);
}

void sub_0807b144(void)
{
    if (gUnk_03002490->unk8C->unk2C == 0)
        gUnk_03002490->unk34 = sub_080640dc(gUnk_0874099C);
}

void sub_0807b16c(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 <= 0)
    {
        if (sub_08002ee8(4) == 0)
            goto other;
        t = gUnk_03002490;
    }
    t->unk30 = 90;
    sub_0806395c(5);
    return;

other:
    gUnk_03002490->unk28++;
    sub_0806395c(4);
}

void sub_0807b1a8(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 <= 0)
    {
        if (sub_08002ee8(4) == 0)
            goto other;
        t = gUnk_03002490;
    }
    t->unk30 = 90;
    sub_0806395c(5);
    return;

other:
    gUnk_03002490->unk28++;
    sub_0806395c(3);
}

void sub_0807b1e4(void)
{
    gUnk_03002490->unk46 = sub_08064d34(173, 1);
}

void sub_0807b200(void)
{
    struct PointPair box;
    s32 id = sub_08063a2c();

    if (id != -1)
    {
        struct Task *t = gUnk_03002490;

        box.x0 = t->unk48 - 30;
        box.y0 = t->unk4A - 48;
        box.x1 = t->unk48 + 30;
        box.y1 = t->unk4A + 48;
        if (sub_08063eb0(&box, id) != 0)
        {
            sub_0806395c(6);
            sub_08006148(sub_0807b3dc, gCurTaskIdx);
        }
    }
}

void sub_0807b294(void *fn)
{
    struct Actor *a = gUnk_03002490->unk8C;
    struct Task *t;

    sub_08063e14();
    t = gUnk_03002490;
    t->unk28 = 0;
    t->unk2C = t->unk43;
    t->unk30 = 1;
    t->unk34 = 0;
    a->unk2C = 0;
    if (sub_08002ee8(4) == 0)
    {
        sub_0806395c(1);
        sub_08006148(fn, gCurTaskIdx);
    }
    else
    {
        sub_0806395c(2);
        sub_08006148(fn, gCurTaskIdx);
    }
}

void sub_0807b300(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807b32c;
    sub_0807b11c();
    sub_08002e98(gUnk_03002490->unk14, 8, gUnk_087409FC);
}

void sub_0807b32c(void)
{
    u8 v = (u8)sub_080692fc();

    if ((gUnk_03002490->unk7A & 1) != 0)
    {
        if ((u8)(gUnk_03005550[4] - 1) > 3)
            gUnk_03002490->unk24 = (u16)gUnk_03002490->unk24 | 0x10000;
    }
    if ((gUnk_03002490->unk7A & 1) == 0)
        gUnk_03002490->unk24 = (u16)gUnk_03002490->unk24;
    if (v == 0)
    {
        sub_0807b088();
        sub_08002e98(gUnk_03002490->unk15, 8, gUnk_08740A1C);
    }
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        gUnk_03002490->unk24 = (gUnk_03002490->unk24 & 0xFFFF0000) | gUnk_03002490->unk4A;
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807b3dc(void)
{
    sub_08002e98(gUnk_03002490->unk14, 8, gUnk_087409FC);
}

void sub_0807b3f8(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    gUnk_03002490->unk1C = 0;
    sub_08063e14();
    sub_080061c0(0x8000, 0x5A5A5A5A);
    sub_0807b144();
    sub_08006138();
}

void sub_0807b430(void)
{
    struct Task *t;

    if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 63)
        sub_0807b294(sub_0807b3dc);
    else
        sub_0807b200();
    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
    t = gUnk_03002490;
    if (t->unk18 == 0)
    {
        if ((t->unk7B & 1) != 0)
            sub_0806a0f0(-2);
        gUnk_03002490->unk18 = 1;
    }
}

void sub_0807b49c(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080062c4();
    sub_08006338(15);
    TaskYieldTrampoline(16);
    sub_0806395c(3);
    sub_08006138();
}

void sub_0807b4c8(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_0807b3dc, gCurTaskIdx);
}

void sub_0807b4f0(void)
{
    gUnk_03002490->unk15 = 3;
    sub_0807b0f0();
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    sub_0807b1e4();
    gUnk_03002490->unk3C -= 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 16;
    TaskYieldTrampoline(8);
    sub_0807b16c();
    sub_08006138();
}

void sub_0807b558(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_0807b3dc, gCurTaskIdx);
    else
        sub_0807b0b4();
}

void sub_0807b584(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080062c4();
    sub_08006338(16);
    TaskYieldTrampoline(16);
    sub_0806395c(4);
    sub_08006138();
}

void sub_0807b5b0(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_0807b3dc, gCurTaskIdx);
}

void sub_0807b5d8(void)
{
    gUnk_03002490->unk15 = 4;
    sub_0807b0f0();
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(2);
    sub_0807b1e4();
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    sub_0807b1a8();
    sub_08006138();
}

void sub_0807b640(void)
{
    if (gUnk_03002490->unk14 != 4)
        sub_08006148(sub_0807b3dc, gCurTaskIdx);
    else
        sub_0807b0b4();
}

void sub_0807b66c(void)
{
    gUnk_03002490->unk15 = 5;
    sub_080062c4();
    gUnk_03002490->unk1C = 0;
    sub_08063e14();
    sub_0807b144();
    sub_080061c0(-0x8000, 0x5A5A5A5A);
    TaskYieldTrampoline(gUnk_03002490->unk30);
    sub_0806395c(0);
    sub_08006138();
}

void sub_0807b6b4(void)
{
    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_0807b3dc, gCurTaskIdx);
}

void sub_0807b6e8(void)
{
    gUnk_03002490->unk15 = 6;
    sub_080062c4();
    sub_0807b0f0();
    sub_0807b1e4();
    if (sub_08002ee8(4) != 0)
    {
        sub_08006338(10);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
    }
    else
    {
        sub_08006338(14);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 16;
        TaskYieldTrampoline(8);
    }
    sub_0806395c(0);
    sub_08006138();
}

void sub_0807b7a8(void)
{
    if (gUnk_03002490->unk14 != 6)
        sub_08006148(sub_0807b3dc, gCurTaskIdx);
}

void sub_0807b7d0(void)
{
    gUnk_03002490->unk15 = 7;
    sub_080062c4();
    sub_0800622c(0, 0x1500, 0x30000);
    sub_0807b144();
    sub_08006138();
}

void sub_0807b7fc(void)
{
}

void sub_0807b800(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807b844;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740A3C);
}

void sub_0807b844(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08740A40);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807b888(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740A3C);
}

void sub_0807b8a4(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    gUnk_03002490->unk34 = sub_080640dc(gUnk_087409C0);
    sub_08006138();
}

void sub_0807b8d0(void)
{
    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
}

void sub_0807b8ec(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807b918;
    sub_0807b11c();
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_08740A44);
}

void sub_0807b918(void)
{
    u8 v = (u8)sub_080692fc();

    if ((gUnk_03002490->unk7A & 1) != 0)
    {
        if ((u8)(gUnk_03005550[4] - 1) > 3)
            gUnk_03002490->unk24 = (u16)gUnk_03002490->unk24 | 0x10000;
    }
    if ((gUnk_03002490->unk7A & 1) == 0)
        gUnk_03002490->unk24 = (u16)gUnk_03002490->unk24;
    if (v == 0)
    {
        sub_0807b088();
        sub_08002e98(gUnk_03002490->unk15, 6, gUnk_08740A5C);
    }
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        gUnk_03002490->unk24 = (gUnk_03002490->unk24 & 0xFFFF0000) | gUnk_03002490->unk4A;
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807b9c8(void)
{
    gUnk_03002490->unk43 = 255;
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_08740A44);
}

void sub_0807b9ec(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    gUnk_03002490->unk43 = 255;
    gUnk_03002490->unk1C = 0;
    sub_0807b144();
    sub_08006138();
}

void sub_0807ba18(void)
{
    struct Task *t;

    if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 63)
        sub_0807b294(sub_0807b9c8);
    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
    t = gUnk_03002490;
    if (t->unk18 == 0)
    {
        if ((t->unk7B & 1) != 0)
            sub_0806a0f0(-2);
        gUnk_03002490->unk18 = 1;
    }
}

void sub_0807ba7c(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080062c4();
    sub_08006338(15);
    TaskYieldTrampoline(16);
    sub_0806395c(3);
    sub_08006138();
}

void sub_0807baa8(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_0807b9c8, gCurTaskIdx);
}

void sub_0807bad0(void)
{
    gUnk_03002490->unk15 = 3;
    sub_0807b0f0();
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    sub_0807b1e4();
    gUnk_03002490->unk3C -= 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 16;
    TaskYieldTrampoline(8);
    sub_0807b16c();
    sub_08006138();
}

void sub_0807bb38(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_0807b9c8, gCurTaskIdx);
}

void sub_0807bb60(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080062c4();
    sub_08006338(16);
    TaskYieldTrampoline(16);
    sub_0806395c(4);
    sub_08006138();
}

void sub_0807bb8c(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_0807b9c8, gCurTaskIdx);
}

void sub_0807bbb4(void)
{
    gUnk_03002490->unk15 = 4;
    sub_0807b0f0();
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(2);
    sub_0807b1e4();
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    sub_0807b1a8();
    sub_08006138();
}

void sub_0807bc1c(void)
{
    if (gUnk_03002490->unk14 != 4)
        sub_08006148(sub_0807b9c8, gCurTaskIdx);
}

void sub_0807bc44(void)
{
    gUnk_03002490->unk15 = 5;
    sub_080062c4();
    gUnk_03002490->unk1C = 0;
    sub_0807b144();
    TaskYieldTrampoline(gUnk_03002490->unk30);
    sub_0806395c(0);
    sub_08006138();
}

void sub_0807bc78(void)
{
    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_0807b9c8, gCurTaskIdx);
}

void sub_0807bcac(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_0874CB5C;
    sub_08002e98(gUnk_03002490->unk73, 1, gUnk_08740A74);
}

void sub_0807bcec(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807bd14;
    sub_08063e14();
    gUnk_03002490->unk3C = 4;
    sub_08006138();
}

void sub_0807bd14(void)
{
    sub_0806bb7c();
}

void sub_0807bd20(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752B08;
    sub_08002e98(gUnk_03002490->unk73, 2, gUnk_08740A78);
}

void sub_0807bd60(void)
{
    sub_08063e14();
    sub_08006148(sub_0807c0fc, gCurTaskIdx);
}

void sub_0807bd7c(void)
{
    s32 r;
    struct Task *t;

    sub_08063e14();
    r = sub_08002ee8(4);
    t = gUnk_03002490;
    t->unk24 = -1;
    t->unk18 = 0;
    t->unk1C = 0;
    t->unk20 = 0;
    if (r == 0)
        sub_0806395c(0);
    else
        sub_0806395c(2);
}

s32 sub_0807bdb8(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 == 5)
    {
        t->unk1C = 0;
        sub_080061c0(gUnk_08740A90[t->unk74], 0x5A5A5A5A);
    }
    sub_0806395c(1);
    sub_08006148(sub_0807c0fc, gCurTaskIdx);
    return 1;
}

s32 sub_0807be08(void)
{
    struct Task *t = gUnk_03002490;
    struct Actor *a = t->unk8C;
    u8 st = t->unk14;

    switch (st)
    {
    case 1:
        sub_0806395c(a->unk1C);
        break;
    case 2:
        sub_0806395c(3);
        break;
    case 3:
        if (gUnk_0300244C != 0 && a->unk2C == 0)
            gUnk_03002490->unk34 = sub_080640dc(gUnk_08740AAC);
        sub_0806395c(4);
        break;
    case 4:
        sub_0806395c(5);
        break;
    }
    if (gUnk_03002490->unk14 != st)
        sub_08006148(sub_0807c0fc, gCurTaskIdx);
    return 1;
}

s32 sub_0807be9c(void)
{
    sub_0806a0f0(-2);
    return 1;
}

s32 sub_0807beac(void)
{
    if (gUnk_03002490->unk14 == 5)
    {
        gUnk_03002490->unk1C = 16;
        sub_080062c4();
    }
    else
    {
        sub_08063ff4();
    }
    return 0;
}

s32 sub_0807bed4(void)
{
    if (gUnk_03002490->unk14 == 5)
    {
        gUnk_03002490->unk1C = 16;
        sub_080062c4();
    }
    else
    {
        sub_08063ff4();
    }
    return 0;
}

void sub_0807befc(void)
{
    gUnk_03002490->unk58 = 0;
}

void sub_0807bf0c(void)
{
    struct Task *t;

    sub_08063e14();
    t = gUnk_03002490;
    if (t->unk24 != 0)
    {
        t->unk20 = gUnk_08740A80[t->unk74];
        t->unk1C = gUnk_08740A88[t->unk74];
        gUnk_03002490->unk34 = sub_080640dc(gUnk_08740A98);
        gUnk_03002490->unk24 = 0;
        sub_080639b4(gUnk_0873F720);
    }
    sub_080061c0(0x8000, 0x5A5A5A5A);
}

void sub_0807bf74(void)
{
    sub_08063e14();
    sub_080062c4();
    sub_080640c8();
    gUnk_03002490->unk7A = 0;
    sub_0800622c(-0x30000, 0x2500, 0x30000);
    sub_08006338(7);
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk34 = sub_080640dc(gUnk_08740AAC);
}

void sub_0807bfd0(void)
{
    sub_08063e14();
    sub_080062c4();
    gUnk_03002490->unk7A = 0;
    sub_0800622c(-0x20000, 0x2500, 0x30000);
}

void sub_0807c000(void)
{
    sub_08063e14();
    sub_080062c4();
    gUnk_03002490->unk7A = 0;
    sub_0800622c(-0x10000, 0x2500, 0x30000);
}

void sub_0807c030(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk24 != 5)
    {
        t->unk20 = 60;
        t->unk1C = 0;
        t->unk18 = 150;
        t->unk24 = 5;
        sub_080061c0(gUnk_08740A90[t->unk74], 0x5A5A5A5A);
        sub_080639b4(gUnk_08740E38);
        sub_080031b8(199);
    }
}

void sub_0807c080(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807c0ac;
    sub_0807bd7c();
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_08740AC8);
}

void sub_0807c0ac(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 6, gUnk_08740AE0);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807c0fc(void)
{
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_08740AC8);
}

void sub_0807c118(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_0807bf0c();
    sub_08006138();
}

void sub_0807c138(void)
{
    struct Task *t;

    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
    t = gUnk_03002490;
    if (t->unk18 > 0)
        goto dec;
    if (--t->unk20 <= 0)
        goto reset;
    if ((sub_08063d2c() < 0 ? -sub_08063d2c() : sub_08063d2c()) <= 9)
    {
        if (--gUnk_03002490->unk1C > 0)
            return;

    reset:
        sub_0806395c(2);
        sub_08006148(sub_0807c0fc, gCurTaskIdx);
        return;
    }
    gUnk_03002490->unk1C = gUnk_08740A88[gUnk_03002490->unk74];
    return;

dec:
    t->unk18--;
}

void sub_0807c1d0(void)
{
    gUnk_03002490->unk15 = 1;
    sub_0800622c(0, 0x2500, 0x30000);
    sub_08006138();
}

void sub_0807c1f4(void)
{
    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
}

void sub_0807c210(void)
{
    gUnk_03002490->unk15 = 2;
    sub_0807bf74();
    sub_08006138();
}

void sub_0807c22c(void)
{
    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
}

void sub_0807c248(void)
{
    gUnk_03002490->unk15 = 3;
    sub_0807bfd0();
    sub_08006138();
}

void sub_0807c264(void)
{
    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
}

void sub_0807c280(void)
{
    gUnk_03002490->unk15 = 4;
    sub_0807c000();
    sub_08006138();
}

void sub_0807c29c(void)
{
    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
}

void sub_0807c2b8(void)
{
    gUnk_03002490->unk15 = 5;
    sub_0807c030();
    while (1)
    {
        sub_0806cc90(1, 1, 4, 4);
        TaskYieldTrampoline(8);
    }
}

void sub_0807c2e0(void)
{
    struct Task *t;

    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
    t = gUnk_03002490;
    if (t->unk1C <= 0)
    {
        if (--t->unk20 <= 0)
        {
            sub_0806395c(0);
            sub_08006148(sub_0807c0fc, gCurTaskIdx);
        }
    }
    else if (--t->unk1C <= 0)
    {
        sub_08063ff4();
        sub_080061c0(gUnk_08740A90[gUnk_03002490->unk74], 0x5A5A5A5A);
    }
}

void sub_0807c350(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807c394;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740AF8);
}

void sub_0807c394(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08740AFC);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807c3d8(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740AF8);
}

void sub_0807c3f4(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
    }
}

void sub_0807c440(void)
{
}

void sub_0807c444(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752B4C;
    sub_08002e98(gUnk_03002490->unk73, 2, gUnk_08740B00);
}

void sub_0807c484(void)
{
    if (--gUnk_02007FB8[0] < 0)
        sub_0806ee2c();
}

void sub_0807c4a0(void)
{
    struct Actor *a = gUnk_03002490->unk8C;
    u32 v;

    sub_08065848(1, 0);
    v = a->unk28;
    if (v == 0)
        v = gUnk_0825B350[2];
    sub_08065760(0, v, a->unk0C);
}

void sub_0807c4d4(void)
{
    struct Task *t;

    sub_08063e14();
    sub_0806395c(1);
    t = gUnk_03002490;
    t->unk1C = 0;
    t->unk20 = 0;
    t->unk24 = 0;
    gUnk_03002490->unk18 = sub_080640a8(gUnk_08740B08);
    sub_0807c4a0();
}

s32 sub_0807c508(void)
{
    s32 r = sub_08002ee8(10);

    if (r <= 4)
        r += 2;
    else
        r += 5;
    r += gUnk_03002490->unk24;
    return 15 & r;
}

void sub_0807c530(s32 a)
{
    struct Rect box;
    struct Task *t;

    gUnk_03002490->unk24 = a;
    sub_08063d7c();
    t = gUnk_03002490;
    t->unk2C = gUnk_08740B3C[t->unk24] + gUnk_030023B4;
    t->unk28 = gUnk_08740B60[t->unk24] + gUnk_030023D4;
    sub_0807c5ac(&box);
    gUnk_03002490->unk34 = (u16)sub_0806425c(box.left, box.top,
                                             (s16)gUnk_03002490->unk2C,
                                             (s16)gUnk_03002490->unk28, 3);
}

s32 sub_0807c5ac(struct Rect *r)
{
    struct Task *t = gUnk_03002490;

    r->left = t->unk48;
    r->top = t->unk4A;
    r->right = t->unk2C;
    r->bottom = t->unk28;
    if (sub_08063bd4((struct PointPair *)r) <= 64)
        return 1;
    else
        return 0;
}

void sub_0807c5e4(void)
{
    struct Task *t;

    sub_0806421c((s16)gUnk_03002490->unk34, 1024);
    t = gUnk_03002490;
    t->unk54 = gUnk_030023B4;
    t->unk58 = gUnk_030023D4;
}

void sub_0807c618(void)
{
    struct ActorSpawn spawn;
    struct Task *t = gUnk_03002490;

    if (sub_08021a40(t->unk48 + t->unk43 * 16, t->unk4A) == 0)
    {
        spawn.unk00 = 31;
        spawn.unk04 = 134;
        spawn.unk08 = 0;
        spawn.unk09 = 0;
        spawn.unk0C = 8;
        spawn.unk0E = 0;
        spawn.unk0A = 1;
        gUnk_03002490->unk46 = sub_08064b5c(&spawn, 0);
        sub_080031b8(165);
    }
}

void sub_0807c684(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807c6b0;
    sub_0807c4d4();
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_08740B84);
}

void sub_0807c6b0(void)
{
    sub_08002e98(gUnk_03002490->unk15, 4, gUnk_08740B94);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807c6f4(void)
{
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_08740B84);
}

void sub_0807c710(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    gUnk_03002490->unk6C = 0;
    while ((s16)gUnk_03002490->unk6C < gUnk_03002490->unk1C)
    {
        gUnk_03002490->unk54 = 0x8000;
        gUnk_03002490->unk58 = 0x2000;
        TaskYieldTrampoline(15);
        gUnk_03002490->unk54 = 0x10000;
        gUnk_03002490->unk58 = 0;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x8000;
        gUnk_03002490->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = -0x8000;
        gUnk_03002490->unk58 = 0x2000;
        TaskYieldTrampoline(15);
        gUnk_03002490->unk54 = -0x10000;
        gUnk_03002490->unk58 = 0;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = -0x8000;
        gUnk_03002490->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk6C++;
    }
    sub_0806395c(1);
    sub_08006138();
}

void sub_0807c7d8(void)
{
    gUnk_03002490->unk18 = sub_08064188(gUnk_03002490->unk18);
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_0807c6f4, gCurTaskIdx);
}

void sub_0807c80c(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080062c4();
    sub_08006138();
}

void sub_0807c828(void)
{
    struct Task *t;
    s32 v;

    gUnk_03002490->unk18 = sub_08064188(gUnk_03002490->unk18);
    t = gUnk_03002490;
    if (++t->unk1C == 3)
        t->unk1C = 0;
    t = gUnk_03002490;
    if (++t->unk20 == 4)
    {
        sub_08063e14();
        v = gUnk_03002490->unk43 == 1 ? 16 : 17;
    }
    else
    {
        v = sub_0807c508();
        if (v > 3)
        {
            if (v <= 11)
            {
                if (sub_08066338() != 0)
                    v = sub_0807c508();
            }
        }
    }
    sub_0807c530(v);
    sub_0806395c(2);
    sub_08006148(sub_0807c6f4, gCurTaskIdx);
}

void sub_0807c8b0(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080062c4();
    sub_0807c5e4();
    sub_08006138();
}

void sub_0807c8d0(void)
{
    struct Rect box;

    gUnk_03002490->unk18 = sub_08064188(gUnk_03002490->unk18);
    if ((u8)sub_0807c5ac(&box) != 0)
    {
        struct Task *t = gUnk_03002490;

        if (t->unk20 == 4)
        {
            t->unk20 = 0;
            sub_0806395c(3);
            sub_08006148(sub_0807c6f4, gCurTaskIdx);
        }
        else
        {
            struct Actor *a = t->unk8C;

            if (sub_08021a40(t->unk48, t->unk4A + ((s8 *)a->unk50)[2]) != 0)
                gUnk_03002490->unk1C = 0;
            {
                struct Task *u = gUnk_03002490;

                if (sub_08021a40(u->unk48 + ((s8 *)a->unk50)[5], u->unk4A) != 0)
                    gUnk_03002490->unk1C = 0;
            }
            {
                struct Task *u = gUnk_03002490;

                if (sub_08021a40(u->unk48 + ((s8 *)a->unk50)[4], u->unk4A) != 0)
                    gUnk_03002490->unk1C = 0;
            }
            sub_0806395c(0);
            sub_08006148(sub_0807c6f4, gCurTaskIdx);
        }
    }
}

void sub_0807c9b4(void)
{
    gUnk_03002490->unk15 = 3;
    sub_080062c4();
    sub_08063e14();
    gUnk_03002490->unk3C = 4;
    TaskYieldTrampoline(10);
    sub_08006338(12);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    sub_0807c618();
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(8);
    TaskYieldTrampoline(20);
    sub_0806395c(1);
    sub_08006138();
}

void sub_0807ca18(void)
{
    if (gUnk_03002490->unk14 != 3)
    {
        gUnk_03002490->unk18 = sub_080640a8(gUnk_08740B08);
        sub_08006148(sub_0807c6f4, gCurTaskIdx);
    }
}

void sub_0807ca50(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807ca98;
    sub_08063e14();
    sub_0807c4a0();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740BA4);
}

void sub_0807ca98(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08740BA8);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807cadc(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740BA4);
}

void sub_0807caf8(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    while (1)
    {
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C = 7;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(8);
    }
}

void sub_0807cbb0(void)
{
}

void sub_0807cbb4(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752BA8;
    sub_08002e98(gUnk_03002490->unk73, 4, gUnk_08740BAC);
}

void sub_0807cbf4(void)
{
    struct Task *t = gUnk_03002490;
    u16 v;

    if (t->unk28 == 1)
        t->unk18 = 1;
    else
        t->unk18 = 0;
    v = sub_080642fc(gUnk_03002490->unk1C, 3);
    sub_0806421c((v + 256) & 511, 102);
    gUnk_03002490->unk54 = gUnk_030023B4;
    sub_0800617c(9);
    TaskYieldTrampoline(12);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk18 = 1;
}

void sub_0807cc68(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807cc9c;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740BBC);
}

void sub_0807cc9c(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08740BC0);
    if (gUnk_03004CA0[gCurTaskIdx] != -1 && gUnk_03002490->unk18 != 0)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807ccec(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740BBC);
}

void sub_0807cd08(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_0807cbf4();
    gUnk_03002490->unk58 = -0x10000;
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
    }
}

void sub_0807cd60(void)
{
}

void sub_0807cd64(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807cd9c;
    sub_08063e14();
    sub_0806395c(0);
    gUnk_03002490->unk24 = 255;
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740BC4);
}

void sub_0807cd9c(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08740BC8);
    if (gUnk_03004CA0[gCurTaskIdx] != -1 && gUnk_03002490->unk18 != 0)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807cdec(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740BC4);
}

void sub_0807ce08(void)
{
    gUnk_03002490->unk15 = 0;
    sub_0807cbf4();
    gUnk_03002490->unk20 = 15;
    sub_080062c4();
    gUnk_03002490->unk58 = -0x18100;
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
    }
}

void sub_0807ce68(void)
{
    struct Task *t;

    if (gUnk_03002490->unk20 <= 0)
    {
        u16 v = sub_08064314(3);

        v = ((v + 32) & 511) >> 6;
        sub_08064680(0x4D00, 0x18100, v);
        gUnk_03002490->unk20 = 15;
    }
    t = gUnk_03002490;
    t->unk20--;
    if (t->unk24 <= 0)
    {
        sub_080639a4(gUnk_08740FA4);
        sub_08006148(sub_0806a344, gCurTaskIdx);
    }
    gUnk_03002490->unk24--;
}

void sub_0807cedc(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807cf20;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740BCC);
}

void sub_0807cf20(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08740BD0);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807cf64(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740BCC);
}

void sub_0807cf80(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
    }
}

void sub_0807cfcc(void)
{
}

void sub_0807cfd0(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_0807cff0;
    t->unk3C = 4;
    sub_08006138();
}

void sub_0807cff0(void)
{
    sub_08006148(sub_0806a344, gCurTaskIdx);
}

void sub_0807d008(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 9;
    t = gUnk_03002490;
    t->unk38 = gUnk_08752234;
    t->unk40 = (0xFFF & t->unk40) | 0xF000;
    sub_08002e98(t->unk73, 1, gUnk_08741088);
}

void sub_0807d060(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807d094;
    sub_08064a60();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_0874108C);
}

void sub_0807d094(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08741090);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807d0d8(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_0874108C);
}

void sub_0807d0f4(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk7A = 0;
    sub_080061c0(gUnk_08741094[gUnk_03002490->unk74], 0x5A5A5A5A);
    t = gUnk_03002490;
    t->unk58 = gUnk_087410A0[t->unk74];
    t->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk1C = 1;
    } while ((s16)++gUnk_03002490->unk6C <= 4);
    sub_08063fe0();
}

void sub_0807d178(void)
{
}

void sub_0807d17c(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 9;
    t = gUnk_03002490;
    t->unk38 = gUnk_08752B8C;
    sub_08002e98(t->unk73, 1, gUnk_087410AC);
}

s32 sub_0807d1bc(void)
{
    sub_0806395c(1);
    sub_08006148(sub_0807d29c, gCurTaskIdx);
    return 1;
}

s32 sub_0807d1dc(void)
{
    sub_0806395c(1);
    sub_08006148(sub_0807d29c, gCurTaskIdx);
    return 1;
}

void sub_0807d1fc(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807d230;
    sub_08064a60();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087410B0);
}

void sub_0807d230(void)
{
    if ((u8)sub_08069604() != 0)
    {
        sub_0806395c(1);
        sub_08006148(sub_0807d29c, gCurTaskIdx);
    }
    else
    {
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_087410B8);
    }
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807d29c(void)
{
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087410B0);
}

void sub_0807d2b8(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0x80000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 4;
    sub_08006138();
}

void sub_0807d2ec(void)
{
    if ((u8)(gUnk_03005550[4] - 1) <= 3)
    {
        sub_080062c4();
        sub_0806395c(1);
        sub_08006148(sub_0807d29c, gCurTaskIdx);
    }
}

void sub_0807d320(void)
{
    struct Task *t;

    gUnk_03002490->unk04 = 0;
    sub_080062c4();
    t = gUnk_03002490;
    t->unk48 += t->unk43 * 16;
    t->unk4C = t->unk48 << 16;
    t->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    sub_08063fe0();
}

void sub_0807d384(void)
{
}

void sub_0807d388(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_0807d3b0;
    t->unk18 = 0;
    TaskYieldTrampoline(3);
    TaskDispatchTrampoline();
}
