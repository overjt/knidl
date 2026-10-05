#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "player.h"
#include "save.h"
#include "ending.h"

/* mode_07b68.c (0x08007B68-0x080082CF, issue #96).
 *
 * AgbMain state 13, the title screen of the six extra modes
 * (ExtraModeTitleMain): the mode's picture, two task-#265 decorations and a
 * level select for the first three modes.  In single-pak link play it
 * first stages a multiboot image at 0x02020000 (ExtraModeTitleLoadMultiBootImage, a common
 * blob plus one of three per-mode chunk sets), sends it and runs the
 * 0x5503 SIO handshake (SendLinkBlockAndVerify/ExtraModeTitleSendModeData); ExtraModeTitleLinkErrorScreen is the
 * failure prompt that returns to state 4. */

/* Not from room.h or actor.h: this file's view of gRoomTable and
   gUnk_02007D00 differs (lesson 3.517). */
extern s8 gSubGameLevel;
extern s32 gUnk_02007D00;
extern u16 gGameState;
extern u8 gRoomTable[];

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void PlaySfx(s32 id);
void LinkMain1(u8 *cmd, u16 *send, u16 *recv);

s32 SendLinkBlockAndVerify(u32 *src, u32 *dst, u32 size)
{
    s32 player;
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
        for (player = 0; player < 4; player++) {
            if (gRecvCmds[0][player] == 0x5503) {
                n++;
                if (gRecvCmds[1][player] != 0)
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

s32 ExtraModeTitleSendModeData(void)
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
    ret = SendLinkBlockAndVerify((u32 *)src, gUnk_02004000, size);
    if (ret != 0)
        return 1;
    gExtraModeTitlePhase = 2;
    gDispCnt &= 0xFEFF;
    gBg0ScrollX = ret;
    LinkStartKeyExchange();
    RunLinkFrames(64);
    return 0;
}

void ExtraModeTitleLinkErrorScreen(void)
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
            PlaySfx(SE_CONFIRM);
            break;
        }
        if (gPressedKeys & 2) {
            PlaySfx(SE_CANCEL);
            break;
        }
        RunFrameNoTasks();
    }
    BeginFastFadeOutToWhite();
    RunFramesNoTasksUntilFadeDone();
    gPrevGameState = gUnk_02007FCC + 14;
    gGameState = GAME_STATE_MAIN_MENU;
}

/* Stage the link-play payload at 0x02020000: the common blob
   0x0876B1FC-0x0876F690 plus three chunks picked by gUnk_02006090.  The table
   at 0x020200C8 (inside the copied blob) gets each chunk's address as seen
   once the payload runs from 0x02000000, hence the - 0x20000.  The addresses
   are written as literals: the ROM rebuilds each one as size + constant. */
void ExtraModeTitleLoadMultiBootImage(void)
{
    u32 *tbl = (u32 *)(EWRAM_START + 0x200C8);
    u8 *dst = (u8 *)gUnk_02020000;
    u32 size;

    size = gUnk_0876F690 - gUnk_0876B1FC;
    CpuSet(gUnk_0876B1FC, dst, ((size + 16) / 2) & 0x1FFFFF);
    dst = (u8 *)(size + (EWRAM_START + 0x20010));
    switch (gUnk_02006090) {
    case 0:
        CpuSet(gUnk_085B4ACC, dst, 0x40);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + (EWRAM_START + 0x20090));
        CpuSet(gUnk_085B4B4C, dst, 0xCBE);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + (EWRAM_START + 0x21A0C));
        CpuSet(gUnk_085B64C8, dst, 0x2E0);
        *tbl = (u32)dst - 0x20000;
        dst = (u8 *)(size + (EWRAM_START + 0x21FCC));
        break;
    case 1:
        CpuSet(gUnk_085B113C, dst, 0x30);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + (EWRAM_START + 0x20070));
        CpuSet(gUnk_085B119C, dst, 0xAD8);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + (EWRAM_START + 0x21620));
        CpuSet(gUnk_085B274C, dst, 0x2E0);
        *tbl = (u32)dst - 0x20000;
        dst = (u8 *)(size + (EWRAM_START + 0x21BE0));
        break;
    case 2:
        CpuSet(gUnk_085B2D0C, dst, 0x40);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + (EWRAM_START + 0x20090));
        CpuSet(gUnk_085B2D8C, dst, 0xBC0);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + (EWRAM_START + 0x21810));
        CpuSet(gUnk_085B450C, dst, 0x2E0);
        *tbl = (u32)dst - 0x20000;
        dst = (u8 *)(size + (EWRAM_START + 0x21DD0));
        break;
    }
    MultiBootSetParams((u8 *)gUnk_02020000, dst);
}

