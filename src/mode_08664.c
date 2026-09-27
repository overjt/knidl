#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* mode_08664.c (0x08008664-0x08008B8B, issue #96).
 *
 * The pause screen PauseScreen: it fades out, loads the level's pause
 * picture (sub_08008fc4), waits for A/START or B and, when the stage
 * allows it, toggles a Continue/Exit choice drawn by sub_080089e0.  Also
 * the per-frame body of AgbMain state 20, sub_08008a00. */

extern u16 gPausingPlayer;
extern s16 gPlayerHealth[];
extern s16 gPlayerLives[];
extern s8 gUnk_02007D64;
extern s16 gInputRecorderMode;
extern vu16 gFadeBlankAtWhite;
extern vs32 gBg3ScrollX;
extern vs32 gBg2ScrollX;
extern vs32 gBg3ScrollY;
extern u8 gUnk_03001390[];
extern vu16 gFadeSteps;
extern vs32 gBg2ScrollY;
extern vu16 gPlayerPressedKeys[];
extern vu16 gDispCnt;
extern s8 gUnk_03001F20;
extern u8 gUnk_03001F30;
extern struct PlayerState gPlayerStates[];
extern u8 gActivePlayerMask;
extern u16 gLocalPlayer;
extern u16 gPlayerCount;
extern s8 gUnk_030023B8;
extern u16 gGameState;
extern u8 gUnk_03002400[8][7];
extern s8 gStageRequest;
extern s8 gUnk_03002444;
extern s16 gUnk_0300244C;
extern u16 gUnk_0857121C[][11];

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void ResetFadeAndBlend(void);
void BeginFastFadeInFromWhite(void);
void BeginFastFadeOutToWhite(void);
void LinkStartKeyExchange(void);
void LinkStopKeyExchange(void);
void LinkRequestSync(void);
void LinkSyncRandom(void);
void RunLinkFrame(void);
void RunLinkFrames(s32 count);
void RunLinkFramesUntilFadeDone(void);
void PlaySfx(s32 id);
void StopSfxOnPlayer(s32 player, s32 songId);
void StopAllSfx(void);
void FadeInSfx(u16 speed);
void FadeOutSfx(s32 speed);
void TaskSetSkipMask(u8 val, s32 idx);
void CheckPauseButton(void);
void LoadBgLayout(s32 a0);
void sub_08008fc4(s32 a0, s32 a1);
void sub_0800b648(void);
void ClearColliderLists(void);
void sub_08027128(void);
void PauseRoom(void);
void ResumeRoom(void);
void sub_08027228(void);
void sub_08027240(void);
void InitPlayerState(s32 a0);
void LatchPlayerKeys(void);
void SuspendHBlankScroll(void);
void RestoreRoomHBlankScroll(void);
void InputRecorderStart(void);
void InputRecorderUpdate(void);
void sub_080089e0(s32 n);

void PauseScreen(void)
{
    s32 i;
    s32 id;
    s32 flag;
    s32 sel;
    s32 pressed;
    u16 mode;

    id = gPlayerStates[gLocalPlayer].unk0D;
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
    sub_08027228();
    mode = gGameState;
    flag = 0;
    if (mode == 8 && gUnk_03002400[gUnk_030023B8][gUnk_03001F20] != 0)
        flag = gUnk_02007D64 != 5;
    LoadBgLayout(5);
    if (gUnk_03001F30 == 1) {
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
    sub_08008fc4(id, flag);
    gBg2ScrollX = gBg2ScrollY = 0;
    gBg3ScrollX = gBg3ScrollY = 0;
    sel = 0;
    sub_080089e0(0);
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
                            gGameState = 5;
                            StopAllSfx();
                        }
                        PlaySfx(0x11F);
                        pressed++;
                    } else if (gPlayerPressedKeys[i] & 2) {
                        PlaySfx(215);
                        pressed++;
                    }
                    if (flag != 0 && (gPlayerPressedKeys[i] & 0xC0)) {
                        PlaySfx(286);
                        sel ^= 1;
                        sub_080089e0(sel);
                    }
                }
            }
        } else {
            if (flag != 0 && (gPlayerPressedKeys[gPausingPlayer] & 0xC0)) {
                PlaySfx(286);
                sel ^= 1;
                sub_080089e0(sel);
            }
            if (gPlayerPressedKeys[gPausingPlayer] & 9) {
                if (sel != 0) {
                    gGameState = 5;
                    StopAllSfx();
                }
                PlaySfx(0x11F);
                pressed++;
            } else if (gPlayerPressedKeys[gPausingPlayer] & 2) {
                PlaySfx(215);
                pressed++;
            }
        }
    } while (pressed == 0);
    if (gUnk_03002444 != 0 || gGameState != 5) {
        LinkStopKeyExchange();
        BeginFastFadeOutToWhite();
        RunLinkFramesUntilFadeDone();
        LoadBgLayout(3);
        sub_08027240();
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

void sub_080089e0(s32 n)
{
    RequestCopy(2, (u32)gUnk_0857121C[n], (u32)gUnk_03001390, 22);
}

void sub_08008a00(void)
{
    s32 done = 0;
    s32 i;

    gPausingPlayer = 0;
    LoadBgLayout(3);
    gInputRecorderMode = 0;
    InputRecorderStart();
    sub_0800b648();
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
        case 0:
            break;
        case 1:
            done = 1;
            break;
        case 2:
            break;
        case 3:
            done = 1;
            break;
        case 4:
            break;
        case 5:
            PauseScreen();
            gStageRequest = 0;
            break;
        case 6:
            gGameState = 22;
            done = 1;
            break;
        case 7:
            gGameState = 11;
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
        if (gPlayerStates[i].unk2C != -1) {
            StopSfxOnPlayer(gPlayerStates[i].unk2C, gPlayerStates[i].unk2E);
            gPlayerStates[i].unk2C = -1;
        }
    }
    sub_08027128();
}
