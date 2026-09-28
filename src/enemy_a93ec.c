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
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
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
extern u32 sub_08068f68(void);
extern u32 ActorCollideTerrain(void);
extern u32 sub_0806951c(void);
extern u32 sub_08069888(void);
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

s32 sub_080a93ec(void)
{
    if ((u8)TaskGetYDirBitTo(gUnk_02007D00[5]) == 2)
    {
        if (abs(TaskGetDxTo(gUnk_02007D00[5])) <= 15)
            return 1;
    }
    return 0;
}

void sub_080a9434(struct Task *pt)
{
    struct Task *t;

    TaskStop();
    t = gCurTask;
    t->unk2C = 0;
    t->unk30 = 32;
    gUnk_02007D00[0] = 131;
    t->unk18 = pt->pixelX;
    t->unk20 = pt->pixelY;
    gUnk_02007D00[4] = ((u16)TaskGetAngleTo(gUnk_02007D00[5], 3) + 256) & 496;
    if (pt->onGround != 0)
    {
        gUnk_02007D00[3] = 1;
        gCurTask->unk20 |= 15;
        gUnk_02007D00[4] = (gUnk_02007D00[4] + gUnk_087490A8[RandomRange(4)]) & 496;
    }
    else
        gUnk_02007D00[3] = 0;
    gCurTask->unk28 = 4;
}

void sub_080a94d4(void)
{
    if (TaskGetDxTo(gUnk_02007D00[5]) < 0)
        gUnk_03001F2C = 0;
    else
        gUnk_03001F2C = 1;
    if (TaskGetDyTo(gUnk_02007D00[5]) < 0)
        gUnk_03002448 = 0;
    else
        gUnk_03002448 = 1;
    gCurTask->unk18 = gUnk_03002448;
    if (RandomRange(4) != 0)
        gCurTask->unk18 ^= 1;
    if (gCurTask->unk18 == 0)
    {
        if (gUnk_03001F2C == 0)
            gUnk_02007D00[4] = 128 << 1;
        else
            gUnk_02007D00[4] = 0;
        gCurTask->unk20 = gUnk_03001F2C;
    }
    else
    {
        if (gUnk_03002448 == 0)
            gUnk_02007D00[4] = 192 << 1;
        else
            gUnk_02007D00[4] = 128;
        gCurTask->unk20 = gUnk_03002448;
    }
    AngleToVector((s16)gUnk_02007D00[4], 320);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
}

s32 sub_080a95dc(void)
{
    struct Task *t;

    if (gCurTask->unk18 == 0)
    {
        if (TaskGetDxTo(gUnk_02007D00[5]) < 0)
            gUnk_03001F2C = 0;
        else
            gUnk_03001F2C = 1;
        t = gCurTask;
        if (t->unk20 != gUnk_03001F2C)
        {
            t->unk18 = 1;
            if (TaskGetDyTo(gUnk_02007D00[5]) < 0)
                gCurTask->unk20 = 0;
            else
                gCurTask->unk20 = 1;
            return 1;
        }
    }
    else
    {
        if (TaskGetDyTo(gUnk_02007D00[5]) < 0)
            gUnk_03001F2C = 0;
        else
            gUnk_03001F2C = 1;
        t = gCurTask;
        if (t->unk20 != gUnk_03001F2C)
        {
            t->unk18 = 0;
            if (TaskGetDxTo(gUnk_02007D00[5]) < 0)
                gCurTask->unk20 = 0;
            else
                gCurTask->unk20 = 1;
            return 1;
        }
    }
    return 0;
}

void sub_080a96a4(void)
{
    struct Task *t = gCurTask;

    if (t->unk34 & 0x8000) {
        if (gUnk_02007D00[6] == 0) {
            gUnk_02007D00[0] = 2;
            t->unk24 = 4;
            t->unk28 = 3;
        } else {
            t->unk34 = 0;
        }
    }
}

void sub_080a96dc(void)
{
    s32 *q;
    s32 *p;
    s32 *p2;
    s32 a;
    s32 b;
    s32 g;
    s32 w;
    s32 m;
    u32 v;

    q = &gUnk_03001F2C;
    v = (u16)TaskGetAngleTo(gUnk_02007D00[5], 3);
    *q = v;
    p = &gUnk_03002448;
    w = v - 16;
    g = gUnk_02007D00[4];
    w = g - w;
    m = 511;
    a = w & m;
    *p = a;
    p2 = &gUnk_03002344;
    v += 16;
    b = (g - v) & m;
    *p2 = b;
    if (a > b)
        gUnk_02007D00[4] = (g - 16) & m;
    else
        gUnk_02007D00[4] = (g + 16) & m;
}

