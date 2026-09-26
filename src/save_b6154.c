#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* save_b6154.c (0x080B6154-0x080B628F, issue #94).
 *
 * HBlank wavy-scroll table driver, the fade variant of the four-loop family
 * (src/save_b6290.c, src/save_b63a4.c, src/save_b6474.c): the first 24 lines
 * and lines 136-151 get 256 - 16 * frame, the 7x8 grid between them
 * 256 -/+ 16 * frame, and the BLDALPHA shadows gUnk_03000B08/gUnk_03001EAC
 * fade with the frame counter gUnk_02016494 until it reaches 16.
 *
 * Matching note: the plain subscripted loops with the counter read inline at
 * every use are the source (lesson 3.465); the ROM's preheader order
 * [table, 256, &gUnk_02016494] comes from agbcc's second loop pass. */

extern u8 gUnk_02016490;
extern s32 gUnk_02016494;
extern u16 gUnk_020164A0[];
extern void (*gUnk_0300003C)(void);
extern u32 gUnk_0300101C;
extern vu8 gUnk_03000B08;
extern vu16 gUnk_03001ED8;
extern vu8 gUnk_03001EAC;
extern vs32 gUnk_03001EE0;

s32 sub_080b6154(void)
{
    s32 i;
    s32 n;
    s32 n2;
    s32 vt;
    u16 *base;
    vs32 *pe;
    u32 *pc;

    if (gUnk_02016490 == 2 || gUnk_02016494 == 16)
    {
        gUnk_0300003C = NULL;
        gUnk_02016494 = 0;
        gUnk_03001EE0 = 0;
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1F00;
        gUnk_03000B08 = 13;
        gUnk_03001EAC = 3;
    }
    else
    {
        if (gUnk_02016494 <= 13)
        {
            gUnk_03000B08 = gUnk_02016494;
            gUnk_03001EAC = 16 - gUnk_02016494;
        }
        pe = &gUnk_03001EE0;
        base = gUnk_020164A0;
        pc = &gUnk_0300101C;
        for (n = 0; n <= 23; n++)
            base[n] = 256 - (gUnk_02016494 << 4);
        for (n = 136; n < 152; n++)
            gUnk_020164A0[n] = 256 - (gUnk_02016494 << 4);
        for (i = 0; i < 7; i++)
        {
            for (n2 = 0; n2 < 8; n2++)
            {
                gUnk_020164A0[(i * 2 + 3) * 8 + n2] = 256 - (gUnk_02016494 << 4);
                gUnk_020164A0[(i * 2 + 4) * 8 + n2] = 256 + (gUnk_02016494 << 4);
            }
        }
        vt = base[0] << 16;
        *pe = vt;
        *pc = 0x04000014;
        return 1;
    }
}
