#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern void (*gUnk_0300003C)(void);
extern u8 gUnk_02016490;
extern s32 gUnk_02016494;
extern u16 gUnk_020164A0[];
extern vu8 gUnk_03000B08;
extern u32 gUnk_0300101C;
extern vu8 gUnk_03001EAC;
extern vu16 gUnk_03001ED8;
extern vs32 gUnk_03001EE0;

s32 sub_080b63a4(void)
{
    s32 i;
    s32 n;
    s32 m;
    u16 *base;
    register vs32 *pe asm("r9");
    u32 *pc;
    u16 *p1;
    u16 *p2;
    u16 *p;
    u16 *q;
    u16 *e;
    s32 z;
    u8 *b;
    s32 k;
    s32 v1;
    s32 v2;
    s32 j;
    s32 vt;

    if (gUnk_02016490 == 2 || gUnk_02016494 == 16)
    {
        gUnk_0300003C = NULL;
        gUnk_02016494 = 0;
        gUnk_03001EE0 = 0;
    }
    else
    {
        pe = &gUnk_03001EE0;
        base = gUnk_020164A0;
        pc = &gUnk_0300101C;
        p1 = base;
        z = 0;
        e = base + 23;
        do
        {
            *e = z;
            e--;
        } while ((s32)e >= (s32)p1);
        for (m = 136; m < 152; m++)
            gUnk_020164A0[m] = 0;
        for (i = 0; i <= 6; i = j)
        {
            k = 2 * i;
            j = i + 1;
            v1 = k + 3;
            v2 = k + 4;
            b = (u8 *)gUnk_020164A0;
            q = (u16 *)(b + (v2 << 4));
            p = (u16 *)(b + (v1 << 4));
            n = 7;
            do
            {
                s32 t = gUnk_02016494;

                *p = -t << 4;
                *q = t << 4;
                q++;
                p++;
            } while (--n >= 0);
        }
        vt = base[0] << 16;
        *pe = vt;
        *pc = 0x04000014;
        return 1;
    }
}
