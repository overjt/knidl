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

extern s32 Div(s32 a, s32 b);
extern s32 Mod(s32 a, s32 b);
extern u16 gUnk_02006068[];
extern struct SaveSlot gUnk_0200E600[];
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
extern u16 gUnk_03004D90[];

void sub_080b79b8(s32 a);
s32 sub_080b7a9c(s32 a);
u32 sub_080b7df4(s32 a);
void sub_080b7e14(s32 a);
void sub_080b8b2c(s32 a);

void sub_080b7e14(s32 a)
{
    s32 i;
    s32 j;

    gUnk_0200E600[a].unk16[gUnk_03002464] = gUnk_030023B8;
    gUnk_0200E600[a].unk18[gUnk_03002464] = gUnk_03001F20;
    gUnk_0200E600[a].unk1A[gUnk_03002464] = gUnk_030023E0;
    gUnk_0200E600[a].unk1C[gUnk_03002464] = gUnk_03002384;
    gUnk_0200E600[a].unk20[gUnk_03002464] = gUnk_030023C8[0];
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
        {
            gUnk_0200E600[a].unk28[i][j] &= 15 << ((gUnk_03002464 ^ 1) * 4);
            gUnk_0200E600[a].unk28[i][j] |= gUnk_03002400[i][j] << (gUnk_03002464 * 4);
        }
    }
    for (i = 0; i <= 3; i++)
    {
        gUnk_0200E600[a].unk60[i] = gUnk_03001F18[i];
        gUnk_0200E600[a].unk68[i] = gUnk_03002378[i];
    }
    sub_080b79b8(gUnk_03002464);
    gUnk_0200E600[a].unk12[gUnk_03002464] = gUnk_0300235C;
    gUnk_0200E600[a].unk10 = gUnk_03002364;
}
void sub_080b7f58(s32 a)
{
    s32 i;
    s32 j;

    for (i = 0; i <= 1; i++)
    {
        gUnk_0200E600[a].unk16[i] = gUnk_030023B8;
        gUnk_0200E600[a].unk18[i] = gUnk_03001F20;
        gUnk_0200E600[a].unk1A[i] = gUnk_030023E0;
        gUnk_0200E600[a].unk1C[i] = gUnk_03002384;
        gUnk_0200E600[a].unk20[i] = gUnk_030023C8[0];
        sub_080b79b8(i);
        gUnk_0200E600[a].unk12[i] = gUnk_0300235C;
    }
    gUnk_0200E600[a].unk10 = gUnk_03002364;
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gUnk_0200E600[a].unk28[i][j] = (gUnk_03002400[i][j] << 4) | gUnk_03002400[i][j];
    }
    for (i = 0; i <= 3; i++)
    {
        gUnk_0200E600[a].unk60[i] = gUnk_03001F18[i];
        gUnk_0200E600[a].unk68[i] = gUnk_03002378[i];
    }
}
void sub_080b8070(s32 a)
{
    s32 i;
    s32 j;

    if (a == -1)
        return;
    if (gUnk_030023AC != 1)
        a = 3;
    gUnk_03002364 = gUnk_0200E600[a].unk10;
    gUnk_030023B8 = gUnk_0200E600[a].unk16[gUnk_03002464];
    gUnk_03001F20 = gUnk_0200E600[a].unk18[gUnk_03002464];
    gUnk_030023E0 = gUnk_0200E600[a].unk1A[gUnk_03002464];
    gUnk_03002384 = gUnk_0200E600[a].unk1C[gUnk_03002464];
    gUnk_030023C8[0] = gUnk_0200E600[a].unk20[gUnk_03002464];
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gUnk_03002400[i][j] = (gUnk_0200E600[a].unk28[i][j] >> (gUnk_03002464 * 4)) & 15;
    }
    for (i = 0; i <= 3; i++)
    {
        gUnk_03001F18[i] = gUnk_0200E600[a].unk60[i];
        gUnk_03002378[i] = gUnk_0200E600[a].unk68[i];
    }
    sub_080b79b8(gUnk_03002464);
}
void sub_080b81a0(void)
{
    s32 i;
    s32 j;

    gUnk_030023B8 = 0;
    gUnk_03001F20 = 0;
    gUnk_030023E0 = 0;
    gUnk_03002384 = 0;
    gUnk_030023C8[0] = 0;
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gUnk_03002400[i][j] = 0;
    }
    sub_080b79b8(0);
}
void sub_080b8200(void)
{
    s32 i;
    s32 j;
    u16 *p;
    u16 *q;

    gUnk_03002464 = 0;
    gUnk_03002364 = 0;
    gUnk_030023B8 = 0;
    gUnk_03001F20 = 0;
    gUnk_030023E0 = 0;
    gUnk_03002384 = 0;
    gUnk_030023C8[0] = 0;
    gUnk_0300235C = 0;
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gUnk_03002400[i][j] = 0;
    }
    p = gUnk_03001F18;
    q = gUnk_03002378;
    q[0] = 0;
    p[0] = 0;
    q[1] = 0;
    p[1] = 0;
    q[2] = 0;
    p[2] = 0;
    q[3] = 0;
    p[3] = 0;
}
s32 sub_080b8290(void)
{
    s32 r;
    u16 v;

    r = 0;
    v = gUnk_03002364;
    if (((v >> gUnk_03002464) & 1) != 0 && (v & (16 << gUnk_03002464)) == 0)
    {
        v |= 16 << gUnk_03002464;
        gUnk_03002364 = v;
        r = 1;
    }
    v = gUnk_03002364;
    if ((v & (4 << gUnk_03002464)) != 0 && (v & (64 << gUnk_03002464)) == 0)
    {
        v |= 64 << gUnk_03002464;
        gUnk_03002364 = v;
        r |= 2;
    }
    if (r == 0)
        goto zero;
    if (gUnk_030023E8 == -1)
        return r;
    if (gUnk_030023AC == 1)
        sub_080b7e14(gUnk_030023E8);
    else
        sub_080b8b2c(gUnk_030023E8);
    gUnk_0200E600[gUnk_030023E8].unk0C++;
    sub_080b7df4(gUnk_030023E8);
    sub_080b7a9c(gUnk_030023E8);
    return r;
zero:
    return 0;
}
u32 sub_080b8348(void)
{
    if (gUnk_03001EA8 == 0)
        return 0;
    ReadSram((u8 *)0x0E000800, gUnk_0200EC80, 240 << 7);
}
u32 sub_080b8374(void)
{
    if (gUnk_03001EA8 == 0)
        return 0;
    return WriteSramEx(gUnk_0200EC80, (u8 *)0x0E000800, 240 << 7);
}
u32 sub_080b83a0(u8 *src, s32 i)
{
    return WriteSramEx(src, (u8 *)(i * 2 + 0x0E00092C), 2);
}
void sub_080b83b8(void)
{
    s32 i;
    s32 j;

    gUnk_0200E600[3].unk16[gUnk_03002464] = gUnk_0200E600[gUnk_030023E8].unk16[gUnk_03002464];
    gUnk_0200E600[3].unk18[gUnk_03002464] = gUnk_0200E600[gUnk_030023E8].unk18[gUnk_03002464];
    gUnk_0200E600[3].unk1A[gUnk_03002464] = gUnk_0200E600[gUnk_030023E8].unk1A[gUnk_03002464];
    gUnk_0200E600[3].unk1C[gUnk_03002464] = gUnk_0200E600[gUnk_030023E8].unk1C[gUnk_03002464];
    gUnk_0200E600[3].unk20[gUnk_03002464] = gUnk_0200E600[gUnk_030023E8].unk20[gUnk_03002464];
    gUnk_0200E600[3].unk12[gUnk_03002464] = gUnk_0200E600[gUnk_030023E8].unk12[gUnk_03002464];
    gUnk_0200E600[3].unk10 = gUnk_0200E600[gUnk_030023E8].unk10;
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gUnk_0200E600[3].unk28[i][j] = gUnk_0200E600[gUnk_030023E8].unk28[i][j];
    }
    for (i = 0; i <= 3; i++)
    {
        gUnk_0200E600[3].unk60[i] = gUnk_0200E600[gUnk_030023E8].unk60[i];
        gUnk_0200E600[3].unk68[i] = gUnk_0200E600[gUnk_030023E8].unk68[i];
    }
}
void sub_080b84f0(void)
{
    s32 q;
    s32 r;

    gUnk_03004D90[0] = gUnk_0200EB80 | (204 << 7);
    switch (gUnk_0200EB80)
    {
    case 0:
        break;
    case 1:
        gUnk_03004D90[1] = ((s8)gUnk_0200E600[gUnk_030023E8].unk16[gUnk_03002464] << 8)
                         | (s8)gUnk_0200E600[gUnk_030023E8].unk18[gUnk_03002464];
        gUnk_03004D90[2] = ((s8)gUnk_0200E600[gUnk_030023E8].unk1A[gUnk_03002464] << 8)
                         | (s8)gUnk_0200E600[gUnk_030023E8].unk1C[gUnk_03002464];
        gUnk_03004D90[3] = gUnk_0200E600[gUnk_030023E8].unk12[gUnk_03002464];
        break;
    case 2:
        gUnk_03004D90[1] = gUnk_0200E600[gUnk_030023E8].unk20[gUnk_03002464] >> 16;
        gUnk_03004D90[2] = gUnk_0200E600[gUnk_030023E8].unk20[gUnk_03002464];
        gUnk_03004D90[3] = gUnk_0200E600[gUnk_030023E8].unk10;
        break;
    default:
        q = Div(gUnk_0200EB80 - 3, 3);
        r = Mod(gUnk_0200EB80 - 3, 3);
        if (q > 7)
            break;
        switch (r)
        {
        case 0:
            gUnk_03004D90[1] = gUnk_0200E600[gUnk_030023E8].unk28[q][0];
            gUnk_03004D90[2] = gUnk_0200E600[gUnk_030023E8].unk28[q][1];
            gUnk_03004D90[3] = gUnk_0200E600[gUnk_030023E8].unk28[q][2];
            break;
        case 1:
            gUnk_03004D90[1] = gUnk_0200E600[gUnk_030023E8].unk28[q][3];
            gUnk_03004D90[2] = gUnk_0200E600[gUnk_030023E8].unk28[q][4];
            gUnk_03004D90[3] = gUnk_0200E600[gUnk_030023E8].unk28[q][5];
            break;
        case 2:
            gUnk_03004D90[1] = gUnk_0200E600[gUnk_030023E8].unk28[q][6];
            break;
        }
        break;
    }
}
