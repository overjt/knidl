#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4b858.c (0x0804B858-0x0804C64B, issue #88).
 *
 * Player action bodies, part 21: action 53 and per-frame handler 50.
 * sub_0804b858 (action 53, mode 13) is `loop: switch (Task.unk73)` over
 * eight states, a stance with six moves.  A fresh entry starts in
 * state 6 with the 120-frame timer PlayerState.unk14 and Task.unk80 =
 * 22: the stance (animation 0xE84, effect 28 on the ground) picks the
 * next move from the input - up 0, back 1 or forward 2 (sub_0803f884,
 * kept in gUnk_03001F2C), down 3, A 4, leaving the ground 5 - or a
 * random one (gUnk_0873B65E[sub_08002ee8(4)]) when the timer runs out,
 * and goes to state 7 once PlayerState.unk08 is 0.  States 0-5 are the
 * moves (animations 0xE97-0xEE4 with the frame index PlayerState.unk16,
 * velocity presets 2 and 56-68, sounds 178/179, a landing with effect
 * 27 and sub_080261d4(2)), each back to state 7, which clears
 * PlayerState.unk42 bit 9, calls sub_0803e1b8(255, 0, player) and ends
 * the action (sub_08006138, falling into state 0, lesson 3.403).  The
 * long `bl`s at 0x0804C49E and 0x0804C488 are cross-jumped `goto loop`
 * tails.  Its handler sub_0804c4ac lets sub_0804042c end state 7,
 * steers state 4 in the air (sub_080061c0 and the 8.8 speed Task.unk64
 * for the direction in gUnk_03001F2C), re-binds state 7 from the other
 * moves once PlayerState.unk16 >= 0 and unk08 == 0, and requests action
 * 23 through sub_0803fce4. */

extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern s32 gUnk_03001F2C;               /* boot_091ac.c spelling */
extern u8 gUnk_0873B65E[];

