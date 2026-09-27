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

    sub_080692fc();
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
    sub_08068e04();
    sub_08069b44();
    if ((u16)(gCurTask->unk3C - 22) <= 1)
        sub_08068cf8((s32)gUnk_08747EF4);
    if (sub_0809f994() != 0)
    {
        if (gCurTask->unk48 < ((s8 *)gCurTask->unk8C->unk50)[4] + 24)
            gCurTask->unk48 = ((s8 *)gCurTask->unk8C->unk50)[4] + 24;
        else
            gCurTask->unk48 = 288 - ((s8 *)gCurTask->unk8C->unk50)[5];
        gCurTask->unk4C = gCurTask->unk48 << 16;
        sub_0809f970();
    }
}
