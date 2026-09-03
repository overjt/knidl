/* game_code_and_rodata 0x0808CCE8-0x0808E404 (issue #70, module M24 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0808CCE8 0x0808E404 src/enemy_8cce8.c --newpb
 *
 * M24 is a bank of six enemy/object behaviour scripts built to the same
 * three-table pattern as M22/M25/M26 (rom-map section 9):
 *
 *   entry       -> installs the draw hook in Task.unk00 (sub_080059d8 or
 *                  sub_080656b4) and the per-frame hook in Task.unk0C, points
 *                  Task.unk38 at a TaskGfx block, and hands Task.unk73 to
 *                  sub_08002e98 with the script's entry table;
 *   unk14 table -> the coroutine BODIES: each installs its own resume function
 *                  in Task.unk04 and then runs a chain of TaskYieldTrampoline
 *                  waits;
 *   unk15 table -> the per-frame GUARDS that re-arm the body through
 *                  sub_08006148(fn, gCurTaskIdx) when the state changes.
 *
 * This batch holds:
 *   * the two stand-alone class-2 bodies `sub_0808cce8` (a two-variant intro
 *     that walks Task.unk4C/unk50 with sub_080064ac and waits on the room byte
 *     gUnk_03004CA0[Task.unk44] through `sub_0808cfec`) and `sub_0808d014`,
 *     plus the smaller `sub_0808d148` and `sub_0808d218`;
 *   * script 1: entry `sub_0808d4e8` (Task.unk73 -> `0x08743188`, 3 rows) with
 *     the row bodies `sub_0808d558` / `sub_0808da00` / `sub_0808df58`, the
 *     body tables `0x08743194` / `0x087431AC` / `0x087431C4` and the guard
 *     tables `0x087431A0` / `0x087431B8` / `0x087431C8`;
 *   * its movement library: `sub_0808d364` / `sub_0808d388` snap Task.unk2C to
 *     the 16-pixel grid, `sub_0808d3e4` rolls a new mode out of the 8-entry
 *     table `gUnk_0874313C`, `sub_0808d460` flips the sprite through
 *     Task.unk3E and `sub_0808d494` / `sub_0808d4a8` / `sub_0808d4bc` /
 *     `sub_0808d4d0` set the animation id in Actor.unk1A;
 *   * script 2's entry `sub_0808e3a8` (Task.unk73 -> `0x087431E4`) and its
 *     aiming half: `sub_0808e070` / `sub_0808e0d0` / `sub_0808e174` turn the
 *     vector to the target into a heading with ArcTan2, `sub_0808e254` spawns
 *     actor 103, `sub_0808e2b4` is the sub_08063bd4 proximity test and
 *     `sub_0808e33c` / `sub_0808e36c` are the per-frame step.  The script's
 *     rows continue in src/enemy_8e404.c.
 *
 * `sub_0808d388` is a pointer-referenced leaf the prologue scan could not
 * propose (no `push`, lesson 3.75); it and the module's three other census
 * fixes are curated in tools/symdb.py.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s8 gUnk_02007FB8[];
extern s8 gUnk_03001F08[];
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern vu16 gUnk_03004CA0[];
extern u8 gUnk_03005550[];

/* ROM tables */
extern u32 gUnk_0873F500[];
extern s16 gUnk_08742FAC[];
extern u8 gUnk_0874313C[];
extern u32 gUnk_08743144[];
extern u32 gUnk_08743158[];
extern u32 gUnk_08743188[];
extern u32 gUnk_08743194[];
extern u32 gUnk_087431A0[];
extern u32 gUnk_087431AC[];
extern u32 gUnk_087431B8[];
extern u32 gUnk_087431C4[];
extern u32 gUnk_087431C8[];
extern u32 gUnk_087431CC[];
extern u32 gUnk_087431D8[];
extern u32 gUnk_087431E4[];
extern u32 gUnk_087521D8[];
extern u32 gUnk_08752248[];
extern u32 gUnk_087522B4[];
extern u32 gUnk_087523E4[];
extern u32 gUnk_08752808[];
extern u32 gUnk_08752C18[];

