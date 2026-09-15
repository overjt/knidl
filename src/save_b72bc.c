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

extern struct LinkSave *gUnk_0200EC6C;
extern s32 Div(s32 a, s32 b);
extern u8 gUnk_02000020;
extern u16 gUnk_02004B50[];
extern u16 gUnk_02005580;
extern s16 gUnk_02005588[];
extern u16 gUnk_020055E0;
extern u8 gUnk_02006170;
extern u8 gUnk_020069F0;
extern u32 gUnk_02007BF0[8][8];
extern s16 gUnk_02007D48[];
extern u8 gUnk_02007D58[];
extern u16 gUnk_0200AEF0;
extern u16 gUnk_0200AF18[];
extern u8 gUnk_0200B04C;
extern u8 gUnk_0200EC68[];
extern u16 gUnk_0200EC70[];
extern u16 gUnk_0200EC78[];
extern u16 gUnk_02016480[];
extern u16 gUnk_03000FAC;
extern u32 gUnk_03000FB4;
extern u16 gUnk_03001EA4;
extern u16 gUnk_03001F18[];
extern u8 gUnk_03001F20;
extern u8 gUnk_03001F30;
extern u16 gUnk_0300235C;
extern u16 gUnk_03002360;
extern u16 gUnk_03002364;
extern u16 gUnk_03002378[];
extern s8 gUnk_03002384;
extern s8 gUnk_0300238C;
extern u16 gUnk_030023AC;
extern u8 gUnk_030023B0;
extern u8 gUnk_030023B8;
extern s32 gUnk_030023C8[];
extern s8 gUnk_030023E0;
extern u8 gUnk_030023EC;
extern u8 gUnk_03002400[8][7];
extern u8 gUnk_03002464;
extern u8 gUnk_03002468;

void sub_080b72bc(void)
{
    s32 i;
    s32 j;
    s32 n;
    s32 m;

    gUnk_03000FB4 = gUnk_0200EC6C->unk00;
    gUnk_03000FAC = gUnk_0200EC6C->unk04;
    gUnk_03001EA4 = gUnk_0200EC6C->unk06;
    gUnk_0300238C = gUnk_0200EC6C->unk08;
    gUnk_030023EC = gUnk_0200EC6C->unk09;
    gUnk_03002468 = gUnk_0200EC6C->unk0A;
    gUnk_030023AC = gUnk_0200EC6C->unk0C;
    gUnk_020069F0 = gUnk_0200EC6C->unk12;
    gUnk_020055E0 = gUnk_0200EC6C->unk0E;
    gUnk_0200AEF0 = gUnk_0200EC6C->unk10;
    gUnk_02000020 = gUnk_0200EC6C->unk13;
    gUnk_02005580 = gUnk_0200EC6C->unk14;
    gUnk_0200B04C = gUnk_0200EC6C->unk36;
    gUnk_03001F30 = gUnk_0200EC6C->unk11E;
    for (i = 0; i <= 7; i++)
        gUnk_02007D58[i] = gUnk_0200EC6C->unk38[i];
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 7; j++)
            gUnk_02007BF0[i][j] = gUnk_0200EC6C->unk40[i][j];
    }
    for (i = 0; i < gUnk_030023AC; i++)
    {
        gUnk_02016480[i] = i;
        gUnk_0200EC78[i] = i;
        gUnk_0200EC68[i] = 0;
        gUnk_02007D48[i] = gUnk_0200EC6C->unk16[i];
        gUnk_02005588[i] = gUnk_0200EC6C->unk1E[i];
        gUnk_02004B50[i] = gUnk_0200EC6C->unk26[i];
        gUnk_0200AF18[i] = gUnk_0200EC6C->unk2E[i];
    }
    n = Div(0x3B6A, gUnk_030023AC);
    asm("" ::: "r0", "r1", "r2");
    for (i = 0; i < gUnk_030023AC; i++)
    {
        asm("" ::: "r1");
        gUnk_0200EC70[i] = n - 4;
    }
    if (gUnk_030023B0 == 0)
    {
        gUnk_03002360 = gUnk_0200EC6C->unk11C;
        gUnk_03002464 = gUnk_0200EC6C->unk0B;
        gUnk_03002364 = gUnk_0200EC6C->unkC0;
        gUnk_030023B8 = gUnk_0200EC6C->unkC4[gUnk_03002464];
        gUnk_03001F20 = gUnk_0200EC6C->unkC6[gUnk_03002464];
        gUnk_030023E0 = gUnk_0200EC6C->unkC8[gUnk_03002464];
        gUnk_03002384 = gUnk_0200EC6C->unkCA[gUnk_03002464];
        gUnk_030023C8[0] = gUnk_0200EC6C->unkCC[gUnk_03002464];
    }
    for (i = 0; i <= 7; i++)
    {
        for (j = 0; j <= 6; j++)
            gUnk_03002400[i][j] = (gUnk_0200EC6C->unkD4[i][j] >> (gUnk_03002464 * 4)) & 15;
    }
    for (i = 0; i <= 3; i++)
    {
        gUnk_03001F18[i] = gUnk_0200EC6C->unk10C[i];
        gUnk_03002378[i] = gUnk_0200EC6C->unk114[i];
    }
    gUnk_0300235C = gUnk_0200EC6C->unkC2;
    gUnk_02006170 = 1;
}
