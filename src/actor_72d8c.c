/* game_code_and_rodata 0x08072D8C-0x08074C0C (issue #79, module M19 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08072D8C 0x08074C0C src/actor_72d8c.c --newpb
 *
 * The middle of M19's cutscene bank: the class-3 states 13-25 of the
 * `0x0873FBD4` / `0x0873FC3C` script tables, each one a linear
 * TaskYieldTrampoline script that walks the 16.16 velocity pair
 * Task.velX/unk58 (and the 16.16 rotation/scale cells unk5C/unk60) through a
 * table of steps, plus the `WarpStarSetTrail` pose setter they all drive.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "cutscene.h"
#include "room.h"
#include "camera.h"
#include "player.h"
#include "actor.h"
#include "enemy.h"

/* callees */
extern s32 PlaySfx(u32 a);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void sub_0801bcac(u32 *p);
extern void RequestScreenShake(u32 a);

void sub_08072d8c(void)
{
    gCurTask->updateState = 13;
    gCurTask->facing = 255;
    {
        struct Task *t = gCurTask;

        t->posX = (t->pixelX - gViewRect[0]) << 16;
        t->posY = (t->pixelY - gViewRect[2]) << 16;
        t->moveCallback = (u32)TaskMoveRelativeToView;
        t->unk24 = 0;
    }
    {
        s32 r;

        gUnk_02005584 = 251;
        r = PlaySfx(251);
        gUnk_02004B4C = r;
    }
    WarpStarStopTrail();
    TaskStop();
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    WarpStarSetTrail(1, 5, 4, 0x300);
    {
        struct Task *t = gCurTask;

        t->velX = -0x8000;
        t->velY = 0x8000;
    }
    TaskYieldTrampoline(8);
    WarpStarStopTrail();
    TaskStop();
    gCurTask->velX = -0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velX = -0x8000;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gCurTask;

        t->velX = 0x40000;
        t->velY = -0x8000;
    }
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velY = -0xC000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x10000;
    TaskYieldTrampoline(8);
    WarpStarSetTrail(0, 0, 5, 0x500);
    gCurTask->velX = 0x8000;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gCurTask;

        t->velX = -0x8000;
        t->velY = -0x10000;
    }
    TaskYieldTrampoline(8);
    gCurTask->velX = -0x10000;
    TaskYieldTrampoline(4);
    gCurTask->unk6C = 0;
    do
    {
        {
            struct Task *t = gCurTask;

            t->velX = -0x40000;
            t->velY = -0x8000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gCurTask;

            t->velX = 0x20000;
            t->velY = -0x10000;
        }
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    gCurTask->unk6C = 0;
    do
    {
        {
            struct Task *t = gCurTask;

            t->velX = -0x40000;
            t->velY = 0x8000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gCurTask;

            t->velX = 0x14000;
            t->velY = 0x10000;
        }
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    gCurTask->unk6C = 0;
    do
    {
        {
            struct Task *t = gCurTask;

            t->velX = -0x30000;
            t->velY = 0x8000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gCurTask;

            t->velX = 0x10000;
            t->velY = -0x8000;
        }
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    gCurTask->unk6C = 0;
    do
    {
        {
            struct Task *t = gCurTask;

            t->velX = -0x60000;
            t->velY = -0x10000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gCurTask;

            t->velX = 0x20000;
            t->velY = -0x8000;
        }
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    {
        struct Task *t = gCurTask;

        t->velX = 0x30000;
        t->velY = 0x20000;
    }
    TaskYieldTrampoline(8);
    gCurTask->unk6C = 0;
    do
    {
        {
            struct Task *t = gCurTask;

            t->velX = -0x60000;
            t->velY = -0x10000;
        }
        TaskYieldTrampoline(2);
        gCurTask->velX = 0x20000;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    {
        struct Task *t = gCurTask;

        t->velX = 0x2000;
        t->velY = 0x8000;
    }
    TaskYieldTrampoline(8);
    gCurTask->unk6C = 0;
    do
    {
        {
            struct Task *t = gCurTask;

            t->velX = -0x20000;
            t->velY = -0x10000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gCurTask;

            t->velX = -0x40000;
            t->velY = -0x30000;
        }
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    WarpStarSetTrail(0, 0, 6, 0x500);
    {
        struct Task *t = gCurTask;

        t->velX = 0x40000;
        t->velY = -0x60000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = 0x20000;
        t->velY = -0x40000;
    }
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x2000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x60000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x2000;
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(4);
    sub_08025f00();
    gCurTask->unk24 = 1;
    TaskSleepForever();
}

void sub_080731c4(void)
{
    WarpStarEmitTrailStars();
}

void sub_080731d0(void)
{
    gCurTask->updateState = 14;
    {
        struct Task *t = gCurTask;

        t->posX = 0x200000;
        t->posY = -0x40000;
        t->moveCallback = (u32)TaskMoveRelativeToView;
    }
    WarpStarStopTrail();
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->velX = 0x20000;
        t->velY = 0x20000;
    }
    TaskYieldTrampoline(44);
    gCurTask->facing = 255;
    sub_080277f0(gCurTask->pixelX, gCurTask->pixelY);
    RequestScreenShake(4);
    StopSfxOnPlayer(gUnk_02004B4C, gUnk_02005584);
    PlaySfx(219);
    CreateBurstEffect(0, 0);
    PlaySfx(272);
    if (gMetaKnightmareMode == 0)
        sub_08071bb0(5);
    else
        sub_08071c38(5);
    gUnk_020061E0 = 0;
    ActorDestroy();
}

void sub_0807328c(void)
{
    WarpStarEmitTrailStars();
}

void sub_08073298(void)
{
    gCurTask->updateState = 15;
    {
        struct Task *t = gCurTask;

        t->posX = (t->pixelX - gViewRect[0]) << 16;
        t->posY = (t->pixelY - gViewRect[2]) << 16;
        t->moveCallback = (u32)TaskMoveRelativeToView;
        t->unk24 = 0;
    }
    {
        s32 r;

        gUnk_02005584 = 250;
        r = PlaySfx(250);
        gUnk_02004B4C = r;
    }
    WarpStarStopTrail();
    TaskStop();
    gCurTask->velY = 0x60000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x30000;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gCurTask;

        t->velY = 0x27000;
        t->accelY = -0x6000;
    }
    TaskYieldTrampoline(20);
    gCurTask->accelY = 0x8C00;
    TaskYieldTrampoline(20);
    TaskStop();
    TaskYieldTrampoline(1);
    gCurTask->velX = -0x1FF00;
    TaskYieldTrampoline(1);
    WarpStarSetTrail(0, 8, 6, 0x600);
    gCurTask->unk6C = 0;
    do
    {
        {
            struct Task *t = gCurTask;

            t->accelX = -0x5000;
            t->velY = 0x20000;
            t->accelY = -0x4000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gCurTask;

            t->accelX = 0x5000;
            t->velY = -0x40000;
            t->accelY = 0x2000;
        }
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 11);
    TaskSetMotion(-0x40000, 0x4000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gCurTask->accelY = 0x3000;
    TaskYieldTrampoline(8);
    CreateChildTaskAt(148, gCurTask->pixelX, gCurTask->pixelY + 16, 0);
    RequestScreenShake(2);
    PlaySfx(272);
    WarpStarStopTrail();
    TaskYieldTrampoline(10);
    WarpStarSetTrail(1, 9, 4, 0x300);
    gCurTask->accelY = -0x4000;
    TaskYieldTrampoline(10);
    WarpStarSetTrail(1, 7, 4, 0x300);
    {
        struct Task *t = gCurTask;

        t->accelX = -0x2000;
        t->velY = 0;
        t->accelY = -0x5000;
    }
    TaskYieldTrampoline(10);
    WarpStarSetTrail(1, 6, 4, 0x300);
    TaskYieldTrampoline(5);
    WarpStarSetTrail(1, 5, 4, 0x300);
    TaskYieldTrampoline(5);
    WarpStarSetTrail(1, 4, 4, 0x300);
    {
        struct Task *t = gCurTask;

        t->velX = 0x2000;
        t->accelX = -0x9000;
        t->accelY = 0xCC00;
    }
    TaskYieldTrampoline(10);
    WarpStarStopTrail();
    TaskYieldTrampoline(5);
    {
        struct Task *t = gCurTask;

        t->accelX = 0x10000;
        t->accelY = -0x7000;
    }
    TaskYieldTrampoline(5);
    WarpStarSetTrail(1, 6, 4, 0x300);
    TaskYieldTrampoline(15);
    {
        struct Task *t = gCurTask;

        t->accelX = -0x20000;
        t->velY = -0x20000;
        t->accelY = 0x4000;
    }
    TaskYieldTrampoline(5);
    WarpStarStopTrail();
    TaskYieldTrampoline(10);
    WarpStarSetTrail(1, 6, 3, 0x300);
    {
        struct Task *t = gCurTask;

        t->accelX = 0x18000;
        t->accelY = 0;
    }
    TaskYieldTrampoline(20);
    WarpStarSetTrail(1, 5, 3, 0x300);
    {
        struct Task *t = gCurTask;

        t->accelX = 0x1000;
        t->accelY = -0x10000;
    }
    TaskYieldTrampoline(30);
    TaskStop();
    TaskYieldTrampoline(10);
    sub_08025f00();
    gCurTask->unk24 = 1;
    TaskSleepForever();
}

void sub_08073578(void)
{
    WarpStarEmitTrailStars();
}

void sub_08073584(void)
{
    gCurTask->updateState = 17;
    {
        struct Task *t = gCurTask;

        t->posX = (t->pixelX - gViewRect[0]) << 16;
        t->posY = (t->pixelY - gViewRect[2]) << 16;
        t->moveCallback = (u32)TaskMoveRelativeToView;
        t->unk24 = 0;
    }
    {
        s32 r;

        gUnk_02005584 = 250;
        r = PlaySfx(250);
        gUnk_02004B4C = r;
    }
    WarpStarStopTrail();
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->velX = -0x80000;
        t->velY = 0x20000;
    }
    TaskYieldTrampoline(8);
    gCurTask->velX = -0x40000;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gCurTask;

        t->velX = -0x20000;
        t->velY = 0x10000;
    }
    TaskYieldTrampoline(8);
    {
        struct Task *t = gCurTask;

        t->velX = -0x10000;
        t->velY = 0x8000;
    }
    TaskYieldTrampoline(8);
    WarpStarSetTrail(1, 8, 4, 0x300);
    {
        struct Task *t = gCurTask;

        t->velX = 0x10000;
        t->velY = -0x10000;
    }
    TaskYieldTrampoline(8);
    CreateChildTaskAt(148, gCurTask->pixelX, gCurTask->pixelY + 16, 0);
    RequestScreenShake(2);
    PlaySfx(272);
    {
        struct Task *t = gCurTask;

        t->velX = 0x20000;
        t->velY = -0x20000;
    }
    TaskYieldTrampoline(8);
    {
        struct Task *t = gCurTask;

        t->velX = 0x40000;
        t->velY = -0x40000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = 0x60000;
        t->velY = -0x20000;
    }
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = 0x40000;
        t->velY = 0x8000;
    }
    TaskYieldTrampoline(3);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = 0x30000;
        t->velY = 0x40000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = 0x20000;
        t->velY = 0x60000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = 0x10000;
        t->velY = 0x80000;
    }
    TaskYieldTrampoline(2);
    gCurTask->velX = -0x10000;
    TaskYieldTrampoline(2);
    {
        struct Task *t = gCurTask;

        t->velX = -0x20000;
        t->velY = 0x60000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = -0x30000;
        t->velY = 0x40000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = -0x40000;
        t->velY = 0x20000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = -0x60000;
        t->velY = 0x10000;
    }
    TaskYieldTrampoline(4);
    WarpStarStopTrail();
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = -0x40000;
        t->velY = 0x800;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = -0x30000;
        t->velY = -0x2000;
    }
    TaskYieldTrampoline(4);
    WarpStarSetTrail(1, 5, 4, 0x300);
    gCurTask->velX = -0x20000;
    TaskYieldTrampoline(2);
    {
        struct Task *t = gCurTask;

        t->velX = 0x10000;
        t->velY = -0x100000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = 0x20000;
        t->velY = -0x80000;
    }
    TaskYieldTrampoline(13);
    TaskStop();
    sub_08025f00();
    gCurTask->unk24 = 1;
    TaskSleepForever();
}

