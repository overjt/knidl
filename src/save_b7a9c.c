#include "gba/gba.h"
#include "gba/agb_sram.h"
#include "global.h"
#include "task.h"

struct SaveSlot
{
    /*0x00*/ u32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ u32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ u16 unk10;
    /*0x12*/ u16 unk12[2];
    /*0x16*/ u8 unk16[2];
    /*0x18*/ u8 unk18[2];
    /*0x1A*/ u8 unk1A[2];
    /*0x1C*/ u8 unk1C[2];
    /*0x1E*/ u8 pad1E[2];
    /*0x20*/ u32 unk20[2];
    /*0x28*/ u8 unk28[8][7];
    /*0x60*/ u16 unk60[4];
    /*0x68*/ u16 unk68[4];
    /*0x70*/ u32 unk70;
    /*0x74*/ u8 filler74[0x8C];
};

extern u16 gUnk_02006068[];
extern struct SaveSlot gUnk_0200E600[];
extern s32 gUnk_03001EA8;
extern u16 gUnk_030023AC;
extern s32 gUnk_030023E8;
extern u8 gUnk_080CFE20[];

s32 sub_080b7a9c(s32 a);
u32 sub_080b7af8(void);
void sub_080b7d94(s32 a);
u32 sub_080b7dd0(s32 a);
u32 sub_080b7df4(s32 a);
void sub_080b7e14(s32 a);
void sub_080b7f58(s32 a);
void sub_080b8b2c(s32 a);

s32 sub_080b7a9c(s32 a)
{
    s32 i;
    s32 n;

    if (gUnk_03001EA8 == 0)
        return 0;
    n = 0;
    for (i = 0; i < 2; i++)
    {
        if (WriteSramEx((u8 *)&gUnk_0200E600[a], (u8 *)((a << 9) + 0x0E000200 + i * 256), 256) != 0)
            n++;
    }
    return n;
}
u32 sub_080b7af8(void)
{
    if (gUnk_03001EA8 == 0)
        return 0;
    return WriteSramEx(gUnk_080CFE20, (u8 *)(224 << 20), 10);
}
void sub_080b7b20(s32 a)
{
    u32 best;
    u32 i;

    sub_080b7f58(a);
    best = 0;
    for (i = 0; i <= 2; i++)
    {
        if (gUnk_0200E600[i].unk08 > best)
            best = gUnk_0200E600[i].unk08;
    }
    gUnk_0200E600[a].unk08 = best + 1;
    gUnk_0200E600[a].unk0C++;
    sub_080b7df4(a);
    sub_080b7a9c(a);
    sub_080b7af8();
}
void sub_080b7b7c(s32 a)
{
    u32 best;
    u32 i;

    if (a == -1)
        return;
    if (gUnk_030023AC == 1)
        sub_080b7e14(gUnk_030023E8);
    else
        sub_080b8b2c(gUnk_030023E8);
    best = 0;
    for (i = 0; i <= 2; i++)
    {
        if (gUnk_0200E600[i].unk08 > best)
            best = gUnk_0200E600[i].unk08;
    }
    gUnk_0200E600[a].unk08 = best + 1;
    gUnk_0200E600[a].unk0C++;
    sub_080b7df4(a);
    sub_080b7a9c(a);
}
void sub_080b7c00(s32 a)
{
    u32 m;
    u32 s;
    u32 t;
    s32 i;
    u32 best;

    if (a == -1)
        return;
    m = 1;
    t = 0;
    s = 0;
    for (i = 0; i <= 3; i++)
    {
        s += gUnk_0200E600[a].unk68[i] * m;
        t += gUnk_02006068[i] * m;
        m = ((m << 4) - m) << 2;
    }
    if (s != 0 && s < t)
        return;
    for (i = 0; i < 4; i++)
        gUnk_0200E600[a].unk68[i] = gUnk_02006068[i];
    best = 0;
    for (i = 0; i < 3; i++)
    {
        if (gUnk_0200E600[i].unk08 > best)
            best = gUnk_0200E600[i].unk08;
    }
    gUnk_0200E600[a].unk08 = best + 1;
    gUnk_0200E600[a].unk0C++;
    sub_080b7df4(a);
    sub_080b7a9c(a);
}
void sub_080b7cb4(s32 a)
{
    u32 m;
    u32 s;
    u32 t;
    s32 i;
    u32 best;

    if (a == -1)
        return;
    if (gUnk_030023AC != 1)
        return;
    m = 1;
    t = 0;
    s = 0;
    for (i = 0; i <= 3; i++)
    {
        s += gUnk_0200E600[a].unk60[i] * m;
        t += gUnk_02006068[i] * m;
        m = ((m << 4) - m) << 2;
    }
    if (s != 0 && s < t)
        return;
    for (i = 0; i < 4; i++)
        gUnk_0200E600[a].unk60[i] = gUnk_02006068[i];
    best = 0;
    for (i = 0; i < 3; i++)
    {
        if (gUnk_0200E600[i].unk08 > best)
            best = gUnk_0200E600[i].unk08;
    }
    gUnk_0200E600[a].unk08 = best + 1;
    gUnk_0200E600[a].unk0C++;
    sub_080b7df4(a);
    sub_080b7a9c(a);
}
void sub_080b7d74(s32 a)
{
    sub_080b7d94(a);
    if (gUnk_03001EA8 != 0)
        sub_080b7a9c(a);
}
void sub_080b7d94(s32 a)
{
    u32 *p;
    u32 *end;

    p = (u32 *)&gUnk_0200E600[a];
    end = (u32 *)&gUnk_0200E600[a].unk70;
    while (p != end)
        *p++ = 0x99999999;
    gUnk_0200E600[a].unk08 = 0;
    sub_080b7df4(a);
}
u32 sub_080b7dd0(s32 a)
{
    u32 *p;
    u32 *end;
    u32 sum;

    p = (u32 *)&gUnk_0200E600[a];
    end = (u32 *)&gUnk_0200E600[a].unk70;
    sum = 0x97538642;
    while (p != end)
        sum += *p++;
    return sum;
}
