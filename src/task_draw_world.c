/* game_code_early 0x08005D9C-0x08006464 (issue #32, batch G1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08005D9C 0x08006464 pending/task_draw_world.c --newpb
 * `--newpb` is a NEW recipe flag; see the report / build/scratch/fn_task_draw_world/
 * fnmatch.patch.  Without -fprologue-bugfix agbcc caches
 * current_function_has_far_jump across the far-jump scan and forces a spurious
 * leaf `push {lr}`; old_agbcc -O2 reproduces the missing push but narrows
 * `(s8)mem == 1` down to a bare `ldrb`+`cmp`, losing the ROM's
 * `lsls #24 / asrs #24`.  Only this recipe gets both right.
 *
 * The range holds 26 functions; symbols.csv lists 23 (TaskDrawWorldOrFree,
 * IsOnScreen and IsWorldPosOnScreen are unreferenced dead exports).
 */
#include "gba/gba.h"
#include "global.h"
#include "main.h"
#include "constants/sprites.h"

/* The draw code's view of struct Task (include/task.h): gCurTask is the
 * running task and gTasks[] the 64 control blocks (stride 0x90, indexed
 * by the same slot number used by TaskSetEntry).
 *
 * Evidence for the field types is in the ROM itself:
 *   frameTable  ldr  [p,#0x38] + ldr [base + idx*4] -> array of pointers
 *   frame       ldrsh, compared against -1          -> signed frame index
 *   facing      ldrb + lsls #24 + asrs #24          -> s8 (see the recipe note)
 *   pixelX      ldrsh / ldrh                        -> s16 world X
 *   pixelY      ldrsh / ldrh                        -> s16 world Y
 *   velX/Y, accelX/Y, speedLimitX/Y are s32 pairs cleared to 0 / 0x80000000.
 */
struct Sprite
{
    /*0x00*/ u8 filler00[0x10];
    /*0x10*/ u16 sleepFrames;
    /*0x12*/ u8 filler12[0x38 - 0x12];
    /*0x38*/ void **frameTable;
    /*0x3C*/ s16 frame;
    /*0x3E*/ u16 spriteFlags;
    /*0x40*/ u16 tileWord;
    /*0x42*/ u8 layer;
    /*0x43*/ s8 facing;
    /*0x44*/ u8 filler44[4];
    /*0x48*/ s16 pixelX;
    /*0x4A*/ s16 pixelY;
    /*0x4C*/ s32 posX;
    /*0x50*/ u8 filler50[4];
    /*0x54*/ s32 velX;
    /*0x58*/ s32 velY;
    /*0x5C*/ s32 accelX;
    /*0x60*/ s32 accelY;
    /*0x64*/ s32 speedLimitX;
    /*0x68*/ s32 speedLimitY;
    /*0x6C*/ u8 filler6C[0x90 - 0x6C];
};

/* Not from task.h: this file's view of gCurTask differs (lesson 3.517). */
extern struct Sprite *gCurTask;
extern struct Sprite gTasks[];
extern void *gTaskResumeAddrs[];
extern u32 gTaskStackPtrs[];
/* Camera scroll origin: subtracted from the world coordinates to get the
 * screen coordinates handed to QueueSprite. */
extern vs32 gCurTaskIdx;

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern void TaskFree(u32 a);
extern u8 TaskIsOnScreen(void);
extern u32 TaskLoadFrameTiles(u32 a);
extern u8 TaskIsInView(void);
extern void TaskYieldTrampoline(u32 a);

void TaskDrawWorld(void)
{
    struct Sprite *p;
    struct Sprite *q;
    void **tbl;

    p = gCurTask;
    if (p->frameTable == NULL) return;
    if (p->frame == -1) return;
    if (TaskIsOnScreen() == 0) return;
    q = gCurTask;
    tbl = q->frameTable;
    QueueSprite(q->layer, (u32)tbl[q->frame], q->spriteFlags, q->tileWord,
                 q->pixelX - gSpriteCameraX, (s16)(q->pixelY - gSpriteCameraY));
}