void sub_080737f8(void)
{
    WarpStarEmitTrailStars();
}

void sub_08073804(void)
{
    gCurTask->updateState = 18;
    {
        struct Task *t = gCurTask;

        t->posX = (t->pixelX - gViewRect[0]) << 16;
        t->posY = (t->pixelY - gViewRect[2]) << 16;
        t->moveCallback = (u32)TaskMoveRelativeToView;
        t->unk24 = 0;
        t->unk18 = 0x3F0000;
        t->unk28 = -0x5000;
    }
    TaskStop();
    WarpStarSetTrail(0, 4, 4, 0x300);
    {
        struct Task *t = gCurTask;

        t->velX = 0x8000;
        t->velY = -0x20000;
    }
    TaskYieldTrampoline(48);
    gCurTask->velY = -0x14000;
    TaskYieldTrampoline(8);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(8);
    WarpStarSetTrail(0, 4, 8, 0x200);
    gCurTask->velY = -0xC000;
    TaskYieldTrampoline(8);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = -0x4000;
    TaskYieldTrampoline(8);
    WarpStarStopTrail();
    gCurTask->velY = -0x2000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x4000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x2000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x1000;
    TaskYieldTrampoline(40);
    sub_08025f00();
    gCurTask->unk24 = 1;
    TaskSleepForever();
}

