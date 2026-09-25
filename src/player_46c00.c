#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_46c00.c (0x08046C00-0x080474E7, issue #87).
 *
 * Player action bodies, part 16: action 40 and per-frame handler 37.
 * sub_08046c00 (action 40, mode 13) is sub_08044d04's sibling: the same
 * opening, a ground form (Task.unk73 = 0) and an air form (1) in two
 * variants (Task.unk30 = Task.unk7B bit 0), the collider record
 * gUnk_0873C060 and the block hit-box sets gUnk_0873CDB4 (ground) /
 * gUnk_0873CDF4 (air) whose rows gUnk_0873CDBC, gUnk_0873CDFC and
 * gUnk_0873CE64 it steps with sub_0803e5f8.  On the ground it probes the
 * metatile 20 pixels ahead (sub_0802259c); a solid one (bits 0-1) gives
 * the impact: sound 241, the screen shake sub_080261d4(2) and effect 35.
 * Its handler sub_08047270 is sub_08045398's twin with the collider rows
 * gUnk_0873C074, gUnk_0873C0C0 and gUnk_0873C128, and in the air it
 * records the held left/right direction in Task.unk34. */

/* M11's per-player records (src/stage_43654.c spells them the same way) */
struct M11R8 { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };

struct M11R20 { u32 w[5]; };

extern struct M11R20 gUnk_020060E0[];
extern struct M11R8 gUnk_02005550[];
extern u32 gUnk_0873C060[];
extern u32 gUnk_0873CDB4[];
extern u32 gUnk_0873CDBC[];
extern u32 gUnk_0873CDF4[];
extern u32 gUnk_0873CDFC[];
extern u32 gUnk_0873CE64[];
extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern u32 gUnk_0873C074[];
extern u32 gUnk_0873C0C0[];
extern u32 gUnk_0873C128[];

void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006338(s32 a);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 sub_0802259c(u16 x, u16 y);
void sub_080261d4(u16 a);
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

void sub_08046c00(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 37;
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
    gUnk_03002490->unk80 = 9;
    if (gUnk_03002490->unk7A & 1)
        gUnk_03002490->unk73 = 0;
    else
        gUnk_03002490->unk73 = 1;
    switch (gUnk_03002490->unk73) {
    case 0:
        sub_0803e34c(130, gUnk_03002490->unk88->unk00);
        gUnk_020060E0[gUnk_03002490->unk88->unk00] = *(struct M11R20 *)gUnk_0873C060;
        gUnk_02005550[gUnk_03002490->unk88->unk00] = *(struct M11R8 *)gUnk_0873CDB4;
        gUnk_03002490->unk88->unk6C = &gUnk_02005550[gUnk_03002490->unk88->unk00];
        sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)gUnk_0873CDBC);
        if (gUnk_03002490->unk30 == 0) {
            gUnk_03002490->unk2C++;
            sub_08006338(0x7D2);
            TaskYieldTrampoline(8);
            sub_0805afac(gUnk_03002490->unk88->unk00, 28, 3);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++) {
                gUnk_03002490->unk2C++;
                sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CDBC + gUnk_03002490->unk2C * 8));
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
            }
            gUnk_03002490->unk2C++;
            sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CDBC + gUnk_03002490->unk2C * 8));
            {
                u16 x;
                s16 r;
                if (gUnk_03002490->unk43 == 1)
                    x = gUnk_03002490->unk48 + 20;
                else
                    x = gUnk_03002490->unk48 - 20;
                r = sub_0802259c(x, gUnk_03002490->unk4A + 13);
                if (r & 3) {
                    sub_0803e34c(241, gUnk_03002490->unk88->unk00);
                    sub_080261d4(2);
                    sub_0805afac(gUnk_03002490->unk88->unk00, 35, 0);
                    sub_0805afac(gUnk_03002490->unk88->unk00, 35, 1);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(1);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(1);
                    gUnk_03002490->unk3C--;
                    TaskYieldTrampoline(1);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(8);
                } else {
                    sub_08006338(0x7D9);
                    TaskYieldTrampoline(11);
                }
            }
            gUnk_03002490->unk2C = -1;
            gUnk_03002490->unk88->unk6C = 0;
            sub_08006338(0x7DD);
            TaskYieldTrampoline(2);
            sub_08006338(0x7DA);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
        } else {
            gUnk_03002490->unk2C++;
            sub_08006338(0x7DE);
            TaskYieldTrampoline(10);
            sub_0805afac(gUnk_03002490->unk88->unk00, 28, 3);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 3; gUnk_03002490->unk6C++) {
                gUnk_03002490->unk2C++;
                sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CDBC + gUnk_03002490->unk2C * 8));
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
            }
            gUnk_03002490->unk2C++;
            sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CDBC + gUnk_03002490->unk2C * 8));
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk2C++;
            sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CDBC + gUnk_03002490->unk2C * 8));
            {
                u16 x;
                s16 r;
                if (gUnk_03002490->unk43 == 1)
                    x = gUnk_03002490->unk48 + 20;
                else
                    x = gUnk_03002490->unk48 - 20;
                r = sub_0802259c(x, gUnk_03002490->unk4A + 13);
                if (r & 3) {
                    sub_0803e34c(241, gUnk_03002490->unk88->unk00);
                    sub_080261d4(2);
                    sub_0805afac(gUnk_03002490->unk88->unk00, 35, 0);
                    sub_0805afac(gUnk_03002490->unk88->unk00, 35, 1);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(1);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(1);
                    gUnk_03002490->unk3C--;
                    TaskYieldTrampoline(1);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(10);
                } else {
                    sub_08006338(0x7E5);
                    TaskYieldTrampoline(13);
                }
            }
            gUnk_03002490->unk2C = -1;
            gUnk_03002490->unk88->unk6C = 0;
            sub_08006338(0x7E9);
            TaskYieldTrampoline(4);
            sub_08006338(0x7E6);
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
        }
        break;
    case 1:
        sub_0803e34c(131, gUnk_03002490->unk88->unk00);
        gUnk_020060E0[gUnk_03002490->unk88->unk00] = *(struct M11R20 *)gUnk_0873C060;
        gUnk_02005550[gUnk_03002490->unk88->unk00] = *(struct M11R8 *)gUnk_0873CDF4;
        gUnk_03002490->unk88->unk6C = &gUnk_02005550[gUnk_03002490->unk88->unk00];
        sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)gUnk_0873CDFC);
        if (gUnk_03002490->unk30 == 0) {
            gUnk_03002490->unk2C++;
            sub_08006338(0x7EA);
            TaskYieldTrampoline(1);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 10; gUnk_03002490->unk6C++) {
                gUnk_03002490->unk2C++;
                sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CDFC + gUnk_03002490->unk2C * 8));
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
            }
            gUnk_03002490->unk2C++;
            sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CDFC + gUnk_03002490->unk2C * 8));
            gUnk_03002490->unk43 = gUnk_03002490->unk34;
            sub_08006338(0x7F6);
            TaskYieldTrampoline(1);
        } else {
            gUnk_03002490->unk2C++;
            sub_08006338(0x7F7);
            TaskYieldTrampoline(1);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 14; gUnk_03002490->unk6C++) {
                gUnk_03002490->unk2C++;
                sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CE64 + gUnk_03002490->unk2C * 8));
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
            }
            gUnk_03002490->unk2C++;
            sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CE64 + gUnk_03002490->unk2C * 8));
            gUnk_03002490->unk43 = gUnk_03002490->unk34;
            sub_08006338(0x807);
            TaskYieldTrampoline(1);
        }
        break;
    }
    gUnk_03002490->unk28++;
    sub_08006138();
}