/* Externals */
extern void TaskDispatchTrampoline(void);
extern void TaskYieldTrampoline(u32 a);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern void sub_08002f14(s16 n);
extern void sub_080059d8(void);
extern void sub_08005d9c(void);
extern void sub_08006138(void);
extern void sub_08006148(void *a, u32 i);
extern void sub_0800617c(s16 a);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_080062c4(void);
extern void sub_08006304(void);
extern void sub_08006338(s32 a);
extern u16 sub_080064ac(s32 base, u8 scale, u8 amount);
extern s32 sub_08021a40(s32 x, s32 y);
extern void sub_0806395c(u16 v);
extern void sub_080639b4(void *p);
extern s32 sub_08063b38(void);
extern s32 sub_08063bd4(struct PointPair *p);
extern void sub_08063e14(void);
extern void sub_08063fe0(void);
extern void sub_08064038(void);
extern void sub_0806421c(s32 a, s32 b);
extern u8 sub_08064398(void);
extern void sub_08064680(s32 step, s32 limit, u16 dir);
extern s8 sub_08064a38(void);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern void sub_0806523c(void);
extern void sub_08065640(void);
extern void sub_080656b4(void);
extern void sub_08065760(u32 i, u32 p1, u8 p2);
extern void sub_08065848(u32 p0, s32 idx);
extern void sub_08068e04(void);
extern u8 sub_080692fc(void);
extern u8 sub_08069604(void);
extern u8 sub_08069660(void);
extern u8 sub_080699a8(void);
extern u32 sub_08069b44(void);
extern void sub_0806a0f0(s32 a);
extern void sub_0806a344(void);
extern void sub_0806ee2c(void);
extern void sub_0808e464(void);

/* Forward declarations */
void sub_0808cce8(void);
void sub_0808cfec(void);
void sub_0808d014(void);
void sub_0808d100(void);
void sub_0808d130(void);
void sub_0808d148(void);
void sub_0808d1d0(void);
void sub_0808d200(void);
void sub_0808d218(void);
void sub_0808d2a8(void);
s32 sub_0808d2b8(void);
s32 sub_0808d304(void);
s32 sub_0808d354(void);
s32 sub_0808d364(void);
s32 sub_0808d388(void);
void sub_0808d3e4(void);
void sub_0808d460(void);
void sub_0808d494(void);
void sub_0808d4a8(void);
void sub_0808d4bc(void);
void sub_0808d4d0(void);
void sub_0808d4e8(void);
void sub_0808d558(void);
void sub_0808d58c(void);
void sub_0808d624(void);
void sub_0808d640(void);
void sub_0808d764(void);
void sub_0808d790(void);
void sub_0808d938(void);
void sub_0808d964(void);
void sub_0808d9fc(void);
void sub_0808da00(void);
void sub_0808da34(void);
void sub_0808dacc(void);
void sub_0808dae8(void);
void sub_0808dc68(void);
void sub_0808dc94(void);
void sub_0808de90(void);
void sub_0808debc(void);
void sub_0808df54(void);
void sub_0808df58(void);
void sub_0808df9c(void);
void sub_0808dfc4(void);
void sub_0808e050(void);
void sub_0808e054(void);
void sub_0808e070(void);
void sub_0808e0d0(void);
void sub_0808e174(void);
void sub_0808e254(void);
void sub_0808e2b4(void);
void sub_0808e33c(void);
void sub_0808e36c(void);
void sub_0808e3a8(void);

