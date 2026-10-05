
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "room.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void ActorLoadDef(u32 def);

void sub_0809c028(void)
{
    struct Task *t;
    u32 *p;

    if (gUnk_0300244C != 0)
        p = gUnk_08745D4C[gCurTask->variant * 4 + gActivePlayerCount - 1];
    else
        p = gUnk_08745CFC[gCurTask->variant * 4 + gActivePlayerCount - 1];
    ActorLoadDef(p[0]);
    gUnk_02007D00[4] = p[1];
    t = gCurTask;
    t->metaKnightsQueue0 = p[2];
    t->metaKnightsQueue1 = p[3];
    t->metaKnightsQueue2 = p[4];
    t->metaKnightsQueue3 = p[5];
    gUnk_02007D00[3] = 0;
    gUnk_02007D00[2] = 0;
    gUnk_02007D00[1] = 0;
    gUnk_02007D00[0] = 0;
    t->metaKnightsQueue3Index = 0;
    t->metaKnightsQueue2Index = 0;
    t->metaKnightsQueue1Index = 0;
    t->metaKnightsQueue0Index = 0;
    gUnk_02007D00[5] = 0;
    t->metaKnightsPaletteTimer = 0;
}
