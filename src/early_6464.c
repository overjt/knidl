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

u32 IsInView(s16 x, s16 y)
{
    if (gViewRect[0] - 64 >= x)
        return 0;
    if (x >= gViewRect[1] + 64)
        return 0;
    if (gViewRect[2] - 64 >= y)
        return 0;
    if (y >= gViewRect[3] + 64)
        return 0;
    return 1;
}

u16 RandomSpread(u16 base, u8 scale, u8 amount)
{
    u32 v = ((RandomRange(0x100) * amount) >> 8) * scale + base;

    return v;
}

s16 RandomSpreadFacing(u16 base, u8 scale, u8 amount)
{
    u32 t = ((RandomRange(0x100) * amount) >> 8) * scale + base;
    s16 v = t;

    if (gCurTask[0x43] == 1)
        return v;
    return -v;
}

void TaskFreezeOrThawOthers(u16 val, s32 idx)
{
    u8 save;
    u16 i;

    if (val == 0) {
        save = gTasks[idx].skipMask;
        for (i = 0; i < 64; i++)
            gTasks[i].skipMask = gTaskSkipMaskStack[gTaskSkipMaskDepth][i];
        gTasks[idx].skipMask = save;
        gTaskSkipMaskDepth--;
    } else {
        gTaskSkipMaskDepth++;
        for (i = 0; i < 64; i++)
            gTaskSkipMaskStack[gTaskSkipMaskDepth][i] = gTasks[i].skipMask;
        save = gTasks[idx].skipMask;
        for (i = 0; i < 64; i++) {
            if ((s16)gTaskSlotTypes[i] != -1)
                gTasks[i].skipMask = val | gTasks[i].skipMask;
            else
                gTasks[i].skipMask = 0;
        }
        gTasks[idx].skipMask = save;
    }
}

void TaskRestoreSkipMask(u32 idx)
{
    gTasks[idx].skipMask = gTaskSkipMaskStack[gTaskSkipMaskDepth][idx];
}

void TaskSaveSkipMask(u32 idx)
{
    gTaskSkipMaskStack[gTaskSkipMaskDepth][idx] = gTasks[idx].skipMask;
}

void InitLinkDriver(void)
{
    u32 zero = 0;

    CpuSet(&zero, gLink, 0x05000135);
    gLocalPlayer = 0;
    gLinkIsMaster = 0;
    gLinkPlayerCount = 1;
    gPlayerCount = 1;
    gIntrTable[0] = SerialCB;
    gIntrTable[1] = Timer3Intr;
    gLinkDriverMode = 0;
    gSerialIntrCount = 0;
    gChecksumAvailable = 0;
    gLinkPauseFrames = 0;
    gLinkRecvVCount = 0;
    gSendCmdFilled = 0;
    gLinkErrorMask = 0x3F000;
}

void EnableSerial(void)
{
    vu32 zero;
    u16 z1;
    u16 z2;
    s32 n;
    s32 k;
    s32 i;
    s32 j;
    u16 *p;
    u16 *b;

    gIntrMasterEnable = REG_IME = REG_IME & 0xFFFE;
    REG_IE &= 0xFF3F;
    REG_RCNT = 0;
    REG_SIOCNT = 0x2000;
    REG_SIOCNT |= 0x4003;
    REG_IE |= 0x80;
    gIntrEnable = REG_IE;
    zero = 0;
    CpuSet((const void *)&zero, gLink, 0x05000135);
    ResetSendBuffer();
    ResetRecvBuffer();
    gSendNonzeroCheck = gRecvNonzeroCheck = 0;
    gLastSendQueueCount = gLastRecvQueueCount = 0;
    gUnk_03005278 = gUnk_03004D34 = 0;
    gLinkDriverMode = 0;
    gSerialIntrCount = 0;
    gChecksumAvailable = 0;
    gLinkPauseFrames = 0;
    gLinkRecvVCount = 0;
    gSendCmdFilled = 0;
    gLinkErrorMask = 0x3F000;

    b = gSendCmd;
    z1 = 0;
    p = b + 3;
    do {
        *p = z1;
        p--;
    } while ((s32)p >= (s32)b);

    i = 0;
    do {
        n = i + 1;
        k = i * 8;
        j = 3;
        p = (u16 *)((u8 *)gRecvCmds + k);
        z2 = 0;
        p += 3;
        do {
            *p = z2;
            p--;
            j--;
        } while (j >= 0);
        i = n;
    } while (i <= 3);

    REG_IME |= 1;
    gIntrMasterEnable = REG_IME;
}

void DisableSerial(void)
{
    u32 zero;

    gLinkSavedIme = REG_IME;
    REG_IME = 0;
    REG_IE &= 0xFF3F;
    gLinkDriverMode = 0;
    gSerialIntrCount = 0;
    REG_IME = gLinkSavedIme;
    REG_SIOCNT = 0;
    REG_TM3CNT_H = 0;
    REG_IF = 0xC0;
    zero = 0;
    CpuSet(&zero, gLink, 0x05000135);
    gLocalPlayer = 0;
    gLinkIsMaster = 0;
    gLinkPlayerCount = 1;
    gPlayerCount = 1;
    gLinkErrorMask = 0;
}

void ResetSerial(void)
{
    EnableSerial();
    DisableSerial();
}

void LinkMain1(u8 *cmd, u16 *send, u16 *recv)
{
    if (gLinkDriverMode == 2)
        return;

    switch (gLink[1]) {
    case 0:
        DisableSerial();
        gLink[1] = 1;
        break;
    case 1:
        if (*cmd == 1) {
            EnableSerial();
            gLink[1] = 2;
        }
        break;
    case 2:
        switch (*cmd) {
        case 1:
            if (gLink[0] == 8 && gLink[3] > 1)
                gLink[16] = 1;
            break;
        case 2:
            gLink[1] = 0;
            break;
        default:
            CheckMasterOrSlave();
            break;
        }
        break;
    case 3:
        InitTimer();
        gLink[1] = 4;
        /* fallthrough */
    case 4:
        EnqueueSendCmd(send);
        DequeueRecvCmds((u16 (*)[4])recv);
        break;
    }

    *cmd = 0;
    gLinkStatus = gLink[2]
        | (gLink[3] << 2)
        | (gLink[0] == 8 ? 0x20 : 0)
        | (gLink[1] == 4 ? 0x40 : 0)
        | (gLink[12] << 8)
        | (gLink[17] << 9)
        | (gLink[18] << 12)
        | (gLink[19] << 13)
        | (gLink[20] << 14)
        | (gLink[21] << 16)
        | (gLink[2] > 3 ? 0x20000 : 0);
}

void CheckMasterOrSlave(void)
{
    u32 t = *(vu32 *)REG_ADDR_SIOCNT & 12;

    if (t == 8 && gLink[2] == 0)
        gLink[0] = 8;
    else
        gLink[0] = 0;
}

void InitTimer(void)
{
    if (gLink[0] != 0) {
        REG_TM3CNT_L = 0xFF7C;
        REG_TM3CNT_H = 0x41;
        gLinkSavedIme = REG_IME;
        REG_IME = 0;
        REG_IE |= 0x40;
        gIntrEnable = REG_IE;
        REG_IME = gLinkSavedIme;
    }
}