void TaskDrawWorldOrFree(void)
{
    struct Sprite *p;
    struct Sprite *q;
    void **tbl;

    p = gCurTask;
    if (p->frameTable == NULL) return;
    if (p->frame == -1) return;
    if (TaskIsOnScreen() != 0) {
        q = gCurTask;
        tbl = q->frameTable;
        QueueSprite(q->layer, (u32)tbl[q->frame], q->spriteFlags, q->tileWord,
                     q->pixelX - gSpriteCameraX,
                     (s16)(q->pixelY - gSpriteCameraY));
    } else {
        TaskFree(gCurTaskIdx);
    }
}

void TaskDrawWorldInView(void)
{
    struct Sprite *p;
    struct Sprite *q;
    void **tbl;

    p = gCurTask;
    if (p->frameTable == NULL) return;
    if (p->frame == -1) return;
    if (TaskIsInView() == 0) return;
    if (TaskIsOnScreen() == 0) return;
    q = gCurTask;
    tbl = q->frameTable;
    QueueSprite(q->layer, (u32)tbl[q->frame], q->spriteFlags, q->tileWord,
                 q->pixelX - gSpriteCameraX,
                 (s16)(q->pixelY - gSpriteCameraY));
}

void TaskDrawWorldInViewOrFree(void)
{
    struct Sprite *p;
    struct Sprite *q;
    void **tbl;

    p = gCurTask;
    if (p->frameTable == NULL) return;
    if (p->frame == -1) return;
    if (TaskIsInView() != 0) {
        if (TaskIsOnScreen() == 0) return;
        q = gCurTask;
        tbl = q->frameTable;
        QueueSprite(q->layer, (u32)tbl[q->frame], q->spriteFlags, q->tileWord,
                     q->pixelX - gSpriteCameraX,
                     (s16)(q->pixelY - gSpriteCameraY));
    } else {
        TaskFree(gCurTaskIdx);
    }
}

void TaskDrawWorldLoadTiles(void)
{
    struct Sprite *p;
    struct Sprite *q;
    u32 v;

    p = gCurTask;
    if (p->frameTable == NULL) return;
    if (p->frame == -1) return;
    if (TaskIsOnScreen() == 0) return;
    v = TaskLoadFrameTiles(0);
    q = gCurTask;
    QueueSprite(q->layer, v, q->spriteFlags, q->tileWord,
                 q->pixelX - gSpriteCameraX,
                 (s16)(q->pixelY - gSpriteCameraY));
}

void TaskDrawWorldTilesLoaded(void)
{
    struct Sprite *p;
    struct Sprite *q;
    void **tbl;
    u32 *r;

    p = gCurTask;
    if (p->frameTable == NULL) return;
    if (p->frame == -1) return;
    if (TaskIsOnScreen() == 0) return;
    q = gCurTask;
    tbl = q->frameTable;
    r = tbl[q->frame];
    QueueSprite(q->layer, *r, q->spriteFlags, q->tileWord,
                 q->pixelX - gSpriteCameraX,
                 (s16)(q->pixelY - gSpriteCameraY));
}

void sub_080060c0(void)
{
    struct Sprite *p;
    void **tbl;
    u16 t;

    p = gCurTask;
    if (p->frameTable == NULL) return;
    if (p->frame == -1) return;
    t = p->pixelX + 31;
    if (t > 302) return;
    if (p->pixelY <= -32) return;
    if (p->pixelY > 191) return;
    tbl = p->frameTable;
    QueueSprite(p->layer, (u32)tbl[p->frame], p->spriteFlags, p->tileWord,
                 p->pixelX, p->pixelY);
}

void TaskSleepForever(void)
{
    while (1)
        TaskYieldTrampoline(0x7FFF);
}

void TaskSetEntry(void *entry, u32 slot)
{
    gTasks[slot].sleepFrames = 0;
    gTaskResumeAddrs[slot] = entry;
    gTaskStackPtrs[slot] = (EWRAM_START + 0x3BFE0) + (slot << 8);
}

void TaskSetFrameByFacing(s16 frame)
{
    struct Sprite *p;

    p = gCurTask;
    if (p->facing == 1)
        p->frame = frame;
    else
        p->frame = frame | 1;
}

void TaskSetMotionX(s32 a, s32 b, s32 c)
{
    struct Sprite *p;

    p = gCurTask;
    p->velX = a;
    p->accelX = b;
    p->speedLimitX = abs(c);
}

