#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];
extern u32 gUnk_0874C600[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_080031b8(s32 id);
void sub_080059d8(void);
void sub_08005ca0(void);
void sub_0800625c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void sub_080062c4(void);
void sub_08010358(s32 a, s32 b);

void sub_08018b84(void)
{
    struct Task *t;
    s32 v;

    v = gUnk_03002490->unk20;
    if (v < 0)
        return;
    if (v == 0)
        sub_080031b8(0x111);
    t = gUnk_03002490;
    v = t->unk20 + 1;
    t->unk20 = v;
    if (v > 2)
        t->unk20 = 0;
}

void sub_08018bb8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08005ca0;
    t->unk42 = 10;
    gUnk_03002490->unk38 = gUnk_0874C600;
    gUnk_03002490->unk40 = 0;
    while (1)
    {
        gUnk_03002490->unk3C = 0xFFFF;
        if (gUnk_03002790[gUnk_03002490->unk44].unk28 == 0)
        {
            do
            {
                TaskYieldTrampoline(1);
            } while (gUnk_03002790[gUnk_03002490->unk44].unk28 == 0);
        }
        u = gUnk_03002490;
        u->unk4C = gUnk_03002790[u->unk44].unk48 << 16;
        u->unk50 = (gUnk_03002790[u->unk44].unk4A + 24) << 16;
        sub_0800625c(0xFFFD0000, 0, 0x5A5A5A5A, 0, 0xFFFFE000, 0x5A5A5A5A);
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(1);
        sub_08010358(54, 32);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        sub_080062c4();
        v = gUnk_03002490;
        v->unk4C = gUnk_03002790[v->unk44].unk48 << 16;
        v->unk50 = (gUnk_03002790[v->unk44].unk4A + 24) << 16;
        sub_0800625c(0xFFFDC000, 0xFFFFC000, 0x5A5A5A5A, 192 << 5, 0xFFFFE000,
                     0x5A5A5A5A);
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 8;
        TaskYieldTrampoline(1);
    }
}

void sub_08018d7c(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;

    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08005ca0;
    t->unk42 = 9;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874C600;
    u->unk4C = gUnk_03002790[u->unk44].unk48 << 16;
    u->unk50 = gUnk_03002790[u->unk44].unk4A << 16;
    u->unk5C = 0x4000;
    u->unk60 = 0xFFFFE000;
    u->unk3C = 4;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}
