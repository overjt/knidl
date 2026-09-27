#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* plobj_52f6c.c (0x08052F6C-0x08053AF3, issue #90).
 *
 * Task type #6 (the objects the player's actions spawn), variants 10-12,
 * and the two spawners.  sub_08052f6c (variant 10) takes its owner's OAM
 * flags (Task.unk8C is the spawning task) and runs one of six sub-states
 * Task.unk18 & 15: states 0-2 trace an eight-step path (the 8.8 velocity
 * rows gUnk_0873B8C6[k] and the five-frame animation rows gUnk_0873B88A
 * that gUnk_0873B872[k] picks, callback sub_08052b08) and fall into state
 * 3, which flies in one of four directions gUnk_0873B862[Task.unk28] and
 * emits effect 33 every other frame; states 4 and 5 are a stationary
 * object with two callbacks, sub_08053380 (the hit test, which ends the
 * object through the shared exit sub_08050814 once PlayerState.ability is
 * 0) and sub_080534d0 (a six-step trail drawn with QueueSprite).
 * Variants 11 and 12 (sub_080535b0, sub_080537dc) are two projectiles
 * that move 4 pixels a frame in the facing direction (animation table
 * gUnk_0874C4E4, M14's shared callback sub_08050f80); their per-frame
 * callbacks sub_080536dc and sub_080538cc register the collider and hand
 * over to sub_08050814 on contact (variant 11 bounces back once on
 * collision result 6).  CreatePlayerObject and sub_08053a44 are the spawners
 * the player's actions call (M09-M14): they start a task of type 6 in
 * the slot band of player 0-3 (4-6, 7-9, 10-12, 13-15; CreatePlayerObject
 * then retries a wider band and, last, a type-7 task in slots 32-62) and
 * copy the spawner's position, facing Task.facing, Task.waterFlags and
 * PlayerState into it, with Task.unk18 = variant << 24 | arg.  They
 * return the task index or -1; the landed callers outside M14 declare
 * them `void (s32, s32, s32)`. */

/* gTerrainResult: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh).  A 16-bit test of
   unk0/unk1 together is `*(u16 *)&gTerrainResult` (M12's sub_08045a50). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

extern u32 gUnk_08751BF4[];
extern u16 gUnk_0873B862[][2];
extern u8 gUnk_0873B872[][8];
extern u8 gUnk_0873B88A[][5];
extern u16 gUnk_0873B8C6[][2][8];
extern s8 gUnk_0873CB5C[];
extern s8 gUnk_0873CB6C[];
extern struct Unk03005550 gTerrainResult;
extern u32 gUnk_0873CC3C[];
extern u32 gUnk_0873CC44[];
extern u32 gUnk_0873BE4C[];
extern u32 gUnk_0873BE60[];
extern s16 gSpriteCameraX;               /* scalar, read with ldrsh (33 landed files) */
extern s16 gSpriteCameraY;               /* scalar, read with ldrsh (32 landed files) */
extern u32 gUnk_087520A8[];
extern u32 gUnk_0874C4E4[];
extern s8 gUnk_0873CB4C[];
extern u32 gUnk_0873BE74[];
extern u32 gUnk_0873BE88[];

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
s32 PlaySfx(s32 id);
s32 TaskCreateInRange(u32 type, s32 start, s32 end);
void TaskMove(void);
void TaskMoveRelativeToParent(void);
void TaskDrawScreenOrFree(void);
void TaskDrawWorld(void);
void TaskDrawWorldInViewOrFree(void);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskStop(void);
u32 IsWorldPosOnScreen(s16 x, s16 y);
u32 IsInView(s16 x, s16 y);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_0801c230(const s8 *p);
void sub_0802205c(s8 *box);
void sub_0802233c(s8 *off);
s32 TaskBreakBlocks(struct HitBoxSet *p, s32 e);   /* M14's callers test r0 unnarrowed (good/sub_08050c48.c); landed M09/M12/M13 files spell it u16 */
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void sub_08050814(void);
void sub_08050f80(void);
void sub_080520dc(void);
s32 sub_08052b08(void);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);
void sub_08053380(void);
void sub_080534d0(void);
void sub_080536dc(void);
void sub_080538cc(void);

