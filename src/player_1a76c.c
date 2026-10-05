#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "player.h"
#include "actor.h"

void sub_0801a76c(s32 i)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)sub_0803ddc0;
    t->layer = 7;
    gCurTask->frameTable = gPlayerFrames;
    gCurTask->tileWord = (i << 13) | (i << 7);
    gCurTask->player = &gPlayerStates[i];
}

void ClearColliderLists(void)
{
    struct Collider *p = gPlayerColliders;
    struct Collider *q = gColliderClass10;
    struct Collider *r = gColliderClass20;
    u8 *ca = &gPlayerColliderCount;
    u8 *cb = &gColliderClass10Count;
    u8 *cc = &gColliderClass20Count;
    s32 i;

    for (i = 0; i < 4; i++)
    {
        p->slot = 0;
        p->x = 0;
        p->y = 0;
        p->bodyBox = 0;
        p++;
    }
    p = q;
    for (i = 0; i < 20; i++)
    {
        p->slot = 0;
        p->x = 0;
        p->y = 0;
        p->bodyBox = 0;
        p++;
    }
    p = r;
    for (i = 0; i < 20; i++)
    {
        p->slot = 0;
        p->x = 0;
        p->y = 0;
        p->bodyBox = 0;
        p++;
    }
    *cc = 0;
    *cb = 0;
    *ca = 0;
}

u32 RegisterCollider(u8 idx, u16 x, u16 y, u8 *p)
{
    struct Collider *r;

    switch (p[8] & 0xF0)
    {
    case 0x00:
        if (gPlayerColliderCount == 4)
            return 1;
        r = &gPlayerColliders[gPlayerColliderCount++];
        break;
    case 0x10:
        if (gColliderClass10Count == 20)
            return 1;
        r = &gColliderClass10[gColliderClass10Count++];
        break;
    case 0x20:
        if (gColliderClass20Count == 20)
            return 1;
        r = &gColliderClass20[gColliderClass20Count++];
        break;
    }
    r->slot = idx;
    r->x = x;
    r->y = y;
    r->bodyBox = p;
    return 0;
}
