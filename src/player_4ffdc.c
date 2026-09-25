#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4ffdc.c (0x0804FFDC-0x080507BB, issue #90).
 *
 * Action 58's sub-actions 18-21 (gUnk_0873B6AC) and sub-handlers 22-25
 * (gUnk_0873B6BC), interleaved in the ROM as 18, 22, 19, 23, 20, 24, 21,
 * 25, and their two helpers.  sub_0804ffdc is a long yield script driven by
 * the 8.8 velocity pairs gUnk_0873B6CC; sub_08050418 spawns task type #6's
 * variant 12 through sub_08053a44; sub_08050508 switches the player to mode
 * 17 with camera presets sub_08040b40(10, 24-27).  sub_08050664 re-binds
 * sub-action 2 when a direction is pressed or held for ten frames, and
 * sub_080506dc steers the player with the held direction (velocity pairs
 * gUnk_0873B724[Task.unk6E], PlayerState.unk10 counting the glide
 * frames). */

extern u8 gUnk_03001F34;
extern u16 gUnk_030023D8;
extern s16 gUnk_02005588[];             /* per-player health (M02's HUD) */
extern s16 gUnk_02005580;
extern u8 gUnk_020055C4;
extern u16 gUnk_0873B6CC[][2];
extern u32 gUnk_0873BD00[];             /* stored to PlayerState.unk64 as (u32)gUnk_0873BD00 */
extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern u8 gUnk_0873B6DC[];
extern u16 gUnk_0873B6E8[][6];
extern u16 gUnk_03001EA4;
extern u16 gUnk_030023C0[];             /* newly-pressed keys, latched per player */
extern u16 gUnk_0873B724[][4];

void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
s32 sub_08009ee8(s32 a, s32 b);
void sub_08032d48(void);
void sub_0803332c(void);
void sub_0803e050(s32 a0);
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_0803f6e0(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_0804fe68(void);
void sub_0804fee8(void);
s32 sub_08053a44(s8 player, u8 variant, s32 arg);
void sub_080502f0(void);
s32 sub_08050664(void);

void sub_0804ffdc(void)
{
    gUnk_03001F34 = 1;
    {
        struct Task *t = gUnk_03002490;
        t->unk04 = (u32)sub_080502f0;
        t->unk70 = 0;
        t->unk3C = 0xFFFF;
        if (gUnk_030023D8 != 20)
        {
            gUnk_02005588[t->unk88->unk00] = 0;
            sub_08009ee8(gUnk_02005580, t->unk88->unk00);
        }
    }
    {
        struct Task *t = gUnk_03002490;
        t->unk4C = 0x80000;
        t->unk50 = 0xB00000;
    }
    TaskYieldTrampoline(65);
    {
        struct Task *t = gUnk_03002490;
        t->unk3C = 0x1056;
        t->unk64 = 0x80000000;
        t->unk68 = 0x80000000;
        t->unk54 = 0x10000;
        t->unk58 = 0xFFFE0000;
    }
    TaskYieldTrampoline(58);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(20);
    {
        struct Task *t = gUnk_03002490;
        t->unk58 = 0xFFFF8000;
        t->unk3C--;
    }
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(12);
    {
        struct Task *t = gUnk_03002490;
        t->unk54 = 0x8000;
        t->unk58 = 0x8000;
        t->unk3C = 0x1051;
    }
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk54 = 0x2000;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk54 = 0xFFFFE000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C = 0x1040;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x1042;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x1045;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x1048;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gUnk_03002490;
        t->unk54 = 0;
        t->unk58 = 0;
        t->unk3C = 0x104B;
    }
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x104E;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gUnk_03002490;
        t->unk3C = 0x105D;
        t->unk54 = 0xFFFF0000;
    }
    TaskYieldTrampoline(16);
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(16);
    if (gUnk_020055C4 == 0)
        sub_0803f6e0();
    {
        struct Task *t = gUnk_03002490;
        u16 *e = gUnk_0873B6CC[t->unk2C];
        s32 v;

        t->unk54 = 0xFFFF8000;
        v = e[0] << 8;
        if (e[0] & 0x8000)
            v |= 0xFF000000;
        t->unk58 = v;
        TaskYieldTrampoline(16);
        t = gUnk_03002490;
        v = e[1] << 8;
        if (e[1] & 0x8000)
            v |= 0xFF000000;
        t->unk58 = v;
    }
    TaskYieldTrampoline(16);
    gUnk_03002490->unk3C = 0x105E;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x1041;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x1044;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x1045;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x1048;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gUnk_03002490;
        t->unk54 = 0xFFFFE000;
        t->unk58 = 0x8000;
    }
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x1049;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x104C;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x104D;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x104F;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gUnk_03002490;
        t->unk54 = 0;
        t->unk3C = 0x103F;
        t->unk58 = 0x2000;
    }
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(14);
    gUnk_03002490->unk58 = 0;
    TaskYieldTrampoline(14);
    {
        struct Task *t = gUnk_03002490;
        t->unk70++;
        t->unk88->unk64 = (u32)gUnk_0873BD00;
    }
    sub_08006138();
}

