#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "room.h"
#include "player.h"
#include "effect.h"
#include "actor.h"

/* effect_5a358.c (0x0805A358-0x0805AFAB, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 45-48.  Variant 45 (PlayerEffectTornadoDust, M13) rides on its spawner
 * through three sub-states (gUnk_08751FCC); PlayerEffectTornadoDustUpdate kills it once the
 * player leaves mode 13.  Variant 46 (PlayerEffectCrashBlast, M13) is the twin of
 * variant 37 (src/effect_57ce0.c): the same stop and release of the tasks of
 * kinds 1, 2, 7 and 8 through gScreenAttackTasks and the task skip mask, around a
 * palette effect - it saves the palette buffer (CpuSet of gUnk_03001570 into
 * gUnk_0200AF20) and calls M11's sub_0803f834/ FreezeOtherTasks, M17's
 * sub_08065e6c and a VRAM transfer.  Its callbacks are PlayerEffectCrashBlastUpdate, which
 * blends the saved palette towards gUnk_0873BC3E (while gInHub is
 * set) or gUnk_0873BB7E with BlendColors (80 or 96 colours by
 * gUnk_02007D64), raising the ratio Task.unk2C by 10 up to 0x100 in state 1
 * (with the collider row gUnk_0873C2B4 at the player's camera position
 * gPlayerCameraPos) and lowering it by 46 to 0 in state 2 (then calling M17's
 * sub_08065ed0), and the draw hook PlayerEffectCrashBlastDraw.  Variant 47 (sub_0805acec,
 * M13) is an animation in world space; its callback sub_0805ae00 clears
 * Task.unk28 in sub-state 0 when the player leaves mode 13 or the spawner's
 * Task.variant is not 4 (and copies the spawner's facing), and in the other
 * sub-states tests the block hit-box set gUnk_0873CF8C (TaskBreakBlocksAt) at the
 * spawner's position offset by PlayerState.pixelOffsetX/unk26 (8.8).  Variant 48
 * (sub_0805ae94, M14's action 55) rides on its spawner with the draw hook
 * sub_0805af80 (shared with variant 34: M11's sub_0803dfc8 in player mode
 * 13, otherwise the task dies) and the callback sub_0805af44, which kills it
 * once the player leaves mode 13 or releases both A and B. */

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
u32 BeginFade(u16 steps, s16 delta, u16 *mask);   /* callers pass -4 as movs/negs (src/player_47fe8.c spells it s16 too) */
void RequestCopy(u32 mode, void *src, void *dst, u32 size);   /* early_1518; effect_5afac's pointer spelling */
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);   /* callers pass f sign-extended (lsls/asrs #16); the early_1518 definition says u16 */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
u32 IsWorldPosOnScreen(s16 a, s16 b);   /* the ROM tests r0 unnarrowed (src callers spell u32) */
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 TaskBreakBlocksAt(struct HitBoxSet *p, s32 x, s32 y, s32 e);

void PlayerEffectTornadoDust(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->moveCallback = (u32)TaskMoveRelativeToParent;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->updateCallback = (u32)PlayerEffectTornadoDustUpdate;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_08751FCC;
    switch (t->unk18 & 15)
    {
    case 0:
        t->posX = 0;
        t->posY = -0x80000;
        t->frame = 0xFFFF;
        TaskYieldTrampoline(6);
        gCurTask->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 10);
        u = gCurTask;
        u->posX = 0;
        u->posY = -0x180000;
        u->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 10);
        break;
    case 1:
        t->posX = 0;
        t->posY = 0;
        t->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 10);
        u = gCurTask;
        u->posX = 0;
        u->posY = -0x100000;
        u->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 10);
        break;
    case 2:
        t->posX = 0;
        t->posY = -0x180000;
        t->frame = 0xFFFF;
        TaskYieldTrampoline(20);
        gCurTask->frame = 14;
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 5);
        break;
    }
    TaskExitTrampoline();
}

void PlayerEffectTornadoDustUpdate(void)
{
    if (gCurTask->player->mode != 13)
        TaskFree(gCurTaskIdx);
}

