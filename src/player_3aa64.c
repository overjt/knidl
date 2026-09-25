#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_3aa64.c (0x0803AA64-0x0803BDE7, issue #91).
 *
 * Player action bodies, part 9: actions 18 and 23-28, per-frame handlers
 * 17 and 20-25.  gUnk_03002458[player] is the held-keys mask the bodies
 * test (0x30 left/right, 0x41 A or up, 0x80 down; gUnk_030023C0[] is the
 * newly-pressed one).  sub_0803aa64 (action 23) is the twin of M11's
 * sub_08043014: a four-state machine over Task.unk73 picked from the keys
 * (left/right = 3, A/up = 1, down = 2, else 0) that plays a row of
 * gUnk_0873D9DA[4][4] chosen by the ability; its per-frame handler 20,
 * sub_0803afcc, re-picks the state and re-binds the coroutine when the
 * keys change.  Actions 24-28 (sub_0803b3c4, sub_0803b4f8, sub_0803b768,
 * sub_0803b87c, sub_0803b9a0) are short animation scripts, the last a
 * four-way directional pick; handlers 21-25 run M11's predicates and
 * request the next action through PlayerState.unk01.  sub_0803bd90
 * (action 18) installs handler 17, the leaf sub_0803bdd4. */

extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern s16 gUnk_0873D9DA[4][4];
extern u16 gUnk_030023C0[];             /* newly-pressed keys, latched per player */
extern s16 gUnk_0300244C;

void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_080061c0(s32 a, s32 b);
void sub_08006338(s32 a);
void sub_0803e050(s32 a0);
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_0803e4a8(void);
s32 sub_0803e4ec(s32 a0);
void sub_0803f870(void);
void sub_0803f9c0(void);
s32 sub_0803fec4(void);
s32 sub_080400c0(void);
s32 sub_08040298(void);
s32 sub_08040340(void);
s32 sub_080403e4(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
void sub_08053940(s32 a, s32 b, s32 c);      /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */

void sub_0803aa64(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s16 *anim;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 15;
    gUnk_03002490->unk15 = 20;
    t = gUnk_03002490;
    if (t->unk88->unk06 != 0)
    {
        if (t->unk88->unk05 != 15)
        {
            t->unk73 = 0;
            u = gUnk_03002490;
            u->unk88->unk14 = 1;
            if (gUnk_03002458[u->unk88->unk00] & 65)
                u->unk28 = 2;
            else
                u->unk28 = 5;
            sub_0803e4a8();
        }
        while (1)
        {
            sub_08006338(364);
            TaskYieldTrampoline(gUnk_03002490->unk28);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 3; gUnk_03002490->unk6C++)
            {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(gUnk_03002490->unk28);
            }
            sub_08006338(0x161);
            TaskYieldTrampoline(gUnk_03002490->unk28);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 9; gUnk_03002490->unk6C++)
            {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(gUnk_03002490->unk28);
                if (gUnk_03002490->unk3C == 356)
                    sub_0803e34c(135, gUnk_03002490->unk88->unk00);
            }
            sub_0803e34c(135, gUnk_03002490->unk88->unk00);
        }
    }
    v = gUnk_03002490;
    if (v->unk88->unk05 != 15)
    {
        if (gUnk_03002458[v->unk88->unk00] & 48)
            v->unk73 = 3;
        else if (gUnk_03002458[v->unk88->unk00] & 65)
            v->unk73 = 1;
        else if (gUnk_03002458[v->unk88->unk00] & 128)
            v->unk73 = 2;
        else
            v->unk73 = 0;
        sub_0803e4a8();
        gUnk_03002490->unk2C = 0;
    }
    gUnk_03002490->unk88->unk3D = 0;
    switch (gUnk_03002490->unk88->unk0D)
    {
    default:
        anim = gUnk_0873D9DA[0];
        break;
    case 4:
        anim = gUnk_0873D9DA[1];
        break;
    case 9:
        anim = gUnk_0873D9DA[2];
        break;
    case 10:
        anim = gUnk_0873D9DA[3];
        break;
    }
    switch (gUnk_03002490->unk73)
    {
    case 0:
        sub_080413a4(13);
        sub_08040b40(11, 4);
        if (gUnk_03002490->unk2C != 0)
        {
            gUnk_03002490->unk2C = 0;
            sub_08006338(anim[3]);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(3);
        }
        sub_08006338(anim[0]);
        break;
    case 1:
        sub_080413a4(13);
        sub_08040b40(11, 4);
        gUnk_03002490->unk46 = anim[1];
        while (1)
        {
        lab1:
            sub_0803e34c(120, gUnk_03002490->unk88->unk00);
            gUnk_03002490->unk28 = 0;
            if (gUnk_03002490->unk88->unk5C & 1)
            {
                sub_080413a4(15);
                sub_08006338((s16)(gUnk_03002490->unk46 + 1));
                TaskYieldTrampoline(2);
                for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
                {
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                }
                gUnk_03002490->unk6C = 0;
                do
                {
                    gUnk_03002490->unk3C++;
                    if (gUnk_03002490->unk28 != 0)
                        goto lab1;
                    TaskYieldTrampoline(1);
                    if (gUnk_03002490->unk28 != 0)
                        goto lab1;
                    TaskYieldTrampoline(1);
                } while ((s16)++gUnk_03002490->unk6C <= 4);
                gUnk_03002490->unk68 = 0x10000;
            }
            sub_08006338(gUnk_03002490->unk46);
            gUnk_03002490->unk6C = 0;
            do
            {
                if (gUnk_03002490->unk28 != 0)
                    goto lab1;
                TaskYieldTrampoline(1);
            } while ((s16)++gUnk_03002490->unk6C <= 14);
            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 65)
                goto lab1;
            gUnk_03002490->unk6C = 0;
            do
            {
                if (gUnk_03002490->unk28 != 0)
                    goto lab1;
                TaskYieldTrampoline(1);
            } while ((s16)++gUnk_03002490->unk6C <= 44);
            gUnk_03002490->unk28 = -1;
            sub_08006138();
        }
    case 2:
        sub_080413a4(14);
        sub_08040b40(11, 4);
        gUnk_03002490->unk88->unk14 = 15;
        sub_08006338(anim[3]);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        break;
    case 3:
        sub_08040b40(11, 3);
        gUnk_03002490->unk28 = 0;
        if (gUnk_03002490->unk88->unk5C & 1)
        {
            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 65)
                sub_080413a4(12);
            else
                sub_080413a4(13);
        }
        else
        {
            sub_080413a4(13);
            gUnk_03002490->unk28 = 10;
        }
        gUnk_03002490->unk88->unk14 = 15;
        gUnk_03002490->unk46 = anim[2];
        while (1)
        {
            sub_0803e34c(120, gUnk_03002490->unk88->unk00);
            sub_08006338(gUnk_03002490->unk46);
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
        }
    }
    sub_08006138();
}

