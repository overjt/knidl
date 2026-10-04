#include "gba/gba.h"
#include "global.h"
#include "main.h"
#include "sound.h"

/* Link (SIO multi-play + multiboot) driver work area at 0x0200EBC0. */
struct SioWork
{
    /*0x00*/ vu8 localId;
    /*0x01*/ vu8 playerCount;
    /*0x02*/ vu8 state;
    /*0x03*/ vu8 errorFlags;
    /*0x04*/ vu8 unk04;
    /*0x05*/ vu8 unk05;
    /*0x06*/ vu16 sendSeq;
    /*0x08*/ vu16 recvSeq;
    /*0x0A*/ vu16 unk0A;
    /*0x0C*/ vu16 sendCode;
    /*0x0E*/ vu16 absentMask;
    /*0x10*/ u32 bootSrc;
    /*0x14*/ u32 unk14;
    /*0x18*/ u32 bootSize;
    /*0x1C*/ vu16 unk1C;
    /*0x1E*/ vu16 prevRecv[3];
    /*0x24*/ vu8 recvStableFrames;
    /*0x25*/ vu8 detectMask;
    /*0x26*/ vu8 unk26;
    /*0x27*/ vu8 unk27;
    /*0x28*/ vu16 sioCnt;
    /*0x2A*/ vu8 unk2A;
    /*0x2B*/ vu8 unk2B;
    /*0x2C*/ vu8 bootError;
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

/* Not from link.h: this file's view of gMultiBootStruct differs (lesson
   3.517). */
extern struct SioWork gMultiBootStruct;
extern struct MultiBootParam gMultiBootParam;
extern vs32 gLinkBlockState;
extern vs32 gLinkSetupMode;
extern vu16 gMultiBootDataRecv[4];
extern s8 gUnk_030023A8[];
extern s16 gUnk_0300244C;

extern void LinkSetupIntr(void);
extern void MultiBootInit(struct MultiBootParam *);
extern u32 MultiBootMain(struct MultiBootParam *);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void MultiBootStartMaster(struct MultiBootParam *, u32, u32, u32, u32);
extern u32 MultiBootCheckComplete(struct MultiBootParam *);

/*FN LinkSetupInit*/

/*FN LinkSetupStop*/
void LinkSetupStop(void)
{
    gIntrMasterEnable = REG_IME = REG_IME & 0xFFFE;
    gLinkSetupMode = -1;
    REG_SIOCNT = REG_SIOCNT & 0xBFFF;
    REG_SIOMLT_SEND = 0xD952;
    gIntrTable[1] = gIntrTable[0] = (u32)LinkSetupIntr;
    gMultiBootStruct.playerCount = gMultiBootStruct.state = 0;
    REG_IE = gIntrEnable = gIntrEnable & 0xFF3F;
    gIntrMasterEnable = REG_IME = REG_IME | 1;
}

/*FN MultiBootSetParams*/
void MultiBootSetParams(u8 *start, u8 *end)
{
    u32 len;

    len = ((u32)end - (u32)start + 16) & ~15;
    gMultiBootStruct.bootSrc = (u32)start + 0xC0;
    gMultiBootStruct.unk14 = (u32)end;
    gMultiBootStruct.bootSize = len - 0xC0;
    gMultiBootParam.masterp = start;
    gMultiBootParam.server_type = 0;
}

/*FN MultiBootInitWithParams*/
void MultiBootInitWithParams(u8 *start, u8 *end)
{
    vu16 zero;
    u32 len;

    len = ((u32)end - (u32)start + 16) & ~15;
    zero = 0;
    CpuSet((void *)&zero, &gMultiBootParam, 0x01000026);
    gMultiBootStruct.bootSrc = (u32)start + 0xC0;
    gMultiBootStruct.unk14 = (u32)end;
    gMultiBootStruct.bootSize = len - 0xC0;
    gMultiBootParam.masterp = start;
    gMultiBootParam.server_type = 0;
    MultiBootInit(&gMultiBootParam);
}

/*FN LinkSetupRequestStart*/
void LinkSetupRequestStart(void)
{
    if (gMultiBootStruct.unk2B != 0)
        return;
    if (gMultiBootStruct.state != 2)
        return;
    gMultiBootStruct.unk2B = 1;
}

/*FN LinkSetupDetect*/
void LinkSetupDetect(void)
{
    vu16 cnt;
    u16 t;
    s32 i;
    s32 mask;

    cnt = REG_SIOCNT;
    if ((REG_SIOCNT & 4) == 0)
        MultiBootMain(&gMultiBootParam);
    gMultiBootStruct.detectMask = 0;
    if ((cnt & 8) == 0)
    {
        t = cnt & 0x80;
        if (t != 0)
            return;
        gMultiBootStruct.recvStableFrames = gMultiBootStruct.detectMask = gMultiBootStruct.errorFlags = t;
        return;
    }
    if (gMultiBootStruct.recvStableFrames <= 29)
        return;
    mask = 0;
    for (i = 0; i <= 2; i++)
    {
        u16 v;

        v = gMultiBootStruct.prevRecv[i] & 0xFFF0;
        if (v == 0x7200)
            mask |= 1;
        if (v == 0xD950)
            mask |= 2;
    }
    gMultiBootStruct.detectMask = mask;
    mask &= 3;
    if (mask == 3)
    {
        gMultiBootStruct.errorFlags |= 2;
        return;
    }
    gMultiBootStruct.unk2A = 0;
    gMultiBootStruct.state = 0;
    gMultiBootStruct.unk0A = 0;
    if (gMultiBootStruct.detectMask == 1)
    {
        gLinkSetupMode = 2;
        return;
    }
    if (gMultiBootStruct.detectMask == 2)
        gLinkSetupMode = 1;
}

/*FN LinkSetupMultiCart*/
void LinkSetupMultiCart(void)
{
    vu16 cnt;
    u16 t;
    u16 t2;

    cnt = REG_SIOCNT;
    if ((gMultiBootStruct.sioCnt & 0x40) != 0 || (gMultiBootStruct.sioCnt & 8) == 0)
    {
        gMultiBootStruct.playerCount = 0;
        gMultiBootStruct.unk2A = gMultiBootStruct.state = gMultiBootStruct.recvStableFrames = gMultiBootStruct.detectMask = 0;
        gLinkSetupMode = 0;
    }
    else
    {
        t = gMultiBootStruct.sioCnt & 4;
        if (t == 0 && (cnt & 0xFC) != 8)
        {
            gMultiBootStruct.playerCount = t;
            gMultiBootStruct.unk2A = gMultiBootStruct.state = gMultiBootStruct.recvStableFrames = gMultiBootStruct.detectMask = t;
            gLinkSetupMode = t;
        }
        else
        {
            if (gMultiBootDataRecv[0] == 0xE4E4 && (gMultiBootStruct.sioCnt & 4) != 0)
                return;
            t = gMultiBootStruct.unk2B;
            if (t != 0)
            {
                REG_SIOMLT_SEND = 0xE4E4;
                REG_SIOCNT |= 0x80;
                return;
            }
            if (gMultiBootStruct.unk2A != 0)
            {
                if (gMultiBootStruct.state != 2 || gMultiBootStruct.playerCount == 1)
                {
                    gMultiBootStruct.playerCount = t;
                    gMultiBootStruct.unk2A = gMultiBootStruct.state = gMultiBootStruct.recvStableFrames = gMultiBootStruct.detectMask = t;
                    gLinkSetupMode = t;
                    return;
                }
            }
            else if (gMultiBootStruct.state == 2)
            {
                gMultiBootStruct.unk2A = 1;
            }
            if (gMultiBootDataRecv[0] == 0xE4E4)
                return;
            t2 = gMultiBootStruct.sioCnt & 4;
            if (t2 == 0)
            {
                if ((cnt & 0xFC) != 8)
                    gMultiBootStruct.state = 0;
                gMultiBootStruct.sendSeq = gMultiBootStruct.sendSeq + 1;
                gMultiBootStruct.sendSeq = gMultiBootStruct.sendSeq & 0x1FFF;
                if (gMultiBootStruct.sendSeq <= 255)
                    gMultiBootStruct.sendSeq = 0x100;
                REG_SIOMLT_SEND = gMultiBootStruct.sendSeq | gMultiBootStruct.sendCode;
                if ((gMultiBootStruct.sioCnt & 0x4000) == 0)
                {
                    gMultiBootStruct.state = 0;
                    return;
                }
                gMultiBootStruct.unk0A = gMultiBootStruct.unk0A + 1;
                REG_SIOCNT |= 0x80;
                return;
            }
            if (gMultiBootStruct.sendSeq == gMultiBootStruct.recvSeq)
            {
                gMultiBootStruct.state = 0;
                gMultiBootStruct.errorFlags &= 0xFE;
            }
            gMultiBootStruct.sendSeq = gMultiBootStruct.recvSeq;
        }
    }
}

/*FN LinkSetupMultiBoot*/
void LinkSetupMultiBoot(void)
{
    vu16 cnt;
    u16 t;
    u32 r;
    u8 cb;

    cnt = REG_SIOCNT;
    t = gMultiBootStruct.sioCnt & 4;
    if (t != 0)
    {
        gLinkSetupMode = 0;
        return;
    }
    if (gMultiBootStruct.unk2A == 1)
    {
        if (gMultiBootStruct.playerCount == 1 || (gMultiBootParam.client_bit & 14) == 0)
        {
            gMultiBootStruct.playerCount = t;
            gMultiBootStruct.unk2A = gMultiBootStruct.state = gMultiBootStruct.recvStableFrames = gMultiBootStruct.detectMask = t;
            gLinkSetupMode = t;
            return;
        }
    }
    else
    {
        t = gMultiBootParam.client_bit & 14;
        if (t == 0)
            goto noconn;
    }
    if (gMultiBootStruct.unk2A == 0)
        gMultiBootStruct.unk2A = 1;
    gMultiBootStruct.state = 1;
    gMultiBootStruct.playerCount = 1;
    cb = gMultiBootParam.client_bit;
    gMultiBootStruct.playerCount = gMultiBootStruct.playerCount + ((cb >> 1) & 1);
    gMultiBootStruct.playerCount = gMultiBootStruct.playerCount + ((cb >> 2) & 1);
    gMultiBootStruct.playerCount = gMultiBootStruct.playerCount + ((cb >> 3) & 1);
    switch (gMultiBootParam.probe_count)
    {
    case 0:
        if (gMultiBootStruct.unk0A <= 15)
            gMultiBootStruct.unk0A = gMultiBootStruct.unk0A + 1;
        if (gMultiBootStruct.unk0A == 16)
            gMultiBootStruct.state = 2;
        gMultiBootStruct.unk04 = 2;
        break;
    case 0xD1:
        gMultiBootStruct.unk2B = 2;
        gMultiBootStruct.unk04 = 3;
        gMultiBootParam.server_type = 1;
        break;
    }
    if (gMultiBootParam.probe_count > 0xDF)
        gMultiBootStruct.unk04 = 4;
    goto next;
noconn:
    gMultiBootStruct.state = t;
    gMultiBootStruct.unk0A = t;
next:
    if (gMultiBootStruct.unk0A == 17)
    {
        MultiBootStartMaster(&gMultiBootParam, gMultiBootStruct.bootSrc, gMultiBootStruct.bootSize, 4, 1);
        gMultiBootStruct.unk0A = 18;
    }
    if (gMultiBootStruct.unk2B == 1 && gMultiBootStruct.unk0A == 16 && gMultiBootParam.probe_count == 0
     && (gMultiBootParam.client_bit & 14) != 0)
    {
        StopAllSound();
        DisableSoundDriver();
        gMultiBootStruct.unk0A = 17;
        gMultiBootStruct.unk2A = 2;
    }
    r = MultiBootMain(&gMultiBootParam);
    if (gMultiBootStruct.unk2B == 2)
    {
        gMultiBootParam.server_type = 0;
        gMultiBootStruct.unk2B = 0;
        if (r != 0)
        {
            REG_SIOCNT |= 0x4000;
            gMultiBootStruct.bootError = 4;
            gMultiBootStruct.errorFlags |= 4;
            gMultiBootStruct.playerCount = 0;
            gMultiBootStruct.unk2A = gMultiBootStruct.state = gMultiBootStruct.recvStableFrames = gMultiBootStruct.detectMask = 0;
            gLinkSetupMode = 0;
            return;
        }
    }
    if (MultiBootCheckComplete(&gMultiBootParam) != 0)
        gMultiBootStruct.state = 3;
}
