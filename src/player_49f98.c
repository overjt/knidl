#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_49f98.c (0x08049F98-0x0804A54B, issue #88).
 *
 * Player action bodies, part 17: action 46 and per-frame handler 43.
 * sub_08049f98 (action 46, mode 13) is a five-state move whose states
 * fall into each other; a fresh entry starts in state 0, or in state 4
 * when it comes from mode 5.  State 0 starts it (effect 42, animation
 * 0xB2D), state 1 points PlayerState.unk6C at the block hit-box set
 * gUnk_0873CF5C with effects 42 x4 and sound 174 and runs velocity
 * presets 38, 0 and 1, and states 3-4 hand the player over to mode 5
 * with handler 7 (animation 0xAD2, then the ability's loop from
 * gUnk_0873D3B8[ability][1]).  Its handler sub_0804a258
 * flashes the palette gUnk_081F59F0 (the VRAM transfer queue
 * sub_080017e4) in state 1, re-binds state 3 on a newly-pressed B after
 * the PlayerState.unk14 frames, registers the collider gUnk_0873C228,
 * steers with the held left/right keys, picks one of five animation
 * rows 0xB2E-0xB3E by |Task.unk54| and cycles Task.unk46 through them,
 * ends the move on landing and re-binds state 4 on a ceiling hit. */

/* gUnk_03005550: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh).  A 16-bit test of
   unk0/unk1 together is `*(u16 *)&gUnk_03005550` (M12's sub_08045a50). */
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

extern u32 gUnk_0873CF5C[];
extern s16 gUnk_0873D3B8[][2];
extern u16 gUnk_03001EA4;
extern u16 gUnk_081F59F0[];
extern u32 gUnk_0873C228[];
extern u8 gUnk_03001470[];              /* OBJ palette buffer (M11 spelling) */
extern u16 gUnk_030023C0[];             /* newly-pressed keys, latched per player */
extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern struct Unk03005550 gUnk_03005550;

