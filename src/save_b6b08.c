#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "camera.h"
#include "save.h"

/* save_b6b08.c (0x080B6B08-0x080B6C3F, issue #94).
 *
 * HBlank wavy-scroll table driver, the sine variant of src/save_b6d04.c: a
 * table of three halfwords per line for REG_BG2HOFS/BG2VOFS/BG3HOFS
 * (gHBlankDmaDest = REG_ADDR_BG2HOFS).  Every eighth line picks a new offset from
 * the s8 wave table gUnk_087561CC scaled by the frame counter gHBlankScrollTimer
 * (which wraps at 0x200), and each of the 160 lines gets the offset, the
 * fixed BG2VOFS value from its shadow gBg2ScrollY and the offset again.
 *
 * Matching note: the ROM keeps &gHBlankScrollTimer in one register from the wrap
 * test to the loop, which is a pointer local (a direct reference makes gcse
 * copy the address into a second register, lesson 3.354); the plain u32
 * local for the fixed scroll is what the allocator spills to [sp]. */

/* Not from main.h: this file's view of gBg2ScrollY differs (lesson 3.517). */
extern void (*gFrameCallback)(void);
extern vs32 gBg3ScrollX;
extern vs32 gBg2ScrollX;
extern vu32 gBg2ScrollY;
extern s16 gSpriteCameraY;

s32 sub_080b6b08(void)
{
    u16 i;
    s32 n;
    u16 v;
    u32 w;
    u16 *p;
    vs32 *pg;

    if (gHBlankScrollState == 3)
    {
        gBg2ScrollX = gHBlankScrollTable[0] << 16;
        gBg3ScrollX = gHBlankScrollTable[2] << 16;
        gHBlankDmaDest = REG_ADDR_BG2HOFS;
        return 3;
    }
    if (gHBlankScrollState == 2)
    {
        gFrameCallback = NULL;
        gHBlankScrollTimer = 0;
    }
    else
    {
        pg = (vs32 *)&gHBlankScrollTimer;
        if (*pg > 0x1FF)
            *pg = 0;
        i = gSpriteCameraY;
        w = gBg2ScrollY >> 16;
        v = ((gUnk_087561CC[i >> 3] * *pg) >> 1) + gHBlankScrollBaseX;
        p = gHBlankScrollTable;
        for (n = 0; n < 160; n++)
        {
            if ((i & 7) == 0)
                v = ((gUnk_087561CC[i >> 3] * *pg) >> 1) + gHBlankScrollBaseX;
            *p++ = v;
            *p++ = w;
            *p++ = v;
            i++;
        }
        gBg2ScrollX = gHBlankScrollTable[0] << 16;
        gBg3ScrollX = gHBlankScrollTable[2] << 16;
        gHBlankDmaDest = REG_ADDR_BG2HOFS;
        return 3;
    }
}
