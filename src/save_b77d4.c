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

extern struct SaveSlot gUnk_0200E600[];
extern u32 gUnk_0200E900[];
extern s16 gUnk_0200EC58;
extern s32 gUnk_03001EA8;
extern s16 gUnk_0300235C;
extern u16 gUnk_03002364;
extern s8 gUnk_03002384;
extern s32 gUnk_030023C8[];
extern s8 gUnk_030023E0;
extern s32 gUnk_030023E8;

void sub_080b75a4(void);
void sub_080b76a8(void);
void sub_080b78e4(void);
s32 sub_080b7918(s32 a, s32 b);
void sub_080b7b20(s32 a);
void sub_080b7d94(s32 a);
u32 sub_080b7dd0(s32 a);
void sub_080b8200(void);

void sub_080b77d4(void)
{
    switch (gUnk_0200EC58)
    {
    default:
        break;
    case 1:
        sub_080b75a4();
        break;
    case 2:
    case 3:
        sub_080b76a8();
        break;
    }
}
void sub_080b7800(void)
{
    s32 i;
    s32 j;
    s32 off;
    s32 r;
    u32 mask;
    u32 *p;

    gUnk_03001EA8 = 1;
    for (i = 0; i <= 2; i++)
        sub_080b7d94(i);
    mask = 0;
    i = 0;
    p = gUnk_0200E900;
    for (; i <= 2; i++)
    {
        r = sub_080b7918(3, i);
        if (*p != 0x99999999 || r != 0)
            mask |= 1 << i;
    }
    if (mask != 0 && gUnk_03001EA8 != 0)
    {
        sub_080b7d94(3);
        for (i = 0, off = 0; i <= 3; i++)
        {
            if (((mask >> i) & 1) != 0)
            {
                for (j = 0; j < 2; j++)
                    WriteSramEx((u8 *)gUnk_0200E900, (u8 *)(off + j * 256 + 0x0E000200), 256);
            }
            off += 512;
        }
    }
    mask = 0;
    for (i = 0; i <= 2; i++)
    {
        if (sub_080b7918(i, i) != 0)
        {
            sub_080b7d94(i);
            mask += 1;
        }
    }
    if (mask != 0)
        gUnk_03001EA8 = 0;
    sub_080b78e4();
}
void sub_080b78e4(void)
{
    s32 i;
    u32 best;

    best = 0;
    gUnk_030023E8 = 0;
    for (i = 0; i <= 2; i++)
    {
        if (gUnk_0200E600[i].unk08 > best)
        {
            best = gUnk_0200E600[i].unk08;
            gUnk_030023E8 = i;
        }
    }
}
s32 sub_080b7918(s32 a, s32 b)
{
    s32 i;

    if (gUnk_03001EA8 != 0)
    {
        for (i = 0; i <= 1; i++)
        {
            ReadSram((u8 *)((((b * 2) + i) << 8) + 0x0E000200), (u8 *)&gUnk_0200E600[a], 256);
            if (sub_080b7dd0(a) == gUnk_0200E600[a].unk70)
                break;
        }
        if (i == 2)
            goto one;
    }
    return 0;
one:
    return 1;
}
void sub_080b798c(s32 a)
{
    gUnk_0200E600[a].unk04 = a;
    gUnk_0200E600[a].unk0C = 0;
    sub_080b8200();
    sub_080b7b20(a);
}
