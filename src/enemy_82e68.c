/* game_code_and_rodata 0x08082E68-0x080844C4 (issue #69, module M22 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08082E68 0x080844C4 src/enemy_82e68.c --newpb
 *
 * M22 is a bank of five enemy/object behaviour scripts, all built to the same
 * three-table pattern (rom-map section 9):
 *
 *   entry     -> installs Task.unk04 (the per-frame hook) and hands
 *                Task.unk73 / Task.unk14 to CallTableEntry, which indexes the
 *                script's table;
 *   unk14 table -> the coroutine BODIES: each sets Task.unk15 to its own state
 *                number and then runs a chain of TaskYieldTrampoline waits;
 *   unk15 table -> the per-frame HANDLERS: each is the six-instruction guard
 *                `if (Task.unk14 != N) TaskSetEntry(entry, gCurTaskIdx);` that
 *                re-arms the entry whenever the requested state changes.
 *
 * The three tables of one script sit consecutively in ROM, so the entry's
 * `count` argument is what separates them (`0x08741778` + 7*4 = `0x08741794`).
 *
 * This batch holds:
 *   * the walker script `sub_08082e68` (7 states, tables `0x08741778` /
 *     `0x08741794`, per-frame hook `sub_08082eb4`, re-arm `sub_08082e98`);
 *   * its terrain library: `sub_08083a48` / `sub_08083ad4` / `sub_08083bbc` /
 *     `sub_08083cb8` probe the room with sub_08021b18/sub_08021bb4 and turn the
 *     `gUnk_087339F0` / `gUnk_08732CF0` / `gUnk_087416A4` index chain into a
 *     tile class, `sub_08083d28` turns a direction code into an aim angle plus
 *     a 16.16 velocity through sub_0806421c, and `sub_08083dfc` is the
 *     five-times-four-frame animation wait;
 *   * the one-state script `sub_0808398c` (`0x087417B0` / `0x087417B4`);
 *   * the class-2 task #105 script `sub_08083e6c` (`0x08741E64` /
 *     `0x08741E68` / `0x08741E6C`);
 *   * the class-2 task #108 script `sub_08084050`, whose unk73 table
 *     `0x08741E7C` has two rows (`0x08741E84`/`0x08741E88` and
 *     `0x08741E8C`/`0x08741E90`);
 *   * the class-2 task #176 one-shot `sub_080843fc` and the class-3 task #10
 *     entry `sub_08084484`, whose script continues in src/enemy_844c4.c.
 *
 * `sub_080839d0`, `sub_08083ee8`, `sub_080840d4` and `sub_0808429c` are dead
 * exports: each is a copy of its host's tail dispatch that nothing in the ROM
 * references (lesson 4.30 / 4.34, curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern vu16 gTaskSlotTypes[];
extern s32 gUnk_03001F2C;

/* ROM tables */
extern s8 gUnk_087339F0[];
extern u8 gUnk_08732CF0[];
extern u32 gUnk_0873F500[];
extern u32 gUnk_0873F720[];
extern u32 gUnk_0873F758[];
extern u32 gUnk_08741778[];
extern u32 gUnk_08741794[];
extern u8 gUnk_087416AD[];
extern s32 gUnk_087416B0[];
extern u8 gUnk_087416CC[];
extern u32 gUnk_0875233C[];
extern u32 gUnk_08752794[];
extern s32 gUnk_08741E54[];
extern s32 gUnk_08741E5C[];
extern u32 gUnk_08741F64[];
extern u32 gUnk_08741E90[];
extern s32 gUnk_08741E94[];
extern s32 gUnk_08741EA4[];
extern u32 gUnk_08741F70[];
extern u32 gUnk_08752150[];
extern u32 gUnk_08752AB4[];
extern s16 gUnk_08741E70[];
extern u32 gUnk_08741E7C[];
extern u32 gUnk_08741E84[];
extern u32 gUnk_08741E88[];
extern u32 gUnk_08741E8C[];
extern u32 gUnk_08741E64[];
extern u32 gUnk_08741E68[];
extern u32 gUnk_08741E6C[];
extern s8 gUnk_08741684[];
extern s8 gUnk_08741688[];
extern s8 gUnk_0874168C[];
extern s8 gUnk_08741690[];
extern s8 gUnk_08741694[];
extern s8 gUnk_08741698[];
extern s8 gUnk_0874169C[];
extern s8 gUnk_087416A0[];
extern u8 gUnk_087416A4[];
extern u16 gUnk_087416D4[];
extern u16 gUnk_087416E4[];
extern s16 gUnk_087416EC[][2];
extern s32 gUnk_087416F8[];
extern s32 gUnk_08741708[];
extern s32 gUnk_08741718[];
extern s32 gUnk_08741728[];
extern struct AnimCmd gUnk_08741744[];
extern struct AnimCmd gUnk_08741758[];
extern u8 gUnk_08741738[];
extern s16 gUnk_0874173C[];
extern u32 gUnk_087417B0[];
extern u32 gUnk_087417B4[];