void sub_0807395c(void)
{
    WarpStarEmitTrailStars();
}

void sub_08073968(void)
{
    gCurTask->updateState = 19;
    {
        s32 r;

        gUnk_02005584 = 218;
        r = PlaySfx(218);
        gUnk_02004B4C = r;
    }
    gCurTask->moveCallback = (u32)TaskMove;
    TaskStop();
    TaskSetMotionXFacing(0xC000, 0x5A5A5A5A);
    gCurTask->velY = 0x20000;
    TaskSleepForever();
}

void sub_080739bc(void)
{
    struct Task *t;

    WarpStarEmitTrailStars();
    sub_0801bcac(gUnk_0873F5CC);
    t = gCurTask;
    if (t->onGround & 1)
    {
        sub_080277f0(t->pixelX, t->pixelY);
        RequestScreenShake(4);
        StopSfxOnPlayer(gUnk_02004B4C, gUnk_02005584);
        PlaySfx(219);
        CreateBurstEffect(0, 0);
        PlaySfx(272);
        if (gMetaKnightmareMode == 0)
            sub_08071bb0(6);
        else
            sub_08071c38(3);
        gUnk_020061E0 = 0;
        ActorDestroy();
    }
}

void sub_08073a54(void)
{
    gCurTask->updateState = 20;
    {
        struct Task *t = gCurTask;

        t->posX = (t->pixelX - gViewRect[0]) << 16;
        t->posY = (t->pixelY - gViewRect[2]) << 16;
        t->moveCallback = (u32)TaskMoveRelativeToView;
        t->unk24 = 0;
    }
    {
        s32 r;

        gUnk_02005584 = 250;
        r = PlaySfx(250);
        gUnk_02004B4C = r;
    }
    WarpStarStopTrail();
    TaskStop();
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gCurTask;

        t->velX = 0x10000;
        t->velY = 0x8000;
    }
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(8);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gCurTask;

        t->velX = 0x8000;
        t->velY = -0x10000;
    }
    TaskYieldTrampoline(8);
    {
        struct Task *t = gCurTask;

        t->velX = -0x10000;
        t->velY = -0x20000;
    }
    TaskYieldTrampoline(8);
    {
        struct Task *t = gCurTask;

        t->velX = -0x20000;
        t->velY = -0x10000;
    }
    TaskYieldTrampoline(8);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gCurTask;

        t->velX = -0x10000;
        t->velY = 0x54000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = -0x8000;
        t->velY = 0x34000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = -0x2000;
        t->velY = 0x18000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->velX = -0x800;
        t->velY = 0xC000;
    }
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(6);
    gCurTask->velY = 0x800;
    TaskYieldTrampoline(15);
    WarpStarSetTrail(1, 4, 4, 0x300);
    TaskYieldTrampoline(15);
    TaskStop();
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(2);
    CreateChildTaskAt(148, gCurTask->pixelX, gCurTask->pixelY + 16, 0);
    RequestScreenShake(2);
    PlaySfx(272);
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(4);
    WarpStarSetTrail(1, 6, 5, 0x300);
    {
        struct Task *t = gCurTask;

        t->velX = 0x2000;
        t->velY = -0x10000;
    }
    TaskYieldTrampoline(16);
    gCurTask->velX = 0x8000;
    TaskYieldTrampoline(16);
    WarpStarSetTrail(1, 8, 6, 0x300);
    gCurTask->velX = 0x10000;
    TaskYieldTrampoline(16);
    gCurTask->velX = 0x14000;
    TaskYieldTrampoline(16);
    gCurTask->velX = 0x20000;
    TaskYieldTrampoline(16);
    gCurTask->velX = 0x30000;
    TaskYieldTrampoline(40);
    TaskStop();
    sub_08025f00();
    gCurTask->unk24 = 1;
    TaskSleepForever();
}

