#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern u32 gUnk_08747EF4[];

/* Externals */
extern u8 ActorCollideTerrain(void);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u32 ActorCheckHits(void);
extern u32 ActorReactToHit(void);
extern void sub_0809f970(void);
extern s32 sub_0809f994(void);
extern void sub_0809f9dc(void);
extern void sub_0809fb10(void);

void sub_0809cb90(void)
{
    struct Task *t;

    ActorCollideTerrain();
    t = gCurTask;
    if (t->unk24 > 0)
    {
        t->unk24--;
        sub_0809f9dc();
    }
    else
    {
        sub_0809fb10();
    }
    ActorCheckHits();
    ActorReactToHit();
    if ((u16)(gCurTask->frame - 22) <= 1)
        ActorCheckHitsWithBox((s32)gUnk_08747EF4);
    if (sub_0809f994() != 0)
    {
        if (gCurTask->pixelX < ((s8 *)gCurTask->unk8C->terrainBox)[4] + 24)
            gCurTask->pixelX = ((s8 *)gCurTask->unk8C->terrainBox)[4] + 24;
        else
            gCurTask->pixelX = 288 - ((s8 *)gCurTask->unk8C->terrainBox)[5];
        gCurTask->posX = gCurTask->pixelX << 16;
        sub_0809f970();
    }
}
