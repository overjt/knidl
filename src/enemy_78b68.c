/* game_code_and_rodata 0x08078B68-0x0807AA5C (issue #77, module M20 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08078B68 0x0807AA5C src/enemy_78b68.c --newpb
 *
 * M20 is enemy/object behaviour bank 1: twenty-one ROM task types whose
 * bodies use the same three-table pattern as M21/M22/M24/M25/M26
 * (rom-map section 9):
 *
 *   entry       -> installs Task.unk04 (the per-frame hook) and hands
 *                  Task.unk14 / Task.unk15 to sub_08002e98, which indexes the
 *                  script's tables;
 *   unk14 table -> the coroutine BODIES (each runs a chain of
 *                  TaskYieldTrampoline waits);
 *   unk15 table -> the per-frame HANDLERS;
 *   unk73 table -> the class-3 dispatch a task type's body selects with.
 *
 * This batch holds the bank's first twelve scripts, among them:
 *   * the shared eight-frame "bob" coroutine `sub_08078b68`, which nine of
 *     the bank's idle bodies tail-call;
 *   * the `0x08740648` / `0x08740668` cue+delay pair the door/lift scripts
 *     index by Task.unk74 (`sub_08078c80`, `sub_08078d88`);
 *   * task #9's four-state script `sub_080794dc`+`sub_0807950c` with its
 *     `0x08740758` class-4 dispatch and the `sub_080795d8` state machine
 *     (a screen-shake amplitude test plus a 120-frame timer);
 *   * `sub_080797ec`, which spawns three class-4 actors from a stack
 *     `struct ActorSpawn` and clears Task.unk74 on the companion it gets
 *     back from sub_08064cdc;
 *   * the `sub_08079eec` / `sub_08079f18` / `sub_08079f54` sound-cue chain
 *     (all `u16`-parameterised) that every later script funnels its
 *     "player hit me" reaction through;
 *   * the two 0x1C0-byte cutscene coroutines `sub_0807a1c0` and
 *     `sub_0807a634` (identical: a seventeen-step frame script followed by an
 *     eight-iteration palette flip between `0x08740DE4` and `0x0873F774`);
 *   * `sub_0807a8fc`, the bank's only `mov pc` jump table (five states over
 *     Task.unk73), and `sub_0807a968`, which clamps a spawn point from
 *     `0x08740824` into the camera box `gUnk_03002158[0..3]`.
 *
 * `sub_0807927c`, `sub_080794d0`, `sub_080799a0`, `sub_08079db8` and
 * `sub_0807a4e4` are dead exports: each is a copy of its host's tail dispatch
 * that nothing in the ROM references (curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s16 gUnk_03002158[];
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern s8 gUnk_02007FB8[];
extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];
extern u8 gUnk_02006178;
extern vs16 gUnk_03004CA0[];

/* ROM tables */
extern s16 gUnk_08740668[];
extern s16 gUnk_08740740[];
extern s16 gUnk_087407BC[];
extern s16 gUnk_087407C0[];
extern s16 gUnk_0874080C[];
extern struct AnimCmd gUnk_087406A0[];
extern struct AnimCmd gUnk_087406EC[];
extern struct AnimCmd gUnk_0874074C[];
extern struct AnimCmd gUnk_08740854[];
extern u32 gUnk_0873F500[];
extern u32 gUnk_0873F774[];
extern u32 gUnk_0873F7AC[];
extern u32 gUnk_08740648[];
extern u32 gUnk_08740658[];
extern u32 gUnk_08740660[];
extern u32 gUnk_08740670[];
extern u32 gUnk_08740678[];
extern u32 gUnk_08740680[];
extern u32 gUnk_08740690[];
extern u32 gUnk_087406C4[];
extern u32 gUnk_087406D0[];
extern u32 gUnk_087406DC[];
extern u32 gUnk_087406E4[];
extern u32 gUnk_08740700[];
extern u32 gUnk_08740704[];
extern u32 gUnk_08740708[];
extern u32 gUnk_08740710[];
extern u32 gUnk_08740718[];
extern u32 gUnk_08740720[];
extern u32 gUnk_08740728[];
extern u32 gUnk_08740758[];
extern u32 gUnk_08740768[];
extern u32 gUnk_08740778[];
extern u32 gUnk_0874077C[];
extern u32 gUnk_08740780[];
extern u32 gUnk_08740788[];
extern u32 gUnk_08740798[];
extern u32 gUnk_087407A8[];
extern u32 gUnk_087407AC[];
extern u32 gUnk_087407B0[];
extern u32 gUnk_087407C4[];
extern u32 gUnk_087407D0[];
extern u32 gUnk_087407DC[];
extern u32 gUnk_087407E4[];
extern u32 gUnk_087407F4[];
extern u32 gUnk_08740804[];
extern u32 gUnk_08740808[];
extern u32 gUnk_08740810[];
extern u32 gUnk_08740818[];
extern u32 gUnk_08740820[];
extern u32 gUnk_08740824[];
extern u32 gUnk_08740C00[];
extern u32 gUnk_08740DE4[];
extern u32 gUnk_08740E1C[];
extern u32 gUnk_08740F2C[];
extern u32 gUnk_08752204[];
extern u32 gUnk_0875246C[];
extern u32 gUnk_08752584[];
extern u32 gUnk_0875265C[];