void ExtraModeTitleMain(void)
{
    s32 i;
    s32 n;
    u8 k;

    ResetTasksAndOam();
    LoadBgLayout(4);
    ExtraModeTitleLoadPicture(gUnk_02006090);
    LoadGfxSet(63);
    gBg0ScrollX = gBg0ScrollY = 0;
    gBg3ScrollX = gBg3ScrollY = 0;
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1800;
    if (gUnk_02006090 == 6 || gUnk_02006090 == 7) {
        LoadGfxSet(4);
        RequestCopy(6, 0, BG_VRAM + 0x1000, 0x800);
        /* Two complete copies: jump2 cross-jumps the identical tails, which
           is what leaves the ROM's `ldr r0, =F18; b join` arm.  A pointer
           local (if/else or ?:) is folded into "p = b; if (c) p = a". */
        if (gUnk_02006090 == 6) {
            if (gBossEnduranceBestTime[0] != 0 || gBossEnduranceBestTime[1] != 0
                || gBossEnduranceBestTime[2] != 0 || gBossEnduranceBestTime[3] != 0) {
                DrawClockToBgMap(gBossEnduranceBestTime, 22, 18);
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1900;
            }
        } else {
            if (gMetaKnightmareBestTime[0] != 0 || gMetaKnightmareBestTime[1] != 0
                || gMetaKnightmareBestTime[2] != 0 || gMetaKnightmareBestTime[3] != 0) {
                DrawClockToBgMap(gMetaKnightmareBestTime, 22, 18);
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1900;
            }
        }
    }
    CreateExtraModeTitleSprites();
    gUnk_02007D00 = 0;
    if (gLinkSetupMode == 2) {
        gLinkPlayerCount = 999;
        ExtraModeTitleLoadMultiBootImage();
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1900;
        gExtraModeTitlePhase = 0;
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
            ExtraModeTitleLinkErrorScreen();
            EnableSoundDriver();
            return;
        }
        EnableSoundDriver();
        gExtraModeTitlePhase = 1;
        PlayBgm(40);
        /* `cancel` sits here in the ROM, between this arm and the else arm */
        if (ExtraModeTitleSendModeData() == 0)
            goto select;
        ExtraModeTitleLinkErrorScreen();
        return;
    cancel:
        PlaySfx(SE_CANCEL);
        BeginFastFadeOutToWhite();
        RunFramesUntilFadeDone();
        gGameState = GAME_STATE_MAIN_MENU;
        gPrevGameState = gUnk_02007FCC + 14;
        return;
    } else {
        LinkRequestSync();
        LinkSyncRandom();
        LinkStartKeyExchange();
        gExtraModeTitlePhase = 3;
        BeginFastFadeInFromWhite();
        RunLinkFramesUntilFadeDone();
        RunLinkFrames(16);
    }
select:
    gExtraModeTitlePhase = 3;
    while (1) {
        RunLinkFrame();
        if (gPrevGameState == GAME_STATE_MAIN_MENU) {
            if (gUnk_02006090 <= 2) {
                if ((gPlayerPressedKeys[0] & 0x20) && gSubGameLevel != 0) {
                    PlaySfx(SE_CURSOR_MOVE);
                    gSubGameLevel--;
                } else if ((gPlayerPressedKeys[0] & 0x10) && gSubGameLevel != 2) {
                    PlaySfx(SE_CURSOR_MOVE);
                    gSubGameLevel++;
                }
            }
            if (gPlayerCount == 1 && (gPlayerPressedKeys[0] & 2))
                goto cancel;
            if (gPlayerPressedKeys[0] & 9) {
                PlaySfx(SE_CONFIRM);
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
    gExtraModeTitlePhase = 4;
    LinkStopKeyExchange();
    /* store address first, then the one read of gUnk_02007FCC, kept in k */
    gGameState = (k = gUnk_02007FCC) + 14;
    if (gPrevGameState == GAME_STATE_MAIN_MENU && k <= 2)
        RunLinkFrames(32);
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
}
