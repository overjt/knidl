/* game_code_and_rodata 0x0806AD18-0x0806B2E4 (issue #64, module M18 batch 2b).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806AD18 0x0806B2E4 pending/batch2b/actor_6ad18.c --newpb
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern void TaskYieldTrampoline(u32 a);

extern vu16 gUnk_03001ED8; /* DISPCNT shadow */
extern u16 gUnk_03001270;
extern u16 gUnk_0873E69A;
extern u16 gUnk_0873E69C;

extern void sub_080261d4(u32);
extern void sub_08027204(u32);
extern void sub_080670ac(u32);
extern void sub_080670d4(void);
extern void sub_080670f0(u16 *);
extern void sub_080031b8(u32);
extern void sub_080055b0(u32, u32);
extern void sub_080061c0(u32, u32);
extern void sub_080062c4(void);
extern void sub_08006338(u32);
extern void sub_08064d34(u32, u32);
extern void sub_08065e1c(u32, u32);
extern void sub_0806523c(void);
extern void sub_0806d4e4(u32, u32);


extern u32 gUnk_0873E6A0[];
extern u32 gUnk_0873E6D0[];
extern u32 gUnk_0874CA78[];
extern u16 gUnk_0873E72E;
extern u16 gUnk_0873E730;
extern u32 gUnk_0873E734[];
extern u16 gUnk_030023D8;

extern void sub_080034d0(void);
extern void sub_080668c8(void);
extern void sub_08067108(void);
extern void sub_0806d928(void);
extern void sub_0800a554(void);
extern void sub_08002e98(u32, u32, void *);
extern void sub_08006148(void (*)(void), u32);
extern void sub_0806395c(u32);
extern void sub_08066f78(void);
extern void sub_08068e04(void);
extern void sub_08069b44(void);
extern void sub_08069fc8(void);
extern void sub_0806a7a0(void);
void sub_0806b2ac(void);
extern u8 sub_080692fc(void);
extern void sub_080656b4(void);
extern void sub_0806b2e4(void);
extern void sub_0806ed9c(void);

extern u32 gUnk_02007D00[];
extern u8 gUnk_0873E758[];
extern u8 gUnk_0873E77C[];
extern u8 gUnk_0873E78C[];
extern u8 gUnk_0873E798[];
extern u32 gUnk_0874C9D8[];

void sub_0806ad18(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk8C->unk1A = 0;
    t->unk50 = (t->unk4A + 1) << 16;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do
    {
        t = gUnk_03002490;
        t->unk50 = (t->unk4A - 2) << 16;
        TaskYieldTrampoline(2);
        t = gUnk_03002490;
        t->unk50 = (t->unk4A + 2) << 16;
        TaskYieldTrampoline(2);
        t = gUnk_03002490;
        t->unk6C++;
    } while ((s16)t->unk6C <= 11);
    t = gUnk_03002490;
    t->unk50 = (t->unk4A - 1) << 16;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk8C->unk1A = 0xFFFF;
}

void sub_0806adb0(void)
{
    struct Task *t;
    u16 v;

    sub_080670ac(15);
    sub_08027204(2);
    sub_080261d4(5);
    v = gUnk_03001270;
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1000;
        sub_080670f0(&gUnk_0873E69A);
        TaskYieldTrampoline(3);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1D00;
        sub_080670f0(&v);
        TaskYieldTrampoline(3);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1000;
        sub_080670f0(&gUnk_0873E69C);
        TaskYieldTrampoline(3);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1D00;
        sub_080670f0(&v);
        TaskYieldTrampoline(3);
        t = gUnk_03002490;
        t->unk6C++;
    } while ((s16)t->unk6C <= 1);
    sub_080261d4(0);
    sub_080670f0(&v);
    sub_080670d4();
}

void sub_0806ae94(void)
{
    sub_080031b8(0x200);
    sub_080055b0(14, gCurTaskIdx);
    sub_0806adb0();
    sub_080055b0(0, gCurTaskIdx);
}

