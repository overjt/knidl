#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u32 gUnk_02000020[];
extern u32 gUnk_020060CC[];
extern u8 gUnk_02006A14[];
extern u8 gUnk_02007CF0;
extern s32 gUnk_02007D00[];
extern s8 gUnk_02007FB8[];
extern u32 gUnk_02008010[];
extern u32 gUnk_02020000[];
extern u32 gUnk_03000010;
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000F8C;
extern vs32 gUnk_03000FA8;
extern vs32 gUnk_03000FC0;
extern u32 gUnk_0300117C;
extern u8 gUnk_03001470[];
extern u32 gUnk_03001570[];
extern vs32 gUnk_03001E94;
extern u16 gUnk_03001EA4;
extern vu16 gUnk_03001ED8;
extern vs32 gUnk_03001EE0;
extern u8 gUnk_03001F34;
extern struct PlayerState gUnk_03002170[];
extern u8 gUnk_03002340;
extern s16 gUnk_03002348;
extern u8 gUnk_03002350;
extern u16 gUnk_03002360;
extern u16 gUnk_030023AC;
extern s32 gUnk_030023B4;
extern u32 gUnk_030023C0[];
extern s32 gUnk_030023D4;
extern s16 gUnk_030023E4;
extern u32 gUnk_03002438[];
extern s16 gUnk_0300244C;
extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];
extern vs16 gUnk_03004CA0[];
extern u32 gUnk_06010000[];
extern u32 gUnk_085B9B6C[];
extern s16 gUnk_0873DBAC[];
extern s16 gUnk_0873DBD4[];
extern u32 gUnk_0873DBE4[];
extern u32 gUnk_0873DC10[];
extern u32 gUnk_0873DC3C[];
extern u8 gUnk_0873DC4C[];
extern u8 gUnk_0873DC66[];
extern u8 gUnk_0873DC80[];
extern u16 gUnk_0873DC9A[];
extern u32 gUnk_0873DCA8[];
extern u32 gUnk_0873DCC0[];
extern u32 gUnk_0873DCC8[];
extern u32 gUnk_0873DCCC[];
extern u32 gUnk_0873DD16[];
extern u32 gUnk_0873DD30[];
extern u32 gUnk_0873DD4C[];
extern u32 gUnk_0873DD5C[];
extern u32 gUnk_0873DD64[];
extern u32 gUnk_0873DD80[];
extern u32 gUnk_0873DDA2[];
extern u32 gUnk_0873DDB4[];
extern u32 gUnk_0873DDBE[];
extern u32 gUnk_0873DDE8[];
extern u32 gUnk_0873DEA0[];
extern u16 gUnk_0873DEA8[];
extern u32 gUnk_0873DEDC[];
extern u32 gUnk_0874C890[];
extern u32 gUnk_0874CCF4[];
extern u32 gUnk_0874CDF8[];
extern u32 gUnk_0874CFEC[];
extern u32 gUnk_08754850[];
extern u32 gUnk_0875488C[];
extern u32 gUnk_087548A0[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_080017e4(u32 mode, void *src, void *dst, u32 size);
void sub_08001a94(u32 a, s32 b, u32 c, u32 d, u32 e, u32 f);
void sub_08002028(void);
void sub_0800214c(void);
void sub_080021dc(void);
void sub_080022ac(void);
void sub_080022fc(void);
void sub_08002338(void);
void sub_08002358(void);
void sub_08002378(void);
void sub_08002d18(void);
void sub_08002e0c(void);
void sub_08002e98(u32 a, u32 b, u32 *c);
u32 sub_08002ee8(u32 range);
s32 sub_08003110(s32 songId);
void sub_080031b8(s32 id);
s32 sub_080058e4(u32 type, s32 idx);
s32 sub_08005904(u32 type, s32 start, s32 end);
void sub_080059d8(void);
u32 sub_08005acc(void);
void sub_08005d9c(void);
void sub_08006138(void);
void sub_08006148(void *fn, u32 i);
void sub_08006214(void);
void sub_0800622c(s32 a, s32 b, s32 c);
void sub_08006244(void);
void sub_0800625c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void sub_080062c4(void);
void sub_08006338(s32 a);
void sub_08006364(s32 a);
s32 sub_080063f0(s16 a, s16 b);
u32 sub_0800641c(s16 x, s16 y);
void sub_08008c4c(u32 a);
void sub_08008c64(u16 a);
void sub_08009b2c(u16 a);
void sub_08009e60(s32 a, s32 b);
void sub_0800a008(u32 a, s32 b, s32 c);
void sub_0800a04c(s32 a, u32 b);
void sub_08023fd4(void);
void sub_080258e0(void);
void sub_08026264(s32 a, s32 b);
void sub_08026998(void);
void sub_08027178(void);
void sub_08033d0c(void);
void sub_08040788(void);
void sub_0805b16c(void);
void sub_0805b370(void);
s32 sub_0805b4bc(void);
void sub_0805b514(void);
s32 sub_0805b5b0(void);
void sub_0805b61c(void);
void sub_0805b670(void);
void sub_0805b83c(void);
void sub_0805b8b8(void);
void sub_0805b8f8(void);
void sub_0805b9a4(void);
void sub_0805ba08(void);
void sub_0805bb90(void);
void sub_0805bc1c(void);
void sub_0805bc5c(void);
void sub_0805bca4(void);
void sub_0805bce0(void);
void sub_0805bd34(void);
void sub_0805be48(void);
void sub_0805c114(void);
void sub_0805c150(void);
void sub_0805c584(void);
void sub_0805c814(void);
void sub_0805c990(void);
s32 sub_0805cc54(void);
void sub_0805ceec(void);
void sub_0805d420(void);
void sub_0805d5fc(void);
void sub_0805d8a4(s32 a0);
void sub_0805d8c4(s32 a0);
void sub_0805d8f4(void);
void sub_0805d918(void);
void sub_0805d994(s32 a0, s32 a1);
void sub_0805da2c(void);
void sub_0805dd4c(void);
void sub_0805deac(void);
void sub_0805e15c(void);
void sub_0805e1bc(void);
void sub_0805e24c(void);
void sub_080656b4(void);
void sub_08068a8c(u32 a, u8 flag);
void sub_0806d4e4(s32 a, s32 b);

s32 sub_0805afac(s32 a0, s32 a1, s32 a2)
{
    s8 kind = a0;
    u8 param = a1;
    s32 base;
    s32 idx;
    struct Task *t;

    if (kind == 0)
        base = 16;
    else if (kind == 1)
        base = 20;
    else if (kind == 2)
        base = 24;
    else if (kind == 3)
        base = 28;
    else
        return -1;
    idx = sub_08005904(7, base, base + 3);
    if (idx == -1)
    {
        if (kind == 0)
            base = 4;
        else if (kind == 1)
            base = 7;
        else if (kind == 2)
            base = 10;
        else if (kind == 3)
            base = 13;
        else
            return -1;
        idx = sub_08005904(7, base, base + 2);
    }
    if (idx != -1)
    {
        t = &gUnk_03002790[idx];
        t->unk18 = (param << 24) | (a2 & 0x00FFFFFF);
        t->unk4C = gUnk_03002490->unk4C;
        t->unk48 = gUnk_03002490->unk48;
        t->unk50 = gUnk_03002490->unk50;
        t->unk4A = gUnk_03002490->unk4A;
        t->unk43 = gUnk_03002490->unk43;
        t->unk88 = gUnk_03002490->unk88;
    }
    return idx;
}

s32 sub_0805b088(s32 a0, s32 a1, s32 a2)
{
    u8 param = a1;
    s32 idx;
    struct Task *t;

    idx = sub_08005904(7, 32, 62);
    if (idx != -1)
    {
        t = &gUnk_03002790[idx];
        t->unk18 = (param << 24) | (a2 & 0x00FFFFFF);
        t->unk4C = gUnk_03002490->unk4C;
        t->unk48 = gUnk_03002490->unk48;
        t->unk50 = gUnk_03002490->unk50;
        t->unk4A = gUnk_03002490->unk4A;
        t->unk43 = gUnk_03002490->unk43;
        t->unk88 = gUnk_03002490->unk88;
        t->unk72 = 10;
    }
    return idx;
}

void sub_0805b110(void)
{
    sub_08008c4c(3);
    sub_0805b16c();
    sub_08009b2c(gUnk_03002360);
    sub_08002358();
    sub_08002378();
    sub_080022fc();
    sub_0800214c();
    sub_08002e0c();
    do
    {
        sub_08002d18();
        sub_08040788();
    } while (*(s8 *)0x03002438 == 0);
    sub_08002338();
    sub_080021dc();
    sub_08002e0c();
    sub_08026998();
    sub_08027178();
}

void sub_0805b16c(void)
{
    s32 i;
    s8 *p;
    s8 *q;
    s32 z;

    sub_08002028();
    sub_080022ac();
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 248 << 5;
    gUnk_03001ED8 |= 128;
    gUnk_0300117C = gUnk_03001EE0 = gUnk_03000F8C = gUnk_03000B78 = 0;
    gUnk_03000010 = gUnk_03000FC0 = gUnk_03001E94 = gUnk_03000FA8 = 0;
    sub_08008c64(0);
    sub_08023fd4();
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if (gUnk_03002170[i].unk0D == 24)
            sub_0800a008(0, -1, i);
        else
            gUnk_03002170[i].unk0D = 0;
    }
    gUnk_03001F34 = 1;
    gUnk_02007CF0 = 0;
    *(s8 *)gUnk_02008010 = -1;
    *(s8 *)gUnk_03002438 = 0;
    q = gUnk_02007FB8;
    z = 0;
    p = q + 2;
    do
    {
        *p = z;
        p--;
    } while ((s32)p >= (s32)q);
}

void sub_0805b278(void)
{
    gUnk_030023D4 = 0;
    gUnk_03002490->unk6C = 0;
    while ((s16)gUnk_03002490->unk6C < gUnk_030023AC)
    {
        if ((s16)gUnk_03002490->unk6C == gUnk_03002490->unk88->unk00)
            gUnk_03002490->unk2C = gUnk_030023D4;
        if (((gUnk_03002340 >> (s16)gUnk_03002490->unk6C) & 1) != 0)
            gUnk_030023D4++;
        gUnk_03002490->unk6C++;
    }
    gUnk_03002490->unk43 = 1;
    gUnk_03002490->unk04 = (u32)sub_0805b4bc;
    gUnk_02007D00[8] |= 1 << gUnk_03002490->unk88->unk00;
    sub_0805b370();
    gUnk_03002490->unk14 = 0;
    sub_08002e98(gUnk_03002490->unk14, 11, gUnk_0873DBE4);
}

void sub_0805b354(void)
{
    sub_08002e98(gUnk_03002490->unk14, 11, gUnk_0873DBE4);
}

