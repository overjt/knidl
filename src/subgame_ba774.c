/* game_code_and_rodata_080653ec_0806ef5c 0x080BA774-0x080BB528
 * (issue #95, module M35, file 2 of 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BA774 0x080BB528 src/subgame_ba774.c --newpb
 *
 * The reaction duel's round controller.  Task.unk14 is the requested state
 * and Task.unk15 the running one; each state is an <entry, per-frame check>
 * pair dispatched through sub_08002e98: 0x087562FC / 0x08756318 in link
 * play and 0x08756334 / 0x08756350 against the computer.  Task.unk2C is the
 * mask of players that pressed in time, gUnk_03002790[i] the task of player
 * i, gUnk_0200B03C[] the per-player win counts, gUnk_0200B07C[4] the rank
 * order and gUnk_02006184 the best reaction time so far (starts at 99).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern s32 gCurTaskIdx;
extern u8 gUnk_02004B5C;
extern s8 gUnk_02006168;
extern u8 gUnk_02006184;
extern u8 gUnk_0200B03C[];
extern u8 gUnk_0200B048;
extern u8 gUnk_0200B07C[];
extern vu16 gUnk_03001E90;
extern vu16 gUnk_03001ED8;
extern u16 gUnk_030023AC;
extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];
extern vs16 gUnk_03004CA0[];
extern u32 gUnk_087562FC[];
extern u32 gUnk_08756318[];
extern u32 gUnk_08756334[];
extern u32 gUnk_08756350[];

void TaskYieldTrampoline(u32 frames);
void sub_0800214c(void);
void sub_080021dc(void);
void sub_08002e98(u32 a, u32 b, u32 *c);
s32 sub_08003110(s32 songId);
s32 sub_080031b8(s32 id);
void sub_080034d0(void);
void sub_080055c4(u16 val, s32 idx);
void sub_08005654(s32 id);
s32 sub_080058e4(u32 type, s32 idx);
void sub_08006138(void);
void sub_08006148(void *fn, u32 i);
void sub_080b9e30(void);
void sub_080ba50c(void);
void sub_080ba61c(void);
void sub_080ba688(void);
void sub_080ba6b4(void);
s32 sub_080ba708(void);
void sub_080bc740(s32 a0, u16 a1);
void sub_080bc79c(u16 a0);
void sub_080bd188(s32 a0);
void sub_080bd1d0(s32 a0, u16 a1);

void sub_080ba900(void);
void sub_080baa38(void);
void sub_080ba94c(void);
void sub_080ba978(void);
void sub_080baabc(void);
void sub_080bab68(void);
void sub_080bb074(void);

u8 sub_080ba774(void)
{
    if (gUnk_03002490->unk20 > 98)
        return 1;
    return 0;
}

void sub_080ba78c(void)
{
    sub_080034d0();
    sub_080ba900();
    TaskYieldTrampoline(120);
    gUnk_03002490->unk30 = 1;
    gUnk_03002490->unk24 = 1;
}

void sub_080ba7b0(void)
{
    while (gUnk_03002490->unk75 == 0)
        TaskYieldTrampoline(1);
    sub_080034d0();
    sub_080ba900();
    TaskYieldTrampoline(120);
    gUnk_03002490->unk30 = 1;
    gUnk_03002490->unk24 = 1;
}

void sub_080ba7fc(u8 a0)
{
    sub_080baa38();
    sub_080ba94c();
    TaskYieldTrampoline(12);
    sub_080055c4(0, gCurTaskIdx);
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1400;
    sub_080ba978();
    if (a0 == 1)
        sub_080031b8(253);
    gUnk_03002490->unk30 = 0;
    gUnk_03002490->unk24 = 0;
}

void sub_080ba860(void)
{
    sub_080baabc();
    sub_080ba94c();
    TaskYieldTrampoline(12);
    sub_080055c4(0, gCurTaskIdx);
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1400;
    sub_080031b8(0x101);
    sub_080ba978();
    TaskYieldTrampoline(68);
    sub_080ba900();
    TaskYieldTrampoline(120);
    gUnk_03002490->unk30 = 0;
    gUnk_03002490->unk24 = 0;
}

void sub_080ba8cc(void)
{
    struct Task *t;
    struct Task *o;

    sub_080bc79c(gUnk_03002490->unk30);
    t = gUnk_03002490;
    o = &gUnk_03002790[t->unk28];
    o->unk1C = 0;
    o->unk18 = 0;
    o->unk3C = 0;
    o->unk24 = 0;
    t->unk20 = 0;
}

void sub_080ba900(void)
{
    s32 i = sub_080058e4(94, 32);

    if (i != -1)
    {
        struct Task *t = &gUnk_03002790[i];
        t->unk73 = 1;
        t->unk18 = 0;
        t->unk1C = 1;
        t->unk20 = 120;
        t->unk24 = -1;
        t->unk48 = 88;
        t->unk4A = 24;
        t->unk34 = 0;
    }
}

void sub_080ba94c(void)
{
    s32 i = sub_080058e4(94, 32);
    struct Task *t;

    if (i != -1)
    {
        t = &gUnk_03002790[i];
        t->unk73 = 3;
    }
}

void sub_080ba978(void)
{
    s32 i = sub_080058e4(94, 32);
    struct Task *t;

    if (i != -1)
    {
        t = &gUnk_03002790[i];
        t->unk73 = 4;
    }
}

void sub_080ba9a4(s32 a0)
{
    u8 buf[4];
    u8 i = 0;
    u8 j = 0;
    s32 done = 0;

    while (j <= 3)
    {
        if (!done && gUnk_0200B03C[gUnk_0200B07C[i]] < gUnk_0200B03C[a0])
        {
            buf[j] = a0;
            done = 1;
            j++;
        }
        else
        {
            if (gUnk_0200B07C[i] != a0 || i == 3)
            {
                buf[j] = gUnk_0200B07C[i];
                j++;
            }
            i++;
        }
    }
    for (j = 0; j <= 3; j++)
        gUnk_0200B07C[j] = buf[j];
}

void sub_080baa38(void)
{
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        struct Task *o = &gUnk_03002790[i];
        s32 bit = 1 << i;
        if (gUnk_03002490->unk2C & bit)
        {
            sub_080bc740(i, 2);
            o->unk20 = 4;
            gUnk_0200B03C[o->unk18]++;
            sub_080ba9a4(o->unk18);
            if (gUnk_02006184 > gUnk_03002490->unk20)
                gUnk_02006184 = gUnk_03002490->unk20;
        }
        else
        {
            sub_080bc740(i, 3);
        }
    }
}

void sub_080baabc(void)
{
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        struct Task *o = &gUnk_03002790[i];
        s32 bit = 1 << i;
        if (gUnk_03002490->unk2C & bit)
        {
            sub_080bc740(i, 2);
            o->unk20 = i;
        }
    }
}

void sub_080bab08(void)
{
    gUnk_03002490->unk04 = (u32)sub_080bab68;
    sub_080ba61c();
    if (gUnk_030023AC != 1)
    {
        gUnk_03002490->unk14 = 0;
        sub_08002e98(gUnk_03002490->unk14, 7, gUnk_087562FC);
    }
    else
    {
        sub_080bb074();
        gUnk_03002490->unk14 = 0;
        sub_08002e98(gUnk_03002490->unk14, 7, gUnk_08756334);
    }
    sub_08006138();
}

void sub_080bab68(void)
{
    if (gUnk_030023AC != 1)
        sub_08002e98(gUnk_03002490->unk15, 7, gUnk_08756318);
    else
        sub_08002e98(gUnk_03002490->unk15, 7, gUnk_08756350);
    sub_080b9e30();
}

void sub_080babb0(s32 a0)
{
    s32 i;
    struct Task *t;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        struct Task *o = &gUnk_03002790[i];
        s32 bit = 1 << i;
        if (gUnk_03002490->unk2C & bit)
        {
            sub_080bc740(i, 4);
            o->unk20 = 4;
            if (a0 != 1)
                o->unk20 = i;
        }
    }
    t = gUnk_03002490;
    t->unk70 += a0;
    t->unk6E |= t->unk2C;
    if (gUnk_030023AC != 1 && (s16)t->unk70 == gUnk_030023AC)
    {
        t->unk14 = 3;
        if (gUnk_03004CA0[62] != -1)
            sub_08005654(62);
    }
}

u8 sub_080bac5c(void)
{
    s32 found = 0;
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        if (gUnk_0200B03C[i] >= gUnk_03002490->unk34)
        {
            gUnk_02004B5C = i;
            found = 1;
            break;
        }
    }
    return found;
}

void sub_080bacbc(s32 a0)
{
    if (a0 == 1)
        gUnk_03002490->unk14 = 4;
    else
        gUnk_03002490->unk14 = 5;
    sub_080ba50c();
}

void sub_080bace4(void)
{
    sub_08002e98(gUnk_03002490->unk14, 7, gUnk_087562FC);
}

void sub_080bad00(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08003110(0x823);
    while (gUnk_03002490->unk24 == 0)
        TaskYieldTrampoline(1);
    sub_080ba6b4();
    gUnk_03002490->unk14 = 1;
    sub_08006138();
}

void sub_080bad44(void)
{
    if (gUnk_03002490->unk24 != 0)
    {
        s32 r = sub_080ba708();
        if (r != 0)
            sub_080babb0(r);
        if (gUnk_03002490->unk14 != 0)
            sub_08006148(sub_080bace4, gCurTaskIdx);
    }
}

void sub_080bad80(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080ba688();
    sub_08006138();
}

void sub_080bad9c(void)
{
    s32 r = sub_080ba708();

    if (r != 0)
        sub_080bacbc(r);
    else if (sub_080ba774())
        gUnk_03002490->unk14 = 2;
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_080bace4, gCurTaskIdx);
}

void sub_080bade4(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080ba78c();
    TaskYieldTrampoline(8);
    gUnk_03002490->unk14 = 6;
    sub_08006138();
}

void sub_080bae0c(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_080bace4, gCurTaskIdx);
}

void sub_080bae34(void)
{
    gUnk_03002490->unk15 = 3;
    sub_080ba7b0();
    TaskYieldTrampoline(8);
    gUnk_03002490->unk14 = 6;
    sub_08006138();
}

void sub_080bae5c(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_080bace4, gCurTaskIdx);
}

void sub_080bae84(void)
{
    gUnk_03002490->unk15 = 4;
    sub_080ba7fc(1);
    TaskYieldTrampoline(80);
    gUnk_03002490->unk14 = 6;
    sub_08006138();
}

void sub_080baeb0(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 != 4 && t->unk18 != 2)
    {
        if (sub_080bac5c())
            gUnk_03002490->unk18 = 2;
        else
            sub_08006148(sub_080bace4, gCurTaskIdx);
    }
}

void sub_080baef0(void)
{
    gUnk_03002490->unk15 = 5;
    sub_080ba860();
    TaskYieldTrampoline(8);
    gUnk_03002490->unk14 = 6;
    sub_08006138();
}

void sub_080baf18(void)
{
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_080bace4, gCurTaskIdx);
}

void sub_080baf40(void)
{
    gUnk_03002490->unk15 = 6;
    sub_080021dc();
    while (gUnk_03001E90 != 0)
        TaskYieldTrampoline(1);
    sub_080ba8cc();
    sub_0800214c();
    while (gUnk_03001E90 != 0)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk14 = 0;
    sub_08006138();
}

void sub_080baf9c(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 != 6 && t->unk18 != 2)
        sub_08006148(sub_080bace4, gCurTaskIdx);
}

void sub_080bafc8(s32 a0)
{
    s32 i;
    struct Task *t;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        struct Task *o = &gUnk_03002790[i];
        s32 bit = 1 << i;
        if (gUnk_03002490->unk2C & bit)
        {
            sub_080bc740(i, 4);
            o->unk20 = 4;
            if (a0 != 1)
                o->unk20 = i;
        }
    }
    t = gUnk_03002490;
    t->unk70 += a0;
    t->unk6E |= t->unk2C;
    if (gUnk_030023AC != 1 && (s16)t->unk70 == gUnk_030023AC)
    {
        t->unk14 = 2;
        if (gUnk_03004CA0[62] != -1)
            sub_08005654(62);
    }
}

void sub_080bb074(void)
{
    s32 i;
    struct Task *t;

    sub_080bd188(0);
    i = sub_080058e4(94, 32);
    if (i != -1)
    {
        t = &gUnk_03002790[i];
        t->unk44 = gCurTaskIdx;
        t->unk73 = 5;
        t->unk74 = gUnk_02006168;
        t->unk76 = 0;
        gUnk_03002490->unk46 = i;
    }
}

void sub_080bb0d8(s32 a0)
{
    struct Task *t = gUnk_03002490;
    struct Task *o = &gUnk_03002790[t->unk46];

    if (t->unk20 == o->unk1C)
    {
        if (a0 == 0)
            t->unk14 = 4;
        else
            t->unk14 = 5;
    }
    else if (a0 == 1)
    {
        t->unk14 = 3;
    }
}

void sub_080bb120(void)
{
    if (gUnk_03002490->unk1C != 0)
    {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x400;
        sub_080bd188(gUnk_0200B048);
    }
    sub_080bd1d0(gUnk_03002490->unk46, gUnk_03002490->unk30);
    sub_080ba8cc();
}

void sub_080bb174(void)
{
    sub_080bd1d0(gUnk_03002490->unk46, 3);
    sub_080ba7fc(1);
    gUnk_03002490->unk1C = 1;
}

void sub_080bb19c(void)
{
    sub_080bd1d0(gUnk_03002490->unk46, 2);
    sub_080ba7fc(0);
    gUnk_03002490->unk1C = 0;
}

void sub_080bb1c4(void)
{
    sub_080bd1d0(gUnk_03002490->unk46, 4);
    sub_080ba860();
    gUnk_03002490->unk1C = 0;
}

u8 sub_080bb1ec(void)
{
    s32 found = 0;
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        if (gUnk_0200B03C[i] > 4)
        {
            gUnk_02004B5C = i;
            found = 1;
            break;
        }
    }
    return found;
}

void sub_080bb23c(void)
{
    sub_08002e98(gUnk_03002490->unk14, 7, gUnk_08756334);
}

void sub_080bb258(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08003110(0x823);
    while (gUnk_03002490->unk24 == 0)
        TaskYieldTrampoline(1);
    sub_080ba6b4();
    gUnk_03002490->unk14 = 1;
    sub_08006138();
}

void sub_080bb29c(void)
{
    if (gUnk_03002490->unk24 != 0)
    {
        s32 r = sub_080ba708();
        if (r != 0)
            sub_080bafc8(r);
        if (gUnk_03002490->unk14 != 0)
            sub_08006148(sub_080bb23c, gCurTaskIdx);
    }
}

void sub_080bb2d8(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080ba688();
    sub_08006138();
}

void sub_080bb2f4(void)
{
    sub_080bb0d8(sub_080ba708());
    if (gUnk_03002490->unk14 != 1)
    {
        sub_080ba50c();
        sub_08006148(sub_080bb23c, gCurTaskIdx);
    }
}

void sub_080bb328(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080ba7b0();
    gUnk_03002490->unk1C = 0;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk14 = 6;
    sub_08006138();
}

void sub_080bb358(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_080bb23c, gCurTaskIdx);
}

void sub_080bb380(void)
{
    gUnk_03002490->unk15 = 3;
    sub_080bb174();
    TaskYieldTrampoline(80);
    gUnk_03002490->unk14 = 6;
    sub_08006138();
}

void sub_080bb3a8(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 != 3 && t->unk18 != 2)
    {
        if (sub_080bb1ec())
            gUnk_03002490->unk18 = 2;
        else
            sub_08006148(sub_080bb23c, gCurTaskIdx);
    }
}

void sub_080bb3e8(void)
{
    gUnk_03002490->unk15 = 4;
    sub_080bb19c();
    TaskYieldTrampoline(80);
    gUnk_03002490->unk14 = 6;
    sub_08006138();
}

void sub_080bb410(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 != 4 && t->unk18 != 2)
        t->unk18 = 2;
}

void sub_080bb42c(void)
{
    gUnk_03002490->unk15 = 5;
    sub_080bb1c4();
    TaskYieldTrampoline(8);
    gUnk_03002490->unk14 = 6;
    sub_08006138();
}

void sub_080bb454(void)
{
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_080bb23c, gCurTaskIdx);
}

void sub_080bb47c(void)
{
    gUnk_03002490->unk15 = 6;
    sub_080021dc();
    while (gUnk_03001E90 != 0)
        TaskYieldTrampoline(1);
    sub_080bb120();
    TaskYieldTrampoline(1);
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1400;
    sub_0800214c();
    while (gUnk_03001E90 != 0)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk14 = 0;
    sub_08006138();
}

void sub_080bb4fc(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk14 != 6 && t->unk18 != 2)
        sub_08006148(sub_080bb23c, gCurTaskIdx);
}
