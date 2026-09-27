#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* mode_07b68.c (0x08007B68-0x080082CF, issue #96).
 *
 * AgbMain state 13, the title screen of the six extra modes
 * (sub_08007f9c): the mode's picture, two task-#265 decorations and a
 * level select for the first three modes.  In single-pak link play it
 * first stages a multiboot image at 0x02020000 (sub_08007e04, a common
 * blob plus one of three per-mode chunk sets), sends it and runs the
 * 0x5503 SIO handshake (sub_08007b68/sub_08007c5c); sub_08007d4c is the
 * failure prompt that returns to state 4. */

extern s8 gLinkSessionMode;
extern u32 gUnk_02004000[];
extern u8 gUnk_02006090;
extern s8 gSubGameLevel;
extern s32 gUnk_02007D00;
extern u8 gUnk_02007FCC;
extern u32 gLinkDriverMode;
extern u8 gLinkBroadcastAcks;
extern vu8 gMultiBootStruct[];
extern vu32 gLinkBlockParentChecksum;
extern u32 gLinkBlockChecksum;
extern u32 gLinkSetupMode;
extern u32 gUnk_02020000[];
extern vs32 gBg0ScrollY;
extern vu16 gPressedKeys;
extern vu16 gFadeBlankAtWhite;
extern vs32 gBg3ScrollX;
extern vs32 gBg3ScrollY;
extern vs32 gBg0ScrollX;
extern vu16 gFadeSteps;
extern vu16 gPlayerPressedKeys[];
extern vu16 gDispCnt;
extern u16 gUnk_03001F18[];
extern u16 gLinkIsMaster;
extern u16 gPrevGameState;
extern u16 gUnk_03002378[];
extern u16 gPlayerCount;
extern u16 gGameState;
extern u16 gLinkPlayerCount;
extern u16 gRecvCmds[4][4];
extern u16 gShouldAdvanceLinkState[];
extern u16 gSendCmd[4];
extern s32 gUnk_03005280;
extern u16 gUnk_085B113C[];
extern u16 gUnk_085B119C[];
extern u16 gUnk_085B274C[];
extern u16 gUnk_085B2D0C[];
extern u16 gUnk_085B2D8C[];
extern u16 gUnk_085B450C[];
extern u16 gUnk_085B4ACC[];
extern u16 gUnk_085B4B4C[];
extern u16 gUnk_085B64C8[];
extern u8 gUnk_0876B1FC[];
extern u8 gUnk_0876F690[];
extern u8 gUnk_087954C0[];
extern u8 gUnk_087C0A4C[];
extern u8 gRoomTable[];

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void BeginFastFadeInFromWhite(void);
void BeginFastFadeOutToWhite(void);
void ResetTasksAndOam(void);
void RunFrameNoTasks(void);
void RunFrame(void);
void LinkStartKeyExchange(void);
void LinkStopKeyExchange(void);
void LinkRequestSync(void);
void LinkSyncRandom(void);
void RunLinkFrame(void);
void RunFrames(s32 count);
void RunFramesNoTasks(s32 count);
void RunLinkFrames(s32 count);
void RunFramesUntilFadeDone(void);
void RunFramesNoTasksUntilFadeDone(void);
void RunLinkFramesUntilFadeDone(void);
s32 PlayBgm(s32 songId);
void PlaySfx(s32 id);
void EnableSoundDriver(void);
void MultiBootSetParams(u8 *start, u8 *end);
void LinkSetupRequestStart(void);
void LinkSetupMain(u16 a);
u32 LinkBroadcastWordStep(void);
void LinkBlockAnnounce(u32 *src, u32 *dst, u32 size);
u32 LinkBlockHandshakeStep(void);
void LinkBlockStart(void);
u32 IsLinkBlockDone(void);
void DisableSerial(void);
void LinkMain1(u8 *cmd, u16 *send, u16 *recv);
u32 ConnectLink(void);
u32 IsLinkError(void);
void sub_080082d0(void);
void LoadBgLayout(s32 a0);
void LoadGfxSet(u16 a0);
void sub_08008e1c(s32 a0);
void DrawClockToBgMap(u16 *a, s32 b, s32 c);

