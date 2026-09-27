#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4ab70.c (0x0804AB70-0x0804B5B3, issue #88).
 *
 * Player action bodies, part 19: actions 50-51 and handlers 47-48.
 * sub_0804ab70 (action 50, mode 13) is a rolling move: sound 150, then
 * the velocity presets 63-65 of M11's sub_08040b40 over three states
 * that fall into each other (animations 0xDC5/0xDDD/0xDDB-0xDDC by the
 * facing, effects 28 and 45 x3).  Its handler sub_0804ada8 jumps on B
 * (preset 50) or falls (49) in state 1, turns the player round at a
 * wall (sub_0803e448 or the collision block gUnk_03005550: Task.unk54
 * and unk5C negated, and the facing on a block hit, with an 8-frame
 * lock in Task.unk28), stops a rise on a ceiling hit, registers the
 * collider gUnk_0873C2A0 and requests action 23 through sub_0803fce4.
 * sub_0804af54 (action 51, mode 13) is a screen-wide blast with the
 * stage frozen (gUnk_03001F34 = 1): it switches the DISPCNT shadow
 * gDispCnt to windowed BG1-BG3, remembers the height Task.unk50 in
 * Task.unk2C, shakes the screen (sub_080261d4(5), sub_08027204(2)),
 * flashes the player's palette gUnk_080DC628[player] towards
 * gUnk_082030B8 twice (BlendColors, presets 51/52), waits for the
 * blast task through PlayerState.unk16 and TaskSetSkipMask, restores the
 * default script gUnk_0873CB1C and resets the HUD ability panel
 * (sub_0800a008(0, -1, player)).  Its handler sub_0804b474 fades the
 * palette in and back out (gUnk_08203098, Task.unk28 in steps of 10 and
 * 16) and keeps the player under the height Task.unk2C. */

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
extern struct Unk03005550 gUnk_03005550;
extern u32 gUnk_0873C2A0[];
extern u8 gUnk_03001F34;
extern vu16 gDispCnt;              /* DISPCNT shadow */
extern u16 gUnk_080DC628[][16];         /* per-player palettes (M03 spelling) */
extern u8 gObjPalette[];              /* OBJ palette buffer (M11 spelling) */
extern u16 gUnk_082030B8[];
extern u32 gUnk_0873CB1C[];
extern u16 gUnk_08203098[];

void TaskYieldTrampoline(s32 frames);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlaySfx(s32 id);
void TaskSetSkipMask(u8 val, s32 idx);
void TaskSleepForever(void);
void TaskSetFrame(s32 a);
s32 sub_0800a008(s32 a, s32 b, u32 c);       /* landed (hud_099fc.c); M12 calls it as sub_0800a008(0, -1, p->unk00) */
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_080261d4(u16 a);
void sub_08027204(u32 a);
void sub_0803e050(s32 a0);
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_0803e3e4(s32 a0);
s32 sub_0803e448(void);
void sub_0803e4a8(void);
s32 sub_0803fa44(void);
s32 sub_0803fce4(s32 a);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_0804ab70(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 47;
    gCurTask->unk28 = 0;
    gCurTask->unk2C = 0;
    gCurTask->unk73 = 0;
    gCurTask->unk80 = 19;
    switch (gCurTask->unk73) {
    case 0:
        sub_0803e34c(150, gCurTask->unk88->unk00);
        sub_0803e1b8(3, 0, gCurTask->unk88->unk00);
        sub_08040b40(11, 63);
        gCurTask->unk3E &= 0x7FFF;
        TaskSetFrame(0xDC5);
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 10);
        gCurTask->unk73 = 1;
        /* fallthrough */
    case 1:
        sub_0805afac(gCurTask->unk88->unk00, 28, 0);
        gCurTask->unk88->unk16 = 0;
        sub_08040b40(11, 64);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C = 0xDDD;
            TaskYieldTrampoline(2);
            gCurTask->unk6E = 0;
            do {
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            } while (++gCurTask->unk6E <= 6);
        } while ((s16)++gCurTask->unk6C <= 5);
        gCurTask->unk73 = 2;
        /* fallthrough */
    case 2:
        gCurTask->unk88->unk16 = 255;
        sub_0805afac(gCurTask->unk88->unk00, 45, 0);
        sub_0805afac(gCurTask->unk88->unk00, 45, 1);
        sub_0805afac(gCurTask->unk88->unk00, 45, 2);
        sub_08040b40(11, 65);
        {
            struct Task *t = gCurTask;
            if (t->unk43 == 1) {
                t->unk3C = 0xDDC;
                TaskYieldTrampoline(2);
            } else {
                t->unk3C = 0xDDB;
                TaskYieldTrampoline(2);
            }
        }
        sub_0803e1b8(255, 0, gCurTask->unk88->unk00);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C -= 2;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 10);
        TaskSetFrame(0xDE5);
        TaskYieldTrampoline(1);
        gCurTask->unk73 = 3;
    }
    TaskSleepForever();
}

