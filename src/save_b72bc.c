#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "room.h"
#include "player.h"
#include "save.h"

extern s32 Div(s32 a, s32 b);

void sub_080b72bc(void)
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
    gUnk_020069F0 = gInputRecordingPtr->unk12;
    gRoomEntryX = gInputRecordingPtr->unk0E;
    gRoomEntryY = gInputRecordingPtr->unk10;
    gUnk_02000020 = gInputRecordingPtr->unk13;
    gMaxHealth = gInputRecordingPtr->unk14;
    gUnk_0200B04C = gInputRecordingPtr->unk36;
    gUnk_03001F30 = gInputRecordingPtr->unk11E;
    for (i = 0; i <= 7; i++)
        gUsedSubGameDoors[i] = gInputRecordingPtr->unk38[i];
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 7; j++)
            gUnk_02007BF0[i][j] = gInputRecordingPtr->unk40[i][j];
    }
    for (i = 0; i < gPlayerCount; i++)
    {
        gUnk_02016480[i] = i;
        gUnk_0200EC78[i] = i;
        gUnk_0200EC68[i] = 0;
        gPlayerLives[i] = gInputRecordingPtr->unk16[i];
        gPlayerHealth[i] = gInputRecordingPtr->unk1E[i];
        gPlayerAbilities[i] = gInputRecordingPtr->unk26[i];
        gPlayerAbilityUses[i] = gInputRecordingPtr->unk2E[i];
    }
    j = Div(0x3B6A, gPlayerCount);
    for (i = 0; i < gPlayerCount; i++)
        gUnk_0200EC70[i] = j - 4;
    if (gUnk_030023B0 == 0)
    {
        gLocalPlayer = gInputRecordingPtr->unk11C;
        gExtraMode = gInputRecordingPtr->unk0B;
        gMilestoneFlags = gInputRecordingPtr->unkC0;
        gUnk_030023B8 = gInputRecordingPtr->unkC4[gExtraMode];
        gUnk_03001F20 = gInputRecordingPtr->unkC6[gExtraMode];
        gUnk_030023E0 = gInputRecordingPtr->unkC8[gExtraMode];
        gUnk_03002384 = gInputRecordingPtr->unkCA[gExtraMode];
        gBigSwitchFlags[0] = gInputRecordingPtr->unkCC[gExtraMode];
    }
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gUnk_03002400[i][j] = (gInputRecordingPtr->unkD4[i][j] >> (gExtraMode * 4)) & 15;
    }
    for (i = 0; i <= 3; i++)
    {
        gUnk_03001F18[i] = gInputRecordingPtr->unk10C[i];
        gUnk_03002378[i] = gInputRecordingPtr->unk114[i];
    }
    gCompletionPercent = gInputRecordingPtr->unkC2;
    gRoomEntrySet = 1;
}
