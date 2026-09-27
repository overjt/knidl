#include "gba/gba.h"
#include "global.h"

/* OAM shadow builder (0x08001B08-0x08001CC7, issue #32 batch B1).
 *
 * Walks 16 priority buckets (gUnk_03000B30[i] = entry count, gUnk_03001680[i]
 * = 64 slot indices per bucket).  Each slot indexes a 12-byte sprite record in
 * gUnk_030004F0 holding:
 *      +0  flags   bit15 = "wide" OAM source stride, bits14-13 = priority,
 *                  bits11-10 = OBJ mode bits OR'd into attr0
 *      +2  flags   bits15-11 = palette bank / blend control,
 *                  bits10-0  = base tile number added to attr2
 *      +4  y bias, +6 x bias, +8 pointer to the OAM template stream
 * The template stream is copied into the OAM shadow at gUnk_03000050 with the
 * biases added (attr0 y in bits 7-0, attr1 x in bits 8-0, both wrapping in
 * their own field width) until a template entry has bit12 set ("last") or the
 * shadow fills up.  Afterwards gUnk_03000B04 keeps the write cursor,
 * gUnk_03001EC8 the number of entries used, and every unused OAM slot gets
 * attr0 = 236 (off-screen y) to hide it.
 *
 * NOTE (IWRAM): AgbInit copies these 0x1C0 bytes to 0x03001F40 with a
 * 256-halfword CpuSet and the game executes the RAM copy.  That is safe and
 * needs no special source treatment: the function makes no BL calls at all,
 * every branch is a PC-relative Thumb b/bcc, and every datum it touches is an
 * absolute IWRAM address loaded from the function's own literal pool -- which
 * sits inside the copied 0x1C0 bytes.
 *
 * Matching notes: plain source.  One u16 `v` carries every halfword the
 * function reads (both record headers and each template word), which is
 * why the ROM keeps them all in r3; the template walk is a goto loop (a
 * do/while lets loop.c hoist `pal & 0x800`), and the in-place ANDs and
 * the separate 255 constants come out of the u16 arithmetic by themselves.
 *
 * STATUS: byte-exact (448/448). */

extern u16 *gUnk_03000B04;       /* OAM shadow write cursor */
extern vu32 gUnk_03000B30[16];   /* per-bucket sprite counts */
extern u8 gUnk_03001680[16][64]; /* per-bucket sprite slot indices */
extern u16 gUnk_030004F0[][6];   /* 12-byte sprite records */
extern u16 gUnk_03000050[];      /* OAM shadow (128 entries * 4 halfwords) */
extern vu16 gUnk_03001EC8;       /* number of OAM entries used */

void sub_08001b08(void)
{
    u16 *dst;
    s32 i;
    u32 j;
    u16 *p;
    u16 v;
    u16 mode, pal, tile, last, x, y, flip;
    u32 prio;
    u32 *q;

    dst = gUnk_03000B04;
    for (i = 0; i < 16; i++)
    {
        if (gUnk_03000B30[i] != 0)
        {
            for (j = 0; j < gUnk_03000B30[i]; j++)
            {
                p = gUnk_030004F0[gUnk_03001680[i][j]];
                v = *p++;
                flip = v & 0x8000;
                prio = (v & 0x6000) >> 3;
                mode = v & 0xC00;
                v = *p++;
                pal = v & 0xF800;
                tile = v & 0x7FF;
                x = *p++;
                y = *p++;
                p = *(u16 **)p;
            next_oam:
                {
                    v = mode | *p++;
                    *dst++ = (v & 0xFF00) | ((y + (v & 0xFF)) & 0xFF);
                    last = v & 0x1000;
                    if (flip)
                    {
                        p++;
                        v = *p++;
                    }
                    else
                    {
                        v = *p;
                        p += 2;
                    }
                    *dst++ = (v & 0xFE00) | ((x + (v & 0x1FF)) & 0x1FF);
                    v = *p++;
                    if (pal != 0)
                    {
                        if (pal & 0x800)
                        {
                            if ((s32)((v + (pal & 0xF000)) & 0xF000) >= (pal & 0xF000))
                                v = (u16)(v + (pal & 0xF000));
                        }
                        else
                            v = (v & 0xFFF) | pal;
                    }
                    if (prio != 0)
                        v = (v & 0xF3FF) | prio;
                    *dst = v + tile;
                    dst += 2;
                    if (dst >= gUnk_03000050 + 512)
                        goto finish;
                    if (!last)
                        goto next_oam;
                }
            }
        }
    }

finish:
    gUnk_03000B04 = dst;
    gUnk_03001EC8 = ((u32)dst - (u32)gUnk_03000050) >> 3;
    for (q = (u32 *)dst; q < (u32 *)(gUnk_03000050 + 512); q += 2)
        *q = 236;
}
