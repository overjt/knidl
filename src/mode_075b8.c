#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "mode.h"
#include "room.h"
#include "actor.h"
#include "save.h"

/* mode_075b8.c (0x080075B8-0x08007B67, issue #96).
 *
 * The per-frame bodies AgbMain pumps while game states 5 (sub_08007624),
 * 9 (sub_0800783c) and 8/17/18/19 (sub_0800791c) hold.  Each runs a setup
 * helper from mode_0b44c.c, fades in, then loops until the stage-request
 * byte gStageRequest asks for something: 1-4 switch the game state, 5 opens
 * the pause screen (PauseScreen), 6 is a lost life (state 22 when nobody
 * has lives left), 7/8/12 go to states 11/10/17 and 9-14 enter the six
 * extra modes.  CheckPauseButton raises request 5 when a present, living
 * player presses START and records that player in gPausingPlayer. */

void BeginFastFadeInFromWhite(void);
void BeginFastFadeOutToWhite(void);
void ResetTasksAndOam(void);
void LinkStartKeyExchange(void);
void LinkStopKeyExchange(void);
void LinkRequestSync(void);
void LinkSyncRandom(void);
void RunLinkFrame(void);
void RunLinkFramesUntilFadeDone(void);
void StopSfxOnPlayer(s32 player, s32 songId);
void StopAllSfx(void);
void PauseScreen(void);
void LoadBgLayout(s32 a0);
void sub_0800b648(void);
void sub_0800b788(void);
void sub_0800b87c(void);
void ClearColliderLists(void);
void sub_08027128(void);
void sub_08027178(void);
void sub_08027198(void);
void sub_080272dc(void);
void sub_080273a0(void);
void LatchPlayerKeys(void);
void InputRecorderStart(void);
void InputRecorderUpdate(void);
void SaveProgress(s32 a);
s32 CheckNewMilestones(void);
void ShowMilestonePicture(void);

void CheckPauseButton(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++) {
        if (((gActivePlayerMask >> i) & 1) && gUnk_03001F34 == 0
            && (gPlayerPressedKeys[i] & 8)) {
            gStageRequest = 5;
            gPausingPlayer = i;
            return;
        }
    }
}

void sub_08007624(void)
{
    s32 done = 0;
    s32 i;

    gPausingPlayer = 0;
    LoadBgLayout(3);
    sub_0800b788();
    LinkRequestSync();
    LinkSyncRandom();
    LinkStartKeyExchange();
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1F00;
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    do {
        ClearColliderLists();
        RunLinkFrame();
        LatchPlayerKeys();
        CheckPauseButton();
        switch (gStageRequest) {
        case 0:
            break;
        case 2:
            gGameState = 6;
            done = 1;
            break;
        case 3:
            gGameState = 8;
            done = 1;
            break;
        case 1:
            gGameState = 5;
            done = 1;
            break;
        case 4:
            break;
        case 5:
            PauseScreen();
            gStageRequest = 0;
            break;
        case 6:
            if (gPlayerLives[gLocalPlayer] != 0) {
                gPlayerHealth[gLocalPlayer] = gMaxHealth;
                gPlayerAbilities[gLocalPlayer] = 0;
                gPlayerAbilityUses[gLocalPlayer] = 0xFFFF;
            } else {
                gGameState = 1;
            }
            done = 1;
            break;
        case 7:
        case 8:
            break;
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            gUnk_03001F2C = gStageRequest + 5;
            gUnk_02006090 = gStageRequest - 9;
            if (!((gUnk_020055CC >> gUnk_02006090) & 1)) {
                gUnk_02007FCC = gUnk_03001F2C - 14;
                gGameState = 13;
                if (gUnk_02006090 > 2)
                    gUnk_020055CC |= 1 << gUnk_02006090;
            } else {
                gGameState = gUnk_03001F2C;
            }
            gPrevGameState = 5;
            done = 1;
            break;
        }
    } while (done == 0);
    LinkStopKeyExchange();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    for (i = 0; i < gPlayerCount; i++) {
        if (gPlayerStates[i].sfxPlayer != -1) {
            StopSfxOnPlayer(gPlayerStates[i].sfxPlayer, gPlayerStates[i].sfxId);
            gPlayerStates[i].sfxPlayer = -1;
        }
    }
    sub_08027178();
}

void sub_0800783c(void)
{
    s32 done = 0;

    LoadBgLayout(3);
    sub_0800b87c();
    LinkRequestSync();
    LinkSyncRandom();
    LinkStartKeyExchange();
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1E00;
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    do {
        RunLinkFrame();
        switch (gStageRequest) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            gGameState = 8;
            done = 1;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            break;
        }
    } while (done == 0);
    LinkStopKeyExchange();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    sub_08027198();
    if (CheckNewMilestones() != 0 && gUnk_03001F30 == 0)
        ShowMilestonePicture();
}

void sub_0800791c(void)
{
    s32 done = 0;
    s32 i;
    s32 n;

    gPausingPlayer = 0;
    gInputRecorderMode = 0;
    InputRecorderStart();
    LoadBgLayout(3);
    sub_0800b648();
    LinkRequestSync();
    LinkSyncRandom();
    LinkStartKeyExchange();
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1D00;
    BeginFastFadeInFromWhite();
    while (gFadeSteps != 0) {
        RunLinkFrame();
        InputRecorderUpdate();
    }
    gFadeBlankAtWhite = 0;
    do {
        ClearColliderLists();
        RunLinkFrame();
        InputRecorderUpdate();
        LatchPlayerKeys();
        CheckPauseButton();
        switch (gStageRequest) {
        case 0:
            break;
        case 2:
            gGameState = 6;
            done = 1;
            break;
        case 1:
            gGameState = 5;
            done = 1;
            break;
        case 3:
            done = 1;
            break;
        case 4:
            gGameState = 9;
            done = 1;
            break;
        case 5:
            PauseScreen();
            if (gGameState == 5) {
                sub_080272dc();
                done = 1;
            } else {
                gStageRequest = 0;
            }
            break;
        case 6:
            if (gPlayerCount == 1) {
                if (gPlayerLives[gLocalPlayer] != 0) {
                    sub_080273a0();
                } else {
                gameover:
                    gGameState = 22;
                }
            } else {
                n = 0;
                for (i = 0; i < gPlayerCount; i++) {
                    if (gPlayerLives[i] != 0)
                        n++;
                }
                if (n == 0)
                    goto gameover;
                sub_080273a0();
            }
            done = 1;
            break;
        case 7:
            gGameState = 11;
            done = 1;
            break;
        case 8:
            gGameState = 10;
            done = 1;
            break;
        case 9:
        case 10:
        case 11:
            break;
        case 12:
            gGameState = 17;
            done = 1;
            break;
        case 13:
        case 14:
            break;
        }
    } while (done == 0);
    LinkStopKeyExchange();
    BeginFastFadeOutToWhite();
    while (gFadeSteps != 0) {
        RunLinkFrame();
        InputRecorderUpdate();
    }
    gFadeBlankAtWhite = 0;
    sub_08027128();
    if (gUnk_03001F30 == 0 && gGameState != 9
        && (gMilestoneFlags & (4 << gExtraMode))
        && !(gMilestoneFlags & (64 << gExtraMode))) {
        gMilestoneFlags |= 64 << gExtraMode;
        SaveProgress(gCurSaveSlot);
        StopAllSfx();
        ResetTasksAndOam();
        ShowMilestonePicture();
    }
}
