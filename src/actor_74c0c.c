/* game_code_and_rodata 0x08074C0C-0x080763E8 (issue #79, module M19 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08074C0C 0x080763E8 src/actor_74c0c.c --newpb
 *
 * M19 batch 3: task type #165 (Task_WarpStarTrailStar, the four-ring sparkle draw loop),
 * type #97 (Task_WarpStarCamera) and type #98 (Task_NightmarePowerOrbEscape) with their coroutine
 * bodies and the gWarpStarCameraPaths dispatch row.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "cutscene.h"
#include "actor.h"
#include "enemy.h"

/* RAM cells and ROM tables */
/* Not from room.h or effect.h: this file's view of gUnk_0200AF20 and
   gUnk_03001370 differs (lesson 3.517). */
extern s16 gViewRect[];
extern u8 gUnk_02005E10[];
extern u8 gUnk_0200AF20[];
extern u8 gUnk_03001370[];
extern u8 gActivePlayerMask;

/* callees */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern s32 PlaySfx(u32 a);
extern u32 RandomRange(u32 range);
extern u32 TaskIsOnScreen(void);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void BlendColors(void *src, void *dst, s32 ratio, s32 count, void *out);
extern void SetCameraFocus(s32 a, s32 b);
extern void AngleToVector(s16 t, s16 mag);
extern void LoadBackdropColor(u32 src);

void Task_WarpStarTrailStar(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)TaskMove;
        t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
        t->layer = 12;
    }
    {
        struct Task *t = gCurTask;

        t->tileWord = 0;
        if (gTasks[t->parent].unk18 < 0)
            t->tileWord |= 0xC00;
        else
            t->tileWord &= 0xF3FF;
    }
    switch (gCurTask->state)
    {
    case 1:
        AngleToVector(gCurTask->unk28, gCurTask->unk2C);
        {
            struct Task *t = gCurTask;

            t->velX = gUnk_030023B4;
            t->velY = gUnk_030023D4;
            t->frameTable = gUnk_0874C44C;
        }
        gCurTask->frame = RandomRange(2) + 4;
        TaskSleepForever();
        break;
    case 0:
        AngleToVector(gCurTask->unk28, gCurTask->unk2C);
        {
            struct Task *t = gCurTask;

            t->velX = gUnk_030023B4;
            t->velY = gUnk_030023D4;
            t->frameTable = gUnk_0874C500;
        }
        gCurTask->frame = RandomRange(8);
        TaskSleepForever();
        break;
    case 2:
        {
            struct Task *t = gCurTask;

            t->drawCallback = 0;
            t->unk6C = 0;
        }
        do
        {
            {
                struct Task *t = gCurTask;

                QueueSprite(1, (u32)gUnk_080D21C8, t->spriteFlags, t->tileWord,
                             t->pixelX + gUnk_0873FCF8[(s16)t->unk6C * 2],
                             t->pixelY + gUnk_0873FCF8[(s16)t->unk6C * 2 + 1]);
            }
            {
                struct Task *t = gCurTask;

                QueueSprite(1, (u32)gUnk_080D21C8, t->spriteFlags, t->tileWord,
                             t->pixelX + gUnk_0873FD20[(s16)t->unk6C * 2],
                             t->pixelY + gUnk_0873FD20[(s16)t->unk6C * 2 + 1]);
            }
            {
                struct Task *t = gCurTask;

                QueueSprite(1, (u32)gUnk_080D21C8, t->spriteFlags, t->tileWord,
                             t->pixelX + gUnk_0873FD48[(s16)t->unk6C * 2],
                             t->pixelY + gUnk_0873FD48[(s16)t->unk6C * 2 + 1]);
            }
            {
                struct Task *t = gCurTask;

                QueueSprite(1, (u32)gUnk_080D21C8, t->spriteFlags, t->tileWord,
                             t->pixelX + gUnk_0873FD70[(s16)t->unk6C * 2],
                             t->pixelY + gUnk_0873FD70[(s16)t->unk6C * 2 + 1]);
            }
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 9);
        TaskExitTrampoline();
        break;
    default:
        TaskExitTrampoline();
        break;
    }
}

