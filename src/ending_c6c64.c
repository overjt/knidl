#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* ending_c6c64.c (0x080C6C64-0x080C7E4B, issue #100).
 *
 * The first ending scene, part 1: task type #100 (class 3), which M37's
 * sub_080c62f0 spawns in AgbMain state 11 and waits on (gEndingSceneActive).
 *   sub_080c6c64   the body: variant 0 (Task.unk73 == 0) loads the graphics
 *       (sub_080c6ca0) and spawns variants 1, 6, 7, 9 and 10 from the list
 *       gUnk_0875735C (sub_080c6d38); variants 1-10 run gUnk_08757330[unk73].
 *   sub_080c6d84 / sub_080c769c   variant 1, the scene's main sprite, and its
 *       scaling draw callback; sub_080c77cc spawns its four variant-2 helpers.
 *   sub_080c7810   variant 2: while the spawner is drawn (Task.unk34), one of
 *       four effects by Task.unk74 (a shaking sprite, drifting puffs, spark
 *       bursts, four variant-3 sprites).
 *   sub_080c7cc0   variant 3: a sprite that drifts right from its spawner and
 *       plays one of three animations five times. */

extern u16 gUnk_02000028;
extern u16 gUnk_02004C94;
extern u32 gUnk_02020000[];         /* decompression buffer */
extern u16 gUnk_03001570[];         /* palette buffer */
extern u32 gObjVram[];         /* OBJ VRAM */
extern u16 gPlayerPalettes[][16];     /* per-player palettes */
extern void (*gUnk_08757330[])(void);
extern struct GfxHeader gUnk_085995AC;
extern struct GfxHeader gUnk_0859990C;
extern u16 gUnk_0875735C[];
extern u32 gUnk_08755708[];
extern u32 gUnk_080DBEF8[];
extern u32 gUnk_080DBF00[];
extern u32 gUnk_080DBF08[];
extern u32 gUnk_080DBF18[];
extern u32 gUnk_080DBF28[];
extern u32 gUnk_080DBF30[];
extern u8 gEndingSceneActive;
extern s16 gUnk_0873FF98[];
extern u32 gUnk_0874CF28[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 (lesson 3.428) */
s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
u32 RandomRange(u32 range);                                 /* random 0 .. range-1 */
s32 PlayBgm(s32 songId);
s32 PlaySfx(s32 id);                                    /* play a sound effect */
s32 TaskCreateFrom(u32 type, s32 idx);                         /* spawn a task */
void TaskMove(void);
void TaskMoveRelativeToParent(void);
void TaskDrawScreen(void);
void TaskSleepForever(void);                                     /* end the running task */
void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void TaskStop(void);
void LoadGfxSet(u16 a0);                                   /* load screen graphics */
void sub_080c6ca0(void);
void sub_080c6d38(void);
void sub_080c769c(void);
void sub_080c77cc(void);

/* Task type #100 (class 3): variant 0 loads the graphics and spawns the
   other variants; variants 1-10 run the anchor table gUnk_08757330[]. */
void sub_080c6c64(void)
{
    gCurTask->unk00 = 0;
    gCurTask->unk0C = 0;
    if (gCurTask->unk73 == 0) {
        sub_080c6ca0();
        sub_080c6d38();
    } else {
        CallTableEntry(gCurTask->unk73, 11, gUnk_08757330);
    }
    TaskExitTrampoline();
}

/* Task type #100's graphics: two sprite sheets, the first one's palette and,
   with more than one player, this player's palette in slot 1. */
void sub_080c6ca0(void)
{
    struct GfxHeader *h = &gUnk_085995AC;

    LoadGfxSet(0);
    LZ77UnCompWram(h->unk0C, gUnk_02020000);
    RequestCopy(4, (u32)gUnk_02020000, (u32)gObjVram, h->unk02 << 5);
    RequestCopy(2, (u32)h->unk08, (u32)gUnk_03001570, h->unk00 << 5);
    if (gUnk_02004C94 > 1)
        RequestCopy(2, (u32)gPlayerPalettes[gUnk_02000028], (u32)&gUnk_03001570[16], 22);
    h = &gUnk_0859990C;
    LZ77UnCompWram(h->unk0C, gUnk_02020000);
    RequestCopy(3, (u32)gUnk_02020000, (u32)gObjVram, h->unk02 << 5);
    LoadGfxSet(71);
}

/* Spawn one task type #100 per variant listed in gUnk_0875735C[] (1, 6, 7,
   9 and 10, ended by 11, the anchor table's size). */
void sub_080c6d38(void)
{
    s32 i;
    s32 id;
    s32 v;
    struct Task *t;

    for (i = 0; v = gUnk_0875735C[i], (s16)gUnk_0875735C[i] <= 10; i++) {
        id = TaskCreateFrom(100, 32);
        if (id == -1)
            for (;;)
                ;
        t = &gTasks[id];
        t->unk73 = v;
    }
}

/* Task type #100 variant 1, the scene's main sprite: spawns variant 4 and
   the four variant-2 helpers (sub_080c77cc), draws itself through the
   scaling callback sub_080c769c, runs its timed motion phases and ends the
   scene by clearing gEndingSceneActive, which M37's sub_080c62f0 waits for. */
void sub_080c6d84(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)sub_080c769c;
    gCurTask->layer = 8;
    gCurTask->unk38 = gUnk_08755708;
    gCurTask->unk40 = 0x8810;
    gCurTask->unk46 = TaskCreateFrom(100, 32);
    gTasks[gCurTask->unk46].unk73 = 4;
    gTasks[gCurTask->unk46].unk44 = gCurTaskIdx;
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
    QueueSprite(12, (u32)gUnk_080DBEF8, 0, 0, gCurTask->unk48, gCurTask->unk4A);
    TaskYieldTrampoline(1);
    QueueSprite(12, (u32)gUnk_080DBF00, 0, 0, gCurTask->unk48, gCurTask->unk4A);
    TaskYieldTrampoline(1);
    QueueSprite(12, (u32)gUnk_080DBF08, 0, 0, gCurTask->unk48, gCurTask->unk4A);
    TaskYieldTrampoline(1);
    QueueSprite(12, (u32)gUnk_080DBF18, 0, 0, gCurTask->unk48, gCurTask->unk4A);
    TaskYieldTrampoline(1);
    QueueSprite(12, (u32)gUnk_080DBF28, 0, 0, gCurTask->unk48, gCurTask->unk4A);
    TaskYieldTrampoline(1);
    QueueSprite(12, (u32)gUnk_080DBF30, 0, 0, gCurTask->unk48, gCurTask->unk4A);
    TaskYieldTrampoline(1);
    TaskYieldTrampoline(16);
    gCurTask->frame = 62;
    PlaySfx(281);
    gCurTask->unk18 = 0x30000;
    gCurTask->unk28 = 0x1000;
    TaskStop();
    TaskYieldTrampoline(4);
    gCurTask->unk54 = -0x2000;
    gCurTask->unk58 = -0x4000;
    TaskYieldTrampoline(80);
    gCurTask->unk54 = -0x1000;
    TaskYieldTrampoline(24);
    gCurTask->unk54 = -0x800;
    TaskYieldTrampoline(24);
    gCurTask->unk54 = 0x800;
    TaskYieldTrampoline(24);
    gCurTask->unk54 = 0x1000;
    TaskYieldTrampoline(24);
    gCurTask->unk54 = 0x2000;
    TaskYieldTrampoline(24);
    gCurTask->unk54 = 0x4000;
    TaskYieldTrampoline(24);
    gCurTask->unk54 = 0x8000;
    TaskYieldTrampoline(24);
    gCurTask->frame = -1;
    TaskStop();
    gCurTask->unk54 = 0x40000;
    TaskYieldTrampoline(28);
    gCurTask->unk54 = -0x6000;
    gCurTask->unk58 = 0x4000;
    TaskYieldTrampoline(32);
    PlayBgm(24);
    gCurTask->frame = 62;
    gCurTask->unk54 = 0x8000;
    gCurTask->unk58 = 0x4000;
    TaskYieldTrampoline(48);
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(32);
    gCurTask->unk58 = 0xC000;
    TaskYieldTrampoline(16);
    gCurTask->frame = -1;
    gCurTask->unk54 = -0x10000;
    gCurTask->unk58 = 0x40000;
    TaskYieldTrampoline(14);
    gCurTask->unk54 = 0x6000;
    gCurTask->unk58 = -0x6000;
    TaskYieldTrampoline(32);
    gCurTask->unk34 = 1;
    gCurTask->frame = 62;
    gCurTask->unk18 = 0x3F0000;
    gCurTask->unk28 = 0;
    gCurTask->unk54 = -0x20000;
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(32);
    gCurTask->unk58 = 0x4000;
    TaskYieldTrampoline(16);
    gCurTask->unk58 = -0x4000;
    TaskYieldTrampoline(16);
    gCurTask->unk58 = -0x8000;
    TaskYieldTrampoline(16);
    gCurTask->unk54 = -0x10000;
    gCurTask->unk58 = -0x10000;
    TaskYieldTrampoline(16);
    gCurTask->unk54 = -0x8000;
    gCurTask->unk58 = -0xC000;
    TaskYieldTrampoline(16);
    gCurTask->unk54 = -0x4000;
    gCurTask->unk58 = -0x8000;
    TaskYieldTrampoline(16);
    gCurTask->unk54 = 0x4000;
    gCurTask->unk58 = -0x4000;
    TaskYieldTrampoline(16);
    gCurTask->unk58 = 0x4000;
    TaskYieldTrampoline(64);
    gCurTask->unk6C = 0;
    do {
        TaskStop();
        gCurTask->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x800;
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(32);
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        TaskStop();
        gCurTask->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x800;
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(32);
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        TaskStop();
        gCurTask->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    gCurTask->unk6C = 0;
    do {
        TaskStop();
        gCurTask->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x800;
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(32);
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        TaskStop();
        gCurTask->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x800;
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(32);
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        TaskStop();
        gCurTask->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 5);
    gCurTask->unk54 = 0x400;
    gCurTask->unk58 = 0x2000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x800;
    gCurTask->unk58 = 0x4000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(32);
    gCurTask->unk58 = 0x4000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x400;
    gCurTask->unk58 = 0x2000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = -0x2000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x800;
    gCurTask->unk58 = -0x4000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = -0x8000;
    TaskYieldTrampoline(32);
    gCurTask->unk58 = -0x4000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x400;
    gCurTask->unk58 = -0x2000;
    TaskYieldTrampoline(8);
    TaskStop();
    gCurTask->unk58 = -0x2000;
    TaskYieldTrampoline(8);
    gCurTask->unk6C = 0;
    do {
        TaskStop();
        gCurTask->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x800;
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(32);
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        TaskStop();
        gCurTask->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x800;
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(32);
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        TaskStop();
        gCurTask->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskStop();
    gCurTask->unk58 = 0x2000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x400;
    gCurTask->unk58 = 0x4000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x800;
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(28);
    gCurTask->unk54 = 0x400;
    gCurTask->unk58 = 0x4000;
    TaskYieldTrampoline(8);
    TaskStop();
    gCurTask->unk58 = 0x2000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = -0x2000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x400;
    gCurTask->unk58 = -0x4000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x800;
    gCurTask->unk58 = -0x8000;
    TaskYieldTrampoline(28);
    gCurTask->unk54 = 0x400;
    gCurTask->unk58 = -0x4000;
    TaskYieldTrampoline(8);
    TaskStop();
    gCurTask->unk58 = -0x2000;
    TaskYieldTrampoline(8);
    gCurTask->unk34 = 1;
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x800;
        gCurTask->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(32);
        gCurTask->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x800;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(24);
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x400;
        gCurTask->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 10);
    gCurTask->unk54 = -0x8000;
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = -0x10000;
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = -0x20000;
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(12);
    gCurTask->unk58 = -0x8000;
    TaskYieldTrampoline(12);
    gCurTask->unk54 = -0x10000;
    gCurTask->unk58 = -0x10000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = -0x8000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x8000;
    gCurTask->unk58 = -0x20000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x10000;
    gCurTask->unk58 = -0x10000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x20000;
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(40);
    gCurTask->unk54 = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x8000;
    gCurTask->unk58 = -0x2000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x2000;
    gCurTask->unk58 = -0x8000;
    TaskYieldTrampoline(8);
    TaskStop();
    TaskYieldTrampoline(16);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = -0x20000;
    TaskYieldTrampoline(2);
    TaskStop();
    TaskYieldTrampoline(60);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = -0x20000;
    TaskYieldTrampoline(2);
    TaskStop();
    TaskYieldTrampoline(14);
    TaskYieldTrampoline(68);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = -0x20000;
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
    if (gCurTask->unk38 != NULL && gCurTask->frame != -1) {
        n = 0;
        if ((s16)gCurTask->unk70 > 1)
            n = 1;
        if ((s16)++gCurTask->unk70 > 5)
            gCurTask->unk70 = 0;
        t = gCurTask;
        if ((u16)(t->unk48 + 63) <= 366 && t->unk4A > -64 && t->unk4A < 224) {
            tbl = t->unk38;
            scale = gUnk_0873FF98[t->unk18 >> 16];
            if ((u16)scale != 0x100)
                QueueSprite(gCurTask->layer, DrawAffineSprite(tbl[t->frame + n], scale, scale, 0),
                             gCurTask->unk3E, gCurTask->unk40, gCurTask->unk48,
                             gCurTask->unk4A);
            else
                QueueSprite(t->layer, tbl[t->frame + n], t->unk3E, t->unk40, t->unk48, t->unk4A);
        }
    }
}

/* Spawn the four task type #100 variant-2 helpers of variant 1 (Task.unk74
   = 0..3, Task.unk44 = the calling task). */
void sub_080c77cc(void)
{
    s32 i;
    s32 id;
    struct Task *t;

    for (i = 0; i <= 3; i++) {
        id = TaskCreateFrom(100, 32);
        t = &gTasks[id];
        t->unk73 = 2;
        t->unk74 = i;
        t->unk44 = gCurTaskIdx;
    }
}

/* Task type #100 variant 2 (four of them, spawned by variant 1's
   sub_080c77cc with Task.unk74 = 0..3 and Task.unk44 = variant 1's task):
   while the spawner has graphics (Task.unk34), each one plays its own
   effect relative to it - 0 a shaking sprite, 1 two drifting puffs, 2
   three bursts of sparks, 3 spawns four variant 3 sprites. */
void sub_080c7810(void)
{
    gCurTask->unk00 = (u32)TaskMoveRelativeToParent;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk38 = gUnk_0874CF28;
    gCurTask->unk40 = 0;
    for (;;) {
        if (gTasks[gCurTask->unk44].unk34 == 0) {
            TaskYieldTrampoline(1);
            continue;
        }
        switch (gCurTask->unk74) {
        case 0:
            TaskStop();
            gCurTask->layer = 12;
            gCurTask->posX = 8 << 16;
            gCurTask->posY = 0;
            gCurTask->unk54 = 0x10000;
            gCurTask->frame = 0;
            TaskYieldTrampoline(1);
            gCurTask->unk54 = -0x10000;
            gCurTask->frame = 0;
            TaskYieldTrampoline(1);
            gCurTask->unk54 = 0x10000;
            gCurTask->frame = 1;
            TaskYieldTrampoline(1);
            gCurTask->unk54 = -0x10000;
            gCurTask->frame = 1;
            TaskYieldTrampoline(1);
            gCurTask->unk54 = 0x10000;
            gCurTask->frame = 2;
            TaskYieldTrampoline(1);
            gCurTask->unk54 = -0x10000;
            gCurTask->frame = 2;
            TaskYieldTrampoline(1);
            gCurTask->unk54 = 0x10000;
            gCurTask->frame = 3;
            TaskYieldTrampoline(1);
            gCurTask->unk54 = -0x10000;
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
            gTasks[gCurTask->unk46].unk73 = 3;
            gTasks[gCurTask->unk46].unk74 = 2;
            gTasks[gCurTask->unk46].unk44 = gCurTaskIdx;
            TaskYieldTrampoline(16);
            gCurTask->unk46 = TaskCreateFrom(100, 32);
            gTasks[gCurTask->unk46].unk73 = 3;
            gTasks[gCurTask->unk46].unk74 = 1;
            gTasks[gCurTask->unk46].unk44 = gCurTaskIdx;
            TaskYieldTrampoline(16);
            gCurTask->unk46 = TaskCreateFrom(100, 32);
            gTasks[gCurTask->unk46].unk73 = 3;
            gTasks[gCurTask->unk46].unk74 = 0;
            gTasks[gCurTask->unk46].unk44 = gCurTaskIdx;
            TaskYieldTrampoline(8);
            gCurTask->unk46 = TaskCreateFrom(100, 32);
            gTasks[gCurTask->unk46].unk73 = 3;
            gTasks[gCurTask->unk46].unk74 = 0;
            gTasks[gCurTask->unk46].unk44 = gCurTaskIdx;
            TaskYieldTrampoline(8);
            break;
        }
    }
}

/* Task type #100 variant 3: a sprite that starts at the spawning task's
   (Task.unk44) position, 0-15 pixels lower, moves right and plays one of
   three four-frame animations (Task.unk74) five times. */
void sub_080c7cc0(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->layer = 12;
    gCurTask->unk38 = gUnk_0874CF28;
    gCurTask->unk40 = 0;
    gCurTask->posX = gTasks[gCurTask->unk44].unk48 << 16;
    gCurTask->posY = (gTasks[gCurTask->unk44].unk4A + RandomRange(16) - 8) << 16;
    gCurTask->unk54 = 0x10000;
    gCurTask->unk5C = 0x2000;
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