void TaskYieldTrampoline(s32 frames);
u32 sub_08002ee8(u32 range);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_080061c0(s32 a, s32 b);
void sub_08006338(s32 a);
void sub_080261d4(u16 a);
void sub_0803e050(s32 a0);
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
s32 sub_0803e34c(s32 a0, u16 a1);
s32 sub_0803e4ec(s32 a0);
void sub_0803e650(s32 a0);
void sub_0803f870(void);
s32 sub_0803f884(void);
void sub_0803f9c0(void);
s32 sub_0803fce4(s32 a);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_0804b858(void)
{
    struct Task *h;
    s32 k;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 50;
    h = gUnk_03002490;
    if (h->unk88->unk05 != 13)
    {
        struct Task *u;

        h->unk73 = 6;
        u = gUnk_03002490;
        u->unk88->unk14 = 120;
        u->unk28 = 0;
        u->unk80 = 22;
    }
loop:
    switch (gUnk_03002490->unk73)
    {
    case 6:
        {
            struct Task *a;

            sub_0803e1b8(3, 0, gUnk_03002490->unk88->unk00);
            gUnk_03002490->unk88->unk16 = 0;
            sub_08006338(0xE84);
            sub_0803e650(12);
            a = gUnk_03002490;
            if (a->unk7A & 1)
            {
                sub_0805afac(a->unk88->unk00, 28, 3);
                TaskYieldTrampoline(10);
            }
            else
            {
                sub_0803e050(3);
                TaskYieldTrampoline(10);
            }
        }
        for (;;)
        {
            struct Task *b;
            struct Task *c;
            struct PlayerState *p;

            b = gUnk_03002490;
            if (--b->unk88->unk14 == 0)
            {
                gUnk_03002490->unk73 = gUnk_0873B65E[sub_08002ee8(4)];
                goto loop;
            }
            if (!(b->unk7A & 1))
            {
                b->unk73 = 5;
                goto loop;
            }
            if (gUnk_03002458[b->unk88->unk00] & 1)
            {
                b->unk73 = 4;
                goto loop;
            }
            k = sub_0803f884();
            gUnk_03001F2C = k;
            if (k != 0)
            {
                if (k == 1)
                {
                    gUnk_03002490->unk73 = 2;
                    goto loop;
                }
                gUnk_03002490->unk73 = 1;
                goto loop;
            }
            c = gUnk_03002490;
            p = c->unk88;
            if (gUnk_03002458[p->unk00] & 0xC0)
            {
                if (gUnk_03002458[p->unk00] & 0x40)
                {
                    c->unk73 = 0;
                    goto loop;
                }
                c->unk73 = 3;
                goto loop;
            }
            if ((s8)p->unk08 == 0)
                c->unk73 = 7;
            TaskYieldTrampoline(1);
        }
    case 7:
        {
            struct Task *d = gUnk_03002490;

            d->unk88->unk42 &= 0xFDFF;
            sub_0803e1b8(255, 0, d->unk88->unk00);
        }
        sub_08006138();
    case 0:
        gUnk_03002490->unk88->unk16 = 1;
        sub_08006338(0xE97);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 2;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 3;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 4;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 5;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 6;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        sub_080413a4(58);
        sub_0803e34c(178, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk88->unk16 = 7;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(17);
        sub_080413a4(59);
        gUnk_03002490->unk88->unk16 = 24;
        sub_08006338(0xEBA);
        while (!(gUnk_03002490->unk7A & 1))
            TaskYieldTrampoline(1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 27, 0);
        sub_080261d4(2);
        sub_0803e34c(179, gUnk_03002490->unk88->unk00);
        sub_0803e050(2);
        gUnk_03002490->unk88->unk16 = 25;
        sub_08006338(0xEBB);
        TaskYieldTrampoline(20);
        gUnk_03002490->unk88->unk16 = 255;
        gUnk_03002490->unk73 = 7;
        goto loop;
    case 1:
        gUnk_03002490->unk88->unk16 = 1;
        sub_08006338(0xE97);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 2;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 3;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 4;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 5;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 6;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        sub_08040b40(11, 66);
        sub_080413a4(60);
        sub_0803e34c(178, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk88->unk16 = 7;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(13);
        gUnk_03002490->unk88->unk16 = 8;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk88->unk16 = 9;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080413a4(61);
        gUnk_03002490->unk88->unk16 = 10;
        gUnk_03002490->unk3C++;
        while (!(gUnk_03002490->unk7A & 1))
            TaskYieldTrampoline(1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 27, 0);
        sub_080261d4(2);
        sub_0803e34c(179, gUnk_03002490->unk88->unk00);
        sub_0803e050(3);
        gUnk_03002490->unk88->unk16 = 11;
        sub_08006338(0xEA1);
        TaskYieldTrampoline(20);
        gUnk_03002490->unk88->unk16 = 255;
        sub_080413a4(56);
        gUnk_03002490->unk6C = 0;
        do {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gUnk_03002490->unk6C <= 4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk73 = 7;
        goto loop;
    case 3:
        gUnk_03002490->unk88->unk16 = 1;
        sub_08006338(0xE97);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 2;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 3;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 4;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 5;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 6;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        sub_080413a4(62);
        sub_0803e34c(178, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk88->unk16 = 7;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(11);
        gUnk_03002490->unk88->unk16 = 12;
        sub_08006338(0xEA8);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk88->unk16 = 13;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk88->unk16 = 14;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080413a4(63);
        do
        {
            gUnk_03002490->unk88->unk16 = 15;
            sub_08006338(0xEAB);
            TaskYieldTrampoline(1);
            if (gUnk_03002490->unk7A & 1)
                goto done3;
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 6; gUnk_03002490->unk6C++)
            {
                gUnk_03002490->unk88->unk16++;
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
                if (gUnk_03002490->unk7A & 1)
                    goto done3;
            }
        } while (!(gUnk_03002490->unk7A & 1));
    done3:
        sub_0805afac(gUnk_03002490->unk88->unk00, 27, 0);
        sub_080261d4(2);
        sub_0803e34c(179, gUnk_03002490->unk88->unk00);
        sub_0803e050(2);
        gUnk_03002490->unk88->unk16 = 23;
        sub_08006338(0xEB3);
        TaskYieldTrampoline(20);
        gUnk_03002490->unk88->unk16 = 255;
        sub_080413a4(56);
        gUnk_03002490->unk6C = 0;
        do {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gUnk_03002490->unk6C <= 4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk73 = 7;
        goto loop;
    case 4:
        gUnk_03002490->unk28 = 0;
        sub_08040b40(11, 67);
        gUnk_03002490->unk88->unk16 = 37;
        sub_08006338(0xECE);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 38;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 39;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 40;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 41;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        sub_0803e050(1);
        gUnk_03002490->unk88->unk16 = 42;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        sub_080413a4(64);
        sub_0803e34c(178, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk88->unk16 = 43;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(11);
        gUnk_03002490->unk88->unk16 = 44;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk88->unk16 = 45;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk88->unk16 = 46;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080413a4(65);
        gUnk_03002490->unk88->unk16 = 47;
        sub_08006338(0xED8);
        while (!(gUnk_03002490->unk7A & 1))
            TaskYieldTrampoline(1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 27, 0);
        sub_080261d4(2);
        sub_0803e34c(179, gUnk_03002490->unk88->unk00);
        sub_0803e050(3);
        gUnk_03002490->unk28++;
        gUnk_03002490->unk88->unk16 = 48;
        sub_08006338(0xED9);
        TaskYieldTrampoline(20);
        gUnk_03002490->unk88->unk16 = 255;
        sub_080413a4(57);
        gUnk_03002490->unk6C = 0;
        do {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gUnk_03002490->unk6C <= 7);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk73 = 7;
        goto loop;
    case 2:
        sub_08040b40(11, 68);
        gUnk_03002490->unk88->unk16 = 26;
        sub_08006338(0xEBD);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 27;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        sub_0803e050(1);
        gUnk_03002490->unk88->unk16 = 28;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        sub_08040b40(11, 69);
        sub_080413a4(66);
        sub_0803e34c(178, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk88->unk16 = 29;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk88->unk16 = 30;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk88->unk16 = 31;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(9);
        gUnk_03002490->unk88->unk16 = 32;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk88->unk16 = 33;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080413a4(67);
        gUnk_03002490->unk88->unk16 = 34;
        sub_08006338(0xEC5);
        while (!(gUnk_03002490->unk7A & 1))
            TaskYieldTrampoline(1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 27, 0);
        sub_080261d4(2);
        sub_0803e34c(179, gUnk_03002490->unk88->unk00);
        sub_0803e050(3);
        gUnk_03002490->unk88->unk16 = 35;
        sub_08006338(0xEC6);
        TaskYieldTrampoline(20);
        gUnk_03002490->unk88->unk16 = 36;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk88->unk16 = 255;
        sub_080413a4(56);
        gUnk_03002490->unk6C = 0;
        do {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gUnk_03002490->unk6C <= 4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk73 = 7;
        goto loop;
    case 5:
        sub_0803e050(3);
        sub_080413a4(68);
        gUnk_03002490->unk88->unk16 = 49;
        sub_08006338(0xEE4);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk88->unk16++;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk88->unk16++;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk88->unk16++;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080413a4(2);
        gUnk_03002490->unk88->unk16++;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16++;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16++;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16 = 254;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk88->unk16 = 255;
        gUnk_03002490->unk73 = 7;
        goto loop;
    }
    goto loop;
}

void sub_0804c4ac(void)
{
    switch (gUnk_03002490->unk73) {
    case 7:
        if (gUnk_03002490->unk7A & 1)
            sub_0803e050(2);
        sub_0804042c();
        return;
    case 4:
        {
            struct Task *t = gUnk_03002490;
            if (t->unk28 == 0 && !(t->unk7A & 1) && (s8)t->unk88->unk16 != -1) {
                s32 k, x, v;
                k = sub_0803f884();
                gUnk_03001F2C = k;
                if (k != 0) {
                    if (k == 1)
                        x = 0x300;
                    else
                        x = 0x100;
                } else {
                    x = 0x200;
                }
                v = x << 8;
                if (x & 0x8000)
                    v |= 0xFF000000;
                sub_080061c0(v, 0);
                {
                    struct Task *u = gUnk_03002490;
                    s32 w = x << 8;
                    if (x & 0x8000)
                        w |= 0xFF000000;
                    u->unk64 = w;
                }
            }
        }
        /* fallthrough */
    case 0:
    case 1:
    case 2:
    case 3:
    case 5:
        {
            struct Task *u = gUnk_03002490;
            struct PlayerState *p = u->unk88;
            if ((s8)p->unk16 >= 0 && (s8)p->unk08 == 0) {
                if (!(u->unk7A & 1)) {
                    sub_0803e050(2);
                    sub_080413a4(2);
                }
                sub_0803e050(1);
                gUnk_03002490->unk73 = 7;
                sub_08006148(sub_0804b858, gCurTaskIdx);
            }
        }
        break;
    }
    sub_0803f9c0();
    if (sub_0803fce4(0) != 0) {
        gUnk_03002490->unk88->unk01 = 23;
        gUnk_03002490->unk88->unk16 = 255;
        sub_0803e1b8(255, 0, gUnk_03002490->unk88->unk00);
    } else if (gUnk_03002490->unk7A & 1) {
        sub_0803e4ec(0);
    } else if ((s8)gUnk_03002490->unk88->unk16 == -1) {
        sub_0803f870();
        sub_08040b40(7, 72);
    }
}
