#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_3919c.c (0x0803919C-0x08039C23, issue #91).
 *
 * Player action bodies, part 7: actions 17 and 20.  sub_0803919c (action
 * 17) is the player's death: it installs sub_080396a4 as the task's
 * per-frame callback Task.unk04 (state 1 falls until the player is below
 * the screen, state 2 waits PlayerState.unk14 frames, state 4 leaves for
 * the results screen), counts the players whose health gUnk_02005588[] is
 * not 0, plays the lost-life or game-over music, loops the fall animation
 * until the callback reaches state 3, and when gUnk_0300234C drops to 0
 * raises M02's stage request gUnk_03002438 = 6; Task.unk73 = 4 or 5 then
 * says whether the player has lives left (gUnk_02007D48[]).
 * sub_080397f8 (action 20) enters a door: it stops the player, plays the
 * landing or crouch animation picked by M11's sub_080404e4, calls M07's
 * door code sub_08025024 and plays the ability's door animation
 * (gUnk_0873D632[ability][0]). */

struct CamPos { u16 x, y; };

extern u8 gUnk_03002350;
extern u8 gUnk_03002340;
extern u16 gUnk_0873D9FA[][2];
extern u16 gUnk_030023AC;               /* number of players */
extern s16 gUnk_02005588[];             /* health per player (M02's HUD) */
extern u16 gUnk_03002360;
extern vu16 gUnk_03001ED8;              /* DISPCNT shadow */
extern vs16 gUnk_03004CA0[];
extern u8 gUnk_02007CF0;
extern u8 gUnk_0300234C;
extern s8 gUnk_03002438;                /* stage request (M02) */
extern s16 gUnk_02007D48[];
extern u8 gUnk_03001F34;
extern u16 gUnk_030023D8;
extern struct CamPos gUnk_030055D0[4];
extern s16 gUnk_030023E4;
extern s8 gUnk_03002444;
extern s16 gUnk_0873D7E4[][3];
extern u16 gUnk_0873D632[][7];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
s32 sub_08003110(s32 songId);
s32 sub_080031b8(s32 id);
void sub_08003484(void);
s32 sub_080035f4(s32 songId);
void sub_08003688(void);
void sub_080055b0(u8 val, s32 idx);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_08006338(s32 a);
s32 sub_0800a008(s32 a, s32 b, u32 c);
s32 sub_08025024(void);
void sub_080261d4(u16 a);
s32 sub_080264b0(void);
void sub_080271ec(void);
void sub_08027204(u32 a);
void sub_08027548(void);
s32 sub_080276ac(s32 a);
s32 sub_080276cc(s32 i);
void sub_0803e050(s32 a0);
void sub_0803e080(void);
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_0803e3e4(s32 a0);
void sub_0803e868(void);
u16 sub_0803f7e0(u16 a0);
s32 sub_080404e4(void);
void sub_08040934(s32 a0);
void sub_080413a4(s32 a0);
void sub_08053940(s32 a, s32 b, s32 c);      /* M14, still asm */
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);
s32 sub_0805b088(s32 a0, s32 a1, s32 a2);
void sub_080b9118(void);
void sub_080b9610(void);
void sub_080396a4(void);

