#include "gba/gba.h"
#include "global.h"

/* Early subsystem: link-play frame driver, wait helpers, RNG, decimal digit
 * split and 15bpp colour blending (0x08002B04-0x0800310F, issue #32 batch C2).
 *
 * Recipe: old_agbcc -O2 -mthumb-interwork (fnmatch --old2).  Evidence: the
 * leaf ApplyBgLayout ends in a bare `bx lr`; agbcc unconditionally emits
 * `push {lr}` / `pop {r0}; bx r0` even for leaves.
 *
 * Matching notes (see docs/lessons-learned.md §3):
 *  - The switch dispatch trees in FillSendCmd / UpdatePlayerKeys are NOT what
 *    gcc's balance_case_nodes produces for the same case values (it puts the
 *    middle case at the root).  The ROM's tree is "root = lowest case, empty
 *    left subtree", which only comes out of an explicit
 *    `if (t != K) { if (t > K) {...} } else {...}` nest — that also puts the
 *    K body last, exactly where the ROM has it.
 *  - gRecvCmds must be a 2-D array: `g[1][i]` materialises the row base
 *    (`adds r0,r6,#0; adds r0,#8; adds r0,r5,r0`) while a flat `g[i + 4]`
 *    folds into the walking pointer as `[r4, #8]`.  The 2-D form is also what
 *    keeps the base register live, which is what pushes 0x8800 into r8.
 *  - gPlayerHeldKeys[] / gPlayerPressedKeys[] are `vu16` arrays: the ROM's dead
 *    pre-read `ldrh` before every store is the volatile *indexed* store idiom
 *    (§3.7), and `gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = 0` is the chained
 *    assignment idiom (§3.8) — outer address materialised first, inner cell
 *    re-read for the outer store.
 *  - ApplyBgLayout's first parameter read must be volatile (`*(vu16 *)p`);
 *    without it the address/temporary pseudos swap r1<->r2.  The remaining
 *    reads p[1..4] are plain (a volatile pointer re-reads them).
 *  - IntToDigits keeps the digit buffer in a function-scope `u8 *b` that
 *    every branch assigns: that is what pins it to r4 in the zero branch too.
 *    `zero`/`e` in the zero branch reproduce the ROM's preheader order
 *    (base, bound copy, zero, base+5) and its signed pointer compare.
 *  - BlendColors's ratio/count parameters are 32-bit; the u16 truncations
 *    belong to the LOCALS (`u16 r = ratio; u16 n = count; n--;`), which is
 *    what places `ldr r7,[sp,#32]` before them and keeps ratio in r6.
 *  - BlendColor is a dead export hidden inside symbols.csv's 0xFC size for
 *    BlendColors (lesson 2.13 / zone lesson 14): nothing in ROM calls it.
 */

extern vu16 gLinkPlayerCount;      /* number of linked players */
extern u32 gLinkPauseFrames;
extern vu16 gLinkCommand;      /* link session state, high byte = command */
extern u16 gSendCmd[4];    /* link send buffer */
extern vu16 gHeldKeys;      /* keys held last frame */
extern vu16 gPressedKeys;      /* keys newly pressed */
extern u8 gLastRecvQueueCount;

extern vu16 gPlayerHeldKeys[];    /* per-player keys held */
extern vu16 gPlayerPressedKeys[];    /* per-player keys pressed */
extern u16 gShouldAdvanceLinkState[];
extern u16 gRecvCmds[3][4]; /* [0]=state [1]=keys held [2]=keys pressed */
extern u8 gLink[];
extern u32 gLinkRecvVCount;
extern vu16 gWaitingForVBlank;      /* VBlank wait flag */
extern u32 gSerialIntrCount;       /* frame counter */
extern u32 gSendCmdFilled;

extern vu16 gFadeSteps;      /* frames left to wait */
extern u16 gFadeBlankAtWhite;
extern vu16 gDispCnt;      /* display/mode flags */
extern u16 gBg0Cnt;
extern u16 gBg1Cnt;
extern u16 gBg2Cnt;
extern u16 gBg3Cnt;
extern vu32 gRngValue;      /* RNG state */
extern u8 gDigits[6];     /* decimal digit buffer, [5] = sign/flag */

void FillSendCmdWithSaveSlot(void);
void ReceiveLinkSaveSlots(void);
void LinkMain1(u16 *a, u16 *b, u16 *c);
void LinkErrorScreen(void);
void RunTasks(void);
void RunBuildOamInIwram(void);
void EndFrame(void);
void ResetSpriteQueue(void);
u32 IsLinkError(void);
void RunFrame(void);
void RunFrameNoTasks(void);

void FillSendCmd(void)
{
    s32 t;

    if (gLinkPlayerCount > 1 && gLinkPauseFrames == 0) {
        t = gLinkCommand & 0xFF00;
        if (t != 0x6600) {
            if (t > 0x6600) {
                switch (t) {
                case 0x8800:
                    gSendCmd[0] = t;
                    gSendCmd[1] = gHeldKeys;
                    gSendCmd[2] = gPressedKeys;
                    gSendCmd[3] = gLastRecvQueueCount;
                    break;
                case 0x9900:
                    gSendCmd[3] = 0;
                    gSendCmd[2] = 0;
                    gSendCmd[1] = 0;
                    gSendCmd[0] = 0;
                    break;
                }
            }
        } else {
            FillSendCmdWithSaveSlot();
        }
    }
}

