#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "mode.h"
#include "hud.h"
#include "player.h"
#include "actor.h"
#include "save.h"

/* mode_0b44c.c (0x0800B44C-0x0800B91F, issue #96).
 *
 * The game-state setup helpers AgbMain and the state bodies call:
 * ResetGameSession (reset the game-state cells), ResetScoresAndMaxHealth (scores, the
 * maximum health 24 or 48, the HUD mode), ResetPlayerRecords (three lives and
 * cleared records per player), HubResetPlayers/ResetTasksAndPlayers, and the three
 * screen setups StageInit/HubInit/BigSwitchViewInit. */

/* Not from room.h or effect.h: this file's view of gSavedPlayerAbilityUses
   and gStageClearDanceBgmPlayed differs (lesson 3.517). */
extern u8 gUnk_02000020;
extern u16 gPlayerAbilities[];
extern u8 gBigSwitchPressActive;
extern s16 gMaxHealth;
extern s16 gPlayerHealth[];
extern u8 gStageClearDanceBgmPlayed;
extern u8 gRoomExitKind;
extern s16 gPlayerLives[];
extern vs16 gSavedPlayerAbilityUses[];
extern u16 gSavedPlayerAbilities[];
extern s8 gDanceId;
extern u16 gPlayerAbilityUses[];
extern u8 gMetaKnightmareMode;
extern u8 gActivePlayerMask;
extern u8 gActivePlayerCount;
extern u16 gLatchedPressedKeys[];
extern s8 gStageRequest;
extern u16 gLatchedHeldKeys[];
extern u8 gExtraMode;

void ResetLevelStateAtHub(void);
void sub_08022f98(void);
void ClearRoomBgmStarted(void);
void LoadRoom(void);
void sub_080233e0(void);
void LoadHubRoom(void);
void LoadBigSwitchViewRoom(void);

void ResetGameSession(void)
{
    gUnk_03000B24 = 1;
    ResetTasksAndOam();
    gLocalPlayer = 0;
    gLinkIsMaster = 0;
    gLinkPlayerCount = 1;
    gPlayerCount = 1;
    gMetaKnightmareMode = 0;
    gInputRecorderMode = 0;
    gCreditsDemoSet = 0;
    gInputRecorderDemo = 0;
}

void ResetScoresAndMaxHealth(void)
{
    s32 i;

    ResetTasksAndOam();
    for (i = 0; i <= 3; i++) {
        gPlayerScores[i] = 0;
        InitPlayerState(i);
    }
    ResetPlayTime();
    if (gMetaKnightmareMode == 0) {
        if (gExtraMode == 1)
            gMaxHealth = 24;
        else
            gMaxHealth = 48;
        HudShowScore();
    } else {
        gMaxHealth = 24;
        HudShowClock();
    }
    ResetLevelStateAtHub();
}

void ResetPlayerRecords(void)
{
    s32 i;

    ResetTasksAndOam();
    for (i = 0; i <= 3; i++) {
        gPlayerLives[i] = 3;
        gPlayerHealth[i] = 0;
        gSavedPlayerAbilities[i] = gPlayerAbilities[i] = ABILITY_NORMAL;
        /* Volatile all-ones stores reuse their dead pre-read (lesson 3.68);
         * the two cells differ in signedness, so the chain stores the
         * constant twice instead of re-reading the inner cell (3.361). */
        gSavedPlayerAbilityUses[i] = gPlayerAbilityUses[i] = 0xFFFF;
        InitPlayerState(i);
        gLifeRequests.requests[i] = 0;
        gLifeRequests.gameOver[i] = 0;
    }
    gActivePlayerMask = 0;
    gActivePlayerCount = 0;
    gLifeRequests.timeout = 0;
    gScreenAttackActive = 0;
    gExtraModeTitleSeen = 0;
    gPauseDisabled = 0;
}

void HubResetPlayers(void)
{
    s32 i;

    sub_08022f98();
    gRoomExitKind = 0;
    for (i = 0; i < gPlayerCount; i++) {
        gLifeRequests.gameOver[i] = 0;
        InitPlayerState(i);
    }
    gUnk_020055C4 = 0;
}

