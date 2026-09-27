#include "gba/gba.h"
#include "global.h"
#include "task.h"

struct M11R8 { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };
struct M11Buf { u8 unk00[4]; u8 unk04[4]; };
struct M11R20 { u32 w[5]; };

extern struct M11R8 gPlayerHitBoxSets[];
extern struct M11R20 gPlayerBodyBoxes[];
extern u16 gLatchedPressedKeys[];
extern u16 gLatchedHeldKeys[];
extern struct Task *gCurTask;
extern u32 gUnk_0873CA7C[];
extern u32 gUnk_0873CA90[];
extern u32 gUnk_0873CAA4[];
extern u32 gUnk_0873D044[];
extern u32 gUnk_0873D04C[];

void TaskYieldTrampoline(s32 frames);
s32 PlaySfx(s32 id);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskSetMotionY(s32 a, s32 b, s32 c);
void TaskSetFrame(s32 a);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);
void sub_080261d4(u32 a);
void PlayerStopAxes(s32 a0);
void PlayerStartSfx(s32 a, u16 b);
void PlayerStopSfx(void);
void PlayerSetWaterMotionY(void);
s32 PlayerLand(s32 a0);
s32 sub_0803e55c(void);
s32 LoadPlayerBodyBoxRect(s32 playerIdx, u8 *src6);
s32 LoadPlayerHitBoxSet(s32 a0, s32 a1);
void PlayerStartOffsetScript(s32 a0);
void PlayerTurnToHeldDirection(void);
s32 PlayerStopAtWall(void);
s32 PlayerCheckLanding(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
s32 PlayerRequestLocomotion(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);
void PlayerSetMotionYPreset(s32 a0);
void CreatePlayerObject(s32 a, s32 b, s32 c);
s32 CreatePlayerEffect(s32 band, s32 id, s32 payload);

void sub_08043654(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 0;
    gCurTask->unk15 = 21;
    if (gCurTask->unk88->unk05 != 0)
    {
        PlayerStopAxes(3);
        gCurTask->unk28 = ((u8 *)gCurTask->unk88)[74];
    }
    TaskSetFrame(0x11CB);
    TaskSleepForever();
}

void sub_080436ac(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 1;
    gCurTask->unk15 = 22;
    PlayerSetMotionXPreset(9, 72);
    while (1)
    {
        TaskSetFrame(0x11D5);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(12);
        gCurTask->unk3C++;
        TaskYieldTrampoline(7);
        gCurTask->unk3C++;
        TaskYieldTrampoline(7);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(12);
        gCurTask->unk3C++;
        TaskYieldTrampoline(7);
        gCurTask->unk3C++;
        TaskYieldTrampoline(7);
    }
}

void sub_0804374c(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 23;
Lloop:
    gCurTask->unk80 = 4;
    {
        struct Task *t = gCurTask;

        t->unk30 = 0;
        t->unk88->unk14 = 0;
        {
            struct M11R20 *d = (struct M11R20 *)gPlayerBodyBoxes;

            d[t->unk88->unk00] = *(struct M11R20 *)gUnk_0873CA90;
        }
    }
    {
        struct M11R8 *g8 = (struct M11R8 *)gPlayerHitBoxSets;

        {
            struct Task *t = gCurTask;

            g8[t->unk88->unk00] = *(struct M11R8 *)gUnk_0873D044;
            t->unk2C = -1;
            *(u32 *)((u8 *)t->unk88 + 108) = 0;
            t->unk73 = 0;
        }
        PlaySfx(266);
        TaskSetFrame(0x1212);
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk28 = t->unk7A;
            if (t->unk28 != 0)
            {
                CreatePlayerEffect(t->unk88->unk00, 28, 3);
                TaskSetMotionXFacing(0x10000, -0x1000);
                gCurTask->unk64 = 0;
            }
        }
        {
            struct Task *t = gCurTask;

            *(u32 *)((u8 *)t->unk88 + 108) = (u32)&g8[t->unk88->unk00];
            t->unk6C = 0;
        }
    }
    do
    {
        struct Task *t = gCurTask;

        LoadPlayerHitBoxSet(t->unk88->unk00,
                     (s32)((u8 *)gUnk_0873D04C + ++t->unk2C * 8));
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 1);
    {
        struct Task *t = gCurTask;

        t->unk88->unk14 = 8;
        t->unk6C = 0;
    }
    do
    {
        struct Task *t = gCurTask;

        LoadPlayerHitBoxSet(t->unk88->unk00,
                     (s32)((u8 *)gUnk_0873D04C + ++t->unk2C * 8));
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 5);
    if (gCurTask->unk28 != 0)
    {
        TaskSetMotionXFacing(0x4000, 0);
        gCurTask->unk64 = 0x4000;
    }
    {
        struct Task *t = gCurTask;

        t->unk2C = -1;
        *(u32 *)((u8 *)t->unk88 + 108) = 0;
        t->unk3C++;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        if (t->unk30 == 0)
            goto Lend;
        t->unk30 = 0;
        t->unk88->unk14 = 0;
        t->unk73 = 1;
    }
    if (gCurTask->unk28 != 0)
        PlayerStopAxes(1);
    PlaySfx(0x10B);
    TaskSetFrame(0x121B);
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->unk28 = t->unk7A;
        if (t->unk28 != 0)
        {
            TaskSetMotionXFacing(-0x800, 0x800);
            gCurTask->unk64 = 0;
        }
    }
    {
        struct Task *t = gCurTask;

        t->unk2C = 8;
        *(u32 *)((u8 *)t->unk88 + 108) =
            (u32)&((struct M11R8 *)gPlayerHitBoxSets)[t->unk88->unk00];
        t->unk6C = 0;
    }
    do
    {
        struct Task *t = gCurTask;

        LoadPlayerHitBoxSet(t->unk88->unk00,
                     (s32)((u8 *)gUnk_0873D04C + --t->unk2C * 8));
        gCurTask->unk3C--;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 1);
    {
        struct Task *t = gCurTask;

        t->unk88->unk14 = 8;
        t->unk6C = 0;
    }
    do
    {
        struct Task *t = gCurTask;

        LoadPlayerHitBoxSet(t->unk88->unk00,
                     (s32)((u8 *)gUnk_0873D04C + --t->unk2C * 8));
        gCurTask->unk3C--;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 5);
    if (gCurTask->unk28 != 0)
    {
        TaskSetMotionXFacing(-0x4000, 0);
        gCurTask->unk64 = 0x4000;
    }
    {
        struct Task *t = gCurTask;

        t->unk2C = -1;
        *(u32 *)((u8 *)t->unk88 + 108) = 0;
        t->unk3C--;
    }
    TaskYieldTrampoline(4);
    if (gCurTask->unk30 == 0)
        goto Lend;
    goto Lloop;
