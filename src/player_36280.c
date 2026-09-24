#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_36280.c (0x08036280-0x08036C93, issue #91).
 *
 * Player action bodies, part 4: per-frame handler 9 and actions 10 and
 * 11.  sub_08036280 (handler 9) is the other half of M09's mode-14
 * coroutine sub_080359f8: a seven-state switch over Task.unk73 that
 * re-binds the coroutine with the next state (1 on a held A or up, 4 on
 * a newly-pressed B, 2/3/5 from the ground flags Task.unk7A/unk7B).
 * sub_080366c4 (action 10, mode 6) installs the scripts
 * gUnk_0873BD28/gUnk_0873CB24 in PlayerState.unk64/unk68 and plays
 * gUnk_0873D4BC[ability][column]; its handler sub_08036888 requests
 * action 11 on a newly-pressed A or B and 7 on the collision flag
 * gUnk_03005550.unk5.  sub_080369b0 (action 11, mode 7) installs the
 * attack hit-box set gUnk_0873CC84 in PlayerState.unk6C; its handler
 * sub_08036b9c registers the box gUnk_0873BE9C with M09's collision
 * registry sub_0801a828 while the player moves faster than 0xE000 and
 * drops to state 1 below 0x8000. */

/* gUnk_03005550: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh). */
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

extern u16 gUnk_03002458[];             /* latched state mask per player (M11) */
extern u16 gUnk_030023C0[];             /* newly-pressed mask per player */
extern struct Unk03005550 gUnk_03005550;
extern u32 gUnk_0873BD28[];
extern u32 gUnk_0873CB24[];
extern u16 gUnk_0873D4BC[][5];
extern u8 gUnk_03001F30;
extern u32 gUnk_0873CC84[];
extern s16 gUnk_0873D5CA[][2];
extern u8 gUnk_0873BE9C[];