s32 sub_08007b68(u32 *src, u32 *dst, u32 size)
{
    s32 i;
    s32 n;
    s32 m;

    LinkBlockAnnounce(src, dst, size);
    do {
        RunFrame();
        LinkMain1((u8 *)gShouldAdvanceLinkState, gSendCmd, gRecvCmds[0]);
        if (IsLinkError() != 0)
            goto fail;
    } while (LinkBlockHandshakeStep() == 0);
    RunFrames(2);
    LinkBlockStart();
    while (IsLinkBlockDone() == 0)
        RunFrame();
    if (ConnectLink() != 0)
        goto fail;
    gSendCmd[0] = 0x5503;
    gSendCmd[1] = 0;
    if (gLinkBlockChecksum == gLinkBlockParentChecksum)
        gSendCmd[1] = 1;
    m = 0;
    n = 0;
    do {
        for (i = 0; i < 4; i++) {
            if (gRecvCmds[0][i] == 0x5503) {
                n++;
                if (gRecvCmds[1][i] != 0)
                    m++;
            }
        }
        if (n == gLinkPlayerCount)
            goto done;
        RunFrame();
        LinkMain1((u8 *)gShouldAdvanceLinkState, gSendCmd, gRecvCmds[0]);
    } while (IsLinkError() == 0);
fail:
    return 1;
done:
    if (n != m)
        goto fail;
    return 0;
}

s32 sub_08007c5c(void)
{
    u8 *src;
    u32 size;
    s32 ret;

    if (ConnectLink() != 0)
        return 1;
    if (gLinkIsMaster != 0) {
        gLinkBroadcastAcks = 0;
        gSendCmd[0] = 0xAA00;
        gSendCmd[1] = gUnk_02006090;
    }
    do {
        RunFrame();
        LinkMain1((u8 *)gShouldAdvanceLinkState, gSendCmd, gRecvCmds[0]);
        if (IsLinkError() != 0)
            return 1;
    } while (LinkBroadcastWordStep() == 0);
    switch (gUnk_02006090) {
    case 0:
        src = gUnk_0876F690;
        size = gUnk_087954C0 - gUnk_0876F690;
        break;
    case 1:
        src = gUnk_087954C0;
        size = gUnk_087C0A4C - gUnk_087954C0;
        break;
    case 2:
        src = gUnk_087C0A4C;
        size = gRoomTable - gUnk_087C0A4C;
        break;
    }
    ret = sub_08007b68((u32 *)src, gUnk_02004000, size);
    if (ret != 0)
        return 1;
    gUnk_03005280 = 2;
    gDispCnt &= 0xFEFF;
    gBg0ScrollX = ret;
    LinkStartKeyExchange();
    RunLinkFrames(64);
    return 0;
}

void sub_08007d4c(void)
{
    BeginFastFadeOutToWhite();
    RunFramesNoTasksUntilFadeDone();
    DisableSerial();
    LoadBgLayout(3);
    LoadGfxSet(64);
    gBg3ScrollX = gBg3ScrollY = 0;
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x800;
    BeginFastFadeInFromWhite();
    RunFramesNoTasks(32);
    gFadeBlankAtWhite = 0;
    while (1) {
        if (gPressedKeys & 9) {
            PlaySfx(102);
            break;
        }
        if (gPressedKeys & 2) {
            PlaySfx(215);
            break;
        }
        RunFrameNoTasks();
    }
    BeginFastFadeOutToWhite();
    RunFramesNoTasksUntilFadeDone();
    gPrevGameState = gUnk_02007FCC + 14;
    gGameState = 4;
}

/* Stage the link-play payload at 0x02020000: the common blob
   0x0876B1FC-0x0876F690 plus three chunks picked by gUnk_02006090.  The table
   at 0x020200C8 (inside the copied blob) gets each chunk's address as seen
   once the payload runs from 0x02000000, hence the - 0x20000.  The addresses
   are written as literals: the ROM rebuilds each one as size + constant. */
void sub_08007e04(void)
{
    u32 *tbl = (u32 *)0x020200C8;
    u8 *dst = (u8 *)gUnk_02020000;
    u32 size;

    size = gUnk_0876F690 - gUnk_0876B1FC;
    CpuSet(gUnk_0876B1FC, dst, ((size + 16) / 2) & 0x1FFFFF);
    dst = (u8 *)(size + 0x02020010);
    switch (gUnk_02006090) {
    case 0:
        CpuSet(gUnk_085B4ACC, dst, 0x40);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02020090);
        CpuSet(gUnk_085B4B4C, dst, 0xCBE);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02021A0C);
        CpuSet(gUnk_085B64C8, dst, 0x2E0);
        *tbl = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02021FCC);
        break;
    case 1:
        CpuSet(gUnk_085B113C, dst, 0x30);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02020070);
        CpuSet(gUnk_085B119C, dst, 0xAD8);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02021620);
        CpuSet(gUnk_085B274C, dst, 0x2E0);
        *tbl = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02021BE0);
        break;
    case 2:
        CpuSet(gUnk_085B2D0C, dst, 0x40);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02020090);
        CpuSet(gUnk_085B2D8C, dst, 0xBC0);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02021810);
        CpuSet(gUnk_085B450C, dst, 0x2E0);
        *tbl = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02021DD0);
        break;
    }
    MultiBootSetParams((u8 *)gUnk_02020000, dst);
}