void sub_0805b370(void)
{
    gUnk_03002490->unk30 = 0;
    gUnk_03002490->unk34 = 0;
    gUnk_02007D00[7] = 0;
    gUnk_02007D00[9] = 0;
    gUnk_03002490->unk70 = 0xFFFF;
    if (gUnk_03002490->unk2C == 0)
    {
        LZ77UnCompWram((const void *)gUnk_085B9B6C[3], gUnk_02020000);
        sub_080017e4(4, gUnk_02020000, gUnk_06010000, ((u16 *)gUnk_085B9B6C)[1] << 5);
        sub_080017e4(2, (void *)gUnk_085B9B6C[2], gUnk_03001570, ((u16 *)gUnk_085B9B6C)[0] << 5);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_02007D00[(s16)gUnk_03002490->unk6C] = sub_080058e4(85, 32);
            (gUnk_03002790 + gUnk_02007D00[(s16)gUnk_03002490->unk6C])->unk73 = gUnk_03002490->unk6C;
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 6);
        sub_080058e4(90, 32);
    }
    gUnk_03002490->unk4C = gUnk_0873DBAC[gUnk_03002350 * 4 + gUnk_03002490->unk2C] << 16;
    gUnk_03002490->unk50 = 232 << 18;
    sub_080058e4(88, 32);
    gUnk_03002490->unk28 = sub_080058e4(84, 32);
    (gUnk_03002790 + gUnk_03002490->unk28)->unk73 = gUnk_03002490->unk88->unk00;
    if (gUnk_03002350 > 1)
    {
        gUnk_03002490->unk46 = sub_080058e4(87, 32);
        (gUnk_03002790 + gUnk_03002490->unk46)->unk73 = gUnk_03002490->unk88->unk00;
    }
}

s32 sub_0805b4bc(void)
{
    sub_08002e98(gUnk_03002490->unk15, 11, gUnk_0873DC10);
}

void sub_0805b4d8(void)
{
    gUnk_03002490->unk15 = 0;
    sub_0805d8c4(0);
    gUnk_03002490->unk60 = 128 << 7;
    TaskYieldTrampoline(27);
    gUnk_03002490->unk14 = 1;
    sub_0805b514();
}

void sub_0805b508(void)
{
    sub_0805d8f4();
}

void sub_0805b514(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080062c4();
    sub_0805d8c4(1);
    sub_08006138();
}

void sub_0805b534(void)
{
    sub_0805d8f4();
    if (sub_0805b5b0() == 0)
    {
        sub_0805c584();
        sub_0805be48();
        gUnk_03002490->unk30++;
        if (gUnk_03002490->unk30 > 35)
        {
            gUnk_03002490->unk14 = 4;
            sub_08006148(sub_0805b354, gCurTaskIdx);
        }
    }
    else if (gUnk_030023D4 != 0)
    {
        gUnk_03002490->unk14 = 2;
        sub_08006148(sub_0805b354, gCurTaskIdx);
    }
    else
    {
        gUnk_03002490->unk14 = 3;
        sub_08006148(sub_0805b354, gCurTaskIdx);
    }
}

s32 sub_0805b5b0(void)
{
    u16 *p = (u16 *)gUnk_030023C0;
    s32 n;

    if ((p[gUnk_03002490->unk88->unk00] & 3) != 0)
    {
        sub_0805b61c();
        n = gUnk_03002490->unk30;
        if (n <= 24)
        {
            gUnk_03002490->unk30 = ((24 - n) >> 1) + 24;
            gUnk_030023D4 = 1;
        }
        else
        {
            gUnk_030023D4 = 0;
        }
        gUnk_03002490->unk70 = 0;
        return 1;
    }
    return 0;
}

void sub_0805b61c(void)
{
    if (gUnk_03002490->unk30 > 24)
        gUnk_03002490->unk34 = 24 - ((gUnk_03002490->unk30 - 24) << 1);
    else
        gUnk_03002490->unk34 = gUnk_03002490->unk30;
    gUnk_03002490->unk34++;
}

void sub_0805b644(void)
{
    gUnk_03002490->unk15 = 2;
    TaskYieldTrampoline(4);
    sub_0805b670();
}

void sub_0805b660(void)
{
    sub_0805d8f4();
    sub_0805c584();
}

void sub_0805b670(void)
{
    gUnk_03002490->unk15 = 3;
    sub_08006138();
}

void sub_0805b688(void)
{
    sub_0805d8f4();
    sub_0805c584();
    gUnk_03002490->unk30++;
    if (gUnk_03002490->unk30 > 35)
    {
        gUnk_03002490->unk14 = 4;
        sub_08006148(sub_0805b354, gCurTaskIdx);
    }
}

void sub_0805b6c0(void)
{
    gUnk_03002490->unk15 = 4;
    sub_0806d4e4(1, 0);
    gUnk_03002490->unk30 = gUnk_0873DC80[gUnk_03002490->unk34];
    sub_0805b83c();
    sub_0805b8b8();
    sub_0805d8c4(3);
    sub_080058e4(81, 32);
    gUnk_03002490->unk24 = 0;
    if (gUnk_03002490->unk88->unk00 == gUnk_03002360)
    {
        if (gUnk_03002490->unk30 <= 1)
            sub_080031b8(244);
        else
            sub_080031b8(227);
    }
    gUnk_03002490->unk58 = 0xFFF80000;
    TaskYieldTrampoline(gUnk_0873DC4C[gUnk_03002490->unk34]);
    gUnk_03002490->unk60 = 128 << 7;
    TaskYieldTrampoline(32);
    if (gUnk_0873DC66[gUnk_03002490->unk34] != 0)
    {
        gUnk_03002490->unk14 = 5;
        sub_0805b8f8();
    }
    else
    {
        gUnk_03002490->unk14 = 6;
        sub_0805b9a4();
    }
}

void sub_0805b788(void)
{
    gUnk_03002490->unk70++;
    sub_0805d8f4();
    gUnk_03002490->unk24++;
    if (sub_0800641c(gUnk_03002490->unk48, gUnk_03002490->unk4A) != 0)
        sub_08001a94(14, gUnk_0873DC3C[gUnk_03002490->unk24 & 3], 0, 0,
                     gUnk_03002490->unk48 - gUnk_03002348,
                     (s16)(gUnk_03002490->unk4A - gUnk_030023E4 + 16));
    if ((gUnk_03002490->unk24 & 7) == 0)
        sub_080058e4(83, 32);
    if (gUnk_03002490->unk24 > 31)
    {
        sub_080058e4(82, 32);
        gUnk_03002490->unk24 = 0;
    }
}

void sub_0805b83c(void)
{
    gUnk_02006A14[gUnk_03002490->unk88->unk00]--;
    if (gUnk_02006A14[gUnk_03002490->unk88->unk00] != gUnk_03002490->unk30)
    {
        gUnk_02006A14[gUnk_03002490->unk88->unk00] = 7;
        if (gUnk_03002490->unk30 == 6)
            gUnk_02006A14[gUnk_03002490->unk88->unk00]--;
    }
    if (gUnk_02006A14[gUnk_03002490->unk88->unk00] == 0)
        gUnk_02007D00[9] = 1;
}

void sub_0805b8b8(void)
{
    struct Task *t;

    t = &gUnk_03002790[gUnk_02007D00[gUnk_03002490->unk30]];
    t->unk28 = 0;
    t->unk2C |= 1 << gUnk_03002490->unk88->unk00;
}

void sub_0805b8f8(void)
{
    gUnk_03002490->unk15 = 5;
    sub_080062c4();
    sub_0805d8c4(4);
    gUnk_03002490->unk4A -= 4;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
    TaskYieldTrampoline(60);
    gUnk_03002490->unk4A -= 4;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk4A -= 4;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk4A -= 4;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
    TaskYieldTrampoline(12);
    gUnk_03002490->unk58 = -163840;
    gUnk_03002490->unk60 = 128 << 8;
    TaskYieldTrampoline(9);
    sub_0805ba08();
}

void sub_0805b998(void)
{
    sub_0805d8f4();
}

void sub_0805b9a4(void)
{
    gUnk_03002490->unk15 = 6;
    sub_0805d8c4(0);
    sub_08006138();
}

void sub_0805b9c0(void)
{
    s32 v;
    s16 *tbl;

    sub_0805d8f4();
    v = gUnk_03002490->unk4A;
    tbl = (s16 *)gUnk_0873DBD4;
    if (v > tbl[gUnk_03002490->unk30] - 2)
    {
        gUnk_03002490->unk14 = 7;
        sub_08006148(sub_0805b354, gCurTaskIdx);
    }
}

void sub_0805ba08(void)
{
    gUnk_03002490->unk15 = 7;
    sub_080062c4();
    gUnk_03002490->unk50 = (gUnk_0873DBD4[gUnk_03002490->unk30] - 2) << 16;
    sub_0805d8c4(5);
    TaskYieldTrampoline(2);
    sub_0805d8c4(6);
    TaskYieldTrampoline(32);
    gUnk_030023D4 = 0;
    gUnk_030023B4 = 0;
    if (gUnk_030023AC > 1)
    {
        gUnk_03002490->unk6C = 0;
        while ((s16)gUnk_03002490->unk6C < gUnk_030023AC)
        {
            if (((gUnk_03002340 >> (s16)gUnk_03002490->unk6C) & 1) != 0
             && gUnk_03002790[(s16)gUnk_03002490->unk6C].unk30 == gUnk_03002490->unk30)
            {
                if ((s16)gUnk_03002490->unk6C == gCurTaskIdx)
                    gUnk_030023B4 = gUnk_030023D4;
                gUnk_030023D4++;
            }
            gUnk_03002490->unk6C++;
        }
        gUnk_03002490->unk24 = ((s16 *)gUnk_0873DBAC)[gUnk_030023D4 * 4 + gUnk_030023B4];
        if (gUnk_03002490->unk24 == gUnk_03002490->unk48)
        {
            gUnk_03002490->unk14 = 8;
            sub_0805bc1c();
        }
        if (gUnk_03002490->unk48 < gUnk_03002490->unk24 + gUnk_03002348)
            gUnk_03002490->unk43 = 1;
        else
            gUnk_03002490->unk43 = -1;
        gUnk_03002490->unk04 = (u32)sub_0805bb90;
        sub_08033d0c();
    }
    sub_080062c4();
    sub_0805d8c4(6);
    sub_0805bca4();
    gUnk_03002490->unk14 = 9;
    sub_0805bce0();
}

void sub_0805bb84(void)
{
    sub_0805d8f4();
}

void sub_0805bb90(void)
{
    if (gUnk_03002490->unk43 == 1)
    {
        if (gUnk_03002490->unk48 >= gUnk_03002490->unk24)
        {
            gUnk_03002490->unk48 = gUnk_03002490->unk24;
            gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
            gUnk_03002490->unk04 = (u32)sub_0805b4bc;
            gUnk_03002490->unk14 = 8;
            sub_08006148(sub_0805b354, gCurTaskIdx);
        }
    }
    else
    {
        if (gUnk_03002490->unk48 <= gUnk_03002490->unk24)
        {
            gUnk_03002490->unk48 = gUnk_03002490->unk24;
            gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
            gUnk_03002490->unk04 = (u32)sub_0805b4bc;
            gUnk_03002490->unk14 = 8;
            sub_08006148(sub_0805b354, gCurTaskIdx);
        }
    }
}

void sub_0805bc1c(void)
{
    gUnk_03002490->unk15 = 8;
    gUnk_03002490->unk08 = (u32)sub_0805bc5c;
    gUnk_03002490->unk43 = 1;
    sub_080062c4();
    sub_0805d8c4(6);
    sub_0805bca4();
    sub_08006138();
}

void sub_0805bc50(void)
{
    sub_0805d8f4();
}

void sub_0805bc5c(void)
{
    struct Task *t;

    t = &gUnk_03002790[gUnk_02007D00[gUnk_03002490->unk30]];
    if (t->unk2C == 0)
    {
        gUnk_03002490->unk14 = 9;
        sub_08006148(sub_0805b354, gCurTaskIdx);
    }
}

void sub_0805bca4(void)
{
    struct Task *t;

    t = &gUnk_03002790[gUnk_02007D00[gUnk_03002490->unk30]];
    t->unk2C &= ~(1 << gUnk_03002490->unk88->unk00);
}

