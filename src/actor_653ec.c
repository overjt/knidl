/* game_code_and_rodata 0x080653EC-0x080673EC (issue #65, module M17 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080653EC 0x080673EC src/actor_653ec.c --newpb
 *
 * Actor drawing and per-frame upkeep: OAM priority/palette packing
 * (ActorInitBossGfx), graphics upload out of the Task.frameTable descriptor table
 * (ActorFlashPalette/ActorSetDefaultPalette), the per-task update sweep (BossDefeatSweep), and
 * the class-2/class-4 task bodies that drive them.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "hud.h"
#include "cutscene.h"
#include "room.h"
#include "camera.h"
#include "actor.h"
#include "enemy.h"

extern void PlaySfx(u32 a);
extern void ActorCheckHitsWithExtraBox(void);
extern void ActorReactToHit(void);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void HudStartTaskHpBar(s16 a, s16 b);
extern void HudStartHpBar(s16 a, s16 b);
void ActorDropParasol(u32 def, u8 b);
extern void ActorSetAttackBox(u32 v);
extern void TaskSetEntry(void *fn, u32 i);
extern void ActorLoadDef(u32 def);
extern void ActorResetHealth(void);
extern void ActorSetState(u8 v);
extern void ActorCheckHits(void);
extern s32 CreateItemOrObject(u32 sub, u32 type, int p2Arg, int xArg, int yArg,
                        int prioArg, int altArg);

extern u32 TaskIsOnScreen(void);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void BlendColors(u32 a, u32 b, u32 c, u32 d, u32 e);

s16 ActorComputeHealthSlot(u32 i);
void ActorDropParasol(u32 def, u8 b);

void ActorDrawStreamedFrameNearViewOrDestroy(void)
{
    struct Task *p;

    p = gCurTask;
    if (p->frameTable == NULL)
        return;
    if (p->frame == -1)
        return;
    if (ActorIsInNearView() != 0)
    {
        if (TaskIsOnScreen() == 0)
            return;
        ActorDrawStreamedFrame();
    }
    else if (ActorIsInFarView() != 0)
    {
        HudRemoveHpBar();
        ActorDestroy();
    }
}

void ActorDrawStreamedFrameNearView(void)
{
    struct Task *p;

    p = gCurTask;
    if (p->frameTable == NULL)
        return;
    if (p->frame == -1)
        return;
    if (ActorIsInNearView() == 0)
        return;
    if (TaskIsOnScreen() == 0)
        return;
    ActorDrawStreamedFrame();
}

/* Push the running task's tile stream into OBJ VRAM, then draw it. */
void ActorDrawStreamedFrame(void)
{
    struct Task *t;
    struct TaskGfx *g;
    u16 *p;
    u16 *q;
    u32 dst;
    u32 prio;

    if (gCurTask->frameTable == NULL)
        return;
    if (gCurTask->frame == -1)
        return;
    prio = gCurTask->tileWord;
    dst = ((prio & 0x7FF) << 5) + (BG_VRAM + 0xFE00);
    g = (struct TaskGfx *)gCurTask->frameTable[gCurTask->frame];
    p = g->tiles;
    if (*p != 0xFFFF)
    {
        do
        {
            q = p + 1;
            RequestCopy(4, (u32)q, dst, *p);
            p = (u16 *)((u8 *)q + *p);
            dst += 0x400;
        } while (*p != 0xFFFF);
    }
    if ((*(u16 *)&gCurTask->u8C.actor->paletteOverridden & 0x101) == 0)
        ActorLoadPalette(g->palette + 1, *g->palette, 0);
    t = gCurTask;
    QueueSprite(t->layer, g->oamTemplate, t->spriteFlags, 0x800 | t->tileWord,
                 t->pixelX - gSpriteCameraX,
                 (s16)(t->pixelY - gSpriteCameraY));
}

/* Draw the running task's main sprite plus its optional second part. */
void ActorDrawSpriteAndExtra(void)
{
    struct Task *t;
    struct Actor *a;
    u32 *tbl;

    t = gCurTask;
    a = t->u8C.actor;
    tbl = t->frameTable;
    QueueSprite(t->layer, tbl[t->frame], t->spriteFlags, t->tileWord,
                 t->pixelX - gSpriteCameraX,
                 (s16)(t->pixelY - gSpriteCameraY));
    if (a->extraFrame != -1)
    {
        t = gCurTask;
        QueueSprite(a->extraLayerOffset + t->layer, tbl[a->extraFrame], t->spriteFlags, a->extraTileWord,
                     t->pixelX - gSpriteCameraX + a->unk16,
                     (s16)(t->pixelY - gSpriteCameraY + a->extraOffsetY));
    }
}

void ActorDrawWorldInViewOrDestroyWithExtra(void)
{
    struct Task *p;

    p = gCurTask;
    if (p->frameTable == NULL)
        return;
    if (p->frame == -1)
        return;
    if (ActorIsInView() != 0)
    {
        if (TaskIsOnScreen() == 0)
            return;
        ActorDrawSpriteAndExtra();
    }
    else
    {
        ActorDestroy();
    }
}

void ActorDrawSpriteAndExtraInView(void)
{
    struct Task *p;

    p = gCurTask;
    if (p->frameTable == NULL)
        return;
    if (p->frame == -1)
        return;
    if (ActorIsInView() == 0)
        return;
    if (TaskIsOnScreen() == 0)
        return;
    ActorDrawSpriteAndExtra();
}

/* Integrate the running task's velocity, clamped to its per-axis maximum. */
void ActorMove(void)
{
    struct Task *w;
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 m;
    s32 n;

    t = gCurTask;
    v = t->velX + t->accelX;
    t->velX = v;
    t->velY = t->velY + t->accelY;
    m = t->speedLimitX;
    if (m != 0x80000000)
    {
        if (v > 0)
        {
            if (v > m)
                t->velX = m;
        }
        else
        {
            if (v < -m)
                t->velX = -m;
        }
    }
    u = gCurTask;
    n = u->speedLimitY;
    if (n != 0x80000000)
    {
        if (u->velY > 0 && u->velY > n)
            u->velY = n;
    }
    w = gCurTask;
    w->posX = w->posX + w->velX;
    w->posY = w->posY + w->velY;
    w->pixelX = w->posX >> 16;
    w->pixelY = w->posY >> 16;
}

void TaskMoveRelativeToView(void)
{
    struct Task *t;

    TaskIntegrateMotion();
    t = gCurTask;
    t->pixelX = (t->posX >> 16) + gViewRect[0];
    t->pixelY = (t->posY >> 16) + gViewRect[2];
}

