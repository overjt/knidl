#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* save_b6b08.c (0x080B6B08-0x080B6C3F, issue #94).
 *
 * HBlank wavy-scroll table driver, the sine variant of src/save_b6d04.c: a
 * table of three halfwords per line for REG_BG2HOFS/BG2VOFS/BG3HOFS
 * (gHBlankDmaDest = 0x04000018).  Every eighth line picks a new offset from
 * the s8 wave table gUnk_087561CC scaled by the frame counter gHBlankScrollTimer
 * (which wraps at 0x200), and each of the 160 lines gets the offset, the
 * fixed BG2VOFS value from its shadow gUnk_03001E94 and the offset again.
 *
 * Matching note: the ROM keeps &gHBlankScrollTimer in one register from the wrap
 * test to the loop, which is a pointer local (a direct reference makes gcse
 * copy the address into a second register, lesson 3.354); the plain u32
 * local for the fixed scroll is what the allocator spills to [sp]. */

extern u8 gHBlankScrollState;
extern s32 gHBlankScrollTimer;
extern u16 gHBlankScrollTable[];
extern s32 gUnk_02016C30;
extern void (*gUnk_0300003C)(void);
extern u32 gHBlankDmaDest;
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000F8C;
extern vu32 gUnk_03001E94;
extern u16 gUnk_030023E4;
extern s8 gUnk_087561CC[];

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
        pg = (vs32 *)&gHBlankScrollTimer;
        if (*pg > 0x1FF)
            *pg = 0;
        i = gUnk_030023E4;
        w = gUnk_03001E94 >> 16;
        v = ((gUnk_087561CC[i >> 3] * *pg) >> 1) + gUnk_02016C30;
        p = gHBlankScrollTable;
        for (n = 0; n < 160; n++)
        {
            if ((i & 7) == 0)
                v = ((gUnk_087561CC[i >> 3] * *pg) >> 1) + gUnk_02016C30;
            *p++ = v;
            *p++ = w;
            *p++ = v;
            i++;
        }
        gUnk_03000F8C = gHBlankScrollTable[0] << 16;
        gUnk_03000B78 = gHBlankScrollTable[2] << 16;
        gHBlankDmaDest = 0x04000018;
        return 3;
    }
}
