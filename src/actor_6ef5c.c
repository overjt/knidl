/* game_code_and_rodata 0x0806EF5C-0x0806FF24 (issue #64, module M18 batch 7).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806EF5C 0x0806FF24 pending/batch7/actor_6ef5c.c --newpb
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"


extern u16 gUnk_03002360;
extern u8 gUnk_0873F5D4[];
extern s16 gUnk_0873D3B8[][2];
extern u16 gUnk_0873D384[];

extern void TaskYieldTrampoline(u32 a);

extern void sub_08006138(void);
extern void sub_080061c0(u32, u32);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_0801bcac(u8 *);
extern void sub_08026264(s32, s32);
extern void sub_0803ddc0(void);
extern void sub_08049738(void);
extern void sub_08049a58(void);
extern void sub_08070208(void);
extern void sub_08070264(void);
extern void sub_0807029c(void);
extern void sub_0807042c(void);
extern void sub_08070614(u32);
extern void sub_080706a8(void);

void sub_0806ef5c(void)
{
    s32 d, n, i;
    u8 k;

    gUnk_03002490->unk15 = 2;
    sub_08070208();
    sub_08049a58();
    k = gUnk_03002490->unk88->unk0D;
    if (k == 1 || k == 2 || (s8)k == 5 || (s8)k == 19 || (s8)k == 25)
    {
        n = 3;
        d = 2;
        if (gUnk_03002490->unk88->unk0D == 25)
        {
            n = 1;
            d = 1;
        }
        while (1)
        {
            gUnk_03002490->unk3C = gUnk_03002490->unk28;
            TaskYieldTrampoline(d);
            for (i = 0; i < n; i++)
            {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(d);
            }
        }
    }
    sub_08006138();
}

void sub_0806efe8(void)
{
}

void sub_0806efec(void)
{
    struct PlayerState *p;

    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk24 = 1;
    gUnk_03002490->unk34 = 0;
    p = gUnk_03002490->unk88;
    if (p->unk0D == 25)
        p->unk37 = 3;
    sub_08049738();
    gUnk_03002490->unk0C = (u32)sub_0803ddc0;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk3E &= 0x7FFF;
    sub_0807029c();
    switch (gUnk_03002490->unk88->unk00)
    {
    case 0:
        sub_080061c0(0x4000, 0x5A5A5A5A);
        break;
    case 1:
        sub_080061c0(0x6000, 0x5A5A5A5A);
        break;
    case 2:
        sub_080061c0(0x2000, 0x5A5A5A5A);
        break;
    case 3:
        sub_080061c0(0x8000, 0x5A5A5A5A);
        break;
    }
    gUnk_03002490->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x40000;
    while (gUnk_03002490->unk24 != 2)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x10000;
    while (gUnk_03002490->unk24 != 3)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08070614(gCurTaskIdx);
    sub_08006138();
}

void sub_0806f174(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk88->unk00 == gUnk_03002360)
        sub_08026264(t->unk48, t->unk4A);
    sub_0801bcac(gUnk_0873F5D4);
    if (gUnk_03002490->unk7A & 1)
    {
        gUnk_03002490->unk7A = 0;
        u = gUnk_03002490;
        u->unk24++;
        if (u->unk24 == 2)
            sub_08070264();
    }
    sub_0807042c();
}

void sub_0806f1e0(void)
{
    struct PlayerState *p;

    gUnk_03002490->unk15 = 3;
    gUnk_03002490->unk24 = 1;
    gUnk_03002490->unk34 = 0;
    p = gUnk_03002490->unk88;
    if (p->unk0D == 25)
        p->unk37 = 3;
    sub_08049738();
    gUnk_03002490->unk0C = (u32)sub_0803ddc0;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk3E &= 0x7FFF;
    sub_0807029c();
    switch (gUnk_03002490->unk88->unk00)
    {
    case 0:
        sub_080061c0(0x8000, 0x5A5A5A5A);
        break;
    case 1:
        sub_080061c0(0xA000, 0x5A5A5A5A);
        break;
    case 2:
        sub_080061c0(0x6000, 0x5A5A5A5A);
        break;
    case 3:
        sub_080061c0(0xC000, 0x5A5A5A5A);
        break;
    }
    gUnk_03002490->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x40000;
    while (gUnk_03002490->unk24 != 2)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x10000;
    while (gUnk_03002490->unk24 != 3)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08070614(gCurTaskIdx);
    sub_08006138();
}

void sub_0806f36c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk88->unk00 == gUnk_03002360)
        sub_08026264(t->unk48, t->unk4A);
    sub_0801bcac(gUnk_0873F5D4);
    if (gUnk_03002490->unk7A & 1)
    {
        gUnk_03002490->unk7A = 0;
        u = gUnk_03002490;
        u->unk24++;
        if (u->unk24 == 2)
            sub_08070264();
    }
    sub_0807042c();
}

void sub_0806f3d8(void)
{
    struct PlayerState *p;

    gUnk_03002490->unk15 = 4;
    gUnk_03002490->unk24 = 0;
    gUnk_03002490->unk34 = 0;
    p = gUnk_03002490->unk88;
    if (p->unk0D == 25)
        p->unk37 = 3;
    sub_08049738();
    gUnk_03002490->unk0C = (u32)sub_0803ddc0;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk43 = -1;
    gUnk_03002490->unk3E &= 0x7FFF;
    sub_0807029c();
    switch (gUnk_03002490->unk88->unk00)
    {
    case 0:
        sub_080061c0(0x8000, 0x5A5A5A5A);
        break;
    case 1:
        sub_080061c0(0xA000, 0x5A5A5A5A);
        break;
    case 2:
        sub_080061c0(0x6000, 0x5A5A5A5A);
        break;
    case 3:
        sub_080061c0(0xC000, 0x5A5A5A5A);
        break;
    }
    gUnk_03002490->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFD0000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFEC000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFF4000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xC000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x14000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x30000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x40000;
    while (gUnk_03002490->unk24 != 2)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x10000;
    while (gUnk_03002490->unk24 != 3)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08070614(gCurTaskIdx);
    sub_08006138();
}

void sub_0806f5c4(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk88->unk00 == gUnk_03002360)
        sub_08026264(t->unk48, t->unk4A);
    sub_0801bcac(gUnk_0873F5D4);
    if (gUnk_03002490->unk7A & 1)
    {
        gUnk_03002490->unk7A = 0;
        u = gUnk_03002490;
        u->unk24++;
        if (u->unk24 == 2)
        {
            u->unk43 = 1;
            sub_08070264();
        }
    }
    sub_0807042c();
}

void sub_0806f638(void)
{
    struct PlayerState *p;
    s32 k;

    gUnk_03002490->unk15 = 5;
    gUnk_03002490->unk24 = 0;
    gUnk_03002490->unk34 = 0;
    p = gUnk_03002490->unk88;
    if (p->unk0D == 25)
        p->unk37 = 3;
    sub_08049738();
    gUnk_03002490->unk0C = (u32)sub_0803ddc0;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk43 = 1;
    switch (gUnk_03002490->unk88->unk00)
    {
    case 0:
        sub_080061c0(0xFFFEF000, 0x5A5A5A5A);
        break;
    case 1:
        sub_080061c0(0xFFFF2000, 0x5A5A5A5A);
        break;
    case 2:
        sub_080061c0(0xFFFED000, 0x5A5A5A5A);
        break;
    case 3:
        sub_080061c0(0xFFFF4000, 0x5A5A5A5A);
        break;
    }
    k = gUnk_03002490->unk88->unk0D;
    if (k == 10)
        sub_08006338(0x841);
    else
        sub_08006338(gUnk_0873D3B8[k][1]);
    gUnk_03002490->unk58 = 0xFFFD0000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    switch (gUnk_03002490->unk88->unk0D)
    {
    case 4:
    case 9:
        sub_08006338(gUnk_0873D3B8[gUnk_03002490->unk88->unk0D][1]);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = 0x10000;
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        break;
    case 10:
        sub_08006338(0x841);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = 0x10000;
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        break;
    default:
        sub_08006338((s16)(gUnk_0873D384[gUnk_03002490->unk88->unk0D] + 6));
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(2);
        sub_08006338((s16)(gUnk_0873D384[gUnk_03002490->unk88->unk0D] + 5));
        TaskYieldTrampoline(2);
        sub_08006338((s16)(gUnk_0873D384[gUnk_03002490->unk88->unk0D] + 4));
        TaskYieldTrampoline(2);
        sub_08006338((s16)(gUnk_0873D384[gUnk_03002490->unk88->unk0D] + 3));
        TaskYieldTrampoline(2);
        sub_08006338((s16)(gUnk_0873D384[gUnk_03002490->unk88->unk0D] + 2));
        gUnk_03002490->unk58 = 0x10000;
        TaskYieldTrampoline(2);
        sub_08006338((s16)(gUnk_0873D384[gUnk_03002490->unk88->unk0D] + 1));
        TaskYieldTrampoline(2);
        break;
    }
    k = gUnk_03002490->unk88->unk0D;
    if (k == 10)
        sub_08006338(0x841);
    else
        sub_08006338(gUnk_0873D3B8[k][1]);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    gUnk_03002490->unk58 = 0x30000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    gUnk_03002490->unk58 = 0x40000;
    while (gUnk_03002490->unk24 != 2)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08070614(gCurTaskIdx);
    sub_08006138();
}

void sub_0806faac(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk88->unk00 == gUnk_03002360)
        sub_08026264(t->unk48, t->unk4A);
    sub_0801bcac(gUnk_0873F5D4);
    if (gUnk_03002490->unk7A & 1)
    {
        gUnk_03002490->unk7A = 0;
        gUnk_03002490->unk24++;
    }
}

void sub_0806fb0c(void)
{
    struct PlayerState *p;

    gUnk_03002490->unk15 = 6;
    gUnk_03002490->unk24 = 1;
    gUnk_03002490->unk34 = 0;
    p = gUnk_03002490->unk88;
    if (p->unk0D == 25)
        p->unk37 = 3;
    sub_08049738();
    gUnk_03002490->unk0C = (u32)sub_0803ddc0;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk3E &= 0x7FFF;
    sub_0807029c();
    switch (gUnk_03002490->unk88->unk00)
    {
    case 0:
        sub_080061c0(0x4000, 0x5A5A5A5A);
        break;
    case 1:
        sub_080061c0(0x6000, 0x5A5A5A5A);
        break;
    case 2:
        sub_080061c0(0x2000, 0x5A5A5A5A);
        break;
    case 3:
        sub_080061c0(0x8000, 0x5A5A5A5A);
        break;
    }
    gUnk_03002490->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x40000;
    while (gUnk_03002490->unk24 != 2)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x10000;
    while (gUnk_03002490->unk24 != 3)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08070614(gCurTaskIdx);
    sub_08006138();
}

void sub_0806fc98(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk88->unk00 == gUnk_03002360)
        sub_08026264(t->unk48, t->unk4A);
    sub_0801bcac(gUnk_0873F5D4);
    if (gUnk_03002490->unk7A & 1)
    {
        gUnk_03002490->unk7A = 0;
        u = gUnk_03002490;
        u->unk24++;
        if (u->unk24 == 2)
            sub_08070264();
    }
    sub_0807042c();
}

void sub_0806fd04(void)
{
    struct PlayerState *p;
    struct Task *t;
    struct Task *u;
    struct Task *v;

    gUnk_03002490->unk15 = 7;
    p = gUnk_03002490->unk88;
    if (p->unk0D == 25)
        p->unk37 = 3;
    sub_08049738();
    gUnk_03002490->unk0C = (u32)sub_0803ddc0;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk24 = 1;
    gUnk_03002490->unk34 = 0;
    sub_0807029c();
    t = gUnk_03002490;
    switch (t->unk88->unk00)
    {
    case 0:
        t->unk54 = 0x10000;
        break;
    case 1:
        t->unk54 = 0x12000;
        break;
    case 2:
        t->unk54 = 0xE000;
        break;
    case 3:
        t->unk54 = 0x14000;
        break;
    }
    gUnk_03002490->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(12);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(4);
    u = gUnk_03002490;
    switch (u->unk88->unk00)
    {
    case 0:
        u->unk54 = 0x8000;
        break;
    case 1:
        u->unk54 = 0xA000;
        break;
    case 2:
        u->unk54 = 0x6000;
        break;
    case 3:
        u->unk54 = 0xC000;
        break;
    }
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFFE000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFFF800;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x800;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(50);
    sub_080062c4();
    v = gUnk_03002490;
    v->unk24++;
    v->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(12);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(12);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    sub_080062c4();
    gUnk_03002490->unk24++;
    sub_08070264();
    sub_080062c4();
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk54 = 0x10000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk54 = 0xFFFE0000;
        TaskYieldTrampoline(2);
    } while (++*(s16 *)&gUnk_03002490->unk6C <= 25);
    sub_080062c4();
    sub_08070614(gCurTaskIdx);
    sub_08006138();
}
