#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* plobj_52f6c.c (0x08052F6C-0x08053AF3, issue #90).
 *
 * Task type #6 (the objects the player's actions spawn), variants 10-12,
 * and the two spawners.  sub_08052f6c (variant 10) takes its owner's OAM
 * flags (Task.unk8C is the spawning task) and runs one of six sub-states
 * Task.unk18 & 15: states 0-2 trace an eight-step path (the 8.8 velocity
 * rows gUnk_0873B8C6[k] and the five-frame animation rows gUnk_0873B88A
 * that gUnk_0873B872[k] picks, callback sub_08052b08) and fall into state
 * 3, which flies in one of four directions gUnk_0873B862[Task.unk28] and
 * emits effect 33 every other frame; states 4 and 5 are a stationary
 * object with two callbacks, sub_08053380 (the hit test, which ends the
 * object through the shared exit sub_08050814 once PlayerState.unk0D is
 * 0) and sub_080534d0 (a six-step trail drawn with sub_08001a94).
 * Variants 11 and 12 (sub_080535b0, sub_080537dc) are two projectiles
 * that move 4 pixels a frame in the facing direction (animation table
 * gUnk_0874C4E4, M14's shared callback sub_08050f80); their per-frame
 * callbacks sub_080536dc and sub_080538cc register the collider and hand
 * over to sub_08050814 on contact (variant 11 bounces back once on
 * collision result 6).  sub_08053940 and sub_08053a44 are the spawners
 * the player's actions call (M09-M14): they start a task of type 6 in
 * the slot band of player 0-3 (4-6, 7-9, 10-12, 13-15; sub_08053940
 * then retries a wider band and, last, a type-7 task in slots 32-62) and
 * copy the spawner's position, facing Task.unk43, Task.unk7B and
 * PlayerState into it, with Task.unk18 = variant << 24 | arg.  They
 * return the task index or -1; the landed callers outside M14 declare
 * them `void (s32, s32, s32)`. */

/* gUnk_03005550: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh).  A 16-bit test of
   unk0/unk1 together is `*(u16 *)&gUnk_03005550` (M12's sub_08045a50). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

extern u32 gUnk_08751BF4[];
extern u16 gUnk_0873B862[][2];
extern u8 gUnk_0873B872[][8];
extern u8 gUnk_0873B88A[][5];
extern u16 gUnk_0873B8C6[][2][8];
extern s8 gUnk_0873CB5C[];
extern s8 gUnk_0873CB6C[];
extern struct Unk03005550 gUnk_03005550;
extern u32 gUnk_0873CC3C[];
extern u32 gUnk_0873CC44[];
extern u32 gUnk_0873BE4C[];
extern u32 gUnk_0873BE60[];
extern s16 gUnk_03002348;               /* scalar, read with ldrsh (33 landed files) */
extern s16 gUnk_030023E4;               /* scalar, read with ldrsh (32 landed files) */
extern u32 gUnk_087520A8[];
extern u32 gUnk_0874C4E4[];
extern s8 gUnk_0873CB4C[];
extern u32 gUnk_0873BE74[];
extern u32 gUnk_0873BE88[];

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
s32 sub_080031b8(s32 id);
s32 sub_08005904(u32 type, s32 start, s32 end);
void sub_080059d8(void);
void sub_080059fc(void);
void sub_08005d18(void);
void sub_08005d9c(void);
void sub_08005f30(void);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_080061c0(s32 a, s32 b);
void sub_080062c4(void);
u32 sub_0800641c(s16 x, s16 y);
u32 sub_08006464(s16 x, s16 y);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_0801c230(const s8 *p);
void sub_0802205c(s8 *box);
void sub_0802233c(s8 *off);
s32 sub_08030848(struct HitBoxSet *p, s32 e);   /* M14's callers test r0 unnarrowed (good/sub_08050c48.c); landed M09/M12/M13 files spell it u16 */
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_08050814(void);
void sub_08050f80(void);
void sub_080520dc(void);
s32 sub_08052b08(void);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);
void sub_08053380(void);
void sub_080534d0(void);
void sub_080536dc(void);
void sub_080538cc(void);

