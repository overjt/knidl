#include "gba/gba.h"
#include "global.h"
#include "main.h"
#include "camera.h"

/*
 * Cooperative task system, user side (issue #32, batch F2:
 * 0x08005654-0x08005D9C).  Recipe: old_agbcc -O2 (`--old2`), established with
 * the leaf `push {lr}` fingerprint of lessons-learned 3.18 (TaskClampVelocity and
 * TaskIsOnScreen are leaves that end in a bare `bx lr`).
 *
 * The zone manages a 64-entry task table:
 *   gTaskCursor  s32   allocation cursor (rotates 0..63), volatile
 *   gTaskSlotTypes  s16[64] per-slot "type" (-1 = free)
 *   gTasks  struct Task[64] (0x90 bytes each) task control blocks
 *   gCurTask  struct Task *  currently running task
 *   gTaskCount  s32   live task count
 *   gTaskResumeAddrs (0x030025F0) u32[64] per-slot flag word from the ROM table
 *   gTaskStackPtrs  u32[64] per-slot 256-byte scratch pointer into 0x0203BFE0
 *   gTaskClassListLen  u8[]  per-class round-robin write index
 *   gTaskClassLists  u8[]  per-class slot list (class*64 + n)
 *   gTaskListRefs  u16[64] packed (class << 8) | n back-reference
 *   gTaskTypes  ROM table, 8 bytes per task type: u8 class, u32 flags
 */

struct Task
{
    /*0x00*/ u32 moveCallback;
    /*0x04*/ u32 updateCallback;
    /*0x08*/ u32 lateUpdateCallback;
    /*0x0C*/ u32 drawCallback;
    /*0x10*/ u16 sleepFrames;
    /*0x12*/ s8 taskClass;
    /*0x13*/ u8 skipMask;
    /*0x14*/ u8 state;
    /*0x15*/ u8 updateState;
    /*0x16*/ u16 serial;
    /*0x18*/ u32 unk18;
    /*0x1C*/ u32 unk1C;
    /*0x20*/ u32 unk20;
    /*0x24*/ u32 unk24;
    /*0x28*/ u32 unk28;
    /*0x2C*/ u32 unk2C;
    /*0x30*/ u32 unk30;
    /*0x34*/ u32 unk34;
    /*0x38*/ u32 *frameTable;
    /*0x3C*/ s16 frame;
    /*0x3E*/ u16 spriteFlags;
    /*0x40*/ u16 tileWord;
    /*0x42*/ u8 layer;
    /*0x43*/ u8 facing;
    /*0x44*/ s16 parent;
    /*0x46*/ s16 unk46;
    /*0x48*/ s16 pixelX;
    /*0x4A*/ s16 pixelY;
    /*0x4C*/ s32 posX;
    /*0x50*/ s32 posY;
    /*0x54*/ s32 velX;
    /*0x58*/ s32 velY;
    /*0x5C*/ s32 accelX;
    /*0x60*/ s32 accelY;
    /*0x64*/ s32 speedLimitX;
    /*0x68*/ s32 speedLimitY;
    /*0x6C*/ u16 unk6C;
    /*0x6E*/ u16 unk6E;
    /*0x70*/ u16 unk70;
    /*0x72*/ u8 actorKind;
    /*0x73*/ u8 variant;
    /*0x74*/ u8 unk74;
    /*0x75*/ u8 hitTimer;
    /*0x76*/ u16 unk76;
    /*0x78*/ u16 health;
    /*0x7A*/ u8 onGround;
    /*0x7B*/ u8 waterFlags;
    /*0x7C*/ u8 hitKind;
    /*0x7D*/ u8 hitDirection;
    /*0x7E*/ s8 hitterSlot;
    /*0x7F*/ s8 hitterPlayer;
    /*0x80*/ s8 u80;
    /*0x81*/ u8 unk81;
    /*0x82*/ u16 hitEffect;
    /*0x84*/ u16 unk84;
    /*0x86*/ u16 unk86;
    /*0x88*/ u32 player;
    /*0x8C*/ u32 u8C;
};

