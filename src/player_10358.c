#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u32 gUnk_02007D00[];
extern u8 gObjPalette[];
extern u16 gPlayerPressedKeys;
extern s32 gUnk_03001F2C;
extern u16 gPlayerCount;
extern u16 gUnk_030023B8;
extern u16 gGameState;
extern struct Task *gCurTask;
extern struct Task gTasks[];
extern u32 gUnk_081AC358[];
extern u32 gUnk_081AC378[];
extern u32 gUnk_08731F78[];
extern u32 gCutsceneDurations[];
extern u32 gUnk_08731FA8[];
extern u32 gUnk_08731FC8[];
extern u32 gUnk_08751C44[];
extern u32 gUnk_08754A14[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void CallTableEntry(u32 a, u32 b, u32 *c);
void PlayBgm(u32 a);
void StopSfxOnPlayer(s32 player, s32 songId);
void TaskFree(s32 id);
s32 TaskCreateInRange(u32 type, s32 start, s32 end);
void TaskMove(void);
void TaskDrawScreen(void);
void TaskSleepForever(void);
void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void TaskStop(void);
void TaskSetFrame(s32 a);
void CutsceneCheckSkip(void);

s32 CreateCutsceneActor(s32 a, s32 b)
{
    s32 i;
    struct Task *t;
    struct Task *dst;

    i = TaskCreateInRange(92, b, 62);
    if (i != -1)
    {
        dst = &gTasks[i];
        t = gCurTask;
        dst->unk48 = t->unk48;
        dst->unk4A = t->unk4A;
        dst->unk4C = t->unk4C;
        dst->unk50 = t->unk50;
        dst->unk43 = t->unk43;
        dst->unk44 = gCurTaskIdx;
        dst->unk18 = a;
        if (gUnk_08731F78[*(s8 *)&gUnk_030023B8] != 0)
            dst->unk40 = 0x8810;
    }
    return i;
}

void CutsceneCheckSkip(void);   /* hdr.c lacks this in-module prototype */

void Task_CutsceneDirector(void)
{
    u16 *p;
    u16 *q;

    gCurTask->unk28 = 0;
    CallTableEntry(*(s8 *)&gUnk_030023B8, 9, gUnk_08731FA8);
    TaskYieldTrampoline(60);
    gCurTask->unk04 = (u32)CutsceneCheckSkip;
    gCurTask->unk6C = 0;
    p = (u16 *)gCutsceneDurations;
    if ((s16)gCurTask->unk6C < p[*(s8 *)&gUnk_030023B8] - 60)
    {
        q = (u16 *)gCutsceneDurations;
        do
        {
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C < q[*(s8 *)&gUnk_030023B8] - 60);
    }
    gGameState = 5;
    TaskSleepForever();
}

void CutsceneCheckSkip(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        u16 *p = &gPlayerPressedKeys;

        if ((p[i] & 9) != 0)
        {
            if (*(s8 *)&gUnk_030023B8 == 7 && gUnk_02007D00[0] != -1)
            {
                StopSfxOnPlayer(gUnk_02007D00[0], 0x21B);
                gUnk_02007D00[0] = -1;
            }
            gGameState = 5;
            TaskFree(gCurTaskIdx);
        }
    }
}

void Task_CutsceneActor(void)
{
    CallTableEntry(gCurTask->unk18, 63, gUnk_08731FC8);
}

void sub_0801050c(void)
{
    PlayBgm(0);
    CreateCutsceneActor(0, 0);
    CreateCutsceneActor(4, 32);
}

