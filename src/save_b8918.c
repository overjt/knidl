#include "gba/gba.h"
#include "gba/agb_sram.h"
#include "global.h"
#include "task.h"

struct SaveSlot
{
    /*0x00*/ u32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ u32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ u16 unk10;
    /*0x12*/ u16 unk12[2];
    /*0x16*/ u8 unk16[2];
    /*0x18*/ u8 unk18[2];
    /*0x1A*/ u8 unk1A[2];
    /*0x1C*/ u8 unk1C[2];
    /*0x1E*/ u8 pad1E[2];
    /*0x20*/ u32 unk20[2];
    /*0x28*/ u8 unk28[8][7];
    /*0x60*/ u16 unk60[4];
    /*0x68*/ u16 unk68[4];
    /*0x70*/ u32 unk70;
    /*0x74*/ u8 filler74[0x8C];
};

struct LinkRec
{
    /*0x00*/ u16 unk00;
    /*0x02*/ u16 unk02[2];
    /*0x06*/ u8 unk06[2];
    /*0x08*/ u8 unk08[2];
    /*0x0A*/ u8 unk0A[2];
    /*0x0C*/ u8 unk0C[2];
    /*0x0E*/ u16 pad0E;
    /*0x10*/ u32 unk10[2];
    /*0x18*/ u8 unk18[8][7];
    /*0x50*/ u8 filler50[0x10];
};

extern s32 Div(s32 a, s32 b);
extern s32 Mod(s32 a, s32 b);
extern u16 gUnk_02006068[];
extern struct SaveSlot gSaveSlots[];
extern struct LinkRec gUnk_0200EA00[];
extern u8 gUnk_0200EB80;
extern u8 gUnk_0200EC80[];
extern s32 gSramAvailable;
extern u16 gUnk_03001F18[];
extern u8 gUnk_03001F20;
extern u16 gCompletionPercent;
extern u16 gUnk_03002364;
extern u16 gUnk_03002378[];
extern s8 gUnk_03002384;
extern u16 gPlayerCount;
extern u8 gUnk_030023B8;
extern s32 gUnk_030023C8[];
extern s8 gUnk_030023E0;
extern s32 gCurSaveSlot;
extern u8 gUnk_03002400[8][7];
extern u8 gExtraMode;
extern u16 gRecvCmds[];
extern u16 gSendCmd[];

s32 CalcCompletionPercent(s32 a);
s32 WriteSaveSlot(s32 a);
u32 UpdateSaveSlotChecksum(s32 a);
void sub_080b7e14(s32 a);
void sub_080b8b2c(s32 a);

