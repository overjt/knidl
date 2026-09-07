#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u32 gUnk_02007D00[];
extern u8 gUnk_03001470[];
extern u16 gUnk_03001EB8;
extern s32 gUnk_03001F2C;
extern u16 gUnk_030023AC;
extern u16 gUnk_030023B8;
extern u16 gUnk_030023D8;
extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];
extern u32 gUnk_081AC358[];
extern u32 gUnk_081AC378[];
extern u32 gUnk_08731F78[];
extern u32 gUnk_08731F98[];
extern u32 gUnk_08731FA8[];
extern u32 gUnk_08731FC8[];
extern u32 gUnk_08751C44[];
extern u32 gUnk_08754A14[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_08002e98(u32 a, u32 b, u32 *c);
void sub_08003110(u32 a);
void sub_080034f0(s32 player, s32 songId);
void sub_08005654(s32 id);
s32 sub_08005904(u32 type, s32 start, s32 end);
void sub_080059d8(void);
void sub_08005ca0(void);
void sub_08006138(void);
void sub_0800625c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void sub_080062c4(void);
void sub_08006338(s32 a);
void sub_08010480(void);

s32 sub_08010358(s32 a, s32 b)
{
    s32 i;
    struct Task *t;
    struct Task *dst;

    i = sub_08005904(92, b, 62);
    if (i != -1)
    {
        dst = &gUnk_03002790[i];
        t = gUnk_03002490;
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

void sub_08010480(void);   /* hdr.c lacks this in-module prototype */

void sub_080103f0(void)
{
    u16 *p;
    u16 *q;

    gUnk_03002490->unk28 = 0;
    sub_08002e98(*(s8 *)&gUnk_030023B8, 9, gUnk_08731FA8);
    TaskYieldTrampoline(60);
    gUnk_03002490->unk04 = (u32)sub_08010480;
    gUnk_03002490->unk6C = 0;
    p = (u16 *)gUnk_08731F98;
    if ((s16)gUnk_03002490->unk6C < p[*(s8 *)&gUnk_030023B8] - 60)
    {
        q = (u16 *)gUnk_08731F98;
        do
        {
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C < q[*(s8 *)&gUnk_030023B8] - 60);
    }
    gUnk_030023D8 = 5;
    sub_08006138();
}

void sub_08010480(void)
{
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        u16 *p = &gUnk_03001EB8;

        if ((p[i] & 9) != 0)
        {
            if (*(s8 *)&gUnk_030023B8 == 7 && gUnk_02007D00[0] != -1)
            {
                sub_080034f0(gUnk_02007D00[0], 0x21B);
                gUnk_02007D00[0] = -1;
            }
            gUnk_030023D8 = 5;
            sub_08005654(gCurTaskIdx);
        }
    }
}

void sub_080104f0(void)
{
    sub_08002e98(gUnk_03002490->unk18, 63, gUnk_08731FC8);
}

void sub_0801050c(void)
{
    sub_08003110(0);
    sub_08010358(0, 0);
    sub_08010358(4, 32);
}

void sub_08010528(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk38 = gUnk_08754A14;
    gUnk_03002490->unk4C = 150 << 16;
    gUnk_03002490->unk50 = 186 << 15;
    sub_080062c4();
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(20);
    gUnk_03002490->unk3C = 38;
    TaskYieldTrampoline(2);
    sub_0800625c(128 << 8, 0, 0x5A5A5A5A, 192 << 8, 0xFFFFE000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 37;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk5C = 0xFFFFE000;
    gUnk_03002490->unk3C = 38;
    TaskYieldTrampoline(3);
    sub_0800625c(128 << 7, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 39;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk54 = 128 << 8;
    gUnk_03002490->unk3C = 38;
    TaskYieldTrampoline(4);
    sub_080062c4();
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(20);
    gUnk_03002490->unk3C = 38;
    TaskYieldTrampoline(2);
    sub_0800625c(128 << 8, 0, 0x5A5A5A5A, 192 << 8, 0xFFFFE000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 37;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk5C = 0xFFFFE000;
    gUnk_03002490->unk3C = 38;
    TaskYieldTrampoline(3);
    sub_0800625c(128 << 6, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 39;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk5C = 128 << 2;
    gUnk_03002490->unk3C = 41;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080062c4();
        gUnk_03002490->unk3C = 14;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 15;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 15);
    sub_0800625c(0xFFFE0000, 0, 0x5A5A5A5A, 160 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 16;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFFC000;
    gUnk_03002490->unk3C = 17;
    TaskYieldTrampoline(7);
    sub_080062c4();
    gUnk_03002490->unk3C = 18;
    TaskYieldTrampoline(2);
    sub_08010358(1, 32);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 19;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 20;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    gUnk_03002490->unk3C = 21;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFC0000;
    gUnk_03002490->unk3C = 23;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 192 << 11;
    gUnk_03002490->unk3C = 24;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFD0000;
    gUnk_03002490->unk3C = 25;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 128 << 10;
    gUnk_03002490->unk3C = 26;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFF0000;
    gUnk_03002490->unk3C = 27;
    TaskYieldTrampoline(2);
    sub_08010358(2, 32);
    sub_080062c4();
    gUnk_03002490->unk3C = 28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 30;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 31;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 22;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 30;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 22;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C = 32;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 33;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 34;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 35;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 36;
    sub_08006138();
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

extern struct M04Spark m04Sparks[][3] __asm__("gUnk_02007E90");

/* OBJ VRAM tile base; not in hdr.c. */
extern u8 gUnk_06010000[];

void sub_08010834(void)
{
    s32 t;
    s32 u;
    u8 *dst = gUnk_06010000;

    gUnk_03001F2C = 0;
    do
    {
        m04Sparks[gUnk_03002490->unk88->unk00][gUnk_03001F2C].unk00 = 0;
        m04Sparks[gUnk_03002490->unk88->unk00][gUnk_03001F2C].unk04 = 0;
        m04Sparks[gUnk_03002490->unk88->unk00][gUnk_03001F2C].unk08 = 0;
        m04Sparks[gUnk_03002490->unk88->unk00][gUnk_03001F2C].unk0C = 1;
        m04Sparks[gUnk_03002490->unk88->unk00][gUnk_03001F2C].unk0D = 0;
        gUnk_03001F2C++;
    } while (gUnk_03001F2C <= 2);
    sub_080017e4(1, (u32)gUnk_081AC378, (u32)(dst + 384), 128);
    sub_080017e4(1, (u32)gUnk_081AC378 + 128, (u32)(dst + 1408), 128);
    sub_080017e4(1, (u32)gUnk_081AC378 + 256, (u32)(dst + 2432), 128);
    sub_080017e4(1, (u32)gUnk_081AC378 + 384, (u32)(dst + 3456), 128);
    sub_080017e4(2, (u32)gUnk_081AC358, (u32)gUnk_03001470, 32);
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08010b38;
    gUnk_03002490->unk04 = (u32)sub_080109c8;
    gUnk_03002490->unk42 = 5;
    gUnk_03002490->unk38 = gUnk_08751C44;
    gUnk_03002490->unk40 = 0x1004;
    gUnk_03002490->unk28 = 0;
    gUnk_03002490->unk6C = 0;
    do
    {
        t = gUnk_03002490->unk28;
        u = *(s16 *)&gUnk_03002490->unk28;
        gUnk_03002490->unk28 = t + 1;
        sub_08006338(u);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 8);
    TaskDispatchTrampoline();
}
