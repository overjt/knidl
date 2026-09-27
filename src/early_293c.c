#include "gba/gba.h"
#include "global.h"
#include "main.h"
#include "link.h"
#include "mode.h"

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void LinkMain1(void *, void *, void *);

void DisconnectLink(void)
{
    int a;
    u32 b;
    int c;
    int i;
    u32 old;

    if (gLinkIsMaster != 0)
        gLinkCommand = 0x7755;
    else
        gLinkCommand = 0x9900;

    if (gLinkPlayerCount <= 1)
        return;

    a = 0;
    b = 0;
    c = 0;
    for (;;) {
        switch (gLinkCommand) {
        case 0x7755:
            gSendCmd[0] = 0x7755;
            gLinkCommand = 0xAA00;
            break;
        case 0xAA00:
            c = 30;
            gSendCmd[0] = 0xAA00;
            gLinkCommand = 0x9900;
            break;
        case 0xAA01:
            gSendCmd[0] = 0xAA01;
            gLinkCommand = 0x9900;
            break;
        case 0xAA02:
            gSendCmd[0] = 0xAA02;
            break;
        }
        old = gSerialIntrCount;
        RunFrameNoTasks();
        LinkMain1(&gShouldAdvanceLinkState, gSendCmd, gRecvCmds);
        if (IsLinkError() != 0)
            LinkErrorScreen();
        if (old == gSerialIntrCount) {
            if (++b > 30)
                LinkErrorScreen();
        }
        for (i = 0; i < 4; i++) {
            switch (gRecvCmds[0][i]) {
            case 0x9900:
                break;
            case 0xAA00:
                gLinkCommand = 0xAA01;
                break;
            case 0xAA01:
                if (gLinkIsMaster != 0) {
                    if (++a >= gLinkPlayerCount)
                        gLinkCommand = 0xAA02;
                }
                break;
            case 0xAA02:
                goto done;
            }
        }
        if (c != 0) {
            if (a == gLinkPlayerCount)
                c = 0;
            else if (--c == 0) {
                gLinkCommand = 0xAA00;
                a = 0;
            }
        }
    }
done:
    if (gLinkIsMaster != 0)
        gLinkDriverMode = 0;
    RunFrames(5);
    DisableSerial();
    gLinkStatus = 0;
}
