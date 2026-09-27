#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_56dd4.c (0x08056DD4-0x08057493, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 29-31.  Variant 29 (sub_08056dd4, M11/M13) has three sub-states
 * with random frames (RandomRange); its callback sub_0805707c sets
 * Task.unk28 once the player leaves mode 13 or its facing no longer matches
 * the spawner's, and kills it when the ability is no longer 1 or when
 * PlayerState.unk40 bit 8 is clear while the spawner's Task.unk7B bit 0 is
 * set.  Variant 30 (sub_0805710c, M11/M13) rides on its spawner with the
 * draw hook TaskDrawWorldLoadTiles (sub-state 0) or TaskDrawWorldTilesLoaded and re-rolls its
 * position every two frames from the {base, scale, amount} rows
 * gUnk_0873BA8C[][2][3]; its callback sub_080573a4 kills it when the player
 * leaves mode 13 or the spawner's Task.unk73 is not 1, and otherwise, while
 * Task.unk28 is clear, registers the collider row gUnk_0873C038 (M05's
 * RegisterCollider) and tests the block hit-box set gUnk_0873CC94 (M09's
 * TaskBreakBlocksAt) at the spawner's position.  Variant 31 (sub_08057430, M12)
 * is a single animation on its spawner (gUnk_08751CEC). */

extern u32 gUnk_08751CA4[];
extern u32 gUnk_08751CBC[];
extern s16 gUnk_0873BA8C[][2][3];   /* {base, scale, amount} rows for RandomSpreadFacing */
extern u32 gUnk_0873C038[];
extern u32 gUnk_0873CC94[];
extern u32 gUnk_08751CEC[];

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
u32 RandomRange(u32 range);                       /* RNG: 0 .. range-1 */
void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void TaskMove(void);
void TaskMoveRelativeToParent(void);
void TaskDrawWorld(void);
void TaskDrawWorldLoadTiles(void);
void TaskDrawWorldTilesLoaded(void);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskSetFrame(s32 a);
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 TaskBreakBlocksAt(struct HitBoxSet *p, s32 x, s32 y, s32 e);
void sub_0805707c(void);
void sub_080573a4(void);

void sub_08056dd4(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk04 = (u32)sub_0805707c;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_08751CA4;
    t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x1800) | 12;
    t->unk28 = 0;
    switch (t->unk18 & 15)
    {
    case 0:
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(12);
        do
        {
            gCurTask->unk4C = (RandomSpreadFacing(16, 1, 32) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(-8, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            TaskSetMotionXFacing(0x18000, -0x800);
            gCurTask->unk58 = 0;
            gCurTask->unk60 = -0x2000;
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
        } while (gCurTask->unk28 == 0);
        break;
    case 1:
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(8);
        do
        {
            gCurTask->unk4C = (RandomSpreadFacing(32, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(0, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            TaskSetMotionXFacing(0x10000, 0x4000);
            gCurTask->unk58 = 0;
            gCurTask->unk60 = (RandomRange(32) - 16) << 8;
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 == 0);
        break;
    case 2:
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(4);
        do
        {
            gCurTask->unk4C = (RandomSpreadFacing(20, 1, 12) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(0, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            TaskSetMotionXFacing(0x8000, 0x2000);
            gCurTask->unk58 = 0;
            gCurTask->unk60 = (RandomRange(32) - 16) << 8;
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 == 0);
        break;
    }
    TaskExitTrampoline();
}

void sub_0805707c(void)
{
    {
        struct Task *t = gCurTask;

        if (t->unk28 == 0 && (t->unk88->unk04 != 13 || t->unk43 != ((struct Task *)t->unk8C)->unk43))
            t->unk28 = 1;
    }
    {
        struct Task *t = gCurTask;

        if (!(t->unk88->unk40 & 0x100) && (((struct Task *)t->unk8C)->unk7B & 1))
            TaskFree(gCurTaskIdx);
    }
    if (gCurTask->unk88->unk0D != 1)
        TaskFree(gCurTaskIdx);
}

void sub_0805710c(void)
{
    struct Task *t;
    s16 *x;
    s16 *y;

    gCurTask->unk00 = (u32)TaskMoveRelativeToParent;
    gCurTask->unk04 = (u32)sub_080573a4;
    gCurTask->unk42 = 8;
    t = gCurTask;
    t->unk38 = gUnk_08751CBC;
    t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x800) | 12;
    if ((t->unk28 = t->unk18 & 15) == 0)
        t->unk0C = (u32)TaskDrawWorldLoadTiles;
    else
        t->unk0C = (u32)TaskDrawWorldTilesLoaded;
    x = gUnk_0873BA8C[gCurTask->unk28][0];
    y = gUnk_0873BA8C[gCurTask->unk28][1];
    for (;;)
    {
        gCurTask->unk4C = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->unk50 = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        TaskSetFrame(0);
        TaskYieldTrampoline(2);
        gCurTask->unk4C = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->unk50 = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(2);
        gCurTask->unk4C = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->unk50 = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(2);
        gCurTask->unk4C = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->unk50 = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        TaskSetFrame(8);
        TaskYieldTrampoline(2);
        gCurTask->unk4C = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->unk50 = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(2);
        gCurTask->unk4C = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->unk50 = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        TaskSetFrame(1);
        TaskYieldTrampoline(2);
        gCurTask->unk4C = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->unk50 = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(2);
        gCurTask->unk4C = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->unk50 = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(2);
        gCurTask->unk4C = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->unk50 = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(2);
        gCurTask->unk4C = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->unk50 = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(2);
        gCurTask->unk4C = RandomSpreadFacing(x[0], x[1], x[2]) << 16;
        gCurTask->unk50 = RandomSpreadFacing(y[0], y[1], y[2]) << 16;
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(2);
    }
}

void sub_080573a4(void)
{
    struct Task *t = gCurTask;
    struct Task *p;

    if (t->unk88->unk04 != 13 || (p = (struct Task *)t->unk8C)->unk73 != 1)
    {
        TaskFree(gCurTaskIdx);
    }
    else if (t->unk28 == 0)
    {
        RegisterCollider(gCurTaskIdx, p->unk48, p->unk4A, gUnk_0873C038);
        TaskBreakBlocksAt((struct HitBoxSet *)gUnk_0873CC94, ((struct Task *)gCurTask->unk8C)->unk48,
                     ((struct Task *)gCurTask->unk8C)->unk4A, gCurTask->unk44);
    }
}

void sub_08057430(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMoveRelativeToParent;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 5;
    gCurTask->unk38 = gUnk_08751CEC;
    gCurTask->unk4C = RandomSpreadFacing(-32, 1, 16) << 16;
    gCurTask->unk50 = RandomSpread(-4, 1, 16) << 16;
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(3);
    TaskExitTrampoline();
}