void sub_0803afcc(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct PlayerState *q;
    u8 *st;

    if (sub_08040340() != 0)
        return;
    sub_0803f870();
    t = gUnk_03002490;
    if (t->unk88->unk06 != 0)
    {
        if (t->unk58 < 0)
        {
            t->unk28 = 2;
        }
        else
        {
            t->unk28 = 5;
            t->unk88->unk14 = 1;
        }
        if ((gUnk_03002458[gUnk_03002490->unk88->unk00] & 65) && --gUnk_03002490->unk88->unk14 == 0)
        {
            gUnk_03002490->unk88->unk14 = 15;
            sub_080413a4(18);
        }
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 128)
            sub_080413a4(17);
        else
            sub_080413a4(16);
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48)
            sub_08040b40(11, 3);
        else
            sub_08040b40(11, 4);
    }
    else
    {
        st = &t->unk73;
        switch (*st)
        {
        case 0:
            if (gUnk_03002458[t->unk88->unk00] & 48)
                goto set3a;
            if (gUnk_03002458[t->unk88->unk00] & 65)
                goto set1a;
            if (!(gUnk_03002458[t->unk88->unk00] & 128))
                break;
            *st = 2;
            sub_08006148(sub_0803aa64, gCurTaskIdx);
            break;
        case 1:
            /* The do { } while (0) changes no code: it counts case 1's
               references one loop level deeper (lessons 3.383, 3.412),
               which lets the key-mask value win its register ahead of
               the switch value, as in the ROM. */
            do
            {
                if ((gUnk_03002458[t->unk88->unk00] & 193) == 128)
                    goto set2b;
                if (gUnk_030023C0[t->unk88->unk00] & 48)
                    goto set3b;
                if (t->unk28 == -1)
                    goto set0b;
                if (gUnk_030023C0[t->unk88->unk00] & 65)
                    t->unk28 = 1;
            } while (0);
            break;
        case 2:
            t->unk2C = 0;
            if (gUnk_03002458[t->unk88->unk00] & 65)
                goto set1c;
            if (gUnk_03002458[t->unk88->unk00] & 48)
                goto set3c;
            if (!(gUnk_03002458[t->unk88->unk00] & 240))
                goto set0c;
            if ((s16)t->unk88->unk14 != 0)
                t->unk88->unk14--;
            break;
        set1d:
            *st = 1;
            sub_08006148(sub_0803aa64, gCurTaskIdx);
            goto keys;
        set2d:
            *st = 2;
            sub_08006148(sub_0803aa64, gCurTaskIdx);
            goto keys;
        dec2:
            t->unk88->unk14--;
            goto keys;
        case 3:
            if (gUnk_03002458[t->unk88->unk00] & 48)
                goto keys3;
            if (gUnk_03002458[t->unk88->unk00] & 65)
                goto set1d;
            if (gUnk_03002458[t->unk88->unk00] & 128)
                goto set2d;
            if ((s16)t->unk88->unk14 != 0)
                goto dec2;
            if (gUnk_03002458[t->unk88->unk00] & 241)
                goto keys;
            if (t->unk58 < 0)
                goto keys;
            *st = 0;
            sub_08006148(sub_0803aa64, gCurTaskIdx);
        keys:
            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48)
            {
            keys3:
                sub_08040b40(11, 3);
            }
            else
                sub_08040b40(11, 4);
            v = gUnk_03002490;
            if (v->unk28 != 0)
                goto dec;
            if (gUnk_03002458[v->unk88->unk00] & 65)
                sub_080413a4(12);
            else
                sub_080413a4(13);
            break;
        set3a:
            *st = 3;
            sub_08006148(sub_0803aa64, gCurTaskIdx);
            break;
        set1a:
            *st = 1;
            sub_08006148(sub_0803aa64, gCurTaskIdx);
            break;
        set2b:
            *st = 2;
            sub_08006148(sub_0803aa64, gCurTaskIdx);
            break;
        set3b:
            *st = 3;
            sub_08006148(sub_0803aa64, gCurTaskIdx);
            break;
        set0b:
            *st = 0;
            sub_08006148(sub_0803aa64, gCurTaskIdx);
            break;
        set1c:
            *st = 1;
            sub_08006148(sub_0803aa64, gCurTaskIdx);
            break;
        set3c:
            *st = 3;
            sub_08006148(sub_0803aa64, gCurTaskIdx);
            break;
        set0c:
            *st = 0;
            gUnk_03002490->unk2C = 1;
            sub_08006148(sub_0803aa64, gCurTaskIdx);
            break;
        set25:
            w->unk88->unk01 = 25;
            goto out;
        dec:
            v->unk28--;
            sub_080413a4(13);
        }
    }
    if (sub_080400c0() == 0 && sub_08040298() == 0)
    {
        x = gUnk_03002490;
        if (!(x->unk7B & 1))
        {
            if ((gUnk_03002458[(q = x->unk88)->unk00] & 64) && q->unk06 != 1)
                q->unk01 = 9;
            else
                q->unk01 = 5;
            gUnk_03002490->unk88->unk3D = 0;
            gUnk_03002490->unk88->unk0F = 0;
        }
        else
        {
            q = x->unk88;
            if (q->unk06 != 0 || x->unk73 != 1 || !(gUnk_03002458[q->unk00] & 65))
            {
                w = gUnk_03002490;
                if (w->unk58 != 0 && (w->unk7A & 1))
                {
                    if (w->unk54 != 0)
                        goto set25;
                    w->unk88->unk01 = 24;
                }
            }
        }
    }
