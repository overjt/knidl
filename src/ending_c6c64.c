#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "sound.h"
#include "player.h"
#include "effect.h"
#include "enemy.h"
#include "ending.h"

/* ending_c6c64.c (0x080C6C64-0x080C7E4B, issue #100).
 *
 * The first ending scene, part 1: task type #100 (class 3), which M37's
 * EndingEpilogueScene spawns in AgbMain state 11 and waits on (gEndingSceneActive).
 *   Task_EndingEpilogue   the body: variant 0 (Task.variant == 0) loads the graphics
 *       (EndingEpilogueLoadGraphics) and spawns variants 1, 6, 7, 9 and 10 from the list
 *       gEndingEpilogueObjectVariants (CreateEndingEpilogueObjects); variants 1-10 run gEndingEpilogueVariants[unk73].
 *   sub_080c6d84 / sub_080c769c   variant 1, the scene's main sprite, and its
 *       scaling draw callback; sub_080c77cc spawns its four variant-2 helpers.
 *   sub_080c7810   variant 2: while the spawner is drawn (Task.unk34), one of
 *       four effects by Task.unk74 (a shaking sprite, drifting puffs, spark
 *       bursts, four variant-3 sprites).
 *   sub_080c7cc0   variant 3: a sprite that drifts right from its spawner and
 *       plays one of three animations five times. */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 (lesson 3.428) */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
u32 RandomRange(u32 range);                                 /* random 0 .. range-1 */
s32 PlaySfx(s32 id);                                    /* play a sound effect */
s32 TaskCreateFrom(u32 type, s32 idx);                         /* spawn a task */
void TaskSleepForever(void);                                     /* end the running task */
void LoadGfxSet(u16 a0);                                   /* load screen graphics */

/* Task type #100 (class 3): variant 0 loads the graphics and spawns the
   other variants; variants 1-10 run the anchor table gEndingEpilogueVariants[]. */
void Task_EndingEpilogue(void)
{
    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = 0;
    if (gCurTask->variant == 0) {
        EndingEpilogueLoadGraphics();
        CreateEndingEpilogueObjects();
    } else {
        CallTableEntry(gCurTask->variant, 11, gEndingEpilogueVariants);
    }
    TaskExitTrampoline();
}

/* Task type #100's graphics: two sprite sheets, the first one's palette and,
   with more than one player, this player's palette in slot 1. */
void EndingEpilogueLoadGraphics(void)
{
    struct GfxHeader *h = &gUnk_085995AC;

    LoadGfxSet(0);
    LZ77UnCompWram(h->tiles, gUnk_02020000);
    RequestCopy(4, (u32)gUnk_02020000, (u32)gObjVram, h->tileCount << 5);
    RequestCopy(2, (u32)h->palette, (u32)gObjPaletteBank8, h->paletteBankCount << 5);
    if (gEndingPlayerCount > 1)
        RequestCopy(2, (u32)gPlayerPalettes[gEndingLocalPlayer], (u32)&gObjPaletteBank8[16], 22);
    h = &gUnk_0859990C;
    LZ77UnCompWram(h->tiles, gUnk_02020000);
    RequestCopy(3, (u32)gUnk_02020000, (u32)gObjVram, h->tileCount << 5);
    LoadGfxSet(71);
}

/* Spawn one task type #100 per variant listed in gEndingEpilogueObjectVariants[] (1, 6, 7,
   9 and 10, ended by 11, the anchor table's size). */
void CreateEndingEpilogueObjects(void)
{
    s32 i;
    s32 id;
    s32 v;
    struct Task *t;

    for (i = 0; v = gEndingEpilogueObjectVariants[i], (s16)gEndingEpilogueObjectVariants[i] <= 10; i++) {
        id = TaskCreateFrom(100, 32);
        if (id == -1)
            for (;;)
                ;
        t = &gTasks[id];
        t->variant = v;
    }
}

/* Task type #100 variant 1, the scene's main sprite: spawns variant 4 and
   the four variant-2 helpers (sub_080c77cc), draws itself through the
   scaling callback sub_080c769c, runs its timed motion phases and ends the
   scene by clearing gEndingSceneActive, which M37's EndingEpilogueScene waits for. */