void UpdatePlayerKeys(void)
{
    s32 i;
    s32 t;
    u32 tries;
    u32 old;

    if (gLinkPlayerCount <= 1) {
        gPlayerHeldKeys[0] = gHeldKeys;
        gPlayerPressedKeys[0] = gPressedKeys;
        return;
    }

    LinkMain1(gShouldAdvanceLinkState, gSendCmd, gRecvCmds[0]);
    if ((gLinkCommand & 0xFF00) == 0x8800) {
        tries = 0;
        if (gLink[12] != 0) {
            u32 v;

            do {
                v = REG_VCOUNT;
                if (v < gLinkRecvVCount)
                    v += 228;
            } while (v - gLinkRecvVCount <= 38);

            while (gLink[12] != 0) {
                old = gSerialIntrCount;
                gWaitingForVBlank = 1;
                if (REG_IME & 1) {
                    while (gWaitingForVBlank != 0)
                        ;
                }
                if (old == gSerialIntrCount) {
                    tries++;
                    if (tries > 29)
                        LinkErrorScreen();
                }
                FillSendCmd();
                LinkMain1(gShouldAdvanceLinkState, gSendCmd, gRecvCmds[0]);
            }
            gSendCmdFilled = 1;
        }
    }

    for (i = 0; i < 4; i++) {
        t = gRecvCmds[0][i] & 0xFF00;
        if (t != 0x6600) {
            if (t > 0x6600) {
                if (t == 0x8800) {
                    gPlayerHeldKeys[i] = gRecvCmds[1][i];
                    gPlayerPressedKeys[i] = gRecvCmds[2][i];
                }
            }
        } else {
            ReceiveLinkSaveSlots();
        }
        if ((gRecvCmds[0][i] & 0xFF00) != 0x8800)
            gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = 0;
    }
}

void RunLinkFrame(void)
{
    RunTasks();
    RunBuildOamInIwram();
    if (gSendCmdFilled == 0)
        FillSendCmd();
    gSendCmdFilled = 0;
    EndFrame();
    ResetSpriteQueue();
    UpdatePlayerKeys();
    if (IsLinkError() != 0)
        LinkErrorScreen();
}

void RunFrames(s32 count)
{
    while (--count != -1)
        RunFrame();
}

void RunFramesNoTasks(s32 count)
{
    while (--count != -1)
        RunFrameNoTasks();
}

void RunLinkFrames(s32 count)
{
    while (--count != -1)
        RunLinkFrame();
}

void RunFramesUntilFadeDone(void)
{
    while (gFadeSteps != 0)
        RunFrame();
    gFadeBlankAtWhite = 0;
}

void RunFramesNoTasksUntilFadeDone(void)
{
    while (gFadeSteps != 0)
        RunFrameNoTasks();
    gFadeBlankAtWhite = 0;
}

void RunLinkFramesUntilFadeDone(void)
{
    while (gFadeSteps != 0)
        RunLinkFrame();
    gFadeBlankAtWhite = 0;
}

void ApplyBgLayout(u16 *p)
{
    gDispCnt &= 0xFF80;
    gDispCnt |= *(vu16 *)p;
    if ((gDispCnt & 7) <= 2) {
        if (p[1] != 0)
            gBg0Cnt = p[1];
        if (p[2] != 0)
            gBg1Cnt = p[2];
        if (p[3] != 0)
            gBg2Cnt = p[3];
        if (p[4] != 0)
            gBg3Cnt = p[4];
    }
}

void CallTableEntry(u32 idx, u32 count, void (**fns)(void))
{
    if (idx < count)
        fns[idx]();
}

void SeedRandom(u32 seed)
{
    gRngValue = seed & 0xFFF;
}

u32 Random(void)
{
    gRngValue = (gRngValue * 61 + 0x579) & 0xFFF;
    return gRngValue;
}

u32 RandomRange(u32 range)
{
    gRngValue = (gRngValue * 61 + 0x579) & 0xFFF;
    return (range * gRngValue) >> 12;
}

void IntToDigits(s16 n)
{
    u8 *b;
    s32 d;
    s16 v = n;

    if (n > 0) {
        gDigits[5] = 18;
        b = gDigits;
    } else if (n < 0) {
        gDigits[5] = 16;
        v = -n;
        b = gDigits;
    } else {
        u8 *e;
        u8 zero;
        u8 *p;

        b = gDigits;
        e = gDigits;
        zero = 0;
        p = b + 5;
        do {
            *p = zero;
            p--;
        } while ((s32)p >= (s32)e);
        b[5] = 17;
        return;
    }

    d = -1;
    while (v >= 0) { v -= 10000; d++; }
    v += 10000;
    b[4] = d;

    d = -1;
    while (v >= 0) { v -= 1000; d++; }
    v += 1000;
    b[3] = d;

    d = -1;
    while (v >= 0) { v -= 100; d++; }
    v += 100;
    b[2] = d;

    d = -1;
    while (v >= 0) { v -= 10; d++; }
    v += 10;
    b[1] = d;

    b[0] = v;
}

void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out)
{
    u16 *p = out;
    u16 r = ratio;
    u16 n = count;

    n--;
    while (n != 0xFFFF) {
        *p = (((*src & 31) + ((((*dst & 31) - (*src & 31)) * r) >> 8)) & 31)
           | (((*src & 0x3E0) + ((((*dst & 0x3E0) - (*src & 0x3E0)) * r) >> 8)) & 0x3E0)
           | (((*src & 0x7C00) + ((((*dst & 0x7C00) - (*src & 0x7C00)) * r) >> 8)) & 0x7C00);
        p++;
        src++;
        dst++;
        n--;
    }
}

u16 BlendColor(u16 a, u16 b, u16 ratio)
{
    return (((a & 31) + ((((b & 31) - (a & 31)) * ratio) >> 8)) & 31)
         | (((a & 0x3E0) + ((((b & 0x3E0) - (a & 0x3E0)) * ratio) >> 8)) & 0x3E0)
         | (((a & 0x7C00) + ((((b & 0x7C00) - (a & 0x7C00)) * ratio) >> 8)) & 0x7C00);
}