void ResetTasksAndPlayers(void)
{
    s32 i;

    ResetTasksAndOam();
    ClearRoomBgmStarted();
    for (i = 0; i <= 3; i++)
        InitPlayerState(i);
}

void StageInit(void)
{
    s32 i;
    s8 *b;
    s8 *p;
    s8 zero;
    u8 *b2;
    u8 *p2;
    u8 zero2;
    u8 *q1;
    s8 *q2;
    s8 *q3;
    u8 *q4;

    gDispCnt |= 0x80;
    ResetFadeAndBlend();
    ResetBgScroll();
    ClearColliderLists();
    LoadGfxSet(0);
    sub_08008c7c();
    gBossSubtype = -1;
    if (gBigSwitchPressActive == 0)
        LoadRoom();
    else
        sub_080233e0();
    if ((u8)(gUnk_02000020 - 2) <= 1)
        sub_08008cb8();
    /* The ROM loads these four addresses before the clear loop below; only
     * pointer locals assigned here reproduce that order. */
    q1 = &gUnk_020061E0;
    q2 = &gBoardedWarpStarSlot;
    q3 = &gCannonFuseState;
    q4 = &gStageClearDanceBgmPlayed;
    b = gPaletteAnimRefCounts;
    zero = 0;
    p = b + 2;
    do {
        *p = zero;
        p--;
    } while ((s32)p >= (s32)b);
    *q1 = 0;
    *q2 = -1;
    *q3 = -1;
    *q4 = 0;
    sub_08066144();
    gNextActorSerial = 0;
    gLinkCommand = 0;
    gPauseDisabled = 0;
    gScreenAttackActive = 0;
    gRoomExitKind = 0;
    gDanceId = -1;
    gUnk_020055C4 = 0;
    for (i = 0; i < 4; i++)
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = gLatchedHeldKeys[i] = gLatchedPressedKeys[i] = 0;
    gStageRequest = STAGE_REQUEST_NONE;
    if (gUnk_0300244C != 0) {
        b2 = gUnk_02007CF4;
        zero2 = 0;
        p2 = b2 + 3;
        do {
            *p2 = zero2;
            p2--;
        } while ((s32)p2 >= (s32)b2);
    }
}

void HubInit(void)
{
    s32 i;
    s8 *b;
    s8 *p;
    s8 zero;

    gDispCnt |= 0x80;
    ResetFadeAndBlend();
    ResetBgScroll();
    ResetTasksAndOam();
    gBg0ScrollX = gBg1ScrollX = gBg2ScrollX = gBg3ScrollX = 0;
    gBg0ScrollY = gBg1ScrollY = gBg2ScrollY = gBg3ScrollY = 0;
    LoadGfxSet(0);
    sub_08008c7c();
    LoadHubRoom();
    b = gPaletteAnimRefCounts;
    zero = 0;
    p = b + 2;
    do {
        *p = zero;
        p--;
    } while ((s32)p >= (s32)b);
    gUnk_020061E0 = 0;
    gBossSubtype = -1;
    sub_08066144();
    for (i = 0; i < 4; i++)
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = gLatchedHeldKeys[i] = gLatchedPressedKeys[i] = 0;
    gLinkCommand = 0;
    gStageRequest = STAGE_REQUEST_NONE;
}

void BigSwitchViewInit(void)
{
    s32 i;
    s8 *b;
    s8 *p;
    s8 zero;

    gDispCnt |= 0x80;
    ResetFadeAndBlend();
    ResetBgScroll();
    LoadGfxSet(0);
    sub_08008c7c();
    LoadBigSwitchViewRoom();
    /* A reversed clear loop only strength-reduces as a do/while over a
     * signed pointer compare with a zero variable (lesson 3.30). */
    b = gPaletteAnimRefCounts;
    zero = 0;
    p = b + 2;
    do {
        *p = zero;
        p--;
    } while ((s32)p >= (s32)b);
    gUnk_020061E0 = 0;
    gBossSubtype = -1;
    sub_08066144();
    for (i = 0; i < 4; i++)
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = gLatchedHeldKeys[i] = gLatchedPressedKeys[i] = 0;
    gLinkCommand = 0;
    gStageRequest = STAGE_REQUEST_NONE;
}