void sub_080c6d84(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)sub_080c769c;
    gCurTask->layer = 8;
    gCurTask->frameTable = gUnk_08755708;
    gCurTask->tileWord = 0x8810;
    gCurTask->unk46 = TaskCreateFrom(100, 32);
    gTasks[gCurTask->unk46].variant = 4;
    gTasks[gCurTask->unk46].parent = gCurTaskIdx;
    sub_080c77cc();
    gCurTask->unk34 = 0;
    gCurTask->unk24 = 0;
    gCurTask->posX = 0x600000;
    gCurTask->posY = 0x320000;
    TaskStop();
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(240);
    TaskYieldTrampoline(12);
    TaskYieldTrampoline(190);
    QueueSprite(12, (u32)gUnk_080DBEF8, 0, 0, gCurTask->pixelX, gCurTask->pixelY);
    TaskYieldTrampoline(1);
    QueueSprite(12, (u32)gUnk_080DBF00, 0, 0, gCurTask->pixelX, gCurTask->pixelY);
    TaskYieldTrampoline(1);
    QueueSprite(12, (u32)gUnk_080DBF08, 0, 0, gCurTask->pixelX, gCurTask->pixelY);
    TaskYieldTrampoline(1);
    QueueSprite(12, (u32)gUnk_080DBF18, 0, 0, gCurTask->pixelX, gCurTask->pixelY);
    TaskYieldTrampoline(1);
    QueueSprite(12, (u32)gUnk_080DBF28, 0, 0, gCurTask->pixelX, gCurTask->pixelY);
    TaskYieldTrampoline(1);
    QueueSprite(12, (u32)gUnk_080DBF30, 0, 0, gCurTask->pixelX, gCurTask->pixelY);
    TaskYieldTrampoline(1);
    TaskYieldTrampoline(16);
    gCurTask->frame = 62;
    PlaySfx(281);
    gCurTask->unk18 = 0x30000;
    gCurTask->unk28 = 0x1000;
    TaskStop();
    TaskYieldTrampoline(4);
    gCurTask->velX = -0x2000;
    gCurTask->velY = -0x4000;
    TaskYieldTrampoline(80);
    gCurTask->velX = -0x1000;
    TaskYieldTrampoline(24);
    gCurTask->velX = -0x800;
    TaskYieldTrampoline(24);
    gCurTask->velX = 0x800;
    TaskYieldTrampoline(24);
    gCurTask->velX = 0x1000;
    TaskYieldTrampoline(24);
    gCurTask->velX = 0x2000;
    TaskYieldTrampoline(24);
    gCurTask->velX = 0x4000;
    TaskYieldTrampoline(24);
    gCurTask->velX = 0x8000;
    TaskYieldTrampoline(24);
    gCurTask->frame = -1;
    TaskStop();
    gCurTask->velX = 0x40000;
    TaskYieldTrampoline(28);
    gCurTask->velX = -0x6000;
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(32);
    PlayBgm(24);
    gCurTask->frame = 62;
    gCurTask->velX = 0x8000;
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(48);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(32);
    gCurTask->velY = 0xC000;
    TaskYieldTrampoline(16);
    gCurTask->frame = -1;
    gCurTask->velX = -0x10000;
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(14);
    gCurTask->velX = 0x6000;
    gCurTask->velY = -0x6000;
    TaskYieldTrampoline(32);
    gCurTask->unk34 = 1;
    gCurTask->frame = 62;
    gCurTask->unk18 = 0x3F0000;
    gCurTask->unk28 = 0;
    gCurTask->velX = -0x20000;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(32);
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(16);
    gCurTask->velY = -0x4000;
    TaskYieldTrampoline(16);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(16);
    gCurTask->velX = -0x10000;
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(16);
    gCurTask->velX = -0x8000;
    gCurTask->velY = -0xC000;
    TaskYieldTrampoline(16);
    gCurTask->velX = -0x4000;
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(16);
    gCurTask->velX = 0x4000;
    gCurTask->velY = -0x4000;
    TaskYieldTrampoline(16);
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(64);
    gCurTask->unk6C = 0;
    do {
        TaskStop();
        gCurTask->velY = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x400;
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x800;
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(32);
        gCurTask->velX = 0x400;
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(8);
        TaskStop();
        gCurTask->velY = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x400;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x800;
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(32);
        gCurTask->velX = 0x400;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(8);
        TaskStop();
        gCurTask->velY = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    gCurTask->unk6C = 0;
    do {
        TaskStop();
        gCurTask->velY = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x400;
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x800;
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(32);
        gCurTask->velX = 0x400;
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(8);
        TaskStop();
        gCurTask->velY = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x400;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x800;
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(32);
        gCurTask->velX = 0x400;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(8);
        TaskStop();
        gCurTask->velY = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 5);
    gCurTask->velX = 0x400;
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x800;
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(32);
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x400;
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(8);
    gCurTask->velY = -0x2000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x800;
    gCurTask->velY = -0x4000;
    TaskYieldTrampoline(8);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(32);
    gCurTask->velY = -0x4000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x400;
    gCurTask->velY = -0x2000;
    TaskYieldTrampoline(8);
    TaskStop();
    gCurTask->velY = -0x2000;
    TaskYieldTrampoline(8);
    gCurTask->unk6C = 0;
    do {
        TaskStop();
        gCurTask->velY = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x400;
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x800;
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(32);
        gCurTask->velX = 0x400;
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(8);
        TaskStop();
        gCurTask->velY = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x400;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x800;
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(32);
        gCurTask->velX = 0x400;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(8);
        TaskStop();
        gCurTask->velY = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskStop();
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x400;
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x800;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(28);
    gCurTask->velX = 0x400;
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(8);
    TaskStop();
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(8);
    gCurTask->velY = -0x2000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x400;
    gCurTask->velY = -0x4000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x800;
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(28);
    gCurTask->velX = 0x400;
    gCurTask->velY = -0x4000;
    TaskYieldTrampoline(8);
    TaskStop();
    gCurTask->velY = -0x2000;
    TaskYieldTrampoline(8);
    gCurTask->unk34 = 1;
    gCurTask->unk6C = 0;
    do {
        gCurTask->velX = 0x400;
        gCurTask->velY = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x800;
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(32);
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x400;
        gCurTask->velY = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x800;
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(24);
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x400;
        gCurTask->velY = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 10);
    gCurTask->velX = -0x8000;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velX = -0x10000;
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velX = -0x20000;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(12);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(12);
    gCurTask->velX = -0x10000;
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velX = -0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x8000;
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x10000;
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x20000;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(40);
    gCurTask->velX = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x8000;
    gCurTask->velY = -0x2000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x2000;
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(8);
    TaskStop();
    TaskYieldTrampoline(16);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(2);
    TaskStop();
    TaskYieldTrampoline(60);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(2);
    TaskStop();
    TaskYieldTrampoline(14);
    TaskYieldTrampoline(68);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(2);
    TaskStop();
    TaskYieldTrampoline(46);
    gEndingSceneActive = 0;
    TaskSleepForever();
}

/* Task type #100 variant 1's draw callback: grow the scale Task.unk18 by
   Task.unk28 (clamped to 0 .. 127 in 16.16), alternate two animation frames
   every three calls and draw the sprite, scaled through gUnk_0873FF98[] when
   the scale is not 1.0 (0x100). */
void sub_080c769c(void)
{
    s32 n;
    struct Task *t;
    u32 *tbl;
    s16 scale;

    gCurTask->unk18 += gCurTask->unk28;
    if (gCurTask->unk18 < 0)
        gCurTask->unk18 = 0;
    if ((gCurTask->unk18 >> 16) > 127)
        gCurTask->unk18 = 0x7F0000;
    if (gCurTask->frameTable != NULL && gCurTask->frame != -1) {
        n = 0;
        if ((s16)gCurTask->unk70 > 1)
            n = 1;
        if ((s16)++gCurTask->unk70 > 5)
            gCurTask->unk70 = 0;
        t = gCurTask;
        if ((u16)(t->pixelX + 63) <= 366 && t->pixelY > -64 && t->pixelY < 224) {
            tbl = t->frameTable;
            scale = gUnk_0873FF98[t->unk18 >> 16];
            if ((u16)scale != 0x100)
                QueueSprite(gCurTask->layer, DrawAffineSprite(tbl[t->frame + n], scale, scale, 0),
                             gCurTask->spriteFlags, gCurTask->tileWord, gCurTask->pixelX,
                             gCurTask->pixelY);
            else
                QueueSprite(t->layer, tbl[t->frame + n], t->spriteFlags, t->tileWord, t->pixelX, t->pixelY);
        }
    }
}

/* Spawn the four task type #100 variant-2 helpers of variant 1 (Task.unk74
   = 0..3, Task.parent = the calling task). */
void sub_080c77cc(void)
{
    s32 i;
    s32 id;
    struct Task *t;

    for (i = 0; i <= 3; i++) {
        id = TaskCreateFrom(100, 32);
        t = &gTasks[id];
        t->variant = 2;
        t->unk74 = i;
        t->parent = gCurTaskIdx;
    }
}

/* Task type #100 variant 2 (four of them, spawned by variant 1's
   sub_080c77cc with Task.unk74 = 0..3 and Task.parent = variant 1's task):
   while the spawner has graphics (Task.unk34), each one plays its own
   effect relative to it - 0 a shaking sprite, 1 two drifting puffs, 2
   three bursts of sparks, 3 spawns four variant 3 sprites. */
void sub_080c7810(void)
{
    gCurTask->moveCallback = (u32)TaskMoveRelativeToParent;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->frameTable = gUnk_0874CF28;
    gCurTask->tileWord = 0;
    for (;;) {
        if (gTasks[gCurTask->parent].unk34 == 0) {
            TaskYieldTrampoline(1);
            continue;
        }
        switch (gCurTask->unk74) {
        case 0:
            TaskStop();
            gCurTask->layer = 12;
            gCurTask->posX = 8 << 16;
            gCurTask->posY = 0;
            gCurTask->velX = 0x10000;
            gCurTask->frame = 0;
            TaskYieldTrampoline(1);
            gCurTask->velX = -0x10000;
            gCurTask->frame = 0;
            TaskYieldTrampoline(1);
            gCurTask->velX = 0x10000;
            gCurTask->frame = 1;
            TaskYieldTrampoline(1);
            gCurTask->velX = -0x10000;
            gCurTask->frame = 1;
            TaskYieldTrampoline(1);
            gCurTask->velX = 0x10000;
            gCurTask->frame = 2;
            TaskYieldTrampoline(1);
            gCurTask->velX = -0x10000;
            gCurTask->frame = 2;
            TaskYieldTrampoline(1);
            gCurTask->velX = 0x10000;
            gCurTask->frame = 3;
            TaskYieldTrampoline(1);
            gCurTask->velX = -0x10000;
            gCurTask->frame = 3;
            TaskYieldTrampoline(1);
            break;
        case 1:
            TaskStop();
            gCurTask->layer = 13;
            gCurTask->posX = (RandomRange(12) + 8) << 16;
            gCurTask->posY = (RandomRange(8) - 4) << 16;
            TaskSetMotion(0x10000, 0x4000, 0x5A5A5A5A, 0x8000, -0x800, 0x5A5A5A5A);
            gCurTask->frame = 4;
            TaskYieldTrampoline(2);
            gCurTask->frame = 5;
            TaskYieldTrampoline(2);
            gCurTask->frame = 6;
            TaskYieldTrampoline(2);
            gCurTask->frame = 7;
            TaskYieldTrampoline(2);
            gCurTask->frame = 8;
            TaskYieldTrampoline(2);
            gCurTask->posX = (RandomRange(12) + 10) << 16;
            gCurTask->posY = (RandomRange(8) - 4) << 16;
            TaskSetMotion(0x10000, 0x4000, 0x5A5A5A5A, -0x8000, 0x800, 0x5A5A5A5A);
            gCurTask->frame = 4;
            TaskYieldTrampoline(2);
            gCurTask->frame = 5;
            TaskYieldTrampoline(2);
            gCurTask->frame = 6;
            TaskYieldTrampoline(2);
            gCurTask->frame = 7;
            TaskYieldTrampoline(2);
            gCurTask->frame = 8;
            TaskYieldTrampoline(2);
            break;
        case 2:
            TaskStop();
            gCurTask->layer = 10;
            gCurTask->posX = (RandomRange(32) + 8) << 16;
            gCurTask->posY = (RandomRange(32) - 16) << 16;
            gCurTask->frame = 9;
            TaskYieldTrampoline(1);
            gCurTask->frame = 10;
            TaskYieldTrampoline(1);
            gCurTask->frame = 11;
            TaskYieldTrampoline(1);
            gCurTask->frame = 12;
            TaskYieldTrampoline(1);
            gCurTask->frame = 13;
            TaskYieldTrampoline(1);
            gCurTask->frame = 14;
            TaskYieldTrampoline(1);
            gCurTask->posX = (RandomRange(32) + 8) << 16;
            gCurTask->posY = (RandomRange(32) - 16) << 16;
            gCurTask->frame = 9;
            TaskYieldTrampoline(1);
            gCurTask->frame = 10;
            TaskYieldTrampoline(1);
            gCurTask->frame = 11;
            TaskYieldTrampoline(1);
            gCurTask->frame = 12;
            TaskYieldTrampoline(1);
            gCurTask->frame = 13;
            TaskYieldTrampoline(1);
            gCurTask->frame = 14;
            TaskYieldTrampoline(1);
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(15);
            gCurTask->posX = (RandomRange(32) + 8) << 16;
            gCurTask->posY = (RandomRange(32) - 16) << 16;
            gCurTask->frame = 9;
            TaskYieldTrampoline(1);
            gCurTask->frame = 10;
            TaskYieldTrampoline(1);
            gCurTask->frame = 11;
            TaskYieldTrampoline(1);
            gCurTask->frame = 12;
            TaskYieldTrampoline(1);
            gCurTask->frame = 13;
            TaskYieldTrampoline(1);
            gCurTask->frame = 14;
            TaskYieldTrampoline(1);
            gCurTask->frame = -1;
            TaskYieldTrampoline(30);
            break;
        case 3:
            gCurTask->unk46 = TaskCreateFrom(100, 32);
            gTasks[gCurTask->unk46].variant = 3;
            gTasks[gCurTask->unk46].unk74 = 2;
            gTasks[gCurTask->unk46].parent = gCurTaskIdx;
            TaskYieldTrampoline(16);
            gCurTask->unk46 = TaskCreateFrom(100, 32);
            gTasks[gCurTask->unk46].variant = 3;
            gTasks[gCurTask->unk46].unk74 = 1;
            gTasks[gCurTask->unk46].parent = gCurTaskIdx;
            TaskYieldTrampoline(16);
            gCurTask->unk46 = TaskCreateFrom(100, 32);
            gTasks[gCurTask->unk46].variant = 3;
            gTasks[gCurTask->unk46].unk74 = 0;
            gTasks[gCurTask->unk46].parent = gCurTaskIdx;
            TaskYieldTrampoline(8);
            gCurTask->unk46 = TaskCreateFrom(100, 32);
            gTasks[gCurTask->unk46].variant = 3;
            gTasks[gCurTask->unk46].unk74 = 0;
            gTasks[gCurTask->unk46].parent = gCurTaskIdx;
            TaskYieldTrampoline(8);
            break;
        }
    }
}

/* Task type #100 variant 3: a sprite that starts at the spawning task's
   (Task.parent) position, 0-15 pixels lower, moves right and plays one of
   three four-frame animations (Task.unk74) five times. */
void sub_080c7cc0(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 12;
    gCurTask->frameTable = gUnk_0874CF28;
    gCurTask->tileWord = 0;
    gCurTask->posX = gTasks[gCurTask->parent].pixelX << 16;
    gCurTask->posY = (gTasks[gCurTask->parent].pixelY + RandomRange(16) - 8) << 16;
    gCurTask->velX = 0x10000;
    gCurTask->accelX = 0x2000;
    switch (gCurTask->unk74) {
    case 0:
        gCurTask->unk6C = 0;
        do {
            gCurTask->frame = 15;
            TaskYieldTrampoline(3);
            gCurTask->frame = 16;
            TaskYieldTrampoline(3);
            gCurTask->frame = 17;
            TaskYieldTrampoline(3);
            gCurTask->frame = 18;
            TaskYieldTrampoline(3);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 1:
        gCurTask->unk6C = 0;
        do {
            gCurTask->frame = 19;
            TaskYieldTrampoline(3);
            gCurTask->frame = 20;
            TaskYieldTrampoline(3);
            gCurTask->frame = 21;
            TaskYieldTrampoline(3);
            gCurTask->frame = 22;
            TaskYieldTrampoline(3);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 2:
        gCurTask->unk6C = 0;
        do {
            gCurTask->frame = 23;
            TaskYieldTrampoline(3);
            gCurTask->frame = 24;
            TaskYieldTrampoline(3);
            gCurTask->frame = 25;
            TaskYieldTrampoline(3);
            gCurTask->frame = 26;
            TaskYieldTrampoline(3);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    }
    TaskExitTrampoline();
}
