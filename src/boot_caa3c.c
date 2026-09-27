#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* boot_caa3c.c (0x080CAA3C-0x080CAAB7, issue #100).
 *
 * The boot logo's 115 script-driven sprite objects (gUnk_02030000[],
 * struct M38LogoObj): BootLogoInitObjects, which M02's logo sequence PlayBootLogo
 * calls once, seeds them from the s16 stream gUnk_08757440 (script id, wait,
 * x, y per object; the draw layer follows y) and clears their saved script
 * cursors gUnk_0201BFD0[].  Their per-frame interpreter BootLogoUpdateObjects
 * (called by M02's task type #0) is src/boot_caab8.c. */

/* One boot-logo sprite object: a command script (BootLogoUpdateObjects) moving a
   sprite in 24.8 fixed point. */
struct M38LogoObj
{
    /*0x00*/ s16 *scriptPos;    /* script cursor */
    /*0x04*/ s16 scriptId;     /* script id, -1 = off */
    /*0x06*/ s16 spriteId;     /* sprite id (gUnk_087554B8), -1 = none */
    /*0x08*/ s16 layer;     /* layer */
    /*0x0A*/ s16 sleepFrames;     /* frames to wait */
    /*0x0C*/ s32 posX;     /* x << 8 */
    /*0x10*/ s32 posY;     /* y << 8 */
    /*0x14*/ s16 velX;     /* x velocity */
    /*0x16*/ s16 velY;     /* y velocity */
    /*0x18*/ s16 accelX;     /* x acceleration */
    /*0x1A*/ s16 accelY;     /* y acceleration */
    /*0x1C*/ s16 loopCount;     /* loop count */
    /*0x1E*/ u16 unk1E;
};

extern struct M38LogoObj gUnk_02030000[];
extern s16 *gUnk_0201BFD0[];
extern s16 gUnk_08757440[];
extern s16 *gUnk_087577D8[];

/* Seed the 115 boot-logo objects from gUnk_08757440[] (script id, wait,
   x, y per object; the layer follows y). */
void BootLogoInitObjects(void)
{
    s16 *s = gUnk_08757440;
    struct M38LogoObj *obj = gUnk_02030000;
    s32 i;

    for (i = 0; i < 115; i++) {
        obj->scriptId = *s;
        obj->scriptPos = gUnk_087577D8[*s++];
        obj->spriteId = 0xFFFF;
        obj->sleepFrames = *s++;
        obj->posX = *s++ << 8;
        obj->posY = *s << 8;
        obj->velX = 0;
        obj->velY = 0;
        obj->accelX = 0;
        obj->accelY = 0;
        obj->layer = 15 - ((*s++ - 1) >> 4);
        obj->loopCount = 0;
        obj++;
        gUnk_0201BFD0[i] = 0;
    }
}
