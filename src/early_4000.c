#include "gba/gba.h"
#include "global.h"
#include "main.h"

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

/* Not from link.h: this file's view of gMultiBootStruct differs (lesson
   3.517). */
extern struct SioWork gMultiBootStruct;
extern vu16 gMultiBootDataRecv[4];
extern u8 gLinkBlockAcks[4];
extern u32 *gLinkBlockSrc;
extern vs32 gLinkBlockState;
extern u8 gLinkBroadcastAcks;
extern u32 *gLinkBlockDst;
extern u32 gUnk_0200EBB8;
extern vs32 gLinkBlockWords;
extern vs32 gLinkBlockIndex;
extern vu32 gLinkBlockParentChecksum;
extern s32 gLinkBlockFrames;
extern u32 gLinkBlockChecksum;
extern vs32 gLinkSetupMode;
extern u8 gUnk_0200EC4C;
extern u16 gRecvCmds[];
extern u16 gSendCmd[4];

extern void LinkSetupDetect(void);
extern void LinkSetupMultiCart(void);
extern void LinkSetupMultiBoot(void);
extern void LinkBlockParentIntr(void);
extern void LinkBlockChildIntr(void);

/*FN LinkSetupMain*/
void LinkSetupMain(u16 a)
{
    vu16 stat;

    stat = REG_SIOCNT;
    if (gLinkSetupMode == -1)
        return;
    if (gMultiBootStruct.unk02 == 3)
        return;
    gMultiBootStruct.unk0C = a << 13;
    switch (gLinkSetupMode)
    {
    case 0:
        LinkSetupDetect();
        break;
    case 1:
        LinkSetupMultiCart();
        break;
    case 2:
        LinkSetupMultiBoot();
        break;
    }
}