void sub_08073cd4(void)
{
    WarpStarEmitTrailStars();
}

void sub_08073ce0(void)
{
    gCurTask->updateState = 21;
    {
        struct Task *t = gCurTask;

        t->posX = (t->pixelX - gViewRect[0]) << 16;
        t->posY = (t->pixelY - gViewRect[2]) << 16;
        t->moveCallback = (u32)TaskMoveRelativeToView;
        t->unk24 = 0;
    }
    WarpStarStopTrail();
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->velX = 0x60000;
        t->velY = -0x20000;
    }
    TaskYieldTrampoline(7);
    WarpStarSetTrail(2, 0, -1, 0);
    TaskYieldTrampoline(1);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(5);
    WarpStarSetTrail(2, 0, -1, 0);
    TaskYieldTrampoline(3);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(3);
    WarpStarSetTrail(2, 0, -1, 0);
    TaskYieldTrampoline(1);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(1);
    WarpStarSetTrail(2, 0, -1, 0);
    TaskYieldTrampoline(5);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(1);
    WarpStarSetTrail(2, 0, -1, 0);
    TaskYieldTrampoline(7);
    sub_08025f00();
    gCurTask->unk24 = 1;
    TaskSleepForever();
}

void sub_08073e00(void)
{
    WarpStarEmitTrailStars();
}

