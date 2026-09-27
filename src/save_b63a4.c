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
    s32 n2;
    s32 vt;
    s32 z;
    u16 *base;
    u16 *p1;
    u16 *e;
    u32 *pc;
    vs32 *pe;

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
        for (n = 136; n < 152; n++)
            gUnk_020164A0[n] = 0;
        for (i = 0; i < 7; i++)
        {
            for (n2 = 0; n2 < 8; n2++)
            {
                gUnk_020164A0[(i * 2 + 3) * 8 + n2] = -gUnk_02016494 << 4;
                gUnk_020164A0[(i * 2 + 4) * 8 + n2] = gUnk_02016494 << 4;
            }
        }
        vt = base[0] << 16;
        *pe = vt;
        *pc = 0x04000014;
        return 1;
    }
}