void sub_0804ada8(void)
{
    switch (gCurTask->unk73) {
    case 1:
        if (gUnk_03002458[gCurTask->unk88->unk00] & 2) {
            gCurTask->unk7A = 0;
            sub_080413a4(50);
        } else if ((gCurTask->unk7A & 1) == 0) {
            sub_080413a4(49);
        }
        {
            struct Task *u = gCurTask;
            if (u->unk2C-- == 0) {
                sub_0803e34c(149, u->unk88->unk00);
                gCurTask->unk2C = 3;
            }
        }
        goto common;
    case 0:
    case 2:
        if ((gCurTask->unk7A & 1) == 0)
            sub_080413a4(49);
    common:
        {
            struct Task *u = gCurTask;
            if (u->unk28 == 0) {
                if (sub_0803e448() != 0) {
                    struct Task *v = gCurTask;
                    v->unk28 = 8;
                    v->unk54 = -v->unk54;
                    v->unk5C = -v->unk5C;
                }
            } else {
                u->unk28--;
            }
        }
        if (gUnk_03005550.unk0 != 0) {
            gCurTask->unk28 = 8;
            gCurTask->unk43 = -gCurTask->unk43;
            {
                struct Task *v = gCurTask;
                v->unk54 = -v->unk54;
                v->unk5C = -v->unk5C;
            }
        }
        {
            struct Task *w = gCurTask;
            if (w->unk58 < 0 && gUnk_03005550.unk1 != 0)
                w->unk58 = 0;
        }
        if (gCurTask->unk73 != 2)
            sub_0801a828(gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A,
                         gUnk_0873C2A0);
        break;
    case 3:
        sub_0804042c();
        break;
    }
    if (gCurTask->unk7A & 1)
        sub_0803fa44();
    if (gCurTask->unk7B != 0 && sub_0803fce4(0) != 0) {
        sub_0803e4a8();
        gCurTask->unk88->unk01 = 23;
        gCurTask->unk88->unk16 = 255;
    }
}

