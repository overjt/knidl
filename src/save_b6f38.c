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
    /*0x11F*/ u8 pad11F[0xD];
    /*0x12C*/ u16 unk12C[0x3B6A];
};

extern struct LinkSave *gUnk_0200EC50;
extern struct LinkSave *gUnk_0200EC6C;
extern s32 Div(s32 a, s32 b);
extern u8 gUnk_02000020;
extern u16 gUnk_02004B50[];
extern u16 gUnk_02005580;
extern u16 gUnk_02005588[];
extern u8 gUnk_020069F0;
extern u32 gUnk_02007BF0[8][8];
extern u16 gUnk_02007D48[];
extern u8 gUnk_02007D58[];
extern u16 gUnk_0200AF18[];
extern u8 gUnk_0200B04C;
extern u8 gUnk_0200EC54;
extern s16 gUnk_0200EC58;
extern u16 gUnk_0200EC60[];
extern u8 gUnk_0200EC68[];
extern u16 gUnk_0200EC70[];
extern u16 gUnk_0200EC78[];
extern struct LinkSave gUnk_0200EC80;
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
extern u8 gUnk_03002384;
extern u8 gUnk_0300238C;
extern u16 gUnk_030023AC;
extern u8 gUnk_030023B8;
extern u32 gUnk_030023C8[];
extern u8 gUnk_030023E0;
extern u8 gUnk_030023EC;
extern u8 gUnk_03002400[8][7];
extern u8 gUnk_03002464;
extern u8 gUnk_03002468;
extern void sub_080b72bc(void);
extern void sub_080b8348(void);
extern void sub_080b8374(void);

void sub_080b6f38(void)
{
    s32 i;
    s32 j;
    s32 n;
    u32 *src;
    u8 *d;
    s32 off;

    gUnk_0200EC54 = 1;
    switch (gUnk_0200EC58)
    {
    case 1:
        gUnk_0200EC6C = &gUnk_0200EC80;
        for (i = 0; i <= 0x3B69; i++)
            gUnk_0200EC6C->unk12C[i] = 0;
        gUnk_0200EC6C->unk00 = gUnk_03000FB4;
        gUnk_0200EC6C->unk04 = gUnk_03000FAC;
        gUnk_0200EC6C->unk06 = gUnk_03001EA4;
        gUnk_0200EC6C->unk08 = gUnk_0300238C;
        gUnk_0200EC6C->unk09 = gUnk_030023EC;
        gUnk_0200EC6C->unk0A = gUnk_03002468;
        gUnk_0200EC6C->unk0B = gUnk_03002464;
        gUnk_0200EC6C->unk0C = gUnk_030023AC;
        gUnk_0200EC6C->unk12 = gUnk_020069F0;
        gUnk_0200EC6C->unk13 = gUnk_02000020;
        gUnk_0200EC6C->unk14 = gUnk_02005580;
        gUnk_0200EC6C->unk36 = gUnk_0200B04C;
        gUnk_0200EC6C->unk11C = gUnk_03002360;
        gUnk_0200EC6C->unk11E = gUnk_03001F30;
        for (i = 0; i <= 7; i++)
        {
            asm("" ::: "r4");
            gUnk_0200EC6C->unk38[i] = gUnk_02007D58[i];
        }
        for (i = 0; i <= 7; i++)
        {
            d = (u8 *)gUnk_0200EC6C->unk40;
            src = gUnk_02007BF0[i];
            off = i * 16;
            n = 7;
            do
            {
                d = (u8 *)gUnk_0200EC6C->unk40;
                *(u16 *)(d + off) = *src++;
                off += 2;
            } while (--n >= 0);
        }
        for (i = 0; i < gUnk_030023AC; i++)
        {
            gUnk_0200EC78[i] = i;
            gUnk_0200EC60[i] = 0xFFFF;
            gUnk_0200EC68[i] = 0;
            gUnk_0200EC6C->unk16[i] = gUnk_02007D48[i];
            gUnk_0200EC6C->unk1E[i] = gUnk_02005588[i];
            gUnk_0200EC6C->unk26[i] = gUnk_02004B50[i];
            gUnk_0200EC6C->unk2E[i] = gUnk_0200AF18[i];
        }
        gUnk_0200EC6C->unkC0 = gUnk_03002364;
        gUnk_0200EC6C->unkC4[gUnk_03002464] = gUnk_030023B8;
        gUnk_0200EC6C->unkC6[gUnk_03002464] = gUnk_03001F20;
        gUnk_0200EC6C->unkC8[gUnk_03002464] = gUnk_030023E0;
        gUnk_0200EC6C->unkCA[gUnk_03002464] = gUnk_03002384;
        gUnk_0200EC6C->unkCC[gUnk_03002464] = gUnk_030023C8[0];
        for (i = 0; i <= 7; i++)
        {
            for (j = 0; j <= 6; j++)
            {
                gUnk_0200EC6C->unkD4[i][j] &= 15 << ((1 ^ gUnk_03002464) * 4);
                gUnk_0200EC6C->unkD4[i][j] |= gUnk_03002400[i][j] << (gUnk_03002464 * 4);
            }
        }
        for (i = 0; i <= 3; i++)
        {
            gUnk_0200EC6C->unk10C[i] = gUnk_03001F18[i];
            gUnk_0200EC6C->unk114[i] = gUnk_03002378[i];
        }
        gUnk_0200EC6C->unkC2 = gUnk_0300235C;
        sub_080b8374();
        n = Div(0x3B6A, gUnk_030023AC);
        asm("" ::: "r3");
        for (i = 0; i < gUnk_030023AC; i++)
            gUnk_0200EC70[i] = n - 4;
        break;
    case 2:
        sub_080b8348();
        gUnk_0200EC6C = &gUnk_0200EC80;
        sub_080b72bc();
        break;
    case 3:
        gUnk_0200EC6C = gUnk_0200EC50;
        sub_080b72bc();
        break;
    }
}
