/* game_code_and_rodata 0x080763E8-0x08077AE0 (issue #79, module M19 batch 4).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080763E8 0x08077AE0 src/actor_763e8.c --newpb
 *
 * M19 batch 4: task type #99's cutscene director tail plus type #75
 * (Task_Cannon) and type #76 (Task_CannonFuse) - the ending-pose and
 * script-walker families over struct M19Script.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "hud.h"
#include "cutscene.h"
#include "collision.h"
#include "room.h"
#include "actor.h"

/* callees */
extern s32 PlaySfx(u32 a);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern s32 sub_080269d8();
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskSetEntry(void *a, u32 i);
extern void TerrainCollideBox(u32 *p);
extern void RequestScreenShake(u32 a);
extern void ActorSetState(u8 v);
extern void AngleToVector(s16 t, s16 mag);

void sub_080763e8(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 <= 0)
    {
        if (t->unk24 != 0)
        {
            if (t->unk2C > 0)
                t->unk2C--;
        }
        else
        {
            if (t->unk2C <= 3)
                t->unk2C++;
        }
        if (gMetaKnightmareMode == 0)
            gCurTask->frame = gUnk_0874009C[gCurTask->unk2C];
        else
            gCurTask->frame = gUnk_087400A6[gCurTask->unk2C];
        gCurTask->unk28 = 1;
    }
    gCurTask->unk28--;
}

void sub_08076454(void)
{
    struct Task *t = gCurTask;

    t->posX = 0;
    t->posY = -0x180000;
    t->unk18 = 0;
    t->unk28 = 1;
    t->unk24 = 1;
    t->unk2C = 2;
    if (gUnk_0300244C != 0 && gMetaKnightmareMode == 1)
        t->frame = 0x1265;
    else
        t->frame = 0x11C7;
    {
        u16 id = gLocalPlayer;
        struct Task *u = gCurTask;

        if (id == u->player->playerIndex)
        {
            s32 x = u->posX + gTasks[u->parent].posX;
            s32 y = u->posY + gTasks[u->parent].posY;

            SetCameraFocus(x >> 16, y >> 16);
        }
    }
}

void sub_080764f8(void)
{
    vu16 *p = gPlayerPressedKeys;
    struct Task *t = gCurTask;

    if (p[t->player->playerIndex] & 1)
    {
        sub_08076f50(gCurTaskIdx);
    }
    else
    {
        if (gPlayerHeldKeys[t->player->playerIndex] & 0x80)
            t->unk24 = 0;
        else
            t->unk24 = 1;
        sub_080763e8();
    }
}

void sub_08076558(u16 a)
{
    struct Task *t = gCurTask;

    switch (t->unk30)
    {
    case 0:
        if (t->unk34 == 2 || t->unk34 == 4)
            a = a + 8;
        break;
    case 1:
        a = a + 8;
        break;
    case 2:
    case 3:
        a = a + 8;
        if (t->unk34 == 4)
            a = a + 16;
        break;
    }
    if (gCurTask->unk30 & 1)
        gCurTask->facing = 255;
    else
        gCurTask->facing = 1;
    AngleToVector((s16)a, 1024);
    TaskSetMotionXFacing(gUnk_030023B4, 0x5A5A5A5A);
    gCurTask->velY = gUnk_030023D4;
}

void sub_080765f4(u16 a)
{
    struct Task *t = gCurTask;

    switch (t->unk30)
    {
    case 0:
        if (t->unk34 == 2 || t->unk34 == 4)
            a = a + 8;
        break;
    case 1:
        a = a - 8;
        break;
    case 2:
    case 3:
        if (t->unk30 & 1)
            a = a - 8;
        else
            a = a + 8;
        if (gCurTask->unk34 == 4)
        {
            if (gCurTask->unk30 & 1)
                a = a - 16;
            else
                a = a + 16;
        }
        break;
    }
    if (gCurTask->unk30 & 1)
        gCurTask->facing = 255;
    else
        gCurTask->facing = 1;
    AngleToVector((s16)a, 1024);
    {
        struct Task *u = gCurTask;

        u->velX = gUnk_030023B4;
        u->velY = gUnk_030023D4;
    }
}

