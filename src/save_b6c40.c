#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "save.h"

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
        gBg2ScrollX = gHBlankScrollTable[0] << 16;
        gBg3ScrollX = gHBlankScrollTable[2] << 16;
        gHBlankDmaDest = 0x04000018;
        return 3;
    }
    if (gHBlankScrollState == 2)
    {
        gFrameCallback = NULL;
        gHBlankScrollTimer = 0;
    }
    else
    {
        if (gHBlankScrollTimer > 127)
            gHBlankScrollTimer = 0;
        t0 = gBg2ScrollY;
        w = t0 >> 16;
        v = (u16)(gHBlankScrollTimer << 2);
        p = gHBlankScrollTable;
        pf = &gBg2ScrollX;
        base = p;
        pb = &gBg3ScrollX;
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