void sub_080502f0(void)
{
    struct Task *t = gUnk_03002490;

    if ((s16)t->unk70 != 0)
    {
        t->unk73 = 1;
        gUnk_03002490->unk04 = (u32)sub_08032d48;
        gUnk_03002490->unk08 = (u32)sub_0803332c;
        sub_08006148(sub_0804fee8, gCurTaskIdx);
        gUnk_03001F34 = 0;
    }
}

void sub_08050340(void)
{
    struct Task *t = gUnk_03002490;

    t->unk46 = 0;
    t->unk28 = 0;
    sub_08006138();
}

void sub_0805035c(void)
{
    if (sub_08050664() == 0)
    {
        u8 k = gUnk_0873B6DC[(gUnk_03002458[gUnk_03002490->unk88->unk00] & 0xF0) >> 4];
        struct Task *t = gUnk_03002490;
        u16 *row = gUnk_0873B6E8[t->unk28];

        if (t->unk28 == 0)
        {
            if (t->unk46 == 0)
                t->unk28 = k;
            else
                t->unk46--;
        }
        else if (t->unk28 == k)
        {
            if (row[t->unk46 + 1] != 0xFFFF)
                t->unk46++;
        }
        else
        {
            if (t->unk46 != 0)
                t->unk46--;
            else
                t->unk28 = k;
        }
        gUnk_03002490->unk3C = row[gUnk_03002490->unk46];
    }
}

void sub_08050418(void)
{
    {
        struct Task *t = gUnk_03002490;
        t->unk70 = 0;
        sub_0803e34c(155, (u16)t->unk88->unk00);
    }
    gUnk_03002490->unk3C = 0x1040;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 0x1041;
    TaskYieldTrampoline(1);
    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
    {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
    }
    sub_08053a44(gUnk_03002490->unk88->unk00, 12, 0);
    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 9; gUnk_03002490->unk6C++)
    {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
    }
    gUnk_03002490->unk70++;
    sub_08006138();
}

void sub_080504d4(void)
{
    struct Task *t = gUnk_03002490;

    if ((s16)t->unk70 != 0)
    {
        t->unk73 = 1;
        sub_08006148(sub_0804fe68, gCurTaskIdx);
    }
}

void sub_08050508(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 17;
    {
        struct Task *t = gUnk_03002490;
        t->unk70 = 0;
        t->unk88->unk3F = 1;
    }
    {
        struct Task *t = gUnk_03002490;
        t->unk88->unk12 = 0x8000;
        if (gUnk_03001EA4 & 1)
            sub_0803e34c(111, (u16)t->unk88->unk00);
        else
            sub_0803e34c(112, (u16)t->unk88->unk00);
    }
    sub_0803e050(3);
    if ((s8)gUnk_03002490->unk7D == 0)
        sub_08040b40(10, 24);
    else
        sub_08040b40(10, 26);
    gUnk_03002490->unk3C = 0x105D;
    TaskYieldTrampoline(4);
    if ((s8)gUnk_03002490->unk7D == 0)
        sub_08040b40(10, 25);
    else
        sub_08040b40(10, 27);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_0803e050(1);
    sub_0803e1b8(1, 96, gUnk_03002490->unk88->unk00);
    gUnk_03002490->unk70++;
    sub_08006138();
}

void sub_08050630(void)
{
    struct Task *t = gUnk_03002490;

    if ((s16)t->unk70 != 0)
    {
        t->unk73 = 1;
        sub_08006148(sub_0804fee8, gCurTaskIdx);
    }
}

s32 sub_08050664(void)
{
    if (!(gUnk_030023C0[gUnk_03002490->unk88->unk00] & 3)
        && (!(gUnk_03002458[gUnk_03002490->unk88->unk00] & 3)
            || (s16)++gUnk_03002490->unk88->unk14 != 10))
        return 0;
    gUnk_03002490->unk88->unk14 = 0;
    gUnk_03002490->unk73 = 2;
    sub_08006148(sub_0804fee8, gCurTaskIdx);
    return 1;
}

void sub_080506dc(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk88->unk04 == 13)
    {
        u16 k = gUnk_03002458[t->unk88->unk00] & 0xF0;

        if (k != 0)
        {
            u16 *e;
            s32 v;

            t->unk6E = k >> 4;
            e = gUnk_0873B724[t->unk6E];
            v = e[0] << 8;
            if (e[0] & 0x8000)
                v |= 0xFF000000;
            t->unk54 = v;
            t->unk64 = 0x20000;
            v = e[1] << 8;
            if (e[1] & 0x8000)
                v |= 0xFF000000;
            t->unk58 = v;
            t->unk68 = 0x20000;
            t->unk88->unk10 = 8;
        }
        else if (t->unk88->unk10 != 0)
        {
            u16 *e;
            s32 v;

            e = gUnk_0873B724[t->unk6E];
            v = e[2] << 8;
            if (e[2] & 0x8000)
                v |= 0xFF000000;
            t->unk54 = v;
            v = e[3] << 8;
            if (e[3] & 0x8000)
                v |= 0xFF000000;
            t->unk58 = v;
            if (--t->unk88->unk10 == 0)
            {
                gUnk_03002490->unk54 = 0;
                gUnk_03002490->unk58 = 0;
            }
        }
    }
}
