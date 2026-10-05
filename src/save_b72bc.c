#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "room.h"
#include "player.h"
#include "save.h"

void InputRecorderRestoreState(void)
{
    s32 i;
    s32 j;

    gRngValue = gInputRecordingPtr->rngValue;
    gVBlankCount = gInputRecordingPtr->vblankCount;
    gFrameCount = gInputRecordingPtr->frameCount;
    gLevelIndex = gInputRecordingPtr->levelIndex;
    gStageIndex = gInputRecordingPtr->stageIndex;
    gRoomIndex = gInputRecordingPtr->roomIndex;
    gPlayerCount = gInputRecordingPtr->playerCount;
    gRoomEntryMode = gInputRecordingPtr->roomEntryMode;
    gRoomEntryX = gInputRecordingPtr->roomEntryX;
    gRoomEntryY = gInputRecordingPtr->roomEntryY;
    gRoomPlayerMode = gInputRecordingPtr->unk13;
    gMaxHealth = gInputRecordingPtr->maxHealth;
    gWarpStarStationLevels = gInputRecordingPtr->warpStarStationLevels;
    gMetaKnightmareMode = gInputRecordingPtr->metaKnightmareMode;
    for (i = 0; i <= 7; i++)
        gUsedSubGameDoors[i] = gInputRecordingPtr->usedSubGameDoors[i];
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 7; j++)
            gUsedRoomObjects[i][j] = gInputRecordingPtr->usedRoomObjects[i][j];
    }
    for (i = 0; i < gPlayerCount; i++)
    {
        gInputRecorderCurPos[i] = i;
        gInputRecorderNextPos[i] = i;
        gInputRecorderEntryFrames[i] = 0;
        gPlayerLives[i] = gInputRecordingPtr->playerLives[i];
        gPlayerHealth[i] = gInputRecordingPtr->playerHealth[i];
        gPlayerAbilities[i] = gInputRecordingPtr->playerAbilities[i];
        gPlayerAbilityUses[i] = gInputRecordingPtr->playerAbilityUses[i];
    }
    j = Div(0x3B6A, gPlayerCount);
    for (i = 0; i < gPlayerCount; i++)
        gInputRecorderEndPos[i] = j - 4;
    if (gCreditsDemoSet == 0)
    {
        gLocalPlayer = gInputRecordingPtr->localPlayer;
        gExtraMode = gInputRecordingPtr->extraMode;
        gMilestoneFlags = gInputRecordingPtr->milestoneFlags;
        gCurLevel = gInputRecordingPtr->curLevel[gExtraMode];
        gCurStage = gInputRecordingPtr->curStage[gExtraMode];
        gFurthestLevel = gInputRecordingPtr->furthestLevel[gExtraMode];
        gFurthestStage = gInputRecordingPtr->furthestStage[gExtraMode];
        gBigSwitchFlags[0] = gInputRecordingPtr->bigSwitchFlags[gExtraMode];
    }
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gStageClearStatus[i][j] = (gInputRecordingPtr->stageClearStatus[i][j] >> (gExtraMode * 4)) & 15;
    }
    for (i = 0; i <= 3; i++)
    {
        gBossEnduranceBestTime[i] = gInputRecordingPtr->bossEnduranceBestTime[i];
        gMetaKnightmareBestTime[i] = gInputRecordingPtr->metaKnightmareBestTime[i];
    }
    gCompletionPercent = gInputRecordingPtr->completionPercent;
    gRoomEntrySet = 1;
}