void sub_080766ac(void)
{
    {
        struct Task *t = gCurTask;

        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        t->frame = 0xFFFF;
    }
    TaskYieldTrampoline(25);
    sub_08076558(384);
    sub_0807029c();
    TaskGetScreenPos();
    if (gUnk_030023D4 > 63)
        gCurTask->unk18 = 1;
    {
        struct Task *t = gCurTask;

        t->unk24 = 0;
        t->unk70 = 8;
    }
}

void sub_08076710(void)
{
    {
        struct Task *t = gCurTask;

        t->pixelX += 13;
        t->pixelY += 8;
        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        t->facing = 1;
    }
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(25);
    sub_080765f4(448);
    sub_0807029c();
    TaskGetScreenPos();
    if (gUnk_030023B4 > 95 || gUnk_030023D4 > 63)
        gCurTask->unk18 = 1;
    {
        struct Task *t = gCurTask;

        t->unk24 = 0;
        t->unk70 = 8;
    }
}

void sub_08076798(void)
{
    sub_08076558(384);
    {
        struct Task *t = gCurTask;

        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        if (gMetaKnightmareMode == 0)
            TaskSetFrame(gUnk_0873D384[t->player->ability]);
        else
            TaskSetFrame(0x11E4);
    }
    TaskGetScreenPos();
    if (gUnk_030023D4 > 63)
        gCurTask->unk18 = 1;
    {
        struct Task *t = gCurTask;

        t->unk24 = 0;
        t->unk2C = 10;
    }
}

void sub_08076828(void)
{
    TaskGetScreenPos();
    if (gCurTask->unk24 == 0 && gUnk_030023D4 <= 0)
    {
        sub_0802610c();
        {
            struct Task *t = gCurTask;

            t->unk18 = 0;
            t->unk24 = 1;
        }
    }
}

void sub_0807685c(s32 a)
{
    struct Task *t = gCurTask;

    t->unk1C = t->pixelX;
    t->unk20 = t->pixelY;
    if (t->frame != -1)
    {
        if (gMetaKnightmareMode == 0)
            sub_08070334();
        {
            struct Task *u = gCurTask;

            if ((s16)u->unk70 <= 0)
            {
                u->unk70 = 8;
                CreateCannonSmoke(0, a);
            }
        }
        gCurTask->unk70--;
        sub_08076828();
    }
}

void sub_080768c8(void)
{
    gCurTask->facing = 1;
    gCurTask->unk18 = 1;
    TaskSetMotionY(0x18000, 0x8000, 0x40000);
    {
        struct Task *t = gCurTask;
        u16 x;

        t->spriteFlags |= 0x8000;
        t->unk24 = 0;
        t->unk34 = 1;
        x = t->posX >> 16;
        switch (t->player->playerIndex)
        {
        case 0:
            break;
        case 3:
            x = ((x << 16) + 0x80000) >> 16;
        case 2:
            x = ((x << 16) + 0x80000) >> 16;
        case 1:
            x = ((x << 16) + 0x80000) >> 16;
            break;
        }
        gCurTask->posX = x << 16;
    }
    DisablePause();
}

void sub_08076958(void)
{
    u8 k = gCurTask->player->ability;

    if (k == 1 || k == 2 || (s8)k == 4 || (s8)k == 5 || (s8)k == 9
     || (s8)k == 10 || (s8)k == 15 || (s8)k == 16 || (s8)k == 17
     || (s8)k == 19 || (s8)k == 22 || (s8)k == 23)
    {
        s32 v = (u16)gCurTask->frame << 16;

    loop:
        TaskSetFrame(v >> 16);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        goto loop;
    }
    TaskSleepForever();
}

void sub_080769d0(void)
{
    while (1)
    {
        TaskSetFrame(0x124D);
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 6);
    }
}

