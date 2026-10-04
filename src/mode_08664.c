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

/* mode_08664.c (0x08008664-0x08008B8B, issue #96).
 *
 * The pause screen PauseScreen: it fades out, loads the level's pause
 * picture (PauseScreenLoadGraphics), waits for A/START or B and, when the stage
 * allows it, toggles a Continue/Exit choice drawn by PauseScreenLoadChoicePalette.  Also
 * the per-frame body of AgbMain state 20, BossEnduranceMain. */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void PlaySfx(s32 id);

void PauseScreen(void)
{
    s32 i;
    s32 id;
    s32 flag;
    s32 sel;
    s32 pressed;
    u16 mode;

    id = gPlayerStates[gLocalPlayer].ability;
    PlaySfx(232);
    for (i = 0; i < 64; i++)
        TaskSetSkipMask(15, i);
    PauseRoom();
    BeginFastFadeOutToWhite();
    RunLinkFrames(8);
    FadeOutSfx(32);
    RunLinkFramesUntilFadeDone();
    for (i = 0; i < 64; i++)
        TaskSetSkipMask(31, i);
    SuspendHBlankScroll();
    PauseSaveBgPalette();
    mode = gGameState;
    flag = 0;
    if (mode == GAME_STATE_STAGE && gStageClearStatus[gCurLevel][gUnk_03001F20] != 0)
        flag = gUnk_02007D64 != 5;
    LoadBgLayout(5);
    if (gMetaKnightmareMode == 1) {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0xC00;
        id = 28;
    } else if (gPlayerLives[gLocalPlayer] == 0 && gPlayerHealth[gLocalPlayer] == 0) {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x800;
        id = 27;
    } else {
        if (!((gActivePlayerMask >> gLocalPlayer) & 1))
            id = 26;
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0xE00;
    }
    PauseScreenLoadGraphics(id, flag);
    gBg2ScrollX = gBg2ScrollY = 0;
    gBg3ScrollX = gBg3ScrollY = 0;
    sel = 0;
    PauseScreenLoadChoicePalette(0);
    LinkRequestSync();
    LinkSyncRandom();
    LinkStartKeyExchange();
    ResetFadeAndBlend();
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    do {
        RunLinkFrame();
        pressed = 0;
        if (gUnk_0300244C == 0) {
            for (i = 0; i < gPlayerCount; i++) {
                if (gPlayerLives[i] != 0 || gPlayerHealth[i] != 0) {
                    if (gPlayerPressedKeys[i] & 9) {
                        if (sel != 0) {
                            gGameState = GAME_STATE_HUB;
                            StopAllSfx();
                        }
                        PlaySfx(0x11F);
                        pressed++;
                    } else if (gPlayerPressedKeys[i] & 2) {
                        PlaySfx(SE_CANCEL);
                        pressed++;
                    }
                    if (flag != 0 && (gPlayerPressedKeys[i] & 0xC0)) {
                        PlaySfx(286);
                        sel ^= 1;
                        PauseScreenLoadChoicePalette(sel);
                    }
                }
            }
        } else {
            if (flag != 0 && (gPlayerPressedKeys[gPausingPlayer] & 0xC0)) {
                PlaySfx(286);
                sel ^= 1;
                PauseScreenLoadChoicePalette(sel);
            }
            if (gPlayerPressedKeys[gPausingPlayer] & 9) {
                if (sel != 0) {
                    gGameState = GAME_STATE_HUB;
                    StopAllSfx();
                }
                PlaySfx(0x11F);
                pressed++;
            } else if (gPlayerPressedKeys[gPausingPlayer] & 2) {
                PlaySfx(SE_CANCEL);
                pressed++;
            }
        }
    } while (pressed == 0);
    if (gInHub != 0 || gGameState != GAME_STATE_HUB) {
        LinkStopKeyExchange();
        BeginFastFadeOutToWhite();
        RunLinkFramesUntilFadeDone();
        LoadBgLayout(3);
        PauseRestoreRoomGraphics();
        for (i = 0; i < 64; i++)
            TaskSetSkipMask(15, i);
        RestoreRoomHBlankScroll();
        LinkRequestSync();
        LinkSyncRandom();
        LinkStartKeyExchange();
        FadeInSfx(16);
        BeginFastFadeInFromWhite();
        RunLinkFramesUntilFadeDone();
        for (i = 0; i < 64; i++)
            TaskSetSkipMask(0, i);
        ResumeRoom();
    }
}

void PauseScreenLoadChoicePalette(s32 n)
{
    RequestCopy(2, (u32)gPauseChoicePalettes[n], (u32)gBgPaletteBank9, 22);
}

void BossEnduranceMain(void)
{
    s32 done = 0;
    s32 i;

    gPausingPlayer = 0;
    LoadBgLayout(3);
    gInputRecorderMode = 0;
    InputRecorderStart();
    StageInit();
    for (i = 0; i < gPlayerCount; i++)
        InitPlayerState(i);
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
        case STAGE_REQUEST_HUB:
            done = 1;
            break;
        case STAGE_REQUEST_STAGE_START:
            break;
        case STAGE_REQUEST_CHANGE_ROOM:
            done = 1;
            break;
        case STAGE_REQUEST_BIG_SWITCH_VIEW:
            break;
        case STAGE_REQUEST_PAUSE:
            PauseScreen();
            gStageRequest = STAGE_REQUEST_NONE;
            break;
        case STAGE_REQUEST_LOST_LIFE:
            gGameState = GAME_STATE_GAME_OVER;
            done = 1;
            break;
        case STAGE_REQUEST_ENDING:
            gGameState = GAME_STATE_ENDING;
            done = 1;
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
    for (i = 0; i < gPlayerCount; i++) {
        if (gPlayerStates[i].sfxPlayer != -1) {
            StopSfxOnPlayer(gPlayerStates[i].sfxPlayer, gPlayerStates[i].sfxId);
            gPlayerStates[i].sfxPlayer = -1;
        }
    }
    sub_08027128();
}
