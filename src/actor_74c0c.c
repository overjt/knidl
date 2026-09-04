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
extern s16 gUnk_03002158[];
extern s16 gUnk_03002348;
extern s16 gUnk_030023E4;
extern s16 gUnk_0873FCF8[];
extern s16 gUnk_0873FD20[];
extern s16 gUnk_0873FD48[];
extern s16 gUnk_0873FD70[];
extern s16 gUnk_0873FF98[];
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern struct PlayerState gUnk_03002170[];
extern struct Task * gUnk_03002490;
extern struct Task gUnk_03002790[];
extern u16 gUnk_03002360;
extern u16 gUnk_030023AC;
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
extern u8 gUnk_03001270[];
extern u8 gUnk_03001370[];
extern u8 gUnk_03002340;
extern vu16 gUnk_03001ED8;

/* callees */
extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 sub_08001cc8(s32 a, s16 b, s16 c, s32 d);
extern s32 sub_080031b8(u32 a);
extern s32 sub_080058e4(u32 type, s32 idx);
extern u32 sub_08002ee8(u32 range);
extern u32 sub_08005acc(void);
extern void TaskDispatchTrampoline(void);
extern void TaskYieldTrampoline(u32 a);
extern void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_08003014(void *src, void *dst, s32 ratio, s32 count, void *out);
extern void sub_080059d8(void);
extern void sub_08006138(void);
extern void sub_080062c4(void);
extern void sub_08026264(s32 a, s32 b);
extern void sub_0806421c(s16 t, s16 mag);
extern void sub_0806523c(void);
extern void sub_080652c8(void);
extern void sub_080670f0(u32 src);
extern void sub_0806d4e4(u32 a, s32 b);

/* defined below */
void sub_08075290(s32 a);
void sub_08076074(void);
void sub_0807637c(void);

void sub_08074c0c(void)
{
    register s32 z asm("r5");

    {
        struct Task *t = gUnk_03002490;
        u8 *a;

        t->unk00 = (u32)sub_080059d8;
        t->unk0C = (u32)sub_0806523c;
        a = (u8 *)&t->unk42;
        z = 0;
        *a = 12;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk40 = z;
        if (gUnk_03002790[t->unk44].unk18 < 0)
            t->unk40 = 0xC00;
        else
            t->unk40 = z;
    }
    switch (gUnk_03002490->unk14)
    {
    case 1:
        sub_0806421c(gUnk_03002490->unk28, gUnk_03002490->unk2C);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = gUnk_030023B4;
            t->unk58 = gUnk_030023D4;
            t->unk38 = gUnk_0874C44C;
        }
        gUnk_03002490->unk3C = sub_08002ee8(2) + 4;
        sub_08006138();
        break;
    case 0:
        sub_0806421c(gUnk_03002490->unk28, gUnk_03002490->unk2C);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = gUnk_030023B4;
            t->unk58 = gUnk_030023D4;
            t->unk38 = gUnk_0874C500;
        }
        gUnk_03002490->unk3C = sub_08002ee8(8);
        sub_08006138();
        break;
    case 2:
        {
            struct Task *t = gUnk_03002490;

            t->unk0C = 0;
            t->unk6C = 0;
        }
        do
        {
            {
                struct Task *t = gUnk_03002490;

                sub_08001a94(1, (u32)gUnk_080D21C8, t->unk3E, t->unk40,
                             t->unk48 + gUnk_0873FCF8[(s16)t->unk6C * 2],
                             t->unk4A + gUnk_0873FCF8[(s16)t->unk6C * 2 + 1]);
            }
            {
                struct Task *t = gUnk_03002490;

                sub_08001a94(1, (u32)gUnk_080D21C8, t->unk3E, t->unk40,
                             t->unk48 + gUnk_0873FD20[(s16)t->unk6C * 2],
                             t->unk4A + gUnk_0873FD20[(s16)t->unk6C * 2 + 1]);
            }
            {
                struct Task *t = gUnk_03002490;

                sub_08001a94(1, (u32)gUnk_080D21C8, t->unk3E, t->unk40,
                             t->unk48 + gUnk_0873FD48[(s16)t->unk6C * 2],
                             t->unk4A + gUnk_0873FD48[(s16)t->unk6C * 2 + 1]);
            }
            {
                struct Task *t = gUnk_03002490;

                sub_08001a94(1, (u32)gUnk_080D21C8, t->unk3E, t->unk40,
                             t->unk48 + gUnk_0873FD70[(s16)t->unk6C * 2],
                             t->unk4A + gUnk_0873FD70[(s16)t->unk6C * 2 + 1]);
            }
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 9);
        TaskDispatchTrampoline();
        break;
    default:
        TaskDispatchTrampoline();
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

    for (i = 0; i < gUnk_030023AC; i++)
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
    sub_08003014(gUnk_02005E10, gUnk_0873FD98, v1, 128, gUnk_03001370);
    sub_08003014(gUnk_02005E10 + 256, gUnk_0873FE98, v2, 128, gUnk_03001370 + 256);
    sub_08003014(gUnk_0200AF20, gUnk_0873FE98, v2, 32, gUnk_03001370 + 576);
    sub_08003014(gUnk_0200AF20 + 64, gUnk_0873FE98, v2, 32, gUnk_03001370 + 704);
}

