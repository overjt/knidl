#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "mode.h"
#include "hud.h"
#include "room.h"
#include "player.h"
#include "actor.h"

/* hud_0a130.c (0x0800A130-0x0800AACF, issue #96).
 *
 * HUD/score state updates: the life/health/score/timer changes, the
 * per-player bar records gHudHpBars[] and the two 5-way state switches
 * (HudOpenAbilityPanel, HudCloseAbilityPanel) over gHudAbilityPanelState. */

void PlaySfx(s32 id);

void HudShowAbility(s32 a, s32 id)
{
    if (id == gLocalPlayer && gHudMode == 1) {
        if (a == 0) {
            HudClearAbilityPicture();
            gHudAbilityPanelState = 0;
            gUnk_0200801C = 0;
            HudDrawAbilityPanel(0);
        } else {
            HudLoadAbilityPicture(a);
        }
    }
}

void sub_0800a178(s32 a, s32 id)
{
    if (id == gLocalPlayer && gHudMode == 1)
        gUnk_0200801C = a;
}

void HudOpenAbilityPanel(s32 id)
{
    if (id == gLocalPlayer && gHudMode == 1) {
        switch (gHudAbilityPanelState) {
        case 0:
            gHudAbilityPanelState = 2;
            gUnk_0200801C = 0;
            break;
        case 1:
            gHudAbilityPanelState = 2;
            gUnk_0200801C = 16;
            break;
        case 2:
            break;
        case 3:
            gUnk_0200801C = 180;
            break;
        case 4:
            gHudAbilityPanelState = 2;
            break;
        }
    }
}

void HudCloseAbilityPanel(s32 id)
{
    if (id == gLocalPlayer && gHudMode == 1) {
        switch (gHudAbilityPanelState) {
        case 0:
        case 1:
            break;
        case 2:
            gHudAbilityPanelState = 4;
            break;
        case 3:
            gHudAbilityPanelState = 4;
            gUnk_0200801C = 48;
            break;
        case 4:
            break;
        }
    }
}

void HudShowHpBar(void)
{
    gHudShowsHpBar = 1;
    HudDrawHpBarFrame();
}

void HudStartHpBar(s32 max, s32 cur)
{
    if (gUnk_030023B0 != 0) {
        gHudHpBarMaxHp = max;
        gHudHpBarLength = 32;
        gHudHpBarValues[0] = 32;
        gHudHpBarFilled = 1;
        HudDrawHpBar(32);
        gHudHpBarIndex = 0;
        gHudHpBars[0].unk0 = 0;
        gHudHpBars[0].unk4 = gHudHpBarValues[0];
        gHudHpBars[0].unk2 = gHudHpBarValues[0];
        gHudHpBars[0].unk6 = 0;
    } else {
        gHudHpBarMaxHp = max;
        gHudHpBarLength = 32;
        gHudHpBarValues[0] = Div(cur << 5, gHudHpBarMaxHp);
        if (gHudHpBarValues[0] > gHudHpBarLength)
            gHudHpBarValues[0] = gHudHpBarLength;
        HudResetHpBar(0);
        HudDrawHpBar(0);
        HudStartHpBarFill(0, gHudHpBarValues[0]);
    }
}

void HudStartTaskHpBar(s32 max, s32 cur)
{
    s32 idx;
    struct HudBar *p;

    if (gUnk_020055D0 == 1)
        idx = 0;
    else
        idx = gUnk_02005590[gCurTaskIdx - 32];
    if (gUnk_030023B0 != 0) {
        if (gHudHpBarMaxHp == 0) {
            gHudHpBarMaxHp = max;
            gHudHpBarLength = 32;
        }
        gHudHpBarValues[idx] = gHudHpBarLength;
        gHudHpBarFilled = 1;
        HudDrawHpBar(32);
        gHudHpBarIndex = idx;
        p = &gHudHpBars[idx];
        p->unk0 = 0;
        p->unk4 = gHudHpBarValues[idx];
        p->unk2 = gHudHpBarValues[idx];
        p->unk6 = 0;
    } else {
        if (gHudHpBarMaxHp == 0) {
            gHudHpBarMaxHp = max;
            gHudHpBarLength = 32;
        }
        gHudHpBarValues[idx] = Div(cur << 5, gHudHpBarMaxHp);
        HudResetHpBar(idx);
        HudDrawHpBar(0);
        HudAnimateHpBar(0, gHudHpBarValues[idx], idx);
    }
}