Lend:
    gCurTask->unk73 = 2;
    TaskSleepForever();
}


void sub_08043a88(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    struct Task *v;
    struct PlayerState *p;

    if (t->unk73 != 2) {
        p = t->unk88;
        if ((s16)p->unk14 != 0) {
            p->unk14--;
            {
                u16 *q = (u16 *)gLatchedPressedKeys;
                if (q[t->unk88->unk00] & 2)
                    t->unk30 = 1;
            }
        }
        u = gCurTask;
        if (u->unk2C != -1) {
            LoadPlayerBodyBoxRect(u->unk88->unk00, (u8 *)gUnk_0873CAA4 + u->unk2C * 8);
            RegisterCollider((u8)gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A,
                         (u8 *)gPlayerBodyBoxes + gCurTask->unk88->unk00 * 20);
        }
    } else {
        PlayerRequestLocomotion();
    }
    v = gCurTask;
    if ((v->unk7A & 1) == 0) {
        if ((v->unk7B & 1) == 0)
            PlayerSetMotionYPreset(2);
        else
            PlayerSetMotionYPreset(13);
    } else {
        PlayerLand(1);
    }
    PlayerStopAtWall();
}


/* NOTE: fwd.h gives this four s32 args; that is an arity.py artifact -
   the script does not model `ldmia rN!, {r2, r3, r4}` (the 20-byte struct
   copy in this body) as WRITING r2/r3, so it reads them as incoming
   arguments.  The real prototype is `void sub_08043b80(void)`, confirmed by
   the anchor table at 0x0873B46C whose entries are `void (*)(void)`.
   The unused parameters are kept only so this file compiles against the
   current fwd.h; they cost no code. */
