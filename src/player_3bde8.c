#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_3bde8.c (0x0803BDE8-0x0803CD5F, issue #91).
 *
 * Player action body, part 10: action 19 and three helpers M09 calls.
 * sub_0803bde8 (action 19, mode 23; its per-frame handler 18 is
 * sub_0803c990) hands something over to the partner player in
 * Task.unk18: the two face each other, play the rows of gUnk_0873DA62 by
 * ability, and PlayerState.unk3A says what passes - 1 refills the
 * partner's health gUnk_02005588[] up to gUnk_02005580 step by step
 * through sub_080b4204 (src/hud_b2fe8.c), 2 gives one or two steps, 3
 * copies PlayerState.unk17/unk18.  sub_0803c9b4 (from M09's
 * sub_08033414, the twin of M04's sub_080109c8) steps and draws the
 * three spark records gUnk_02007E90[player][]; sub_0803cbd8 (M09's
 * sub_0803332c) steps the knock-back script
 * gUnk_0873A994[PlayerState.filler2A][PlayerState.unk28] into the 8.8
 * offsets PlayerState.unk24/unk26; sub_0803ccd8 (M09's sub_08034f8c)
 * applies step n of the 8.8 motion table gUnk_0873AEBC. */

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

/* one step of a player's knock-back script: {dx, dy, flags} with
   flags & 15 = frames to hold, & 64 = mirror dx with the facing,
   & 128 = sound; a zero flags byte ends the script */
struct Unk0873A994
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 filler5[3];
};

extern u16 gUnk_0873DA62[][2];
extern s16 gUnk_0873DACA[][2];
extern struct PlayerState gUnk_03002170[];
extern s16 gUnk_02005588[];             /* health per player (M02's HUD) */
extern s16 gUnk_02005580;
extern u16 gUnk_03002360;
extern u8 gUnk_03002464;
extern struct M04Spark gUnk_02007E90[][3];
extern s16 gUnk_030023E4;
extern s16 gUnk_03002348;
extern s16 gUnk_0873A924[][16];
extern u32 gUnk_0873A964[];
extern struct Unk0873A994 *gUnk_0873A994[];
extern u16 gUnk_0873AEBC[][2];

void TaskYieldTrampoline(s32 frames);
s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
u32 sub_08002ee8(u32 range);
s32 sub_080031b8(s32 id);
void sub_080055b0(u8 val, s32 idx);
void sub_08006138(void);
void sub_0800622c(s32 a, s32 b, s32 c);
void sub_08006338(s32 a);
u32 sub_0800641c(s16 a, s16 b);        /* u8 in early_5d9c.c; u32 as in player_109c8.c (u8 costs 78 bytes) */
void sub_0803e050(s32 a0);
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_0803e3e4(s32 a0);
void sub_08040710(void);
s32 sub_080b4204(u32 a);

/* Action 19 enter (mode 23, per-frame handler 18), aimed at the player
   Task.unk18 names.  With itself as the target it only plays the ability's
   animation (gUnk_0873DA62[ability][1], or gUnk_0873DACA[k][1] when
   Task.unk7B bit 0 is set) and ends; otherwise it turns to face the target
   task gUnk_03002790[unk18], plays gUnk_0873DA62[ability][0] (or
   gUnk_0873DACA[k][0]) and, by PlayerState.unk3A, raises the target's
   health gUnk_02005588[] through sub_080b4204 while it is below the maximum
   gUnk_02005580 (1: until full, 2: at most 1 or 2 steps by gUnk_03002464)
   or copies its own unk17/unk18 to the target (3); then it restores both
   tasks' Task.unk42/unk43 (saved on the stack), clears PlayerState.unk42
   bit 8 on both players and sets the target's bit in PlayerState.unk3B. */
void sub_0803bde8(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *q;
    s16 n;
    u16 a43;
    u16 b43;
    u8 a42;
    u8 b42;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 23;
    gUnk_03002490->unk15 = 18;
    if (gUnk_03002490->unk88->unk06 == 2)
        gUnk_03002490->unk88->unk06 = 0;
    sub_0803e34c(292, gUnk_03002490->unk88->unk00);
    if (gUnk_03002490->unk18 == gUnk_03002490->unk88->unk00)
    {
        gUnk_03002490->unk42 = 5;
        sub_080055b0(14, gCurTaskIdx);
        t = gUnk_03002490;
        t->unk88->unk42 |= 0x100;
        t->unk88->unk42 &= 0xFFEF;
        if (!(t->unk7B & 1))
        {
            t->unk46 = gUnk_0873DA62[t->unk88->unk0D][1];
            switch (t->unk88->unk0D)
            {
            case 0:
                if (gUnk_03002490->unk88->unk06 == 1)
                    gUnk_03002490->unk46 = 324;
            default:
                sub_08006338(gUnk_03002490->unk46);
            case 24:
                sub_08006138();
            case 1:
            case 2:
            case 5:
            case 15:
            case 19:
                while (1)
                {
                    sub_08006338(gUnk_03002490->unk46);
                    TaskYieldTrampoline(2);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                }
            }
        }
        else
        {
            switch (gUnk_03002490->unk88->unk0D)
            {
            default:
                if (gUnk_03002490->unk88->unk06 == 1)
                    sub_08006338(326);
                else
                    sub_08006338(gUnk_0873DACA[0][1]);
                break;
            case 4:
                sub_08006338(gUnk_0873DACA[1][1]);
                break;
            case 9:
                sub_08006338(gUnk_0873DACA[2][1]);
                break;
            case 10:
                sub_08006338(gUnk_0873DACA[3][1]);
                break;
            case 24:
                break;
            }
            sub_08006138();
        }
    }
    q = &gUnk_03002170[gUnk_03002490->unk18];
    u = &gUnk_03002790[gUnk_03002490->unk18];
    sub_0803e3e4(15);
    sub_080055b0(12, gCurTaskIdx);
    sub_080055b0(0, gUnk_03002490->unk18);
    a43 = gUnk_03002490->unk43;
    a42 = gUnk_03002490->unk42;
    sub_0803e050(3);
    t = gUnk_03002490;
    t->unk88->unk42 |= 0x100;
    t->unk88->unk42 &= 0xFFEF;
    if (u->unk48 - t->unk48 > 0)
        t->unk43 = 1;
    else
        t->unk43 = -1;
    gUnk_03002490->unk42 = 4;
    b43 = u->unk43;
    b42 = u->unk42;
    u->unk54 = u->unk5C = u->unk64 = 0;
    u->unk58 = u->unk60 = u->unk68 = 0;
    if ((u->unk43 = -gUnk_03002490->unk43) == 1)
        u->unk3E &= 0x7FFF;
    else
        u->unk3E |= 0x8000;
    t = gUnk_03002490;
    if (!(t->unk7B & 1))
    {
        t->unk46 = gUnk_0873DA62[t->unk88->unk0D][0];
        switch (t->unk88->unk0D)
        {
        default:
        case 0:
            if (gUnk_03002490->unk88->unk06 == 1)
            {
                sub_08006338(325);
                TaskYieldTrampoline(2);
            }
            else
            {
                sub_08006338(gUnk_03002490->unk46);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
            }
            switch (gUnk_03002490->unk88->unk3A)
            {
            case 1:
                if (gUnk_02005588[gUnk_03002490->unk18] < gUnk_02005580)
                {
                    do
                    {
                        if (gUnk_03002360 == gUnk_03002490->unk88->unk00 || gUnk_03002360 == gUnk_03002490->unk18)
                            sub_080031b8(221);
                        gUnk_03002490->unk28 = sub_080b4204(gUnk_03002490->unk18);
                        TaskYieldTrampoline(8);
                    } while (gUnk_03002490->unk28 == 0);
                }
                TaskYieldTrampoline(16);
                break;
            case 2:
                if (gUnk_02005588[gUnk_03002490->unk18] < gUnk_02005580)
                {
                    if (gUnk_03002464 == 0)
                        n = 2;
                    else
                        n = 1;
                    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < n; gUnk_03002490->unk6C++)
                    {
                        if (gUnk_03002360 == gUnk_03002490->unk88->unk00 || gUnk_03002360 == gUnk_03002490->unk18)
                            sub_080031b8(221);
                        gUnk_03002490->unk28 = sub_080b4204(gUnk_03002490->unk18);
                        TaskYieldTrampoline(8);
                        if (gUnk_03002490->unk28 != 0)
                            break;
                    }
                }
                TaskYieldTrampoline(16);
                break;
            case 3:
                if ((s8)q->unk22 != 0)
                    q->unk1E = q->unk20 = 0;
                q->unk17 = gUnk_03002490->unk88->unk17;
                q->unk18 = gUnk_03002490->unk88->unk18;
                TaskYieldTrampoline(32);
                break;
            }
            if (gUnk_03002490->unk88->unk06 == 1)
            {
                sub_08006338(319);
                TaskYieldTrampoline(2);
            }
            else
            {
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
            }
            break;
        case 1:
        case 2:
        case 5:
        case 19:
        case 22:
        case 23:
            sub_08006338(gUnk_03002490->unk46);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            switch (gUnk_03002490->unk88->unk3A)
            {
            case 1:
                if (gUnk_02005588[gUnk_03002490->unk18] < gUnk_02005580)
                {
                    do
                    {
                        if (gUnk_03002360 == gUnk_03002490->unk88->unk00 || gUnk_03002360 == gUnk_03002490->unk18)
                            sub_080031b8(221);
                        gUnk_03002490->unk28 = sub_080b4204(gUnk_03002490->unk18);
                        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 1; gUnk_03002490->unk6C++)
                        {
                            sub_08006338((s16)(gUnk_03002490->unk46 + 3));
                            TaskYieldTrampoline(2);
                            sub_08006338((s16)(gUnk_03002490->unk46 + 15));
                            TaskYieldTrampoline(2);
                        }
                    } while (gUnk_03002490->unk28 == 0);
                }
                for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 3; gUnk_03002490->unk6C++)
                {
                    sub_08006338((s16)(gUnk_03002490->unk46 + 3));
                    TaskYieldTrampoline(2);
                    sub_08006338((s16)(gUnk_03002490->unk46 + 15));
                    TaskYieldTrampoline(2);
                }
                break;
            case 2:
                if (gUnk_02005588[gUnk_03002490->unk18] < gUnk_02005580)
                {
                    if (gUnk_03002464 == 0)
                        n = 2;
                    else
                        n = 1;
                    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < n; gUnk_03002490->unk6C++)
                    {
                        if (gUnk_03002360 == gUnk_03002490->unk88->unk00 || gUnk_03002360 == gUnk_03002490->unk18)
                            sub_080031b8(221);
                        gUnk_03002490->unk28 = sub_080b4204(gUnk_03002490->unk18);
                        for (gUnk_03002490->unk6E = 0; gUnk_03002490->unk6E <= 1; gUnk_03002490->unk6E++)
                        {
                            sub_08006338((s16)(gUnk_03002490->unk46 + 3));
                            TaskYieldTrampoline(2);
                            sub_08006338((s16)(gUnk_03002490->unk46 + 15));
                            TaskYieldTrampoline(2);
                        }
                        if (gUnk_03002490->unk28 != 0)
                            break;
                    }
                }
                for (gUnk_03002490->unk6E = 0; gUnk_03002490->unk6E <= 3; gUnk_03002490->unk6E++)
                {
                    sub_08006338((s16)(gUnk_03002490->unk46 + 3));
                    TaskYieldTrampoline(2);
                    sub_08006338((s16)(gUnk_03002490->unk46 + 15));
                    TaskYieldTrampoline(2);
                }
                break;
            case 3:
                if ((s8)q->unk22 != 0)
                    q->unk1E = q->unk20 = 0;
                q->unk17 = gUnk_03002490->unk88->unk17;
                q->unk18 = gUnk_03002490->unk88->unk18;
                for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 7; gUnk_03002490->unk6C++)
                {
                    sub_08006338((s16)(gUnk_03002490->unk46 + 3));
                    TaskYieldTrampoline(2);
                    sub_08006338((s16)(gUnk_03002490->unk46 + 15));
                    TaskYieldTrampoline(2);
                }
                break;
            }
            sub_08006338((s16)(gUnk_03002490->unk46 + 3));
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            break;
        }
    }
    else
    {
        switch (t->unk88->unk0D)
        {
        default:
            gUnk_03002490->unk46 = gUnk_0873DACA[0][0];
            break;
        case 4:
            t->unk46 = gUnk_0873DACA[1][0];
            break;
        case 9:
            t->unk46 = gUnk_0873DACA[2][0];
            break;
        case 10:
            t->unk46 = gUnk_0873DACA[3][0];
            break;
        case 24:
            t->unk46 = gUnk_0873DACA[4][0];
            break;
        }
        sub_08006338(gUnk_03002490->unk46);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        switch (gUnk_03002490->unk88->unk3A)
        {
        case 1:
            if (gUnk_02005588[gUnk_03002490->unk18] < gUnk_02005580)
            {
                do
                {
                    if (gUnk_03002360 == gUnk_03002490->unk88->unk00 || gUnk_03002360 == gUnk_03002490->unk18)
                        sub_080031b8(221);
                    gUnk_03002490->unk28 = sub_080b4204(gUnk_03002490->unk18);
                    TaskYieldTrampoline(8);
                } while (gUnk_03002490->unk28 == 0);
            }
            TaskYieldTrampoline(16);
            break;
        case 2:
            if (gUnk_02005588[gUnk_03002490->unk18] < gUnk_02005580)
            {
                if (gUnk_03002464 == 0)
                    n = 2;
                else
                    n = 1;
                for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < n; gUnk_03002490->unk6C++)
                {
                    if (gUnk_03002360 == gUnk_03002490->unk88->unk00 || gUnk_03002360 == gUnk_03002490->unk18)
                        sub_080031b8(221);
                    gUnk_03002490->unk28 = sub_080b4204(gUnk_03002490->unk18);
                    TaskYieldTrampoline(8);
                    if (gUnk_03002490->unk28 != 0)
                        break;
                }
            }
            TaskYieldTrampoline(16);
            break;
        case 3:
            if ((s8)q->unk22 != 0)
                q->unk1E = q->unk20 = 0;
            q->unk17 = gUnk_03002490->unk88->unk17;
            q->unk18 = gUnk_03002490->unk88->unk18;
            TaskYieldTrampoline(32);
            break;
        }
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
    }
    gUnk_03002490->unk43 = a43;
    gUnk_03002490->unk42 = a42;
    gUnk_03002490->unk88->unk42 &= 0xFEFF;
    u->unk43 = b43;
    u->unk42 = b42;
    gUnk_03002170[gUnk_03002490->unk18].unk42 &= 0xFEFF;
    u->unk12--;
    sub_0803e3e4(0);
    sub_080055b0(0, gCurTaskIdx);
    gUnk_03002490->unk88->unk3B |= 1 << gUnk_03002490->unk18;
    sub_08006138();
}

