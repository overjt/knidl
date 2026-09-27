#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "camera.h"
#include "save.h"

/* Not from main.h: this file's view of gBg2ScrollY differs (lesson 3.517). */
extern void (*gFrameCallback)(void);
extern vs32 gBg3ScrollX;
extern vs32 gBg2ScrollX;
extern vu32 gBg2ScrollY;
extern s16 gSpriteCameraY;

/* The fixed BG2VOFS value `w` is a plain u32 local that global allocation
   spills (the ROM's `str r0, [sp]` ... `mov r0, sp; ldrh`, lesson 3.467), and
   the tail store goes through the pointer local `pc`, assigned with the other
   three before the loop: global allocation drops it and reload re-loads the
   address right before the store, after the value (lesson 3.258).  */

s32 sub_080b6d04(void)
{
    u16 i;
    s32 n;
    s32 vt1;
    s32 vt2;
    u16 v;
    u32 w;
    u16 *p;
    u16 *base;
    vs32 *pf;
    vs32 *pb;
    s32 *pc2;
    s32 *pc3;
    s8 *pt2;
    s8 *pt;
    s32 d;
    u32 *pc;

    if (gHBlankScrollState == 3)
    {
        gBg2ScrollX = gHBlankScrollTable[0] << 16;
        gBg3ScrollX = gHBlankScrollTable[2] << 16;
        gHBlankDmaDest = 0x04000018;
        return 3;
    }
    if (gHBlankScrollState == 2)
    {
        gFrameCallback = NULL;
        gHBlankScrollTimer = 0;
    }
    else
    {
        if (gHBlankScrollTimer > 0x1FF)
            gHBlankScrollTimer = 0;
        d = 512 - gHBlankScrollTimer;
        i = gSpriteCameraY;
        w = gBg2ScrollY >> 16;
        pt = gUnk_087561CC;
        v = ((d * pt[i >> 3]) >> 1) + *(pc2 = &gUnk_02016C30);
        pf = &gBg2ScrollX;
        base = gHBlankScrollTable;
        pb = &gBg3ScrollX;
        pc = &gHBlankDmaDest;
        pc3 = pc2;
        pt2 = pt;
        p = base;
        n = 159;
        do
        {
            if ((i & 7) == 0)
                v = ((d * pt2[i >> 3]) >> 1) + *pc3;
            p[0] = v;
            p[1] = w;
            p[2] = v;
            i++;
            p += 3;
        } while (--n >= 0);
        vt1 = base[0] << 16;
        *pf = vt1;
        vt2 = base[2] << 16;
        *pb = vt2;
        *pc = 0x04000018;
        return 3;
    }
}