void PlayerEffectCrashBlast(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *q;
    s32 i;
    s32 n;
    s32 k;
    s32 o;
    s32 m;
    s32 r;
    u16 x0;
    u16 y0;

    switch (t->unk18 & 15)
    {
    case 0:
        t->moveCallback = (u32)TaskMoveRelativeToParent;
        t->drawCallback = (u32)TaskDrawWorldInView;
        t->layer = 8;
        u = gCurTask;
        u->frameTable = gUnk_08752020;
        u->tileWord = (u->u8C.parentTask)->tileWord | 0xF008;
        u->posX = 0;
        u->posY = 0;
        u->frame = 0xFFFF;
        TaskYieldTrampoline(18);
        gCurTask->frame = 6;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame = -1;
        TaskYieldTrampoline(12);
        if (gCurTask->pixelY <= 60)
        {
            gCurTask->frame = 9;
            TaskYieldTrampoline(3);
            gCurTask->frame |= -1;
            TaskYieldTrampoline(12);
            gCurTask->frame = 10;
            TaskYieldTrampoline(3);
        }
        else
        {
            gCurTask->frame = 1;
            TaskYieldTrampoline(3);
            gCurTask->frame |= -1;
            TaskYieldTrampoline(12);
            gCurTask->frame = 2;
            TaskYieldTrampoline(3);
        }
        o = ((gCurTask->u8C.parentTask)->tileWord & 0x7FF) << 5;
        RequestCopy(1, gUnk_082030D8, (void *)(o + (OBJ_VRAM0 + 0x80)), 0x180);
        RequestCopy(1, gUnk_082030D8 + 0x180, (void *)(o + (OBJ_VRAM0 + 0x480)), 0x180);
        RequestCopy(1, gUnk_082030D8 + 0x300, (void *)(o + (OBJ_VRAM0 + 0x880)), 0x180);
        RequestCopy(1, gUnk_082030D8 + 0x480, (void *)(o + (OBJ_VRAM0 + 0xC80)), 0x180);
        break;
    case 1:
        t->moveCallback = 0;
        t->drawCallback = (u32)PlayerEffectCrashBlastDraw;
        t->updateCallback = (u32)PlayerEffectCrashBlastUpdate;
        t->layer = 15;
        w = gCurTask;
        w->frameTable = gUnk_08752020;
        w->tileWord = ((w->u8C.parentTask)->tileWord + 0x800) | 4;
        x0 = gPlayerCameraPos[w->parent].x;
        x0 -= 120;
        y0 = gPlayerCameraPos[w->parent].y - 80;
        w->pixelX = (w->u8C.parentTask)->pixelX - gSpriteCameraX;
        w->pixelY = (w->u8C.parentTask)->pixelY - gSpriteCameraY;
        w->posX = (w->u8C.parentTask)->pixelX;
        w->posY = (w->u8C.parentTask)->pixelY;
        sub_0803f834(w->player->playerIndex, gUnk_0873BB3E);
        if (gUnk_02007D64 == 2 || gUnk_02007D64 == 3)
            gUnk_02007F60[29] = 0;
        v = gCurTask;
        v->unk28 = 0;
        v->unk2C = 0;
        sub_08065e6c();
        CpuSet(gUnk_03001570, gUnk_0200AF20, 96);
        gCurTask->frame = 0;
        gCurTask->unk6C = 0;
        do
        {
            if ((s16)gCurTask->unk6C == 10)
                BeginFade(6, 5, gUnk_02007F60);
            if ((s16)gCurTask->unk6C == 4)
                gCurTask->unk28 = 1;
            TaskYieldTrampoline(4);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 10);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        gScreenAttackActive = 0;
        for (i = 0; i < 20; i++)
            gScreenAttackTasks[i] |= -1;
        k = 0;
        for (i = 0; i <= 62; i++)
        {
            if (gInHub == 0 && gTaskSlotTypes[i] != -1)
            {
                switch (gTasks[i].actorKind)
                {
                case 1:
                case 2:
                case 7:
                case 8:
                    gScreenAttackTasks[k++] = i;
                    break;
                }
            }
        }
        m = 0;
        n = 0;
        while ((s16)gScreenAttackTasks[n] != -1 && n != 20)
        {
            gScreenAttackActive = 1;
            TaskRestoreSkipMask((s16)gScreenAttackTasks[n++]);
            m++;
        }
        TaskYieldTrampoline(1);
        while (n != 0)
        {
            n--;
            switch (gTasks[(s16)gScreenAttackTasks[n]].actorKind)
            {
            case 1:
            case 2:
            case 7:
            case 8:
                TaskSaveSkipMask((s16)gScreenAttackTasks[n]);
                break;
            default:
                gTasks[(s16)gScreenAttackTasks[n]].skipMask = 0;
                TaskSaveSkipMask((s16)gScreenAttackTasks[n]);
                break;
            }
            TaskSetSkipMask(15, (s16)gScreenAttackTasks[n]);
        }
        if (m != 0)
            TaskYieldTrampoline(3);
        gScreenAttackActive = 0;
        for (i = 32; i <= 62; i++)
        {
            if (gInHub != 0)
                continue;
            if (gTaskSlotTypes[i] == -1)
                continue;
            q = &gTasks[i];
            if (q->skipMask == 0)
                continue;
            if (q->drawCallback == 0)
                continue;
            if (q->pixelX >= (s16)x0 && q->pixelX < (s16)x0 + 240
                && q->pixelY >= (s16)y0 && q->pixelY < (s16)y0 + 160)
                gScreenAttackActive = 1;
            switch (gTasks[i].actorKind)
            {
            case 5:
                r = 0;
                if (gTasks[i].unk76 == 2)
                {
                    TaskRestoreSkipMask(i);
                    TaskYieldTrampoline(1);
                    r = 1;
                }
                break;
            case 6:
                r = 0;
                if (gTasks[i].unk76 != 5)
                {
                    TaskRestoreSkipMask(i);
                    TaskYieldTrampoline(1);
                    r = 1;
                }
                break;
            case 0:
            case 3:
            case 4:
            case 9:
                TaskRestoreSkipMask(i);
                TaskYieldTrampoline(2);
                r = 2;
                break;
            default:
                continue;
            }
            if (r != 0)
            {
                TaskSaveSkipMask(i);
                TaskSetSkipMask(15, i);
                TaskYieldTrampoline(2);
                gScreenAttackActive = 0;
            }
        }
        gScreenAttackActive = 0;
        gCurTask->player->unk16 = 0;
        while (gCurTask->player->terrainBox == 0)
            TaskYieldTrampoline(1);
        TaskYieldTrampoline(10);
        gCurTask->unk28 = 2;
        BeginFade(7, -4, gUnk_02007F60);
        TaskYieldTrampoline(8);
        gBrightness = 0;
        FreezeOtherTasks(0);
        gPauseDisabled = 0;
        break;
    }
    TaskExitTrampoline();
}

