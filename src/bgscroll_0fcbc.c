#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* bgscroll_0fcbc.c (0x0800FCBC-0x080100AB, issue #99).
 *
 * The menus' BG scroll animator and small shared helpers.  Up to eight
 * scrolls run at once, one per BG and axis: gBgScrollActive holds the
 * running bits (bit bg = x, bit 4 + bg = y), gBgScrollSpeeds[axis][bg] the
 * speed and gBgScrollTargets[axis][bg] the target, and the BGnHOFS/BGnVOFS
 * shadow cells are reached through the pointer tables gBgScrollXPtrs and
 * gBgScrollYPtrs.  BgScrollInit resets it, BgScrollStart/BgScrollStartX/
 * BgScrollStartY/BgScrollStartBg3Slide start scrolls, BgScrollFinish snaps them to
 * their targets and task type #257 (Task_BgScroll) steps them every frame.
 * Also: sub_0800ffd8 (link work byte 1 minus one, floored at 0),
 * TaskIsOnScreenNoCamera (is the current task on screen), SetBlend (the four
 * blend shadow bytes) and SetWindow (window 0/1 setup). */

extern u8 gBgScrollActive;
extern s32 gBgScrollSpeeds[2][4];
extern s32 gBgScrollTargets[2][4];
extern vu8 gUnk_0200EBC0[];
extern vu8 gBldCntTarget2;
extern vu16 gWin0V;
extern vu16 gWin1V;
extern vu8 gBldAlphaEva;
extern vu8 gWinIn0;
extern vu8 gUnk_03000F7C;
extern vs32 gBg3ScrollY;
extern vu16 gWin0H;
extern vu16 gWin1H;
extern vu8 gBldCntTarget1;
extern vu8 gBldAlphaEvb;
extern vu8 gWinIn1;
extern vu16 gDispCnt;
extern vs32 *const gBgScrollYPtrs[4];
extern vs32 *const gBgScrollXPtrs[4];

void TaskYieldTrampoline(s32 frames);
void BgScrollStartX(s32 speed, s32 dist, s32 bg);
void BgScrollStartY(s32 speed, s32 dist, s32 bg);

void BgScrollInit(void)
{
    s32 i;

    gBgScrollActive = 0;
    for (i = 0; i <= 3; i++) {
        gBgScrollSpeeds[0][i] = 0;
        gBgScrollTargets[0][i] = 0;
        *gBgScrollYPtrs[i] = 0;
        *gBgScrollXPtrs[i] = 0;
    }
}

void BgScrollStart(s32 xspeed, s32 yspeed, s32 xdist, s32 ydist, s32 bg)
{
    if (xspeed != 0)
        BgScrollStartX(xspeed, xdist, bg);
    if (yspeed != 0)
        BgScrollStartY(yspeed, ydist, bg);
}

void BgScrollStartX(s32 speed, s32 dist, s32 bg)
{
    s32 v;

    gBgScrollSpeeds[0][bg] = speed;
    if ((gBgScrollActive >> bg) & 1) {
        if (speed > 0)
            gBgScrollTargets[0][bg] += dist << 16;
        else
            gBgScrollTargets[0][bg] -= dist << 16;
    } else {
        gBgScrollActive |= 1 << bg;
        v = *gBgScrollXPtrs[bg] & 0x01FF0000;
        *gBgScrollXPtrs[bg] = v;
        if (speed > 0)
            gBgScrollTargets[0][bg] = v + (dist << 16);
        else
            gBgScrollTargets[0][bg] = v - (dist << 16);
    }
}

void BgScrollStartY(s32 speed, s32 dist, s32 bg)
{
    s32 v;

    gBgScrollSpeeds[1][bg] = speed;
    if (gBgScrollActive & (16 << bg)) {
        if (speed > 0)
            gBgScrollTargets[1][bg] += dist << 16;
        else
            gBgScrollTargets[1][bg] -= dist << 16;
    } else {
        gBgScrollActive |= 16 << bg;
        v = *gBgScrollYPtrs[bg] & 0x01FF0000;
        *gBgScrollYPtrs[bg] = v;
        if (speed > 0)
            gBgScrollTargets[1][bg] = v + (dist << 16);
        else
            gBgScrollTargets[1][bg] = v - (dist << 16);
    }
}

s32 BgScrollStartBg3Slide(s32 speed)
{
    if (speed > 0)
        gBg3ScrollY = 0;
    else
        gBg3ScrollY = 144 << 16;
    gBgScrollTargets[1][3] &= 0x01FF0000;
    BgScrollStartY(speed, 144, 3);
}

void BgScrollFinish(void)
{
    s32 i;

    for (i = 0; i <= 3; i++) {
        if ((gBgScrollActive >> i) & 1) {
            *gBgScrollXPtrs[i] = gBgScrollTargets[0][i];
            gBgScrollActive &= ~(1 << i);
        }
        if (gBgScrollActive & (16 << i)) {
            *gBgScrollYPtrs[i] = gBgScrollTargets[1][i];
            gBgScrollActive &= ~(16 << i);
        }
    }
}

void Task_BgScroll(void)
{
    s32 i;
    s32 v;

    for (;;) {
        for (i = 0; i <= 3; i++) {
            if ((gBgScrollActive >> i) & 1) {
                v = *gBgScrollXPtrs[i] + gBgScrollSpeeds[0][i];
                *gBgScrollXPtrs[i] = v;
                if ((gBgScrollSpeeds[0][i] > 0 && v >= gBgScrollTargets[0][i])
                    || (gBgScrollSpeeds[0][i] < 0 && v <= gBgScrollTargets[0][i])) {
                    *gBgScrollXPtrs[i] = gBgScrollTargets[0][i];
                    gBgScrollActive &= ~(1 << i);
                }
            }
            if (gBgScrollActive & (16 << i)) {
                v = *gBgScrollYPtrs[i] + gBgScrollSpeeds[1][i];
                *gBgScrollYPtrs[i] = v;
                if ((gBgScrollSpeeds[1][i] > 0 && v >= gBgScrollTargets[1][i])
                    || (gBgScrollSpeeds[1][i] < 0 && v <= gBgScrollTargets[1][i])) {
                    *gBgScrollYPtrs[i] = gBgScrollTargets[1][i];
                    gBgScrollActive &= ~(16 << i);
                }
            }
        }
        TaskYieldTrampoline(1);
    }
}

s32 sub_0800ffd8(void)
{
    s32 n = gUnk_0200EBC0[1];

    if (n != 0)
        n--;
    return n;
}

u8 TaskIsOnScreenNoCamera(void)
{
    struct Task *t = gCurTask;
    s16 x = t->unk48;
    s16 y = t->unk4A;

    if ((u16)(x + 63) > 366)
        return 0;
    if (y <= -64)
        return 0;
    if (y > 223)
        return 0;
    return 1;
}

void SetBlend(s32 a, s32 b, s32 c, s32 d)
{
    gBldCntTarget1 = a;
    gBldCntTarget2 = b;
    gBldAlphaEva = c;
    gBldAlphaEvb = d;
}

void SetWindow(s32 in, s32 out, s32 h, s32 v, s32 win)
{
    if (win == 0x2000) {
        gWinIn0 = in;
        gWin0H = h;
        gWin0V = v;
    }
    if (win == 0x4000) {
        gWinIn1 = in;
        gWin1H = h;
        gWin1V = v;
    }
    gUnk_03000F7C = out;
    gDispCnt |= win;
}
