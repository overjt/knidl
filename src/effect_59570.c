#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_59570.c (0x08059570-0x0805A357, issue #89).
 *
 * Task type #7: variants 42-44. */

extern u32 gUnk_08751F0C[];
extern u32 gUnk_0874C804[];
extern u32 gUnk_0874C828[];
extern u32 gUnk_0873C23C[];
extern u32 gUnk_0873C250[];
extern s16 gUnk_03002348;               /* scalar, read with ldrsh (33 landed files) */
extern s16 gUnk_030023E4;               /* scalar, read with ldrsh (32 landed files) */
extern u32 gUnk_08751ECC[];
extern u32 gUnk_08751F84[];
extern u8 gUnk_081FD870[];
extern u16 gUnk_0873BB0E[][3];   /* per sub-state: 8.8 x velocity, 8.8 y velocity, frame */

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_080017e4(u32 mode, void *src, void *dst, u32 size);   /* early_1518; effect_5afac's pointer spelling */
s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);   /* callers pass f sign-extended (lsls/asrs #16); the early_1518 definition says u16 */
void sub_08005654(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void sub_080059d8(void);
void sub_080059fc(void);
void sub_08005a74(void);
void sub_08005d9c(void);
void sub_08006040(void);
void sub_080061c0(s32 a, s32 b);
u32 sub_0800641c(s16 a, s16 b);   /* the ROM tests r0 unnarrowed (src callers spell u32) */
u16 sub_080064ac(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 sub_080064dc(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
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

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk38 = gUnk_08751F0C;
    switch (gUnk_03002490->unk18 & 15)
    {
    case 0:
        gUnk_03002490->unk42 = 8;
        while (1)
        {
            u = gUnk_03002490;
            u->unk54 = -((struct Task *)u->unk8C)->unk54;
            u->unk64 = 0x40000;
            u->unk58 = 0x20000;
            u->unk68 = 0x20000;
            u->unk4C = ((struct Task *)u->unk8C)->unk48 << 16;
            u->unk50 = (((struct Task *)u->unk8C)->unk4A + 16) << 16;
            u->unk3C = 16;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            if (gUnk_03002490->unk88->unk3F != 3 || gUnk_03002490->unk88->unk04 != 13)
                break;
        }
        break;
    case 1:
        gUnk_03002490->unk42 = 5;
        while (1)
        {
            v = gUnk_03002490;
            v->unk54 = -((struct Task *)v->unk8C)->unk54;
            v->unk64 = 0x40000;
            v->unk58 = 0x20000;
            v->unk68 = 0x20000;
            gUnk_03002490->unk4C = (sub_080064ac(-8, 1, 16) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-8, 1, 16) + ((struct Task *)gUnk_03002490->unk8C)->unk4A + 16) << 16;
            gUnk_03002490->unk3C = 26;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            if (gUnk_03002490->unk88->unk3F != 3 || gUnk_03002490->unk88->unk04 != 13)
                break;
        }
        break;
    case 2:
        gUnk_03002490->unk42 = 5;
        while (1)
        {
            w = gUnk_03002490;
            w->unk54 = -((struct Task *)w->unk8C)->unk54;
            w->unk64 = 0x40000;
            w->unk58 = 0x20000;
            w->unk68 = 0x20000;
            gUnk_03002490->unk4C = (sub_080064ac(-8, 1, 16) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-8, 1, 16) + ((struct Task *)gUnk_03002490->unk8C)->unk4A + 16) << 16;
            gUnk_03002490->unk3C = 22;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            if (gUnk_03002490->unk88->unk3F != 3 || gUnk_03002490->unk88->unk04 != 13)
                break;
        }
        break;
    case 3:
        gUnk_03002490->unk00 = (u32)sub_08005a74;
        gUnk_03002490->unk04 = (u32)sub_08059aac;
        gUnk_03002490->unk42 = 8;
        gUnk_03002490->unk38 = gUnk_0874C804;
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk04 = 0;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        TaskDispatchTrampoline();
    case 4:
        gUnk_03002490->unk00 = (u32)sub_08005a74;
        gUnk_03002490->unk0C = (u32)sub_08005d9c;
        gUnk_03002490->unk04 = (u32)sub_08059aac;
        gUnk_03002490->unk42 = 5;
        t = gUnk_03002490;
        t->unk38 = gUnk_0874C828;
        t->unk50 = (t->unk4A + 1) << 16;
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk04 = 0;
        gUnk_03002490->unk3C = 16;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 24;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 17;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        TaskDispatchTrampoline();
    case 5:
        gUnk_03002490->unk00 = (u32)sub_080059d8;
        gUnk_03002490->unk0C = (u32)sub_08059b18;
        gUnk_03002490->unk42 = 5;
        t = gUnk_03002490;
        t->unk28 = t->unk48;
        t->unk2C = t->unk4A;
        t->unk4C = 0;
        t->unk50 = 0x30000;
        t->unk54 = -0x40000;
        t->unk3C = 0;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk54 = -0x10000;
        gUnk_03002490->unk5C = 0x800;
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 2;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 1);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 1);
        break;
    }
    TaskDispatchTrampoline();
}

