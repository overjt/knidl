#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern void (*gUnk_0300003C)(void);
extern u8 gHBlankScrollState;
extern s32 gHBlankScrollTimer;
extern u16 gHBlankScrollTable[];
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000F8C;
extern u32 gHBlankDmaDest;
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

    if (gHBlankScrollState == 3)
    {
        gUnk_03000F8C = gHBlankScrollTable[0] << 16;
        gUnk_03000B78 = gHBlankScrollTable[2] << 16;
        gHBlankDmaDest = 0x04000018;
        return 3;
    }
    if (gHBlankScrollState == 2)
    {
        gUnk_0300003C = NULL;
        gHBlankScrollTimer = 0;
    }
    else
    {
        if (gHBlankScrollTimer > 127)
            gHBlankScrollTimer = 0;
        t0 = gUnk_03001E94;
        w = t0 >> 16;
        v = (u16)(gHBlankScrollTimer << 2);
        p = gHBlankScrollTable;
        pf = &gUnk_03000F8C;
        base = p;
        pb = &gUnk_03000B78;
        pc = &gHBlankDmaDest;
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
