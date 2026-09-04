/* game_code_and_rodata 0x08072D8C-0x08074C0C (issue #79, module M19 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08072D8C 0x08074C0C src/actor_72d8c.c --newpb
 *
 * The middle of M19's cutscene bank: the class-3 states 13-25 of the
 * `0x0873FBD4` / `0x0873FC3C` script tables, each one a linear
 * TaskYieldTrampoline script that walks the 16.16 velocity pair
 * Task.unk54/unk58 (and the 16.16 rotation/scale cells unk5C/unk60) through a
 * table of steps, plus the `sub_0807186c` pose setter they all drive.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"


/* RAM cells and ROM tables */
extern s16 gUnk_03002158[];
extern s32 gUnk_030023D4;
extern struct Task * gUnk_03002490;
extern struct Task gUnk_03002790[];
extern u16 gUnk_03002360;
extern u32 gUnk_02004B4C;
extern u32 gUnk_02005584;
extern u32 gUnk_0873F5CC[];
extern u32 gUnk_0873FC94[];
extern u32 gUnk_08754560[];
extern u8 gUnk_020061E0;
extern u8 gUnk_03001F30;

/* callees */
extern s32 sub_08003110(s32 songId);
extern s32 sub_080031b8(u32 a);
extern s32 sub_080058e4(u32 type, s32 idx);
extern s32 sub_08025b0c();
extern s32 sub_08025e00();
extern s32 sub_08025e0c();
extern s32 sub_08025f00();
extern s32 sub_08026278();
extern s32 sub_08027798();
extern s32 sub_080277f0();
extern s32 sub_08064d6c(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern void TaskDispatchTrampoline(void);
extern void TaskYieldTrampoline(u32 a);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_080034d0(void);
extern void sub_080034f0(s32 player, s32 songId);
extern void sub_080059d8(void);
extern void sub_08006138(void);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_0800625c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_0801bcac(u32 *p);
extern void sub_080261d4(u32 a);
extern void sub_08063fe0(void);
extern void sub_08064970(void);
extern void sub_0806572c(void);
extern void sub_0806d4e4(u32 a, s32 b);
extern void sub_0807186c(int a, int b, int c, int d);
extern void sub_08071898(void);
extern void sub_080718c0(void);
extern void sub_08071bb0(u16 a);
extern void sub_08071c38(u16 a);
extern void sub_08071d2c(void);
extern void sub_08075290(s32 a);

/* defined below */
void sub_08074568(void);

void sub_08072d8c(void)
{
    gUnk_03002490->unk15 = 13;
    gUnk_03002490->unk43 = 255;
    {
        struct Task *t = gUnk_03002490;

        t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
        t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
        t->unk00 = (u32)sub_0806572c;
        t->unk24 = 0;
    }
    {
        s32 r;

        gUnk_02005584 = 251;
        r = sub_080031b8(251);
        gUnk_02004B4C = r;
    }
    sub_08071898();
    sub_080062c4();
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    sub_0807186c(1, 5, 4, 0x300);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x8000;
        t->unk58 = 0x8000;
    }
    TaskYieldTrampoline(8);
    sub_08071898();
    sub_080062c4();
    gUnk_03002490->unk54 = -0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = -0x8000;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x40000;
        t->unk58 = -0x8000;
    }
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0xC000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x10000;
    TaskYieldTrampoline(8);
    sub_0807186c(0, 0, 5, 0x500);
    gUnk_03002490->unk54 = 0x8000;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x8000;
        t->unk58 = -0x10000;
    }
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = -0x10000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk6C = 0;
    do
    {
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x40000;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x20000;
            t->unk58 = -0x10000;
        }
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    gUnk_03002490->unk6C = 0;
    do
    {
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x40000;
            t->unk58 = 0x8000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x14000;
            t->unk58 = 0x10000;
        }
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    gUnk_03002490->unk6C = 0;
    do
    {
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x30000;
            t->unk58 = 0x8000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x10000;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    gUnk_03002490->unk6C = 0;
    do
    {
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x60000;
            t->unk58 = -0x10000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x20000;
            t->unk58 = -0x8000;
        }
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x30000;
        t->unk58 = 0x20000;
    }
    TaskYieldTrampoline(8);
    gUnk_03002490->unk6C = 0;
    do
    {
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x60000;
            t->unk58 = -0x10000;
        }
        TaskYieldTrampoline(2);
        gUnk_03002490->unk54 = 0x20000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x2000;
        t->unk58 = 0x8000;
    }
    TaskYieldTrampoline(8);
    gUnk_03002490->unk6C = 0;
    do
    {
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x20000;
            t->unk58 = -0x10000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x40000;
            t->unk58 = -0x30000;
        }
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    sub_0807186c(0, 0, 6, 0x500);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x40000;
        t->unk58 = -0x60000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x20000;
        t->unk58 = -0x40000;
    }
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = -0x2000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x40000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x60000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x40000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = -0x2000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x40000;
    TaskYieldTrampoline(4);
    sub_08025f00();
    gUnk_03002490->unk24 = 1;
    sub_08006138();
}