void sub_0803919c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *t2;
    u16 *anim;
    u16 n;
    u16 i;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 18;
    gUnk_03002490->unk04 = (u32)sub_080396a4;
    gUnk_03002490->unk08 = 0;
    gUnk_03002490->unk43 = 1;
    t = gUnk_03002490;
    t->unk3E &= 0x7FFF;
    gUnk_03002350--;
    gUnk_03002340 &= ~(1 << t->unk88->unk00);
    if (t->unk82 == 0x200)
        sub_08027548();
    u = gUnk_03002490;
    u->unk88->unk40 |= 8;
    u->unk88->unk42 |= 0x100;
    u->unk73 = 0;
    gUnk_03002490->unk88->unk42 &= 0xFFEF;
    sub_0803e1b8(255, 0, gUnk_03002490->unk88->unk00);
    sub_0803e050(3);
    gUnk_03002490->unk88->unk06 = 0;
    sub_0800a008(0, -1, gUnk_03002490->unk88->unk00);
    sub_080276ac(gUnk_03002490->unk88->unk00);
    gUnk_03002490->unk88->unk16 = 255;
    anim = gUnk_0873D9FA[gUnk_03002490->unk88->unk0D];
    n = 0;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if (gUnk_02005588[i] != 0)
            n++;
    }
    if (n == 0)
    {
        sub_08003484();
        sub_08003688();
        gUnk_03002490->unk42 = 4;
        gUnk_03002490->unk88->unk17 = n;
        gUnk_03002490->unk88->unk18 = n;
        gUnk_03002490->unk88->unk1A = gUnk_03002490->unk88->unk1C = n;
    }
    else
    {
        sub_0803e868();
    }
    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
    {
        gUnk_03002490->unk3C = anim[0];
        if (gUnk_030023AC == 1)
        {
            sub_08003688();
            sub_08003484();
            sub_0803e3e4(15);
            sub_08027204(2);
            if (!(gUnk_03001ED8 & 0x400))
            {
                gUnk_03001ED8 &= 0xE0FF;
                gUnk_03001ED8 |= 0x1D00;
            }
        }
        else if (gUnk_03002340 == 0)
        {
            for (i = 4; i <= 63; i++)
            {
                if (gUnk_03004CA0[i] != -1 && i != 63)
                    sub_080055b0(15, i);
            }
            sub_08003688();
            sub_080271ec();
            sub_08027204(2);
            if (!(gUnk_03001ED8 & 0x400))
            {
                gUnk_03001ED8 &= 0xE0FF;
                gUnk_03001ED8 |= 0x1D00;
            }
        }
        else
        {
            sub_08040934(gUnk_03002490->unk88->unk00);
        }
        if (gUnk_03002490->unk88->unk37 != 2 || gUnk_03002340 == 0)
            sub_080261d4(4);
    }
    else
    {
        gUnk_03002490->unk3C = anim[0];
        if (gUnk_03002340 == 0)
        {
            for (i = 4; i <= 63; i++)
            {
                if (gUnk_03004CA0[i] != -1 && i != 63)
                    sub_080055b0(15, i);
            }
            sub_08003688();
            sub_080271ec();
            sub_08027204(2);
            sub_080261d4(4);
            if (!(gUnk_03001ED8 & 0x400))
            {
                gUnk_03001ED8 &= 0xE0FF;
                gUnk_03001ED8 |= 0x1D00;
            }
        }
        else
        {
            sub_08040934(gUnk_03002490->unk88->unk00);
        }
    }
    TaskYieldTrampoline(1);
    if (gUnk_03002340 == 0)
        sub_080031b8(158);
    else
        sub_0803e34c(158, gUnk_03002490->unk88->unk00);
    TaskYieldTrampoline(59);
    gUnk_03002490->unk73 = 1;
    gUnk_03002490->unk88->unk22 = 1;
    gUnk_03002490->unk88->unk12 = 0x8000;
    for (i = 0; i <= 3; i++)
        sub_0805afac(gUnk_03002490->unk88->unk00, 12, i);
    if (n == 0)
        sub_08003110(3);
    else if (gUnk_02007CF0 != 1)
        sub_080031b8(270);
    sub_080413a4(32);
    gUnk_03002490->unk46 = anim[1];
    do
    {
        struct Task *w = gUnk_03002490;
        struct Task *x;

        if (w->unk0C != 0)
            sub_0805b088(w->unk88->unk00, 5, 0);
        gUnk_03002490->unk3C = gUnk_03002490->unk46;
        TaskYieldTrampoline(1);
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 6; gUnk_03002490->unk6C++)
        {
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(1);
        }
        x = gUnk_03002490;
        if (x->unk0C != 0)
            sub_0805b088(x->unk88->unk00, 5, 0);
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 7; gUnk_03002490->unk6C++)
        {
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(1);
        }
    } while (gUnk_03002490->unk73 != 3);
    gUnk_0300234C--;
    t2 = gUnk_03002490;
    t2->unk00 = 0;
    t2->unk0C = 0;
    t2->unk08 = 0;
    if (gUnk_0300234C == 0)
    {
        gUnk_03002438 = 6;
        TaskDispatchTrampoline();
    }
    sub_080276cc(gUnk_03002490->unk88->unk00);
    if (gUnk_02007D48[gUnk_03002490->unk88->unk00] == 0)
        gUnk_03002490->unk73 = 4;
    else
        gUnk_03002490->unk73 = 5;
    gUnk_03001F34 = 0;
    sub_08006138();
}

