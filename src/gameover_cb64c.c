#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* gameover_cb64c.c (0x080CB64C-0x080CBED3, issue #100).
 *
 * The game-over screen, task type #264 variant 0: the player character.
 * Its sub-states gUnk_087582AC[Task.unk14] and per-frame handlers
 * gUnk_087582B8[Task.unk15] (sub_080cb5c4 starts it, sub_080cb62c re-enters
 * it with Task.unk24 = 1):
 *   sub_080cb64c / sub_080cb6d8   sub-state 0, the idle loop; once re-entered
 *       (Task.unk24) the handler counts Task.unk20 down and then ends the
 *       screen (gUnk_02007BE0 = 1) with game state 1.
 *   sub_080cb70c / sub_080cbabc   sub-state 1, the "continue" animation, which
 *       ends the screen with game state 5 (back into the game).
 *   sub_080cbac0 / sub_080cbea4   sub-state 2, the "give up" animation (it
 *       spawns variants 3 and 4); its handler re-enters sub-state 0 with a
 *       120-frame count and spawns variant 2. */

extern u16 gUnk_030023D8;           /* game state (AgbMain dispatch) */
extern u8 gUnk_02007BE0;            /* game-over screen: done flag */
extern u32 gUnk_08754914[];
extern u32 gUnk_087548B8[];

void TaskYieldTrampoline(s32 frames);
s32 sub_080031b8(s32 id);                                    /* play a sound effect */
void sub_080034f0(s32 player, s32 songId);
void sub_08006138(void);                                     /* end the running task */
void sub_08006148(void *a, u32 i);
void sub_0800625c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void sub_080062c4(void);
void sub_08006338(s32 a);
void sub_080cb62c(void);
void sub_080cbf34(u8 variant);

/* Task type #264 variant 0, sub-state 0. */
void sub_080cb64c(void)
{
    gUnk_03002490->unk38 = gUnk_08754914;
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk4C = 120 << 16;
    gUnk_03002490->unk50 = 129 << 16;
    for (;;) {
        sub_08006338(0);
        TaskYieldTrampoline(16);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(20);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(12);
        if (gUnk_03002490->unk18 != 0)
            sub_080031b8(180);
        gUnk_03002490->unk18 = 1;
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(12);
        sub_08006338(3);
        TaskYieldTrampoline(48);
    }
}

/* Task type #264 variant 0, handler 0. */
void sub_080cb6d8(void)
{
    if (gUnk_03002490->unk24 != 0) {
        if (gUnk_03002490->unk20 <= 0) {
            gUnk_02007BE0 = 1;
            gUnk_030023D8 = 1;
        }
        gUnk_03002490->unk20--;
    }
}

/* Task type #264 variant 0, sub-state 1. */
void sub_080cb70c(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080062c4();
    gUnk_03002490->unk3C = 3;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 9;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 12;
    sub_0800625c(0, 0, 0x5A5A5A5A, -0x27000, 0x2000, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 13;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 11;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 6;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(26);
    gUnk_03002490->unk3C = 7;
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 5;
    sub_0800625c(0, 0, 0x5A5A5A5A, -0x14000, 0x2000, 0x5A5A5A5A);
    TaskYieldTrampoline(19);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(11);
    sub_0800625c(0x20000, -0x1900, 0x5A5A5A5A, 0x5800, -0x1000, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    sub_0800625c(0x5A5A5A5A, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    sub_0800625c(0x20000, -0x1900, 0x5A5A5A5A, 0x5800, -0x1000, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    sub_0800625c(0x5A5A5A5A, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3C = 8;
    sub_0800625c(0x20000, -0x1900, 0x5A5A5A5A, 0x5800, -0x1000, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(0x5A5A5A5A, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C = 8;
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3C = 24;
    sub_0800625c(-0x4000, 0, 0x5A5A5A5A, -0x18000, 0x3000, 0x5A5A5A5A);
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    sub_080031b8(156);
    gUnk_03002490->unk3C = 17;
    sub_0800625c(0x8000, 0x800, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    sub_0800625c(0x18000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    sub_080031b8(156);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 14;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_080031b8(156);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    gUnk_02007BE0 = 1;
    gUnk_030023D8 = 5;
    sub_08006138();
}

void sub_080cbabc(void)
{
}

/* Task type #264 variant 0, sub-state 2. */
void sub_080cbac0(void)
{
    gUnk_03002490->unk15 = 2;
    gUnk_03002490->unk38 = gUnk_087548B8;
    sub_080062c4();
    gUnk_03002490->unk3C = 0;
    gUnk_03002490->unk54 = 0x10000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = -0x10000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x8000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = -0x8000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0;
    TaskYieldTrampoline(12);
    gUnk_03002490->unk54 = 0x10000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = -0x10000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x8000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = -0x8000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0;
    TaskYieldTrampoline(12);
    gUnk_03002490->unk54 = 0xC000;
    gUnk_03002490->unk5C = -0x4000;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk54 = -0xC000;
    gUnk_03002490->unk5C = 0x4000;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk54 = 0xC000;
    gUnk_03002490->unk5C = -0x4000;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk54 = -0xC000;
    gUnk_03002490->unk5C = 0x4000;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk5C = 0;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(30);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(20);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 7;
    gUnk_03002490->unk58 = 0x14000;
    gUnk_03002490->unk60 = -0x4000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk1C = sub_080031b8(103);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    sub_080062c4();
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 12);
    sub_080034f0(gUnk_03002490->unk1C, 0x67);
    sub_080031b8(104);
    sub_080cbf34(3);
    gUnk_03002490->unk3C++;
    gUnk_03002490->unk54 = -0x40000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x60000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = -0x30000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x20000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = -0x10000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    gUnk_03002490->unk54 = 0;
    TaskYieldTrampoline(2);
    sub_080cbf34(4);
    gUnk_03002490->unk3C = 11;
    sub_0800625c(0, 0, 0x5A5A5A5A, -0x38000, 0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C++;
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 11;
    sub_0800625c(0, 0, 0x5A5A5A5A, -0x38000, 0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C++;
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(8);
    sub_0800625c(0x38000, -0x10000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C++;
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(20);
    sub_080031b8(113);
    gUnk_03002490->unk3C = 13;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk43 = -1;
    gUnk_03002490->unk14 = 0;
    sub_08006138();
}

/* Task type #264 variant 0, handler 2. */
void sub_080cbea4(void)
{
    if (gUnk_03002490->unk14 != 2) {
        gUnk_03002490->unk20 = 120;
        sub_080cbf34(2);
        sub_08006148(sub_080cb62c, gCurTaskIdx);
    }
}
