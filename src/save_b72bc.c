#include "gba/gba.h"
#include "global.h"
#include "task.h"

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
};

extern struct LinkSave *gInputRecordingPtr;
extern s32 Div(s32 a, s32 b);
extern u8 gUnk_02000020;
extern u16 gUnk_02004B50[];
extern u16 gUnk_02005580;
extern s16 gUnk_02005588[];
extern u16 gRoomEntryX;
extern u8 gRoomEntrySet;
extern u8 gUnk_020069F0;
extern u32 gUnk_02007BF0[8][8];
extern s16 gPlayerLives[];
extern u8 gUnk_02007D58[];
extern u16 gRoomEntryY;
extern u16 gUnk_0200AF18[];
extern u8 gUnk_0200B04C;
extern u8 gUnk_0200EC68[];
extern u16 gUnk_0200EC70[];
extern u16 gUnk_0200EC78[];
extern u16 gUnk_02016480[];
extern u16 gVBlankCount;
extern u32 gRngValue;
extern u16 gFrameCount;
extern u16 gUnk_03001F18[];
extern u8 gUnk_03001F20;
extern u8 gUnk_03001F30;
extern u16 gCompletionPercent;
extern u16 gLocalPlayer;
extern u16 gUnk_03002364;
extern u16 gUnk_03002378[];
extern s8 gUnk_03002384;
extern s8 gLevelIndex;
extern u16 gPlayerCount;
extern u8 gUnk_030023B0;
extern u8 gUnk_030023B8;
extern s32 gUnk_030023C8[];
extern s8 gUnk_030023E0;
extern u8 gStageIndex;
extern u8 gUnk_03002400[8][7];
extern u8 gExtraMode;
extern u8 gRoomIndex;

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
    gUnk_02005580 = gInputRecordingPtr->unk14;
    gUnk_0200B04C = gInputRecordingPtr->unk36;
    gUnk_03001F30 = gInputRecordingPtr->unk11E;
    for (i = 0; i <= 7; i++)
        gUnk_02007D58[i] = gInputRecordingPtr->unk38[i];
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
        gUnk_02005588[i] = gInputRecordingPtr->unk1E[i];
        gUnk_02004B50[i] = gInputRecordingPtr->unk26[i];
        gUnk_0200AF18[i] = gInputRecordingPtr->unk2E[i];
    }
    j = Div(0x3B6A, gPlayerCount);
    for (i = 0; i < gPlayerCount; i++)
        gUnk_0200EC70[i] = j - 4;
    if (gUnk_030023B0 == 0)
    {
        gLocalPlayer = gInputRecordingPtr->unk11C;
        gExtraMode = gInputRecordingPtr->unk0B;
        gUnk_03002364 = gInputRecordingPtr->unkC0;
        gUnk_030023B8 = gInputRecordingPtr->unkC4[gExtraMode];
        gUnk_03001F20 = gInputRecordingPtr->unkC6[gExtraMode];
        gUnk_030023E0 = gInputRecordingPtr->unkC8[gExtraMode];
        gUnk_03002384 = gInputRecordingPtr->unkCA[gExtraMode];
        gUnk_030023C8[0] = gInputRecordingPtr->unkCC[gExtraMode];
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
