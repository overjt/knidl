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
extern void sub_08030db8();
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
extern s32 sub_08067120(s16 x, s16 y, s16 dir, u8 p8);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u32 ActorCheckHits(void);
extern u32 sub_08068f68(void);
extern u32 ActorCollideTerrain(void);
extern u32 sub_0806951c(void);
extern u32 sub_08069888(void);
extern u32 ActorReactToHit(void);

/* Module functions */
void sub_080a2b2c();
void sub_080b54a4();
s32 sub_080b5670();
s32 sub_080b5840();
s32 sub_080b590c();
void sub_080b59d8();
s32 sub_080b5a94();
s32 sub_080b5bdc();
s32 sub_080b5d84();
void sub_080a860c(void)
{
    s32 n;

    switch (gCurTask->unk28)
    {
    case 0:
        if (gCurTask->pixelY < gViewRect[2] - 62)
        {
            TaskStopY();
            gUnk_02007D00[0] = 4;
            gCurTask->unk28++;
        }
        sub_08066718();
        break;
    case 1:
        gCurTask->posY = (gViewRect[2] - 62) << 16;
        if (sub_08066718() == 1)
            gCurTask->unk28++;
        break;
    case 2:
        if (gViewRect[3] <= 229)
        {
            n = 0;
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gPlayerCount; gCurTask->unk6C++)
            {
                if ((gPlayerLives[(s16)gCurTask->unk6C] != 0 || gPlayerHealth[(s16)gCurTask->unk6C] != 0) && ((gActivePlayerMask >> (s16)gCurTask->unk6C) & 1))
                {
                    TaskGetPosSlot((s16)gCurTask->unk6C);
                    if (gUnk_030023D4 > 151)
                        continue;
                }
                n++;
            }
            if (n == gPlayerCount)
            {
                TaskGetNearestPlayerScreenPos();
                if (gUnk_030023B4 == 128)
                    gCurTask->posX = (gUnk_087490E4[RandomRange(2)] + gViewRect[0]) << 16;
                if (gUnk_030023B4 <= 127)
                    gCurTask->posX = (gViewRect[0] + 168) << 16;
                else
                    gCurTask->posX = (gViewRect[0] + 72) << 16;
                gCurTask->unk28++;
            }
        }
        gCurTask->posY = (gViewRect[2] - 62) << 16;
        break;
    case 3:
        TaskSetEntry(sub_080a87c8, gCurTaskIdx);
        break;
    }
}
