#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_44d04.c (0x08044D04-0x080455C7, issue #87).
 *
 * Player action bodies, part 12: action 35 and per-frame handler 32.
 * sub_08044d04 (action 35, mode 13) is an attack with a ground form
 * (Task.unk73 = 0) and an air form (1), each in two variants picked by
 * Task.unk7B bit 0 (Task.unk30).  It installs the player's collider
 * record gUnk_020060E0[] (registered with M05's sub_0801a828) and
 * block hit-box set gUnk_02005550[] (tested by M09's sub_08030848) from
 * gUnk_0873BF28/gUnk_0873CCAC or gUnk_0873BF84/gUnk_0873CCFC, points
 * PlayerState.unk6C at the set while the swing is live and steps it
 * through the 8-byte rows of gUnk_0873CCB4 (ground) or gUnk_0873CD04 /
 * gUnk_0873CD44 (air, two passes, counters Task.unk6C/unk6E) with
 * sub_0803e5f8, with effects 31 and 28 and sounds 147/148; the air form
 * restores the facing Task.unk43 it saved in Task.unk34.  Its handler
 * sub_08045398 copies the collider row Task.unk2C of gUnk_0873BF3C
 * (ground) or gUnk_0873BF98/gUnk_0873BFD8 (air) with sub_0803e5c0 and
 * registers it every frame, requests action 23 through sub_0803fce4 and
 * picks the sub_08040b40 preset from the held left/right keys. */

/* M11's per-player records (src/stage_43654.c spells them the same way) */
struct M11R8 { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };

struct M11R20 { u32 w[5]; };

extern struct M11R20 gUnk_020060E0[];
extern struct M11R8 gUnk_02005550[];
extern u32 gUnk_0873BF28[];
extern u32 gUnk_0873CCAC[];
extern u32 gUnk_0873CCB4[];
extern u32 gUnk_0873BF84[];
extern u32 gUnk_0873CCFC[];
extern u32 gUnk_0873CD04[];
extern u32 gUnk_0873CD44[];
extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern u32 gUnk_0873BF3C[];
extern u32 gUnk_0873BF98[];
extern u32 gUnk_0873BFD8[];

void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006338(s32 a);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
s32 sub_0803e34c(s32 a0, u16 a1);
s32 sub_0803e4ec(s32 a0);
s32 sub_0803e5c0(s32 playerIdx, u8 *src6);
s32 sub_0803e5f8(s32 a0, s32 a1);
void sub_0803f9c0(void);
s32 sub_0803fce4(s32 a);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_08044d04(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 32;
    {
        struct Task *t = gUnk_03002490;
        t->unk28 = 0;
        t->unk2C = -1;
        if (t->unk7B & 1)
            t->unk30 = 1;
        else
            t->unk30 = 0;
    }
    gUnk_03002490->unk34 = gUnk_03002490->unk43;
    gUnk_03002490->unk80 = 4;
    if (gUnk_03002490->unk7A & 1)
        gUnk_03002490->unk73 = 0;
    else
        gUnk_03002490->unk73 = 1;
    switch (gUnk_03002490->unk73) {
    case 0:
        gUnk_020060E0[gUnk_03002490->unk88->unk00] = *(struct M11R20 *)gUnk_0873BF28;
        gUnk_02005550[gUnk_03002490->unk88->unk00] = *(struct M11R8 *)gUnk_0873CCAC;
        if (gUnk_03002490->unk30 == 0) {
            sub_08006338(0x4BA);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            sub_0805afac(gUnk_03002490->unk88->unk00, 31, 0);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            sub_0805afac(gUnk_03002490->unk88->unk00, 28, 3);
            sub_0803e34c(147, gUnk_03002490->unk88->unk00);
            {
                struct Task *t = gUnk_03002490;
                t->unk88->unk6C = &gUnk_02005550[t->unk88->unk00];
                t->unk2C++;
                sub_0803e5f8(t->unk88->unk00, (s32)((u8 *)gUnk_0873CCB4 + t->unk2C * 8));
            }
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk2C++;
            sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CCB4 + gUnk_03002490->unk2C * 8));
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk2C++;
            sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CCB4 + gUnk_03002490->unk2C * 8));
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk2C++;
            sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CCB4 + gUnk_03002490->unk2C * 8));
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk2C++;
            sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CCB4 + gUnk_03002490->unk2C * 8));
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk2C = -1;
            gUnk_03002490->unk88->unk6C = 0;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } else {
            sub_08006338(0x4CA);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            sub_0805afac(gUnk_03002490->unk88->unk00, 31, 0);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            sub_0805afac(gUnk_03002490->unk88->unk00, 28, 3);
            sub_0803e34c(147, gUnk_03002490->unk88->unk00);
            {
                struct Task *t = gUnk_03002490;
                t->unk88->unk6C = &gUnk_02005550[t->unk88->unk00];
                t->unk2C++;
                sub_0803e5f8(t->unk88->unk00, (s32)((u8 *)gUnk_0873CCB4 + t->unk2C * 8));
            }
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk2C++;
            sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CCB4 + gUnk_03002490->unk2C * 8));
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk2C++;
            sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CCB4 + gUnk_03002490->unk2C * 8));
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk2C++;
            sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CCB4 + gUnk_03002490->unk2C * 8));
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk2C++;
            sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CCB4 + gUnk_03002490->unk2C * 8));
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk2C++;
            gUnk_03002490->unk88->unk6C = 0;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk2C = -1;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
        }
        break;
    case 1:
        gUnk_020060E0[gUnk_03002490->unk88->unk00] = *(struct M11R20 *)gUnk_0873BF84;
        gUnk_02005550[gUnk_03002490->unk88->unk00] = *(struct M11R8 *)gUnk_0873CCFC;
        gUnk_03002490->unk88->unk6C = 0;
        if (gUnk_03002490->unk30 == 0) {
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 1; gUnk_03002490->unk6C++) {
                sub_0803e34c(148, gUnk_03002490->unk88->unk00);
                {
                    struct Task *t = gUnk_03002490;
                    t->unk88->unk6C = &gUnk_02005550[t->unk88->unk00];
                    t->unk2C = 0;
                    sub_0803e5f8(t->unk88->unk00, (s32)gUnk_0873CD04);
                }
                sub_08006338(0x4DA);
                TaskYieldTrampoline(1);
                for (gUnk_03002490->unk6E = 0; gUnk_03002490->unk6E <= 6; gUnk_03002490->unk6E++) {
                    gUnk_03002490->unk2C++;
                    sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CD04 + gUnk_03002490->unk2C * 8));
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(1);
                }
            }
            gUnk_03002490->unk2C = -1;
            gUnk_03002490->unk88->unk6C = 0;
            gUnk_03002490->unk43 = gUnk_03002490->unk34;
            sub_08006338(0x4E2);
            TaskYieldTrampoline(1);
        } else {
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 1; gUnk_03002490->unk6C++) {
                sub_0803e34c(148, gUnk_03002490->unk88->unk00);
                {
                    struct Task *t = gUnk_03002490;
                    t->unk88->unk6C = &gUnk_02005550[t->unk88->unk00];
                    t->unk2C = 0;
                    sub_0803e5f8(t->unk88->unk00, (s32)gUnk_0873CD44);
                }
                sub_08006338(0x4E3);
                TaskYieldTrampoline(1);
                for (gUnk_03002490->unk6E = 0; gUnk_03002490->unk6E <= 10; gUnk_03002490->unk6E++) {
                    gUnk_03002490->unk2C++;
                    sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CD44 + gUnk_03002490->unk2C * 8));
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(1);
                }
            }
            gUnk_03002490->unk2C = -1;
            gUnk_03002490->unk88->unk6C = 0;
            gUnk_03002490->unk43 = gUnk_03002490->unk34;
            sub_08006338(0x4EF);
            TaskYieldTrampoline(1);
        }
        break;
    }
    gUnk_03002490->unk28++;
    sub_08006138();
}

