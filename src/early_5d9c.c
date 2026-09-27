/* game_code_early 0x08005D9C-0x08006464 (issue #32, batch G1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08005D9C 0x08006464 pending/early_5d9c.c --newpb
 * `--newpb` is a NEW recipe flag; see the report / build/scratch/fn_early_5d9c/
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

/* Sprite / draw-request record.  gCurTask points at the entry that is
 * currently being filled in; gTasks[] is the array those entries live
 * in (stride 0x90, indexed by the same slot number used by TaskSetEntry).
 *
 * Evidence for the field types is in the ROM itself:
 *   unk38  ldr  [p,#0x38] + ldr [base + idx*4]   -> array of pointers
 *   unk3C  ldrsh, compared against -1            -> signed slot index
 *   unk43  ldrb + lsls #24 + asrs #24            -> s8 (see the recipe note)
 *   unk48  ldrsh / ldrh                          -> s16 world X
 *   unk4A  ldrsh / ldrh                          -> s16 world Y
 *   unk54/58/5C/60/64/68 are s32 pairs that are cleared to 0 / 0x80000000.
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

extern struct Sprite *gCurTask;
extern struct Sprite gTasks[];
extern void *gTaskResumeAddrs[];
extern u32 gTaskStackPtrs[];
/* Camera scroll origin: subtracted from the world coordinates to get the
 * screen coordinates handed to QueueSprite. */
extern s16 gSpriteCameraX;
extern s16 gSpriteCameraY;
extern u32 gCurTaskIdx;

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

void TaskSetEntry(void *a, u32 i)
{
    gTasks[i].sleepFrames = 0;
    gTaskResumeAddrs[i] = a;
    gTaskStackPtrs[i] = 0x0203BFE0 + (i << 8);
}

void TaskSetFrameByFacing(s16 a)
{
    struct Sprite *p;

    p = gCurTask;
    if (p->facing == 1)
        p->frame = a;
    else
        p->frame = a | 1;
}

void TaskSetMotionX(s32 a, s32 b, s32 c)
{
    struct Sprite *p;

    p = gCurTask;
    p->velX = a;
    p->accelX = b;
    p->speedLimitX = abs(c);
}

void TaskSetMotionXFacing(s32 a, s32 b)
{
    struct Sprite *p;

    p = gCurTask;
    if (p->facing == 1) {
        if (a != 0x5A5A5A5A) p->velX = a;
        if (b != 0x5A5A5A5A) gCurTask->accelX = b;
    } else {
        if (a != 0x5A5A5A5A) p->velX = -a;
        if (b != 0x5A5A5A5A) gCurTask->accelX = -b;
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

void TaskSetMotionY(s32 a, s32 b, s32 c)
{
    struct Sprite *p;

    p = gCurTask;
    p->velY = a;
    p->accelY = b;
    p->speedLimitY = abs(c);
}

void TaskStopY(void)
{
    struct Sprite *p;

    p = gCurTask;
    p->accelY = 0;
    p->velY = 0;
    p->speedLimitY = 0x80000000;
}

void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f)
{
    if (a != 0x5A5A5A5A) gCurTask->velX = a;
    if (b != 0x5A5A5A5A) gCurTask->accelX = b;
    if (c != 0x5A5A5A5A) gCurTask->speedLimitX = abs(c);
    if (d != 0x5A5A5A5A) gCurTask->velY = d;
    if (e != 0x5A5A5A5A) gCurTask->accelY = e;
    if (f != 0x5A5A5A5A) gCurTask->speedLimitY = abs(f);
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
        p->spriteFlags &= 0x7FFF;
    else
        p->spriteFlags |= 0x8000;
}

void TaskSetFrame(s32 a)
{
    gCurTask->frame = a;
    TaskUpdateFlip();
}

void TaskSetFrameNoFlip(s32 a)
{
    struct Sprite *p;

    p = gCurTask;
    p->spriteFlags &= 0x7FFF;
    p->frame = a;
}

void TaskSetFrameFlip(s32 a)
{
    struct Sprite *p;

    p = gCurTask;
    p->spriteFlags |= 0x8000;
    p->frame = a;
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
