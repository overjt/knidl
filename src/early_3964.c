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

/* AGB SDK MultiBootParam (0x4C bytes) at 0x0200EBF0. */
struct MultiBootParam
{
    /*0x00*/ u32 system_work[5];
    /*0x14*/ u8 handshake_data;
    /*0x15*/ u8 padding;
    /*0x16*/ u16 handshake_timeout;
    /*0x18*/ vu8 probe_count;
    /*0x19*/ u8 client_data[3];
    /*0x1C*/ u8 palette_data;
    /*0x1D*/ u8 response_bit;
    /*0x1E*/ vu8 client_bit;
    /*0x1F*/ u8 reserved1;
    /*0x20*/ u8 *boot_srcp;
    /*0x24*/ u8 *boot_endp;
    /*0x28*/ u8 *masterp;
    /*0x2C*/ u8 *reserved2[3];
    /*0x38*/ u32 system_work2[4];
    /*0x48*/ u8 sendflag;
    /*0x49*/ u8 probe_target_bit;
    /*0x4A*/ u8 check_wait;
    /*0x4B*/ u8 server_type;
};

struct Unk030023A8
{
    /*0x00*/ s8 unk00[3];
    /*0x03*/ u8 unk03;
};

extern struct SioWork gUnk_0200EBC0;
extern struct MultiBootParam gUnk_0200EBF0;
extern u32 gUnk_0200EBA8;
extern u32 gUnk_0200EC48;
extern vu16 gUnk_0200EB98[];
extern struct Unk030023A8 gUnk_030023A8;
extern vu16 gUnk_0300244C;
extern u32 gUnk_030004B0[];
extern vu16 gUnk_03000018;
extern vu16 gUnk_03001EF8;

extern void sub_08003484(void);
extern void sub_08003840(void);
extern void sub_08004068(void);
extern void sub_08004968(struct MultiBootParam *);
extern u32 sub_08004984(struct MultiBootParam *);
extern void sub_08004dd8(struct MultiBootParam *, u32, u32, u32, u32);
extern u32 sub_08004e9c(struct MultiBootParam *);

/*FN sub_08003888*/

/*FN sub_08003964*/
void sub_08003964(void)
{
    gUnk_03001EF8 = REG_IME = REG_IME & 0xFFFE;
    gUnk_0200EC48 = -1;
    REG_SIOCNT = REG_SIOCNT & 0xBFFF;
    REG_SIOMLT_SEND = 0xD952;
    gUnk_030004B0[1] = gUnk_030004B0[0] = (u32)sub_08004068;
    gUnk_0200EBC0.unk01 = gUnk_0200EBC0.unk02 = 0;
    REG_IE = gUnk_03000018 = gUnk_03000018 & 0xFF3F;
    gUnk_03001EF8 = REG_IME = REG_IME | 1;
}

/*FN sub_08003a00*/
void sub_08003a00(u8 *start, u8 *end)
{
    u32 len;

    len = ((u32)end - (u32)start + 16) & ~15;
    gUnk_0200EBC0.unk10 = (u32)start + 0xC0;
    gUnk_0200EBC0.unk14 = (u32)end;
    gUnk_0200EBC0.unk18 = len - 0xC0;
    gUnk_0200EBF0.masterp = start;
    gUnk_0200EBF0.server_type = 0;
}

/*FN sub_08003a34*/
void sub_08003a34(u8 *start, u8 *end)
{
    vu16 zero;
    u32 len;

    len = ((u32)end - (u32)start + 16) & ~15;
    zero = 0;
    CpuSet((void *)&zero, &gUnk_0200EBF0, 0x01000026);
    gUnk_0200EBC0.unk10 = (u32)start + 0xC0;
    gUnk_0200EBC0.unk14 = (u32)end;
    gUnk_0200EBC0.unk18 = len - 0xC0;
    gUnk_0200EBF0.masterp = start;
    gUnk_0200EBF0.server_type = 0;
    sub_08004968(&gUnk_0200EBF0);
}

/*FN sub_08003a98*/
void sub_08003a98(void)
{
    if (gUnk_0200EBC0.unk2B != 0)
        return;
    if (gUnk_0200EBC0.unk02 != 2)
        return;
    gUnk_0200EBC0.unk2B = 1;
}