void sub_08052f6c(void)
{
    u16 *xs;
    u16 *ys;
    u8 *steps;

    {
        struct Task *t = gCurTask;
        t->tileWord = ((struct Task *)t->unk8C)->tileWord | 0xF008;
        switch (t->unk18 & 15)
        {
        case 0:
            PlaySfxIfLocalPlayer(207, gCurTask->parent);
            xs = gUnk_0873B8C6[0][0];
            ys = gUnk_0873B8C6[0][1];
            steps = gUnk_0873B872[0];
            goto common;
        case 1:
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            xs = gUnk_0873B8C6[1][0];
            ys = gUnk_0873B8C6[1][1];
            steps = gUnk_0873B872[1];
            goto common;
        case 2:
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(2);
            xs = gUnk_0873B8C6[2][0];
            ys = gUnk_0873B8C6[2][1];
            steps = gUnk_0873B872[2];
        common:
            {
                struct Task *u = gCurTask;
                u->moveCallback = (u32)TaskMoveRelativeToParent;
                u->drawCallback = (u32)TaskDrawWorld;
                u->updateCallback = (u32)sub_08052b08;
                u->layer = 5;
                u = gCurTask;
                u->frameTable = gUnk_08751BF4;
                if (u->facing == 1)
                    u->unk28 = 10;
                else
                    u->unk28 = -10;
            }
            {
                struct Task *u = gCurTask;
                u->unk2C = 4;
                u->unk80 = 16;
            }
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 7; gCurTask->unk6C++)
            {
                u8 *e;
                s32 v;
                {
                    struct Task *u = gCurTask;
                    u->posX = u->unk28 << 16;
                    u->posY = u->unk2C << 16;
                    v = xs[(s16)u->unk6C] << 8;
                    if (xs[(s16)u->unk6C] & 0x8000)
                        v |= 0xFF000000;
                }
                TaskSetMotionXFacing(v, 0x5A5A5A5A);
                {
                    struct Task *u = gCurTask;
                    v = ys[(s16)u->unk6C] << 8;
                    if (ys[(s16)u->unk6C] & 0x8000)
                        v |= 0xFF000000;
                    u->velY = v;
                    e = gUnk_0873B88A[steps[(s16)u->unk6C]];
                    u->unk6E = 0;
                }
                do
                {
                    gCurTask->frame = e[gCurTask->unk6E];
                    TaskYieldTrampoline(1);
                } while (++gCurTask->unk6E <= 4);
                {
                    struct Task *u = gCurTask;
                    u->velX = 0;
                    u->velY = 0;
                }
            }
            TaskExitTrampoline();
        case 3:
        {
            struct Task *u = gCurTask;
            if (u->frameTable == NULL)
            {
                u->moveCallback = (u32)TaskMove;
                u->drawCallback = (u32)TaskDrawWorldInViewOrFree;
                u->layer = 7;
                u = gCurTask;
                u->frameTable = gUnk_08751BF4;
                u->posY = (u->pixelY + 4) << 16;
                if (u->facing == 1)
                {
                    u->posX = (u->pixelX + 6) << 16;
                    u->unk28 = 1;
                }
                else
                {
                    u->posX = (u->pixelX - 6) << 16;
                    u->unk28 = 3;
                }
                gCurTask->unk2C = 3;
                sub_0802233c(gUnk_0873CB5C);
                gCurTask->unk80 = 6;
            }
            else
            {
                u->updateCallback = 0;
                TaskStop();
                gCurTask->frame = 8;
                TaskYieldTrampoline(2);
                gCurTask->frame = 0xFFFF;
                TaskYieldTrampoline(1);
                gCurTask->frame = 9;
                TaskYieldTrampoline(2);
                gCurTask->frame = -1;
                TaskYieldTrampoline(1);
                gCurTask->onGround = 0;
            }
        }
        {
            struct Task *u = gCurTask;
            s32 k = u->unk28;
            u16 *e = gUnk_0873B862[k];
            s32 v;

            u->updateCallback = (u32)sub_080520dc;
            u->frame = e[0];
            if (k == 1 || k == 3)
            {
                v = e[1] << 8;
                if (e[1] & 0x8000)
                    v |= 0xFF000000;
                u->velX = v;
            }
            else
            {
                v = e[1] << 8;
                if (e[1] & 0x8000)
                    v |= 0xFF000000;
                u->velY = v;
            }
        }
            while (1)
            {
                CreatePlayerEffect(gCurTask->player->playerIndex, 33, gCurTask->unk28);
                TaskYieldTrampoline(2);
            }
        case 4:
        case 5:
        {
            struct Task *u = gCurTask;
            u->moveCallback = (u32)TaskMove;
            u->drawCallback = (u32)TaskDrawWorldInViewOrFree;
            u->updateCallback = (u32)sub_08053380;
            u->lateUpdateCallback = (u32)sub_080534d0;
            u->layer = 7;
        }
            gCurTask->frameTable = gUnk_08751BF4;
            sub_0802205c(gUnk_0873CB6C);
            {
                struct Task *u = gCurTask;
                if (u->facing == 1)
                    u->posX = (u->pixelX + 10) << 16;
                else
                    u->posX = (u->pixelX - 10) << 16;
            }
            {
                struct Task *u = gCurTask;
                u->posY = (u->pixelY + 4) << 16;
                u->unk80 = 24;
            }
            gCurTask->unk28 = 0;
            TaskSetMotionXFacing(0x60000, 0x5A5A5A5A);
            {
                struct Task *u = gCurTask;
                if ((u->unk18 & 15) == 4)
                    u->frame = 18;
                else
                    u->frame = 19;
            }
            break;
        }
    }
    TaskSleepForever();
}

