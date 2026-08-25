#include "gba/gba.h"
#include "global.h"

/* Link (SIO multi-play + multiboot) driver work area at 0x0200EBC0. */
struct SioWork
{
    /*0x00*/ vu8 unk00;
    /*0x01*/ vu8 unk01;
    /*0x02*/ vu8 unk02;
    /*0x03*/ vu8 unk03;
    /*0x04*/ vu8 unk04;
    /*0x05*/ vu8 unk05;
    /*0x06*/ vu16 unk06;
    /*0x08*/ vu16 unk08;
    /*0x0A*/ vu16 unk0A;
    /*0x0C*/ vu16 unk0C;
    /*0x0E*/ vu16 unk0E;
    /*0x10*/ u32 unk10;
    /*0x14*/ u32 unk14;
    /*0x18*/ u32 unk18;
    /*0x1C*/ vu16 unk1C;
    /*0x1E*/ vu16 unk1E[3];
    /*0x24*/ vu8 unk24;
    /*0x25*/ vu8 unk25;
    /*0x26*/ vu8 unk26;
    /*0x27*/ vu8 unk27;
    /*0x28*/ vu16 unk28;
    /*0x2A*/ vu8 unk2A;
    /*0x2B*/ vu8 unk2B;
    /*0x2C*/ vu8 unk2C;
    /*0x2D*/ vu8 unk2D;
    /*0x2E*/ vu16 unk2E;
};

/* Assigning an 8-byte struct is what makes agbcc emit the ROM's DImode block
 * move (ldr [4]; ldr [0]; str [0]; str [4]) instead of an ldmia/stmia pair.  */
struct SioRecv
{
    u32 a;
    u32 b;
};

extern struct SioWork gUnk_0200EBC0;
extern vu16 gUnk_0200EB98[4];
extern u8 gUnk_0200EB94[4];
extern u32 *gUnk_0200EB90;
extern u32 gUnk_0200EBA0;
extern u32 gUnk_0200EBA8;
extern u8 gUnk_0200EBB0;
extern u32 *gUnk_0200EBB4;
extern u32 gUnk_0200EBB8;
extern vs32 gUnk_0200EBAC;
extern vs32 gUnk_0200EBBC;
extern vu32 gUnk_0200EC3C;
extern u32 gUnk_0200EC40;
extern u32 gUnk_0200EC44;
extern vs32 gUnk_0200EC48;
extern u8 gUnk_0200EC4C;
extern vu16 gUnk_03000018;
extern u32 gUnk_030004B0[];
extern vu16 gUnk_03001EF8;
extern u16 gUnk_03001F38;
extern u16 gUnk_03004D50[];
extern u16 gUnk_03004D90[];

extern void sub_08001518(void);
extern void sub_08003ab8(void);
extern void sub_08003bd8(void);
extern void sub_08003dc4(void);
extern void sub_080045e8(void);
extern void sub_0800469c(void);

/*FN sub_08004000*/
void sub_08004000(u16 a)
{
    vu16 stat;

    stat = REG_SIOCNT;
    if (gUnk_0200EC48 == -1)
        return;
    if (gUnk_0200EBC0.unk02 == 3)
        return;
    gUnk_0200EBC0.unk0C = a << 13;
    switch (gUnk_0200EC48)
    {
    case 0:
        sub_08003ab8();
        break;
    case 1:
        sub_08003bd8();
        break;
    case 2:
        sub_08003dc4();
        break;
    }
}

