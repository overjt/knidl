#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* plobj_509ec.c (0x080509EC-0x080514F7, issue #90).
 *
 * Task type #6, variants 0-2, each variant body followed by the callbacks
 * only it installs.  A variant body installs the sprite (TaskMove /
 * TaskDrawWorldInViewOrFree, Task.layer = 5, an animation table in Task.frameTable) and runs
 * `switch (Task.unk18 & 15)` over its sub-states, each a yield script that
 * ends in TaskExitTrampoline; its per-frame callback (Task.updateCallback) runs
 * the hit test TaskBreakBlocks and the terrain checks and re-binds the body
 * in another sub-state, or the shared exit sub_08050814, on contact.
 * Variant 0 (sub_080509ec, callback sub_08050c48) spawns a copy of itself
 * in sub-state 1 while it moves; variants 1 and 2 (sub_08050d00,
 * sub_08051124) have the collision callbacks sub_08050e84/sub_080512f8
 * (sound 125 on contact) and the Task.lateUpdateCallback callbacks sub_08050f80/
 * sub_080513d4, which draw a six-step trail behind the object
 * (sub_08050f80 is also installed by variants 11 and 12). */

/* gTerrainResult: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh).  A 16-bit test of
   unk0/unk1 together is `*(u16 *)&gTerrainResult` (M12's sub_08045a50). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 slope;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

extern u32 gUnk_0874C568[];
extern s8 gUnk_0873CB44[];              /* collision box passed to sub_0802205c / sub_0801c230 */
extern u32 gUnk_0873BD64[];             /* collider row passed to RegisterCollider (4th arg) */
extern struct Unk03005550 gTerrainResult;
extern u32 gUnk_0873CB84[];
extern u32 gUnk_0874C44C[];
extern s8 gUnk_0873CB4C[];
extern u32 gUnk_0873CB94[];
extern u32 gUnk_0873BD78[];
extern u8 gUnk_02000020;
extern s16 gSpriteCameraX;               /* scalar, read with ldrsh (33 landed files) */
extern s16 gSpriteCameraY;               /* scalar, read with ldrsh (32 landed files) */
extern u32 gUnk_0874C478[];
extern u32 gUnk_0873CBA4[];
extern u32 gUnk_0873BD8C[];

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void TaskMove(void);
void TaskDrawWorldInViewOrFree(void);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrameByFacing(s16 a);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskStopX(void);
void TaskStop(void);
u32 IsWorldPosOnScreen(s16 a, s16 b);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_0801c230(const s8 *p);
void sub_0802205c(s8 *box);
s32 TaskBreakBlocks(struct HitBoxSet *p, s32 e);   /* M14's callers test r0 unnarrowed (good/sub_08050c48.c); landed M09/M12/M13 files spell it u16 */
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void sub_08050814(void);

s32 CreatePlayerObject(s8 player, u8 variant, s32 arg);
void sub_08050c48(void);
void sub_08050e84(void);
void sub_08050f80(void);
void sub_080512f8(void);
void sub_080513d4(void);

void sub_080509ec(void)
{
    {
        struct Task *t = gCurTask;
        t->moveCallback = (u32)TaskMove;
        t->drawCallback = (u32)TaskDrawWorldInViewOrFree;
        t->layer = 5;
    }
    gCurTask->frameTable = gUnk_0874C568;
    sub_0802205c(gUnk_0873CB44);
    switch (gCurTask->unk18 & 15)
    {
    case 0:
    {
        struct Task *t = gCurTask;
        t->updateCallback = (u32)sub_08050c48;
        if (t->facing == 1)
            t->posX = (t->pixelX + 8) << 16;
        else
            t->posX = (t->pixelX - 8) << 16;
    }
    {
        struct Task *t = gCurTask;
        t->posY = (t->pixelY + 2) << 16;
        PlaySfxIfLocalPlayer(114, t->parent);
    }
        TaskSetMotionXFacing(0x3C000, -0x2000);
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++)
        {
            TaskSetFrameByFacing(0);
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            CreatePlayerObject(gCurTask->player->playerIndex, 0, 1);
            gCurTask->frame -= 2;
            TaskYieldTrampoline(2);
            TaskSetFrameByFacing(4);
            TaskYieldTrampoline(2);
        }
        TaskStopX();
        gCurTask->updateCallback = 0;
        TaskSetFrameByFacing(0);
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(2);
        TaskSetFrameByFacing(4);
        TaskYieldTrampoline(1);
        TaskSetFrameByFacing(10);
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        break;
    case 1:
        TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
        TaskSetFrameByFacing(6);
        TaskYieldTrampoline(4);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        break;
    case 2:
        TaskStop();
        {
            struct Task *t = gCurTask;
            t->updateCallback = 0;
            RegisterCollider(gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873BD64);
        }
        gCurTask->frame = 26;
        TaskYieldTrampoline(1);
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 10; gCurTask->unk6C++)
        {
            gCurTask->frame++;
            TaskYieldTrampoline(1);
        }
        break;
    }
    TaskExitTrampoline();
}