/*FN sub_08003ab8*/
void sub_08003ab8(void)
{
    vu16 cnt;
    u16 t;
    s32 i;
    s32 mask;

    cnt = REG_SIOCNT;
    if ((REG_SIOCNT & 4) == 0)
        sub_08004984(&gUnk_0200EBF0);
    gUnk_0200EBC0.unk25 = 0;
    if ((cnt & 8) == 0)
    {
        t = cnt & 0x80;
        if (t != 0)
            return;
        gUnk_0200EBC0.unk24 = gUnk_0200EBC0.unk25 = gUnk_0200EBC0.unk03 = t;
        return;
    }
    if (gUnk_0200EBC0.unk24 <= 29)
        return;
    mask = 0;
    for (i = 0; i <= 2; i++)
    {
        u16 v;

        v = gUnk_0200EBC0.unk1E[i] & 0xFFF0;
        if (v == 0x7200)
            mask |= 1;
        if (v == 0xD950)
            mask |= 2;
    }
    gUnk_0200EBC0.unk25 = mask;
    mask &= 3;
    if (mask == 3)
    {
        gUnk_0200EBC0.unk03 |= 2;
        return;
    }
    gUnk_0200EBC0.unk2A = 0;
    gUnk_0200EBC0.unk02 = 0;
    gUnk_0200EBC0.unk0A = 0;
    if (gUnk_0200EBC0.unk25 == 1)
    {
        gUnk_0200EC48 = 2;
        return;
    }
    if (gUnk_0200EBC0.unk25 == 2)
        gUnk_0200EC48 = 1;
}

/*FN sub_08003bd8*/
void sub_08003bd8(void)
{
    vu16 cnt;
    u16 t;
    u16 t2;

    cnt = REG_SIOCNT;
    if ((gUnk_0200EBC0.unk28 & 0x40) != 0 || (gUnk_0200EBC0.unk28 & 8) == 0)
    {
        gUnk_0200EBC0.unk01 = 0;
        gUnk_0200EBC0.unk2A = gUnk_0200EBC0.unk02 = gUnk_0200EBC0.unk24 = gUnk_0200EBC0.unk25 = 0;
        gUnk_0200EC48 = 0;
    }
    else
    {
        t = gUnk_0200EBC0.unk28 & 4;
        if (t == 0 && (cnt & 0xFC) != 8)
        {
            gUnk_0200EBC0.unk01 = t;
            gUnk_0200EBC0.unk2A = gUnk_0200EBC0.unk02 = gUnk_0200EBC0.unk24 = gUnk_0200EBC0.unk25 = t;
            gUnk_0200EC48 = t;
        }
        else
        {
            if (gUnk_0200EB98[0] == 0xE4E4 && (gUnk_0200EBC0.unk28 & 4) != 0)
                return;
            t = gUnk_0200EBC0.unk2B;
            if (t != 0)
            {
                REG_SIOMLT_SEND = 0xE4E4;
                REG_SIOCNT |= 0x80;
                return;
            }
            if (gUnk_0200EBC0.unk2A != 0)
            {
                if (gUnk_0200EBC0.unk02 != 2 || gUnk_0200EBC0.unk01 == 1)
                {
                    gUnk_0200EBC0.unk01 = t;
                    gUnk_0200EBC0.unk2A = gUnk_0200EBC0.unk02 = gUnk_0200EBC0.unk24 = gUnk_0200EBC0.unk25 = t;
                    gUnk_0200EC48 = t;
                    return;
                }
            }
            else if (gUnk_0200EBC0.unk02 == 2)
            {
                gUnk_0200EBC0.unk2A = 1;
            }
            if (gUnk_0200EB98[0] == 0xE4E4)
                return;
            t2 = gUnk_0200EBC0.unk28 & 4;
            if (t2 == 0)
            {
                if ((cnt & 0xFC) != 8)
                    gUnk_0200EBC0.unk02 = 0;
                gUnk_0200EBC0.unk06 = gUnk_0200EBC0.unk06 + 1;
                gUnk_0200EBC0.unk06 = gUnk_0200EBC0.unk06 & 0x1FFF;
                if (gUnk_0200EBC0.unk06 <= 255)
                    gUnk_0200EBC0.unk06 = 0x100;
                REG_SIOMLT_SEND = gUnk_0200EBC0.unk06 | gUnk_0200EBC0.unk0C;
                if ((gUnk_0200EBC0.unk28 & 0x4000) == 0)
                {
                    gUnk_0200EBC0.unk02 = 0;
                    return;
                }
                gUnk_0200EBC0.unk0A = gUnk_0200EBC0.unk0A + 1;
                REG_SIOCNT |= 0x80;
                return;
            }
            if (gUnk_0200EBC0.unk06 == gUnk_0200EBC0.unk08)
            {
                gUnk_0200EBC0.unk02 = 0;
                gUnk_0200EBC0.unk03 &= 0xFE;
            }
            gUnk_0200EBC0.unk06 = gUnk_0200EBC0.unk08;
        }
    }
}