void sub_0803c990(void)
{
    struct PlayerState *p = gUnk_03002490->unk88;

    if (p->unk0D != 24)
        sub_08040710();
    else
        p->unk01 = 55;
}

/* M09's sub_08033414: step and draw the player's three sparks (twin of M04's
   sub_080109c8, src/player_109c8.c).  a != 0 freezes the frame counter and
   draws frame `a` with no palette offset; Task.unk13 & 1 skips the motion. */
void sub_0803c9b4(s32 a)
{
    struct M04Spark *p;
    s32 i;
    s32 n;
    s32 t;
    s32 u;
    s32 d;
    s32 k;
    s32 x;
    s32 y;

    for (i = 0; i <= 2; i++)
    {
        p = &gUnk_02007E90[gUnk_03002490->unk88->unk00][i];
        if (gUnk_03002490->unk13 & 1)
        {
            k = 0;
            if (a == 0)
                k = gUnk_03002490->unk40 + 0x180C;
            d = p->unk0D;
            if (a == 0 && d == 0)
                d = (gUnk_03002490->unk43 == 1) ? 2 : 3;
        }
        else
        {
            if (p->unk00 == 0)
            {
                p->unk08 = 0;
                n = sub_08002ee8(16);
                if (gUnk_03002490->unk43 == 1)
                    p->unk00 = gUnk_0873A924[0][n] << 16;
                else
                    p->unk00 = -(gUnk_0873A924[0][n] << 16);
                p->unk04 = gUnk_0873A924[1][n] << 16;
            }
            if (abs(p->unk00) <= 0xF0000)
            {
                p->unk00 = 0;
                p->unk0C = 1;
                p->unk0D = 0;
                continue;
            }
            if (p->unk00 > 0)
                p->unk08 -= 0x6000;
            else
                p->unk08 += 0x6000;
            /* The two volatile reads are the twin's placeholders: the ROM
               re-reads unk00 here and unk04 below, and a plain read is
               folded into the earlier one by gcse's PRE. */
            p->unk00 = *(volatile s32 *)&p->unk00 + p->unk08;
            /* the shift count reuses n: its own local takes r1 (sh/u swap) */
            n = (abs(p->unk00) >> 20) + 1;
            u = *(volatile s32 *)&p->unk04;
            t = (abs(u) & 0xFFFF0000) >> n;
            if (p->unk04 > 0)
                t = -t;
            p->unk04 += t;
            d = a;
            k = 0;
            if (a == 0)
            {
                if (--p->unk0C == 0)
                {
                    if (p->unk0D == 0)
                    {
                        if (gUnk_03002490->unk43 == 1)
                            p->unk0D = 2;
                        else
                            p->unk0D = 3;
                    }
                    else if (p->unk0D <= 9)
                        p->unk0D += 2;
                    p->unk0C = 1;
                }
                d = p->unk0D;
                k = gUnk_03002490->unk40 + 0x180C;
            }
        }
        x = gUnk_03002490->unk48 + ((s16 *)&p->unk00)[1];
        y = gUnk_03002490->unk4A + ((s16 *)&p->unk04)[1] + 4;
        if (sub_0800641c(x, y))
        {
            x -= gUnk_03002348;
            y -= gUnk_030023E4;
            sub_08001a94(gUnk_03002490->unk42, gUnk_0873A964[d], 0, k, x, y);
        }
    }
}

