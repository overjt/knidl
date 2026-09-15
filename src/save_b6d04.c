#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u8 gUnk_02016490;
extern s32 gUnk_02016494;
extern u16 gUnk_020164A0[];
extern s32 gUnk_02016C30;
extern void (*gUnk_0300003C)(void);
extern u32 gUnk_0300101C;
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000F8C;
extern vu32 gUnk_03001E94;
extern u16 gUnk_030023E4;
extern s8 gUnk_087561CC[];

/* Three placeholders stand in for source shapes nobody has identified
   (lesson 4.11); all three are byte-verified levers, not guesses:

   - `asm("" ::: "r8", "ip")` after the loop forbids those two hi registers
     for the pseudos still live there (`pf`, `pb`), which is what rotates the
     four hi-register pointers into the ROM's r8/r9/sl/ip assignment
     (lesson 3.341);
   - `register u32 tw asm("r0")` puts the stored word in r0 as the ROM does;
     without it the final pair comes out `ldr r1, =0x4000018; ldr r0, =...`;
   - the tail store is spelled through the raw address because writing it as
     `gUnk_0300101C = 0x04000018;` makes gcse share the address expression
     with the `gUnk_02016490 == 3` branch and hoist it into the loop's
     preheader, which the ROM does not do (26 differing bytes).  The pool
     word is identical either way (lesson 4.52 notes the style cost).  */

s32 sub_080b6d04(void)
{
    u16 i;
    s32 n;
    s32 vt1;
    s32 vt2;
    u16 v;
    u16 w[2];
    u32 t0;
    register u32 tw asm("r0");
    u16 *p;
    u16 *base;
    vs32 *pf;
    vs32 *pb;
    s32 *pc2;
    s32 *pc3;
    s8 *pt2;
    s8 *pt;
    s32 d;

    if (gUnk_02016490 == 3)
    {
        gUnk_03000F8C = gUnk_020164A0[0] << 16;
        gUnk_03000B78 = gUnk_020164A0[2] << 16;
        gUnk_0300101C = 0x04000018;
        return 3;
    }
    if (gUnk_02016490 == 2)
    {
        gUnk_0300003C = NULL;
        gUnk_02016494 = 0;
    }
    else
    {
        if (gUnk_02016494 > 0x1FF)
            gUnk_02016494 = 0;
        d = 512 - gUnk_02016494;
        i = gUnk_030023E4;
        t0 = gUnk_03001E94;
        *(u32 *)w = t0 >> 16;
        pt = gUnk_087561CC;
        v = ((d * pt[i >> 3]) >> 1) + *(pc2 = &gUnk_02016C30);
        pf = &gUnk_03000F8C;
        base = gUnk_020164A0;
        pb = &gUnk_03000B78;
        pc3 = pc2;
        pt2 = pt;
        p = base;
        n = 159;
        do
        {
            if ((i & 7) == 0)
                v = ((d * pt2[i >> 3]) >> 1) + *pc3;
            p[0] = v;
            p[1] = w[0];
            p[2] = v;
            i++;
            p += 3;
        } while (--n >= 0);
        asm("" ::: "r8", "ip");
        vt1 = base[0] << 16;
        *pf = vt1;
        vt2 = base[2] << 16;
        *pb = vt2;
        tw = 0x04000018;
        *(u32 *)0x0300101C = tw;
        return 3;
    }
}
