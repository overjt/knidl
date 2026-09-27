#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* early_5654.c (0x08005654-0x080058E3, issue #63).
 *
 * Task free and task create, the two ends of the task system of
 * src/early_4fec.c, src/early_5228.c and src/early_55b0.c (64 task control
 * blocks of 0x90 bytes at gTasks; gTaskSlotTypes[slot] is the slot's
 * task type, -1 when free, and gTaskCount the live count).
 *
 * TaskFree frees slot `id` (tasks free themselves with
 * TaskFree(gCurTaskIdx), and the ARM dispatcher at 0x08000288 calls it
 * for a task whose coroutine returns): it clears the
 * slot's resume address gTaskResumeAddrs[id], resets every field of the
 * control block to its default (callbacks and counters 0, class -1, graphics
 * index -1, parent and link -1, velocity limits 0x80000000) and, while the
 * per-class lists of the frame driver are live (gTaskRunPhase >= 0), marks
 * its entry in gTaskClassLists[class][] (found through the back-reference
 * gTaskListRefs[id] = class << 8 | position) as removed.
 *
 * TaskCreate claims the first free slot at or after the allocation cursor
 * gTaskCursor (wrapping at 64; -1 when the table is full) for task type
 * `type`: the class byte and the entry address come from the task-type table
 * gTaskTypes[type], the stack is the slot's 256 bytes at 0x0203BFE0, the
 * parent (Task.parent) is the running task gCurTaskIdx, and the slot is
 * appended to its class list when the lists are live.  The cursor moves past
 * the slot, whose index is returned.  src/early_58e4.c's allocation wrappers
 * call it.
 *
 * Matching notes (issue #63): TaskFree's tail is `cls = ... >> 8;
 * gTaskClassLists[cls][...] = 0xFF;` (lesson 3.485); in TaskCreate the search's
 * start slot and the returned slot are one variable and the stack base is the
 * plain number the landed TaskSetEntry uses (lesson 3.486). */

extern vs16 gTaskSlotTypes[];
extern s32 gTaskCount;
extern s32 gTaskRunPhase;
extern u32 gTaskResumeAddrs[];
extern vu16 gTaskListRefs[];
extern vu8 gTaskClassLists[][64];
extern vs32 gTaskCursor;
extern u32 gTaskStackPtrs[];
extern struct TaskType gTaskTypes[];
extern vu8 gTaskClassListLen[];

/* Free the task in slot `id`. */
void TaskFree(s32 id)
{
    struct Task *t;
    u8 cls;

    if (gTaskSlotTypes[id] == -1)
        return;
    gTaskCount--;
    gTaskSlotTypes[id] = 0xFFFF;
    gTaskResumeAddrs[id] = 0;
    t = &gTasks[id];
    t->taskClass = -1;
    t->moveCallback = 0;
    t->updateCallback = 0;
    t->lateUpdateCallback = 0;
    t->drawCallback = 0;
    t->serial = 0;
    t->updateState = 0;
    t->state = 0;
    t->skipMask = 0;
    t->frame = -1;
    t->layer = 0;
    t->frameTable = 0;
    t->facing = 0;
    t->tileWord = 0;
    t->spriteFlags = 0;
    t->unk46 = -1;
    t->parent = -1;
    t->unk34 = 0;
    t->unk30 = 0;
    t->unk2C = 0;
    t->unk28 = 0;
    t->unk24 = 0;
    t->unk20 = 0;
    t->unk1C = 0;
    t->unk18 = 0;
    t->posY = 0;
    t->posX = 0;
    t->pixelY = 0;
    t->pixelX = 0;
    t->accelY = 0;
    t->accelX = 0;
    t->velY = 0;
    t->velX = 0;
    t->speedLimitY = 0x80000000;
    t->speedLimitX = 0x80000000;
    t->unk70 = 0;
    t->unk6E = 0;
    t->unk6C = 0;
    t->player = (struct PlayerState *)(t->unk8C = 0);
    t->unk76 = 0;
    t->unk74 = 0;
    t->variant = 0;
    t->actorKind = 0;
    t->hitTimer = 0;
    t->health = 0;
    t->waterFlags = 0;
    t->onGround = 0;
    t->hitDirection = 0;
    t->unk82 = 0;
    t->hitKind = 0;
    t->hitterSlot = -1;
    t->hitterPlayer = -1;
    t->unk80 = -1;
    if (gTaskRunPhase >= 0)
    {
        cls = gTaskListRefs[id] >> 8;
        gTaskClassLists[cls][gTaskListRefs[id] & 0xFF] = 0xFF;
    }
}

/* Allocate a task of the given type; returns its slot index or -1. */
s32 TaskCreate(u32 type)
{
    s32 id;     /* the search's start slot, then the slot returned: ONE
                 * variable, which is what puts both in the ROM's r4 */
    s8 cls;
    struct Task *t;

    id = gTaskCursor;
    while (gTaskSlotTypes[gTaskCursor] != -1)
    {
        gTaskCursor++;
        if (gTaskCursor > 63)
            gTaskCursor = 0;
        if (gTaskCursor == id)
            return -1;
    }
    gTaskCount++;
    gTaskSlotTypes[gTaskCursor] = type;
    t = &gTasks[gTaskCursor];
    t->taskClass = gTaskTypes[type].taskClass;
    /* The task's 256-byte stack in EWRAM, spelled as the literal that
     * src/early_5d9c.c's TaskSetEntry uses: a CONST_INT operand that reload
     * materialises (the ROM's `ldr r7, =0x0203BFE0` in the dead `type`
     * register); `(u32)gUnk_0203BFE0` makes it a local pseudo instead
     * (lesson 3.486). */
    gTaskStackPtrs[gTaskCursor] = 0x0203BFE0 + (gTaskCursor << 8);
    gTaskResumeAddrs[gTaskCursor] = gTaskTypes[type].entry;
    t->sleepFrames = 0;
    t->skipMask = 0;
    t->parent = gCurTaskIdx;
    if (gTaskRunPhase >= 0)
    {
        cls = t->taskClass;
        gTaskClassLists[cls][gTaskClassListLen[cls]] = gTaskCursor;
        gTaskListRefs[gTaskCursor] = (cls << 8) | gTaskClassListLen[cls];
        gTaskClassListLen[cls]++;
        if (gTaskClassListLen[cls] > 63)
            gTaskClassListLen[cls] = 0;
    }
    id = gTaskCursor;
    gTaskCursor++;
    if (gTaskCursor > 63)
        gTaskCursor = 0;
    return id;
}