void sub_08043b80(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 24;
    gCurTask->unk80 = 4;
    {
        struct Task *t = gCurTask;
        t->unk30 = 0;
        t->unk28 = 0;
        {
            struct M11R20 *d = (struct M11R20 *)gPlayerBodyBoxes;
            d[t->unk88->unk00] = *(struct M11R20 *)gUnk_0873CA90;
        }
    }
    {
        struct M11R8 *g8 = (struct M11R8 *)gPlayerHitBoxSets;
        {
            struct Task *t = gCurTask;
            g8[t->unk88->unk00] = *(struct M11R8 *)gUnk_0873D044;
            t->unk2C = -1;
            *(u32 *)((u8 *)t->unk88 + 108) = 0;
        }
        PlaySfx(266);
        gCurTask->unk5C = 0;
        TaskSetFrame(0x120C);
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);

        TaskSetMotionXFacing(0x60000, -0x4000);
        {
            struct Task *t = gCurTask;
            t->unk64 = 0;
            *(u32 *)((u8 *)t->unk88 + 108) = (u32)&g8[t->unk88->unk00];
            LoadPlayerHitBoxSet(t->unk88->unk00,
                         (s32)((u8 *)gUnk_0873D04C + (t->unk2C = 8) * 8));
        }
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);

        TaskSetMotionXFacing(0x20000, 0);
        {
            struct Task *t = gCurTask;
            t->unk64 = 0x20000;
            if ((t->unk7A & 1) != 0)
                CreatePlayerEffect(t->unk88->unk00, 28, 3);
        }
        {
            struct Task *t = gCurTask;
            t->unk88->unk14 = 8;
            LoadPlayerHitBoxSet(t->unk88->unk00,
                         (s32)((u8 *)gUnk_0873D04C + ++t->unk2C * 8));
        }
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        {
            struct Task *t = gCurTask;
            LoadPlayerHitBoxSet(t->unk88->unk00,
                         (s32)((u8 *)gUnk_0873D04C + ++t->unk2C * 8));
        }
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);

        TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
        {
            struct Task *t = gCurTask;
            t->unk2C = -1;
            *(u32 *)((u8 *)t->unk88 + 108) = 0;
            t->unk3C++;
        }
        TaskYieldTrampoline(10);
        PlayerStopAxes(1);

        if (gCurTask->unk30 == 0) {
            TaskYieldTrampoline(10);
        } else {
            PlaySfx(0x10B);
            TaskSetFrame(0x121B);
            TaskYieldTrampoline(4);
            TaskSetMotionXFacing(-0x800, 0x800);
            {
                struct Task *t = gCurTask;
                t->unk64 = 0;
                t->unk2C = 8;
                *(u32 *)((u8 *)t->unk88 + 108) = (u32)&g8[t->unk88->unk00];
                t->unk6C = 0;
            }
            do {
                struct Task *t = gCurTask;
                LoadPlayerHitBoxSet(t->unk88->unk00,
                             (s32)((u8 *)gUnk_0873D04C + --t->unk2C * 8));
                gCurTask->unk3C--;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 7);
            TaskSetMotionXFacing(-0x4000, 0);
            {
                struct Task *t = gCurTask;
                t->unk64 = 0x4000;
                t->unk2C = -1;
                *(u32 *)((u8 *)t->unk88 + 108) = 0;
                t->unk3C--;
            }
            TaskYieldTrampoline(4);
        }
    }
    gCurTask->unk28++;
    TaskSleepForever();
}


void sub_08043e28(void)
{
    if (gCurTask->unk28 != 0) {
        if ((gCurTask->unk7A & 1) == 0) {
            if ((gCurTask->unk7B & 1) == 0)
                gCurTask->unk88->unk01 = 7;
            else
                gCurTask->unk88->unk01 = 23;
        } else {
            u16 *q = (u16 *)gLatchedHeldKeys;
            if ((q[gCurTask->unk88->unk00] & 48) != 0) {
                PlayerTurnToHeldDirection();
                if ((gCurTask->unk7B & 1) == 0) {
                    *((u8 *)gCurTask->unk88 + 61) = 1;
                    gCurTask->unk88->unk01 = 3;
                } else {
                    *((u8 *)gCurTask->unk88 + 61) = 0;
                    gCurTask->unk88->unk01 = 23;
                }
            } else {
                *((u8 *)gCurTask->unk88 + 61) = 0;
                if ((gCurTask->unk7B & 1) == 0)
                    gCurTask->unk88->unk01 = 1;
                else
                    gCurTask->unk88->unk01 = 24;
            }
        }
    } else {
        if (gCurTask->unk2C != -1) {
            LoadPlayerBodyBoxRect(gCurTask->unk88->unk00,
                         (u8 *)gUnk_0873CAA4 + gCurTask->unk2C * 8);
            RegisterCollider((u8)gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A,
                         (u8 *)gPlayerBodyBoxes + gCurTask->unk88->unk00 * 20);
        }
        {
            struct Task *t = gCurTask;
            struct PlayerState *p = t->unk88;
            if ((s16)p->unk14 != 0) {
                p->unk14--;
                {
                    u16 *q = (u16 *)gLatchedPressedKeys;
                    if (q[t->unk88->unk00] & 2)
                        t->unk30 = 1;
                }
            }
        }
    }
    if ((gCurTask->unk7A & 1) == 0)
        PlayerSetMotionYPreset(2);
    else
        PlayerLand(1);
}


