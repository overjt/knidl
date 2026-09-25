#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_37ed8.c (0x08037ED8-0x0803919B, issue #91).
 *
 * Player action body, part 6: action 16 and per-frame handler 16.
 * sub_08037ed8 (action 16, mode 17, 4368 bytes) is the twin of M11's
 * sub_08042580: a goto loop around a seven-state switch over Task.unk73.
 * State 5, the entry, picks the next state from Task.unk82 (its low
 * nibble, or 4 when bit 7 is set) and, for a player holding an ability
 * (PlayerState.unk0D) with bit 1 of PlayerState.unk42 clear, releases it
 * through M17's sub_08064eb8(PlayerState.unk30) and M02's HUD
 * (sub_0800a008); states 0-4 play the ability's animations and state 6
 * leaves.  sub_08038fe8, handler 16, steers every state into state 6 and
 * re-binds the coroutine. */

extern u16 gUnk_03002360;
extern u16 gUnk_03001EA4;

void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_08006338(s32 a);
void sub_0800634c(s32 a);
void sub_08006364(s32 a);
s32 sub_0800a008(s32 a, s32 b, u32 c);
void sub_0800a130(s32 a, s32 id);
void sub_080261d4(u16 a);
void sub_0803e050(s32 a0);
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_0803e4a8(void);
void sub_0803f9c0(void);
s32 sub_0803fa44(void);
s32 sub_0803fce4(s32 a);
void sub_08040710(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);
s32 sub_08064eb8(u8 p2);

