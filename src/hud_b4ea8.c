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
#include "save.h"

/* RAM cells / ROM tables */
struct Unk020055D8Entry
{
    /*0x00*/ s8 kind;
    /*0x01*/ s8 unk1;
    /*0x02*/ s8 unk2;
    /*0x03*/ s8 unk3;
    /*0x04*/ u16 x;
    /*0x06*/ u16 y;
};

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
extern void sub_080639f0(struct ActorAux *v);
extern void sub_08063a00(u32 v);
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

/* Module functions */
void sub_080a2b2c();
void ReleaseRoomObject();
s32 LoadRoomEnemyGfx();
s32 LoadRoomMidBossGfx();
s32 LoadRoomBossGfx();
void LoadRoomMetaKnightsGfx();
s32 sub_080b5a94();
s32 SpawnRoomEnemy();
s32 sub_080b5d84();

void LoadRoomObjectGfx(void)
{
    s32 n;
    s32 i;
    s32 j;
    struct Unk020055D8Entry *e;
    struct Unk020055D8Entry *f;

    n = 0;
    if (gRoomObjectList.count == 0)
        return;
    gObjTileCursor = n;
    gObjPaletteCursor = 8;
    e = gRoomObjectList.entries;
    for (i = 0; i < gRoomObjectList.count; e++, i++)
    {
        switch (e->kind)
        {
        case 0:
        case 4:
        case 7:
            break;
        case 1:
            if (LoadRoomEnemyGfx(e, i, n) != 0)
                n++;
            break;
        case 5:
            if (sub_080b5a94(e, i, n) != 0)
                n++;
            break;
        case 2:
            if (LoadRoomMidBossGfx(e, i, n) != 0)
                n++;
            break;
        case 6:
            if (e->unk1 == 0)
            {
                f = &gRoomObjectList.entries[e->unk2];
                for (j = e->unk2; j < e->unk2 + e->unk3; f++, j++)
                {
                    if (f->kind == 7 && LoadRoomMidBossGfx(f, j, n) != 0)
                        n++;
                }
            }
            break;
        case 3:
            if (LoadRoomBossGfx(e, i, n) != 0)
                n++;
            break;
        case 8:
            LoadRoomMetaKnightsGfx(e, i, n);
            n++;
            break;
        }
    }
    if (gUnk_0200B078 == 4)
        sub_080b5558();
}
