/* game_code_and_rodata 0x08091F9C-0x08093F64 (issue #67, module M25 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08091F9C 0x08093F64 src/enemy_91f9c.c --newpb
 *
 * M25's third and fourth boss scripts.  The third (entry sub_08091f08 in
 * src/enemy_91f08.c, table 0x08743ADC) starts here with sub_08091f9c, which
 * installs the per-frame body sub_08091ffc and the animation script
 * gUnk_08743AC8.  sub_08091ffc is the busiest body in the module: besides the
 * usual Task.unk15 dispatch it calls sub_080227a4 (the camera/room hook) on
 * entry, and when the row gUnk_08743A58[Task.unk34] is non-null it runs the
 * "hit the wall" transition - sub_0806914c, then Task.unk1C = Task.unk7E,
 * a re-seat of the actor at gUnk_030023B4 - Task.unk43 * 16, and a hand-off to
 * sub_080685ec / sub_08006148.
 *
 * States 0-12 follow as <body, guard> pairs.  sub_08092250 is the attack
 * chooser: it walks Task.unk6C over gUnk_08743A70[Task.unk74] rounds, and per
 * round stores |sub_08063cd0()| in gUnk_03001F2C, classifies it into
 * gUnk_02007D00[6] (0/1/2) against the RNG, and plays one of three yield
 * sequences; sub_080926fc is the three-phase charge, sub_08092cdc the
 * multi-hit dive, sub_080930ac the four-way finisher whose case 3 spawns the
 * actor 154 at gUnk_030023B4/gUnk_030023D4, and sub_08093380 the defeat
 * sequence.  sub_080934b8 is the shake helper the first states yield to and
 * sub_080934f8 is the collision probe: ten sub_08021bb4 samples along
 * gUnk_08743AB8, mapped through the terrain-class table gUnk_087339F0 into a
 * two-bit result that picks the next Task.unk28 direction from gUnk_08743AC2.
 * sub_0809364c / sub_080936a0 / sub_08093780 are the shared step sequences,
 * sub_080937d0 the hit hook, sub_08093858 the four-instruction "stop moving"
 * leaf the census had missed, and sub_0809397c the companion body.
 *
 * The fourth boss starts at sub_08093a24 (table 0x087441A4, graphics
 * gUnk_08752F60): sub_08093a64 installs sub_08093a98 as its body,
 * sub_08093ac8 is its one state, sub_08093bd4 / sub_08093c30 are the second
 * entry pair (graphics gUnk_08753160, Actor.unk38 = 0x20E), sub_08093ccc and
 * sub_08093dcc are the endless spawners that call sub_08064cdc(181, -8, -8, 1)
 * every six frames, and sub_08093cf8 / sub_08093e58 / sub_08093f00 are the
 * companions that copy the boss's 16.16 position (±8 rows) and expire with it.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_02007D00[];
extern s32 gUnk_03001F2C;
extern u16 gUnk_03001EA4;
extern struct PlayerState gUnk_03002170[];
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern u16 gUnk_03002360;
extern vs16 gUnk_03004CA0[];

/* ROM tables */
extern u8 gUnk_08743A70[];
extern u8 gUnk_08743A8E[];
extern s32 gUnk_08743A9C[];
extern u32 gUnk_08743AA4[];
extern u32 gUnk_08743AAC[];
extern u8 gUnk_08743AB4[];
extern s8 gUnk_08743AB8[];
extern s8 gUnk_08743AC2[];
extern s8 gUnk_087339F0[];
extern u32 gUnk_0874410C[];
extern u32 gUnk_08744170[];
extern u32 gUnk_08744174[];
extern u32 gUnk_08744178[];
extern u32 gUnk_08752F60[];
extern u32 gUnk_0874430C[];
extern u32 gUnk_08753160[];
extern u32 gUnk_087441A4[];
extern u32 gUnk_087441AC[];
extern u32 gUnk_0874417C[][2];
extern u32 gUnk_0874418C[][2];
extern u32 gUnk_08744324[];
extern u32 gUnk_0874419C[];
extern u32 gUnk_087441B4[];
extern u8 gUnk_08743B48[];
extern u32 gUnk_087536FC[];
extern vu8 gUnk_03005550;
extern u32 gUnk_08743A94[];
extern u32 gUnk_08743A74[];
extern u8 gUnk_08743A7C[];
extern u8 gUnk_08743A82[];
extern u8 gUnk_08743A88[];
extern u32 gUnk_08743A10[];
extern u32 gUnk_08743A28[];
extern u32 gUnk_08743A40[];
extern u32 gUnk_08743A58[];
extern u32 gUnk_08743B14[];
extern void *gUnk_082959A8;
extern struct AnimCmd gUnk_08743AC8[];
extern u32 gUnk_08743AE0[];
extern struct AnimCmd gUnk_0874397C[];
extern u32 gUnk_08743988[];
extern u32 gUnk_087439A4[];
extern u32 gUnk_087440F4[];
extern struct AnimCmd *gUnk_08743A00[];
extern u32 gUnk_08743ADC[];
extern u32 gUnk_087535FC[];
extern u32 gUnk_08753128[];
extern u32 gUnk_08753148[];
extern u8 gUnk_087438DC[];
extern u32 gUnk_087438E4[];
extern u32 gUnk_087438EC[];
extern u32 gUnk_0874391C[];
extern u32 gUnk_0874394C[];
extern struct GfxHeader gUnk_0827565C;
extern u32 gUnk_08275670;