/* NOTE: fwd.h gives this four s32 args; that is an arity.py artifact -
   the script does not model `ldmia rN!, {r2, r3, r4}` (the 20-byte struct
   copy in this body) as WRITING r2/r3, so it reads them as incoming
   arguments.  The real prototype is `void sub_08043fa8(void)`, confirmed by
   the anchor table at 0x0873B46C whose entries are `void (*)(void)`.
   The unused parameters are kept only so this file compiles against the
   current fwd.h; they cost no code. */
void sub_08043fa8(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 25;
    gCurTask->unk80 = 4;
    {
        struct Task *t = gCurTask;
        t->unk28 = 0;
        {
            struct M11R20 *d = (struct M11R20 *)gPlayerBodyBoxes;
            d[t->unk88->unk00] = *(struct M11R20 *)gUnk_0873CA90;
        }
    }
    {
        struct M11R8 *g8 = (struct M11R8 *)gPlayerHitBoxSets;
        struct Task *t = gCurTask;
        g8[t->unk88->unk00] = *(struct M11R8 *)gUnk_0873D044;
        t->unk2C = -1;
        *(u32 *)((u8 *)t->unk88 + 108) = 0;
    }
    PlayerStopAxes(2);
    PlayerSetMotionXPreset(0, 72);
    PlayerStartOffsetScript(19);
    if ((gCurTask->unk7A & 1) != 0) {
        TaskSetFrame(0x121C);
        TaskYieldTrampoline(6);
    } else {
        TaskSetFrame(0x1223);
        TaskYieldTrampoline(6);
    }
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk7A = 0;
    PlaySfx(0x10B);
    TaskSetMotionY(-0x40000, 0x8000, 0x40000);
    TaskSetFrame(0x121E);
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    PlayerStopAxes(2);
    {
        struct Task *t = gCurTask;
        *(u32 *)((u8 *)t->unk88 + 108) =
            (u32)((u8 *)gPlayerHitBoxSets + t->unk88->unk00 * 8);
        LoadPlayerHitBoxSet(t->unk88->unk00,
                     (s32)((u8 *)gUnk_0873D04C + (t->unk2C = 11) * 8));
    }
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gCurTask;
        LoadPlayerHitBoxSet(t->unk88->unk00,
                     (s32)((u8 *)gUnk_0873D04C + ++t->unk2C * 8));
    }
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    {
        struct Task *t = gCurTask;
        t->unk2C = -1;
        *(u32 *)((u8 *)t->unk88 + 108) = 0;
        t->unk3C++;
    }
    TaskYieldTrampoline(5);
    TaskSetMotionY(0x40000, -0x8000, 0x40000);
    TaskSetFrame(0x121F);
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk28++;
    TaskSleepForever();
}


void sub_080441cc(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (t->unk28 != 0) {
        PlayerRequestLocomotion();
    } else if (t->unk2C != -1) {
        LoadPlayerBodyBoxRect(t->unk88->unk00, (u8 *)gUnk_0873CAA4 + t->unk2C * 8);
        RegisterCollider((u8)gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A,
                     (u8 *)gPlayerBodyBoxes + gCurTask->unk88->unk00 * 20);
    }
    u = gCurTask;
    if ((u->unk7A & 1) == 0) {
        if ((u->unk7B & 1) == 0)
            PlayerSetMotionYPreset(2);
        else
            PlayerSetMotionYPreset(13);
    } else {
        PlayerLand(1);
    }
    PlayerStopAtWall();
}


/* NOTE: fwd.h gives this four s32 args; that is an arity.py artifact -
   the script does not model `ldmia rN!, {r2, r3, r4}` (the 20-byte struct
   copy in this body) as WRITING r2/r3, so it reads them as incoming
   arguments.  The real prototype is `void sub_08044288(void)`, confirmed by
   the anchor table at 0x0873B46C whose entries are `void (*)(void)`.
   The unused parameters are kept only so this file compiles against the
   current fwd.h; they cost no code. */
