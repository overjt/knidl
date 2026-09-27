#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* hud_0a130.c (0x0800A130-0x0800AACF, issue #96).
 *
 * HUD/score state updates: the life/health/score/timer changes, the
 * per-player bar records gHudHpBars[] and the two 5-way state switches
 * (sub_0800a19c, sub_0800a21c) over gHudAbilityPanelState. */

struct HudBar
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
    /*0x06*/ s16 unk6;
};

struct Unk02005E00
{
    /*0x00*/ s32 unk00;
    /*0x04*/ u8 unk04[4];
    /*0x08*/ u8 unk08[4];
};

extern u8 gUnk_02000034;
extern s8 gHudHpBarIndex;
extern s16 gHudHpBarMaxHp;
extern s8 gUnk_02005590[];
extern s8 gUnk_020055D0;
extern s8 gUnk_020055F0[];
extern struct Unk02005E00 gUnk_02005E00;
extern u8 gHudMode;
extern s32 gPlayerScores[];
extern u8 gHudShowsClock;
extern u16 gUnk_02006068[];
extern s8 gUnk_0200617C;
extern s8 gHudAbilityPanelState;
extern struct HudBar gHudHpBars[];
extern s16 gUnk_02007D30;
extern s16 gHudHpBarValues[];
extern s16 gUnk_02008014[];
extern s16 gUnk_0200801C;
extern u8 gUnk_0200AFF8;
extern u8 gHudShowsHpBar;
extern void (*gFrameEndCallback)(void);
extern u16 gLocalPlayer;
extern u8 gUnk_030023B0;
extern s8 gUnk_03002444;
extern s32 gUnk_03002448;

void PlaySfx(s32 id);
void sub_08008ebc(void);
void sub_08008ed4(s32 a0);
void sub_0800ab08(void);
void HudDrawScore(s32 v);
void HudDrawClock(u16 *time);
void HudDrawAbilityPanel(s32 n);
void sub_0800b0fc(void);
void HudDrawHpBar(s32 x);
void sub_0800b190(s32 from, s32 to);
void sub_0800b230(s32 a, s32 b);
void HudClearTiles(s32 x, s32 y, s32 n);
void HudFlushTilemap(void);
void sub_0800a9a0(s32 from, s32 to, s32 i);
s32 sub_0800aa18(s32 from, s32 to);
void sub_0800aa74(s32 x, s32 i);
void sub_0800aa94(s32 i);
void sub_0800aaac(s32 i);

void HudShowAbility(s32 a, s32 id)
{
    if (id == gLocalPlayer && gHudMode == 1) {
        if (a == 0) {
            sub_08008ebc();
            gHudAbilityPanelState = 0;
            gUnk_0200801C = 0;
            HudDrawAbilityPanel(0);
        } else {
            sub_08008ed4(a);
        }
    }
}

void sub_0800a178(s32 a, s32 id)
{
    if (id == gLocalPlayer && gHudMode == 1)
        gUnk_0200801C = a;
}

void sub_0800a19c(s32 id)
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

void sub_0800a21c(s32 id)
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

void sub_0800a280(void)
{
    gHudShowsHpBar = 1;
    sub_0800b0fc();
}

void sub_0800a294(s32 max, s32 cur)
{
    if (gUnk_030023B0 != 0) {
        gHudHpBarMaxHp = max;
        gUnk_02007D30 = 32;
        gHudHpBarValues[0] = 32;
        gUnk_0200AFF8 = 1;
        HudDrawHpBar(32);
        gHudHpBarIndex = 0;
        gHudHpBars[0].unk0 = 0;
        gHudHpBars[0].unk4 = gHudHpBarValues[0];
        gHudHpBars[0].unk2 = gHudHpBarValues[0];
        gHudHpBars[0].unk6 = 0;
    } else {
        gHudHpBarMaxHp = max;
        gUnk_02007D30 = 32;
        gHudHpBarValues[0] = Div(cur << 5, gHudHpBarMaxHp);
        if (gHudHpBarValues[0] > gUnk_02007D30)
            gHudHpBarValues[0] = gUnk_02007D30;
        sub_0800aa94(0);
        HudDrawHpBar(0);
        sub_0800aa18(0, gHudHpBarValues[0]);
    }
}

void sub_0800a340(s32 max, s32 cur)
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
            gUnk_02007D30 = 32;
        }
        gHudHpBarValues[idx] = gUnk_02007D30;
        gUnk_0200AFF8 = 1;
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
            gUnk_02007D30 = 32;
        }
        gHudHpBarValues[idx] = Div(cur << 5, gHudHpBarMaxHp);
        sub_0800aa94(idx);
        HudDrawHpBar(0);
        sub_0800a9a0(0, gHudHpBarValues[idx], idx);
    }
}

void sub_0800a42c(void)
{
    s32 idx;
    s32 v;

    if (gHudHpBarMaxHp != 0) {
        if (gUnk_020055D0 == 1)
            idx = 0;
        else
            idx = gUnk_02005590[gCurTaskIdx - 32];
        if (gUnk_02008014[idx] != -1 && gCurTask->unk78 > 0) {
            v = Div(gCurTask->unk78 << 5, gHudHpBarMaxHp);
            if (v != gHudHpBarValues[idx])
                sub_0800a9a0(gHudHpBarValues[idx], v, idx);
            gHudHpBarValues[idx] = v;
        }
    }
}

void sub_0800a4c0(void)
{
    s32 idx;
    s32 v;

    if (gHudHpBarMaxHp != 0) {
        if (gUnk_020055D0 == 1)
            idx = 0;
        else
            idx = gUnk_02005590[gCurTaskIdx - 32];
        if (gUnk_02008014[idx] != -1) {
            if (gCurTask->unk78 > 0)
                v = Div(gCurTask->unk78 << 5, gHudHpBarMaxHp);
            else
                v = 0;
            gHudHpBarValues[idx] = v;
            sub_0800aa74(v, idx);
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
                    sub_0800aa74(gHudHpBarValues[i], idx);
            }
        } else {
            if (gHudMode == 1) {
                HudClearTiles(20, 18, 2);
                HudClearTiles(20, 19, 2);
                if (gHudShowsClock == 0)
                    HudDrawScore(gPlayerScores[gLocalPlayer]);
                else
                    HudDrawClock(gUnk_02006068);
            }
            gHudShowsHpBar = 0;
            gHudHpBarMaxHp = gUnk_02007D30 = 0;
        }
    }
}

void sub_0800a698(void)
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
        sub_0800ab08();
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
                sub_0800a21c(gLocalPlayer);
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
                    sub_0800b190(p->unk4, v);
                p->unk4 = v;
                if (p->unk4 == p->unk2) {
                    p->unk0 = 0;
                    gUnk_0200AFF8 = 1;
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
                    sub_0800b190(p->unk4, v);
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
                    sub_0800b190(p->unk4, v);
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

void sub_0800a9a0(s32 from, s32 to, s32 i)
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

s32 sub_0800aa18(s32 from, s32 to)
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
        gUnk_0200AFF8 = 0;
    }
}

void sub_0800aa74(s32 x, s32 i)
{
    sub_0800aaac(i);
    HudDrawHpBar(x);
    gHudHpBarIndex = i;
}

void sub_0800aa94(s32 i)
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
