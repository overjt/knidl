#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_46330.c (0x08046330-0x08046BFF, issue #87).
 *
 * Player action bodies, part 15: action 39 and per-frame handler 36.
 * sub_08046330 (action 39, mode 13) is a re-entrant five-state machine
 * over Task.unk73: state 0 winds up (animation 0x701, effect 6, sound
 * 154), state 1 installs the script gUnk_0873CB34 and the block hit-box
 * set gUnk_0873CDAC and spins in an endless yield loop, state 2 turns
 * round (it negates the facing Task.unk43) and goes back to state 1,
 * state 3 finishes the move and state 4 bounces off (sound 153, the
 * screen shake sub_080261d4(4), velocity preset 36).  Its handler
 * sub_0804676c is what leaves the spin: every frame of state 1 it
 * re-binds the coroutine to state 3 on a newly-pressed B, to state 2
 * when the held direction opposes the facing, and to state 4 when the
 * collision block gUnk_03005550 reports a hit; it keeps the player on
 * slopes and ledges with M06's terrain probes sub_08022624 and
 * sub_08021c4c and registers the collider gUnk_0873BF14. */

/* gUnk_03005550: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh).  A 16-bit test of
   unk0/unk1 together is `*(u16 *)&gUnk_03005550` (good/sub_08045a50.c). */
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

extern u32 gUnk_0873CB1C[];
extern u32 gUnk_0873CB34[];
extern u32 gUnk_0873CDAC[];
extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern u16 gUnk_030023C0[];             /* newly-pressed keys, latched per player */
extern struct Unk03005550 gUnk_03005550;
extern u32 gUnk_0873BF14[];