void sub_08074e8c(void)
{
    CpuSet(gUnk_03001370, gUnk_02005E10, 128);
    CpuSet(gUnk_03001370 + 256, gUnk_02005E10 + 256, 128);
    CpuSet(gUnk_03001370 + 576, gUnk_0200AF20, 32);
    CpuSet(gUnk_03001370 + 704, gUnk_0200AF20 + 64, 32);
}

void sub_08074ee0(u32 flag)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            struct PlayerState *p = &gPlayerStates[i];

            if (flag)
                p->unk42 |= 0x10;
            else
                p->unk42 &= 0xFFEF;
        }
    }
}

void sub_08074f48(u8 a)
{
    s32 v1;
    s32 v2;

    switch (a)
    {
    case 0:
        v1 = 96;
        v2 = 64;
        sub_08074ee0(1);
        break;
    case 1:
        v1 = 192;
        v2 = 128;
        sub_08074ee0(1);
        break;
    case 2:
        v2 = 256;
        v1 = v2;
        sub_08074ee0(1);
        break;
    case 3:
        v1 = 0;
        v2 = 0;
        sub_08074ee0(0);
        break;
    }
    BlendColors(gUnk_02005E10, gUnk_0873FD98, v1, 128, gUnk_03001370);
    BlendColors(gUnk_02005E10 + 256, gUnk_0873FE98, v2, 128, gUnk_03001370 + 256);
    BlendColors(gUnk_0200AF20, gUnk_0873FE98, v2, 32, gUnk_03001370 + 576);
    BlendColors(gUnk_0200AF20 + 64, gUnk_0873FE98, v2, 32, gUnk_03001370 + 704);
}

void Task_NightmarePowerOrbEscape(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)TaskMove;
        t->drawCallback = (u32)sub_080652c8;
        t->layer = 11;
    }
    gCurTask->frameTable = gUnk_0875549C;
    RequestCopy(2, (u32)gUnk_085E6FA4, IWRAM_START + 0x15B0, 64);
    LZ77UnCompWram(gUnk_085E6FE4, (void *)(EWRAM_START + 0x20000));
    RequestCopy(4, EWRAM_START + 0x20000, OBJ_VRAM0 + 0x3000, 0x800);
    LZ77UnCompWram(gUnk_085E72D4, (void *)(EWRAM_START + 0x20000));
    RequestCopy(4, EWRAM_START + 0x20000, OBJ_VRAM0 + 0x4000, 0x1000);
    {
        struct Task *t = gCurTask;

        t->tileWord = 0xA990;
        t->posX = 0xD00000;
        t->posY = 0x1000000;
        t->frame = 0;
        t->velX = -0x2000;
    }
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 1;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 8);
    sub_08074e8c();
    gCurTask->unk6C = 0;
    do
    {
        sub_08074f48(0);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        sub_08074f48(3);
        gCurTask->frame = 1;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 8);
    gCurTask->unk6C = 0;
    do
    {
        sub_08074f48(0);
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        sub_08074f48(1);
        gCurTask->frame = 2;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 8);
    gCurTask->unk6C = 0;
    do
    {
        sub_08074f48(1);
        gCurTask->frame = 4;
        TaskYieldTrampoline(2);
        sub_08074f48(2);
        gCurTask->frame = 3;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 8);
    sub_08074f48(2);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->frame = 4;
    TaskYieldTrampoline(2);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->frame = 4;
    TaskYieldTrampoline(2);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->frame = 4;
    TaskYieldTrampoline(2);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->frame = 4;
    TaskYieldTrampoline(2);
    sub_08074f48(1);
    gCurTask->frame = 5;
    TaskYieldTrampoline(2);
    gCurTask->frame = 4;
    TaskYieldTrampoline(2);
    sub_08074f48(0);
    TaskYieldTrampoline(4);
    sub_08074f48(3);
    gCurTask->frame = 5;
    TaskYieldTrampoline(12);
    {
        struct Task *t = gCurTask;

        t->velX = -0x800;
        t->frame = 5;
    }
    TaskYieldTrampoline(76);
    CreateNightmarePowerOrbEscapeStars(12);
    TaskYieldTrampoline(52);
    CreateNightmarePowerOrbEscapeStars(0);
    gCurTask->velX = 0x20000;
    TaskYieldTrampoline(16);
    CreateNightmarePowerOrbEscapeStars(1);
    gCurTask->velX = 0x40000;
    TaskYieldTrampoline(19);
    CreateNightmarePowerOrbEscapeStars(2);
    TaskExitTrampoline();
}