void PlayerEffectCrashBlastUpdate(void)
{
    s32 n;
    struct Task *t;
    struct Task *u;

    n = 6;
    if (gUnk_02007D64 == 2 || gUnk_02007D64 == 3)
        n = 5;
    switch (gCurTask->unk28)
    {
    case 0:
        break;
    case 1:
        if (gInHub != 0)
            BlendColors(gUnk_0200AF20, gUnk_0873BC3E, (u16)gCurTask->unk2C, n << 4, gUnk_03001570);
        else
            BlendColors(gUnk_0200AF20, gUnk_0873BB7E, (u16)gCurTask->unk2C, n << 4, gUnk_03001570);
        t = gCurTask;
        if (t->unk2C != 0x100)
        {
            t->unk2C += 10;
            if (t->unk2C > 0x100)
                t->unk2C = 0x100;
        }
        RegisterCollider((u8)gCurTaskIdx, gPlayerCameraPos[gCurTask->player->playerIndex].x,
                     gPlayerCameraPos[gCurTask->player->playerIndex].y, gUnk_0873C2B4);
        break;
    case 2:
        if (gInHub != 0)
            BlendColors(gUnk_0200AF20, gUnk_0873BC3E, (u16)gCurTask->unk2C, n << 4, gUnk_03001570);
        else
            BlendColors(gUnk_0200AF20, gUnk_0873BB7E, (u16)gCurTask->unk2C, n << 4, gUnk_03001570);
        u = gCurTask;
        if (u->unk2C != 0)
        {
            u->unk2C -= 46;
            if (u->unk2C < 0)
                u->unk2C = 0;
        }
        else
        {
            u->unk28 = 0;
            sub_08065ed0();
        }
        break;
    }
}

