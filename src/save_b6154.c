#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* save_b6154.c (0x080B6154-0x080B628F, issue #94).
 *
 * HBlank wavy-scroll table driver, the fade variant of the four-loop family
 * (src/save_b6290.c, src/save_b63a4.c, src/save_b6474.c): the first 24 lines
 * and lines 136-151 get 256 - 16 * frame, the 7x8 grid between them
 * 256 -/+ 16 * frame, and the BLDALPHA shadows gBldAlphaEva/gBldAlphaEvb
 * fade with the frame counter gHBlankScrollTimer until it reaches 16.
 *
 * Matching note: the plain subscripted loops with the counter read inline at
 * every use are the source (lesson 3.465); the ROM's preheader order
 * [table, 256, &gHBlankScrollTimer] comes from agbcc's second loop pass. */

extern u8 gHBlankScrollState;
extern s32 gHBlankScrollTimer;
extern u16 gHBlankScrollTable[];
extern void (*gFrameCallback)(void);
extern u32 gHBlankDmaDest;
extern vu8 gBldAlphaEva;
extern vu16 gDispCnt;
extern vu8 gBldAlphaEvb;
extern vs32 gBg1ScrollX;

s32 sub_080b6154(void)
{
    s32 i;
    s32 n;
    s32 n2;
    s32 vt;
    u16 *base;
    vs32 *pe;
    u32 *pc;

    if (gHBlankScrollState == 2 || gHBlankScrollTimer == 16)
    {
        gFrameCallback = NULL;
        gHBlankScrollTimer = 0;
        gBg1ScrollX = 0;
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1F00;
        gBldAlphaEva = 13;
        gBldAlphaEvb = 3;
    }
    else
    {
        if (gHBlankScrollTimer <= 13)
        {
            gBldAlphaEva = gHBlankScrollTimer;
            gBldAlphaEvb = 16 - gHBlankScrollTimer;
        }
        pe = &gBg1ScrollX;
        base = gHBlankScrollTable;
        pc = &gHBlankDmaDest;
        for (n = 0; n <= 23; n++)
            base[n] = 256 - (gHBlankScrollTimer << 4);
        for (n = 136; n < 152; n++)
            gHBlankScrollTable[n] = 256 - (gHBlankScrollTimer << 4);
        for (i = 0; i < 7; i++)
        {
            for (n2 = 0; n2 < 8; n2++)
            {
                gHBlankScrollTable[(i * 2 + 3) * 8 + n2] = 256 - (gHBlankScrollTimer << 4);
                gHBlankScrollTable[(i * 2 + 4) * 8 + n2] = 256 + (gHBlankScrollTimer << 4);
            }
        }
        vt = base[0] << 16;
        *pe = vt;
        *pc = 0x04000014;
        return 1;
    }
}
