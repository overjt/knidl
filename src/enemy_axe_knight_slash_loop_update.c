#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "enemy.h"

/* Externals */
extern u8 ActorCollideTerrain(void);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u32 ActorCheckHits(void);
extern u32 ActorReactToHit(void);

void AxeKnightSlashLoopUpdate(void)
{
    struct Task *t;

    ActorCollideTerrain();
    t = gCurTask;
    if (t->metaKnightsKnightFlashTimer > 0)
    {
        t->metaKnightsKnightFlashTimer--;
        MetaKnightsKnightFlashPalette();
    }
    else
    {
        MetaKnightsKnightRestorePalette();
    }
    ActorCheckHits();
    ActorReactToHit();
    if ((u16)(gCurTask->frame - 22) <= 1)
        ActorCheckHitsWithBox((s32)gAxeKnightSlashAttackBox);
    if (MetaKnightsKnightIsAtEdge() != 0)
    {
        if (gCurTask->pixelX < ((s8 *)gCurTask->u8C.actor->terrainBox)[4] + 24)
            gCurTask->pixelX = ((s8 *)gCurTask->u8C.actor->terrainBox)[4] + 24;
        else
            gCurTask->pixelX = 288 - ((s8 *)gCurTask->u8C.actor->terrainBox)[5];
        gCurTask->posX = gCurTask->pixelX << 16;
        sub_0809f970();
    }
}