void SetPaletteAnimSource(u32 i, u32 p1, u8 p2)
{
    struct Task *t;
    struct Actor *a;
    s8 j;

    j = gPaletteAnimTasks[i];
    if (j != -1)
    {
        t = &gTasks[j];
        t->u8C.actor = a = &gActors[j];
        a->palette = p1;
        a->paletteVariant = p2;
    }
}

void ResetBgPaletteBlend(void)
{
    struct Task *t;
    s32 j;

    j = gPaletteAnimTasks[2];
    if (j != -1)
    {
        t = &gTasks[j];
        t->paletteAnimBlendMode = 2;
    }
}

void EndBgPaletteBlend(u32 v)
{
    struct Task *t;
    s32 j;

    j = gPaletteAnimTasks[2];
    if (j != -1)
    {
        t = &gTasks[j];
        t->paletteAnimBlendStep = v;
        t->paletteAnimBlendMode = 1;
    }
}

void StartBgPaletteBlend(u32 a, u32 b)
{
    struct Task *t;
    s32 j;

    AcquirePaletteAnim(4, 2);
    j = gPaletteAnimTasks[2];
    if (j != -1)
    {
        t = &gTasks[j];
        t->paletteAnimBlendStep = a;
        t->paletteAnimBlendMode = 0;
        t->paletteAnimBlendTarget = b;
        CpuSet(gBgPaletteBank2, gUnk_02005E10, 224);
    }
}

/* Reference-counted spawn of the slot-`idx` helper task. */
void AcquirePaletteAnim(u32 p0, s32 idx)
{
    struct Task *paletteAnim;
    s32 paletteAnimSlot;

    if (idx > 2)
        return;
    if (gPaletteAnimRefCounts[idx] == 0)
    {
        paletteAnimSlot = CreateChildTaskHere(TASK_PALETTE_ANIM, 1);
        if (paletteAnimSlot != -1)
        {
            paletteAnim = &gTasks[paletteAnimSlot];
            paletteAnim->paletteAnimPaletteBank = gCurTask->tileWord >> 12;
            paletteAnim->variant = p0;
            paletteAnim->paletteAnimRefIndex = idx;
            gPaletteAnimTasks[idx] = paletteAnimSlot;
        }
    }
    gPaletteAnimRefCounts[idx]++;
}

void Task_PaletteAnim(void)
{
    CallTableEntry(gCurTask->variant, 5, gPaletteAnimVariants);
}