/*FN sub_08004068*/
void sub_08004068(void)
{
    vu16 stat;
    s32 i;
    s32 flag;
    s32 prev;
    vu16 *dst;
    vu16 *src;
    vu16 *pcur;
    vu16 *pprev;

    stat = REG_SIOCNT;
    gUnk_0200EBC0.unk28 = stat;
    gUnk_0200EBC0.unk26++;
    *(struct SioRecv *)gUnk_0200EB98 = *(struct SioRecv *)REG_ADDR_SIOMULTI0;
    gUnk_0200EBC0.unk00 = (stat & 0x30) >> 4;
    gUnk_0200EBC0.unk03 = gUnk_0200EBC0.unk03 & 0xBF;
    gUnk_0200EBC0.unk03 = (stat & 0x40) | gUnk_0200EBC0.unk03;
    if (gUnk_0200EC48 == 2 || gUnk_0200EC48 == 0)
    {
        if (REG_SIOCNT & 4)
            REG_SIOMLT_SEND = 0xD951;
        if (gUnk_0200EB98[1] == 0xFFFF)
            gUnk_0200EBC0.unk24 = 0;
        src = gUnk_0200EB98 + 1;
        for (i = 0; i < 3; i++)
        {
            if (src[i] != gUnk_0200EBC0.unk1E[i])
                gUnk_0200EBC0.unk24 = 0;
            gUnk_0200EBC0.unk1E[i] = src[i];
        }
        gUnk_0200EBC0.unk24++;
        if (gUnk_0200EBC0.unk24 > 29)
            gUnk_0200EBC0.unk24 = 30;
        return;
    }
    if (gUnk_0200EB98[0] == 0xE4E4)
    {
        REG_SIOMLT_SEND = 0xE4E4;
        gUnk_0200EBC0.unk02 = 3;
        return;
    }
    if ((stat & 4) == 0)
    {
        if (gUnk_0200EBC0.unk0A > 19)
            gUnk_0200EBC0.unk02 = 2;
    }
    else
    {
        gUnk_0200EBC0.unk08++;
        gUnk_0200EBC0.unk08 = gUnk_0200EBC0.unk08 & 0x1FFF;
        if (gUnk_0200EBC0.unk08 <= 255)
            gUnk_0200EBC0.unk08 = 0x100;
        if (gUnk_0200EBC0.unk08 == (gUnk_0200EB98[0] & 0x1FFF))
        {
            if (gUnk_0200EBC0.unk0A > 3)
            {
                if (gUnk_0200EBC0.unk0C == (gUnk_0200EB98[0] & 0xE000))
                {
                    gUnk_0200EBC0.unk03 = gUnk_0200EBC0.unk03 & 0xFE;
                    gUnk_0200EBC0.unk0A++;
                }
                else if ((gUnk_0200EB98[0] & 0xE000) != 0)
                {
                    gUnk_0200EBC0.unk03 = gUnk_0200EBC0.unk03 | 1;
                    gUnk_0200EBC0.unk0A = 0;
                }
            }
            else
            {
                gUnk_0200EBC0.unk0A++;
            }
        }
        else
        {
            gUnk_0200EBC0.unk0A = 0;
        }
        gUnk_0200EBC0.unk08 = gUnk_0200EB98[0];
        if (gUnk_0200EBC0.unk0A > 30)
        {
            gUnk_0200EBC0.unk02 = 2;
            REG_SIOMLT_SEND = 0x26AE;
        }
        else
        {
            gUnk_0200EBC0.unk02 = 0;
            REG_SIOMLT_SEND = 0xD951;
        }
    }
    gUnk_0200EBC0.unk01 = 1;
    flag = 1;
    prev = gUnk_0200EBC0.unk0E;
    gUnk_0200EBC0.unk0E = 0;
    for (i = 1; i <= 3; i++)
    {
        if (gUnk_0200EB98[i] == 0xFFFF)
            gUnk_0200EBC0.unk0E = gUnk_0200EBC0.unk0E | (1 << i);
        if (((prev >> i) & 1) == 0)
        {
            if (gUnk_0200EB98[i] == 0x26AE)
            {
                if (gUnk_0200EB98[i - 1] == 0xFFFF)
                {
                    flag = 0;
                    gUnk_0200EBC0.unk02 = 1;
                }
                if (flag)
                    gUnk_0200EBC0.unk01++;
                else
                    gUnk_0200EBC0.unk01 = 1;
            }
            else if (gUnk_0200EB98[i] == 0xD951)
            {
                gUnk_0200EBC0.unk02 = 1;
                if (gUnk_0200EB98[i - 1] == 0xFFFF)
                {
                    flag = 0;
                    gUnk_0200EBC0.unk0A = 0;
                }
            }
            else
            {
                flag = 0;
            }
        }
        else
        {
            flag = 0;
        }
    }
}