/*FN LinkSetupIntr*/
void LinkSetupIntr(void)
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
    gMultiBootStruct.unk28 = stat;
    gMultiBootStruct.unk26++;
    *(struct SioRecv *)gMultiBootDataRecv = *(struct SioRecv *)REG_ADDR_SIOMULTI0;
    gMultiBootStruct.unk00 = (stat & 0x30) >> 4;
    gMultiBootStruct.unk03 = gMultiBootStruct.unk03 & 0xBF;
    gMultiBootStruct.unk03 = (stat & 0x40) | gMultiBootStruct.unk03;
    if (gLinkSetupMode == 2 || gLinkSetupMode == 0)
    {
        if (REG_SIOCNT & 4)
            REG_SIOMLT_SEND = 0xD951;
        if (gMultiBootDataRecv[1] == 0xFFFF)
            gMultiBootStruct.unk24 = 0;
        src = gMultiBootDataRecv + 1;
        for (i = 0; i < 3; i++)
        {
            if (src[i] != gMultiBootStruct.unk1E[i])
                gMultiBootStruct.unk24 = 0;
            gMultiBootStruct.unk1E[i] = src[i];
        }
        gMultiBootStruct.unk24++;
        if (gMultiBootStruct.unk24 > 29)
            gMultiBootStruct.unk24 = 30;
        return;
    }
    if (gMultiBootDataRecv[0] == 0xE4E4)
    {
        REG_SIOMLT_SEND = 0xE4E4;
        gMultiBootStruct.unk02 = 3;
        return;
    }
    if ((stat & 4) == 0)
    {
        if (gMultiBootStruct.unk0A > 19)
            gMultiBootStruct.unk02 = 2;
    }
    else
    {
        gMultiBootStruct.unk08++;
        gMultiBootStruct.unk08 = gMultiBootStruct.unk08 & 0x1FFF;
        if (gMultiBootStruct.unk08 <= 255)
            gMultiBootStruct.unk08 = 0x100;
        if (gMultiBootStruct.unk08 == (gMultiBootDataRecv[0] & 0x1FFF))
        {
            if (gMultiBootStruct.unk0A > 3)
            {
                if (gMultiBootStruct.unk0C == (gMultiBootDataRecv[0] & 0xE000))
                {
                    gMultiBootStruct.unk03 = gMultiBootStruct.unk03 & 0xFE;
                    gMultiBootStruct.unk0A++;
                }
                else if ((gMultiBootDataRecv[0] & 0xE000) != 0)
                {
                    gMultiBootStruct.unk03 = gMultiBootStruct.unk03 | 1;
                    gMultiBootStruct.unk0A = 0;
                }
            }
            else
            {
                gMultiBootStruct.unk0A++;
            }
        }
        else
        {
            gMultiBootStruct.unk0A = 0;
        }
        gMultiBootStruct.unk08 = gMultiBootDataRecv[0];
        if (gMultiBootStruct.unk0A > 30)
        {
            gMultiBootStruct.unk02 = 2;
            REG_SIOMLT_SEND = 0x26AE;
        }
        else
        {
            gMultiBootStruct.unk02 = 0;
            REG_SIOMLT_SEND = 0xD951;
        }
    }
    gMultiBootStruct.unk01 = 1;
    flag = 1;
    prev = gMultiBootStruct.unk0E;
    gMultiBootStruct.unk0E = 0;
    for (i = 1; i <= 3; i++)
    {
        if (gMultiBootDataRecv[i] == 0xFFFF)
            gMultiBootStruct.unk0E = gMultiBootStruct.unk0E | (1 << i);
        if (((prev >> i) & 1) == 0)
        {
            if (gMultiBootDataRecv[i] == 0x26AE)
            {
                if (gMultiBootDataRecv[i - 1] == 0xFFFF)
                {
                    flag = 0;
                    gMultiBootStruct.unk02 = 1;
                }
                if (flag)
                    gMultiBootStruct.unk01++;
                else
                    gMultiBootStruct.unk01 = 1;
            }
            else if (gMultiBootDataRecv[i] == 0xD951)
            {
                gMultiBootStruct.unk02 = 1;
                if (gMultiBootDataRecv[i - 1] == 0xFFFF)
                {
                    flag = 0;
                    gMultiBootStruct.unk0A = 0;
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

/*FN LinkBroadcastWordStep*/
u32 LinkBroadcastWordStep(void)
{
    s32 i;
    u16 *p;
    u32 n;

    if (gRecvCmds[0] == 0xAA02)
        return 1;
    if (gRecvCmds[0] == 0xAA00)
    {
        gSendCmd[0] = 0xAA01;
        gUnk_0200EC4C = gRecvCmds[4];
    }
    if (gLinkIsMaster != 0)
    {
        p = gRecvCmds;
        for (i = 0; i < 4; i++)
        {
            if (*p == 0xAA01)
                gLinkBroadcastAcks++;
            p++;
        }
        /* Loading unk01 into a temp first, and reading the counter through a
         * volatile cast, is what puts the counter in r0 and unk01 in r1 and
         * emits `cmp r0, r1` the way the ROM does.  */
        n = gMultiBootStruct.unk01;
        if (*(vu8 *)&gLinkBroadcastAcks == n)
        {
            gLinkBroadcastAcks = 0;
            gSendCmd[0] = 0xAA02;
        }
    }
    return 0;
}

/*FN LinkBlockAnnounce*/
void LinkBlockAnnounce(u32 *src, u32 *dst, u32 size)
{
    if (gLinkIsMaster == 0)
        return;
    gUnk_0200EBB8 = 0;
    gLinkBlockSrc = src;
    gLinkBlockDst = dst;
    gLinkBlockWords = (size + 15) >> 2;
    gLinkBlockChecksum = 0;
    gSendCmd[0] = 0x5500;
    gSendCmd[1] = (u32)dst;
    gSendCmd[2] = (u32)dst >> 16;
    gSendCmd[3] = (u32)gLinkBlockWords >> 2;
    gLinkBlockAcks[0] = gLinkBlockAcks[1] = gLinkBlockAcks[2] = gLinkBlockAcks[3] = 0;
}

/*FN LinkBlockHandshakeStep*/
u32 LinkBlockHandshakeStep(void)
{
    s32 i;

    if (gRecvCmds[0] == 0x5502)
        return 1;
    if (gRecvCmds[0] == 0x5500)
    {
        gSendCmd[0] = 0x5501;
        gLinkBlockDst = (u32 *)(gRecvCmds[4] | (gRecvCmds[8] << 16));
        gLinkBlockWords = gRecvCmds[12] << 2;
    }
    if (gLinkIsMaster != 0)
    {
        for (i = 0; i < 4; i++)
        {
            if (gRecvCmds[i] == 0x5501)
            {
                gLinkBlockAcks[i] = 1;
                gUnk_0200EBB8++;
            }
        }
        if (gUnk_0200EBB8 == gMultiBootStruct.unk01)
        {
            gUnk_0200EBB8 = 0;
            gLinkBlockAcks[0] = gLinkBlockAcks[1] = gLinkBlockAcks[2] = gLinkBlockAcks[3] = 0;
            gSendCmd[0] = 0x5502;
        }
    }
    return 0;
}

/*FN LinkBlockStart*/
void LinkBlockStart(void)
{
    gLinkBlockState = 0;
    gLinkBlockFrames = 0;
    gLinkBlockIndex = -1;
    if (gLinkIsMaster != 0)
    {
        gIntrMasterEnable = REG_IME = REG_IME & 0xFFFE;
        REG_IE = gIntrEnable = gIntrEnable & 0xFF3F;
        gIntrMasterEnable = REG_IME = REG_IME | 1;
        REG_SIOCNT = 0x2000;
        REG_TM3CNT = 0;
        gIntrTable[0] = (u32)IntrDummy;
        gIntrTable[1] = (u32)LinkBlockParentIntr;
        REG_SIOCNT = 0x1000;
        REG_SIOCNT = 0x1001;
    }
    else
    {
        gIntrMasterEnable = REG_IME = REG_IME & 0xFFFE;
        REG_IE = gIntrEnable = gIntrEnable & 0xFF3F;
        gIntrMasterEnable = REG_IME = REG_IME | 1;
        gIntrTable[0] = (u32)LinkBlockChildIntr;
        gIntrTable[1] = (u32)IntrDummy;
        REG_SIOCNT = 0x2000;
    }
    gLinkBlockChecksum = 0;
    gLinkDriverMode = 2;
}

/*FN LinkBlockParentIntr*/
void LinkBlockParentIntr(void)
{
    u32 *p;

    REG_TM3CNT_H = 0;
    if (gLinkBlockIndex < 0)
    {
        REG_SIODATA32 = 0xFDB99BDF;
    }
    else if (gLinkBlockIndex < gLinkBlockWords)
    {
        REG_SIODATA32 = *gLinkBlockSrc;
        gLinkBlockChecksum += *gLinkBlockSrc++;
    }
    else if (gLinkBlockIndex == gLinkBlockWords)
    {
        REG_SIODATA32 = gLinkBlockParentChecksum = gLinkBlockChecksum;
    }
    else
    {
        REG_SIODATA32 = 0x9BDFFDB9;
    }
    gLinkBlockIndex++;
    REG_SIOCNT = REG_SIOCNT | 0x80;
    REG_TM3CNT_H = 0xC0;
}

/*FN LinkBlockChildIntr*/
void LinkBlockChildIntr(void)
{
    u32 v;
    u32 *p;

    v = REG_SIODATA32;
    REG_SIOCNT = REG_SIOCNT | 0x80;
    if (gLinkBlockIndex < 0)
    {
        if (v != 0xFDB99BDF)
            return;
    }
    else if (gLinkBlockIndex < gLinkBlockWords)
    {
        p = gLinkBlockDst;
        *p++ = v;
        gLinkBlockDst = p;
        gLinkBlockChecksum += v;
    }
    else if (gLinkBlockIndex == gLinkBlockWords)
    {
        gLinkBlockParentChecksum = v;
    }
    gLinkBlockIndex++;
}

/*FN IsLinkBlockDone*/
u32 IsLinkBlockDone(void)
{
    if (gLinkBlockState == 0x9999)
    {
        gLinkBlockState = 0;
        return 1;
    }
    return 0;
}
