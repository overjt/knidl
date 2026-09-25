#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4a54c.c (0x0804A54C-0x0804AB6F, issue #88).
 *
 * Player action bodies, part 18: actions 47-48 and handlers 44-45.
 * sub_0804a54c (action 47, mode 13) is a linear yield script (animation
 * 0xBBD, M14's sub_08053940(player, 8, 0..2), three two-step loops over
 * 0xBBF/0xBC1/0xBC3); its handler sub_0804a6a0 lets M11's sub_0804042c
 * end it once Task.unk28 is set.  sub_0804a6bc (action 48, mode 13) is
 * a re-entrant four-state move: state 0 winds up (sound 152, velocity
 * preset 39 and animation 0xC53 on the ground, then 0xC40), state 1
 * holds animation 0xC48 until B is newly pressed after Task.unk28
 * frames, state 2 releases it (preset 42, 13 or 2 by ground and
 * variant, animation 0xC4D, effect 43 x4) and state 3 ends it.  Its
 * handler sub_0804a970 registers the collider gUnk_0873C264 in the air
 * or tests the block set gUnk_0873CF6C on the ground in state 0; in
 * state 1 it plays the landing (effect 27, sound 143, sub_080261d4(2))
 * and the terrain animation gUnk_0873B654[gUnk_03005550.unk4] on the
 * ground, or the rising presets 40/41 and the collider gUnk_0873C278 in
 * the air, and it stops a rise on a ceiling hit (gUnk_03005550.unk1). */

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

extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern u16 gUnk_030023C0[];             /* newly-pressed keys, latched per player */
extern struct Unk03005550 gUnk_03005550;
extern u32 gUnk_0873C264[];
extern u32 gUnk_0873C278[];
extern u32 gUnk_0873CF6C[];
extern s16 gUnk_0873B654[];

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006338(s32 a);
void sub_0800634c(s32 a);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_080261d4(u16 a);
u16 sub_08030848(struct HitBoxSet *p, s32 e);
void sub_0803e050(s32 a0);
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_0803e4a8(void);
s32 sub_0803e4ec(s32 a0);
s32 sub_0803e55c(void);
void sub_0803f870(void);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
void sub_08053940(s32 a, s32 b, s32 c);      /* M14, still asm */
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_0804a54c(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 44;
    gUnk_03002490->unk28 = 0;
    gUnk_03002490->unk80 = 0;
    sub_08006338(0xBBD);
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_08053940(gUnk_03002490->unk88->unk00, 8, 0);
    sub_08053940(gUnk_03002490->unk88->unk00, 8, 1);
    sub_08053940(gUnk_03002490->unk88->unk00, 8, 2);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08006338(0xBBF);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
    } while ((s16)++gUnk_03002490->unk6C <= 1);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08006338(0xBC1);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
    } while ((s16)++gUnk_03002490->unk6C <= 1);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08006338(0xBC3);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
    } while ((s16)++gUnk_03002490->unk6C <= 1);
    sub_08006338(0xBBF);
    TaskYieldTrampoline(4);
    gUnk_03002490->unk28++;
    sub_08006138();
}

void sub_0804a6a0(void)
{
    if (gUnk_03002490->unk28 != 0)
        sub_0804042c();
    sub_0803e55c();
}

void sub_0804a6bc(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 45;
    {
        struct Task *t = gUnk_03002490;
        if (t->unk88->unk05 != 13) {
            struct Task *u;
            t->unk73 = 0;
            u = gUnk_03002490;
            u->unk28 = 15;
            u->unk80 = 17;
        }
    }
    switch (gUnk_03002490->unk73) {
    case 0:
        sub_0803e34c(152, gUnk_03002490->unk88->unk00);
        if ((gUnk_03002458[gUnk_03002490->unk88->unk00] & 48) == 0)
            sub_0803e050(1);
        if (gUnk_03002490->unk7A & 1) {
            sub_080413a4(39);
            sub_08006338(0xC53);
            TaskYieldTrampoline(3);
        } else {
            sub_0803e050(2);
        }
        sub_08006338(0xC40);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C = 0;
        do {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gUnk_03002490->unk6C <= 3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk42 = 13;
        gUnk_03002490->unk73 = 1;
        /* fallthrough */
    case 1:
        sub_0803e050(1);
        sub_08006338(0xC48);
        while (1) {
            if (gUnk_03002490->unk28 == 0) {
                if (gUnk_030023C0[gUnk_03002490->unk88->unk00] & 2)
                    break;
            } else {
                gUnk_03002490->unk28--;
            }
            TaskYieldTrampoline(1);
        }
        gUnk_03002490->unk73 = 2;
        /* fallthrough */
    case 2:
        gUnk_03002490->unk42 = 7;
        {
            struct Task *t = gUnk_03002490;
            if (t->unk7A & 1) {
                sub_080413a4(42);
            } else if (t->unk7B & 1) {
                sub_080413a4(13);
            } else {
                sub_080413a4(2);
                gUnk_03002490->unk58 = gUnk_03002490->unk68;
            }
        }
        sub_08006338(0xC4D);
        TaskYieldTrampoline(2);
        sub_0805afac(gUnk_03002490->unk88->unk00, 43, 0);
        sub_0805afac(gUnk_03002490->unk88->unk00, 43, 1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 43, 2);
        sub_0805afac(gUnk_03002490->unk88->unk00, 43, 3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk6C = 0;
        do {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gUnk_03002490->unk6C <= 3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk73 = 3;
        sub_0803e1b8(255, 0, gUnk_03002490->unk88->unk00);
    }
    sub_08006138();
}

void sub_0804a970(void)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk73) {
    case 0:
        if ((t->unk7A & 1) == 0) {
            sub_0803f870();
            sub_08040b40(7, 72);
            sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                         gUnk_0873C264);
        } else {
            sub_0803e4ec(1);
            sub_08030848((struct HitBoxSet *)gUnk_0873CF6C, gUnk_03002490->unk88->unk00);
        }
        break;
    case 1:
        if (t->unk7A & 1) {
            if (t->unk58 != 0) {
                sub_0805afac(t->unk88->unk00, 27, 0);
                sub_0803e34c(143, gUnk_03002490->unk88->unk00);
                sub_080261d4(2);
                sub_0803e050(2);
            }
            if (gUnk_03005550.unk4 == 0)
                sub_08006338(0xC48);
            else
                sub_0800634c(gUnk_0873B654[gUnk_03005550.unk4]);
            {
                struct PlayerState *p = gUnk_03002490->unk88;
                if (p->unk3F != 2)
                    sub_0803e1b8(2, 0, p->unk00);
            }
            sub_08030848((struct HitBoxSet *)gUnk_0873CF6C, gUnk_03002490->unk88->unk00);
        } else {
            sub_08006338(0xC48);
            if (gUnk_03002490->unk7B & 1)
                sub_0803e4a8();
            if ((gUnk_03002490->unk7B & 1) == 0)
                sub_080413a4(40);
            else
                sub_080413a4(41);
            if (gUnk_03002490->unk58 >= 0) {
                struct PlayerState *p = gUnk_03002490->unk88;
                if (p->unk3F != 3)
                    sub_0803e1b8(3, 0, p->unk00);
                sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                             gUnk_0873C278);
            }
        }
        break;
    case 2:
        if (t->unk7A & 1)
            sub_0803e4ec(1);
        break;
    case 3:
        sub_0804042c();
        break;
    }
    if (gUnk_03002490->unk58 < 0 && gUnk_03005550.unk1 != 0)
        gUnk_03002490->unk58 = 0;
}
