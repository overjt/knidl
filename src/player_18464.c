#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern struct Task *gUnk_03002490;

void sub_08003770(u16 v);

void sub_08018464(void)
{
    struct Task *t = gUnk_03002490;
    s32 d = t->unk24;
    s32 v;

    if (d < 0)
    {
        v = 255;
    }
    else
    {
        v = t->unk20 - d;
        t->unk20 = v;
        if (v >= 0)
            goto done;
        v = 0;
    }
    t->unk20 = v;
done:
    sub_08003770((u16)gUnk_03002490->unk20);
}
