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

    gRngValue = gInputRecordingPtr->unk00;
    gVBlankCount = gInputRecordingPtr->unk04;
    gFrameCount = gInputRecordingPtr->unk06;
    gLevelIndex = gInputRecordingPtr->unk08;
    gStageIndex = gInputRecordingPtr->unk09;
    gRoomIndex = gInputRecordingPtr->unk0A;
    gPlayerCount = gInputRecordingPtr->unk0C;
    gRoomEntryMode = gInputRecordingPtr->unk12;
    gRoomEntryX = gInputRecordingPtr->unk0E;
    gRoomEntryY = gInputRecordingPtr->unk10;
    gUnk_02000020 = gInputRecordingPtr->unk13;
    gMaxHealth = gInputRecordingPtr->unk14;
    gWarpStarStationLevels = gInputRecordingPtr->unk36;
    gMetaKnightmareMode = gInputRecordingPtr->unk11E;
    for (i = 0; i <= 7; i++)
        gUsedSubGameDoors[i] = gInputRecordingPtr->unk38[i];
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 7; j++)
            gUsedRoomObjects[i][j] = gInputRecordingPtr->unk40[i][j];
    }
    for (i = 0; i < gPlayerCount; i++)
    {
        gInputRecorderCurPos[i] = i;
        gInputRecorderNextPos[i] = i;
        gInputRecorderEntryFrames[i] = 0;
        gPlayerLives[i] = gInputRecordingPtr->unk16[i];
        gPlayerHealth[i] = gInputRecordingPtr->unk1E[i];
        gPlayerAbilities[i] = gInputRecordingPtr->unk26[i];
        gPlayerAbilityUses[i] = gInputRecordingPtr->unk2E[i];
    }
    j = Div(0x3B6A, gPlayerCount);
    for (i = 0; i < gPlayerCount; i++)
        gInputRecorderEndPos[i] = j - 4;
    if (gCreditsDemoSet == 0)
    {
        gLocalPlayer = gInputRecordingPtr->unk11C;
        gExtraMode = gInputRecordingPtr->unk0B;
        gMilestoneFlags = gInputRecordingPtr->unkC0;
        gCurLevel = gInputRecordingPtr->unkC4[gExtraMode];
        gUnk_03001F20 = gInputRecordingPtr->unkC6[gExtraMode];
        gFurthestLevel = gInputRecordingPtr->unkC8[gExtraMode];
        gFurthestStage = gInputRecordingPtr->unkCA[gExtraMode];
        gBigSwitchFlags[0] = gInputRecordingPtr->unkCC[gExtraMode];
    }
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gStageClearStatus[i][j] = (gInputRecordingPtr->unkD4[i][j] >> (gExtraMode * 4)) & 15;
    }
    for (i = 0; i <= 3; i++)
    {
        gBossEnduranceBestTime[i] = gInputRecordingPtr->unk10C[i];
        gMetaKnightmareBestTime[i] = gInputRecordingPtr->unk114[i];
    }
    gCompletionPercent = gInputRecordingPtr->unkC2;
    gRoomEntrySet = 1;
}
