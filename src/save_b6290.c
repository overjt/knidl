#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u8 gHBlankScrollState;
extern s32 gHBlankScrollTimer;
extern u16 gHBlankScrollTable[];
extern void (*gUnk_0300003C)(void);
extern u32 gHBlankDmaDest;
extern vu8 gUnk_03000B08;
extern vu16 gUnk_03001ED8;
extern vu8 gUnk_03001EAC;
extern vs32 gUnk_03001EE0;

/* The two loop counters are NOT interchangeable (lesson 4.55): `n` covers the
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
    u16 *base;
    u16 *p;
    u16 *q;
    u8 *b;
    vs32 *pe;
    u32 *pc;

    if (gHBlankScrollState == 2 || gHBlankScrollTimer == 16)
    {
        gUnk_0300003C = NULL;
        gHBlankScrollTimer = 0;
        gUnk_03001EE0 = 0x01000000;
        gUnk_03001ED8 &= 0xFDFF;
    }
    else
    {
        if (gHBlankScrollTimer <= 13)
        {
            gUnk_03000B08 = 13 - gHBlankScrollTimer;
            gUnk_03001EAC = 16 - gUnk_03000B08;
        }
        pe = &gUnk_03001EE0;
        base = gHBlankScrollTable;
        pc = &gHBlankDmaDest;
        for (n = 0; n <= 23; n++)
        {
            base[n] = -*(vs32 *)&gHBlankScrollTimer << 4;
        }
        for (n = 136; n < 152; n++)
            gHBlankScrollTable[n] = -gHBlankScrollTimer << 4;
        for (i = 0; i <= 6; i = j)
        {
            k = 2 * i;
            j = i + 1;
            v1 = k + 3;
            v2 = k + 4;
            b = (u8 *)gHBlankScrollTable;
            q = (u16 *)((v2 << 4) + (u32)b);
            p = (u16 *)((v1 << 4) + (u32)b);
            n2 = 7;
            do
            {
                t = gHBlankScrollTimer;
                *p = -t << 4;
                *q = t << 4;
                q++;
                p++;
            } while (--n2 >= 0);
        }
        vt = base[0] << 16;
        *pe = vt;
        *pc = 0x04000014;
        return 1;
    }
}