void sub_080a9738(void)
{
    s32 v;

    v = ((u16)TaskGetAngleToNearestPlayer(3) + 32) & 0x1FF;
    gCurTask->frame = (v >> 6) + 4;
}

s32 sub_080a9760(void)
{
    sub_0806619c(13, (u32)sub_080a9794, (u32)gUnk_082FD438, 48, 0);
    gUnk_02007D00[1] = gUnk_02007D00[0];
    gUnk_02007D00[0] = 2;
    return 0;
}

void sub_080a9794(void)
{
    if (gFrameCount & 1)
        gCurTask->frame = (gCurTask->frame + 1) & 7;
    if (gUnk_02006190[3] == 0) {
        sub_0806621c();
        gUnk_02007D00[0] = gUnk_02007D00[1];
    }
}

s32 sub_080a97d8(void)
{
    ResetFadeAndBlend();
    TaskStop();
    ActorSetHitReactions((u32)gUnk_08749B48);
    gCurTask->unk34 = 1;
    ResetBgPaletteBlend();
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

void sub_080a9814(void)
{
    gCurTask->unk34 = 2;
    CreateStarRodPiece(0, gCurTask->pixelX, gCurTask->pixelY);
}

void Task_KrackoJrOrbs(void)
{
    s32 w;
    s32 n;

    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 12;
    gCurTask->frameTable = gUnk_08754418;
    gCurTask->updateCallback = (u32)KrackoJrOrbsUpdate;
    TaskFaceNearestPlayer();
    gCurTask->unk28 = gUnk_02007D00[0] & 15;
    gCurTask->unk2C = 0;
    gCurTask->unk30 = gUnk_087491A8[gCurTask->unk28];
    gCurTask->frame = gUnk_087491A0[0];
    while (gUnk_02007D00[0] != 4) {
        if (gCurTask->unk28 != (gUnk_02007D00[0] & 15)) {
            gCurTask->unk28 = gUnk_02007D00[0] & 15;
            gCurTask->unk30 = gUnk_087491A8[gCurTask->unk28];
        }
        w = gCurTask->unk30 - 1;
        gCurTask->unk30 = w;
        if (w == 0) {
            n = gCurTask->unk2C + 1;
            gCurTask->unk2C = n;
            if (n > 3)
                gCurTask->unk2C = w;
            gCurTask->frame = gUnk_087491A0[gCurTask->unk2C];
            gCurTask->unk30 = gUnk_087491A8[gCurTask->unk28];
        }
        TaskYieldTrampoline(1);
    }
    gCurTask->frame = 0xFFFF;
    TaskSleepForever();
}

void KrackoJrOrbsUpdate(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1) {
        struct Task *o = &gTasks[i];

        t->pixelX = o->pixelX;
        t->pixelY = o->pixelY;
    }
    if (gUnk_02007D00[0] & 128) {
        TaskFaceNearestPlayer();
        TaskUpdateFlip();
        gUnk_02007D00[0] &= ~128;
    }
    if (gUnk_02007D00[0] == 0)
        TaskSetEntry(Task_KrackoCloud, gCurTaskIdx);
}

void Task_KrackoCloud(void)
{
    s16 *p;
    s32 d;
    s32 d2;
    s32 w;
    s32 n;

    gCurTask->moveCallback = 0;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 12;
    gCurTask->frameTable = gUnk_08754418;
    gCurTask->updateCallback = (u32)sub_080a9ba0;
    gCurTask->facing = 1;
    gCurTask->frame = 10;
    TaskYieldTrampoline(6);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    gCurTask->frame = 8;
    TaskYieldTrampoline(6);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame = 8;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame = 8;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame--;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->unk28 = (d = gUnk_02007D00[0]);
    gCurTask->unk2C = 0;
    gCurTask->frame = gUnk_087491BC[0];
    p = gUnk_087491D4[d];
    gCurTask->unk30 = p[0];
    for (;;) {
        if (gCurTask->unk28 != gUnk_02007D00[0]) {
            d2 = gUnk_02007D00[0];
            gCurTask->unk28 = d2;
            p = gUnk_087491D4[d2];
            gCurTask->unk30 = p[gCurTask->unk2C];
        }
        w = gCurTask->unk30 - 1;
        gCurTask->unk30 = w;
        if (w == 0) {
            n = gCurTask->unk2C + 1;
            gCurTask->unk2C = n;
            if (n > 3)
                gCurTask->unk2C = w;
            gCurTask->frame = gUnk_087491BC[gCurTask->unk2C];
            gCurTask->unk30 = p[gCurTask->unk2C];
        }
        TaskYieldTrampoline(1);
    }
}