void sub_08076a14(void)
{
    while (1)
    {
        TaskSetFrame(0x124D);
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 6);
    }
}

void sub_08076a58(void)
{
    TaskSetMotionXFacing(0x40000, -0x10000);
    TaskSetFrame(0x1255);
    TaskYieldTrampoline(4);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0x10000, 0);
    {
        struct Task *t = gCurTask;

        t->velY = -0x49800;
        t->accelY = 0x3800;
        t->unk6C = 0;
    }
    do
    {
        TaskSetFrame(0x1256);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskSetMotionXFacing(0x50000, -0x20000);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(-0x30000, 0x20000);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(0x50000, -0x20000);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(-0x30000, 0x20000);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(0x30000, 0);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    if (gCurTask->unk24 == 1)
    {
        do
        {
            TaskYieldTrampoline(1);
        } while (gCurTask->unk24 == 1);
    }
    TaskYieldTrampoline(3);
    sub_08070614(gCurTaskIdx);
    TaskSleepForever();
}

void sub_08076c00(void)
{
    struct Task *t = gCurTask;

    if (t->velY != 0 && (t->onGround & 1))
    {
        t->unk24++;
        TaskStop();
        gCurTask->onGround = 0;
        {
            struct Task *u = gCurTask;

            switch (u->unk24)
            {
            case 1:
                PlaySfx(153);
                RequestScreenShake(4);
                CreateBurstEffect(0, 0);
                {
                    struct Task *v = gCurTask;

                    v->frame = 0x1255;
                    v->state = 5;
                }
                TaskSetEntry(sub_080763c4, gCurTaskIdx);
                break;
            case 2:
                u->frame = 0x1264;
                break;
            }
        }
    }
}

void sub_08076c88(void)
{
    gCurTask->updateState = 0;
    gCurTask->moveCallback = (u32)TaskMoveRelativeToParent;
    sub_08076454();
    TaskSleepForever();
}

void sub_08076cac(void)
{
    struct Task *t = gCurTask;

    t->unk1C = t->pixelX;
    t->unk20 = t->pixelY;
    sub_080764f8();
}

void sub_08076cd4(void)
{
    gCurTask->updateState = 1;
    sub_080766ac();
    if (gMetaKnightmareMode == 1)
        sub_080769d0();
    TaskSleepForever();
}

void sub_08076d00(void)
{
    sub_0807685c(0);
}

void sub_08076d0c(void)
{
    gCurTask->updateState = 2;
    sub_08076710();
    if (gMetaKnightmareMode == 1)
        sub_080769d0();
    TaskSleepForever();
}

void sub_08076d38(void)
{
    sub_0807685c(1);
}

void sub_08076d44(void)
{
    gCurTask->updateState = 3;
    sub_08076798();
    if (gMetaKnightmareMode == 0)
        sub_08076958();
    else
        TaskSleepForever();
}

void sub_08076d70(void)
{
    struct Task *t = gCurTask;

    t->unk1C = t->pixelX;
    t->unk20 = t->pixelY;
    if (t->unk2C <= 0)
        sub_08076f04(gCurTaskIdx);
    else
        t->unk2C--;
}

void sub_08076dac(void)
{
    gCurTask->updateState = 4;
    sub_080768c8();
    if (gMetaKnightmareMode == 0)
    {
        sub_08070264();
        sub_0807022c();
    }
    else
    {
        sub_08076a14();
    }
}

void sub_08076ddc(void)
{
    TerrainCollideBox(gUnk_0873F5E4);
    if (gMetaKnightmareMode == 0)
    {
        sub_080703a8();
        if (gCurTask->unk24 == 0)
            sub_080702d8();
    }
    else
    {
        sub_08076c00();
    }
    {
        struct Task *t = gCurTask;

        t->unk1C = t->pixelX;
        t->unk20 = t->pixelY;
    }
}

void sub_08076e30(void)
{
    gCurTask->updateState = 5;
    sub_08076a58();
}