void sub_08073e0c(void)
{
    gCurTask->updateState = 22;
    {
        struct Task *t = gCurTask;

        t->posX = (t->pixelX - gViewRect[0]) << 16;
        t->posY = (t->pixelY - gViewRect[2]) << 16;
        t->moveCallback = (u32)TaskMoveRelativeToView;
    }
    {
        s32 r;

        gUnk_02005584 = 218;
        r = PlaySfx(218);
        gUnk_02004B4C = r;
    }
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->velX = 0x10000;
        t->velY = 0x20000;
    }
    TaskSleepForever();
}

void sub_08073e80(void)
{
    struct Task *t;

    WarpStarEmitTrailStars();
    sub_0801bcac(gUnk_0873F5CC);
    t = gCurTask;
    if (t->onGround & 1)
    {
        sub_080277f0(t->pixelX, t->pixelY);
        RequestScreenShake(4);
        StopSfxOnPlayer(gUnk_02004B4C, gUnk_02005584);
        PlaySfx(219);
        CreateBurstEffect(0, 0);
        PlaySfx(272);
        if (gMetaKnightmareMode == 0)
            sub_08071bb0(3);
        else
            sub_08071c38(3);
        gUnk_020061E0 = 0;
        ActorDestroy();
    }
}

void sub_08073f18(void)
{
    gCurTask->updateState = 23;
    StopBgm();
    {
        s32 r;

        gUnk_02005584 = 126;
        r = PlaySfx(126);
        gUnk_02004B4C = r;
    }
    WarpStarStopTrail();
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->velX = 0x18000;
        t->velY = -0x8000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        t->accelY = -0x8000;
        t->speedLimitY = 0x60000;
    }
    while (gViewRect[2] > 8)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(60);
    StopSfxOnPlayer(gUnk_02004B4C, gUnk_02005584);
    {
        s32 r;

        gUnk_02005584 = 217;
        r = PlaySfx(217);
        gUnk_02004B4C = r;
    }
    {
        struct Task *t = gCurTask;

        t->unk18 = 0x100000;
        t->unk28 = -0xD00;
        t->posX = 0x8A0000;
        t->posY = -0x100000;
        t->velX = -0x10000;
        t->velY = 0x8000;
    }
    TaskYieldTrampoline(28);
    gCurTask->velX = -0xC000;
    TaskYieldTrampoline(28);
    gCurTask->velX = -0x8000;
    TaskYieldTrampoline(28);
    gCurTask->velX = -0x4000;
    TaskYieldTrampoline(14);
    {
        struct Task *t = gCurTask;

        t->velX = -0x1000;
        t->velY = 0x4000;
    }
    TaskYieldTrampoline(14);
    gCurTask->velX = -0x800;
    TaskYieldTrampoline(14);
    gCurTask->velX = 0x1000;
    TaskYieldTrampoline(14);
    gCurTask->velX = 0x2000;
    TaskYieldTrampoline(14);
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(62);
    {
        struct Task *t = gCurTask;

        t->unk28 = 0;
        t->frameTable = gUnk_08754560;
        t->tileWord = 0xF010;
        t->facing = 1;
    }
    {
        struct Task *t = gCurTask;

        t->unk18 = -2;
        t->unk28 = 0;
    }
    TaskSetFrame(1);
    TaskYieldTrampoline(50);
    gCurTask->velY = 0x1000;
    TaskYieldTrampoline(84);
    sub_08025b0c();
    TaskSleepForever();
}

