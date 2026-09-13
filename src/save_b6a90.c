#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern void (*gUnk_0300003C)(void);
extern s16 (*gUnk_08756198[])(void);
extern u16 *gUnk_03001EF0;
extern u8 gUnk_02016490;
extern s32 gUnk_02016494;
extern u16 gUnk_020164A0[];
extern s16 gUnk_02016860;
extern u16 gUnk_02016870[];
extern s32 gUnk_02016C30;
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000F8C;
extern u32 gUnk_03000FA4;
extern u32 gUnk_0300101C;
extern u32 gUnk_03001184;
extern u32 gUnk_03001E94;
extern u16 gUnk_030023E4;
extern s8 gUnk_087561CC[];

void sub_080b603c(void);

void sub_080b6a90(void)
{
    s32 r;
    s32 v;

    r = (s16)gUnk_08756198[gUnk_02016860]();
    gUnk_03001EF0 = &gUnk_02016870[r];
    v = 0xA2600000 | r;
    gUnk_03001184 = v;
    gUnk_03000FA4 = (u32)sub_080b603c;
    if (gUnk_02016490 != 3 && gUnk_02016494 <= 0xFFFF)
        gUnk_02016494++;
}