/* Externals */
extern void sub_080227a4(struct Task *t);
extern u32 sub_0806914c(s32 a);
extern void sub_08063ddc(u32 i);
extern u16 sub_0806660c(u16 a);
extern u16 sub_080665fc(void);
extern u8 sub_08064358(void);
extern s32 sub_08064d6c(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern s32 sub_08064cdc(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern void sub_08068920(s32 i, u8 c);
extern s32 sub_08021bb4(s16 x, s16 y, s32 c, s32 d);
extern void sub_08063d4c(u32 i);
extern void sub_080685ec(s32 i, s32 j, u8 c);
extern s32 sub_08063b38(void);
extern void TaskYieldTrampoline(u32 a);
extern void TaskDispatchTrampoline(void);
extern void sub_08064a60(void);
extern u16 sub_08066630(u16 a);
extern void sub_080651b4(void);
extern void sub_080059d8(void);
extern void sub_0806523c(void);
extern void sub_08068e04(void);
extern void sub_08063ff4(void);
extern void sub_080689c8(s32 i, s32 d);
extern void sub_080653ec(void);
extern void sub_080656b4(void);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern void sub_080031b8(s32 id);
extern void sub_08005654(s32 id);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_080061a8(s32 a, s32 b, s32 c);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006214(void);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006304(void);
extern void sub_08006338(s32 a);
extern void sub_080261d4(s32 a);
extern void sub_0806395c(u16 v);
extern void sub_080639a4(u32 *p);
extern void sub_080639b4(u32 v);
extern void sub_080639f0(u32 v);
extern void sub_08063a00(u32 v);
extern s32 sub_08063cd0(void);
extern s32 sub_08063d2c(void);
extern void sub_08063e14(void);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_080640fc(void);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064c1c(u32 type, int xArg, int yArg, int prioArg);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern u16 sub_08066088(u32 mode);
extern void sub_080663f4(void *src, u32 size);
extern void sub_08066468(void);
extern void sub_08066480(struct GfxHeader *h, u32 src, u32 size);
extern void sub_080664cc(struct GfxHeader *h);
extern void sub_08066580(void);
extern void sub_080666cc(struct AnimCmd *p);
extern void sub_080667c0(u8 a, u16 b);
extern void sub_0806684c(void);
extern void sub_08066ae0(void);
extern u8 sub_08067060(void);
extern s32 sub_08067120(s16 x, s16 y, u16 dir, u8 p8);
extern void sub_08068f68(void);
extern u32 sub_08068cf8(s32 a);
extern u8 sub_080692fc(void);
extern s32 sub_08069b44(void);
extern u8 sub_0806acf8(void);
extern void sub_0806ad18(void);
extern void sub_0806a344(void);
extern s16 sub_0806caa0(u8 kind, s32 dx, s32 dy);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern void sub_0806cffc(s16 dx, s16 dy);
extern s32 Div(s32 numerator, s32 denominator);

/* Defined below */
void sub_08091fe0(void);
void sub_08091ffc(void);
void sub_080934b8(void);
void sub_080934f8(void);
void sub_0809364c(void);
void sub_080936a0(void);
void sub_08093780(void);
void sub_08093a00(s32 a);
void sub_0809397c(void);
void sub_08093a98(void);
void sub_08093c7c(void);
void sub_08093c60(void);
void sub_08091f9c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_08091ffc;
    sub_080666cc(gUnk_08743AC8);
    u = gUnk_03002490;
    u->unk24 = u->unk8C->unk28;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 13, gUnk_08743AE0);
}

void sub_08091fe0(void)
{
    sub_08002e98(gUnk_03002490->unk14, 13, gUnk_08743AE0);
}

void sub_08091ffc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    s32 m;

    sub_080227a4(gUnk_03002490);
    t = gUnk_03002490;
    if (t->unk20 != 0)
    {
        t->unk20--;
        sub_08002e98(t->unk15, 13, gUnk_08743B14);
    }
    else if (sub_0806acf8() == 0)
    {
        if (sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 13, gUnk_08743B14);
    }
    else
    {
        sub_08002e98(gUnk_03002490->unk15, 13, gUnk_08743B14);
    }
    u = gUnk_03002490;
    if (u->unk30 == 1)
    {
        if (u->unk75 != 0)
            sub_080663f4(&gUnk_082959A8, 16);
        else
        {
            u->unk30 = 0;
            sub_08066468();
        }
    }
    sub_080639b4(gUnk_08743A10[gUnk_03002490->unk34]);
    sub_080639f0(gUnk_08743A28[gUnk_03002490->unk34]);
    sub_08063a00(gUnk_08743A40[gUnk_03002490->unk34]);
    sub_08068f68();
    sub_08069b44();
    if (gUnk_08743A58[gUnk_03002490->unk34] != 0)
    {
        sub_0806914c(gUnk_08743A58[gUnk_03002490->unk34]);
        v = gUnk_03002490;
        if (v->unk7C == 8)
        {
            v->unk1C = v->unk7E;
            sub_08063ddc(v->unk1C);
            w = gUnk_03002490;
            w->unk34 = 5;
            w->unk18 = 0;
            sub_08063d4c(w->unk1C);
            x = gUnk_03002490;
            x->unk48 = gUnk_030023B4 - x->unk43 * 16;
            m = x->unk48;
            x->unk4C = m << 16;
            sub_080227a4(x);
            y = gUnk_03002490;
            y->unk8C->unk28 = y->unk24;
            sub_08006338(36);
            if (gUnk_03002490->unk1C == gUnk_03002360)
                sub_080031b8(0x23D);
            sub_080685ec(gUnk_03002490->unk1C, gCurTaskIdx, 3);
            sub_0806395c(10);
            sub_08006148(sub_08091fe0, gCurTaskIdx);
        }
    }
}