/* Externals */
extern void TaskYieldTrampoline(u32 frames);
extern void TaskExitTrampoline(void);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(s32 a);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, s32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskStopX(void);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void sub_0806395c(s32 a);
extern void sub_080639a4(u32 *p);
extern void sub_080639b4(u32 *p);
extern void sub_08063e14(void);
extern u16 sub_08064314(s32 a);
extern s32 sub_08064188(s32 n);
extern void sub_0806421c(s32 a, s32 b);
extern void sub_08063d7c(void);
extern void sub_08064a60(void);
extern void sub_0806a0f0(s32 a);
extern u8 sub_0806951c(void);
extern void sub_0806a344(void);
extern void sub_080656b4(void);
extern void sub_0806523c(void);
extern s32 sub_08063cd0(void);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_08064cdc(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern s32 sub_08063df4(void);
extern s32 sub_08063bd4(struct PointPair *p);
extern s32 sub_08063f00(struct PointPair *p);
extern s32 sub_08063e2c(struct PointPair *r);
extern void sub_08063fe0(void);
extern s32 sub_08021b18(u16 x, u16 y);
extern s32 sub_08021bb4(s16 x, s16 y, s32 c, s32 d);
extern void sub_08068e04(void);
extern u8 sub_080692fc(void);
extern void sub_08069b44(void);

/* Defined below */
void sub_08082eb4(void);
void sub_080839ec(void);
u8 sub_08083a48(s32 dir);
u8 sub_08083ad4(s32 dir);
u8 sub_08083bbc(s32 dir, s32 k);
u8 sub_08083cb8(s16 x, s16 y);
s32 sub_08083d28(u8 a);
void sub_08083dfc(void);
void sub_08083f04(void);
void sub_080840f0(void);
void sub_080842b8(void);

void sub_08082e68(void)
{
    gCurTask->unk04 = (u32)sub_08082eb4;
    sub_0806395c(0);
    CallTableEntry(gCurTask->unk14, 7, gUnk_08741778);
}

void sub_08082e98(void)
{
    CallTableEntry(gCurTask->unk14, 7, gUnk_08741778);
}

void sub_08082eb4(void)
{
    switch (gCurTask->unk15)
    {
    case 0:
    case 1:
    case 2:
        if (sub_080692fc() == 0)
            CallTableEntry(gCurTask->unk15, 7, gUnk_08741794);
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        CallTableEntry(gCurTask->unk15, 7, gUnk_08741794);
        break;
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_08082f04(void)
{
    struct Task *t;
    u16 v;

    gCurTask->unk15 = 0;
    sub_08063e14();
    TaskSetFrame(4);
    TaskYieldTrampoline(1);
    t = gCurTask;
    t->unk18 = 0;
    t->unk30 = 4;
    do
    {
        t = gCurTask;
        t->unk30 -= 2;
        if (sub_08083a48(t->unk30) != 0)
        {
            gCurTask->unk18 = 1;
            v = sub_08064314(2);
            t = gCurTask;
            if ((t->unk30 & 1) == 0)
            {
                if ((u16)(v - 64) > 128)
                    t->unk34 = 3;
                else
                    t->unk34 = 1;
            }
            else
            {
                if (v <= 127)
                    t->unk34 = 0;
                else
                    t->unk34 = 2;
            }
            gCurTask->unk1C = 0;
            goto done;
        }
    } while (gCurTask->unk30 != 0);
done:
    if (gCurTask->unk18 == 0)
        sub_0806395c(2);
    else
        sub_0806395c(1);
    TaskSleepForever();
}

void sub_08082fb4(void)
{
    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_08082e98, gCurTaskIdx);
}

void sub_08082fdc(void)
{
    struct Task *t;

    gCurTask->unk15 = 1;
    TaskStop();
    TaskSetFrame(4);
    while (1)
    {
        TaskYieldTrampoline(gUnk_087416AD[gCurTask->unk74]);
        t = gCurTask;
        t->unk3C++;
        if ((s16)t->unk3C > 7)
            t->unk3C = 4;
    }
}

void sub_08083020(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct PointPair p;
    s32 n;
    s32 n2;
    s32 m;
    u8 d;
    u8 e;
    u8 f;

    t = gCurTask;
    switch (t->unk1C)
    {
    case 0:
        d = sub_08083ad4(t->unk30);
        if (d == 0)
        {
            u = gCurTask;
            u->unk1C = 1;
            u->unk54 = 0;
            u->unk58 = 0;
            break;
        }
        w = gCurTask;
        w->unk18 = d;
        if ((w->unk30 & 2) != 0 && d == 1)
        {
            e = sub_08083bbc(w->unk30, w->unk34);
            f = sub_08083bbc(gCurTask->unk30, 0);
            if (e > 5 && f > 5)
                gCurTask->unk18 = e;
        }
        d = sub_08083a48(gCurTask->unk34);
        if (d == 1 || ((gCurTask->unk34 & 1) == 0 && d > 1))
        {
            gUnk_03001F2C = n = gCurTask->unk34;
            gCurTask->unk34 = (gCurTask->unk30 + 2) & 3;
            gCurTask->unk30 = n;
            sub_08083d28(d);
        }
        else
        {
            sub_08083d28(gCurTask->unk18);
        }
        break;
    case 1:
        m = t->unk18;
        if (m > 1)
        {
            t->unk4C &= 0xFFFF0000;
            t->unk50 &= 0xFFFF0000;
            t->unk4C += gUnk_087416F8[t->unk30] - gUnk_08741718[t->unk34];
            t->unk50 += gUnk_08741708[t->unk30] - gUnk_08741728[t->unk34];
            t->unk18 = 1;
            t->unk1C = 0;
        }
        else
        {
            t->unk4C = (t->unk4C & 0xFFF00000) | 0x80000;
            t->unk50 = (t->unk50 & 0xFFF00000) | 0x80000;
            t->unk4C += gUnk_087416F8[t->unk30] + gUnk_08741718[t->unk34];
            t->unk50 += gUnk_08741708[t->unk30] + gUnk_08741728[t->unk34];
            t->unk1C = 2;
            gUnk_03001F2C = n2 = t->unk30;
            t->unk30 = (t->unk34 + 2) & 3;
            t->unk34 = n2;
            sub_08083d28(m);
        }
        x = gCurTask;
        x->unk48 = x->unk4C >> 16;
        x->unk4A = x->unk50 >> 16;
        break;
    case 2:
        if (sub_08083ad4(t->unk30) == 0)
            sub_0806395c(0);
        y = gCurTask;
        y->unk1C = 0;
        if ((y->unk34 & 1) != 0)
            y->unk58 = 0;
        break;
    }
    v = gCurTask;
    if (v->unk30 != 0)
        v->unk7A = 0;
    else
        v->unk7A = 1;
    z = gCurTask;
    if ((z->unk28 & 0x80) == 0 && --z->unk28 == 0)
    {
        p.x0 = z->unk48 - 64;
        p.y0 = z->unk4A - 64;
        p.x1 = z->unk48 + 64;
        p.y1 = gCurTask->unk4A + 64;
        if (sub_08063f00(&p) != 0)
            sub_0806395c(3);
        gCurTask->unk28 = 20;
    }
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_08082e98, gCurTaskIdx);
}

void sub_080832d0(void)
{
    struct Task *t;

    gCurTask->unk15 = 2;
    t = gCurTask;
    t->unk30 = 0;
    t->unk18 = 0;
    t->unk7A = 0;
    TaskStopX();
    TaskSetMotionY(0, 0x1500, 0x30000);
    TaskSetFrame(4);
    TaskSleepForever();
}

void sub_0808330c(void)
{
    u16 m;

    if (gCurTask->unk7A != 0)
    {
        m = sub_08064314(2);
        m -= 64;
        if (m > 128)
            gCurTask->unk34 = 3;
        else
            gCurTask->unk34 = 1;
        gCurTask->unk1C = 0;
        sub_0806395c(1);
        TaskSetEntry(sub_08082e98, gCurTaskIdx);
    }
}

void sub_08083370(void)
{
    struct Task *t;

    gCurTask->unk15 = 3;
    sub_08063e14();
    TaskStop();
    gCurTask->unk7A = 0;
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    TaskSetFrame(4);
    gCurTask->unk6C = 0;
    do
    {
        t = gCurTask;
        t->unk58 = gUnk_087416B0[(s16)t->unk6C];
        TaskYieldTrampoline(gUnk_087416CC[(s16)t->unk6C]);
        gCurTask->unk3C++;
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    sub_08083dfc();
    sub_0806395c(4);
    TaskSleepForever();
}

void sub_08083400(void)
{
    if (gCurTask->unk14 != 3)
        TaskSetEntry(sub_08082e98, gCurTaskIdx);
}

void sub_08083428(void)
{
    struct Task *t;

    gCurTask->unk15 = 4;
    sub_080639b4(gUnk_0873F758);
    gCurTask->unk20 = sub_08064314(3);
    gCurTask->unk1C = 1;
    while (1)
    {
        TaskSetFrame(8);
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        do
        {
            t = gCurTask;
            t->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 2);
    }
}

void sub_08083488(void)
{
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct PointPair p;
    s16 a;

    if (--gCurTask->unk1C == 0)
    {
        sub_08063d7c();
        if (sub_08063df4() == 1)
            gUnk_030023B4 = gUnk_030023B4 - 64;
        else
            gUnk_030023B4 = gUnk_030023B4 + 64;
        a = (u16)ArcTan2(gUnk_030023B4 - gCurTask->unk48, gUnk_030023D4 - gCurTask->unk4A) >> 7;
        if (a > 384 && gCurTask->unk20 <= 127)
            a = a - 512;
        else if (a <= 127 && gCurTask->unk20 > 384)
            a = a + 512;
        if (a > (u = gCurTask)->unk20)
            u->unk20 = u->unk20 + 32;
        else
            u->unk20 = u->unk20 - 32;
        v = gCurTask;
        v->unk20 &= 0x1FF;
        p.x0 = v->unk48;
        p.y0 = v->unk4A;
        p.x1 = gUnk_030023B4;
        p.y1 = gUnk_030023D4;
        if (sub_08063bd4(&p) <= 99)
        {
            sub_0806395c(5);
            TaskSetEntry(sub_08082e98, gCurTaskIdx);
        }
        else
        {
            w = gCurTask;
            w->unk1C = gUnk_08741738[w->unk74];
            sub_0806421c((s16)w->unk20, gUnk_0874173C[w->unk74]);
            x = gCurTask;
            x->unk54 = gUnk_030023B4;
            x->unk58 = gUnk_030023D4;
        }
    }
}

void sub_08083614(void)
{
    s32 n;

    gCurTask->unk15 = 5;
    sub_08063e14();
    TaskStop();
    gCurTask->unk20 = sub_080640dc(gUnk_08741744);
    TaskSetMotionXFacing(-0x18000, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(-0xC000, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(0x40000, 0x5A5A5A5A);
    gCurTask->unk20 = sub_080640dc(gUnk_08741758);
    gCurTask->unk24 = -1;
    while (1)
    {
        gCurTask->unk24++;
        if ((gCurTask->unk24 & 3) == 0)
        {
            gCurTask->unk46 = sub_08064cdc(176, 0, 0, 1);
            (gTasks + (s16)gCurTask->unk46)->unk18 = (gCurTask->unk24 >> 2) & 3;
        }
        n = sub_08063cd0();
        gCurTask->unk1C = n;
        if (gCurTask->unk43 == 1 && n < 0)
            break;
        if (gCurTask->unk43 == -1 && n > 0)
            break;
        TaskYieldTrampoline(1);
    }
    TaskYieldTrampoline(12);
    gCurTask->unk20 = sub_080640dc(gUnk_08741744);
    TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
    gCurTask->unk58 = -0x10000;
    TaskYieldTrampoline(6);
    TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
    gCurTask->unk58 = -0x20000;
    TaskYieldTrampoline(6);
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    gCurTask->unk58 = -0x30000;
    TaskYieldTrampoline(6);
    if (gCurTask->unk2C == 0)
    {
        TaskSetMotionXFacing(0, 0x5A5A5A5A);
        gCurTask->unk58 = -0x20000;
        TaskYieldTrampoline(6);
        gCurTask->unk58 = -0x10000;
        TaskYieldTrampoline(6);
        TaskStop();
        gCurTask->unk2C++;
        sub_0806395c(6);
    }
    TaskSleepForever();
}

void sub_0808379c(void)
{
    gCurTask->unk20 = sub_08064188(gCurTask->unk20);
    if (gCurTask->unk14 != 5)
        TaskSetEntry(sub_08082e98, gCurTaskIdx);
}

void sub_080837d0(void)
{
    struct Task *t;

    gCurTask->unk15 = 6;
    sub_080639b4(gUnk_0873F720);
    gCurTask->unk20 = sub_08064314(3);
    gCurTask->unk1C = 1;
    TaskSetFrame(8);
    while (gCurTask->unk1C != 0)
    {
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->unk3C++;
        if ((s16)t->unk3C > 11)
            t->unk3C = 8;
    }
    TaskStop();
    sub_08083dfc();
    TaskSetFrame(8);
    TaskYieldTrampoline(6);
    gCurTask->unk6C = 0;
    do
    {
        t = gCurTask;
        t->unk3C++;
        TaskYieldTrampoline(6);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskSetFrame(4);
    TaskYieldTrampoline(8);
    gCurTask->unk6C = 0;
    do
    {
        t = gCurTask;
        t->unk3C++;
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    gCurTask->unk28 = 20;
    sub_0806395c(2);
    TaskSleepForever();
}

void sub_080838bc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    u16 n;

    t = gCurTask;
    if (t->unk1C != 0)
    {
        if (--t->unk1C == 0)
        {
            n = sub_08064314(3);
            u = gCurTask;
            if (n > u->unk20)
                u->unk20 = u->unk20 + 32;
            else
                u->unk20 = u->unk20 - 32;
            gUnk_03001F2C = 4;
            while (gUnk_03001F2C != 0 && sub_08083ad4(gUnk_03001F2C - 1) == 0)
                gUnk_03001F2C--;
            if (gUnk_03001F2C != 0)
            {
                v = gCurTask;
                v->unk1C = gUnk_08741738[v->unk74];
                sub_0806421c((s16)v->unk20, gUnk_0874173C[v->unk74]);
                w = gCurTask;
                w->unk54 = gUnk_030023B4;
                w->unk58 = gUnk_030023D4;
            }
        }
    }
    if (gCurTask->unk14 != 6)
        TaskSetEntry(sub_08082e98, gCurTaskIdx);
}

void sub_0808398c(void)
{
    gCurTask->unk04 = (u32)sub_080839ec;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gCurTask->unk78 = 2;
    sub_0806395c(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087417B0);
}

void sub_080839d0(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_087417B0);
}

void sub_080839ec(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087417B4);
    sub_08068e04();
    sub_08069b44();
}

void sub_08083a10(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    TaskSetFrame(4);
    while (1)
    {
        TaskYieldTrampoline(10);
        t = gCurTask;
        t->unk3C++;
        if ((s16)t->unk3C > 7)
            t->unk3C = 4;
    }
}

void sub_08083a44(void)
{
}

u8 sub_08083a48(s32 dir)
{
    struct Task *t;
    s16 a;
    s16 b;
    s16 i;
    u8 r;

    r = 0;
    t = gCurTask;
    a = t->unk48 + gUnk_08741684[dir];
    b = t->unk4A + gUnk_08741688[dir];
    i = sub_08021b18(a, b);
    if (gUnk_087339F0[i] != 0)
    {
        r = gUnk_087416A4[gUnk_08732CF0[i]];
        if ((u8)(r - 2) <= 3 && gCurTask->unk7A == 0)
            r = 0;
    }
    return r;
}

u8 sub_08083ad4(s32 dir)
{
    struct Task *t;
    u16 a;
    u16 b;
    u16 c;
    u16 d;
    u8 r;

    t = gCurTask;
    if ((t->unk34 & 2) != 0)
    {
        a = t->unk48 + gUnk_0874168C[dir];
        b = t->unk4A + gUnk_08741690[dir];
        c = t->unk48 + gUnk_08741694[dir];
        d = t->unk4A + gUnk_08741698[dir];
    }
    else
    {
        a = t->unk48 + gUnk_08741694[dir];
        b = t->unk4A + gUnk_08741698[dir];
        c = t->unk48 + gUnk_0874168C[dir];
        d = t->unk4A + gUnk_08741690[dir];
    }
    r = sub_08083cb8(a, b);
    if (r == 0)
        r = sub_08083cb8(c, d);
    return r;
}

u8 sub_08083bbc(s32 dir, s32 k)
{
    struct Task *t;
    u16 a;
    u16 b;
    s16 i;
    u8 r;
    s8 *p;
    s8 *q;
    r = 0;
    t = gCurTask;
    if ((t->unk34 & 2) != 0)
    {
        a = t->unk48 + gUnk_0874168C[dir];
        b = t->unk4A + gUnk_08741690[dir];
    }
    else
    {
        a = t->unk48 + gUnk_08741694[dir];
        b = t->unk4A + gUnk_08741698[dir];
    }
    p = &gUnk_0874169C[k];
    q = &gUnk_087416A0[k];
    i = sub_08021bb4(a, b, *p, *q);
    if (i == -1)
        return 0;
    if (gUnk_087339F0[i] != 0)
    {
        r = gUnk_087416A4[gUnk_08732CF0[i]];
        if ((u8)(r - 2) <= 3 && gCurTask->unk7A == 0)
            r = 0;
    }
    return r;
}

u8 sub_08083cb8(s16 x, s16 y)
{
    s32 i;
    u8 r;

    r = 0;
    i = (s16)sub_08021b18(x, y);
    if (i == -1)
        return 0;
    if (gUnk_087339F0[i] != 0)
    {
        r = gUnk_087416A4[gUnk_08732CF0[i]];
        if ((u8)(r - 2) <= 3 && gCurTask->unk7A == 0)
            r = 0;
    }
    return r;
}

s32 sub_08083d28(u8 a)
{
    struct Task *t;
    s16 v;

    if (a > 5)
    {
        v = gUnk_087416D4[a - 2];
        if (gCurTask->unk34 == 1)
            v = (v + 272) & 0x1FF;
        else
            v = (v - 16) & 0x1FF;
    }
    else if (a > 1)
    {
        v = gUnk_087416D4[a - 2];
        if (gCurTask->unk34 == 1)
            v = (v + 256) & 0x1FF;
    }
    else
    {
        v = gUnk_087416E4[gCurTask->unk34];
    }
    sub_0806421c(v, gUnk_087416EC[gCurTask->unk74][0]);
    t = gCurTask;
    t->unk54 = gUnk_030023B4;
    t->unk58 = gUnk_030023D4;
}

void sub_08083dfc(void)
{
    struct Task *t;

    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(8);
        TaskYieldTrampoline(3);
        t = gCurTask;
        t->unk3C++;
        TaskYieldTrampoline(3);
        t = gCurTask;
        t->unk3C++;
        TaskYieldTrampoline(3);
        t = gCurTask;
        t->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 4);
}

u8 sub_08083e5c(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_08083e6c(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 9;
    gCurTask->unk38 = gUnk_0875233C;
    PlaySfx(186);
    CallTableEntry(gCurTask->unk73, 1, gUnk_08741E64);
}

void sub_08083eb4(void)
{
    gCurTask->unk04 = (u32)sub_08083f04;
    sub_08064a60();
    sub_0806395c(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741E68);
}

void sub_08083ee8(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741E68);
}

void sub_08083f04(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_08741E6C);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_08083f48(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    gCurTask->unk7A = 0;
    TaskSetMotionXFacing(gUnk_08741E54[gCurTask->unk74], gUnk_08741E5C[gCurTask->unk74]);
    gCurTask->unk64 = 0x2A800;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->unk3C++;
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->unk3C++;
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->unk3C++;
        TaskYieldTrampoline(2);
    }
}

void sub_08083fbc(void)
{
    struct Task *t;
    struct PointPair p;
    vu16 *q;
    s16 i;

    q = gTaskSlotTypes;
    i = gCurTask->unk44;
    if ((s16)q[i] != -1)
    {
        t = gTasks + i;
        p.x0 = t->unk48 - 6;
        p.y0 = t->unk4A - 6;
        p.x1 = t->unk48 + 6;
        p.y1 = t->unk4A + 6;
        if (sub_08063e2c(&p) != 0)
            sub_08063fe0();
    }
}

void sub_08084050(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 3;
    u = gCurTask;
    u->unk38 = gUnk_08752794;
    u->unk7A = 0;
    sub_08064a60();
    PlaySfx(194);
    CallTableEntry(gCurTask->unk73, 2, gUnk_08741E7C);
}

void sub_080840a4(void)
{
    gCurTask->unk04 = (u32)sub_080840f0;
    sub_0806395c(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741E84);
}

void sub_080840d4(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741E84);
}

