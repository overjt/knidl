#include "gba/gba.h"
#include "global.h"

/* Early subsystem: view-bounds test + task skip-mask stack + the SIO
 * MULTI-PLAY link driver (0x08006464-0x08006D17, issue #32 batch G2).
 *
 * Recipe: agbcc -O2 -mthumb-interwork -fprologue-bugfix (fnmatch --newpb).
 * Evidence: the whole range compiles byte-identically under --newpb; under
 * plain agbcc the branching leaves CheckMasterOrSlave / LinkVSync pick up a
 * spurious `push {lr}` (the far_jump_used_p cache bug, lessons 3.75-3.79).
 * Two functions here (RandomSpreadFacing, CheckMasterOrSlave) each pin the recipe from
 * one side, so this range is a good regression pair for that flag.
 *
 * ROM order / semantics:
 *   IsInView  in-view test: 1 when (x,y) is inside the camera rect
 *                 gViewRect[0..3] widened by 64 px on every side.
 *   RandomSpread  base + ((rand(256) * amount) >> 8) * scale   (u16)
 *   RandomSpreadFacing  same, signed by the running task's facing byte
 *                 (gCurTask->b43 == 1 -> +, else -).
 *   TaskFreezeOrThawOthers  push/pop the per-task "phase skip mask" (Task.skipMask, see
 *                 pending/early_4fec.c): val==0 pops the snapshot from
 *                 gTaskSkipMaskStack[--gTaskSkipMaskDepth], val!=0 pushes one and then
 *                 ORs val into every ALLOCATED task's mask (free slots, i.e.
 *                 gTaskSlotTypes[i] == -1, get 0).  One task id is exempt.
 *   TaskRestoreSkipMask  restore one task's mask from the current snapshot  (HIDDEN)
 *   TaskSaveSkipMask  save one task's mask into the current snapshot     (HIDDEN)
 *   InitLinkDriver  cold link init (clear the session block, install the SIO
 *                 and timer-3 IRQ handlers at gIntrTable[0]/[1]) (HIDDEN)
 *   EnableSerial  start a MULTI-PLAY session: RCNT=0, SIOCNT=0x2000|0x4003
 *                 (multi-play, 115200 bd, IRQ), enable IE bit 7 (serial),
 *                 clear the session block and the key mirrors.
 *   DisableSerial  stop the session: mask IE bits 6/7, stop timer 3, ack
 *                 IF 0xC0, clear the session block.
 *   ResetSerial  stop + start (HIDDEN)
 *   LinkMain1  the per-frame link driver called from src/early_2b04.c;
 *                 5-state machine on gLink[1], then packs the link
 *                 status word into gLinkStatus.
 *   CheckMasterOrSlave  refresh the connection state from SIOCNT bits 2-3
 *                 (8 = "all players ready" and we are the parent).
 *   InitTimer  arm timer 3 (0xFF7C, /1024 + IRQ) and enable IE bit 6.
 *   EnqueueSendCmd  queue one 4-halfword send frame into the send ring
 *                 (gLink+28, u16[4][30]) and clear the caller's
 *                 buffer.  Overflow (>=30 pending) sets flag byte [20].
 *   DequeueRecvCmds  dequeue one 4x4 receive frame out of the receive ring
 *                 (gLink+0x110, u16[4][4][30]); when nothing is
 *                 pending the caller's matrix is zeroed and [12] is set.
 *   LinkVSync  per-VBlank link tick, called from VBlankIntr (pokeruby's LinkVSync).
 *
 * symbols.csv hides FOUR unreferenced/extra functions in this range
 * (lesson 2.13 / zone lesson 14): TaskSaveSkipMask and InitLinkDriver inside the
 * 0xE8 recorded for TaskRestoreSkipMask, and ResetSerial inside the 0xAC recorded
 * for DisableSerial.  The assigned RANGE is right; only the sizes are wrong.
 *
 * Matching notes (docs/lessons-learned.md 3.x):
 *  - RandomSpread/RandomSpreadFacing: computing the sum in a `u32` local is what
 *    keeps both `|`-operands SImode registers so the source operand order
 *    survives.  Writing `return base + product;` directly narrows the PLUS
 *    to HImode (convert_to_integer's "shorten"), which turns the product
 *    into `(subreg:SI (reg:HI))`, swaps the operands and shifts the whole
 *    function's allocation by one register.  (New; see the batch report.)
 *  - TaskFreezeOrThawOthers's free-slot test must be `!= -1` with the OR branch first:
 *    the reversed spelling flips every branch in the loop.
 *  - EnableSerial: the CpuSet fill source must be `vu32` (3.48) so its zero
 *    is not CSE'd with the plain zero stores; the 4x4 clear loop needs the
 *    row offset in its own local (`k = i * 8;`) so `j = 3` lands between the
 *    shift and the base add, and the outer loop must be the explicit
 *    `n = i + 1; ...; i = n;` do/while shape.
 *  - DisableSerial/a70/ac8/bb4 restore REG_IME by RE-READING the shadow
 *    gLinkSavedIme, not from a local (a local adds a `lsls/lsrs` pair).
 *  - LinkMain1's status word is ONE assignment expression with the two
 *    conditions as ternaries: that is what materialises the destination
 *    address into r8 first (expand_assignment does the LHS first, 3.60) and
 *    what makes gcc evaluate all six shifted bytes before the branch.
 *    Splitting it into `v = ...; if (...) v |= ...; gLinkStatus = v;`
 *    loads the fields lazily and drops the r8 push.
 *
 * STATUS: 14 of the 16 functions are byte-exact.  EnqueueSendCmd is 8 bytes
 * off (the hoisted `movs r6,#0` sits after the induction-variable init
 * instead of before it) and DequeueRecvCmds is 140 bytes off (same size, same
 * instruction sequence: the i/j loop counters land in r4/r3 instead of the
 * ROM's r3/r4 and the inner bound stays in r5 instead of spilling to
 * [sp,#4]).  See the batch report.
 */