void sub_080731c4(void)
{
    sub_080718c0();
}

void sub_080731d0(void)
{
    gUnk_03002490->unk15 = 14;
    {
        struct Task *t = gUnk_03002490;

        t->unk4C = 0x200000;
        t->unk50 = -0x40000;
        t->unk00 = (u32)sub_0806572c;
    }
    sub_08071898();
    sub_080062c4();
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x20000;
        t->unk58 = 0x20000;
    }
    TaskYieldTrampoline(44);
    gUnk_03002490->unk43 = 255;
    sub_080277f0(gUnk_03002490->unk48, gUnk_03002490->unk4A);
    sub_080261d4(4);
    sub_080034f0(gUnk_02004B4C, gUnk_02005584);
    sub_080031b8(219);
    sub_0806d4e4(0, 0);
    sub_080031b8(272);
    if (gUnk_03001F30 == 0)
        sub_08071bb0(5);
    else
        sub_08071c38(5);
    gUnk_020061E0 = 0;
    sub_08063fe0();
}

void sub_0807328c(void)
{
    sub_080718c0();
}

void sub_08073298(void)
{
    gUnk_03002490->unk15 = 15;
    {
        struct Task *t = gUnk_03002490;

        t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
        t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
        t->unk00 = (u32)sub_0806572c;
        t->unk24 = 0;
    }
    {
        s32 r;

        gUnk_02005584 = 250;
        r = sub_080031b8(250);
        gUnk_02004B4C = r;
    }
    sub_08071898();
    sub_080062c4();
    gUnk_03002490->unk58 = 0x60000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x40000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x30000;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gUnk_03002490;

        t->unk58 = 0x27000;
        t->unk60 = -0x6000;
    }
    TaskYieldTrampoline(20);
    gUnk_03002490->unk60 = 0x8C00;
    TaskYieldTrampoline(20);
    sub_080062c4();
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = -0x1FF00;
    TaskYieldTrampoline(1);
    sub_0807186c(0, 8, 6, 0x600);
    gUnk_03002490->unk6C = 0;
    do
    {
        {
            struct Task *t = gUnk_03002490;

            t->unk5C = -0x5000;
            t->unk58 = 0x20000;
            t->unk60 = -0x4000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *t = gUnk_03002490;

            t->unk5C = 0x5000;
            t->unk58 = -0x40000;
            t->unk60 = 0x2000;
        }
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 11);
    sub_0800625c(-0x40000, 0x4000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(10);
    gUnk_03002490->unk60 = 0x3000;
    TaskYieldTrampoline(8);
    sub_08064d6c(148, gUnk_03002490->unk48, gUnk_03002490->unk4A + 16, 0);
    sub_080261d4(2);
    sub_080031b8(272);
    sub_08071898();
    TaskYieldTrampoline(10);
    sub_0807186c(1, 9, 4, 0x300);
    gUnk_03002490->unk60 = -0x4000;
    TaskYieldTrampoline(10);
    sub_0807186c(1, 7, 4, 0x300);
    {
        struct Task *t = gUnk_03002490;

        t->unk5C = -0x2000;
        t->unk58 = 0;
        t->unk60 = -0x5000;
    }
    TaskYieldTrampoline(10);
    sub_0807186c(1, 6, 4, 0x300);
    TaskYieldTrampoline(5);
    sub_0807186c(1, 5, 4, 0x300);
    TaskYieldTrampoline(5);
    sub_0807186c(1, 4, 4, 0x300);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x2000;
        t->unk5C = -0x9000;
        t->unk60 = 0xCC00;
    }
    TaskYieldTrampoline(10);
    sub_08071898();
    TaskYieldTrampoline(5);
    {
        struct Task *t = gUnk_03002490;

        t->unk5C = 0x10000;
        t->unk60 = -0x7000;
    }
    TaskYieldTrampoline(5);
    sub_0807186c(1, 6, 4, 0x300);
    TaskYieldTrampoline(15);
    {
        struct Task *t = gUnk_03002490;

        t->unk5C = -0x20000;
        t->unk58 = -0x20000;
        t->unk60 = 0x4000;
    }
    TaskYieldTrampoline(5);
    sub_08071898();
    TaskYieldTrampoline(10);
    sub_0807186c(1, 6, 3, 0x300);
    {
        struct Task *t = gUnk_03002490;

        t->unk5C = 0x18000;
        t->unk60 = 0;
    }
    TaskYieldTrampoline(20);
    sub_0807186c(1, 5, 3, 0x300);
    {
        struct Task *t = gUnk_03002490;

        t->unk5C = 0x1000;
        t->unk60 = -0x10000;
    }
    TaskYieldTrampoline(30);
    sub_080062c4();
    TaskYieldTrampoline(10);
    sub_08025f00();
    gUnk_03002490->unk24 = 1;
    sub_08006138();
}

