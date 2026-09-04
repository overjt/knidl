
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern u16 gUnk_03001EA4;
extern struct Task gUnk_03002790[];
extern vu16 gUnk_03004CA0[];

void sub_0809f9dc(void)
{
    struct Task *t;
    struct Task *w;
    u8 *s;
    s32 i;

    if ((gUnk_03001EA4 & 2) != 0)
    {
        w = gUnk_03002490;
        w->unk40 = (w->unk40 & 0xFFF) | (240 << 8);
        if (w->unk74 == 2)
        {
            if (gUnk_03004CA0[w->unk46] == 130)
                gUnk_03002790[w->unk46].unk40 = (w->unk40 & 0xFFF) | (240 << 8);
        }
    }
    else
    {
        t = gUnk_03002490;
        s = &t->unk74;
        switch (*s)
        {
        case 0:
            t->unk40 = (t->unk40 & 0xFFF) | (128 << 8);
            break;
        case 1:
            t->unk40 = (t->unk40 & 0xFFF) | (144 << 8);
            break;
        case 2:
            t->unk40 = (t->unk40 & 0xFFF) | (160 << 8);
            if (*s == 2)
            {
                if (gUnk_03004CA0[t->unk46] == 130)
                    gUnk_03002790[t->unk46].unk40 = (t->unk40 & 0xFFF) | (160 << 8);
            }
            break;
        case 3:
            t->unk40 = (t->unk40 & 0xFFF) | (176 << 8);
            break;
        }
    }
}

void sub_0809fb10(void)
{
    struct Task *t;
    u8 *s;

    t = gUnk_03002490;
    s = &t->unk74;
    switch (*s)
    {
    case 0:
        t->unk40 = (t->unk40 & 0xFFF) | (128 << 8);
        break;
    case 1:
        t->unk40 = (t->unk40 & 0xFFF) | (144 << 8);
        break;
    case 2:
        t->unk40 = (t->unk40 & 0xFFF) | (160 << 8);
        if (*s == 2)
        {
            if (gUnk_03004CA0[t->unk46] == 130)
                gUnk_03002790[t->unk46].unk40 = (t->unk40 & 0xFFF) | (160 << 8);
        }
        break;
    case 3:
        t->unk40 = (t->unk40 & 0xFFF) | (176 << 8);
        break;
    }
}
