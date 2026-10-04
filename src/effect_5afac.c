#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "hud.h"
#include "player.h"
#include "actor.h"
#include "enemy.h"

/* Not from room.h or effect.h: this file's view of gUnk_02000020 and
   gUnk_02008010 differs (lesson 3.517). */
extern u32 gUnk_02000020[];
extern u32 gUnk_020060CC[];
extern u8 gUnk_02006A14[];
extern u8 gRoomExitKind;
extern u32 gUnk_02008010[];
extern u16 gObjPaletteBank8[];
extern u8 gActivePlayerMask;
extern u8 gActivePlayerCount;
extern u16 gLatchedPressedKeys[];
extern u32 gStageRequest[];
extern u32 gUnk_085B9B2C[];
extern u32 gUnk_085B9B6C[];
extern s16 gUnk_0873DBAC[];
extern s16 gGoalGameLayerHeights[];
extern u32 gPlayerGoalGameStates[];
extern u32 gPlayerGoalGameStateUpdates[];
extern u32 gUnk_0873DC3C[];
extern u8 gUnk_0873DC4C[];
extern u8 gUnk_0873DC66[];
extern u8 gUnk_0873DC80[];
extern u16 gGoalGameLayerScores[];
extern u32 gUnk_0873DCA8[];
extern u32 gUnk_0873DCC0[];
extern u32 gUnk_0873DCC8[];
extern u32 gUnk_0873DCCC[];
extern u32 gUnk_0873DD16[];
extern u32 gUnk_0873DD30[];
extern u32 gUnk_0873DD4C[];
extern u32 gUnk_0873DD5C[];
extern u32 gUnk_0873DD64[];
extern u32 gUnk_0873DD80[];
extern u32 gUnk_0873DDA2[];
extern u32 gUnk_0873DDB4[];
extern u32 gUnk_0873DDBE[];
extern u32 gUnk_0873DDE8[];
extern u32 gUnk_0873DEA0[];
extern u16 gUnk_0873DEA8[];
extern u32 gPlayerDances[];
extern u32 gUnk_0874C890[];
extern u32 gUnk_0874CDF8[];
extern u32 gUnk_08754850[];
extern u32 gUnk_0875488C[];
extern u32 gUnk_087548A0[];

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
void RequestCopy(u32 mode, void *src, void *dst, u32 size);
void QueueSprite(u32 a, s32 b, u32 c, u32 d, u32 e, u32 f);
void CallTableEntry(u32 a, u32 b, u32 *c);
u32 RandomRange(u32 range);
void PlaySfx(s32 id);
u32 TaskIsOnScreen(void);
void TaskSetEntry(void *fn, u32 i);
s32 IsOnScreen(s16 a, s16 b);
u32 IsWorldPosOnScreen(s16 x, s16 y);
void LoadGoalGameRoom(void);
void ExitClearedStage(void);
void SetCameraFocus(s32 a, s32 b);
void sub_08026998(void);
void sub_08027178(void);
void GoalGameInit(void);
void sub_0805b370(void);
s32 PlayerGoalGameUpdate(void);
void PlayerGoalGameWaitForPress(void);
s32 PlayerGoalGameCheckPress(void);
void PlayerGoalGameSetLaunchPower(void);
void sub_0805b670(void);
void sub_0805b83c(void);
void sub_0805b8b8(void);
void sub_0805b8f8(void);
void sub_0805b9a4(void);
void PlayerGoalGameLand(void);
void sub_0805bb90(void);
void sub_0805bc1c(void);
void sub_0805bc5c(void);
void sub_0805bca4(void);
void PlayerGoalGameDance(void);
void PlayerGoalGameFinish(void);
void sub_0805be48(void);
void GoalGameLaunchStarsUpdate(void);
void GoalGameLaunchStarsDraw(void);
void sub_0805c584(void);
void GoalGameCameraFollowPlayer(void);
void GoalGameCameraUpdate(void);
s32 GoalGamePlayerMarkerFollowParent(void);
void sub_0805ceec(void);
void GoalGameHelperKirbyUpdate(void);
void sub_0805d5fc(void);
void TaskStartFrameScript(s32 a0);
void TaskStartFrameScriptId(s32 a0);
void TaskUpdateFrameScript(void);
void TaskAdvanceFrameScript(void);
void sub_0805d994(s32 a0, s32 a1);
void sub_0805da2c(void);
void sub_0805dd4c(void);
void StartAllPlayersDance(void);
void PlayerDance(void);
void PlayerDanceInGoalGame(void);
void PlayerDanceAfterStageClear(void);

s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2)
{
    s8 kind = a0;
    u8 param = a1;
    s32 base;
    s32 idx;
    struct Task *t;

    if (kind == 0)
        base = 16;
    else if (kind == 1)
        base = 20;
    else if (kind == 2)
        base = 24;
    else if (kind == 3)
        base = 28;
    else
        return -1;
    idx = TaskCreateInRange(7, base, base + 3);
    if (idx == -1)
    {
        if (kind == 0)
            base = 4;
        else if (kind == 1)
            base = 7;
        else if (kind == 2)
            base = 10;
        else if (kind == 3)
            base = 13;
        else
            return -1;
        idx = TaskCreateInRange(7, base, base + 2);
    }
    if (idx != -1)
    {
        t = &gTasks[idx];
        t->unk18 = (param << 24) | (a2 & 0x00FFFFFF);
        t->posX = gCurTask->posX;
        t->pixelX = gCurTask->pixelX;
        t->posY = gCurTask->posY;
        t->pixelY = gCurTask->pixelY;
        t->facing = gCurTask->facing;
        t->player = gCurTask->player;
    }
    return idx;
}

s32 CreatePlayerEffectHighSlot(s32 a0, s32 a1, s32 a2)
{
    u8 param = a1;
    s32 idx;
    struct Task *t;

    idx = TaskCreateInRange(7, 32, 62);
    if (idx != -1)
    {
        t = &gTasks[idx];
        t->unk18 = (param << 24) | (a2 & 0x00FFFFFF);
        t->posX = gCurTask->posX;
        t->pixelX = gCurTask->pixelX;
        t->posY = gCurTask->posY;
        t->pixelY = gCurTask->pixelY;
        t->facing = gCurTask->facing;
        t->player = gCurTask->player;
        t->actorKind = 10;
    }
    return idx;
}

void GoalGameMain(void)
{
    LoadBgLayout(3);
    GoalGameInit();
    HudInit(gLocalPlayer);
    LinkRequestSync();
    LinkSyncRandom();
    LinkStartKeyExchange();
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    do
    {
        RunLinkFrame();
        LatchPlayerKeys();
    } while (*(s8 *)(IWRAM_START + 0x2438) == 0);
    LinkStopKeyExchange();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    sub_08026998();
    sub_08027178();
}

void GoalGameInit(void)
{
    s32 i;
    s8 *p;
    s8 *q;
    s32 z;

    ResetFadeAndBlend();
    ResetTasksAndOam();
    gDispCnt &= 0xE0FF;
    gDispCnt |= 248 << 5;
    gDispCnt |= 128;
    gBg0ScrollX = gBg1ScrollX = gBg2ScrollX = gBg3ScrollX = 0;
    gBg0ScrollY = gBg1ScrollY = gBg2ScrollY = gBg3ScrollY = 0;
    LoadGfxSet(0);
    LoadGoalGameRoom();
    for (i = 0; i < gPlayerCount; i++)
    {
        if (gPlayerStates[i].ability == 24)
            SetPlayerAbility(0, -1, i);
        else
            gPlayerStates[i].ability = 0;
    }
    gPauseDisabled = 1;
    gRoomExitKind = 0;
    *(s8 *)gUnk_02008010 = -1;
    *(s8 *)gStageRequest = 0;
    q = gPaletteAnimRefCounts;
    z = 0;
    p = q + 2;
    do
    {
        *p = z;
        p--;
    } while ((s32)p >= (s32)q);
}

void PlayerGoalGameInit(void)
{
    gUnk_030023D4 = 0;
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gPlayerCount)
    {
        if ((s16)gCurTask->unk6C == gCurTask->player->playerIndex)
            gCurTask->unk2C = gUnk_030023D4;
        if (((gActivePlayerMask >> (s16)gCurTask->unk6C) & 1) != 0)
            gUnk_030023D4++;
        gCurTask->unk6C++;
    }
    gCurTask->facing = 1;
    gCurTask->updateCallback = (u32)PlayerGoalGameUpdate;
    gUnk_02007D00[8] |= 1 << gCurTask->player->playerIndex;
    sub_0805b370();
    gCurTask->state = 0;
    CallTableEntry(gCurTask->state, 11, gPlayerGoalGameStates);
}

void PlayerGoalGameEnterState(void)
{
    CallTableEntry(gCurTask->state, 11, gPlayerGoalGameStates);
}

void sub_0805b370(void)
{
    gCurTask->unk30 = 0;
    gCurTask->unk34 = 0;
    gUnk_02007D00[7] = 0;
    gUnk_02007D00[9] = 0;
    gCurTask->unk70 = 0xFFFF;
    if (gCurTask->unk2C == 0)
    {
        LZ77UnCompWram((const void *)gUnk_085B9B6C[3], gUnk_02020000);
        RequestCopy(4, gUnk_02020000, gObjVram, ((u16 *)gUnk_085B9B6C)[1] << 5);
        RequestCopy(2, (void *)gUnk_085B9B6C[2], gObjPaletteBank8, ((u16 *)gUnk_085B9B6C)[0] << 5);
        gCurTask->unk6C = 0;
        do
        {
            gUnk_02007D00[(s16)gCurTask->unk6C] = TaskCreateFrom(85, 32);
            (gTasks + gUnk_02007D00[(s16)gCurTask->unk6C])->variant = gCurTask->unk6C;
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 6);
        TaskCreateFrom(90, 32);
    }
    gCurTask->posX = gUnk_0873DBAC[gActivePlayerCount * 4 + gCurTask->unk2C] << 16;
    gCurTask->posY = 232 << 18;
    TaskCreateFrom(88, 32);
    gCurTask->unk28 = TaskCreateFrom(84, 32);
    (gTasks + gCurTask->unk28)->variant = gCurTask->player->playerIndex;
    if (gActivePlayerCount > 1)
    {
        gCurTask->unk46 = TaskCreateFrom(87, 32);
        (gTasks + gCurTask->unk46)->variant = gCurTask->player->playerIndex;
    }
}

s32 PlayerGoalGameUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 11, gPlayerGoalGameStateUpdates);
}

void PlayerGoalGameState0(void)
{
    gCurTask->updateState = 0;
    TaskStartFrameScriptId(0);
    gCurTask->accelY = 128 << 7;
    TaskYieldTrampoline(27);
    gCurTask->state = 1;
    PlayerGoalGameWaitForPress();
}

void PlayerGoalGameState0Update(void)
{
    TaskUpdateFrameScript();
}

void PlayerGoalGameWaitForPress(void)
{
    gCurTask->updateState = 1;
    TaskStop();
    TaskStartFrameScriptId(1);
    TaskSleepForever();
}

void PlayerGoalGameWaitForPressUpdate(void)
{
    TaskUpdateFrameScript();
    if (PlayerGoalGameCheckPress() == 0)
    {
        sub_0805c584();
        sub_0805be48();
        gCurTask->unk30++;
        if (gCurTask->unk30 > 35)
        {
            gCurTask->state = 4;
            TaskSetEntry(PlayerGoalGameEnterState, gCurTaskIdx);
        }
    }
    else if (gUnk_030023D4 != 0)
    {
        gCurTask->state = 2;
        TaskSetEntry(PlayerGoalGameEnterState, gCurTaskIdx);
    }
    else
    {
        gCurTask->state = 3;
        TaskSetEntry(PlayerGoalGameEnterState, gCurTaskIdx);
    }
}

s32 PlayerGoalGameCheckPress(void)
{
    u16 *p = (u16 *)gLatchedPressedKeys;
    s32 n;

    if ((p[gCurTask->player->playerIndex] & 3) != 0)
    {
        PlayerGoalGameSetLaunchPower();
        n = gCurTask->unk30;
        if (n <= 24)
        {
            gCurTask->unk30 = ((24 - n) >> 1) + 24;
            gUnk_030023D4 = 1;
        }
        else
        {
            gUnk_030023D4 = 0;
        }
        gCurTask->unk70 = 0;
        return 1;
    }
    return 0;
}

void PlayerGoalGameSetLaunchPower(void)
{
    if (gCurTask->unk30 > 24)
        gCurTask->unk34 = 24 - ((gCurTask->unk30 - 24) << 1);
    else
        gCurTask->unk34 = gCurTask->unk30;
    gCurTask->unk34++;
}

void PlayerGoalGameState2(void)
{
    gCurTask->updateState = 2;
    TaskYieldTrampoline(4);
    sub_0805b670();
}

void PlayerGoalGameState2Update(void)
{
    TaskUpdateFrameScript();
    sub_0805c584();
}

void sub_0805b670(void)
{
    gCurTask->updateState = 3;
    TaskSleepForever();
}