void sub_0805bce0(void)
{
    gUnk_03002490->unk15 = 9;
    gUnk_03002490->unk08 = 0;
    if (gUnk_03002490->unk30 == 0)
        TaskYieldTrampoline(50);
    else
        TaskYieldTrampoline(30);
    sub_0805d8c4(0);
    sub_0805e15c();
    gUnk_03002490->unk14 = 10;
    sub_0805bd34();
}

void sub_0805bd28(void)
{
    sub_0805d8f4();
}

void sub_0805bd34(void)
{
    gUnk_03002490->unk04 = (u32)sub_0805b4bc;
    gUnk_03002490->unk15 = 10;
    if ((gUnk_03002490->unk3E & (128 << 8)) != 0)
        gUnk_03002490->unk43 = -1;
    else
        gUnk_03002490->unk43 = 1;
    sub_0805d8c4(7);
    if (gUnk_03002490->unk30 == 0)
        TaskYieldTrampoline(30);
    else if (gUnk_03002490->unk30 != 6)
        sub_0800a04c(gUnk_0873DC9A[gUnk_03002490->unk30],
                     gUnk_03002490->unk88->unk00);
    if (gUnk_02006A14[gUnk_03002490->unk88->unk00] == 0)
    {
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(30);
    }
    gUnk_02007D00[8] &= ~(1 << gUnk_03002490->unk88->unk00);
    while (gUnk_02007D00[8] != 0)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(10);
    *(s8 *)gUnk_03002438 = 1;
    sub_08006138();
}

void sub_0805be3c(void)
{
    sub_0805d8f4();
}

void sub_0805be48(void)
{
    if (((gUnk_03002340 >> gUnk_03002360) & 1) != 0
     && gUnk_03002360 == gUnk_03002490->unk88->unk00
     && (gUnk_03001EA4 & 4) != 0)
        sub_08001a94(8, 0x085B9B2C, 0, 0x00009010, 120, 70);
}

void sub_0805beb0(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_0805c150;
    gUnk_03002490->unk04 = (u32)sub_0805c114;
    gUnk_03002490->unk42 = 12;
    gUnk_03002490->unk38 = gUnk_0874C890;
    gUnk_03002490->unk40 = 0;
    sub_080062c4();
    gUnk_03002490->unk48 = gUnk_03002790[gUnk_03002490->unk44].unk48;
    gUnk_03002490->unk4A = gUnk_03002790[gUnk_03002490->unk44].unk4A + 32;
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 1;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 2;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 3;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 1;
    TaskYieldTrampoline(1);
    while (1)
    {
        gUnk_03002490->unk48 = gUnk_03002790[gUnk_03002490->unk44].unk48;
        gUnk_03002490->unk4A = gUnk_03002790[gUnk_03002490->unk44].unk4A + 32;
        gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
        gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 7;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 7;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk48 = gUnk_03002790[gUnk_03002490->unk44].unk48;
        gUnk_03002490->unk4A = gUnk_03002790[gUnk_03002490->unk44].unk4A + 32;
        gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
        gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
        gUnk_03002490->unk3C = 8;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 10;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 11;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 8;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
    }
}

void sub_0805c0a8(void)
{
    gUnk_03002490->unk04 = 0;
    gUnk_03002490->unk60 = 128 << 7;
    gUnk_03002490->unk68 = 192 << 10;
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 10;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 11;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 8;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 9);
    TaskDispatchTrampoline();
}

void sub_0805c114(void)
{
    if (gUnk_03002790[gUnk_03002490->unk44].unk60 != 0)
        sub_08006148(sub_0805c0a8, gCurTaskIdx);
}

void sub_0805c150(void)
{
    sub_08005d9c();
    {
        s16 *tbl = (s16 *)gUnk_0873DCA8;

        if (tbl[gUnk_03002490->unk3C] != -1
            && sub_080063f0(gUnk_03002490->unk48 - gUnk_03002348,
                            gUnk_03002490->unk4A - gUnk_030023E4 + 48))
        {
            struct Task *q = gUnk_03002490;
            u32 *g = q->unk38;

            sub_08001a94(q->unk42, g[tbl[q->unk3C]], q->unk3E, q->unk40,
                         q->unk48 - gUnk_03002348,
                         (s16)(q->unk4A - gUnk_030023E4 + 48));
        }
    }
}

void sub_0805c204(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_0874C890;
    gUnk_03002490->unk40 = 0;
    gUnk_03002490->unk48 = gUnk_03002790[gUnk_03002490->unk44].unk48
                         + ((s8 *)gUnk_0873DCC0)[sub_08002ee8(8)];
    gUnk_03002490->unk4A = gUnk_03002790[gUnk_03002490->unk44].unk4A
                         + ((s8 *)gUnk_0873DCC8)[sub_08002ee8(4)];
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
    sub_0800622c(gUnk_03002790[gUnk_03002490->unk44].unk58 + (128 << 9),
                 128 << 7, 192 << 10);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 13;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 14;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 15;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 14;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 16;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 17;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 18;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 19;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 20;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 21;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 22;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 23;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 24;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 25;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 26;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 27;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    TaskDispatchTrampoline();
}

void sub_0805c410(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 13;
    gUnk_03002490->unk38 = gUnk_0874C890;
    gUnk_03002490->unk40 = 0;
    gUnk_03002490->unk48 = gUnk_03002790[gUnk_03002490->unk44].unk48
                         + ((s8 *)gUnk_0873DCC0)[sub_08002ee8(8)];
    gUnk_03002490->unk4A = gUnk_03002790[gUnk_03002490->unk44].unk4A
                         + ((s8 *)gUnk_0873DCC8)[sub_08002ee8(4)];
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
    sub_0800622c(gUnk_03002790[gUnk_03002490->unk44].unk58 + (128 << 8),
                 128 << 7, 192 << 10);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 20;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 21;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 22;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 23;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 24;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 25;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 26;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 27;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    TaskDispatchTrampoline();
}

void sub_0805c584(void)
{
    struct Task *t = gUnk_03002490;
    s16 *tbl = (s16 *)gUnk_0873DCCC;

    t->unk4A = 1024 + tbl[t->unk30];
    t->unk50 = t->unk4A << 16;
    (gUnk_03002790 + t->unk28)->unk50 = (tbl[t->unk30] + 1048) << 16;
    if (t->unk30 == 24)
    {
        (gUnk_03002790 + t->unk28)->unk3C = 2;
        (gUnk_03002790 + t->unk28)->unk10 = 2;
    }
}

void sub_0805c5fc(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk04 = (u32)sub_0805c990;
    if ((gUnk_03002340 >> gUnk_03002360) & 1)
    {
        sub_080062c4();
        gUnk_03002490->unk28 = -1;
        gUnk_03002490->unk2C = 0;
        gUnk_03002490->unk48 = 144;
        gUnk_03002490->unk4A = gUnk_03002790[gUnk_03002360].unk4A;
        gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
        gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
        while (gUnk_03002790[gUnk_03002360].unk14 <= 3)
        {
            gUnk_03002490->unk4A = gUnk_03002790[gUnk_03002360].unk4A;
            gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
            TaskYieldTrampoline(1);
        }
        gUnk_03002490->unk4A = gUnk_03002790[gUnk_03002360].unk4A;
        gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk58 = 0xFFF78000;
        {
            u8 *t1 = (u8 *)gUnk_0873DD16;

            TaskYieldTrampoline(t1[gUnk_03002790[gUnk_03002360].unk34]);
        }
        gUnk_03002490->unk60 = 136 << 7;
        TaskYieldTrampoline(32);
        sub_080062c4();
        {
            u8 *t2 = (u8 *)gUnk_0873DD30;

            TaskYieldTrampoline(t2[gUnk_03002790[gUnk_03002360].unk34]);
        }
        gUnk_03002490->unk60 = 128 << 7;
        while (gUnk_03002490->unk4A
               < gUnk_0873DBD4[gUnk_03002790[gUnk_03002360].unk30] - 2)
            TaskYieldTrampoline(1);
        sub_080062c4();
        sub_08006138();
    }
    else
    {
        for (gUnk_030023D4 = 0; gUnk_030023D4 < gUnk_030023AC; gUnk_030023D4++)
        {
            if ((gUnk_03002340 >> gUnk_030023D4) & 1)
            {
                gUnk_03002490->unk28 = gUnk_030023D4;
                break;
            }
        }
        gUnk_03002490->unk2C = 0;
        sub_0805c814();
    }
}

void sub_0805c814(void)
{
    sub_080062c4();
    gUnk_03002490->unk48 = 144;
    gUnk_03002490->unk4A = gUnk_03002790[gUnk_03002490->unk28].unk4A;
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
    while (gUnk_03002790[gUnk_03002490->unk28].unk14 <= 3)
    {
        gUnk_03002490->unk4A = gUnk_03002790[gUnk_03002490->unk28].unk4A;
        gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
        TaskYieldTrampoline(1);
    }
    gUnk_03002490->unk4A = gUnk_03002790[gUnk_03002490->unk28].unk4A;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFF78000;
    {
        u8 *t1 = (u8 *)gUnk_0873DD16;

        TaskYieldTrampoline(t1[gUnk_03002790[gUnk_03002490->unk28].unk34]
                            - (s16)gUnk_03002790[gUnk_03002490->unk28].unk70 + 1);
    }
    gUnk_03002490->unk60 = 136 << 7;
    TaskYieldTrampoline(32);
    sub_080062c4();
    {
        u8 *t2 = (u8 *)gUnk_0873DD30;

        TaskYieldTrampoline(t2[gUnk_03002790[gUnk_03002490->unk28].unk34]);
    }
    gUnk_03002490->unk60 = 128 << 7;
    while (gUnk_03002490->unk4A
           < gUnk_0873DBD4[gUnk_03002790[gUnk_03002490->unk28].unk30] - 2)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08006138();
}

void sub_0805c990(void)
{
    u16 x;
    u16 y;

    x = gUnk_03002490->unk48;
    y = gUnk_03002490->unk4A + gUnk_03002490->unk2C;
    if ((s16)y > 0x41C)
        y = 0x41C;
    sub_08026264((s16)x, (s16)y);
    if (gUnk_03002490->unk2C != 0)
    {
        if (gUnk_03002490->unk2C > 0)
        {
            gUnk_03002490->unk2C -= 4;
            if (gUnk_03002490->unk2C < 0)
                gUnk_03002490->unk2C = 0;
        }
        else
        {
            gUnk_03002490->unk2C += 4;
            if (gUnk_03002490->unk2C > 0)
                gUnk_03002490->unk2C = 0;
        }
    }
    if (gUnk_03002490->unk28 >= 0)
    {
        for (gUnk_030023D4 = 0; gUnk_030023D4 < gUnk_030023AC; gUnk_030023D4++)
        {
            if ((s16)gUnk_03002790[gUnk_03002490->unk28].unk70 < 0)
            {
                if (((gUnk_03002340 >> gUnk_030023D4) & 1)
                    && gUnk_03002490->unk28 != gUnk_030023D4
                    && (s16)gUnk_03002790[gUnk_030023D4].unk70 >= 0)
                {
                    gUnk_03002490->unk2C = 0;
                    gUnk_03002490->unk28 = gUnk_030023D4;
                    gUnk_03002490->unk4A = gUnk_03002790[gUnk_030023D4].unk4A;
                    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
                    sub_08006148(sub_0805c814, gCurTaskIdx);
                }
            }
            else
            {
                if (((gUnk_03002340 >> gUnk_030023D4) & 1)
                    && gUnk_03002490->unk28 != gUnk_030023D4
                    && (s16)gUnk_03002790[gUnk_030023D4].unk70 > 0
                    && gUnk_03002790[gUnk_03002490->unk28].unk34
                           < gUnk_03002790[gUnk_030023D4].unk34)
                {
                    gUnk_03002490->unk2C = gUnk_03002490->unk4A + gUnk_03002490->unk2C
                                         - gUnk_03002790[gUnk_030023D4].unk4A;
                    gUnk_03002490->unk28 = gUnk_030023D4;
                    gUnk_03002490->unk4A = gUnk_03002790[gUnk_030023D4].unk4A;
                    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
                    sub_08006148(sub_0805c814, gCurTaskIdx);
                }
            }
        }
    }
}