/* Externals */
extern s32 sub_08002ee8(s32 a);
extern s32 sub_080031b8(s32 id);
extern s32 sub_08063c74(void);
extern s32 sub_08063cd0(void);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_0806415c(s32 n);
extern s32 sub_08064188(s32 n);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064cdc(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern s32 sub_08069b44(void);
extern u32 sub_08068e04(void);
extern u32 sub_080692fc(void);
extern u32 sub_0806a344(void);
extern void TaskYieldTrampoline(u32 frames);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, u32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_0806395c(u32 v);
extern void sub_080639a4(u32 *p);
extern void sub_080639b4(u32 *p);
extern void sub_08063d7c(void);
extern void sub_08063e14(void);
extern void sub_08063f24(s32 i);
extern void sub_08063ff4(void);
extern void sub_08064038(void);
extern void sub_080640c8(void);
extern void sub_0806523c(void);
extern void sub_080656b4(void);
extern void sub_08065848(u32 p0, s32 idx);
extern void sub_08066b34(u32 *p);
extern void sub_08066ba8(void);
extern void sub_08066bdc(void);
extern void sub_080670ac(u32);
extern void sub_080670d4(void);
extern void sub_0806a0f0(s32 a);
extern void sub_0806ee2c(void);

/* Forward declarations */
void sub_08078c14(void);
void sub_08078d1c(void);
void sub_08078e9c(void);
void sub_08079128(void);
void sub_08079298(void);
void sub_0807933c(void);
void sub_0807950c(void);
void sub_0807955c(void);
void sub_08079578(void);
void sub_0807964c(void);
void sub_0807968c(void);
void sub_080796d8(void);
void sub_0807995c(void);
void sub_08079ad4(void);
void sub_08079b24(void);
void sub_08079d74(void);
void sub_0807a09c(void);
void sub_0807a10c(void);
void sub_0807a4a0(void);
void sub_0807a5a8(void);
void sub_0807a618(void);
extern s32 sub_0807ab38();

void sub_08078b68(void)
{
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(11);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(11);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(8);
    }
}

void sub_08078be0(void)
{
    gUnk_03002490->unk04 = (u32)sub_08078c14;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08740658);
}

void sub_08078c14(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_08740660);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_08078c64(void)
{
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08740658);
}

void sub_08078c80(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_080061c0(gUnk_08740648[gUnk_03002490->unk74], 0x5A5A5A5A);
    sub_08078b68();
}

void sub_08078cb8(void)
{
}

void sub_08078cbc(void)
{
    gUnk_03002490->unk15 = 1;
    sub_0800622c(0, 0x1500, 0x30000);
    sub_08078b68();
    sub_08006138();
}