void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_08006338(s32 a);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u8 sub_08021c4c(s16 x, s16 y);
s32 sub_08022624(u16 x, u16 y);
void sub_080261d4(u16 a);
void sub_0803e050(s32 a0);
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_0803e374(s32 a0, u16 a1);
void sub_0803e3ac(void);
s32 sub_0803e4ec(s32 a0);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_08046330(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 36;
    {
        struct Task *t = gUnk_03002490;
        if (t->unk88->unk05 != 13) {
            t->unk28 = 0;
            if (t->unk7A & 1)
                t->unk2C = 1;
            else
                t->unk2C = 0;
            gUnk_03002490->unk30 = 0;
            sub_0805afac(gUnk_03002490->unk88->unk00, 34, 0);
            gUnk_03002490->unk80 = 8;
            gUnk_03002490->unk73 = 0;
        }
    }
again:
    {
        struct Task *t = gUnk_03002490;
        t->unk88->unk6C = 0;
        switch (t->unk73) {
        case 0:
            sub_08040b40(11, 44);
            sub_08006338(0x701);
            TaskYieldTrampoline(4);
            sub_0805afac(gUnk_03002490->unk88->unk00, 6, 30);
            sub_08040b40(11, 45);
            gUnk_03002490->unk6C = 0;
            do {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
            } while ((s16)++gUnk_03002490->unk6C <= 7);
            sub_0803e374(154, gUnk_03002490->unk88->unk00);
            gUnk_03002490->unk73 = 1;
            sub_08040b40(11, 46);
            /* fallthrough */
        case 1:
            {
                struct Task *u = gUnk_03002490;
                u->unk88->unk68 = (u32)gUnk_0873CB34;
                u->unk88->unk6C = gUnk_0873CDAC;
                sub_0803e1b8(3, 0, u->unk88->unk00);
            }
            while (1) {
                if (gUnk_03002490->unk7A & 1 || gUnk_03002490->unk28 != 0)
                    sub_0805afac(gUnk_03002490->unk88->unk00, 34, 1);
                sub_08006338(0x70A);
                TaskYieldTrampoline(1);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
            }
        case 2:
            sub_0803e1b8(255, 0, gUnk_03002490->unk88->unk00);
            sub_0803e34c(245, gUnk_03002490->unk88->unk00);
            if (gUnk_03002490->unk7A & 1 || gUnk_03002490->unk28 != 0) {
                sub_0805afac(gUnk_03002490->unk88->unk00, 6, 4);
                sub_08006338(0x70E);
                TaskYieldTrampoline(4);
                sub_08040b40(11, 47);
                gUnk_03002490->unk43 = -gUnk_03002490->unk43;
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(3);
                sub_08040b40(11, 48);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                sub_08040b40(11, 49);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(3);
                sub_0803e050(1);
            } else {
                sub_08040b40(11, 50);
                sub_08006338(0x70E);
                TaskYieldTrampoline(4);
                gUnk_03002490->unk43 = -gUnk_03002490->unk43;
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(3);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(3);
            }
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            {
                struct Task *u = gUnk_03002490;
                if (u->unk7A & 1 || u->unk28 != 0) {
                    sub_0805afac(u->unk88->unk00, 6, 4);
                    sub_08040b40(11, 46);
                }
            }
            gUnk_03002490->unk73 = 1;
            goto again;
        case 3:
            sub_0803e3ac();
            {
                struct Task *u = gUnk_03002490;
                u->unk88->unk68 = (u32)gUnk_0873CB1C;
                sub_0803e1b8(255, 0, u->unk88->unk00);
            }
            sub_08040b40(11, 51);
            sub_08006338(0x702);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C = 0;
            do {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
            } while ((s16)++gUnk_03002490->unk6C <= 6);
            sub_08006338(0x701);
            gUnk_03002490->unk73 = 5;
            break;
        case 4:
            {
                struct Task *u = gUnk_03002490;
                u->unk88->unk68 = (u32)gUnk_0873CB1C;
                sub_0803e1b8(255, 0, u->unk88->unk00);
            }
            sub_0803e3ac();
            sub_0803e34c(153, gUnk_03002490->unk88->unk00);
            sub_080261d4(4);
            gUnk_03002490->unk7A = 0;
            sub_08040b40(11, 17);
            sub_080413a4(36);
            while (1) {
                sub_08006338(0x702);
                TaskYieldTrampoline(1);
                gUnk_03002490->unk6C = 0;
                do {
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(1);
                } while ((s16)++gUnk_03002490->unk6C <= 6);
            }
        }
    }
    sub_08006138();
}

