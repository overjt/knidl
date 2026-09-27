/* game_code_and_rodata 0x08082E68-0x080844C4 (issue #69, module M22 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08082E68 0x080844C4 src/enemy_82e68.c --newpb
 *
 * M22 is a bank of five enemy/object behaviour scripts, all built to the same
 * three-table pattern (rom-map section 9):
 *
 *   entry     -> installs Task.updateCallback (the per-frame hook) and hands
 *                Task.unk73 / Task.state to CallTableEntry, which indexes the
 *                script's table;
 *   unk14 table -> the coroutine BODIES: each sets Task.updateState to its own state
 *                number and then runs a chain of TaskYieldTrampoline waits;
 *   unk15 table -> the per-frame HANDLERS: each is the six-instruction guard
 *                `if (Task.state != N) TaskSetEntry(entry, gCurTaskIdx);` that
 *                re-arms the entry whenever the requested state changes.
 *
 * The three tables of one script sit consecutively in ROM, so the entry's
 * `count` argument is what separates them (`0x08741778` + 7*4 = `0x08741794`).
 *
 * This batch holds:
 *   * the walker script `sub_08082e68` (7 states, tables `0x08741778` /
 *     `0x08741794`, per-frame hook `sub_08082eb4`, re-arm `sub_08082e98`);
 *   * its terrain library: `sub_08083a48` / `sub_08083ad4` / `sub_08083bbc` /
 *     `sub_08083cb8` probe the room with GetCollisionTileAtPixel/GetCollisionTileAtOffset and turn the
 *     `gUnk_087339F0` / `gCollisionTileSlope` / `gUnk_087416A4` index chain into a
 *     tile class, `sub_08083d28` turns a direction code into an aim angle plus
 *     a 16.16 velocity through AngleToVector, and `sub_08083dfc` is the
 *     five-times-four-frame animation wait;
 *   * the one-state script `sub_0808398c` (`0x087417B0` / `0x087417B4`);
 *   * the class-2 task #105 script `sub_08083e6c` (`0x08741E64` /
 *     `0x08741E68` / `0x08741E6C`);
 *   * the class-2 task #108 script `sub_08084050`, whose unk73 table
 *     `0x08741E7C` has two rows (`0x08741E84`/`0x08741E88` and
 *     `0x08741E8C`/`0x08741E90`);
 *   * the class-2 task #176 one-shot `sub_080843fc` and the class-3 task #10
 *     entry `Task_Noddy`, whose script continues in src/enemy_844c4.c.
 *
 * `sub_080839d0`, `sub_08083ee8`, `sub_080840d4` and `sub_0808429c` are dead
 * exports: each is a copy of its host's tail dispatch that nothing in the ROM
 * references (lesson 4.30 / 4.34, curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern vu16 gTaskSlotTypes[];
extern s32 gUnk_03001F2C;

/* ROM tables */
extern s8 gUnk_087339F0[];
extern u8 gCollisionTileSlope[];
extern u32 gUnk_0873F500[];
extern u32 gUnk_0873F720[];
extern u32 gUnk_0873F758[];
extern u32 gUnk_08741778[];
extern u32 gUnk_08741794[];
extern u8 gUnk_087416AD[];
extern s32 gUnk_087416B0[];
extern u8 gUnk_087416CC[];
extern u32 gUnk_0875233C[];
extern u32 gUnk_08752794[];
extern s32 gUnk_08741E54[];
extern s32 gUnk_08741E5C[];
extern u32 gUnk_08741F64[];
extern u32 gUnk_08741E90[];
extern s32 gUnk_08741E94[];
extern s32 gUnk_08741EA4[];
extern u32 gNoddyVariants[];
extern u32 gNoddyFrames[];
extern u32 gFlamerFrames[];
extern s16 gUnk_08741E70[];
extern u32 gUnk_08741E7C[];
extern u32 gUnk_08741E84[];
extern u32 gUnk_08741E88[];
extern u32 gUnk_08741E8C[];
extern u32 gUnk_08741E64[];
extern u32 gUnk_08741E68[];
extern u32 gUnk_08741E6C[];
extern s8 gUnk_08741684[];
extern s8 gUnk_08741688[];
extern s8 gUnk_0874168C[];
extern s8 gUnk_08741690[];
extern s8 gUnk_08741694[];
extern s8 gUnk_08741698[];
extern s8 gUnk_0874169C[];
extern s8 gUnk_087416A0[];
extern u8 gUnk_087416A4[];
extern u16 gUnk_087416D4[];
extern u16 gUnk_087416E4[];
extern s16 gUnk_087416EC[][2];
extern s32 gUnk_087416F8[];
extern s32 gUnk_08741708[];
extern s32 gUnk_08741718[];
extern s32 gUnk_08741728[];
extern struct AnimCmd gUnk_08741744[];
extern struct AnimCmd gUnk_08741758[];
extern u8 gUnk_08741738[];
extern s16 gUnk_0874173C[];
extern u32 gUnk_087417B0[];
extern u32 gUnk_087417B4[];