void sub_0804af54(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 48;
    gUnk_03001F34 = 1;
    gCurTask->unk80 = 20;
    sub_0803e050(3);
    sub_0803e3e4(15);
    if ((gDispCnt & 0x400) == 0) {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
    }
    sub_0803e1b8(3, 0, gCurTask->unk88->unk00);
    gCurTask->unk88->unk42 |= 0x700;
    gCurTask->unk2C = gCurTask->unk50;
    sub_0805afac(gCurTask->unk88->unk00, 46, 0);
    gCurTask->unk88->unk68 = 0;
    gCurTask->unk73 = 0;
    sub_080261d4(5);
    sub_08027204(2);
    PlaySfx(248);
    gCurTask->unk6C = 0;
    do {
        TaskSetFrame(0xDE7);
        TaskYieldTrampoline(4);
        TaskSetFrame(0xDFA);
        TaskYieldTrampoline(4);
    } while ((s16)++gCurTask->unk6C <= 1);
    gCurTask->unk6C = 0;
    do {
        TaskSetFrame(0xDE7);
        TaskYieldTrampoline(2);
        TaskSetFrame(0xDFA);
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 3);
    TaskSetFrame(0xDE8);
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    sub_080413a4(51);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk28 = 0;
    gCurTask->unk88->unk42 |= 16;
    gCurTask->unk6C = 0;
    do {
        BlendColors(gUnk_080DC628[gCurTask->unk88->unk00], gUnk_082030B8,
                     (u16)gCurTask->unk28, 16,
                     (u16 *)(gObjPalette + ((gCurTask->unk40 >> 12) << 5)));
        gCurTask->unk28 += 85;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 2);
    TaskYieldTrampoline(3);
    gCurTask->unk88->unk42 &= 0xFFEF;
    TaskYieldTrampoline(1);
    sub_0803e050(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    sub_080413a4(52);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk28 = 0;
    gCurTask->unk88->unk42 |= 16;
    gCurTask->unk6C = 0;
    do {
        BlendColors(gUnk_080DC628[gCurTask->unk88->unk00], gUnk_082030B8,
                     (u16)gCurTask->unk28, 16,
                     (u16 *)(gObjPalette + ((gCurTask->unk40 >> 12) << 5)));
        gCurTask->unk28 += 85;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 2);
    gCurTask->unk88->unk42 &= 0xFFEF;
    TaskYieldTrampoline(9);
    sub_0803e050(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    sub_0805afac(gCurTask->unk88->unk00, 46, 1);
    sub_080413a4(53);
    TaskSetFrame(0xDE7);
    TaskYieldTrampoline(3);
    gCurTask->unk88->unk42 |= 16;
    gCurTask->unk73 = 1;
    gCurTask->unk28 = 0;
    gCurTask->unk88->unk16 = 1;
    {
        /* a second pseudo for the task-pointer address (lesson 3.291) */
        struct Task **c = &gCurTask;

        gCurTask->unk6C = 0;
        do {
            TaskSetFrame(0xDE7);
            TaskYieldTrampoline(2);
            TaskSetFrame(0xDEF);
            TaskYieldTrampoline(2);
            gCurTask->unk6E = 0;
            do {
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            } while (++gCurTask->unk6E <= 9);
        } while ((s16)++gCurTask->unk6C <= 1);
        if ((s8)(*c)->unk88->unk16 == 0) {
            TaskSetFrame(0xDE7);
            TaskYieldTrampoline(2);
            TaskSetFrame(0xDEF);
            TaskYieldTrampoline(2);
            (*c)->unk6E = 0;
            do {
                (*c)->unk3C++;
                TaskYieldTrampoline(2);
            } while (++(*c)->unk6E <= 9);
        } else {
            TaskSetSkipMask(2, gCurTaskIdx);
            do {
                if ((s8)(*c)->unk88->unk16 == 0) {
                    TaskSetSkipMask(0, gCurTaskIdx);
                    (*c)->unk88->unk16 = 2;
                }
                TaskSetFrame(0xDE7);
                TaskYieldTrampoline(2);
                TaskSetFrame(0xDEF);
                TaskYieldTrampoline(2);
                (*c)->unk6E = 0;
                do {
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                } while (++gCurTask->unk6E <= 9);
            } while ((s8)(*c)->unk88->unk16 != 2);
        }
    }
    TaskSetFrame(0xDE7);
    TaskYieldTrampoline(2);
    TaskSetFrame(0xDFB);
    TaskYieldTrampoline(2);
    sub_0803e050(2);
    gCurTask->unk88->unk68 = (u32)gUnk_0873CB1C;
    sub_0800a008(0, -1, gCurTask->unk88->unk00);
    gCurTask->unk73 = 2;
    gCurTask->unk3C++;
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 31; gCurTask->unk6C++) {
        if (gUnk_03001F34 == 0)
            gCurTask->unk88->unk42 &= 0xFBFF;
        TaskYieldTrampoline(1);
    }
    sub_0803e1b8(255, 0, gCurTask->unk88->unk00);
    gCurTask->unk88->unk42 &= 0xFCFF;
    gCurTask->unk73 = 3;
    TaskSleepForever();
}

void sub_0804b474(void)
{
    struct Task *t = gCurTask;

    switch (t->unk73) {
    case 1:
        BlendColors(gUnk_080DC628[t->unk88->unk00], gUnk_08203098, (u16)t->unk28, 16,
                     (u16 *)(gObjPalette + ((t->unk40 >> 12) << 5)));
        {
            struct Task *u = gCurTask;
            if (u->unk28 == 256) {
                u->unk73 = 0;
            } else {
                u->unk28 += 10;
                if (u->unk28 > 255)
                    u->unk28 = 256;
            }
        }
        break;
    case 2:
        BlendColors(gUnk_080DC628[t->unk88->unk00], gUnk_08203098, (u16)t->unk28, 16,
                     (u16 *)(gObjPalette + ((t->unk40 >> 12) << 5)));
        {
            struct Task *u = gCurTask;
            if (u->unk28 == 0) {
                u->unk73 = 0;
                sub_080261d4(0);
                gCurTask->unk88->unk42 &= 0xFFEF;
            } else {
                u->unk28 -= 16;
                if (u->unk28 <= 0)
                    u->unk28 = 0;
            }
        }
        break;
    case 3:
        if (t->unk7A & 1)
            t->unk88->unk01 = 1;
        else
            t->unk88->unk01 = 7;
        break;
    }
    {
        struct Task *v = gCurTask;
        if (v->unk58 > 0 && v->unk50 > v->unk2C) {
            sub_0803e050(2);
            {
                struct Task *w = gCurTask;
                w->unk50 = w->unk2C;
                w->unk4A = w->unk2C >> 16;
            }
        }
    }
}
