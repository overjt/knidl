#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_455c8.c (0x080455C8-0x08045D33, issue #87).
 *
 * Player action bodies, part 13: actions 36-37 and per-frame handlers
 * 33-34.  sub_080455c8 (action 36, mode 13) is a four-state machine over
 * Task.unk73: state 0 starts the move (velocity preset 34, effect 32,
 * animation 0x5CB) and installs the attack box gUnk_0873CCA4 in
 * PlayerState.unk6C, state 1 swaps in the scripts gUnk_0873CB2C /
 * gUnk_0873BD3C (PlayerState.unk68/unk64) and cycles the hit-box row
 * Task.unk2C through 0-2, state 2 restores the default scripts
 * gUnk_0873CB1C / gUnk_0873BD00 and lands (preset 2), and state 3 is the
 * bounce-off (sound 153, sub_080261d4(4), preset 23).  Its handler
 * sub_08045a50 drives it from the collision block gUnk_03005550 -
 * re-binding state 3 on a hit, fading the palette of gUnk_0873B510[]
 * row Task.unk2C and registering the box gUnk_0873BF00.
 * sub_08045c40 (action 37) is a linear script (animations
 * 0x658/0x65A, M14's sub_08053940, sound 172); its handler sub_08045d18
 * waits for it to finish. */

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

/* gUnk_0873B510[]: a palette fade, src/dst palettes and the blend step */
struct M12Fade
{
    /*0x00*/ u16 *unk0;
    /*0x04*/ u16 *unk4;
    /*0x08*/ s32 unk8;
};

extern u32 gUnk_0873CB1C[];
extern u32 gUnk_0873BD00[];             /* stored to PlayerState.unk64 as (u32)gUnk_0873BD00 */
extern u32 gUnk_0873CCA4[];
extern u32 gUnk_0873CB2C[];
extern u32 gUnk_0873BD3C[];
extern struct Unk03005550 gUnk_03005550;
extern struct M12Fade gUnk_0873B510[];
extern u8 gUnk_03001470[];              /* OBJ palette buffer (M11 spelling) */
extern u32 gUnk_0873BF00[];

void TaskYieldTrampoline(s32 frames);
void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_08006338(s32 a);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_080261d4(u16 a);
void sub_0803e050(s32 a0);
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_0803e374(s32 a0, u16 a1);
void sub_0803e3ac(void);
s32 sub_0803e55c(void);
s32 sub_0803fce4(s32 a);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
void sub_08053940(s32 a, s32 b, s32 c);      /* M14, still asm */
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_080455c8(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 33;
    {
        struct Task *t = gUnk_03002490;
        if (t->unk88->unk05 != 13) {
            t->unk73 = 0;
            gUnk_03002490->unk80 = 5;
            gUnk_03002490->unk28 = 0;
        }
    }
    switch (gUnk_03002490->unk73) {
    case 0:
        gUnk_03002490->unk7A = 0;
        sub_08040b40(11, 38);
        sub_080413a4(34);
        sub_0805afac(gUnk_03002490->unk88->unk00, 32, 2);
        sub_0805afac(gUnk_03002490->unk88->unk00, 32, 0);
        sub_08006338(0x5CB);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        sub_0803e050(2);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk88->unk6C = gUnk_0873CCA4;
        sub_08040b40(11, 39);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_08040b40(11, 40);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk73 = 1;
        /* fallthrough */
    case 1:
        sub_0803e374(137, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk7A = 0;
        gUnk_03002490->unk88->unk68 = (u32)gUnk_0873CB2C;
        sub_0803e1b8(3, 0, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk88->unk64 = (u32)gUnk_0873BD3C;
        gUnk_03002490->unk88->unk42 |= 16;
        gUnk_03002490->unk6C = 0;
        gUnk_03002490->unk2C = 0;
        sub_08006338(0x5CF);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 32, 3);
        sub_0805afac(gUnk_03002490->unk88->unk00, 32, 4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C = 0;
        gUnk_03002490->unk2C = 1;
        sub_08006338(0x5D3);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C = 0;
        gUnk_03002490->unk2C = 2;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C = 0;
        gUnk_03002490->unk2C = 0;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C = 0;
        gUnk_03002490->unk2C = 1;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C = 0;
        gUnk_03002490->unk2C = 2;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C = 0;
        gUnk_03002490->unk2C = 0;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C = 0;
        gUnk_03002490->unk2C = 1;
        sub_08006338(0x5DF);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk2C = -1;
        gUnk_03002490->unk88->unk42 &= 0xFFEF;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_0803e3ac();
        gUnk_03002490->unk73 = 2;
        /* fallthrough */
    case 2:
        gUnk_03002490->unk88->unk6C = 0;
        sub_0803e1b8(255, 0, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk88->unk68 = (u32)gUnk_0873CB1C;
        gUnk_03002490->unk88->unk64 = (u32)gUnk_0873BD00;
        sub_08040b40(11, 41);
        sub_080413a4(2);
        sub_0805afac(gUnk_03002490->unk88->unk00, 32, 5);
        sub_0805afac(gUnk_03002490->unk88->unk00, 32, 6);
        sub_0805afac(gUnk_03002490->unk88->unk00, 32, 7);
        sub_0805afac(gUnk_03002490->unk88->unk00, 32, 8);
        sub_08006338(0x53C);
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
        sub_08006338(0x544);
        gUnk_03002490->unk73 = 4;
        break;
    case 3:
        gUnk_03002490->unk88->unk6C = 0;
        sub_0803e1b8(255, 0, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk88->unk42 &= 0xFFEF;
        sub_0803e3ac();
        gUnk_03002490->unk7A = 0;
        sub_0803e34c(153, gUnk_03002490->unk88->unk00);
        sub_080261d4(4);
        sub_08040b40(11, 17);
        sub_080413a4(23);
        sub_08006338(0x544);
        break;
    }
    sub_08006138();
}

void sub_08045a50(void)
{
    switch (gUnk_03002490->unk73) {
    case 0:
        gUnk_03002490->unk7A = 0;
        {
            struct Task *t = gUnk_03002490;
            if (t->unk58 < 0 && gUnk_03005550.unk1 != 0)
                t->unk58 = 0;
        }
        if (gUnk_03005550.unk0 == 0)
            break;
        sub_0803e050(1);
        {
            s8 d = gUnk_03002490->unk43;
            if ((d == 1 && gUnk_03005550.unk0 == 1)
                || (d == -1 && gUnk_03005550.unk0 == 2))
                gUnk_03002490->unk28 = 1;
        }
        break;
    case 1:
        {
            struct Task *t = gUnk_03002490;
            if (t->unk7A & 1) {
                if (gUnk_03005550.unk4 != 0) {
                    t->unk88->unk01 = 2;
                    sub_08040b40(11, 41);
                    sub_0803e1b8(255, 0, gUnk_03002490->unk88->unk00);
                }
            } else if (*(u16 *)&gUnk_03005550 != 0 || t->unk28 != 0
                       || (t->unk88->unk48 & 11) != 0) {
                gUnk_03002490->unk73 = 3;
                sub_08006148(sub_080455c8, gCurTaskIdx);
            }
        }
        {
            struct Task *t = gUnk_03002490;
            if (t->unk2C != -1) {
                struct M12Fade *f = &gUnk_0873B510[t->unk2C];
                t->unk6E += f->unk8;
                if (t->unk6E > 255)
                    t->unk6E = 256;
                sub_08003014(f->unk0, f->unk4, (u16)gUnk_03002490->unk6E, 16,
                             (u16 *)(gUnk_03001470 + ((gUnk_03002490->unk40 >> 12) << 5)));
            }
        }
        sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                     gUnk_0873BF00);
        break;
    case 2:
        sub_0803e55c();
        break;
    case 3:
    case 4:
        sub_0804042c();
        break;
    }
    if (sub_0803fce4(0) != 0)
        gUnk_03002490->unk88->unk01 = 23;
    {
        struct PlayerState *p = gUnk_03002490->unk88;
        if (p->unk01 != 0)
            p->unk42 &= 0xFFEF;
    }
}

void sub_08045c40(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 34;
    gUnk_03002490->unk28 = 0;
    gUnk_03002490->unk80 = 0;
    sub_08006338(0x658);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    sub_08006338(0x65A);
    TaskYieldTrampoline(2);
    sub_08053940(gUnk_03002490->unk88->unk00, 6, 0);
    sub_0803e34c(172, gUnk_03002490->unk88->unk00);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 15);
    sub_08006338(0x658);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk28++;
    sub_08006138();
}

void sub_08045d18(void)
{
    if (gUnk_03002490->unk28 != 0)
        sub_0804042c();
    sub_0803e55c();
}
