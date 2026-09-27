
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s32 gUnk_02007D00[];
extern u8 gActivePlayerCount;
extern s16 gUnk_0300244C;
extern u32 * gUnk_08745CFC[];
extern u32 * gUnk_08745D4C[];

/* Externals */
extern void ActorLoadDef(u32 def);

void sub_0809c028(void)
{
    struct Task *t;
    u32 *p;

    if (gUnk_0300244C != 0)
        p = gUnk_08745D4C[gCurTask->unk73 * 4 + gActivePlayerCount - 1];
    else
        p = gUnk_08745CFC[gCurTask->unk73 * 4 + gActivePlayerCount - 1];
    ActorLoadDef(p[0]);
    gUnk_02007D00[4] = p[1];
    t = gCurTask;
    t->unk28 = p[2];
    t->unk2C = p[3];
    t->unk30 = p[4];
    t->unk34 = p[5];
    gUnk_02007D00[3] = 0;
    gUnk_02007D00[2] = 0;
    gUnk_02007D00[1] = 0;
    gUnk_02007D00[0] = 0;
    t->unk24 = 0;
    t->unk20 = 0;
    t->unk1C = 0;
    t->unk18 = 0;
    gUnk_02007D00[5] = 0;
    t->unk6C = 0;
}