void PlayerGoalGameState3Update(void)
{
    TaskUpdateFrameScript();
    sub_0805c584();
    gCurTask->unk30++;
    if (gCurTask->unk30 > 35)
    {
        gCurTask->state = 4;
        TaskSetEntry(PlayerGoalGameEnterState, gCurTaskIdx);
    }
}

void PlayerGoalGameLaunch(void)
{
    gCurTask->updateState = 4;
    CreateBurstEffect(1, 0);
    gCurTask->unk30 = gUnk_0873DC80[gCurTask->unk34];
    sub_0805b83c();
    sub_0805b8b8();
    TaskStartFrameScriptId(3);
    TaskCreateFrom(81, 32);
    gCurTask->unk24 = 0;
    if (gCurTask->player->playerIndex == gLocalPlayer)
    {
        if (gCurTask->unk30 <= 1)
            PlaySfx(244);
        else
            PlaySfx(227);
    }
    gCurTask->velY = 0xFFF80000;
    TaskYieldTrampoline(gUnk_0873DC4C[gCurTask->unk34]);
    gCurTask->accelY = 128 << 7;
    TaskYieldTrampoline(32);
    if (gUnk_0873DC66[gCurTask->unk34] != 0)
    {
        gCurTask->state = 5;
        sub_0805b8f8();
    }
    else
    {
        gCurTask->state = 6;
        sub_0805b9a4();
    }
}

void PlayerGoalGameLaunchUpdate(void)
{
    gCurTask->unk70++;
    TaskUpdateFrameScript();
    gCurTask->unk24++;
    if (IsWorldPosOnScreen(gCurTask->pixelX, gCurTask->pixelY) != 0)
        QueueSprite(14, gUnk_0873DC3C[gCurTask->unk24 & 3], 0, 0,
                     gCurTask->pixelX - gSpriteCameraX,
                     (s16)(gCurTask->pixelY - gSpriteCameraY + 16));
    if ((gCurTask->unk24 & 7) == 0)
        TaskCreateFrom(83, 32);
    if (gCurTask->unk24 > 31)
    {
        TaskCreateFrom(82, 32);
        gCurTask->unk24 = 0;
    }
}

void sub_0805b83c(void)
{
    gUnk_02006A14[gCurTask->player->playerIndex]--;
    if (gUnk_02006A14[gCurTask->player->playerIndex] != gCurTask->unk30)
    {
        gUnk_02006A14[gCurTask->player->playerIndex] = 7;
        if (gCurTask->unk30 == 6)
            gUnk_02006A14[gCurTask->player->playerIndex]--;
    }
    if (gUnk_02006A14[gCurTask->player->playerIndex] == 0)
        gUnk_02007D00[9] = 1;
}

void sub_0805b8b8(void)
{
    struct Task *t;

    t = &gTasks[gUnk_02007D00[gCurTask->unk30]];
    t->unk28 = 0;
    t->unk2C |= 1 << gCurTask->player->playerIndex;
}

void sub_0805b8f8(void)
{
    gCurTask->updateState = 5;
    TaskStop();
    TaskStartFrameScriptId(4);
    gCurTask->pixelY -= 4;
    gCurTask->posY = gCurTask->pixelY << 16;
    TaskYieldTrampoline(60);
    gCurTask->pixelY -= 4;
    gCurTask->posY = gCurTask->pixelY << 16;
    TaskYieldTrampoline(4);
    gCurTask->pixelY -= 4;
    gCurTask->posY = gCurTask->pixelY << 16;
    TaskYieldTrampoline(8);
    gCurTask->pixelY -= 4;
    gCurTask->posY = gCurTask->pixelY << 16;
    TaskYieldTrampoline(12);
    gCurTask->velY = -163840;
    gCurTask->accelY = 128 << 8;
    TaskYieldTrampoline(9);
    PlayerGoalGameLand();
}

void PlayerGoalGameState5Update(void)
{
    TaskUpdateFrameScript();
}

void sub_0805b9a4(void)
{
    gCurTask->updateState = 6;
    TaskStartFrameScriptId(0);
    TaskSleepForever();
}

void PlayerGoalGameState6Update(void)
{
    s32 v;
    s16 *tbl;

    TaskUpdateFrameScript();
    v = gCurTask->pixelY;
    tbl = (s16 *)gGoalGameLayerHeights;
    if (v > tbl[gCurTask->unk30] - 2)
    {
        gCurTask->state = 7;
        TaskSetEntry(PlayerGoalGameEnterState, gCurTaskIdx);
    }
}

void PlayerGoalGameLand(void)
{
    gCurTask->updateState = 7;
    TaskStop();
    gCurTask->posY = (gGoalGameLayerHeights[gCurTask->unk30] - 2) << 16;
    TaskStartFrameScriptId(5);
    TaskYieldTrampoline(2);
    TaskStartFrameScriptId(6);
    TaskYieldTrampoline(32);
    gUnk_030023D4 = 0;
    gUnk_030023B4 = 0;
    if (gPlayerCount > 1)
    {
        gCurTask->unk6C = 0;
        while ((s16)gCurTask->unk6C < gPlayerCount)
        {
            if (((gActivePlayerMask >> (s16)gCurTask->unk6C) & 1) != 0
             && gTasks[(s16)gCurTask->unk6C].unk30 == gCurTask->unk30)
            {
                if ((s16)gCurTask->unk6C == gCurTaskIdx)
                    gUnk_030023B4 = gUnk_030023D4;
                gUnk_030023D4++;
            }
            gCurTask->unk6C++;
        }
        gCurTask->unk24 = ((s16 *)gUnk_0873DBAC)[gUnk_030023D4 * 4 + gUnk_030023B4];
        if (gCurTask->unk24 == gCurTask->pixelX)
        {
            gCurTask->state = 8;
            sub_0805bc1c();
        }
        if (gCurTask->pixelX < gCurTask->unk24 + gSpriteCameraX)
            gCurTask->facing = 1;
        else
            gCurTask->facing = -1;
        gCurTask->updateCallback = (u32)sub_0805bb90;
        PlayerActionWalk();
    }
    TaskStop();
    TaskStartFrameScriptId(6);
    sub_0805bca4();
    gCurTask->state = 9;
    PlayerGoalGameDance();
}

void PlayerGoalGameLandUpdate(void)
{
    TaskUpdateFrameScript();
}

void sub_0805bb90(void)
{
    if (gCurTask->facing == 1)
    {
        if (gCurTask->pixelX >= gCurTask->unk24)
        {
            gCurTask->pixelX = gCurTask->unk24;
            gCurTask->posX = gCurTask->pixelX << 16;
            gCurTask->updateCallback = (u32)PlayerGoalGameUpdate;
            gCurTask->state = 8;
            TaskSetEntry(PlayerGoalGameEnterState, gCurTaskIdx);
        }
    }
    else
    {
        if (gCurTask->pixelX <= gCurTask->unk24)
        {
            gCurTask->pixelX = gCurTask->unk24;
            gCurTask->posX = gCurTask->pixelX << 16;
            gCurTask->updateCallback = (u32)PlayerGoalGameUpdate;
            gCurTask->state = 8;
            TaskSetEntry(PlayerGoalGameEnterState, gCurTaskIdx);
        }
    }
}

void sub_0805bc1c(void)
{
    gCurTask->updateState = 8;
    gCurTask->lateUpdateCallback = (u32)sub_0805bc5c;
    gCurTask->facing = 1;
    TaskStop();
    TaskStartFrameScriptId(6);
    sub_0805bca4();
    TaskSleepForever();
}

void PlayerGoalGameState8Update(void)
{
    TaskUpdateFrameScript();
}

void sub_0805bc5c(void)
{
    struct Task *t;

    t = &gTasks[gUnk_02007D00[gCurTask->unk30]];
    if (t->unk2C == 0)
    {
        gCurTask->state = 9;
        TaskSetEntry(PlayerGoalGameEnterState, gCurTaskIdx);
    }
}

void sub_0805bca4(void)
{
    struct Task *t;

    t = &gTasks[gUnk_02007D00[gCurTask->unk30]];
    t->unk2C &= ~(1 << gCurTask->player->playerIndex);
}

void PlayerGoalGameDance(void)
{
    gCurTask->updateState = 9;
    gCurTask->lateUpdateCallback = 0;
    if (gCurTask->unk30 == 0)
        TaskYieldTrampoline(50);
    else
        TaskYieldTrampoline(30);
    TaskStartFrameScriptId(0);
    PlayerDance();
    gCurTask->state = 10;
    PlayerGoalGameFinish();
}

void PlayerGoalGameDanceUpdate(void)
{
    TaskUpdateFrameScript();
}

void PlayerGoalGameFinish(void)
{
    gCurTask->updateCallback = (u32)PlayerGoalGameUpdate;
    gCurTask->updateState = 10;
    if ((gCurTask->spriteFlags & (128 << 8)) != 0)
        gCurTask->facing = -1;
    else
        gCurTask->facing = 1;
    TaskStartFrameScriptId(7);
    if (gCurTask->unk30 == 0)
        TaskYieldTrampoline(30);
    else if (gCurTask->unk30 != 6)
        AddPlayerScore(gGoalGameLayerScores[gCurTask->unk30],
                     gCurTask->player->playerIndex);
    if (gUnk_02006A14[gCurTask->player->playerIndex] == 0)
    {
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(30);
    }
    gUnk_02007D00[8] &= ~(1 << gCurTask->player->playerIndex);
    while (gUnk_02007D00[8] != 0)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(10);
    *(s8 *)gStageRequest = 1;
    TaskSleepForever();
}

void PlayerGoalGameFinishUpdate(void)
{
    TaskUpdateFrameScript();
}

void sub_0805be48(void)
{
    if (((gActivePlayerMask >> gLocalPlayer) & 1) != 0
     && gLocalPlayer == gCurTask->player->playerIndex
     && (gFrameCount & 4) != 0)
        QueueSprite(8, (u32)gUnk_085B9B2C, 0, 0x00009010, 120, 70);
}

void Task_GoalGameLaunchStars(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)GoalGameLaunchStarsDraw;
    gCurTask->updateCallback = (u32)GoalGameLaunchStarsUpdate;
    gCurTask->layer = 12;
    gCurTask->frameTable = gUnk_0874C890;
    gCurTask->tileWord = 0;
    TaskStop();
    gCurTask->pixelX = gTasks[gCurTask->parent].pixelX;
    gCurTask->pixelY = gTasks[gCurTask->parent].pixelY + 32;
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->posY = gCurTask->pixelY << 16;
    gCurTask->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame = 1;
    TaskYieldTrampoline(1);
    gCurTask->frame = 2;
    TaskYieldTrampoline(1);
    gCurTask->frame = 3;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame = 1;
    TaskYieldTrampoline(1);
    while (1)
    {
        gCurTask->pixelX = gTasks[gCurTask->parent].pixelX;
        gCurTask->pixelY = gTasks[gCurTask->parent].pixelY + 32;
        gCurTask->posX = gCurTask->pixelX << 16;
        gCurTask->posY = gCurTask->pixelY << 16;
        gCurTask->frame = 6;
        TaskYieldTrampoline(1);
        gCurTask->frame = 7;
        TaskYieldTrampoline(1);
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->frame = 5;
        TaskYieldTrampoline(1);
        gCurTask->frame = 6;
        TaskYieldTrampoline(1);
        gCurTask->frame = 7;
        TaskYieldTrampoline(1);
        gCurTask->pixelX = gTasks[gCurTask->parent].pixelX;
        gCurTask->pixelY = gTasks[gCurTask->parent].pixelY + 32;
        gCurTask->posX = gCurTask->pixelX << 16;
        gCurTask->posY = gCurTask->pixelY << 16;
        gCurTask->frame = 8;
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        gCurTask->frame = 10;
        TaskYieldTrampoline(1);
        gCurTask->frame = 11;
        TaskYieldTrampoline(1);
        gCurTask->frame = 8;
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
    }
}

void GoalGameLaunchStarsFall(void)
{
    gCurTask->updateCallback = 0;
    gCurTask->accelY = 128 << 7;
    gCurTask->speedLimitY = 192 << 10;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 10;
        TaskYieldTrampoline(1);
        gCurTask->frame = 11;
        TaskYieldTrampoline(1);
        gCurTask->frame = 8;
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 9);
    TaskExitTrampoline();
}

void GoalGameLaunchStarsUpdate(void)
{
    if (gTasks[gCurTask->parent].accelY != 0)
        TaskSetEntry(GoalGameLaunchStarsFall, gCurTaskIdx);
}

void GoalGameLaunchStarsDraw(void)
{
    TaskDrawWorld();
    {
        s16 *tbl = (s16 *)gUnk_0873DCA8;

        if (tbl[gCurTask->frame] != -1
            && IsOnScreen(gCurTask->pixelX - gSpriteCameraX,
                            gCurTask->pixelY - gSpriteCameraY + 48))
        {
            struct Task *q = gCurTask;
            u32 *g = q->frameTable;

            QueueSprite(q->layer, g[tbl[q->frame]], q->spriteFlags, q->tileWord,
                         q->pixelX - gSpriteCameraX,
                         (s16)(q->pixelY - gSpriteCameraY + 48));
        }
    }
}

