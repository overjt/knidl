#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern void (*gFrameCallback)(void);
extern u8 gUnk_0200B040;
extern u8 gHBlankScrollState;
extern s32 gHBlankScrollTimer;
extern u16 gHBlankScrollEffect;

void sub_080b60e8(void);
void sub_080b6a90(void);

void ResetHBlankScroll(void)
{
    gFrameCallback = NULL;
    gHBlankScrollTimer = 0;
    REG_DMA0CNT_H = 0;
}
void StopHBlankScroll(void)
{
    gHBlankScrollState = 2;
}
void StartHBlankScroll(s32 a)
{
    gHBlankScrollTimer = 0;
    gHBlankScrollState = 1;
    gHBlankScrollEffect = a;
    gFrameCallback = sub_080b60e8;
    gUnk_0200B040 = 1;
}
void sub_080b6ea0(s32 a)
{
    gHBlankScrollTimer = 0;
    gHBlankScrollState = 1;
    gHBlankScrollEffect = a;
    gFrameCallback = sub_080b6a90;
    gUnk_0200B040 = 1;
}
void sub_080b6ed4(void)
{
    if (gUnk_0200B040 != 0)
        gHBlankScrollState = 3;
}
void sub_080b6eec(void)
{
    if (gUnk_0200B040 != 0)
        gFrameCallback = NULL;
}
void sub_080b6f04(void)
{
    if (gUnk_0200B040 != 0)
        gFrameCallback = sub_080b6a90;
}
void sub_080b6f20(void)
{
    if (gUnk_0200B040 != 0)
        gHBlankScrollState = 1;
}