/* Task body: cross-fade two palettes while the helper's refcount holds. */
/* Task body: cross-fade two palettes while the helper's refcount holds. */
/* Task body: cross-fade two palettes while the helper's refcount holds. */
void PaletteAnimVariant0(void)
{
    struct Task *w;
    struct Task *u;
    struct Task *v;
    struct Task *x;
    struct Task *y;

    gCurTask->paletteAnimPairTimer = 12;
    gCurTask->paletteAnimFromIndex = 2;
    gCurTask->paletteAnimToIndex = 3;
    gCurTask->paletteAnimPairRatio = 0;
    while (gPaletteAnimRefCounts[gCurTask->paletteAnimRefIndex] != 0)
    {
        u = gCurTask;
        u->paletteAnimPairTimer--;
        if (u->paletteAnimPairTimer == 0)
        {
            u->paletteAnimPairTimer = 12;
            u->paletteAnimFromIndex++;
            if (u->paletteAnimFromIndex > 3)
                u->paletteAnimFromIndex = 0;
            v = gCurTask;
            v->paletteAnimToIndex++;
            if (v->paletteAnimToIndex > 3)
                v->paletteAnimToIndex = 0;
            gCurTask->paletteAnimPairRatio = 0;
        }
        w = gCurTask;
        if (w->paletteAnimToIndex == 0)
            w->paletteAnimPairRatio = w->paletteAnimPairRatio + 128;
        else
            w->paletteAnimPairRatio = w->paletteAnimPairRatio + 64;
        x = gCurTask;
        if (x->paletteAnimPairRatio > 256)
            x->paletteAnimPairRatio = 256;
        y = gCurTask;
        BlendColors((u32)(gUnk_0825088C + (y->paletteAnimFromIndex << 5)),
                     (u32)(gUnk_0825088C + (y->paletteAnimToIndex << 5)), (u16)y->paletteAnimPairRatio,
                     16, (u32)(gObjPalette + (y->paletteAnimPaletteBank << 5)));
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

/* Task body: cycle the actor's 32-byte palette out of gUnk_0873DF38. */
/* Task body: cycle the actor's 32-byte palette out of gUnk_0873DF38. */
void PaletteAnimCycle(void)
{
    struct Task *w;
    struct Task *t;
    struct Task *u;
    struct Task *x;
    struct Actor *a;
    u32 v;

    t = gCurTask;
    a = t->u8C.actor;
    t->paletteAnimTimer = 10;
    t->paletteAnimCycleStep = 0;
    while (gPaletteAnimRefCounts[gCurTask->paletteAnimRefIndex] != 0 && a->paletteVariant <= 3)
    {
        u = gCurTask;
        if (u->paletteAnimTimer <= 0)
        {
            v = gUnk_0873DF38[a->paletteVariant][u->paletteAnimCycleStep];
            if (v == 0)
                v = a->palette;
            RequestCopy(2, v,
                         (u32)(gObjPalette + ((u->tileWord >> 12) << 5)), 32);
            w = gCurTask;
            w->paletteAnimTimer = 10;
            w->paletteAnimCycleStep = (w->paletteAnimCycleStep + 1) & 3;
        }
        x = gCurTask;
        x->paletteAnimTimer--;
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

/* Task body: flash one palette entry on and off. */
void PaletteAnimFlashColor(void)
{
    struct Task *w;
    struct Task *t;
    struct Task *u;
    struct Task *x;
    u32 off;

    t = gCurTask;
    t->paletteAnimTimer = 2;
    t->paletteAnimFlashOn = 0;
    off = (t->tileWord >> 12) << 5;
    off += 26;
    while (gPaletteAnimRefCounts[gCurTask->paletteAnimRefIndex] != 0)
    {
        u = gCurTask;
        if (u->paletteAnimTimer <= 0)
        {
            if (u->paletteAnimFlashOn != 0)
                RequestCopy(2, (u32)gUnk_0873DF78,
                             (u32)(gObjPalette + off), 2);
            else
                RequestCopy(2, (u32)gUnk_082530C8,
                             (u32)(gObjPalette + off), 2);
            w = gCurTask;
            w->paletteAnimTimer = 2;
            w->paletteAnimFlashOn ^= 1;
        }
        x = gCurTask;
        x->paletteAnimTimer--;
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

/* Task body: cross-fade the actor's palette pair out of gUnk_0873DF7C. */
void PaletteAnimVariant3(void)
{
    struct Task *w;
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *x;
    struct Task *y;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    t->paletteAnimPairTimer = 10;
    t->paletteAnimFromIndex = 1;
    t->paletteAnimToIndex = 2;
    t->paletteAnimPairRatio = 0;
    while (gPaletteAnimRefCounts[gCurTask->paletteAnimRefIndex] != 0)
    {
        u = gCurTask;
        u->paletteAnimPairTimer--;
        if (u->paletteAnimPairTimer == 0)
        {
            u->paletteAnimPairTimer = 10;
            u->paletteAnimFromIndex++;
            if (u->paletteAnimFromIndex > 2)
            {
                u->paletteAnimFromIndex = 0;
                u->paletteAnimPairTimer = 2;
            }
            v = gCurTask;
            v->paletteAnimToIndex++;
            if (v->paletteAnimToIndex > 2)
                v->paletteAnimToIndex = 0;
            gCurTask->paletteAnimPairRatio = 0;
        }
        w = gCurTask;
        if (w->paletteAnimToIndex == 0)
            w->paletteAnimPairRatio = w->paletteAnimPairRatio + 16;
        else
            w->paletteAnimPairRatio = w->paletteAnimPairRatio + 128;
        x = gCurTask;
        if (x->paletteAnimPairRatio > 256)
            x->paletteAnimPairRatio = 256;
        y = gCurTask;
        BlendColors(gUnk_0873DF7C[a->paletteVariant][y->paletteAnimFromIndex],
                     gUnk_0873DF7C[a->paletteVariant][y->paletteAnimToIndex], (u16)y->paletteAnimPairRatio, 16,
                     (u32)(gObjPalette + (y->paletteAnimPaletteBank << 5)));
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

/* Task body: fade the shared palette in, out, or straight to zero. */
void PaletteAnimBgBlend(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->paletteAnimBlendRatio = 0;
    while (gPaletteAnimRefCounts[gCurTask->paletteAnimRefIndex] != 0)
    {
        t = gCurTask;
        switch (t->paletteAnimBlendMode)
        {
        case 0:
            t->paletteAnimBlendRatio += t->paletteAnimBlendStep;
            if (t->paletteAnimBlendRatio > t->paletteAnimBlendTarget)
            {
                t->paletteAnimBlendRatio = t->paletteAnimBlendTarget;
                t->paletteAnimBlendMode = 3;
            }
            u = gCurTask;
            BlendColors((u32)gUnk_02005E10, (u32)gUnk_0873DFAC,
                         (u16)u->paletteAnimBlendRatio, 224, (u32)gBgPaletteBank2);
            break;
        case 1:
            t->paletteAnimBlendRatio -= t->paletteAnimBlendStep;
            if (t->paletteAnimBlendRatio < 0)
            {
                t->paletteAnimBlendRatio = 0;
                t->paletteAnimBlendMode = 3;
            }
            u = gCurTask;
            BlendColors((u32)gUnk_02005E10, (u32)gUnk_0873DFAC,
                         (u16)u->paletteAnimBlendRatio, 224, (u32)gBgPaletteBank2);
            break;
        case 2:
            BlendColors((u32)gUnk_02005E10, (u32)gUnk_0873DFAC, 0, 224,
                         (u32)gBgPaletteBank2);
            gCurTask->paletteAnimBlendMode = 3;
            break;
        }
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

/* Point the actor at the palette its class/sub/level combination wants. */
void ActorSelectPaletteVariant(u32 i)
{
    struct Task *t;
    struct Actor *a;
    u32 *tbl;
    u32 v;
    u32 k;
    u32 *q;

    t = &gTasks[i];
    a = t->u8C.actor;
    switch (t->actorKind)
    {
    case ACTOR_KIND_ENEMY:
        tbl = gEnemyPaletteVariants[t->u76.subtype];
        break;
    case ACTOR_KIND_MID_BOSS:
        tbl = gMidBossPaletteVariants[t->u76.subtype];
        break;
    default:
        tbl = NULL;
        break;
    }
    if (a->paletteVariant > 3 || tbl == NULL)
    {
        a->paletteVariant = 0;
    }
    else
    {
        k = a->paletteVariant - 1;
        q = tbl + k;
        v = *q;
        if (v != 0)
            a->palette = v;
    }
}

/* Copy one palette bank out of the class/sub/level palette tables.
   `sub` is reused as the table pointer, matching the ROM's register use. */
/* Copy one palette bank out of the class/sub/level palette tables.  `sub`
   doubles as the table pointer and `src` as the source address, matching
   the ROM's register use. */
/* Copy one palette bank out of the class/sub/level palette tables.
   `sub` is reused as the table pointer, matching the ROM's register use. */
void LoadActorPaletteVariant(u32 slot, u32 sub, u32 level, u32 n, u8 kind, u32 src)
{
    u32 off;
    u32 k;
    u32 *q;
    u32 p;

    switch (kind)
    {
    case 0:
        sub = (u32)gEnemyPaletteVariants[sub];
        break;
    case 1:
        sub = (u32)gMidBossPaletteVariants[sub];
        break;
    default:
        sub = 0;
        break;
    }
    if (level > 3)
        return;
    if (sub == 0)
        return;
    if (kind == 1 && level == 0)
    {
        p = src;
    }
    else
    {
        k = level - 1;
        q = (u32 *)sub + k;
        p = *q;
    }
    if (p == 0)
        return;
    off = ((u32 *)sub)[4] << 1;
    p += off;
    slot <<= 5;
    slot += off;
    if (kind == 0)
        n = ((u32 *)sub)[5];
    RequestCopy(2, p, (u32)(gObjPalette + slot), n << 1);
}

void LoadEnemyPaletteVariant(u32 slot, u32 sub, u32 level)
{
    LoadActorPaletteVariant(slot, sub, level, 0, 0, 0);
}

void sub_08065dd0(u32 slot, u32 i)
{
    u32 p;

    p = gUnk_0873F01C[i];
    if (p != 0)
        RequestCopy(2, p, (u32)(gObjPalette + (slot << 5)), 32);
}

void sub_08065dfc(u32 slot)
{
    slot <<= 5;
    RequestCopy(2, (u32)gUnk_0825CA44, (u32)(gObjPalette + slot), 32);
}

void ActorPlaySfx(u32 def, u32 which)
{
    struct Actor *a;
    s32 v;

    a = gCurTask->u8C.actor;
    if (which != 0)
    {
        v = a->unk34;
        if (v == -2)
            return;
        if (v == -1)
            PlaySfx(def);
        else
            PlaySfx(v);
    }
    else
    {
        v = a->sfxOverride;
        if (v == -2)
            return;
        if (v == -1)
            PlaySfx(def);
        else
            PlaySfx(v);
    }
}

/* Set bit 0 of every live enemy actor's two flag bytes. */
void LockAllActorPalettes(void)
{
    struct Task *t;
    struct Actor *a;
    s32 i;

    for (i = 32; i < 63; i++)
    {
        if (gTaskSlotTypes[i] != -1 && gTaskSlotTypes[i] != TASK_MAP_EVENT)
        {
            t = &gTasks[i];
            a = t->u8C.actor;
            if (a != NULL && (u8)(t->actorKind - 7) > 3)
            {
                a->paletteOverridden |= 1;
                a->paletteLocked |= 1;
            }
        }
    }
}

void ClearActorPaletteOverrides(void)
{
    struct Task *t;
    struct Actor *a;
    s32 i;

    for (i = 32; i < 63; i++)
    {
        if (gTaskSlotTypes[i] != -1 && gTaskSlotTypes[i] != TASK_MAP_EVENT)
        {
            t = &gTasks[i];
            a = t->u8C.actor;
            if (a != NULL && (u8)(t->actorKind - 7) > 3)
            {
                a->paletteOverridden = 0;
                a->paletteLocked = 0;
            }
        }
    }
}

u8 TaskHasSameSerial(u32 i)
{
    struct Task *g;
    struct Task *a;
    struct Task *b;

    g = gTasks;
    a = &g[gCurTaskIdx];
    b = &g[i];
    if (a->serial == b->serial)
        return 1;
    return 0;
}

s16 ActorComputeHealth(void)
{
    return ActorComputeHealthSlot(gCurTaskIdx);
}

/* Horizontal draw offset of task `i`'s sprite for the current view mode. */
/* Horizontal draw offset of task `i`'s sprite for the current view mode. */
s16 ActorComputeHealthSlot(u32 i)
{
    struct Task *t;
    struct Actor *a;
    s16 w;
    s16 adj;
    s16 r;
    s32 m;

    t = &gTasks[i];
    a = t->u8C.actor;
    switch (gActivePlayerCount)
    {
    case 2:
        w = a->def->health2Players;
        break;
    case 3:
        w = a->def->health3Players;
        break;
    case 4:
        w = a->def->health4Players;
        break;
    case 1:
    default:
        w = a->def->health1Player;
        break;
    }
    switch (t->actorKind)
    {
    case ACTOR_KIND_MID_BOSS:
        m = gUnk_02007D60 & 15;
        if (w > 30)
            adj = m << 2;
        else
            adj = m << 1;
        break;
    case ACTOR_KIND_BOSS:
        m = gUnk_02007FF0 & 15;
        if (w > 30)
            adj = m << 2;
        else
            adj = m << 1;
        break;
    default:
        adj = 0;
        break;
    }
    r = w - adj;
    return r + a->healthBonus;
}

u32 *sub_0806601c(void)
{
    struct Task *t;
    struct Actor *a;
    u32 *r;
    u32 *tbl;
    u32 v;
    u32 k;
    u32 *q;

    t = gCurTask;
    a = t->u8C.actor;
    if (t->actorKind == ACTOR_KIND_MID_BOSS)
    {
        r = gUnk_0873F0C4[t->u76.subtype];
        if (a->paletteVariant != 0)
        {
            tbl = gMidBossPaletteVariants[t->u76.subtype];
            if (tbl != NULL)
            {
                k = a->paletteVariant - 1;
                q = tbl + k;
                v = *q;
                if (v != 0)
                    a->palette = v;
            }
        }
    }
    else
    {
        r = gUnk_0873F138[t->u76.subtype];
    }
    return r;
}

u16 ActorInitBossGfx(u32 mode)
{
    struct Task *t;
    struct Actor *a;
    u32 *p;
    u16 prio;
    s32 sh;
    u32 lo;
    u32 w40;

    a = gCurTask->u8C.actor;
    a->savedTileWord = gCurTask->tileWord;
    prio = gCurTask->tileWord;
    p = sub_0806601c();
    t = gCurTask;
    if (t->actorKind == ACTOR_KIND_MID_BOSS)
        a->gfx.header = (struct GfxHeader *)gMidBossGfx[t->u76.subtype];
    else
        a->gfx.header = (struct GfxHeader *)gBossGfx[t->u76.subtype];
    a->gfx.tileBits = gCurTask->tileWord & 0xFFF;
    a->gfx.paletteBank = gCurTask->tileWord >> 12;
    if (p != NULL)
    {
        if (mode == 1)
        {
            sh = AllocObjTiles(p[0] << 4);
            lo = a->gfx.paletteBank;
        }
        else
        {
            sh = AllocObjTilesAndPalettes(p[0] << 4, p[1]);
            lo = sh & 0xFFFF;
            sh = sh >> 16;
        }
        w40 = (lo << 12) | ((sh << 1) + 16);
        gCurTask->tileWord = w40;
    }
    else
    {
        ActorLoadPalette(a->gfx.header->palette, a->gfx.header->paletteBankCount << 5, 0);
    }
    return prio;
}

void sub_08066144(void)
{
    s32 i;

    for (i = 0; i < 10; i++)
        gUnk_02007D00[i] = 0;
    for (i = 0; i < 8; i++)
        gUnk_02006190[i] = 0;
    for (i = 0; i < 10; i++)
        gUnk_02006040[i] = 0;
    gBossHitStunCallback = gBossHitStunFlashPalette = 0;
}

void BossStartHitStun(u32 p0, u32 p1, u32 p2, u16 p3, u8 p4)
{
    struct Task *t;

    TaskSetSkipMask((TASK_SKIP_COROUTINE | TASK_SKIP_MOVE | TASK_SKIP_UPDATE), gCurTaskIdx);
    t = gCurTask;
    t->lateUpdateCallback = (u32)BossHitStunLateUpdate;
    gUnk_02006190[0] = t->pixelX;
    gUnk_02006190[1] = t->pixelY;
    gUnk_02006190[2] = t->frame;
    gBossHitStunFlashPalette = p2;
    gUnk_02006190[4] = p3;
    gUnk_02006190[3] = p0;
    gBossHitStunCallback = p1;
    gUnk_02006190[5] = p4;
}

void BossEndHitStun(void)
{
    struct Task *t;
    struct Actor *a;

    a = gCurTask->u8C.actor;
    TaskSetSkipMask(0, gCurTaskIdx);
    t = gCurTask;
    t->lateUpdateCallback = 0;
    t->pixelX = gUnk_02006190[0];
    t->pixelY = gUnk_02006190[1];
    t->frame = gUnk_02006190[2];
    if (gBossHitStunFlashPalette != 0)
    {
        if (gUnk_02006190[5] != 0)
            ActorClearPaletteOverride();
        else
            ActorLoadHeaderPalette(a->gfx.header);
    }
}

/* Walk one step of the queued knock-back path. */
void BossHitStunShake(void)
{
    struct Task *t;
    s32 i;
    s16 j;

    i = gUnk_02006190[3];
    if (i < 0)
        return;
    j = gUnk_0873E184[i] * 2;
    t = gCurTask;
    t->pixelX += gUnk_0873E16C[j];
    t->pixelY += gUnk_0873E16C[j + 1];
    gUnk_02006190[3]--;
}

void BossHitStunLateUpdate(void)
{
    struct Actor *a;
    u32 v;

    a = gCurTask->u8C.actor;
    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
    if (gBossHitStunFlashPalette != 0)
    {
        v = gUnk_02006190[5];
        switch (v)
        {
        case 1:
            ActorFlashPalette((void *)gBossHitStunFlashPalette, gUnk_02006190[4]);
            break;
        case 0:
            ActorFlashHeaderPalette(a->gfx.header, gBossHitStunFlashPalette, gUnk_02006190[4]);
            break;
        }
    }
    BossHitStunShake();
    if (gBossHitStunCallback != 0)
        ((void (*)(void))gBossHitStunCallback)();
}

/* True when every active player is in state 1. */
u8 AreAllPlayersOnGround(void)
{
    s32 n;
    s32 m;
    s32 i;
    struct Task *t;

    n = 0;
    m = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            n++;
            t = &gTasks[i];
            if (t->onGround == 1)
                m++;
        }
    }
    if (n != 0 && n == m)
        return 1;
    return 0;
}

s32 GetLivingActivePlayerHealth(void)
{
    s32 i;
    s32 v;

    i = 0;
    v = 0;
    for (; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            v = gPlayerHealth[i];
            if (v != 0)
                break;
        }
    }
    return v;
}

void ActorFlashPalette(void *src, u32 size)
{
    struct Task *t;
    struct Actor *a;
    struct TaskGfx *g;
    struct Task *u;
    u32 *tbl;

    t = gCurTask;
    if (t->frameTable == NULL)
        return;
    if (t->frame == -1)
        return;
    a = t->u8C.actor;
    if (a->paletteLocked & 1)
        return;
    a->paletteOverridden |= 1;
    u = gCurTask;
    tbl = u->frameTable;
    g = (struct TaskGfx *)tbl[u->frame];
    if ((gFrameCount & 2) == 0)
        ActorLoadPalette(g->palette + 1, *g->palette, 0);
    else
        ActorLoadPalette(src, size << 1, 1);
}

void ActorClearPaletteOverride(void)
{
    gCurTask->u8C.actor->paletteOverridden &= 254;
}

void ActorFlashHeaderPalette(struct GfxHeader *h, u32 src, u32 size)
{
    struct Actor *a;

    a = gCurTask->u8C.actor;
    if (a->paletteLocked & 1)
        return;
    if ((gFrameCount & 2) == 0)
        ActorLoadPalette(h->palette, h->paletteBankCount << 5, 0);
    else
        ActorLoadPalette((void *)src, size << 1, 1);
}

void ActorLoadHeaderPalette(struct GfxHeader *h)
{
    ActorLoadPalette(h->palette, h->paletteBankCount << 5, 0);
}

/* Task body: hand the actor over to the "carried" routine at 0x08066754. */
void ActorIntroPoseUntilHpBarFull(struct AnimCmd *p)
{
    struct Task *t;

    sub_08066544();
    TaskSetSkipMask(TASK_SKIP_LATE_UPDATE, gCurTaskIdx);
    t = gCurTask;
    t->actorSavedUpdateCallback = t->updateCallback;
    t->updateCallback = (u32)ActorIntroPoseUpdate;
    ActorStopAnim();
    if (p != NULL)
        gCurTask->actorAnimDelay24 = ActorStartAnim(p);
    if (gHudHpBarFilled == 0)
    {
        do
        {
            TaskYieldTrampoline(1);
        } while (gHudHpBarFilled == 0);
    }
    ActorResetAttackBox();
    ActorEndIntroPose();
}

void sub_08066544(void)
{
    struct Actor *a;

    a = gCurTask->u8C.actor;
    ActorSetAttackBox(a->aux->altAttackBox);
    ActorShowHpBar();
}

void ActorResetAttackBox(void)
{
    struct Actor *a;

    a = gCurTask->u8C.actor;
    ActorSetAttackBox(a->def->attackBox);
}

void sub_08066580(void)
{
    struct Actor *a;

    a = gCurTask->u8C.actor;
    ActorShowHpBar();
    ActorSetAttackBox(a->def->attackBox);
}

void ActorShowHpBar(void)
{
    struct Task **g;
    struct Task *t;
    s32 v;

    g = &gCurTask;
    gHudHpBarFilled = 0;
    HudShowHpBar();
    t = *g;
    if (t->actorKind == ACTOR_KIND_MID_BOSS || (t->actorKind == ACTOR_KIND_BOSS && t->u76.subtype == 7))
    {
        v = t->health;
        HudStartTaskHpBar(v, v);
    }
    else
    {
        v = gCurTask->health;
        HudStartHpBar(v, v);
    }
}

u16 ActorGetGfxTileWord(void)
{
    return ActorGetGfxTileWordPalOffset(0);
}

u16 ActorGetGfxTileWordPalOffset(u16 a)
{
    struct Actor *p;

    p = gCurTask->u8C.actor;
    return ((a + p->gfx.paletteBank) << 12) | p->gfx.tileBits;
}

u16 ActorGetTileWordPalOffset(u16 a)
{
    struct Task *t;
    u16 v;
    u16 w;
    u32 r;
    s32 m;

    t = gCurTask;
    v = t->tileWord;
    w = a + (v >> 12);
    r = w << 12;
    m = 0xFFF;
    m &= v;
    return r | m;
}

void ActorStartIntroPose(struct AnimCmd *p)
{
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    t->actorSavedUpdateCallback = t->updateCallback;
    t->updateCallback = (u32)ActorIntroPoseUpdate;
    ActorStopAnim();
    if (p != NULL)
        gCurTask->actorAnimDelay24 = ActorStartAnim(p);
    TaskSetSkipMask(TASK_SKIP_LATE_UPDATE, gCurTaskIdx);
    ActorSetAttackBox(a->aux->altAttackBox);
}

void ActorEndIntroPose(void)
{
    struct Task *t;

    TaskSetSkipMask(0, gCurTaskIdx);
    t = gCurTask;
    t->updateCallback = t->actorSavedUpdateCallback;
    t->actorAnimDelay24 = 0;
    t->actorSavedUpdateCallback = 0;
}

void ActorIntroPoseUntilMidBossFight(struct AnimCmd *p)
{
    ActorStartIntroPose(p);
    if (gMidBossFightState == 0)
    {
        do
        {
            TaskYieldTrampoline(1);
        } while (gMidBossFightState == 0);
    }
    ActorEndIntroPose();
}

void ActorIntroPoseUntilScrollLocked(struct AnimCmd *p)
{
    ActorStartIntroPose(p);
    while (BossCheckScrollLock() == 0)
        TaskYieldTrampoline(1);
    ActorEndIntroPose();
}

u32 BossCheckScrollLock(void)
{
    switch (gCurTask->u76.subtype)
    {
    case 5:
        return WhispyWoodsCheckScrollLock();
    case 6:
        return KrackoCheckScrollLock();
    case 0:
        return KingDededeCheckScrollLock();
    }
    return 0;
}

void ActorIntroPoseUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->actorKind == ACTOR_KIND_BOSS && t->u76.subtype == 5)
        gCurTask->actorAnimDelay24 = ActorTickAnim(t->actorAnimDelay24);
    else
        gCurTask->actorAnimDelay24 = ActorTickAnimFacingNearestPlayer(gCurTask->actorAnimDelay24);
    ActorCheckHitsWithExtraBox();
    ActorReactToHit();
}

void ArenaDropMaximTomato(void)
{
    if (gGameState == GAME_STATE_ARENA)
        CreateItemAt(2, TASK_MAXIM_TOMATO, 0, 128, 0);
}

void MidBossStartDefeat(u8 a, u16 b)
{
    struct Task *t;
    struct Actor *p;

    p = gCurTask->u8C.actor;
    HudRemoveHpBar();
    t = gCurTask;
    if (t->drawCallback == (u32)ActorDrawStreamedFrameNearView || t->drawCallback == (u32)ActorDrawStreamedFrameNearViewOrDestroy)
        t->drawCallback = (u32)ActorDrawStreamedFrameNearViewOrDestroy;
    else
        t->drawCallback = (u32)ActorDrawWorldNearViewOrDestroy;
    gCurTask->health += gUnk_0873E1B4[gActivePlayerCount - 1];
    p->hitState = 2;
    p->score = 0;
    ActorSetDefaultPalette(a);
    ActorFaceHitter();
    TaskSetFrame((s16)b);
    MidBossDefeatFlash();
}

void EndMidBossFightWithReward(void)
{
    EndMidBossFight();
    ArenaDropMaximTomato();
}

void ActorLoadPalette(void *src, u32 size, u8 force)
{
    struct Task *t;
    struct Actor *a;
    u32 slot;

    t = gCurTask;
    a = t->u8C.actor;
    slot = t->tileWord >> 12;
    if (force == 0 && t->actorKind == ACTOR_KIND_MID_BOSS && a->paletteVariant != 0 && a->palette != 0)
        RequestCopy(2, a->palette, (u32)(gObjPalette + (slot << 5)), size);
    else
        RequestCopy(2, (u32)src, (u32)(gObjPalette + (slot << 5)), size);
    a->paletteColorCount = size >> 1;
}

/* Retire every other live task the running one is allowed to clean up. */
void BossDefeatSweep(void)
{
    struct Task *t;
    struct Actor *a;
    u32 *p;
    u32 cls;
    u32 v;
    s32 i;

    a = gCurTask->u8C.actor;
    for (i = 32; i <= 62; i++)
    {
        if (gTaskSlotTypes[i] == -1)
            continue;
        if (i == gCurTaskIdx)
            continue;
        if (gTaskSlotTypes[i] == TASK_MAP_EVENT)
            continue;
        t = &gTasks[i];
        if (gCurTask->actorKind == ACTOR_KIND_BOSS && a->defeatSweepCallback != 0)
        {
            if ((u8)((u8 (*)(s32))a->defeatSweepCallback)(i) != 1)
                continue;
        }
        p = &cls;
        cls = t->actorKind;
        if (*p == ACTOR_KIND_ITEM && t->u76.subtype == 0)
            continue;
        if (*p == 10)
            continue;
        v = *p;
        if (v == 9 || v == ACTOR_KIND_BOSS_CHILD_TASK || v == ACTOR_KIND_MID_BOSS_CHILD_TASK)
        {
            if (gTaskSlotTypes[i] == TASK_PALETTE_ANIM)
                continue;
            ActorDestroySlot(i);
        }
        else
        {
            sub_08066988(i);
        }
    }
}

/* Park task `i`: reset it to the idle body or kill it outright. */
void sub_08066988(u32 i)
{
    struct Task *t;
    struct Actor *a;
    struct PlayerState *p;

    t = &gTasks[i];
    a = t->u8C.actor;
    if (a->attachEffect != 0)
    {
        p = t->player;
        ActorAttachedReleaseCarrierSlot(i);
        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        if (a->attachEffect != 1)
        {
            if ((u8)(p->playerHoldPose + 2) <= 1)
            {
                ActorDestroySlot(i);
                return;
            }
        }
    }
    t->lateUpdateCallback = 0;
    t->updateCallback = 0;
    t->hitEffect = 0;
    TaskSetEntry(ActorDie, i);
    TaskSetSkipMask(0, i);
}

/* Is the running task inside the camera window padded by (dx, dy)? */
u8 ActorIsInViewMargin(s16 dx, u16 dy)
{
    if (gViewRect[0] - dx < gCurTask->pixelX
        && gCurTask->pixelX < gViewRect[1] + dx
        && gViewRect[2] - (s16)dy < gCurTask->pixelY
        && gCurTask->pixelY < gViewRect[3] + (s16)dy)
        return 1;
    return 0;
}

u8 ActorIsInNearView(void)
{
    return ActorIsInViewMargin(80, 40);
}

u8 ActorIsInFarView(void)
{
    return ActorIsInViewMargin(360, 240);
}

void ActorSetDefaultPalette(u8 mode)
{
    struct Task *t;
    struct Actor *a;
    struct TaskGfx *g;
    u16 *p;
    u32 *tbl;
    u32 n;

    t = gCurTask;
    a = t->u8C.actor;
    if (a->palette != 0)
        return;
    if (mode == 1)
    {
        tbl = t->frameTable;
        g = (struct TaskGfx *)tbl[t->frame];
        p = g->palette;
        a->palette = (u32)(p + 1);
        n = *p >> 1;
    }
    else
    {
        p = (u16 *)a->gfx.header;
        a->palette = ((u32 *)p)[2];
        n = *p << 4;
    }
    a->paletteColorCount = n;
}

void MidBossResetHealth(void)
{
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    if (t->variant != 0)
    {
        switch (gActivePlayerCount)
        {
        case 2:
            a->healthBonus = 13;
            break;
        case 3:
            a->healthBonus = 16;
            break;
        case 4:
            a->healthBonus = 18;
            break;
        case 1:
        default:
            a->healthBonus = 10;
            break;
        }
        gCurTask->variant = 0;
    }
    ActorResetHealth();
}

void ActorStartCarryingParasol(u32 def)
{
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    if (a->extraFrame != -1)
    {
        t->drawCallback = (u32)ActorDrawWorldInViewOrDestroyWithParasol;
        ActorLoadDef(def);
        gCurTask->u8C.actor->unk16 = 6;
    }
}

void TaskBounceParasolDriftOffWall(void)
{
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    if (a->extraFrame != -1)
    {
        if (t->velX >= 0)
            t->actorParasolSwayStep = 3;
        else
            t->actorParasolSwayStep = 9;
    }
    else
    {
        TaskTurnAroundAndReverseX();
    }
}

void TaskStartParasolDrift(void)
{
    struct Task *t;

    TaskFaceNearestPlayer();
    TaskStop();
    t = gCurTask;
    t->velY = 0x8000;
    if (t->facing == 1)
        t->actorParasolSwayStep = 0;
    else
        t->actorParasolSwayStep = 6;
}

void TaskStepParasolDrift(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->actorParasolSwayStep = t->actorParasolSwayStep + 1;
    if (t->actorParasolSwayStep > 11)
        t->actorParasolSwayStep = 0;
    u = gCurTask;
    u->velX = gParasolDriftSwayVelX[u->actorParasolSwayStep];
}

void ActorDropParasol(u32 def, u8 b)
{
    gCurTask->u8C.actor->extraFrame = 0xFFFF;
    ActorLoadDef(def);
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    CreateDroppedParasol(b);
}

void ActorDropParasolOnLanding(u32 def)
{
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    if (a->extraFrame != -1 && t->unk74 != 2)
        ActorDropParasol(def, 1);
}

/* Draw the running task plus its trailing "sparkle" sprite. */
/* Draw the running task plus its trailing "sparkle" sprite. */
void ActorDrawWorldInViewOrDestroyWithParasol(void)
{
    struct Task *p;
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    u32 *tbl;
    s16 x;
    s16 y;

    p = gCurTask;
    if (p->frameTable == NULL)
        return;
    if (p->frame == -1)
        return;
    if (ActorIsInView() != 0)
    {
        if (TaskIsOnScreen() == 0)
            return;
        t = gCurTask;
        tbl = t->frameTable;
        QueueSprite(t->layer, tbl[t->frame], t->spriteFlags, t->tileWord,
                     t->pixelX - gSpriteCameraX,
                     (s16)(t->pixelY - gSpriteCameraY));
        u = gCurTask;
        if ((u->skipMask & (TASK_SKIP_COROUTINE | TASK_SKIP_MOVE | TASK_SKIP_UPDATE)) == 0)
        {
            if (u->frame == 3)
            {
                u->u8C.actor->extraFrame = 3;
            }
            else
            {
                a = u->u8C.actor;
                if (a->unk16 <= 0)
                {
                    a->unk16 = 6;
                    u->u8C.actor->extraFrame++;
                    a = u->u8C.actor;
                    if (a->extraFrame > 7)
                        a->extraFrame = 4;
                }
            }
        }
        ActorGetParasolOffset();
        t = gCurTask;
        x = t->pixelX - gSpriteCameraX + gUnk_030023B4;
        y = t->pixelY - gSpriteCameraY + gUnk_030023D4;
        QueueSprite(t->layer, gParasolFrames[t->u8C.actor->extraFrame], t->spriteFlags, 0,
                     x, y);
        a = gCurTask->u8C.actor;
        a->unk16--;
    }
    else
    {
        ActorDestroy();
    }
}

/* Offset of the trailing sprite for the running task's animation. */
/* Offset of the trailing sprite for the running task's animation. */
/* Offset of the trailing sprite for the running task's animation. */
/* Offset of the trailing sprite for the running task's animation. */
void ActorGetParasolOffset(void)
{
    struct Task *t;
    s32 i;
    s32 j;

    t = gCurTask;
    i = t->frame;
    j = i * 2;
    switch (t->u76.subtype)
    {
    case 0:
        gUnk_030023B4 = gUnk_0873E1E8[i * 2] * t->facing;
        gUnk_030023D4 = gUnk_0873E1E8[j + 1];
        break;
    case 9:
        gUnk_030023B4 = gUnk_0873E220[i * 2] * t->facing;
        gUnk_030023D4 = gUnk_0873E220[j + 1];
        break;
    case 17:
        gUnk_030023B4 = 0;
        if (t->frame == 3)
            gUnk_030023D4 = 13;
        else
            gUnk_030023D4 = -13;
        break;
    default:
        sub_0806ee2c();
        gUnk_030023B4 = gUnk_030023D4 = 0;
        break;
    }
}

void CreateDroppedParasol(u8 a)
{
    struct Task *t;
    struct Task *s;
    struct Task *u;
    s32 i;
    u8 kind;
    s32 v;
    s16 x;
    s16 y;

    if (gCurTask->unk74 == 1)
        kind = 1;
    else
        kind = 0;
    ActorGetParasolOffset();
    t = gCurTask;
    x = t->pixelX + gUnk_030023B4;
    y = t->pixelY + gUnk_030023D4;
    i = CreateActorByKind(ACTOR_KIND_ENEMY, 38, kind, 0, x, y, 0);
    if (i != -1)
    {
        u = &gTasks[i];
        s = gCurTask;
        v = (s->hitterPlayer == -1) ? TaskFindNearestPlayer() : s->hitterPlayer;
        u->unk1C = v;
        u->unk28 = a;
        if (gScreenAttackActive == 1)
        {
            gCurTask->u8C.actor->keepExtraOnDefeat = 1;
            u->variant = 3;
            u->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
            u->layer = 11;
            u->frameTable = gParasolFrames;
            u->frame = 0;
        }
    }
}

void CreateNextRoomWarpStar(s32 x, s32 y)
{
    CreateWarpStar(x - gViewRect[0], y - gViewRect[2], 23);
    PlayBgm(1);
}

void LoadStarRodPieceGfx(void)
{
    struct GfxHeader *h;

    h = (struct GfxHeader *)gUnk_08334DC0;
    RequestCopy(4, (u32)h->tiles, OBJ_VRAM0 + 0x7800, h->tileCount << 5);
    RequestCopy(2, gStarRodPiecePalettes[gLevelIndex], (u32)gObjPaletteBank13,
                 h->paletteBankCount << 5);
}

void CreateStarRodPiece(u8 p3, s16 x, s16 y)
{
    struct Task *t;
    s32 i;

    i = CreateItemOrObject(5, TASK_STAR_ROD_PIECE, 0, x, y, 0xD3D0, 1);
    if (i != -1)
    {
        t = &gTasks[i];
        if (gGameState == GAME_STATE_STAGE && gMetaKnightmareMode == 0)
        {
            t->variant = p3;
            PlayBgm(1);
        }
        else
        {
            t->variant = 2;
            t->unk74 = gCurTask->u76.subtype;
        }
    }
}

void CreateRoomStarRodPiece(void)
{
    LoadStarRodPieceGfx();
    CreateStarRodPiece(0, 128, 104);
}

u8 IsMidBossDroppingIn(void)
{
    if (gMidBossDropsIn == 0)
        return 0;
    return 1;
}

u8 CountActivePlayers(void)
{
    s32 i;
    s32 n;

    for (i = 0, n = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
            n++;
    }
    return n;
}

void FreezeStage(u16 a)
{
    TaskFreezeOrThawOthers(a, gCurTaskIdx);
    TaskSetSkipMask(0, 63);
    PauseRoom();
    DisablePause();
}

void ThawStage(void)
{
    TaskFreezeOrThawOthers(0, gCurTaskIdx);
    ResumeRoom();
    EnablePause();
}

void LoadBackdropColor(u32 src)
{
    RequestCopy(2, src, (u32)gBgPalette, 2);
}

void DisablePause(void)
{
    gPauseDisabled = 1;
}

void EnablePause(void)
{
    gPauseDisabled = 0;
}

s32 CreateInhalableStar(s16 x, s16 y, u16 dir, u8 p8)
{
    struct ActorSpawn sp;
    struct Task *t;
    s32 i;

    sp.subtype = 36;
    sp.taskType = TASK_INHALABLE_STAR;
    sp.variant = p8;
    sp.spawnArg = 0;
    sp.x = x;
    sp.y = y;
    sp.tileWord = 0;
    sp.checkTerrain = 0;
    i = CreateActorFromDesc(&sp, 1);
    if (i != -1)
    {
        t = &gTasks[i];
        t->facing = dir;
    }
    return i;
}

void InhalableStarInitVariant(void)
{
    s32 v;

    v = gCurTask->variant;
    switch (v)
    {
    case 1:
        ActorLoadDef((u32)&gUnk_0873F690);
        break;
    case 2:
    case 3:
        ActorSetAttackBox((u32)gInhalableStarInitVariantAttackBox);
        break;
    }
    if (gCurTask->facing == 0)
        TaskFaceLikeParent();
}

void Task_InhalableStar(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    t = gCurTask;
    t->frameTable = gInhalableStarFrames;
    t->updateCallback = (u32)InhalableStarUpdate;
    InhalableStarInitVariant();
    ActorSetState(INHALABLE_STAR_STATE_0);
    CallTableEntry(gCurTask->state, 1, gInhalableStarStates);
}

void InhalableStarUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gInhalableStarStateUpdates);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

/* Task body: the player's "spin out and vanish" death animation. */
void InhalableStarState0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    gCurTask->updateState = INHALABLE_STAR_STATE_0;
    gCurTask->onGround = 0;
    gCurTask->unk24 = 2;
    TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
    t = gCurTask;
    t->velY = 0xFFFE8000;
    t->frame = 13;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame--;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame--;
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    u = gCurTask;
    u->velY = 0xFFFF8000;
    u->inhalableStarLoopCount = 0;
    do
    {
        t = gCurTask;
        t->frame = 8;
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->frame = 4;
        TaskYieldTrampoline(3);
        v = gCurTask;
        v->frame = 10;
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->frame = 5;
        TaskYieldTrampoline(3);
        t = gCurTask;
        t->frame = 9;
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->frame = 6;
        TaskYieldTrampoline(3);
        TaskStop();
        v = gCurTask;
        v->frame = 10;
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->frame = 7;
        TaskYieldTrampoline(3);
        t = gCurTask;
        t->inhalableStarLoopCount++;
    } while ((s16)t->inhalableStarLoopCount <= 3);
    t = gCurTask;
    t->updateCallback = 0;
    t->frame = 11;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame++;
    TaskYieldTrampoline(2);
    t = gCurTask;
    t->frame++;
    TaskYieldTrampoline(1);
    ActorDestroy();
}

void InhalableStarState0Update(void)
{
}

void HeldPlayerInit(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *p;
    struct PlayerState *r;

    t = gCurTask;
    t->updateCallback = (u32)HeldPlayerUpdate;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    TaskStop();
    gCurTask->facing = 1;
    u = gCurTask;
    u->spriteFlags &= ~SPRITE_FLAG_FLIP_X;
    u->layer = 7;
    p = gCurTask->player;
    p->statusFlags &= ~PLAYER_STATUS_PALETTE_LOCKED;
    r = gCurTask->player;
    if (r->mouthState == 2)
        r->mouthState = 0;
    CallTableEntry(gCurTask->state, 11, gHeldPlayerStates);
}