void Task_GoalGameBigTrailStar(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 11;
    gCurTask->frameTable = gUnk_0874C890;
    gCurTask->tileWord = 0;
    gCurTask->pixelX = gTasks[gCurTask->parent].pixelX
                         + ((s8 *)gUnk_0873DCC0)[RandomRange(8)];
    gCurTask->pixelY = gTasks[gCurTask->parent].pixelY
                         + ((s8 *)gUnk_0873DCC8)[RandomRange(4)];
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->posY = gCurTask->pixelY << 16;
    TaskSetMotionY(gTasks[gCurTask->parent].velY + (128 << 9),
                 128 << 7, 192 << 10);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 13;
        TaskYieldTrampoline(1);
        gCurTask->frame = 14;
        TaskYieldTrampoline(1);
        gCurTask->frame = 15;
        TaskYieldTrampoline(1);
        gCurTask->frame = 14;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 16;
        TaskYieldTrampoline(1);
        gCurTask->frame = 17;
        TaskYieldTrampoline(1);
        gCurTask->frame = 18;
        TaskYieldTrampoline(1);
        gCurTask->frame = 19;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 20;
        TaskYieldTrampoline(1);
        gCurTask->frame = 21;
        TaskYieldTrampoline(1);
        gCurTask->frame = 22;
        TaskYieldTrampoline(1);
        gCurTask->frame = 23;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 24;
        TaskYieldTrampoline(1);
        gCurTask->frame = 25;
        TaskYieldTrampoline(1);
        gCurTask->frame = 26;
        TaskYieldTrampoline(1);
        gCurTask->frame = 27;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskExitTrampoline();
}

void Task_GoalGameSmallTrailStar(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 13;
    gCurTask->frameTable = gUnk_0874C890;
    gCurTask->tileWord = 0;
    gCurTask->pixelX = gTasks[gCurTask->parent].pixelX
                         + ((s8 *)gUnk_0873DCC0)[RandomRange(8)];
    gCurTask->pixelY = gTasks[gCurTask->parent].pixelY
                         + ((s8 *)gUnk_0873DCC8)[RandomRange(4)];
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->posY = gCurTask->pixelY << 16;
    TaskSetMotionY(gTasks[gCurTask->parent].velY + (128 << 8),
                 128 << 7, 192 << 10);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 20;
        TaskYieldTrampoline(1);
        gCurTask->frame = 21;
        TaskYieldTrampoline(1);
        gCurTask->frame = 22;
        TaskYieldTrampoline(1);
        gCurTask->frame = 23;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 24;
        TaskYieldTrampoline(1);
        gCurTask->frame = 25;
        TaskYieldTrampoline(1);
        gCurTask->frame = 26;
        TaskYieldTrampoline(1);
        gCurTask->frame = 27;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskExitTrampoline();
}

void sub_0805c584(void)
{
    struct Task *t = gCurTask;
    s16 *tbl = (s16 *)gUnk_0873DCCC;

    t->pixelY = 1024 + tbl[t->unk30];
    t->posY = t->pixelY << 16;
    (gTasks + t->unk28)->posY = (tbl[t->unk30] + 1048) << 16;
    if (t->unk30 == 24)
    {
        (gTasks + t->unk28)->frame = 2;
        (gTasks + t->unk28)->sleepFrames = 2;
    }
}

void Task_GoalGameCamera(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->updateCallback = (u32)GoalGameCameraUpdate;
    if ((gActivePlayerMask >> gLocalPlayer) & 1)
    {
        TaskStop();
        gCurTask->unk28 = -1;
        gCurTask->unk2C = 0;
        gCurTask->pixelX = 144;
        gCurTask->pixelY = gTasks[gLocalPlayer].pixelY;
        gCurTask->posX = gCurTask->pixelX << 16;
        gCurTask->posY = gCurTask->pixelY << 16;
        while (gTasks[gLocalPlayer].state <= 3)
        {
            gCurTask->pixelY = gTasks[gLocalPlayer].pixelY;
            gCurTask->posY = gCurTask->pixelY << 16;
            TaskYieldTrampoline(1);
        }
        gCurTask->pixelY = gTasks[gLocalPlayer].pixelY;
        gCurTask->posY = gCurTask->pixelY << 16;
        TaskYieldTrampoline(1);
        gCurTask->velY = 0xFFF78000;
        {
            u8 *t1 = (u8 *)gUnk_0873DD16;

            TaskYieldTrampoline(t1[gTasks[gLocalPlayer].unk34]);
        }
        gCurTask->accelY = 136 << 7;
        TaskYieldTrampoline(32);
        TaskStop();
        {
            u8 *t2 = (u8 *)gUnk_0873DD30;

            TaskYieldTrampoline(t2[gTasks[gLocalPlayer].unk34]);
        }
        gCurTask->accelY = 128 << 7;
        while (gCurTask->pixelY
               < gGoalGameLayerHeights[gTasks[gLocalPlayer].unk30] - 2)
            TaskYieldTrampoline(1);
        TaskStop();
        TaskSleepForever();
    }
    else
    {
        for (gUnk_030023D4 = 0; gUnk_030023D4 < gPlayerCount; gUnk_030023D4++)
        {
            if ((gActivePlayerMask >> gUnk_030023D4) & 1)
            {
                gCurTask->unk28 = gUnk_030023D4;
                break;
            }
        }
        gCurTask->unk2C = 0;
        GoalGameCameraFollowPlayer();
    }
}

void GoalGameCameraFollowPlayer(void)
{
    TaskStop();
    gCurTask->pixelX = 144;
    gCurTask->pixelY = gTasks[gCurTask->unk28].pixelY;
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->posY = gCurTask->pixelY << 16;
    while (gTasks[gCurTask->unk28].state <= 3)
    {
        gCurTask->pixelY = gTasks[gCurTask->unk28].pixelY;
        gCurTask->posY = gCurTask->pixelY << 16;
        TaskYieldTrampoline(1);
    }
    gCurTask->pixelY = gTasks[gCurTask->unk28].pixelY;
    gCurTask->posY = gCurTask->pixelY << 16;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFF78000;
    {
        u8 *t1 = (u8 *)gUnk_0873DD16;

        TaskYieldTrampoline(t1[gTasks[gCurTask->unk28].unk34]
                            - (s16)gTasks[gCurTask->unk28].unk70 + 1);
    }
    gCurTask->accelY = 136 << 7;
    TaskYieldTrampoline(32);
    TaskStop();
    {
        u8 *t2 = (u8 *)gUnk_0873DD30;

        TaskYieldTrampoline(t2[gTasks[gCurTask->unk28].unk34]);
    }
    gCurTask->accelY = 128 << 7;
    while (gCurTask->pixelY
           < gGoalGameLayerHeights[gTasks[gCurTask->unk28].unk30] - 2)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSleepForever();
}

void GoalGameCameraUpdate(void)
{
    u16 x;
    u16 y;

    x = gCurTask->pixelX;
    y = gCurTask->pixelY + gCurTask->unk2C;
    if ((s16)y > 0x41C)
        y = 0x41C;
    SetCameraFocus((s16)x, (s16)y);
    if (gCurTask->unk2C != 0)
    {
        if (gCurTask->unk2C > 0)
        {
            gCurTask->unk2C -= 4;
            if (gCurTask->unk2C < 0)
                gCurTask->unk2C = 0;
        }
        else
        {
            gCurTask->unk2C += 4;
            if (gCurTask->unk2C > 0)
                gCurTask->unk2C = 0;
        }
    }
    if (gCurTask->unk28 >= 0)
    {
        for (gUnk_030023D4 = 0; gUnk_030023D4 < gPlayerCount; gUnk_030023D4++)
        {
            if ((s16)gTasks[gCurTask->unk28].unk70 < 0)
            {
                if (((gActivePlayerMask >> gUnk_030023D4) & 1)
                    && gCurTask->unk28 != gUnk_030023D4
                    && (s16)gTasks[gUnk_030023D4].unk70 >= 0)
                {
                    gCurTask->unk2C = 0;
                    gCurTask->unk28 = gUnk_030023D4;
                    gCurTask->pixelY = gTasks[gUnk_030023D4].pixelY;
                    gCurTask->posY = gCurTask->pixelY << 16;
                    TaskSetEntry(GoalGameCameraFollowPlayer, gCurTaskIdx);
                }
            }
            else
            {
                if (((gActivePlayerMask >> gUnk_030023D4) & 1)
                    && gCurTask->unk28 != gUnk_030023D4
                    && (s16)gTasks[gUnk_030023D4].unk70 > 0
                    && gTasks[gCurTask->unk28].unk34
                           < gTasks[gUnk_030023D4].unk34)
                {
                    gCurTask->unk2C = gCurTask->pixelY + gCurTask->unk2C
                                         - gTasks[gUnk_030023D4].pixelY;
                    gCurTask->unk28 = gUnk_030023D4;
                    gCurTask->pixelY = gTasks[gUnk_030023D4].pixelY;
                    gCurTask->posY = gCurTask->pixelY << 16;
                    TaskSetEntry(GoalGameCameraFollowPlayer, gCurTaskIdx);
                }
            }
        }
    }
}

void Task_GoalGameSpring(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 8;
    if (gActivePlayerCount == 1)
        gCurTask->frameTable = gUnk_08754850;
    else
        gCurTask->frameTable = (u32 *)gUnk_0873DD4C[gCurTask->variant];
    gCurTask->tileWord = 0x00009010;
    {
        s16 *tbl = (s16 *)gUnk_0873DBAC;

        gCurTask->posX = tbl[(gActivePlayerCount << 2)
            + gTasks[gCurTask->parent].unk2C] << 16;
    }
    gCurTask->posY = 131 << 19;
    while (1)
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(4);
        gCurTask->frame = 1;
        TaskYieldTrampoline(4);
    }
}

void Task_GoalGamePlayerMarker(void)
{
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 11;
    gCurTask->frameTable = gUnk_0875488C;
    gCurTask->updateCallback = (u32)GoalGamePlayerMarkerFollowParent;
    gCurTask->tileWord = 0x0000A010;
    if (gLocalPlayer == gCurTask->variant)
    {
        gCurTask->frame = 4;
    }
    else
    {
        u16 *tbl = (u16 *)gUnk_0873DD5C;
        gCurTask->frame = tbl[gCurTask->variant];
    }
    TaskYieldTrampoline(27);
    TaskExitTrampoline();
}

s32 GoalGamePlayerMarkerFollowParent(void)
{
    gCurTask->pixelX = (gTasks + gCurTask->parent)->pixelX;
    gCurTask->pixelY = (gTasks + gCurTask->parent)->pixelY - 24;
}

void Task_GoalGameSign(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 9;
    gCurTask->frameTable = (u32 *)gUnk_0873DD64[gCurTask->variant];
    gCurTask->posX = 200 << 16;
    {
        s16 *t = (s16 *)gGoalGameLayerHeights;

        gCurTask->posY = (t[gCurTask->variant] - 8) << 16;
    }
    gCurTask->tileWord = 0x00008010;
    gCurTask->unk28 = 1;
    gCurTask->unk2C = 0;
    gCurTask->frame = 0;
    do
    {
        TaskYieldTrampoline(1);
    } while (gCurTask->unk28 != 0 || gCurTask->unk2C != 0);
    gCurTask->unk46 = TaskCreateFrom(86, 32);
    (gTasks + gCurTask->unk46)->unk74 = gCurTask->variant;
    (gTasks + gCurTask->unk46)->unk28 = gUnk_02007D00[7];
    gUnk_02007D00[7]++;
    if (gCurTask->variant == 0)
    {
        TaskYieldTrampoline(24);
        gCurTask->updateCallback = (u32)sub_0805ceec;
        gCurTask->unk28 = 1;
        do
        {
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 != 0);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 1;
            TaskYieldTrampoline(1);
            gCurTask->frame = 2;
            TaskYieldTrampoline(1);
            gCurTask->frame = 3;
            TaskYieldTrampoline(2);
            gCurTask->frame = 4;
            TaskYieldTrampoline(24);
            gCurTask->frame = 3;
            TaskYieldTrampoline(2);
            gCurTask->frame = 2;
            TaskYieldTrampoline(1);
            gCurTask->frame = 1;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 2);
        gCurTask->frame = 1;
        TaskYieldTrampoline(3);
        gCurTask->frame = 2;
        TaskYieldTrampoline(3);
        gCurTask->frame = 3;
        TaskYieldTrampoline(3);
        gCurTask->frame = 4;
        TaskSleepForever();
    }
    else
    {
        gCurTask->unk28 = 1;
        do
        {
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 != 0);
        gCurTask->velY = 0xFFF8CD00;
        TaskYieldTrampoline(3);
        gCurTask->velY = 0xFFFB3300;
        TaskYieldTrampoline(3);
        gCurTask->velY = 0xFFFD9A00;
        TaskYieldTrampoline(3);
        gCurTask->velY = 0x00026600;
        TaskYieldTrampoline(3);
        gCurTask->velY = 0x0004CD00;
        TaskYieldTrampoline(5);
        TaskStop();
        TaskYieldTrampoline(27);
        gCurTask->frame = 1;
        TaskYieldTrampoline(5);
        gCurTask->frame = 2;
        TaskYieldTrampoline(5);
        gCurTask->frame = 3;
        TaskYieldTrampoline(5);
        gCurTask->frame = 4;
    }
    TaskSleepForever();
}