void CreateNightmarePowerOrbEscapeStars(s32 a)
{
    struct Task *t;
    s32 id;

    if (a == 12)
    {
        a = 3;
    loop:
        id = TaskCreateFrom(99, 32);
        if (id != -1)
        {
            t = &gTasks[id];
            t->variant = a;
        }
        a++;
        if (a <= 10)
            goto loop;
    }
    else
    {
        id = TaskCreateFrom(99, 32);
        if (id != -1)
        {
            t = &gTasks[id];
            t->variant = a;
        }
    }
}

void Task_NightmarePowerOrbEscapeStar(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)TaskMove;
        t->drawCallback = (u32)NightmarePowerOrbEscapeStarDraw;
        t->layer = 12;
    }
    {
        struct Task *t = gCurTask;

        t->frameTable = gUnk_0875549C;
        t->tileWord = 0xA210;
    }
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->frame = 0xFFFF;
        t->unk18 = 0;
        t->unk1C = -1;
        switch (t->variant)
        {
    case 0:
        {
            struct Task *t = gCurTask;

            t->posX = (gViewRect[0] + 168) << 16;
            t->posY = (gViewRect[2] + 40) << 16;
        }
        {
            struct Task *t = gCurTask;

            t->frame = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x50000;
            t->unk1C = 0;
            t->unk2C = 16;
            t->velX = -0x40000;
            t->velY = -0x80000;
        }
        TaskYieldTrampoline(3);
        gCurTask->velY = -0x40000;
        TaskYieldTrampoline(3);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(3);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
        }
        TaskStop();
        TaskYieldTrampoline(3);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(3);
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->velX = -0x80000;
            t->velY = 0x20000;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 1:
        {
            struct Task *t = gCurTask;

            t->posX = (gViewRect[0] + 248) << 16;
            t->posY = (gViewRect[2] + 40) << 16;
        }
        TaskYieldTrampoline(35);
        {
            struct Task *t = gCurTask;

            t->frame = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x50000;
            t->unk1C = 0;
            t->unk2C = 16;
            t->velX = -0x40000;
            t->velY = -0x80000;
        }
        TaskYieldTrampoline(3);
        gCurTask->velY = -0x40000;
        TaskYieldTrampoline(3);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(3);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
        }
        TaskStop();
        TaskYieldTrampoline(3);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(3);
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->velX = -0x80000;
            t->velY = 0x20000;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 2:
        {
            struct Task *t = gCurTask;

            t->posX = (gViewRect[0] + 200) << 16;
            t->posY = (gViewRect[2] + 40) << 16;
        }
        TaskYieldTrampoline(17);
        {
            struct Task *t = gCurTask;

            t->frame = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x20000;
            t->unk1C = 0;
            t->unk2C = 16;
            t->velX = -0x40000;
            t->velY = 0x40000;
        }
        TaskYieldTrampoline(12);
        gCurTask->velY = 0x20000;
        TaskYieldTrampoline(10);
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->velX = -0x20000;
            t->velY = 0x8000;
        }
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
            t->velY = -0x8000;
        }
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(6);
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x10000;
            t->velY = -0x40000;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 3:
        {
            struct Task *t = gCurTask;

            t->posX = (gViewRect[0] + 240) << 16;
            t->posY = (gViewRect[2] + 64) << 16;
        }
        TaskYieldTrampoline(128);
        {
            struct Task *t = gCurTask;

            t->frame = 6;
            t->unk18 = 0x3F0000;
            t->unk28 = -0x10000;
            t->unk1C = 0;
            t->unk2C = -16;
            t->velX = -0x60000;
            t->velY = 0x40000;
        }
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->velX = -0x40000;
            t->velY = 0x20000;
        }
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->velX = -0x60000;
            t->velY = -0x20000;
        }
        TaskYieldTrampoline(5);
        gCurTask->unk2C = -8;
        CreateBurstEffect(4, 256);
        PlaySfx(189);
        {
            struct Task *t = gCurTask;

            t->velX = -0x40000;
            t->velY = -0x40000;
        }
        TaskYieldTrampoline(6);
        {
            struct Task *t = gCurTask;

            t->velX = -0x60000;
            t->velY = -0x20000;
        }
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->velX = -0x40000;
            t->velY = 0x20000;
        }
        TaskYieldTrampoline(6);
        {
            struct Task *t = gCurTask;

            t->velX = -0x60000;
            t->velY = 0x40000;
        }
        TaskYieldTrampoline(5);
        {
            struct Task *t = gCurTask;

            t->velX = -0x40000;
            t->velY = 0x10000;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 4:
        {
            struct Task *t = gCurTask;

            t->posX = (gViewRect[0] + 248) << 16;
            t->posY = (gViewRect[2] + 88) << 16;
        }
        TaskYieldTrampoline(160);
        {
            struct Task *t = gCurTask;

            t->frame = 6;
            t->unk18 = 0x3F0000;
            t->unk28 = -0x10000;
            t->unk1C = 0;
            t->unk2C = -16;
            t->velX = -0x80000;
            t->velY = -0x80000;
        }
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->velX = -0x60000;
            t->velY = -0x40000;
        }
        TaskYieldTrampoline(7);
        {
            struct Task *t = gCurTask;

            t->velX = -0x40000;
            t->velY = -0x20000;
        }
        TaskYieldTrampoline(6);
        gCurTask->unk2C = -8;
        CreateBurstEffect(4, 256);
        PlaySfx(189);
        {
            struct Task *t = gCurTask;

            t->velX = -0x30000;
            t->velY = -0x10000;
        }
        TaskYieldTrampoline(5);
        {
            struct Task *t = gCurTask;

            t->velX = -0x20000;
            t->velY = -0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x14000;
            t->velY = 0;
        }
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->velX = -0xC000;
            t->velY = 0x8000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gCurTask;

            t->velX = -0x8000;
            t->velY = 0x10000;
            t->unk28 = 0;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 16);
        break;
    case 5:
        {
            struct Task *t = gCurTask;

            t->posX = (gViewRect[0] + 248) << 16;
            t->posY = (gViewRect[2] + 16) << 16;
        }
        TaskYieldTrampoline(192);
        {
            struct Task *t = gCurTask;

            t->frame = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x20000;
            t->unk1C = 0;
            t->unk2C = 16;
            t->velX = -0x80000;
            t->velY = -0x40000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x60000;
            t->velY = -0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x40000;
            t->velY = -0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x30000;
            t->velY = -0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x20000;
            t->velY = 0;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x14000;
            t->velY = 0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x10000;
            t->velY = 0xC000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0xC000;
            t->velY = 0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
            t->velX = -0x8000;
            t->velY = 0x14000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = 0;
            t->velY = 0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = 0x8000;
            t->velY = 0x30000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = 0x10000;
            t->velY = 0x40000;
        }
        TaskYieldTrampoline(4);
        gCurTask->velX = 0x20000;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0x40000;
        TaskYieldTrampoline(4);
        gCurTask->unk6C = 0;
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 6:
        {
            struct Task *t = gCurTask;

            t->posX = (gViewRect[0] + 248) << 16;
            t->posY = (gViewRect[2] + 80) << 16;
        }
        TaskYieldTrampoline(224);
        {
            struct Task *t = gCurTask;

            t->frame = 6;
            t->unk18 = 0x3F0000;
            t->unk28 = -0x10000;
            t->unk1C = 0;
            t->unk2C = -16;
            t->velX = -0x40000;
            t->velY = 0x10000;
        }
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(12);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(4);
        gCurTask->unk2C = -8;
        CreateBurstEffect(4, 256);
        PlaySfx(189);
        {
            struct Task *t = gCurTask;

            t->velX = -0x40000;
            t->velY = -0x8000;
        }
        TaskYieldTrampoline(4);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(8);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(18);
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(2);
        break;
    case 7:
        {
            struct Task *t = gCurTask;

            t->posX = (gViewRect[0] + 120) << 16;
            t->posY = (gViewRect[2] + 120) << 16;
        }
        TaskYieldTrampoline(128);
        {
            struct Task *t = gCurTask;

            t->frame = 6;
            t->unk18 = 0x3F0000;
            t->unk28 = -0x20000;
            t->unk1C = 0;
            t->unk2C = -16;
            t->velX = -0x2000;
            t->velY = -0x40000;
        }
        TaskYieldTrampoline(12);
        gCurTask->unk2C = -8;
        CreateBurstEffect(4, 256);
        PlaySfx(189);
        {
            struct Task *t = gCurTask;

            t->velX = -0x8000;
            t->velY = -0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x8000;
            t->velY = 0;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x10000;
            t->velY = 0x8000;
        }
        TaskYieldTrampoline(4);
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velY = 0x20000;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 8:
        {
            struct Task *t = gCurTask;

            t->posX = (gViewRect[0] + 254) << 16;
            t->posY = (gViewRect[2] + 48) << 16;
        }
        TaskYieldTrampoline(224);
        {
            struct Task *t = gCurTask;

            t->frame = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x20000;
            t->unk1C = 256;
            t->unk2C = 16;
            t->velX = -0x40000;
            t->velY = 0x80000;
        }
        TaskYieldTrampoline(3);
        gCurTask->velY = 0x40000;
        TaskYieldTrampoline(3);
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(3);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(3);
        TaskStop();
        TaskYieldTrampoline(3);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(3);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->velX = -0x80000;
            t->velY = -0x20000;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 9:
        {
            struct Task *t = gCurTask;

            t->posX = (gViewRect[0] + 248) << 16;
            t->posY = (gViewRect[2] + 32) << 16;
        }
        TaskYieldTrampoline(80);
        TaskYieldTrampoline(192);
        {
            struct Task *t = gCurTask;

            t->frame = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x20000;
            t->unk1C = 256;
            t->unk2C = 16;
            t->velX = -0x80000;
            t->velY = 0x40000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x60000;
            t->velY = 0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x40000;
            t->velY = 0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x30000;
            t->velY = 0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x20000;
            t->velY = 0;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x14000;
            t->velY = -0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x10000;
            t->velY = -0xC000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0xC000;
            t->velY = -0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x8000;
            t->velY = -0x14000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = 0;
            t->velY = -0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = 0x8000;
            t->velY = -0x30000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = 0x10000;
            t->velY = -0x40000;
        }
        TaskYieldTrampoline(4);
        gCurTask->velX = 0x20000;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0x40000;
        TaskYieldTrampoline(4);
        gCurTask->unk6C = 0;
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 10:
        {
            struct Task *t = gCurTask;

            t->posX = (gViewRect[0] + 160) << 16;
            t->posY = (gViewRect[2] - 32) << 16;
        }
        TaskYieldTrampoline(128);
        {
            struct Task *t = gCurTask;

            t->frame = 6;
            t->unk18 = 0x3F0000;
            t->unk28 = -0x10000;
            t->unk1C = 0;
            t->unk2C = 16;
            t->velX = -0x2000;
            t->velY = 0x40000;
        }
        TaskYieldTrampoline(12);
        gCurTask->unk2C = 8;
        CreateBurstEffect(4, 256);
        PlaySfx(189);
        {
            struct Task *t = gCurTask;

            t->velX = -0x8000;
            t->velY = 0x20000;
        }
        TaskYieldTrampoline(4);
        gCurTask->velY = 0;
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->velX = -0x10000;
            t->velY = -0x8000;
        }
        TaskYieldTrampoline(4);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(4);
        gCurTask->unk6C = 0;
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 11:
        {
            struct Task *t = gCurTask;

            t->posX = (gViewRect[0] + 248) << 16;
            t->posY = (gViewRect[2]) << 16;
        }
        TaskYieldTrampoline(1);
        {
            struct Task *t = gCurTask;

            t->frame = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x20000;
            t->unk1C = 32;
            t->unk2C = 16;
            t->velX = -0x40000;
            t->velY = 0x20000;
        }
        TaskYieldTrampoline(16);
        {
            struct Task *t = gCurTask;

            t->unk2C = 8;
            t->velY = 0x10000;
        }
        TaskYieldTrampoline(12);
        TaskYieldTrampoline(16);
        {
            struct Task *t = gCurTask;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
        }
        CreateBurstEffect(4, 256);
        PlaySfx(189);
        {
            struct Task *t = gCurTask;

            t->velX = 0x2000;
            t->velY = 0x10000;
        }
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x10000;
        TaskYieldTrampoline(16);
        {
            struct Task *t = gCurTask;

            t->velX = 0x20000;
            t->velY = 0x20000;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->frame = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 8);
        break;
        }
    }
    TaskExitTrampoline();
}