void sub_08078ce4(void)
{
}

void sub_08078ce8(void)
{
    gUnk_03002490->unk04 = (u32)sub_08078d1c;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08740670);
}

void sub_08078d1c(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_08740678);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_08078d6c(void)
{
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08740670);
}

void sub_08078d88(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    t = gUnk_03002490;
    t->unk28 = gUnk_08740668[t->unk74];
    sub_080061c0(gUnk_08740648[t->unk74], 0x5A5A5A5A);
    sub_08078b68();
}

void sub_08078dd4(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 <= 0)
    {
        struct Task *u;

        sub_08063ff4();
        u = gUnk_03002490;
        u->unk28 = gUnk_08740668[u->unk74];
    }
    else
    {
        t->unk28--;
    }
}

void sub_08078e10(void)
{
    gUnk_03002490->unk15 = 1;
    sub_0800622c(0, 0x1500, 0x30000);
    sub_08078b68();
    sub_08006138();
}

void sub_08078e38(void)
{
}

void sub_08078e3c(void)
{
    gUnk_03002490->unk04 = (u32)sub_08078e9c;
    sub_08063e14();
    gUnk_03002490->unk28 = 80;
    sub_0806395c(0);
    gUnk_03002490->unk34 = sub_080640dc(gUnk_087406A0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087406C4);
}

void sub_08078e80(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087406C4);
}

void sub_08078e9c(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_087406D0);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_08078eec(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_080061c0(gUnk_08740648[gUnk_03002490->unk74], 0x5A5A5A5A);
    sub_08006138();
}

void sub_08078f24(void)
{
    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
    if (--gUnk_03002490->unk28 < 0)
    {
        if (sub_08002ee8(4) == 0)
        {
            sub_0806395c(1);
            sub_08006148(sub_08078e80, gCurTaskIdx);
        }
        else
        {
            gUnk_03002490->unk28 = 30;
            sub_0806395c(0);
            sub_08006148(sub_08078e80, gCurTaskIdx);
        }
    }
}

void sub_08078f8c(void)
{
    s32 saved;

    gUnk_03002490->unk15 = 1;
    {
        struct Task *t = gUnk_03002490;

        t->unk2C = 0;
        saved = t->unk54;
        t->unk54 = 0;
        t->unk6C = 0;
    }
    do
    {
        sub_08006338(4);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(4);
    } while ((s16)++gUnk_03002490->unk6C <= 1);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = saved;
        u->unk2C = 1;
        u->unk7A = 0;
    }
    sub_0800622c(-gUnk_08740680[gUnk_03002490->unk74],
                 gUnk_08740690[gUnk_03002490->unk74], 0x30000);
    sub_08006138();
}

void sub_0807906c(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk2C != 0 && t->unk7A != 0)
    {
        sub_08006244();
        gUnk_03002490->unk28 = 30;
        gUnk_03002490->unk34 = sub_080640dc(gUnk_087406A0);
        sub_0806395c(0);
        sub_08006148(sub_08078e80, gCurTaskIdx);
    }
}

void sub_080790c0(void)
{
    gUnk_03002490->unk15 = 2;
    sub_0800622c(0, 0x1500, 0x30000);
    sub_08078b68();
    sub_08006138();
}

void sub_080790e8(void)
{
}

void sub_080790ec(void)
{
    gUnk_03002490->unk04 = (u32)sub_08079128;
    sub_08066b34(gUnk_08740C00);
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087406DC);
}

void sub_08079128(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_087406E4);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_08079178(void)
{
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087406DC);
}

void sub_08079194(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_080061c0(0xC000, 0x5A5A5A5A);
    sub_08078b68();
}

void sub_080791bc(void)
{
}

