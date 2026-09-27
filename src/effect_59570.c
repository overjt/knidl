#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_59570.c (0x08059570-0x0805A357, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 42-44, spawned by M13's actions (44 also by M14's action 49
 * sub-actions).  Variant 42 (sub_08059570) is a six-way jump table over its
 * sub-state, mixing forms that stay put and forms that move (gUnk_0874C804,
 * gUnk_0874C828, gUnk_08751F0C); its callbacks are sub_08059aac (the
 * collider rows gUnk_0873C23C and gUnk_0873C250 through RegisterCollider in
 * sub-states 3 and 4) and the draw hook sub_08059b18 (QueueSprite when on
 * screen).  Variant 43 (sub_08059c28) has four sub-states with the draw
 * hooks TaskDrawWorldTilesLoaded and M11's sub_0803dfc8.  Variant 44 (sub_08059d7c)
 * selects on the second byte of Task.unk18 (0x100-0x500), queues a VRAM
 * transfer (RequestCopy) and installs sub_0805a320, which kills it when the
 * spawner's Task.unk73 is 8 or the player is in neither mode 13 nor mode 3. */

extern u32 gUnk_08751F0C[];
extern u32 gUnk_0874C804[];
extern u32 gUnk_0874C828[];
extern u32 gUnk_0873C23C[];
extern u32 gUnk_0873C250[];
extern s16 gSpriteCameraX;               /* scalar, read with ldrsh (33 landed files) */
extern s16 gSpriteCameraY;               /* scalar, read with ldrsh (32 landed files) */
extern u32 gUnk_08751ECC[];
extern u32 gUnk_08751F84[];
extern u8 gUnk_081FD870[];
extern u16 gUnk_0873BB0E[][3];   /* per sub-state: 8.8 x velocity, 8.8 y velocity, frame */

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, void *src, void *dst, u32 size);   /* early_1518; effect_5afac's pointer spelling */
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);   /* callers pass f sign-extended (lsls/asrs #16); the early_1518 definition says u16 */
void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void TaskMove(void);
void TaskMoveRelativeToParent(void);
void TaskUpdatePixelPos(void);
void TaskDrawWorld(void);
void TaskDrawWorldTilesLoaded(void);
void TaskSetMotionXFacing(s32 a, s32 b);
u32 IsWorldPosOnScreen(s16 a, s16 b);   /* the ROM tests r0 unnarrowed (src callers spell u32) */
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.facing != 1 */
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_0803dfc8(void);
void sub_08059aac(void);
void sub_08059b18(void);
void sub_0805a320(void);

void sub_08059570(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk38 = gUnk_08751F0C;
    switch (gCurTask->unk18 & 15)
    {
    case 0:
        gCurTask->layer = 8;
        while (1)
        {
            u = gCurTask;
            u->unk54 = -((struct Task *)u->unk8C)->unk54;
            u->unk64 = 0x40000;
            u->unk58 = 0x20000;
            u->unk68 = 0x20000;
            u->posX = ((struct Task *)u->unk8C)->unk48 << 16;
            u->posY = (((struct Task *)u->unk8C)->unk4A + 16) << 16;
            u->frame = 16;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            if (gCurTask->unk88->unk3F != 3 || gCurTask->unk88->unk04 != 13)
                break;
        }
        break;
    case 1:
        gCurTask->layer = 5;
        while (1)
        {
            v = gCurTask;
            v->unk54 = -((struct Task *)v->unk8C)->unk54;
            v->unk64 = 0x40000;
            v->unk58 = 0x20000;
            v->unk68 = 0x20000;
            gCurTask->posX = (RandomSpread(-8, 1, 16) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->posY = (RandomSpread(-8, 1, 16) + ((struct Task *)gCurTask->unk8C)->unk4A + 16) << 16;
            gCurTask->frame = 26;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            if (gCurTask->unk88->unk3F != 3 || gCurTask->unk88->unk04 != 13)
                break;
        }
        break;
    case 2:
        gCurTask->layer = 5;
        while (1)
        {
            w = gCurTask;
            w->unk54 = -((struct Task *)w->unk8C)->unk54;
            w->unk64 = 0x40000;
            w->unk58 = 0x20000;
            w->unk68 = 0x20000;
            gCurTask->posX = (RandomSpread(-8, 1, 16) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->posY = (RandomSpread(-8, 1, 16) + ((struct Task *)gCurTask->unk8C)->unk4A + 16) << 16;
            gCurTask->frame = 22;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            if (gCurTask->unk88->unk3F != 3 || gCurTask->unk88->unk04 != 13)
                break;
        }
        break;
    case 3:
        gCurTask->unk00 = (u32)TaskUpdatePixelPos;
        gCurTask->unk04 = (u32)sub_08059aac;
        gCurTask->layer = 8;
        gCurTask->unk38 = gUnk_0874C804;
        gCurTask->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->unk04 = 0;
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        TaskExitTrampoline();
    case 4:
        gCurTask->unk00 = (u32)TaskUpdatePixelPos;
        gCurTask->unk0C = (u32)TaskDrawWorld;
        gCurTask->unk04 = (u32)sub_08059aac;
        gCurTask->layer = 5;
        t = gCurTask;
        t->unk38 = gUnk_0874C828;
        t->posY = (t->unk4A + 1) << 16;
        t->frame = 0xFFFF;
        TaskYieldTrampoline(4);
        gCurTask->unk04 = 0;
        gCurTask->frame = 16;
        TaskYieldTrampoline(1);
        gCurTask->frame = 24;
        TaskYieldTrampoline(1);
        gCurTask->frame = 17;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        TaskExitTrampoline();
    case 5:
        gCurTask->unk00 = (u32)TaskMove;
        gCurTask->unk0C = (u32)sub_08059b18;
        gCurTask->layer = 5;
        t = gCurTask;
        t->unk28 = t->unk48;
        t->unk2C = t->unk4A;
        t->posX = 0;
        t->posY = 0x30000;
        t->unk54 = -0x40000;
        t->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->unk54 = -0x10000;
        gCurTask->unk5C = 0x800;
        gCurTask->frame += 2;
        TaskYieldTrampoline(5);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(3);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 2;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 1);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 0;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 1);
        break;
    }
    TaskExitTrampoline();
}

