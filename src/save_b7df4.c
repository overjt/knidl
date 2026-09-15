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

extern u16 gUnk_02006068[];
extern struct SaveSlot gUnk_0200E600[];
extern s32 gUnk_03001EA8;
extern u16 gUnk_030023AC;

s32 sub_080b7a9c(s32 a);
void sub_080b7d94(s32 a);
u32 sub_080b7dd0(s32 a);
u32 sub_080b7df4(s32 a);

u32 sub_080b7df4(s32 a)
{
    u32 c;
    register u32 d asm("r2");

    c = sub_080b7dd0(a);
    d = c;
    asm("" : "+r"(d));
    gUnk_0200E600[a].unk70 = d;
    return c;
}
