#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* gameover_cbed4.c (0x080CBED4-0x080CCD4B, issue #100).
 *
 * The game-over screen, task type #264 variant 1: the cursor.  Its
 * sub-states gUnk_087582C4[Task.unk14] and per-frame handlers
 * gUnk_087582DC[Task.unk15] (sub_080cbed4 starts it, sub_080cbfc8 re-enters
 * it), plus the helpers the whole screen uses:
 *   sub_080cbf34   spawn task type #264 with Task.unk73 = variant.
 *   sub_080cbf68 / sub_080cbfac   re-enter variant 0 (its task index is
 *       gUnk_02007D28) in sub-state 2 or 1 by this cursor's Task.unk24.
 *   sub_080cbfe4 / sub_080cc024   sub-state 0, the cursor at rest; up or down
 *       (sub_080cb108) moves it (sub-state 1), A or START picks: with
 *       Task.unk24 set sub-state 4, otherwise task type #263 and sub-state 2.
 *   sub_080cc0a4 / sub_080cc14c   sub-state 1, the move, which flips
 *       gUnk_02006164.
 *   sub_080cc180 ... sub_080cc768 / sub_080ccd10   sub-states 2-5, the cursor's
 *       animations after a choice; handlers 3 and 5 hand over to variant 0. */

extern vu16 gUnk_03001EB8[];        /* keys pressed per player */
extern s8 gUnk_02006164;            /* game-over screen: cursor (continue = 0?) */
extern s16 gUnk_02007D28;           /* game-over screen: the #264 variant-0 task's index */
extern u32 gUnk_087549B0[];
extern void (*gUnk_087582C4[])(void);
extern void (*gUnk_087582DC[])(void);

void TaskYieldTrampoline(s32 frames);
void sub_08002e98(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
s32 sub_080031b8(s32 id);                                    /* play a sound effect */
s32 sub_080058e4(u32 type, s32 idx);                         /* spawn a task */
void sub_08006138(void);                                     /* end the running task */
void sub_08006148(void *a, u32 i);
void sub_0800625c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void sub_080062c4(void);
u8 sub_080cb108(void);
void sub_080cb62c(void);
void sub_080cbf18(void);

/* Task type #264 variant 1: sub-states gUnk_087582C4[Task.unk14], per-frame
   handlers gUnk_087582DC[Task.unk15] (sub_080cbf18). */
void sub_080cbed4(void)
{
    gUnk_03002490->unk04 = (u32)sub_080cbf18;
    gUnk_03002490->unk42 = 6;
    gUnk_03002490->unk38 = gUnk_087549B0;
    gUnk_03002490->unk24 = 0;
    gUnk_03002490->unk20 = 0;
    gUnk_03002490->unk14 = 0;
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_087582C4);
    sub_08006138();
}

void sub_080cbf18(void)
{
    sub_08002e98(gUnk_03002490->unk15, 6, gUnk_087582DC);
}

/* Spawn task type #264 with Task.unk73 = variant. */
void sub_080cbf34(u8 variant)
{
    s32 id = sub_080058e4(264, 32);
    struct Task *t;

    if (id != -1) {
        t = &gUnk_03002790[id];
        t->unk73 = variant;
    }
}

void sub_080cbf68(void)
{
    struct Task *t = &gUnk_03002790[gUnk_02007D28];

    if (gUnk_03002490->unk24 != 0)
        t->unk14 = 2;
    else
        t->unk14 = 1;
    sub_08006148(sub_080cb62c, gUnk_02007D28);
}

void sub_080cbfac(void)
{
    sub_080cbf68();
    gUnk_03002490->unk20 = 0;
    sub_080cbf34(3);
}

void sub_080cbfc8(void)
{
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_087582C4);
}

/* Task type #264 variant 1, sub-state 0. */
void sub_080cbfe4(void)
{
    gUnk_03002490->unk15 = 0;
    if (gUnk_03002490->unk24 != 0) {
        gUnk_03002490->unk4C = 128 << 16;
        gUnk_03002490->unk50 = 110 << 16;
    } else {
        gUnk_03002490->unk4C = 128 << 16;
        gUnk_03002490->unk50 = 85 << 16;
    }
    gUnk_03002490->unk3C = 0;
    sub_08006138();
}

/* Task type #264 variant 1, handler 0. */
void sub_080cc024(void)
{
    if (sub_080cb108()) {
        gUnk_03002490->unk24 ^= 1;
        gUnk_03002490->unk14 = 1;
    } else if (gUnk_03001EB8[0] & 9) {
        sub_080031b8(102);
        if (gUnk_03002490->unk24 != 0) {
            gUnk_03002490->unk14 = 4;
        } else {
            sub_080058e4(263, 32);
            gUnk_03002490->unk14 = 2;
        }
    }
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_080cbfc8, gCurTaskIdx);
}

