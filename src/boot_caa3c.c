#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* boot_caa3c.c (0x080CAA3C-0x080CAAB7, issue #100).
 *
 * The boot logo's 115 script-driven sprite objects (gUnk_02030000[],
 * struct M38LogoObj): sub_080caa3c, which M02's logo sequence sub_08009200
 * calls once, seeds them from the s16 stream gUnk_08757440 (script id, wait,
 * x, y per object; the draw layer follows y) and clears their saved script
 * cursors gUnk_0201BFD0[].  Their per-frame interpreter sub_080caab8
 * (0x080CAAB8-0x080CACEF, called by M02's task type #0) is still asm. */

/* One boot-logo sprite object: a command script (sub_080caab8) moving a
   sprite in 24.8 fixed point. */
struct M38LogoObj
{
    /*0x00*/ s16 *unk00;    /* script cursor */
    /*0x04*/ s16 unk04;     /* script id, -1 = off */
    /*0x06*/ s16 unk06;     /* sprite id (gUnk_087554B8), -1 = none */
    /*0x08*/ s16 unk08;     /* layer */
    /*0x0A*/ s16 unk0A;     /* frames to wait */
    /*0x0C*/ s32 unk0C;     /* x << 8 */
    /*0x10*/ s32 unk10;     /* y << 8 */
    /*0x14*/ s16 unk14;     /* x velocity */
    /*0x16*/ s16 unk16;     /* y velocity */
    /*0x18*/ s16 unk18;     /* x acceleration */
    /*0x1A*/ s16 unk1A;     /* y acceleration */
    /*0x1C*/ s16 unk1C;     /* loop count */
    /*0x1E*/ u16 unk1E;
};

extern struct M38LogoObj gUnk_02030000[];
extern s16 *gUnk_0201BFD0[];
extern s16 gUnk_08757440[];
extern s16 *gUnk_087577D8[];

/* Seed the 115 boot-logo objects from gUnk_08757440[] (script id, wait,
   x, y per object; the layer follows y). */
void sub_080caa3c(void)
{
    s16 *s = gUnk_08757440;
    struct M38LogoObj *obj = gUnk_02030000;
    s32 i;

    for (i = 0; i < 115; i++) {
        obj->unk04 = *s;
        obj->unk00 = gUnk_087577D8[*s++];
        obj->unk06 = 0xFFFF;
        obj->unk0A = *s++;
        obj->unk0C = *s++ << 8;
        obj->unk10 = *s << 8;
        obj->unk14 = 0;
        obj->unk16 = 0;
        obj->unk18 = 0;
        obj->unk1A = 0;
        obj->unk08 = 15 - ((*s++ - 1) >> 4);
        obj->unk1C = 0;
        obj++;
        gUnk_0201BFD0[i] = 0;
    }
}