void HudAnimateTaskHpBar(void)
{
    s32 idx;
    s32 v;

    if (gHudHpBarMaxHp != 0) {
        if (gUnk_020055D0 == 1)
            idx = 0;
        else
            idx = gUnk_02005590[gCurTaskIdx - 32];
        if (gUnk_02008014[idx] != -1 && gCurTask->health > 0) {
            v = Div(gCurTask->health << 5, gHudHpBarMaxHp);
            if (v != gHudHpBarValues[idx])
                HudAnimateHpBar(gHudHpBarValues[idx], v, idx);
            gHudHpBarValues[idx] = v;
        }
    }
}

void HudSetTaskHpBar(void)
{
    s32 idx;
    s32 v;

    if (gHudHpBarMaxHp != 0) {
        if (gUnk_020055D0 == 1)
            idx = 0;
        else
            idx = gUnk_02005590[gCurTaskIdx - 32];
        if (gUnk_02008014[idx] != -1) {
            if (gCurTask->health > 0)
                v = Div(gCurTask->health << 5, gHudHpBarMaxHp);
            else
                v = 0;
            gHudHpBarValues[idx] = v;
            HudSetHpBar(v, idx);
        }
    }
}

void HudRemoveHpBar(void)
{
    s32 idx;
    s32 i;

    if (gUnk_020055D0 == 1)
        idx = 0;
    else
        idx = gUnk_02005590[gCurTaskIdx - 32];
    if (gUnk_02008014[idx] != -1) {
        gUnk_02008014[idx] = -1;
        if (--gUnk_02000034 != 0) {
            if (gHudHpBarIndex == idx) {
                for (i = 0; i < 2; i++) {
                    if (i != idx && gUnk_02005590[gUnk_02008014[i] - 32] != -1)
                        break;
                }
                if (gHudMode == 1)
                    HudSetHpBar(gHudHpBarValues[i], idx);
            }
        } else {
            if (gHudMode == 1) {
                HudClearTiles(20, 18, 2);
                HudClearTiles(20, 19, 2);
                if (gHudShowsClock == 0)
                    HudDrawScore(gPlayerScores[gLocalPlayer]);
                else
                    HudDrawClock(gHudClock);
            }
            gHudShowsHpBar = 0;
            gHudHpBarMaxHp = gHudHpBarLength = 0;
        }
    }
}

void HudStopClock(void)
{
    gFrameEndCallback = 0;
}

void sub_0800a6a4(void)
{
    u8 mode = gHudMode;
    s32 row;

    if (mode != 0) {
        if (mode == 1) {
            if (gUnk_020055F0[0] == 0) {
                if (gUnk_02005E00.unk04[gLocalPlayer] != 0) {
                    gUnk_020055F0[0] = mode;
                    gUnk_020055F0[1] = 0;
                }
                if (gUnk_020055F0[0] == 0)
                    goto done;
            }
            if (gUnk_02005E00.unk04[gLocalPlayer] == 0) {
                gUnk_020055F0[0] = 0;
                row = gUnk_03002444 ? 2 : 0;
                HudClearTiles(12, row, 16);
                HudClearTiles(12, row + 1, 16);
            } else {
                if (gUnk_020055F0[1] == 0) {
                    if (gUnk_020055F0[0] == 2) {
                        gUnk_020055F0[0] = 1;
                        sub_0800b230(gUnk_02005E00.unk04[gLocalPlayer] >> 4, 1);
                    } else {
                        gUnk_020055F0[0] = 2;
                        sub_0800b230(gLocalPlayer, 2);
                    }
                    gUnk_020055F0[1] = 90;
                }
                gUnk_020055F0[1]--;
            }
        }
    done:
        HudRedrawClock();
        HudFlushTilemap();
    }
}