void NightmarePowerOrbEscapeStarDraw(void)
{
    {
        struct Task *u = gCurTask;

        u->unk18 += u->unk28;
        if (u->unk18 < 0)
            u->unk18 = 0;
    }
    {
        struct Task *u = gCurTask;

        if (u->unk18 > 0x3F0000)
            u->unk18 = 0x3F0000;
    }
    {
        struct Task *u = gCurTask;

        u->unk1C += u->unk2C;
        if (u->unk1C < 0)
            u->unk1C += 512;
    }
    {
        struct Task *u = gCurTask;

        if (u->unk1C > 0x1FF)
            u->unk1C -= 512;
    }
    {
        struct Task *u = gCurTask;

        if (u->frameTable == NULL)
            return;
        if (u->frame == -1)
            return;
    }
    if (TaskIsOnScreen() == 0)
        return;
    {
    struct Task *t = gCurTask;
    u32 *g = t->frameTable;
    s32 gfx;

    if (t->unk18 != 63 || t->unk1C != 0)
    {
        gfx = DrawAffineSprite(g[t->frame], gUnk_0873FF98[t->unk18 >> 16],
                           gUnk_0873FF98[t->unk18 >> 16], (s16)t->unk1C);
        {
            struct Task *u = gCurTask;

            QueueSprite(u->layer, gfx, u->spriteFlags, u->tileWord,
                         u->pixelX - gSpriteCameraX, u->pixelY - gSpriteCameraY);
        }
    }
    else
    {
        QueueSprite(t->layer, g[t->frame], t->spriteFlags, t->tileWord,
                     t->pixelX - gSpriteCameraX, t->pixelY - gSpriteCameraY);
    }
    }
}

