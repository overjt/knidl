#include "gba/gba.h"
#include "global.h"

/* Early subsystem: the cooperative TASK ENGINE (0x08004FEC-0x08005653,
 * issue #32 batch F1).
 *
 * 64 task slots of 0x90 bytes live at 0x03002790.  Each slot carries four
 * callbacks (+0x00/+0x04/+0x08/+0x0C), a countdown (+0x10), a priority-group
 * id (+0x12, negative = free) and a per-phase skip mask (+0x13).  Slots are
 * bucketed into five priority groups: gTaskClassLists[group][slot] holds the
 * task ids, gTaskClassPassEnd[group] the live count, gTaskClassListLen/gTaskClassPassStart
 * the pending/processed counts used to detect list growth during a pass.
 * gTaskResumeAddrs[id]/gTaskStackPtrs[id] are the coroutine resume PC/SP consumed
 * by the ARM task switcher at 0x08000234 (reached through its thumb veneer
 * TaskSwitchTrampoline); gTaskSavedR0 is that switcher's "sleep" result.
 *
 *   InitTasks  cold init: clears the bucket tables, the id map and every
 *                 task slot (two CpuSet fills plus a per-slot field reset).
 *   RunTasks  the per-frame driver (called from RunLinkFrame): rebuilds
 *                 the buckets from the slot table, then runs phases 1..5 --
 *                 resume/countdown + callback[0]/[1], then callback[2] and
 *                 callback[3] -- restarting whenever a callback added tasks.
 *   TaskSetSkipMask  set one task's +0x13 skip mask.
 *   TaskSetOthersSkipMask  set +0x13 on every allocated task, preserving one slot's.
 *   TaskSetAllSkipMask  set +0x13 on every allocated task (dead export).
 *
 * Recipe: old_agbcc -O2 -mthumb-interwork (fnmatch --old2).  Evidence: the
 * leaf TaskSetSkipMask ends in a bare `bx lr`; agbcc always emits
 * `push {lr}` / `pop {r0}; bx r0` even for leaves.
 *
 * Matching notes (docs/lessons-learned.md §3):
 *  - TaskSetAllSkipMask is a dead export hidden inside symbols.csv's 0x90 size for
 *    TaskSetOthersSkipMask (lesson 2.13 / zone lesson 14): nothing in ROM calls it.
 *  - `gTaskListRefs[i] = 0xFFFF; gTaskSlotTypes[i] = gTaskListRefs[i];` is the
 *    shape behind the ROM's `ldrh/orrs/strh` triplet: agbcc emits the
 *    volatile indexed store's dead pre-read (3.7) and then REUSES that
 *    register by OR-ing the all-ones constant into it instead of
 *    materialising a fresh value.  Spelling the statement `|= 0xFFFF` adds a
 *    second `ldrh`.
 *  - `p->w88 = p->w8C = 0;` and
 *    `gTaskBaseSp = gTaskSavedSp = gTaskSavedLr = gTaskSavedR0 = 0;`
 *    are chains: agbcc materialises the lvalue ADDRESSES left-to-right and
 *    performs the STORES right-to-left, which is exactly the interleaving the
 *    ROM shows.  Separate statements give address/store pairs instead.
 *  - `fill` must be `vu16`: a plain `u16` stack temp makes agbcc load 0xFFFF
 *    straight into the destination, while the ROM shows the movhi scratch
 *    pair `ldr rS,=0xFFFF; adds rD,rS,#0` (3.24).
 *  - the restart of RunTasks's phase-1..3 pass is a `goto`, not a
 *    do/while: a loop note re-weights every reference inside it by one more
 *    loop level and moves three long-lived address pseudos onto different
 *    hard registers (3.21 applied to allocation rather than to hoisting).
 *  - RunTasks folds the restart counter into `j`; that is what raises
 *    j's global-alloc priority past &gTaskSavedR0's and puts j on r5.
 *
 * STATUS: InitTasks, TaskSetSkipMask, TaskSetOthersSkipMask and TaskSetAllSkipMask are
 * byte-exact.  RunTasks reproduces the ROM's instruction sequence
 * one-for-one but diverges on register NAMES only (908 vs 904 bytes).  Root
 * cause: agbcc's local allocator gives the current-task pointer r1 (reusing
 * the dying `ldr r1,=gTasks`) where the ROM uses a fresh r2; that
 * pushes the `task->b13` temp from r1 to r6, denies r6 to the phase-4/5
 * &gCurTask pseudo, evicts &gCurTaskListPos from r8 into the
 * caller-clobbered r3 and so costs one extra `mov` plus a second caller-save
 * slot (`sub sp,#8`).  No source spelling tried moves that one local-alloc
 * decision - see the batch report for the list.
 */