void sub_0808cce8(void)
{

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 10;
    gUnk_03002490->unk38 = gUnk_087522B4;
    switch (gUnk_03002490->unk14)
    {
    case 0:
        sub_0808cfec();
        gUnk_03002490->unk4C = (sub_080064ac(-20, 1, 32) + gUnk_03002790[gUnk_03002490->unk44].unk48) << 16;
        gUnk_03002490->unk50 = (sub_080064ac(20, 1, 8) + gUnk_03002790[gUnk_03002490->unk44].unk4A) << 16;
        sub_080061c0(0xFFFF4000, 128 << 5);
        gUnk_03002490->unk58 = 0xFFFEC000;
        gUnk_03002490->unk60 = 0xFFFFE000;
        sub_08006338(2);
        TaskYieldTrampoline(3);
        sub_08006338(3);
        TaskYieldTrampoline(3);
        sub_08006338(4);
        TaskYieldTrampoline(3);
        sub_08006338(5);
        TaskYieldTrampoline(3);
        sub_0808cfec();
        gUnk_03002490->unk4C = (sub_080064ac(-20, 1, 32) + gUnk_03002790[gUnk_03002490->unk44].unk48) << 16;
        gUnk_03002490->unk50 = (sub_080064ac(4, 1, 8) + gUnk_03002790[gUnk_03002490->unk44].unk4A) << 16;
        sub_080061c0(0xFFFF4000, 128 << 5);
        gUnk_03002490->unk58 = 0xFFFEC000;
        gUnk_03002490->unk60 = 0xFFFFE000;
        sub_08006338(2);
        TaskYieldTrampoline(3);
        sub_08006338(3);
        TaskYieldTrampoline(3);
        sub_08006338(4);
        TaskYieldTrampoline(3);
        sub_08006338(5);
        TaskYieldTrampoline(3);
        sub_08006338(6);
        TaskYieldTrampoline(3);
        sub_08006338(7);
        TaskYieldTrampoline(3);
        break;
    case 1:
        sub_0808cfec();
        gUnk_03002490->unk4C = (sub_080064ac(-12, 1, 32) + gUnk_03002790[gUnk_03002490->unk44].unk48) << 16;
        gUnk_03002490->unk50 = (sub_080064ac(16, 1, 8) + gUnk_03002790[gUnk_03002490->unk44].unk4A) << 16;
        sub_080061c0(192 << 8, 0xFFFFF000);
        gUnk_03002490->unk58 = 0xFFFEC000;
        gUnk_03002490->unk60 = 0xFFFFE000;
        sub_08006338(2);
        TaskYieldTrampoline(3);
        sub_08006338(3);
        TaskYieldTrampoline(3);
        sub_08006338(4);
        TaskYieldTrampoline(3);
        sub_08006338(5);
        TaskYieldTrampoline(3);
        sub_0808cfec();
        gUnk_03002490->unk50 = (sub_080064ac(0, 1, 8) + gUnk_03002790[gUnk_03002490->unk44].unk4A) << 16;
        sub_080061c0(192 << 8, 0xFFFFF000);
        gUnk_03002490->unk58 = 0xFFFEC000;
        gUnk_03002490->unk60 = 0xFFFFE000;
        sub_08006338(2);
        TaskYieldTrampoline(3);
        sub_08006338(3);
        TaskYieldTrampoline(3);
        sub_08006338(4);
        TaskYieldTrampoline(3);
        sub_08006338(5);
        TaskYieldTrampoline(3);
        sub_08006338(6);
        TaskYieldTrampoline(3);
        sub_08006338(7);
        TaskYieldTrampoline(3);
        break;
    }
    TaskDispatchTrampoline();
}

void sub_0808cfec(void)
{
    if (gUnk_03004CA0[gUnk_03002490->unk44] != 104)
        TaskDispatchTrampoline();
}

void sub_0808d014(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    struct Task *z;
    s32 v;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_087523E4;
    u->unk04 = (u32)sub_0808d100;
    u->unk7A = 0;
    w = gUnk_03002490;
    w->unk28 = 0;
    v = gUnk_03002790[w->unk44].unk34;
    w->unk34 = v;
    if (w->unk74 <= 1)
        w->unk34 = v >> 1;
    gUnk_03002490->unk43 = sub_08064a38();
    sub_0806421c(gUnk_08742FAC[gUnk_03002490->unk34], 128 << 4);
    sub_080061c0(gUnk_030023B4, 0x5A5A5A5A);
    gUnk_03002490->unk58 = gUnk_030023D4;
    sub_08006338(0);
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08006338(0);
        TaskYieldTrampoline(1);
        sub_08006338(1);
        TaskYieldTrampoline(1);
        z = gUnk_03002490;
        z->unk6C++;
    } while ((s16)z->unk6C <= 2);
    sub_08063fe0();
}