void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_08006338(s32 a);
u32 sub_0801a828(u8 idx, s16 x, s16 y, u8 *p);
void sub_080359f8(void);
void sub_0803e050(s32 a0);
void sub_0803e374(s32 a0, u16 a1);
s32 sub_0803e448(void);
s32 sub_0803e4ec(s32 a0);
void sub_0803f870(void);
void sub_0803f8e8(void);
void sub_0803f9c0(void);
s32 sub_0803f9e8(void);
s32 sub_0803fa44(void);
s32 sub_0803fce4(s32 a);
s32 sub_0803fd20(s32 a0);
s32 sub_0803fe68(void);
s32 sub_08040298(void);
s32 sub_08040340(void);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
void sub_08041e8c(void);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_08036280(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    if (gUnk_03002490->unk73 != 6 && sub_08040340() != 0)
    {
        if (gUnk_03002490->unk73 == 5)
            gUnk_03002490->unk73 = 2;
        return;
    }
    sub_0803f870();
    switch (gUnk_03002490->unk73)
    {
    case 0:
        if (sub_08040298() != 0)
            break;
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 65)
        {
            sub_080413a4(8);
        }
        else if (gUnk_03002490->unk7A & 1)
        {
            sub_0803e050(2);
        }
        else if (gUnk_03002490->unk7B & 1)
        {
            gUnk_03002490->unk73 = 4;
            sub_08006148(sub_080359f8, gCurTaskIdx);
            break;
        }
        else
        {
            sub_080413a4(7);
        }
        gUnk_03002490->unk7A = 0;
        sub_08040b40(6, 72);
        sub_0803f9c0();
        break;
    case 1:
        if (sub_08040298() != 0)
            break;
        gUnk_03002490->unk7A = 0;
        sub_080413a4(8);
        sub_08040b40(6, 72);
        sub_0803f9c0();
        if (gUnk_03002490->unk7A & 1)
            sub_0803e050(2);
        if (gUnk_030023C0[gUnk_03002490->unk88->unk00] & 2)
        {
            gUnk_03002490->unk73 = 4;
            sub_08006148(sub_080359f8, gCurTaskIdx);
            break;
        }
        break;
    case 2:
        if (sub_08040298() != 0)
            break;
        if (!(gUnk_03002490->unk7A & 1))
            sub_080413a4(7);
        else
            sub_0803e050(2);
        sub_08040b40(6, 72);
        sub_0803f9c0();
        t = gUnk_03002490;
        if (t->unk7A & 1)
        {
            t->unk73 = 3;
            sub_08006148(sub_080359f8, gCurTaskIdx);
            sub_0803e050(2);
            break;
        }
        if (gUnk_03002458[t->unk88->unk00] & 65)
        {
            t->unk73 = 1;
            sub_08006148(sub_080359f8, gCurTaskIdx);
            break;
        }
        if (gUnk_030023C0[t->unk88->unk00] & 2)
        {
            t->unk73 = 4;
            sub_08006148(sub_080359f8, gCurTaskIdx);
            break;
        }
        if (!(t->unk7B & 1))
            break;
        sub_0803e050(2);
        gUnk_03002490->unk28 = -1;
        gUnk_03002490->unk73 = 5;
        sub_08006148(sub_080359f8, gCurTaskIdx);
        break;
    case 3:
        if (sub_08040298() != 0)
            break;
        if (!(gUnk_03002490->unk7A & 1))
            sub_080413a4(7);
        else
            sub_0803e050(2);
        sub_08040b40(6, 72);
        sub_0803f9c0();
        u = gUnk_03002490;
        if (!(u->unk7A & 1))
        {
            u->unk73 = 2;
            sub_08006148(sub_080359f8, gCurTaskIdx);
            break;
        }
        if (gUnk_03002458[u->unk88->unk00] & 65)
        {
            u->unk73 = 1;
            sub_08006148(sub_080359f8, gCurTaskIdx);
            break;
        }
        if (gUnk_030023C0[u->unk88->unk00] & 2)
        {
            u->unk73 = 4;
            sub_08006148(sub_080359f8, gCurTaskIdx);
            break;
        }
        break;
    case 4:
        v = gUnk_03002490;
        if (!(v->unk7A & 1))
        {
            if (!(v->unk7B & 1))
                sub_080413a4(2);
            else
                sub_080413a4(13);
        }
        sub_08040b40(7, 72);
        w = gUnk_03002490;
        if (w->unk58 < 0)
        {
            if (gUnk_03005550.unk1 != 0)
                w->unk58 = 0;
        }
        else if (w->unk7A & 1)
        {
            if ((u32)w->unk58 > 0xC000)
                sub_0803e4ec(1);
            else
                sub_0803fa44();
        }
        sub_0803f9e8();
        break;
    case 5:
        sub_08040b40(6, 72);
        sub_0803f9e8();
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 65)
        {
            sub_0803e050(2);
            gUnk_03002490->unk73 = 1;
            sub_08006148(sub_080359f8, gCurTaskIdx);
        }
        else if (gUnk_030023C0[gUnk_03002490->unk88->unk00] & 2)
        {
            sub_0803e050(2);
            gUnk_03002490->unk73 = 4;
            sub_08006148(sub_080359f8, gCurTaskIdx);
        }
        break;
    case 6:
        sub_0804042c();
        x = gUnk_03002490;
        if (x->unk7A & 1)
        {
            if ((u32)x->unk58 > 0xC000)
                sub_0803e4ec(1);
            else
                sub_0803fa44();
        }
        break;
    }
    if (sub_0803fce4(0) != 0)
    {
        gUnk_03002490->unk88->unk06 = 0;
        gUnk_03002490->unk88->unk01 = 23;
    }
}

void sub_080366c4(void)
{
    struct Task *t;
    struct PlayerState *p;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 6;
    gUnk_03002490->unk15 = 10;
    t = gUnk_03002490;
    if (t->unk88->unk05 != 6)
    {
        t->unk88->unk64 = (u32)gUnk_0873BD28;
        t->unk88->unk68 = (u32)gUnk_0873CB24;
        t->unk2C = t->unk88->unk4B;
        sub_08040b40(0, 72);
    }
    gUnk_03002490->unk28 = 8;
    gUnk_03002490->unk88->unk33 = sub_0803fd20(gUnk_03002490->unk88->unk00);
    p = gUnk_03002490->unk88;
    p->unk35 = 0;
    p->unk34 = 0;
    gUnk_03002490->unk46 = gUnk_0873D4BC[gUnk_03002490->unk88->unk0D][sub_0803fd20(gUnk_03002490->unk88->unk00)];
    switch (gUnk_03002490->unk88->unk0D)
    {
    case 1:
    case 2:
    case 5:
    case 19:
        while (1)
        {
            sub_08006338(gUnk_03002490->unk46);
            TaskYieldTrampoline(2);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 2; gUnk_03002490->unk6C++)
            {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
            }
        }
    case 15:
        while (1)
        {
            sub_08006338(gUnk_03002490->unk46);
            TaskYieldTrampoline(6);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 2; gUnk_03002490->unk6C++)
            {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(6);
            }
        }
    case 0:
    default:
        sub_08006338(gUnk_03002490->unk46);
        sub_08006138();
    }
}

