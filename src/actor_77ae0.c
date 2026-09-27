/* game_code_and_rodata 0x08077AE0-0x08078B68 (issue #79, module M19 batch 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08077AE0 0x08078B68 src/actor_77ae0.c --newpb
 *
 * M19 batch 5: task types #77 (Task_BigSwitch), #78 (Task_Stake), #79
 * (sub_08078598, the credits particle system over gUnk_03000FE0) and #8
 * (Task_WaddleDee), whose states hand off to module M20.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"


/* The eight 4-byte records at 0x03000FE0 that M19's credits tasks animate:
   a frame table index (unk00), the frame within it (unk01), the timer
   (unk02) and the countdown sub_080782b4 draws on (unk03). */
struct M19Particle
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 unk02;
    /*0x03*/ u8 unk03;
};

/* The 24-entry animation rows at 0x08740320 / 0x087404A0 the credits
   particles walk: `unk00` indexes the gfx pointer table, `unk02` is the
   step's delay. */
struct M19Frame
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 unk02;
    /*0x03*/ u8 unk03;
};


/* RAM cells and ROM tables */
extern s16 gSpriteCameraX;
extern s16 gSpriteCameraY;
extern s8 gCannonFuseState;
extern struct AnimCmd gUnk_087406A0[];
extern struct M19Frame gUnk_08740320[][24];
extern struct M19Frame gUnk_087404A0[][24];
extern struct M19Particle gUnk_03000FE0[];
extern struct PlayerState gPlayerStates[];
extern struct Task * gCurTask;
extern u16 gPlayerCount;
extern u32 gBigSwitchVariants[];
extern u32 gBigSwitchStates[];
extern u32 gBigSwitchStateUpdates[];
extern u32 gStakeVariants[];
extern u32 gStakeStates[];
extern u32 gStakeStateUpdates[];
extern u32 gUnk_087402FC[];
extern u32 gWaddleDeeVariants[];
extern u32 gUnk_08740BD4[];
extern u32 gWaddleDeeFrames[];
extern u32 gBigSwitchFrames[];
extern u32 gStakeFrames[];
extern u32 gUnk_08752DB8[];
extern u32 gUnk_08752E00[];
extern u8 gUnk_02004B64;
extern u8 gActivePlayerMask;
extern u8 gUnk_08740620[];
extern vs16 gBrightness;
extern vs32 gCurTaskIdx;