void sub_0808d100(void)
{
    if (sub_08069660() != 0) {
        sub_080062c4();
        sub_08006148(sub_0806a344, gCurTaskIdx);
    } else {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0808d130(void)
{
    sub_08006148(sub_0806a344, gCurTaskIdx);
}

void sub_0808d148(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0808d1d0;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752808;
    gUnk_03002490->unk43 = sub_08064a38();
    gUnk_03002490->unk7A = 0;
    gUnk_03002490->unk58 = 0xFFFA0000;
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08006338(4);
        TaskYieldTrampoline(1);
        sub_08006338(5);
        TaskYieldTrampoline(1);
        t = gUnk_03002490;
        t->unk6C++;
    } while ((s16)t->unk6C <= 3);
    sub_08063fe0();
}

void sub_0808d1d0(void)
{
    if (sub_08069604() != 0)
    {
        sub_080062c4();
        sub_08006148(sub_0806a344, gCurTaskIdx);
    }
    else
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_0808d200(void)
{
    sub_08006148(sub_0806a344, gCurTaskIdx);
}

void sub_0808d218(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    t = gUnk_03002490;
    t->unk38 = gUnk_08752C18;
    t->unk04 = (u32)sub_0808d2a8;
    gUnk_03002490->unk43 = sub_08064a38();
    sub_080061c0(192 << 9, 0x5A5A5A5A);
    sub_0800622c(0xFFFD0000, 168 << 5, 192 << 10);
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(4);
        sub_08006338(5);
        TaskYieldTrampoline(4);
        sub_08006338(6);
        TaskYieldTrampoline(4);
        sub_08006338(7);
        TaskYieldTrampoline(4);
    }
}

void sub_0808d2a8(void)
{
    sub_08068e04();
    sub_08069b44();
}

s32 sub_0808d2b8(void)
{
    switch (gUnk_03002490->unk73)
    {
    case 0:
        sub_0806395c(2);
        sub_08006148(sub_0808d624, gCurTaskIdx);
        return 1;
    case 1:
        sub_0806395c(2);
        sub_08006148(sub_0808dacc, gCurTaskIdx);
        return 1;
    }
    return 0;
}

s32 sub_0808d304(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk30 = 1;
    t->unk34 = 1;
    switch (t->unk73)
    {
    case 0:
        sub_0806395c(0);
        sub_08006148(sub_0808d624, gCurTaskIdx);
        return 1;
    case 1:
        sub_0806395c(0);
        sub_08006148(sub_0808dacc, gCurTaskIdx);
        return 1;
    }
    return 0;
}

s32 sub_0808d354(void)
{
    sub_0806a0f0(-2);
    return 1;
}

s32 sub_0808d364(void)
{
    if (gUnk_03002490->unk54 != 0 && sub_080699a8() != 0)
        sub_080062c4();
    return 0;
}

s32 sub_0808d388(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk54 >= 0)
        t->unk2C = ((t->unk48 - 16) & 0xFFF0) + 15;
    else
        t->unk2C = (t->unk48 + 16) & 0xFFF0;
    u = gUnk_03002490;
    u->unk18 = u->unk2C - u->unk48;
    u->unk48 = u->unk2C + u->unk18;
    u->unk4C = u->unk48 << 16;
    return 0;
}

void sub_0808d3e4(void)
{
    struct Task *t;
    s8 i;
    u8 v;

    do
    {
        sub_08002f14((s16)sub_08002ee8(8));
        t = gUnk_03002490;
        i = gUnk_03001F08[0];
    } while (t->unk34 == gUnk_0874313C[i]);
    v = gUnk_0874313C[i];
    switch (v)
    {
    case 0:
        t->unk34 = v;
        sub_0806395c(1);
        break;
    case 1:
        t->unk34 = v;
        sub_0806395c(0);
        break;
    case 2:
        t->unk34 = v;
        t->unk43 = -1 * t->unk43;
        sub_0806395c(0);
        break;
    default:
        sub_0806ee2c();
        break;
    }
}

void sub_0808d460(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk43 == 1)
        t->unk3E |= 0x8000;
    else
        t->unk3E &= 0x7FFF;
}

void sub_0808d494(void)
{
    gUnk_03002490->unk8C->unk1A = 9;
}

void sub_0808d4a8(void)
{
    gUnk_03002490->unk8C->unk1A = 8;
}

void sub_0808d4bc(void)
{
    gUnk_03002490->unk8C->unk1A = 10;
}

void sub_0808d4d0(void)
{
    gUnk_03002490->unk8C->unk1A = -1;
}

