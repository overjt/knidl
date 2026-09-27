#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* boot_091ac.c (0x080091AC-0x080099FB, issue #96).
 *
 * The boot and title sequence.  AgbMain state 1 (BootLogoMain) runs the
 * skippable logo sequence PlayBootLogo, whose frame waits BootLogoWait(n)
 * return 1 on A/B/START (sub_080093cc is a dead twin) and whose task type
 * #0 (Task_BootLogo) runs every frame.  State 3 (TitleMain) alternates
 * the title screen TitleScreen - task type #1 (Task_TitlePalette) animates the
 * title palette, task type #2 (Task_TitleSprites) spawns ten sprite children
 * (sub_08009640) - with the nine-scene intro story IntroStory. */

extern vs32 gBg0ScrollY;
extern vu16 gPressedKeys;
extern vu8 gBldCntTarget2;
extern vu16 gWin0V;
extern vu16 gUnk_03000048;
extern vu16 gFadeStep;
extern vu8 gBldAlphaEva;
extern vu8 gWinIn0;
extern vs32 gBg3ScrollX;
extern vu8 gUnk_03000F7C;
extern vs32 gBg2ScrollX;
extern vs32 gBg3ScrollY;
extern vs16 gBrightness;
extern vs32 gBg1ScrollY;
extern vu16 gWin0H;
extern vu16 gFadeTimer;
extern vs32 gBg0ScrollX;
extern vu8 gBldCntTarget1;
extern u16 gUnk_03001430[];
extern vu16 gFadeSteps;
extern vs32 gBg2ScrollY;
extern vu16 gFadeInterval;
extern vu8 gBldAlphaEvb;
extern u16 *gFadeKeepMask;
extern vu16 gDispCnt;
extern vs32 gBg1ScrollX;
extern s32 gUnk_03001F2C;
extern u16 gUnk_03002150;
extern u16 gLinkPlayerCount;
extern u16 gUnk_08541D98[][16];
extern u16 gUnk_08541F58[];
extern u16 gUnk_08731C88[];
extern u16 gUnk_08731CC8[];
extern u8 gUnk_08731CDC[];
extern u16 gUnk_08731CE6[];
extern u32 gUnk_087555B4[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
u32 BeginFade(u16 steps, u16 delta, u16 *mask);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void BeginFadeInFromWhite(void);
void BeginFastFadeInFromWhite(void);
void BeginFadeOutToWhite(void);
void BeginFastFadeOutToWhite(void);
void ResetTasksAndOam(void);
void RunFrame(void);
void RunFrames(s32 count);
void RunFramesUntilFadeDone(void);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlayBgm(s32 songId);
s32 PlaySfx(s32 id);
void StopAllSfx(void);
void SetBgmVolume(u16 volume);
s32 TaskCreateFrom(u32 type, s32 idx);
void TaskMove(void);
void TaskDrawScreen(void);
void TaskSleepForever(void);
void LoadBgLayout(s32 a0);
void LoadGfxSet(u16 a0);
void BootLogoInitObjects(void);
void BootLogoUpdateObjects(void);
s32 PlayBootLogo(void);
s32 BootLogoWait(s32 n);
void sub_080095e4(void);
void sub_08009640(void);
s32 TitleScreen(void);
void IntroStory(void);
s32 sub_080099c8(s32 n);

void BootLogoMain(void)
{
    s32 i, vol;

    if (PlayBootLogo() == 1) {
        BeginFade(16, 2, gUnk_08731C88);
        vol = 256;
        for (i = 0; i < 16; i++) {
            SetBgmVolume(vol);
            RunFrame();
            vol -= 16;
        }
        StopAllSfx();
        SetBgmVolume(256);
        ResetTasksAndOam();
    } else {
        gUnk_03002150 = 1;
    }
}

s32 PlayBootLogo(void)
{
    s32 i;

    ResetTasksAndOam();
    BootLogoInitObjects();
    gLinkPlayerCount = 0x9999;
    LoadBgLayout(0);
    LoadGfxSet(6);
    LoadGfxSet(7);
    gBg0ScrollY = gBg0ScrollX = gBg1ScrollY = gBg1ScrollX = gBg2ScrollY = gBg2ScrollX = gBg3ScrollY = gBg3ScrollX = 0;
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1100;
    BeginFastFadeInFromWhite();
    RunFramesUntilFadeDone();
    RunFrames(60);
    TaskCreateFrom(0, 0);
    RunFrames(60);
    gLinkPlayerCount = 1;
    if (BootLogoWait(70) != 0)
        return 1;
    PlaySfx(0x10D);
    if (BootLogoWait(35) != 0)
        return 1;
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1F00;
    if (BootLogoWait(21) != 0)
        return 1;
    gWinIn0 = 49;
    gUnk_03000F7C = 62;
    gWin0H = 255;
    gWin0V = 160;
    gDispCnt |= 0x2000;
    for (i = 85; i >= 0; i--) {
        gWin0H = i * 3;
        RunFrame();
    }
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1E00;
    gDispCnt &= 0xDFFF;
    return 0;
}

s32 BootLogoWait(s32 n)
{
    s32 i;

    for (i = 0; i < n; i++) {
        if (gPressedKeys & 11)
            return 1;
        RunFrame();
    }
    return 0;
}

s32 sub_080093cc(void)
{
    while (gFadeSteps != 0) {
        if (gPressedKeys & 11)
            return 1;
        RunFrame();
    }
    return 0;
}

void Task_BootLogo(void)
{
    struct Task *t = gCurTask;

    t->unk00 = 0;
    t->unk0C = 0;
    for (;;) {
        BootLogoUpdateObjects();
        TaskYieldTrampoline(1);
    }
}

void Task_TitlePalette(void)
{
    struct Task *t = gCurTask;
    s32 *p;
    u16 *q;

    t->unk00 = 0;
    t->unk0C = 0;
    for (t->unk6C = 0; (s16)gCurTask->unk6C <= 10; gCurTask->unk6C++) {
        RequestCopy(2, (u32)gUnk_08541D98[(s16)gCurTask->unk6C], (u32)gUnk_03001430, 32);
        TaskYieldTrampoline(1);
    }
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 11; gCurTask->unk6C++) {
        BlendColors(gUnk_08541D98[13], gUnk_08541D98[11], (u16)(((s16)gCurTask->unk6C + 1) * 21), 16, gUnk_03001430);
        BlendColors(gUnk_08541F58, gUnk_08541F58 + 8, (u16)(((s16)gCurTask->unk6C + 1) * 21), 8, gUnk_03001430 + 17);
        TaskYieldTrampoline(1);
    }
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 7; gCurTask->unk6C++) {
        BlendColors(gUnk_08541D98[11], gUnk_08541D98[12], (u16)(((s16)gCurTask->unk6C + 1) * 32), 16, gUnk_03001430);
        BlendColors(gUnk_08541F58 + 8, gUnk_08541F58, (u16)(((s16)gCurTask->unk6C + 1) * 32), 8, gUnk_03001430 + 17);
        TaskYieldTrampoline(1);
    }
    /* Loop 4: the ROM hoists the store's &gUnk_03001F2C (after the task
       address) but reloads it from the pool for the u16 re-read.  The store
       goes through p, set inside the outer loop so loop.c hoists it in that
       order; the read goes through q, which is also assigned here, before
       the loop.  That dead assignment makes q live outside the loop, so
       loop.c does not merge q's in-loop load with p (combine_movables skips
       loop-global regs).  q's load then stays in the loop, as in the ROM.
       With a plain `(u16)gUnk_03001F2C` read, GCSE (store address in the
       same iteration) or combine_movables (p in the loop) gives the read a
       hoisted register instead. */
    q = (u16 *)&gUnk_03001F2C;
    for (;;) {
        gCurTask->unk6C = 0;
        p = &gUnk_03001F2C;
        for (; (s16)gCurTask->unk6C <= 15; gCurTask->unk6C++) {
            *p = ((s16)gCurTask->unk6C > 7 ? 16 - (s16)gCurTask->unk6C : (s16)gCurTask->unk6C) << 5;
            q = (u16 *)&gUnk_03001F2C;
            BlendColors(gUnk_08541D98[12], gUnk_08541D98[13], *q, 16, gUnk_03001430);
            TaskYieldTrampoline(1);
        }
    }
}

