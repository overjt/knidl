#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern void (*gUnk_0300003C)(void);
extern u8 gUnk_02016490;
extern s32 gUnk_02016494;
extern u16 gUnk_020164A0[];
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000F8C;
extern u32 gUnk_0300101C;
extern vu32 gUnk_03001E94;
extern vu16 gUnk_03001ED8;
extern vs32 gUnk_03001EE0;

s32 sub_080b6c40(void)
{
    s32 n;
    u32 v;
    u32 w;
    u32 t0;
    u16 *p;
    u16 *base;
    vs32 *pf;
    vs32 *pb;
    u32 *pc;

    if (gUnk_02016490 == 3)
    {
        gUnk_03000F8C = gUnk_020164A0[0] << 16;
        gUnk_03000B78 = gUnk_020164A0[2] << 16;
        gUnk_0300101C = 0x04000018;
        return 3;
    }
    if (gUnk_02016490 == 2)
    {
        gUnk_0300003C = NULL;
        gUnk_02016494 = 0;
    }
    else
    {
        if (gUnk_02016494 > 127)
            gUnk_02016494 = 0;
        t0 = gUnk_03001E94;
        w = t0 >> 16;
        v = (u16)(gUnk_02016494 << 2);
        p = gUnk_020164A0;
        pf = &gUnk_03000F8C;
        base = p;
        pb = &gUnk_03000B78;
        pc = &gUnk_0300101C;
        n = 159;
        do
        {
            *p = v;
            p++;
            *p = w;
            p++;
            *p = v;
            p++;
        } while (--n >= 0);
        *pf = base[0] << 16;
        *pb = base[2] << 16;
        *pc = 0x04000018;
        return 3;
    }
}