void sub_0806aec0(void)
{
    struct Task *t;

    sub_080062c4();
    sub_08006338(0);
    sub_08064d34(142, 0);
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080061c0(gUnk_0873E6A0[(s16)gUnk_03002490->unk6C], 0x5A5A5A5A);
        t = gUnk_03002490;
        t->unk58 = gUnk_0873E6D0[(s16)t->unk6C];
        TaskYieldTrampoline(1);
        t = gUnk_03002490;
        t->unk6C++;
    } while ((s16)t->unk6C <= 5);
    sub_080062c4();
    t = gUnk_03002490;
    t->unk0C = (u32)sub_0806523c;
    t->unk38 = gUnk_0874CA78;
    t->unk40 = 0;
    sub_080261d4(4);
    sub_08065e1c(0x1F9, 0);
    sub_0806d4e4(1, 0);
    TaskYieldTrampoline(20);
}

void sub_0806af78(void)
{
    struct Task *t;
    u16 v;

    sub_080670ac(15);
    sub_08027204(2);
    sub_080261d4(5);
    v = gUnk_03001270;
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1000;
        sub_080670f0(&gUnk_0873E72E);
        TaskYieldTrampoline(4);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1D00;
        sub_080670f0(&v);
        TaskYieldTrampoline(3);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1000;
        sub_080670f0(&gUnk_0873E730);
        TaskYieldTrampoline(4);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1D00;
        sub_080670f0(&v);
        TaskYieldTrampoline(3);
        t = gUnk_03002490;
        t->unk6C++;
    } while ((s16)t->unk6C <= 1);
    sub_080261d4(0);
    sub_080670f0(&v);
    sub_080670d4();
}

void sub_0806b05c(void)
{
    sub_080031b8(0x1FF);
    sub_0806af78();
}

void sub_0806b070(void)
{
    void (*f)(void);

    f = (void (*)(void))gUnk_0873E734[gUnk_03002490->unk76];
    if (f != NULL)
        f();
}

void sub_0806b098(void)
{
    sub_080670ac(15);
    sub_080668c8();
    sub_08027204(2);
    sub_08065e1c(0x1FD, 0);
    sub_080261d4(4);
    sub_0806d928();
    sub_0806d4e4(1, 0);
    gUnk_03002490->unk3C = 0xFFFF;
    sub_0806b070();
    TaskYieldTrampoline(24);
    sub_080670d4();
    sub_08067108();
}

s32 sub_0806b0f0(void)
{
    switch (gUnk_03002490->unk76)
    {
    case 1:
    case 3:
    case 4:
    case 5:
    case 6:
        if (gUnk_030023D8 != 20)
            sub_080034d0();
        break;
    case 2:
        break;
    default:
        sub_080034d0();
        break;
    }
}

void sub_0806b12c(void)
{
    gUnk_02007D00[9] = gUnk_03002490->unk40;
    sub_0800a554();
    sub_0806b0f0();
    sub_0806b05c();
    sub_0806b098();
    gUnk_03002490->unk40 = gUnk_02007D00[9];
    sub_08066f78();
    sub_08002e98(gUnk_03002490->unk76, 9, gUnk_0873E758);
}

void sub_0806b178(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk82 > 3)
        t->unk82 = 0;
    sub_08002e98(gUnk_03002490->unk82, 4, gUnk_0873E77C);
}

void sub_0806b1a8(void)
{
    sub_080062c4();
    sub_08006338(0);
    sub_08065e1c(212, 0);
    sub_08069fc8();
}

void sub_0806b1c4(void)
{
    struct Task *t;

    sub_080062c4();
    sub_08006338(0);
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C9D8;
    t->unk40 = 0;
    sub_08065e1c(212, 0);
    sub_08069fc8();
}

void sub_0806b1f4(void)
{
    struct Task *t;

    sub_080062c4();
    sub_08006338(0);
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C9D8;
    t->unk40 = 0;
    sub_08065e1c(212, 0);
    sub_08069fc8();
}

void sub_0806b224(void)
{
    sub_0806a7a0();
}

void sub_0806b230(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = 0;
    t->unk78 = 127;
    sub_0806ed9c();
}

s32 sub_0806b24c(void)
{
    sub_0806395c(1);
    sub_08006148(sub_0806b2e4, gCurTaskIdx);
    return 1;
}

void sub_0806b26c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk04 = (u32)sub_0806b2ac;
    t->unk78 = 1;
    t->unk08 = 0;
    sub_08002e98(t->unk14, 3, gUnk_0873E78C);
}

void sub_0806b2ac(void)
{
    if (sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_0873E798);
    if (gUnk_03002490->unk14 != 2)
    {
        sub_08068e04();
        sub_08069b44();
    }
}
