#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "room.h"
#include "player.h"
#include "actor.h"
#include "save.h"
#include "ending.h"

/* mode_075b8.c (0x080075B8-0x08007B67, issue #96).
 *
 * The per-frame bodies AgbMain pumps while game states 5 (HubMain),
 * 9 (BigSwitchViewMain) and 8/17/18/19 (StageMain) hold.  Each runs a setup
 * helper from mode_0b44c.c, fades in, then loops until the stage-request
 * byte gStageRequest asks for something: 1-4 switch the game state, 5 opens
 * the pause screen (PauseScreen), 6 is a lost life (state 22 when nobody
 * has lives left), 7/8/12 go to states 11/10/17 and 9-14 enter the six
 * extra modes.  CheckPauseButton raises request 5 when a present, living
 * player presses START and records that player in gPausingPlayer. */

void CheckPauseButton(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++) {
        if (((gActivePlayerMask >> i) & 1) && gPauseDisabled == 0
            && (gPlayerPressedKeys[i] & 8)) {
            gStageRequest = STAGE_REQUEST_PAUSE;
            gPausingPlayer = i;
            return;
        }
    }
}

void HubMain(void)
{
    s32 done = 0;
    s32 player;

    gPausingPlayer = 0;
    LoadBgLayout(3);
    HubInit();
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
        case STAGE_REQUEST_NONE:
            break;
        case STAGE_REQUEST_STAGE_START:
            gGameState = GAME_STATE_STAGE_START;
            done = 1;
            break;
        case STAGE_REQUEST_CHANGE_ROOM:
            gGameState = GAME_STATE_STAGE;
            done = 1;
            break;
        case STAGE_REQUEST_HUB:
            gGameState = GAME_STATE_HUB;
            done = 1;
            break;
        case STAGE_REQUEST_BIG_SWITCH_VIEW:
            break;
        case STAGE_REQUEST_PAUSE:
            PauseScreen();
            gStageRequest = STAGE_REQUEST_NONE;
            break;
        case STAGE_REQUEST_LOST_LIFE:
            if (gPlayerLives[gLocalPlayer] != 0) {
                gPlayerHealth[gLocalPlayer] = gMaxHealth;
                gPlayerAbilities[gLocalPlayer] = ABILITY_NORMAL;
                gPlayerAbilityUses[gLocalPlayer] = 0xFFFF;
            } else {
                gGameState = GAME_STATE_BOOT_LOGO;
            }
            done = 1;
            break;
        case STAGE_REQUEST_ENDING:
        case STAGE_REQUEST_GOAL_GAME:
            break;
        case STAGE_REQUEST_QUICK_DRAW:
        case STAGE_REQUEST_BOMB_RALLY:
        case STAGE_REQUEST_AIR_GRIND:
        case STAGE_REQUEST_WARP_STAR_STATION:
        case STAGE_REQUEST_MUSEUM:
        case STAGE_REQUEST_ARENA:
            gUnk_03001F2C = gStageRequest + 5;
            gUnk_02006090 = gStageRequest - 9;
            if (!((gExtraModeTitleSeen >> gUnk_02006090) & 1)) {
                gUnk_02007FCC = gUnk_03001F2C - 14;
                gGameState = GAME_STATE_EXTRA_MODE_TITLE;
                if (gUnk_02006090 > 2)
                    gExtraModeTitleSeen |= 1 << gUnk_02006090;
            } else {
                gGameState = gUnk_03001F2C;
            }
            gPrevGameState = GAME_STATE_HUB;
            done = 1;
            break;
        }
    } while (done == 0);
    LinkStopKeyExchange();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    for (player = 0; player < gPlayerCount; player++) {
        if (gPlayerStates[player].sfxPlayer != -1) {
            StopSfxOnPlayer(gPlayerStates[player].sfxPlayer, gPlayerStates[player].sfxId);
            gPlayerStates[player].sfxPlayer = -1;
        }
    }
    StopRoom();
}