void sub_08073578(void)
{
    sub_080718c0();
}

void sub_08073584(void)
{
    gUnk_03002490->unk15 = 17;
    {
        struct Task *t = gUnk_03002490;

        t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
        t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
        t->unk00 = (u32)sub_0806572c;
        t->unk24 = 0;
    }
    {
        s32 r;

        gUnk_02005584 = 250;
        r = sub_080031b8(250);
        gUnk_02004B4C = r;
    }
    sub_08071898();
    sub_080062c4();
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x80000;
        t->unk58 = 0x20000;
    }
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = -0x40000;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x20000;
        t->unk58 = 0x10000;
    }
    TaskYieldTrampoline(8);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x10000;
        t->unk58 = 0x8000;
    }
    TaskYieldTrampoline(8);
    sub_0807186c(1, 8, 4, 0x300);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x10000;
        t->unk58 = -0x10000;
    }
    TaskYieldTrampoline(8);
    sub_08064d6c(148, gUnk_03002490->unk48, gUnk_03002490->unk4A + 16, 0);
    sub_080261d4(2);
    sub_080031b8(272);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x20000;
        t->unk58 = -0x20000;
    }
    TaskYieldTrampoline(8);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x40000;
        t->unk58 = -0x40000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x60000;
        t->unk58 = -0x20000;
    }
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x40000;
        t->unk58 = 0x8000;
    }
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x30000;
        t->unk58 = 0x40000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x20000;
        t->unk58 = 0x60000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x10000;
        t->unk58 = 0x80000;
    }
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = -0x10000;
    TaskYieldTrampoline(2);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x20000;
        t->unk58 = 0x60000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x30000;
        t->unk58 = 0x40000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x40000;
        t->unk58 = 0x20000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x60000;
        t->unk58 = 0x10000;
    }
    TaskYieldTrampoline(4);
    sub_08071898();
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x40000;
        t->unk58 = 0x800;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x30000;
        t->unk58 = -0x2000;
    }
    TaskYieldTrampoline(4);
    sub_0807186c(1, 5, 4, 0x300);
    gUnk_03002490->unk54 = -0x20000;
    TaskYieldTrampoline(2);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x10000;
        t->unk58 = -0x100000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x20000;
        t->unk58 = -0x80000;
    }
    TaskYieldTrampoline(13);
    sub_080062c4();
    sub_08025f00();
    gUnk_03002490->unk24 = 1;
    sub_08006138();
}