void sub_0805cb30(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 8;
    if (gUnk_03002350 == 1)
        gUnk_03002490->unk38 = gUnk_08754850;
    else
        gUnk_03002490->unk38 = (u32 *)gUnk_0873DD4C[gUnk_03002490->unk73];
    gUnk_03002490->unk40 = 0x00009010;
    {
        s16 *tbl = (s16 *)gUnk_0873DBAC;

        gUnk_03002490->unk4C = tbl[(gUnk_03002350 << 2)
            + gUnk_03002790[gUnk_03002490->unk44].unk2C] << 16;
    }
    gUnk_03002490->unk50 = 131 << 19;
    while (1)
    {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(4);
    }
}

void sub_0805cbec(void)
{
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_0875488C;
    gUnk_03002490->unk04 = (u32)sub_0805cc54;
    gUnk_03002490->unk40 = 0x0000A010;
    if (gUnk_03002360 == gUnk_03002490->unk73)
    {
        gUnk_03002490->unk3C = 4;
    }
    else
    {
        u16 *tbl = (u16 *)gUnk_0873DD5C;
        gUnk_03002490->unk3C = tbl[gUnk_03002490->unk73];
    }
    TaskYieldTrampoline(27);
    TaskDispatchTrampoline();
}

s32 sub_0805cc54(void)
{
    gUnk_03002490->unk48 = (gUnk_03002790 + gUnk_03002490->unk44)->unk48;
    gUnk_03002490->unk4A = (gUnk_03002790 + gUnk_03002490->unk44)->unk4A - 24;
}

void sub_0805cca0(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 9;
    gUnk_03002490->unk38 = (u32 *)gUnk_0873DD64[gUnk_03002490->unk73];
    gUnk_03002490->unk4C = 200 << 16;
    {
        s16 *t = (s16 *)gUnk_0873DBD4;

        gUnk_03002490->unk50 = (t[gUnk_03002490->unk73] - 8) << 16;
    }
    gUnk_03002490->unk40 = 0x00008010;
    gUnk_03002490->unk28 = 1;
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk3C = 0;
    do
    {
        TaskYieldTrampoline(1);
    } while (gUnk_03002490->unk28 != 0 || gUnk_03002490->unk2C != 0);
    gUnk_03002490->unk46 = sub_080058e4(86, 32);
    (gUnk_03002790 + gUnk_03002490->unk46)->unk74 = gUnk_03002490->unk73;
    (gUnk_03002790 + gUnk_03002490->unk46)->unk28 = gUnk_02007D00[7];
    gUnk_02007D00[7]++;
    if (gUnk_03002490->unk73 == 0)
    {
        TaskYieldTrampoline(24);
        gUnk_03002490->unk04 = (u32)sub_0805ceec;
        gUnk_03002490->unk28 = 1;
        do
        {
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk28 != 0);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C = 1;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 2;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 3;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 4;
            TaskYieldTrampoline(24);
            gUnk_03002490->unk3C = 3;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 2;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 1;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 2);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 4;
        sub_08006138();
    }
    else
    {
        gUnk_03002490->unk28 = 1;
        do
        {
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk28 != 0);
        gUnk_03002490->unk58 = 0xFFF8CD00;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = 0xFFFB3300;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = 0xFFFD9A00;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = 0x00026600;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk58 = 0x0004CD00;
        TaskYieldTrampoline(5);
        sub_080062c4();
        TaskYieldTrampoline(27);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 4;
    }
    sub_08006138();
}

void sub_0805ceec(void)
{
    gUnk_03002490->unk48 = (gUnk_03002790 + gUnk_03002490->unk46)->unk48 - 4;
    gUnk_03002490->unk4A = (gUnk_03002790 + gUnk_03002490->unk46)->unk4A - 16;
}

void sub_0805cf3c(void)
{
    gUnk_03002490->unk04 = (u32)sub_0805d420;
    gUnk_03002490->unk4C = 248 << 16;
    gUnk_03002490->unk43 = 255;
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk30 = 0;
    gUnk_03002490->unk24 = 0;
    if (gUnk_03002490->unk74 == 0)
        gUnk_03002490->unk73 = 1;
    else
        gUnk_03002490->unk73 = 0;
    sub_0805d994(gUnk_03002490->unk28, gUnk_03002490->unk73);
    gUnk_03002490->unk42 = 8;
    switch (gUnk_03002490->unk73)
    {
    case 1:
        if (((gUnk_03002340 >> gUnk_03002360) & 1)
            && (gUnk_03002790 + gUnk_03002360)->unk30 != 0)
            gUnk_03002490->unk0C = 0;
        gUnk_03002490->unk4C = 244 << 16;
        gUnk_03002490->unk50 = 196 << 16;
        sub_0805d8a4((s32)gUnk_0873DDB4);
        gUnk_03002490->unk54 = 0xFFFB3300;
        gUnk_03002490->unk58 = 0xFFFC6600;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0xFFFD9A00;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0xFFFECD00;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0xFFFF6600;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0x00009A00;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0x00013300;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0x00026600;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0x0004CD00;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk58 = 0;
        TaskYieldTrampoline(5);
        sub_080062c4();
        TaskYieldTrampoline(53);
        sub_0805d8a4((s32)gUnk_0873DDA2);
        gUnk_03002490->unk54 = 0xFFFD6000;
        TaskYieldTrampoline(60);
        sub_0805d8a4((s32)gUnk_0873DD80);
        sub_080062c4();
        (gUnk_03002790 + gUnk_03002490->unk44)->unk28 = 0;
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk58 = 0xFFFECD00;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk58 = 0xFFFF6600;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk58 = 0x00009A00;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk58 = 0x00013300;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 7);
        gUnk_03002490->unk58 = 0;
        sub_0805d8a4((s32)gUnk_0873DDA2);
        gUnk_03002490->unk54 = 0x00009A00;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0x00013300;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0x00026600;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0x0004CD00;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk54 = 0x0004CD00;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0x00026600;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0x00013300;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0x00009A00;
        TaskYieldTrampoline(5);
        for (gUnk_03002490->unk6C = 0;
             (s16)gUnk_03002490->unk6C < gUnk_030023AC;
             gUnk_03002490->unk6C++)
        {
            if (((gUnk_03002340 >> (s16)gUnk_03002490->unk6C) & 1)
                && (gUnk_03002790 + (s16)gUnk_03002490->unk6C)->unk30 == 0)
            {
                gUnk_03002490->unk2C |= 1 << (s16)gUnk_03002490->unk6C;
                gUnk_03002490->unk30++;
            }
        }
        gUnk_03002490->unk34 = gUnk_03002490->unk30 - 1;
        gUnk_03002490->unk54 = 0xFFFF6600;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0xFFFECD00;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0xFFFD9A00;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk54 = 0xFFFB3300;
        TaskYieldTrampoline(60);
        sub_080062c4();
        if (gUnk_02007D00[9] != 0)
        {
            gUnk_03002490->unk43 = 1;
            gUnk_03002490->unk54 = 192 << 10;
            TaskYieldTrampoline(36);
            gUnk_03002490->unk54 = 128 << 10;
            TaskYieldTrampoline(8);
            gUnk_03002490->unk54 = 128 << 9;
            TaskYieldTrampoline(8);
            gUnk_03002490->unk54 = 128 << 8;
            TaskYieldTrampoline(8);
            sub_080062c4();
            gUnk_03002490->unk6C = 0;
            do
            {
                for (gUnk_03002490->unk6E = 0;
                     gUnk_03002490->unk6E < gUnk_030023AC;
                     gUnk_03002490->unk6E++)
                {
                    if (((gUnk_03002340 >> gUnk_03002490->unk6E) & 1)
                        && ((u8 *)gUnk_02006A14)[gUnk_03002490->unk6E] == 0)
                    {
                        gUnk_03002490->unk46 = sub_080058e4(89, 32);
                        (gUnk_03002790 + gUnk_03002490->unk46)->unk2C
                            = gUnk_03002490->unk6E;
                        (gUnk_03002790 + gUnk_03002490->unk46)->unk73 = 1;
                    }
                }
                TaskYieldTrampoline(11);
                gUnk_03002490->unk6C++;
            } while ((s16)gUnk_03002490->unk6C <= 29);
        }
        sub_08006138();
        break;
    case 0:
        gUnk_03002490->unk4C = 240 << 16;
        {
            s16 *t = (s16 *)gUnk_0873DBD4;

            gUnk_03002490->unk50 = (t[gUnk_03002490->unk74] - 2) << 16;
        }
        sub_0805d8a4((s32)gUnk_0873DDBE);
        gUnk_03002490->unk54 = 0xFFFE0000;
        TaskYieldTrampoline(23);
        sub_080062c4();
        sub_0805d8a4(0);
        gUnk_03002490->unk43 = 1;
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_08006338(165);
            TaskYieldTrampoline(4);
            sub_08006338(166);
            TaskYieldTrampoline(4);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 3);
        sub_08006338(167);
        TaskYieldTrampoline(4);
        (gUnk_03002790 + gUnk_03002490->unk44)->unk28 = 0;
        sub_08006338(165);
        TaskYieldTrampoline(2);
        sub_08006338(168);
        TaskYieldTrampoline(6);
        sub_08006338(169);
        TaskYieldTrampoline(4);
        sub_08006338(170);
        TaskYieldTrampoline(4);
        sub_08006338(171);
        sub_08006138();
        break;
    }
}

void sub_0805d420(void)
{
    sub_0805d8f4();
    if (gUnk_03002490->unk2C == 0)
        return;
    for (gUnk_03002490->unk6C = gUnk_030023AC - 1;
         (s16)gUnk_03002490->unk6C >= 0;
         gUnk_03002490->unk6C--)
    {
        if (((gUnk_03002340 >> (s16)gUnk_03002490->unk6C) & 1)
            && ((gUnk_03002490->unk2C >> (s16)gUnk_03002490->unk6C) & 1)
            && gUnk_03002490->unk48 < gUnk_0873DBAC[(gUnk_03002490->unk30 << 2) + gUnk_03002490->unk34] - 6)
        {
            if (gUnk_02006A14[(s16)gUnk_03002490->unk6C] != 0)
            {
                gUnk_03002490->unk46 = sub_080058e4(89, 32);
                (gUnk_03002790 + gUnk_03002490->unk46)->unk2C = (s16)gUnk_03002490->unk6C;
                (gUnk_03002790 + gUnk_03002490->unk46)->unk30 = gUnk_03002490->unk30;
                (gUnk_03002790 + gUnk_03002490->unk46)->unk34 = gUnk_03002490->unk34;
                (gUnk_03002790 + gUnk_03002490->unk46)->unk73 = 0;
            }
            gUnk_03002490->unk2C &= ~(1 << (s16)gUnk_03002490->unk6C);
            gUnk_03002490->unk34--;
        }
    }
    gUnk_03002490->unk24++;
}