void BigSwitchViewMain(void)
{
    s32 done = 0;

    LoadBgLayout(3);
    BigSwitchViewInit();
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
        case STAGE_REQUEST_NONE:
        case STAGE_REQUEST_HUB:
        case STAGE_REQUEST_STAGE_START:
            break;
        case STAGE_REQUEST_CHANGE_ROOM:
            gGameState = GAME_STATE_STAGE;
            done = 1;
            break;
        case STAGE_REQUEST_BIG_SWITCH_VIEW:
        case STAGE_REQUEST_PAUSE:
        case STAGE_REQUEST_LOST_LIFE:
        case STAGE_REQUEST_ENDING:
        case STAGE_REQUEST_GOAL_GAME:
        case STAGE_REQUEST_QUICK_DRAW:
        case STAGE_REQUEST_BOMB_RALLY:
        case STAGE_REQUEST_AIR_GRIND:
        case STAGE_REQUEST_WARP_STAR_STATION:
        case STAGE_REQUEST_MUSEUM:
        case STAGE_REQUEST_ARENA:
            break;
        }
    } while (done == 0);
    LinkStopKeyExchange();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    FreeRoomAndDoorObjects();
    if (CheckNewMilestones() != 0 && gMetaKnightmareMode == 0)
        ShowMilestonePicture();
}

void StageMain(void)
{
    s32 done = 0;
    s32 i;
    s32 n;

    gPausingPlayer = 0;
    gInputRecorderMode = 0;
    InputRecorderStart();
    LoadBgLayout(3);
    StageInit();
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
        case STAGE_REQUEST_NONE:
            break;
        case STAGE_REQUEST_STAGE_START:
            gGameState = GAME_STATE_STAGE_START;
            done = 1;
            break;
        case STAGE_REQUEST_HUB:
            gGameState = GAME_STATE_HUB;
            done = 1;
            break;
        case STAGE_REQUEST_CHANGE_ROOM:
            done = 1;
            break;
        case STAGE_REQUEST_BIG_SWITCH_VIEW:
            gGameState = GAME_STATE_BIG_SWITCH_VIEW;
            done = 1;
            break;
        case STAGE_REQUEST_PAUSE:
            PauseScreen();
            if (gGameState == GAME_STATE_HUB) {
                ReturnToHubStageDoor();
                done = 1;
            } else {
                gStageRequest = STAGE_REQUEST_NONE;
            }
            break;
        case STAGE_REQUEST_LOST_LIFE:
            if (gPlayerCount == 1) {
                if (gPlayerLives[gLocalPlayer] != 0) {
                    ReturnToRestartPoint();
                } else {
                gameover:
                    gGameState = GAME_STATE_GAME_OVER;
                }
            } else {
                n = 0;
                for (i = 0; i < gPlayerCount; i++) {
                    if (gPlayerLives[i] != 0)
                        n++;
                }
                if (n == 0)
                    goto gameover;
                ReturnToRestartPoint();
            }
            done = 1;
            break;
        case STAGE_REQUEST_ENDING:
            gGameState = GAME_STATE_ENDING;
            done = 1;
            break;
        case STAGE_REQUEST_GOAL_GAME:
            gGameState = GAME_STATE_GOAL_GAME;
            done = 1;
            break;
        case STAGE_REQUEST_QUICK_DRAW:
        case STAGE_REQUEST_BOMB_RALLY:
        case STAGE_REQUEST_AIR_GRIND:
            break;
        case STAGE_REQUEST_WARP_STAR_STATION:
            gGameState = GAME_STATE_WARP_STAR_STATION;
            done = 1;
            break;
        case STAGE_REQUEST_MUSEUM:
        case STAGE_REQUEST_ARENA:
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
    StopRoomAndApplyExitFlags();
    if (gMetaKnightmareMode == 0 && gGameState != GAME_STATE_BIG_SWITCH_VIEW
        && (gMilestoneFlags & (4 << gExtraMode))
        && !(gMilestoneFlags & (64 << gExtraMode))) {
        gMilestoneFlags |= 64 << gExtraMode;
        SaveProgress(gCurSaveSlot);
        StopAllSfx();
        ResetTasksAndOam();
        ShowMilestonePicture();
    }
}