void sub_080737f8(void)
{
    sub_080718c0();
}

void sub_08073804(void)
{
    gUnk_03002490->unk15 = 18;
    {
        struct Task *t = gUnk_03002490;

        t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
        t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
        t->unk00 = (u32)sub_0806572c;
        t->unk24 = 0;
        t->unk18 = 0x3F0000;
        t->unk28 = -0x5000;
    }
    sub_080062c4();
    sub_0807186c(0, 4, 4, 0x300);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x8000;
        t->unk58 = -0x20000;
    }
    TaskYieldTrampoline(48);
    gUnk_03002490->unk58 = -0x14000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(8);
    sub_0807186c(0, 4, 8, 0x200);
    gUnk_03002490->unk58 = -0xC000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0x4000;
    TaskYieldTrampoline(8);
    sub_08071898();
    gUnk_03002490->unk58 = -0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x4000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x4000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x1000;
    TaskYieldTrampoline(40);
    sub_08025f00();
    gUnk_03002490->unk24 = 1;
    sub_08006138();
}

void sub_0807395c(void)
{
    sub_080718c0();
}

void sub_08073968(void)
{
    gUnk_03002490->unk15 = 19;
    {
        s32 r;

        gUnk_02005584 = 218;
        r = sub_080031b8(218);
        gUnk_02004B4C = r;
    }
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    sub_080062c4();
    sub_080061c0(0xC000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0x20000;
    sub_08006138();
}

void sub_080739bc(void)
{
    struct Task *t;

    sub_080718c0();
    sub_0801bcac(gUnk_0873F5CC);
    t = gUnk_03002490;
    if (t->unk7A & 1)
    {
        sub_080277f0(t->unk48, t->unk4A);
        sub_080261d4(4);
        sub_080034f0(gUnk_02004B4C, gUnk_02005584);
        sub_080031b8(219);
        sub_0806d4e4(0, 0);
        sub_080031b8(272);
        if (gUnk_03001F30 == 0)
            sub_08071bb0(6);
        else
            sub_08071c38(3);
        gUnk_020061E0 = 0;
        sub_08063fe0();
    }
}

void sub_08073a54(void)
{
    gUnk_03002490->unk15 = 20;
    {
        struct Task *t = gUnk_03002490;

        t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
        t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
        t->unk00 = (u32)sub_0806572c;
        t->unk24 = 0;
    }
    {
        s32 r;

        gUnk_02005584 = 250;
        r = sub_080031b8(250);
        gUnk_02004B4C = r;
    }
    sub_08071898();
    sub_080062c4();
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x10000;
        t->unk58 = 0x8000;
    }
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x8000;
        t->unk58 = -0x10000;
    }
    TaskYieldTrampoline(8);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x10000;
        t->unk58 = -0x20000;
    }
    TaskYieldTrampoline(8);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x20000;
        t->unk58 = -0x10000;
    }
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x40000;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x10000;
        t->unk58 = 0x54000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x8000;
        t->unk58 = 0x34000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x2000;
        t->unk58 = 0x18000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x800;
        t->unk58 = 0xC000;
    }
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = 0x800;
    TaskYieldTrampoline(15);
    sub_0807186c(1, 4, 4, 0x300);
    TaskYieldTrampoline(15);
    sub_080062c4();
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(2);
    sub_08064d6c(148, gUnk_03002490->unk48, gUnk_03002490->unk4A + 16, 0);
    sub_080261d4(2);
    sub_080031b8(272);
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(4);
    sub_0807186c(1, 6, 5, 0x300);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x2000;
        t->unk58 = -0x10000;
    }
    TaskYieldTrampoline(16);
    gUnk_03002490->unk54 = 0x8000;
    TaskYieldTrampoline(16);
    sub_0807186c(1, 8, 6, 0x300);
    gUnk_03002490->unk54 = 0x10000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk54 = 0x14000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk54 = 0x20000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk54 = 0x30000;
    TaskYieldTrampoline(40);
    sub_080062c4();
    sub_08025f00();
    gUnk_03002490->unk24 = 1;
    sub_08006138();
}

