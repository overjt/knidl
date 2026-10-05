#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "player.h"

/* cutscene_fountain_blend.c (0x08019EEC-0x0801A07B, issue #125).
 *
 * Animation script 62, the last entry (70) of gCutsceneStarts: a timed blend
 * sequence.  It turns alpha blending on (the BLDCNT shadows gBldCntTarget1 and
 * gBldCntTarget2), holds for 272 frames, cross-fades the BLDALPHA shadows
 * (gBldAlphaEva down, gBldAlphaEvb up) over 64 frames, holds for 798 more,
 * then blends the 128 colours of the palette buffer gBgPaletteBank8 from
 * gUnk_085E2920 to gUnk_085E2A20, back, and from gUnk_085E2920 to
 * gUnk_085E2B20, 16 frames each (the same three palettes M38's ending uses,
 * src/ending_star_rod_return.c), and ends the task.
 *
 * Matching note (issue #125): the four blend shadows are vu8, as everywhere
 * else in src/; the ROM re-reads gBldAlphaEvb right after storing it
 * (lesson 3.472). */

void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);

void CutsceneFountainActorScript62(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMove;
    t->drawCallback = 0;
    gBldCntTarget1 = 66;
    gBldCntTarget2 = 29;
    gBldAlphaEva = 16;
    gBldAlphaEvb = 0;
    TaskYieldTrampoline(112);
    TaskYieldTrampoline(160);
    gCurTask->cutsceneActorLoopCount = 0;
    do
    {
        gBldAlphaEvb = (s16)gCurTask->cutsceneActorLoopCount >> 2;
        gBldAlphaEva = 16 - gBldAlphaEvb;
        TaskYieldTrampoline(1);
        gCurTask->cutsceneActorLoopCount++;
    } while ((s16)gCurTask->cutsceneActorLoopCount <= 63);
    gBldAlphaEva = 0;
    gBldAlphaEvb = 16;
    TaskYieldTrampoline(64);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(134);
    gCurTask->cutsceneActorLoopCount = 0;
    do
    {
        BlendColors(gUnk_085E2920, gUnk_085E2A20, (u16)((s16)gCurTask->cutsceneActorLoopCount * 16), 128, gBgPaletteBank8);
        TaskYieldTrampoline(1);
        gCurTask->cutsceneActorLoopCount++;
    } while ((s16)gCurTask->cutsceneActorLoopCount <= 15);
    TaskYieldTrampoline(86);
    gCurTask->cutsceneActorLoopCount = 0;
    do
    {
        BlendColors(gUnk_085E2A20, gUnk_085E2920, (u16)((s16)gCurTask->cutsceneActorLoopCount * 16), 128, gBgPaletteBank8);
        TaskYieldTrampoline(1);
        gCurTask->cutsceneActorLoopCount++;
    } while ((s16)gCurTask->cutsceneActorLoopCount <= 15);
    gCurTask->cutsceneActorLoopCount = 0;
    do
    {
        BlendColors(gUnk_085E2920, gUnk_085E2B20, (u16)((s16)gCurTask->cutsceneActorLoopCount * 16), 128, gBgPaletteBank8);
        TaskYieldTrampoline(1);
        gCurTask->cutsceneActorLoopCount++;
    } while ((s16)gCurTask->cutsceneActorLoopCount <= 15);
    TaskSleepForever();
}
