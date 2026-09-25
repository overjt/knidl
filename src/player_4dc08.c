#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4dc08.c (0x0804DC08-0x0804E39F, issue #90).
 *
 * Player action bodies, part 24: actions 30 and 31 and per-frame handler
 * 27 (handler 28, the pair of action 31, is PR #133's src/sub_0804e3a0.c).
 * sub_0804dc08 (action 30, mode 10) is a three-state `switch (Task.unk73)`
 * whose states fall into each other: state 0 winds up (animation 0xE79,
 * camera preset sub_08040b40(11, 5)), state 1 plays sound 200 and effect 6
 * and loops animations 0xE7C/0xE85 four times, and state 2 either swings
 * (animation 0xE7D, M11's sub_080413a4 steering, effect 28 when it lands)
 * or, once PlayerState.unk07 is set, plays sound 177, sets
 * PlayerState.unk42 bit 9 and stops; every pass counts Task.unk28.  Its
 * handler sub_0804df00 re-binds state 2 when PlayerState.unk07 is set, runs
 * the hit test sub_08030898(gUnk_0873CC64) in state 1 (which spawns
 * sub_08065100's object and marks PlayerState.unk09) and, in state 2,
 * requests action 53, 8 or 1 once the swing is over.  sub_0804e0e0
 * (action 31, mode 10; the twin of M10's sub_080375e0) clears the three
 * records gUnk_02007E90[player][] (and gUnk_02007CF4[player] in link
 * play), plays sound 103 and holds animation 0xF71 with PlayerState.unk40
 * bit 2 set until PlayerState.unk07 is non-zero and equal to unk08, then
 * recovers or releases (sound 201, PlayerState.unk42 bit 9). */

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

/* gUnk_02007E90[4][3]: M04's per-player spark records (src/player_10358.c) */
struct M04Spark
{
    /*0x00*/ s32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ u8 unk0C;
    /*0x0D*/ u8 unk0D;
    /*0x0E*/ u16 unk0E;
};

extern u32 gUnk_0873CC64[];             /* hit-box set, passed as (struct HitBoxSet *) */
extern s16 gUnk_02007FA0[];
extern s16 gUnk_02004B6C[];
extern struct Unk03005550 gUnk_03005550;
extern u32 gUnk_0873BED8[];             /* collider row passed to sub_0801a828 (4th arg) */
extern s16 gUnk_0300244C;
extern u8 gUnk_02007CF4[];
extern s32 gUnk_03001F2C;               /* boot_091ac.c spelling */
extern struct M04Spark gUnk_02007E90[][3];

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_08006338(s32 a);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_080261d4(u16 a);
u16 sub_08030898(struct HitBoxSet *p, s32 e);
void sub_0803e050(s32 a0);
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_0803e374(s32 a0, u16 a1);
void sub_0803e3ac(void);
void sub_0803e4a8(void);
s32 sub_0803fa44(void);
s32 sub_0803fce4(s32 a);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);
s32 sub_08065100(s32 x, s32 y, u32 p2, u8 p3, u8 p4);   /* this caller passes x and y unnarrowed (ldrsh; adds #8) */

void sub_0804dc08(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 10;
    gUnk_03002490->unk15 = 27;
    if (gUnk_03002490->unk88->unk05 != 10)
    {
        gUnk_03002490->unk28 = 0;
        gUnk_03002490->unk73 = 0;
        gUnk_03002490->unk88->unk16 = 0;
        {
            struct PlayerState *p = gUnk_03002490->unk88;

            p->unk09 = 0;
            p->unk08 = 0;
            p->unk07 = 0;
        }
    }
    switch (gUnk_03002490->unk73)
    {
    case 0:
        sub_0803e050(2);
        sub_08040b40(11, 5);
        sub_08006338(0xE79);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        sub_0803e050(1);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk73 = 1;
    case 1:
        sub_0803e34c(200, gUnk_03002490->unk88->unk00);
        sub_0805afac(gUnk_03002490->unk88->unk00, 6, 260);
        sub_08040b40(11, 6);
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 3; gUnk_03002490->unk6C++)
        {
            sub_08006338(0xE7C);
            TaskYieldTrampoline(2);
            sub_08006338(0xE85);
            TaskYieldTrampoline(2);
        }
        gUnk_03002490->unk73 = 2;
    case 2:
        if ((s8)gUnk_03002490->unk88->unk07 == 0)
        {
            if (gUnk_03002490->unk7A & 1)
                sub_0805afac(gUnk_03002490->unk88->unk00, 4, 0);
            else if (!(gUnk_03002490->unk7B & 1))
                sub_080413a4(2);
            else
                sub_080413a4(13);
            sub_08040b40(11, 7);
            if (gUnk_03002490->unk7A & 1)
                sub_080413a4(19);
            sub_08006338(0xE7D);
            TaskYieldTrampoline(5);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            if (gUnk_03002490->unk7A & 1)
                sub_080413a4(19);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(5);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            if (gUnk_03002490->unk7A & 1)
                sub_080413a4(19);
            sub_08006338(0xE7D);
            TaskYieldTrampoline(5);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            if (gUnk_03002490->unk7A & 1)
                sub_080413a4(19);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            if (gUnk_03002490->unk7A & 1)
            {
                sub_0803e050(2);
                sub_0805afac(gUnk_03002490->unk88->unk00, 28, 5);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                sub_08040b40(11, 8);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(8);
            }
        }
        else
        {
            sub_0803e34c(177, gUnk_03002490->unk88->unk00);
            sub_0803e1b8(3, 0, gUnk_03002490->unk88->unk00);
            gUnk_03002490->unk88->unk42 |= 0x200;
            sub_0803e050(1);
            sub_08006338(0xE83);
            TaskYieldTrampoline(1);
        }
        gUnk_03002490->unk28++;
    }
    sub_08006138();
}

void sub_0804df00(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *p;

    t = gUnk_03002490;
    switch (t->unk73)
    {
    case 0:
        break;
    case 1:
        if ((s8)t->unk88->unk07 != 0)
        {
            t->unk73 = 2;
            sub_08006148(sub_0804dc08, gCurTaskIdx);
        }
        else if (t->unk88->unk09 == 0)
        {
            if (sub_08030898((struct HitBoxSet *)gUnk_0873CC64, t->unk88->unk00) != 0)
            {
                sub_08065100(gUnk_02007FA0[0] + 8, gUnk_02004B6C[0] + 8, gCurTaskIdx, 4, 3);
                gUnk_03002490->unk88->unk09 = 2;
            }
            u = gUnk_03002490;
            if (u->unk88->unk09 == 0)
            {
                if (gUnk_03005550.unk0 != 0)
                {
                    sub_080261d4(1);
                    gUnk_03002490->unk88->unk01 = 18;
                }
                else
                {
                    sub_0801a828(gCurTaskIdx, u->unk48, u->unk4A, gUnk_0873BED8);
                }
            }
        }
        break;
    case 2:
        if (t->unk28 != 0)
        {
            p = t->unk88;
            if ((s8)p->unk08 != 0)
                p->unk01 = 53;
            else if (!(t->unk7A & 1))
                p->unk01 = 8;
            else
                p->unk01 = 1;
        }
        if (gUnk_03005550.unk0 != 0)
            sub_0803e050(1);
        u = gUnk_03002490;
        if (u->unk58 != 0)
        {
            if (sub_0803fce4(0) != 0)
            {
                sub_0803e4a8();
                if ((s8)gUnk_03002490->unk88->unk07 == 0)
                    gUnk_03002490->unk88->unk01 = 23;
            }
            else if (sub_0803fa44() != 0)
            {
                struct Task *v = gUnk_03002490;

                if (!(v->unk7B & 1) && (v->unk58 & 0xFFFF0000))
                    sub_0805afac(v->unk88->unk00, 4, 0);
                sub_0803e050(2);
            }
        }
        else if (!(u->unk7A & 1))
        {
            if (!(u->unk7B & 1))
                sub_080413a4(2);
            else
                sub_080413a4(13);
        }
        break;
    }
}

void sub_0804e0e0(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 10;
    gUnk_03002490->unk15 = 28;
    t = gUnk_03002490;
    if (t->unk88->unk05 != 10)
    {
        t->unk30 = 0;
        t->unk73 = 0;
    }
    u = gUnk_03002490;
    switch (u->unk73)
    {
    case 0:
        u->unk28 = 1;
        u->unk2C = 30;
        u->unk88->unk16 = 0;
        {
            struct PlayerState *p = gUnk_03002490->unk88;

            p->unk09 = 0;
            p->unk08 = 0;
            p->unk07 = 0;
        }
        if (gUnk_0300244C != 0)
            gUnk_02007CF4[gUnk_03002490->unk88->unk00] = 0;
        gUnk_03001F2C = 0;
        do
        {
            gUnk_02007E90[gUnk_03002490->unk88->unk00][gUnk_03001F2C].unk00 = 0;
            gUnk_02007E90[gUnk_03002490->unk88->unk00][gUnk_03001F2C].unk04 = 0;
            gUnk_02007E90[gUnk_03002490->unk88->unk00][gUnk_03001F2C].unk08 = 0;
            gUnk_02007E90[gUnk_03002490->unk88->unk00][gUnk_03001F2C].unk0C = 1;
            gUnk_02007E90[gUnk_03002490->unk88->unk00][gUnk_03001F2C].unk0D = 0;
            gUnk_03001F2C++;
        } while (gUnk_03001F2C <= 2);
        gUnk_03002490->unk73 = 1;
        sub_08006338(0xF6E);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_0803e374(103, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk88->unk40 |= 4;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk73 = 1;
    case 1:
        while (1)
        {
            sub_08006338(0xF71);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 2; gUnk_03002490->unk6C++)
            {
                if ((s8)gUnk_03002490->unk88->unk07 != 0 && (s8)gUnk_03002490->unk88->unk07 == (s8)gUnk_03002490->unk88->unk08)
                    goto hit;
                TaskYieldTrampoline(1);
            }
            gUnk_03002490->unk3C++;
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 2; gUnk_03002490->unk6C++)
            {
                if ((s8)gUnk_03002490->unk88->unk07 != 0 && (s8)gUnk_03002490->unk88->unk07 == (s8)gUnk_03002490->unk88->unk08)
                    goto hit;
                TaskYieldTrampoline(1);
            }
        }
    hit:
        gUnk_03002490->unk73 = 2;
    case 2:
        gUnk_03002490->unk88->unk40 &= 0xFFFB;
        sub_0803e3ac();
        if ((s8)gUnk_03002490->unk88->unk08 == 0)
        {
            sub_08006338(0xF6E);
            TaskYieldTrampoline(2);
        }
        else
        {
            sub_0803e34c(201, gUnk_03002490->unk88->unk00);
            sub_0803e1b8(3, 0, gUnk_03002490->unk88->unk00);
            gUnk_03002490->unk88->unk42 |= 0x200;
            sub_08006338(0xF73);
            TaskYieldTrampoline(1);
        }
        gUnk_03002490->unk73 = 3;
    }
    sub_08006138();
}
