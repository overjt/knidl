#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "room.h"

struct LinkSave
{
    /*0x000*/ u32 unk00;
    /*0x004*/ u16 unk04;
    /*0x006*/ u16 unk06;
    /*0x008*/ u8 unk08;
    /*0x009*/ u8 unk09;
    /*0x00A*/ u8 unk0A;
    /*0x00B*/ u8 unk0B;
    /*0x00C*/ u16 unk0C;
    /*0x00E*/ u16 unk0E;
    /*0x010*/ u16 unk10;
    /*0x012*/ u8 unk12;
    /*0x013*/ u8 unk13;
    /*0x014*/ u16 unk14;
    /*0x016*/ u16 unk16[4];
    /*0x01E*/ u16 unk1E[4];
    /*0x026*/ u16 unk26[4];
    /*0x02E*/ u16 unk2E[4];
    /*0x036*/ u16 unk36;
    /*0x038*/ u8 unk38[8];
    /*0x040*/ u16 unk40[8][8];
    /*0x0C0*/ u16 unkC0;
    /*0x0C2*/ u16 unkC2;
    /*0x0C4*/ u8 unkC4[2];
    /*0x0C6*/ u8 unkC6[2];
    /*0x0C8*/ u8 unkC8[2];
    /*0x0CA*/ u8 unkCA[2];
    /*0x0CC*/ u32 unkCC[2];
    /*0x0D4*/ u8 unkD4[8][7];
    /*0x10C*/ u16 unk10C[4];
    /*0x114*/ u16 unk114[4];
    /*0x11C*/ u16 unk11C;
    /*0x11E*/ u8 unk11E;
    /*0x11F*/ u8 pad11F[0xD];
    /*0x12C*/ u16 unk12C[0x3B6A];
};

/* Not from save.h: this file's view of gUnk_0200EC50 differs (lesson 3.517). */
extern struct LinkSave *gUnk_0200EC50;
extern struct LinkSave *gInputRecordingPtr;
extern u8 gInputRecorderRunning;
extern s16 gInputRecorderMode;
extern u16 gInputRecorderKeys[];
extern u8 gInputRecorderEntryFrames[];
extern u16 gInputRecorderEndPos[];
extern u16 gInputRecorderNextPos[];
extern struct LinkSave gInputRecording;
extern u16 gBossEnduranceBestTime[];
extern u16 gCompletionPercent;
extern u16 gMetaKnightmareBestTime[];
extern void sub_080b72bc(void);
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
            gInputRecordingPtr->unk12C[i] = 0;
        gInputRecordingPtr->unk00 = gRngValue;
        gInputRecordingPtr->unk04 = gVBlankCount;
        gInputRecordingPtr->unk06 = gFrameCount;
        gInputRecordingPtr->unk08 = gLevelIndex;
        gInputRecordingPtr->unk09 = gStageIndex;
        gInputRecordingPtr->unk0A = gRoomIndex;
        gInputRecordingPtr->unk0B = gExtraMode;
        gInputRecordingPtr->unk0C = gPlayerCount;
        gInputRecordingPtr->unk12 = gUnk_020069F0;
        gInputRecordingPtr->unk13 = gUnk_02000020;
        gInputRecordingPtr->unk14 = gMaxHealth;
        gInputRecordingPtr->unk36 = gWarpStarStationLevels;
        gInputRecordingPtr->unk11C = gLocalPlayer;
        gInputRecordingPtr->unk11E = gMetaKnightmareMode;
        for (i = 0; i <= 7; i++)
        {
            gInputRecordingPtr->unk38[i] = gUsedSubGameDoors[i];
        }
        for (i = 0; i <= 7; i++)
        {
            for (j = 0; j <= 7; j++)
                gInputRecordingPtr->unk40[i][j] = gUnk_02007BF0[i][j];
        }
        for (i = 0; i < gPlayerCount; i++)
        {
            gInputRecorderNextPos[i] = i;
            gInputRecorderKeys[i] = 0xFFFF;
            gInputRecorderEntryFrames[i] = 0;
            gInputRecordingPtr->unk16[i] = gPlayerLives[i];
            gInputRecordingPtr->unk1E[i] = gPlayerHealth[i];
            gInputRecordingPtr->unk26[i] = gPlayerAbilities[i];
            gInputRecordingPtr->unk2E[i] = gPlayerAbilityUses[i];
        }
        gInputRecordingPtr->unkC0 = gMilestoneFlags;
        gInputRecordingPtr->unkC4[gExtraMode] = gCurLevel;
        gInputRecordingPtr->unkC6[gExtraMode] = gUnk_03001F20;
        gInputRecordingPtr->unkC8[gExtraMode] = gFurthestLevel;
        gInputRecordingPtr->unkCA[gExtraMode] = gFurthestStage;
        gInputRecordingPtr->unkCC[gExtraMode] = gBigSwitchFlags[0];
        for (i = 0; i <= 7; i++)
        {
            for (j = 0; j <= 6; j++)
            {
                gInputRecordingPtr->unkD4[i][j] &= 15 << ((1 ^ gExtraMode) * 4);
                gInputRecordingPtr->unkD4[i][j] |= gStageClearStatus[i][j] << (gExtraMode * 4);
            }
        }
        for (i = 0; i <= 3; i++)
        {
            gInputRecordingPtr->unk10C[i] = gBossEnduranceBestTime[i];
            gInputRecordingPtr->unk114[i] = gMetaKnightmareBestTime[i];
        }
        gInputRecordingPtr->unkC2 = gCompletionPercent;
        WriteInputRecording();
        j = Div(0x3B6A, gPlayerCount);
        for (i = 0; i < gPlayerCount; i++)
            gInputRecorderEndPos[i] = j - 4;
        break;
    case 2:
        ReadInputRecording();
        gInputRecordingPtr = &gInputRecording;
        sub_080b72bc();
        break;
    case 3:
        gInputRecordingPtr = gUnk_0200EC50;
        sub_080b72bc();
        break;
    }
}
