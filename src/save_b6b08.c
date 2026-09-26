#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* save_b6b08.c (0x080B6B08-0x080B6C3F, issue #94).
 *
 * HBlank wavy-scroll table driver, the sine variant of src/save_b6d04.c: a
 * table of three halfwords per line for REG_BG2HOFS/BG2VOFS/BG3HOFS
 * (gUnk_0300101C = 0x04000018).  Every eighth line picks a new offset from
 * the s8 wave table gUnk_087561CC scaled by the frame counter gUnk_02016494
 * (which wraps at 0x200), and each of the 160 lines gets the offset, the
 * fixed BG2VOFS value from its shadow gUnk_03001E94 and the offset again.
 *
 * Matching note: the ROM keeps &gUnk_02016494 in one register from the wrap
 * test to the loop, which is a pointer local (a direct reference makes gcse
 * copy the address into a second register, lesson 3.354); the plain u32
 * local for the fixed scroll is what the allocator spills to [sp]. */

extern u8 gUnk_02016490;
extern s32 gUnk_02016494;
extern u16 gUnk_020164A0[];
extern s32 gUnk_02016C30;
extern void (*gUnk_0300003C)(void);
extern u32 gUnk_0300101C;
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
        pg = (vs32 *)&gUnk_02016494;
        if (*pg > 0x1FF)
            *pg = 0;
        i = gUnk_030023E4;
        w = gUnk_03001E94 >> 16;
        v = ((gUnk_087561CC[i >> 3] * *pg) >> 1) + gUnk_02016C30;
        p = gUnk_020164A0;
        for (n = 0; n < 160; n++)
        {
            if ((i & 7) == 0)
                v = ((gUnk_087561CC[i >> 3] * *pg) >> 1) + gUnk_02016C30;
            *p++ = v;
            *p++ = w;
            *p++ = v;
            i++;
        }
        gUnk_03000F8C = gUnk_020164A0[0] << 16;
        gUnk_03000B78 = gUnk_020164A0[2] << 16;
        gUnk_0300101C = 0x04000018;
        return 3;
    }
}