void sub_08092198(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 0;
    sub_08006338(58);
    u = gUnk_03002490;
    u->unk60 = 9472;
    u->unk7A = 0;
    TaskYieldTrampoline(1);
    if (gUnk_03002490->unk7A == 0)
    {
        do
            TaskYieldTrampoline(1);
        while (gUnk_03002490->unk7A == 0);
        sub_080261d4(2);
        sub_080031b8(0x1F7);
    }
    sub_080062c4();
    sub_08066580();
    sub_08006338(59);
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    sub_0806395c(1);
    sub_08006138();
}

void sub_08092228(void)
{
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_08091fe0, gCurTaskIdx);
}
void sub_08092250(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 r;
    s32 zero;

    t = gUnk_03002490;
    t->unk15 = 1;
    u = gUnk_03002490;
    u->unk6C = 0;
    while ((s16)gUnk_03002490->unk6C < gUnk_08743A70[gUnk_03002490->unk74])
    {
        gUnk_02007D00[4] = sub_08063b38();
        sub_08063ddc(gUnk_02007D00[4]);
        gUnk_02007D00[6] = sub_08002ee8(8);
        gUnk_03001F2C = (sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0());
        if (gUnk_03001F2C <= 43)
        {
            if (gUnk_02007D00[6] <= 5)
                gUnk_02007D00[6] = 1;
            else
                gUnk_02007D00[6] = 0;
        }
        else if (gUnk_03001F2C <= 87)
        {
            if (gUnk_02007D00[6] <= 4)
                gUnk_02007D00[6] = 1;
            else
                gUnk_02007D00[6] = 0;
        }
        else if (gUnk_02007D00[6] <= 3)
        {
            gUnk_02007D00[6] = 0;
        }
        else if (gUnk_02007D00[6] <= 4)
        {
            gUnk_02007D00[6] = 1;
        }
        else
        {
            gUnk_02007D00[6] = 2;
        }
        switch (gUnk_02007D00[6])
        {
        case 0:
            gUnk_03002490->unk54 = 0;
            sub_08006338(4);
            TaskYieldTrampoline(8);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(7);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(7);
            break;
        case 1:
            sub_080061c0(-gUnk_08743A74[gUnk_03002490->unk74], 0x5A5A5A5A);
            sub_08006338(8);
            TaskYieldTrampoline(2);
            sub_08006338(16);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            break;
        case 2:
            sub_080061c0(gUnk_08743A74[gUnk_03002490->unk74], 0x5A5A5A5A);
            sub_08006338(8);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            sub_08006338(8);
            break;
        }
        gUnk_03002490->unk6C++;
    }
    v = gUnk_03002490;
    zero = 0;
    v->unk54 = zero;
    if (++v->unk2C > 2)
    {
        v->unk2C = zero;
        sub_0806395c(2);
    }
    else
    {
        gUnk_02007D00[6] = gUnk_03002170[gUnk_02007D00[4]].unk04;
        if (gUnk_02007D00[6] == 14)
            gUnk_02007D00[5] = 2;
        else if (gUnk_02007D00[6] == 4)
            gUnk_02007D00[5] = 1;
        else
            gUnk_02007D00[5] = zero;
        if (gUnk_03002490->unk74 != 0)
            gUnk_02007D00[5] += 3;
        r = sub_08002ee8(8);
        gUnk_02007D00[6] = r;
        if (r < gUnk_08743A7C[gUnk_02007D00[5]])
            sub_0806395c(3);
        else if (r < gUnk_08743A82[gUnk_02007D00[5]])
            sub_0806395c(4);
        else if (r < gUnk_08743A88[gUnk_02007D00[5]])
            sub_0806395c(6);
        else
            sub_0806395c(7);
    }
    sub_08006138();
}
void sub_08092590(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_08091fe0, gCurTaskIdx);
}

void sub_080925b8(void)
{
    struct Task *t;
    struct ActorSpawn spawn;

    t = gUnk_03002490;
    t->unk15 = 2;
    sub_08063e14();
    sub_08006338(29);
    TaskYieldTrampoline(12);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    spawn.unk00 = 30;
    spawn.unk04 = 133;
    spawn.unk08 = 0;
    spawn.unk09 = gUnk_03002490->unk74;
    spawn.unk0C = 0xFFFE;
    spawn.unk0E = 0;
    spawn.unk10 = sub_0806660c(1);
    sub_08064b5c(&spawn, 1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(24);
    spawn.unk00 = 30;
    spawn.unk04 = 133;
    spawn.unk08 = 1;
    spawn.unk09 = gUnk_03002490->unk74;
    spawn.unk0C = 0xFFFE;
    spawn.unk0E = 0;
    spawn.unk10 = sub_0806660c(1);
    spawn.unk0A = 0;
    sub_08064b5c(&spawn, 1);
    TaskYieldTrampoline(4);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(4);
    } while ((s16)++gUnk_03002490->unk6C <= 1);
    sub_0806395c(1);
    sub_08006138();
}

