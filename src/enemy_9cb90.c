#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern u32 gUnk_08747EF4[];

/* Externals */
extern u8 sub_080692fc(void);
extern u32 sub_08068cf8(s32 a);
extern u32 sub_08068e04(void);
extern u32 sub_08069b44(void);
extern void sub_0809f970(void);
extern s32 sub_0809f994(void);
extern void sub_0809f9dc(void);
extern void sub_0809fb10(void);

void sub_0809cb90(void)
{
    struct Task *t;
    struct Task *u;
    struct Task **tq;
    register struct Task **tp asm("r0");
    struct Actor *a;
    register s8 *q asm("r3");
    register s32 r asm("r0");
    register s32 c asm("r3");
    s32 d;
    register s32 h asm("r1");
    register s16 *p asm("r2");
    sub_080692fc();
    t = gUnk_03002490;
    if (t->unk24 > 0)
    {
        t->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    sub_08068e04();
    sub_08069b44();
    tq = &gUnk_03002490;
    asm("" : "+r"(tq));
    if ((u16)((*tq)->unk3C - 22) <= 1)
        sub_08068cf8((s32)gUnk_08747EF4);
    if (sub_0809f994() != 0)
    {
        u = *tq;
        p = &u->unk48;
        h = *p;
        a = u->unk8C;
        q = (s8 *)a->unk50;
        r = q[4];
        r += 24;
        if (h >= r)
        {
            d = q[5];
            c = 288;
            asm("" : "+r"(c));
            r = c;
            asm("" : "+r"(r));
            r -= d;
        }
        *p = r;
        tp = &gUnk_03002490;
        (*tp)->unk4C = (*tp)->unk48 << 16;
        sub_0809f970();
    }
}
