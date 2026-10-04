#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "hud.h"
#include "collision.h"
#include "room.h"
#include "camera.h"
#include "player.h"
#include "effect.h"
#include "actor.h"
#include "enemy.h"

/* RAM cells / ROM tables */
struct RoomObjectEntry
{
    /*0x00*/ s8 kind;
    /*0x01*/ s8 unk1;
    /*0x02*/ s8 unk2;
    /*0x03*/ s8 unk3;
    /*0x04*/ u16 x;
    /*0x06*/ u16 y;
};
/* Not from save.h: this file's view of gHBlankScrollTimer differs (lesson
   3.517). */
extern u32 gHBlankScrollTimer[];
extern u16 gHBlankScrollTable[];
extern u32 gHBlankScrollEffect[];
extern u16 gHBlankScrollDmaTable[];
extern u32 gHBlankDmaDest[];
extern u32 gHBlankDmaCnt[];
extern u32 gHBlankDmaSrc[];

/* External functions */
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
extern s32 PlaySfx(s32 id);
extern u32 TaskIsOnScreen(void);
extern void TaskSetEntry(void *a, u32 i);
extern void HudStartHpBar();
extern s32 GetCollisionTileAtOffset(s16 x, s16 y, s32 c, s32 d);
extern void RequestScreenShake(u32 a);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void TaskBreakBlocksNoPlayer();
extern void TaskBreakTopBlockRow();
extern void ActorLoadDef(struct ActorDef *d);
extern void ActorSetState();
extern void ActorSetStateSlot(u32 i, u16 v);
extern void ActorSetHitReactions(u32 v);
extern void ActorSetAttackBox(u32 v);
extern void ActorSetAux(struct ActorAux *v);
extern void ActorSetExtraAttackBox(u32 v);
extern s32 TaskGetDxTo(u32 i);
extern s32 TaskIsInRectSlot(struct Rect *r, u32 i);
extern s32 ActorStartAnimNoFlip(struct AnimCmd *p);
extern void AngleToVector(s16 t, s16 mag);
extern u16 TaskGetAngleToNearestPlayer(s32 prec);
extern s16 ActorComputeHealth(void);
extern s32 CreateInhalableStar(s16 x, s16 y, s16 dir, u8 p8);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u32 ActorCheckHits(void);
extern u32 ActorCheckHitsWithExtraBox(void);
extern u32 ActorCollideTerrain(void);
extern u32 ActorCollideTerrainAlongVelocity(void);
extern u32 ActorCollideTerrainFloor(void);
extern u32 ActorReactToHit(void);
extern void SaveBossEnduranceBestTime();

/* Module functions */
void MrBrightFlashPalette();
void ReleaseRoomObject();
s32 LoadRoomEnemyGfx();
s32 LoadRoomMidBossGfx();
s32 LoadRoomBossGfx();
void LoadRoomMetaKnightsGfx();
s32 LoadRoomStageObjectGfx();
s32 SpawnRoomEnemy();
s32 SpawnRoomMapEvent();

s32 LoadRoomMidBossGfx(u8 *e, s32 i, s32 k)
{
    u8 *d5;
    u8 *p6;
    u8 *pX;
    s32 o1;
    s32 o7;
    u8 *bA;
    s32 w2;
    s32 w9;
    s32 r;

    d5 = (u8 *)gMidBossGfx[(s8)e[1]];
    if (d5 == 0)
        return 0;
    ((u8 *)gRoomObjectGfxSlotIds)[i] = k;
    bA = (u8 *)gRoomObjectGfxSlots;
    o1 = k << 2;
    p6 = (u8 *)(o1 + (u32)bA);
    p6[0] = e[1];
    w2 = *(u16 *)(d5 + 2);
    o7 = o1;
    if (w2 != 0)
    {
        *(u16 *)(p6 + 2) = AllocObjTiles(w2);
        if (*(u16 *)(d5 + 6) != 0)
        {
            LZ77UnCompVram((void *)*(u32 *)(d5 + 12), (void *)gUnk_02020000);
            RequestCopy(4, (u32)gUnk_02020000, (*(s16 *)(p6 + 2) << 6) + OBJ_VRAM0, *(u16 *)(d5 + 2) << 5);
        }
        else
        {
            RequestCopy(4, *(u32 *)(d5 + 12), (*(s16 *)(p6 + 2) << 6) + OBJ_VRAM0, *(u16 *)(d5 + 2) << 5);
        }
    }
    if (*(u16 *)d5 != 0)
    {
        r = AllocObjPalettes(*(u16 *)d5);
        pX = (u8 *)((u32)gRoomObjectGfxSlots + o7);
        pX[1] = r;
        w9 = pX[1];
        RequestCopy(2, *(u32 *)(d5 + 8), ((s8)w9 << 5) + (u32)gObjPalette, *(u16 *)d5 << 5);
    }
    return 1;
}

