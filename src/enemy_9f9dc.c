
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern u16 gFrameCount;
extern struct Task gTasks[];
extern vu16 gTaskSlotTypes[];

void sub_0809f9dc(void)
{
    struct Task *t;
    struct Task *w;
    u8 *s;
    s32 i;

    if ((gFrameCount & 2) != 0)
    {
        w = gCurTask;
        w->tileWord = (w->tileWord & 0xFFF) | (240 << 8);
        if (w->unk74 == 2)
        {
            if (gTaskSlotTypes[w->unk46] == 130)
                gTasks[w->unk46].tileWord = (w->tileWord & 0xFFF) | (240 << 8);
        }
    }
    else
    {
        t = gCurTask;
        s = &t->unk74;
        switch (*s)
        {
        case 0:
            t->tileWord = (t->tileWord & 0xFFF) | (128 << 8);
            break;
        case 1:
            t->tileWord = (t->tileWord & 0xFFF) | (144 << 8);
            break;
        case 2:
            t->tileWord = (t->tileWord & 0xFFF) | (160 << 8);
            if (*s == 2)
            {
                if (gTaskSlotTypes[t->unk46] == 130)
                    gTasks[t->unk46].tileWord = (t->tileWord & 0xFFF) | (160 << 8);
            }
            break;
        case 3:
            t->tileWord = (t->tileWord & 0xFFF) | (176 << 8);
            break;
        }
    }
}

void sub_0809fb10(void)
{
    struct Task *t;
    u8 *s;

    t = gCurTask;
    s = &t->unk74;
    switch (*s)
    {
    case 0:
        t->tileWord = (t->tileWord & 0xFFF) | (128 << 8);
        break;
    case 1:
        t->tileWord = (t->tileWord & 0xFFF) | (144 << 8);
        break;
    case 2:
        t->tileWord = (t->tileWord & 0xFFF) | (160 << 8);
        if (*s == 2)
        {
            if (gTaskSlotTypes[t->unk46] == 130)
                gTasks[t->unk46].tileWord = (t->tileWord & 0xFFF) | (160 << 8);
        }
        break;
    case 3:
        t->tileWord = (t->tileWord & 0xFFF) | (176 << 8);
        break;
    }
}
