#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* hud_099fc.c (0x080099FC-0x0800A12F, issue #96).
 *
 * Task type #237 (Task_IntroStoryPicture, one intro-story picture) and the
 * HUD/score interface other modules call: HUD init/redraw
 * (HudInit/HudRedraw), lives (AddPlayerLives), health
 * (AddPlayerHealth, returns the new value), score (SetPlayerAbilityNoHud/SetPlayerAbility/
 * AddPlayerScore/AddPlayerScoreNoHud, clamped to 99999999) and the clock mode
 * (HudShowScore/HudShowClock). */

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

extern u16 gPlayerAbilities[];
extern s8 gHudHpBarIndex;
extern s16 gHudHpBarMaxHp;
extern s16 gMaxHealth;
extern s16 gPlayerHealth[];
extern s8 gUnk_020055F0[];
extern struct Unk02005E00 gUnk_02005E00;
extern u8 gHudMode;
extern s32 gPlayerScores[];
extern u8 gHudShowsClock;
extern u16 gHudClock[];
extern s8 gUnk_0200617C;
extern s8 gHudAbilityPanelState;
extern struct HudBar gHudHpBars[];
extern s16 gUnk_02007D30;
extern s16 gPlayerLives[];
extern s16 gHudHpBarValues[];
extern s16 gUnk_0200801C;
extern u16 gUnk_0200AF18[];
extern u8 gHudShowsHpBar;
extern u32 gUnk_02020000[];
extern void (*gFrameEndCallback)(void);
extern u16 gObjPalette[];
extern u8 gUnk_03001F34;
extern struct PlayerState gPlayerStates[];
extern u8 gActivePlayerMask;
extern u16 gLocalPlayer;
extern s8 gUnk_03002444;
extern u8 gPlayerCameraMode[];
extern u32 gObjVram[];
extern u16 gUnk_08731CE6[];
extern u32 gUnk_087555D8[];
extern struct GfxHeader *gUnk_087555FC[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 PlaySfx(s32 id);
void TaskMove(void);
void TaskDrawScreen(void);
void HudClearAbilityPicture(void);
void HudLoadAbilityPicture(s32 a);
void HudShowAbility(s32 a, s32 id);
void sub_0800a19c(s32 a);
void HudShowHpBar(void);
void HudResetHpBar(s32 idx);
void sub_0800aad0(void);
void sub_0800ab3c(void);
void HudDrawPlayerIcon(s32 a);
void HudDrawLives(s32 n);
void HudDrawHealth(s32 n);
void sub_0800acbc(s32 a, s32 b);
void HudDrawScore(s32 v);
void HudDrawClock(u16 *time);
void HudDrawAbilityPanel(s32 n);
void HudDrawHpBar(s32 x);
void sub_0800b230(s32 a, s32 b);
void HudClearWholeTilemap(void);
void HudClearTilemap(void);
void HudFlushTilemap(void);
void HudLoadGfx(void);
void HudShowAbilityAnimated(s32 a, s32 b);

void Task_IntroStoryPicture(void)
{
    struct GfxHeader *h;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk42 = 8;
    gCurTask->unk38 = gUnk_087555D8;
    gCurTask->unk40 = 0x800;
    gCurTask->unk4C = 0x300000;
    gCurTask->unk50 = 0x780000;
    h = gUnk_087555FC[gCurTask->unk18];
    RequestCopy(2, (u32)h->unk08, (u32)gObjPalette, h->unk00 << 5);
    LZ77UnCompVram(h->unk0C, gUnk_02020000);
    RequestCopy(3, (u32)gUnk_02020000, (u32)gObjVram, h->unk02 << 5);
    gCurTask->unk3C = gCurTask->unk18;
    TaskYieldTrampoline(gUnk_08731CE6[gCurTask->unk18] + 67);
    TaskExitTrampoline();
}

void HudShowScore(void)
{
    gHudShowsClock = 0;
    gFrameEndCallback = 0;
}

void HudShowClock(void)
{
    gHudShowsClock = 1;
    gFrameEndCallback = sub_0800aad0;
    gFrameEndCallback();
}

void HudReset(void)
{
    s32 i;

    gHudMode = 0;
    gUnk_0200617C = 0;
    for (i = 0; i < 2; i++) {
        gHudHpBarValues[i] = 0;
        HudResetHpBar(i);
    }
    gHudHpBarMaxHp = gUnk_02007D30 = 0;
    gUnk_020055F0[0] = 0;
    gUnk_020055F0[1] = 0;
}

void HudInit(s32 i)
{
    s32 j;

    HudClearWholeTilemap();
    if (gUnk_03002444 != 0)
        sub_0800ab3c();
    if (gPlayerLives[i] != 0 || gPlayerHealth[i] != 0) {
        gHudMode = 1;
        gUnk_0200617C = 1;
        HudLoadGfx();
        HudDrawPlayerIcon(i);
        HudDrawLives(gPlayerLives[i]);
        HudDrawHealth(gPlayerHealth[i] >> 3);
        if ((gActivePlayerMask >> i) & 1) {
            HudShowAbility((s16)gPlayerAbilities[i], i);
            if ((s16)gPlayerAbilities[i] != 0) {
                gHudAbilityPanelState = 1;
                gUnk_0200801C = 16;
                HudDrawAbilityPanel(2);
            } else {
                gHudAbilityPanelState = 0;
                gUnk_0200801C = 0;
                HudDrawAbilityPanel(0);
            }
        } else if (gPlayerCameraMode[i] != 1) {
            HudShowAbility(26, i);
            gHudAbilityPanelState = 1;
            gUnk_0200801C = 16;
            HudDrawAbilityPanel(2);
        } else {
            gHudAbilityPanelState = 0;
            gUnk_0200801C = 0;
            HudDrawAbilityPanel(0);
        }
        if (gHudShowsClock == 0)
            HudDrawScore(gPlayerScores[i]);
        else
            HudDrawClock(gHudClock);
        gHudShowsHpBar = 0;
        for (j = 0; j < 2; j++) {
            gHudHpBarValues[j] = 0;
            HudResetHpBar(j);
        }
        gHudHpBarMaxHp = gUnk_02007D30 = 0;
        gUnk_020055F0[0] = 0;
        gUnk_020055F0[1] = 0;
    } else {
        gHudMode = 2;
        gUnk_0200617C = 0;
    }
    HudFlushTilemap();
}

void HudRedraw(s32 i)
{
    gHudMode = 1;
    gUnk_0200617C = 1;
    HudClearTilemap();
    HudLoadGfx();
    if (gUnk_03002444 != 0)
        sub_0800ab3c();
    HudDrawPlayerIcon(i);
    HudDrawLives(gPlayerLives[i]);
    HudDrawHealth(gPlayerHealth[i] >> 3);
    if ((gActivePlayerMask >> i) & 1) {
        HudShowAbility((s16)gPlayerAbilities[i], i);
        if ((s16)gPlayerAbilities[i] != 0) {
            gHudAbilityPanelState = 1;
            gUnk_0200801C = 16;
            HudDrawAbilityPanel(2);
        } else {
            gHudAbilityPanelState = 0;
            gUnk_0200801C = 0;
            HudDrawAbilityPanel(0);
        }
    } else {
        HudShowAbilityAnimated(26, i);
    }
    if (gHudShowsHpBar == 0) {
        if (gHudShowsClock == 0)
            HudDrawScore(gPlayerScores[i]);
        else
            HudDrawClock(gHudClock);
    } else {
        HudShowHpBar();
        HudDrawHpBar(gHudHpBars[gHudHpBarIndex].unk4);
    }
    if (gUnk_020055F0[0] != 0) {
        if (gUnk_020055F0[0] == 2)
            sub_0800b230(gUnk_02005E00.unk04[i] >> 4, gUnk_020055F0[0]);
        else
            sub_0800b230(i, gUnk_020055F0[0]);
    }
    HudFlushTilemap();
}

void sub_08009e14(void)
{
    gUnk_0200617C = 0;
}

void sub_08009e20(void)
{
    gUnk_0200617C = 1;
}

void sub_08009e2c(void)
{
    gHudMode = 2;
    gUnk_0200617C = 0;
    HudClearTilemap();
    if (gUnk_03002444 != 0)
        sub_0800ab3c();
}

s32 AddPlayerLives(s32 a, u32 b)
{
    if (b < 4) {
        gPlayerLives[b] = gPlayerLives[b] + a;
        if (gPlayerLives[b] >= 100)
            gPlayerLives[b] = 99;
        else if (gPlayerLives[b] < 0)
            gPlayerLives[b] = 0;
        if (b == gLocalPlayer && gHudMode == 1)
            HudDrawLives(gPlayerLives[b]);
    }
}

s32 AddPlayerLivesNoHud(s32 a, u32 b)
{
    if (b < 4) {
        gPlayerLives[b] = gPlayerLives[b] + a;
        if (gPlayerLives[b] >= 100)
            gPlayerLives[b] = 99;
        else if (gPlayerLives[b] < 0)
            gPlayerLives[b] = 0;
    }
}

s32 AddPlayerHealth(s32 a, u32 b)
{
    s32 old, delta;

    if (b < 4) {
        old = gPlayerHealth[b];
        if (a > 0) {
            if (old + a > gMaxHealth) {
                delta = gMaxHealth - old;
                gPlayerHealth[b] = gMaxHealth;
            } else {
                delta = a;
                gPlayerHealth[b] += delta;
            }
        } else {
            if (old + a < 0) {
                delta = -old;
                gPlayerHealth[b] = 0;
            } else {
                delta = a;
                gPlayerHealth[b] += delta;
            }
            if (gPlayerHealth[b] == 0)
                gUnk_03001F34 = 1;
            else if (gPlayerHealth[b] == 8 && gLocalPlayer == b)
                PlaySfx(262);
        }
        gTasks[b].unk78 = gPlayerHealth[b];
        if (b == gLocalPlayer && gHudMode == 1)
            sub_0800acbc(old >> 3, delta >> 3);
        return gTasks[b].unk78;
    }
}

s32 SetPlayerAbilityNoHud(s32 a, s32 b, u32 c)
{
    if (c < 4) {
        struct PlayerState *p;
        gPlayerAbilities[c] = a;
        gUnk_0200AF18[c] = b;
        p = &gPlayerStates[c];
        p->unk0D = a;
        p->unk0E = b;
        return p->unk0D;
    }
}

s32 SetPlayerAbility(s32 a, s32 b, u32 c)
{
    if (c < 4) {
        struct PlayerState *p;
        gPlayerAbilities[c] = a;
        gUnk_0200AF18[c] = b;
        p = &gPlayerStates[c];
        p->unk0D = a;
        p->unk0E = b;
        HudShowAbility(a, c);
        return p->unk0D;
    }
}

void AddPlayerScore(s32 a, u32 b)
{
    if (b < 4) {
        if (gPlayerScores[b] < 99999999) {
            gPlayerScores[b] = gPlayerScores[b] + a;
            if (gPlayerScores[b] > 99999999)
                gPlayerScores[b] = 99999999;
        }
        if (gLocalPlayer == b && gHudShowsHpBar == 0 && gHudShowsClock == 0)
            HudDrawScore(gPlayerScores[b]);
    }
}

void AddPlayerScoreNoHud(s32 a, u32 b)
{
    if (b < 4) {
        if (gPlayerScores[b] < 99999999) {
            gPlayerScores[b] = gPlayerScores[b] + a;
            if (gPlayerScores[b] > 99999999)
                gPlayerScores[b] = 99999999;
        }
    }
}

void HudShowAbilityAnimated(s32 a, s32 b)
{
    if (b == gLocalPlayer && gHudMode == 1) {
        if (a == 0) {
            HudClearAbilityPicture();
            gHudAbilityPanelState = a;
            gUnk_0200801C = a;
            HudDrawAbilityPanel(0);
        } else {
            HudLoadAbilityPicture(a);
            sub_0800a19c(b);
        }
    }
}