s32 LoadRoomBossGfx(u8 *e, s32 i, s32 k)
{
    u8 *d5;
    u8 *p6;
    u8 *pX;
    s32 o1;
    s32 o7;
    u8 *bA;
    s32 w2;
    s32 w9;
    s32 r;

    d5 = (u8 *)gBossGfx[(s8)e[1]];
    if (d5 == 0)
        return 0;
    ((u8 *)gRoomObjectGfxSlotIds)[i] = k;
    bA = (u8 *)gRoomObjectGfxSlots;
    o1 = k << 2;
    p6 = (u8 *)(o1 + (u32)bA);
    p6[0] = e[1];
    w2 = *(u16 *)(d5 + 2);
    o7 = o1;
    if (w2 != 0)
    {
        *(u16 *)(p6 + 2) = AllocObjTiles(w2);
        if (*(u16 *)(d5 + 6) != 0)
        {
            LZ77UnCompVram((void *)*(u32 *)(d5 + 12), (void *)gUnk_02020000);
            RequestCopy(4, (u32)gUnk_02020000, (*(s16 *)(p6 + 2) << 6) + OBJ_VRAM0, *(u16 *)(d5 + 2) << 5);
        }
        else
        {
            RequestCopy(4, *(u32 *)(d5 + 12), (*(s16 *)(p6 + 2) << 6) + OBJ_VRAM0, *(u16 *)(d5 + 2) << 5);
        }
    }
    if (*(u16 *)d5 != 0)
    {
        r = AllocObjPalettes(*(u16 *)d5);
        pX = (u8 *)((u32)gRoomObjectGfxSlots + o7);
        pX[1] = r;
        w9 = pX[1];
        RequestCopy(2, *(u32 *)(d5 + 8), ((s8)w9 << 5) + (u32)gObjPalette, *(u16 *)d5 << 5);
    }
    return 1;
}

void LoadRoomMetaKnightsGfx(u8 *e, s32 idx, s32 n)
{
    struct RoomObjectGfx *d;
    s32 i;

    gRoomObjectGfxSlotIds[idx] = n;
    gRoomObjectGfxSlots[n].unk0 = e[1];
    gRoomObjectGfxSlots[n].unk2 = 0;
    gRoomObjectGfxSlots[n].paletteBank = 8;
    d = gMetaKnightsGfx[0];
    if (d->tileCount != 0)
    {
        if (d->tilesCompressed != 0)
        {
            LZ77UnCompVram((void *)d->tiles, gUnk_02020000);
            RequestCopy(4, (u32)gUnk_02020000, OBJ_VRAM0, d->tileCount << 5);
        }
        else
        {
            RequestCopy(4, d->tiles, OBJ_VRAM0, d->tileCount << 5);
        }
    }
    if (d->paletteBankCount != 0)
        RequestCopy(2, d->palette, (u32)gObjPaletteBank8, d->paletteBankCount << 5);
    for (i = 1; i <= 4; i++)
    {
        d = gMetaKnightsGfx[i];
        if (d->tilesCompressed != 0)
            LZ77UnCompVram((void *)d->tiles, (void *)(gUnk_08756184[i] + (u32)gUnk_02020000));
    }
}