void sub_08044288(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 26;
    {
        struct Task *t = gCurTask;
        if (t->unk88->unk05 != 13) {
            t->unk73 = 0;
            {
                struct M11R20 *d = (struct M11R20 *)gPlayerBodyBoxes;
                d[gCurTask->unk88->unk00] = *(struct M11R20 *)gUnk_0873CA90;
            }
            {
                struct M11R8 *g8 = (struct M11R8 *)gPlayerHitBoxSets;
                struct Task *u = gCurTask;
                g8[u->unk88->unk00] = *(struct M11R8 *)gUnk_0873D044;
                u->unk2C = -1;
                *(u32 *)((u8 *)u->unk88 + 108) = 0;
            }
            PlaySfx(0x10B);
            gCurTask->unk80 = 4;
        }
    }
    switch (gCurTask->unk73) {
    case 0:
        PlayerStopAxes(3);
        {
            struct Task *t = gCurTask;
            t->unk88->unk14 = 10;
            *(u32 *)((u8 *)t->unk88 + 108) =
                (u32)((u8 *)gPlayerHitBoxSets + t->unk88->unk00 * 8);
            LoadPlayerHitBoxSet(t->unk88->unk00,
                         (s32)((u8 *)gUnk_0873D04C + (t->unk2C = 13) * 8));
        }
        TaskSetFrame(0x1225);
        TaskYieldTrampoline(2);
        {
            struct Task *t = gCurTask;
            t->unk2C = -1;
            *(u32 *)((u8 *)t->unk88 + 108) = 0;
            t->unk3C++;
        }
        TaskYieldTrampoline(4);
        TaskSetMotionY(0x60000, 0, 0x60000);
        {
            struct Task *t = gCurTask;
            *(u32 *)((u8 *)t->unk88 + 108) =
                (u32)((u8 *)gPlayerHitBoxSets + t->unk88->unk00 * 8);
            LoadPlayerHitBoxSet(t->unk88->unk00,
                         (s32)((u8 *)gUnk_0873D04C + (t->unk2C = 14) * 8));
        }
        break;
    case 1:
        sub_080261d4(2);
        PlaySfx(0x10F);
        CreatePlayerEffect(gCurTask->unk88->unk00, 39, 0);
        CreatePlayerEffect(gCurTask->unk88->unk00, 39, 1);
        {
            struct Task *t = gCurTask;
            t->unk2C = -1;
            *(u32 *)((u8 *)t->unk88 + 108) = 0;
            RegisterCollider((u8)gCurTaskIdx, t->unk48, t->unk4A,
                         (u8 *)gUnk_0873CA7C);
        }
        TaskSetFrame(0x1227);
        TaskYieldTrampoline(8);
        gCurTask->unk73 = 2;
        break;
    }
    TaskSleepForever();
}


void sub_08044470(void)
{
    struct Task *t = gCurTask;

    switch (t->unk73) {
    case 0:
        if (PlayerCheckLanding() != 0) {
            PlayerStopAxes(3);
            gCurTask->unk73 = 1;
            TaskSetEntry(sub_08044288, gCurTaskIdx);
            break;
        }
        {
            struct Task *u = gCurTask;
            if (u->unk58 != 0) {
                struct PlayerState *p = u->unk88;
                if ((s16)p->unk14 == 0) {
                    u16 *q = (u16 *)gLatchedPressedKeys;
                    if ((q[p->unk00] & 2) != 0) {
                        if ((u->unk7B & 1) == 0) {
                            PlayerSetMotionYPreset(2);
                            gCurTask->unk88->unk01 = 7;
                            break;
                        } else {
                            PlayerSetWaterMotionY();
                            gCurTask->unk88->unk01 = 23;
                            break;
                        }
                    }
                } else {
                    p->unk14--;
                }
                {
                    u16 *q = (u16 *)gLatchedHeldKeys;
                    struct Task *v = gCurTask;
                    if ((q[v->unk88->unk00] & 48) != 0) {
                        if ((q[v->unk88->unk00] & 16) != 0)
                            v->unk5C = 0x2000;
                        else
                            v->unk5C = -0x2000;
                        v->unk64 = 0x1DE00;
                    } else if ((v->unk7B & 1) == 0) {
                        PlayerSetMotionXPreset(0, 72);
                    } else {
                        PlayerSetMotionXPreset(8, 72);
                    }
                }
            }
            if (PlayerHasCrossedWaterSurface(0) != 0) {
                gCurTask->unk88->unk01 = 23;
                break;
            }
            {
                struct Task *w = gCurTask;
                if (w->unk2C != -1) {
                    LoadPlayerBodyBoxRect(w->unk88->unk00,
                                 (u8 *)gUnk_0873CAA4 + w->unk2C * 8);
                    RegisterCollider((u8)gCurTaskIdx, gCurTask->unk48,
                                 gCurTask->unk4A,
                                 (u8 *)gPlayerBodyBoxes
                                     + gCurTask->unk88->unk00 * 20);
                }
            }
        }
        break;
    case 1:
        if ((t->unk7A & 1) != 0) {
            PlayerLand(1);
            break;
        }
        /* fallthrough */
    case 2:
        PlayerRequestLocomotion();
        break;
    }
    PlayerStopAtWall();
}


