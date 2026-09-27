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
 *       screen (gGameOverDone = 1) with game state 1.
 *   sub_080cb70c / sub_080cbabc   sub-state 1, the "continue" animation, which
 *       ends the screen with game state 5 (back into the game).
 *   sub_080cbac0 / sub_080cbea4   sub-state 2, the "give up" animation (it
 *       spawns variants 3 and 4); its handler re-enters sub-state 0 with a
 *       120-frame count and spawns variant 2. */

extern u16 gGameState;           /* game state (AgbMain dispatch) */
extern u8 gGameOverDone;            /* game-over screen: done flag */
extern u32 gUnk_08754914[];
extern u32 gUnk_087548B8[];

void TaskYieldTrampoline(s32 frames);
s32 PlaySfx(s32 id);                                    /* play a sound effect */
void StopSfxOnPlayer(s32 player, s32 songId);
void TaskSleepForever(void);                                     /* end the running task */
void TaskSetEntry(void *a, u32 i);
void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void TaskStop(void);
void TaskSetFrame(s32 a);
void sub_080cb62c(void);
void CreateGameOverObject(u8 variant);

/* Task type #264 variant 0, sub-state 0. */
void sub_080cb64c(void)
{
    gCurTask->unk38 = gUnk_08754914;
    gCurTask->unk15 = 0;
    gCurTask->unk4C = 120 << 16;
    gCurTask->unk50 = 129 << 16;
    for (;;) {
        TaskSetFrame(0);
        TaskYieldTrampoline(16);
        gCurTask->unk3C++;
        TaskYieldTrampoline(16);
        gCurTask->unk3C++;
        TaskYieldTrampoline(20);
        gCurTask->unk3C--;
        TaskYieldTrampoline(12);
        if (gCurTask->unk18 != 0)
            PlaySfx(180);
        gCurTask->unk18 = 1;
        gCurTask->unk3C--;
        TaskYieldTrampoline(12);
        TaskSetFrame(3);
        TaskYieldTrampoline(48);
    }
}

/* Task type #264 variant 0, handler 0. */
void sub_080cb6d8(void)
{
    if (gCurTask->unk24 != 0) {
        if (gCurTask->unk20 <= 0) {
            gGameOverDone = 1;
            gGameState = 1;
        }
        gCurTask->unk20--;
    }
}

/* Task type #264 variant 0, sub-state 1. */
void sub_080cb70c(void)
{
    gCurTask->unk15 = 1;
    TaskStop();
    gCurTask->unk3C = 3;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 9;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 12;
    TaskSetMotion(0, 0, 0x5A5A5A5A, -0x27000, 0x2000, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 10;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 13;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 11;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 6;
    TaskYieldTrampoline(4);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(26);
    gCurTask->unk3C = 7;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(4);
    gCurTask->unk3C = 5;
    TaskSetMotion(0, 0, 0x5A5A5A5A, -0x14000, 0x2000, 0x5A5A5A5A);
    TaskYieldTrampoline(19);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(11);
    TaskSetMotion(0x20000, -0x1900, 0x5A5A5A5A, 0x5800, -0x1000, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    TaskSetMotion(0x5A5A5A5A, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    TaskSetMotion(0x20000, -0x1900, 0x5A5A5A5A, 0x5800, -0x1000, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    TaskSetMotion(0x5A5A5A5A, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gCurTask->unk3C = 8;
    TaskSetMotion(0x20000, -0x1900, 0x5A5A5A5A, 0x5800, -0x1000, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0x5A5A5A5A, -0x2000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    gCurTask->unk3C = 8;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gCurTask->unk3C = 24;
    TaskSetMotion(-0x4000, 0, 0x5A5A5A5A, -0x18000, 0x3000, 0x5A5A5A5A);
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    PlaySfx(156);
    gCurTask->unk3C = 17;
    TaskSetMotion(0x8000, 0x800, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(8);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskSetMotion(0x18000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    PlaySfx(156);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C = 14;
    TaskYieldTrampoline(8);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    PlaySfx(156);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(8);
    gGameOverDone = 1;
    gGameState = 5;
    TaskSleepForever();
}

void sub_080cbabc(void)
{
}

/* Task type #264 variant 0, sub-state 2. */
void sub_080cbac0(void)
{
    gCurTask->unk15 = 2;
    gCurTask->unk38 = gUnk_087548B8;
    TaskStop();
    gCurTask->unk3C = 0;
    gCurTask->unk54 = 0x10000;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = -0x10000;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0x8000;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = -0x8000;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0;
    TaskYieldTrampoline(12);
    gCurTask->unk54 = 0x10000;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = -0x10000;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0x8000;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = -0x8000;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0;
    TaskYieldTrampoline(12);
    gCurTask->unk54 = 0xC000;
    gCurTask->unk5C = -0x4000;
    TaskYieldTrampoline(5);
    gCurTask->unk54 = -0xC000;
    gCurTask->unk5C = 0x4000;
    TaskYieldTrampoline(5);
    gCurTask->unk54 = 0xC000;
    gCurTask->unk5C = -0x4000;
    TaskYieldTrampoline(5);
    gCurTask->unk54 = -0xC000;
    gCurTask->unk5C = 0x4000;
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    gCurTask->unk54 = 0;
    gCurTask->unk5C = 0;
    TaskYieldTrampoline(10);
    gCurTask->unk3C++;
    TaskYieldTrampoline(10);
    gCurTask->unk3C++;
    TaskYieldTrampoline(10);
    gCurTask->unk3C++;
    TaskYieldTrampoline(30);
    gCurTask->unk3C++;
    TaskYieldTrampoline(10);
    gCurTask->unk3C++;
    TaskYieldTrampoline(6);
    gCurTask->unk3C--;
    TaskYieldTrampoline(8);
    gCurTask->unk3C++;
    TaskYieldTrampoline(20);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 7;
    gCurTask->unk58 = 0x14000;
    gCurTask->unk60 = -0x4000;
    TaskYieldTrampoline(6);
    gCurTask->unk1C = PlaySfx(103);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    TaskStop();
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk3C = 9;
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 12);
    StopSfxOnPlayer(gCurTask->unk1C, 0x67);
    PlaySfx(104);
    CreateGameOverObject(3);
    gCurTask->unk3C++;
    gCurTask->unk54 = -0x40000;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0x60000;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = -0x30000;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = -0x10000;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    gCurTask->unk54 = 0;
    TaskYieldTrampoline(2);
    CreateGameOverObject(4);
    gCurTask->unk3C = 11;
    TaskSetMotion(0, 0, 0x5A5A5A5A, -0x38000, 0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gCurTask->unk3C++;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 11;
    TaskSetMotion(0, 0, 0x5A5A5A5A, -0x38000, 0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gCurTask->unk3C++;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(8);
    TaskSetMotion(0x38000, -0x10000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    gCurTask->unk3C++;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(20);
    PlaySfx(113);
    gCurTask->unk3C = 13;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk43 = -1;
    gCurTask->unk14 = 0;
    TaskSleepForever();
}

/* Task type #264 variant 0, handler 2. */
void sub_080cbea4(void)
{
    if (gCurTask->unk14 != 2) {
        gCurTask->unk20 = 120;
        CreateGameOverObject(2);
        TaskSetEntry(sub_080cb62c, gCurTaskIdx);
    }
}