void Task_TitleSprites(void)
{
    if (gCurTask->unk18 == -1)
        sub_080095e4();
    else
        sub_08009640();
}

void sub_080095e4(void)
{
    struct Task *t;
    s32 idx;

    TaskYieldTrampoline(gCurTask->unk1C);
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < 10; gCurTask->unk6C++) {
        idx = TaskCreateFrom(2, 0);
        t = gCurTask;
        t->unk28 = idx;
        gTasks[idx].unk18 = (s16)t->unk6C;
        TaskYieldTrampoline(3);
    }
    TaskExitTrampoline();
}

void sub_08009640(void)
{
    struct Task *t, *u, *v;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)TaskDrawScreen;
    t->unk38 = gUnk_087555B4;
    t->unk42 = 8;
    u = gCurTask;
    if (u->unk18 <= 4) {
        u->unk4C = u->unk18 * 0x140000 + 0x180000;
        u->unk50 = 0x800000;
    } else {
        u->unk4C = (u->unk18 - 5) * 0x140000 + 0x880000;
        u->unk50 = 0x800000;
    }
    gCurTask->unk3C = 8;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    v = gCurTask;
    v->unk3C = gUnk_08731CC8[v->unk18];
    TaskSleepForever();
}

void TitleMain(void)
{
    while (TitleScreen() == 0)
        IntroStory();
    gFadeSteps = 10;
    gBrightness = 2;
    gFadeStep = 3;
    gFadeTimer = 0;
    gFadeInterval = 1;
    gUnk_03000048 = 1;
    gFadeKeepMask = 0;
    RunFramesUntilFadeDone();
}

