#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "hud.h"
#include "camera.h"
#include "enemy.h"

/* Not from save.h: this file's view of gHBlankDmaSrc differs (lesson 3.517). */
extern u16 *gHBlankDmaSrc;
extern u8 gHBlankScrollState;
extern s32 gHBlankScrollTimer;
extern u16 gHBlankScrollTable[];
extern s16 gHBlankScrollEffect;
extern u16 gHBlankScrollDmaTable[];
extern u32 gHBlankDmaDest;
extern u32 gHBlankDmaCnt;
extern s8 gUnk_087561CC[];

void UpdateRoomHBlankScroll(void)
{
    s32 r;
    s32 v;

    r = (s16)gHBlankScrollEffects[gHBlankScrollEffect]();
    gHBlankDmaSrc = &gHBlankScrollDmaTable[r];
    v = 0xA2600000 | r;
    gHBlankDmaCnt = v;
    gVBlankCallback = (u32)HBlankScrollVBlankCallback;
    if (gHBlankScrollState != 3 && gHBlankScrollTimer <= 0xFFFF)
        gHBlankScrollTimer++;
}
