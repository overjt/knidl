
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "enemy.h"

void MetaKnightsKnightFlashPalette(void)
{
    struct Task *t;
    struct Task *w;
    u8 *s;
    s32 i;

    if ((gFrameCount & 2) != 0)
    {
        w = gCurTask;
        w->tileWord = (w->tileWord & 0xFFF) | (240 << 8);
        if (w->actorSpawnArg == 2)
        {
            if (gTaskSlotTypes[w->metaKnightsKnightWeaponSlot] == 130)
                gTasks[w->metaKnightsKnightWeaponSlot].tileWord = (w->tileWord & 0xFFF) | (240 << 8);
        }
    }
    else
    {
        t = gCurTask;
        s = &t->actorSpawnArg;
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
                if (gTaskSlotTypes[t->metaKnightsKnightWeaponSlot] == 130)
                    gTasks[t->metaKnightsKnightWeaponSlot].tileWord = (t->tileWord & 0xFFF) | (160 << 8);
            }
            break;
        case 3:
            t->tileWord = (t->tileWord & 0xFFF) | (176 << 8);
            break;
        }
    }
}

void MetaKnightsKnightRestorePalette(void)
{
    struct Task *t;
    u8 *s;

    t = gCurTask;
    s = &t->actorSpawnArg;
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
            if (gTaskSlotTypes[t->metaKnightsKnightWeaponSlot] == 130)
                gTasks[t->metaKnightsKnightWeaponSlot].tileWord = (t->tileWord & 0xFFF) | (160 << 8);
        }
        break;
    case 3:
        t->tileWord = (t->tileWord & 0xFFF) | (176 << 8);
        break;
    }
}
