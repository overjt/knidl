#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_337f4.c (0x080337F4-0x080343BF, issue #92).
 *
 * Player mode bodies, part 1.  The six empty handlers sub_080337f4 ...
 * sub_08033808 are entries 59-61 of the action table 0x0873A74C and
 * entry 56 of the per-frame table 0x0873A844 (two of them, sub_080337f8
 * and sub_08033800, are dead exports nothing points at).  Then actions 1
 * and 2: sub_0803380c enters mode 0 (per-frame handler 1, sub_08033a2c)
 * and sub_08033d0c mode 1 (handler 2, sub_08034278).  Their animations
 * come from gUnk_0873D0F8[ability][5] (column picked by M11's
 * sub_0803fd20, row 26 when PlayerState.unk06 == 1) and gUnk_0873D2E8. */

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

extern u16 gUnk_0873D0F8[][5];
extern u16 gUnk_03002458[];             /* latched state mask per player (M11) */
extern u8 gUnk_03001F30;
extern s16 gUnk_0873D206[];
extern u16 gUnk_0873D2E8[];
extern u16 gUnk_0873D350[];
extern struct Unk03005550 gUnk_03005550;

void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_08006338(s32 a);
s32 sub_08022624(u16 x, u16 y);
s32 sub_08022788(s32 y, s32 i);
void sub_0803cd60(void);
void sub_0803e050(s32 a0);
void sub_0803e414(void);
s32 sub_0803e448(void);
void sub_0803f8e8(void);
s32 sub_0803fd20(s32 a0);
s32 sub_0803fd90(void);
s32 sub_0803fdf4(void);
s32 sub_0803fe68(void);
s32 sub_0803fec4(void);
s32 sub_0803ff7c(void);
s32 sub_0803ffe0(void);
s32 sub_080400c0(void);
s32 sub_08040298(void);
s32 sub_08040340(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_08041438(void);
s32 sub_0805afac(s32 band, s32 id, s32 payload);

void sub_080337f4(void)
{
}

void sub_080337f8(void)
{
}

void sub_080337fc(void)
{
}

void sub_08033800(void)
{
}

void sub_08033804(void)
{
}

void sub_08033808(void)
{
}

void sub_0803380c(void)
{
    struct PlayerState *p;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 0;
    gUnk_03002490->unk15 = 1;

    if (gUnk_03002490->unk88->unk05 != 0)
    {
        struct Task *t;
        struct Task *t2;

        sub_0803e050(3);
        t = gUnk_03002490;
        t->unk28 = (u16)t->unk88->unk4E;
        t->unk2C = t->unk88->unk4B;
        if (t->unk88->unk4A != 0)
            t->unk88->unk46 = t->unk88->unk4A;
        gUnk_03002490->unk88->unk3D = 0;
        t2 = gUnk_03002490;
        t2->unk88->unk40 &= 0xFFEF;
        t2->unk88->unk0F = 0;
        sub_0803cd60();
    }
    gUnk_03002490->unk88->unk33 = sub_0803fd20(gUnk_03002490->unk88->unk00);
    p = gUnk_03002490->unk88;
    p->unk35 = 0;
    p->unk34 = 0;
    if (gUnk_03002490->unk88->unk06 == 1)
        gUnk_03002490->unk46 = gUnk_0873D0F8[26][sub_0803fd20(gUnk_03002490->unk88->unk00)];
    else
        gUnk_03002490->unk46 = gUnk_0873D0F8[gUnk_03002490->unk88->unk0D][sub_0803fd20(gUnk_03002490->unk88->unk00)];
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
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
    case 15:
        while (1)
        {
            sub_08006338(gUnk_03002490->unk46);
            TaskYieldTrampoline(6);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(6);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(6);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(6);
        }
    case 0:
    default:
        sub_08006338(gUnk_03002490->unk46);
        sub_08006138();
    }
}