void sub_08052f6c(void)
{
    u16 *xs;
    u16 *ys;
    u8 *steps;

    {
        struct Task *t = gUnk_03002490;
        t->unk40 = ((struct Task *)t->unk8C)->unk40 | 0xF008;
        switch (t->unk18 & 15)
        {
        case 0:
            sub_0803e34c(207, gUnk_03002490->unk44);
            xs = gUnk_0873B8C6[0][0];
            ys = gUnk_0873B8C6[0][1];
            steps = gUnk_0873B872[0];
            goto common;
        case 1:
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            xs = gUnk_0873B8C6[1][0];
            ys = gUnk_0873B8C6[1][1];
            steps = gUnk_0873B872[1];
            goto common;
        case 2:
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(2);
            xs = gUnk_0873B8C6[2][0];
            ys = gUnk_0873B8C6[2][1];
            steps = gUnk_0873B872[2];
        common:
            {
                struct Task *u = gUnk_03002490;
                u->unk00 = (u32)sub_080059fc;
                u->unk0C = (u32)sub_08005d9c;
                u->unk04 = (u32)sub_08052b08;
                u->unk42 = 5;
                u = gUnk_03002490;
                u->unk38 = gUnk_08751BF4;
                if (u->unk43 == 1)
                    u->unk28 = 10;
                else
                    u->unk28 = -10;
            }
            {
                struct Task *u = gUnk_03002490;
                u->unk2C = 4;
                u->unk80 = 16;
            }
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 7; gUnk_03002490->unk6C++)
            {
                u8 *e;
                s32 v;
                {
                    struct Task *u = gUnk_03002490;
                    u->unk4C = u->unk28 << 16;
                    u->unk50 = u->unk2C << 16;
                    v = xs[(s16)u->unk6C] << 8;
                    if (xs[(s16)u->unk6C] & 0x8000)
                        v |= 0xFF000000;
                }
                sub_080061c0(v, 0x5A5A5A5A);
                {
                    struct Task *u = gUnk_03002490;
                    v = ys[(s16)u->unk6C] << 8;
                    if (ys[(s16)u->unk6C] & 0x8000)
                        v |= 0xFF000000;
                    u->unk58 = v;
                    e = gUnk_0873B88A[steps[(s16)u->unk6C]];
                    u->unk6E = 0;
                }
                do
                {
                    gUnk_03002490->unk3C = e[gUnk_03002490->unk6E];
                    TaskYieldTrampoline(1);
                } while (++gUnk_03002490->unk6E <= 4);
                {
                    struct Task *u = gUnk_03002490;
                    u->unk54 = 0;
                    u->unk58 = 0;
                }
            }
            TaskDispatchTrampoline();
        case 3:
        {
            struct Task *u = gUnk_03002490;
            if (u->unk38 == NULL)
            {
                u->unk00 = (u32)sub_080059d8;
                u->unk0C = (u32)sub_08005f30;
                u->unk42 = 7;
                u = gUnk_03002490;
                u->unk38 = gUnk_08751BF4;
                u->unk50 = (u->unk4A + 4) << 16;
                if (u->unk43 == 1)
                {
                    u->unk4C = (u->unk48 + 6) << 16;
                    u->unk28 = 1;
                }
                else
                {
                    u->unk4C = (u->unk48 - 6) << 16;
                    u->unk28 = 3;
                }
                gUnk_03002490->unk2C = 3;
                sub_0802233c(gUnk_0873CB5C);
                gUnk_03002490->unk80 = 6;
            }
            else
            {
                u->unk04 = 0;
                sub_080062c4();
                gUnk_03002490->unk3C = 8;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C = 0xFFFF;
                TaskYieldTrampoline(1);
                gUnk_03002490->unk3C = 9;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C = -1;
                TaskYieldTrampoline(1);
                gUnk_03002490->unk7A = 0;
            }
        }
        {
            struct Task *u = gUnk_03002490;
            s32 k = u->unk28;
            u16 *e = gUnk_0873B862[k];
            s32 v;

            u->unk04 = (u32)sub_080520dc;
            u->unk3C = e[0];
            if (k == 1 || k == 3)
            {
                v = e[1] << 8;
                if (e[1] & 0x8000)
                    v |= 0xFF000000;
                u->unk54 = v;
            }
            else
            {
                v = e[1] << 8;
                if (e[1] & 0x8000)
                    v |= 0xFF000000;
                u->unk58 = v;
            }
        }
            while (1)
            {
                sub_0805afac(gUnk_03002490->unk88->unk00, 33, gUnk_03002490->unk28);
                TaskYieldTrampoline(2);
            }
        case 4:
        case 5:
        {
            struct Task *u = gUnk_03002490;
            u->unk00 = (u32)sub_080059d8;
            u->unk0C = (u32)sub_08005f30;
            u->unk04 = (u32)sub_08053380;
            u->unk08 = (u32)sub_080534d0;
            u->unk42 = 7;
        }
            gUnk_03002490->unk38 = gUnk_08751BF4;
            sub_0802205c(gUnk_0873CB6C);
            {
                struct Task *u = gUnk_03002490;
                if (u->unk43 == 1)
                    u->unk4C = (u->unk48 + 10) << 16;
                else
                    u->unk4C = (u->unk48 - 10) << 16;
            }
            {
                struct Task *u = gUnk_03002490;
                u->unk50 = (u->unk4A + 4) << 16;
                u->unk80 = 24;
            }
            gUnk_03002490->unk28 = 0;
            sub_080061c0(0x60000, 0x5A5A5A5A);
            {
                struct Task *u = gUnk_03002490;
                if ((u->unk18 & 15) == 4)
                    u->unk3C = 18;
                else
                    u->unk3C = 19;
            }
            break;
        }
    }
    sub_08006138();
}