void HudUpdateAbilityPanel(void)
{
    if (gHudMode != 0 && gUnk_0200617C != 0) {
        switch (gHudAbilityPanelState) {
        case 0:
        case 1:
            break;
        case 2:
            gUnk_0200801C += 8;
            gUnk_03002448 = gUnk_0200801C;
            if (gUnk_03002448 > 47) {
                gUnk_03002448 = 48;
                gHudAbilityPanelState = 3;
                gUnk_0200801C = 180;
            }
            HudDrawAbilityPanel(gUnk_03002448 >> 3);
            break;
        case 3:
            if (--gUnk_0200801C <= 0)
                HudCloseAbilityPanel(gLocalPlayer);
            break;
        case 4:
            gUnk_0200801C -= 8;
            gUnk_03002448 = gUnk_0200801C;
            if (gUnk_03002448 <= 16) {
                gUnk_03002448 = 16;
                gHudAbilityPanelState = 1;
                gUnk_0200801C = 0;
            }
            HudDrawAbilityPanel(gUnk_03002448 >> 3);
            break;
        }
    }
}

void HudUpdateHpBars(void)
{
    s32 i;
    struct HudBar *p;
    s32 v;

    if (gHudMode != 0 && gHudShowsHpBar == 1) {
        for (i = 0; i < 2; i++) {
            p = &gHudHpBars[i];
            switch (p->unk0) {
            case 0:
                break;
            case 1:
                if (--p->unk6 > 0)
                    break;
                PlaySfx(221);
                v = p->unk4 + 1;
                if (v > p->unk2)
                    v = p->unk2;
                if (gHudHpBarIndex == i)
                    HudDrawHpBarChange(p->unk4, v);
                p->unk4 = v;
                if (p->unk4 == p->unk2) {
                    p->unk0 = 0;
                    gHudHpBarFilled = 1;
                } else {
                    p->unk6 = 4;
                }
                break;
            case 2:
                if (--p->unk6 > 0)
                    break;
                PlaySfx(221);
                v = p->unk4 + 1;
                if (v > p->unk2)
                    v = p->unk2;
                if (gHudHpBarIndex == i)
                    HudDrawHpBarChange(p->unk4, v);
                p->unk4 = v;
                if (p->unk4 == p->unk2)
                    p->unk0 = 0;
                else
                    p->unk6 = 4;
                break;
            case 3:
                if (--p->unk6 > 0)
                    break;
                v = p->unk4 - 1;
                if (v < p->unk2)
                    v = p->unk2;
                if (gHudHpBarIndex == i)
                    HudDrawHpBarChange(p->unk4, v);
                p->unk4 = v;
                if (p->unk4 == p->unk2)
                    p->unk0 = 0;
                else
                    p->unk6 = 4;
                break;
            }
        }
    }
}

void HudAnimateHpBar(s32 from, s32 to, s32 i)
{
    struct HudBar *p;
    s32 d;

    if (gHudMode != 0 && gHudShowsHpBar == 1) {
        p = &gHudHpBars[i];
        if (p->unk0 != 1) {
            HudDrawHpBar(p->unk4);
            if (p->unk0 != 0) {
                d = to - p->unk4;
                p->unk2 = to;
                if (d > 0)
                    p->unk0 = 2;
                else
                    p->unk0 = 3;
            } else {
                d = to - from;
                if (d != 0) {
                    p->unk4 = from;
                    p->unk2 = to;
                    p->unk6 = 4;
                    if (d > 0)
                        p->unk0 = 2;
                    else
                        p->unk0 = 3;
                }
            }
            gHudHpBarIndex = i;
        }
    }
}

s32 HudStartHpBarFill(s32 from, s32 to)
{
    s32 i;

    if (gHudMode != 0 && gHudShowsHpBar == 1 && to > from) {
        for (i = 0; i < 2; i++) {
            gHudHpBars[i].unk4 = from;
            gHudHpBars[i].unk2 = to;
            gHudHpBars[i].unk6 = 4;
            gHudHpBars[i].unk0 = 1;
        }
        gHudHpBarIndex = 0;
        gHudHpBarFilled = 0;
    }
}

void HudSetHpBar(s32 x, s32 i)
{
    sub_0800aaac(i);
    HudDrawHpBar(x);
    gHudHpBarIndex = i;
}

void HudResetHpBar(s32 i)
{
    struct HudBar *p = &gHudHpBars[i];

    p->unk0 = 0;
    p->unk4 = 0;
    p->unk2 = 0;
    p->unk6 = 0;
}

void sub_0800aaac(s32 i)
{
    struct HudBar *p = &gHudHpBars[i];

    p->unk0 = 0;
    p->unk4 = gHudHpBarValues[i];
    p->unk2 = gHudHpBarValues[i];
    p->unk6 = 0;
}