/* Externals */
extern void TaskYieldTrampoline(u32 frames);
extern void TaskExitTrampoline(void);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(s32 a);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, s32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskStopX(void);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void ActorSetState(s32 a);
extern void ActorSetHitReactions(u32 *p);
extern void ActorSetAttackBox(u32 *p);
extern void TaskFaceNearestPlayer(void);
extern u16 TaskGetAngleToNearestPlayer(s32 a);
extern s32 ActorTickAnim(s32 n);
extern void AngleToVector(s32 a, s32 b);
extern void TaskGetNearestPlayerPos(void);
extern void TaskFaceLikeParent(void);
extern void sub_0806a0f0(s32 a);
extern u8 sub_0806951c(void);
extern void ActorDie(void);
extern void ActorMove(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 ActorStartAnim(struct AnimCmd *p);
extern s32 CreateChildTaskAtOffsetFacing(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern s32 TaskGetFacingTowardNearestPlayer(void);
extern s32 GetDistSq(struct PointPair *p);
extern s32 TaskIsNearestPlayerInRect(struct PointPair *p);
extern s32 TaskIsInRect(struct PointPair *r);
extern void ActorDestroy(void);
extern s32 GetCollisionTileAtPixel(u16 x, u16 y);
extern s32 GetCollisionTileAtOffset(s16 x, s16 y, s32 c, s32 d);
extern void ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern void ActorReactToHit(void);

/* Defined below */
void sub_08082eb4(void);
void sub_080839ec(void);
u8 sub_08083a48(s32 dir);
u8 sub_08083ad4(s32 dir);
u8 sub_08083bbc(s32 dir, s32 k);
u8 sub_08083cb8(s16 x, s16 y);
s32 sub_08083d28(u8 a);
void sub_08083dfc(void);
void sub_08083f04(void);
void sub_080840f0(void);
void sub_080842b8(void);

void sub_08082e68(void)
{
    gCurTask->updateCallback = (u32)sub_08082eb4;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 7, gUnk_08741778);
}

void sub_08082e98(void)
{
    CallTableEntry(gCurTask->state, 7, gUnk_08741778);
}

void sub_08082eb4(void)
{
    switch (gCurTask->updateState)
    {
    case 0:
    case 1:
    case 2:
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 7, gUnk_08741794);
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        CallTableEntry(gCurTask->updateState, 7, gUnk_08741794);
        break;
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08082f04(void)
{
    struct Task *t;
    u16 v;

    gCurTask->updateState = 0;
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskYieldTrampoline(1);
    t = gCurTask;
    t->unk18 = 0;
    t->unk30 = 4;
    do
    {
        t = gCurTask;
        t->unk30 -= 2;
        if (sub_08083a48(t->unk30) != 0)
        {
            gCurTask->unk18 = 1;
            v = TaskGetAngleToNearestPlayer(2);
            t = gCurTask;
            if ((t->unk30 & 1) == 0)
            {
                if ((u16)(v - 64) > 128)
                    t->unk34 = 3;
                else
                    t->unk34 = 1;
            }
            else
            {
                if (v <= 127)
                    t->unk34 = 0;
                else
                    t->unk34 = 2;
            }
            gCurTask->unk1C = 0;
            goto done;
        }
    } while (gCurTask->unk30 != 0);
done:
    if (gCurTask->unk18 == 0)
        ActorSetState(2);
    else
        ActorSetState(1);
    TaskSleepForever();
}

void sub_08082fb4(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(sub_08082e98, gCurTaskIdx);
}

void sub_08082fdc(void)
{
    struct Task *t;

    gCurTask->updateState = 1;
    TaskStop();
    TaskSetFrame(4);
    while (1)
    {
        TaskYieldTrampoline(gUnk_087416AD[gCurTask->unk74]);
        t = gCurTask;
        t->frame++;
        if ((s16)t->frame > 7)
            t->frame = 4;
    }
}

void sub_08083020(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct PointPair p;
    s32 n;
    s32 n2;
    s32 m;
    u8 d;
    u8 e;
    u8 f;

    t = gCurTask;
    switch (t->unk1C)
    {
    case 0:
        d = sub_08083ad4(t->unk30);
        if (d == 0)
        {
            u = gCurTask;
            u->unk1C = 1;
            u->velX = 0;
            u->velY = 0;
            break;
        }
        w = gCurTask;
        w->unk18 = d;
        if ((w->unk30 & 2) != 0 && d == 1)
        {
            e = sub_08083bbc(w->unk30, w->unk34);
            f = sub_08083bbc(gCurTask->unk30, 0);
            if (e > 5 && f > 5)
                gCurTask->unk18 = e;
        }
        d = sub_08083a48(gCurTask->unk34);
        if (d == 1 || ((gCurTask->unk34 & 1) == 0 && d > 1))
        {
            gUnk_03001F2C = n = gCurTask->unk34;
            gCurTask->unk34 = (gCurTask->unk30 + 2) & 3;
            gCurTask->unk30 = n;
            sub_08083d28(d);
        }
        else
        {
            sub_08083d28(gCurTask->unk18);
        }
        break;
    case 1:
        m = t->unk18;
        if (m > 1)
        {
            t->posX &= 0xFFFF0000;
            t->posY &= 0xFFFF0000;
            t->posX += gUnk_087416F8[t->unk30] - gUnk_08741718[t->unk34];
            t->posY += gUnk_08741708[t->unk30] - gUnk_08741728[t->unk34];
            t->unk18 = 1;
            t->unk1C = 0;
        }
        else
        {
            t->posX = (t->posX & 0xFFF00000) | 0x80000;
            t->posY = (t->posY & 0xFFF00000) | 0x80000;
            t->posX += gUnk_087416F8[t->unk30] + gUnk_08741718[t->unk34];
            t->posY += gUnk_08741708[t->unk30] + gUnk_08741728[t->unk34];
            t->unk1C = 2;
            gUnk_03001F2C = n2 = t->unk30;
            t->unk30 = (t->unk34 + 2) & 3;
            t->unk34 = n2;
            sub_08083d28(m);
        }
        x = gCurTask;
        x->pixelX = x->posX >> 16;
        x->pixelY = x->posY >> 16;
        break;
    case 2:
        if (sub_08083ad4(t->unk30) == 0)
            ActorSetState(0);
        y = gCurTask;
        y->unk1C = 0;
        if ((y->unk34 & 1) != 0)
            y->velY = 0;
        break;
    }
    v = gCurTask;
    if (v->unk30 != 0)
        v->onGround = 0;
    else
        v->onGround = 1;
    z = gCurTask;
    if ((z->unk28 & 0x80) == 0 && --z->unk28 == 0)
    {
        p.x0 = z->pixelX - 64;
        p.y0 = z->pixelY - 64;
        p.x1 = z->pixelX + 64;
        p.y1 = gCurTask->pixelY + 64;
        if (TaskIsNearestPlayerInRect(&p) != 0)
            ActorSetState(3);
        gCurTask->unk28 = 20;
    }
    if (gCurTask->state != 1)
        TaskSetEntry(sub_08082e98, gCurTaskIdx);
}

void sub_080832d0(void)
{
    struct Task *t;

    gCurTask->updateState = 2;
    t = gCurTask;
    t->unk30 = 0;
    t->unk18 = 0;
    t->onGround = 0;
    TaskStopX();
    TaskSetMotionY(0, 0x1500, 0x30000);
    TaskSetFrame(4);
    TaskSleepForever();
}

void sub_0808330c(void)
{
    u16 m;

    if (gCurTask->onGround != 0)
    {
        m = TaskGetAngleToNearestPlayer(2);
        m -= 64;
        if (m > 128)
            gCurTask->unk34 = 3;
        else
            gCurTask->unk34 = 1;
        gCurTask->unk1C = 0;
        ActorSetState(1);
        TaskSetEntry(sub_08082e98, gCurTaskIdx);
    }
}

void sub_08083370(void)
{
    struct Task *t;

    gCurTask->updateState = 3;
    TaskFaceNearestPlayer();
    TaskStop();
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    TaskSetFrame(4);
    gCurTask->unk6C = 0;
    do
    {
        t = gCurTask;
        t->velY = gUnk_087416B0[(s16)t->unk6C];
        TaskYieldTrampoline(gUnk_087416CC[(s16)t->unk6C]);
        gCurTask->frame++;
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    sub_08083dfc();
    ActorSetState(4);
    TaskSleepForever();
}

void sub_08083400(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(sub_08082e98, gCurTaskIdx);
}

void sub_08083428(void)
{
    struct Task *t;

    gCurTask->updateState = 4;
    ActorSetAttackBox(gUnk_0873F758);
    gCurTask->unk20 = TaskGetAngleToNearestPlayer(3);
    gCurTask->unk1C = 1;
    while (1)
    {
        TaskSetFrame(8);
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        do
        {
            t = gCurTask;
            t->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 2);
    }
}

void sub_08083488(void)
{
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct PointPair p;
    s16 a;

    if (--gCurTask->unk1C == 0)
    {
        TaskGetNearestPlayerPos();
        if (TaskGetFacingTowardNearestPlayer() == 1)
            gUnk_030023B4 = gUnk_030023B4 - 64;
        else
            gUnk_030023B4 = gUnk_030023B4 + 64;
        a = (u16)ArcTan2(gUnk_030023B4 - gCurTask->pixelX, gUnk_030023D4 - gCurTask->pixelY) >> 7;
        if (a > 384 && gCurTask->unk20 <= 127)
            a = a - 512;
        else if (a <= 127 && gCurTask->unk20 > 384)
            a = a + 512;
        if (a > (u = gCurTask)->unk20)
            u->unk20 = u->unk20 + 32;
        else
            u->unk20 = u->unk20 - 32;
        v = gCurTask;
        v->unk20 &= 0x1FF;
        p.x0 = v->pixelX;
        p.y0 = v->pixelY;
        p.x1 = gUnk_030023B4;
        p.y1 = gUnk_030023D4;
        if (GetDistSq(&p) <= 99)
        {
            ActorSetState(5);
            TaskSetEntry(sub_08082e98, gCurTaskIdx);
        }
        else
        {
            w = gCurTask;
            w->unk1C = gUnk_08741738[w->unk74];
            AngleToVector((s16)w->unk20, gUnk_0874173C[w->unk74]);
            x = gCurTask;
            x->velX = gUnk_030023B4;
            x->velY = gUnk_030023D4;
        }
    }
}

void sub_08083614(void)
{
    s32 n;

    gCurTask->updateState = 5;
    TaskFaceNearestPlayer();
    TaskStop();
    gCurTask->unk20 = ActorStartAnim(gUnk_08741744);
    TaskSetMotionXFacing(-0x18000, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(-0xC000, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(0x40000, 0x5A5A5A5A);
    gCurTask->unk20 = ActorStartAnim(gUnk_08741758);
    gCurTask->unk24 = -1;
    while (1)
    {
        gCurTask->unk24++;
        if ((gCurTask->unk24 & 3) == 0)
        {
            gCurTask->unk46 = CreateChildTaskAtOffsetFacing(176, 0, 0, 1);
            (gTasks + (s16)gCurTask->unk46)->unk18 = (gCurTask->unk24 >> 2) & 3;
        }
        n = TaskGetNearestPlayerDx();
        gCurTask->unk1C = n;
        if (gCurTask->facing == 1 && n < 0)
            break;
        if (gCurTask->facing == -1 && n > 0)
            break;
        TaskYieldTrampoline(1);
    }
    TaskYieldTrampoline(12);
    gCurTask->unk20 = ActorStartAnim(gUnk_08741744);
    TaskSetMotionXFacing(0x30000, 0x5A5A5A5A);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(6);
    TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(6);
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    gCurTask->velY = -0x30000;
    TaskYieldTrampoline(6);
    if (gCurTask->unk2C == 0)
    {
        TaskSetMotionXFacing(0, 0x5A5A5A5A);
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(6);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(6);
        TaskStop();
        gCurTask->unk2C++;
        ActorSetState(6);
    }
    TaskSleepForever();
}

void sub_0808379c(void)
{
    gCurTask->unk20 = ActorTickAnim(gCurTask->unk20);
    if (gCurTask->state != 5)
        TaskSetEntry(sub_08082e98, gCurTaskIdx);
}

void sub_080837d0(void)
{
    struct Task *t;

    gCurTask->updateState = 6;
    ActorSetAttackBox(gUnk_0873F720);
    gCurTask->unk20 = TaskGetAngleToNearestPlayer(3);
    gCurTask->unk1C = 1;
    TaskSetFrame(8);
    while (gCurTask->unk1C != 0)
    {
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->frame++;
        if ((s16)t->frame > 11)
            t->frame = 8;
    }
    TaskStop();
    sub_08083dfc();
    TaskSetFrame(8);
    TaskYieldTrampoline(6);
    gCurTask->unk6C = 0;
    do
    {
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(6);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskSetFrame(4);
    TaskYieldTrampoline(8);
    gCurTask->unk6C = 0;
    do
    {
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    gCurTask->unk28 = 20;
    ActorSetState(2);
    TaskSleepForever();
}

void sub_080838bc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    u16 n;

    t = gCurTask;
    if (t->unk1C != 0)
    {
        if (--t->unk1C == 0)
        {
            n = TaskGetAngleToNearestPlayer(3);
            u = gCurTask;
            if (n > u->unk20)
                u->unk20 = u->unk20 + 32;
            else
                u->unk20 = u->unk20 - 32;
            gUnk_03001F2C = 4;
            while (gUnk_03001F2C != 0 && sub_08083ad4(gUnk_03001F2C - 1) == 0)
                gUnk_03001F2C--;
            if (gUnk_03001F2C != 0)
            {
                v = gCurTask;
                v->unk1C = gUnk_08741738[v->unk74];
                AngleToVector((s16)v->unk20, gUnk_0874173C[v->unk74]);
                w = gCurTask;
                w->velX = gUnk_030023B4;
                w->velY = gUnk_030023D4;
            }
        }
    }
    if (gCurTask->state != 6)
        TaskSetEntry(sub_08082e98, gCurTaskIdx);
}

void sub_0808398c(void)
{
    gCurTask->updateCallback = (u32)sub_080839ec;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->health = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_087417B0);
}

void sub_080839d0(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_087417B0);
}

void sub_080839ec(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_087417B4);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08083a10(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    TaskSetFrame(4);
    while (1)
    {
        TaskYieldTrampoline(10);
        t = gCurTask;
        t->frame++;
        if ((s16)t->frame > 7)
            t->frame = 4;
    }
}

void sub_08083a44(void)
{
}

u8 sub_08083a48(s32 dir)
{
    struct Task *t;
    s16 a;
    s16 b;
    s16 i;
    u8 r;

    r = 0;
    t = gCurTask;
    a = t->pixelX + gUnk_08741684[dir];
    b = t->pixelY + gUnk_08741688[dir];
    i = GetCollisionTileAtPixel(a, b);
    if (gUnk_087339F0[i] != 0)
    {
        r = gUnk_087416A4[gCollisionTileSlope[i]];
        if ((u8)(r - 2) <= 3 && gCurTask->onGround == 0)
            r = 0;
    }
    return r;
}

u8 sub_08083ad4(s32 dir)
{
    struct Task *t;
    u16 a;
    u16 b;
    u16 c;
    u16 d;
    u8 r;

    t = gCurTask;
    if ((t->unk34 & 2) != 0)
    {
        a = t->pixelX + gUnk_0874168C[dir];
        b = t->pixelY + gUnk_08741690[dir];
        c = t->pixelX + gUnk_08741694[dir];
        d = t->pixelY + gUnk_08741698[dir];
    }
    else
    {
        a = t->pixelX + gUnk_08741694[dir];
        b = t->pixelY + gUnk_08741698[dir];
        c = t->pixelX + gUnk_0874168C[dir];
        d = t->pixelY + gUnk_08741690[dir];
    }
    r = sub_08083cb8(a, b);
    if (r == 0)
        r = sub_08083cb8(c, d);
    return r;
}

u8 sub_08083bbc(s32 dir, s32 k)
{
    struct Task *t;
    u16 a;
    u16 b;
    s16 i;
    u8 r;
    s8 *p;
    s8 *q;
    r = 0;
    t = gCurTask;
    if ((t->unk34 & 2) != 0)
    {
        a = t->pixelX + gUnk_0874168C[dir];
        b = t->pixelY + gUnk_08741690[dir];
    }
    else
    {
        a = t->pixelX + gUnk_08741694[dir];
        b = t->pixelY + gUnk_08741698[dir];
    }
    p = &gUnk_0874169C[k];
    q = &gUnk_087416A0[k];
    i = GetCollisionTileAtOffset(a, b, *p, *q);
    if (i == -1)
        return 0;
    if (gUnk_087339F0[i] != 0)
    {
        r = gUnk_087416A4[gCollisionTileSlope[i]];
        if ((u8)(r - 2) <= 3 && gCurTask->onGround == 0)
            r = 0;
    }
    return r;
}

u8 sub_08083cb8(s16 x, s16 y)
{
    s32 i;
    u8 r;

    r = 0;
    i = (s16)GetCollisionTileAtPixel(x, y);
    if (i == -1)
        return 0;
    if (gUnk_087339F0[i] != 0)
    {
        r = gUnk_087416A4[gCollisionTileSlope[i]];
        if ((u8)(r - 2) <= 3 && gCurTask->onGround == 0)
            r = 0;
    }
    return r;
}

s32 sub_08083d28(u8 a)
{
    struct Task *t;
    s16 v;

    if (a > 5)
    {
        v = gUnk_087416D4[a - 2];
        if (gCurTask->unk34 == 1)
            v = (v + 272) & 0x1FF;
        else
            v = (v - 16) & 0x1FF;
    }
    else if (a > 1)
    {
        v = gUnk_087416D4[a - 2];
        if (gCurTask->unk34 == 1)
            v = (v + 256) & 0x1FF;
    }
    else
    {
        v = gUnk_087416E4[gCurTask->unk34];
    }
    AngleToVector(v, gUnk_087416EC[gCurTask->unk74][0]);
    t = gCurTask;
    t->velX = gUnk_030023B4;
    t->velY = gUnk_030023D4;
}

void sub_08083dfc(void)
{
    struct Task *t;

    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(8);
        TaskYieldTrampoline(3);
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(3);
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(3);
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(3);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 4);
}

u8 sub_08083e5c(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_08083e6c(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    gCurTask->frameTable = gUnk_0875233C;
    PlaySfx(186);
    CallTableEntry(gCurTask->unk73, 1, gUnk_08741E64);
}

void sub_08083eb4(void)
{
    gCurTask->updateCallback = (u32)sub_08083f04;
    TaskFaceLikeParent();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08741E68);
}

void sub_08083ee8(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08741E68);
}

void sub_08083f04(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_08741E6C);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_08083f48(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(gUnk_08741E54[gCurTask->unk74], gUnk_08741E5C[gCurTask->unk74]);
    gCurTask->speedLimitX = 0x2A800;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->frame++;
        TaskYieldTrampoline(2);
    }
}

void sub_08083fbc(void)
{
    struct Task *t;
    struct PointPair p;
    vu16 *q;
    s16 i;

    q = gTaskSlotTypes;
    i = gCurTask->parent;
    if ((s16)q[i] != -1)
    {
        t = gTasks + i;
        p.x0 = t->pixelX - 6;
        p.y0 = t->pixelY - 6;
        p.x1 = t->pixelX + 6;
        p.y1 = t->pixelY + 6;
        if (TaskIsInRect(&p) != 0)
            ActorDestroy();
    }
}

void sub_08084050(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 3;
    u = gCurTask;
    u->frameTable = gUnk_08752794;
    u->onGround = 0;
    TaskFaceLikeParent();
    PlaySfx(194);
    CallTableEntry(gCurTask->unk73, 2, gUnk_08741E7C);
}

void sub_080840a4(void)
{
    gCurTask->updateCallback = (u32)sub_080840f0;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08741E84);
}