void sub_08053380(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk88->unk0D == 0)
    {
        sub_08006148(sub_08050814, gCurTaskIdx);
        return;
    }
    switch (t->unk18 & 15)
    {
    case 4:
    {
        s32 hit;

        if (sub_08030848((struct HitBoxSet *)gUnk_0873CC3C, t->unk44))
            gUnk_03002490->unk7C = 1;
        else
            sub_0801c230(gUnk_0873CB6C);
        hit = 0;
        if ((gUnk_03002490->unk7A & 1) || *(u16 *)&gUnk_03005550 != 0 || gUnk_03002490->unk7C != 0)
            hit++;
        if (hit)
        {
            if (gUnk_03005550.unk1 != 0 || (gUnk_03002490->unk7A & 1) || gUnk_03005550.unk0 != 0)
                sub_0803e34c(125, gUnk_03002490->unk44);
            sub_08006148(sub_08050814, gCurTaskIdx);
            gUnk_03002490->unk24 = (s32)gUnk_0873BE4C;
        }
        sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A, gUnk_0873BE4C);
        break;
    }
    case 5:
        t->unk78 = 127;
        sub_08030848((struct HitBoxSet *)gUnk_0873CC44, t->unk44);
        sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A, gUnk_0873BE60);
        break;
    }
}

void sub_080534d0(void)
{
    s32 k = -1;

    if ((gUnk_03002490->unk18 & 15) == 4)
        k = 12;
    else if ((gUnk_03002490->unk18 & 15) == 5)
        k = 0;
    if (k != -1)
    {
        if (sub_08006464(gUnk_03002490->unk48, gUnk_03002490->unk4A)
            && sub_0800641c(gUnk_03002490->unk48, gUnk_03002490->unk4A))
        {
            struct Task *t = gUnk_03002490;
            if (t->unk43 == -1)
                k++;
            sub_08001a94(t->unk42, gUnk_087520A8[t->unk28 * 2 + k], 0, 0,
                         t->unk48 - gUnk_03002348, t->unk4A - gUnk_030023E4);
        }
        {
            struct Task *t = gUnk_03002490;
            if (++t->unk28 > 5)
                t->unk28 = 0;
        }
    }
}