void sub_08075000(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk00 = (u32)sub_080059d8;
        t->unk0C = (u32)sub_080652c8;
        t->unk42 = 11;
    }
    gUnk_03002490->unk38 = gUnk_0875549C;
    sub_080017e4(2, (u32)gUnk_085E6FA4, 0x030015B0, 64);
    LZ77UnCompWram(gUnk_085E6FE4, (void *)0x02020000);
    sub_080017e4(4, 0x02020000, 0x06013000, 0x800);
    LZ77UnCompWram(gUnk_085E72D4, (void *)0x02020000);
    sub_080017e4(4, 0x02020000, 0x06014000, 0x1000);
    {
        struct Task *t = gUnk_03002490;

        t->unk40 = 0xA990;
        t->unk4C = 0xD00000;
        t->unk50 = 0x1000000;
        t->unk3C = 0;
        t->unk54 = -0x2000;
    }
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 8);
    sub_08074e8c();
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08074f48(0);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(2);
        sub_08074f48(3);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 8);
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08074f48(0);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(2);
        sub_08074f48(1);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 8);
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08074f48(1);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(2);
        sub_08074f48(2);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 8);
    sub_08074f48(2);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 4;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 4;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 4;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 4;
    TaskYieldTrampoline(2);
    sub_08074f48(1);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 4;
    TaskYieldTrampoline(2);
    sub_08074f48(0);
    TaskYieldTrampoline(4);
    sub_08074f48(3);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(12);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x800;
        t->unk3C = 5;
    }
    TaskYieldTrampoline(76);
    sub_08075290(12);
    TaskYieldTrampoline(52);
    sub_08075290(0);
    gUnk_03002490->unk54 = 0x20000;
    TaskYieldTrampoline(16);
    sub_08075290(1);
    gUnk_03002490->unk54 = 0x40000;
    TaskYieldTrampoline(19);
    sub_08075290(2);
    TaskDispatchTrampoline();
}

void sub_08075290(s32 a)
{
    struct Task *t;
    s32 id;

    if (a == 12)
    {
        a = 3;
    loop:
        id = sub_080058e4(99, 32);
        if (id != -1)
        {
            t = &gUnk_03002790[id];
            t->unk73 = a;
        }
        a++;
        if (a <= 10)
            goto loop;
    }
    else
    {
        id = sub_080058e4(99, 32);
        if (id != -1)
        {
            t = &gUnk_03002790[id];
            t->unk73 = a;
        }
    }
}