void sub_08007f9c(void)
{
    s32 i;
    s32 n;
    u8 k;

    ResetTasksAndOam();
    LoadBgLayout(4);
    sub_08008e1c(gUnk_02006090);
    LoadGfxSet(63);
    gBg0ScrollX = gBg0ScrollY = 0;
    gBg3ScrollX = gBg3ScrollY = 0;
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1800;
    if (gUnk_02006090 == 6 || gUnk_02006090 == 7) {
        LoadGfxSet(4);
        RequestCopy(6, 0, 0x06001000, 0x800);
        /* Two complete copies: jump2 cross-jumps the identical tails, which
           is what leaves the ROM's `ldr r0, =F18; b join` arm.  A pointer
           local (if/else or ?:) is folded into "p = b; if (c) p = a". */
        if (gUnk_02006090 == 6) {
            if (gUnk_03001F18[0] != 0 || gUnk_03001F18[1] != 0
                || gUnk_03001F18[2] != 0 || gUnk_03001F18[3] != 0) {
                DrawClockToBgMap(gUnk_03001F18, 22, 18);
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1900;
            }
        } else {
            if (gUnk_03002378[0] != 0 || gUnk_03002378[1] != 0
                || gUnk_03002378[2] != 0 || gUnk_03002378[3] != 0) {
                DrawClockToBgMap(gUnk_03002378, 22, 18);
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1900;
            }
        }
    }
    sub_080082d0();
    gUnk_02007D00 = 0;
    if (gLinkSetupMode == 2) {
        gLinkPlayerCount = 999;
        sub_08007e04();
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1900;
        gUnk_03005280 = 0;
        BeginFastFadeInFromWhite();
        while (gFadeSteps != 0) {
            RunFrame();
            LinkSetupMain(gLinkSessionMode);
        }
        gFadeBlankAtWhite = 0;
        LinkSetupRequestStart();
        do {
            RunFrame();
            LinkSetupMain(gLinkSessionMode);
        } while (gMultiBootStruct[2] != 3 && gMultiBootStruct[44] == 0);
        if (gMultiBootStruct[3] != 0) {
            sub_08007d4c();
            EnableSoundDriver();
            return;
        }
        EnableSoundDriver();
        gUnk_03005280 = 1;
        PlayBgm(40);
        /* `cancel` sits here in the ROM, between this arm and the else arm */
        if (sub_08007c5c() == 0)
            goto select;
        sub_08007d4c();
        return;
    cancel:
        PlaySfx(215);
        BeginFastFadeOutToWhite();
        RunFramesUntilFadeDone();
        gGameState = 4;
        gPrevGameState = gUnk_02007FCC + 14;
        return;
    } else {
        LinkRequestSync();
        LinkSyncRandom();
        LinkStartKeyExchange();
        gUnk_03005280 = 3;
        BeginFastFadeInFromWhite();
        RunLinkFramesUntilFadeDone();
        RunLinkFrames(16);
    }
select:
    gUnk_03005280 = 3;
    while (1) {
        RunLinkFrame();
        if (gPrevGameState == 4) {
            if (gUnk_02006090 <= 2) {
                if ((gPlayerPressedKeys[0] & 0x20) && gSubGameLevel != 0) {
                    PlaySfx(101);
                    gSubGameLevel--;
                } else if ((gPlayerPressedKeys[0] & 0x10) && gSubGameLevel != 2) {
                    PlaySfx(101);
                    gSubGameLevel++;
                }
            }
            if (gPlayerCount == 1 && (gPlayerPressedKeys[0] & 2))
                goto cancel;
            if (gPlayerPressedKeys[0] & 9) {
                PlaySfx(102);
                break;
            }
        } else {
            n = 0;
            for (i = 0; i < gPlayerCount; i++) {
                if (gPlayerPressedKeys[i] & 9)
                    n++;
            }
            if (n != 0)
                break;
        }
    }
    if (gLinkSetupMode == 2)
        gLinkDriverMode = 0;
    gUnk_03005280 = 4;
    LinkStopKeyExchange();
    /* store address first, then the one read of gUnk_02007FCC, kept in k */
    gGameState = (k = gUnk_02007FCC) + 14;
    if (gPrevGameState == 4 && k <= 2)
        RunLinkFrames(32);
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
}