void sub_08033a2c(void)
{
    s32 dir = gUnk_03002490->unk43;
    s32 turn = 0;

    if (sub_0803e448() != 0)
        sub_0803e414();
    while (sub_0803fdf4() == 0 && sub_0803fe68() == 0 && sub_08040298() == 0
           && sub_0803ff7c() == 0 && sub_0803fec4() == 0 && sub_0803ffe0() == 0
           && sub_080400c0() == 0)
    {
        if (sub_08040340() != 0)
            goto end;
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48)
        {
            if (gUnk_03002490->unk88->unk46 != 0)
            {
                if (dir == gUnk_03002490->unk43)
                {
                    if (gUnk_03002490->unk43 == 1)
                    {
                        if (sub_08022624(gUnk_03002490->unk48 + 7, gUnk_03002490->unk4A) == 0)
                            turn = 1;
                    }
                    else
                    {
                        if (sub_08022624(gUnk_03002490->unk48 - 7, gUnk_03002490->unk4A) == 0)
                            turn = 1;
                    }
                    if (turn == 0)
                    {
                        struct PlayerState *q = gUnk_03002490->unk88;
                        u8 v = q->unk46;

                        if (v == 1 && (gUnk_03002458[q->unk00] & 32))
                            turn = 1;
                        else if (v == 2 && (gUnk_03002458[q->unk00] & 16))
                            turn = 1;
                    }
                }
                else
                {
                    turn = 1;
                }
            }
            else
            {
                turn = 1;
            }
        }
        if (turn != 0)
        {
            struct Task *u = gUnk_03002490;
            s32 x = u->unk28;

            if (x != -1 && dir == u->unk43 && sub_08022788(x, u->unk88->unk00) != 0)
                turn = 0;
        }
        if (turn != 0)
        {
            gUnk_03002490->unk88->unk01 = 2;
            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 16)
                gUnk_03002490->unk4C = (gUnk_03002490->unk4C & 0xFFFF0000) | 0xF000;
            else
                gUnk_03002490->unk4C = (gUnk_03002490->unk4C & 0xFFFF0000) | 0x1000;
            if (gUnk_03001F30 == 0)
            {
                if (gUnk_03002490->unk88->unk06 == 1)
                    gUnk_03002490->unk46 = gUnk_0873D0F8[26][sub_0803fd20(gUnk_03002490->unk88->unk00)];
                else
                    gUnk_03002490->unk46 = gUnk_0873D0F8[gUnk_03002490->unk88->unk0D][sub_0803fd20(gUnk_03002490->unk88->unk00)];
            }
            else
            {
                gUnk_03002490->unk46 = gUnk_0873D206[sub_0803fd20(gUnk_03002490->unk88->unk00)];
            }
            sub_08006338(gUnk_03002490->unk46);
            gUnk_03002490->unk88->unk46 = 0;
            break;
        }
        {
            struct Task *w = gUnk_03002490;

            if (w->unk88->unk4B != w->unk2C || dir != w->unk43)
            {
                if (gUnk_03001F30 == 0)
                    sub_08006148(sub_0803380c, gCurTaskIdx);
                else
                    sub_08006148(sub_08041438, gCurTaskIdx);
            }
        }
        break;
    }
end:
    gUnk_03002490->unk2C = gUnk_03002490->unk88->unk4B;
}