struct Task {
    /* 0x00 */ void (*f00)(void);
    /* 0x04 */ void (*f04)(void);
    /* 0x08 */ void (*f08)(void);
    /* 0x0C */ void (*f0C)(void);
    /* 0x10 */ s16 sleepFrames;
    /* 0x12 */ u8  taskClass;
    /* 0x13 */ u8  skipMask;
    /* 0x14 */ u8  pad14[0x7C];
};

extern struct Task gTasks[];
extern vu16 gTaskSlotTypes[];
extern u8  gTaskSkipMaskStack[2][64];
extern u8  gTaskSkipMaskDepth;
extern s16 gViewRect[];
extern s8 *gCurTask;
extern u8  gLink[];
extern vu16 gIntrMasterEnable;
extern vu16 gIntrEnable;
extern u16 gLocalPlayer;
extern u16 gLinkIsMaster;
extern vu16 gLinkPlayerCount;
extern u16 gPlayerCount;
extern void (*gIntrTable[])(void);
extern u32 gLinkDriverMode;
extern u32 gSerialIntrCount;
extern u32 gChecksumAvailable;
extern u32 gLinkStatus;
extern u32 gLinkPauseFrames;
extern u32 gLinkRecvVCount;
extern u32 gSendCmdFilled;
extern u32 gLinkErrorMask;
extern u16 gRecvNonzeroCheck;
extern u16 gSendNonzeroCheck;
extern u16 gLinkSavedIme;
extern u8  gLastRecvQueueCount;
extern u8  gLastSendQueueCount;
extern u8  gUnk_03004D34;
extern u8  gUnk_03005278;
extern u16 gSendCmd[4];
extern u16 gRecvCmds[4][4];

extern u16 gSendNonzeroCheck;
extern u8  gUnk_03005270;
void StartTransfer(void);
u32 RandomRange(u32 range);
void SerialCB(void);
void Timer3Intr(void);
void ResetSendBuffer(void);
void ResetRecvBuffer(void);
void EnableSerial(void);
void DisableSerial(void);
void CheckMasterOrSlave(void);
void InitTimer(void);
void EnqueueSendCmd(u16 *p);
void DequeueRecvCmds(u16 (*p)[4]);
















void LinkVSync(void)
{
    if (gLink[0] == 0)
        return;
    switch (gLink[1]) {
    case 4:
        if ((s8)gLink[13] <= 4) {
            if (gLink[18] != 1)
                gLink[21] = 1;
        } else {
            gLink[13] = 0;
            StartTransfer();
        }
        break;
    case 2:
        gLink[13] = 4;
        StartTransfer();
        break;
    }
}