/*FN sub_08004308*/
u32 sub_08004308(void)
{
    s32 i;
    u16 *p;
    u32 n;

    if (gUnk_03004D50[0] == 0xAA02)
        return 1;
    if (gUnk_03004D50[0] == 0xAA00)
    {
        gUnk_03004D90[0] = 0xAA01;
        gUnk_0200EC4C = gUnk_03004D50[4];
    }
    if (gUnk_03001F38 != 0)
    {
        p = gUnk_03004D50;
        for (i = 0; i < 4; i++)
        {
            if (*p == 0xAA01)
                gUnk_0200EBB0++;
            p++;
        }
        /* Loading unk01 into a temp first, and reading the counter through a
         * volatile cast, is what puts the counter in r0 and unk01 in r1 and
         * emits `cmp r0, r1` the way the ROM does.  */
        n = gUnk_0200EBC0.unk01;
        if (*(vu8 *)&gUnk_0200EBB0 == n)
        {
            gUnk_0200EBB0 = 0;
            gUnk_03004D90[0] = 0xAA02;
        }
    }
    return 0;
}

/*FN sub_08004390*/
void sub_08004390(u32 *src, u32 *dst, u32 size)
{
    if (gUnk_03001F38 == 0)
        return;
    gUnk_0200EBB8 = 0;
    gUnk_0200EB90 = src;
    gUnk_0200EBB4 = dst;
    gUnk_0200EBAC = (size + 15) >> 2;
    gUnk_0200EC44 = 0;
    gUnk_03004D90[0] = 0x5500;
    gUnk_03004D90[1] = (u32)dst;
    gUnk_03004D90[2] = (u32)dst >> 16;
    gUnk_03004D90[3] = (u32)gUnk_0200EBAC >> 2;
    gUnk_0200EB94[0] = gUnk_0200EB94[1] = gUnk_0200EB94[2] = gUnk_0200EB94[3] = 0;
}

/* Matching notes for sub_08004400:
 *  - The three handshake reads and the loop read must be VOLATILE (cast, so
 *    the shared u16 declaration stays plain): the ROM re-loads them and
 *    evaluates the low half of the address before the high half.  A plain read
 *    lets agbcc sink the low ldrh past the shift.
 *  - `(lo = ...) | (hi = ... << 16)` inside the assignment keeps the ldr of
 *    the store address ahead of both ldrh (expand_assignment does the LHS
 *    address first), and the two hard-register pins give the ROM's
 *    `orrs r1, r0`: with a volatile u16 source the OR operand is a paradoxical
 *    (subreg:SI (reg:HI)), which gcc's regmove refuses to tie to operand 0, so
 *    it would otherwise tie the result to the shift (`orrs r0, r1`).
 */