void sub_0808d4e8(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk8C->unk16 = 0;
    t->unk8C->unk18 = 0;
    t->unk8C->unk1E = (t->unk40 & 0xFFF) | (240 << 8);
    t->unk34 = 1;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08065640;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_087521D8;
    sub_08002e98(u->unk73, 3, gUnk_08743188);
}

void sub_0808d558(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808d58c;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08743194);
}

void sub_0808d58c(void)
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
        sub_0808d364();
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_087431A0);
    }
    v = gUnk_03002490;
    v->unk24 = (v->unk24 & 0xFFFF0000) | v->unk4A;
    sub_08068e04();
    sub_08069b44();
}

void sub_0808d624(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08743194);
}

void sub_0808d640(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    gUnk_03002490->unk15 = 0;
    sub_08063e14();
    if (gUnk_03002490->unk34 == 2)
        sub_08064038();
    t = gUnk_03002490;
    t->unk28 = 0;
    t->unk6C = 0;
    do
    {
        sub_0808d460();
        sub_080061c0(gUnk_08743144[0], 0x5A5A5A5A);
        sub_0808d4d0();
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(5);
        sub_08006304();
        sub_080061c0(gUnk_08743144[0], 0x5A5A5A5A);
        sub_0808d4d0();
        sub_08006338(6);
        TaskYieldTrampoline(5);
        sub_080061c0(gUnk_08743144[1], 0x5A5A5A5A);
        sub_0808d4d0();
        sub_08006338(5);
        TaskYieldTrampoline(5);
        sub_080061c0(gUnk_08743144[1], 0x5A5A5A5A);
        sub_0808d494();
        sub_08006338(4);
        TaskYieldTrampoline(5);
        sub_080061c0(gUnk_08743144[2], 0x5A5A5A5A);
        sub_0808d494();
        sub_08006338(4);
        TaskYieldTrampoline(1);
        sub_080061c0(gUnk_08743144[2], 0x5A5A5A5A);
        sub_0808d4a8();
        sub_08006338(7);
        TaskYieldTrampoline(7);
        sub_080061c0(gUnk_08743144[3], 0x5A5A5A5A);
        sub_0808d4bc();
        sub_08006338(6);
        TaskYieldTrampoline(6);
        sub_080061c0(gUnk_08743144[4], 0x5A5A5A5A);
        sub_0808d4bc();
        sub_08006338(6);
        TaskYieldTrampoline(2);
        u = gUnk_03002490;
        u->unk6C++;
    } while ((s16)u->unk6C <= 1);
    sub_080062c4();
    v = gUnk_03002490;
    v->unk28 = 2;
    sub_08006138();
}

void sub_0808d764(void)
{
    if (gUnk_03002490->unk28 == 2)
    {
        sub_0808d3e4();
        sub_08006148(sub_0808d624, gCurTaskIdx);
    }
}

void sub_0808d790(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk28 = 0;
    sub_08063e14();
    sub_0808d460();
    sub_080061c0(gUnk_08743158[0], 0x5A5A5A5A);
    sub_0808d4d0();
    gUnk_03002490->unk3C = 6;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08006304();
        sub_080061c0(gUnk_08743158[0], 0x5A5A5A5A);
        sub_0808d494();
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(2);
        sub_080061c0(gUnk_08743158[0], 0x5A5A5A5A);
        sub_0808d494();
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(6);
        sub_080061c0(gUnk_08743158[1], 0x5A5A5A5A);
        sub_0808d4a8();
        sub_08006338(4);
        TaskYieldTrampoline(7);
        sub_080061c0(gUnk_08743158[1], 0x5A5A5A5A);
        sub_0808d4bc();
        sub_08006338(4);
        TaskYieldTrampoline(1);
        sub_080061c0(gUnk_08743158[2], 0x5A5A5A5A);
        sub_0808d4bc();
        sub_08006338(7);
        TaskYieldTrampoline(7);
        sub_080061c0(gUnk_08743158[2], 0x5A5A5A5A);
        sub_0808d4d0();
        sub_08006338(6);
        TaskYieldTrampoline(1);
        sub_0808d460();
        sub_080061c0(gUnk_08743158[3], 0x5A5A5A5A);
        sub_0808d494();
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(2);
        sub_080061c0(gUnk_08743158[3], 0x5A5A5A5A);
        sub_0808d494();
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(6);
        sub_080061c0(gUnk_08743158[4], 0x5A5A5A5A);
        sub_0808d4a8();
        sub_0800617c(4);
        TaskYieldTrampoline(7);
        sub_080061c0(gUnk_08743158[4], 0x5A5A5A5A);
        sub_0808d4bc();
        sub_0800617c(4);
        TaskYieldTrampoline(1);
        sub_080061c0(gUnk_08743158[5], 0x5A5A5A5A);
        sub_0808d4bc();
        sub_0800617c(7);
        TaskYieldTrampoline(7);
        sub_080061c0(gUnk_08743158[5], 0x5A5A5A5A);
        sub_0808d4d0();
        sub_0800617c(6);
        TaskYieldTrampoline(1);
        t = gUnk_03002490;
        t->unk6C++;
    } while ((s16)t->unk6C <= 2);
    sub_080062c4();
    gUnk_03002490->unk28 = 1;
    sub_08006138();
}