void sub_0804462c(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 29;
    {
        struct Task *t = gCurTask;
        if (t->unk88->unk05 != 13) {
            struct Task *u;
            t->unk73 = 0;
            u = gCurTask;
            u->unk28 = 15;
            u->unk80 = 1;
        }
    }
    switch (gCurTask->unk73) {
    case 0:
        TaskSetFrame(0x289);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(6);
        gCurTask->unk3C--;
        TaskYieldTrampoline(2);
        TaskSetFrame(652);
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk73 = 1;
        /* fallthrough */
    case 1:
        {
            struct PlayerState *p = gCurTask->unk88;
            if ((p->unk42 & 128) == 0)
                PlayerStartSfx(128, p->unk00);
        }
        CreatePlayerObject(gCurTask->unk88->unk00, 4, 0);
        CreatePlayerObject(gCurTask->unk88->unk00, 4, 1);
        CreatePlayerEffect(gCurTask->unk88->unk00, 29, 0);
        CreatePlayerEffect(gCurTask->unk88->unk00, 29, 1);
        CreatePlayerEffect(gCurTask->unk88->unk00, 29, 2);
        CreatePlayerEffect(gCurTask->unk88->unk00, 28, 4);
        while (1) {
            TaskSetFrame(0x279);
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 14);
        }
    case 2:
        TaskSetFrame(0x28D);
        TaskYieldTrampoline(3);
        PlayerStopSfx();
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk73 = 3;
        break;
    }
    TaskSleepForever();
}


void sub_08044800(void)
{
    struct Task *t = gCurTask;

    switch (t->unk73) {
    case 0:
        break;
    case 1:
        if (t->unk28 == 0) {
            u16 *p = (u16 *)gLatchedHeldKeys;
            if ((p[t->unk88->unk00] & 2) == 0) {
                t->unk73 = 2;
                TaskSetEntry(sub_0804462c, gCurTaskIdx);
            }
        } else {
            t->unk28--;
        }
        break;
    case 2:
        break;
    case 3:
        PlayerRequestLocomotion();
        break;
    }
    sub_0803e55c();
}


void sub_08044878(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 30;
    {
        struct Task *t = gCurTask;
        if (t->unk88->unk05 != 13) {
            struct Task *u;
            t->unk73 = 0;
            u = gCurTask;
            u->unk28 = 15;
            u->unk80 = 2;
        }
    }
    switch (gCurTask->unk73) {
    case 0:
        TaskSetFrame(0x36A);
        TaskYieldTrampoline(4);
        gCurTask->unk73 = 1;
        /* fallthrough */
    case 1:
        {
            struct Task *t = gCurTask;
            t->unk70 = 0;
            t->unk6E = 0;
            CreatePlayerEffect(t->unk88->unk00, 30, 0);
        }
        CreatePlayerEffect(gCurTask->unk88->unk00, 30, 1);
        CreatePlayerEffect(gCurTask->unk88->unk00, 30, 2);
        PlayerStartSfx(136, gCurTask->unk88->unk00);
        while (1) {
            TaskSetFrame(0x36B);
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 6);
        }
    case 2:
        gCurTask->unk88->unk42 &= 0xFFEF;
        PlayerStopSfx();
        TaskSetFrame(0x36A);
        TaskYieldTrampoline(2);
        gCurTask->unk73 = 3;
        break;
    }
    TaskSleepForever();
}