void sub_0804676c(void)
{
    {
        struct Task *t = gUnk_03002490;
        if (t->unk30 == 0) {
            if (gUnk_03005550.unkD != 0) {
                t->unk88->unk14 = 5;
                t->unk30 = 1;
            } else {
                t->unk88->unk14 = 0;
            }
        } else {
            struct PlayerState *p = t->unk88;
            if ((s16)p->unk14 == 0) {
                if (sub_08022624(t->unk48, (t->unk4A & ~15) + 16) != 0)
                    gUnk_03002490->unk7A = 1;
            } else {
                p->unk14--;
            }
        }
    }
    {
        struct Task *t = gUnk_03002490;
        if (t->unk7A & 1) {
            t->unk30 = 0;
            t->unk88->unk14 = 0;
        }
    }
    switch (gUnk_03002490->unk73) {
    case 0:
    case 3:
        if (gUnk_03002490->unk7A & 1) {
            sub_0803e4ec(1);
            sub_08040b40(0, 72);
        } else {
            sub_080413a4(2);
            sub_08040b40(11, 2);
        }
        if (gUnk_03005550.unk0 == 0)
            break;
        {
            struct Task *t = gUnk_03002490;
            if (t->unk73 == 0)
                t->unk54 = 0;
            else
                sub_0803e050(1);
        }
        break;
    case 1:
        {
            u16 k = gUnk_030023C0[gUnk_03002490->unk88->unk00] & 2;
            struct Task *t = gUnk_03002490;
            if (k) {
                t->unk73 = 3;
                sub_08006148(sub_08046330, gCurTaskIdx);
            } else {
                if (t->unk7A & 1) {
                    t->unk28 = 0;
                    sub_08040b40(11, 46);
                } else if (t->unk28 == 0) {
                    if (sub_08021c4c(t->unk48, t->unk4A + 15) != 0) {
                        {
                            struct Task *u = gUnk_03002490;
                            if (u->unk7B & 1)
                                u->unk4A -= 8;
                        }
                        sub_0803e050(2);
                        {
                            struct Task *u = gUnk_03002490;
                            u->unk50 = ((u->unk4A & 0xFFF0) + 5) << 16;
                            u->unk4A = u->unk50 >> 16;
                            u->unk28 = 1;
                        }
                    } else {
                        if (gUnk_03002490->unk88->unk48 & 3)
                            sub_0803e050(1);
                        sub_08040b40(11, 50);
                    }
                } else {
                    sub_08040b40(11, 46);
                }
                if ((gUnk_03002458[gUnk_03002490->unk88->unk00] & 16 && gUnk_03002490->unk43 == -1)
                    || (gUnk_03002458[gUnk_03002490->unk88->unk00] & 32 && gUnk_03002490->unk43 == 1)) {
                    gUnk_03002490->unk73 = 2;
                    sub_08006148(sub_08046330, gCurTaskIdx);
                } else if (*(u16 *)&gUnk_03005550 != 0
                           || ((gUnk_03002490->unk88->unk48 & 3)
                               && ((gUnk_03002490->unk7A & 1) || gUnk_03002490->unk28 != 0))) {
                    if (gUnk_03005550.unk1 != 0)
                        gUnk_03002490->unk58 = 0;
                    else
                        sub_0803e050(1);
                    gUnk_03002490->unk73 = 4;
                    sub_08006148(sub_08046330, gCurTaskIdx);
                }
            }
        }
        {
            struct Task *t = gUnk_03002490;
            if ((t->unk7A & 1) || t->unk28 != 0) {
                sub_0803e050(2);
            } else {
                if (t->unk2C != 0)
                    sub_0803e050(2);
                sub_080413a4(2);
            }
        }
        sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A, gUnk_0873BF14);
        break;
    case 2:
        {
            u16 k = gUnk_030023C0[gUnk_03002490->unk88->unk00] & 2;
            if (k) {
                gUnk_03002490->unk73 = 3;
                sub_08006148(sub_08046330, gCurTaskIdx);
                break;
            }
            if (*(u16 *)&gUnk_03005550 != 0 || (gUnk_03002490->unk88->unk48 != 0 && (gUnk_03002490->unk7A & 1))) {
                if (gUnk_03005550.unk1 != 0)
                    gUnk_03002490->unk58 = 0;
                else
                    sub_0803e050(1);
                gUnk_03002490->unk73 = 4;
                sub_08006148(sub_08046330, gCurTaskIdx);
            }
        }
        if (gUnk_03002490->unk7A & 1) {
            gUnk_03002490->unk2C = 1;
            if (gUnk_03002490->unk58 != 0)
                sub_0803e050(2);
        } else {
            gUnk_03002490->unk2C = 0;
            sub_080413a4(2);
        }
        break;
    case 4:
        {
            struct Task *t = gUnk_03002490;
            if (t->unk58 < 0) {
                if (gUnk_03005550.unk1 != 0)
                    t->unk58 = 0;
                break;
            }
        }
        /* fallthrough */
    case 5:
        sub_0804042c();
        {
            struct PlayerState *p = gUnk_03002490->unk88;
            if (p->unk01 == 2)
                p->unk01 = 4;
        }
        return;
    }
    if (gUnk_03002490->unk7B & 1) {
        sub_0803e3ac();
        gUnk_03002490->unk88->unk01 = 23;
    }
    {
        struct Task *t = gUnk_03002490;
        if (t->unk7A & 1)
            t->unk2C = 1;
        else
            t->unk2C = 0;
    }
}