void sub_0805ceec(void)
{
    gCurTask->pixelX = (gTasks + gCurTask->unk46)->pixelX - 4;
    gCurTask->pixelY = (gTasks + gCurTask->unk46)->pixelY - 16;
}

void Task_GoalGameHelperKirby(void)
{
    gCurTask->updateCallback = (u32)GoalGameHelperKirbyUpdate;
    gCurTask->posX = 248 << 16;
    gCurTask->facing = 255;
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 0;
    gCurTask->unk24 = 0;
    if (gCurTask->unk74 == 0)
        gCurTask->variant = 1;
    else
        gCurTask->variant = 0;
    sub_0805d994(gCurTask->unk28, gCurTask->variant);
    gCurTask->layer = 8;
    switch (gCurTask->variant)
    {
    case 1:
        if (((gActivePlayerMask >> gLocalPlayer) & 1)
            && (gTasks + gLocalPlayer)->unk30 != 0)
            gCurTask->drawCallback = 0;
        gCurTask->posX = 244 << 16;
        gCurTask->posY = 196 << 16;
        TaskStartFrameScript((s32)gUnk_0873DDB4);
        gCurTask->velX = 0xFFFB3300;
        gCurTask->velY = 0xFFFC6600;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0xFFFD9A00;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0xFFFECD00;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0xFFFF6600;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0x00009A00;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0x00013300;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0x00026600;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0x0004CD00;
        TaskYieldTrampoline(5);
        gCurTask->velY = 0;
        TaskYieldTrampoline(5);
        TaskStop();
        TaskYieldTrampoline(53);
        TaskStartFrameScript((s32)gUnk_0873DDA2);
        gCurTask->velX = 0xFFFD6000;
        TaskYieldTrampoline(60);
        TaskStartFrameScript((s32)gUnk_0873DD80);
        TaskStop();
        (gTasks + gCurTask->parent)->unk28 = 0;
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->velY = 0xFFFECD00;
            TaskYieldTrampoline(3);
            gCurTask->velY = 0xFFFF6600;
            TaskYieldTrampoline(3);
            gCurTask->velY = 0x00009A00;
            TaskYieldTrampoline(3);
            gCurTask->velY = 0x00013300;
            TaskYieldTrampoline(3);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 7);
        gCurTask->velY = 0;
        TaskStartFrameScript((s32)gUnk_0873DDA2);
        gCurTask->velX = 0x00009A00;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0x00013300;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0x00026600;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0x0004CD00;
        TaskYieldTrampoline(10);
        gCurTask->velX = 0x0004CD00;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0x00026600;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0x00013300;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0x00009A00;
        TaskYieldTrampoline(5);
        for (gCurTask->unk6C = 0;
             (s16)gCurTask->unk6C < gPlayerCount;
             gCurTask->unk6C++)
        {
            if (((gActivePlayerMask >> (s16)gCurTask->unk6C) & 1)
                && (gTasks + (s16)gCurTask->unk6C)->unk30 == 0)
            {
                gCurTask->unk2C |= 1 << (s16)gCurTask->unk6C;
                gCurTask->unk30++;
            }
        }
        gCurTask->unk34 = gCurTask->unk30 - 1;
        gCurTask->velX = 0xFFFF6600;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0xFFFECD00;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0xFFFD9A00;
        TaskYieldTrampoline(5);
        gCurTask->velX = 0xFFFB3300;
        TaskYieldTrampoline(60);
        TaskStop();
        if (gUnk_02007D00[9] != 0)
        {
            gCurTask->facing = 1;
            gCurTask->velX = 192 << 10;
            TaskYieldTrampoline(36);
            gCurTask->velX = 128 << 10;
            TaskYieldTrampoline(8);
            gCurTask->velX = 128 << 9;
            TaskYieldTrampoline(8);
            gCurTask->velX = 128 << 8;
            TaskYieldTrampoline(8);
            TaskStop();
            gCurTask->unk6C = 0;
            do
            {
                for (gCurTask->unk6E = 0;
                     gCurTask->unk6E < gPlayerCount;
                     gCurTask->unk6E++)
                {
                    if (((gActivePlayerMask >> gCurTask->unk6E) & 1)
                        && ((u8 *)gUnk_02006A14)[gCurTask->unk6E] == 0)
                    {
                        gCurTask->unk46 = TaskCreateFrom(89, 32);
                        (gTasks + gCurTask->unk46)->unk2C
                            = gCurTask->unk6E;
                        (gTasks + gCurTask->unk46)->variant = 1;
                    }
                }
                TaskYieldTrampoline(11);
                gCurTask->unk6C++;
            } while ((s16)gCurTask->unk6C <= 29);
        }
        TaskSleepForever();
        break;
    case 0:
        gCurTask->posX = 240 << 16;
        {
            s16 *t = (s16 *)gGoalGameLayerHeights;

            gCurTask->posY = (t[gCurTask->unk74] - 2) << 16;
        }
        TaskStartFrameScript((s32)gUnk_0873DDBE);
        gCurTask->velX = 0xFFFE0000;
        TaskYieldTrampoline(23);
        TaskStop();
        TaskStartFrameScript(0);
        gCurTask->facing = 1;
        gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame(165);
            TaskYieldTrampoline(4);
            TaskSetFrame(166);
            TaskYieldTrampoline(4);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 3);
        TaskSetFrame(167);
        TaskYieldTrampoline(4);
        (gTasks + gCurTask->parent)->unk28 = 0;
        TaskSetFrame(165);
        TaskYieldTrampoline(2);
        TaskSetFrame(168);
        TaskYieldTrampoline(6);
        TaskSetFrame(169);
        TaskYieldTrampoline(4);
        TaskSetFrame(170);
        TaskYieldTrampoline(4);
        TaskSetFrame(171);
        TaskSleepForever();
        break;
    }
}

void GoalGameHelperKirbyUpdate(void)
{
    TaskUpdateFrameScript();
    if (gCurTask->unk2C == 0)
        return;
    for (gCurTask->unk6C = gPlayerCount - 1;
         (s16)gCurTask->unk6C >= 0;
         gCurTask->unk6C--)
    {
        if (((gActivePlayerMask >> (s16)gCurTask->unk6C) & 1)
            && ((gCurTask->unk2C >> (s16)gCurTask->unk6C) & 1)
            && gCurTask->pixelX < gUnk_0873DBAC[(gCurTask->unk30 << 2) + gCurTask->unk34] - 6)
        {
            if (gUnk_02006A14[(s16)gCurTask->unk6C] != 0)
            {
                gCurTask->unk46 = TaskCreateFrom(89, 32);
                (gTasks + gCurTask->unk46)->unk2C = (s16)gCurTask->unk6C;
                (gTasks + gCurTask->unk46)->unk30 = gCurTask->unk30;
                (gTasks + gCurTask->unk46)->unk34 = gCurTask->unk34;
                (gTasks + gCurTask->unk46)->variant = 0;
            }
            gCurTask->unk2C &= ~(1 << (s16)gCurTask->unk6C);
            gCurTask->unk34--;
        }
    }
    gCurTask->unk24++;
}

void sub_0805d564(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)sub_0805d5fc;
    gCurTask->layer = 15;
    gCurTask->frameTable = gUnk_087548A0;
    gCurTask->tileWord = 0x0000B010;
    gCurTask->posX = (gTasks + gCurTask->parent)->pixelX << 16;
    gCurTask->posY = (gTasks + gCurTask->parent)->pixelY << 16;
    gCurTask->frame = 0;
    while (1)
    {
        gCurTask->unk6C = 0;
        do
        {
            TaskYieldTrampoline(3);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 3);
    }
}

void sub_0805d5fc(void)
{
    TaskDrawWorld();
    if (TaskIsOnScreen() != 0)
    {
        u32 *tbl = gUnk_0874CDF8;
        struct Task *t = gCurTask;

        QueueSprite(14, tbl[(s16)t->unk6C], 0, 0,
                     t->pixelX - gSpriteCameraX,
                     (s16)(t->pixelY - gSpriteCameraY));
    }
}

void Task_GoalGameOneUp(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 10;
    gCurTask->frameTable = gOneUpFrames;
    gCurTask->tileWord = 240 << 8;
    gCurTask->frame = 4;
    switch (gCurTask->variant)
    {
    case 0:
        gCurTask->posX = (gUnk_0873DBAC[(gCurTask->unk30 << 2)
            + gCurTask->unk34] - 6) << 16;
        gCurTask->posY = (gTasks + gCurTask->parent)->pixelY << 16;
        gCurTask->unk24 = 44 - gTasks[gCurTask->parent].unk24;
        if (gCurTask->unk24 & 1)
            TaskYieldTrampoline(1);
        gCurTask->velY = 0xFFFE0000;
        TaskYieldTrampoline(gCurTask->unk24 >> 1);
        gCurTask->velX = 128 << 8;
        gCurTask->velY = 0xFFFE0000;
        TaskYieldTrampoline(4);
        gCurTask->velY = 0xFFFF0000;
        TaskYieldTrampoline(4);
        gCurTask->velY = 0xFFFF8000;
        TaskYieldTrampoline(4);
        TaskStop();
        TaskYieldTrampoline(5);
        gCurTask->velY = 128 << 9;
        TaskYieldTrampoline(8);
        gCurTask->velY = 128 << 10;
        TaskYieldTrampoline(gCurTask->unk24 >> 1);
        TaskYieldTrampoline(22);
        break;
    case 1:
        gCurTask->posX = gTasks[gCurTask->parent].pixelX << 16;
        gCurTask->posY = gTasks[gCurTask->parent].pixelY << 16;
        gCurTask->velX = (gTasks[gCurTask->unk2C].pixelX
            - gTasks[gCurTask->parent].pixelX) << 11;
        gCurTask->velY = 0xFFFC0000;
        TaskYieldTrampoline(4);
        gCurTask->velY = 0xFFFE0000;
        TaskYieldTrampoline(4);
        gCurTask->velY = 0xFFFF0000;
        TaskYieldTrampoline(4);
        gCurTask->velY = 0xFFFF8000;
        TaskYieldTrampoline(4);
        gCurTask->velY = 128 << 8;
        TaskYieldTrampoline(4);
        gCurTask->velY = 128 << 9;
        TaskYieldTrampoline(4);
        gCurTask->velY = 128 << 10;
        TaskYieldTrampoline(4);
        gCurTask->velY = 128 << 11;
        TaskYieldTrampoline(4);
        TaskStopX();
        TaskYieldTrampoline(8);
        break;
    }
    if (gCurTask->unk2C == gLocalPlayer)
        PlaySfx(220);
    AddPlayerLives(1, gCurTask->unk2C);
    TaskExitTrampoline();
}

void TaskStartFrameScript(s32 a0)
{
    gCurTask->unk18 = a0;
    gCurTask->unk1C = 0;
    gCurTask->unk20 = 0;
    if (a0 != 0)
        TaskAdvanceFrameScript();
}

void TaskStartFrameScriptId(s32 a0)
{
    u32 i = (u8)a0;

    if (i > 7)
        while (1) {}
    gCurTask->unk18 = *(gUnk_0873DDE8 + i);
    gCurTask->unk1C = 0;
    gCurTask->unk20 = 0;
    TaskAdvanceFrameScript();
}

void TaskUpdateFrameScript(void)
{
    if (gCurTask->unk18 != 0)
    {
        if (--gCurTask->unk20 <= 0)
            TaskAdvanceFrameScript();
    }
}

void TaskAdvanceFrameScript(void)
{
    struct Task *t;
    s32 i;
    s16 *p;
    s32 c;
    s32 j;
    struct Task *t2;
    s32 k;
    s16 *q;
    s32 v;
    struct Task *t3;
    s32 m;
    s16 *r;

top:
    t = gCurTask;
    i = t->unk1C;
    p = (s16 *)t->unk18;
    c = p[i];
    switch (c)
    {
    case -2:
        return;
    case -3:
        t->unk1C = 0;
        goto top;
    case -4:
        j = i + 1;
        t->unk1C = j;
        TaskStartFrameScriptId(*(u8 *)&p[j]);
        return;
    }
    t2 = gCurTask;
    k = t2->unk1C;
    q = (s16 *)t2->unk18;
    v = q[k];
    k++;
    t2->unk1C = k;
    TaskSetFrame(v);
    t3 = gCurTask;
    m = t3->unk1C;
    r = (s16 *)t3->unk18;
    t3->unk20 = r[m];
    m++;
    t3->unk1C = m;
}

void sub_0805d994(s32 a0, s32 a1)
{
    if (a1 == 0)
    {
        if (a0 <= 1)
            gCurTask->tileWord = ((a0 << 3) + 768) | -16368;
        else
            gCurTask->tileWord = ((a0 << 3) + 896) | -16368;
    }
    else
    {
        if (a0 <= 1)
            gCurTask->tileWord = ((a0 << 3) + 768) | -12272;
        else
            gCurTask->tileWord = ((a0 << 3) + 896) | -12272;
    }
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)sub_0805da2c;
    gCurTask->layer = 7;
    gCurTask->frameTable = gPlayerFrames;
}