void sub_08053380(void)
{
    struct Task *t = gCurTask;

    if (t->player->ability == 0)
    {
        TaskSetEntry(sub_08050814, gCurTaskIdx);
        return;
    }
    switch (t->unk18 & 15)
    {
    case 4:
    {
        s32 hit;

        if (TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CC3C, t->parent))
            gCurTask->hitKind = 1;
        else
            sub_0801c230(gUnk_0873CB6C);
        hit = 0;
        if ((gCurTask->onGround & 1) || *(u16 *)&gTerrainResult != 0 || gCurTask->hitKind != 0)
            hit++;
        if (hit)
        {
            if (gTerrainResult.unk1 != 0 || (gCurTask->onGround & 1) || gTerrainResult.unk0 != 0)
                PlaySfxIfLocalPlayer(125, gCurTask->parent);
            TaskSetEntry(sub_08050814, gCurTaskIdx);
            gCurTask->unk24 = (s32)gUnk_0873BE4C;
        }
        RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873BE4C);
        break;
    }
    case 5:
        t->health = 127;
        TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CC44, t->parent);
        RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873BE60);
        break;
    }
}

void sub_080534d0(void)
{
    s32 k = -1;

    if ((gCurTask->unk18 & 15) == 4)
        k = 12;
    else if ((gCurTask->unk18 & 15) == 5)
        k = 0;
    if (k != -1)
    {
        if (IsInView(gCurTask->pixelX, gCurTask->pixelY)
            && IsWorldPosOnScreen(gCurTask->pixelX, gCurTask->pixelY))
        {
            struct Task *t = gCurTask;
            if (t->facing == -1)
                k++;
            QueueSprite(t->layer, gUnk_087520A8[t->unk28 * 2 + k], 0, 0,
                         t->pixelX - gSpriteCameraX, t->pixelY - gSpriteCameraY);
        }
        {
            struct Task *t = gCurTask;
            if (++t->unk28 > 5)
                t->unk28 = 0;
        }
    }
}

void sub_080535b0(void)
{
    {
        struct Task *t = gCurTask;
        t->moveCallback = (u32)TaskMove;
        t->drawCallback = (u32)TaskDrawWorldInViewOrFree;
        t->updateCallback = (u32)sub_080536dc;
        t->layer = 5;
    }
    gCurTask->frameTable = gUnk_0874C4E4;
    sub_0802205c(gUnk_0873CB4C);
    {
        struct Task *t = gCurTask;
        t->posY = (t->pixelY + 4) << 16;
        if (t->facing == 1)
            t->posX = (t->pixelX + 16) << 16;
        else
            t->posX = (t->pixelX - 16) << 16;
    }
    {
        struct Task *t = gCurTask;
        t->unk28 = 0;
        if (t->facing == 1)
            t->unk2C = -0x40000;
        else
            t->unk2C = 0x40000;
    }
    gCurTask->unk30 = 0;
    TaskSetMotionXFacing(0x40000, 0x5A5A5A5A);
    gCurTask->frame = 0;
    TaskYieldTrampoline(3);
    gCurTask->lateUpdateCallback = (u32)sub_08050f80;
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    while (1)
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
    }
}