/* callees */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 PlaySfx(u32 a);
extern s32 sub_08009e14();
extern s32 sub_08009e20();
extern s32 GetCollisionTileAtPixel(u16 x, u16 y);
extern s32 sub_08025bc8();
extern s32 ActorStartAnim(struct AnimCmd *p);
extern s32 sub_080b4204();
extern u32 RandomRange(u32 range);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern void TaskYieldTrampoline(u32 a);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void FadeInSfx(u16 speed);
extern void FadeOutSfx(s32 speed);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *a, u32 i);
extern void TaskStop(void);
extern void RequestScreenShake(u32 a);
extern void SetRoomUpdateFlags(u32);
extern void ActorSetState(u8 v);
extern void ActorSetStateSlot(u32 i, u8 v);
extern void ActorDestroy(void);
extern void TaskTurnAroundAndReverseX(void);
extern void ActorDrawWorldInView(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void ActorMove(void);
extern void sub_08066b70(void);
extern void sub_08066c08(u32 *p, s32 b);
extern void sub_08066c3c(u32 *p);
extern void sub_080670ac(u16 a);
extern void sub_080670d4(void);
extern void sub_08067108(void);
extern void sub_08067114(void);
extern void sub_0806a0f0(s32 a);
extern void ActorDie(void);
extern void sub_0806ee2c(void);
extern void sub_0807775c(void);
extern void sub_08077830(void);
extern void sub_08077980(void);
extern void sub_08077a48(void);
extern void CannonFuseEnterState(void);
extern void WaddleDeeWalkEnterState(void);
extern void WaddleDeePaceEnterState(void);
extern void WaddleDeeJumpEnterState(void);
extern void ParasolWaddleDeeWalkEnterState(void);
extern void ParasolWaddleDeeStandEnterState(void);

/* defined below */
void BigSwitchEnterState(void);
void BigSwitchUpdate(void);
void StakeUpdate(void);
void sub_080781fc(struct M19Particle *p);
void sub_080782b4(struct M19Particle *p);
void sub_0807831c(struct M19Particle *p);
u8 sub_080783e0(s16 x, s16 y);
void sub_080786b4(void);
void sub_08078734(void);
void sub_080787b8(void);
void sub_0807883c(void);
void sub_080788e0(void);

void sub_08077ae0(void)
{
    gCurTask->updateState = 0;
    gCurTask->facing = 1;
    TaskStop();
    gCannonFuseState = -1;
    {
        struct Task *t = gCurTask;

        t->posX = t->unk30 << 16;
        t->posY = t->unk2C << 16;
        t->frame = 42;
    }
    TaskSleepForever();
}

void sub_08077b24(void)
{
    if (ActorCheckHits())
    {
        gCannonFuseState = 0;
        gCurTask->unk24 = 1;
        ActorSetState(1);
        TaskSetEntry(CannonFuseEnterState, gCurTaskIdx);
    }
}

void sub_08077b60(void)
{
    gCurTask->updateState = 1;
    TaskStop();
    sub_0807775c();
    sub_08077a48();
    while (gCannonFuseState == 0)
    {
        PlaySfx(230);
        TaskYieldTrampoline(3);
    }
    TaskSleepForever();
}

void sub_08077ba8(void)
{
    if (gCurTask->unk20 != -1)
    {
        sub_08077830();
    }
    else if (gCannonFuseState == 0)
    {
        ActorSetState(2);
        TaskSetEntry(CannonFuseEnterState, gCurTaskIdx);
    }
}

void sub_08077bf0(void)
{
    gCurTask->updateState = 2;
    TaskStop();
    gCurTask->unk24 = -1;
    sub_0807775c();
    {
        struct Task *t = gCurTask;

        t->pixelY += 16;
        t->posY = t->pixelY << 16;
    }
    TaskSleepForever();
}

void sub_08077c2c(void)
{
    if (gCurTask->unk20 != -1)
        sub_08077980();
    if (gCurTask->state != 2)
        TaskSetEntry(CannonFuseEnterState, gCurTaskIdx);
}

void Task_BigSwitch(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)ActorMove;
        t->drawCallback = (u32)ActorDrawWorldInView;
        t->layer = 11;
    }
    {
        struct Task *t = gCurTask;

        t->frameTable = gBigSwitchFrames;
        CallTableEntry(t->variant, 1, gBigSwitchVariants);
    }
}

s32 sub_08077ca4(void)
{
    struct PlayerState *p = &gPlayerStates[gCurTask->hitterSlot];

    if (p->ability == 7 && p->mode == 13)
        return 0;
    return 1;
}

void sub_08077cd4(void)
{
    gCurTask->frame = 1;
    sub_080670ac(15);
    SetRoomUpdateFlags(2);
}

void sub_08077cf4(void)
{
    sub_08067108();
    gUnk_02004B64 = 1;
    PlaySfx(226);
    RequestScreenShake(4);
    sub_08077cd4();
    sub_08009e14();
    ActorSetStateSlot(gCurTaskIdx, 1);
    TaskSetEntry(BigSwitchEnterState, gCurTaskIdx);
}

void sub_08077d38(s32 id)
{
    ActorSetStateSlot(id, 2);
    TaskSetEntry(BigSwitchEnterState, id);
}

void sub_08077d54(void)
{
    u8 i;

    sub_08067108();
    TaskYieldTrampoline(15);
    FadeInSfx(16);
    TaskYieldTrampoline(15);
    RequestScreenShake(4);
    gUnk_02004B64 = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            u8 done;

            do
            {
                PlaySfx(221);
                done = sub_080b4204(i);
                TaskYieldTrampoline(8);
            } while (done == 0);
        }
    }
    sub_080670d4();
    sub_08009e20();
    sub_08067114();
    ActorDie();
}

void BigSwitchInit(void)
{
    gCurTask->updateCallback = (u32)BigSwitchUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gBigSwitchStates);
}

void BigSwitchUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gBigSwitchStateUpdates);
}

void BigSwitchEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gBigSwitchStates);
}

void sub_08077e4c(void)
{
    gCurTask->updateState = 0;
    gCurTask->facing = 1;
    TaskStop();
    gCurTask->frame = 0;
    TaskSleepForever();
}

void sub_08077e74(void)
{
    if (gUnk_02004B64 == 0 && ActorCheckHits() && (u8)sub_08077ca4())
        sub_08077cf4();
}

void sub_08077e9c(void)
{
    gCurTask->updateState = 1;
    TaskYieldTrampoline(8);
    FadeOutSfx(16);
    sub_08025bc8(gCurTaskIdx);
    TaskSleepForever();
}

void sub_08077ecc(void)
{
}