void sub_08073cd4(void)
{
    sub_080718c0();
}

void sub_08073ce0(void)
{
    gUnk_03002490->unk15 = 21;
    {
        struct Task *t = gUnk_03002490;

        t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
        t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
        t->unk00 = (u32)sub_0806572c;
        t->unk24 = 0;
    }
    sub_08071898();
    sub_080062c4();
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x60000;
        t->unk58 = -0x20000;
    }
    TaskYieldTrampoline(7);
    sub_0807186c(2, 0, -1, 0);
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(5);
    sub_0807186c(2, 0, -1, 0);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(3);
    sub_0807186c(2, 0, -1, 0);
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(1);
    sub_0807186c(2, 0, -1, 0);
    TaskYieldTrampoline(5);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(1);
    sub_0807186c(2, 0, -1, 0);
    TaskYieldTrampoline(7);
    sub_08025f00();
    gUnk_03002490->unk24 = 1;
    sub_08006138();
}

void sub_08073e00(void)
{
    sub_080718c0();
}

void sub_08073e0c(void)
{
    gUnk_03002490->unk15 = 22;
    {
        struct Task *t = gUnk_03002490;

        t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
        t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
        t->unk00 = (u32)sub_0806572c;
    }
    {
        s32 r;

        gUnk_02005584 = 218;
        r = sub_080031b8(218);
        gUnk_02004B4C = r;
    }
    sub_080062c4();
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x10000;
        t->unk58 = 0x20000;
    }
    sub_08006138();
}

void sub_08073e80(void)
{
    struct Task *t;

    sub_080718c0();
    sub_0801bcac(gUnk_0873F5CC);
    t = gUnk_03002490;
    if (t->unk7A & 1)
    {
        sub_080277f0(t->unk48, t->unk4A);
        sub_080261d4(4);
        sub_080034f0(gUnk_02004B4C, gUnk_02005584);
        sub_080031b8(219);
        sub_0806d4e4(0, 0);
        sub_080031b8(272);
        if (gUnk_03001F30 == 0)
            sub_08071bb0(3);
        else
            sub_08071c38(3);
        gUnk_020061E0 = 0;
        sub_08063fe0();
    }
}

void sub_08073f18(void)
{
    gUnk_03002490->unk15 = 23;
    sub_080034d0();
    {
        s32 r;

        gUnk_02005584 = 126;
        r = sub_080031b8(126);
        gUnk_02004B4C = r;
    }
    sub_08071898();
    sub_080062c4();
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x18000;
        t->unk58 = -0x8000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk60 = -0x8000;
        t->unk68 = 0x60000;
    }
    while (gUnk_03002158[2] > 8)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(60);
    sub_080034f0(gUnk_02004B4C, gUnk_02005584);
    {
        s32 r;

        gUnk_02005584 = 217;
        r = sub_080031b8(217);
        gUnk_02004B4C = r;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk18 = 0x100000;
        t->unk28 = -0xD00;
        t->unk4C = 0x8A0000;
        t->unk50 = -0x100000;
        t->unk54 = -0x10000;
        t->unk58 = 0x8000;
    }
    TaskYieldTrampoline(28);
    gUnk_03002490->unk54 = -0xC000;
    TaskYieldTrampoline(28);
    gUnk_03002490->unk54 = -0x8000;
    TaskYieldTrampoline(28);
    gUnk_03002490->unk54 = -0x4000;
    TaskYieldTrampoline(14);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x1000;
        t->unk58 = 0x4000;
    }
    TaskYieldTrampoline(14);
    gUnk_03002490->unk54 = -0x800;
    TaskYieldTrampoline(14);
    gUnk_03002490->unk54 = 0x1000;
    TaskYieldTrampoline(14);
    gUnk_03002490->unk54 = 0x2000;
    TaskYieldTrampoline(14);
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(62);
    {
        struct Task *t = gUnk_03002490;

        t->unk28 = 0;
        t->unk38 = gUnk_08754560;
        t->unk40 = 0xF010;
        t->unk43 = 1;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk18 = -2;
        t->unk28 = 0;
    }
    sub_08006338(1);
    TaskYieldTrampoline(50);
    gUnk_03002490->unk58 = 0x1000;
    TaskYieldTrampoline(84);
    sub_08025b0c();
    sub_08006138();
}