void sub_080a9ba0(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gTaskSlotTypes;
    t = gCurTask;
    i = t->parent;
    if ((s16)arr[i] != -1) {
        struct Task *o = &gTasks[i];

        if (o->unk76 == 6 && o->unk34 != 2) {
            t->pixelX = o->pixelX;
            t->pixelY = o->pixelY;
        } else {
            TaskFree(gCurTaskIdx);
        }
    } else {
        TaskFree(gCurTaskIdx);
    }
}

void Task_KrackoLightningTop(void)
{

    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 12;
    gCurTask->frameTable = gKrackoLightningFrames;
    gCurTask->updateCallback = (u32)KrackoLightningUpdate;
    gCurTask->facing = 1;
    gCurTask->frame = RandomRange(12);
    gCurTask->pixelX += RandomRange(16) + (u16)(0xFFF8 + gUnk_087491E4[gCurTask->frame]);
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->unk46 = CreateChildTaskAtOffsetFacing(198, gUnk_087491FC[gCurTask->frame], 16, 1);
    gTasks[gCurTask->unk46].parent = gCurTask->parent;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void Task_KrackoLightningMiddle(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 12;
    gCurTask->frameTable = gKrackoLightningFrames;
    gCurTask->updateCallback = (u32)KrackoLightningUpdate;
    gCurTask->facing = 1;
    gCurTask->frame = RandomRange(12);
    gCurTask->pixelX += gUnk_087491E4[gCurTask->frame];
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->unk46 = CreateChildTaskAtOffsetFacing(199, gUnk_087491FC[gCurTask->frame], 12, 1);
    gTasks[gCurTask->unk46].parent = gCurTask->parent;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void Task_KrackoLightningBottom(void)
{
    s32 r;

    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 10;
    gCurTask->frameTable = gKrackoLightningFrames;
    gCurTask->updateCallback = (u32)KrackoLightningUpdate;
    gCurTask->facing = 1;
    r = RandomRange(3);
    gCurTask->posX = (gCurTask->pixelX + gUnk_08749214[r]) << 16;
    gCurTask->frame = 12;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void KrackoLightningUpdate(void)
{
    vs16 *arr;
    s16 i;

    arr = gTaskSlotTypes;
    i = gCurTask->parent;
    if ((s16)arr[i] != -1) {
        struct Task *o = &gTasks[i];

        if (o->unk76 == 6 && o->unk34 != 1)
            ActorCheckHitsWithBox((s32)gUnk_08749774);
        else
            TaskFree(gCurTaskIdx);
    } else {
        TaskFree(gCurTaskIdx);
    }
}

s32 sub_080a9e88(s32 a)
{
    if (a == gCurTask->unk46)
        return 0;
    return 1;
}

void sub_080a9ea4(void)
{
    vs16 *arr;
    s16 i;

    arr = gTaskSlotTypes;
    i = gCurTask->unk46;
    if ((s16)arr[i] != -1)
        ActorDestroySlot(i);
}

void sub_080a9ed8(void)
{
    if ((u8)sub_08066a6c() != 0 && TaskIsOnScreen() != 0)
        sub_080a9ef4();
}

void sub_080a9ef4(void)
{
    struct Task *t;
    struct TaskGfx *g;
    u32 *gt;
    u16 *nx;
    u16 *src;
    u32 dst;
    u32 x;
    s32 pal;

    t = gCurTask;
    gt = t->frameTable;
    if (gt == 0)
        return;
    if (t->frame == -1)
        return;
    x = t->tileWord;
    pal = x >> 12;
    dst = ((x & 0x7FF) << 5) + 0x0600FE00;
    g = (struct TaskGfx *)gt[t->frame];
    src = g->tiles;
    while (src[0] != 0xFFFF) {
        nx = src + 1;
        RequestCopy(4, (u32)nx, dst, src[0]);
        src = (u16 *)((u32)nx + src[0]);
        dst += 128 << 3;
    }
    gUnk_03001F2C = gFrameCount & 63;
    if (gUnk_03001F2C > 31)
        gUnk_03001F2C = 64 - gUnk_03001F2C;
    gUnk_03001F2C <<= 3;
    switch (g->palette[0]) {
    case 32:
        BlendColors(gUnk_08334480, gUnk_08334480 + 16, (u16)gUnk_03001F2C, 16, (u16 *)((pal << 5) + (u32)gObjPalette));
        break;
    case 96:
        BlendColors(gUnk_08334480, gUnk_08334480 + 16, (u16)gUnk_03001F2C, 16, (u16 *)(((pal + 2) << 5) + (u32)gObjPalette));
        RequestCopy(2, (((gFrameCount >> 1) & 1) << 5) + (u32)gUnk_083344C0, ((pal + 1) << 5) + (u32)gObjPalette, 32);
        RequestCopy(2, (u32)g->palette + 2, (pal << 5) + (u32)gObjPalette, 32);
        break;
    }
    QueueSprite(gCurTask->layer, g->oamTemplate, gCurTask->spriteFlags,
                 (128 << 4) | gCurTask->tileWord,
                 gCurTask->pixelX - gViewRect[0],
                 (s16)(gCurTask->pixelY - gViewRect[2]));
    gUnk_03001F2C = (s8)gUnk_087493A4[gCurTask->frame];
    if (gUnk_03001F2C == -1)
        return;
    dst = 0x06014800;
    g = (struct TaskGfx *)gUnk_087546D0[gUnk_03001F2C];
    src = g->tiles;
    while (src[0] != 0xFFFF) {
        nx = src + 1;
        RequestCopy(4, (u32)nx, dst, src[0]);
        src = (u16 *)((u32)nx + src[0]);
        dst += 128 << 3;
    }
    pal = 12;
    switch (g->palette[0]) {
    case 64:
        pal = 11;
        RequestCopy(2, (((gFrameCount >> 1) & 1) << 5) + (u32)gUnk_083344C0, ((pal + 1) << 5) + (u32)gObjPalette, 32);
    case 32:
        RequestCopy(2, (u32)g->palette + 2, (pal << 5) + (u32)gObjPalette, 32);
        break;
    }
    QueueSprite(gCurTask->layer - 1, g->oamTemplate, gCurTask->spriteFlags,
                 (pal << 12) | 0xA50,
                 gCurTask->pixelX - gViewRect[0],
                 (s16)(gCurTask->pixelY - gViewRect[2]));
}

void sub_080aa16c(void)
{
    if ((u8)sub_08066a6c() != 0 && TaskIsOnScreen() != 0)
        sub_080aa188();
}

void sub_080aa188(void)
{
    struct Task *t;
    struct TaskGfx *g;
    u32 *gt;
    u16 *nx;
    u16 *src;
    u32 dst;
    u32 x;

    t = gCurTask;
    gt = t->frameTable;
    if (gt == 0)
        return;
    if (t->frame == -1)
        return;
    x = t->tileWord;
    dst = ((0x7FF & x) << 5) + 0x0600FE00;
    g = (struct TaskGfx *)gt[t->frame];
    src = g->tiles;
    while (src[0] != 0xFFFF) {
        nx = src + 1;
        RequestCopy(4, (u32)nx, dst, src[0]);
        src = (u16 *)((u32)nx + src[0]);
        dst += 128 << 3;
    }
    RequestCopy(2, (((gFrameCount >> 1) & 1) << 5) + (u32)gUnk_083344C0, ((x >> 12) << 5) + (u32)gObjPalette, 32);
    QueueSprite(gCurTask->layer, g->oamTemplate, gCurTask->spriteFlags,
                 (128 << 4) | gCurTask->tileWord,
                 gCurTask->pixelX - gViewRect[0],
                 (s16)(gCurTask->pixelY - gViewRect[2]));
}

void Task_NightmareWizard(void)
{
    struct Task *o;

    PlayBgm(34);
    o = &gTasks[TaskFindNearestPlayer()];
    if (gRoomEntryMode == 2) {
        if (o->onGround == 0) {
            do
                TaskYieldTrampoline(1);
            while (o->onGround == 0);
        }
    }
    sub_08066088(0);
    sub_08066144();
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)sub_080a9ed8;
    gCurTask->layer = 11;
    gCurTask->frameTable = gNightmareWizardFrames;
    gCurTask->u8C.actor->sfxOverride = 0x23E;
    gUnk_02007D00[2] = 0;
    gUnk_02007D00[3] = 1;
    gUnk_02007D00[4] = 0;
    CreateChildTaskHere(208, 0);
    CallTableEntry(gCurTask->variant, 1, gNightmareWizardVariants);
}