void sub_080840f0(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_08741E88);
    sub_08068e04();
    sub_08069b44();
}

void sub_08084114(void)
{
    struct Task *t;
    struct Task *o;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    s32 n;
    s32 m;

    t = gCurTask;
    o = gTasks + (s16)t->unk44;
    t->unk15 = 0;
    u = gCurTask;
    if (u->unk43 == 1)
        u->unk28 = 0;
    else
        u->unk28 = 256;
    v = gCurTask;
    n = (s16)o->unk6E;
    v->unk2C = n;
    m = (v->unk28 + gUnk_08741E70[n]) & 0x1FF;
    v->unk28 = m;
    if ((u32)(m - 128) > 255)
        v->unk4C = v->unk4C + 0xE0000;
    else
        v->unk4C = v->unk4C - 0xE0000;
    sub_0806421c((s16)gCurTask->unk28, 768);
    w = gCurTask;
    w->unk54 = gUnk_030023B4;
    w->unk58 = gUnk_030023D4;
    if ((w->unk2C & 2) != 0)
        w->unk3E = w->unk3E & 0x7FFF;
    else
        w->unk3E = w->unk3E | 0x8000;
    gCurTask->unk3C = 4;
    TaskYieldTrampoline(2);
    x = gCurTask;
    x->unk3C++;
    TaskYieldTrampoline(3);
    x = gCurTask;
    x->unk3C++;
    TaskYieldTrampoline(3);
    x = gCurTask;
    x->unk3C++;
    TaskYieldTrampoline(1);
    x = gCurTask;
    x->unk3C++;
    TaskYieldTrampoline(2);
    x = gCurTask;
    x->unk3C++;
    TaskYieldTrampoline(2);
    sub_08063fe0();
}

