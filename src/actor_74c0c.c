/* game_code_and_rodata 0x08074C0C-0x080763E8 (issue #79, module M19 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08074C0C 0x080763E8 src/actor_74c0c.c --newpb
 *
 * M19 batch 3: task type #165 (sub_08074c0c, the four-ring sparkle draw loop),
 * type #97 (sub_0807450c) and type #98 (sub_08075000) with their coroutine
 * bodies and the gUnk_0873FC94 dispatch row.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"


/* RAM cells and ROM tables */
extern s16 gViewRect[];
extern s16 gSpriteCameraX;
extern s16 gSpriteCameraY;
extern s16 gUnk_0873FCF8[];
extern s16 gUnk_0873FD20[];
extern s16 gUnk_0873FD48[];
extern s16 gUnk_0873FD70[];
extern s16 gUnk_0873FF98[];
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern struct PlayerState gUnk_03002170[];
extern struct Task * gCurTask;
extern struct Task gTasks[];
extern u16 gLocalPlayer;
extern u16 gPlayerCount;
extern u32 gUnk_080D21C8[];
extern u32 gUnk_085E6FA4[];
extern u32 gUnk_085E6FE4[];
extern u32 gUnk_085E72D4[];
extern u32 gUnk_0873FD98[];
extern u32 gUnk_0873FE98[];
extern u32 gUnk_08740098[];
extern u32 gUnk_087400B0[];
extern u32 gUnk_087400C8[];
extern u32 gUnk_0874C44C[];
extern u32 gUnk_0874C500[];
extern u32 gUnk_0875549C[];
extern u8 gUnk_02005E10[];
extern u8 gUnk_0200AF20[];
extern u8 gBgPalette[];
extern u8 gUnk_03001370[];
extern u8 gUnk_03002340;
extern vu16 gDispCnt;

/* callees */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern s32 PlaySfx(u32 a);
extern s32 TaskCreateFrom(u32 type, s32 idx);
extern u32 RandomRange(u32 range);
extern u32 TaskIsOnScreen(void);
extern void TaskExitTrampoline(void);
extern void TaskYieldTrampoline(u32 a);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void BlendColors(void *src, void *dst, s32 ratio, s32 count, void *out);
extern void TaskMove(void);
extern void TaskSleepForever(void);
extern void TaskStop(void);
extern void sub_08026264(s32 a, s32 b);
extern void AngleToVector(s16 t, s16 mag);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void sub_080652c8(void);
extern void LoadBackdropColor(u32 src);
extern void sub_0806d4e4(u32 a, s32 b);

/* defined below */
void sub_08075290(s32 a);
void sub_08076074(void);
void sub_0807637c(void);