void sub_0807409c(void)
{
    if ((gUnk_03002490->unk50 >> 16) < -32)
        sub_080062c4();
}

void sub_080740bc(void)
{
    gUnk_03002490->unk15 = 24;
    sub_08003110(33);
    sub_080058e4(98, 32);
    {
        struct Task *t = gUnk_03002490;

        t->unk4C = 0;
        t->unk50 = -0x220000;
        t->unk00 = (u32)sub_0806572c;
    }
    sub_08071898();
    sub_080062c4();
    TaskYieldTrampoline(16);
    gUnk_03002490->unk6C = 0;
    do
    {
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x8000;
            t->unk58 = -0x1000;
        }
        TaskYieldTrampoline(8);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x4000;
            t->unk58 = -0x2000;
        }
        TaskYieldTrampoline(8);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x8000;
            t->unk58 = -0x1000;
        }
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(8);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x4000;
            t->unk58 = 0x10000;
        }
        TaskYieldTrampoline(8);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x8000;
            t->unk58 = 0x8000;
        }
        TaskYieldTrampoline(8);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 5);
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(24);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk6C = 0;
    do
    {
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x8000;
            t->unk58 = 0x10000;
        }
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    gUnk_03002490->unk6C = 0;
    do
    {
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x2000;
            t->unk58 = 0x10000;
        }
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk6C = 0;
    do
    {
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = 0x40000;
            t->unk58 = -0x40000;
        }
        TaskYieldTrampoline(16);
        gUnk_03002490->unk58 = 0x40000;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk54 = -0x40000;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk58 = -0x20000;
        TaskYieldTrampoline(16);
        sub_080062c4();
        gUnk_03002490->unk58 = -0x40000;
        TaskYieldTrampoline(16);
        sub_080062c4();
        gUnk_03002490->unk54 = 0x40000;
        TaskYieldTrampoline(16);
        sub_080062c4();
        gUnk_03002490->unk58 = 0x40000;
        TaskYieldTrampoline(16);
        {
            struct Task *t = gUnk_03002490;

            t->unk54 = -0x40000;
            t->unk58 = -0x20000;
        }
        TaskYieldTrampoline(16);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    sub_08003110(34);
    sub_08075290(11);
    gUnk_03002490->unk54 = -0x2000;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = -0x20000;
        t->unk58 = 0x8000;
    }
    TaskYieldTrampoline(16);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x20000;
        t->unk58 = -0x8000;
    }
    TaskYieldTrampoline(16);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x10000;
        t->unk58 = 0x20000;
    }
    TaskYieldTrampoline(8);
    sub_080261d4(4);
    gUnk_03002490->unk43 = 1;
    sub_080261d4(4);
    sub_080034f0(gUnk_02004B4C, gUnk_02005584);
    sub_080031b8(219);
    sub_0806d4e4(0, 0);
    sub_080031b8(272);
    if (gUnk_03001F30 != 0)
        while (1)
            ;
    sub_08071bb0(7);
    gUnk_020061E0 = 0;
    sub_08063fe0();
}

void sub_080743c8(void)
{
}

void sub_080743cc(void)
{
    gUnk_03002490->unk15 = 25;
    sub_08071d2c();
    gUnk_03002490->unk68 = 0x50000;
    sub_08006138();
}

void sub_080743f0(void)
{
    sub_08064970();
    if (gUnk_03002490->unk24 == 0 && gUnk_030023D4 <= 0)
    {
        sub_08025f00();
        gUnk_03002490->unk24 = 1;
    }
}