void sub_08077ed0(void)
{
    gCurTask->updateState = 2;
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    sub_08077d54();
    TaskSleepForever();
}

void sub_08077f08(void)
{
}

void Task_Stake(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)ActorMove;
        t->drawCallback = (u32)ActorDrawWorldInView;
        t->layer = 11;
    }
    {
        struct Task *t = gCurTask;

        t->frameTable = gStakeFrames;
        CallTableEntry(t->variant, 1, gStakeVariants);
    }
}

void StakeInit(void)
{
    gCurTask->updateCallback = (u32)StakeUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gStakeStates);
}

void StakeUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gStakeStateUpdates);
}

void sub_08077f98(void)
{
    gCurTask->updateState = 0;
    gCurTask->frame = 0;
    while (GetCollisionTileAtPixel(gCurTask->pixelX, gCurTask->pixelY) == 51)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0xFFFF;
    ActorDestroy();
}

void sub_08077ff4(void)
{
}

void sub_08077ff8(void)
{
    gCurTask->unk28 = 0;
    do
    {
        sub_080781fc(&gUnk_03000FE0[gCurTask->unk28]);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 7);
}

void sub_0807802c(void)
{
    if (gSpriteCameraY > gCurTask->unk34)
    {
        for (gCurTask->unk28 = 0;
             gCurTask->unk28 < gCurTask->unk30;
             gCurTask->unk28++)
        {
            struct M19Particle *base = gUnk_03000FE0;
            struct M19Particle *p = &base[gCurTask->unk28];

            if (p->unk03 > 8)
                sub_080782b4(p);
        }
    }
    else
    {
        for (gCurTask->unk28 = 0;
             gCurTask->unk28 < gCurTask->unk30;
             gCurTask->unk28++)
        {
            struct M19Particle *base = gUnk_03000FE0;
            struct M19Particle *p = &base[gCurTask->unk28];

            if (p->unk03 > gCurTask->unk34 - gSpriteCameraY)
                sub_080782b4(p);
        }
    }
}

void sub_080780c4(void)
{
    gCurTask->unk28 = 0;
    do
    {
        sub_0807831c(&gUnk_03000FE0[gCurTask->unk28]);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 2);
    gCurTask->unk28 = 3;
    do
    {
        sub_0807831c(&gUnk_03000FE0[gCurTask->unk28]);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 5);
}

void sub_0807811c(struct M19Particle *p)
{
    p->unk01++;
    if (gUnk_08740320[p->unk00][p->unk01].unk00 == 255)
        p->unk01 = 0;
    {
        s32 j = p->unk01 * 4;
        s32 k = p->unk00 * 96;

        p->unk02 += ((u8 *)gUnk_08740320)[j + k + 2];
    }
    if (p->unk02 > 240)
    {
        p->unk02 = 0;
        p->unk03 = RandomRange(132) + 8;
    }
}

void sub_0807817c(struct M19Particle *p, u8 a, u8 b)
{
    switch (a)
    {
    case 0:
    case 1:
        p->unk02 = RandomRange(64) + 100;
        break;
    case 2:
        p->unk02 = RandomRange(48) + 170;
        break;
    case 3:
        switch (b)
        {
        case 0:
            p->unk02 = RandomRange(48) + 120;
            break;
        case 1:
            p->unk02 = RandomRange(48) + 40;
            break;
        }
        break;
    }
    p->unk03 = RandomRange(132) + 8;
    p->unk01 = RandomRange(10);
    p->unk00 = gUnk_08740620[gCurTask->unk28];
}

void sub_080781fc(struct M19Particle *p)
{
    struct Task *t = gCurTask;

    QueueSprite(t->layer,
                 gUnk_08752DB8[gUnk_08740320[p->unk00][p->unk01].unk00],
                 t->spriteFlags, t->tileWord, p->unk02, p->unk03);
}

void sub_08078258(struct M19Particle *p)
{
    struct Task *t = gCurTask;

    QueueSprite(t->layer,
                 gUnk_08752E00[gUnk_087404A0[p->unk00][p->unk01].unk00],
                 t->spriteFlags, t->tileWord, p->unk02, p->unk03);
}

void sub_080782b4(struct M19Particle *p)
{
    struct Task *t = gCurTask;

    QueueSprite(t->layer,
                 gUnk_08752E00[gUnk_087404A0[p->unk00][p->unk01].unk00],
                 t->spriteFlags, t->tileWord, p->unk02 - gSpriteCameraX, p->unk03);
}