void sub_080840d4(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08741E84);
}

void sub_080840f0(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_08741E88);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_08084114(void)
{
    struct Task *t;
    struct Task *o;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    s32 n;
    s32 m;

    t = gCurTask;
    o = gTasks + (s16)t->parent;
    t->updateState = 0;
    u = gCurTask;
    if (u->facing == 1)
        u->unk28 = 0;
    else
        u->unk28 = 256;
    v = gCurTask;
    n = (s16)o->unk6E;
    v->unk2C = n;
    m = (v->unk28 + gUnk_08741E70[n]) & 0x1FF;
    v->unk28 = m;
    if ((u32)(m - 128) > 255)
        v->posX = v->posX + 0xE0000;
    else
        v->posX = v->posX - 0xE0000;
    AngleToVector((s16)gCurTask->unk28, 768);
    w = gCurTask;
    w->velX = gUnk_030023B4;
    w->velY = gUnk_030023D4;
    if ((w->unk2C & 2) != 0)
        w->spriteFlags = w->spriteFlags & 0x7FFF;
    else
        w->spriteFlags = w->spriteFlags | 0x8000;
    gCurTask->frame = 4;
    TaskYieldTrampoline(2);
    x = gCurTask;
    x->frame++;
    TaskYieldTrampoline(3);
    x = gCurTask;
    x->frame++;
    TaskYieldTrampoline(3);
    x = gCurTask;
    x->frame++;
    TaskYieldTrampoline(1);
    x = gCurTask;
    x->frame++;
    TaskYieldTrampoline(2);
    x = gCurTask;
    x->frame++;
    TaskYieldTrampoline(2);
    ActorDestroy();
}