void sub_0808d938(void)
{
    if (gUnk_03002490->unk28 == 1)
    {
        sub_0808d3e4();
        sub_08006148(sub_0808d624, gCurTaskIdx);
    }
}

void sub_0808d964(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 2;
    sub_08063e14();
    sub_080062c4();
    t = gUnk_03002490;
    t->unk60 = 168 << 5;
    t->unk68 = 192 << 10;
    while (1)
    {
        sub_0808d460();
        sub_0808d4d0();
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(8);
        sub_08006304();
        sub_0808d4d0();
        sub_08006338(6);
        TaskYieldTrampoline(8);
        sub_0808d4d0();
        sub_08006338(5);
        TaskYieldTrampoline(8);
        sub_0808d494();
        sub_08006338(4);
        TaskYieldTrampoline(8);
        sub_08006338(7);
        sub_0808d4a8();
        sub_08006338(7);
        TaskYieldTrampoline(8);
        sub_0808d4bc();
        sub_08006338(6);
        TaskYieldTrampoline(8);
    }
}

void sub_0808d9fc(void)
{
}

void sub_0808da00(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808da34;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087431AC);
}

void sub_0808da34(void)
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
        sub_0808d364();
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_087431B8);
    }
    v = gUnk_03002490;
    v->unk24 = (v->unk24 & 0xFFFF0000) | v->unk4A;
    sub_08068e04();
    sub_08069b44();
}

void sub_0808dacc(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087431AC);
}

void sub_0808dae8(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk15 = 0;
    sub_08063e14();
    if (gUnk_03002490->unk34 == 2)
        sub_08064038();
    t = gUnk_03002490;
    t->unk28 = 0;
    t->unk6C = 0;
    do
    {
        sub_0808d460();
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743144[0], 0x5A5A5A5A);
        sub_0808d4d0();
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(7);
        sub_08006304();
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743144[0], 0x5A5A5A5A);
        sub_0808d4d0();
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(3);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743144[1], 0x5A5A5A5A);
        sub_0808d4d0();
        sub_08006338(6);
        TaskYieldTrampoline(4);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743144[1], 0x5A5A5A5A);
        sub_0808d4d0();
        sub_08006338(5);
        TaskYieldTrampoline(6);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743144[2], 0x5A5A5A5A);
        sub_0808d494();
        sub_08006338(4);
        TaskYieldTrampoline(6);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743144[2], 0x5A5A5A5A);
        sub_0808d4a8();
        sub_08006338(7);
        TaskYieldTrampoline(2);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743144[3], 0x5A5A5A5A);
        sub_0808d4a8();
        sub_08006338(7);
        TaskYieldTrampoline(4);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743144[3], 0x5A5A5A5A);
        sub_0808d4bc();
        sub_08006338(6);
        TaskYieldTrampoline(4);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743144[4], 0x5A5A5A5A);
        sub_0808d4bc();
        sub_08006338(6);
        TaskYieldTrampoline(2);
        u = gUnk_03002490;
        u->unk6C++;
    } while ((s16)u->unk6C <= 1);
    sub_080062c4();
    gUnk_03002490->unk28 = 2;
    sub_08006138();
}

