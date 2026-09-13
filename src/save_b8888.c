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
extern void sub_08002338(void);
extern void sub_08002348(void);
extern void sub_08002d18(void);

void sub_080b79b8(s32 a);
s32 sub_080b7a9c(s32 a);
u32 sub_080b7df4(s32 a);
void sub_080b7e14(s32 a);
void sub_080b8b2c(s32 a);

void sub_080b8888(void)
{
    s32 n;
    s32 i;

    sub_08002348();
    gUnk_0200EB80 = 0;
    do
    {
        sub_08002d18();
        n = 0;
        if (gUnk_0200EB80 <= 26)
        {
            gUnk_0200EB80++;
        }
        else
        {
            for (i = 0; i < gUnk_030023AC; i++)
            {
                if ((gUnk_03004D50[i] & 0xFF00) == (204 << 7) && (gUnk_03004D50[i] & 255) == 27)
                    n++;
            }
        }
    } while (n != gUnk_030023AC);
    sub_08002338();
}