void sub_080791c0(void)
{
    gUnk_03002490->unk15 = 1;
    if (gUnk_03002490->unk8C->unk1A == -1)
    {
        sub_080640c8();
        sub_0800622c(0, 0x1500, 0x30000);
        sub_08078b68();
    }
    else
    {
        gUnk_03002490->unk34 = sub_080640dc(gUnk_087406EC);
        sub_08066ba8();
        while (1)
        {
            sub_08066bdc();
            TaskYieldTrampoline(8);
        }
    }
}

void sub_0807921c(void)
{
    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
}

void sub_08079238(void)
{
    gUnk_03002490->unk04 = (u32)sub_08079298;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740700);
}

void sub_0807927c(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740700);
}

void sub_08079298(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08740704);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_080792dc(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08078b68();
}

void sub_080792f4(void)
{
}

void sub_080792f8(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807933c;
    sub_08066b34(gUnk_08740C00);
    gUnk_03002490->unk74 = 2;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08740708);
}

void sub_0807933c(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_08740710);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807938c(void)
{
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08740708);
}

void sub_080793a8(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_08078b68();
}

void sub_080793c4(void)
{
    sub_08063e14();
}

void sub_080793d0(void)
{
    gUnk_03002490->unk15 = 1;
    if (gUnk_03002490->unk8C->unk1A == -1)
    {
        sub_080640c8();
        sub_0800622c(0, 0x1500, 0x30000);
        sub_08078b68();
    }
    else
    {
        gUnk_03002490->unk34 = sub_080640dc(gUnk_087406EC);
        sub_08066ba8();
    }
    sub_08006138();
}

void sub_08079424(void)
{
    gUnk_03002490->unk34 = sub_0806415c(gUnk_03002490->unk34);
}

void sub_08079440(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752204;
    sub_08002e98(gUnk_03002490->unk73, 2, gUnk_08740718);
}

s32 sub_08079480(void)
{
    sub_0806395c(3);
    sub_08006148(sub_0807955c, gCurTaskIdx);
    return 1;
}

s32 sub_080794a0(void)
{
    sub_0806395c(1);
    sub_08006148(sub_0807955c, gCurTaskIdx);
    return 1;
}

s32 sub_080794c0(void)
{
    sub_0806a0f0(-2);
    return 1;
}

s32 sub_080794d0(void)
{
    sub_08063ff4();
    return 0;
}

void sub_080794dc(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807950c;
    sub_08063e14();
    sub_08079578();
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_08740758);
}

void sub_0807950c(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 4, gUnk_08740768);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807955c(void)
{
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_08740758);
}

void sub_08079578(void)
{
    struct Task *t = gUnk_03002490;
    s32 r;

    t->unk2C = 0;
    t->unk30 = 0;
    r = sub_08002ee8(4);
    if (r == 0)
    {
        gUnk_03002490->unk28 = r;
        sub_0806395c(0);
    }
    else
    {
        gUnk_03002490->unk28 = 240;
        sub_0806395c(2);
    }
}

void sub_080795b4(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    while (1)
    {
        sub_08063e14();
        sub_08006338(4);
        TaskYieldTrampoline(1);
    }
}

void sub_080795d8(void)
{
    struct Task *t = gUnk_03002490;
    s32 n = t->unk28;

    if (n <= 0)
    {
        if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 63)
        {
            if (gUnk_03002490->unk2C <= 0 && sub_08002ee8(2) == 0)
                sub_0807964c();
            else
                sub_080796d8();
        }
        else
        {
            gUnk_03002490->unk2C = 0;
            sub_0807968c();
        }
    }
    else
    {
        t->unk2C = 0;
        t->unk28 = n - 1;
        sub_0807968c();
    }
}

void sub_0807964c(void)
{
    sub_0806395c(2);
    sub_08006148(sub_0807955c, gCurTaskIdx);
}

void sub_0807966c(void)
{
    sub_0806395c(1);
    sub_08006148(sub_0807955c, gCurTaskIdx);
}

void sub_0807968c(void)
{
    s32 n = ++gUnk_03002490->unk30;

    if (n == 120)
    {
        sub_0807966c();
    }
    else if (n == 75 || n == 90 || n == 105)
    {
        if (sub_08002ee8(gUnk_08740720[gUnk_03002490->unk74]) == 0)
            sub_0807966c();
    }
}