void sub_0807409c(void)
{
    if ((gCurTask->posY >> 16) < -32)
        TaskStop();
}

void sub_080740bc(void)
{
    gCurTask->updateState = 24;
    PlayBgm(33);
    TaskCreateFrom(98, 32);
    {
        struct Task *t = gCurTask;

        t->posX = 0;
        t->posY = -0x220000;
        t->moveCallback = (u32)TaskMoveRelativeToView;
    }
    WarpStarStopTrail();
    TaskStop();
    TaskYieldTrampoline(16);
    gCurTask->unk6C = 0;
    do
    {
        {
            struct Task *t = gCurTask;

            t->velX = 0x8000;
            t->velY = -0x1000;
        }
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->velX = 0x4000;
            t->velY = -0x2000;
        }
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->velX = 0x8000;
            t->velY = -0x1000;
        }
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->velX = 0x4000;
            t->velY = 0x10000;
        }
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->velX = 0x8000;
            t->velY = 0x8000;
        }
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 5);
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(24);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(16);
    gCurTask->unk6C = 0;
    do
    {
        {
            struct Task *t = gCurTask;

            t->velX = -0x8000;
            t->velY = 0x10000;
        }
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk6C = 0;
    do
    {
        {
            struct Task *t = gCurTask;

            t->velX = -0x2000;
            t->velY = 0x10000;
        }
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(8);
    gCurTask->unk6C = 0;
    do
    {
        {
            struct Task *t = gCurTask;

            t->velX = 0x40000;
            t->velY = -0x40000;
        }
        TaskYieldTrampoline(16);
        gCurTask->velY = 0x40000;
        TaskYieldTrampoline(16);
        gCurTask->velX = -0x40000;
        TaskYieldTrampoline(16);
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(16);
        TaskStop();
        gCurTask->velY = -0x40000;
        TaskYieldTrampoline(16);
        TaskStop();
        gCurTask->velX = 0x40000;
        TaskYieldTrampoline(16);
        TaskStop();
        gCurTask->velY = 0x40000;
        TaskYieldTrampoline(16);
        {
            struct Task *t = gCurTask;

            t->velX = -0x40000;
            t->velY = -0x20000;
        }
        TaskYieldTrampoline(16);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    PlayBgm(34);
    sub_08075290(11);
    gCurTask->velX = -0x2000;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gCurTask;

        t->velX = -0x20000;
        t->velY = 0x8000;
    }
    TaskYieldTrampoline(16);
    {
        struct Task *t = gCurTask;

        t->velX = 0x20000;
        t->velY = -0x8000;
    }
    TaskYieldTrampoline(16);
    {
        struct Task *t = gCurTask;

        t->velX = 0x10000;
        t->velY = 0x20000;
    }
    TaskYieldTrampoline(8);
    RequestScreenShake(4);
    gCurTask->facing = 1;
    RequestScreenShake(4);
    StopSfxOnPlayer(gUnk_02004B4C, gUnk_02005584);
    PlaySfx(219);
    CreateBurstEffect(0, 0);
    PlaySfx(272);
    if (gMetaKnightmareMode != 0)
        while (1)
            ;
    sub_08071bb0(7);
    gUnk_020061E0 = 0;
    ActorDestroy();
}

void sub_080743c8(void)
{
}

void sub_080743cc(void)
{
    gCurTask->updateState = 25;
    sub_08071d2c();
    gCurTask->speedLimitY = 0x50000;
    TaskSleepForever();
}

void sub_080743f0(void)
{
    TaskGetScreenPos();
    if (gCurTask->unk24 == 0 && gUnk_030023D4 <= 0)
    {
        sub_08025f00();
        gCurTask->unk24 = 1;
    }
}

void sub_08074420(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)TaskMove;
        t->updateState = 4;
    }
    {
        s32 r;

        gUnk_02005584 = 218;
        r = PlaySfx(218);
        gUnk_02004B4C = r;
    }
    gCurTask->facing = 1;
    WarpStarSetTrail(1, 11, 4, 0x400);
    {
        struct Task *t = gCurTask;

        t->velX = 0x6000;
        t->velY = 0x48000;
    }
    TaskSleepForever();
}

