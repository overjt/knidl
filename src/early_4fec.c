#include "gba/gba.h"
#include "global.h"

/* Early subsystem: the cooperative TASK ENGINE (0x08004FEC-0x08005653,
 * issue #32 batch F1).
 *
 * 64 task slots of 0x90 bytes live at 0x03002790.  Each slot carries four
 * callbacks (+0x00/+0x04/+0x08/+0x0C), a countdown (+0x10), a priority-group
 * id (+0x12, negative = free) and a per-phase skip mask (+0x13).  Slots are
 * bucketed into five priority groups: gUnk_030024A0[group][slot] holds the
 * task ids, gUnk_03002480[group] the live count, gUnk_03002700/gUnk_03002708
 * the pending/processed counts used to detect list growth during a pass.
 * gTaskFlagsTable[id]/gUnk_03004B90[id] are the coroutine resume PC/SP consumed
 * by the ARM task switcher at 0x08000234 (reached through its thumb veneer
 * TaskSwitchTrampoline); gTaskSavedR0 is that switcher's "sleep" result.
 *
 *   sub_08004fec  cold init: clears the bucket tables, the id map and every
 *                 task slot (two CpuSet fills plus a per-slot field reset).
 *   sub_08005228  the per-frame driver (called from sub_08002d18): rebuilds
 *                 the buckets from the slot table, then runs phases 1..5 --
 *                 resume/countdown + callback[0]/[1], then callback[2] and
 *                 callback[3] -- restarting whenever a callback added tasks.
 *   sub_080055b0  set one task's +0x13 skip mask.
 *   sub_080055c4  set +0x13 on every allocated task, preserving one slot's.
 *   sub_08005618  set +0x13 on every allocated task (dead export).
 *
 * Recipe: old_agbcc -O2 -mthumb-interwork (fnmatch --old2).  Evidence: the
 * leaf sub_080055b0 ends in a bare `bx lr`; agbcc always emits
 * `push {lr}` / `pop {r0}; bx r0` even for leaves.
 *
 * Matching notes (docs/lessons-learned.md §3):
 *  - sub_08005618 is a dead export hidden inside symbols.csv's 0x90 size for
 *    sub_080055c4 (lesson 2.13 / zone lesson 14): nothing in ROM calls it.
 *  - `gUnk_03002710[i] = 0xFFFF; gUnk_03004CA0[i] = gUnk_03002710[i];` is the
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
 *  - the restart of sub_08005228's phase-1..3 pass is a `goto`, not a
 *    do/while: a loop note re-weights every reference inside it by one more
 *    loop level and moves three long-lived address pseudos onto different
 *    hard registers (3.21 applied to allocation rather than to hoisting).
 *  - sub_08005228 folds the restart counter into `j`; that is what raises
 *    j's global-alloc priority past &gTaskSavedR0's and puts j on r5.
 *
 * STATUS: sub_08004fec, sub_080055b0, sub_080055c4 and sub_08005618 are
 * byte-exact.  sub_08005228 reproduces the ROM's instruction sequence
 * one-for-one but diverges on register NAMES only (908 vs 904 bytes).  Root
 * cause: agbcc's local allocator gives the current-task pointer r1 (reusing
 * the dying `ldr r1,=gUnk_03002790`) where the ROM uses a fresh r2; that
 * pushes the `task->b13` temp from r1 to r6, denies r6 to the phase-4/5
 * &gUnk_03002490 pseudo, evicts &gUnk_03002488 from r8 into the
 * caller-clobbered r3 and so costs one extra `mov` plus a second caller-save
 * slot (`sub sp,#8`).  No source spelling tried moves that one local-alloc
 * decision - see the batch report for the list.
 */

struct Task {
    /* 0x00 */ void (*f00)(void);
    /* 0x04 */ void (*f04)(void);
    /* 0x08 */ void (*f08)(void);
    /* 0x0C */ void (*f0C)(void);
    /* 0x10 */ s16 h10;
    /* 0x12 */ u8  b12;
    /* 0x13 */ u8  b13;
    /* 0x14 */ u8  b14;
    /* 0x15 */ u8  b15;
    /* 0x16 */ u16 h16;
    /* 0x18 */ u32 w18[8];
    /* 0x38 */ u32 w38;
    /* 0x3C */ u16 h3C;
    /* 0x3E */ u16 h3E;
    /* 0x40 */ u16 h40;
    /* 0x42 */ u8  b42;
    /* 0x43 */ u8  b43;
    /* 0x44 */ u16 h44;
    /* 0x46 */ u16 h46;
    /* 0x48 */ u16 h48;
    /* 0x4A */ u16 h4A;
    /* 0x4C */ u32 w4C;
    /* 0x50 */ u32 w50;
    /* 0x54 */ u32 w54;
    /* 0x58 */ u32 w58;
    /* 0x5C */ u32 w5C;
    /* 0x60 */ u32 w60;
    /* 0x64 */ u32 w64;
    /* 0x68 */ u32 w68;
    /* 0x6C */ u16 h6C;
    /* 0x6E */ u16 h6E;
    /* 0x70 */ u16 h70;
    /* 0x72 */ u8  b72;
    /* 0x73 */ u8  b73;
    /* 0x74 */ u8  b74;
    /* 0x75 */ u8  b75;
    /* 0x76 */ u16 h76;
    /* 0x78 */ u16 h78;
    /* 0x7A */ u8  b7A;
    /* 0x7B */ u8  b7B;
    /* 0x7C */ u8  b7C;
    /* 0x7D */ u8  b7D;
    /* 0x7E */ u8  b7E;
    /* 0x7F */ u8  b7F;
    /* 0x80 */ u8  b80;
    /* 0x81 */ u8  b81;
    /* 0x82 */ u16 h82;
    /* 0x84 */ u16 h84;
    /* 0x86 */ u16 h86;
    /* 0x88 */ u32 w88;
    /* 0x8C */ u32 w8C;
};