void sub_080796d8(void)
{
    struct Task *t = gUnk_03002490;

    if (++t->unk2C == 33)
    {
        t->unk2C = 3;
        if (sub_08002ee8(3) == 0)
            sub_0807968c();
        else
            sub_0807964c();
    }
    else
    {
        sub_0807968c();
    }
}

void sub_08079710(void)
{
    struct Task *t = gUnk_03002490;
    s16 *delay;
    u32 *cue;
    s32 i;
    s32 idx;
    u32 *cbase;
    s16 *dbase;

    t->unk8C->unk2C = 0;
    t->unk15 = 1;
    sub_080062c4();
    gUnk_03002490->unk34 = sub_080640dc(gUnk_0874074C);
    idx = gUnk_03002490->unk74;
    cbase = gUnk_08740728;
    dbase = gUnk_08740740;
    i = 2;
    delay = &dbase[idx];
    cue = &cbase[idx];
    do
    {
        sub_080061c0(*cue, 0x5A5A5A5A);
        TaskYieldTrampoline(*delay);
        delay += 2;
        cue += 2;
    } while (--i >= 0);
    gUnk_03002490->unk54 = 0;
    sub_08006338(6);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    sub_0806395c(0);
    sub_08006138();
}

void sub_080797b4(void)
{
    gUnk_03002490->unk34 = sub_08064188(gUnk_03002490->unk34);
    if (gUnk_03002490->unk14 != 1)
    {
        gUnk_03002490->unk30 = 0;
        sub_08006148(sub_0807955c, gCurTaskIdx);
    }
}

void sub_080797ec(void)
{
    struct ActorSpawn spawn;
    s32 i;

    gUnk_03002490->unk15 = 2;
    sub_08006338(7);
    TaskYieldTrampoline(15);
    i = 0;
    spawn.unk00 = 0;
    spawn.unk04 = 102;
    spawn.unk08 = 0;
    spawn.unk0A = 0;
    gUnk_03002490->unk6C = 0;
    do
    {
        spawn.unk09 = i;
        spawn.unk0C = 10;
        spawn.unk0E = 0;
        sub_080031b8(164);
        gUnk_03002490->unk46 = sub_08064b5c(&spawn, 0);
        sub_08064cdc(216, 10, 0, 1);
        {
            s32 id = sub_08064cdc(217, 10, 0, 1);

            if (id != -1)
            {
                struct Task *p = &gUnk_03002790[id];

                p->unk74 = i;
            }
        }
        sub_08006338(8);
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(5);
        if (++i > 2)
            i = 0;
    } while ((s16)++gUnk_03002490->unk6C <= 9);
    sub_0806395c(0);
    sub_08006138();
}

void sub_080798b8(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 != 2)
    {
        t->unk28 = 240;
        t->unk2C = 0;
        t->unk30 = 0;
        sub_08006148(sub_0807955c, gCurTaskIdx);
    }
}

void sub_080798e8(void)
{
    gUnk_03002490->unk15 = 3;
    sub_08006338(5);
    sub_0800622c(0, 0x2500, 0x30000);
    sub_08006138();
}

void sub_08079914(void)
{
}

void sub_08079918(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807995c;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740778);
}

void sub_0807995c(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_0874077C);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_080799a0(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740778);
}

void sub_080799bc(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_08006338(4);
    sub_08006138();
}

void sub_080799dc(void)
{
}

void sub_080799e0(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_0875246C;
    sub_08002e98(gUnk_03002490->unk73, 2, gUnk_08740780);
}

s32 sub_08079a20(void)
{
    sub_0806395c(1);
    sub_08006148(sub_08079b24, gCurTaskIdx);
    return 1;
}

s32 sub_08079a40(void)
{
    sub_0806395c(3);
    sub_08006148(sub_08079b24, gCurTaskIdx);
    return 1;
}

s32 sub_08079a60(void)
{
    sub_0806a0f0(-2);
    return 1;
}