out:
    sub_0803f9c0();
    if (gUnk_03002490->unk7A & 1)
        sub_0803e4ec(0);
}

void sub_0803b3c4(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 0;
    gUnk_03002490->unk15 = 21;
    if (gUnk_03002490->unk88->unk05 != 0)
    {
        sub_0803e050(3);
        gUnk_03002490->unk28 = gUnk_03002490->unk88->unk4A;
    }
    if (gUnk_03002490->unk88->unk06 == 1)
    {
        gUnk_03002490->unk46 = 352;
    }
    else
    {
        switch (gUnk_03002490->unk88->unk0D)
        {
        default:
            gUnk_03002490->unk46 = 221;
            break;
        case 4:
            gUnk_03002490->unk46 = 1184;
            break;
        case 9:
            gUnk_03002490->unk46 = 0x7C1;
            break;
        case 10:
            gUnk_03002490->unk46 = 0x8C1;
            break;
        }
    }
    sub_08006338(gUnk_03002490->unk46);
    sub_08006138();
}

void sub_0803b47c(void)
{
    sub_0803f870();
    while (sub_08040298() == 0 && sub_080403e4() == 0 && sub_0803fec4() == 0
           && sub_080400c0() == 0 && sub_08040340() == 0)
    {
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48)
        {
            if ((gUnk_03002458[gUnk_03002490->unk88->unk00] & 16) && gUnk_03002490->unk28 == 1)
                break;
            if ((gUnk_03002458[gUnk_03002490->unk88->unk00] & 32) && gUnk_03002490->unk28 == 2)
                break;
            gUnk_03002490->unk88->unk01 = 25;
        }
        break;
    }
}

