#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "room.h"

struct InputRecording
{
    /*0x000*/ u32 rngValue;
    /*0x004*/ u16 vblankCount;
    /*0x006*/ u16 frameCount;
    /*0x008*/ u8 levelIndex;
    /*0x009*/ u8 stageIndex;
    /*0x00A*/ u8 roomIndex;
    /*0x00B*/ u8 extraMode;
    /*0x00C*/ u16 playerCount;
    /*0x00E*/ u16 roomEntryX;
    /*0x010*/ u16 roomEntryY;
    /*0x012*/ u8 roomEntryMode;
    /*0x013*/ u8 unk13;
    /*0x014*/ u16 maxHealth;
    /*0x016*/ u16 playerLives[4];
    /*0x01E*/ u16 playerHealth[4];
    /*0x026*/ u16 playerAbilities[4];
    /*0x02E*/ u16 playerAbilityUses[4];
    /*0x036*/ u16 warpStarStationLevels;
    /*0x038*/ u8 usedSubGameDoors[8];
    /*0x040*/ u16 usedRoomObjects[8][8];
    /*0x0C0*/ u16 milestoneFlags;
    /*0x0C2*/ u16 completionPercent;
    /*0x0C4*/ u8 curLevel[2];
    /*0x0C6*/ u8 unkC6[2];
    /*0x0C8*/ u8 furthestLevel[2];
    /*0x0CA*/ u8 furthestStage[2];
    /*0x0CC*/ u32 bigSwitchFlags[2];
    /*0x0D4*/ u8 stageClearStatus[8][7];
    /*0x10C*/ u16 bossEnduranceBestTime[4];
    /*0x114*/ u16 metaKnightmareBestTime[4];
    /*0x11C*/ u16 localPlayer;
    /*0x11E*/ u8 metaKnightmareMode;
    /*0x11F*/ u8 pad11F[0xD];
    /*0x12C*/ u16 keyLog[0x3B6A];
};

/* Not from save.h: this file's view of gInputRecorderDemo differs (lesson 3.517). */
extern struct InputRecording *gInputRecorderDemo;
extern struct InputRecording *gInputRecordingPtr;
extern u8 gInputRecorderRunning;
extern s16 gInputRecorderMode;
extern u16 gInputRecorderKeys[];
extern u8 gInputRecorderEntryFrames[];
extern u16 gInputRecorderEndPos[];
extern u16 gInputRecorderNextPos[];
extern struct InputRecording gInputRecording;
extern u16 gBossEnduranceBestTime[];
extern u16 gCompletionPercent;
extern u16 gMetaKnightmareBestTime[];
extern void InputRecorderRestoreState(void);
extern void ReadInputRecording(void);
extern void WriteInputRecording(void);

void InputRecorderStart(void)
{
    s32 i;
    s32 j;

    gInputRecorderRunning = 1;
    switch (gInputRecorderMode)
    {
    case 1:
        gInputRecordingPtr = &gInputRecording;
        for (i = 0; i <= 0x3B69; i++)
            gInputRecordingPtr->keyLog[i] = 0;
        gInputRecordingPtr->rngValue = gRngValue;
        gInputRecordingPtr->vblankCount = gVBlankCount;
        gInputRecordingPtr->frameCount = gFrameCount;
        gInputRecordingPtr->levelIndex = gLevelIndex;
        gInputRecordingPtr->stageIndex = gStageIndex;
        gInputRecordingPtr->roomIndex = gRoomIndex;
        gInputRecordingPtr->extraMode = gExtraMode;
        gInputRecordingPtr->playerCount = gPlayerCount;
        gInputRecordingPtr->roomEntryMode = gRoomEntryMode;
        gInputRecordingPtr->unk13 = gRoomPlayerMode;
        gInputRecordingPtr->maxHealth = gMaxHealth;
        gInputRecordingPtr->warpStarStationLevels = gWarpStarStationLevels;
        gInputRecordingPtr->localPlayer = gLocalPlayer;
        gInputRecordingPtr->metaKnightmareMode = gMetaKnightmareMode;
        for (i = 0; i <= 7; i++)
        {
            gInputRecordingPtr->usedSubGameDoors[i] = gUsedSubGameDoors[i];
        }
        for (i = 0; i <= 7; i++)
        {
            for (j = 0; j <= 7; j++)
                gInputRecordingPtr->usedRoomObjects[i][j] = gUsedRoomObjects[i][j];
        }
        for (i = 0; i < gPlayerCount; i++)
        {
            gInputRecorderNextPos[i] = i;
            gInputRecorderKeys[i] = 0xFFFF;
            gInputRecorderEntryFrames[i] = 0;
            gInputRecordingPtr->playerLives[i] = gPlayerLives[i];
            gInputRecordingPtr->playerHealth[i] = gPlayerHealth[i];
            gInputRecordingPtr->playerAbilities[i] = gPlayerAbilities[i];
            gInputRecordingPtr->playerAbilityUses[i] = gPlayerAbilityUses[i];
        }
        gInputRecordingPtr->milestoneFlags = gMilestoneFlags;
        gInputRecordingPtr->curLevel[gExtraMode] = gCurLevel;
        gInputRecordingPtr->unkC6[gExtraMode] = gCurStage;
        gInputRecordingPtr->furthestLevel[gExtraMode] = gFurthestLevel;
        gInputRecordingPtr->furthestStage[gExtraMode] = gFurthestStage;
        gInputRecordingPtr->bigSwitchFlags[gExtraMode] = gBigSwitchFlags[0];
        for (i = 0; i <= 7; i++)
        {
            for (j = 0; j <= 6; j++)
            {
                gInputRecordingPtr->stageClearStatus[i][j] &= 15 << ((1 ^ gExtraMode) * 4);
                gInputRecordingPtr->stageClearStatus[i][j] |= gStageClearStatus[i][j] << (gExtraMode * 4);
            }
        }
        for (i = 0; i <= 3; i++)
        {
            gInputRecordingPtr->bossEnduranceBestTime[i] = gBossEnduranceBestTime[i];
            gInputRecordingPtr->metaKnightmareBestTime[i] = gMetaKnightmareBestTime[i];
        }
        gInputRecordingPtr->completionPercent = gCompletionPercent;
        WriteInputRecording();
        j = Div(0x3B6A, gPlayerCount);
        for (i = 0; i < gPlayerCount; i++)
            gInputRecorderEndPos[i] = j - 4;
        break;
    case 2:
        ReadInputRecording();
        gInputRecordingPtr = &gInputRecording;
        InputRecorderRestoreState();
        break;
    case 3:
        gInputRecordingPtr = gInputRecorderDemo;
        InputRecorderRestoreState();
        break;
    }
}