struct Task {
    /* 0x00 */ void (*moveCallback)(void);
    /* 0x04 */ void (*updateCallback)(void);
    /* 0x08 */ void (*lateUpdateCallback)(void);
    /* 0x0C */ void (*drawCallback)(void);
    /* 0x10 */ s16 sleepFrames;
    /* 0x12 */ u8  taskClass;
    /* 0x13 */ u8  skipMask;
    /* 0x14 */ u8  state;
    /* 0x15 */ u8  updateState;
    /* 0x16 */ u16 serial;
    /* 0x18 */ u32 w18[8];
    /* 0x38 */ u32 frameTable;
    /* 0x3C */ u16 frame;
    /* 0x3E */ u16 spriteFlags;
    /* 0x40 */ u16 tileWord;
    /* 0x42 */ u8  layer;
    /* 0x43 */ u8  facing;
    /* 0x44 */ u16 parent;
    /* 0x46 */ u16 h46;
    /* 0x48 */ u16 pixelX;
    /* 0x4A */ u16 pixelY;
    /* 0x4C */ u32 posX;
    /* 0x50 */ u32 posY;
    /* 0x54 */ u32 velX;
    /* 0x58 */ u32 velY;
    /* 0x5C */ u32 accelX;
    /* 0x60 */ u32 accelY;
    /* 0x64 */ u32 speedLimitX;
    /* 0x68 */ u32 speedLimitY;
    /* 0x6C */ u16 h6C;
    /* 0x6E */ u16 h6E;
    /* 0x70 */ u16 h70;
    /* 0x72 */ u8  actorKind;
    /* 0x73 */ u8  variant;
    /* 0x74 */ u8  b74;
    /* 0x75 */ u8  hitTimer;
    /* 0x76 */ u16 h76;
    /* 0x78 */ u16 health;
    /* 0x7A */ u8  onGround;
    /* 0x7B */ u8  waterFlags;
    /* 0x7C */ u8  hitKind;
    /* 0x7D */ u8  hitDirection;
    /* 0x7E */ u8  hitterSlot;
    /* 0x7F */ u8  hitterPlayer;
    /* 0x80 */ u8  b80;
    /* 0x81 */ u8  b81;
    /* 0x82 */ u16 h82;
    /* 0x84 */ u16 h84;
    /* 0x86 */ u16 h86;
    /* 0x88 */ u32 player;
    /* 0x8C */ u32 w8C;
};


extern struct Task gTasks[];
extern vu16 gTaskSlotTypes[];
extern vu32 gTaskCount;
extern vs32 gTaskRunPhase;
extern vu32 gTaskSavedLr;
extern vu32 gTaskSavedSp;
extern vu8  gTaskClassListPos[];
extern vu8  gTaskClassPassEnd[];
extern vu32 gCurTaskListPos;
extern vs32 gCurTaskIdx;
extern struct Task *gCurTask;
extern vu32 gTaskCursor;
extern vu8  gTaskClassLists[5][64];
extern s32  gTaskSavedR0;
extern u32  gTaskResumeAddrs[];
extern vu8  gTaskClassListLen[];
extern vu8  gTaskClassPassStart[];
extern vu16 gTaskListRefs[];
extern u32  gTaskStackPtrs[];
extern vs32 gCurTaskClass;
extern vu32 gTaskBaseSp;
extern vu8  gTaskSkipMaskDepth;

void TaskSwitchTrampoline(s32 id, u32 fn, u32 stack);

void InitTasks(void)
{
    u32 i;
    u32 j;
    vu16 fill;
    u16 fill2;

    gCurTaskIdx = gTaskRunPhase = gCurTaskClass = -1;
    gTaskCount = gTaskCursor = 0;

    for (i = 0; i < 5; i++) {
        for (j = 0; j < 64; j++)
            gTaskClassLists[i][j] = 0;
        gTaskClassPassStart[i] = gTaskClassListPos[i] = gTaskClassListLen[i] = gTaskClassPassEnd[i] = 0;
    }

    gTaskBaseSp = gTaskSavedSp = gTaskSavedLr = gTaskSavedR0 = 0;

    for (i = 0; i < 64; i++) {
        gTaskSlotTypes[i] = gTaskListRefs[i] = 0xFFFF;
        gTaskResumeAddrs[i] = 0;
    }

    fill = 0xFFFF;
    CpuSet((const void *)&fill, (void *)0x0203BF00, 0x01002000);
    fill2 = 0;
    CpuSet(&fill2, gTasks, 0x01001200);

    for (i = 0; i < 64; i++) {
        struct Task *p = &gTasks[i];

        p->taskClass |= 0xFF;
        p->sleepFrames = 0;
        p->drawCallback = 0;
        p->lateUpdateCallback = 0;
        p->updateCallback = 0;
        p->moveCallback = 0;
        p->serial = 0;
        p->updateState = 0;
        p->state = 0;
        p->skipMask = 0;
        p->w18[7] = 0;
        p->w18[6] = 0;
        p->w18[5] = 0;
        p->w18[4] = 0;
        p->w18[3] = 0;
        p->w18[2] = 0;
        p->w18[1] = 0;
        p->w18[0] = 0;
        p->frame |= 0xFFFF;
        p->layer = 0;
        p->frameTable = 0;
        p->facing = 0;
        p->tileWord = 0;
        p->spriteFlags = 0;
        p->h46 |= 0xFFFF;
        p->parent |= 0xFFFF;
        p->posY = 0;
        p->posX = 0;
        p->pixelY = 0;
        p->pixelX = 0;
        p->accelY = 0;
        p->accelX = 0;
        p->velY = 0;
        p->velX = 0;
        p->speedLimitY = 0x80000000;
        p->speedLimitX = 0x80000000;
        p->h70 = 0;
        p->h6E = 0;
        p->h6C = 0;
        p->player = p->w8C = 0;
        p->h76 = 0;
        p->b74 = 0;
        p->variant = 0;
        p->actorKind = 0;
        p->hitTimer = 0;
        p->health = 0;
        p->waterFlags = 0;
        p->onGround = 0;
        p->hitDirection = 0;
        p->h82 = 0;
        p->hitKind = 0;
        p->hitterSlot |= 0xFF;
        p->hitterPlayer |= 0xFF;
        p->h84 = 0x80;
        p->b80 |= 0xFF;
    }

    gTaskSkipMaskDepth = 0xFF;
}