void sub_080752f4(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk00 = (u32)sub_080059d8;
        t->unk0C = (u32)sub_08076074;
        t->unk42 = 12;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk38 = gUnk_0875549C;
        t->unk40 = 0xA210;
    }
    sub_080062c4();
    {
        struct Task *t = gUnk_03002490;

        t->unk3C = 0xFFFF;
        t->unk18 = 0;
        t->unk1C = -1;
        switch (t->unk73)
        {
    case 0:
        {
            struct Task *t = gUnk_03002490;

            t->unk4C = (gUnk_03002158[0] + 168) << 16;
            t->unk50 = (gUnk_03002158[2] + 40) << 16;
        }
        {
            struct Task *t = gUnk_03002490;

            t->unk3C = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x50000;
            t->unk1C = 0;
            t->unk2C = 16;
            t->unk54 = -0x40000;
            t->unk58 = -0x80000;
        }
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = -0x40000;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gUnk_03002490;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
        }
        sub_080062c4();
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = 0x10000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x80000;
            t->unk58 = 0x20000;
            t->unk6C = 0;
        }
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 6;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 4);
        break;
    case 1:
        {
            struct Task *t = gUnk_03002490;

            t->unk4C = (gUnk_03002158[0] + 248) << 16;
            t->unk50 = (gUnk_03002158[2] + 40) << 16;
        }
        TaskYieldTrampoline(35);
        {
            struct Task *t = gUnk_03002490;

            t->unk3C = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x50000;
            t->unk1C = 0;
            t->unk2C = 16;
            t->unk54 = -0x40000;
            t->unk58 = -0x80000;
        }
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = -0x40000;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gUnk_03002490;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
        }
        sub_080062c4();
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = 0x10000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x80000;
            t->unk58 = 0x20000;
            t->unk6C = 0;
        }
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 6;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 4);
        break;
    case 2:
        {
            struct Task *t = gUnk_03002490;

            t->unk4C = (gUnk_03002158[0] + 200) << 16;
            t->unk50 = (gUnk_03002158[2] + 40) << 16;
        }
        TaskYieldTrampoline(17);
        {
            struct Task *t = gUnk_03002490;

            t->unk3C = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x20000;
            t->unk1C = 0;
            t->unk2C = 16;
            t->unk54 = -0x40000;
            t->unk58 = 0x40000;
        }
        TaskYieldTrampoline(12);
        gUnk_03002490->unk58 = 0x20000;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk58 = 0x10000;
        TaskYieldTrampoline(8);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x20000;
            t->unk58 = 0x8000;
        }
        TaskYieldTrampoline(8);
        {
            struct Task *t = gUnk_03002490;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk58 = -0x20000;
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x10000;
            t->unk58 = -0x40000;
            t->unk6C = 0;
        }
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 6;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 4);
        break;
    case 3:
        {
            struct Task *t = gUnk_03002490;

            t->unk4C = (gUnk_03002158[0] + 240) << 16;
            t->unk50 = (gUnk_03002158[2] + 64) << 16;
        }
        TaskYieldTrampoline(128);
        {
            struct Task *t = gUnk_03002490;

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
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x40000;
            t->unk58 = 0x20000;
        }
        TaskYieldTrampoline(3);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x60000;
            t->unk58 = -0x20000;
        }
        TaskYieldTrampoline(5);
        gUnk_03002490->unk2C = -8;
        sub_0806d4e4(4, 256);
        sub_080031b8(189);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x40000;
            t->unk58 = -0x40000;
        }
        TaskYieldTrampoline(6);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x60000;
            t->unk58 = -0x20000;
        }
        TaskYieldTrampoline(3);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x40000;
            t->unk58 = 0x20000;
        }
        TaskYieldTrampoline(6);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x60000;
            t->unk58 = 0x40000;
        }
        TaskYieldTrampoline(5);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x40000;
            t->unk58 = 0x10000;
            t->unk6C = 0;
        }
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 6;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 4);
        break;
    case 4:
        {
            struct Task *t = gUnk_03002490;

            t->unk4C = (gUnk_03002158[0] + 248) << 16;
            t->unk50 = (gUnk_03002158[2] + 88) << 16;
        }
        TaskYieldTrampoline(160);
        {
            struct Task *t = gUnk_03002490;

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
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x60000;
            t->unk58 = -0x40000;
        }
        TaskYieldTrampoline(7);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x40000;
            t->unk58 = -0x20000;
        }
        TaskYieldTrampoline(6);
        gUnk_03002490->unk2C = -8;
        sub_0806d4e4(4, 256);
        sub_080031b8(189);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x30000;
            t->unk58 = -0x10000;
        }
        TaskYieldTrampoline(5);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x20000;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x14000;
            t->unk58 = 0;
        }
        TaskYieldTrampoline(3);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0xC000;
            t->unk58 = 0x8000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x8000;
            t->unk58 = 0x10000;
            t->unk28 = 0;
            t->unk6C = 0;
        }
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 6;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 16);
        break;
    case 5:
        {
            struct Task *t = gUnk_03002490;

            t->unk4C = (gUnk_03002158[0] + 248) << 16;
            t->unk50 = (gUnk_03002158[2] + 16) << 16;
        }
        TaskYieldTrampoline(192);
        {
            struct Task *t = gUnk_03002490;

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
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x60000;
            t->unk58 = -0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x40000;
            t->unk58 = -0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x30000;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x20000;
            t->unk58 = 0;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x14000;
            t->unk58 = 0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x10000;
            t->unk58 = 0xC000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0xC000;
            t->unk58 = 0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
            t->unk54 = -0x8000;
            t->unk58 = 0x14000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0;
            t->unk58 = 0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x8000;
            t->unk58 = 0x30000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x10000;
            t->unk58 = 0x40000;
        }
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0x20000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0x40000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk6C = 0;
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 6;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 4);
        break;
    case 6:
        {
            struct Task *t = gUnk_03002490;

            t->unk4C = (gUnk_03002158[0] + 248) << 16;
            t->unk50 = (gUnk_03002158[2] + 80) << 16;
        }
        TaskYieldTrampoline(224);
        {
            struct Task *t = gUnk_03002490;

            t->unk3C = 6;
            t->unk18 = 0x3F0000;
            t->unk28 = -0x10000;
            t->unk1C = 0;
            t->unk2C = -16;
            t->unk54 = -0x40000;
            t->unk58 = 0x10000;
        }
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk2C = -8;
        sub_0806d4e4(4, 256);
        sub_080031b8(189);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x40000;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk58 = 0x10000;
        TaskYieldTrampoline(2);
        break;
    case 7:
        {
            struct Task *t = gUnk_03002490;

            t->unk4C = (gUnk_03002158[0] + 120) << 16;
            t->unk50 = (gUnk_03002158[2] + 120) << 16;
        }
        TaskYieldTrampoline(128);
        {
            struct Task *t = gUnk_03002490;

            t->unk3C = 6;
            t->unk18 = 0x3F0000;
            t->unk28 = -0x20000;
            t->unk1C = 0;
            t->unk2C = -16;
            t->unk54 = -0x2000;
            t->unk58 = -0x40000;
        }
        TaskYieldTrampoline(12);
        gUnk_03002490->unk2C = -8;
        sub_0806d4e4(4, 256);
        sub_080031b8(189);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x8000;
            t->unk58 = -0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x8000;
            t->unk58 = 0;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x10000;
            t->unk58 = 0x8000;
        }
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 0x10000;
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk58 = 0x20000;
            t->unk6C = 0;
        }
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 6;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 4);
        break;
    case 8:
        {
            struct Task *t = gUnk_03002490;

            t->unk4C = (gUnk_03002158[0] + 254) << 16;
            t->unk50 = (gUnk_03002158[2] + 48) << 16;
        }
        TaskYieldTrampoline(224);
        {
            struct Task *t = gUnk_03002490;

            t->unk3C = 6;
            t->unk18 = 0x30000;
            t->unk28 = 0x20000;
            t->unk1C = 256;
            t->unk2C = 16;
            t->unk54 = -0x40000;
            t->unk58 = 0x80000;
        }
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = 0x40000;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = 0x10000;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(3);
        sub_080062c4();
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(3);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x80000;
            t->unk58 = -0x20000;
            t->unk6C = 0;
        }
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 6;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 4);
        break;
    case 9:
        {
            struct Task *t = gUnk_03002490;

            t->unk4C = (gUnk_03002158[0] + 248) << 16;
            t->unk50 = (gUnk_03002158[2] + 32) << 16;
        }
        TaskYieldTrampoline(80);
        TaskYieldTrampoline(192);
        {
            struct Task *t = gUnk_03002490;

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
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x60000;
            t->unk58 = 0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x40000;
            t->unk58 = 0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x30000;
            t->unk58 = 0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x20000;
            t->unk58 = 0;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x14000;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x10000;
            t->unk58 = -0xC000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0xC000;
            t->unk58 = -0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x8000;
            t->unk58 = -0x14000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0;
            t->unk58 = -0x20000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x8000;
            t->unk58 = -0x30000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x10000;
            t->unk58 = -0x40000;
        }
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0x20000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0x40000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk6C = 0;
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 6;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 4);
        break;
    case 10:
        {
            struct Task *t = gUnk_03002490;

            t->unk4C = (gUnk_03002158[0] + 160) << 16;
            t->unk50 = (gUnk_03002158[2] - 32) << 16;
        }
        TaskYieldTrampoline(128);
        {
            struct Task *t = gUnk_03002490;

            t->unk3C = 6;
            t->unk18 = 0x3F0000;
            t->unk28 = -0x10000;
            t->unk1C = 0;
            t->unk2C = 16;
            t->unk54 = -0x2000;
            t->unk58 = 0x40000;
        }
        TaskYieldTrampoline(12);
        gUnk_03002490->unk2C = 8;
        sub_0806d4e4(4, 256);
        sub_080031b8(189);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x8000;
            t->unk58 = 0x20000;
        }
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 0;
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x10000;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk6C = 0;
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 6;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 4);
        break;
    case 11:
        {
            struct Task *t = gUnk_03002490;

            t->unk4C = (gUnk_03002158[0] + 248) << 16;
            t->unk50 = (gUnk_03002158[2]) << 16;
        }
        TaskYieldTrampoline(1);
        {
            struct Task *t = gUnk_03002490;

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
            struct Task *t = gUnk_03002490;

            t->unk2C = 8;
            t->unk58 = 0x10000;
        }
        TaskYieldTrampoline(12);
        TaskYieldTrampoline(16);
        {
            struct Task *t = gUnk_03002490;

            t->unk18 = 0x3F0000;
            t->unk28 = 0;
            t->unk2C = 8;
        }
        sub_0806d4e4(4, 256);
        sub_080031b8(189);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x2000;
            t->unk58 = 0x10000;
        }
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x10000;
        TaskYieldTrampoline(16);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x20000;
            t->unk58 = 0x20000;
            t->unk6C = 0;
        }
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 6;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 8);
        break;
        }
    }
    TaskDispatchTrampoline();
}

