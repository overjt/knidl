#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* mode_075b8.c (0x080075B8-0x08007B67, issue #96).
 *
 * The per-frame bodies AgbMain pumps while game states 5 (sub_08007624),
 * 9 (sub_0800783c) and 8/17/18/19 (sub_0800791c) hold.  Each runs a setup
 * helper from mode_0b44c.c, fades in, then loops until the stage-request
 * byte gUnk_03002438 asks for something: 1-4 switch the game state, 5 opens
 * the pause screen (sub_08008664), 6 is a lost life (state 22 when nobody
 * has lives left), 7/8/12 go to states 11/10/17 and 9-14 enter the six
 * extra modes.  sub_080075b8 raises request 5 when a present, living
 * player presses START and records that player in gUnk_02004B60. */

extern u16 gUnk_02004B50[];
extern u16 gUnk_02004B60;
extern s16 gUnk_02005580;
extern s16 gUnk_02005588[];
extern u8 gUnk_020055CC;
extern u8 gUnk_02006090;
extern s16 gPlayerLives[];
extern u8 gUnk_02007FCC;
extern u16 gUnk_0200AF18[];
extern s16 gInputRecorderMode;
extern vu16 gUnk_03000048;
extern vu16 gFadeSteps;
extern vu16 gPlayerPressedKeys[];
extern vu16 gDispCnt;
extern s32 gUnk_03001F2C;
extern u8 gUnk_03001F30;
extern u8 gUnk_03001F34;
extern u16 gUnk_03002150;
extern struct PlayerState gUnk_03002170[];
extern u8 gUnk_03002340;
extern u16 gLocalPlayer;
extern u16 gUnk_03002364;
extern u16 gPlayerCount;
extern u16 gGameState;
extern s32 gCurSaveSlot;
extern s8 gUnk_03002438;
extern u8 gExtraMode;

void BeginFastFadeInFromWhite(void);
void BeginFastFadeOutToWhite(void);
void ResetTasksAndOam(void);
void sub_080022fc(void);
void sub_08002338(void);
void sub_08002358(void);
void sub_08002378(void);
void RunLinkFrame(void);
void RunLinkFramesUntilFadeDone(void);
void StopSfxOnPlayer(s32 player, s32 songId);
void StopAllSfx(void);
void sub_08008664(void);
void sub_08008c4c(s32 a0);
void sub_0800b648(void);
void sub_0800b788(void);
void sub_0800b87c(void);
void sub_0801a7b4(void);
void sub_08027128(void);
void sub_08027178(void);
void sub_08027198(void);
void sub_080272dc(void);
void sub_080273a0(void);
void sub_08040788(void);
void InputRecorderStart(void);
void InputRecorderUpdate(void);
void sub_080b7b7c(s32 a);
s32 sub_080b8290(void);
void sub_080c6750(void);

void sub_080075b8(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++) {
        if (((gUnk_03002340 >> i) & 1) && gUnk_03001F34 == 0
            && (gPlayerPressedKeys[i] & 8)) {
            gUnk_03002438 = 5;
            gUnk_02004B60 = i;
            return;
        }
    }
}

void sub_08007624(void)
{
    s32 done = 0;
    s32 i;

    gUnk_02004B60 = 0;
    sub_08008c4c(3);
    sub_0800b788();
    sub_08002358();
    sub_08002378();
    sub_080022fc();
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1F00;
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    do {
        sub_0801a7b4();
        RunLinkFrame();
        sub_08040788();
        sub_080075b8();
        switch (gUnk_03002438) {
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
            sub_08008664();
            gUnk_03002438 = 0;
            break;
        case 6:
            if (gPlayerLives[gLocalPlayer] != 0) {
                gUnk_02005588[gLocalPlayer] = gUnk_02005580;
                gUnk_02004B50[gLocalPlayer] = 0;
                gUnk_0200AF18[gLocalPlayer] = 0xFFFF;
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
            gUnk_03001F2C = gUnk_03002438 + 5;
            gUnk_02006090 = gUnk_03002438 - 9;
            if (!((gUnk_020055CC >> gUnk_02006090) & 1)) {
                gUnk_02007FCC = gUnk_03001F2C - 14;
                gGameState = 13;
                if (gUnk_02006090 > 2)
                    gUnk_020055CC |= 1 << gUnk_02006090;
            } else {
                gGameState = gUnk_03001F2C;
            }
            gUnk_03002150 = 5;
            done = 1;
            break;
        }
    } while (done == 0);
    sub_08002338();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    for (i = 0; i < gPlayerCount; i++) {
        if (gUnk_03002170[i].unk2C != -1) {
            StopSfxOnPlayer(gUnk_03002170[i].unk2C, gUnk_03002170[i].unk2E);
            gUnk_03002170[i].unk2C = -1;
        }
    }
    sub_08027178();
}

void sub_0800783c(void)
{
    s32 done = 0;

    sub_08008c4c(3);
    sub_0800b87c();
    sub_08002358();
    sub_08002378();
    sub_080022fc();
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1E00;
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    do {
        RunLinkFrame();
        switch (gUnk_03002438) {
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
    sub_08002338();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    sub_08027198();
    if (sub_080b8290() != 0 && gUnk_03001F30 == 0)
        sub_080c6750();
}

void sub_0800791c(void)
{
    s32 done = 0;
    s32 i;
    s32 n;

    gUnk_02004B60 = 0;
    gInputRecorderMode = 0;
    InputRecorderStart();
    sub_08008c4c(3);
    sub_0800b648();
    sub_08002358();
    sub_08002378();
    sub_080022fc();
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1D00;
    BeginFastFadeInFromWhite();
    while (gFadeSteps != 0) {
        RunLinkFrame();
        InputRecorderUpdate();
    }
    gUnk_03000048 = 0;
    do {
        sub_0801a7b4();
        RunLinkFrame();
        InputRecorderUpdate();
        sub_08040788();
        sub_080075b8();
        switch (gUnk_03002438) {
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
            sub_08008664();
            if (gGameState == 5) {
                sub_080272dc();
                done = 1;
            } else {
                gUnk_03002438 = 0;
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
    sub_08002338();
    BeginFastFadeOutToWhite();
    while (gFadeSteps != 0) {
        RunLinkFrame();
        InputRecorderUpdate();
    }
    gUnk_03000048 = 0;
    sub_08027128();
    if (gUnk_03001F30 == 0 && gGameState != 9
        && (gUnk_03002364 & (4 << gExtraMode))
        && !(gUnk_03002364 & (64 << gExtraMode))) {
        gUnk_03002364 |= 64 << gExtraMode;
        sub_080b7b7c(gCurSaveSlot);
        StopAllSfx();
        ResetTasksAndOam();
        sub_080c6750();
    }
}