void sub_08045398(void)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk73) {
    case 0:
        if (t->unk28 != 0) {
            sub_0804042c();
        } else {
            if (t->unk58 > 0 && sub_0803fce4(0) != 0)
                gUnk_03002490->unk88->unk01 = 23;
            {
                struct Task *u = gUnk_03002490;
                if (u->unk2C != -1) {
                    sub_0803e5c0(u->unk88->unk00, (u8 *)gUnk_0873BF3C + u->unk2C * 8);
                    sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                                 (u8 *)gUnk_020060E0 + gUnk_03002490->unk88->unk00 * 20);
                }
            }
        }
        sub_08040b40(0, 72);
        break;
    case 1:
        if ((t->unk7A & 1) || t->unk28 != 0)
            sub_0804042c();
        else if (t->unk58 > 0 && sub_0803fce4(0) != 0)
            gUnk_03002490->unk88->unk01 = 23;
        {
            struct Task *u = gUnk_03002490;
            if (u->unk2C != -1) {
                if (u->unk30 == 0)
                    sub_0803e5c0(u->unk88->unk00, (u8 *)gUnk_0873BF98 + u->unk2C * 8);
                else
                    sub_0803e5c0(u->unk88->unk00, (u8 *)gUnk_0873BFD8 + u->unk2C * 8);
                sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                             (u8 *)gUnk_020060E0 + gUnk_03002490->unk88->unk00 * 20);
            }
        }
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48) {
            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 16) {
                if (gUnk_03002490->unk30 == 0)
                    sub_08040b40(10, 56);
                else
                    sub_08040b40(10, 58);
            } else {
                if (gUnk_03002490->unk30 == 0)
                    sub_08040b40(10, 57);
                else
                    sub_08040b40(10, 59);
            }
        } else {
            if (gUnk_03002490->unk30 == 0)
                sub_08040b40(11, 2);
            else
                sub_08040b40(11, 4);
        }
        break;
    }
    {
        struct Task *u = gUnk_03002490;
        if ((u->unk7A & 1) == 0) {
            if ((u->unk7B & 1) == 0)
                sub_080413a4(2);
            else
                sub_080413a4(13);
        } else {
            sub_0803e4ec(1);
        }
    }
    sub_0803f9c0();
}