void sub_0805da2c(void)
{
    struct Task *t;
    struct Task *s;
    struct Task *u;
    struct TaskGfx *g;
    u32 *tbl;
    u16 *p;
    u8 *dst;
    u16 a;
    u16 **pal;

    t = gCurTask;
    if (t->frameTable == NULL)
        return;
    if (t->frame == -1)
        return;
    if (TaskIsOnScreen() == 0)
        return;
    s = gCurTask;
    a = s->tileWord;
    tbl = s->frameTable;
    g = (struct TaskGfx *)tbl[s->frame];
    if ((g->oamTemplate & 1) != 0)
    {
        pal = &g->palette;
        if (g->palette != NULL)
            RequestCopy(2, g->palette + 1, gObjPalette + ((a >> 12) << 5), *g->palette);
        p = pal[1];
        dst = (u8 *)(((a & 0x7FF) << 5) + (BG_VRAM + 0xFE00));
        while (*p != 0xFFFF)
        {
            u16 *q = p + 1;
            RequestCopy(4, q, dst, *p);
            p = (u16 *)((u8 *)q + *p);
            dst += 0x400;
        }
    }
    else
    {
        if (*g->palette != 0)
            RequestCopy(2, g->palette + 1, gObjPalette + ((a >> 12) << 5), *g->palette);
        p = g->tiles;
        dst = (u8 *)(((a & 0x7FF) << 5) + (BG_VRAM + 0xFE00));
        while (*p != 0xFFFF)
        {
            u16 *q = p + 1;
            RequestCopy(4, q, dst, *p);
            p = (u16 *)((u8 *)q + *p);
            dst += 0x400;
        }
    }
    u = gCurTask;
    QueueSprite(u->layer, g->oamTemplate & ~1, u->spriteFlags, 0x800 | u->tileWord,
                 u->pixelX - gSpriteCameraX, (s16)(u->pixelY - gSpriteCameraY));
}

void sub_0805dba0(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateCallback = 0;
    t->lateUpdateCallback = 0;
    t->moveCallback = (u32)TaskMove;
    t->frameTable = gPlayerFrames;
    TaskStop();
    u = gCurTask;
    u->spriteFlags &= 0x7FFF;
    u->player->unk42 &= 0xFFEF;
    CallTableEntry(u->state, 2, gUnk_0873DEA0);
}

void sub_0805dbfc(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_0873DEA0);
}

/* NOTE: needs hdr.c corrected to "extern u16 gUnk_0873DEA8[];" (stride 2, ldrh, symbol-first) */
void sub_0805dc18(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    t = gCurTask;
    t->updateCallback = (u32)sub_0805dd4c;
    t->facing = 1;
    u = gCurTask;
    u->spriteFlags &= 0x7FFF;
    switch ((s8)u->player->ability)
    {
    case 0:
    case 7:
    case 20:
    case 21:
    case 24:
        v = gCurTask;
        v->spriteFlags |= 128 << 8;
        break;
    case 1:
    case 2:
    case 5:
    case 19:
        while (1)
        {
            w = gCurTask;
            w->frame = gUnk_0873DEA8[(s8)w->player->ability];
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    }
    x = gCurTask;
    x->frame = gUnk_0873DEA8[(s8)x->player->ability];
    TaskSleepForever();
}

void sub_0805dd4c(void)
{
    if (gActivePlayerCount == 1)
    {
        if ((s16)gTaskSlotTypes[gCurTask->unk46] == -1)
            StartAllPlayersDance();
    }
}

void sub_0805dd88(void)
{
    gCurTask->updateCallback = (u32)sub_0805dd4c;
    gCurTask->facing = 1;
    TaskSetFrameFlip(146);
    TaskSleepForever();
}

void sub_0805ddb0(s32 a0)
{
    struct Task *t;

    t = &gTasks[a0];
    t->unk46 = CreatePlayerEffectHighSlot((s8)a0, 19, 0);
    gTasks[t->unk46].parent = a0;
    gTasks[t->unk46].player = t->player;
    switch ((s8)gPlayerStates[a0].ability)
    {
    case 0:
    case 7:
    case 11:
    case 20:
    case 21:
        break;
    default:
        gTasks[CreatePlayerEffect((s8)a0, 17, 0)].parent = a0;
        break;
    }
    gPlayerStates[a0].ability = 0;
    gPlayerStates[a0].mode = 22;
}

void StartAllPlayersDance(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (((gActivePlayerMask >> i) & 1) != 0)
        {
            if (gPlayerCount != 1)
            {
                switch ((s8)gPlayerStates[i].ability)
                {
                case 0:
                case 7:
                case 11:
                case 20:
                case 21:
                    break;
                default:
                    gTasks[CreatePlayerEffect((s8)i, 17, 0)].parent = i;
                    break;
                }
                gPlayerStates[i].ability = 0;
                gPlayerStates[i].mode = 22;
            }
            TaskSetEntry(PlayerDance, i);
        }
    }
}

void sub_0805df9c(void)
{
    if (gPlayerCount != 1)
        gCurTask->state = 0;
    else
        gCurTask->state = gPlayerCount;
    TaskSetEntry(sub_0805dba0, gCurTaskIdx);
    gCurTask->player->unk16 = 2;
    TaskStop();
}

void PlayerWalkToDanceSpotUpdate(void)
{
    struct Task *t;
    u16 d;

    t = gCurTask;
    d = t->pixelX - gSpriteCameraX;
    if (t->facing == 1)
    {
        if ((s16)d >= t->unk18)
            sub_0805df9c();
    }
    else
    {
        if ((s16)d <= t->unk18)
            sub_0805df9c();
    }
}

void PlayerSetDanceSpot(s32 a0)
{
    struct PlayerState *ps;
    struct Task *t;
    u16 v;
    s32 w;

    ps = &gPlayerStates[a0];
    t = &gTasks[a0];
    switch (gPlayerCount)
    {
    case 1:
        v = (ps->playerIndex + 1) * 104 + 24;
        break;
    case 2:
        v = (ps->playerIndex + 1) * 69 + 24;
        break;
    case 3:
        v = (ps->playerIndex + 1) * 52 + 24;
        break;
    case 4:
        v = (ps->playerIndex + 1) * 41 + 24;
        break;
    }
    w = (s16)v;
    t->unk18 = w;
    if (gUnk_0300244C == 0)
    {
        if (t->pixelX - w <= 0)
            t->facing = 1;
        else
            t->facing = 255;
    }
    else
    {
        if (t->pixelX - gSpriteCameraX - w <= 0)
            t->facing = 1;
        else
            t->facing = 255;
    }
}

void PlayerWalkToDanceSpot(s32 a0)
{
    struct PlayerState *ps;
    struct Task *t;

    ps = &gPlayerStates[a0];
    t = &gTasks[a0];
    PlayerSuspendControl(a0, 1);
    PlayerSetDanceSpot(a0);
    TaskSetEntry(PlayerActionWalk, a0);
    ps->unk16 = 1;
    t->updateCallback = (u32)PlayerWalkToDanceSpotUpdate;
}

void PlayerDance(void)
{
    struct PlayerState *ps;
    struct Task *t;

    ps = gCurTask->player;
    ps->prevMode = ps->mode;
    gCurTask->player->mode = 22;
    t = gCurTask;
    t->updateCallback = 0;
    t->lateUpdateCallback = 0;
    t->player->bodyBox = 0;
    t->player->terrainBox = 0;
    t->player->hitBoxSet = 0;
    t->speedLimitX = 128 << 24;
    t->unk2C = t->posY;
    t->unk28 = t->posX;
    if (((u8 *)gUnk_02000020)[0] == 1)
    {
        PlayerDanceInGoalGame();
    }
    else
    {
        PlayerDanceAfterStageClear();
        TaskSleepForever();
    }
}

void PlayerDanceInGoalGame(void)
{
    struct Task *t;

    if (*(s8 *)gUnk_02008010 < 0)
        *(s8 *)gUnk_02008010 = RandomRange(7);
    t = gCurTask;
    if (t->unk30 != 0)
    {
        if (gLocalPlayer == t->player->playerIndex)
            PlayBgm(14);
        CallTableEntry(*(s8 *)gUnk_02008010, 14, gPlayerDances);
    }
    else
    {
        if (gLocalPlayer == t->player->playerIndex)
            PlayBgm(13);
        CallTableEntry(*(s8 *)gUnk_02008010 + 7, 14, gPlayerDances);
    }
}

void PlayerDanceAfterStageClear(void)
{
    TaskSetFrameFlip(146);
    TaskYieldTrampoline(30);
    if (*(s8 *)gUnk_02008010 < 0)
    {
        *(s8 *)gUnk_02008010 = RandomRange(7) + 7;
        gCurTask->unk34 = 1;
    }
    else
    {
        gCurTask->unk34 = 0;
    }
    if (*(u8 *)gUnk_020060CC == 0)
    {
        PlayBgm(13);
        *(u8 *)gUnk_020060CC = 1;
    }
    CallTableEntry(*(s8 *)gUnk_02008010, 14, gPlayerDances);
    TaskYieldTrampoline(60);
    if (gCurTask->unk34 != 0)
        ExitClearedStage();
}

void sub_0805e2d4(void)
{
    TaskSetMotion(128 << 9, 0, 0x5A5A5A5A, 144 << 10, 0, 0x5A5A5A5A);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 147 << 1;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFD8000;
    gCurTask->accelY = 128 << 7;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000135;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(4);
    TaskSetMotion(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFFD4000, 128 << 7, 0x5A5A5A5A);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0xFFFB0000, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFC1000;
    gCurTask->accelY = 192 << 6;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x25;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame = 0x2F;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x2C;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x27;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x21;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x25;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x2F;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x2C;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x27;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x21;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(1);
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 9, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFF8800;
    gCurTask->accelX = 192 << 5;
    gCurTask->frame = 0x66;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->velX = 240 << 7;
    gCurTask->accelX = 0xFFFFE800;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 7, 0, 0x5A5A5A5A, 242 << 7, 0xFFFFF500, 0x5A5A5A5A);
    gCurTask->frame = 0x7B;
    TaskYieldTrampoline(4);
    gCurTask->velX = 0;
    TaskYieldTrampoline(15);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->spriteFlags &= 0x7FFF;
    CreatePlayerEffect(gCurTask->player->playerIndex, 16, 0);
    gCurTask->frame = 0x92;
    TaskYieldTrampoline(0x15);
}

void sub_0805e7b4(void)
{
    gCurTask->velY = 144 << 10;
    gCurTask->accelY = 0xFFFF8000;
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 0;
    TaskYieldTrampoline(4);
    gCurTask->frame = 9;
    TaskYieldTrampoline(4);
    TaskSetMotion(152 << 8, 0, 0x5A5A5A5A, 0xFFFBE000, 128 << 7, 0x5A5A5A5A);
    gCurTask->frame = 4;
    TaskYieldTrampoline(2);
    gCurTask->frame = 40;
    TaskYieldTrampoline(10);
    gCurTask->velY = 0xFFF8E000;
    gCurTask->frame = 5;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFF2000;
    TaskYieldTrampoline(3);
    gCurTask->velY = 188 << 11;
    gCurTask->spriteFlags &= 0x7FFF;
    TaskYieldTrampoline(1);
    gCurTask->velY = 128 << 6;
    TaskYieldTrampoline(3);
    gCurTask->frame = 40;
    TaskYieldTrampoline(12);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(4);
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags |= 128 << 8;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFC8000;
    gCurTask->frame = 97;
    TaskYieldTrampoline(8);
    gCurTask->accelX = 160 << 8;
    TaskYieldTrampoline(4);
    gCurTask->velX = 0xFFFF8000;
    gCurTask->accelX = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0;
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags &= 0x7FFF;
    TaskYieldTrampoline(1);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velX = 128 << 10;
    gCurTask->frame = 97;
    TaskYieldTrampoline(8);
    gCurTask->accelX = 0xFFFF6000;
    TaskYieldTrampoline(4);
    gCurTask->velX = 0xFFFF8000;
    gCurTask->accelX = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFE0000, 128 << 9, 0x5A5A5A5A);
    gCurTask->frame = 40;
    TaskYieldTrampoline(3);
    TaskStop();
    TaskYieldTrampoline(2);
    gCurTask->velX = 192 << 8;
    gCurTask->accelX = 0xFFFFD000;
    gCurTask->frame = 102;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->velX = 0xFFFE8000;
    gCurTask->accelX = 192 << 5;
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 17;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 111;
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0xFFFEA000, 128 << 7, 0x5A5A5A5A);
    gCurTask->frame = 40;
    TaskYieldTrampoline(10);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0xFFFE6000, 128 << 7, 0x5A5A5A5A);
    gCurTask->frame = 40;
    TaskYieldTrampoline(12);
    TaskSetMotion(128 << 7, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(7);
    TaskStop();
    gCurTask->spriteFlags |= 128 << 8;
    CreatePlayerEffect(gCurTask->player->playerIndex, 16, 0);
    gCurTask->frame = 146;
    TaskYieldTrampoline(21);
}

