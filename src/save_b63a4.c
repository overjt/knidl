#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern void (*gFrameCallback)(void);
extern u8 gHBlankScrollState;
extern s32 gHBlankScrollTimer;
extern u16 gHBlankScrollTable[];
extern vu8 gBldAlphaEva;
extern u32 gHBlankDmaDest;
extern vu8 gBldAlphaEvb;
extern vu16 gDispCnt;
extern vs32 gBg1ScrollX;

s32 sub_080b63a4(void)
{
    s32 i;
    s32 n;
    s32 n2;
    s32 vt;
    s32 z;
    u16 *base;
    u16 *p1;
    u16 *e;
    u32 *pc;
    vs32 *pe;

    if (gHBlankScrollState == 2 || gHBlankScrollTimer == 16)
    {
        gFrameCallback = NULL;
        gHBlankScrollTimer = 0;
        gBg1ScrollX = 0;
    }
    else
    {
        pe = &gBg1ScrollX;
        base = gHBlankScrollTable;
        pc = &gHBlankDmaDest;
        p1 = base;
        z = 0;
        e = base + 23;
        do
        {
            *e = z;
            e--;
        } while ((s32)e >= (s32)p1);
        for (n = 136; n < 152; n++)
            gHBlankScrollTable[n] = 0;
        for (i = 0; i < 7; i++)
        {
            for (n2 = 0; n2 < 8; n2++)
            {
                gHBlankScrollTable[(i * 2 + 3) * 8 + n2] = -gHBlankScrollTimer << 4;
                gHBlankScrollTable[(i * 2 + 4) * 8 + n2] = gHBlankScrollTimer << 4;
            }
        }
        vt = base[0] << 16;
        *pe = vt;
        *pc = 0x04000014;
        return 1;
    }
}