void sub_0805d564(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_0805d5fc;
    gUnk_03002490->unk42 = 15;
    gUnk_03002490->unk38 = gUnk_087548A0;
    gUnk_03002490->unk40 = 0x0000B010;
    gUnk_03002490->unk4C = (gUnk_03002790 + gUnk_03002490->unk44)->unk48 << 16;
    gUnk_03002490->unk50 = (gUnk_03002790 + gUnk_03002490->unk44)->unk4A << 16;
    gUnk_03002490->unk3C = 0;
    while (1)
    {
        gUnk_03002490->unk6C = 0;
        do
        {
            TaskYieldTrampoline(3);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 3);
    }
}

void sub_0805d5fc(void)
{
    sub_08005d9c();
    if (sub_08005acc() != 0)
    {
        u32 *tbl = gUnk_0874CDF8;
        struct Task *t = gUnk_03002490;

        sub_08001a94(14, tbl[(s16)t->unk6C], 0, 0,
                     t->unk48 - gUnk_03002348,
                     (s16)(t->unk4A - gUnk_030023E4));
    }
}

void sub_0805d668(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 10;
    gUnk_03002490->unk38 = gUnk_0874CCF4;
    gUnk_03002490->unk40 = 240 << 8;
    gUnk_03002490->unk3C = 4;
    switch (gUnk_03002490->unk73)
    {
    case 0:
        gUnk_03002490->unk4C = (gUnk_0873DBAC[(gUnk_03002490->unk30 << 2)
            + gUnk_03002490->unk34] - 6) << 16;
        gUnk_03002490->unk50 = (gUnk_03002790 + gUnk_03002490->unk44)->unk4A << 16;
        gUnk_03002490->unk24 = 44 - gUnk_03002790[gUnk_03002490->unk44].unk24;
        if (gUnk_03002490->unk24 & 1)
            TaskYieldTrampoline(1);
        gUnk_03002490->unk58 = 0xFFFE0000;
        TaskYieldTrampoline(gUnk_03002490->unk24 >> 1);
        gUnk_03002490->unk54 = 128 << 8;
        gUnk_03002490->unk58 = 0xFFFE0000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 0xFFFF0000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 0xFFFF8000;
        TaskYieldTrampoline(4);
        sub_080062c4();
        TaskYieldTrampoline(5);
        gUnk_03002490->unk58 = 128 << 9;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 128 << 10;
        TaskYieldTrampoline(gUnk_03002490->unk24 >> 1);
        TaskYieldTrampoline(22);
        break;
    case 1:
        gUnk_03002490->unk4C = gUnk_03002790[gUnk_03002490->unk44].unk48 << 16;
        gUnk_03002490->unk50 = gUnk_03002790[gUnk_03002490->unk44].unk4A << 16;
        gUnk_03002490->unk54 = (gUnk_03002790[gUnk_03002490->unk2C].unk48
            - gUnk_03002790[gUnk_03002490->unk44].unk48) << 11;
        gUnk_03002490->unk58 = 0xFFFC0000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 0xFFFE0000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 0xFFFF0000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 0xFFFF8000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 128 << 8;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 128 << 9;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 128 << 10;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 128 << 11;
        TaskYieldTrampoline(4);
        sub_08006214();
        TaskYieldTrampoline(8);
        break;
    }
    if (gUnk_03002490->unk2C == gUnk_03002360)
        sub_080031b8(220);
    sub_08009e60(1, gUnk_03002490->unk2C);
    TaskDispatchTrampoline();
}

void sub_0805d8a4(s32 a0)
{
    gUnk_03002490->unk18 = a0;
    gUnk_03002490->unk1C = 0;
    gUnk_03002490->unk20 = 0;
    if (a0 != 0)
        sub_0805d918();
}

void sub_0805d8c4(s32 a0)
{
    u32 i = (u8)a0;

    if (i > 7)
        while (1) {}
    gUnk_03002490->unk18 = *(gUnk_0873DDE8 + i);
    gUnk_03002490->unk1C = 0;
    gUnk_03002490->unk20 = 0;
    sub_0805d918();
}

void sub_0805d8f4(void)
{
    if (gUnk_03002490->unk18 != 0)
    {
        if (--gUnk_03002490->unk20 <= 0)
            sub_0805d918();
    }
}

void sub_0805d918(void)
{
    struct Task *t;
    s32 i;
    s16 *p;
    s32 c;
    s32 j;
    struct Task *t2;
    s32 k;
    s16 *q;
    s32 v;
    struct Task *t3;
    s32 m;
    s16 *r;

top:
    t = gUnk_03002490;
    i = t->unk1C;
    p = (s16 *)t->unk18;
    c = p[i];
    switch (c)
    {
    case -2:
        return;
    case -3:
        t->unk1C = 0;
        goto top;
    case -4:
        j = i + 1;
        t->unk1C = j;
        sub_0805d8c4(*(u8 *)&p[j]);
        return;
    }
    t2 = gUnk_03002490;
    k = t2->unk1C;
    q = (s16 *)t2->unk18;
    v = q[k];
    k++;
    t2->unk1C = k;
    sub_08006338(v);
    t3 = gUnk_03002490;
    m = t3->unk1C;
    r = (s16 *)t3->unk18;
    t3->unk20 = r[m];
    m++;
    t3->unk1C = m;
}

void sub_0805d994(s32 a0, s32 a1)
{
    if (a1 == 0)
    {
        if (a0 <= 1)
            gUnk_03002490->unk40 = ((a0 << 3) + 768) | -16368;
        else
            gUnk_03002490->unk40 = ((a0 << 3) + 896) | -16368;
    }
    else
    {
        if (a0 <= 1)
            gUnk_03002490->unk40 = ((a0 << 3) + 768) | -12272;
        else
            gUnk_03002490->unk40 = ((a0 << 3) + 896) | -12272;
    }
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_0805da2c;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk38 = gUnk_0874CFEC;
}

void sub_0805da2c(void)
{
    struct Task *t;
    struct Task *s;
    struct Task *u;
    struct TaskGfx *g;
    u32 *tbl;
    u16 *p;
    u8 *dst;
    u16 a;
    u16 **pal;

    t = gUnk_03002490;
    if (t->unk38 == NULL)
        return;
    if (t->unk3C == -1)
        return;
    if (sub_08005acc() == 0)
        return;
    s = gUnk_03002490;
    a = s->unk40;
    tbl = s->unk38;
    g = (struct TaskGfx *)tbl[s->unk3C];
    if ((g->unk00 & 1) != 0)
    {
        pal = &g->unk04;
        if (g->unk04 != NULL)
            sub_080017e4(2, g->unk04 + 1, gUnk_03001470 + ((a >> 12) << 5), *g->unk04);
        p = pal[1];
        dst = (u8 *)(((a & 0x7FF) << 5) + 0x0600FE00);
        while (*p != 0xFFFF)
        {
            u16 *q = p + 1;
            sub_080017e4(4, q, dst, *p);
            p = (u16 *)((u8 *)q + *p);
            dst += 0x400;
        }
    }
    else
    {
        if (*g->unk04 != 0)
            sub_080017e4(2, g->unk04 + 1, gUnk_03001470 + ((a >> 12) << 5), *g->unk04);
        p = g->unk08;
        dst = (u8 *)(((a & 0x7FF) << 5) + 0x0600FE00);
        while (*p != 0xFFFF)
        {
            u16 *q = p + 1;
            sub_080017e4(4, q, dst, *p);
            p = (u16 *)((u8 *)q + *p);
            dst += 0x400;
        }
    }
    u = gUnk_03002490;
    sub_08001a94(u->unk42, g->unk00 & ~1, u->unk3E, 0x800 | u->unk40,
                 u->unk48 - gUnk_03002348, (s16)(u->unk4A - gUnk_030023E4));
}

void sub_0805dba0(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk04 = 0;
    t->unk08 = 0;
    t->unk00 = (u32)sub_080059d8;
    t->unk38 = gUnk_0874CFEC;
    sub_080062c4();
    u = gUnk_03002490;
    u->unk3E &= 0x7FFF;
    u->unk88->unk42 &= 0xFFEF;
    sub_08002e98(u->unk14, 2, gUnk_0873DEA0);
}

void sub_0805dbfc(void)
{
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_0873DEA0);
}

/* NOTE: needs hdr.c corrected to "extern u16 gUnk_0873DEA8[];" (stride 2, ldrh, symbol-first) */
void sub_0805dc18(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0805dd4c;
    t->unk43 = 1;
    u = gUnk_03002490;
    u->unk3E &= 0x7FFF;
    switch ((s8)u->unk88->unk0D)
    {
    case 0:
    case 7:
    case 20:
    case 21:
    case 24:
        v = gUnk_03002490;
        v->unk3E |= 128 << 8;
        break;
    case 1:
    case 2:
    case 5:
    case 19:
        while (1)
        {
            w = gUnk_03002490;
            w->unk3C = gUnk_0873DEA8[(s8)w->unk88->unk0D];
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
    }
    x = gUnk_03002490;
    x->unk3C = gUnk_0873DEA8[(s8)x->unk88->unk0D];
    sub_08006138();
}

void sub_0805dd4c(void)
{
    if (gUnk_03002350 == 1)
    {
        if ((s16)gUnk_03004CA0[gUnk_03002490->unk46] == -1)
            sub_0805deac();
    }
}

void sub_0805dd88(void)
{
    gUnk_03002490->unk04 = (u32)sub_0805dd4c;
    gUnk_03002490->unk43 = 1;
    sub_08006364(146);
    sub_08006138();
}

void sub_0805ddb0(s32 a0)
{
    struct Task *t;

    t = &gUnk_03002790[a0];
    t->unk46 = sub_0805b088((s8)a0, 19, 0);
    gUnk_03002790[t->unk46].unk44 = a0;
    gUnk_03002790[t->unk46].unk88 = t->unk88;
    switch ((s8)gUnk_03002170[a0].unk0D)
    {
    case 0:
    case 7:
    case 11:
    case 20:
    case 21:
        break;
    default:
        gUnk_03002790[sub_0805afac((s8)a0, 17, 0)].unk44 = a0;
        break;
    }
    gUnk_03002170[a0].unk0D = 0;
    gUnk_03002170[a0].unk04 = 22;
}

void sub_0805deac(void)
{
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        if (((gUnk_03002340 >> i) & 1) != 0)
        {
            if (gUnk_030023AC != 1)
            {
                switch ((s8)gUnk_03002170[i].unk0D)
                {
                case 0:
                case 7:
                case 11:
                case 20:
                case 21:
                    break;
                default:
                    gUnk_03002790[sub_0805afac((s8)i, 17, 0)].unk44 = i;
                    break;
                }
                gUnk_03002170[i].unk0D = 0;
                gUnk_03002170[i].unk04 = 22;
            }
            sub_08006148(sub_0805e15c, i);
        }
    }
}

void sub_0805df9c(void)
{
    if (gUnk_030023AC != 1)
        gUnk_03002490->unk14 = 0;
    else
        gUnk_03002490->unk14 = gUnk_030023AC;
    sub_08006148(sub_0805dba0, gCurTaskIdx);
    gUnk_03002490->unk88->unk16 = 2;
    sub_080062c4();
}

void sub_0805dfe8(void)
{
    struct Task *t;
    u16 d;

    t = gUnk_03002490;
    d = t->unk48 - gUnk_03002348;
    if (t->unk43 == 1)
    {
        if ((s16)d >= t->unk18)
            sub_0805df9c();
    }
    else
    {
        if ((s16)d <= t->unk18)
            sub_0805df9c();
    }
}