void sub_0807831c(struct M19Particle *p)
{
    gCurTask->unk2C = 0;
    do
    {
        if (sub_080783e0(p->unk02 - gSpriteCameraX + gCurTask->unk2C * 192,
                         p->unk03))
        {
            struct Task *t = gCurTask;

            QueueSprite(t->layer,
                         gUnk_08752E00[gUnk_087404A0[p->unk00][p->unk01].unk00],
                         t->spriteFlags, t->tileWord,
                         p->unk02 - gSpriteCameraX + t->unk2C * 192,
                         p->unk03);
        }
        gCurTask->unk2C++;
    } while (gCurTask->unk2C <= 4);
}

u8 sub_080783e0(s16 x, s16 y)
{
    u16 v = y;

    if ((u32)((x << 16) + 0x3F0000) > 0x16E0000)
        return 0;
    if ((s16)v <= -64)
        return 0;
    if ((s16)v > 223)
        return 0;
    return 1;
}

void sub_0807840c(struct M19Particle *p, u8 a)
{
    p->unk01++;
    if (gUnk_087404A0[p->unk00][p->unk01].unk00 == 255)
        p->unk01 = 0;
    switch (a)
    {
    case 0:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->unk03 -= q[i];
        }
        if (p->unk03 <= 7)
        {
            p->unk02 = RandomRange(250);
            p->unk03 = 160;
        }
        break;
    case 2:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->unk03 -= q[i];
        }
        if (p->unk03 <= 7)
        {
            {
                s32 v = RandomRange(48) + 232;

                p->unk02 = v + (u8)gCurTask->pixelX;
            }
            p->unk03 = 160;
        }
        break;
    case 1:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->unk03 -= q[i];
        }
        if (p->unk03 <= 7)
        {
            {
                s32 v = RandomRange(64) + 224;

                p->unk02 = v + (u8)gCurTask->pixelX;
            }
            p->unk03 = 160;
        }
        break;
    case 3:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->unk03 -= q[i];
        }
        if (p->unk03 <= 7)
        {
            {
                s32 v = RandomRange(48) + 232;

                p->unk02 = v + (u8)gCurTask->pixelX + 96;
            }
            p->unk03 = 160;
        }
        break;
    case 4:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->unk03 += q[i];
        }
        if (p->unk03 > 160)
        {
            {
                s32 v = RandomRange(48) + 232;

                p->unk02 = v + (u8)gCurTask->pixelX;
            }
            p->unk03 = 0;
        }
        break;
    }
}

void sub_08078598(void)
{
    switch (gCurTask->variant)
    {
    case 0:
        {
            struct Task *t = gCurTask;

            t->drawCallback = (u32)sub_08077ff8;
            t->frameTable = gUnk_08752DB8;
        }
        break;
    case 1:
        {
            struct Task *t = gCurTask;

            t->drawCallback = (u32)sub_0807802c;
            t->frameTable = gUnk_08752E00;
        }
        break;
    case 2:
        {
            struct Task *t = gCurTask;

            t->drawCallback = (u32)sub_0807802c;
            t->frameTable = gUnk_08752E00;
        }
        break;
    case 3:
        {
            struct Task *t = gCurTask;

            t->drawCallback = (u32)sub_0807802c;
            t->frameTable = gUnk_08752E00;
        }
        break;
    case 4:
        {
            struct Task *t = gCurTask;

            t->drawCallback = (u32)sub_080780c4;
            t->frameTable = gUnk_08752E00;
        }
        break;
    default:
        sub_0806ee2c();
        break;
    }
    gCurTask->layer = 4;
    {
        struct Task *t = gCurTask;

        t->tileWord = 0xF000 | t->tileWord;
        CallTableEntry(t->variant, 5, gUnk_087402FC);
    }
}

void sub_08078670(void)
{
    gCurTask->updateCallback = (u32)sub_080786b4;
    gCurTask->unk28 = 0;
    do
    {
        sub_0807817c(&gUnk_03000FE0[gCurTask->unk28], 0, 0);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 7);
    TaskSleepForever();
}

void sub_080786b4(void)
{
    gCurTask->unk28 = 0;
    do
    {
        sub_0807811c(&gUnk_03000FE0[gCurTask->unk28]);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 7);
}

void sub_080786e8(void)
{
    {
        struct Task *t = gCurTask;

        t->updateCallback = (u32)sub_08078734;
        t->unk30 = 8;
        t->unk34 = 0;
    }
    gCurTask->unk28 = 0;
    do
    {
        sub_0807817c(&gUnk_03000FE0[gCurTask->unk28], 0, 0);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 7);
    TaskSleepForever();
}