void sub_080926d4(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_08091fe0, gCurTaskIdx);
}
void sub_080926fc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *u3;
    struct Task *v;
    struct Task *v2;
    struct Task *v3;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    s32 zero;

    t = gUnk_03002490;
    t->unk15 = 3;
    sub_08063e14();
    sub_080031b8(0x23F);
    gUnk_03002490->unk6C = 0;
    while ((s16)gUnk_03002490->unk6C < gUnk_08743A8E[gUnk_03002490->unk74])
    {
        sub_080061c0(65536, 0x5A5A5A5A);
        sub_08006338(17);
        TaskYieldTrampoline(2);
        if (gUnk_03002490->unk7A != 0)
            sub_0806cc90(1, 1, -24, 24);
        sub_080061c0(0xFFFE0000, 0x5A5A5A5A);
        u = gUnk_03002490;
        u->unk24 = u->unk8C->unk28;
        u->unk8C->unk28 = 0;
        u->unk3C++;
        TaskYieldTrampoline(2);
        v = gUnk_03002490;
        v->unk8C->unk28 = v->unk24;
        gUnk_03002490->unk6C++;
    }
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080061c0(65536, 0x5A5A5A5A);
        sub_08006338(19);
        TaskYieldTrampoline(2);
        if (gUnk_03002490->unk7A != 0)
            sub_0806cc90(1, 1, -24, 24);
        sub_080061c0(0xFFFE0000, 0x5A5A5A5A);
        u2 = gUnk_03002490;
        u2->unk24 = u2->unk8C->unk28;
        u2->unk8C->unk28 = 0;
        u2->unk3C++;
        TaskYieldTrampoline(2);
        v2 = gUnk_03002490;
        v2->unk8C->unk28 = v2->unk24;
    } while ((s16)++gUnk_03002490->unk6C <= 0);
    gUnk_03002490->unk6C = 0;
    while ((s16)gUnk_03002490->unk6C < gUnk_08743A8E[gUnk_03002490->unk74])
    {
        sub_080061c0(65536, 0x5A5A5A5A);
        sub_08006338(21);
        TaskYieldTrampoline(2);
        if (gUnk_03002490->unk7A != 0)
            sub_0806cc90(1, 1, -24, 24);
        sub_080061c0(0xFFFE0000, 0x5A5A5A5A);
        u3 = gUnk_03002490;
        u3->unk24 = u3->unk8C->unk28;
        u3->unk8C->unk28 = 0;
        u3->unk3C++;
        TaskYieldTrampoline(2);
        v3 = gUnk_03002490;
        v3->unk8C->unk28 = v3->unk24;
        gUnk_03002490->unk6C++;
    }
    w = gUnk_03002490;
    zero = 0;
    w->unk54 = zero;
    w->unk3C++;
    TaskYieldTrampoline(12);
    sub_080031b8(500);
    x = gUnk_03002490;
    x->unk34 = 3;
    gUnk_03002790[sub_08064c1c(200, x->unk48, x->unk4A, sub_080665fc())].unk74 =
        gUnk_03002490->unk74;
    gUnk_03002490->unk18 = 1;
    sub_080061c0(gUnk_08743A94[gUnk_03002490->unk74], 0x5A5A5A5A);
    gUnk_03002490->unk6C = zero;
    do
    {
        sub_08006338(24);
        TaskYieldTrampoline(2);
        sub_08006338(26);
        TaskYieldTrampoline(2);
    } while ((s16)++gUnk_03002490->unk6C <= 9);
    sub_080061c0(0x5A5A5A5A, 0xFFFFC000);
    while (1)
    {
        TaskYieldTrampoline(1);
        y = gUnk_03002490;
        if (y->unk43 == 1)
        {
            if (y->unk54 <= 0)
                break;
        }
        else if (y->unk54 >= 0)
        {
            break;
        }
    }
    sub_08006214();
    gUnk_03002490->unk34 = 1;
    sub_0806395c(8);
    sub_08006138();
}
void sub_080929ec(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_08091fe0, gCurTaskIdx);
}

void sub_08092a14(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gUnk_03002490;
    t->unk15 = 4;
    sub_08006338(60);
    TaskYieldTrampoline(6);
    v = gUnk_03002490;
    v->unk3C--;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk7A = 0;
    u = gUnk_03002490;
    u->unk58 = gUnk_08743A9C[u->unk74];
    u->unk60 = 9472;
    if (u->unk58 < 0)
    {
        do
        {
            if ((gUnk_03001EA4 & 2) != 0)
                sub_08006338(52);
            else
                sub_08006338(61);
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk58 < 0);
    }
    sub_08006244();
    w = gUnk_03002490;
    if (w->unk74 == 0)
    {
        gUnk_02007D00[4] = 2;
        w->unk6C = 0;
        do
        {
            sub_080934b8();
            TaskYieldTrampoline(1);
        } while ((s16)++gUnk_03002490->unk6C <= 59);
    }
    if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 31)
        sub_0806395c(8);
    else if ((sub_08063d2c() < 0 ? -sub_08063d2c() : sub_08063d2c()) <= 15)
        sub_0806395c(3);
    else
        sub_0806395c(5);
    sub_08006138();
}