void sub_0805e038(s32 a0)
{
    struct PlayerState *ps;
    struct Task *t;
    u16 v;
    s32 w;

    ps = &gUnk_03002170[a0];
    t = &gUnk_03002790[a0];
    switch (gUnk_030023AC)
    {
    case 1:
        v = (ps->unk00 + 1) * 104 + 24;
        break;
    case 2:
        v = (ps->unk00 + 1) * 69 + 24;
        break;
    case 3:
        v = (ps->unk00 + 1) * 52 + 24;
        break;
    case 4:
        v = (ps->unk00 + 1) * 41 + 24;
        break;
    }
    w = (s16)v;
    t->unk18 = w;
    if (gUnk_0300244C == 0)
    {
        if (t->unk48 - w <= 0)
            t->unk43 = 1;
        else
            t->unk43 = 255;
    }
    else
    {
        if (t->unk48 - gUnk_03002348 - w <= 0)
            t->unk43 = 1;
        else
            t->unk43 = 255;
    }
}

void sub_0805e110(s32 a0)
{
    struct PlayerState *ps;
    struct Task *t;

    ps = &gUnk_03002170[a0];
    t = &gUnk_03002790[a0];
    sub_08068a8c(a0, 1);
    sub_0805e038(a0);
    sub_08006148(sub_08033d0c, a0);
    ps->unk16 = 1;
    t->unk04 = (u32)sub_0805dfe8;
}

void sub_0805e15c(void)
{
    struct PlayerState *ps;
    struct Task *t;

    ps = gUnk_03002490->unk88;
    ps->unk05 = ps->unk04;
    gUnk_03002490->unk88->unk04 = 22;
    t = gUnk_03002490;
    t->unk04 = 0;
    t->unk08 = 0;
    t->unk88->unk64 = 0;
    t->unk88->unk68 = 0;
    t->unk88->unk6C = 0;
    t->unk64 = 128 << 24;
    t->unk2C = t->unk50;
    t->unk28 = t->unk4C;
    if (((u8 *)gUnk_02000020)[0] == 1)
    {
        sub_0805e1bc();
    }
    else
    {
        sub_0805e24c();
        sub_08006138();
    }
}

void sub_0805e1bc(void)
{
    struct Task *t;

    if (*(s8 *)gUnk_02008010 < 0)
        *(s8 *)gUnk_02008010 = sub_08002ee8(7);
    t = gUnk_03002490;
    if (t->unk30 != 0)
    {
        if (gUnk_03002360 == t->unk88->unk00)
            sub_08003110(14);
        sub_08002e98(*(s8 *)gUnk_02008010, 14, gUnk_0873DEDC);
    }
    else
    {
        if (gUnk_03002360 == t->unk88->unk00)
            sub_08003110(13);
        sub_08002e98(*(s8 *)gUnk_02008010 + 7, 14, gUnk_0873DEDC);
    }
}

void sub_0805e24c(void)
{
    sub_08006364(146);
    TaskYieldTrampoline(30);
    if (*(s8 *)gUnk_02008010 < 0)
    {
        *(s8 *)gUnk_02008010 = sub_08002ee8(7) + 7;
        gUnk_03002490->unk34 = 1;
    }
    else
    {
        gUnk_03002490->unk34 = 0;
    }
    if (*(u8 *)gUnk_020060CC == 0)
    {
        sub_08003110(13);
        *(u8 *)gUnk_020060CC = 1;
    }
    sub_08002e98(*(s8 *)gUnk_02008010, 14, gUnk_0873DEDC);
    TaskYieldTrampoline(60);
    if (gUnk_03002490->unk34 != 0)
        sub_080258e0();
}

