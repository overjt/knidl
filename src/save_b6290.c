#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u8 gUnk_02016490;
extern s32 gUnk_02016494;
extern u16 gUnk_020164A0[];
extern void (*gUnk_0300003C)(void);
extern u32 gUnk_0300101C;
extern vu8 gUnk_03000B08;
extern vu16 gUnk_03001ED8;
extern vu8 gUnk_03001EAC;
extern vs32 gUnk_03001EE0;

/* `register u32 wv asm("r0")` for the final `*pc = 0x04000014;` is a placeholder
   for a source shape nobody has identified (lesson 4.11): without it the pair
   comes out `ldr r1, =0x4000014; mov r0, r9` instead of the ROM's
   `ldr r0, =0x4000014; mov r1, r9`.  The same lever closed `sub_080b6d04`.

   The two loop counters are NOT interchangeable (lesson 4.55): `n` covers the
   first two loops and the third loop's outer index, which never overlap, while
   `n2` covers the third loop's inner body, which is live where the index is.
   Sharing one counter invents a conflict the ROM does not have and rotates
   every low register.  */

s32 sub_080b6290(void)
{
    s32 i;
    s32 j;
    s32 k;
    s32 n;
    s32 n2;
    s32 t;
    s32 v1;
    s32 v2;
    s32 vt;
    register u32 wv asm("r0");
    u16 *base;
    u16 *p;
    u16 *q;
    u8 *b;
    vs32 *pe;
    u32 *pc;

    if (gUnk_02016490 == 2 || gUnk_02016494 == 16)
    {
        gUnk_0300003C = NULL;
        gUnk_02016494 = 0;
        gUnk_03001EE0 = 0x01000000;
        gUnk_03001ED8 &= 0xFDFF;
    }
    else
    {
        if (gUnk_02016494 <= 13)
        {
            gUnk_03000B08 = 13 - gUnk_02016494;
            gUnk_03001EAC = 16 - gUnk_03000B08;
        }
        pe = &gUnk_03001EE0;
        base = gUnk_020164A0;
        pc = &gUnk_0300101C;
        for (n = 0; n <= 23; n++)
        {
            base[n] = -*(vs32 *)&gUnk_02016494 << 4;
        }
        for (n = 136; n < 152; n++)
            gUnk_020164A0[n] = -gUnk_02016494 << 4;
        for (i = 0; i <= 6; i = j)
        {
            k = 2 * i;
            j = i + 1;
            v1 = k + 3;
            v2 = k + 4;
            b = (u8 *)gUnk_020164A0;
            q = (u16 *)((v2 << 4) + (u32)b);
            p = (u16 *)((v1 << 4) + (u32)b);
            n2 = 7;
            do
            {
                t = gUnk_02016494;
                *p = -t << 4;
                *q = t << 4;
                q++;
                p++;
            } while (--n2 >= 0);
        }
        vt = base[0] << 16;
        *pe = vt;
        wv = 0x04000014;
        *pc = wv;
        return 1;
    }
}