/* Task type #264 variant 1, sub-state 1. */
void sub_080cc0a4(void)
{
    gUnk_03002490->unk15 = 1;
    if (gUnk_03002490->unk24 != 0) {
        sub_0800625c(-0xA000, 0x1000, 0x5A5A5A5A, 0, 0x5000, 0x5A5A5A5A);
        TaskYieldTrampoline(9);
        gUnk_03002490->unk60 = -0x5000;
        TaskYieldTrampoline(9);
    } else {
        sub_0800625c(-0xA000, 0x1000, 0x5A5A5A5A, 0, -0x5000, 0x5A5A5A5A);
        TaskYieldTrampoline(9);
        gUnk_03002490->unk60 = 0x5000;
        TaskYieldTrampoline(9);
    }
    sub_080062c4();
    TaskYieldTrampoline(5);
    gUnk_03002490->unk14 = 0;
    sub_08006138();
}

/* Task type #264 variant 1, handler 1. */
void sub_080cc14c(void)
{
    if (gUnk_03002490->unk14 != 1) {
        sub_08006148(sub_080cbfc8, gCurTaskIdx);
        gUnk_02006164 ^= 1;
    }
}

/* Task type #264 variant 1, sub-state 2. */
void sub_080cc180(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080062c4();
    gUnk_03002490->unk3C = 1;
    gUnk_03002490->unk54 = -0xA000;
    gUnk_03002490->unk5C = 0x2000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(3);
    sub_080062c4();
    gUnk_03002490->unk3C = 1;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    gUnk_03002490->unk54 = 0x18000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 2;
    sub_0800625c(0x18000, -0x4800, 0x5A5A5A5A, 0x8400, -0x1800, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    sub_080062c4();
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3C = 1;
    sub_0800625c(-0x20000, 0x1000, 0x5A5A5A5A, 0, 0x1000, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(15);
    gUnk_03002490->unk3C = 1;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3C = 3;
    gUnk_03002490->unk60 = -0x800;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    sub_080062c4();
    gUnk_03002490->unk14 = 3;
    sub_08006138();
}

/* Task type #264 variant 1, handler 2. */
void sub_080cc2b8(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_080cbfc8, gCurTaskIdx);
}

/* Task type #264 variant 1, sub-state 3. */
void sub_080cc2e0(void)
{
    gUnk_03002490->unk15 = 3;
    sub_080062c4();
    gUnk_03002490->unk3C = 4;
    sub_0800625c(0x20000, 0, 0x5A5A5A5A, 0x20000, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk20 = 1;
    gUnk_03002490->unk3C++;
    sub_0800625c(0, -0x1000, 0x5A5A5A5A, -0x30000, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    sub_080031b8(280);
    gUnk_03002490->unk3C++;
    sub_0800625c(-0x10000, -0x8000, 0x5A5A5A5A, -0x30000, 0x2000, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    sub_0800625c(-0x10000, 0x1000, 0x5A5A5A5A, 0, 0x1800, 0x5A5A5A5A);
    TaskYieldTrampoline(12);
    sub_0800625c(-0x4100, 0xA00, 0x5A5A5A5A, 0x5A5A5A5A, 0x1D00, 0x5A5A5A5A);
    TaskYieldTrampoline(12);
    sub_0800625c(0, 0xA00, 0x5A5A5A5A, 0x16000, -0xC00, 0x5A5A5A5A);
    TaskYieldTrampoline(24);
    sub_0800625c(0x10000, 0x1C00, 0x5A5A5A5A, 0x4000, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 8;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    sub_080031b8(279);
    gUnk_03002490->unk3C = 4;
    sub_0800625c(0x20000, -0x1900, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gUnk_03002490->unk5C = -0x2000;
    TaskYieldTrampoline(8);
    sub_080062c4();
    TaskYieldTrampoline(10);
    sub_080031b8(279);
    sub_0800625c(0x20000, -0x1900, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gUnk_03002490->unk5C = -0x2000;
    TaskYieldTrampoline(8);
    sub_080062c4();
    TaskYieldTrampoline(10);
    sub_080031b8(279);
    sub_0800625c(0x20000, -0x1900, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gUnk_03002490->unk5C = -0x2000;
    TaskYieldTrampoline(8);
    sub_080062c4();
    TaskYieldTrampoline(16);
    gUnk_03002490->unk54 = -0x10000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk5C = 0x1000;
    TaskYieldTrampoline(16);
    sub_0800625c(-0x10000, 0x4000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(4);
    sub_0800625c(0x20000, -0x800, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(14);
    gUnk_03002490->unk5C = -0x2000;
    TaskYieldTrampoline(8);
    sub_0800625c(-0x10000, 0x4000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(4);
    sub_0800625c(0x20000, -0x800, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(14);
    gUnk_03002490->unk5C = -0x2000;
    TaskYieldTrampoline(8);
    sub_0800625c(-0x10000, 0x4000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(4);
    sub_0800625c(0x20000, -0x800, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(14);
    gUnk_03002490->unk5C = -0x2000;
    TaskYieldTrampoline(8);
    sub_08006138();
}

/* Task type #264 variant 1, handler 3. */
void sub_080cc5d4(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_080cbfc8, gCurTaskIdx);
    if (gUnk_03002490->unk20 != 0)
        sub_080cbfac();
}

/* Task type #264 variant 1, sub-state 4. */
void sub_080cc608(void)
{
    gUnk_03002490->unk15 = 4;
    sub_080062c4();
    gUnk_03002490->unk3C = 1;
    gUnk_03002490->unk54 = -0xA000;
    gUnk_03002490->unk5C = 0x2000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(3);
    sub_080062c4();
    gUnk_03002490->unk3C = 1;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    gUnk_03002490->unk54 = 0x18000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 2;
    sub_0800625c(0x18000, -0x4800, 0x5A5A5A5A, 0x8400, -0x1800, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    sub_080062c4();
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3C = 1;
    sub_0800625c(-0x20000, 0xC00, 0x5A5A5A5A, 0, 0x800, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(15);
    gUnk_03002490->unk3C = 1;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(20);
    sub_0800625c(0x20000, 0, 0x5A5A5A5A, 0x10000, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    sub_080062c4();
    gUnk_03002490->unk14 = 5;
    sub_08006138();
}

/* Task type #264 variant 1, handler 4. */
void sub_080cc740(void)
{
    if (gUnk_03002490->unk14 != 4)
        sub_08006148(sub_080cbfc8, gCurTaskIdx);
}

/* Task type #264 variant 1, sub-state 5. */
void sub_080cc768(void)
{
    gUnk_03002490->unk15 = 5;
    sub_080062c4();
    sub_080031b8(278);
    gUnk_03002490->unk3C = 2;
    sub_0800625c(0x18000, -0x4800, 0x5A5A5A5A, 0x8400, -0x1800, 0x5A5A5A5A);
    TaskYieldTrampoline(7);
    gUnk_03002490->unk20 = 1;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 0;
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    sub_080031b8(278);
    gUnk_03002490->unk3C = 2;
    sub_0800625c(0x18000, -0x4800, 0x5A5A5A5A, 0x8400, -0x1800, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3C = 0;
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(20);
    sub_0800625c(-0x18C00, 0xC00, 0x5A5A5A5A, -0x6000, 0x200, 0x5A5A5A5A);
    TaskYieldTrampoline(33);
    gUnk_03002490->unk3C = 1;
    sub_0800625c(-0x14000, 0x4000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(7);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 1;
    sub_0800625c(0x1C000, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 2;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 1;
    sub_0800625c(-0x14000, 0x4000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(7);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 1;
    sub_0800625c(0x1C000, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 2;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 1;
    sub_0800625c(-0x14000, 0x4000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(7);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 1;
    sub_0800625c(0x1C000, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 2;
    TaskYieldTrampoline(11);
    gUnk_03002490->unk3C = 1;
    sub_0800625c(0x10000, 0x8000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 11;
    TaskYieldTrampoline(5);
    sub_0800625c(-0x18000, 0x8000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(5);
    sub_0800625c(0x1C000, -0x8000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(5);
    sub_0800625c(-0xA000, 0, 0x5A5A5A5A, -0x28000, 0x8000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(5);
    sub_0800625c(-0x4000, 0x800, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    sub_0800625c(-0x8000, 0x1000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    sub_0800625c(-0x10000, 0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C = 13;
        sub_0800625c(0x4000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    gUnk_03002490->unk3C = 12;
    sub_0800625c(0x30000, 0x8000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(2);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gUnk_03002490->unk4C = 120 << 16;
    gUnk_03002490->unk50 = 125 << 16;
    gUnk_03002490->unk3C = 17;
    sub_0800625c(0, 0, 0x5A5A5A5A, -0x60000, 0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0x60000, -0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = -1;
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 17;
    sub_0800625c(0, 0, 0x5A5A5A5A, -0x60000, 0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0x60000, -0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = -1;
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gUnk_03002490->unk4C = 120 << 16;
    gUnk_03002490->unk50 = 133 << 16;
    gUnk_03002490->unk3C = 18;
    sub_0800625c(0x60000, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    sub_0800625c(-0x60000, 0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = -1;
    sub_080062c4();
    sub_08006138();
}

/* Task type #264 variant 1, handler 5. */
void sub_080ccd10(void)
{
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_080cbfc8, gCurTaskIdx);
    if (gUnk_03002490->unk20 != 0) {
        sub_080cbf68();
        gUnk_03002490->unk20 = 0;
    }
}