void sub_0808dc68(void)
{
    if (gUnk_03002490->unk28 == 2)
    {
        sub_0808d3e4();
        sub_08006148(sub_0808dacc, gCurTaskIdx);
    }
}

void sub_0808dc94(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk28 = 0;
    sub_08063e14();
    sub_0808d460();
    if (gUnk_03002490->unk30 != 0)
        sub_080061c0(gUnk_08743158[0], 0x5A5A5A5A);
    sub_0808d4d0();
    gUnk_03002490->unk3C = 6;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08006304();
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743158[0], 0x5A5A5A5A);
        sub_0808d494();
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(2);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743158[0], 0x5A5A5A5A);
        sub_0808d494();
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(6);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743158[1], 0x5A5A5A5A);
        sub_0808d4a8();
        sub_08006338(4);
        TaskYieldTrampoline(7);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743158[1], 0x5A5A5A5A);
        sub_0808d4bc();
        sub_08006338(4);
        TaskYieldTrampoline(1);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743158[2], 0x5A5A5A5A);
        sub_0808d4bc();
        sub_08006338(7);
        TaskYieldTrampoline(7);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743158[2], 0x5A5A5A5A);
        sub_0808d4d0();
        sub_08006338(6);
        TaskYieldTrampoline(1);
        sub_0808d460();
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743158[3], 0x5A5A5A5A);
        sub_0808d494();
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(2);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743158[3], 0x5A5A5A5A);
        sub_0808d494();
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(6);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743158[4], 0x5A5A5A5A);
        sub_0808d4a8();
        sub_0800617c(4);
        TaskYieldTrampoline(7);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743158[4], 0x5A5A5A5A);
        sub_0808d4bc();
        sub_0800617c(4);
        TaskYieldTrampoline(1);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743158[5], 0x5A5A5A5A);
        sub_0808d4bc();
        sub_0800617c(7);
        TaskYieldTrampoline(7);
        if (gUnk_03002490->unk30 != 0)
            sub_080061c0(gUnk_08743158[5], 0x5A5A5A5A);
        sub_0808d4d0();
        sub_0800617c(6);
        TaskYieldTrampoline(1);
        t = gUnk_03002490;
        t->unk6C++;
    } while ((s16)t->unk6C <= 2);
    sub_080062c4();
    gUnk_03002490->unk28 = 1;
    sub_08006138();
}

void sub_0808de90(void)
{
    if (gUnk_03002490->unk28 == 1)
    {
        sub_0808d3e4();
        sub_08006148(sub_0808dacc, gCurTaskIdx);
    }
}

void sub_0808debc(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 2;
    sub_08063e14();
    sub_080062c4();
    t = gUnk_03002490;
    t->unk60 = 168 << 5;
    t->unk68 = 192 << 10;
    while (1)
    {
        sub_0808d460();
        sub_0808d4d0();
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(8);
        sub_08006304();
        sub_0808d4d0();
        sub_08006338(6);
        TaskYieldTrampoline(8);
        sub_0808d4d0();
        sub_08006338(5);
        TaskYieldTrampoline(8);
        sub_0808d494();
        sub_08006338(4);
        TaskYieldTrampoline(8);
        sub_08006338(7);
        sub_0808d4a8();
        sub_08006338(7);
        TaskYieldTrampoline(8);
        sub_0808d4bc();
        sub_08006338(6);
        TaskYieldTrampoline(8);
    }
}

void sub_0808df54(void)
{
}

void sub_0808df58(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808df9c;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087431C4);
}

void sub_0808df9c(void)
{
    sub_080692fc();
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087431C8);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808dfc4(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08063e14();
    gUnk_03002490->unk7A = 1;
    sub_080062c4();
    while (1)
    {
        sub_0808d460();
        sub_0808d4d0();
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(8);
        sub_08006304();
        sub_0808d4d0();
        sub_08006338(6);
        TaskYieldTrampoline(8);
        sub_0808d4d0();
        sub_08006338(5);
        TaskYieldTrampoline(8);
        sub_0808d494();
        sub_08006338(4);
        TaskYieldTrampoline(8);
        sub_0808d4a8();
        sub_08006338(7);
        TaskYieldTrampoline(8);
        sub_0808d4bc();
        sub_08006338(6);
        TaskYieldTrampoline(8);
    }
}