void TaskYieldTrampoline(s32 frames);
void sub_080017e4(u32 mode, void *src, void *dst, u32 size);   /* early_1518; effect_5afac's pointer spelling */
void sub_08006148(void *a, u32 i);
void sub_08006338(s32 a);
void sub_0800634c(s32 a);
void sub_08006364(s32 a);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_0803e050(s32 a0);
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
void sub_0803e374(s32 a0, u16 a1);
void sub_0803e3ac(void);
s32 sub_0803e448(void);
s32 sub_0803e4ec(s32 a0);
void sub_0803e650(s32 a0);
void sub_0803f8e8(void);
s32 sub_0803ff7c(void);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_08049f98(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 43;
    {
        struct Task *t = gUnk_03002490;
        if (t->unk88->unk05 != 13) {
            if (t->unk88->unk05 == 5)
                t->unk73 = 4;
            else
                t->unk73 = 0;
            gUnk_03002490->unk80 = 15;
        }
    }
    switch (gUnk_03002490->unk73) {
    case 0:
        sub_0805afac(gUnk_03002490->unk88->unk00, 42, 5);
        sub_0803e050(2);
        sub_0803e650(5);
        sub_08006338(0xB2D);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk73 = 1;
        gUnk_03002490->unk88->unk14 = 4;
        /* fallthrough */
    case 1:
        sub_080413a4(38);
        sub_0803e1b8(3, 0, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk88->unk6C = gUnk_0873CF5C;
        sub_0803e374(174, gUnk_03002490->unk88->unk00);
        sub_0805afac(gUnk_03002490->unk88->unk00, 42, 0);
        sub_0805afac(gUnk_03002490->unk88->unk00, 42, 1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 42, 2);
        sub_0805afac(gUnk_03002490->unk88->unk00, 42, 4);
        gUnk_03002490->unk46 = 0;
        gUnk_03002490->unk28 = 2;
        TaskYieldTrampoline(23);
        sub_080413a4(0);
        TaskYieldTrampoline(10);
        gUnk_03002490->unk73 = 2;
        /* fallthrough */
    case 2:
        {
            struct Task *t = gUnk_03002490;
            t->unk88->unk42 &= 0xFFEF;
            sub_0803e1b8(255, 0, t->unk88->unk00);
        }
        TaskYieldTrampoline(13);
        sub_080413a4(1);
        TaskYieldTrampoline(5);
        gUnk_03002490->unk73 = 3;
        /* fallthrough */
    case 3:
        gUnk_03002490->unk88->unk42 &= 0xFFEF;
        sub_080413a4(2);
        gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
        gUnk_03002490->unk88->unk04 = 5;
        gUnk_03002490->unk15 = 7;
        gUnk_03002490->unk88->unk6C = 0;
        gUnk_03002490->unk73 = 4;
        gUnk_03002490->unk88->unk14 = 300;
        sub_08006338(0xAD2);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C = 0;
        do {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gUnk_03002490->unk6C <= 4);
        /* fallthrough */
    case 4:
        {
            struct Task *t = gUnk_03002490;
            struct PlayerState *p;
            t->unk88->unk42 &= 0xFFEF;
            p = t->unk88;
            if (p->unk04 != 5) {
                p->unk05 = p->unk04;
                gUnk_03002490->unk88->unk04 = 5;
                gUnk_03002490->unk15 = 7;
                gUnk_03002490->unk88->unk6C = 0;
                sub_080413a4(2);
            }
        }
        {
            struct Task *t = gUnk_03002490;
            t->unk88->unk14 = 30;
            t->unk46 = gUnk_0873D3B8[t->unk88->unk0D][1];
        }
        while (1) {
            sub_08006338(gUnk_03002490->unk46);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
    }
}

void sub_0804a258(void)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk73) {
    case 1:
        t->unk88->unk42 &= 0xFFEF;
        if ((gUnk_03001EA4 & 7) <= 3) {
            sub_080017e4(2, gUnk_081F59F0, gUnk_03001470 + (t->unk40 >> 12) * 32, 64);
            gUnk_03002490->unk88->unk42 |= 16;
        }
        /* fallthrough */
    case 2:
        {
            struct Task *u = gUnk_03002490;
            struct PlayerState *p = u->unk88;
            if ((s16)p->unk14 == 0) {
                if (gUnk_030023C0[p->unk00] & 2) {
                    u->unk73 = 3;
                    sub_08006148(sub_08049f98, gCurTaskIdx);
                    sub_0803e1b8(255, 0, gUnk_03002490->unk88->unk00);
                    sub_0803e3ac();
                    sub_0805afac(gUnk_03002490->unk88->unk00, 42, 3);
                    sub_0803e050(2);
                }
            } else {
                p->unk14--;
            }
        }
        /* fallthrough */
    case 3:
        {
            struct Task *u = gUnk_03002490;
            if (u->unk28-- == 0) {
                u->unk46 = (u->unk46 + 1) & 3;
                u->unk28 = 2;
            }
        }
        sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A, gUnk_0873C228);
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48) {
            sub_0803e448();
            sub_08040b40(11, 60);
        } else {
            sub_08040b40(11, 61);
        }
        if (gUnk_03005550.unk0 != 0)
            sub_0803e050(1);
        {
            struct Task *u = gUnk_03002490;
            if ((u8)(u->unk73 - 1) <= 1) {
                s32 a;
                if (abs(u->unk54) <= 0x4000)
                    a = 0xB2E;
                else if (abs(u->unk54) <= 0x10000)
                    a = 0xB32;
                else if (abs(u->unk54) <= 0x14000)
                    a = 0xB36;
                else if (abs(u->unk54) <= 0x1C000)
                    a = 0xB3A;
                else
                    a = 0xB3E;
                {
                    struct Task *w = gUnk_03002490;
                    if (w->unk54 == 0)
                        sub_08006338((s16)(w->unk46 + a));
                    else if (w->unk54 < 0)
                        sub_08006364(w->unk46 + a);
                    else
                        sub_0800634c((s16)(w->unk46 + a));
                }
            }
        }
        {
            struct Task *w = gUnk_03002490;
            if (w->unk7A & 1) {
                sub_0803f8e8();
                sub_0803e4ec(1);
                sub_0804042c();
                break;
            }
            if (w->unk58 < 0 && (gUnk_03005550.unk1 != 0 || (w->unk88->unk48 & 4))) {
                w->unk58 = 0;
                w->unk73 = 4;
                sub_08006148(sub_08049f98, gCurTaskIdx);
                sub_0803e1b8(255, 0, gUnk_03002490->unk88->unk00);
                sub_0803e3ac();
                return;
            }
        }
        sub_0803ff7c();
        break;
    }
    {
        struct PlayerState *p = gUnk_03002490->unk88;
        if (p->unk01 != 0)
            p->unk42 &= 0xFFEF;
    }
}