void sub_0805eb2c(s32 a0, s32 a1, s32 a2)
{
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0xFFFD4000, 128 << 8, 0x5A5A5A5A);
    gCurTask->frame = 0x133;
    TaskYieldTrampoline(10);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 144 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gCurTask->frame = 123;
    TaskYieldTrampoline(8);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(3);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0xFFFD4000, 128 << 8, 0x5A5A5A5A);
    gCurTask->frame = 0x133;
    TaskYieldTrampoline(10);
    gCurTask->spriteFlags |= 128 << 8;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 144 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gCurTask->frame = 123;
    TaskYieldTrampoline(8);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(3);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->velX = 0xFFF80000;
    gCurTask->accelX = 128 << 9;
    gCurTask->frame = 100;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    TaskYieldTrampoline(2);
    TaskSetMotion(192 << 12, 0xFFFFD000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 30;
    TaskYieldTrampoline(1);
    gCurTask->velX = 180 << 10;
    TaskYieldTrampoline(15);
    gCurTask->velX = 128 << 7;
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->frame = 32;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0xFFFE0000, 128 << 7, 0x5A5A5A5A);
    gCurTask->frame = 40;
    TaskYieldTrampoline(15);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(4);
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0xFFFE0000, 128 << 7, 0x5A5A5A5A);
    gCurTask->frame = 40;
    TaskYieldTrampoline(15);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(4);
    TaskSetMotion(240 << 8, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->frame = 32;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->spriteFlags |= 128 << 8;
    CreatePlayerEffect(gCurTask->player->playerIndex, 16, 0);
    gCurTask->frame = 146;
    TaskYieldTrampoline(21);
}

void sub_0805ee90(void)
{
    gCurTask->velY = 144 << 9;
    gCurTask->accelY = 0xFFFFC000;
    gCurTask->frame = 150;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFC8000;
    gCurTask->accelY = 128 << 7;
    gCurTask->frame = 0x133;
    TaskYieldTrampoline(2);
    gCurTask->frame = 151;
    TaskYieldTrampoline(12);
    gCurTask->frame = 0x133;
    TaskYieldTrampoline(3);
    gCurTask->velY = 144 << 9;
    gCurTask->accelY = 0xFFFFC000;
    gCurTask->frame = 150;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFC8000;
    gCurTask->accelY = 128 << 7;
    gCurTask->frame = 0x133;
    TaskYieldTrampoline(2);
    gCurTask->frame = 151;
    TaskYieldTrampoline(12);
    gCurTask->accelY = 192 << 6;
    gCurTask->frame = 0x133;
    TaskYieldTrampoline(3);
    gCurTask->velY = 208 << 8;
    gCurTask->accelY = 128 << 6;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->velY = 192 << 11;
    gCurTask->accelY = 0;
    gCurTask->frame = 310;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFB2000;
    gCurTask->accelY = 0xFFFFC000;
    gCurTask->frame = 0x133 - 8;
    TaskYieldTrampoline(1);
    gCurTask->velY = 224 << 8;
    TaskYieldTrampoline(3);
    gCurTask->velY = 0xFFFDA000;
    gCurTask->accelY = 128 << 7;
    gCurTask->frame = 151 + 161;
    TaskYieldTrampoline(14);
    gCurTask->frame = 0x133 - 8;
    TaskYieldTrampoline(3);
    gCurTask->velY = 192 << 11;
    gCurTask->accelY = 0;
    gCurTask->frame = 310;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFB2000;
    gCurTask->accelY = 0xFFFF8000;
    gCurTask->frame = 0x133 - 8;
    TaskYieldTrampoline(1);
    gCurTask->velY = 224 << 8;
    TaskYieldTrampoline(3);
    gCurTask->velY = 0xFFFBC000;
    gCurTask->accelY = 128 << 7;
    gCurTask->frame = 151 + 161;
    TaskYieldTrampoline(14);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 0x139;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->frame = 106;
    TaskYieldTrampoline(2);
    gCurTask->velY = 128 << 10;
    gCurTask->accelY = 0xFFFF0000;
    TaskYieldTrampoline(3);
    TaskStopY();
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFEA000;
    gCurTask->accelX = 128 << 6;
    TaskYieldTrampoline(4);
    gCurTask->spriteFlags &= 0x7FFF;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame = 17;
    TaskYieldTrampoline(1);
    gCurTask->frame = 111;
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->spriteFlags |= 128 << 8;
    CreatePlayerEffect(gCurTask->player->playerIndex, 16, 0);
    gCurTask->frame = 146;
    TaskYieldTrampoline(21);
}

void sub_0805f1bc(void)
{
    gCurTask->velX = 128 << 9;
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 20;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->velX = 128 << 8;
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->velX = 128 << 7;
    gCurTask->frame--;
    TaskYieldTrampoline(10);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->velX = 128 << 9;
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->velX = 128 << 7;
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(10);
    gCurTask->velX = 176 << 8;
    gCurTask->accelX = 0xFFFFF000;
    gCurTask->frame = 102;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 17;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 111;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 110;
    TaskYieldTrampoline(2);
    gCurTask->velX = 128 << 9;
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 102;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 17;
    TaskYieldTrampoline(1);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 111;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFF38000;
    gCurTask->accelX = 0xFFFE8000;
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 0x10F7;
    TaskYieldTrampoline(1);
    gCurTask->velX = 160 << 10;
    TaskYieldTrampoline(1);
    gCurTask->velX = 140 << 13;
    gCurTask->frame = 104;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame = 107;
    TaskYieldTrampoline(2);
    gCurTask->velX = 200 << 12;
    gCurTask->accelX = 128 << 8;
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 0x10F7;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFFD8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFEE8000;
    gCurTask->velY = 128 << 10;
    gCurTask->frame = 37;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    gCurTask->velY = 0;
    TaskYieldTrampoline(1);
    gCurTask->spriteFlags |= 128 << 8;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFF38000, 0xFFFE8000, 0x5A5A5A5A, 0xFFFE0000, 0, 0x5A5A5A5A);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 0x10F7;
    TaskYieldTrampoline(1);
    gCurTask->velX = 160 << 10;
    gCurTask->velY = 0;
    TaskYieldTrampoline(1);
    gCurTask->velX = 140 << 13;
    gCurTask->frame = 104;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame = 107;
    TaskYieldTrampoline(2);
    gCurTask->velX = 200 << 12;
    gCurTask->accelX = 128 << 8;
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 0x10F7;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFFD8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFEE8000;
    gCurTask->velY = 128 << 10;
    gCurTask->frame = 37;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    gCurTask->velY = 0;
    TaskYieldTrampoline(1);
    gCurTask->spriteFlags |= 128 << 8;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFF38000, 0xFFFE8000, 0x5A5A5A5A, 0xFFFE0000, 0, 0x5A5A5A5A);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 0x10F7;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFFD8000;
    gCurTask->velY = 0;
    TaskYieldTrampoline(1);
    gCurTask->velX = 140 << 13;
    gCurTask->frame = 104;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame = 37;
    TaskYieldTrampoline(2);
    gCurTask->velX = 200 << 12;
    gCurTask->accelX = 128 << 8;
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 0x10F7;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFFD8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFEE8000;
    gCurTask->velY = 128 << 10;
    gCurTask->frame = 37;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    gCurTask->velY = 0;
    TaskYieldTrampoline(1);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 37;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0xFFFC6000, 128 << 7, 0x5A5A5A5A);
    gCurTask->frame = 31;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFD9000;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 128 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(7);
    TaskStop();
    gCurTask->spriteFlags |= 128 << 8;
    CreatePlayerEffect(gCurTask->player->playerIndex, 16, 0);
    gCurTask->frame = 146;
    TaskYieldTrampoline(21);
}

void sub_0805f778(void)
{
    gCurTask->velX = 0xFFFD8000;
    gCurTask->accelX = 128 << 7;
    gCurTask->frame = 20;
    TaskYieldTrampoline(5);
    gCurTask->velX = 0xFFFF0000;
    gCurTask->accelX = 128 << 5;
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(10);
    gCurTask->velX = 0xFFFF8000;
    gCurTask->accelX = 0;
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->velX = 160 << 10;
    gCurTask->accelX = 0xFFFFC000;
    gCurTask->frame = 18;
    TaskYieldTrampoline(5);
    gCurTask->velX = 128 << 9;
    gCurTask->accelX = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->velX = 128 << 8;
    gCurTask->accelX = 0;
    gCurTask->frame = 12;
    TaskYieldTrampoline(10);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFFE000, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0xFFFAA000, 128 << 7, 0x5A5A5A5A);
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame = 37;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 7, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(6);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 224 << 8, 0xFFFFE000, 0x5A5A5A5A);
    gCurTask->frame = 123;
    TaskYieldTrampoline(13);
    gCurTask->spriteFlags |= 128 << 8;
    TaskStopY();
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->spriteFlags |= 128 << 8;
    CreatePlayerEffect(gCurTask->player->playerIndex, 16, 0);
    gCurTask->frame = 146;
    TaskYieldTrampoline(21);
}