void sub_080536dc(void)
{
    gCurTask->health = 127;
    switch (gCurTask->hitKind)
    {
    case 1:
    case 2:
        TaskSetEntry(sub_08050814, gCurTaskIdx);
        return;
    default:
        RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873BE74);
        break;
    case 6:
        PlaySfx(0x232);
        {
            struct Task *t = gCurTask;
            t->lateUpdateCallback = 0;
            if (t->facing == 1)
                t->facing = -1;
            else
                t->facing = 1;
        }
        TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
        {
            struct Task *t = gCurTask;
            t->velY = -0x10000;
            t->hitKind = 4;
        }
        break;
    case 4:
        break;
    }
    sub_0801c230(gUnk_0873CB4C);
    {
        struct Task *t = gCurTask;
        if ((t->onGround & 1) || *(u16 *)&gTerrainResult != 0)
        {
            PlaySfxIfLocalPlayer(125, t->parent);
            TaskSetEntry(sub_08050814, gCurTaskIdx);
        }
    }
}

void sub_080537dc(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawScreenOrFree;
    t->updateCallback = (u32)sub_080538cc;
    t->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C4E4;
    t->posX = (t->pixelX + 24) << 16;
    t->unk28 = 0;
    if (t->facing == 1)
        t->unk2C = -0x40000;
    else
        t->unk2C = 0x40000;
    gCurTask->unk30 = 0;
    TaskSetMotionXFacing(0x40000, 0x5A5A5A5A);
    gCurTask->frame = 0;
    TaskYieldTrampoline(3);
    gCurTask->lateUpdateCallback = (u32)sub_08050f80;
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    while (1)
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
    }
}

void sub_080538cc(void)
{
    struct Task *t = gCurTask;

    t->health = 127;
    if (t->hitKind != 0)
        TaskSetEntry(sub_08050814, gCurTaskIdx);
    else
        RegisterCollider(gCurTaskIdx, t->pixelX + gSpriteCameraX, t->pixelY + gSpriteCameraY, gUnk_0873BE88);
}

s32 CreatePlayerObject(s8 player, u8 variant, s32 arg)
{
    s32 prio;
    s32 idx;

    if (player == 0)
        prio = 4;
    else if (player == 1)
        prio = 7;
    else if (player == 2)
        prio = 10;
    else if (player == 3)
        prio = 13;
    else
        return -1;
    idx = TaskCreateInRange(6, prio, prio + 2);
    if (idx == -1)
    {
        if (player == 0)
            prio = 16;
        else if (player == 1)
            prio = 20;
        else if (player == 2)
            prio = 24;
        else if (player == 3)
            prio = 28;
        idx = TaskCreateInRange(6, prio, prio + 3);
        if (idx == -1)
            idx = TaskCreateInRange(7, 32, 62);
    }
    if (idx != -1)
    {
        struct Task *t = &gTasks[idx];
        t->unk18 = (variant << 24) | (arg & 0xFFFFFF);
        t->posX = gCurTask->posX;
        t->pixelX = gCurTask->pixelX;
        t->posY = gCurTask->posY;
        t->pixelY = gCurTask->pixelY;
        t->facing = gCurTask->facing;
        t->waterFlags = gCurTask->waterFlags;
        t->player = gCurTask->player;
    }
    return idx;
}

s32 sub_08053a44(s8 player, u8 variant, s32 arg)
{
    s32 prio;
    s32 idx;

    if (player == 0)
        prio = 4;
    else if (player == 1)
        prio = 7;
    else if (player == 2)
        prio = 10;
    else if (player == 3)
        prio = 13;
    else
        return -1;
    idx = TaskCreateInRange(6, prio, prio + 2);
    if (idx != -1)
    {
        struct Task *t = &gTasks[idx];
        t->unk18 = (variant << 24) | (arg & 0xFFFFFF);
        t->posX = gCurTask->posX;
        t->pixelX = gCurTask->pixelX;
        t->posY = gCurTask->posY;
        t->pixelY = gCurTask->pixelY;
        t->facing = gCurTask->facing;
        t->waterFlags = gCurTask->waterFlags;
        t->player = gCurTask->player;
    }
    return idx;
}