void sub_08037ed8(void)
{
    struct Task *t;
    struct PlayerState *p;
    struct Task *u;
    s32 anim;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 17;
    gUnk_03002490->unk15 = 16;
    t = gUnk_03002490;
    p = t->unk88;
    if (p->unk05 != 17)
    {
        p->unk42 &= 0xFEEF;
        t->unk73 = 5;
        if (gUnk_03002490->unk88->unk06 == 2)
            gUnk_03002490->unk88->unk06 = 0;
    }
loop:
    switch (gUnk_03002490->unk73)
    {
    case 5:
        gUnk_03002490->unk28 = 0;
        gUnk_03002490->unk88->unk3D = 0;
        if (gUnk_03002490->unk88->unk37 == 2 || gUnk_03002490->unk88->unk37 == 3)
        {
            gUnk_03002490->unk73 = gUnk_03002490->unk82 & 15;
        }
        else
        {
            if (gUnk_03002490->unk88->unk0D == 11)
            {
                gUnk_03002490->unk88->unk42 &= 0xFFFD;
                sub_0800a008(0, -1, gUnk_03002490->unk88->unk00);
            }
            u = gUnk_03002490;
            if (u->unk82 & 128)
                u->unk73 = 4;
            else
                u->unk73 = u->unk82 & 15;
            if (!(gUnk_03002490->unk88->unk42 & 2) && gUnk_03002490->unk88->unk37 == 0)
            {
                if (gUnk_03002490->unk88->unk0D != 0)
                {
                    gUnk_03002490->unk88->unk42 &= 0xFFFB;
                    sub_08064eb8(gUnk_03002490->unk88->unk30);
                    sub_0800a008(0, -1, gUnk_03002490->unk88->unk00);
                }
            }
            else
            {
                sub_0800a130(gUnk_03002490->unk88->unk0D, gUnk_03002490->unk88->unk00);
            }
            if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
                sub_080261d4(2);
        }
        gUnk_03002490->unk88->unk3F = 1;
        gUnk_03002490->unk88->unk12 = 0x8000;
        goto loop;
    case 0:
        sub_0803e050(2);
        if (gUnk_03002490->unk88->unk06 == 1)
        {
            sub_0803e34c(158, gUnk_03002490->unk88->unk00);
            if (!(gUnk_03002490->unk7B & 1))
                anim = 0x171;
            else
                anim = 0x19B;
            if ((s8)gUnk_03002490->unk7D == 0)
                sub_08040b40(10, 24);
            else
                sub_08040b40(10, 26);
            sub_08006338(anim + 1);
            TaskYieldTrampoline(3);
            if ((s8)gUnk_03002490->unk7D == 0)
                sub_08040b40(10, 25);
            else
                sub_08040b40(10, 27);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(5);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
        }
        else
        {
            if (gUnk_03001EA4 & 2)
                sub_0803e34c(111, gUnk_03002490->unk88->unk00);
            else
                sub_0803e34c(112, gUnk_03002490->unk88->unk00);
            switch (gUnk_03002490->unk88->unk0D)
            {
            case 0:
            default:
                if (!(gUnk_03002490->unk7B & 1))
                    anim = 243;
                else
                    anim = 0x11D;
                break;
            case 4:
                anim = 0x4A1;
                break;
            case 25:
                anim = 0x1036;
                break;
            }
            if (((s8)gUnk_03002490->unk7D == 0 && gUnk_03002490->unk43 == -1)
                || ((s8)gUnk_03002490->unk7D == 4 && gUnk_03002490->unk43 == 1))
            {
                if ((s8)gUnk_03002490->unk7D == 0)
                    sub_08040b40(10, 18);
                else
                    sub_08040b40(10, 21);
                sub_08006338(anim);
                TaskYieldTrampoline(3);
                if ((s8)gUnk_03002490->unk7D == 0)
                    sub_08040b40(10, 19);
                else
                    sub_08040b40(10, 22);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                if ((s8)gUnk_03002490->unk7D == 0)
                    sub_08040b40(10, 20);
                else
                    sub_08040b40(10, 23);
                for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
                {
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(3);
                }
            }
            else
            {
                if ((s8)gUnk_03002490->unk7D == 0)
                    sub_08040b40(10, 18);
                else
                    sub_08040b40(10, 21);
                if (gUnk_03002490->unk43 == 1)
                {
                    sub_08006364(anim);
                    TaskYieldTrampoline(3);
                }
                else
                {
                    sub_0800634c(anim);
                    TaskYieldTrampoline(3);
                }
                if ((s8)gUnk_03002490->unk7D == 0)
                    sub_08040b40(10, 19);
                else
                    sub_08040b40(10, 22);
                sub_08006338(anim + 8);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                if ((s8)gUnk_03002490->unk7D == 0)
                    sub_08040b40(10, 20);
                else
                    sub_08040b40(10, 23);
                for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
                {
                    gUnk_03002490->unk3C--;
                    TaskYieldTrampoline(2);
                }
            }
        }
        sub_0803e050(1);
        gUnk_03002490->unk73 = 6;
        goto loop;
    case 1:
    case 2:
        if (gUnk_03002490->unk28 == 0)
        {
            if (gUnk_03002490->unk73 == 1)
            {
                if (gUnk_03002490->unk88->unk06 == 0)
                    sub_0803e34c(0x125, gUnk_03002490->unk88->unk00);
                else
                    sub_0803e34c(0x126, gUnk_03002490->unk88->unk00);
            }
            else if (gUnk_03002490->unk88->unk06 == 0)
            {
                if (gUnk_03001EA4 & 2)
                    sub_0803e34c(111, gUnk_03002490->unk88->unk00);
                else
                    sub_0803e34c(112, gUnk_03002490->unk88->unk00);
            }
            else
            {
                sub_0803e34c(158, gUnk_03002490->unk88->unk00);
            }
            if ((s8)gUnk_03002490->unk7D == 0)
                sub_08040b40(10, 28);
            else
                sub_08040b40(10, 30);
            sub_080413a4(24);
        }
        gUnk_03002490->unk88->unk14 = 120;
        if (gUnk_03002490->unk88->unk06 == 0)
        {
            switch (gUnk_03002490->unk28)
            {
            case 0:
                gUnk_03002490->unk28 = 1;
                if (gUnk_03002490->unk73 == 1)
                {
                    sub_0805afac(gUnk_03002490->unk88->unk00, 22, 0);
                    while (1)
                    {
                        sub_08006338(252);
                        TaskYieldTrampoline(2);
                        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 6; gUnk_03002490->unk6C++)
                        {
                            gUnk_03002490->unk3C++;
                            TaskYieldTrampoline(2);
                        }
                    }
                }
                sub_0805afac(gUnk_03002490->unk88->unk00, 23, 0);
                while (1)
                {
                    sub_08006338(260);
                    TaskYieldTrampoline(4);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(4);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                }
            case 1:
                if (gUnk_03002490->unk73 == 1)
                    sub_0805afac(gUnk_03002490->unk88->unk00, 22, 1);
                else
                    sub_0805afac(gUnk_03002490->unk88->unk00, 23, 1);
                sub_0805afac(gUnk_03002490->unk88->unk00, 25, 0);
                gUnk_03002490->unk28++;
                sub_08006338(264);
                TaskYieldTrampoline(3);
                if ((s8)gUnk_03002490->unk7D == 0)
                    sub_08040b40(10, 29);
                else
                    sub_08040b40(10, 31);
                sub_080413a4(25);
                while (1)
                {
                    sub_08006338(0x109);
                    TaskYieldTrampoline(3);
                    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 6; gUnk_03002490->unk6C++)
                    {
                        gUnk_03002490->unk3C++;
                        TaskYieldTrampoline(3);
                    }
                }
            case 2:
                gUnk_03002490->unk28++;
                sub_08006338(43);
                TaskYieldTrampoline(2);
                sub_080413a4(26);
                for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 6; gUnk_03002490->unk6C++)
                {
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                }
                gUnk_03002490->unk88->unk14 = 1;
                break;
            default:
                gUnk_03002490->unk88->unk14 = 1;
                break;
            }
        }
        else
        {
            switch (gUnk_03002490->unk28)
            {
            case 0:
                gUnk_03002490->unk28++;
                if (gUnk_03002490->unk73 == 1)
                {
                    sub_0805afac(gUnk_03002490->unk88->unk00, 22, 0);
                    while (1)
                    {
                        sub_08006338(372);
                        TaskYieldTrampoline(2);
                        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 6; gUnk_03002490->unk6C++)
                        {
                            gUnk_03002490->unk3C++;
                            TaskYieldTrampoline(2);
                        }
                    }
                }
                sub_0805afac(gUnk_03002490->unk88->unk00, 23, 0);
                while (1)
                {
                    sub_08006338(380);
                    TaskYieldTrampoline(2);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                }
            case 1:
                if (gUnk_03002490->unk73 == 1)
                    sub_0805afac(gUnk_03002490->unk88->unk00, 22, 1);
                else
                    sub_0805afac(gUnk_03002490->unk88->unk00, 23, 1);
                sub_0805afac(gUnk_03002490->unk88->unk00, 25, 0);
                gUnk_03002490->unk28++;
                sub_08006338(0x183);
                TaskYieldTrampoline(3);
                if ((s8)gUnk_03002490->unk7D == 0)
                    sub_08040b40(10, 29);
                else
                    sub_08040b40(10, 31);
                sub_080413a4(25);
                while (1)
                {
                    sub_08006338(0x181);
                    TaskYieldTrampoline(4);
                    gUnk_03002490->unk3C--;
                    TaskYieldTrampoline(2);
                    sub_08006338(386);
                    TaskYieldTrampoline(4);
                    sub_08006338(384);
                    TaskYieldTrampoline(2);
                }
            case 2:
                gUnk_03002490->unk28++;
                sub_08006338(0x183);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
                sub_080413a4(26);
                while (1)
                {
                    sub_08006338(0x183);
                    TaskYieldTrampoline(2);
                    sub_08006338(0x181);
                    TaskYieldTrampoline(4);
                    gUnk_03002490->unk3C--;
                    TaskYieldTrampoline(2);
                    sub_08006338(386);
                    TaskYieldTrampoline(4);
                    sub_08006338(384);
                    TaskYieldTrampoline(2);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(1);
                }
            case 3:
                gUnk_03002490->unk28++;
                sub_08006338(0x183);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                sub_08006338(384);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk88->unk14 = 1;
                break;
            case 4:
            default:
                gUnk_03002490->unk88->unk14 = 1;
                break;
            }
        }
        break;
    case 3:
        if (gUnk_03002490->unk28 == 0)
        {
            if (gUnk_03002490->unk88->unk06 == 0)
                sub_0803e34c(121, gUnk_03002490->unk88->unk00);
            else
                sub_0803e34c(0x127, gUnk_03002490->unk88->unk00);
            if ((s8)gUnk_03002490->unk7D == 0)
                sub_08040b40(10, 28);
            else
                sub_08040b40(10, 30);
            sub_080413a4(24);
        }
        gUnk_03002490->unk88->unk14 = 120;
        if (gUnk_03002490->unk88->unk06 == 0)
        {
            switch (gUnk_03002490->unk28)
            {
            case 0:
                gUnk_03002490->unk28 = 1;
                sub_08006338(0x111);
                TaskYieldTrampoline(4);
                for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
                {
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(4);
                }
                gUnk_03002490->unk3C++;
                sub_0805afac(gUnk_03002490->unk88->unk00, 24, 0);
                break;
            case 1:
                gUnk_03002490->unk28 = 2;
                sub_08006338(280);
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(4);
                sub_08006338(0x117);
                TaskYieldTrampoline(48);
                sub_0805afac(gUnk_03002490->unk88->unk00, 26, 0);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                sub_080413a4(26);
                gUnk_03002490->unk88->unk14 = 120;
            case 2:
                gUnk_03002490->unk28++;
                sub_08006338(250);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                sub_08006338(244);
                TaskYieldTrampoline(3);
                for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
                {
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                }
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(4);
                gUnk_03002490->unk88->unk14 = 1;
                break;
            default:
                gUnk_03002490->unk88->unk14 = 1;
                break;
            }
        }
        else
        {
            switch (gUnk_03002490->unk28)
            {
            case 0:
                gUnk_03002490->unk28++;
                sub_08006338(0x185);
                TaskYieldTrampoline(4);
                for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
                {
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(4);
                }
                gUnk_03002490->unk3C++;
                break;
            case 1:
                gUnk_03002490->unk28++;
                sub_080413a4(27);
                gUnk_03002490->unk88->unk14 = 120;
            case 2:
                gUnk_03002490->unk28++;
                sub_08006338(0x191);
                TaskYieldTrampoline(2);
                sub_08006338(0x18B);
                TaskYieldTrampoline(16);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(4);
                sub_08006338(0x18B);
                TaskYieldTrampoline(20);
                sub_08006338(0x191);
                TaskYieldTrampoline(4);
                sub_08006338(0x18B);
                TaskYieldTrampoline(2);
                sub_08006338(402);
                TaskYieldTrampoline(4);
                sub_08006338(0x18B);
                TaskYieldTrampoline(2);
                sub_08006338(0x191);
                TaskYieldTrampoline(2);
                sub_08006338(0x18B);
                TaskYieldTrampoline(1);
                sub_08006338(402);
                TaskYieldTrampoline(2);
                sub_080413a4(28);
                gUnk_03002490->unk88->unk14 = 120;
            case 3:
                gUnk_03002490->unk28++;
                sub_08006338(398);
                TaskYieldTrampoline(2);
                sub_0805afac(gUnk_03002490->unk88->unk00, 26, 0);
                sub_08006338(0x193);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                if (gUnk_03002490->unk7A & 1)
                {
                    sub_080413a4(29);
                    gUnk_03002490->unk88->unk14 = 120;
                    sub_08006338(410);
                    while (!(gUnk_03002490->unk7A & 1))
                        TaskYieldTrampoline(1);
                }
            case 4:
                gUnk_03002490->unk88->unk14 = 1;
                break;
            }
        }
        break;
    case 4:
        if (gUnk_03002490->unk88->unk06 == 0)
        {
            if (gUnk_03001EA4 & 2)
                sub_0803e34c(111, gUnk_03002490->unk88->unk00);
            else
                sub_0803e34c(112, gUnk_03002490->unk88->unk00);
        }
        else
        {
            sub_0803e34c(158, gUnk_03002490->unk88->unk00);
        }
        sub_0803e050(3);
        gUnk_03002490->unk7A = 0;
        if (gUnk_03002490->unk88->unk06 == 0)
        {
            sub_080413a4(30);
            if (!(gUnk_03002490->unk7B & 1))
            {
                sub_08006338(244);
                TaskYieldTrampoline(2);
            }
            else
            {
                sub_08006338(0x11D);
                TaskYieldTrampoline(2);
            }
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 6; gUnk_03002490->unk6C++)
            {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
            }
        }
        else
        {
            sub_080413a4(31);
            if (!(gUnk_03002490->unk7B & 1))
            {
                sub_08006338(370);
                TaskYieldTrampoline(2);
            }
            else
            {
                sub_08006338(412);
                TaskYieldTrampoline(2);
            }
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(4);
        }
        gUnk_03002490->unk73 = 6;
        goto loop;
    case 6:
        if (gUnk_03002490->unk7B & 1)
            sub_0803e4a8();
        sub_0803e1b8(1, 96, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk88->unk42 |= 0x200;
        break;
    }
    sub_08006138();
}