void sub_08033d0c(void)
{
    struct PlayerState *p;
    struct Task *t;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 1;
    gUnk_03002490->unk15 = 2;
    gUnk_03002490->unk2C = -1;
    sub_08040b40(1, 72);
    p = gUnk_03002490->unk88;
    if (p->unk05 != 1)
    {
        p->unk3D = 0;
        gUnk_03002490->unk88->unk0F = 0;
        gUnk_03002490->unk88->unk46 = 0;
        gUnk_03002490->unk28 = 0;
        sub_0803cd60();
        if (sub_0803fd20(gUnk_03002490->unk88->unk00) == 4)
            gUnk_03002490->unk73 = 1;
        else
            gUnk_03002490->unk73 = 0;
    }
    if (gUnk_03002490->unk73 == 0)
    {
        if (gUnk_03002490->unk88->unk06 == 1)
        {
            while (1)
            {
                sub_08006338(0x153);
                TaskYieldTrampoline(gUnk_03002490->unk28 + 2);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 3);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 3);
                sub_08006338(0x148);
                TaskYieldTrampoline(gUnk_03002490->unk28 + 5);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 3);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 3);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 3);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 5);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 3);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
            }
        }
        gUnk_03002490->unk46 = gUnk_0873D2E8[gUnk_03002490->unk88->unk0D];
        switch (gUnk_03002490->unk88->unk0D)
        {
        case 0:
        default:
            while (1)
            {
                sub_08006338(gUnk_03002490->unk46);
                TaskYieldTrampoline(gUnk_03002490->unk28 + 4);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 8);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 4);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 4);
                sub_08006338((s16)(gUnk_03002490->unk46 - 4));
                TaskYieldTrampoline(gUnk_03002490->unk28 + 8);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 4);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
            }
        case 1:
        case 2:
        case 5:
        case 19:
            while (1)
            {
                sub_08006338(gUnk_03002490->unk46);
                TaskYieldTrampoline(gUnk_03002490->unk28 + 2);
                gUnk_03002490->unk6C = 0;
                do
                {
                    t = gUnk_03002490;
                    t->unk3C++;
                    TaskYieldTrampoline(t->unk28 + 2);
                    gUnk_03002490->unk6C++;
                } while ((s16)gUnk_03002490->unk6C <= 10);
                sub_08006338((s16)(gUnk_03002490->unk46 - 8));
                TaskYieldTrampoline(gUnk_03002490->unk28 + 2);
                gUnk_03002490->unk6C = 0;
                do
                {
                    t = gUnk_03002490;
                    t->unk3C++;
                    TaskYieldTrampoline(t->unk28 + 2);
                    gUnk_03002490->unk6C++;
                } while ((s16)gUnk_03002490->unk6C <= 6);
            }
        case 4:
        case 15:
        case 16:
        case 17:
        case 22:
        case 23:
            while (1)
            {
                sub_08006338(gUnk_03002490->unk46);
                TaskYieldTrampoline(gUnk_03002490->unk28 + 4);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 4);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 4);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 4);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 4);
                sub_08006338((s16)(gUnk_03002490->unk46 - 5));
                TaskYieldTrampoline(gUnk_03002490->unk28 + 4);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 4);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 4);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
                t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
            }
        }
    }
    sub_0805afac(gUnk_03002490->unk88->unk00, 6, 0x200);
    if (gUnk_03002490->unk88->unk06 == 1)
        gUnk_03002490->unk46 = 0x15D;
    else
        gUnk_03002490->unk46 = gUnk_0873D350[gUnk_03002490->unk88->unk0D];
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
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
    case 0:
    default:
        sub_08006338(gUnk_03002490->unk46);
        sub_08006138();
    }
}

void sub_08034278(void)
{
    while (sub_0803fd90() == 0 && sub_0803fdf4() == 0 && sub_0803fe68() == 0
           && sub_08040298() == 0 && sub_0803ff7c() == 0 && sub_0803fec4() == 0
           && sub_0803ffe0() == 0)
    {
        struct Task *t;

        if (sub_080400c0() != 0)
            goto end;
        if (sub_08040340() != 0)
            goto end;
        t = gUnk_03002490;
        if (t->unk54 == 0 && t->unk64 == 0)
        {
            t->unk88->unk01 = 1;
        }
        else if (gUnk_03005550.unk0 != 0)
        {
            sub_0803f8e8();
            gUnk_03002490->unk88->unk01 = 1;
        }
        else
        {
            struct Task *t2 = gUnk_03002490;
            struct PlayerState *p = t2->unk88;
            s32 v = p->unk3D;

            if (v != 0)
            {
                p->unk01 = 3;
            }
            else
            {
                if ((gUnk_03002458[p->unk00] & 48) == 0)
                {
                    s32 d = abs(t2->unk54);

                    if ((u32)d <= 0xFFFF)
                        t2->unk28 = 2;
                }
                else
                {
                    t2->unk28 = v;
                }
                if (gUnk_03002490->unk73 == 0)
                {
                    if (sub_0803fd20(gUnk_03002490->unk88->unk00) == 4)
                    {
                        gUnk_03002490->unk73 = 1;
                        sub_08006148(sub_08033d0c, gCurTaskIdx);
                    }
                }
                else if (sub_0803fd20(gUnk_03002490->unk88->unk00) != 4)
                {
                    gUnk_03002490->unk73 = 0;
                    sub_08006148(sub_08033d0c, gCurTaskIdx);
                }
            }
        }
        break;
    }
end:
    sub_08040b40(2, 72);
}