void sub_08076e48(void)
{
    if ((gCurTask->onGround & 1) == 0)
        TerrainCollideBox(gUnk_0873F5E4);
    sub_08076c00();
    {
        struct Task *t = gCurTask;

        t->unk1C = t->pixelX;
        t->unk20 = t->pixelY;
    }
}

void sub_08076e88(s32 id, s32 v)
{
    struct Task *t = &gTasks[id];

    sub_08068a8c(id, 1);
    t->state = 0;
    t->parent = v;
    t->waterFlags = 0;
    TaskSetEntry(sub_08076318, id);
}

void sub_08076ec8(s32 id)
{
    struct Task *u = &gTasks[id];
    struct Task *t = &gTasks[u->parent];
    t->unk18 = 60;
    t->unk1C--;
    t->unk20 &= ~(1 << gCurTaskIdx);
}

void sub_08076f04(s32 id)
{
    struct Task *t = &gTasks[id];
    struct PlayerState *p = &gPlayerStates[id];

    sub_08076ec8(id);
    t->onGround = 0;
    TaskStopY();
    PlayerResumeControl(id, 0, 1, 0);
    HudShowAbility(p->ability, id);
}

void sub_08076f50(s32 id)
{
    struct Task *t = &gTasks[id];
    struct PlayerState *p = &gPlayerStates[id];
    struct Task *u = &gTasks[t->parent];

    sub_08076ec8(id);
    t->facing = 1;
    t->onGround = 0;
    {
        struct Task *v = gCurTask;

        t->posX = v->pixelX << 16;
        t->posY = v->pixelY << 16;
    }
    TaskStopSlot(id);
    PlayerResumeControl(id, 6, 1, 0);
    p->unk14 = 8;
    HudShowAbility(p->ability, id);
    if (u->unk1C > 0)
        DisablePause();
}

void sub_08076fe4(int a)
{
    u16 v = a;
    s32 i = 0;
    s32 n = 0;

    for (; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            struct Task *t = &gTasks[i];

            if ((gCurTask->unk20 >> i) & 1)
            {
                t->unk30 = n;
                n++;
                t->unk34 = gCurTask->unk1C;
                t->state = v;
                TaskSetEntry(sub_080763c4, i);
            }
        }
    }
}

void Task_Cannon(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)ActorMove;
        t->drawCallback = (u32)ActorDrawWorldInView;
        t->layer = 13;
    }
    {
        struct Task *t = gCurTask;

        t->frameTable = gCannonFrames;
        t->unk18 = 0;
        CallTableEntry(t->variant, 1, gCannonVariants);
    }
}

u16 sub_080770a0(void)
{
    s32 i = gLevelIndex * 2 + gCurTask->unk74;
    u16 v;

    if (i > 13)
        i = 0;
    v = gUnk_087400E4[i];
    switch (v)
    {
    case 1:
        ActorSetState(2);
        break;
    case 2:
        ActorSetState(3);
        break;
    }
    return v;
}

void sub_080770f0(s32 id)
{
    if (gUnk_0300244C == 0 || gPlayerHealth[id] > 0)
    {
        struct Task *t = &gTasks[id];
        struct PlayerState *p = &gPlayerStates[id];

        if (p->mouthState != 1 && t->velY > 0)
        {
            sub_08076e88(id, gCurTaskIdx);
            {
                struct Task *u = gCurTask;

                u->unk1C++;
                u->unk18 = 0;
                u->unk20 |= 1 << id;
            }
        }
        else
        {
            gCurTask->hitTimer = 0;
        }
    }
}

void sub_0807717c(void)
{
    if (gCurTask->unk1C == 0)
        CreateCannonSmoke(1, 0);
    CreateCannonSmoke(2, 2);
    RequestScreenShake(4);
    PlaySfx(296);
}

void sub_080771b0(void)
{
    CreateCannonSmoke(2, 2);
    RequestScreenShake(4);
}

void sub_080771c4(void)
{
    CreateCannonSmoke(3, 3);
    RequestScreenShake(4);
}