void sub_080535b0(void)
{
    {
        struct Task *t = gUnk_03002490;
        t->unk00 = (u32)sub_080059d8;
        t->unk0C = (u32)sub_08005f30;
        t->unk04 = (u32)sub_080536dc;
        t->unk42 = 5;
    }
    gUnk_03002490->unk38 = gUnk_0874C4E4;
    sub_0802205c(gUnk_0873CB4C);
    {
        struct Task *t = gUnk_03002490;
        t->unk50 = (t->unk4A + 4) << 16;
        if (t->unk43 == 1)
            t->unk4C = (t->unk48 + 16) << 16;
        else
            t->unk4C = (t->unk48 - 16) << 16;
    }
    {
        struct Task *t = gUnk_03002490;
        t->unk28 = 0;
        if (t->unk43 == 1)
            t->unk2C = -0x40000;
        else
            t->unk2C = 0x40000;
    }
    gUnk_03002490->unk30 = 0;
    sub_080061c0(0x40000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk08 = (u32)sub_08050f80;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    while (1)
    {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
    }
}

void sub_080536dc(void)
{
    gUnk_03002490->unk78 = 127;
    switch (gUnk_03002490->unk7C)
    {
    case 1:
    case 2:
        sub_08006148(sub_08050814, gCurTaskIdx);
        return;
    default:
        sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A, gUnk_0873BE74);
        break;
    case 6:
        sub_080031b8(0x232);
        {
            struct Task *t = gUnk_03002490;
            t->unk08 = 0;
            if (t->unk43 == 1)
                t->unk43 = -1;
            else
                t->unk43 = 1;
        }
        sub_080061c0(0x30000, 0x5A5A5A5A);
        {
            struct Task *t = gUnk_03002490;
            t->unk58 = -0x10000;
            t->unk7C = 4;
        }
        break;
    case 4:
        break;
    }
    sub_0801c230(gUnk_0873CB4C);
    {
        struct Task *t = gUnk_03002490;
        if ((t->unk7A & 1) || *(u16 *)&gUnk_03005550 != 0)
        {
            sub_0803e34c(125, t->unk44);
            sub_08006148(sub_08050814, gCurTaskIdx);
        }
    }
}

void sub_080537dc(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08005d18;
    t->unk04 = (u32)sub_080538cc;
    t->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C4E4;
    t->unk4C = (t->unk48 + 24) << 16;
    t->unk28 = 0;
    if (t->unk43 == 1)
        t->unk2C = -0x40000;
    else
        t->unk2C = 0x40000;
    gUnk_03002490->unk30 = 0;
    sub_080061c0(0x40000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk08 = (u32)sub_08050f80;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    while (1)
    {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
    }
}

void sub_080538cc(void)
{
    struct Task *t = gUnk_03002490;

    t->unk78 = 127;
    if (t->unk7C != 0)
        sub_08006148(sub_08050814, gCurTaskIdx);
    else
        sub_0801a828(gCurTaskIdx, t->unk48 + gUnk_03002348, t->unk4A + gUnk_030023E4, gUnk_0873BE88);
}

s32 sub_08053940(s8 player, u8 variant, s32 arg)
{
    s32 prio;
    s32 idx;

    if (player == 0)
        prio = 4;
    else if (player == 1)
        prio = 7;
    else if (player == 2)
        prio = 10;
    else if (player == 3)
        prio = 13;
    else
        return -1;
    idx = sub_08005904(6, prio, prio + 2);
    if (idx == -1)
    {
        if (player == 0)
            prio = 16;
        else if (player == 1)
            prio = 20;
        else if (player == 2)
            prio = 24;
        else if (player == 3)
            prio = 28;
        idx = sub_08005904(6, prio, prio + 3);
        if (idx == -1)
            idx = sub_08005904(7, 32, 62);
    }
    if (idx != -1)
    {
        struct Task *t = &gUnk_03002790[idx];
        t->unk18 = (variant << 24) | (arg & 0xFFFFFF);
        t->unk4C = gUnk_03002490->unk4C;
        t->unk48 = gUnk_03002490->unk48;
        t->unk50 = gUnk_03002490->unk50;
        t->unk4A = gUnk_03002490->unk4A;
        t->unk43 = gUnk_03002490->unk43;
        t->unk7B = gUnk_03002490->unk7B;
        t->unk88 = gUnk_03002490->unk88;
    }
    return idx;
}

s32 sub_08053a44(s8 player, u8 variant, s32 arg)
{
    s32 prio;
    s32 idx;

    if (player == 0)
        prio = 4;
    else if (player == 1)
        prio = 7;
    else if (player == 2)
        prio = 10;
    else if (player == 3)
        prio = 13;
    else
        return -1;
    idx = sub_08005904(6, prio, prio + 2);
    if (idx != -1)
    {
        struct Task *t = &gUnk_03002790[idx];
        t->unk18 = (variant << 24) | (arg & 0xFFFFFF);
        t->unk4C = gUnk_03002490->unk4C;
        t->unk48 = gUnk_03002490->unk48;
        t->unk50 = gUnk_03002490->unk50;
        t->unk4A = gUnk_03002490->unk4A;
        t->unk43 = gUnk_03002490->unk43;
        t->unk7B = gUnk_03002490->unk7B;
        t->unk88 = gUnk_03002490->unk88;
    }
    return idx;
}