s32 LoadRoomStageObjectGfx(struct RoomObjectEntry *e, s32 idx, s32 n)
{
    struct RoomObjectGfx *d;
    s32 i;

    if (gStageObjectSubtypes[e->unk1] == -1)
        return 0;
    d = gStageObjectGfx[gStageObjectSubtypes[e->unk1]];
    if (d == NULL)
        return 0;
    for (i = 0; i < idx; i++)
    {
        if (gRoomObjectList.entries[i].kind == 5)
        {
            if (gRoomObjectGfxSlotIds[i] != -1
             && gStageObjectGfx[gRoomObjectGfxSlots[gRoomObjectGfxSlotIds[i]].unk0] == d)
            {
                gRoomObjectGfxSlotIds[idx] = gRoomObjectGfxSlotIds[i];
                return 0;
            }
        }
    }
    gRoomObjectGfxSlotIds[idx] = n;
    gRoomObjectGfxSlots[n].unk0 = e->unk1;
    if (d->tileCount != 0)
    {
        gRoomObjectGfxSlots[n].unk2 = AllocObjTiles(d->tileCount);
        if (d->tilesCompressed != 0)
        {
            LZ77UnCompVram((void *)d->tiles, gUnk_02020000);
            RequestCopy(4, (u32)gUnk_02020000, (gRoomObjectGfxSlots[n].unk2 << 6) + OBJ_VRAM0, d->tileCount << 5);
        }
        else
        {
            RequestCopy(4, d->tiles, (gRoomObjectGfxSlots[n].unk2 << 6) + OBJ_VRAM0, d->tileCount << 5);
        }
    }
    if (d->paletteBankCount != 0)
    {
        gRoomObjectGfxSlots[n].paletteBank = AllocObjPalettes(d->paletteBankCount);
        RequestCopy(2, d->palette, (gRoomObjectGfxSlots[n].paletteBank << 5) + (u32)gObjPalette, d->paletteBankCount << 5);
    }
    return 1;
}

s32 SpawnRoomEnemy(struct RoomObjectEntry *e, s32 i)
{
    s32 res = -1;

    if (gRoomObjectGfxSlotIds[i] != -1)
    {
        if (e->unk1 == 32)
        {
            if (!(gUsedRoomObjects[gLevelIndex][gStageIndex] & (1 << (e->unk3 & 31))))
                res = CreateActorByKind(ACTOR_KIND_ENEMY, e->unk1, e->unk2, 0, e->x, e->y,
                                   (gRoomObjectGfxSlots[gRoomObjectGfxSlotIds[i]].paletteBank << 12) | ((gRoomObjectGfxSlots[gRoomObjectGfxSlotIds[i]].unk2 * 2) + 16));
        }
        else if (e->unk1 == 37)
        {
            if (!(gUsedRoomObjects[gLevelIndex][gStageIndex] & (1 << (e->unk3 & 31))))
            {
                res = CreateActorByKind(ACTOR_KIND_ENEMY, e->unk1, e->unk2, 0, e->x, e->y,
                                   (gRoomObjectGfxSlots[gRoomObjectGfxSlotIds[i]].paletteBank << 12) | ((gRoomObjectGfxSlots[gRoomObjectGfxSlotIds[i]].unk2 * 2) + 16));
                gUsedRoomObjects[gLevelIndex][gStageIndex] |= 1 << (e->unk3 & 31);
            }
        }
        else
        {
            res = CreateActorByKind(ACTOR_KIND_ENEMY, e->unk1, e->unk2, e->unk3 & 31, e->x, e->y,
                               (gRoomObjectGfxSlots[gRoomObjectGfxSlotIds[i]].paletteBank << 12) | ((gRoomObjectGfxSlots[gRoomObjectGfxSlotIds[i]].unk2 * 2) + 16));
        }
    }
    else
    {
        res = CreateActorByKind(ACTOR_KIND_ENEMY, e->unk1, e->unk2, e->unk3 & 31, e->x, e->y, 0);
    }
    return res;
}