void sub_080771d8(void)
{
    if (gCurTask->unk1C == gActivePlayerCount)
        sub_08076fe4((s16)sub_080770a0());
    else
    {
        sub_08076fe4(3);
        ActorSetState(1);
    }
    TaskSetEntry(CannonEnterState, gCurTaskIdx);
}

void CannonInit(void)
{
    gCurTask->updateCallback = (u32)CannonUpdate;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 4, gCannonStates);
}

void CannonUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 4, gCannonStateUpdates);
}

void CannonEnterState(void)
{
    CallTableEntry(gCurTask->state, 4, gCannonStates);
}

void sub_0807728c(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->unk1C = 0;
        t->unk20 = 0;
        t->frame = 0;
    }
    TaskSleepForever();
}

void sub_080772b0(void)
{
    if ((u8)ActorCollideTerrain() == 0)
    {
        if (gCannonFuseState == 1)
        {
            sub_080771d8();
        }
        else
        {
            struct Task *t = gCurTask;

            if (t->unk18 <= 0)
            {
                if (ActorCheckHits())
                    sub_080770f0(gCurTask->hitterSlot);
            }
            else
            {
                t->unk18--;
            }
        }
    }
}

void sub_08077308(void)
{
    gCurTask->updateState = 1;
    TaskStop();
    sub_0807717c();
    gCurTask->frame = 12;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gCurTask;

        t->frame = 0;
        t->velY = -0x60000;
    }
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x30000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x30000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x60000;
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(2);
    TaskStop();
    gCannonFuseState = 0;
    ActorSetState(0);
    TaskSleepForever();
}

void sub_080773a8(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(CannonEnterState, gCurTaskIdx);
}

void sub_080773d0(void)
{
    gCurTask->updateState = 2;
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->velY = -0x24000;
        t->accelY = 0x4000;
        t->frame = 1;
    }
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
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
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->frame = 12;
    TaskYieldTrampoline(8);
    PlaySfx(225);
    sub_080771b0();
    {
        struct Task *t = gCurTask;

        t->frame = 0;
        t->unk6C = 0;
    }
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->velY = 0x20000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    TaskStop();
    TaskSleepForever();
}

void sub_08077564(void)
{
}

void sub_08077568(void)
{
    gCurTask->updateState = 3;
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->velY = -0x24000;
        t->accelY = 0x4000;
        t->frame = 1;
    }
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
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
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->frame = 14;
    TaskYieldTrampoline(8);
    PlaySfx(225);
    sub_080771c4();
    {
        struct Task *t = gCurTask;

        t->frame--;
        t->unk6C = 0;
    }
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->velY = 0x20000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    TaskStop();
    TaskSleepForever();
}

void sub_08077718(void)
{
}

void Task_CannonFuse(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)ActorMove;
        t->drawCallback = (u32)ActorDrawWorldInView;
        t->layer = 13;
    }
    {
        struct Task *t = gCurTask;

        t->frameTable = gCannonFuseFrames;
        CallTableEntry(t->variant, 1, gCannonFuseVariants);
    }
}

void sub_0807775c(void)
{
    struct Task *t = gCurTask;

    t->frame = 0xFFFF;
    t->unk34 = 0;
    t->unk20 = 0;
    t->unk1C = 3;
    t->unk18 = 0;
}

s32 sub_0807777c(struct M19Script *p)
{
    struct Task *t = gCurTask;
    u8 *q;
    s32 i;
    u16 v;

    if (t->unk24 == 1)
    {
        if (t->unk18 != 0)
        {
            i = t->unk1C;
            i *= 2;
            q = (u8 *)p + 18;
            q += i;
            v = *(vu16 *)q;
            return (s16)v;
        }
    }
    else
    {
        if (t->unk18 == 0)
        {
            i = t->unk1C;
            i *= 2;
            q = (u8 *)p + 18;
            q += i;
            v = *(vu16 *)q;
            return (s16)v;
        }
    }
    i = t->unk1C;
    i *= 2;
    q = (u8 *)p + 6;
    q += i;
    v = *(vu16 *)q;
    return (s16)v;
}