void sub_08074c0c(void)
{
    {
        struct Task *t = gCurTask;

        t->unk00 = (u32)TaskMove;
        t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
        t->unk42 = 12;
    }
    {
        struct Task *t = gCurTask;

        t->unk40 = 0;
        if (gTasks[t->unk44].unk18 < 0)
            t->unk40 |= 0xC00;
        else
            t->unk40 &= 0xF3FF;
    }
    switch (gCurTask->unk14)
    {
    case 1:
        AngleToVector(gCurTask->unk28, gCurTask->unk2C);
        {
            struct Task *t = gCurTask;

            t->unk54 = gUnk_030023B4;
            t->unk58 = gUnk_030023D4;
            t->unk38 = gUnk_0874C44C;
        }
        gCurTask->unk3C = RandomRange(2) + 4;
        TaskSleepForever();
        break;
    case 0:
        AngleToVector(gCurTask->unk28, gCurTask->unk2C);
        {
            struct Task *t = gCurTask;

            t->unk54 = gUnk_030023B4;
            t->unk58 = gUnk_030023D4;
            t->unk38 = gUnk_0874C500;
        }
        gCurTask->unk3C = RandomRange(8);
        TaskSleepForever();
        break;
    case 2:
        {
            struct Task *t = gCurTask;

            t->unk0C = 0;
            t->unk6C = 0;
        }
        do
        {
            {
                struct Task *t = gCurTask;

                QueueSprite(1, (u32)gUnk_080D21C8, t->unk3E, t->unk40,
                             t->unk48 + gUnk_0873FCF8[(s16)t->unk6C * 2],
                             t->unk4A + gUnk_0873FCF8[(s16)t->unk6C * 2 + 1]);
            }
            {
                struct Task *t = gCurTask;

                QueueSprite(1, (u32)gUnk_080D21C8, t->unk3E, t->unk40,
                             t->unk48 + gUnk_0873FD20[(s16)t->unk6C * 2],
                             t->unk4A + gUnk_0873FD20[(s16)t->unk6C * 2 + 1]);
            }
            {
                struct Task *t = gCurTask;

                QueueSprite(1, (u32)gUnk_080D21C8, t->unk3E, t->unk40,
                             t->unk48 + gUnk_0873FD48[(s16)t->unk6C * 2],
                             t->unk4A + gUnk_0873FD48[(s16)t->unk6C * 2 + 1]);
            }
            {
                struct Task *t = gCurTask;

                QueueSprite(1, (u32)gUnk_080D21C8, t->unk3E, t->unk40,
                             t->unk48 + gUnk_0873FD70[(s16)t->unk6C * 2],
                             t->unk4A + gUnk_0873FD70[(s16)t->unk6C * 2 + 1]);
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
        if ((gUnk_03002340 >> i) & 1)
        {
            struct PlayerState *p = &gUnk_03002170[i];

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

void sub_08075000(void)
{
    {
        struct Task *t = gCurTask;

        t->unk00 = (u32)TaskMove;
        t->unk0C = (u32)sub_080652c8;
        t->unk42 = 11;
    }
    gCurTask->unk38 = gUnk_0875549C;
    RequestCopy(2, (u32)gUnk_085E6FA4, 0x030015B0, 64);
    LZ77UnCompWram(gUnk_085E6FE4, (void *)0x02020000);
    RequestCopy(4, 0x02020000, 0x06013000, 0x800);
    LZ77UnCompWram(gUnk_085E72D4, (void *)0x02020000);
    RequestCopy(4, 0x02020000, 0x06014000, 0x1000);
    {
        struct Task *t = gCurTask;

        t->unk40 = 0xA990;
        t->unk4C = 0xD00000;
        t->unk50 = 0x1000000;
        t->unk3C = 0;
        t->unk54 = -0x2000;
    }
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C = 1;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 0;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 8);
    sub_08074e8c();
    gCurTask->unk6C = 0;
    do
    {
        sub_08074f48(0);
        gCurTask->unk3C = 2;
        TaskYieldTrampoline(2);
        sub_08074f48(3);
        gCurTask->unk3C = 1;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 8);
    gCurTask->unk6C = 0;
    do
    {
        sub_08074f48(0);
        gCurTask->unk3C = 3;
        TaskYieldTrampoline(2);
        sub_08074f48(1);
        gCurTask->unk3C = 2;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 8);
    gCurTask->unk6C = 0;
    do
    {
        sub_08074f48(1);
        gCurTask->unk3C = 4;
        TaskYieldTrampoline(2);
        sub_08074f48(2);
        gCurTask->unk3C = 3;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 8);
    sub_08074f48(2);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 4;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 4;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 4;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 4;
    TaskYieldTrampoline(2);
    sub_08074f48(1);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 4;
    TaskYieldTrampoline(2);
    sub_08074f48(0);
    TaskYieldTrampoline(4);
    sub_08074f48(3);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(12);
    {
        struct Task *t = gCurTask;

        t->unk54 = -0x800;
        t->unk3C = 5;
    }
    TaskYieldTrampoline(76);
    sub_08075290(12);
    TaskYieldTrampoline(52);
    sub_08075290(0);
    gCurTask->unk54 = 0x20000;
    TaskYieldTrampoline(16);
    sub_08075290(1);
    gCurTask->unk54 = 0x40000;
    TaskYieldTrampoline(19);
    sub_08075290(2);
    TaskExitTrampoline();
}

void sub_08075290(s32 a)
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
            t->unk73 = a;
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
            t->unk73 = a;
        }
    }
}

void sub_080752f4(void)
{
    {
        struct Task *t = gCurTask;

        t->unk00 = (u32)TaskMove;
        t->unk0C = (u32)sub_08076074;
        t->unk42 = 12;
    }
    {
        struct Task *t = gCurTask;

        t->unk38 = gUnk_0875549C;
        t->unk40 = 0xA210;
    }
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->unk3C = 0xFFFF;
        t->unk18 = 0;
        t->unk1C = -1;
        switch (t->unk73)
        {
    case 0:
        {
            struct Task *t = gCurTask;

            t->unk4C = (gViewRect[0] + 168) << 16;
            t->unk50 = (gViewRect[2] + 40) << 16;
        }
        {
            struct Task *t = gCurTask;

            t->unk3C = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x50000;
            t->unk1C = 0;
            t->unk2C = 16;
            t->unk54 = -0x40000;
            t->unk58 = -0x80000;
        }
        TaskYieldTrampoline(3);
        gCurTask->unk58 = -0x40000;
        TaskYieldTrampoline(3);
        gCurTask->unk58 = -0x10000;
        TaskYieldTrampoline(3);
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
        }
        TaskStop();
        TaskYieldTrampoline(3);
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(3);
        gCurTask->unk58 = 0x10000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x80000;
            t->unk58 = 0x20000;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 1:
        {
            struct Task *t = gCurTask;

            t->unk4C = (gViewRect[0] + 248) << 16;
            t->unk50 = (gViewRect[2] + 40) << 16;
        }
        TaskYieldTrampoline(35);
        {
            struct Task *t = gCurTask;

            t->unk3C = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x50000;
            t->unk1C = 0;
            t->unk2C = 16;
            t->unk54 = -0x40000;
            t->unk58 = -0x80000;
        }
        TaskYieldTrampoline(3);
        gCurTask->unk58 = -0x40000;
        TaskYieldTrampoline(3);
        gCurTask->unk58 = -0x10000;
        TaskYieldTrampoline(3);
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
        }
        TaskStop();
        TaskYieldTrampoline(3);
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(3);
        gCurTask->unk58 = 0x10000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x80000;
            t->unk58 = 0x20000;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 2:
        {
            struct Task *t = gCurTask;

            t->unk4C = (gViewRect[0] + 200) << 16;
            t->unk50 = (gViewRect[2] + 40) << 16;
        }
        TaskYieldTrampoline(17);
        {
            struct Task *t = gCurTask;

            t->unk3C = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x20000;
            t->unk1C = 0;
            t->unk2C = 16;
            t->unk54 = -0x40000;
            t->unk58 = 0x40000;
        }
        TaskYieldTrampoline(12);
        gCurTask->unk58 = 0x20000;
        TaskYieldTrampoline(10);
        gCurTask->unk58 = 0x10000;
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x20000;
            t->unk58 = 0x8000;
        }
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(8);
        gCurTask->unk58 = -0x10000;
        TaskYieldTrampoline(6);
        gCurTask->unk58 = -0x20000;
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x10000;
            t->unk58 = -0x40000;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 3:
        {
            struct Task *t = gCurTask;

            t->unk4C = (gViewRect[0] + 240) << 16;
            t->unk50 = (gViewRect[2] + 64) << 16;
        }
        TaskYieldTrampoline(128);
        {
            struct Task *t = gCurTask;

            t->unk3C = 6;
            t->unk18 = 0x3F0000;
            t->unk28 = -0x10000;
            t->unk1C = 0;
            t->unk2C = -16;
            t->unk54 = -0x60000;
            t->unk58 = 0x40000;
        }
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x40000;
            t->unk58 = 0x20000;
        }
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x60000;
            t->unk58 = -0x20000;
        }
        TaskYieldTrampoline(5);
        gCurTask->unk2C = -8;
        sub_0806d4e4(4, 256);
        PlaySfx(189);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x40000;
            t->unk58 = -0x40000;
        }
        TaskYieldTrampoline(6);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x60000;
            t->unk58 = -0x20000;
        }
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x40000;
            t->unk58 = 0x20000;
        }
        TaskYieldTrampoline(6);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x60000;
            t->unk58 = 0x40000;
        }
        TaskYieldTrampoline(5);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x40000;
            t->unk58 = 0x10000;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 4:
        {
            struct Task *t = gCurTask;

            t->unk4C = (gViewRect[0] + 248) << 16;
            t->unk50 = (gViewRect[2] + 88) << 16;
        }
        TaskYieldTrampoline(160);
        {
            struct Task *t = gCurTask;

            t->unk3C = 6;
            t->unk18 = 0x3F0000;
            t->unk28 = -0x10000;
            t->unk1C = 0;
            t->unk2C = -16;
            t->unk54 = -0x80000;
            t->unk58 = -0x80000;
        }
        TaskYieldTrampoline(8);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x60000;
            t->unk58 = -0x40000;
        }
        TaskYieldTrampoline(7);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x40000;
            t->unk58 = -0x20000;
        }
        TaskYieldTrampoline(6);
        gCurTask->unk2C = -8;
        sub_0806d4e4(4, 256);
        PlaySfx(189);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x30000;
            t->unk58 = -0x10000;
        }
        TaskYieldTrampoline(5);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x20000;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x14000;
            t->unk58 = 0;
        }
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0xC000;
            t->unk58 = 0x8000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x8000;
            t->unk58 = 0x10000;
            t->unk28 = 0;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 16);
        break;
    case 5:
        {
            struct Task *t = gCurTask;

            t->unk4C = (gViewRect[0] + 248) << 16;
            t->unk50 = (gViewRect[2] + 16) << 16;
        }
        TaskYieldTrampoline(192);
        {
            struct Task *t = gCurTask;

            t->unk3C = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x20000;
            t->unk1C = 0;
            t->unk2C = 16;
            t->unk54 = -0x80000;
            t->unk58 = -0x40000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x60000;
            t->unk58 = -0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x40000;
            t->unk58 = -0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x30000;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x20000;
            t->unk58 = 0;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x14000;
            t->unk58 = 0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x10000;
            t->unk58 = 0xC000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0xC000;
            t->unk58 = 0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
            t->unk54 = -0x8000;
            t->unk58 = 0x14000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = 0;
            t->unk58 = 0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = 0x8000;
            t->unk58 = 0x30000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = 0x10000;
            t->unk58 = 0x40000;
        }
        TaskYieldTrampoline(4);
        gCurTask->unk54 = 0x20000;
        TaskYieldTrampoline(4);
        gCurTask->unk54 = 0x40000;
        TaskYieldTrampoline(4);
        gCurTask->unk6C = 0;
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 6:
        {
            struct Task *t = gCurTask;

            t->unk4C = (gViewRect[0] + 248) << 16;
            t->unk50 = (gViewRect[2] + 80) << 16;
        }
        TaskYieldTrampoline(224);
        {
            struct Task *t = gCurTask;

            t->unk3C = 6;
            t->unk18 = 0x3F0000;
            t->unk28 = -0x10000;
            t->unk1C = 0;
            t->unk2C = -16;
            t->unk54 = -0x40000;
            t->unk58 = 0x10000;
        }
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(12);
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(4);
        gCurTask->unk2C = -8;
        sub_0806d4e4(4, 256);
        PlaySfx(189);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x40000;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(4);
        gCurTask->unk58 = -0x10000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(18);
        gCurTask->unk58 = 0x10000;
        TaskYieldTrampoline(2);
        break;
    case 7:
        {
            struct Task *t = gCurTask;

            t->unk4C = (gViewRect[0] + 120) << 16;
            t->unk50 = (gViewRect[2] + 120) << 16;
        }
        TaskYieldTrampoline(128);
        {
            struct Task *t = gCurTask;

            t->unk3C = 6;
            t->unk18 = 0x3F0000;
            t->unk28 = -0x20000;
            t->unk1C = 0;
            t->unk2C = -16;
            t->unk54 = -0x2000;
            t->unk58 = -0x40000;
        }
        TaskYieldTrampoline(12);
        gCurTask->unk2C = -8;
        sub_0806d4e4(4, 256);
        PlaySfx(189);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x8000;
            t->unk58 = -0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x8000;
            t->unk58 = 0;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x10000;
            t->unk58 = 0x8000;
        }
        TaskYieldTrampoline(4);
        gCurTask->unk58 = 0x10000;
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk58 = 0x20000;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 8:
        {
            struct Task *t = gCurTask;

            t->unk4C = (gViewRect[0] + 254) << 16;
            t->unk50 = (gViewRect[2] + 48) << 16;
        }
        TaskYieldTrampoline(224);
        {
            struct Task *t = gCurTask;

            t->unk3C = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x20000;
            t->unk1C = 256;
            t->unk2C = 16;
            t->unk54 = -0x40000;
            t->unk58 = 0x80000;
        }
        TaskYieldTrampoline(3);
        gCurTask->unk58 = 0x40000;
        TaskYieldTrampoline(3);
        gCurTask->unk58 = 0x10000;
        TaskYieldTrampoline(3);
        gCurTask->unk58 = 0x8000;
        TaskYieldTrampoline(3);
        TaskStop();
        TaskYieldTrampoline(3);
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(3);
        gCurTask->unk58 = -0x10000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x80000;
            t->unk58 = -0x20000;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 9:
        {
            struct Task *t = gCurTask;

            t->unk4C = (gViewRect[0] + 248) << 16;
            t->unk50 = (gViewRect[2] + 32) << 16;
        }
        TaskYieldTrampoline(80);
        TaskYieldTrampoline(192);
        {
            struct Task *t = gCurTask;

            t->unk3C = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x20000;
            t->unk1C = 256;
            t->unk2C = 16;
            t->unk54 = -0x80000;
            t->unk58 = 0x40000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x60000;
            t->unk58 = 0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x40000;
            t->unk58 = 0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x30000;
            t->unk58 = 0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x20000;
            t->unk58 = 0;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x14000;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x10000;
            t->unk58 = -0xC000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0xC000;
            t->unk58 = -0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x8000;
            t->unk58 = -0x14000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = 0;
            t->unk58 = -0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = 0x8000;
            t->unk58 = -0x30000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = 0x10000;
            t->unk58 = -0x40000;
        }
        TaskYieldTrampoline(4);
        gCurTask->unk54 = 0x20000;
        TaskYieldTrampoline(4);
        gCurTask->unk54 = 0x40000;
        TaskYieldTrampoline(4);
        gCurTask->unk6C = 0;
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 10:
        {
            struct Task *t = gCurTask;

            t->unk4C = (gViewRect[0] + 160) << 16;
            t->unk50 = (gViewRect[2] - 32) << 16;
        }
        TaskYieldTrampoline(128);
        {
            struct Task *t = gCurTask;

            t->unk3C = 6;
            t->unk18 = 0x3F0000;
            t->unk28 = -0x10000;
            t->unk1C = 0;
            t->unk2C = 16;
            t->unk54 = -0x2000;
            t->unk58 = 0x40000;
        }
        TaskYieldTrampoline(12);
        gCurTask->unk2C = 8;
        sub_0806d4e4(4, 256);
        PlaySfx(189);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x8000;
            t->unk58 = 0x20000;
        }
        TaskYieldTrampoline(4);
        gCurTask->unk58 = 0;
        TaskYieldTrampoline(4);
        {
            struct Task *t = gCurTask;

            t->unk54 = -0x10000;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(4);
        gCurTask->unk58 = -0x10000;
        TaskYieldTrampoline(4);
        gCurTask->unk6C = 0;
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        break;
    case 11:
        {
            struct Task *t = gCurTask;

            t->unk4C = (gViewRect[0] + 248) << 16;
            t->unk50 = (gViewRect[2]) << 16;
        }
        TaskYieldTrampoline(1);
        {
            struct Task *t = gCurTask;

            t->unk3C = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x20000;
            t->unk1C = 32;
            t->unk2C = 16;
            t->unk54 = -0x40000;
            t->unk58 = 0x20000;
        }
        TaskYieldTrampoline(16);
        {
            struct Task *t = gCurTask;

            t->unk2C = 8;
            t->unk58 = 0x10000;
        }
        TaskYieldTrampoline(12);
        TaskYieldTrampoline(16);
        {
            struct Task *t = gCurTask;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
        }
        sub_0806d4e4(4, 256);
        PlaySfx(189);
        {
            struct Task *t = gCurTask;

            t->unk54 = 0x2000;
            t->unk58 = 0x10000;
        }
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x10000;
        TaskYieldTrampoline(16);
        {
            struct Task *t = gCurTask;

            t->unk54 = 0x20000;
            t->unk58 = 0x20000;
            t->unk6C = 0;
        }
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 6;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 8);
        break;
        }
    }
    TaskExitTrampoline();
}