/*FN sub_08004400*/
u32 sub_08004400(void)
{
    s32 i;
    register u32 lo asm("r1");
    register u32 hi asm("r0");
    s32 v;

    v = gUnk_03004D50[0];
    if (v == 0x5502)
        return 1;
    if (v == 0x5500)
    {
        gUnk_03004D90[0] = 0x5501;
        gUnk_0200EBB4 = (u32 *)((lo = *(vu16 *)&gUnk_03004D50[4]) | (hi = *(vu16 *)&gUnk_03004D50[8] << 16));
        gUnk_0200EBAC = *(vu16 *)&gUnk_03004D50[12] << 2;
    }
    if (gUnk_03001F38 != 0)
    {
        for (i = 0; i < 4; i++)
        {
            if (*(vu16 *)&gUnk_03004D50[i] == 0x5501)
            {
                gUnk_0200EB94[i] = 1;
                gUnk_0200EBB8++;
            }
        }
        if (gUnk_0200EBB8 == gUnk_0200EBC0.unk01)
        {
            gUnk_0200EBB8 = 0;
            gUnk_0200EB94[0] = gUnk_0200EB94[1] = gUnk_0200EB94[2] = gUnk_0200EB94[3] = 0;
            gUnk_03004D90[0] = 0x5502;
        }
    }
    return 0;
}

/*FN sub_080044b8*/
void sub_080044b8(void)
{
    gUnk_0200EBA8 = 0;
    gUnk_0200EC40 = 0;
    gUnk_0200EBBC = -1;
    if (gUnk_03001F38 != 0)
    {
        gUnk_03001EF8 = REG_IME = REG_IME & 0xFFFE;
        REG_IE = gUnk_03000018 = gUnk_03000018 & 0xFF3F;
        gUnk_03001EF8 = REG_IME = REG_IME | 1;
        REG_SIOCNT = 0x2000;
        REG_TM3CNT = 0;
        gUnk_030004B0[0] = (u32)sub_08001518;
        gUnk_030004B0[1] = (u32)sub_080045e8;
        REG_SIOCNT = 0x1000;
        REG_SIOCNT = 0x1001;
    }
    else
    {
        gUnk_03001EF8 = REG_IME = REG_IME & 0xFFFE;
        REG_IE = gUnk_03000018 = gUnk_03000018 & 0xFF3F;
        gUnk_03001EF8 = REG_IME = REG_IME | 1;
        gUnk_030004B0[0] = (u32)sub_0800469c;
        gUnk_030004B0[1] = (u32)sub_08001518;
        REG_SIOCNT = 0x2000;
    }
    gUnk_0200EC44 = 0;
    gUnk_0200EBA0 = 2;
}

/*FN sub_080045e8*/
void sub_080045e8(void)
{
    u32 *p;

    REG_TM3CNT_H = 0;
    if (gUnk_0200EBBC < 0)
    {
        REG_SIODATA32 = 0xFDB99BDF;
    }
    else if (gUnk_0200EBBC < gUnk_0200EBAC)
    {
        REG_SIODATA32 = *gUnk_0200EB90;
        gUnk_0200EC44 += *gUnk_0200EB90++;
    }
    else if (gUnk_0200EBBC == gUnk_0200EBAC)
    {
        REG_SIODATA32 = gUnk_0200EC3C = gUnk_0200EC44;
    }
    else
    {
        REG_SIODATA32 = 0x9BDFFDB9;
    }
    gUnk_0200EBBC++;
    REG_SIOCNT = REG_SIOCNT | 0x80;
    REG_TM3CNT_H = 0xC0;
}

/*FN sub_0800469c*/
void sub_0800469c(void)
{
    u32 v;
    u32 *p;

    v = REG_SIODATA32;
    REG_SIOCNT = REG_SIOCNT | 0x80;
    if (gUnk_0200EBBC < 0)
    {
        if (v != 0xFDB99BDF)
            return;
    }
    else if (gUnk_0200EBBC < gUnk_0200EBAC)
    {
        p = gUnk_0200EBB4;
        *p++ = v;
        gUnk_0200EBB4 = p;
        gUnk_0200EC44 += v;
    }
    else if (gUnk_0200EBBC == gUnk_0200EBAC)
    {
        gUnk_0200EC3C = v;
    }
    gUnk_0200EBBC++;
}

/*FN sub_08004714*/
u32 sub_08004714(void)
{
    if (gUnk_0200EBA8 == 0x9999)
    {
        gUnk_0200EBA8 = 0;
        return 1;
    }
    return 0;
}
