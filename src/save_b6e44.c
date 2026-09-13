#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern void (*gUnk_0300003C)(void);
extern u8 gUnk_0200B040;
extern u8 gUnk_02016490;
extern s32 gUnk_02016494;
extern u16 gUnk_02016860;

void sub_080b60e8(void);
void sub_080b6a90(void);

void sub_080b6e44(void)
{
    gUnk_0300003C = NULL;
    gUnk_02016494 = 0;
    REG_DMA0CNT_H = 0;
}
void sub_080b6e60(void)
{
    gUnk_02016490 = 2;
}
void sub_080b6e6c(s32 a)
{
    gUnk_02016494 = 0;
    gUnk_02016490 = 1;
    gUnk_02016860 = a;
    gUnk_0300003C = sub_080b60e8;
    gUnk_0200B040 = 1;
}
void sub_080b6ea0(s32 a)
{
    gUnk_02016494 = 0;
    gUnk_02016490 = 1;
    gUnk_02016860 = a;
    gUnk_0300003C = sub_080b6a90;
    gUnk_0200B040 = 1;
}
void sub_080b6ed4(void)
{
    if (gUnk_0200B040 != 0)
        gUnk_02016490 = 3;
}
void sub_080b6eec(void)
{
    if (gUnk_0200B040 != 0)
        gUnk_0300003C = NULL;
}
void sub_080b6f04(void)
{
    if (gUnk_0200B040 != 0)
        gUnk_0300003C = sub_080b6a90;
}
void sub_080b6f20(void)
{
    if (gUnk_0200B040 != 0)
        gUnk_02016490 = 1;
}
