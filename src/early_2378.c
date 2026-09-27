#include "gba/gba.h"
#include "global.h"

/* early_2378.c (0x08002378-0x08002667, issue #63).
 *
 * The 0x7700-series link-play handshake, twin of sub_08002668
 * (src/early_2668.c, which keeps the payload in the record gUnk_02006068;
 * this one keeps it in gRngValue and gFrameCount).  It blocks, pumping
 * the link layer once per frame (RunFrameNoTasks, LinkMain1, the abort poll
 * IsLinkError and the reset path sub_08008b8c after 31 frames without the
 * frame counter gSerialIntrCount moving), and drives the state word
 * gLinkCommand through the 0xBB00 exchange of the per-player bytes
 * gUnk_030023A8 and the 0x7700-0x7706 payload exchange, reading the other
 * players' words from gRecvCmds; when the payload
 * arrives and the per-player bytes are still unnegotiated (gUnk_0300244C ==
 * -1) it clamps them and stores their minimum in gUnk_0300244C.
 *
 * Matching note (issue #63): the negotiation tail sits after the loop and is
 * reached by a goto, and the handler's test carries a zero-code STAND-IN
 * conjunct (`&& gUnk_03001EFC == 0`, true because the handler has just
 * cleared that cell) that keeps the branch two-way until the first cse pass;
 * see the comment at the site and lesson 3.488.  Issue #32 had called the
 * residue a gcse insertion one block late (lesson 3.55). */

extern vu32 gRngValue;
extern u16 gFrameCount;
extern u32 gUnk_03001EFC;
extern u16 gLinkIsMaster;
extern s8 gUnk_030023A8[];
extern u16 gLocalPlayer;
extern u16 gLinkPlayerCount;
extern s16 gUnk_0300244C;
extern u16 gRecvCmds[4][4];
extern u32 gSerialIntrCount;
extern u16 gShouldAdvanceLinkState;
extern u16 gSendCmd[4];
extern u16 gLinkCommand;

void RunFrameNoTasks(void);
void LinkMain1(void *, void *, void *);
int IsLinkError(void);
void sub_08008b8c(void);

/* The 0x7700-series link handshake, twin of sub_08002668 (src/early_2668.c)
 * with the payload kept in gRngValue/gFrameCount.  The negotiation
 * tail after the loop is reached by a goto; merge_blocks splices it back in
 * behind the 0x7706 handler, which is where the ROM has the store, `i = 0`
 * and the entry test (lesson 3.488). */
void sub_08002378(void)
{
    int a;
    u32 b;
    int c;
    int i;
    u32 old;

    if (gLinkPlayerCount <= 1)
        return;

    a = 0;
    b = 0;
    c = 0;
    for (;;) {
        switch (gLinkCommand) {
        case 0x7755:
            gSendCmd[0] = 0x7755;
            if (gUnk_0300244C == -1)
                gLinkCommand = 0xBB00;
            else
                gLinkCommand = 0x7700;
            break;
        case 0xBB00:
        case 0xBB01:
        case 0xBB02:
        case 0xBB03:
        case 0xBB04:
            gSendCmd[0] = 0xBB00;
            gSendCmd[1] = 1;
            if (gLocalPlayer == 0) {
                gLinkCommand++;
                if (gLinkCommand > 0xBB04)
                    gLinkCommand = 0x7700;
            } else {
                gLinkCommand = 0x9900;
            }
            break;
        case 0x7700:
            c = 30;
            gSendCmd[0] = 0x7700;
            gLinkCommand = 0x9900;
            break;
        case 0x7701:
            gSendCmd[0] = 0x7701;
            gLinkCommand = 0x9900;
            break;
        case 0x7703:
        case 0x7704:
        case 0x7705:
            gLinkCommand = gLinkCommand + 1;
            break;
        case 0x7702:
        case 0x7706:
            gSendCmd[0] = 0x7706;
            gSendCmd[1] = gRngValue;
            gSendCmd[2] = gRngValue >> 16;
            gSendCmd[3] = gFrameCount;
            gLinkCommand = 0x9900;
            break;
        case 0x9900:
            break;
        }
        old = gSerialIntrCount;
        RunFrameNoTasks();
        LinkMain1(&gShouldAdvanceLinkState, gSendCmd, gRecvCmds);
        if (IsLinkError() != 0)
            sub_08008b8c();
        if (old == gSerialIntrCount) {
            if (++b > 30)
                sub_08008b8c();
        }
        for (i = 0; i < 4; i++) {
            switch (gRecvCmds[0][i]) {
            case 0xBB00:
                gUnk_030023A8[i] = gRecvCmds[1][i];
                if (gLocalPlayer != 0) {
                    if (i == 0)
                        gLinkCommand = 0xBB00;
                }
                break;
            case 0x7700:
                gLinkCommand = 0x7701;
                break;
            case 0x7701:
                if (gLinkIsMaster != 0) {
                    if (++a >= gLinkPlayerCount)
                        gLinkCommand = 0x7702;
                }
                break;
            case 0x7706:
                gRngValue = (gRecvCmds[2][0] << 16) | gRecvCmds[1][0];
                gFrameCount = gRecvCmds[3][0];
                gUnk_03001EFC = 0;
                /* STAND-IN: `gUnk_03001EFC == 0` is always true here (it was
                 * just cleared) and costs no code; it keeps this branch
                 * two-way until cse1 folds it, so the splice happens after
                 * cse1 and the -1 test and the `= 1` store keep separate
                 * loads of &gUnk_0300244C, which loop.c hoists into sl as
                 * the ROM does.  Whatever the original tested here, it
                 * folded the same way; the plain `goto negotiate` form
                 * gives #32's residue (lesson 3.488). */
                if (gUnk_0300244C == -1 && gUnk_03001EFC == 0)
                    goto negotiate;
                return;
            case 0x7755:
                break;
            case 0x9900:
                break;
            }
        }
        if (c != 0) {
            if (a == gLinkPlayerCount)
                c = 0;
            else if (--c == 0) {
                gLinkCommand = 0x7700;
                a = 0;
            }
        }
    }
negotiate:
    gUnk_0300244C = 1;
    for (i = 0; i < gLinkPlayerCount; i++) {
        if (gUnk_030023A8[i] == -1)
            gUnk_030023A8[i] = 0;
        if (gUnk_030023A8[i] < gUnk_0300244C)
            gUnk_0300244C = gUnk_030023A8[i];
    }
}