/* The key-wait loop is `while (1)` with both exits as gotos (a `break` would
   make expand_end_loop rotate it), and the result goes through one `ret`
   local: with a plain `return 0` the done block ends in a jump and jump.c's
   "if (foo) bar; else break;" swap moves it in front of the loop body. */
s32 TitleScreen(void)
{
    s32 idx, i, ret;

    if (gUnk_03002150 != 1) {
        ResetTasksAndOam();
        LoadBgLayout(0);
        LoadGfxSet(7);
        gBg0ScrollY = gBg0ScrollX = gBg1ScrollY = gBg1ScrollX = gBg2ScrollY = gBg2ScrollX = gBg3ScrollY = gBg3ScrollX = 0;
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1E00;
    }
    if (gUnk_03002150 != 1) {
        BeginFastFadeInFromWhite();
        RunFramesUntilFadeDone();
        idx = TaskCreateFrom(2, 0);
        gTasks[idx].unk18 = -1;
        gTasks[idx].unk1C = 0;
    } else {
        idx = TaskCreateFrom(2, 0);
        gTasks[idx].unk18 = -1;
        gTasks[idx].unk1C = 90;
    }
    TaskCreateFrom(1, 0);
    PlayBgm(26);
    if (gUnk_03002150 != 1)
        RunFrames(60);
    else
        RunFrames(210);
    i = 600;
    while (1) {
        if (--i == 0)
            goto timeout;
        RunFrame();
        if (gPressedKeys & 9)
            goto pressed;
    }
pressed:
    ret = 1;
    goto out;
timeout:
    gUnk_03002150 = 3;
    BeginFastFadeOutToWhite();
    RunFramesUntilFadeDone();
    ret = 0;
out:
    return ret;
}

void IntroStory(void)
{
    s32 i;
    s32 j;
    s32 t;

    ResetTasksAndOam();
    LoadBgLayout(1);
    LoadGfxSet(8);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1C00;
    gBldCntTarget1 = 84;
    gBldCntTarget2 = 8;
    gBldAlphaEva = 0;
    gBldAlphaEvb = 16;
    BeginFadeInFromWhite();
    RunFramesUntilFadeDone();
    for (i = 0; i < 9; i++) {
        LoadGfxSet(gUnk_08731CDC[i]);
        t = TaskCreateFrom(237, 0);
        gTasks[t].unk18 = i;
        for (j = 0; j <= 16; j++) {
            gBldAlphaEva = j;
            gBldAlphaEvb = 16 - j;
            if (sub_080099c8(2))
                goto end;
        }
        if (sub_080099c8(gUnk_08731CE6[i]))
            break;
        if (i == 8)
            break;
        for (j = 0; j <= 16; j++) {
            gBldAlphaEva = 16 - j;
            gBldAlphaEvb = j;
            if (sub_080099c8(2))
                goto end;
        }
    }
end:
    BeginFadeOutToWhite();
    RunFramesUntilFadeDone();
    gBldCntTarget1 = gBldCntTarget2 = gBldAlphaEva = gBldAlphaEvb = 0;
}

s32 sub_080099c8(s32 n)
{
    s32 i;

    for (i = 0; i < n; i++) {
        if (gPressedKeys & 11)
            return 1;
        RunFrame();
    }
    return 0;
}