s32 sub_08079a70(void)
{
    sub_0806395c(2);
    sub_08006148(sub_08079b24, gCurTaskIdx);
    return 1;
}

s32 sub_08079a90(void)
{
    sub_08063ff4();
    return 0;
}

void sub_08079a9c(void)
{
    gUnk_03002490->unk04 = (u32)sub_08079ad4;
    sub_08063e14();
    sub_0806395c(0);
    gUnk_03002490->unk28 = 0;
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_08740788);
}

void sub_08079ad4(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 4, gUnk_08740798);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_08079b24(void)
{
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_08740788);
}

void sub_08079b40(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_080061c0(0x4D00, 0x5A5A5A5A);
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
    }
}

void sub_08079b98(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 == 0)
        t->unk28 = 1;
}

void sub_08079bac(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080062c4();
    sub_08064038();
    sub_08006338(10);
    sub_0800622c(0, 0x1500, 0x30000);
    sub_08006138();
}

void sub_08079be0(void)
{
}

void sub_08079be4(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 2;
    sub_080062c4();
    t = gUnk_03002490;
    t->unk4C = (t->unk48 + t->unk43 * 4) << 16;
    if (t->unk28 == 1)
    {
        t->unk6C = 0;
        do
        {
            sub_08006338(8);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(5);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
        } while ((s16)++gUnk_03002490->unk6C <= 4);
    }
    {
        struct Task *u = gUnk_03002490;

        u->unk4C = (u->unk48 + u->unk43 * 4) << 16;
    }
    sub_0806395c(1);
    sub_08006138();
}

void sub_08079c84(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_08078c64, gCurTaskIdx);
}

void sub_08079cac(void)
{
    u8 v;

    gUnk_03002490->unk15 = 3;
    sub_080062c4();
    v = gUnk_02006178;
    if (v == 0)
    {
        sub_080639b4(gUnk_0873F7AC);
        sub_080670ac(15);
        gUnk_03002490->unk6C = v;
        do
        {
            sub_08006338(11);
            TaskYieldTrampoline(1);
            sub_08006338(7);
            TaskYieldTrampoline(2);
        } while ((s16)++gUnk_03002490->unk6C <= 3);
        sub_080670d4();
        sub_080639a4(gUnk_08740F2C);
        sub_0806a344();
    }
    else
    {
        sub_08006138();
    }
}

void sub_08079d2c(void)
{
}

void sub_08079d30(void)
{
    gUnk_03002490->unk04 = (u32)sub_08079d74;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087407A8);
}

void sub_08079d74(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087407AC);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_08079db8(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087407A8);
}

void sub_08079dd4(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
    }
}

void sub_08079e20(void)
{
}

void sub_08079e24(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752584;
    sub_08065848(2, 0);
    sub_08002e98(gUnk_03002490->unk73, 3, gUnk_087407B0);
}

void sub_08079e70(void)
{
    if (--gUnk_02007FB8[0] < 0)
        sub_0806ee2c();
}

void sub_08079e8c(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 != 0)
        sub_080061c0(gUnk_087407DC[t->unk74], 0x5A5A5A5A);
    sub_0800622c(-gUnk_087407C4[gUnk_03002490->unk28],
                 gUnk_087407D0[gUnk_03002490->unk28], 0x30000);
    gUnk_03002490->unk7A = 0;
}

void sub_08079eec(u16 a)
{
    s32 r = sub_08002ee8(4);

    gUnk_03002490->unk28 = r;
    if (r > 2)
        gUnk_03002490->unk28 = 2;
    sub_0806395c(a);
}

void sub_08079f18(u16 a)
{
    if (sub_08063c74() > 0x1000)
    {
        sub_08079eec(a);
    }
    else if (sub_08002ee8(4) == 0)
    {
        sub_0806395c(0);
    }
    else
    {
        sub_08079eec(a);
    }
}