void sub_080396a4(void)
{
    switch (gUnk_03002490->unk73)
    {
    case 4:
    {
        struct Task *t = gUnk_03002490;

        t->unk04 = 0;
        t->unk12 = 4;
        if (gUnk_030023D8 != 20)
            sub_08006148(sub_080b9610, gCurTaskIdx);
        else
            sub_080b9118();
        break;
    }
    case 1:
    {
        struct Task *t = gUnk_03002490;

        if (t->unk58 > 0)
        {
            struct PlayerState *p;
            u16 y;

            t->unk68 = 0x40000;
            p = t->unk88;
            if (p->unk37 != 2)
                y = t->unk4A - (u16)(gUnk_030055D0[p->unk00].y - 80);
            else
                y = t->unk4A;
            if ((s16)y > 184)
            {
                struct Task *u;
                struct PlayerState *q;

                gUnk_03002490->unk88->unk14 = 90;
                gUnk_03002490->unk73 = 2;
                sub_0803e050(2);
                u = gUnk_03002490;
                q = u->unk88;
                if (q->unk37 != 2 && u->unk4A - gUnk_030023E4 <= 183)
                    sub_0805afac(q->unk00, 18, 0);
                gUnk_03002490->unk0C = 0;
            }
        }
        if (!(gUnk_03002490->unk88->unk42 & 32))
            sub_0803e080();
        break;
    }
    case 2:
    {
        struct Task *t = gUnk_03002490;

        if (--t->unk88->unk14 == 0)
            t->unk73 = 3;
        break;
    }
    case 0:
    case 3:
    case 5:
        break;
    }
}

void sub_080397f8(void)
{
    struct Task *t;
    s32 n;
    s32 i;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 19;
    gUnk_03002490->unk04 = 0;
    gUnk_03002490->unk08 = 0;
    gUnk_03002490->unk88->unk3D = 0;
    sub_0803e050(3);
    gUnk_03002490->unk88->unk42 |= 0x100;
    sub_080261d4(0);
    if (gUnk_03002444 == 0)
    {
        sub_0803e3e4(15);
        if (!(gUnk_03001ED8 & 0x400))
        {
            gUnk_03001ED8 &= 0xE0FF;
            gUnk_03001ED8 |= 0x1D00;
        }
        n = 0;
        if (gUnk_030023AC > 1 && gUnk_0300234C > 1)
        {
            for (i = 0; i < gUnk_030023AC; i++)
            {
                if (gUnk_02005588[i] == 0)
                {
                    n++;
                    sub_080055b0(0, i);
                }
            }
        }
        if (n != 0)
            sub_080035f4(158);
        else
            sub_08003688();
    }
    switch (sub_080404e4())
    {
    case 1:
        gUnk_03002490->unk88->unk06 = 0;
        sub_08053940(gUnk_03002490->unk88->unk00, 0, 0);
        sub_08006338(gUnk_0873D7E4[gUnk_03002490->unk88->unk0D][2]);
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        break;
    case 2:
        gUnk_03002490->unk88->unk06 = 0;
        if (!(gUnk_03002490->unk7B & 1))
        {
            sub_08006338(79);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            if ((s8)gUnk_03002490->unk88->unk07 > 1)
                sub_08053940(gUnk_03002490->unk88->unk00, 2, 0);
            else
                sub_08053940(gUnk_03002490->unk88->unk00, 1, 0);
            if (gUnk_03002490->unk7A & 1)
            {
                sub_0805afac(gUnk_03002490->unk88->unk00, 2, 0);
                sub_0805afac(gUnk_03002490->unk88->unk00, 2, 1);
            }
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
    default:
            TaskYieldTrampoline(1);
            break;
        }
        sub_08006338(224);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        if ((s8)gUnk_03002490->unk88->unk07 > 1)
            sub_08053940(gUnk_03002490->unk88->unk00, 2, 0);
        else
            sub_08053940(gUnk_03002490->unk88->unk00, 1, 0);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        break;
    }
    sub_08025024();
    if (gUnk_03002444 != 0)
        sub_080264b0();
    if (gUnk_03002444 == 0)
        sub_080031b8(181);
    t = gUnk_03002490;
    if (!(t->unk7B & 1))
    {
        t->unk46 = gUnk_0873D632[t->unk88->unk0D][0];
        switch (t->unk88->unk0D)
        {
        case 0:
        default:
            sub_08006338(gUnk_03002490->unk46);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
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
            while (1)
            {
                sub_08006338((s16)(gUnk_03002490->unk46 + 3));
                TaskYieldTrampoline(2);
                sub_08006338((s16)(gUnk_03002490->unk46 + 15));
                TaskYieldTrampoline(2);
            }
        }
    }
    else
    {
        gUnk_03002490->unk46 = sub_0803f7e0(0);
        sub_08006338(gUnk_03002490->unk46);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
    }
    sub_08006138();
}
