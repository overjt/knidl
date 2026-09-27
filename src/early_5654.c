#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* early_5654.c (0x08005654-0x080058E3, issue #63).
 *
 * Task free and task create, the two ends of the task system of
 * src/early_4fec.c, src/early_5228.c and src/early_55b0.c (64 task control
 * blocks of 0x90 bytes at gUnk_03002790; gUnk_03004CA0[slot] is the slot's
 * task type, -1 when free, and gUnk_030026F0 the live count).
 *
 * sub_08005654 frees slot `id` (tasks free themselves with
 * sub_08005654(gCurTaskIdx), and the ARM dispatcher at 0x08000288 calls it
 * for a task whose coroutine returns): it clears the
 * slot's resume address gTaskFlagsTable[id], resets every field of the
 * control block to its default (callbacks and counters 0, class -1, graphics
 * index -1, parent and link -1, velocity limits 0x80000000) and, while the
 * per-class lists of the frame driver are live (gUnk_030026F4 >= 0), marks
 * its entry in gUnk_030024A0[class][] (found through the back-reference
 * gUnk_03002710[id] = class << 8 | position) as removed.
 *
 * sub_0800579c claims the first free slot at or after the allocation cursor
 * gUnk_03002494 (wrapping at 64; -1 when the table is full) for task type
 * `type`: the class byte and the entry address come from the task-type table
 * gUnk_0872FF30[type], the stack is the slot's 256 bytes at 0x0203BFE0, the
 * parent (Task.unk44) is the running task gCurTaskIdx, and the slot is
 * appended to its class list when the lists are live.  The cursor moves past
 * the slot, whose index is returned.  src/early_58e4.c's allocation wrappers
 * call it.
 *
 * Matching notes (issue #63): TaskFree's tail is `cls = ... >> 8;
 * gUnk_030024A0[cls][...] = 0xFF;` (lesson 3.485); in TaskCreate the search's
 * start slot and the returned slot are one variable and the stack base is the
 * plain number the landed sub_08006148 uses (lesson 3.486). */

extern vs16 gUnk_03004CA0[];
extern s32 gUnk_030026F0;
extern s32 gUnk_030026F4;
extern u32 gTaskFlagsTable[];
extern vu16 gUnk_03002710[];
extern vu8 gUnk_030024A0[][64];
extern vs32 gUnk_03002494;
extern u32 gUnk_03004B90[];
extern struct TaskType gUnk_0872FF30[];
extern vu8 gUnk_03002700[];

/* Free the task in slot `id`. */
void sub_08005654(s32 id)
{
    struct Task *t;
    u8 cls;

    if (gUnk_03004CA0[id] == -1)
        return;
    gUnk_030026F0--;
    gUnk_03004CA0[id] = 0xFFFF;
    gTaskFlagsTable[id] = 0;
    t = &gUnk_03002790[id];
    t->unk12 = -1;
    t->unk00 = 0;
    t->unk04 = 0;
    t->unk08 = 0;
    t->unk0C = 0;
    t->unk16 = 0;
    t->unk15 = 0;
    t->unk14 = 0;
    t->unk13 = 0;
    t->unk3C = -1;
    t->unk42 = 0;
    t->unk38 = 0;
    t->unk43 = 0;
    t->unk40 = 0;
    t->unk3E = 0;
    t->unk46 = -1;
    t->unk44 = -1;
    t->unk34 = 0;
    t->unk30 = 0;
    t->unk2C = 0;
    t->unk28 = 0;
    t->unk24 = 0;
    t->unk20 = 0;
    t->unk1C = 0;
    t->unk18 = 0;
    t->unk50 = 0;
    t->unk4C = 0;
    t->unk4A = 0;
    t->unk48 = 0;
    t->unk60 = 0;
    t->unk5C = 0;
    t->unk58 = 0;
    t->unk54 = 0;
    t->unk68 = 0x80000000;
    t->unk64 = 0x80000000;
    t->unk70 = 0;
    t->unk6E = 0;
    t->unk6C = 0;
    t->unk88 = (struct PlayerState *)(t->unk8C = 0);
    t->unk76 = 0;
    t->unk74 = 0;
    t->unk73 = 0;
    t->unk72 = 0;
    t->unk75 = 0;
    t->unk78 = 0;
    t->unk7B = 0;
    t->unk7A = 0;
    t->unk7D = 0;
    t->unk82 = 0;
    t->unk7C = 0;
    t->unk7E = -1;
    t->unk7F = -1;
    t->unk80 = -1;
    if (gUnk_030026F4 >= 0)
    {
        cls = gUnk_03002710[id] >> 8;
        gUnk_030024A0[cls][gUnk_03002710[id] & 0xFF] = 0xFF;
    }
}

/* Allocate a task of the given type; returns its slot index or -1. */
s32 sub_0800579c(u32 type)
{
    s32 id;     /* the search's start slot, then the slot returned: ONE
                 * variable, which is what puts both in the ROM's r4 */
    s8 cls;
    struct Task *t;

    id = gUnk_03002494;
    while (gUnk_03004CA0[gUnk_03002494] != -1)
    {
        gUnk_03002494++;
        if (gUnk_03002494 > 63)
            gUnk_03002494 = 0;
        if (gUnk_03002494 == id)
            return -1;
    }
    gUnk_030026F0++;
    gUnk_03004CA0[gUnk_03002494] = type;
    t = &gUnk_03002790[gUnk_03002494];
    t->unk12 = gUnk_0872FF30[type].unk00;
    /* The task's 256-byte stack in EWRAM, spelled as the literal that
     * src/early_5d9c.c's sub_08006148 uses: a CONST_INT operand that reload
     * materialises (the ROM's `ldr r7, =0x0203BFE0` in the dead `type`
     * register); `(u32)gUnk_0203BFE0` makes it a local pseudo instead
     * (lesson 3.486). */
    gUnk_03004B90[gUnk_03002494] = 0x0203BFE0 + (gUnk_03002494 << 8);
    gTaskFlagsTable[gUnk_03002494] = gUnk_0872FF30[type].unk04;
    t->unk10 = 0;
    t->unk13 = 0;
    t->unk44 = gCurTaskIdx;
    if (gUnk_030026F4 >= 0)
    {
        cls = t->unk12;
        gUnk_030024A0[cls][gUnk_03002700[cls]] = gUnk_03002494;
        gUnk_03002710[gUnk_03002494] = (cls << 8) | gUnk_03002700[cls];
        gUnk_03002700[cls]++;
        if (gUnk_03002700[cls] > 63)
            gUnk_03002700[cls] = 0;
    }
    id = gUnk_03002494;
    gUnk_03002494++;
    if (gUnk_03002494 > 63)
        gUnk_03002494 = 0;
    return id;
}