void sub_08047270(void)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk73) {
    case 0:
        if (t->unk28 != 0)
            sub_0804042c();
        else if (t->unk58 > 0 && sub_0803fce4(0) != 0)
            gUnk_03002490->unk88->unk01 = 23;
        {
            struct Task *u = gUnk_03002490;
            if (u->unk2C != -1) {
                sub_0803e5c0(u->unk88->unk00, (u8 *)gUnk_0873C074 + u->unk2C * 8);
                sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                             (u8 *)gUnk_020060E0 + gUnk_03002490->unk88->unk00 * 20);
            }
        }
        sub_08040b40(0, 72);
        break;
    case 1:
        if (gUnk_03002458[t->unk88->unk00] & 48) {
            if (gUnk_03002458[t->unk88->unk00] & 16)
                t->unk34 = 1;
            else
                t->unk34 = -1;
        }
        {
            struct Task *u = gUnk_03002490;
            if ((u->unk7A & 1) || u->unk28 != 0) {
                u->unk88->unk6C = 0;
                sub_0804042c();
            } else if (u->unk58 > 0 && sub_0803fce4(0) != 0) {
                gUnk_03002490->unk88->unk01 = 23;
            }
        }
        {
            struct Task *u = gUnk_03002490;
            if (u->unk2C != -1) {
                if (u->unk30 == 0)
                    sub_0803e5c0(u->unk88->unk00, (u8 *)gUnk_0873C0C0 + u->unk2C * 8);
                else
                    sub_0803e5c0(u->unk88->unk00, (u8 *)gUnk_0873C128 + u->unk2C * 8);
                sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                             (u8 *)gUnk_020060E0 + gUnk_03002490->unk88->unk00 * 20);
            }
        }
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48) {
            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 16) {
                if (gUnk_03002490->unk30 == 0)
                    sub_08040b40(10, 52);
                else
                    sub_08040b40(10, 54);
            } else {
                if (gUnk_03002490->unk30 == 0)
                    sub_08040b40(10, 53);
                else
                    sub_08040b40(10, 55);
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