void sub_08074420(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk00 = (u32)sub_080059d8;
        t->unk15 = 4;
    }
    {
        s32 r;

        gUnk_02005584 = 218;
        r = sub_080031b8(218);
        gUnk_02004B4C = r;
    }
    gUnk_03002490->unk43 = 1;
    sub_0807186c(1, 11, 4, 0x400);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x6000;
        t->unk58 = 0x48000;
    }
    sub_08006138();
}

void sub_0807447c(void)
{
    struct Task *t;

    sub_080718c0();
    sub_0801bcac(gUnk_0873F5CC);
    t = gUnk_03002490;
    if (t->unk7A & 1)
    {
        sub_080277f0(t->unk48, t->unk4A);
        sub_080261d4(4);
        sub_080034f0(gUnk_02004B4C, gUnk_02005584);
        sub_080031b8(219);
        sub_0806d4e4(0, 0);
        if (gUnk_03001F30 == 0)
            sub_08071bb0(1);
        else
            sub_08071c38(1);
        gUnk_020061E0 = 0;
        sub_08063fe0();
    }
}

void sub_0807450c(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = (u32)sub_080059d8;
    t->unk0C = 0;
    t->unk04 = (u32)sub_08074568;
    sub_08027798(t->unk48, t->unk4A);
    sub_08002e98(gUnk_03002790[gUnk_03002490->unk44].unk14, 25, gUnk_0873FC94);
}

void sub_08074568(void)
{
    struct Task *t = gUnk_03002490;

    sub_08026278(t->unk48, t->unk4A);
}

void sub_08074588(void)
{
    while (1)
    {
        struct Task *t = gUnk_03002490;

        t->unk4C = gUnk_03002790[gUnk_03002360].unk48 << 16;
        t->unk50 = gUnk_03002790[gUnk_03002360].unk4A << 16;
        TaskYieldTrampoline(1);
    }
}

void sub_080745d0(void)
{
    TaskDispatchTrampoline();
}

void sub_080745dc(void)
{
    sub_080062c4();
    TaskYieldTrampoline(48);
    sub_08025e00();
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0x40000;
    sub_08006138();
}

void sub_08074628(void)
{
    sub_080062c4();
    TaskDispatchTrampoline();
}

void sub_08074638(void)
{
    sub_080062c4();
    TaskYieldTrampoline(24);
    sub_08025e00();
    gUnk_03002490->unk54 = 0x10000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x20000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x40000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x60000;
    TaskYieldTrampoline(38);
    gUnk_03002490->unk54 = 0x40000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x20000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x10000;
    TaskYieldTrampoline(2);
    sub_080062c4();
    TaskDispatchTrampoline();
}

void sub_080746c0(void)
{
    sub_080062c4();
    gUnk_03002490->unk58 = -0x40000;
    TaskYieldTrampoline(84);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(16);
    sub_080062c4();
    TaskDispatchTrampoline();
}

void sub_0807470c(void)
{
    sub_080062c4();
    sub_08025e00();
    TaskYieldTrampoline(102);
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(9);
    gUnk_03002490->unk58 = -0x40000;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk58 = -0x60000;
    TaskYieldTrampoline(42);
    gUnk_03002490->unk58 = -0x40000;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(9);
    sub_080062c4();
    TaskDispatchTrampoline();
}

void sub_08074784(void)
{
    sub_080062c4();
    TaskDispatchTrampoline();
}

void sub_08074794(void)
{
    sub_080062c4();
    gUnk_03002490->unk58 = -0x60000;
    TaskYieldTrampoline(64);
    gUnk_03002490->unk58 = -0x60000;
    TaskYieldTrampoline(64);
    gUnk_03002490->unk58 = -0x60000;
    TaskYieldTrampoline(64);
    gUnk_03002490->unk58 = -0x60000;
    TaskYieldTrampoline(64);
    sub_080062c4();
    TaskDispatchTrampoline();
}

void sub_080747dc(void)
{
    sub_080062c4();
    TaskDispatchTrampoline();
}