void sub_08076074(void)
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

        if (u->unk38 == NULL)
            return;
        if (u->unk3C == -1)
            return;
    }
    if (TaskIsOnScreen() == 0)
        return;
    {
    struct Task *t = gCurTask;
    u32 *g = t->unk38;
    s32 gfx;

    if (t->unk18 != 63 || t->unk1C != 0)
    {
        gfx = DrawAffineSprite(g[t->unk3C], gUnk_0873FF98[t->unk18 >> 16],
                           gUnk_0873FF98[t->unk18 >> 16], (s16)t->unk1C);
        {
            struct Task *u = gCurTask;

            QueueSprite(u->unk42, gfx, u->unk3E, u->unk40,
                         u->unk48 - gSpriteCameraX, u->unk4A - gSpriteCameraY);
        }
    }
    else
    {
        QueueSprite(t->unk42, g[t->unk3C], t->unk3E, t->unk40,
                     t->unk48 - gSpriteCameraX, t->unk4A - gSpriteCameraY);
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

void sub_08076318(void)
{
    {
        struct Task *t = gCurTask;

        t->unk04 = (u32)sub_0807637c;
        t->unk00 = (u32)TaskMove;
    }
    TaskStop();
    gCurTask->unk43 = 1;
    {
        struct Task *t = gCurTask;

        t->unk3E &= 0x7FFF;
        t->unk42 = 7;
    }
    {
        struct Task *t = gCurTask;

        t->unk88->unk42 &= 0xFFEF;
        CallTableEntry(t->unk14, 6, gUnk_087400B0);
    }
}

void sub_0807637c(void)
{
    u16 id;
    struct Task *t;

    CallTableEntry(gCurTask->unk15, 6, gUnk_087400C8);
    id = gLocalPlayer;
    t = gCurTask;
    if (id == t->unk88->unk00 && t->unk18 != 0)
        sub_08026264(t->unk1C, t->unk20);
}

void sub_080763c4(void)
{
    struct Task *t = gCurTask;

    t->unk00 = (u32)TaskMove;
    CallTableEntry(t->unk14, 6, gUnk_087400B0);
}
