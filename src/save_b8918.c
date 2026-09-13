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
extern struct SaveSlot gUnk_0200E600[];
extern struct LinkRec gUnk_0200EA00[];
extern u8 gUnk_0200EB80;
extern u8 gUnk_0200EC80[];
extern s32 gUnk_03001EA8;
extern u16 gUnk_03001F18[];
extern u8 gUnk_03001F20;
extern u16 gUnk_0300235C;
extern u16 gUnk_03002364;
extern u16 gUnk_03002378[];
extern s8 gUnk_03002384;
extern u16 gUnk_030023AC;
extern u8 gUnk_030023B8;
extern s32 gUnk_030023C8[];
extern s8 gUnk_030023E0;
extern s32 gUnk_030023E8;
extern u8 gUnk_03002400[8][7];
extern u8 gUnk_03002464;
extern u16 gUnk_03004D50[];
extern u16 gUnk_03004D90[];

void sub_080b79b8(s32 a);
s32 sub_080b7a9c(s32 a);
u32 sub_080b7df4(s32 a);
void sub_080b7e14(s32 a);
void sub_080b8b2c(s32 a);

void sub_080b8918(void)
{
    s32 i;
    s32 j;
    s32 k;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        if ((s8)gUnk_0200E600[3].unk1A[gUnk_03002464] > (s8)gUnk_0200EA00[i].unk0A[gUnk_03002464])
        {
            gUnk_0200E600[3].unk1A[gUnk_03002464] = gUnk_0200EA00[i].unk0A[gUnk_03002464];
            gUnk_0200E600[3].unk1C[gUnk_03002464] = gUnk_0200EA00[i].unk0C[gUnk_03002464];
            gUnk_0200E600[3].unk16[gUnk_03002464] = gUnk_0200EA00[i].unk06[gUnk_03002464];
            gUnk_0200E600[3].unk18[gUnk_03002464] = gUnk_0200EA00[i].unk08[gUnk_03002464];
        }
        else if ((s8)gUnk_0200E600[3].unk1A[gUnk_03002464] == (s8)gUnk_0200EA00[i].unk0A[gUnk_03002464])
        {
            if ((s8)gUnk_0200E600[3].unk1C[gUnk_03002464] > (s8)gUnk_0200EA00[i].unk0C[gUnk_03002464])
            {
                gUnk_0200E600[3].unk1C[gUnk_03002464] = gUnk_0200EA00[i].unk0C[gUnk_03002464];
                gUnk_0200E600[3].unk16[gUnk_03002464] = gUnk_0200EA00[i].unk06[gUnk_03002464];
                gUnk_0200E600[3].unk18[gUnk_03002464] = gUnk_0200EA00[i].unk08[gUnk_03002464];
            }
            else if ((s8)gUnk_0200E600[3].unk1C[gUnk_03002464] == (s8)gUnk_0200EA00[i].unk0C[gUnk_03002464])
            {
                if ((s8)gUnk_0200E600[3].unk16[gUnk_03002464] > (s8)gUnk_0200EA00[i].unk06[gUnk_03002464])
                {
                    gUnk_0200E600[3].unk16[gUnk_03002464] = gUnk_0200EA00[i].unk06[gUnk_03002464];
                    gUnk_0200E600[3].unk18[gUnk_03002464] = gUnk_0200EA00[i].unk08[gUnk_03002464];
                }
                else if ((s8)gUnk_0200E600[3].unk16[gUnk_03002464] == (s8)gUnk_0200EA00[i].unk06[gUnk_03002464])
                {
                    if ((s8)gUnk_0200E600[3].unk18[gUnk_03002464] > (s8)gUnk_0200EA00[i].unk08[gUnk_03002464])
                        gUnk_0200E600[3].unk18[gUnk_03002464] = gUnk_0200EA00[i].unk08[gUnk_03002464];
                }
            }
        }
        if (gUnk_0200E600[3].unk12[gUnk_03002464] > gUnk_0200EA00[i].unk02[gUnk_03002464])
            gUnk_0200E600[3].unk12[gUnk_03002464] = gUnk_0200EA00[i].unk02[gUnk_03002464];
        gUnk_0200E600[3].unk20[gUnk_03002464] &= gUnk_0200EA00[i].unk10[gUnk_03002464];
        gUnk_0200E600[3].unk10 &= gUnk_0200EA00[i].unk00;
        for (j = 0; j <= 7; j++)
        {
            for (k = 0; k <= 6; k++)
            {
                if (gUnk_0200E600[3].unk28[j][k] > gUnk_0200EA00[i].unk18[j][k])
                    gUnk_0200E600[3].unk28[j][k] = gUnk_0200EA00[i].unk18[j][k];
            }
        }
    }
}
void sub_080b8b2c(s32 a)
{
    s32 i;
    s32 j;

    gUnk_0200E600[3].unk16[gUnk_03002464] = gUnk_030023B8;
    gUnk_0200E600[3].unk18[gUnk_03002464] = gUnk_03001F20;
    gUnk_0200E600[3].unk1A[gUnk_03002464] = gUnk_030023E0;
    gUnk_0200E600[3].unk1C[gUnk_03002464] = gUnk_03002384;
    gUnk_0200E600[3].unk20[gUnk_03002464] = gUnk_030023C8[0];
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
        {
            gUnk_0200E600[3].unk28[i][j] &= 15 << ((gUnk_03002464 ^ 1) * 4);
            gUnk_0200E600[3].unk28[i][j] |= gUnk_03002400[i][j] << (gUnk_03002464 * 4);
        }
    }
    sub_080b79b8(gUnk_03002464);
    gUnk_0200E600[3].unk12[gUnk_03002464] = gUnk_0300235C;
    gUnk_0200E600[3].unk10 = gUnk_03002364;
    gUnk_0200E600[a].unk16[gUnk_03002464] = gUnk_0200E600[3].unk16[gUnk_03002464];
    gUnk_0200E600[a].unk18[gUnk_03002464] = gUnk_0200E600[3].unk18[gUnk_03002464];
    if ((s8)gUnk_0200E600[a].unk1A[gUnk_03002464] < (s8)gUnk_0200E600[3].unk1A[gUnk_03002464])
    {
        gUnk_0200E600[a].unk1A[gUnk_03002464] = gUnk_0200E600[3].unk1A[gUnk_03002464];
        gUnk_0200E600[a].unk1C[gUnk_03002464] = gUnk_0200E600[3].unk1C[gUnk_03002464];
    }
    else if ((s8)gUnk_0200E600[a].unk1A[gUnk_03002464] == (s8)gUnk_0200E600[3].unk1A[gUnk_03002464])
    {
        if ((s8)gUnk_0200E600[a].unk1C[gUnk_03002464] < (s8)gUnk_0200E600[3].unk1C[gUnk_03002464])
            gUnk_0200E600[a].unk1C[gUnk_03002464] = gUnk_0200E600[3].unk1C[gUnk_03002464];
    }
    gUnk_0200E600[a].unk20[gUnk_03002464] |= gUnk_0200E600[3].unk20[gUnk_03002464];
    gUnk_0200E600[a].unk10 |= gUnk_0200E600[3].unk10;
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
        {
            if (gUnk_0200E600[a].unk28[i][j] < gUnk_0200E600[3].unk28[i][j])
                gUnk_0200E600[a].unk28[i][j] = gUnk_0200E600[3].unk28[i][j];
        }
    }
    if ((gUnk_0200E600[a].unk10 & (4 << gUnk_03002464)) != 0)
    {
        gUnk_0200E600[a].unk12[gUnk_03002464] = 100;
    }
    else
    {
        if ((s8)gUnk_0200E600[a].unk1A[gUnk_03002464] > 1)
        {
            gUnk_0200E600[a].unk12[gUnk_03002464] = (s8)gUnk_0200E600[a].unk1C[gUnk_03002464] * 2;
            gUnk_0200E600[a].unk12[gUnk_03002464] += (s8)gUnk_0200E600[a].unk1A[gUnk_03002464]
                + ((((s8)gUnk_0200E600[a].unk1A[gUnk_03002464] - 2) * 3) * 4 + 14);
            if (((gUnk_0200E600[a].unk10 >> gUnk_03002464) & 1) != 0)
                gUnk_0200E600[a].unk12[gUnk_03002464] += 3;
        }
        else if ((s8)gUnk_0200E600[a].unk1A[gUnk_03002464] == 1)
        {
            gUnk_0200E600[a].unk12[gUnk_03002464] = (s8)gUnk_0200E600[a].unk1C[gUnk_03002464] * 2;
            gUnk_0200E600[a].unk12[gUnk_03002464] += (s8)gUnk_0200E600[a].unk1A[gUnk_03002464] + 4;
        }
        else if ((s8)gUnk_0200E600[a].unk1A[gUnk_03002464] == 0)
        {
            gUnk_0200E600[a].unk12[gUnk_03002464] = (s8)gUnk_0200E600[a].unk1C[gUnk_03002464];
        }
        for (i = 0; i <= 16; i++)
        {
            if ((gUnk_0200E600[a].unk20[gUnk_03002464] & (1 << i)) != 0)
                gUnk_0200E600[a].unk12[gUnk_03002464]++;
        }
        if (gUnk_0200E600[a].unk12[gUnk_03002464] == 100)
            gUnk_0200E600[a].unk10 |= 4 << gUnk_03002464;
    }
}