s32 SpawnRoomMapEvent(struct RoomObjectEntry *e)
{
    s32 slot;
    struct Task *t;
    struct RoomObjectEntry *f;
    s32 i;
    s32 n;
    s32 y;
    s32 r;

    slot = CreateMapEvent(0);
    if (slot == -1)
        return slot;
    t = &gTasks[slot];
    t->mapEventEndAction = 0;
    t->mapEventWaitScrollLock = 0;
    if (gUnk_02007D64 != 4)
    {
        if (gUnk_0200AF0C != gRoomIndex)
        {
            gUnk_02007D60 = 0;
            gUnk_0200AF0C = gRoomIndex;
        }
        gUnk_02007D60 &= 0xFF;
    }
    f = &gRoomObjectList.entries[e->unk2];
    for (i = e->unk2; i < e->unk2 + e->unk3; f++, i++)
    {
        if (f->kind == 6)
        {
            switch (f->unk1)
            {
            case 1:
                break;
            case 2:
                if (e->x < (gViewRect[0] + gViewRect[1]) >> 1)
                    gScrollLock.unkA = e->x + 156;
                else
                    gScrollLock.unkA = e->x - 156;
                StartScrollLock(f->x, f->x + 240, 0xFFFF, 0xFFFF);
                t->mapEventWaitScrollLock = 1;
                break;
            case 3:
                if (e->y < (gViewRect[3] + gViewRect[2]) >> 1)
                    gScrollLock.unkC = e->y + 120;
                else
                    gScrollLock.unkC = e->y - 120;
                StartScrollLock(0xFFFF, 0xFFFF, f->y, f->y + 160);
                t->mapEventWaitScrollLock = 1;
                break;
            case 4:
                t->mapEventEndAction = 1;
                t->mapEventTargetX = f->x;
                t->mapEventTargetY = f->y;
                break;
            case 5:
                t->mapEventEndAction = 2;
                t->mapEventTargetX = f->x;
                t->mapEventTargetY = f->y;
                break;
            }
        }
    }
    for (i = 0; i < 2; i++)
        gHudHpBarTasks[i] = 0xFFFF;
    f = &gRoomObjectList.entries[e->unk2];
    n = 0;
    for (i = e->unk2; i < e->unk2 + e->unk3; f++, i++)
    {
        if (f->kind == 7)
        {
            gMidBossFightState = 0;
            if (t->mapEventWaitScrollLock != 0)
            {
                if (gActivePlayerCount == 1)
                {
                    gMidBossDropsIn = 0;
                    y = f->y;
                }
                else
                {
                    gMidBossDropsIn = 1;
                    y = -16;
                }
            }
            else
            {
                gMidBossDropsIn = 0;
                y = f->y;
            }
            r = CreateActorByKind(ACTOR_KIND_MID_BOSS, f->unk1, f->unk2, f->unk3, f->x, y,
                             (gRoomObjectGfxSlots[gRoomObjectGfxSlotIds[i]].paletteBank << 12) | ((gRoomObjectGfxSlots[gRoomObjectGfxSlotIds[i]].unk2 * 2) + 16));
            if (r != -1)
            {
                gUnk_02005590[r - 32] = n;
                gHudHpBarTasks[n] = r;
            }
            n++;
        }
    }
    gHudHpBarCount = gHudHpBarsLeft = n;
    return slot;
}

void HBlankScrollVBlankCallback(void)
{
    REG_DMA0CNT_H = 0;
    if (gFrameCallback == 0)
    {
        gVBlankCallback = 0;
        *(vu32 *)gHBlankDmaDest = *(vu32 *)gHBlankDmaSrc = *(vu32 *)gHBlankDmaCnt = 0;
        gUnk_03000B74 = 0;
    }
    else
    {
        if (gUnk_03000B74 & 1)
        {
            CpuSet(gHBlankScrollTable, gHBlankScrollDmaTable, 0x040000F0);
            gUnk_03000B74 = 2;
        }
        if (gUnk_03000B74 & 2)
        {
            REG_DMA0SAD = gHBlankDmaSrc[0];
            REG_DMA0DAD = gHBlankDmaDest[0];
            REG_DMA0CNT = gHBlankDmaCnt[0];
        }
    }
}

void UpdateHBlankScroll(void)
{
    s32 w;
    s32 m;
    vu32 *p;
    s32 w1;

    w = (s16)((s32 (*)(void))gHBlankScrollEffects[*(s16 *)gHBlankScrollEffect])();
    *(vu32 *)gHBlankDmaSrc = (w << 1) + (u32)gHBlankScrollDmaTable;
    m = 0xA2600000;
    m |= w;
    *(vu32 *)gHBlankDmaCnt = m;
    *(vu32 *)&gVBlankCallback = (u32)HBlankScrollVBlankCallback;
    p = (vu32 *)gHBlankScrollTimer;
    w1 = *p;
    if (w1 <= 0xFFFF)
        *p = w1 + 1;
}