void sub_080b8918(void)
{
    s32 i;
    s32 j;
    s32 k;

    for (i = 0; i < gPlayerCount; i++)
    {
        if ((s8)gSaveSlots[3].unk1A[gExtraMode] > (s8)gUnk_0200EA00[i].unk0A[gExtraMode])
        {
            gSaveSlots[3].unk1A[gExtraMode] = gUnk_0200EA00[i].unk0A[gExtraMode];
            gSaveSlots[3].unk1C[gExtraMode] = gUnk_0200EA00[i].unk0C[gExtraMode];
            gSaveSlots[3].unk16[gExtraMode] = gUnk_0200EA00[i].unk06[gExtraMode];
            gSaveSlots[3].unk18[gExtraMode] = gUnk_0200EA00[i].unk08[gExtraMode];
        }
        else if ((s8)gSaveSlots[3].unk1A[gExtraMode] == (s8)gUnk_0200EA00[i].unk0A[gExtraMode])
        {
            if ((s8)gSaveSlots[3].unk1C[gExtraMode] > (s8)gUnk_0200EA00[i].unk0C[gExtraMode])
            {
                gSaveSlots[3].unk1C[gExtraMode] = gUnk_0200EA00[i].unk0C[gExtraMode];
                gSaveSlots[3].unk16[gExtraMode] = gUnk_0200EA00[i].unk06[gExtraMode];
                gSaveSlots[3].unk18[gExtraMode] = gUnk_0200EA00[i].unk08[gExtraMode];
            }
            else if ((s8)gSaveSlots[3].unk1C[gExtraMode] == (s8)gUnk_0200EA00[i].unk0C[gExtraMode])
            {
                if ((s8)gSaveSlots[3].unk16[gExtraMode] > (s8)gUnk_0200EA00[i].unk06[gExtraMode])
                {
                    gSaveSlots[3].unk16[gExtraMode] = gUnk_0200EA00[i].unk06[gExtraMode];
                    gSaveSlots[3].unk18[gExtraMode] = gUnk_0200EA00[i].unk08[gExtraMode];
                }
                else if ((s8)gSaveSlots[3].unk16[gExtraMode] == (s8)gUnk_0200EA00[i].unk06[gExtraMode])
                {
                    if ((s8)gSaveSlots[3].unk18[gExtraMode] > (s8)gUnk_0200EA00[i].unk08[gExtraMode])
                        gSaveSlots[3].unk18[gExtraMode] = gUnk_0200EA00[i].unk08[gExtraMode];
                }
            }
        }
        if (gSaveSlots[3].unk12[gExtraMode] > gUnk_0200EA00[i].unk02[gExtraMode])
            gSaveSlots[3].unk12[gExtraMode] = gUnk_0200EA00[i].unk02[gExtraMode];
        gSaveSlots[3].unk20[gExtraMode] &= gUnk_0200EA00[i].unk10[gExtraMode];
        gSaveSlots[3].unk10 &= gUnk_0200EA00[i].unk00;
        for (j = 0; j <= 7; j++)
        {
            for (k = 0; k <= 6; k++)
            {
                if (gSaveSlots[3].unk28[j][k] > gUnk_0200EA00[i].unk18[j][k])
                    gSaveSlots[3].unk28[j][k] = gUnk_0200EA00[i].unk18[j][k];
            }
        }
    }
}
void sub_080b8b2c(s32 a)
{
    s32 i;
    s32 j;

    gSaveSlots[3].unk16[gExtraMode] = gUnk_030023B8;
    gSaveSlots[3].unk18[gExtraMode] = gUnk_03001F20;
    gSaveSlots[3].unk1A[gExtraMode] = gUnk_030023E0;
    gSaveSlots[3].unk1C[gExtraMode] = gUnk_03002384;
    gSaveSlots[3].unk20[gExtraMode] = gUnk_030023C8[0];
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
        {
            gSaveSlots[3].unk28[i][j] &= 15 << ((gExtraMode ^ 1) * 4);
            gSaveSlots[3].unk28[i][j] |= gUnk_03002400[i][j] << (gExtraMode * 4);
        }
    }
    CalcCompletionPercent(gExtraMode);
    gSaveSlots[3].unk12[gExtraMode] = gCompletionPercent;
    gSaveSlots[3].unk10 = gUnk_03002364;
    gSaveSlots[a].unk16[gExtraMode] = gSaveSlots[3].unk16[gExtraMode];
    gSaveSlots[a].unk18[gExtraMode] = gSaveSlots[3].unk18[gExtraMode];
    if ((s8)gSaveSlots[a].unk1A[gExtraMode] < (s8)gSaveSlots[3].unk1A[gExtraMode])
    {
        gSaveSlots[a].unk1A[gExtraMode] = gSaveSlots[3].unk1A[gExtraMode];
        gSaveSlots[a].unk1C[gExtraMode] = gSaveSlots[3].unk1C[gExtraMode];
    }
    else if ((s8)gSaveSlots[a].unk1A[gExtraMode] == (s8)gSaveSlots[3].unk1A[gExtraMode])
    {
        if ((s8)gSaveSlots[a].unk1C[gExtraMode] < (s8)gSaveSlots[3].unk1C[gExtraMode])
            gSaveSlots[a].unk1C[gExtraMode] = gSaveSlots[3].unk1C[gExtraMode];
    }
    gSaveSlots[a].unk20[gExtraMode] |= gSaveSlots[3].unk20[gExtraMode];
    gSaveSlots[a].unk10 |= gSaveSlots[3].unk10;
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
        {
            if (gSaveSlots[a].unk28[i][j] < gSaveSlots[3].unk28[i][j])
                gSaveSlots[a].unk28[i][j] = gSaveSlots[3].unk28[i][j];
        }
    }
    if ((gSaveSlots[a].unk10 & (4 << gExtraMode)) != 0)
    {
        gSaveSlots[a].unk12[gExtraMode] = 100;
    }
    else
    {
        if ((s8)gSaveSlots[a].unk1A[gExtraMode] > 1)
        {
            gSaveSlots[a].unk12[gExtraMode] = (s8)gSaveSlots[a].unk1C[gExtraMode] * 2;
            gSaveSlots[a].unk12[gExtraMode] += (s8)gSaveSlots[a].unk1A[gExtraMode]
                + ((((s8)gSaveSlots[a].unk1A[gExtraMode] - 2) * 3) * 4 + 14);
            if (((gSaveSlots[a].unk10 >> gExtraMode) & 1) != 0)
                gSaveSlots[a].unk12[gExtraMode] += 3;
        }
        else if ((s8)gSaveSlots[a].unk1A[gExtraMode] == 1)
        {
            gSaveSlots[a].unk12[gExtraMode] = (s8)gSaveSlots[a].unk1C[gExtraMode] * 2;
            gSaveSlots[a].unk12[gExtraMode] += (s8)gSaveSlots[a].unk1A[gExtraMode] + 4;
        }
        else if ((s8)gSaveSlots[a].unk1A[gExtraMode] == 0)
        {
            gSaveSlots[a].unk12[gExtraMode] = (s8)gSaveSlots[a].unk1C[gExtraMode];
        }
        for (i = 0; i <= 16; i++)
        {
            if ((gSaveSlots[a].unk20[gExtraMode] & (1 << i)) != 0)
                gSaveSlots[a].unk12[gExtraMode]++;
        }
        if (gSaveSlots[a].unk12[gExtraMode] == 100)
            gSaveSlots[a].unk10 |= 4 << gExtraMode;
    }
}
