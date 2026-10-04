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

/* hud_099fc.c (0x080099FC-0x0800A12F, issue #96).
 *
 * Task type #237 (Task_IntroStoryPicture, one intro-story picture) and the
 * HUD/score interface other modules call: HUD init/redraw
 * (HudInit/HudRedraw), lives (AddPlayerLives), health
 * (AddPlayerHealth, returns the new value), score (SetPlayerAbilityNoHud/SetPlayerAbility/
 * AddPlayerScore/AddPlayerScoreNoHud, clamped to 99999999) and the clock mode
 * (HudShowScore/HudShowClock). */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 PlaySfx(s32 id);

void Task_IntroStoryPicture(void)
{
    struct GfxHeader *h;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 8;
    gCurTask->frameTable = gIntroStoryPictureFrames;
    gCurTask->tileWord = 0x800;
    gCurTask->posX = 0x300000;
    gCurTask->posY = 0x780000;
    h = gUnk_087555FC[gCurTask->introStoryPictureIndex];
    RequestCopy(2, (u32)h->palette, (u32)gObjPalette, h->paletteBankCount << 5);
    LZ77UnCompVram(h->tiles, gUnk_02020000);
    RequestCopy(3, (u32)gUnk_02020000, (u32)gObjVram, h->tileCount << 5);
    gCurTask->frame = gCurTask->introStoryPictureIndex;
    TaskYieldTrampoline(gUnk_08731CE6[gCurTask->introStoryPictureIndex] + 67);
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
    gFrameEndCallback = HudUpdateClock;
    gFrameEndCallback();
}

void HudReset(void)
{
    s32 i;

    gHudMode = 0;
    gHudAbilityPanelActive = 0;
    for (i = 0; i < 2; i++) {
        gHudHpBarValues[i] = 0;
        HudResetHpBar(i);
    }
    gHudHpBarMaxHp = gHudHpBarLength = 0;
    gUnk_020055F0[0] = 0;
    gUnk_020055F0[1] = 0;
}

void HudInit(s32 i)
{
    s32 j;

    HudClearWholeTilemap();
    if (gInHub != 0)
        sub_0800ab3c();
    if (gPlayerLives[i] != 0 || gPlayerHealth[i] != 0) {
        gHudMode = 1;
        gHudAbilityPanelActive = 1;
        HudLoadGfx();
        HudDrawPlayerIcon(i);
        HudDrawLives(gPlayerLives[i]);
        HudDrawHealth(gPlayerHealth[i] >> 3);
        if ((gActivePlayerMask >> i) & 1) {
            HudShowAbility((s16)gPlayerAbilities[i], i);
            if ((s16)gPlayerAbilities[i] != ABILITY_NORMAL) {
                gHudAbilityPanelState = 1;
                gUnk_0200801C = 16;
                HudDrawAbilityPanel(2);
            } else {
                gHudAbilityPanelState = 0;
                gUnk_0200801C = 0;
                HudDrawAbilityPanel(0);
            }
        } else if (gPlayerCameraMode[i] != 1) {
            HudShowAbility(ABILITY_PICTURE_WAIT, i);
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
        gHudHpBarMaxHp = gHudHpBarLength = 0;
        gUnk_020055F0[0] = 0;
        gUnk_020055F0[1] = 0;
    } else {
        gHudMode = 2;
        gHudAbilityPanelActive = 0;
    }
    HudFlushTilemap();
}

void HudRedraw(s32 i)
{
    gHudMode = 1;
    gHudAbilityPanelActive = 1;
    HudClearTilemap();
    HudLoadGfx();
    if (gInHub != 0)
        sub_0800ab3c();
    HudDrawPlayerIcon(i);
    HudDrawLives(gPlayerLives[i]);
    HudDrawHealth(gPlayerHealth[i] >> 3);
    if ((gActivePlayerMask >> i) & 1) {
        HudShowAbility((s16)gPlayerAbilities[i], i);
        if ((s16)gPlayerAbilities[i] != ABILITY_NORMAL) {
            gHudAbilityPanelState = 1;
            gUnk_0200801C = 16;
            HudDrawAbilityPanel(2);
        } else {
            gHudAbilityPanelState = 0;
            gUnk_0200801C = 0;
            HudDrawAbilityPanel(0);
        }
    } else {
        HudShowAbilityAnimated(ABILITY_PICTURE_WAIT, i);
    }
    if (gHudShowsHpBar == 0) {
        if (gHudShowsClock == 0)
            HudDrawScore(gPlayerScores[i]);
        else
            HudDrawClock(gHudClock);
    } else {
        HudShowHpBar();
        HudDrawHpBar(gHudHpBars[gHudHpBarIndex].shownValue);
    }
    if (gUnk_020055F0[0] != 0) {
        if (gUnk_020055F0[0] == 2)
            sub_0800b230(gLifeRequests.requests[i] >> 4, gUnk_020055F0[0]);
        else
            sub_0800b230(i, gUnk_020055F0[0]);
    }
    HudFlushTilemap();
}

void HudDeactivateAbilityPanel(void)
{
    gHudAbilityPanelActive = 0;
}

void HudActivateAbilityPanel(void)
{
    gHudAbilityPanelActive = 1;
}

void sub_08009e2c(void)
{
    gHudMode = 2;
    gHudAbilityPanelActive = 0;
    HudClearTilemap();
    if (gInHub != 0)
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
                gPauseDisabled = 1;
            else if (gPlayerHealth[b] == 8 && gLocalPlayer == b)
                PlaySfx(262);
        }
        gTasks[b].health = gPlayerHealth[b];
        if (b == gLocalPlayer && gHudMode == 1)
            HudDrawHealthChange(old >> 3, delta >> 3);
        return gTasks[b].health;
    }
}

s32 SetPlayerAbilityNoHud(s32 a, s32 b, u32 c)
{
    if (c < 4) {
        struct PlayerState *p;
        gPlayerAbilities[c] = a;
        gPlayerAbilityUses[c] = b;
        p = &gPlayerStates[c];
        p->ability = a;
        p->abilityUses = b;
        return p->ability;
    }
}

s32 SetPlayerAbility(s32 a, s32 b, u32 c)
{
    if (c < 4) {
        struct PlayerState *p;
        gPlayerAbilities[c] = a;
        gPlayerAbilityUses[c] = b;
        p = &gPlayerStates[c];
        p->ability = a;
        p->abilityUses = b;
        HudShowAbility(a, c);
        return p->ability;
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
            HudOpenAbilityPanel(b);
        }
    }
}