void sub_08010528(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk42 = 7;
    gCurTask->unk38 = gUnk_08754A14;
    gCurTask->unk4C = 150 << 16;
    gCurTask->unk50 = 186 << 15;
    TaskStop();
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(20);
    gCurTask->unk3C = 38;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 192 << 8, 0xFFFFE000, 0x5A5A5A5A);
    gCurTask->unk3C = 37;
    TaskYieldTrampoline(8);
    gCurTask->unk5C = 0xFFFFE000;
    gCurTask->unk3C = 38;
    TaskYieldTrampoline(3);
    TaskSetMotion(128 << 7, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 39;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 128 << 8;
    gCurTask->unk3C = 38;
    TaskYieldTrampoline(4);
    TaskStop();
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(20);
    gCurTask->unk3C = 38;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 192 << 8, 0xFFFFE000, 0x5A5A5A5A);
    gCurTask->unk3C = 37;
    TaskYieldTrampoline(8);
    gCurTask->unk5C = 0xFFFFE000;
    gCurTask->unk3C = 38;
    TaskYieldTrampoline(3);
    TaskSetMotion(128 << 6, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 39;
    TaskYieldTrampoline(10);
    gCurTask->unk5C = 128 << 2;
    gCurTask->unk3C = 41;
    TaskYieldTrampoline(10);
    gCurTask->unk6C = 0;
    do
    {
        TaskStop();
        gCurTask->unk3C = 14;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 15;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 15);
    TaskSetMotion(0xFFFE0000, 0, 0x5A5A5A5A, 160 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gCurTask->unk3C = 16;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFFC000;
    gCurTask->unk3C = 17;
    TaskYieldTrampoline(7);
    TaskStop();
    gCurTask->unk3C = 18;
    TaskYieldTrampoline(2);
    CreateCutsceneActor(1, 32);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C = 19;
        TaskYieldTrampoline(3);
        gCurTask->unk3C = 20;
        TaskYieldTrampoline(3);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    gCurTask->unk3C = 21;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFC0000;
    gCurTask->unk3C = 23;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 192 << 11;
    gCurTask->unk3C = 24;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFD0000;
    gCurTask->unk3C = 25;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 128 << 10;
    gCurTask->unk3C = 26;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFF0000;
    gCurTask->unk3C = 27;
    TaskYieldTrampoline(2);
    CreateCutsceneActor(2, 32);
    TaskStop();
    gCurTask->unk3C = 28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 30;
    TaskYieldTrampoline(4);
    gCurTask->unk3C = 31;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 22;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 30;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 22;
    TaskYieldTrampoline(8);
    gCurTask->unk3C = 32;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 33;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 34;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 35;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 36;
    TaskSleepForever();
}

/* Not in hdr.c yet: the two task entry points this state installs. */
void sub_080109c8(void);
void sub_08010b38(void);

/* 16-byte per-slot record, 3 slots per player, at gUnk_02007E90.
   unk00/unk04 are 16.16 (x,y), unk08 a 16.16 y-delta, unk0C a timer,
   unk0D a frame/anim id.  hdr.c only has `extern u32 gUnk_02007E90[]`, so
   the real 2-D shape is aliased in here. */
struct M04Spark
{
    /*0x00*/ s32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ u8 unk0C;
    /*0x0D*/ u8 unk0D;
    /*0x0E*/ u16 unk0E;
};

extern struct M04Spark gUnk_02007E90[][3];

/* OBJ VRAM tile base; not in hdr.c. */
extern u8 gObjVram[];

void sub_08010834(void)
{
    s32 t;
    s32 u;
    u8 *dst = gObjVram;

    gUnk_03001F2C = 0;
    do
    {
        gUnk_02007E90[gCurTask->unk88->unk00][gUnk_03001F2C].unk00 = 0;
        gUnk_02007E90[gCurTask->unk88->unk00][gUnk_03001F2C].unk04 = 0;
        gUnk_02007E90[gCurTask->unk88->unk00][gUnk_03001F2C].unk08 = 0;
        gUnk_02007E90[gCurTask->unk88->unk00][gUnk_03001F2C].unk0C = 1;
        gUnk_02007E90[gCurTask->unk88->unk00][gUnk_03001F2C].unk0D = 0;
        gUnk_03001F2C++;
    } while (gUnk_03001F2C <= 2);
    RequestCopy(1, (u32)gUnk_081AC378, (u32)(dst + 384), 128);
    RequestCopy(1, (u32)gUnk_081AC378 + 128, (u32)(dst + 1408), 128);
    RequestCopy(1, (u32)gUnk_081AC378 + 256, (u32)(dst + 2432), 128);
    RequestCopy(1, (u32)gUnk_081AC378 + 384, (u32)(dst + 3456), 128);
    RequestCopy(2, (u32)gUnk_081AC358, (u32)gObjPalette, 32);
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)sub_08010b38;
    gCurTask->unk04 = (u32)sub_080109c8;
    gCurTask->unk42 = 5;
    gCurTask->unk38 = gUnk_08751C44;
    gCurTask->unk40 = 0x1004;
    gCurTask->unk28 = 0;
    gCurTask->unk6C = 0;
    do
    {
        t = gCurTask->unk28;
        u = *(s16 *)&gCurTask->unk28;
        gCurTask->unk28 = t + 1;
        TaskSetFrame(u);
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 8);
    TaskExitTrampoline();
}