void sub_08050c48(void)
{
    if (TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CB84, gCurTask->parent))
        gCurTask->hitKind = 1;
    else
        sub_0801c230(gUnk_0873CB44);
    if (gTerrainResult.unk1 != 0 || (gCurTask->onGround & 1) || gTerrainResult.unk0 != 0
        || gCurTask->hitKind != 0)
    {
        struct Task *t = gCurTask;
        t->unk18 = (t->unk18 & ~15) | 2;
        TaskSetEntry(sub_080509ec, gCurTaskIdx);
    }
    RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873BD64);
}

void sub_08050d00(void)
{
    {
        struct Task *t = gCurTask;
        t->moveCallback = (u32)TaskMove;
        t->drawCallback = (u32)TaskDrawWorldInViewOrFree;
        t->updateCallback = (u32)sub_08050e84;
        t->layer = 5;
    }
    gCurTask->frameTable = gUnk_0874C44C;
    sub_0802205c(gUnk_0873CB4C);
    {
        struct Task *t = gCurTask;
        t->posY = (t->pixelY + 4) << 16;
        if (t->facing == 1)
            t->posX = (t->pixelX + 8) << 16;
        else
            t->posX = (t->pixelX - 8) << 16;
    }
    PlaySfxIfLocalPlayer(105, gCurTask->parent);
    {
        struct Task *t = gCurTask;
        if (!(t->waterFlags & 1))
        {
            t->unk28 = 0;
            if (t->facing == 1)
                t->unk2C = -0x40000;
            else
                t->unk2C = 0x40000;
            gCurTask->unk30 = 0;
            TaskSetMotionXFacing(0x40000, 0x5A5A5A5A);
            while (1)
            {
                gCurTask->frame = 4;
                TaskYieldTrampoline(3);
                gCurTask->lateUpdateCallback = (u32)sub_08050f80;
                gCurTask->frame++;
                TaskYieldTrampoline(3);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
            }
        }
    }
    gCurTask->unk28 = 0;
    gCurTask->accelY = 0x400;
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
    {
        gCurTask->frame = 4;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
    }
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_08050e84(void)
{
    s32 hit;

    if (TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CB94, gCurTask->parent))
        gCurTask->hitKind = 1;
    else
        sub_0801c230(gUnk_0873CB4C);
    hit = 0;
    {
        struct Task *t = gCurTask;
        if ((t->onGround & 1) || *(u16 *)&gTerrainResult != 0 || t->hitKind != 0)
            hit++;
        else if ((t->waterFlags & 1) && t->unk28 != 0)
            hit = 1;
    }
    if (hit)
    {
        struct Task *t;
        TaskSetEntry(sub_08050814, gCurTaskIdx);
        t = gCurTask;
        t->unk24 = (s32)gUnk_0873BD78;
        if (gTerrainResult.unk1 != 0 || (t->onGround & 1) || gTerrainResult.unk0 != 0)
            PlaySfxIfLocalPlayer(125, gCurTask->parent);
    }
    RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873BD78);
}

void sub_08050f80(void)
{
    s32 dx;
    s32 x, y;
    struct Task *t;

    t = gCurTask;
    if (t->facing == 1)
        dx = -8;
    else
        dx = 8;
    switch (t->unk28)
    {
    case 0:
    case 1:
        if (gUnk_02000020 == 2 || gUnk_02000020 == 3)
            gCurTask->unk34 = 4;
        else
            gCurTask->unk34 = 8;
        break;
    case 2:
    case 3:
        if (gUnk_02000020 == 2 || gUnk_02000020 == 3)
            gCurTask->unk34 = 5;
        else
            gCurTask->unk34 = 9;
        break;
    case 4:
    case 5:
        if (gUnk_02000020 == 2 || gUnk_02000020 == 3)
            gCurTask->unk34 = 6;
        else
            gCurTask->unk34 = 10;
        break;
    }
    t = gCurTask;
    if (t->unk28++ > 5)
    {
        t->unk28 = 0;
        if (t->facing == 1)
            t->unk2C = -0x40000;
        else
            t->unk2C = 0x40000;
        gCurTask->unk30 = 0;
    }
    else
    {
        if (t->facing == 1)
            t->unk30 += -0x10000;
        else
            t->unk30 += 0x10000;
        gCurTask->unk2C += gCurTask->unk30;
    }
    t = gCurTask;
    if (t->player->unk37 != 2)
    {
        x = t->pixelX + dx + ((s16 *)&t->unk2C)[1];
        y = t->pixelY;
        if (!IsWorldPosOnScreen(x, y))
            return;
        x -= gSpriteCameraX;
        y -= gSpriteCameraY;
    }
    else
    {
        x = t->pixelX + dx + ((s16 *)&t->unk2C)[1];
        y = t->pixelY;
    }
    {
        u32 *tbl = gCurTask->frameTable;
        QueueSprite(gCurTask->layer, tbl[gCurTask->unk34], 0, 0, x, y);
    }
}

