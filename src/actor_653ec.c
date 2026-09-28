/* game_code_and_rodata 0x080653EC-0x080673EC (issue #65, module M17 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080653EC 0x080673EC src/actor_653ec.c --newpb
 *
 * Actor drawing and per-frame upkeep: OAM priority/palette packing
 * (sub_08066088), graphics upload out of the Task.frameTable descriptor table
 * (ActorFlashPalette/sub_08066A94), the per-task update sweep (sub_080668C8), and
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
extern void sub_08068f68(void);
extern void ActorReactToHit(void);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void sub_0800a340(s16 a, s16 b);
extern void HudStartHpBar(s16 a, s16 b);
void sub_08066c08(u32 def, u8 b);
extern void ActorSetAttackBox(u32 v);
extern void TaskSetEntry(void *fn, u32 i);
extern void ActorLoadDef(u32 def);
extern void sub_080637cc(void);
extern void ActorSetState(u8 v);
extern void ActorCheckHits(void);
extern s32 sub_08064d9c(u32 sub, u32 type, int p2Arg, int xArg, int yArg,
                        int prioArg, int altArg);

extern u32 TaskIsOnScreen(void);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void BlendColors(u32 a, u32 b, u32 c, u32 d, u32 e);

s16 ActorComputeHealthSlot(u32 i);
void sub_08066c08(u32 def, u8 b);

void sub_080653ec(void)
{
    struct Task *p;

    p = gCurTask;
    if (p->frameTable == NULL)
        return;
    if (p->frame == -1)
        return;
    if (sub_08066a6c() != 0)
    {
        if (TaskIsOnScreen() == 0)
            return;
        sub_08065470();
    }
    else if (sub_08066a80() != 0)
    {
        HudRemoveHpBar();
        ActorDestroy();
    }
}

void sub_08065438(void)
{
    struct Task *p;

    p = gCurTask;
    if (p->frameTable == NULL)
        return;
    if (p->frame == -1)
        return;
    if (sub_08066a6c() == 0)
        return;
    if (TaskIsOnScreen() == 0)
        return;
    sub_08065470();
}

/* Push the running task's tile stream into OBJ VRAM, then draw it. */
void sub_08065470(void)
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
    dst = ((prio & 0x7FF) << 5) + 0x0600FE00;
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
void sub_0806555c(void)
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

void sub_08065640(void)
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
        sub_0806555c();
    }
    else
    {
        ActorDestroy();
    }
}

void sub_0806567c(void)
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
    sub_0806555c();
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
        t->unk1C = 2;
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
        t->unk18 = v;
        t->unk1C = 1;
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
        t->unk18 = a;
        t->unk1C = 0;
        t->unk24 = b;
        CpuSet(gUnk_030012B0, gUnk_02005E10, 224);
    }
}

/* Reference-counted spawn of the slot-`idx` helper task. */
void AcquirePaletteAnim(u32 p0, s32 idx)
{
    struct Task *t;
    s32 i;

    if (idx > 2)
        return;
    if (gUnk_02007FB8[idx] == 0)
    {
        i = CreateChildTaskHere(174, 1);
        if (i != -1)
        {
            t = &gTasks[i];
            t->unk18 = gCurTask->tileWord >> 12;
            t->variant = p0;
            t->unk74 = idx;
            gPaletteAnimTasks[idx] = i;
        }
    }
    gUnk_02007FB8[idx]++;
}

void Task_PaletteAnim(void)
{
    CallTableEntry(gCurTask->variant, 5, gPaletteAnimVariants);
}