void sub_08079f54(u16 a)
{
    sub_08063e14();
    sub_08079f18(a);
    switch (gUnk_03002490->unk73)
    {
    case 0:
        sub_08006148(sub_0807a10c, gCurTaskIdx);
        break;
    case 2:
        sub_08006148(sub_0807a618, gCurTaskIdx);
        break;
    }
}

void sub_08079fa8(u16 a)
{
    sub_08063e14();
    if (sub_08002ee8(4) == 0)
        sub_0806395c(0);
    else
        sub_08079f18(a);
}

s32 sub_08079fd0(void)
{
    if (gUnk_03002490->unk73 != 0)
        return 0;
    sub_0806395c(3);
    sub_08006148(sub_0807a10c, gCurTaskIdx);
    return 1;
}

s32 sub_0807a008(void)
{
    if (gUnk_03002490->unk73 != 0)
        return 0;
    sub_0806395c(1);
    sub_08006148(sub_0807a10c, gCurTaskIdx);
    return 1;
}

s32 sub_0807a040(void)
{
    sub_0806a0f0(-2);
    return 1;
}

s32 sub_0807a050(void)
{
    sub_08063ff4();
    return 0;
}

void sub_0807a05c(void)
{
    gUnk_03002490->unk58 = 0;
}

void sub_0807a06c(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807a09c;
    sub_08079fa8(2);
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_087407E4);
}

void sub_0807a09c(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 4, gUnk_087407F4);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        struct Task *t;

        sub_08068e04();
        sub_08069b44();
        t = gUnk_03002490;
        if (t->unk7C != 0 && t->unk46 != -1)
            sub_08063f24(t->unk46);
    }
}

void sub_0807a10c(void)
{
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_087407E4);
}

void sub_0807a128(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080062c4();
    sub_08006338(7);
    TaskYieldTrampoline(gUnk_087407C0[gUnk_03002490->unk74]);
    sub_08079e8c();
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_08006138();
}

void sub_0807a1bc(void)
{
}

void sub_0807a1c0(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk30 = 3;
    gUnk_03002490->unk3C = 20;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 22;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 19;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 23;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 22;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 23;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 22;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 23;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 22;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 21;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 23;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 18;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 22;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 23;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 20;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 22;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 23;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk2C = 2;
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080639b4(gUnk_08740DE4);
        gUnk_03002490->unk3C = 10;
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_0873F774);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_08740DE4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_0873F774);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_08740DE4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_0873F774);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_08740DE4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_0873F774);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
    } while ((s16)++gUnk_03002490->unk6C <= 7);
    gUnk_03002490->unk2C = 1;
    sub_08006138();
}

void sub_0807a380(void)
{
    struct Task *t = gUnk_03002490;
    s32 n = t->unk2C;

    if (n == 1)
    {
        sub_08079f54(2);
    }
    else if (n == 2)
    {
        if (t->unk30 <= 0)
        {
            sub_080031b8(191);
            gUnk_03002490->unk30 = 3;
        }
        gUnk_03002490->unk30--;
    }
}

void sub_0807a3bc(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk2C = 0;
    sub_08006244();
    gUnk_03002490->unk54 >>= 1;
    sub_08006338(7);
    TaskYieldTrampoline(gUnk_087407BC[gUnk_03002490->unk74]);
    gUnk_03002490->unk2C = 1;
    sub_08006138();
}

void sub_0807a408(void)
{
    if (gUnk_03002490->unk2C != 0)
        sub_08079f54(2);
}

void sub_0807a424(void)
{
    gUnk_03002490->unk15 = 3;
    sub_080062c4();
    sub_08064038();
    sub_08006338(8);
    sub_0800622c(0, 0x1500, 0x30000);
    sub_08006138();
}

void sub_0807a458(void)
{
}

void sub_0807a45c(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807a4a0;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740804);
}

void sub_0807a4a0(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08740808);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0807a4e4(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08740804);
}

void sub_0807a500(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    while (1)
    {
        sub_08006338(7);
        TaskYieldTrampoline(7);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
    }
}

void sub_0807a574(void)
{
}