void sub_080777bc(s32 x, s32 y, s32 d)
{
    struct M19Script *p = gUnk_087401E4[gCurTask->unk20];
    struct Task *t = gCurTask;

    {
        s32 k = d + t->unk20 * 4;

        t->unk18 = gUnk_087401CC[k];
    }
    if (t->unk24 == 1)
    {
        sub_080269d8(x, y, 0);
        {
            struct Task *u = gCurTask;

            u->unk1C = 0;
            u->frame = p->unk04;
        }
    }
    else
    {
        sub_080269d8(t->pixelX, t->pixelY, 1);
        {
            struct Task *u = gCurTask;

            u->unk1C = p->unk00[2] - 1;
            u->frame = 0xFFFF;
        }
    }
}

void sub_08077830(void)
{
    struct M19Script *p = gUnk_087401E4[gCurTask->unk20];
    struct Task *t = gCurTask;

    if (t->unk34 <= 0)
    {
        gCurTask->frame = sub_0807777c(p);
        if (gCurTask->frame != -1)
        {
            struct Task *u = gCurTask;

            u->unk1C++;
            sub_080779dc();
        }
        else
        {
            sub_08077898(p);
        }
        gCurTask->unk34 = 2;
    }
    gCurTask->unk34--;
}

void sub_08077898(struct M19Script *p)
{
    struct Task *t = gCurTask;
    u8 d = p->unk00[t->unk18];
    s32 x = t->pixelX;
    s32 y = t->pixelY;
    s32 id;

    switch (d)
    {
    case 0:
        y -= 16;
        break;
    case 1:
        y += 16;
        break;
    case 2:
        x -= 16;
        break;
    case 3:
        x += 16;
        break;
    }
    id = sub_08022540(x, y);
    gCurTask->unk20 = id;
    if (id != -1)
    {
        sub_080777bc(x, y, d);
    }
    else
    {
        struct Task *u = gCurTask;

        if (u->unk24 == 1)
        {
            gCannonFuseState = u->unk24;
            TaskFree(u->unk46);
        }
        else
        {
            sub_080269d8(u->pixelX, u->pixelY, 1);
            gCannonFuseState = id;
            gCurTask->frame = 42;
            ActorSetState(0);
        }
    }
    {
        struct Task *v = gCurTask;

        v->pixelX = x;
        v->pixelY = y;
        v->posX = v->pixelX << 16;
        v->posY = v->pixelY << 16;
    }
}

void sub_08077980(void)
{
    struct M19Script *p = gUnk_087401E4[gCurTask->unk20];
    struct Task *t = gCurTask;

    if (t->unk34 <= 0)
    {
        if (t->unk1C < 0)
        {
            sub_08077898(p);
        }
        else
        {
            struct Task *u;

            gCurTask->frame = sub_0807777c(p);
            u = gCurTask;
            u->unk1C--;
        }
        gCurTask->unk34 = 2;
    }
    gCurTask->unk34--;
}

void sub_080779dc(void)
{
    struct Task *t = gCurTask;
    s32 i = t->frame;
    s32 j = i * 2;
    struct Task *u = &gTasks[t->unk46];

    u->pixelX = t->pixelX + gUnk_08740124[i * 2];
    u->pixelY = t->pixelY + gUnk_08740124[j + 1];
    u->posX = u->pixelX << 16;
    u->posY = u->pixelY << 16;
}

void CreateCannonFuseSpark(void)
{
    gCurTask->unk46 = CreateChildTaskHere(157, 1);
}

void CannonFuseInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)CannonFuseUpdate;
    t->unk30 = t->pixelX;
    t->unk2C = t->pixelY;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gCannonFuseStates);
}

void CannonFuseUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 3, gCannonFuseStateUpdates);
}

void CannonFuseEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gCannonFuseStates);
}