void sub_0807447c(void)
{
    struct Task *t;

    WarpStarEmitTrailStars();
    sub_0801bcac(gUnk_0873F5CC);
    t = gCurTask;
    if (t->onGround & 1)
    {
        sub_080277f0(t->pixelX, t->pixelY);
        RequestScreenShake(4);
        StopSfxOnPlayer(gUnk_02004B4C, gUnk_02005584);
        PlaySfx(219);
        CreateBurstEffect(0, 0);
        if (gMetaKnightmareMode == 0)
            sub_08071bb0(1);
        else
            sub_08071c38(1);
        gUnk_020061E0 = 0;
        ActorDestroy();
    }
}

void Task_WarpStarCamera(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMove;
    t->drawCallback = 0;
    t->updateCallback = (u32)WarpStarCameraUpdate;
    sub_08027798(t->pixelX, t->pixelY);
    CallTableEntry(gTasks[gCurTask->parent].state, 25, gWarpStarCameraPaths);
}

void WarpStarCameraUpdate(void)
{
    struct Task *t = gCurTask;

    SetCameraFocusOrAnchor(t->pixelX, t->pixelY);
}

void sub_08074588(void)
{
    while (1)
    {
        struct Task *t = gCurTask;

        t->posX = gTasks[gLocalPlayer].pixelX << 16;
        t->posY = gTasks[gLocalPlayer].pixelY << 16;
        TaskYieldTrampoline(1);
    }
}

void sub_080745d0(void)
{
    TaskExitTrampoline();
}

void sub_080745dc(void)
{
    TaskStop();
    TaskYieldTrampoline(48);
    sub_08025e00();
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velY = -0x40000;
    TaskSleepForever();
}

void sub_08074628(void)
{
    TaskStop();
    TaskExitTrampoline();
}

void sub_08074638(void)
{
    TaskStop();
    TaskYieldTrampoline(24);
    sub_08025e00();
    gCurTask->velX = 0x10000;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x40000;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x60000;
    TaskYieldTrampoline(38);
    gCurTask->velX = 0x40000;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->velX = 0x10000;
    TaskYieldTrampoline(2);
    TaskStop();
    TaskExitTrampoline();
}

void sub_080746c0(void)
{
    TaskStop();
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(84);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(16);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(16);
    TaskStop();
    TaskExitTrampoline();
}

void sub_0807470c(void)
{
    TaskStop();
    sub_08025e00();
    TaskYieldTrampoline(102);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(9);
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(10);
    gCurTask->velY = -0x60000;
    TaskYieldTrampoline(42);
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(10);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(9);
    TaskStop();
    TaskExitTrampoline();
}

void sub_08074784(void)
{
    TaskStop();
    TaskExitTrampoline();
}