void sub_0808e050(void)
{
}

void sub_0808e054(void)
{
    if (--gUnk_02007FB8[1] < 0)
        sub_0806ee2c();
}

void sub_0808e070(void)
{
    switch (sub_08064398())
    {
    case 4:
        gUnk_03002490->unk2C = (&gUnk_03002790[sub_08063b38()])->unk48 - 64;
        break;
    case 8:
        gUnk_03002490->unk2C = (&gUnk_03002790[sub_08063b38()])->unk48 + 64;
        break;
    }
}

void sub_0808e0d0(void)
{
    struct Task *t;
    s32 dx;
    s32 dy;

    switch (sub_08064398())
    {
    case 4:
        gUnk_03002490->unk2C = (&gUnk_03002790[sub_08063b38()])->unk48 - 64;
        break;
    case 8:
        gUnk_03002490->unk2C = (&gUnk_03002790[sub_08063b38()])->unk48 + 64;
        break;
    }
    t = gUnk_03002490;
    dx = (s16)(t->unk2C - (u16)t->unk48);
    dy = (s16)((u16)(&gUnk_03002790[sub_08063b38()])->unk4A - (u16)gUnk_03002490->unk4A);
    gUnk_03002490->unk18 = (((u16)ArcTan2(dx, dy) >> 8) + 16) >> 5;
}

void sub_0808e174(void)
{
    struct Task *t;
    s32 dx;
    s32 dy;
    s32 i;

    t = gUnk_03002490;
    dx = (s16)(t->unk2C - (u16)t->unk48);
    dy = (s16)((u16)(&gUnk_03002790[sub_08063b38()])->unk4A - (u16)gUnk_03002490->unk4A);
    i = (((u16)ArcTan2(dx, dy) >> 8) + 16) >> 5;
    switch (i)
    {
    case 0:
        gUnk_03002490->unk34 = 0;
        break;
    case 1:
        gUnk_03002490->unk34 = 1;
        break;
    case 2:
        gUnk_03002490->unk34 = 2;
        break;
    case 3:
        gUnk_03002490->unk34 = 3;
        break;
    case 4:
        gUnk_03002490->unk34 = 4;
        break;
    case 5:
        gUnk_03002490->unk34 = 5;
        break;
    case 6:
        gUnk_03002490->unk34 = 6;
        break;
    case 7:
        gUnk_03002490->unk34 = 7;
        break;
    }
}

void sub_0808e254(void)
{
    struct ActorSpawn sp;
    struct Task *t;

    t = gUnk_03002490;
    if (sub_08021a40(t->unk48 + (t->unk43 << 4), t->unk4A) == 0)
    {
        sp.unk00 = 1;
        sp.unk04 = 103;
        sp.unk08 = 0;
        sp.unk09 = gUnk_03002490->unk34;
        sp.unk0C = 16;
        sp.unk0E = 0;
        sp.unk0A = 1;
        sub_08064b5c(&sp, 0);
    }
}

void sub_0808e2b4(void)
{
    struct PointPair p;
    struct Task *t;

    p.x0 = gUnk_03002490->unk2C;
    p.y0 = (&gUnk_03002790[sub_08063b38()])->unk4A;
    t = gUnk_03002490;
    p.x1 = t->unk48;
    p.y1 = t->unk4A;
    if (sub_08063bd4(&p) <= 16)
    {
        sub_0806395c(1);
        sub_08006148(sub_0808e464, gCurTaskIdx);
    }
}

void sub_0808e33c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk30 == 0)
    {
        t->unk30 = 16;
        t->unk28 = t->unk18;
        sub_0808e0d0();
        u = gUnk_03002490;
        if (u->unk28 != u->unk18)
            sub_0808e174();
    }
}

void sub_0808e36c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk30 == 16 || t->unk30 == 8)
        sub_08064680(gUnk_087431CC[t->unk74], gUnk_087431D8[t->unk74], (u16)t->unk34);
}

void sub_0808e3a8(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752248;
    sub_08065848(3, 1);
    sub_08065760(1, 0, gUnk_03002490->unk8C->unk0C);
    sub_08002e98(gUnk_03002490->unk73, 2, gUnk_087431E4);
}