void sub_0803b4f8(void)
{
    struct Task *t;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 1;
    gUnk_03002490->unk15 = 22;
    t = gUnk_03002490;
    if ((u32)abs(t->unk54) > 0x9900)
    {
        if (t->unk54 < 0)
            t->unk54 = -0x9900;
        else
            t->unk54 = 0x9900;
    }
    sub_08040b40(9, 72);
    if (gUnk_03002490->unk88->unk06 == 1)
    {
        while (1)
        {
            sub_08006338(364);
            TaskYieldTrampoline(5);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 3; gUnk_03002490->unk6C++)
            {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(5);
            }
            sub_08006338(0x161);
            TaskYieldTrampoline(5);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 9; gUnk_03002490->unk6C++)
            {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(5);
            }
        }
    }
    switch (gUnk_03002490->unk88->unk0D)
    {
    default:
        gUnk_03002490->unk46 = 207;
        break;
    case 4:
        gUnk_03002490->unk46 = 0x492;
        break;
    case 9:
        gUnk_03002490->unk46 = 0x7B3;
        break;
    case 10:
        gUnk_03002490->unk46 = 0x8B3;
        break;
    }
    while (1)
    {
        sub_08006338(gUnk_03002490->unk46);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
        }
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        sub_08006338((s16)(gUnk_03002490->unk46 - 6));
        TaskYieldTrampoline(10);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 3; gUnk_03002490->unk6C++)
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
        }
    }
}

void sub_0803b6ec(void)
{
    sub_0803f870();
    while (sub_08040298() == 0 && sub_080403e4() == 0 && sub_0803fec4() == 0
           && sub_080400c0() == 0 && sub_08040340() == 0)
    {
        if (gUnk_03002490->unk54 == 0 && gUnk_03002490->unk64 == 0)
        {
            gUnk_03002490->unk88->unk01 = 24;
            break;
        }
        if (!(gUnk_03002490->unk7A & 1))
            gUnk_03002490->unk88->unk01 = 23;
        break;
    }
    sub_08040b40(9, 72);
}

void sub_0803b768(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 12;
    gUnk_03002490->unk15 = 25;
    gUnk_03002490->unk28 = 0;
    gUnk_03002490->unk88->unk06 = 0;
    sub_08006338(225);
    TaskYieldTrampoline(2);
    sub_0803e34c(113, gUnk_03002490->unk88->unk00);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_08006338(227);
    TaskYieldTrampoline(2);
    sub_08006338(229);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk28++;
    sub_08006138();
}

void sub_0803b828(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 != 0)
    {
        gUnk_03002458[t->unk88->unk00] = gUnk_030023C0[t->unk88->unk00] = 0;
        if (t->unk7A & 1)
            t->unk88->unk01 = 24;
        else
            t->unk88->unk01 = 23;
    }
}

void sub_0803b87c(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 11;
    gUnk_03002490->unk15 = 24;
    gUnk_03002490->unk28 = 0;
    gUnk_03002490->unk88->unk06 = 0;
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
    gUnk_03002490->unk28++;
    sub_08006138();
}

void sub_0803b914(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 != 0)
    {
        if (!(t->unk7A & 1) || (gUnk_03002458[t->unk88->unk00] & 65))
            t->unk88->unk01 = 23;
        else
            t->unk88->unk01 = 24;
    }
    if (gUnk_03002490->unk7A & 1)
    {
        if (gUnk_03002490->unk54 != 0)
            sub_08040b40(8, 72);
        sub_0803e4ec(1);
    }
    else
    {
        sub_080413a4(13);
        sub_08040b40(11, 4);
    }
    sub_0803f9c0();
}