extern struct Task gUnk_03002790[];
extern vu16 gUnk_03004CA0[];
extern vu32 gUnk_030026F0;
extern vs32 gUnk_030026F4;
extern vu32 gTaskSavedLr;
extern vu32 gTaskSavedSp;
extern vu8  gUnk_03002478[];
extern vu8  gUnk_03002480[];
extern vu32 gUnk_03002488;
extern vs32 gCurTaskIdx;
extern struct Task *gUnk_03002490;
extern vu32 gUnk_03002494;
extern vu8  gUnk_030024A0[5][64];
extern s32  gTaskSavedR0;
extern u32  gTaskFlagsTable[];
extern vu8  gUnk_03002700[];
extern vu8  gUnk_03002708[];
extern vu16 gUnk_03002710[];
extern u32  gUnk_03004B90[];
extern vs32 gUnk_03004C90;
extern vu32 gTaskBaseSp;
extern vu8  gUnk_0200D110;

void TaskSwitchTrampoline(s32 id, u32 fn, u32 stack);

void sub_08004fec(void)
{
    u32 i;
    u32 j;
    vu16 fill;
    u16 fill2;

    gCurTaskIdx = gUnk_030026F4 = gUnk_03004C90 = -1;
    gUnk_030026F0 = gUnk_03002494 = 0;

    for (i = 0; i < 5; i++) {
        for (j = 0; j < 64; j++)
            gUnk_030024A0[i][j] = 0;
        gUnk_03002708[i] = gUnk_03002478[i] = gUnk_03002700[i] = gUnk_03002480[i] = 0;
    }

    gTaskBaseSp = gTaskSavedSp = gTaskSavedLr = gTaskSavedR0 = 0;

    for (i = 0; i < 64; i++) {
        gUnk_03004CA0[i] = gUnk_03002710[i] = 0xFFFF;
        gTaskFlagsTable[i] = 0;
    }

    fill = 0xFFFF;
    CpuSet((const void *)&fill, (void *)0x0203BF00, 0x01002000);
    fill2 = 0;
    CpuSet(&fill2, gUnk_03002790, 0x01001200);

    for (i = 0; i < 64; i++) {
        struct Task *p = &gUnk_03002790[i];

        p->b12 |= 0xFF;
        p->h10 = 0;
        p->f0C = 0;
        p->f08 = 0;
        p->f04 = 0;
        p->f00 = 0;
        p->h16 = 0;
        p->b15 = 0;
        p->b14 = 0;
        p->b13 = 0;
        p->w18[7] = 0;
        p->w18[6] = 0;
        p->w18[5] = 0;
        p->w18[4] = 0;
        p->w18[3] = 0;
        p->w18[2] = 0;
        p->w18[1] = 0;
        p->w18[0] = 0;
        p->h3C |= 0xFFFF;
        p->b42 = 0;
        p->w38 = 0;
        p->b43 = 0;
        p->h40 = 0;
        p->h3E = 0;
        p->h46 |= 0xFFFF;
        p->h44 |= 0xFFFF;
        p->w50 = 0;
        p->w4C = 0;
        p->h4A = 0;
        p->h48 = 0;
        p->w60 = 0;
        p->w5C = 0;
        p->w58 = 0;
        p->w54 = 0;
        p->w68 = 0x80000000;
        p->w64 = 0x80000000;
        p->h70 = 0;
        p->h6E = 0;
        p->h6C = 0;
        p->w88 = p->w8C = 0;
        p->h76 = 0;
        p->b74 = 0;
        p->b73 = 0;
        p->b72 = 0;
        p->b75 = 0;
        p->h78 = 0;
        p->b7B = 0;
        p->b7A = 0;
        p->b7D = 0;
        p->h82 = 0;
        p->b7C = 0;
        p->b7E |= 0xFF;
        p->b7F |= 0xFF;
        p->h84 = 0x80;
        p->b80 |= 0xFF;
    }

    gUnk_0200D110 = 0xFF;
}