void sub_08092b30(void)
{
    if (gUnk_03002490->unk14 != 4)
        sub_08006148(sub_08091fe0, gCurTaskIdx);
}

void sub_08092b58(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk15 = 5;
    sub_080061c0(gUnk_08743AA4[gUnk_03002490->unk74], 0x5A5A5A5A);
    if (sub_08064358() == 1)
        gUnk_03002490->unk58 = 49152;
    gUnk_02007D00[4] = 2;
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080934b8();
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 119);
    sub_0806395c(8);
    sub_08006138();
}

void sub_08092bd8(void)
{
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_08091fe0, gCurTaskIdx);
}

void sub_08092c00(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk15 = 6;
    sub_08063e14();
    sub_08006338(60);
    TaskYieldTrampoline(4);
    v = gUnk_03002490;
    v->unk3C--;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk7A = 0;
    sub_080061c0(gUnk_08743AAC[gUnk_03002490->unk74], 0x5A5A5A5A);
    u = gUnk_03002490;
    u->unk58 = 0xFFFC0000;
    u->unk60 = 9472;
    u->unk6C = 0;
    do
    {
        if ((gUnk_03001EA4 & 2) != 0)
            sub_08006338(52);
        else
            sub_08006338(61);
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 13);
    sub_0806395c(8);
    sub_08006138();
}

void sub_08092cb4(void)
{
    if (gUnk_03002490->unk14 != 6)
        sub_08006148(sub_08091fe0, gCurTaskIdx);
}
void sub_08092cdc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *p;

    t = gUnk_03002490;
    t->unk15 = 7;
    sub_08063e14();
    sub_08006338(60);
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(10);
    gUnk_02007D00[4] =
        gUnk_08743AB4[sub_08002ee8(2) + gUnk_03002490->unk74 * 2];
    gUnk_03002490->unk6C = 0;
    while ((s16)gUnk_03002490->unk6C < gUnk_02007D00[4])
    {
        gUnk_03002490->unk7A = 0;
        v = gUnk_03002490;
        v->unk58 = 0xFFFB0000;
        v->unk60 = 20480;
        v->unk6C = 0;
        do
        {
            if ((gUnk_03001EA4 & 2) != 0)
                sub_08006338(52);
            else
                sub_08006338(61);
            TaskYieldTrampoline(1);
        } while ((s16)++gUnk_03002490->unk6C <= 13);
        while (gUnk_03002490->unk7A == 0)
        {
            if ((gUnk_03001EA4 & 2) != 0)
                sub_08006338(58);
            else
                sub_08006338(57);
            TaskYieldTrampoline(1);
        }
        sub_08006244();
        sub_080261d4(2);
        sub_080031b8(0x1F7);
        sub_08006338(59);
        TaskYieldTrampoline(5);
        gUnk_03002490->unk6C++;
    }
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    p = &gUnk_03002790[sub_08063b38()];
    if (p->unk7A == 0)
        sub_0806395c(4);
    else
        sub_0806395c(3);
    sub_08006138();
}
void sub_08092e40(void)
{
    if (gUnk_03002490->unk14 != 7)
        sub_08006148(sub_08091fe0, gCurTaskIdx);
}

void sub_08092e68(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 8;
    u = gUnk_03002490;
    u->unk60 = 9472;
    if (u->unk7A == 0)
    {
        do
        {
            if ((gUnk_03001EA4 & 2) != 0)
                sub_08006338(58);
            else
                sub_08006338(57);
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk7A == 0);
        sub_080261d4(2);
        sub_080031b8(0x1F7);
    }
    sub_080062c4();
    sub_08006338(59);
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    sub_0806395c(1);
    sub_08006138();
}

void sub_08092f04(void)
{
    if (gUnk_03002490->unk14 != 8)
        sub_08006148(sub_08091fe0, gCurTaskIdx);
}

void sub_08092f2c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk15 = 9;
    sub_080062c4();
    sub_080261d4(4);
    sub_080031b8(0x1F7);
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0xFFFF0000, 0x5A5A5A5A);
    u = gUnk_03002490;
    u->unk58 = 0xFFFD0000;
    u->unk60 = 9472;
    sub_08006338(63);
    TaskYieldTrampoline(8);
    v = gUnk_03002490;
    v->unk3C--;
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_080261d4(4);
    sub_080031b8(0x1F7);
    sub_0806cc90(0, 4, 24, 24);
    sub_08006244();
    sub_080061c0(0xFFFF8000, 0x5A5A5A5A);
    TaskYieldTrampoline(30);
    gUnk_03002490->unk54 = 0;
    sub_08093780();
    sub_0806395c(11);
    sub_08006138();
}

void sub_08092ff4(void)
{
    if (gUnk_03002490->unk14 != 9)
        sub_08006148(sub_08091fe0, gCurTaskIdx);
}

void sub_0809301c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 11;
    sub_080061c0(0xFFFFC000, 0x5A5A5A5A);
    sub_08006338(48);
    TaskYieldTrampoline(4);
    gUnk_03002490->unk54 = 0;
    sub_08006338(48);
    TaskYieldTrampoline(10);
    sub_08006338(50);
    TaskYieldTrampoline(8);
    sub_08006338(43);
    TaskYieldTrampoline(16);
    u = gUnk_03002490;
    u->unk34 = 1;
    sub_0806395c(1);
    sub_08006138();
}