void sub_08059aac(void)
{
    struct Task *t = gCurTask;

    switch (t->unk18 & 15)
    {
    case 3:
        RegisterCollider((u8)gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873C23C);
        break;
    case 4:
        RegisterCollider((u8)gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873C250);
        break;
    }
}

void sub_08059b18(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (t->frame != -1)
    {
        if (IsWorldPosOnScreen((s16)(t->unk28 + t->unk48), (s16)(t->unk2C + t->unk4A)) != 0)
        {
            u = gCurTask;
            QueueSprite(u->layer, gUnk_0874C828[u->frame], 0, 0,
                         u->unk28 + u->unk48 - gSpriteCameraX,
                         (s16)(u->unk2C + u->unk4A - gSpriteCameraY));
        }
        t = gCurTask;
        if (IsWorldPosOnScreen((s16)(t->unk28 - t->unk48), (s16)(t->unk2C + t->unk4A)) != 0)
        {
            u = gCurTask;
            QueueSprite(u->layer, gUnk_0874C828[(s16)(u->frame | 1)], 0, 0,
                         u->unk28 - u->unk48 - gSpriteCameraX,
                         (s16)(u->unk2C + u->unk4A - gSpriteCameraY));
        }
    }
}

void sub_08059c28(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->layer = 5;
    t = gCurTask;
    t->unk38 = gUnk_08751ECC;
    t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x800) | 12;
    t->unk3E = 0;
    gCurTask->posX = (RandomSpreadFacing(-4, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
    gCurTask->posY = (RandomSpread(-4, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
    u = gCurTask;
    switch (u->unk18 & 15)
    {
    case 0:
        u->unk0C = (u32)sub_0803dfc8;
        u->unk54 = -0x18000;
        u->unk58 = -0x18000;
        u->frame = 0;
        break;
    case 1:
        u->unk0C = (u32)TaskDrawWorldTilesLoaded;
        u->unk54 = 0x18000;
        u->unk58 = -0x18000;
        u->frame = 4;
        break;
    case 2:
        u->unk0C = (u32)TaskDrawWorldTilesLoaded;
        u->unk54 = -0x18000;
        u->unk58 = 0x18000;
        u->frame = 8;
        break;
    case 3:
        u->unk0C = (u32)TaskDrawWorldTilesLoaded;
        u->unk54 = 0x18000;
        u->unk58 = 0x18000;
        u->frame = 11;
        break;
    }
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->unk54 = 0;
    gCurTask->unk58 = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_08059d7c(void)
{
    struct Task *t = gCurTask;

    switch (t->unk18 & 0xFF00)
    {
    case 0x100:
        t->unk00 = (u32)TaskMoveRelativeToParent;
        t->unk0C = (u32)sub_0803dfc8;
        t->unk04 = (u32)sub_0805a320;
        t->layer = 5;
        {
            struct Task *u = gCurTask;

            u->unk38 = gUnk_08751F84;
            u->unk40 = (((struct Task *)u->unk8C)->unk40 + 0x1800) | 12;
            u->posX = 0;
            u->posY = 0;
        }
        for (;;)
        {
            while (gCurTask->unk88->unk3F == 3)
            {
                gCurTask->frame = 0;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame = 0xFFFF;
                TaskYieldTrampoline(4);
            }
            gCurTask->frame = 0xFFFF;
            while (gCurTask->unk88->unk3F != 3)
                TaskYieldTrampoline(1);
        }
    case 0x200:
        gCurTask->unk00 = (u32)TaskUpdatePixelPos;
        gCurTask->unk0C = (u32)TaskDrawWorld;
        gCurTask->unk04 = (u32)sub_0805a320;
        gCurTask->layer = 8;
        gCurTask->unk38 = gUnk_08751F0C;
        for (;;)
        {
            while (abs(((struct Task *)gCurTask->unk8C)->unk58) > 0x2FFFF)
            {
                gCurTask->posX = (RandomSpread(-8, 1, 16) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
                gCurTask->posY = (RandomSpread(-8, 1, 16) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
                gCurTask->frame = 26;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
            }
            gCurTask->frame = 0xFFFF;
            while (abs(((struct Task *)gCurTask->unk8C)->unk58) <= 0x2FFFF)
                TaskYieldTrampoline(1);
        }
    case 0x300:
        gCurTask->unk00 = (u32)TaskUpdatePixelPos;
        gCurTask->unk0C = (u32)TaskDrawWorld;
        gCurTask->unk04 = (u32)sub_0805a320;
        gCurTask->layer = 5;
        gCurTask->unk38 = gUnk_08751F0C;
        for (;;)
        {
            while (abs(((struct Task *)gCurTask->unk8C)->unk58) > 0x2FFFF)
            {
                gCurTask->posX = (RandomSpread(-8, 1, 16) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
                gCurTask->posY = (RandomSpread(-8, 1, 16) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
                gCurTask->frame = 22;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            }
            gCurTask->frame = 0xFFFF;
            while (abs(((struct Task *)gCurTask->unk8C)->unk58) <= 0x2FFFF)
                TaskYieldTrampoline(1);
        }
    case 0x400:
        gCurTask->unk00 = (u32)TaskUpdatePixelPos;
        gCurTask->unk0C = (u32)TaskDrawWorld;
        gCurTask->unk04 = (u32)sub_0805a320;
        gCurTask->layer = 8;
        gCurTask->unk38 = gUnk_08751F0C;
        for (;;)
        {
            while (abs(((struct Task *)gCurTask->unk8C)->unk58) > 0x37FFF)
            {
                gCurTask->posX = ((struct Task *)gCurTask->unk8C)->unk48 << 16;
                gCurTask->posY = ((struct Task *)gCurTask->unk8C)->unk4A << 16;
                gCurTask->frame = 16;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            }
            gCurTask->frame = 0xFFFF;
            while (abs(((struct Task *)gCurTask->unk8C)->unk58) <= 0x37FFF)
                TaskYieldTrampoline(1);
        }
    case 0x500:
        gCurTask->unk00 = (u32)TaskMove;
        gCurTask->unk0C = (u32)TaskDrawWorld;
        gCurTask->layer = 5;
        {
            struct Task *u = gCurTask;
            u32 off;
            u8 *src;

            u->unk38 = gUnk_08751F0C;
            u->unk40 = ((struct Task *)u->unk8C)->unk40 | 0xF008;
            off = (((struct Task *)u->unk8C)->unk40 & 0x7FF) << 5;
            src = gUnk_081FD870;
            RequestCopy(1, src, (void *)(off + 0x06010100), 128);
            RequestCopy(1, src + 128, (void *)(off + 0x06010500), 128);
            RequestCopy(1, src + 256, (void *)(off + 0x06010900), 128);
            RequestCopy(1, src + 384, (void *)(off + 0x06010D00), 128);
        }
        gCurTask->posX = (gCurTask->unk48 + RandomSpreadFacing(0, 1, 8)) << 16;
        gCurTask->posY = (gCurTask->unk4A + RandomSpread(0, 1, 8)) << 16;
        {
            u16 *row = gUnk_0873BB0E[gCurTask->unk18 & 15];

            {
                s32 a = row[0];
                s32 b = a << 8;

                if (a & 0x8000)
                    b |= 0xFF000000;
                TaskSetMotionXFacing(b, 0x5A5A5A5A);
            }
            {
                struct Task *w = gCurTask;
                s32 a = row[1];
                s32 b = a << 8;

                if (a & 0x8000)
                    b |= 0xFF000000;
                w->unk58 = b;
                w->frame = row[2];
            }
        }
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->unk54 = 0;
        gCurTask->unk58 = 0;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        TaskExitTrampoline();
        break;
    }
    TaskExitTrampoline();
}

void sub_0805a320(void)
{
    struct Task *t = gCurTask;

    if (((struct Task *)t->unk8C)->unk73 == 8 || (t->unk88->unk04 != 13 && t->unk88->unk04 != 3))
        TaskFree(gCurTaskIdx);
}