void sub_0807a578(void)
{
    gUnk_03002490->unk04 = (u32)sub_0807a5a8;
    sub_08079fa8(1);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08740810);
}

void sub_0807a5a8(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_08740818);
    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        struct Task *t;

        sub_08068e04();
        sub_08069b44();
        t = gUnk_03002490;
        if (t->unk7C != 0 && t->unk46 != -1)
            sub_08063f24(t->unk46);
    }
}

void sub_0807a618(void)
{
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08740810);
}

void sub_0807a634(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk30 = 3;
    gUnk_03002490->unk3C = 20;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 22;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 19;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 23;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 22;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 23;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 22;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 23;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 22;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 21;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 23;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 18;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 22;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 23;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 20;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 22;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 23;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk2C = 2;
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080639b4(gUnk_08740DE4);
        gUnk_03002490->unk3C = 10;
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_0873F774);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_08740DE4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_0873F774);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_08740DE4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_0873F774);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_08740DE4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080639b4(gUnk_0873F774);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
    } while ((s16)++gUnk_03002490->unk6C <= 7);
    gUnk_03002490->unk2C = 1;
    sub_08006138();
}

void sub_0807a7f4(void)
{
    struct Task *t = gUnk_03002490;
    s32 n = t->unk2C;

    if (n == 1)
    {
        sub_08079f54(1);
    }
    else if (n == 2)
    {
        if (t->unk30 <= 0)
        {
            sub_080031b8(191);
            gUnk_03002490->unk30 = 3;
        }
        gUnk_03002490->unk30--;
    }
}

void sub_0807a830(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk2C = 0;
    sub_08006244();
    gUnk_03002490->unk54 >>= 1;
    sub_08006338(7);
    TaskYieldTrampoline(gUnk_0874080C[gUnk_03002490->unk74]);
    gUnk_03002490->unk2C = 1;
    sub_08006138();
}

void sub_0807a87c(void)
{
    if (gUnk_03002490->unk2C != 0)
        sub_08079f54(1);
}

void sub_0807a898(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_0875265C;
    sub_08002e98(0, 1, gUnk_08740820);
}

void sub_0807a8d4(void)
{
    if (gUnk_03002490->unk73 == 0)
        sub_0806395c(2);
    else
        sub_0806395c(0);
}

void sub_0807a8fc(void)
{
    switch (gUnk_03002490->unk73)
    {
    case 0:
        gUnk_03002490->unk28 = -1;
        gUnk_03002490->unk34 = sub_080640dc(gUnk_08740854);
        break;
    case 1:
        gUnk_03002490->unk28 = 1;
        break;
    case 2:
        gUnk_03002490->unk28 = 1;
        break;
    case 3:
    case 4:
        gUnk_03002490->unk28 = -1;
        break;
    }
}

void sub_0807a968(void)
{
    struct Task *t = gUnk_03002490;
    s32 i = (t->unk73 - 1) * 3;
    s32 x;
    s32 y;

    t->unk58 = gUnk_08740824[i];
    sub_08063d7c();
    x = gUnk_08740824[i + 1] + gUnk_030023B4;
    y = gUnk_08740824[i + 2] + gUnk_030023D4;
    if (x <= gUnk_03002158[0] - 64)
        x = gUnk_03002158[0] - 48;
    if (x >= gUnk_03002158[1] + 64)
        x = gUnk_03002158[1] + 48;
    if (y <= gUnk_03002158[2] - 64)
        y = gUnk_03002158[2] - 48;
    if (y >= gUnk_03002158[3] + 64)
        y = gUnk_03002158[3] + 48;
    gUnk_03002490->unk4C = x << 16;
    gUnk_03002490->unk50 = y << 16;
}

void sub_0807aa0c(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk7C == 6 && (u16)(t->unk82 - 2) <= 1)
    {
        sub_080639b4(gUnk_08740E1C);
        sub_0806395c(3);
        sub_08006148(sub_0807ab38, gCurTaskIdx);
    }
}