void PlayerEffectCrashBlastDraw(void)
{
    struct Task *t;
    s32 g;
    s16 x;

    if (gCurTask->frame != -1 && IsInView(gCurTask->posX, gCurTask->posY)
        && IsWorldPosOnScreen(gCurTask->posX, gCurTask->posY))
    {
        x = gUnk_0873BB26[(s16)gCurTask->unk6C];
        g = DrawAffineSprite(*gCurTask->frameTable, x, x, 0);
        t = gCurTask;
        QueueSprite(t->layer, g, t->spriteFlags, t->tileWord, t->pixelX, t->pixelY);
    }
}

void sub_0805acec(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    if ((t->unk18 & 15) == 0)
    {
        t->moveCallback = (u32)TaskMove;
        t->drawCallback = (u32)TaskDrawWorld;
        t->updateCallback = (u32)sub_0805ae00;
        t->layer = 5;
        u = gCurTask;
        u->frameTable = gUnk_0874C600;
        u->unk28 = 1;
        do
        {
            v = gCurTask;
            if ((v->u8C.parentTask)->onGround != 0)
            {
                v->posY = ((v->u8C.parentTask)->pixelY + 10) << 16;
                if (v->facing == 1)
                    v->posX = ((v->u8C.parentTask)->pixelX - 8) << 16;
                else
                    v->posX = ((v->u8C.parentTask)->pixelX + 8) << 16;
                TaskStop();
                TaskSetMotionXFacing(-0x30000, 0x6000);
                gCurTask->velY = -0x20000;
                gCurTask->accelY = 0x4000;
                TaskSetFrameByFacing(0);
                TaskYieldTrampoline(4);
                TaskSetFrameByFacing(10);
                TaskYieldTrampoline(2);
                gCurTask->frame += 2;
                TaskYieldTrampoline(2);
            }
            else
            {
                v->frame = 0xFFFF;
                TaskYieldTrampoline(1);
            }
        } while (gCurTask->unk28 != 0);
    }
    else
    {
        t->moveCallback = 0;
        t->drawCallback = 0;
        t->lateUpdateCallback = (u32)sub_0805ae00;
        TaskYieldTrampoline(6);
    }
    TaskExitTrampoline();
}

void sub_0805ae00(void)
{
    struct Task *t = gCurTask;
    s32 s = t->unk18 & 15;

    if (s == 0)
    {
        if (t->unk28 != 0 && (t->player->mode != 13 || (t->u8C.parentTask)->variant != 4))
            t->unk28 = 0;
        gCurTask->facing = (gCurTask->u8C.parentTask)->facing;
    }
    else
    {
        TaskBreakBlocksAt((struct HitBoxSet *)gUnk_0873CF8C,
                     (t->u8C.parentTask)->pixelX + ((s16)t->player->pixelOffsetX >> 8),
                     (t->u8C.parentTask)->pixelY + ((s16)t->player->pixelOffsetY >> 8), t->parent);
    }
}

void sub_0805ae94(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMoveRelativeToParent;
    gCurTask->drawCallback = (u32)sub_0805af80;
    gCurTask->updateCallback = (u32)sub_0805af44;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_08752090;
    t->tileWord = (t->u8C.parentTask)->tileWord | 0xF004;
    t->posX = 0;
    t->posY = 0;
    t->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0805af44(void)
{
    struct PlayerState *p = gCurTask->player;

    if (p->mode != 13 || !(gLatchedHeldKeys[p->playerIndex] & 3))
        TaskFree(gCurTaskIdx);
}

void sub_0805af80(void)
{
    if (gCurTask->player->mode != 13)
        TaskFree(gCurTaskIdx);
    else
        sub_0803dfc8();
}