void sub_0803b9a0(void)
{
    struct Task *t;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 11;
    gUnk_03002490->unk15 = 23;
    t = gUnk_03002490;
    if (t->unk88->unk05 != 11)
    {
        if (gUnk_03002458[t->unk88->unk00] & 240)
        {
            if (gUnk_03002458[t->unk88->unk00] & 64)
                t->unk28 = 3;
            else if (gUnk_03002458[t->unk88->unk00] & 128)
                t->unk28 = 1;
            else if (gUnk_03002458[t->unk88->unk00] & 16)
                t->unk28 = 0;
            else if (gUnk_03002458[t->unk88->unk00] & 32)
                t->unk28 = 2;
        }
        else if (t->unk43 == 1)
        {
            t->unk28 = 0;
        }
        else
        {
            t->unk28 = 2;
        }
        gUnk_03002490->unk73 = 0;
        {
            struct Task *u = gUnk_03002490;

            u->unk2C = 0;
            u->unk88->unk14 = 15;
            if ((u32)abs(u->unk54) > 0x10C00)
                sub_080061c0(0x10C00, 0x5A5A5A5A);
        }
        if (gUnk_0300244C != 0)
        {
            struct Task *v = gUnk_03002490;

            if ((u32)abs(v->unk58) > 0xE000)
            {
                v->unk58 = 0xE000;
                sub_080413a4(13);
            }
        }
    }
    switch (gUnk_03002490->unk28)
    {
    case 3:
        if (gUnk_03002490->unk73 == 0)
        {
            sub_08053940(gUnk_03002490->unk88->unk00, 3, 0);
            while (1)
            {
                sub_08006338(239);
                TaskYieldTrampoline(3);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(3);
            }
        }
        gUnk_03002490->unk28 = -1;
        sub_08006338(241);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk2C++;
        break;
    case 1:
        if (gUnk_03002490->unk73 == 0)
        {
            sub_08053940(gUnk_03002490->unk88->unk00, 3, 0);
            while (1)
            {
                sub_08006338(235);
                TaskYieldTrampoline(3);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(3);
            }
        }
        gUnk_03002490->unk28 = -1;
        sub_08006338(237);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk2C++;
        break;
    case 0:
        if (gUnk_03002490->unk73 == 0)
        {
            gUnk_03002490->unk43 = 1;
            sub_08053940(gUnk_03002490->unk88->unk00, 3, 0);
            while (1)
            {
                sub_08006338(231);
                TaskYieldTrampoline(3);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(3);
            }
        }
        gUnk_03002490->unk28 = -1;
        sub_08006338(233);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk2C++;
        break;
    case 2:
        if (gUnk_03002490->unk73 == 0)
        {
            gUnk_03002490->unk43 = -1;
            sub_08053940(gUnk_03002490->unk88->unk00, 3, 0);
            while (1)
            {
                sub_08006338(231);
                TaskYieldTrampoline(3);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(3);
            }
        }
        gUnk_03002490->unk28 = -1;
        sub_08006338(233);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk2C++;
        break;
    }
    sub_08006138();
}

void sub_0803bbf0(void)
{
    struct Task *t = gUnk_03002490;
    u8 *st = &t->unk73;

    if (*st == 0)
    {
        if (!(gUnk_03002458[t->unk88->unk00] & 2) && (s16)t->unk88->unk14 == 0)
        {
            *st = 1;
            sub_08006148(sub_0803b9a0, gCurTaskIdx);
        }
        else
        {
            s32 d = gUnk_03002490->unk28;

            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 240)
            {
                if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 64)
                    d = 3;
                else if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 128)
                    d = 1;
                else if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 16)
                    d = 0;
                else if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 32)
                    d = 2;
            }
            if (d != gUnk_03002490->unk28)
            {
                gUnk_03002490->unk28 = d;
                sub_08006148(sub_0803b9a0, gCurTaskIdx);
            }
        }
        if ((s16)gUnk_03002490->unk88->unk14 != 0)
            gUnk_03002490->unk88->unk14--;
    }
    else if (t->unk2C != 0 && sub_080403e4() == 0)
    {
        if (!(gUnk_03002490->unk7A & 1) || (gUnk_03002458[gUnk_03002490->unk88->unk00] & 65))
            gUnk_03002490->unk88->unk01 = 23;
        else
            gUnk_03002490->unk88->unk01 = 24;
    }
    if (!(gUnk_03002490->unk7B & 1))
    {
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 64)
            gUnk_03002490->unk88->unk01 = 9;
        else
            gUnk_03002490->unk88->unk01 = 5;
    }
    if (gUnk_03002490->unk7A & 1)
    {
        sub_0803e4ec(1);
        sub_08040b40(8, 72);
    }
    else
    {
        sub_08040b40(11, 4);
        sub_080413a4(13);
    }
    sub_0803f9c0();
}

void sub_0803bd90(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 5;
    gUnk_03002490->unk15 = 17;
    sub_08040b40(11, 16);
    sub_080413a4(22);
    gUnk_03002490->unk7A = 0;
    sub_08006138();
}

void sub_0803bdd4(void)
{
    gUnk_03002490->unk88->unk01 = 7;
}
