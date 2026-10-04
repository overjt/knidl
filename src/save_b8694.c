#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "room.h"
#include "save.h"

/* save_b8694.c (0x080B8694-0x080B8887, issue #94).
 *
 * Link play: copy what each player sent this frame into its 96-byte record
 * gLinkSaveSlots[] (src/early_2b04.c calls it when a mailbox row reads
 * 0x66xx).  The low byte of row 0 of the SIO mailbox gRecvCmds says what
 * rows 1-3 carry: 1 = the byte pairs unk06/unk08/unk0A/unk0C and the
 * halfword unk02 of slot gExtraMode, 2 = the word unk10 of that slot and
 * unk00, 3 and up = one third of a row of the 8x7 grid unk18 (row (k - 3) / 3,
 * columns 0-2, 3-5 or 6 by (k - 3) % 3). */

void ReceiveLinkSaveSlots(void)
{
    s32 i;
    s32 q;
    s32 r;

    for (i = 0; i < gPlayerCount; i++)
    {
        switch (gRecvCmds[0][i] & 0xFF)
        {
        case 0:
            break;
        case 1:
            gLinkSaveSlots[i].curLevel[gExtraMode] = gRecvCmds[1][i] >> 8;
            gLinkSaveSlots[i].unk08[gExtraMode] = gRecvCmds[1][i];
            gLinkSaveSlots[i].furthestLevel[gExtraMode] = gRecvCmds[2][i] >> 8;
            gLinkSaveSlots[i].furthestStage[gExtraMode] = gRecvCmds[2][i];
            gLinkSaveSlots[i].completionPercent[gExtraMode] = gRecvCmds[3][i];
            break;
        case 2:
            gLinkSaveSlots[i].bigSwitchFlags[gExtraMode] = (gRecvCmds[1][i] << 16) | gRecvCmds[2][i];
            gLinkSaveSlots[i].milestoneFlags = gRecvCmds[3][i];
            break;
        default:
            q = Div((gRecvCmds[0][i] & 0xFF) - 3, 3);
            r = Mod((gRecvCmds[0][i] & 0xFF) - 3, 3);
            if (q <= 7)
            {
                switch (r)
                {
                case 0:
                    gLinkSaveSlots[i].stageClearStatus[q][0] = gRecvCmds[1][i];
                    gLinkSaveSlots[i].stageClearStatus[q][1] = gRecvCmds[2][i];
                    gLinkSaveSlots[i].stageClearStatus[q][2] = gRecvCmds[3][i];
                    break;
                case 1:
                    gLinkSaveSlots[i].stageClearStatus[q][3] = gRecvCmds[1][i];
                    gLinkSaveSlots[i].stageClearStatus[q][4] = gRecvCmds[2][i];
                    gLinkSaveSlots[i].stageClearStatus[q][5] = gRecvCmds[3][i];
                    break;
                case 2:
                    gLinkSaveSlots[i].stageClearStatus[q][6] = gRecvCmds[1][i];
                    break;
                }
            }
            break;
        }
    }
}