/* Task body: cross-fade two palettes while the helper's refcount holds. */
/* Task body: cross-fade two palettes while the helper's refcount holds. */
/* Task body: cross-fade two palettes while the helper's refcount holds. */
void sub_080658d8(void)
{
    struct Task *w;
    struct Task *u;
    struct Task *v;
    struct Task *x;
    struct Task *y;

    gCurTask->unk28 = 12;
    gCurTask->unk2C = 2;
    gCurTask->unk30 = 3;
    gCurTask->unk34 = 0;
    while (gUnk_02007FB8[gCurTask->unk74] != 0)
    {
        u = gCurTask;
        u->unk28--;
        if (u->unk28 == 0)
        {
            u->unk28 = 12;
            u->unk2C++;
            if (u->unk2C > 3)
                u->unk2C = 0;
            v = gCurTask;
            v->unk30++;
            if (v->unk30 > 3)
                v->unk30 = 0;
            gCurTask->unk34 = 0;
        }
        w = gCurTask;
        if (w->unk30 == 0)
            w->unk34 = w->unk34 + 128;
        else
            w->unk34 = w->unk34 + 64;
        x = gCurTask;
        if (x->unk34 > 256)
            x->unk34 = 256;
        y = gCurTask;
        BlendColors((u32)(gUnk_0825088C + (y->unk2C << 5)),
                     (u32)(gUnk_0825088C + (y->unk30 << 5)), (u16)y->unk34,
                     16, (u32)(gObjPalette + (y->unk18 << 5)));
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

/* Task body: cycle the actor's 32-byte palette out of gUnk_0873DF38. */
/* Task body: cycle the actor's 32-byte palette out of gUnk_0873DF38. */
void sub_080659b4(void)
{
    struct Task *w;
    struct Task *t;
    struct Task *u;
    struct Task *x;
    struct Actor *a;
    u32 v;

    t = gCurTask;
    a = t->u8C.actor;
    t->unk18 = 10;
    t->unk1C = 0;
    while (gUnk_02007FB8[gCurTask->unk74] != 0 && a->paletteVariant <= 3)
    {
        u = gCurTask;
        if (u->unk18 <= 0)
        {
            v = gUnk_0873DF38[a->paletteVariant][u->unk1C];
            if (v == 0)
                v = a->palette;
            RequestCopy(2, v,
                         (u32)(gObjPalette + ((u->tileWord >> 12) << 5)), 32);
            w = gCurTask;
            w->unk18 = 10;
            w->unk1C = (w->unk1C + 1) & 3;
        }
        x = gCurTask;
        x->unk18--;
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

/* Task body: flash one palette entry on and off. */
void sub_08065a68(void)
{
    struct Task *w;
    struct Task *t;
    struct Task *u;
    struct Task *x;
    u32 off;

    t = gCurTask;
    t->unk18 = 2;
    t->unk1C = 0;
    off = (t->tileWord >> 12) << 5;
    off += 26;
    while (gUnk_02007FB8[gCurTask->unk74] != 0)
    {
        u = gCurTask;
        if (u->unk18 <= 0)
        {
            if (u->unk1C != 0)
                RequestCopy(2, (u32)gUnk_0873DF78,
                             (u32)(gObjPalette + off), 2);
            else
                RequestCopy(2, (u32)gUnk_082530C8,
                             (u32)(gObjPalette + off), 2);
            w = gCurTask;
            w->unk18 = 2;
            w->unk1C ^= 1;
        }
        x = gCurTask;
        x->unk18--;
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

/* Task body: cross-fade the actor's palette pair out of gUnk_0873DF7C. */
void sub_08065b14(void)
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
    t->unk28 = 10;
    t->unk2C = 1;
    t->unk30 = 2;
    t->unk34 = 0;
    while (gUnk_02007FB8[gCurTask->unk74] != 0)
    {
        u = gCurTask;
        u->unk28--;
        if (u->unk28 == 0)
        {
            u->unk28 = 10;
            u->unk2C++;
            if (u->unk2C > 2)
            {
                u->unk2C = 0;
                u->unk28 = 2;
            }
            v = gCurTask;
            v->unk30++;
            if (v->unk30 > 2)
                v->unk30 = 0;
            gCurTask->unk34 = 0;
        }
        w = gCurTask;
        if (w->unk30 == 0)
            w->unk34 = w->unk34 + 16;
        else
            w->unk34 = w->unk34 + 128;
        x = gCurTask;
        if (x->unk34 > 256)
            x->unk34 = 256;
        y = gCurTask;
        BlendColors(gUnk_0873DF7C[a->paletteVariant][y->unk2C],
                     gUnk_0873DF7C[a->paletteVariant][y->unk30], (u16)y->unk34, 16,
                     (u32)(gObjPalette + (y->unk18 << 5)));
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

/* Task body: fade the shared palette in, out, or straight to zero. */
void PaletteAnimBgBlend(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->unk20 = 0;
    while (gUnk_02007FB8[gCurTask->unk74] != 0)
    {
        t = gCurTask;
        switch (t->unk1C)
        {
        case 0:
            t->unk20 += t->unk18;
            if (t->unk20 > t->unk24)
            {
                t->unk20 = t->unk24;
                t->unk1C = 3;
            }
            u = gCurTask;
            BlendColors((u32)gUnk_02005E10, (u32)gUnk_0873DFAC,
                         (u16)u->unk20, 224, (u32)gUnk_030012B0);
            break;
        case 1:
            t->unk20 -= t->unk18;
            if (t->unk20 < 0)
            {
                t->unk20 = 0;
                t->unk1C = 3;
            }
            u = gCurTask;
            BlendColors((u32)gUnk_02005E10, (u32)gUnk_0873DFAC,
                         (u16)u->unk20, 224, (u32)gUnk_030012B0);
            break;
        case 2:
            BlendColors((u32)gUnk_02005E10, (u32)gUnk_0873DFAC, 0, 224,
                         (u32)gUnk_030012B0);
            gCurTask->unk1C = 3;
            break;
        }
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

/* Point the actor at the palette its class/sub/level combination wants. */
void sub_08065ce0(u32 i)
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
    case 0:
        tbl = gUnk_0873EF74[t->unk76];
        break;
    case 1:
        tbl = gUnk_0873F118[t->unk76];
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
void sub_08065d44(u32 slot, u32 sub, u32 level, u32 n, u8 kind, u32 src)
{
    u32 off;
    u32 k;
    u32 *q;
    u32 p;

    switch (kind)
    {
    case 0:
        sub = (u32)gUnk_0873EF74[sub];
        break;
    case 1:
        sub = (u32)gUnk_0873F118[sub];
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

void sub_08065dbc(u32 slot, u32 sub, u32 level)
{
    sub_08065d44(slot, sub, level, 0, 0, 0);
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
void sub_08065e6c(void)
{
    struct Task *t;
    struct Actor *a;
    s32 i;

    for (i = 32; i < 63; i++)
    {
        if (gTaskSlotTypes[i] != -1 && gTaskSlotTypes[i] != 4)
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

void sub_08065ed0(void)
{
    struct Task *t;
    struct Actor *a;
    s32 i;

    for (i = 32; i < 63; i++)
    {
        if (gTaskSlotTypes[i] != -1 && gTaskSlotTypes[i] != 4)
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
    case 1:
        m = gUnk_02007D60 & 15;
        if (w > 30)
            adj = m << 2;
        else
            adj = m << 1;
        break;
    case 2:
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
    if (t->actorKind == 1)
    {
        r = gUnk_0873F0C4[t->unk76];
        if (a->paletteVariant != 0)
        {
            tbl = gUnk_0873F118[t->unk76];
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
        r = gUnk_0873F138[t->unk76];
    }
    return r;
}

u16 sub_08066088(u32 mode)
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
    if (t->actorKind == 1)
        a->gfx.header = (struct GfxHeader *)gMidBossGfx[t->unk76];
    else
        a->gfx.header = (struct GfxHeader *)gBossGfx[t->unk76];
    a->gfx.tileBits = gCurTask->tileWord & 0xFFF;
    a->gfx.paletteBank = gCurTask->tileWord >> 12;
    if (p != NULL)
    {
        if (mode == 1)
        {
            sh = sub_080b5628(p[0] << 4);
            lo = a->gfx.paletteBank;
        }
        else
        {
            sh = sub_080b55d8(p[0] << 4, p[1]);
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
    gUnk_0200AEF4 = gUnk_02004C90 = 0;
}

void sub_0806619c(u32 p0, u32 p1, u32 p2, u16 p3, u8 p4)
{
    struct Task *t;

    TaskSetSkipMask(7, gCurTaskIdx);
    t = gCurTask;
    t->lateUpdateCallback = (u32)sub_080662d8;
    gUnk_02006190[0] = t->pixelX;
    gUnk_02006190[1] = t->pixelY;
    gUnk_02006190[2] = t->frame;
    gUnk_02004C90 = p2;
    gUnk_02006190[4] = p3;
    gUnk_02006190[3] = p0;
    gUnk_0200AEF4 = p1;
    gUnk_02006190[5] = p4;
}

void sub_0806621c(void)
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
    if (gUnk_02004C90 != 0)
    {
        if (gUnk_02006190[5] != 0)
            sub_08066468();
        else
            ActorLoadHeaderPalette(a->gfx.header);
    }
}

/* Walk one step of the queued knock-back path. */
void sub_0806627c(void)
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

void sub_080662d8(void)
{
    struct Actor *a;
    u32 v;

    a = gCurTask->u8C.actor;
    sub_08068f68();
    ActorReactToHit();
    if (gUnk_02004C90 != 0)
    {
        v = gUnk_02006190[5];
        switch (v)
        {
        case 1:
            ActorFlashPalette((void *)gUnk_02004C90, gUnk_02006190[4]);
            break;
        case 0:
            sub_08066480(a->gfx.header, gUnk_02004C90, gUnk_02006190[4]);
            break;
        }
    }
    sub_0806627c();
    if (gUnk_0200AEF4 != 0)
        ((void (*)(void))gUnk_0200AEF4)();
}

/* True when every active player is in state 1. */
u8 sub_08066338(void)
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

s32 sub_08066394(void)
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

void sub_08066468(void)
{
    gCurTask->u8C.actor->paletteOverridden &= 254;
}

void sub_08066480(struct GfxHeader *h, u32 src, u32 size)
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
void sub_080664e0(struct AnimCmd *p)
{
    struct Task *t;

    sub_08066544();
    TaskSetSkipMask(8, gCurTaskIdx);
    t = gCurTask;
    t->unk20 = t->updateCallback;
    t->updateCallback = (u32)sub_08066754;
    ActorStopAnim();
    if (p != NULL)
        gCurTask->unk24 = ActorStartAnim(p);
    if (gUnk_0200AFF8 == 0)
    {
        do
        {
            TaskYieldTrampoline(1);
        } while (gUnk_0200AFF8 == 0);
    }
    sub_08066564();
    sub_080666a4();
}

void sub_08066544(void)
{
    struct Actor *a;

    a = gCurTask->u8C.actor;
    ActorSetAttackBox(a->unk60->altAttackBox);
    ActorShowHpBar();
}

void sub_08066564(void)
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
    gUnk_0200AFF8 = 0;
    HudShowHpBar();
    t = *g;
    if (t->actorKind == 1 || (t->actorKind == 2 && t->unk76 == 7))
    {
        v = t->health;
        sub_0800a340(v, v);
    }
    else
    {
        v = gCurTask->health;
        HudStartHpBar(v, v);
    }
}

u16 sub_080665fc(void)
{
    return sub_0806660c(0);
}

u16 sub_0806660c(u16 a)
{
    struct Actor *p;

    p = gCurTask->u8C.actor;
    return ((a + p->gfx.paletteBank) << 12) | p->gfx.tileBits;
}

u16 sub_08066630(u16 a)
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

void sub_08066658(struct AnimCmd *p)
{
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    t->unk20 = t->updateCallback;
    t->updateCallback = (u32)sub_08066754;
    ActorStopAnim();
    if (p != NULL)
        gCurTask->unk24 = ActorStartAnim(p);
    TaskSetSkipMask(8, gCurTaskIdx);
    ActorSetAttackBox(a->unk60->altAttackBox);
}

void sub_080666a4(void)
{
    struct Task *t;

    TaskSetSkipMask(0, gCurTaskIdx);
    t = gCurTask;
    t->updateCallback = t->unk20;
    t->unk24 = 0;
    t->unk20 = 0;
}

void sub_080666cc(struct AnimCmd *p)
{
    sub_08066658(p);
    if (gUnk_0200D080 == 0)
    {
        do
        {
            TaskYieldTrampoline(1);
        } while (gUnk_0200D080 == 0);
    }
    sub_080666a4();
}

void sub_080666f8(struct AnimCmd *p)
{
    sub_08066658(p);
    while (sub_08066718() == 0)
        TaskYieldTrampoline(1);
    sub_080666a4();
}

u32 sub_08066718(void)
{
    switch (gCurTask->unk76)
    {
    case 5:
        return sub_08026a0c();
    case 6:
        return sub_08026a80();
    case 0:
        return sub_08026aec();
    }
    return 0;
}

void sub_08066754(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->actorKind == 2 && t->unk76 == 5)
        gCurTask->unk24 = ActorTickAnim(t->unk24);
    else
        gCurTask->unk24 = ActorTickAnimFacingNearestPlayer(gCurTask->unk24);
    sub_08068f68();
    ActorReactToHit();
}

void sub_08066798(void)
{
    if (gGameState == 19)
        sub_08064e90(2, 70, 0, 128, 0);
}

void sub_080667c0(u8 a, u16 b)
{
    struct Task *t;
    struct Actor *p;

    p = gCurTask->u8C.actor;
    HudRemoveHpBar();
    t = gCurTask;
    if (t->drawCallback == (u32)sub_08065438 || t->drawCallback == (u32)sub_080653ec)
        t->drawCallback = (u32)sub_080653ec;
    else
        t->drawCallback = (u32)sub_08065350;
    gCurTask->health += gUnk_0873E1B4[gActivePlayerCount - 1];
    p->hitState = 2;
    p->score = 0;
    sub_08066a94(a);
    ActorFaceHitter();
    TaskSetFrame((s16)b);
    sub_0806ae94();
}

void sub_0806684c(void)
{
    sub_080262dc();
    sub_08066798();
}

void ActorLoadPalette(void *src, u32 size, u8 force)
{
    struct Task *t;
    struct Actor *a;
    u32 slot;

    t = gCurTask;
    a = t->u8C.actor;
    slot = t->tileWord >> 12;
    if (force == 0 && t->actorKind == 1 && a->paletteVariant != 0 && a->palette != 0)
        RequestCopy(2, a->palette, (u32)(gObjPalette + (slot << 5)), size);
    else
        RequestCopy(2, (u32)src, (u32)(gObjPalette + (slot << 5)), size);
    a->paletteColorCount = size >> 1;
}

/* Retire every other live task the running one is allowed to clean up. */
void sub_080668c8(void)
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
        if (gTaskSlotTypes[i] == 4)
            continue;
        t = &gTasks[i];
        if (gCurTask->actorKind == 2 && a->unk3C != 0)
        {
            if ((u8)((u8 (*)(s32))a->unk3C)(i) != 1)
                continue;
        }
        p = &cls;
        cls = t->actorKind;
        if (*p == 6 && t->unk76 == 0)
            continue;
        if (*p == 10)
            continue;
        v = *p;
        if (v == 9 || v == 7 || v == 8)
        {
            if (gTaskSlotTypes[i] == 174)
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
    if (a->unk04 != 0)
    {
        p = t->player;
        sub_0806be4c(i);
        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        if (a->unk04 != 1)
        {
            if ((u8)(p->unk16 + 2) <= 1)
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

u8 sub_08066a6c(void)
{
    return ActorIsInViewMargin(80, 40);
}

u8 sub_08066a80(void)
{
    return ActorIsInViewMargin(360, 240);
}

void sub_08066a94(u8 mode)
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

void sub_08066ae0(void)
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
    sub_080637cc();
}

void sub_08066b34(u32 def)
{
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    if (a->extraFrame != -1)
    {
        t->drawCallback = (u32)sub_08066c74;
        ActorLoadDef(def);
        gCurTask->u8C.actor->unk16 = 6;
    }
}

void sub_08066b70(void)
{
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    if (a->extraFrame != -1)
    {
        if (t->velX >= 0)
            t->unk1C = 3;
        else
            t->unk1C = 9;
    }
    else
    {
        TaskTurnAroundAndReverseX();
    }
}

void sub_08066ba8(void)
{
    struct Task *t;

    TaskFaceNearestPlayer();
    TaskStop();
    t = gCurTask;
    t->velY = 0x8000;
    if (t->facing == 1)
        t->unk1C = 0;
    else
        t->unk1C = 6;
}

void sub_08066bdc(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk1C = t->unk1C + 1;
    if (t->unk1C > 11)
        t->unk1C = 0;
    u = gCurTask;
    u->velX = gUnk_0873E1B8[u->unk1C];
}

void sub_08066c08(u32 def, u8 b)
{
    gCurTask->u8C.actor->extraFrame = 0xFFFF;
    ActorLoadDef(def);
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    sub_08066e88(b);
}

void sub_08066c3c(u32 def)
{
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    if (a->extraFrame != -1 && t->unk74 != 2)
        sub_08066c08(def, 1);
}

/* Draw the running task plus its trailing "sparkle" sprite. */
/* Draw the running task plus its trailing "sparkle" sprite. */
void sub_08066c74(void)
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
        if ((u->skipMask & 7) == 0)
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
        sub_08066dcc();
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
void sub_08066dcc(void)
{
    struct Task *t;
    s32 i;
    s32 j;

    t = gCurTask;
    i = t->frame;
    j = i * 2;
    switch (t->unk76)
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

void sub_08066e88(u8 a)
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
    sub_08066dcc();
    t = gCurTask;
    x = t->pixelX + gUnk_030023B4;
    y = t->pixelY + gUnk_030023D4;
    i = CreateActorByKind(0, 38, kind, 0, x, y, 0);
    if (i != -1)
    {
        u = &gTasks[i];
        s = gCurTask;
        v = (s->hitterPlayer == -1) ? TaskFindNearestPlayer() : s->hitterPlayer;
        u->unk1C = v;
        u->unk28 = a;
        if (gUnk_02006178 == 1)
        {
            gCurTask->u8C.actor->unk0D = 1;
            u->variant = 3;
            u->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
            u->layer = 11;
            u->frameTable = gParasolFrames;
            u->frame = 0;
        }
    }
}

void sub_08066f50(s32 x, s32 y)
{
    sub_080713f8(x - gViewRect[0], y - gViewRect[2], 23);
    PlayBgm(1);
}

void LoadStarRodPieceGfx(void)
{
    struct GfxHeader *h;

    h = (struct GfxHeader *)gUnk_08334DC0;
    RequestCopy(4, (u32)h->tiles, 0x06017800, h->tileCount << 5);
    RequestCopy(2, gStarRodPiecePalettes[gLevelIndex], (u32)gUnk_03001610,
                 h->paletteBankCount << 5);
}

void CreateStarRodPiece(u8 p3, s16 x, s16 y)
{
    struct Task *t;
    s32 i;

    i = sub_08064d9c(5, 73, 0, x, y, 0xD3D0, 1);
    if (i != -1)
    {
        t = &gTasks[i];
        if (gGameState == 8 && gUnk_03001F30 == 0)
        {
            t->variant = p3;
            PlayBgm(1);
        }
        else
        {
            t->variant = 2;
            t->unk74 = gCurTask->unk76;
        }
    }
}

void sub_0806704c(void)
{
    LoadStarRodPieceGfx();
    CreateStarRodPiece(0, 128, 104);
}

u8 sub_08067060(void)
{
    if (gUnk_0200B030 == 0)
        return 0;
    return 1;
}

u8 sub_08067074(void)
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

void sub_080670ac(u16 a)
{
    TaskFreezeOrThawOthers(a, gCurTaskIdx);
    TaskSetSkipMask(0, 63);
    PauseRoom();
    sub_08067108();
}

void sub_080670d4(void)
{
    TaskFreezeOrThawOthers(0, gCurTaskIdx);
    ResumeRoom();
    sub_08067114();
}

void LoadBackdropColor(u32 src)
{
    RequestCopy(2, src, (u32)gBgPalette, 2);
}

void sub_08067108(void)
{
    gUnk_03001F34 = 1;
}

void sub_08067114(void)
{
    gUnk_03001F34 = 0;
}

s32 CreateInhalableStar(s16 x, s16 y, u16 dir, u8 p8)
{
    struct ActorSpawn sp;
    struct Task *t;
    s32 i;

    sp.subtype = 36;
    sp.taskType = 139;
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

void sub_08067170(void)
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
        ActorSetAttackBox((u32)gUnk_0873F7E4);
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
    sub_08067170();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_0873E280);
}

void InhalableStarUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_0873E284);
    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

/* Task body: the player's "spin out and vanish" death animation. */
void sub_08067258(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    gCurTask->updateState = 0;
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
    u->unk6C = 0;
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
        t->unk6C++;
    } while ((s16)t->unk6C <= 3);
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

void sub_08067378(void)
{
}

void sub_0806737c(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *p;
    struct PlayerState *r;

    t = gCurTask;
    t->updateCallback = (u32)sub_08067408;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    TaskStop();
    gCurTask->facing = 1;
    u = gCurTask;
    u->spriteFlags &= 0x7FFF;
    u->layer = 7;
    p = gCurTask->player;
    p->unk42 &= 0xFFEF;
    r = gCurTask->player;
    if (r->mouthState == 2)
        r->mouthState = 0;
    CallTableEntry(gCurTask->state, 11, gUnk_0873E2F0);
}
