#include "gba/gba.h"
#include "global.h"

/* early_2378.c (0x08002378-0x08002667, issue #63).
 *
 * The 0x7700-series link-play handshake, twin of sub_08002668
 * (src/early_2668.c, which keeps the payload in the record gUnk_02006068;
 * this one keeps it in gUnk_03000FB4 and gUnk_03001EA4).  It blocks, pumping
 * the link layer once per frame (RunFrameNoTasks, LinkMain1, the abort poll
 * IsLinkError and the reset path sub_08008b8c after 31 frames without the
 * frame counter gUnk_03004D7C moving), and drives the state word
 * gUnk_03005274 through the 0xBB00 exchange of the per-player bytes
 * gUnk_030023A8 and the 0x7700-0x7706 payload exchange, reading the other
 * players' words from gUnk_03004D50; when the payload
 * arrives and the per-player bytes are still unnegotiated (gUnk_0300244C ==
 * -1) it clamps them and stores their minimum in gUnk_0300244C.
 *
 * Matching note (issue #63): the negotiation tail sits after the loop and is
 * reached by a goto, and the handler's test carries a zero-code STAND-IN
 * conjunct (`&& gUnk_03001EFC == 0`, true because the handler has just
 * cleared that cell) that keeps the branch two-way until the first cse pass;
 * see the comment at the site and lesson 3.488.  Issue #32 had called the
 * residue a gcse insertion one block late (lesson 3.55). */

extern vu32 gUnk_03000FB4;
extern u16 gUnk_03001EA4;
extern u32 gUnk_03001EFC;
extern u16 gUnk_03001F38;
extern s8 gUnk_030023A8[];
extern u16 gUnk_03002360;
extern u16 gUnk_0300243C;
extern s16 gUnk_0300244C;
extern u16 gUnk_03004D50[4][4];
extern u32 gUnk_03004D7C;
extern u16 gUnk_03004D88;
extern u16 gUnk_03004D90[4];
extern u16 gUnk_03005274;

void RunFrameNoTasks(void);
void LinkMain1(void *, void *, void *);
int IsLinkError(void);
void sub_08008b8c(void);

/* The 0x7700-series link handshake, twin of sub_08002668 (src/early_2668.c)
 * with the payload kept in gUnk_03000FB4/gUnk_03001EA4.  The negotiation
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

    if (gUnk_0300243C <= 1)
        return;

    a = 0;
    b = 0;
    c = 0;
    for (;;) {
        switch (gUnk_03005274) {
        case 0x7755:
            gUnk_03004D90[0] = 0x7755;
            if (gUnk_0300244C == -1)
                gUnk_03005274 = 0xBB00;
            else
                gUnk_03005274 = 0x7700;
            break;
        case 0xBB00:
        case 0xBB01:
        case 0xBB02:
        case 0xBB03:
        case 0xBB04:
            gUnk_03004D90[0] = 0xBB00;
            gUnk_03004D90[1] = 1;
            if (gUnk_03002360 == 0) {
                gUnk_03005274++;
                if (gUnk_03005274 > 0xBB04)
                    gUnk_03005274 = 0x7700;
            } else {
                gUnk_03005274 = 0x9900;
            }
            break;
        case 0x7700:
            c = 30;
            gUnk_03004D90[0] = 0x7700;
            gUnk_03005274 = 0x9900;
            break;
        case 0x7701:
            gUnk_03004D90[0] = 0x7701;
            gUnk_03005274 = 0x9900;
            break;
        case 0x7703:
        case 0x7704:
        case 0x7705:
            gUnk_03005274 = gUnk_03005274 + 1;
            break;
        case 0x7702:
        case 0x7706:
            gUnk_03004D90[0] = 0x7706;
            gUnk_03004D90[1] = gUnk_03000FB4;
            gUnk_03004D90[2] = gUnk_03000FB4 >> 16;
            gUnk_03004D90[3] = gUnk_03001EA4;
            gUnk_03005274 = 0x9900;
            break;
        case 0x9900:
            break;
        }
        old = gUnk_03004D7C;
        RunFrameNoTasks();
        LinkMain1(&gUnk_03004D88, gUnk_03004D90, gUnk_03004D50);
        if (IsLinkError() != 0)
            sub_08008b8c();
        if (old == gUnk_03004D7C) {
            if (++b > 30)
                sub_08008b8c();
        }
        for (i = 0; i < 4; i++) {
            switch (gUnk_03004D50[0][i]) {
            case 0xBB00:
                gUnk_030023A8[i] = gUnk_03004D50[1][i];
                if (gUnk_03002360 != 0) {
                    if (i == 0)
                        gUnk_03005274 = 0xBB00;
                }
                break;
            case 0x7700:
                gUnk_03005274 = 0x7701;
                break;
            case 0x7701:
                if (gUnk_03001F38 != 0) {
                    if (++a >= gUnk_0300243C)
                        gUnk_03005274 = 0x7702;
                }
                break;
            case 0x7706:
                gUnk_03000FB4 = (gUnk_03004D50[2][0] << 16) | gUnk_03004D50[1][0];
                gUnk_03001EA4 = gUnk_03004D50[3][0];
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
            if (a == gUnk_0300243C)
                c = 0;
            else if (--c == 0) {
                gUnk_03005274 = 0x7700;
                a = 0;
            }
        }
    }
negotiate:
    gUnk_0300244C = 1;
    for (i = 0; i < gUnk_0300243C; i++) {
        if (gUnk_030023A8[i] == -1)
            gUnk_030023A8[i] = 0;
        if (gUnk_030023A8[i] < gUnk_0300244C)
            gUnk_0300244C = gUnk_030023A8[i];
    }
}