void sub_08038fe8(void)
{
    struct Task *t;

    switch (gUnk_03002490->unk73)
    {
    case 0:
        if (sub_0803fce4(0) != 0)
        {
            gUnk_03002490->unk73 = 6;
            sub_08006148(sub_08037ed8, gCurTaskIdx);
        }
        break;
    case 1:
    case 2:
        if (gUnk_03002490->unk58 > 0)
            sub_080413a4(2);
        if (sub_0803fce4(0) == 0)
        {
            if (sub_0803fa44() != 0)
                goto e1;
            t = gUnk_03002490;
            if (--t->unk88->unk14 != 0)
                break;
            t->unk73 = 6;
            sub_08006148(sub_08037ed8, gCurTaskIdx);
            break;
        }
        gUnk_03002490->unk73 = 6;
        sub_08006148(sub_08037ed8, gCurTaskIdx);
        break;
    case 3:
        if (gUnk_03002490->unk58 > 0)
            sub_080413a4(2);
        if (sub_0803fce4(0) == 0)
        {
            t = gUnk_03002490;
            if (t->unk58 != 0 && (t->unk7A & 1))
            {
                if (t->unk88->unk06 != 1)
                {
                    if (t->unk28 <= 2)
                    {
                        sub_0803e050(3);
                        sub_08006148(sub_08037ed8, gCurTaskIdx);
                        break;
                    }
                }
                else if (t->unk28 <= 1)
                    goto e3;
            }
            t = gUnk_03002490;
            if (--t->unk88->unk14 != 0)
                break;
            if (t->unk88->unk06 != 1)
            {
                if (t->unk28 <= 2)
                    goto s2;
                t->unk73 = 6;
                sub_08006148(sub_08037ed8, gCurTaskIdx);
                break;
            }
            if (t->unk28 <= 3)
                goto s3;
            t->unk73 = 6;
            sub_08006148(sub_08037ed8, gCurTaskIdx);
            break;
        }
        /* An empty loop (a compiled-out macro?): its NOTE_INSN_LOOP_END
           stops the first cse pass from following the sub_0803fce4 jump
           into this block, so the task-address reload here is not chained
           to case 3's PRE copy; without it that copy outlives gcse's
           r4 pseudo and takes r5 (push {r4, r5}). */
        do
        {
        } while (0);
        gUnk_03002490->unk73 = 6;
        sub_08006148(sub_08037ed8, gCurTaskIdx);
        break;
    case 4:
        if (sub_0803fce4(0) != 0)
        {
            gUnk_03002490->unk73 = 6;
            sub_08006148(sub_08037ed8, gCurTaskIdx);
        }
        sub_08040b40(7, 72);
        break;
    e1:
        sub_0803e050(1);
        sub_08006148(sub_08037ed8, gCurTaskIdx);
        break;
    e3:
        sub_0803e050(3);
        sub_08006148(sub_08037ed8, gCurTaskIdx);
        break;
    s2:
        t->unk28 = 2;
        sub_08006148(sub_08037ed8, gCurTaskIdx);
        break;
    s3:
        t->unk28 = 3;
        sub_08006148(sub_08037ed8, gCurTaskIdx);
        break;
    case 6:
        sub_08040710();
        break;
    }
    sub_0803f9c0();
}