void sub_08084248(void)
{
}

void sub_0808424c(void)
{
    gCurTask->updateCallback = (u32)sub_080842b8;
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorSetState(0);
        CallTableEntry(gCurTask->state, 1, gUnk_08741E8C);
    }
}

void sub_0808429c(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_08741E8C);
}

void sub_080842b8(void)
{
    if (sub_0806951c() == 1)
    {
        ActorSetHitReactions(gUnk_08741F64);
        TaskSetEntry(ActorDie, gCurTaskIdx);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 1, gUnk_08741E90);
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_08084308(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 n;

    gCurTask->updateState = 0;
    gCurTask->unk28 = 0;
    gCurTask->unk2C = n = TaskGetAngleToNearestPlayer(3);
    if (n >= 25 && n <= 127)
        gCurTask->unk2C = 24;
    else if (n >= 128 && n <= 231)
        gCurTask->unk2C = 232;
    else if (n >= 281 && n <= 383)
        gCurTask->unk2C = 280;
    else if (n >= 384 && n <= 487)
        gCurTask->unk2C = 488;
    AngleToVector((s16)gCurTask->unk2C, 768);
    u = gCurTask;
    u->velX = gUnk_030023B4;
    u->velY = gUnk_030023D4;
    while (1)
    {
        TaskSetFrame(10);
        TaskYieldTrampoline(2);
        TaskSetFrame(6);
        TaskYieldTrampoline(1);
        v = gCurTask;
        v->frame++;
        TaskYieldTrampoline(1);
        TaskSetFrame(14);
        TaskYieldTrampoline(2);
        TaskSetFrame(11);
        TaskYieldTrampoline(2);
        TaskSetFrame(13);
        TaskYieldTrampoline(2);
        v = gCurTask;
        v->frame--;
        TaskYieldTrampoline(1);
        TaskSetFrame(15);
        TaskYieldTrampoline(2);
    }
}

void sub_080843f8(void)
{
}

void sub_080843fc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 3;
    gCurTask->frameTable = gFlamerFrames;
    TaskFaceLikeParent();
    TaskSetMotionXFacing(0x10000, 0);
    u = gCurTask;
    u->velY = gUnk_08741E94[u->unk18];
    u->accelY = gUnk_08741EA4[u->unk18];
    TaskSetFrame(18);
    TaskYieldTrampoline(4);
    v = gCurTask;
    v->frame++;
    TaskYieldTrampoline(4);
    v = gCurTask;
    v->frame++;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void Task_Noddy(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->frameTable = gNoddyFrames;
    CallTableEntry(u->unk73, 2, gNoddyVariants);
}