void TaskSetMotionXFacing(s32 velX, s32 accelX)
{
    struct Sprite *p;

    p = gCurTask;
    if (p->facing == 1) {
        if (velX != 0x5A5A5A5A) p->velX = velX;
        if (accelX != 0x5A5A5A5A) gCurTask->accelX = accelX;
    } else {
        if (velX != 0x5A5A5A5A) p->velX = -velX;
        if (accelX != 0x5A5A5A5A) gCurTask->accelX = -accelX;
    }
}

void TaskStopX(void)
{
    struct Sprite *p;

    p = gCurTask;
    p->accelX = 0;
    p->velX = 0;
    p->speedLimitX = 0x80000000;
}

void TaskSetMotionY(s32 velY, s32 accelY, s32 speedLimitY)
{
    struct Sprite *p;

    p = gCurTask;
    p->velY = velY;
    p->accelY = accelY;
    p->speedLimitY = abs(speedLimitY);
}

void TaskStopY(void)
{
    struct Sprite *p;

    p = gCurTask;
    p->accelY = 0;
    p->velY = 0;
    p->speedLimitY = 0x80000000;
}

void TaskSetMotion(s32 velX, s32 accelX, s32 speedLimitX, s32 velY, s32 accelY, s32 speedLimitY)
{
    if (velX != 0x5A5A5A5A) gCurTask->velX = velX;
    if (accelX != 0x5A5A5A5A) gCurTask->accelX = accelX;
    if (speedLimitX != 0x5A5A5A5A) gCurTask->speedLimitX = abs(speedLimitX);
    if (velY != 0x5A5A5A5A) gCurTask->velY = velY;
    if (accelY != 0x5A5A5A5A) gCurTask->accelY = accelY;
    if (speedLimitY != 0x5A5A5A5A) gCurTask->speedLimitY = abs(speedLimitY);
}

void TaskStop(void)
{
    struct Sprite *p;

    p = gCurTask;
    p->accelY = 0;
    p->velY = 0;
    p->accelX = 0;
    p->velX = 0;
    p->speedLimitY = 0x80000000;
    p->speedLimitX = 0x80000000;
}

void TaskStopSlot(u32 i)
{
    struct Sprite *p;

    p = &gTasks[i];
    p->accelY = 0;
    p->velY = 0;
    p->accelX = 0;
    p->velX = 0;
    p->speedLimitY = 0x80000000;
    p->speedLimitX = 0x80000000;
}

void TaskUpdateFlip(void)
{
    struct Sprite *p;

    p = gCurTask;
    if (p->facing == 1)
        p->spriteFlags &= ~SPRITE_FLAG_FLIP_X;
    else
        p->spriteFlags |= SPRITE_FLAG_FLIP_X;
}

void TaskSetFrame(s32 frame)
{
    gCurTask->frame = frame;
    TaskUpdateFlip();
}

void TaskSetFrameNoFlip(s32 frame)
{
    struct Sprite *p;

    p = gCurTask;
    p->spriteFlags &= ~SPRITE_FLAG_FLIP_X;
    p->frame = frame;
}

void TaskSetFrameFlip(s32 frame)
{
    struct Sprite *p;

    p = gCurTask;
    p->spriteFlags |= SPRITE_FLAG_FLIP_X;
    p->frame = frame;
}

void TaskSetPosXFacing(u16 a)
{
    struct Sprite *p;

    p = gCurTask;
    if (p->facing == 1)
        p->posX = a << 16;
    else
        p->posX = -(a << 16);
}

void TaskStepForward(s16 a)
{
    struct Sprite *p;

    p = gCurTask;
    if (p->facing == 1)
        p->posX = (p->pixelX + a) << 16;
    else
        p->posX = (p->pixelX - a) << 16;
}

u8 IsOnScreen(s16 a, s16 b)
{
    u16 t;

    t = a + 63;
    if (t > 366) return 0;
    if (b <= -64) return 0;
    if (b > 223) return 0;
    return 1;
}

u8 IsWorldPosOnScreen(s16 a, s16 b)
{
    s16 x;
    s16 y;
    u16 t;

    x = a - gSpriteCameraX;
    y = b - gSpriteCameraY;
    t = x + 63;
    if (t > 366) return 0;
    if (y <= -64) return 0;
    if (y > 223) return 0;
    return 1;
}