/* 8 bytes per task type in ROM at 0x0872FF30. */
struct TaskType
{
    /*0x00*/ u8 taskClass;
    /*0x01*/ u8 pad01[3];
    /*0x04*/ u32 entry;
};

/* Per-task graphics descriptor reached through Task.frameTable[Task.frame]. */
struct TaskGfx
{
    /*0x00*/ u32 oamTemplate;
    /*0x04*/ u16 *palette;
    /*0x08*/ u16 *tiles;
};

/* Not from task.h: this file's view of struct TaskType differs (lesson
   3.517). */
extern struct Task *gCurTask;
extern struct Task gTasks[];
extern struct TaskType gTaskTypes[];
extern vs32 gTaskCursor;
extern vs16 gTaskSlotTypes[];
extern vu32 gTaskCount;
extern vs32 gTaskRunPhase;
extern u32 gTaskStackPtrs[];
extern u8 gUnk_0203BFE0[];
extern u32 gTaskResumeAddrs[];
extern vs32 gCurTaskIdx;
extern vu8  gTaskClassLists[5][64];
extern vu8 gTaskClassListLen[];
extern vu16 gTaskListRefs[];

extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
/* NOTE: src/main_copy_queue.c declares the last parameter `u16 f`; the two call
 * sites in this file pass a sign-extended s16, so the real prototype must be
 * signed (see the report). */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);

void TaskFree(s32 id);
s32 TaskCreate(u32 type);
void TaskIntegrateMotion(void);
void TaskClampVelocity(void);

/* Free the task in slot `id`. */

/* Allocate a task of the given type; returns its slot index or -1. */

/* Allocate, optionally forcing the cursor to `idx` first. */

/* Allocate, scanning [start, end] for a free slot first. */

/* Clamp the running task's velocity to its per-axis maximum. */

/* Integrate acceleration into velocity and velocity into position. */

/* Task body: integrate, then publish the 16.16 position as screen coords. */

/* Task body: integrate if moving, then publish position relative to the
 * parent task's position (Task.parent indexes the task array). */

/* Hidden (unreferenced) export inside TaskMoveRelativeToParent's symbols.csv size. */

/* Task body: integrate, publish position relative to the camera. */

/* Is the running task on screen (with a 63/64-pixel margin) relative to
 * gSpriteCameraX/gSpriteCameraY? */

/* Hidden (unreferenced) export inside TaskIsOnScreen's symbols.csv size:
 * upload the running task's tile stream plus its palette. */

/* Upload the running task's tile stream. */

/* Is the running task inside the rectangle at gViewRect (+/- 64)? */
u32 TaskIsInView(void)
{
    if (gViewRect[0] - 64 >= gCurTask->pixelX)
        return 0;
    if (gCurTask->pixelX >= gViewRect[1] + 64)
        return 0;
    if (gViewRect[2] - 64 >= gCurTask->pixelY)
        return 0;
    if (gCurTask->pixelY >= gViewRect[3] + 64)
        return 0;
    return 1;
}

/* Task body: enqueue the running task's sprite if it is on screen. */
void TaskDrawScreen(void)
{
    struct Task *t;
    s16 y;

    t = gCurTask;
    if (t->frameTable == 0)
        return;
    if (t->frame == -1)
        return;
    if ((u16)(t->pixelX + 63) > 366)
        return;
    y = t->pixelY;
    if (y <= -64)
        return;
    if (y > 223)
        return;
    QueueSprite(t->layer, t->frameTable[t->frame], t->spriteFlags, t->tileWord, t->pixelX, t->pixelY);
}

/* Same, but free the task when it leaves the screen. */
void TaskDrawScreenOrFree(void)
{
    struct Task *t;
    s16 y;

    t = gCurTask;
    if (t->frameTable == 0)
        return;
    if (t->frame == -1)
        return;
    if ((u16)(t->pixelX + 63) > 366)
        goto kill;
    y = t->pixelY;
    if (y <= -64)
        goto kill;
    if (y > 223)
        goto kill;
    QueueSprite(t->layer, t->frameTable[t->frame], t->spriteFlags, t->tileWord, t->pixelX, t->pixelY);
    return;
kill:
    TaskFree(gCurTaskIdx);
}