void sub_08093084(void)
{
    if (gUnk_03002490->unk14 != 11)
        sub_08006148(sub_08091fe0, gCurTaskIdx);
}
void sub_080930ac(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct Task *s;

    t = gUnk_03002490;
    t->unk15 = 10;
    TaskYieldTrampoline(1);
    sub_080062c4();
    gUnk_03002490->unk54 = 131072;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFE0000;
    TaskYieldTrampoline(2);
    u = gUnk_03002490;
    u->unk3C++;
    u->unk54 = 131072;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFE0000;
    TaskYieldTrampoline(2);
    sub_080062c4();
    v = gUnk_03002490;
    v->unk60 = 9472;
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_080934f8();
    s = gUnk_03002490;
    switch (s->unk28)
    {
    case 0:
        sub_080061c0(81920, 0x5A5A5A5A);
    case 1:
        gUnk_03002490->unk7A = 0;
        w = gUnk_03002490;
        w->unk58 = 0xFFFC0000;
        w->unk60 = 9472;
        sub_08006338(51);
        TaskYieldTrampoline(13);
        while (gUnk_03002490->unk58 < 0)
        {
            if ((gUnk_03001EA4 & 2) != 0)
                sub_08006338(52);
            else
                sub_08006338(61);
            TaskYieldTrampoline(1);
        }
        x = gUnk_03002490;
        x->unk58 = 262144;
        x->unk60 = 0;
        sub_08006338(53);
        while (gUnk_03002490->unk7A == 0)
            TaskYieldTrampoline(1);
        sub_080062c4();
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        sub_0809364c();
        sub_080936a0();
        break;
    case 2:
        s->unk7A = 0;
        sub_080061c0(0xFFFE8000, 0x5A5A5A5A);
        y = gUnk_03002490;
        y->unk58 = 0xFFFC0000;
        y->unk60 = 16384;
        sub_08006338(45);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        sub_08006338(49);
        TaskYieldTrampoline(4);
        sub_08006338(47);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        while (gUnk_03002490->unk7A == 0)
            TaskYieldTrampoline(1);
        sub_080062c4();
        sub_0809364c();
        sub_0806395c(11);
        break;
    case 3:
        s->unk6C = 0;
        do
        {
            sub_08006338(38);
            TaskYieldTrampoline(6);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            sub_080261d4(2);
            sub_08093a00(504);
            sub_08063d4c(gUnk_03002490->unk1C);
            sub_08064d6c(154, *(s16 *)&gUnk_030023B4, *(s16 *)&gUnk_030023D4, 0);
            TaskYieldTrampoline(8);
        } while ((s16)++gUnk_03002490->unk6C <= 5);
        sub_08068920(gUnk_03002490->unk1C, 4);
        sub_080936a0();
        break;
    }
    sub_08006138();
}

void sub_08093354(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk14 != 10)
    {
        t->unk1C = -1;
        sub_08006148(sub_08091fe0, gCurTaskIdx);
    }
}

void sub_08093380(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk15 = 12;
    gUnk_02007D00[4] = 0;
    if (--gUnk_02007D00[7] == 0)
        sub_0806684c();
    sub_080667c0(1, 63);
    sub_0806caa0(1, 0, 0);
    sub_080062c4();
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0xFFFF0000, 0x5A5A5A5A);
    sub_0800622c(0xFFFD0000, 9472, 196608);
    sub_08006338(63);
    TaskYieldTrampoline(8);
    u = gUnk_03002490;
    u->unk3C--;
    TaskYieldTrampoline(15);
    gUnk_03002490->unk34 = 4;
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_0806caa0(0, 0, 10);
    sub_080261d4(4);
    sub_080031b8(0x1F7);
    sub_0806cc90(0, 4, 24, 24);
    sub_08006244();
    sub_080061c0(0xFFFF8000, 0x5A5A5A5A);
    TaskYieldTrampoline(30);
    sub_08006214();
    TaskYieldTrampoline(170);
    sub_0806caa0(1, 0, 0);
    sub_0806ad18();
    gUnk_02007D00[4] = 1;
    sub_08006138();
}

void sub_08093488(void)
{
    sub_080663f4(&gUnk_082959A8, 16);
    if (gUnk_02007D00[4] == 1)
        sub_08006148(sub_0806a344, gCurTaskIdx);
}