/*FN sub_08003dc4*/
void sub_08003dc4(void)
{
    vu16 cnt;
    u16 t;
    u32 r;
    u8 cb;

    cnt = REG_SIOCNT;
    t = gUnk_0200EBC0.unk28 & 4;
    if (t != 0)
    {
        gUnk_0200EC48 = 0;
        return;
    }
    if (gUnk_0200EBC0.unk2A == 1)
    {
        if (gUnk_0200EBC0.unk01 == 1 || (gUnk_0200EBF0.client_bit & 14) == 0)
        {
            gUnk_0200EBC0.unk01 = t;
            gUnk_0200EBC0.unk2A = gUnk_0200EBC0.unk02 = gUnk_0200EBC0.unk24 = gUnk_0200EBC0.unk25 = t;
            gUnk_0200EC48 = t;
            return;
        }
    }
    else
    {
        t = gUnk_0200EBF0.client_bit & 14;
        if (t == 0)
            goto noconn;
    }
    if (gUnk_0200EBC0.unk2A == 0)
        gUnk_0200EBC0.unk2A = 1;
    gUnk_0200EBC0.unk02 = 1;
    gUnk_0200EBC0.unk01 = 1;
    cb = gUnk_0200EBF0.client_bit;
    gUnk_0200EBC0.unk01 = gUnk_0200EBC0.unk01 + ((cb >> 1) & 1);
    gUnk_0200EBC0.unk01 = gUnk_0200EBC0.unk01 + ((cb >> 2) & 1);
    gUnk_0200EBC0.unk01 = gUnk_0200EBC0.unk01 + ((cb >> 3) & 1);
    switch (gUnk_0200EBF0.probe_count)
    {
    case 0:
        if (gUnk_0200EBC0.unk0A <= 15)
            gUnk_0200EBC0.unk0A = gUnk_0200EBC0.unk0A + 1;
        if (gUnk_0200EBC0.unk0A == 16)
            gUnk_0200EBC0.unk02 = 2;
        gUnk_0200EBC0.unk04 = 2;
        break;
    case 0xD1:
        gUnk_0200EBC0.unk2B = 2;
        gUnk_0200EBC0.unk04 = 3;
        gUnk_0200EBF0.server_type = 1;
        break;
    }
    if (gUnk_0200EBF0.probe_count > 0xDF)
        gUnk_0200EBC0.unk04 = 4;
    goto next;
noconn:
    gUnk_0200EBC0.unk02 = t;
    gUnk_0200EBC0.unk0A = t;
next:
    if (gUnk_0200EBC0.unk0A == 17)
    {
        sub_08004dd8(&gUnk_0200EBF0, gUnk_0200EBC0.unk10, gUnk_0200EBC0.unk18, 4, 1);
        gUnk_0200EBC0.unk0A = 18;
    }
    if (gUnk_0200EBC0.unk2B == 1 && gUnk_0200EBC0.unk0A == 16 && gUnk_0200EBF0.probe_count == 0
     && (gUnk_0200EBF0.client_bit & 14) != 0)
    {
        sub_08003484();
        sub_08003840();
        gUnk_0200EBC0.unk0A = 17;
        gUnk_0200EBC0.unk2A = 2;
    }
    r = sub_08004984(&gUnk_0200EBF0);
    if (gUnk_0200EBC0.unk2B == 2)
    {
        gUnk_0200EBF0.server_type = 0;
        gUnk_0200EBC0.unk2B = 0;
        if (r != 0)
        {
            REG_SIOCNT |= 0x4000;
            gUnk_0200EBC0.unk2C = 4;
            gUnk_0200EBC0.unk03 |= 4;
            gUnk_0200EBC0.unk01 = 0;
            gUnk_0200EBC0.unk2A = gUnk_0200EBC0.unk02 = gUnk_0200EBC0.unk24 = gUnk_0200EBC0.unk25 = 0;
            gUnk_0200EC48 = 0;
            return;
        }
    }
    if (sub_08004e9c(&gUnk_0200EBF0) != 0)
        gUnk_0200EBC0.unk02 = 3;
}