void sub_0805fb88(void)
{
    TaskSetMotion(204 << 9, 0xFFFFE800, 0x5A5A5A5A, 0xFFFE7800, 224 << 6, 0x5A5A5A5A);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 13;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->velX = 192 << 6;
    gCurTask->accelX = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->velY = 0;
    gCurTask->accelY = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    TaskSetMotion(204 << 9, 0xFFFFE800, 0x5A5A5A5A, 0xFFFE7800, 224 << 6, 0x5A5A5A5A);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->velX = 192 << 6;
    gCurTask->accelX = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->velY = 0;
    gCurTask->accelY = 0;
    gCurTask->frame = 12;
    TaskYieldTrampoline(8);
    gCurTask->velX = 168 << 9;
    gCurTask->accelX = 0xFFFFE800;
    gCurTask->frame = 102;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->velX = 192 << 6;
    gCurTask->accelX = 0;
    TaskYieldTrampoline(2);
    TaskSetMotion(192 << 6, 0, 0x5A5A5A5A, 0xFFFD6000, 192 << 7, 0x5A5A5A5A);
    TaskSetFrameFlip(17);
    TaskYieldTrampoline(1);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 111;
    TaskYieldTrampoline(1);
    gCurTask->frame = 102;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetMotion(192 << 6, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->frame = 40;
    TaskYieldTrampoline(11);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF2800, 0, 0x5A5A5A5A, 0xFFFB8000, 192 << 6, 0x5A5A5A5A);
    gCurTask->frame = 31;
    TaskYieldTrampoline(14);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFFA000;
    gCurTask->accelX = 0xFFFFF400;
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFFF2800;
    gCurTask->frame = 40;
    TaskYieldTrampoline(3);
    gCurTask->velX = 0xFFFFA000;
    gCurTask->frame = 32;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFF7000;
    gCurTask->frame = 40;
    TaskYieldTrampoline(7);
    TaskYieldTrampoline(5);
    TaskSetMotion(0xFFFFD000, 0, 0x5A5A5A5A, 224 << 7, 0xFFFFF000, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFF4000;
    gCurTask->accelX = 192 << 5;
    gCurTask->frame = 123;
    TaskYieldTrampoline(6);
    gCurTask->velX = 0;
    gCurTask->accelX = 0;
    TaskYieldTrampoline(5);
    TaskStop();
    gCurTask->frame = 5;
    TaskYieldTrampoline(3);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 146;
    TaskYieldTrampoline(19);
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFE2000;
    gCurTask->accelX = 240 << 4;
    gCurTask->frame = 97;
    TaskYieldTrampoline(19);
    gCurTask->velX = 0;
    gCurTask->accelX = 0;
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFE8000;
    gCurTask->accelX = 144 << 4;
    gCurTask->frame = 97;
    TaskYieldTrampoline(11);
    gCurTask->velX = 0xFFFFA000;
    gCurTask->frame = 123;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0xFFFE8600;
    gCurTask->accelX = 216 << 6;
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->velY = 160 << 11;
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 154 << 1;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame = 309;
    TaskYieldTrampoline(4);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFDC000, 192 << 7, 0x5A5A5A5A);
    gCurTask->frame = 147 << 1;
    TaskYieldTrampoline(1);
    gCurTask->frame = 309;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0;
    gCurTask->accelY = 0;
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->velX = 144 << 9;
    gCurTask->accelX = 192 << 4;
    gCurTask->frame = 149 << 1;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame = 309;
    TaskYieldTrampoline(2);
    gCurTask->velX = 216 << 8;
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->velX = 144 << 8;
    gCurTask->frame--;
    TaskYieldTrampoline(4);
    TaskSetMotion(144 << 7, 0, 0x5A5A5A5A, 0xFFFB0000, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0;
    TaskYieldTrampoline(1);
    gCurTask->velX = 144 << 7;
    gCurTask->frame = 102;
    TaskYieldTrampoline(7);
    gCurTask->velY = 0;
    gCurTask->accelY = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->accelX = 192 << 3;
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 16;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 111;
    TaskYieldTrampoline(2);
    TaskSetMotion(144 << 8, 0xFFFFEE00, 0x5A5A5A5A, 242 << 7, 0xFFFFF500, 0x5A5A5A5A);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 123;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0;
    gCurTask->accelX = 0;
    TaskYieldTrampoline(11);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->spriteFlags |= 128 << 8;
    CreatePlayerEffect(gCurTask->player->playerIndex, 16, 0);
    gCurTask->frame = 146;
    TaskYieldTrampoline(21);
}

void sub_08060308(void)
{
    TaskSetMotion(0xFFFF8C00, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 15;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0xFFFF4000;
    gCurTask->accelY = 128 << 6;
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(7);
    gCurTask->velX = 0xFFFE8000;
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(7);
    gCurTask->frame = 19;
    TaskYieldTrampoline(1);
    TaskSetMotion(0xFFFF8C00, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 20;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0xFFFF4000;
    gCurTask->accelY = 128 << 6;
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame = 12;
    TaskYieldTrampoline(7);
    gCurTask->velX = 0xFFFE8000;
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(7);
    gCurTask->frame = 14;
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame = 111;
    TaskYieldTrampoline(3);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 111;
    TaskYieldTrampoline(3);
    gCurTask->velX = 128 << 8;
    gCurTask->frame = 102;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame = 108;
    TaskYieldTrampoline(3);
    gCurTask->velX = 128 << 9;
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->velX = 192 << 9;
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 111;
    TaskYieldTrampoline(3);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 111;
    TaskYieldTrampoline(3);
    gCurTask->velX = 128 << 8;
    gCurTask->frame = 148;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFCA000, 128 << 6, 0x5A5A5A5A);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame = 172;
    TaskYieldTrampoline(2);
    gCurTask->frame = 151;
    TaskYieldTrampoline(2);
    gCurTask->frame = 172;
    TaskYieldTrampoline(2);
    gCurTask->frame = 151;
    TaskYieldTrampoline(2);
    gCurTask->frame = 172;
    TaskYieldTrampoline(2);
    gCurTask->frame = 151;
    TaskYieldTrampoline(5);
    gCurTask->frame = 0x00000133;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame = 147 << 1;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(11);
    gCurTask->velY = 160 << 12;
    gCurTask->accelY = 0xFFFF0000;
    gCurTask->frame = 155 << 1;
    TaskYieldTrampoline(1);
    gCurTask->velY = 192 << 10;
    TaskYieldTrampoline(6);
    gCurTask->velY = 0xFFFE0000;
    gCurTask->accelY = 0;
    gCurTask->frame = 0x0000012B;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0xFFFD0000;
    gCurTask->frame = 156 << 1;
    TaskYieldTrampoline(1);
    TaskStop();
    TaskYieldTrampoline(18);
    gCurTask->frame = 0x0000012B;
    TaskYieldTrampoline(2);
    gCurTask->velY = 160 << 11;
    gCurTask->frame = 0x00000137;
    TaskYieldTrampoline(1);
    TaskStop();
    TaskYieldTrampoline(1);
    TaskSetMotion(192 << 8, 0, 0x5A5A5A5A, 0xFFF7C000, 128 << 7, 0x5A5A5A5A);
    gCurTask->frame = 149 << 1;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000135;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->spriteFlags |= 128 << 8;
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(212 << 8, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF4000, 0, 0x5A5A5A5A, 0xFFFD5000, 128 << 6, 0x5A5A5A5A);
    gCurTask->frame = 31;
    TaskYieldTrampoline(21);
    gCurTask->frame = 111;
    TaskYieldTrampoline(3);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 111;
    TaskYieldTrampoline(3);
    gCurTask->frame = 41;
    TaskYieldTrampoline(2);
    gCurTask->frame = 40;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFFF8000;
    gCurTask->frame = 102;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFFE8000;
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFFF8000;
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->frame = 111;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->velX = 128 << 8;
    gCurTask->frame = 111;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velX = 128 << 8;
    gCurTask->frame = 111;
    TaskYieldTrampoline(3);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->velX = 0xFFFF8000;
    gCurTask->frame = 111;
    TaskYieldTrampoline(3);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->velX = 0xFFFF0000;
    gCurTask->frame = 109;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFFF8000;
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->velX = 128 << 8;
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->velX = 128 << 9;
    gCurTask->frame--;
    TaskYieldTrampoline(1);
    gCurTask->velX = 128 << 8;
    gCurTask->frame = 111;
    TaskYieldTrampoline(1);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->velX = 0xFFFF8000;
    gCurTask->frame = 111;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->velX = 0xFFFF0000;
    gCurTask->frame = 109;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->velX = 128 << 8;
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->velX = 128 << 9;
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->velX = 128 << 7;
    gCurTask->frame = 14;
    TaskYieldTrampoline(2);
    gCurTask->velY = 128 << 9;
    gCurTask->frame = 150;
    TaskYieldTrampoline(1);
    gCurTask->velY = 128 << 8;
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0;
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0;
    gCurTask->velY = 0xFFFE1600;
    gCurTask->frame = 151;
    TaskYieldTrampoline(1);
    gCurTask->velY = 176 << 5;
    TaskYieldTrampoline(1);
    gCurTask->frame = 172;
    TaskYieldTrampoline(2);
    gCurTask->frame = 151;
    TaskYieldTrampoline(2);
    gCurTask->frame = 172;
    TaskYieldTrampoline(2);
    gCurTask->frame = 151;
    TaskYieldTrampoline(2);
    gCurTask->frame = 172;
    TaskYieldTrampoline(2);
    gCurTask->velY = 248 << 6;
    gCurTask->frame = 151;
    TaskYieldTrampoline(2);
    gCurTask->frame = 172;
    TaskYieldTrampoline(2);
    gCurTask->velY = 128 << 8;
    gCurTask->frame = 148;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0xFFFD0000;
    gCurTask->spriteFlags |= 128 << 8;
    CreatePlayerEffect(gCurTask->player->playerIndex, 16, 0);
    gCurTask->frame = 146;
    TaskYieldTrampoline(1);
    TaskStop();
    TaskYieldTrampoline(20);
}

void sub_08060c2c(void)
{
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->velX = 0xFFFFC000;
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF8000, 128 << 3, 0x5A5A5A5A, 0xFFFE0800, 224 << 6, 0x5A5A5A5A);
    gCurTask->frame = 40;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFFC000;
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF4000, 192 << 3, 0x5A5A5A5A, 0xFFFE0800, 224 << 6, 0x5A5A5A5A);
    gCurTask->frame = 40;
    TaskYieldTrampoline(3);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 109;
    TaskYieldTrampoline(2);
    gCurTask->frame = 107;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 16;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0;
    gCurTask->frame = 15;
    TaskYieldTrampoline(2);
    gCurTask->velX = 128 << 7;
    gCurTask->accelX = 160 << 3;
    gCurTask->frame = 10;
    TaskYieldTrampoline(14);
    gCurTask->velY = 0xFFFE0000;
    gCurTask->accelY = 128 << 9;
    gCurTask->frame = 10;
    TaskYieldTrampoline(3);
    TaskSetMotion(128 << 9, 0, 0x5A5A5A5A, 128 << 10, 0, 0x5A5A5A5A);
    gCurTask->frame = 38;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0;
    gCurTask->accelY = 0;
    gCurTask->frame = 33;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0xFFFC0000;
    gCurTask->frame = 43;
    TaskYieldTrampoline(1);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 43;
    TaskYieldTrampoline(1);
    gCurTask->velX = 128 << 7;
    gCurTask->velY = 128 << 11;
    gCurTask->frame = 42;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFE0000;
    gCurTask->accelY = 128 << 9;
    gCurTask->frame = 42;
    TaskYieldTrampoline(3);
    gCurTask->velY = 0xFFFE0000;
    gCurTask->accelY = 128 << 9;
    gCurTask->frame = 42;
    TaskYieldTrampoline(3);
    gCurTask->velY = 0xFFFE0000;
    gCurTask->accelY = 128 << 9;
    gCurTask->frame = 42;
    TaskYieldTrampoline(3);
    gCurTask->velX = 128 << 7;
    gCurTask->velY = 0;
    gCurTask->accelY = 0;
    gCurTask->frame = 42;
    TaskYieldTrampoline(3);
    gCurTask->velY = 0xFFFC0000;
    gCurTask->frame = 43;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0;
    gCurTask->frame = 43;
    TaskYieldTrampoline(1);
    gCurTask->velX = 128 << 9;
    gCurTask->velY = 0xFFFD8000;
    gCurTask->accelY = 128 << 7;
    gCurTask->frame = 44;
    TaskYieldTrampoline(2);
    gCurTask->frame = 46;
    TaskYieldTrampoline(4);
    gCurTask->frame = 47;
    TaskYieldTrampoline(2);
    gCurTask->frame = 36;
    TaskYieldTrampoline(2);
    gCurTask->frame = 49;
    TaskYieldTrampoline(2);
    gCurTask->frame = 37;
    TaskYieldTrampoline(2);
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->frame = 32;
    TaskYieldTrampoline(3);
    TaskSetMotion(128 << 7, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 43;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFF6000;
    gCurTask->velY = 0xFFFD4000;
    gCurTask->accelY = 128 << 7;
    gCurTask->frame = 42;
    TaskYieldTrampoline(6);
    gCurTask->frame = 33;
    TaskYieldTrampoline(2);
    gCurTask->frame = 32;
    TaskYieldTrampoline(2);
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->frame = 41;
    TaskYieldTrampoline(2);
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->frame = 41;
    TaskYieldTrampoline(2);
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->frame = 40;
    TaskYieldTrampoline(1);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(14);
    gCurTask->velX = 0;
    gCurTask->frame = 5;
    TaskYieldTrampoline(5);
    gCurTask->velX = 0xFFFA0000;
    gCurTask->frame = 100;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    gCurTask->frame = 100;
    TaskYieldTrampoline(1);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->velX = 192 << 11;
    gCurTask->frame = 146;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    gCurTask->frame = 146;
    TaskYieldTrampoline(20);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->velY = 160 << 9;
    gCurTask->accelY = 0xFFFFC000;
    gCurTask->frame = 123;
    TaskYieldTrampoline(9);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0xFFFE0000;
    gCurTask->accelY = 128 << 8;
    gCurTask->frame = 146;
    TaskYieldTrampoline(7);
    gCurTask->velY = 0;
    gCurTask->accelY = 0;
    gCurTask->frame = 146;
    TaskYieldTrampoline(3);
    gCurTask->spriteFlags |= 128 << 8;
    TaskSetMotion(0xFFFFE000, 0, 0x5A5A5A5A, 160 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gCurTask->frame = 123;
    TaskYieldTrampoline(9);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0xFFFE0000;
    gCurTask->accelY = 128 << 8;
    gCurTask->frame = 146;
    TaskYieldTrampoline(7);
    gCurTask->velY = 0;
    gCurTask->accelY = 0;
    gCurTask->frame = 146;
    TaskYieldTrampoline(3);
    gCurTask->spriteFlags &= 0x7FFF;
    TaskSetMotion(128 << 6, 0, 0x5A5A5A5A, 160 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gCurTask->frame = 123;
    TaskYieldTrampoline(9);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->accelX = 192 << 4;
    gCurTask->velY = 0xFFFDF800;
    gCurTask->accelY = 160 << 6;
    gCurTask->frame = 146;
    TaskYieldTrampoline(4);
    gCurTask->frame = 31;
    TaskYieldTrampoline(4);
    gCurTask->frame = 32;
    TaskYieldTrampoline(2);
    gCurTask->frame = 33;
    TaskYieldTrampoline(2);
    gCurTask->frame = 34;
    TaskYieldTrampoline(2);
    gCurTask->frame = 35;
    TaskYieldTrampoline(2);
    gCurTask->frame = 36;
    TaskYieldTrampoline(2);
    gCurTask->frame = 37;
    TaskYieldTrampoline(2);
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->frame = 41;
    TaskYieldTrampoline(3);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFF2000, 128 << 7, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(6);
    gCurTask->spriteFlags |= 128 << 8;
    gCurTask->velY = 0;
    gCurTask->accelY = 0;
    gCurTask->frame = 15;
    TaskYieldTrampoline(2);
    gCurTask->frame = 14;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFFC000;
    gCurTask->accelX = 0xFFFFFB00;
    gCurTask->frame = 10;
    TaskYieldTrampoline(14);
    gCurTask->accelX = 0;
    gCurTask->velY = 0xFFFE0000;
    gCurTask->accelY = 128 << 9;
    gCurTask->frame = 10;
    TaskYieldTrampoline(3);
    TaskSetMotion(0xFFFF4000, 0, 0x5A5A5A5A, 128 << 10, 0, 0x5A5A5A5A);
    gCurTask->frame = 38;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0;
    gCurTask->accelY = 0;
    gCurTask->frame = 33;
    TaskYieldTrampoline(1);
    gCurTask->frame = 43;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFFFB800;
    gCurTask->frame = 42;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFE0000;
    gCurTask->accelY = 128 << 9;
    gCurTask->frame = 42;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0xFFFE0000;
    gCurTask->accelY = 136 << 7;
    gCurTask->frame = 46;
    TaskYieldTrampoline(2);
    gCurTask->frame = 36;
    TaskYieldTrampoline(2);
    gCurTask->frame = 37;
    TaskYieldTrampoline(2);
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    gCurTask->frame = 41;
    TaskYieldTrampoline(2);
    gCurTask->frame = 40;
    TaskYieldTrampoline(2);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velY = 132 << 8;
    gCurTask->accelY = 0xFFFFF400;
    gCurTask->frame = 5;
    TaskYieldTrampoline(19);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    CreatePlayerEffect(gCurTask->player->playerIndex, 16, 0);
    gCurTask->frame = 146;
    TaskYieldTrampoline(21);
}

void sub_080613e4(void)
{
    gCurTask->spriteFlags |= 0x8000;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags &= 0x7FFF;
    TaskSetMotion(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFFDC000, 0x4000, 0x5A5A5A5A);
    gCurTask->frame = 0x134;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000135;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x126;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000127;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x128;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000129;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x12A;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x0000012B;
    TaskYieldTrampoline(3);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0x60000, 0, 0x5A5A5A5A);
    gCurTask->frame = 0x00000137;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0x20000;
    gCurTask->accelY = 0xFFFF0000;
    gCurTask->frame = 0x00000137;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    gCurTask->frame = 0x00000137;
    TaskYieldTrampoline(2);
    TaskSetMotion(0x10000, 0, 0x5A5A5A5A, 0xFFF7C000, 0x4000, 0x5A5A5A5A);
    gCurTask->frame = 0x0000012B;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFE0000;
    gCurTask->frame = 0x0000012B;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x12A;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000129;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x128;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000127;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x126;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000135;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x134;
    TaskYieldTrampoline(3);
    TaskSetMotion(0x4000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x5800;
    gCurTask->accelX = 0xFFFFF800;
    gCurTask->frame = 5;
    TaskYieldTrampoline(5);
    gCurTask->frame = 0x10;
    TaskYieldTrampoline(3);
    gCurTask->frame = 0x66;
    TaskYieldTrampoline(3);
    gCurTask->frame = 0x67;
    TaskYieldTrampoline(3);
    gCurTask->velY = 0x6000;
    gCurTask->accelY = 0xFFFFE000;
    gCurTask->frame = 0x68;
    TaskYieldTrampoline(3);
    gCurTask->frame = 0x69;
    TaskYieldTrampoline(2);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFE5000, 0x3000, 0x5A5A5A5A);
    gCurTask->frame = 0x5A;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x58;
    TaskYieldTrampoline(15);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(6);
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    TaskSetMotion(0x10000, 0, 0x5A5A5A5A, 0xFFFDC000, 0x4000, 0x5A5A5A5A);
    gCurTask->frame = 0x132;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000131;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x130;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x12000;
    gCurTask->frame = 0x0000012F;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x12E;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x0000012D;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x12C;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x0000012B;
    TaskYieldTrampoline(3);
    TaskSetMotion(0x4000, 0, 0x5A5A5A5A, 0x60000, 0, 0x5A5A5A5A);
    gCurTask->frame = 0x136;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0x20000;
    gCurTask->accelY = 0xFFFF0000;
    gCurTask->frame = 0x136;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    gCurTask->frame = 0x136;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFF7C000, 0x4000, 0x5A5A5A5A);
    gCurTask->frame = 0x0000012B;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFE0000;
    gCurTask->frame = 0x0000012B;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x12C;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x0000012D;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x12E;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x0000012F;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x130;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000131;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x132;
    TaskYieldTrampoline(3);
    gCurTask->spriteFlags |= 0x8000;
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFFA800;
    gCurTask->accelX = 0x800;
    gCurTask->frame = 5;
    TaskYieldTrampoline(5);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 0x6C;
    TaskYieldTrampoline(3);
    gCurTask->frame = 0x6D;
    TaskYieldTrampoline(3);
    gCurTask->frame = 0x6E;
    TaskYieldTrampoline(3);
    gCurTask->spriteFlags |= 0x8000;
    gCurTask->velY = 0x6000;
    gCurTask->accelY = 0xFFFFE800;
    gCurTask->frame = 0x6F;
    TaskYieldTrampoline(5);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFE4800, 0x2800, 0x5A5A5A5A);
    gCurTask->frame = 0x92;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x92;
    TaskYieldTrampoline(0x13);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0xFFFDC000;
    gCurTask->accelY = 0x4000;
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(1);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0xFFFDC000;
    gCurTask->accelY = 0x4000;
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(1);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags |= 0x8000;
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0xFFFB8000;
    gCurTask->accelY = 0x4000;
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(10);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->velY = 0xFFFF7000;
    gCurTask->accelY = 0x1800;
    gCurTask->frame = 0x00000133;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x00000135;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000127;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000129;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x0000012B;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x0000012D;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0;
    gCurTask->accelY = 0x3000;
    gCurTask->frame = 0x0000012F;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000131;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000133;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x134;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000135;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x126;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000127;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x128;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x00000129;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x60000;
    gCurTask->accelY = 0;
    gCurTask->frame = 0x136;
    TaskYieldTrampoline(1);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 0x136;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0xFFFD3000;
    gCurTask->accelY = 0x3000;
    gCurTask->frame = 0x0000012B;
    TaskYieldTrampoline(9);
    gCurTask->frame = 0x00000139;
    TaskYieldTrampoline(3);
    gCurTask->frame = 0x13A;
    TaskYieldTrampoline(3);
    gCurTask->frame = 0x0000013B;
    TaskYieldTrampoline(3);
    gCurTask->frame = 0x13C;
    TaskYieldTrampoline(3);
    gCurTask->frame = 0x0000013D;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFC0000;
    gCurTask->accelY = 0xFFFE8000;
    gCurTask->frame = 0x6A;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0x18000;
    gCurTask->frame = 0x6A;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0;
    gCurTask->accelY = 0;
    gCurTask->frame = 0x69;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x68;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x67;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x66;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags |= 0x8000;
    gCurTask->frame = 0x6E;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 0x6F;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags |= 0x8000;
    gCurTask->frame = 0x11;
    TaskYieldTrampoline(2);
    gCurTask->frame = 5;
    TaskYieldTrampoline(15);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->spriteFlags |= 0x8000;
    CreatePlayerEffect(gCurTask->player->playerIndex, 16, 0);
    gCurTask->frame = 0x92;
    TaskYieldTrampoline(0x15);
}