void sub_08076074(void)
{
    {
        struct Task *u = gUnk_03002490;

        u->unk18 += u->unk28;
        if (u->unk18 < 0)
            u->unk18 = 0;
    }
    {
        struct Task *u = gUnk_03002490;

        if (u->unk18 > 0x3F0000)
            u->unk18 = 0x3F0000;
    }
    {
        struct Task *u = gUnk_03002490;

        u->unk1C += u->unk2C;
        if (u->unk1C < 0)
            u->unk1C += 512;
    }
    {
        struct Task *u = gUnk_03002490;

        if (u->unk1C > 0x1FF)
            u->unk1C -= 512;
    }
    {
        struct Task *u = gUnk_03002490;

        if (u->unk38 == NULL)
            return;
        if (u->unk3C == -1)
            return;
    }
    if (sub_08005acc() == 0)
        return;
    {
    struct Task *t = gUnk_03002490;
    u32 *g = t->unk38;
    s32 gfx;

    if (t->unk18 != 63 || t->unk1C != 0)
    {
        gfx = sub_08001cc8(g[t->unk3C], gUnk_0873FF98[t->unk18 >> 16],
                           gUnk_0873FF98[t->unk18 >> 16], (s16)t->unk1C);
        {
            struct Task *u = gUnk_03002490;

            sub_08001a94(u->unk42, gfx, u->unk3E, u->unk40,
                         u->unk48 - gUnk_03002348, u->unk4A - gUnk_030023E4);
        }
    }
    else
    {
        sub_08001a94(t->unk42, g[t->unk3C], t->unk3E, t->unk40,
                     t->unk48 - gUnk_03002348, t->unk4A - gUnk_030023E4);
    }
    }
}

