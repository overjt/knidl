#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* Early game-code block 0x08001CC8-0x08002377 (issue #32, batch B2).
 *
 * Contents, in ROM order:
 *   DrawAffineSprite  affine/rotscale OBJ emitter: walks a sprite template list,
 *                 converts each entry into the affine OAM staging buffer at
 *                 0x03001190 and fills one 0x03000050 affine matrix.
 *   ResetBgScroll  clears the eight BG scroll shadow cells.
 *   ResetFadeAndBlend  clears the brightness/fade block and the blend/window
 *                 shadow bytes.
 *   BeginFadeInFromWhite .. BeginFadeOutToBlack  the fade-request family: each one seeds the
 *                 brightness state block (target/current level, step, flags).
 *   CheckWarmBoot  compares and refreshes the 9 header bytes at 0x03000000
 *                 against the ROM copy at 0x0872EB2C; result cached in
 *                 gWarmBoot and returned.
 *   ClearWarmBoot .. LinkRequestSync  small helpers (reset the cached flag,
 *                 re-init wrappers, OAM-shadow bookkeeping, 0x03005274 codes).
 *
 * Recipe: this translation unit is old_agbcc -O2 -mthumb-interwork
 * (fnmatch.sh --old2), NOT the default agbcc -O2 that the rest of the early
 * game code uses -- see the report/lessons note: the leaf functions here have
 * no `push {lr}` at all, which agbcc always emits.
 *
 * Three functions in this range are dead exports with no in-ROM references
 * (lesson 2.13): BeginFadeInFromBlack, BeginFadeOutToBlack and LinkRequestSync.  They sit
 * between evidenced functions and were recovered by disassembling the gaps.
 */

/* Not from main.h: this file's view of gBg2ScrollX differs (lesson 3.517). */
extern vs16 gAffineSpriteBuffer[];       /* affine OBJ staging buffer */
extern vu16 gAffineSpriteBufferPos;         /* staging buffer write index */
extern vu16 gOamBuffer[];       /* OAM shadow (attrs + affine params) */
extern vu16 gOamAffineCount;         /* affine matrix index */
extern const u8 gUnk_0872EB14[4][4][2];   /* shape/size -> {w,h} half-dims */
extern s16 gCosTable[];  /* trig table (mid pointer) */

/* BG scroll shadow cells (cleared as one volatile chain each). */
extern vs32 gBg0ScrollX;
extern vs32 gBg1ScrollX;
extern vu32 gBg2ScrollX;
extern vu32 gBg3ScrollX;
extern vs32 gBg0ScrollY;
extern vs32 gBg1ScrollY;
extern vu32 gBg2ScrollY;
extern vu32 gBg3ScrollY;

/* Brightness/fade state block. */
extern vs16 gFadeSteps;
extern vs16 gBrightness;
extern vs16 gFadeStep;
extern vu16 gFadeTimer;
extern vu16 gFadeInterval;
extern vu16 gFadeBlankAtWhite;
extern u32 gFadeKeepMask;

/* Blend/window shadow bytes. */
extern vu8 gBldCntTarget1;
extern vu8 gBldCntTarget2;
extern vu8 gBldAlphaEva;
extern vu8 gBldAlphaEvb;
extern vu16 gBldY;

extern vu32 gWarmBoot;
extern u16 gPlayTime[4];
extern vu16 gPlayerPressedKeys[];
extern vu16 gPlayerHeldKeys[];
extern u16 gLinkCommand;
extern u16 gLinkIsMaster;
extern const u8 gBootSignature[];

extern void ResetOamShadow(void);
extern void RunBuildOamInIwram(void);
extern void EndFrame(void);
extern void ResetSpriteQueue(void);

void ResetBgScroll(void)
{
    gBg0ScrollX = gBg1ScrollX = gBg2ScrollX = gBg3ScrollX = 0;
    gBg0ScrollY = gBg1ScrollY = gBg2ScrollY = gBg3ScrollY = 0;
}

void ResetFadeAndBlend(void)
{
    gBrightness = gFadeStep = gFadeTimer = gFadeInterval = gFadeSteps = gFadeBlankAtWhite = gFadeKeepMask = 0;
    gBldCntTarget1 = gBldCntTarget2 = gBldAlphaEva = gBldAlphaEvb = gBldY = 0;
}

void BeginFadeInFromWhite(void)
{
    gFadeSteps = gBrightness = 31;
    gFadeStep = -1;
    gFadeTimer = 0;
    gFadeInterval = 1;
    gFadeBlankAtWhite = 1;
    gFadeKeepMask = 0;
}

void BeginFadeInFromBlack(void)
{
    gFadeSteps = 31;
    gBrightness = -31;
    gFadeStep = 1;
    gFadeTimer = 0;
    gFadeInterval = 1;
    gFadeBlankAtWhite = 1;
    gFadeKeepMask = 0;
}

void BeginFastFadeInFromWhite(void)
{
    gFadeSteps = 16;
    gBrightness = 32;
    gFadeStep = -2;
    gFadeTimer = 0;
    gFadeInterval = 1;
    gFadeBlankAtWhite = 1;
    gFadeKeepMask = 0;
}

void BeginFadeOutToWhite(void)
{
    gFadeSteps = 31;
    gBrightness = 0;
    gFadeStep = 1;
    gFadeTimer = 0;
    gFadeInterval = 1;
    gFadeBlankAtWhite = 1;
    gFadeKeepMask = 0;
}

void BeginFastFadeOutToWhite(void)
{
    gFadeSteps = 16;
    gBrightness = 0;
    gFadeStep = 2;
    gFadeTimer = 0;
    gFadeInterval = 1;
    gFadeBlankAtWhite = 1;
    gFadeKeepMask = 0;
}

void BeginFadeOutToBlack(void)
{
    gFadeSteps = 31;
    gBrightness = 0;
    gFadeStep = -1;
    gFadeTimer = 0;
    gFadeInterval = 1;
    gFadeBlankAtWhite = 0;
    gFadeKeepMask = 0;
}

u32 CheckWarmBoot(void)
{
    vu8 *p = (vu8 *)IWRAM_START;
    u32 ok = 1;
    u32 i;
    u32 c;

    for (i = 0; i < 9; i++)
    {
        c = gBootSignature[i];
        if (*p != c)
            ok = 0;
        *p++ = c;
    }
    gWarmBoot = ok;
    return ok;
}

u32 ClearWarmBoot(u32 arg)
{
    gWarmBoot = 0;
    return arg;
}

void ResetTasksAndOam(void)
{
    InitTasks();
    ResetOamShadow();
}

void ResetPlayTime(void)
{
    gPlayTime[3] = 0;
    gPlayTime[2] = 0;
    gPlayTime[1] = 0;
    gPlayTime[0] = 0;
}

void RunFrameNoTasks(void)
{
    RunBuildOamInIwram();
    EndFrame();
    ResetSpriteQueue();
}

void RunFrame(void)
{
    RunTasks();
    RunBuildOamInIwram();
    EndFrame();
    ResetSpriteQueue();
}

void LinkStartKeyExchange(void)
{
    s32 player;

    for (player = 0; player < 4; player++)
        gPlayerHeldKeys[player] = gPlayerPressedKeys[player] = 0;
    gLinkCommand = 0x8800;
}

void LinkStopKeyExchange(void)
{
    gLinkCommand = 0x9900;
}

void LinkStartRecordExchange(void)
{
    gLinkCommand = 0x6600;
}

void LinkRequestSync(void)
{
    if (gLinkIsMaster != 0)
        gLinkCommand = 0x7755;
}