void sub_0805e2d4(void)
{
    sub_0800625c(128 << 9, 0, 0x5A5A5A5A, 144 << 10, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 147 << 1;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFD8000;
    gUnk_03002490->unk60 = 128 << 7;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000135;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(4);
    sub_0800625c(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFFD4000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    sub_0800625c(0xFFFF8000, 0, 0x5A5A5A5A, 0xFFFB0000, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFC1000;
    gUnk_03002490->unk60 = 192 << 6;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x25;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 0x2F;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x2C;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x27;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x21;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x25;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x2F;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x2C;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x27;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x21;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(1);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(128 << 9, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    sub_0800625c(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFF8800;
    gUnk_03002490->unk5C = 192 << 5;
    gUnk_03002490->unk3C = 0x66;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_080062c4();
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 240 << 7;
    gUnk_03002490->unk5C = 0xFFFFE800;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_0800625c(128 << 7, 0, 0x5A5A5A5A, 242 << 7, 0xFFFFF500, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x7B;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk54 = 0;
    TaskYieldTrampoline(15);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_080062c4();
    gUnk_03002490->unk3E &= 0x7FFF;
    sub_0805afac(gUnk_03002490->unk88->unk00, 16, 0);
    gUnk_03002490->unk3C = 0x92;
    TaskYieldTrampoline(0x15);
}

void sub_0805e7b4(void)
{
    gUnk_03002490->unk58 = 144 << 10;
    gUnk_03002490->unk60 = 0xFFFF8000;
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 9;
    TaskYieldTrampoline(4);
    sub_0800625c(152 << 8, 0, 0x5A5A5A5A, 0xFFFBE000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 4;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk58 = 0xFFF8E000;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFF2000;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 188 << 11;
    gUnk_03002490->unk3E &= 0x7FFF;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 128 << 6;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(12);
    sub_0800625c(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E |= 128 << 8;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFC8000;
    gUnk_03002490->unk3C = 97;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk5C = 160 << 8;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk54 = 0xFFFF8000;
    gUnk_03002490->unk5C = 0;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E &= 0x7FFF;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 128 << 10;
    gUnk_03002490->unk3C = 97;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk5C = 0xFFFF6000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk54 = 0xFFFF8000;
    gUnk_03002490->unk5C = 0;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0xFFFE0000, 128 << 9, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(3);
    sub_080062c4();
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 192 << 8;
    gUnk_03002490->unk5C = 0xFFFFD000;
    gUnk_03002490->unk3C = 102;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 0xFFFE8000;
    gUnk_03002490->unk5C = 192 << 5;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 17;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(1);
    sub_080062c4();
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(128 << 8, 0, 0x5A5A5A5A, 0xFFFEA000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(10);
    sub_0800625c(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_080062c4();
    TaskYieldTrampoline(2);
    sub_0800625c(128 << 8, 0, 0x5A5A5A5A, 0xFFFE6000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(12);
    sub_0800625c(128 << 7, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(7);
    sub_080062c4();
    gUnk_03002490->unk3E |= 128 << 8;
    sub_0805afac(gUnk_03002490->unk88->unk00, 16, 0);
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(21);
}

void sub_0805eb2c(s32 a0, s32 a1, s32 a2)
{
    sub_0800625c(0xFFFFC000, 0, 0x5A5A5A5A, 0xFFFD4000, 128 << 8, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x133;
    TaskYieldTrampoline(10);
    sub_0800625c(0, 0, 0x5A5A5A5A, 144 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 123;
    TaskYieldTrampoline(8);
    sub_0800625c(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(3);
    sub_0800625c(0xFFFFC000, 0, 0x5A5A5A5A, 0xFFFD4000, 128 << 8, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x133;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3E |= 128 << 8;
    sub_0800625c(0, 0, 0x5A5A5A5A, 144 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 123;
    TaskYieldTrampoline(8);
    sub_0800625c(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk54 = 0xFFF80000;
    gUnk_03002490->unk5C = 128 << 9;
    gUnk_03002490->unk3C = 100;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0;
    TaskYieldTrampoline(2);
    sub_0800625c(192 << 12, 0xFFFFD000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 30;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 180 << 10;
    TaskYieldTrampoline(15);
    gUnk_03002490->unk54 = 128 << 7;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 32;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_080062c4();
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFF8000, 0, 0x5A5A5A5A, 0xFFFE0000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(15);
    sub_0800625c(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(4);
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFF8000, 0, 0x5A5A5A5A, 0xFFFE0000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(15);
    sub_0800625c(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(4);
    sub_0800625c(240 << 8, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 32;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_080062c4();
    gUnk_03002490->unk3E |= 128 << 8;
    sub_0805afac(gUnk_03002490->unk88->unk00, 16, 0);
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(21);
}

void sub_0805ee90(void)
{
    gUnk_03002490->unk58 = 144 << 9;
    gUnk_03002490->unk60 = 0xFFFFC000;
    gUnk_03002490->unk3C = 150;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFC8000;
    gUnk_03002490->unk60 = 128 << 7;
    gUnk_03002490->unk3C = 0x133;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 151;
    TaskYieldTrampoline(12);
    gUnk_03002490->unk3C = 0x133;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 144 << 9;
    gUnk_03002490->unk60 = 0xFFFFC000;
    gUnk_03002490->unk3C = 150;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFC8000;
    gUnk_03002490->unk60 = 128 << 7;
    gUnk_03002490->unk3C = 0x133;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 151;
    TaskYieldTrampoline(12);
    gUnk_03002490->unk60 = 192 << 6;
    gUnk_03002490->unk3C = 0x133;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 208 << 8;
    gUnk_03002490->unk60 = 128 << 6;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 192 << 11;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C = 310;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFB2000;
    gUnk_03002490->unk60 = 0xFFFFC000;
    gUnk_03002490->unk3C = 0x133 - 8;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 224 << 8;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 0xFFFDA000;
    gUnk_03002490->unk60 = 128 << 7;
    gUnk_03002490->unk3C = 151 + 161;
    TaskYieldTrampoline(14);
    gUnk_03002490->unk3C = 0x133 - 8;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 192 << 11;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C = 310;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFB2000;
    gUnk_03002490->unk60 = 0xFFFF8000;
    gUnk_03002490->unk3C = 0x133 - 8;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 224 << 8;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 0xFFFBC000;
    gUnk_03002490->unk60 = 128 << 7;
    gUnk_03002490->unk3C = 151 + 161;
    TaskYieldTrampoline(14);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 0x139;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C = 106;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 128 << 10;
    gUnk_03002490->unk60 = 0xFFFF0000;
    TaskYieldTrampoline(3);
    sub_08006244();
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFEA000;
    gUnk_03002490->unk5C = 128 << 6;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3E &= 0x7FFF;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 17;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(1);
    sub_080062c4();
    gUnk_03002490->unk3E |= 128 << 8;
    sub_0805afac(gUnk_03002490->unk88->unk00, 16, 0);
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(21);
}

void sub_0805f1bc(void)
{
    gUnk_03002490->unk54 = 128 << 9;
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 20;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 128 << 8;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 128 << 7;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 128 << 9;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 128 << 7;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk54 = 176 << 8;
    gUnk_03002490->unk5C = 0xFFFFF000;
    gUnk_03002490->unk3C = 102;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 17;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 110;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 128 << 9;
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 102;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
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
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 17;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFF38000;
    gUnk_03002490->unk5C = 0xFFFE8000;
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 0x10F7;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 160 << 10;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 140 << 13;
    gUnk_03002490->unk3C = 104;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 107;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 200 << 12;
    gUnk_03002490->unk5C = 128 << 8;
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 0x10F7;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFFD8000;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFEE8000;
    gUnk_03002490->unk58 = 128 << 10;
    gUnk_03002490->unk3C = 37;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk58 = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3E |= 128 << 8;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFF38000, 0xFFFE8000, 0x5A5A5A5A, 0xFFFE0000, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 0x10F7;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 160 << 10;
    gUnk_03002490->unk58 = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 140 << 13;
    gUnk_03002490->unk3C = 104;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 107;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 200 << 12;
    gUnk_03002490->unk5C = 128 << 8;
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 0x10F7;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFFD8000;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFEE8000;
    gUnk_03002490->unk58 = 128 << 10;
    gUnk_03002490->unk3C = 37;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk58 = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3E |= 128 << 8;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFF38000, 0xFFFE8000, 0x5A5A5A5A, 0xFFFE0000, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 0x10F7;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFFD8000;
    gUnk_03002490->unk58 = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 140 << 13;
    gUnk_03002490->unk3C = 104;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 37;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 200 << 12;
    gUnk_03002490->unk5C = 128 << 8;
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 0x10F7;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFFD8000;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFEE8000;
    gUnk_03002490->unk58 = 128 << 10;
    gUnk_03002490->unk3C = 37;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk58 = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 37;
    TaskYieldTrampoline(2);
    sub_0800625c(128 << 8, 0, 0x5A5A5A5A, 0xFFFC6000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 31;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFD9000;
    TaskYieldTrampoline(3);
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
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_0800625c(0, 0, 0x5A5A5A5A, 128 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(7);
    sub_080062c4();
    gUnk_03002490->unk3E |= 128 << 8;
    sub_0805afac(gUnk_03002490->unk88->unk00, 16, 0);
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(21);
}

void sub_0805f778(void)
{
    gUnk_03002490->unk54 = 0xFFFD8000;
    gUnk_03002490->unk5C = 128 << 7;
    gUnk_03002490->unk3C = 20;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk54 = 0xFFFF0000;
    gUnk_03002490->unk5C = 128 << 5;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk54 = 0xFFFF8000;
    gUnk_03002490->unk5C = 0;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 160 << 10;
    gUnk_03002490->unk5C = 0xFFFFC000;
    gUnk_03002490->unk3C = 18;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk54 = 128 << 9;
    gUnk_03002490->unk5C = 0;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 128 << 8;
    gUnk_03002490->unk5C = 0;
    gUnk_03002490->unk3C = 12;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(128 << 8, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    sub_0800625c(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFFE000, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    sub_0800625c(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFFC000, 0, 0x5A5A5A5A, 0xFFFAA000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 37;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_0800625c(128 << 7, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(6);
    sub_0800625c(0, 0, 0x5A5A5A5A, 224 << 8, 0xFFFFE000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 123;
    TaskYieldTrampoline(13);
    gUnk_03002490->unk3E |= 128 << 8;
    sub_08006244();
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_080062c4();
    gUnk_03002490->unk3E |= 128 << 8;
    sub_0805afac(gUnk_03002490->unk88->unk00, 16, 0);
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(21);
}

void sub_0805fb88(void)
{
    sub_0800625c(204 << 9, 0xFFFFE800, 0x5A5A5A5A, 0xFFFE7800, 224 << 6, 0x5A5A5A5A);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 13;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 192 << 6;
    gUnk_03002490->unk5C = 0;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    sub_0800625c(204 << 9, 0xFFFFE800, 0x5A5A5A5A, 0xFFFE7800, 224 << 6, 0x5A5A5A5A);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 192 << 6;
    gUnk_03002490->unk5C = 0;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C = 12;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 168 << 9;
    gUnk_03002490->unk5C = 0xFFFFE800;
    gUnk_03002490->unk3C = 102;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
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
    gUnk_03002490->unk54 = 192 << 6;
    gUnk_03002490->unk5C = 0;
    TaskYieldTrampoline(2);
    sub_0800625c(192 << 6, 0, 0x5A5A5A5A, 0xFFFD6000, 192 << 7, 0x5A5A5A5A);
    sub_08006364(17);
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 102;
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
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_0800625c(192 << 6, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(11);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFF2800, 0, 0x5A5A5A5A, 0xFFFB8000, 192 << 6, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 31;
    TaskYieldTrampoline(14);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFFA000;
    gUnk_03002490->unk5C = 0xFFFFF400;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFFF2800;
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 0xFFFFA000;
    gUnk_03002490->unk3C = 32;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFF7000;
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(7);
    TaskYieldTrampoline(5);
    sub_0800625c(0xFFFFD000, 0, 0x5A5A5A5A, 224 << 7, 0xFFFFF000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFF4000;
    gUnk_03002490->unk5C = 192 << 5;
    gUnk_03002490->unk3C = 123;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk5C = 0;
    TaskYieldTrampoline(5);
    sub_080062c4();
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(19);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(2);
    sub_080062c4();
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFE2000;
    gUnk_03002490->unk5C = 240 << 4;
    gUnk_03002490->unk3C = 97;
    TaskYieldTrampoline(19);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk5C = 0;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFE8000;
    gUnk_03002490->unk5C = 144 << 4;
    gUnk_03002490->unk3C = 97;
    TaskYieldTrampoline(11);
    gUnk_03002490->unk54 = 0xFFFFA000;
    gUnk_03002490->unk3C = 123;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0xFFFE8600;
    gUnk_03002490->unk5C = 216 << 6;
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 160 << 11;
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 154 << 1;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 309;
    TaskYieldTrampoline(4);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0xFFFDC000, 192 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 147 << 1;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 309;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 144 << 9;
    gUnk_03002490->unk5C = 192 << 4;
    gUnk_03002490->unk3C = 149 << 1;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 309;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 216 << 8;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 144 << 8;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(4);
    sub_0800625c(144 << 7, 0, 0x5A5A5A5A, 0xFFFB0000, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 144 << 7;
    gUnk_03002490->unk3C = 102;
    TaskYieldTrampoline(7);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk5C = 192 << 3;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 16;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(2);
    sub_0800625c(144 << 8, 0xFFFFEE00, 0x5A5A5A5A, 242 << 7, 0xFFFFF500, 0x5A5A5A5A);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 123;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk5C = 0;
    TaskYieldTrampoline(11);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_080062c4();
    gUnk_03002490->unk3E |= 128 << 8;
    sub_0805afac(gUnk_03002490->unk88->unk00, 16, 0);
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(21);
}

void sub_08060308(void)
{
    sub_0800625c(0xFFFF8C00, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 15;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0xFFFF4000;
    gUnk_03002490->unk60 = 128 << 6;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(7);
    gUnk_03002490->unk54 = 0xFFFE8000;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    sub_0800625c(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(7);
    gUnk_03002490->unk3C = 19;
    TaskYieldTrampoline(1);
    sub_0800625c(0xFFFF8C00, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 20;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0xFFFF4000;
    gUnk_03002490->unk60 = 128 << 6;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 12;
    TaskYieldTrampoline(7);
    gUnk_03002490->unk54 = 0xFFFE8000;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    sub_0800625c(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(7);
    gUnk_03002490->unk3C = 14;
    TaskYieldTrampoline(1);
    sub_080062c4();
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 128 << 8;
    gUnk_03002490->unk3C = 102;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 108;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 128 << 9;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 192 << 9;
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 128 << 8;
    gUnk_03002490->unk3C = 148;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0xFFFCA000, 128 << 6, 0x5A5A5A5A);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 172;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 151;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 172;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 151;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 172;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 151;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C = 0x00000133;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 147 << 1;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(11);
    gUnk_03002490->unk58 = 160 << 12;
    gUnk_03002490->unk60 = 0xFFFF0000;
    gUnk_03002490->unk3C = 155 << 1;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 192 << 10;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = 0xFFFE0000;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C = 0x0000012B;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0xFFFD0000;
    gUnk_03002490->unk3C = 156 << 1;
    TaskYieldTrampoline(1);
    sub_080062c4();
    TaskYieldTrampoline(18);
    gUnk_03002490->unk3C = 0x0000012B;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 160 << 11;
    gUnk_03002490->unk3C = 0x00000137;
    TaskYieldTrampoline(1);
    sub_080062c4();
    TaskYieldTrampoline(1);
    sub_0800625c(192 << 8, 0, 0x5A5A5A5A, 0xFFF7C000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 149 << 1;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000135;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3E |= 128 << 8;
    sub_0800625c(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(212 << 8, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    sub_0800625c(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFF4000, 0, 0x5A5A5A5A, 0xFFFD5000, 128 << 6, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 31;
    TaskYieldTrampoline(21);
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 41;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFFF8000;
    gUnk_03002490->unk3C = 102;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFFE8000;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFFF8000;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk54 = 128 << 8;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(2);
    sub_0800625c(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 128 << 8;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk54 = 0xFFFF8000;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk54 = 0xFFFF0000;
    gUnk_03002490->unk3C = 109;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFFF8000;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 128 << 8;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 128 << 9;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 128 << 8;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk54 = 0xFFFF8000;
    gUnk_03002490->unk3C = 111;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk54 = 0xFFFF0000;
    gUnk_03002490->unk3C = 109;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 128 << 8;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 128 << 9;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 128 << 7;
    gUnk_03002490->unk3C = 14;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 128 << 9;
    gUnk_03002490->unk3C = 150;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 128 << 8;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk58 = 0xFFFE1600;
    gUnk_03002490->unk3C = 151;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 176 << 5;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 172;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 151;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 172;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 151;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 172;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 248 << 6;
    gUnk_03002490->unk3C = 151;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 172;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 128 << 8;
    gUnk_03002490->unk3C = 148;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0xFFFD0000;
    gUnk_03002490->unk3E |= 128 << 8;
    sub_0805afac(gUnk_03002490->unk88->unk00, 16, 0);
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(1);
    sub_080062c4();
    TaskYieldTrampoline(20);
}

void sub_08060c2c(void)
{
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk54 = 0xFFFFC000;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFF8000, 128 << 3, 0x5A5A5A5A, 0xFFFE0800, 224 << 6, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    sub_0800625c(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFFC000;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFF4000, 192 << 3, 0x5A5A5A5A, 0xFFFE0800, 224 << 6, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 109;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 107;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 16;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 15;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 128 << 7;
    gUnk_03002490->unk5C = 160 << 3;
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(14);
    gUnk_03002490->unk58 = 0xFFFE0000;
    gUnk_03002490->unk60 = 128 << 9;
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(3);
    sub_0800625c(128 << 9, 0, 0x5A5A5A5A, 128 << 10, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 38;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C = 33;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0xFFFC0000;
    gUnk_03002490->unk3C = 43;
    TaskYieldTrampoline(1);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 43;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 128 << 7;
    gUnk_03002490->unk58 = 128 << 11;
    gUnk_03002490->unk3C = 42;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFE0000;
    gUnk_03002490->unk60 = 128 << 9;
    gUnk_03002490->unk3C = 42;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 0xFFFE0000;
    gUnk_03002490->unk60 = 128 << 9;
    gUnk_03002490->unk3C = 42;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 0xFFFE0000;
    gUnk_03002490->unk60 = 128 << 9;
    gUnk_03002490->unk3C = 42;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 128 << 7;
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C = 42;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 0xFFFC0000;
    gUnk_03002490->unk3C = 43;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk3C = 43;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 128 << 9;
    gUnk_03002490->unk58 = 0xFFFD8000;
    gUnk_03002490->unk60 = 128 << 7;
    gUnk_03002490->unk3C = 44;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 46;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 47;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 36;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 49;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 37;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 32;
    TaskYieldTrampoline(3);
    sub_0800625c(128 << 7, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 43;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFF6000;
    gUnk_03002490->unk58 = 0xFFFD4000;
    gUnk_03002490->unk60 = 128 << 7;
    gUnk_03002490->unk3C = 42;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C = 33;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 32;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 41;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 41;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(1);
    sub_0800625c(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(14);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk54 = 0xFFFA0000;
    gUnk_03002490->unk3C = 100;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 100;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk54 = 192 << 11;
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(20);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk58 = 160 << 9;
    gUnk_03002490->unk60 = 0xFFFFC000;
    gUnk_03002490->unk3C = 123;
    TaskYieldTrampoline(9);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0xFFFE0000;
    gUnk_03002490->unk60 = 128 << 8;
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(7);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3E |= 128 << 8;
    sub_0800625c(0xFFFFE000, 0, 0x5A5A5A5A, 160 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 123;
    TaskYieldTrampoline(9);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0xFFFE0000;
    gUnk_03002490->unk60 = 128 << 8;
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(7);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3E &= 0x7FFF;
    sub_0800625c(128 << 6, 0, 0x5A5A5A5A, 160 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 123;
    TaskYieldTrampoline(9);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk5C = 192 << 4;
    gUnk_03002490->unk58 = 0xFFFDF800;
    gUnk_03002490->unk60 = 160 << 6;
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 31;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 32;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 33;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 34;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 35;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 36;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 37;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 41;
    TaskYieldTrampoline(3);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0xFFFF2000, 128 << 7, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3E |= 128 << 8;
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C = 15;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 14;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFFC000;
    gUnk_03002490->unk5C = 0xFFFFFB00;
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(14);
    gUnk_03002490->unk5C = 0;
    gUnk_03002490->unk58 = 0xFFFE0000;
    gUnk_03002490->unk60 = 128 << 9;
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(3);
    sub_0800625c(0xFFFF4000, 0, 0x5A5A5A5A, 128 << 10, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 38;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C = 33;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 43;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFFFB800;
    gUnk_03002490->unk3C = 42;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFE0000;
    gUnk_03002490->unk60 = 128 << 9;
    gUnk_03002490->unk3C = 42;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0xFFFE0000;
    gUnk_03002490->unk60 = 136 << 7;
    gUnk_03002490->unk3C = 46;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 36;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 37;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 41;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 40;
    TaskYieldTrampoline(2);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 132 << 8;
    gUnk_03002490->unk60 = 0xFFFFF400;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(19);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_080062c4();
    sub_0805afac(gUnk_03002490->unk88->unk00, 16, 0);
    gUnk_03002490->unk3C = 146;
    TaskYieldTrampoline(21);
}

void sub_080613e4(void)
{
    gUnk_03002490->unk3E |= 0x8000;
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E &= 0x7FFF;
    sub_0800625c(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFFDC000, 0x4000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x134;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000135;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x126;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000127;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x128;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000129;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x12A;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x0000012B;
    TaskYieldTrampoline(3);
    sub_0800625c(0xFFFFC000, 0, 0x5A5A5A5A, 0x60000, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x00000137;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0x20000;
    gUnk_03002490->unk60 = 0xFFFF0000;
    gUnk_03002490->unk3C = 0x00000137;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 0x00000137;
    TaskYieldTrampoline(2);
    sub_0800625c(0x10000, 0, 0x5A5A5A5A, 0xFFF7C000, 0x4000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x0000012B;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFE0000;
    gUnk_03002490->unk3C = 0x0000012B;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x12A;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000129;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x128;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000127;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x126;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000135;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x134;
    TaskYieldTrampoline(3);
    sub_0800625c(0x4000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x5800;
    gUnk_03002490->unk5C = 0xFFFFF800;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C = 0x10;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 0x66;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 0x67;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 0x6000;
    gUnk_03002490->unk60 = 0xFFFFE000;
    gUnk_03002490->unk3C = 0x68;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 0x69;
    TaskYieldTrampoline(2);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0xFFFE5000, 0x3000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x5A;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x58;
    TaskYieldTrampoline(15);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(2);
    sub_0800625c(0x10000, 0, 0x5A5A5A5A, 0xFFFDC000, 0x4000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x132;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000131;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x130;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x12000;
    gUnk_03002490->unk3C = 0x0000012F;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x12E;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x0000012D;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x12C;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x0000012B;
    TaskYieldTrampoline(3);
    sub_0800625c(0x4000, 0, 0x5A5A5A5A, 0x60000, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x136;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0x20000;
    gUnk_03002490->unk60 = 0xFFFF0000;
    gUnk_03002490->unk3C = 0x136;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 0x136;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFF7C000, 0x4000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x0000012B;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFE0000;
    gUnk_03002490->unk3C = 0x0000012B;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x12C;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x0000012D;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x12E;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x0000012F;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x130;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000131;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x132;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3E |= 0x8000;
    sub_0800625c(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFFA800;
    gUnk_03002490->unk5C = 0x800;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 0x6C;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 0x6D;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 0x6E;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3E |= 0x8000;
    gUnk_03002490->unk58 = 0x6000;
    gUnk_03002490->unk60 = 0xFFFFE800;
    gUnk_03002490->unk3C = 0x6F;
    TaskYieldTrampoline(5);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0xFFFE4800, 0x2800, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x92;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x92;
    TaskYieldTrampoline(0x13);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0xFFFDC000;
    gUnk_03002490->unk60 = 0x4000;
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(1);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0xFFFDC000;
    gUnk_03002490->unk60 = 0x4000;
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(1);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E |= 0x8000;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0xFFFB8000;
    gUnk_03002490->unk60 = 0x4000;
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk58 = 0xFFFF7000;
    gUnk_03002490->unk60 = 0x1800;
    gUnk_03002490->unk3C = 0x00000133;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x00000135;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000127;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000129;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x0000012B;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x0000012D;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = 0x3000;
    gUnk_03002490->unk3C = 0x0000012F;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000131;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000133;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x134;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000135;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x126;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000127;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x128;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x00000129;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x60000;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C = 0x136;
    TaskYieldTrampoline(1);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x136;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0xFFFD3000;
    gUnk_03002490->unk60 = 0x3000;
    gUnk_03002490->unk3C = 0x0000012B;
    TaskYieldTrampoline(9);
    gUnk_03002490->unk3C = 0x00000139;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 0x13A;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 0x0000013B;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 0x13C;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 0x0000013D;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFC0000;
    gUnk_03002490->unk60 = 0xFFFE8000;
    gUnk_03002490->unk3C = 0x6A;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0x18000;
    gUnk_03002490->unk3C = 0x6A;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C = 0x69;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x68;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x67;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x66;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E |= 0x8000;
    gUnk_03002490->unk3C = 0x6E;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 0x6F;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E |= 0x8000;
    gUnk_03002490->unk3C = 0x11;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(15);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_080062c4();
    gUnk_03002490->unk3E |= 0x8000;
    sub_0805afac(gUnk_03002490->unk88->unk00, 16, 0);
    gUnk_03002490->unk3C = 0x92;
    TaskYieldTrampoline(0x15);
}

void sub_08061cac(void)
{
    gUnk_03002490->unk3E &= 0x7FFF;
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFB0000, 0, 0x5A5A5A5A, 0xFFFD0000, 0x10000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x64;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 0x64;
    TaskYieldTrampoline(4);
    sub_0800625c(0x80000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0x20000;
    gUnk_03002490->unk5C = 0xFFFFC000;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(7);
    sub_0800625c(0x4000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 0x13;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFB0000, 0, 0x5A5A5A5A, 0xFFFD0000, 0x10000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x64;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 0x64;
    TaskYieldTrampoline(4);
    sub_0800625c(0x80000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0x20000;
    gUnk_03002490->unk5C = 0xFFFFC000;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(7);
    sub_0800625c(0x4000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C = 0x13;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFF0000;
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFA0000, 0, 0x5A5A5A5A, 0xFFFB8000, 0x8000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x64;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFFF0000;
    gUnk_03002490->unk3C = 0x64;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk54 = 0x40000;
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFFF0000;
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    sub_0800625c(0x8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFF0000, 0x1000, 0x5A5A5A5A, 0xFFFE8000, 0x4000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x1E;
    TaskYieldTrampoline(11);
    sub_0800625c(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x1E;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x4000;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFFDC000, 0x4000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x14000;
    gUnk_03002490->unk3C = 0x00000133;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFF8000;
    gUnk_03002490->unk3C = 0x00000133;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3E |= 0x8000;
    gUnk_03002490->unk58 = 0xFFFEC000;
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0x8000;
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0xFFFFC000;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(1);
    sub_0800625c(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x7B;
    TaskYieldTrampoline(2);
    sub_0800625c(0x10000, 0, 0x5A5A5A5A, 0xFFFDC000, 0x4000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk58 = 0x14000;
    gUnk_03002490->unk3C = 0x00000133;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFF8000;
    gUnk_03002490->unk3C = 0x00000133;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk58 = 0xFFFEC000;
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0x8000;
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0x4000;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(1);
    sub_0800625c(0x4000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x7B;
    TaskYieldTrampoline(2);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0xFFFA8000, 0x8000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x66;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x68;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x6B;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x6D;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E |= 0x8000;
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk58 = 0x38000;
    gUnk_03002490->unk3C = 0x00000133;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0x20000;
    gUnk_03002490->unk3C = 0x00000133;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x10000;
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0x38000;
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x20000;
    gUnk_03002490->unk60 = 0xFFFF0000;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 0x20000;
    gUnk_03002490->unk60 = 0;
    gUnk_03002490->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFE0000;
    gUnk_03002490->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0x20000;
    gUnk_03002490->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFE0000;
    gUnk_03002490->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0x10000;
    gUnk_03002490->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFF0000;
    gUnk_03002490->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0x10000;
    gUnk_03002490->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0xFFFF0000;
    gUnk_03002490->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk3C = 0x7B;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3E |= 0x8000;
    gUnk_03002490->unk54 = 0x20000;
    gUnk_03002490->unk5C = 0xFFFFF000;
    gUnk_03002490->unk3C = 0x12;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk54 = 0x10000;
    gUnk_03002490->unk3C = 0x11;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk54 = 0x10000;
    gUnk_03002490->unk3C = 0x10;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk54 = 0x8000;
    gUnk_03002490->unk3C = 15;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 14;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x20000;
    gUnk_03002490->unk5C = 0xFFFFF000;
    gUnk_03002490->unk3C = 13;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk54 = 0x10000;
    gUnk_03002490->unk3C = 12;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk54 = 0x10000;
    gUnk_03002490->unk3C = 0x15;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk54 = 0x8000;
    gUnk_03002490->unk3C = 0x14;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 0x13;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk54 = 0x10000;
    gUnk_03002490->unk3C = 0x6D;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x6C;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xC000;
    gUnk_03002490->unk3C = 0x6B;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x6A;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x69;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x8000;
    gUnk_03002490->unk3C = 0x68;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x67;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x66;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x4000;
    gUnk_03002490->unk3C = 0x6F;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3E |= 0x8000;
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 0x11;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 0x10;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 0xFFFFC000;
    gUnk_03002490->unk5C = 0;
    gUnk_03002490->unk3C = 15;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFF8000;
    gUnk_03002490->unk3C = 14;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 13;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk54 = 0xFFFF0000;
    gUnk_03002490->unk3C = 0x1B;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x1C;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x1D;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x1C;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x1B;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFF8000;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_0800625c(0xFFFEC000, 0, 0x5A5A5A5A, 0xFFFD4000, 0x4000, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 0x16;
    TaskYieldTrampoline(14);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0x29;
    TaskYieldTrampoline(1);
    sub_0800625c(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0xFFFFC000;
    gUnk_03002490->unk3C = 0x1E;
    TaskYieldTrampoline(0xF);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x4000;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk54 = 0;
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(14);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(2);
    sub_080062c4();
    sub_0805afac(gUnk_03002490->unk88->unk00, 16, 0);
    gUnk_03002490->unk3C = 0x92;
    TaskYieldTrampoline(0x15);
}