void sub_0803cbd8(void)
{
    struct Unk0873A994 *e;
    if ((s8)--gUnk_03002490->unk88->unk29 > 0)
    {
        gUnk_03002490->unk88->unk29--;
        return;
    }
    e = gUnk_0873A994[(s8)gUnk_03002490->unk88->filler2A];
    e += (s8)gUnk_03002490->unk88->unk28;
    if (e->unk4 != 0)
    {
        if (e->unk4 & 128)
            sub_080055b0(3, gCurTaskIdx);
        if (e->unk4 & 64)
        {
            if (gUnk_03002490->unk43 == 1)
                gUnk_03002490->unk88->unk24 = e->unk0;
            else
                gUnk_03002490->unk88->unk24 = -e->unk0;
        }
        else
        {
            gUnk_03002490->unk88->unk24 = e->unk0;
        }
        gUnk_03002490->unk88->unk26 = e->unk2;
        gUnk_03002490->unk88->unk29 = e->unk4 & 15;
        gUnk_03002490->unk88->unk28++;
    }
    else
    {
        gUnk_03002490->unk88->unk24 = gUnk_03002490->unk88->unk26 = 0;
        gUnk_03002490->unk88->unk40 &= 0xFFFE;
        sub_080055b0(0, gCurTaskIdx);
    }
}

void sub_0803ccd8(s32 a)
{
    u16 *e = gUnk_0873AEBC[a];
    s32 x = e[1] << 8;
    s32 y;
    struct Task *t;

    if (e[1] & 0x8000)
        x |= 0xFF000000;
    y = e[1] << 8;
    if (e[1] & 0x8000)
        y |= 0xFF000000;
    sub_0800622c(x, 0, y);
    t = gUnk_03002490;
    if (t->unk43 == 1)
    {
        s32 v = e[0] << 8;
        if (e[0] & 0x8000)
            v |= 0xFF000000;
        t->unk2C = v;
    }
    else
    {
        t->unk2C = -(e[0] & 0x8000 ? (e[0] << 8) | 0xFF000000 : e[0] << 8);
    }
}