void sub_08059aac(void)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk18 & 15)
    {
    case 3:
        sub_0801a828((u8)gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873C23C);
        break;
    case 4:
        sub_0801a828((u8)gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873C250);
        break;
    }
}

void sub_08059b18(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;

    if (t->unk3C != -1)
    {
        if (sub_0800641c((s16)(t->unk28 + t->unk48), (s16)(t->unk2C + t->unk4A)) != 0)
        {
            u = gUnk_03002490;
            sub_08001a94(u->unk42, gUnk_0874C828[u->unk3C], 0, 0,
                         u->unk28 + u->unk48 - gUnk_03002348,
                         (s16)(u->unk2C + u->unk4A - gUnk_030023E4));
        }
        t = gUnk_03002490;
        if (sub_0800641c((s16)(t->unk28 - t->unk48), (s16)(t->unk2C + t->unk4A)) != 0)
        {
            u = gUnk_03002490;
            sub_08001a94(u->unk42, gUnk_0874C828[(s16)(u->unk3C | 1)], 0, 0,
                         u->unk28 - u->unk48 - gUnk_03002348,
                         (s16)(u->unk2C + u->unk4A - gUnk_030023E4));
        }
    }
}

void sub_08059c28(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_08751ECC;
    t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x800) | 12;
    t->unk3E = 0;
    gUnk_03002490->unk4C = (sub_080064dc(-4, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
    gUnk_03002490->unk50 = (sub_080064ac(-4, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
    u = gUnk_03002490;
    switch (u->unk18 & 15)
    {
    case 0:
        u->unk0C = (u32)sub_0803dfc8;
        u->unk54 = -0x18000;
        u->unk58 = -0x18000;
        u->unk3C = 0;
        break;
    case 1:
        u->unk0C = (u32)sub_08006040;
        u->unk54 = 0x18000;
        u->unk58 = -0x18000;
        u->unk3C = 4;
        break;
    case 2:
        u->unk0C = (u32)sub_08006040;
        u->unk54 = -0x18000;
        u->unk58 = 0x18000;
        u->unk3C = 8;
        break;
    case 3:
        u->unk0C = (u32)sub_08006040;
        u->unk54 = 0x18000;
        u->unk58 = 0x18000;
        u->unk3C = 11;
        break;
    }
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_08059d7c(void)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk18 & 0xFF00)
    {
    case 0x100:
        t->unk00 = (u32)sub_080059fc;
        t->unk0C = (u32)sub_0803dfc8;
        t->unk04 = (u32)sub_0805a320;
        t->unk42 = 5;
        {
            struct Task *u = gUnk_03002490;

            u->unk38 = gUnk_08751F84;
            u->unk40 = (((struct Task *)u->unk8C)->unk40 + 0x1800) | 12;
            u->unk4C = 0;
            u->unk50 = 0;
        }
        for (;;)
        {
            while (gUnk_03002490->unk88->unk3F == 3)
            {
                gUnk_03002490->unk3C = 0;
                TaskYieldTrampoline(1);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
                gUnk_03002490->unk3C = 0xFFFF;
                TaskYieldTrampoline(4);
            }
            gUnk_03002490->unk3C = 0xFFFF;
            while (gUnk_03002490->unk88->unk3F != 3)
                TaskYieldTrampoline(1);
        }
    case 0x200:
        gUnk_03002490->unk00 = (u32)sub_08005a74;
        gUnk_03002490->unk0C = (u32)sub_08005d9c;
        gUnk_03002490->unk04 = (u32)sub_0805a320;
        gUnk_03002490->unk42 = 8;
        gUnk_03002490->unk38 = gUnk_08751F0C;
        for (;;)
        {
            while (abs(((struct Task *)gUnk_03002490->unk8C)->unk58) > 0x2FFFF)
            {
                gUnk_03002490->unk4C = (sub_080064ac(-8, 1, 16) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
                gUnk_03002490->unk50 = (sub_080064ac(-8, 1, 16) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
                gUnk_03002490->unk3C = 26;
                TaskYieldTrampoline(1);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
            }
            gUnk_03002490->unk3C = 0xFFFF;
            while (abs(((struct Task *)gUnk_03002490->unk8C)->unk58) <= 0x2FFFF)
                TaskYieldTrampoline(1);
        }
    case 0x300:
        gUnk_03002490->unk00 = (u32)sub_08005a74;
        gUnk_03002490->unk0C = (u32)sub_08005d9c;
        gUnk_03002490->unk04 = (u32)sub_0805a320;
        gUnk_03002490->unk42 = 5;
        gUnk_03002490->unk38 = gUnk_08751F0C;
        for (;;)
        {
            while (abs(((struct Task *)gUnk_03002490->unk8C)->unk58) > 0x2FFFF)
            {
                gUnk_03002490->unk4C = (sub_080064ac(-8, 1, 16) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
                gUnk_03002490->unk50 = (sub_080064ac(-8, 1, 16) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
                gUnk_03002490->unk3C = 22;
                TaskYieldTrampoline(1);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
            }
            gUnk_03002490->unk3C = 0xFFFF;
            while (abs(((struct Task *)gUnk_03002490->unk8C)->unk58) <= 0x2FFFF)
                TaskYieldTrampoline(1);
        }
    case 0x400:
        gUnk_03002490->unk00 = (u32)sub_08005a74;
        gUnk_03002490->unk0C = (u32)sub_08005d9c;
        gUnk_03002490->unk04 = (u32)sub_0805a320;
        gUnk_03002490->unk42 = 8;
        gUnk_03002490->unk38 = gUnk_08751F0C;
        for (;;)
        {
            while (abs(((struct Task *)gUnk_03002490->unk8C)->unk58) > 0x37FFF)
            {
                gUnk_03002490->unk4C = ((struct Task *)gUnk_03002490->unk8C)->unk48 << 16;
                gUnk_03002490->unk50 = ((struct Task *)gUnk_03002490->unk8C)->unk4A << 16;
                gUnk_03002490->unk3C = 16;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
            }
            gUnk_03002490->unk3C = 0xFFFF;
            while (abs(((struct Task *)gUnk_03002490->unk8C)->unk58) <= 0x37FFF)
                TaskYieldTrampoline(1);
        }
    case 0x500:
        gUnk_03002490->unk00 = (u32)sub_080059d8;
        gUnk_03002490->unk0C = (u32)sub_08005d9c;
        gUnk_03002490->unk42 = 5;
        {
            struct Task *u = gUnk_03002490;
            u32 off;
            u8 *src;

            u->unk38 = gUnk_08751F0C;
            u->unk40 = ((struct Task *)u->unk8C)->unk40 | 0xF008;
            off = (((struct Task *)u->unk8C)->unk40 & 0x7FF) << 5;
            src = gUnk_081FD870;
            sub_080017e4(1, src, (void *)(off + 0x06010100), 128);
            sub_080017e4(1, src + 128, (void *)(off + 0x06010500), 128);
            sub_080017e4(1, src + 256, (void *)(off + 0x06010900), 128);
            sub_080017e4(1, src + 384, (void *)(off + 0x06010D00), 128);
        }
        gUnk_03002490->unk4C = (gUnk_03002490->unk48 + sub_080064dc(0, 1, 8)) << 16;
        gUnk_03002490->unk50 = (gUnk_03002490->unk4A + sub_080064ac(0, 1, 8)) << 16;
        {
            u16 *row = gUnk_0873BB0E[gUnk_03002490->unk18 & 15];

            {
                s32 a = row[0];
                s32 b = a << 8;

                if (a & 0x8000)
                    b |= 0xFF000000;
                sub_080061c0(b, 0x5A5A5A5A);
            }
            {
                struct Task *w = gUnk_03002490;
                s32 a = row[1];
                s32 b = a << 8;

                if (a & 0x8000)
                    b |= 0xFF000000;
                w->unk58 = b;
                w->unk3C = row[2];
            }
        }
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0;
        gUnk_03002490->unk58 = 0;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        TaskDispatchTrampoline();
        break;
    }
    TaskDispatchTrampoline();
}

void sub_0805a320(void)
{
    struct Task *t = gUnk_03002490;

    if (((struct Task *)t->unk8C)->unk73 == 8 || (t->unk88->unk04 != 13 && t->unk88->unk04 != 3))
        sub_08005654(gCurTaskIdx);
}
