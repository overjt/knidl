#include "gba/gba.h"
#include "global.h"
#include "task.h"

struct Unk0200D120
{
    /*0x00*/ u8 filler00[0x20];
    /*0x20*/ u16 savedTileWord;
    /*0x22*/ u8 filler22[0x26];
    /*0x48*/ s8 *attackBox;
    /*0x4C*/ u8 filler4C[0x24];
};

/* RAM cells */
extern struct Unk0200D120 gUnk_0200D120[];

/* ROM tables */
extern u32 gUnk_087537E8[];

/* Externals */
extern void TaskMove(void);
extern void TaskDrawWorld(void);
extern u32 RandomRange(u32 range);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetFrame(s32 a);
extern void TaskYieldTrampoline(u32 frames);
extern void sub_080974c8(void);

/* Defined below */
void sub_080970c4(void);

void sub_080970c4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *z;
    struct Task *q0;
    struct Task *q1;
    struct Task *r;
    s8 *p0;
    s8 *p1;
    s8 *p2;
    s8 *p3;
    s8 *p2b;
    s8 *p3b;
    s8 *pa;
    s8 *pb;
    u16 x;
    u16 y;
    s32 i0;
    s32 i1;
    s32 i2;
    s32 j2;
    s32 i3;
    s32 j3;
    s32 ia;
    s32 ib;
    s32 n;
    s32 k0;
    s32 k1;
    s32 k2;
    s32 k3;
    s32 c0_2;
    s32 c2_2;
    s32 c4_2;
    s32 u48_2;
    s32 c0_3;
    s32 c2_3;
    s32 c4_3;
    s32 u48_3;
    s32 h3;
    s16 *a0;
    s16 *a1;
    s16 *b2;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawWorld;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gUnk_087537E8;
    u->tileWord = gUnk_0200D120[u->parent - 32].savedTileWord;
    if (RandomRange(2) != 0)
        gCurTask->facing = 1;
    else
        gCurTask->facing = -1;
    v = gCurTask;
    v->unk28 = 0;
    v->updateCallback = (u32)sub_080974c8;
    while (1) {
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(RandomRange(8));
        w = gCurTask;
        switch (w->unk73) {
        case 0:
            i0 = w->parent;
            q0 = &gTasks[i0];
            a0 = &q0->pixelX;
            k0 = q0->facing;
            p0 = gUnk_0200D120[i0 - 32].attackBox;
            x = k0 * (p0[2] + p0[0]) + *a0;
            y = (p0[3] + q0->pixelY) + p0[1];
            break;
        case 1:
            i1 = w->parent;
            q1 = &gTasks[i1];
            a1 = &q1->pixelX;
            k1 = q1->facing;
            p1 = gUnk_0200D120[i1 - 32].attackBox;
            x = k1 * (p1[2] + p1[0]) + *a1;
            y = (q1->pixelY + p1[5]) - (abs(p1[5] - p1[3]) >> 1) + p1[1];
            break;
        case 2:
            i2 = w->parent;
            p2 = gUnk_0200D120[i2 - 32].attackBox;
            c4_2 = p2[4];
            c0_2 = p2[0];
            k2 = gTasks[i2].facing;
            u48_2 = gTasks[i2].pixelX;
            c2_2 = p2[2];
            x = u48_2 + (c4_2 - (abs(c4_2 - c2_2) >> 1) + c0_2) * k2;
            j2 = gCurTask->parent;
            r = &gTasks[j2];
            b2 = &r->pixelY;
            p2b = gUnk_0200D120[j2 - 32].attackBox;
            y = (p2b[3] + *b2) + p2b[1];
            break;
        case 3:
            i3 = w->parent;
            p3 = gUnk_0200D120[i3 - 32].attackBox;
            c4_3 = p3[4];
            c0_3 = p3[0];
            k3 = gTasks[i3].facing;
            u48_3 = gTasks[i3].pixelX;
            c2_3 = p3[2];
            x = u48_3 + (c4_3 - (abs(c4_3 - c2_3) >> 1) + c0_3) * k3;
            j3 = gCurTask->parent;
            h3 = gTasks[j3].pixelY;
            p3b = gUnk_0200D120[j3 - 32].attackBox;
            y = (h3 + p3b[5]) - (abs(p3b[5] - p3b[3]) >> 1) + p3b[1];
            break;
        }
        ia = gCurTask->parent;
        pa = gUnk_0200D120[ia - 32].attackBox;
        n = RandomRange(abs(pa[4] - pa[2]) >> 1);
        ib = gCurTask->parent;
        x = x + n * gTasks[ib].facing;
        pb = gUnk_0200D120[ib - 32].attackBox;
        y = y + RandomRange(abs(pb[5] - pb[3]) >> 1);
        z = gCurTask;
        z->posX = x << 16;
        z->posY = y << 16;
        if ((z->unk28 & 1) != 0)
            z->facing = -z->facing;
        gCurTask->unk28++;
        TaskSetMotionXFacing(0x14000, -0x1C00);
        gCurTask->velY = -0x10000;
        TaskSetFrame(1);
        TaskYieldTrampoline(1);
        TaskSetFrame(2);
        TaskYieldTrampoline(2);
        TaskSetFrame(3);
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x20000;
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        TaskSetFrame(1);
        TaskYieldTrampoline(1);
        TaskSetMotionXFacing(-0x4000, 0x5A5A5A5A);
        gCurTask->velY = -0x40000;
        TaskSetFrame(0);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(0x4000, 0x5A5A5A5A);
        TaskSetFrame(0);
        TaskYieldTrampoline(2);
    }
}
