#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* plobj_509ec.c (0x080509EC-0x080514F7, issue #90).
 *
 * Task type #6, variants 0-2, each variant body followed by the callbacks
 * only it installs.  A variant body installs the sprite (TaskMove /
 * TaskDrawWorldInViewOrFree, Task.unk42 = 5, an animation table in Task.unk38) and runs
 * `switch (Task.unk18 & 15)` over its sub-states, each a yield script that
 * ends in TaskExitTrampoline; its per-frame callback (Task.unk04) runs
 * the hit test TaskBreakBlocks and the terrain checks and re-binds the body
 * in another sub-state, or the shared exit sub_08050814, on contact.
 * Variant 0 (sub_080509ec, callback sub_08050c48) spawns a copy of itself
 * in sub-state 1 while it moves; variants 1 and 2 (sub_08050d00,
 * sub_08051124) have the collision callbacks sub_08050e84/sub_080512f8
 * (sound 125 on contact) and the Task.unk08 callbacks sub_08050f80/
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
        t->unk00 = (u32)TaskMove;
        t->unk0C = (u32)TaskDrawWorldInViewOrFree;
        t->unk42 = 5;
    }
    gCurTask->unk38 = gUnk_0874C568;
    sub_0802205c(gUnk_0873CB44);
    switch (gCurTask->unk18 & 15)
    {
    case 0:
    {
        struct Task *t = gCurTask;
        t->unk04 = (u32)sub_08050c48;
        if (t->unk43 == 1)
            t->unk4C = (t->unk48 + 8) << 16;
        else
            t->unk4C = (t->unk48 - 8) << 16;
    }
    {
        struct Task *t = gCurTask;
        t->unk50 = (t->unk4A + 2) << 16;
        PlaySfxIfLocalPlayer(114, t->unk44);
    }
        TaskSetMotionXFacing(0x3C000, -0x2000);
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++)
        {
            TaskSetFrameByFacing(0);
            TaskYieldTrampoline(2);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(1);
            CreatePlayerObject(gCurTask->unk88->unk00, 0, 1);
            gCurTask->unk3C -= 2;
            TaskYieldTrampoline(2);
            TaskSetFrameByFacing(4);
            TaskYieldTrampoline(2);
        }
        TaskStopX();
        gCurTask->unk04 = 0;
        TaskSetFrameByFacing(0);
        TaskYieldTrampoline(2);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(1);
        gCurTask->unk3C -= 2;
        TaskYieldTrampoline(2);
        TaskSetFrameByFacing(4);
        TaskYieldTrampoline(1);
        TaskSetFrameByFacing(10);
        TaskYieldTrampoline(1);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(1);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(1);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(1);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(1);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(1);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(1);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(1);
        break;
    case 1:
        TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
        TaskSetFrameByFacing(6);
        TaskYieldTrampoline(4);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(2);
        break;
    case 2:
        TaskStop();
        {
            struct Task *t = gCurTask;
            t->unk04 = 0;
            RegisterCollider(gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873BD64);
        }
        gCurTask->unk3C = 26;
        TaskYieldTrampoline(1);
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 10; gCurTask->unk6C++)
        {
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
        }
        break;
    }
    TaskExitTrampoline();
}

void sub_08050c48(void)
{
    if (TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CB84, gCurTask->unk44))
        gCurTask->unk7C = 1;
    else
        sub_0801c230(gUnk_0873CB44);
    if (gTerrainResult.unk1 != 0 || (gCurTask->unk7A & 1) || gTerrainResult.unk0 != 0
        || gCurTask->unk7C != 0)
    {
        struct Task *t = gCurTask;
        t->unk18 = (t->unk18 & ~15) | 2;
        TaskSetEntry(sub_080509ec, gCurTaskIdx);
    }
    RegisterCollider(gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A, gUnk_0873BD64);
}

void sub_08050d00(void)
{
    {
        struct Task *t = gCurTask;
        t->unk00 = (u32)TaskMove;
        t->unk0C = (u32)TaskDrawWorldInViewOrFree;
        t->unk04 = (u32)sub_08050e84;
        t->unk42 = 5;
    }
    gCurTask->unk38 = gUnk_0874C44C;
    sub_0802205c(gUnk_0873CB4C);
    {
        struct Task *t = gCurTask;
        t->unk50 = (t->unk4A + 4) << 16;
        if (t->unk43 == 1)
            t->unk4C = (t->unk48 + 8) << 16;
        else
            t->unk4C = (t->unk48 - 8) << 16;
    }
    PlaySfxIfLocalPlayer(105, gCurTask->unk44);
    {
        struct Task *t = gCurTask;
        if (!(t->unk7B & 1))
        {
            t->unk28 = 0;
            if (t->unk43 == 1)
                t->unk2C = -0x40000;
            else
                t->unk2C = 0x40000;
            gCurTask->unk30 = 0;
            TaskSetMotionXFacing(0x40000, 0x5A5A5A5A);
            while (1)
            {
                gCurTask->unk3C = 4;
                TaskYieldTrampoline(3);
                gCurTask->unk08 = (u32)sub_08050f80;
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
            }
        }
    }
    gCurTask->unk28 = 0;
    gCurTask->unk60 = 0x400;
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
    {
        gCurTask->unk3C = 4;
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
    }
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_08050e84(void)
{
    s32 hit;

    if (TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CB94, gCurTask->unk44))
        gCurTask->unk7C = 1;
    else
        sub_0801c230(gUnk_0873CB4C);
    hit = 0;
    {
        struct Task *t = gCurTask;
        if ((t->unk7A & 1) || *(u16 *)&gTerrainResult != 0 || t->unk7C != 0)
            hit++;
        else if ((t->unk7B & 1) && t->unk28 != 0)
            hit = 1;
    }
    if (hit)
    {
        struct Task *t;
        TaskSetEntry(sub_08050814, gCurTaskIdx);
        t = gCurTask;
        t->unk24 = (s32)gUnk_0873BD78;
        if (gTerrainResult.unk1 != 0 || (t->unk7A & 1) || gTerrainResult.unk0 != 0)
            PlaySfxIfLocalPlayer(125, gCurTask->unk44);
    }
    RegisterCollider(gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A, gUnk_0873BD78);
}