void sub_08051124(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorldInViewOrFree;
    gCurTask->updateCallback = (u32)sub_080512f8;
    gCurTask->lateUpdateCallback = (u32)sub_080513d4;
    gCurTask->layer = 5;
    gCurTask->frameTable = gUnk_0874C478;
    sub_0802205c(gUnk_0873CB4C);
    {
        struct Task *t = gCurTask;
        if (t->facing == 1)
            t->posX = (t->pixelX + 8) << 16;
        else
            t->posX = (t->pixelX - 8) << 16;
    }
    {
        struct Task *t = gCurTask;
        t->posY = (t->pixelY + 4) << 16;
        t->unk28 = 6;
        t->unk34 = 0;
        PlaySfxIfLocalPlayer(106, t->parent);
    }
    TaskSetMotionXFacing(0x40000, 0x5A5A5A5A);
    while (1)
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->frame = 16;
        TaskYieldTrampoline(1);
        gCurTask->frame = 1;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame = 17;
        TaskYieldTrampoline(1);
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame = 18;
        TaskYieldTrampoline(1);
        gCurTask->frame = 5;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame = 19;
        TaskYieldTrampoline(1);
        gCurTask->frame = 7;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame = 20;
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame = 21;
        TaskYieldTrampoline(1);
        gCurTask->frame = 11;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame = 22;
        TaskYieldTrampoline(1);
        gCurTask->frame = 13;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame = 23;
        TaskYieldTrampoline(1);
        gCurTask->frame = 15;
        TaskYieldTrampoline(2);
    }
}

void sub_080512f8(void)
{
    {
        struct Task *t = gCurTask;
        t->health = 127;
        TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CBA4, t->parent);
    }
    sub_0801c230(gUnk_0873CB4C);
    {
        struct Task *t = gCurTask;
        if ((t->onGround & 1) || *(u16 *)&gTerrainResult != 0)
        {
            PlaySfxIfLocalPlayer(125, t->parent);
            TaskSetEntry(sub_08050814, gCurTaskIdx);
            gCurTask->unk24 = (s32)gUnk_0873BD8C;
        }
        else if (t->waterFlags & 1)
        {
            if (t->velX & 0xFFFF0000)
                TaskSetMotionXFacing(0x5A5A5A5A, -0x1800);
            gCurTask->accelY = 0x1000;
        }
    }
    RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873BD8C);
}

void sub_080513d4(void)
{
    struct Task *t;

    switch (gCurTask->unk28)
    {
    case 0:
        {
            struct Task *u = gCurTask;
            if (u->facing == 1)
                u->unk30 = u->pixelX - 8;
            else
                u->unk30 = u->pixelX + 8;
        }
    case 1:
        gCurTask->unk2C = 24;
        break;
    case 2:
    case 3:
        gCurTask->unk2C = 25;
        break;
    case 4:
    case 5:
        gCurTask->unk2C = 26;
        break;
    case 6:
    case 7:
        {
            struct Task *v = gCurTask;
            v->unk2C = -1;
            v->unk34 = 0;
        }
        break;
    }
    t = gCurTask;
    t->unk28 = (t->unk28 + 1) & 7;
    if (t->unk2C != -1)
    {
        if (t->facing == 1)
            t->unk34--;
        else
            t->unk34++;
        if (IsWorldPosOnScreen(gCurTask->unk30 + gCurTask->unk34, gCurTask->pixelY))
        {
            u32 *tbl = gCurTask->frameTable;
            QueueSprite(gCurTask->layer, tbl[gCurTask->unk2C], 0, 0,
                         gCurTask->unk30 + gCurTask->unk34 - gSpriteCameraX,
                         gCurTask->pixelY - gSpriteCameraY);
        }
    }
}