void sub_080934b8(void)
{
    struct Task *t;

    sub_08063e14();
    if (--gUnk_02007D00[4] == 0)
    {
        t = gUnk_03002490;
        if (++t->unk3C > 35)
            t->unk3C = 34;
        sub_08006304();
        gUnk_02007D00[4] = 2;
    }
}
void sub_080934f8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    s16 flags;
    s32 i;
    s32 f;
    s16 m;
    s32 n;

    flags = 0;
    for (i = 9; i >= 0; i--)
    {
        t = gUnk_03002490;
        m = sub_08021bb4(t->unk48, t->unk4A, gUnk_08743AB8[i] * t->unk43, 0);
        if (m == -1)
            m = 1;
        if (gUnk_087339F0[m] != 0)
        {
            if (i > 4)
                flags |= 1;
            else
                flags |= 2;
        }
    }
    n = (u16)sub_08002ee8(8);
    f = flags;
    if (f == 0)
    {
        if ((s16)n <= 3)
        {
            u = gUnk_03002490;
            if (u->unk28 != 0)
                u->unk28 = f;
            else
                u->unk28 = 2;
        }
        else if ((s16)n <= 6)
        {
            u = gUnk_03002490;
            if (u->unk28 != 2)
                u->unk28 = 2;
            else
                u->unk28 = 3;
        }
        else
        {
            u = gUnk_03002490;
            if (u->unk28 == 3)
                u->unk28 = 1;
            else
                u->unk28 = 3;
        }
    }
    else if (f == 1)
    {
        u = gUnk_03002490;
        u->unk28 = 2;
    }
    else if (f == 3)
    {
        u = gUnk_03002490;
        u->unk28 = 1;
    }
    else if ((s16)n <= 5)
    {
        w = gUnk_03002490;
        if (w->unk28 != gUnk_08743AC2[f - 1])
            w->unk28 = gUnk_08743AC2[f - 1];
        else
            w->unk28 = 3;
    }
    else
    {
        w = gUnk_03002490;
        if (w->unk28 != 3)
            w->unk28 = 3;
        else
            w->unk28 = gUnk_08743AC2[f - 1];
    }
}
void sub_0809364c(void)
{
    sub_08068920(gUnk_03002490->unk1C, 4);
    sub_080261d4(4);
    sub_08063d4c(gUnk_03002490->unk1C);
    sub_08064d6c(154, *(s16 *)&gUnk_030023B4, *(s16 *)&gUnk_030023D4, 0);
    sub_08093a00(504);
    sub_08093780();
    sub_080062c4();
}

void sub_080936a0(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk7A = 0;
    sub_080061c0(0xFFFE8000, 0x5A5A5A5A);
    t = gUnk_03002490;
    t->unk58 = 0xFFFE0000;
    t->unk60 = 12032;
    t->unk6C = 0;
    do
    {
        sub_08006338(56);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
    } while ((s16)++gUnk_03002490->unk6C <= 2);
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08006338(58);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
    } while ((s16)++gUnk_03002490->unk6C <= 1);
    sub_08006338(59);
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    TaskYieldTrampoline(8);
    u = gUnk_03002490;
    u->unk34 = 1;
    sub_0806395c(1);
}

void sub_08093780(void)
{
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk7A = 0;
        gUnk_03002490->unk58 = 0xFFFF0000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = 65536;
        TaskYieldTrampoline(2);
    } while ((s16)++gUnk_03002490->unk6C <= 6);
}

s32 sub_080937d0(void)
{
    struct Task *t;
    struct Task *u;
    s32 r;
    s8 k;

    r = 0;
    t = gUnk_03002490;
    switch (t->unk14)
    {
    case 3:
        k = t->unk43;
        if ((k == 1 && (k & gUnk_03005550) != 0)
         || (k == -1 && (gUnk_03005550 & 2) != 0))
        {
            u = gUnk_03002490;
            u->unk18 = 0;
            u->unk34 = 2;
            sub_0806395c(9);
            sub_08006148(sub_08091fe0, gCurTaskIdx);
            r = 1;
        }
        break;
    case 6:
    case 8:
        sub_08063ff4();
        break;
    }
    return r;
}

void sub_08093858(void)
{
    gUnk_03002490->unk58 = 0;
}

s32 sub_08093868(void)
{
    gUnk_03002490->unk30 = 1;
    sub_0806caa0(1, 0, 0);
    sub_080261d4(2);
    return 0;
}

s32 sub_0809388c(void)
{
    struct Task *t;
    s32 n;

    t = gUnk_03002490;
    t->unk34 = 0;
    t->unk18 = 0;
    n = t->unk1C;
    if (n != -1)
    {
        sub_080689c8(n, -t->unk43);
        gUnk_03002490->unk1C = -1;
    }
    sub_080639a4(gUnk_0874410C);
    sub_0806395c(12);
    sub_08006148(sub_08091fe0, gCurTaskIdx);
    return 1;
}

void sub_080938e4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_080651b4;
    t->unk42 = 12;
    u = gUnk_03002490;
    u->unk38 = gUnk_087536FC;
    u->unk04 = (u32)sub_0809397c;
    sub_08064a60();
    v = gUnk_03002490;
    v->unk28 = 4;
    v->unk2C = gUnk_08743B48[v->unk74];
    while (--gUnk_03002490->unk2C >= 0)
    {
        w = gUnk_03002490;
        if ((w->unk2C & 1) != 0)
        {
            w->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
        }
        else
        {
            sub_08006338(6);
            TaskYieldTrampoline(1);
        }
    }
    TaskDispatchTrampoline();
}

void sub_0809397c(void)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    if ((s16)gUnk_03004CA0[i = (t = gUnk_03002490)->unk44] != -1
     && (u = &gUnk_03002790[i])->unk76 == 5 && u->unk18 != 0)
    {
        if (--t->unk28 == 0)
        {
            t->unk4C = u->unk4C;
            t->unk50 = u->unk50;
            t->unk48 = u->unk48;
            t->unk4A = u->unk4A;
            t->unk28 = 4;
        }
    }
    else
    {
        t->unk2C = 0;
    }
}