void sub_080761b4(void)
{
    u16 pal;

    pal = *(u16 *)gBgPalette;

    gCurTask->unk6C = 0;
    do
    {
        gDispCnt = 0xE0FF & gDispCnt;
        gDispCnt = 0x1100 | gDispCnt;
        LoadBackdropColor((u32)gUnk_08740098);
        TaskYieldTrampoline(3);
        gDispCnt = 0xE0FF & gDispCnt;
        gDispCnt = 0x1D00 | gDispCnt;
        LoadBackdropColor((u32)&pal);
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk6C = 0;
    do
    {
        gDispCnt = 0xE0FF & gDispCnt;
        gDispCnt = 0x1100 | gDispCnt;
        LoadBackdropColor((u32)gUnk_08740098);
        TaskYieldTrampoline(1);
        gDispCnt = 0xE0FF & gDispCnt;
        gDispCnt = 0x1D00 | gDispCnt;
        LoadBackdropColor((u32)&pal);
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 3);
    gCurTask->unk6C = 0;
    do
    {
        gDispCnt = 0xE0FF & gDispCnt;
        gDispCnt = 0x1D00 | gDispCnt;
        LoadBackdropColor((u32)&pal);
        TaskYieldTrampoline(2);
        gDispCnt = 0xE0FF & gDispCnt;
        gDispCnt = 0x1100 | gDispCnt;
        LoadBackdropColor((u32)gUnk_08740098);
        TaskYieldTrampoline(1);
        gDispCnt = 0xE0FF & gDispCnt;
        gDispCnt = 0x1D00 | gDispCnt;
        LoadBackdropColor((u32)&pal);
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    LoadBackdropColor((u32)&pal);
    TaskExitTrampoline();
}

void PlayerCannonInit(void)
{
    {
        struct Task *t = gCurTask;

        t->updateCallback = (u32)PlayerCannonUpdate;
        t->moveCallback = (u32)TaskMove;
    }
    TaskStop();
    gCurTask->facing = 1;
    {
        struct Task *t = gCurTask;

        t->spriteFlags &= 0x7FFF;
        t->layer = 7;
    }
    {
        struct Task *t = gCurTask;

        t->player->unk42 &= 0xFFEF;
        CallTableEntry(t->state, 6, gPlayerCannonStates);
    }
}

void PlayerCannonUpdate(void)
{
    u16 id;
    struct Task *t;

    CallTableEntry(gCurTask->updateState, 6, gPlayerCannonStateUpdates);
    id = gLocalPlayer;
    t = gCurTask;
    if (id == t->player->playerIndex && t->unk18 != 0)
        SetCameraFocus(t->unk1C, t->unk20);
}

void PlayerCannonEnterState(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMove;
    CallTableEntry(t->state, 6, gPlayerCannonStates);
}