void sub_08061cac(void)
{
    gCurTask->spriteFlags &= 0x7FFF;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFB0000, 0, 0x5A5A5A5A, 0xFFFD0000, 0x10000, 0x5A5A5A5A);
    gCurTask->frame = 0x64;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    gCurTask->frame = 0x64;
    TaskYieldTrampoline(4);
    TaskSetMotion(0x80000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0x20000;
    gCurTask->accelX = 0xFFFFC000;
    gCurTask->frame = 5;
    TaskYieldTrampoline(7);
    TaskSetMotion(0x4000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(4);
    gCurTask->frame = 0x13;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0;
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFB0000, 0, 0x5A5A5A5A, 0xFFFD0000, 0x10000, 0x5A5A5A5A);
    gCurTask->frame = 0x64;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    gCurTask->frame = 0x64;
    TaskYieldTrampoline(4);
    TaskSetMotion(0x80000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0x20000;
    gCurTask->accelX = 0xFFFFC000;
    gCurTask->frame = 5;
    TaskYieldTrampoline(7);
    TaskSetMotion(0x4000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(4);
    gCurTask->frame = 0x13;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFF0000;
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFA0000, 0, 0x5A5A5A5A, 0xFFFB8000, 0x8000, 0x5A5A5A5A);
    gCurTask->frame = 0x64;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFFF0000;
    gCurTask->frame = 0x64;
    TaskYieldTrampoline(4);
    gCurTask->velX = 0x40000;
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFFF0000;
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    TaskSetMotion(0x8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF0000, 0x1000, 0x5A5A5A5A, 0xFFFE8000, 0x4000, 0x5A5A5A5A);
    gCurTask->frame = 0x1E;
    TaskYieldTrampoline(11);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 0x1E;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x4000;
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFFDC000, 0x4000, 0x5A5A5A5A);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x14000;
    gCurTask->frame = 0x00000133;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFF8000;
    gCurTask->frame = 0x00000133;
    TaskYieldTrampoline(5);
    gCurTask->spriteFlags |= 0x8000;
    gCurTask->velY = 0xFFFEC000;
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0x8000;
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0xFFFFC000;
    gCurTask->frame = 5;
    TaskYieldTrampoline(1);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x7B;
    TaskYieldTrampoline(2);
    TaskSetMotion(0x10000, 0, 0x5A5A5A5A, 0xFFFDC000, 0x4000, 0x5A5A5A5A);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->velY = 0x14000;
    gCurTask->frame = 0x00000133;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFF8000;
    gCurTask->frame = 0x00000133;
    TaskYieldTrampoline(5);
    gCurTask->velY = 0xFFFEC000;
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0x8000;
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0x4000;
    gCurTask->frame = 5;
    TaskYieldTrampoline(1);
    TaskSetMotion(0x4000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x7B;
    TaskYieldTrampoline(2);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFA8000, 0x8000, 0x5A5A5A5A);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x66;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x68;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x6B;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x6D;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags |= 0x8000;
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->velY = 0x38000;
    gCurTask->frame = 0x00000133;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0x20000;
    gCurTask->frame = 0x00000133;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x10000;
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0x38000;
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x20000;
    gCurTask->accelY = 0xFFFF0000;
    gCurTask->frame = 5;
    TaskYieldTrampoline(3);
    gCurTask->velY = 0x20000;
    gCurTask->accelY = 0;
    gCurTask->frame = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFE0000;
    gCurTask->frame = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0x20000;
    gCurTask->frame = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFE0000;
    gCurTask->frame = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0x10000;
    gCurTask->frame = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFF0000;
    gCurTask->frame = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0x10000;
    gCurTask->frame = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFF0000;
    gCurTask->frame = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->velY = 0;
    gCurTask->frame = 0x7B;
    TaskYieldTrampoline(10);
    gCurTask->spriteFlags |= 0x8000;
    gCurTask->velX = 0x20000;
    gCurTask->accelX = 0xFFFFF000;
    gCurTask->frame = 0x12;
    TaskYieldTrampoline(6);
    gCurTask->velX = 0x10000;
    gCurTask->frame = 0x11;
    TaskYieldTrampoline(6);
    gCurTask->velX = 0x10000;
    gCurTask->frame = 0x10;
    TaskYieldTrampoline(4);
    gCurTask->velX = 0x8000;
    gCurTask->frame = 15;
    TaskYieldTrampoline(3);
    gCurTask->velX = 0;
    gCurTask->frame = 14;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x20000;
    gCurTask->accelX = 0xFFFFF000;
    gCurTask->frame = 13;
    TaskYieldTrampoline(6);
    gCurTask->velX = 0x10000;
    gCurTask->frame = 12;
    TaskYieldTrampoline(6);
    gCurTask->velX = 0x10000;
    gCurTask->frame = 0x15;
    TaskYieldTrampoline(4);
    gCurTask->velX = 0x8000;
    gCurTask->frame = 0x14;
    TaskYieldTrampoline(3);
    gCurTask->velX = 0;
    gCurTask->frame = 0x13;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->velX = 0x10000;
    gCurTask->frame = 0x6D;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x6C;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xC000;
    gCurTask->frame = 0x6B;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x6A;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x69;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x8000;
    gCurTask->frame = 0x68;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x67;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x66;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x4000;
    gCurTask->frame = 0x6F;
    TaskYieldTrampoline(2);
    gCurTask->spriteFlags |= 0x8000;
    gCurTask->velX = 0;
    gCurTask->frame = 0x11;
    TaskYieldTrampoline(3);
    gCurTask->velX = 0;
    gCurTask->frame = 0x10;
    TaskYieldTrampoline(3);
    gCurTask->velX = 0xFFFFC000;
    gCurTask->accelX = 0;
    gCurTask->frame = 15;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFF8000;
    gCurTask->frame = 14;
    TaskYieldTrampoline(1);
    gCurTask->frame = 13;
    TaskYieldTrampoline(3);
    gCurTask->velX = 0xFFFF0000;
    gCurTask->frame = 0x1B;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x1C;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x1D;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x1C;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x1B;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFF8000;
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFEC000, 0, 0x5A5A5A5A, 0xFFFD4000, 0x4000, 0x5A5A5A5A);
    gCurTask->frame = 0x16;
    TaskYieldTrampoline(14);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x29;
    TaskYieldTrampoline(1);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0xFFFFC000;
    gCurTask->frame = 0x1E;
    TaskYieldTrampoline(0xF);
    gCurTask->velX = 0;
    gCurTask->frame = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x4000;
    gCurTask->frame = 5;
    TaskYieldTrampoline(5);
    gCurTask->velX = 0;
    gCurTask->frame = 5;
    TaskYieldTrampoline(14);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    CreatePlayerEffect(gCurTask->player->playerIndex, 16, 0);
    gCurTask->frame = 0x92;
    TaskYieldTrampoline(0x15);
}