void sub_08050f80(void)
{
    s32 dx;
    s32 x, y;
    struct Task *t;

    t = gCurTask;
    if (t->unk43 == 1)
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
        if (t->unk43 == 1)
            t->unk2C = -0x40000;
        else
            t->unk2C = 0x40000;
        gCurTask->unk30 = 0;
    }
    else
    {
        if (t->unk43 == 1)
            t->unk30 += -0x10000;
        else
            t->unk30 += 0x10000;
        gCurTask->unk2C += gCurTask->unk30;
    }
    t = gCurTask;
    if (t->unk88->unk37 != 2)
    {
        x = t->unk48 + dx + ((s16 *)&t->unk2C)[1];
        y = t->unk4A;
        if (!IsWorldPosOnScreen(x, y))
            return;
        x -= gSpriteCameraX;
        y -= gSpriteCameraY;
    }
    else
    {
        x = t->unk48 + dx + ((s16 *)&t->unk2C)[1];
        y = t->unk4A;
    }
    {
        u32 *tbl = gCurTask->unk38;
        QueueSprite(gCurTask->unk42, tbl[gCurTask->unk34], 0, 0, x, y);
    }
}

void sub_08051124(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorldInViewOrFree;
    gCurTask->unk04 = (u32)sub_080512f8;
    gCurTask->unk08 = (u32)sub_080513d4;
    gCurTask->unk42 = 5;
    gCurTask->unk38 = gUnk_0874C478;
    sub_0802205c(gUnk_0873CB4C);
    {
        struct Task *t = gCurTask;
        if (t->unk43 == 1)
            t->unk4C = (t->unk48 + 8) << 16;
        else
            t->unk4C = (t->unk48 - 8) << 16;
    }
    {
        struct Task *t = gCurTask;
        t->unk50 = (t->unk4A + 4) << 16;
        t->unk28 = 6;
        t->unk34 = 0;
        PlaySfxIfLocalPlayer(106, t->unk44);
    }
    TaskSetMotionXFacing(0x40000, 0x5A5A5A5A);
    while (1)
    {
        gCurTask->unk3C = 0;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 16;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 1;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 17;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 3;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 18;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 5;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 19;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 7;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 20;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 9;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 21;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 11;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 22;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 13;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 23;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 15;
        TaskYieldTrampoline(2);
    }
}

void sub_080512f8(void)
{
    {
        struct Task *t = gCurTask;
        t->unk78 = 127;
        TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CBA4, t->unk44);
    }
    sub_0801c230(gUnk_0873CB4C);
    {
        struct Task *t = gCurTask;
        if ((t->unk7A & 1) || *(u16 *)&gTerrainResult != 0)
        {
            PlaySfxIfLocalPlayer(125, t->unk44);
            TaskSetEntry(sub_08050814, gCurTaskIdx);
            gCurTask->unk24 = (s32)gUnk_0873BD8C;
        }
        else if (t->unk7B & 1)
        {
            if (t->unk54 & 0xFFFF0000)
                TaskSetMotionXFacing(0x5A5A5A5A, -0x1800);
            gCurTask->unk60 = 0x1000;
        }
    }
    RegisterCollider(gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A, gUnk_0873BD8C);
}

void sub_080513d4(void)
{
    struct Task *t;

    switch (gCurTask->unk28)
    {
    case 0:
        {
            struct Task *u = gCurTask;
            if (u->unk43 == 1)
                u->unk30 = u->unk48 - 8;
            else
                u->unk30 = u->unk48 + 8;
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
        if (t->unk43 == 1)
            t->unk34--;
        else
            t->unk34++;
        if (IsWorldPosOnScreen(gCurTask->unk30 + gCurTask->unk34, gCurTask->unk4A))
        {
            u32 *tbl = gCurTask->unk38;
            QueueSprite(gCurTask->unk42, tbl[gCurTask->unk2C], 0, 0,
                         gCurTask->unk30 + gCurTask->unk34 - gSpriteCameraX,
                         gCurTask->unk4A - gSpriteCameraY);
        }
    }
}