void sub_08036888(void)
{
    s32 dir = gUnk_03002490->unk43;

    sub_0803e448();
    while (sub_0803fe68() == 0 && sub_08040340() == 0)
    {
        u16 *q = gUnk_030023C0;
        struct Task *t = gUnk_03002490;

        if (q[t->unk88->unk00] & 3)
        {
            t->unk88->unk01 = 11;
            break;
        }
        if (!(gUnk_03002458[t->unk88->unk00] & 128))
        {
            sub_0804042c();
            break;
        }
        if (gUnk_03005550.unk5 != 0)
        {
            if (t->unk28 == 0)
            {
                t->unk7A = 0;
                gUnk_03002490->unk88->unk01 = 7;
                gUnk_03002490->unk84 = 0;
                gUnk_03002490->unk50 += 0x10000;
                break;
            }
            t->unk28--;
        }
        {
            struct Task *w = gUnk_03002490;

            if (w->unk88->unk4B != w->unk2C || dir != w->unk43)
            {
                if (gUnk_03001F30 == 0)
                    sub_08006148(sub_080366c4, gCurTaskIdx);
                else
                    sub_08006148(sub_08041e8c, gCurTaskIdx);
            }
        }
        break;
    }
    gUnk_03002490->unk2C = gUnk_03002490->unk88->unk4B;
    if (gUnk_03005550.unk0 != 0)
        sub_0803e050(1);
}

void sub_080369b0(void)
{
    struct Task *t;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 7;
    gUnk_03002490->unk15 = 11;
    t = gUnk_03002490;
    if (t->unk88->unk05 != 7)
    {
        t->unk28 = 0;
        t->unk73 = 0;
        gUnk_03002490->unk88->unk14 = 10;
        sub_0803e374(118, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk88->unk6C = gUnk_0873CC84;
        sub_08040b40(11, 0);
        sub_0805afac(gUnk_03002490->unk88->unk00, 8, 0);
    }
    switch (gUnk_03002490->unk73)
    {
    case 0:
        gUnk_03002490->unk46 = gUnk_0873D5CA[gUnk_03002490->unk88->unk0D][0];
        switch (gUnk_03002490->unk88->unk0D)
        {
        case 0:
        default:
            sub_08006338(gUnk_03002490->unk46);
            sub_08006138();
        case 1:
        case 2:
        case 5:
            while (1)
            {
                sub_08006338(gUnk_03002490->unk46);
                TaskYieldTrampoline(2);
                for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 2; gUnk_03002490->unk6C++)
                {
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                }
            }
        case 4:
        case 15:
        case 16:
        case 17:
        case 19:
        case 22:
        case 23:
            while (1)
            {
                sub_08006338(gUnk_03002490->unk46);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
            }
        }
    case 1:
        sub_08006338(gUnk_0873D5CA[gUnk_03002490->unk88->unk0D][1]);
        break;
    }
    gUnk_03002490->unk88->unk6C = 0;
    gUnk_03002490->unk28++;
    sub_08006138();
}

void sub_08036b9c(void)
{
    struct Task *t;
    struct PlayerState *p;

    while (1)
    {
        if (sub_0803fe68() != 0)
        {
            if (gUnk_03002490->unk5C == 0)
                sub_08040b40(5, 72);
            break;
        }
        if (sub_08040340() != 0)
            break;
        t = gUnk_03002490;
        if (t->unk28 != 0)
        {
            t->unk88->unk01 = 1;
        }
        else if (gUnk_03005550.unk0 != 0)
        {
            sub_0803f8e8();
            sub_0803e050(1);
            gUnk_03002490->unk88->unk01 = 1;
        }
        else
        {
            if (abs(t->unk54) <= 0x7FFF)
            {
                t->unk73 = 1;
                sub_08006148(sub_080369b0, gCurTaskIdx);
            }
            if (abs(gUnk_03002490->unk54) > 0xE000)
                sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A, gUnk_0873BE9C);
        }
        break;
    }
    p = gUnk_03002490->unk88;
    if ((s16)p->unk14 == 0)
    {
        sub_08040b40(5, 72);
        gUnk_03002490->unk88->unk14--;
    }
    else if ((s16)p->unk14 > 0)
    {
        p->unk14--;
    }
}
