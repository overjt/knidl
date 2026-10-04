#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "mode.h"
#include "ending.h"

/* boot_caa3c.c (0x080CAA3C-0x080CAAB7, issue #100).
 *
 * The boot logo's 115 script-driven sprite objects (gUnk_02030000[],
 * struct M38LogoObj): BootLogoInitObjects, which M02's logo sequence PlayBootLogo
 * calls once, seeds them from the s16 stream gBootLogoObjectSeeds (script id, wait,
 * x, y per object; the draw layer follows y) and clears their saved script
 * cursors gBootLogoSavedCursors[].  Their per-frame interpreter BootLogoUpdateObjects
 * (called by M02's task type #0) is src/boot_caab8.c. */

/* Seed the 115 boot-logo objects from gBootLogoObjectSeeds[] (script id, wait,
   x, y per object; the layer follows y). */
void BootLogoInitObjects(void)
{
    s16 *s = gBootLogoObjectSeeds;
    struct M38LogoObj *obj = gUnk_02030000;
    s32 i;

    for (i = 0; i < 115; i++) {
        obj->scriptId = *s;
        obj->scriptPos = gBootLogoScripts[*s++];
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
        gBootLogoSavedCursors[i] = 0;
    }
}
