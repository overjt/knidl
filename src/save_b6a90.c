#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern void (*gFrameCallback)(void);
extern s16 (*gHBlankScrollEffects[])(void);
extern u16 *gHBlankDmaSrc;
extern u8 gHBlankScrollState;
extern s32 gHBlankScrollTimer;
extern u16 gHBlankScrollTable[];
extern s16 gHBlankScrollEffect;
extern u16 gHBlankScrollDmaTable[];
extern s32 gUnk_02016C30;
extern vs32 gBg3ScrollX;
extern vs32 gBg2ScrollX;
extern u32 gUnk_03000FA4;
extern u32 gHBlankDmaDest;
extern u32 gHBlankDmaCnt;
extern u32 gBg2ScrollY;
extern u16 gSpriteCameraY;
extern s8 gUnk_087561CC[];

void HBlankScrollVBlankCallback(void);

void sub_080b6a90(void)
{
    s32 r;
    s32 v;

    r = (s16)gHBlankScrollEffects[gHBlankScrollEffect]();
    gHBlankDmaSrc = &gHBlankScrollDmaTable[r];
    v = 0xA2600000 | r;
    gHBlankDmaCnt = v;
    gUnk_03000FA4 = (u32)HBlankScrollVBlankCallback;
    if (gHBlankScrollState != 3 && gHBlankScrollTimer <= 0xFFFF)
        gHBlankScrollTimer++;
}