void sub_08074794(void)
{
    TaskStop();
    gCurTask->velY = -0x60000;
    TaskYieldTrampoline(64);
    gCurTask->velY = -0x60000;
    TaskYieldTrampoline(64);
    gCurTask->velY = -0x60000;
    TaskYieldTrampoline(64);
    gCurTask->velY = -0x60000;
    TaskYieldTrampoline(64);
    TaskStop();
    TaskExitTrampoline();
}

void sub_080747dc(void)
{
    TaskStop();
    TaskExitTrampoline();
}

void sub_080747ec(void)
{
    TaskStop();
    TaskYieldTrampoline(40);
    sub_08025e00();
    gCurTask->velX = 0x8000;
    TaskYieldTrampoline(64);
    gCurTask->velX = 0x10000;
    TaskYieldTrampoline(64);
    gCurTask->velX = 0x20000;
    TaskYieldTrampoline(16);
    gCurTask->velX = 0x40000;
    TaskYieldTrampoline(10);
    gCurTask->velX = 0x60000;
    TaskYieldTrampoline(48);
    gCurTask->velX = 0x40000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x10000;
    TaskYieldTrampoline(8);
    TaskStop();
    TaskExitTrampoline();
}

void sub_08074880(void)
{
    TaskStop();
    SetCameraFocusOrAnchor(gCurTask->pixelX, 0x10D);
    TaskExitTrampoline();
}

void sub_080748a8(void)
{
    TaskStop();
    TaskYieldTrampoline(130);
    sub_08025e00();
    gCurTask->accelX = 0x6000;
    TaskYieldTrampoline(28);
    {
        struct Task *t = gCurTask;

        t->velX = 0x90000;
        t->accelX = 0;
    }
    TaskYieldTrampoline(67);
    gCurTask->accelX = -0x8000;
    TaskYieldTrampoline(10);
    gCurTask->accelX = 0;
    TaskYieldTrampoline(80);
    TaskExitTrampoline();
}

void sub_08074904(void)
{
    TaskStop();
    TaskYieldTrampoline(32);
    sub_08025e00();
    gCurTask->velX = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x30000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x40000;
    TaskYieldTrampoline(40);
    gCurTask->velX = 0x30000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x10000;
    TaskYieldTrampoline(8);
    TaskStop();
    TaskExitTrampoline();
}

void sub_08074974(void)
{
    TaskStop();
    TaskStop();
    TaskExitTrampoline();
}

void sub_08074988(void)
{
    TaskStop();
    TaskYieldTrampoline(80);
    sub_08025e00();
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(6);
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(6);
    gCurTask->velY = 0x60000;
    TaskYieldTrampoline(18);
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(6);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(6);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(4);
    TaskStop();
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(6);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(6);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(6);
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(6);
    gCurTask->velY = -0x60000;
    TaskYieldTrampoline(81);
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(6);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(6);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(6);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(6);
    TaskStop();
    TaskExitTrampoline();
}

void sub_08074ab8(void)
{
    TaskStop();
    TaskExitTrampoline();
}

void sub_08074ac8(void)
{
    TaskStop();
    TaskYieldTrampoline(20);
    TaskSetMotionY(-0x10000, -0x1000, 0x60000);
    while (gCurTask->velY >= -((gViewRect[2] - 8) << 12))
        TaskYieldTrampoline(1);
loop:
    if (gViewRect[2] <= 7)
    {
        TaskStop();
        gCurTask->posY = 0x620000;
    }
    else
    {
        struct Task *t = gCurTask;
        s32 v = -((gViewRect[2] - 8) << 12);

        t->velY = v;
        if (v < -0x60000)
            t->velY = -0x60000;
    }
    TaskYieldTrampoline(1);
    goto loop;
}

void sub_08074b60(void)
{
    {
        struct Task *t = gCurTask;

        t->posX = t->pixelX << 16;
        t->posY = 0x700000;
    }
    TaskStop();
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(160);
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(128);
    TaskStop();
    sub_08025e0c();
    TaskExitTrampoline();
}

void CreateWarpStarTrailStar(int a, int b, int c)
{
    u16 x = a;
    u16 y = b;
    u8 z = c;
    s32 id;

    id = CreateChildTaskAt(165, gCurTask->pixelX, gCurTask->pixelY, 0);
    gTasks[id].state = z;
    if (z != 2)
    {
        gTasks[id].unk28 = x;
        gTasks[id].unk2C = (s16)y;
    }
}