void sub_08084248(void)
{
}

void sub_0808424c(void)
{
    gCurTask->unk04 = (u32)sub_080842b8;
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        sub_0806395c(0);
        CallTableEntry(gCurTask->unk14, 1, gUnk_08741E8C);
    }
}

void sub_0808429c(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_08741E8C);
}

void sub_080842b8(void)
{
    if (sub_0806951c() == 1)
    {
        sub_080639a4(gUnk_08741F64);
        TaskSetEntry(sub_0806a344, gCurTaskIdx);
    }
    else
    {
        CallTableEntry(gCurTask->unk15, 1, gUnk_08741E90);
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_08084308(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 n;

    gCurTask->unk15 = 0;
    gCurTask->unk28 = 0;
    gCurTask->unk2C = n = sub_08064314(3);
    if (n >= 25 && n <= 127)
        gCurTask->unk2C = 24;
    else if (n >= 128 && n <= 231)
        gCurTask->unk2C = 232;
    else if (n >= 281 && n <= 383)
        gCurTask->unk2C = 280;
    else if (n >= 384 && n <= 487)
        gCurTask->unk2C = 488;
    sub_0806421c((s16)gCurTask->unk2C, 768);
    u = gCurTask;
    u->unk54 = gUnk_030023B4;
    u->unk58 = gUnk_030023D4;
    while (1)
    {
        TaskSetFrame(10);
        TaskYieldTrampoline(2);
        TaskSetFrame(6);
        TaskYieldTrampoline(1);
        v = gCurTask;
        v->unk3C++;
        TaskYieldTrampoline(1);
        TaskSetFrame(14);
        TaskYieldTrampoline(2);
        TaskSetFrame(11);
        TaskYieldTrampoline(2);
        TaskSetFrame(13);
        TaskYieldTrampoline(2);
        v = gCurTask;
        v->unk3C--;
        TaskYieldTrampoline(1);
        TaskSetFrame(15);
        TaskYieldTrampoline(2);
    }
}

void sub_080843f8(void)
{
}

void sub_080843fc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 3;
    gCurTask->unk38 = gUnk_08752AB4;
    sub_08064a60();
    TaskSetMotionXFacing(0x10000, 0);
    u = gCurTask;
    u->unk58 = gUnk_08741E94[u->unk18];
    u->unk60 = gUnk_08741EA4[u->unk18];
    TaskSetFrame(18);
    TaskYieldTrampoline(4);
    v = gCurTask;
    v->unk3C++;
    TaskYieldTrampoline(4);
    v = gCurTask;
    v->unk3C++;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void sub_08084484(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    u = gCurTask;
    u->unk38 = gUnk_08752150;
    CallTableEntry(u->unk73, 2, gUnk_08741F70);
}