void sub_080747ec(void)
{
    sub_080062c4();
    TaskYieldTrampoline(40);
    sub_08025e00();
    gUnk_03002490->unk54 = 0x8000;
    TaskYieldTrampoline(64);
    gUnk_03002490->unk54 = 0x10000;
    TaskYieldTrampoline(64);
    gUnk_03002490->unk54 = 0x20000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk54 = 0x40000;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk54 = 0x60000;
    TaskYieldTrampoline(48);
    gUnk_03002490->unk54 = 0x40000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x10000;
    TaskYieldTrampoline(8);
    sub_080062c4();
    TaskDispatchTrampoline();
}

void sub_08074880(void)
{
    sub_080062c4();
    sub_08026278(gUnk_03002490->unk48, 0x10D);
    TaskDispatchTrampoline();
}

void sub_080748a8(void)
{
    sub_080062c4();
    TaskYieldTrampoline(130);
    sub_08025e00();
    gUnk_03002490->unk5C = 0x6000;
    TaskYieldTrampoline(28);
    {
        struct Task *t = gUnk_03002490;

        t->unk54 = 0x90000;
        t->unk5C = 0;
    }
    TaskYieldTrampoline(67);
    gUnk_03002490->unk5C = -0x8000;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk5C = 0;
    TaskYieldTrampoline(80);
    TaskDispatchTrampoline();
}

void sub_08074904(void)
{
    sub_080062c4();
    TaskYieldTrampoline(32);
    sub_08025e00();
    gUnk_03002490->unk54 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x30000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x40000;
    TaskYieldTrampoline(40);
    gUnk_03002490->unk54 = 0x30000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x10000;
    TaskYieldTrampoline(8);
    sub_080062c4();
    TaskDispatchTrampoline();
}

void sub_08074974(void)
{
    sub_080062c4();
    sub_080062c4();
    TaskDispatchTrampoline();
}

void sub_08074988(void)
{
    sub_080062c4();
    TaskYieldTrampoline(80);
    sub_08025e00();
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = 0x40000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = 0x60000;
    TaskYieldTrampoline(18);
    gUnk_03002490->unk58 = 0x40000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(4);
    sub_080062c4();
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = -0x40000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = -0x60000;
    TaskYieldTrampoline(81);
    gUnk_03002490->unk58 = -0x40000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(6);
    sub_080062c4();
    TaskDispatchTrampoline();
}

void sub_08074ab8(void)
{
    sub_080062c4();
    TaskDispatchTrampoline();
}

void sub_08074ac8(void)
{
    sub_080062c4();
    TaskYieldTrampoline(20);
    sub_0800622c(-0x10000, -0x1000, 0x60000);
    while (gUnk_03002490->unk58 >= -((gUnk_03002158[2] - 8) << 12))
        TaskYieldTrampoline(1);
loop:
    if (gUnk_03002158[2] <= 7)
    {
        sub_080062c4();
        gUnk_03002490->unk50 = 0x620000;
    }
    else
    {
        struct Task *t = gUnk_03002490;
        s32 v = -((gUnk_03002158[2] - 8) << 12);

        t->unk58 = v;
        if (v < -0x60000)
            t->unk58 = -0x60000;
    }
    TaskYieldTrampoline(1);
    goto loop;
}

void sub_08074b60(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk4C = t->unk48 << 16;
        t->unk50 = 0x700000;
    }
    sub_080062c4();
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(160);
    gUnk_03002490->unk58 = 0x4000;
    TaskYieldTrampoline(128);
    sub_080062c4();
    sub_08025e0c();
    TaskDispatchTrampoline();
}

void sub_08074bb0(int a, int b, int c)
{
    u16 x = a;
    u16 y = b;
    u8 z = c;
    s32 id;

    id = sub_08064d6c(165, gUnk_03002490->unk48, gUnk_03002490->unk4A, 0);
    gUnk_03002790[id].unk14 = z;
    if (z != 2)
    {
        gUnk_03002790[id].unk28 = x;
        gUnk_03002790[id].unk2C = (s16)y;
    }
}