void sub_08078734(void)
{
    gCurTask->unk28 = 0;
    do
    {
        sub_0807840c(&gUnk_03000FE0[gCurTask->unk28], 0);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 7);
}

void sub_0807876c(void)
{
    {
        struct Task *t = gCurTask;

        t->updateCallback = (u32)sub_080787b8;
        t->unk30 = 3;
        t->unk34 = 208;
    }
    gCurTask->unk28 = 0;
    do
    {
        sub_0807817c(&gUnk_03000FE0[gCurTask->unk28], 2, 0);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 2);
    TaskSleepForever();
}

void sub_080787b8(void)
{
    gCurTask->unk28 = 0;
    do
    {
        sub_0807840c(&gUnk_03000FE0[gCurTask->unk28], 2);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 2);
}

void sub_080787f0(void)
{
    {
        struct Task *t = gCurTask;

        t->updateCallback = (u32)sub_0807883c;
        t->unk30 = 4;
        t->unk34 = 64;
    }
    gCurTask->unk28 = 0;
    do
    {
        sub_0807817c(&gUnk_03000FE0[gCurTask->unk28], 1, 0);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 3);
    TaskSleepForever();
}

void sub_0807883c(void)
{
    gCurTask->unk28 = 0;
    do
    {
        sub_0807840c(&gUnk_03000FE0[gCurTask->unk28], 1);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 3);
}

void sub_08078874(void)
{
    gCurTask->updateCallback = (u32)sub_080788e0;
    gCurTask->unk28 = 0;
    do
    {
        sub_0807817c(&gUnk_03000FE0[gCurTask->unk28], 3, 0);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 2);
    gCurTask->unk28 = 3;
    do
    {
        sub_0807817c(&gUnk_03000FE0[gCurTask->unk28], 3, 1);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 5);
    TaskSleepForever();
}

void sub_080788e0(void)
{
    gCurTask->unk28 = 0;
    do
    {
        sub_0807840c(&gUnk_03000FE0[gCurTask->unk28], 3);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 2);
    gCurTask->unk28 = 3;
    do
    {
        sub_0807840c(&gUnk_03000FE0[gCurTask->unk28], 4);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 5);
}

void Task_WaddleDee(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)ActorMove;
        t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
        t->layer = 11;
    }
    {
        struct Task *t = gCurTask;

        t->frameTable = gWaddleDeeFrames;
        t->unk8C->extraFrame = 4;
        CallTableEntry(t->variant, 6, gWaddleDeeVariants);
    }
}

s32 sub_08078984(void)
{
    sub_08066c08(gUnk_08740BD4, 0);
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

s32 sub_080789ac(void)
{
    s32 r = 0;

    switch (gCurTask->variant)
    {
    case 0:
        ActorSetState(1);
        TaskSetEntry(WaddleDeeWalkEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 1:
        ActorSetState(1);
        TaskSetEntry(WaddleDeePaceEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 2:
        ActorSetState(2);
        TaskSetEntry(WaddleDeeJumpEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 3:
        ActorSetState(1);
        TaskSetEntry(ParasolWaddleDeeWalkEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 5:
        ActorSetState(1);
        TaskSetEntry(ParasolWaddleDeeStandEnterState, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 sub_08078a48(void)
{
    s32 r = 0;

    switch (gCurTask->variant)
    {
    case 0:
        ActorSetState(0);
        TaskSetEntry(WaddleDeeWalkEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 1:
        ActorSetState(0);
        TaskSetEntry(WaddleDeePaceEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 2:
        {
            struct Task *t = gCurTask;

            if (t->unk28 <= 0)
                t->unk28 = 30;
        }
        gCurTask->unk34 = ActorStartAnim(gUnk_087406A0);
        ActorSetState(0);
        TaskSetEntry(WaddleDeeJumpEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 3:
        sub_08066c3c(gUnk_08740BD4);
        ActorSetState(0);
        TaskSetEntry(ParasolWaddleDeeWalkEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 5:
        ActorSetState(0);
        TaskSetEntry(ParasolWaddleDeeStandEnterState, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 sub_08078b08(void)
{
    u8 v = gCurTask->variant;

    if (v == 3 || v == 5)
        sub_08066c08(gUnk_08740BD4, 0);
    sub_0806a0f0(-2);
    return 1;
}

s32 sub_08078b38(void)
{
    struct Task *t = gCurTask;

    if (t->variant == 3 && t->state == 1)
        sub_08066b70();
    else
        TaskTurnAroundAndReverseX();
    return 0;
}

s32 sub_08078b64(void)
{
    return 0;
}