void sub_08093a00(s32 a)
{
    if (gUnk_03002490->unk1C == gUnk_03002360)
        sub_080031b8(a);
}

void sub_08093a24(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 9;
    u = gUnk_03002490;
    u->unk38 = gUnk_08752F60;
    sub_08002e98(u->unk73, 1, gUnk_08744170);
}

void sub_08093a64(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_08093a98;
    sub_08064a60();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08744174);
}

void sub_08093a98(void)
{
    if (sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08744178);
    sub_08068e04();
    sub_08069b44();
}

void sub_08093ac8(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk28 = 0;
    sub_08006338(4);
    gUnk_03002490->unk7A = 0;
    sub_080061c0(98304, 0x5A5A5A5A);
    sub_0800622c(0xFFFE2000, 5376, 196608);
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk7A = 0;
    sub_080061c0(49152, 0x5A5A5A5A);
    sub_0800622c(0xFFFEE000, 5376, 196608);
    while (gUnk_03002490->unk7A == 0)
        TaskYieldTrampoline(1);
    t = gUnk_03002490;
    t->unk28++;
    sub_08006138();
}

void sub_08093b80(void)
{
    if (gUnk_03002490->unk28 != 0)
    {
        sub_080639a4(gUnk_0874430C);
        sub_08006148(sub_0806a344, gCurTaskIdx);
    }
}

s32 sub_08093bb0(void)
{
    sub_080639a4(gUnk_0874430C);
    sub_08006148(sub_0806a344, gCurTaskIdx);
    return 1;
}

void sub_08093bd4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 13;
    u = gUnk_03002490;
    u->unk38 = gUnk_08753160;
    sub_08064a60();
    v = gUnk_03002490;
    v->unk8C->unk38 = 0x20E;
    v->unk7A = 0;
    sub_08002e98(gUnk_03002490->unk73, 2, gUnk_087441A4);
}

void sub_08093c30(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_08093c7c;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087441AC);
}

void sub_08093c60(void)
{
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087441AC);
}

void sub_08093c7c(void)
{
    switch (gUnk_03002490->unk15)
    {
    case 0:
        sub_08002e98(0, 2, gUnk_087441B4);
        break;
    case 1:
        if (sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 2, gUnk_087441B4);
        sub_08068e04();
        sub_08069b44();
        break;
    }
}

void sub_08093ccc(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08006338(4);
    while (1)
    {
        TaskYieldTrampoline(6);
        sub_08064cdc(181, -8, -8, 1);
    }
}

void sub_08093cf8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s16 *p;
    s16 *q;
    s32 i;
    s32 x;
    s32 y;

    if ((s16)gUnk_03004CA0[i = (t = gUnk_03002490)->unk44] != -1)
    {
        u = &gUnk_03002790[i];
        t->unk43 = u->unk43;
        sub_08006304();
        if (u->unk1C != 0)
        {
            v = gUnk_03002490;
            p = &u->unk48;
            x = *p;
            x <<= 16;
            v->unk4C = x;
            p += 1;
            v->unk50 = (*p + 8) << 16;
            y = x >> 16;
            v->unk48 = y;
            v->unk4A = v->unk50 >> 16;
            sub_0806395c(1);
            sub_08006148(sub_08093c60, gCurTaskIdx);
        }
        else
        {
            v = gUnk_03002490;
            q = &u->unk48;
            x = *q;
            x <<= 16;
            v->unk4C = x;
            q += 1;
            v->unk50 = (*q - 8) << 16;
            y = x >> 16;
            v->unk48 = y;
            v->unk4A = v->unk50 >> 16;
        }
    }
    else
    {
        sub_08005654(gCurTaskIdx);
    }
}

void sub_08093dcc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk15 = 1;
    u = gUnk_03002490;
    u->unk42 = 9;
    v = gUnk_03002490;
    v->unk28 = 3;
    sub_080061c0(gUnk_0874417C[v->unk73][v->unk74], 0x5A5A5A5A);
    sub_0800622c(gUnk_0874418C[gUnk_03002490->unk73][gUnk_03002490->unk74],
                 8192, 458752);
    gUnk_03002490->unk7A = 0;
    while (1)
    {
        TaskYieldTrampoline(6);
        sub_08064cdc(181, -8, -8, 1);
    }
}

void sub_08093e54(void)
{
}

void sub_08093e58(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk38 = gUnk_08753160;
    t->unk42 = gUnk_03002790[t->unk44].unk42 - 1;
    sub_08064a60();
    u = gUnk_03002490;
    u->unk58 = 0xFFFE0000;
    sub_08006338(5);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

s32 sub_08093edc(void)
{
    sub_080639a4(gUnk_08744324);
    sub_08006148(sub_0806a344, gCurTaskIdx);
    return 1;
}

s32 sub_08093f00(void)
{
    struct Task *t;
    s32 r;

    r = 0;
    t = gUnk_03002490;
    if (--t->unk28 == 0)
    {
        sub_080639a4(gUnk_08744324);
        sub_08006148(sub_0806a344, gCurTaskIdx);
        r = 1;
    }
    else
    {
        t->unk7A = 0;
        sub_0800622c(gUnk_0874419C[gUnk_03002490->unk74], 8192, 458752);
    }
    return r;
}