void sub_080761b4(void)
{
    u16 pal;

    pal = *(u16 *)gUnk_03001270;

    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03001ED8 = 0xE0FF & gUnk_03001ED8;
        gUnk_03001ED8 = 0x1100 | gUnk_03001ED8;
        sub_080670f0((u32)gUnk_08740098);
        TaskYieldTrampoline(3);
        gUnk_03001ED8 = 0xE0FF & gUnk_03001ED8;
        gUnk_03001ED8 = 0x1D00 | gUnk_03001ED8;
        sub_080670f0((u32)&pal);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03001ED8 = 0xE0FF & gUnk_03001ED8;
        gUnk_03001ED8 = 0x1100 | gUnk_03001ED8;
        sub_080670f0((u32)gUnk_08740098);
        TaskYieldTrampoline(1);
        gUnk_03001ED8 = 0xE0FF & gUnk_03001ED8;
        gUnk_03001ED8 = 0x1D00 | gUnk_03001ED8;
        sub_080670f0((u32)&pal);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03001ED8 = 0xE0FF & gUnk_03001ED8;
        gUnk_03001ED8 = 0x1D00 | gUnk_03001ED8;
        sub_080670f0((u32)&pal);
        TaskYieldTrampoline(2);
        gUnk_03001ED8 = 0xE0FF & gUnk_03001ED8;
        gUnk_03001ED8 = 0x1100 | gUnk_03001ED8;
        sub_080670f0((u32)gUnk_08740098);
        TaskYieldTrampoline(1);
        gUnk_03001ED8 = 0xE0FF & gUnk_03001ED8;
        gUnk_03001ED8 = 0x1D00 | gUnk_03001ED8;
        sub_080670f0((u32)&pal);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    sub_080670f0((u32)&pal);
    TaskDispatchTrampoline();
}

void sub_08076318(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk04 = (u32)sub_0807637c;
        t->unk00 = (u32)sub_080059d8;
    }
    sub_080062c4();
    gUnk_03002490->unk43 = 1;
    {
        struct Task *t = gUnk_03002490;

        t->unk3E &= 0x7FFF;
        t->unk42 = 7;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk88->unk42 &= 0xFFEF;
        sub_08002e98(t->unk14, 6, gUnk_087400B0);
    }
}

void sub_0807637c(void)
{
    u16 id;
    struct Task *t;

    sub_08002e98(gUnk_03002490->unk15, 6, gUnk_087400C8);
    id = gUnk_03002360;
    t = gUnk_03002490;
    if (id == t->unk88->unk00 && t->unk18 != 0)
        sub_08026264(t->unk1C, t->unk20);
}

void sub_080763c4(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = (u32)sub_080059d8;
    sub_08002e98(t->unk14, 6, gUnk_087400B0);
}
