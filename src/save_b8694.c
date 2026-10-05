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
    s32 player;
    s32 level;
    s32 r;

    for (player = 0; player < gPlayerCount; player++)
    {
        switch (gRecvCmds[0][player] & 0xFF)
        {
        case 0:
            break;
        case 1:
            gLinkSaveSlots[player].curLevel[gExtraMode] = gRecvCmds[1][player] >> 8;
            gLinkSaveSlots[player].curStage[gExtraMode] = gRecvCmds[1][player];
            gLinkSaveSlots[player].furthestLevel[gExtraMode] = gRecvCmds[2][player] >> 8;
            gLinkSaveSlots[player].furthestStage[gExtraMode] = gRecvCmds[2][player];
            gLinkSaveSlots[player].completionPercent[gExtraMode] = gRecvCmds[3][player];
            break;
        case 2:
            gLinkSaveSlots[player].bigSwitchFlags[gExtraMode] = (gRecvCmds[1][player] << 16) | gRecvCmds[2][player];
            gLinkSaveSlots[player].milestoneFlags = gRecvCmds[3][player];
            break;
        default:
            level = Div((gRecvCmds[0][player] & 0xFF) - 3, 3);
            r = Mod((gRecvCmds[0][player] & 0xFF) - 3, 3);
            if (level <= 7)
            {
                switch (r)
                {
                case 0:
                    gLinkSaveSlots[player].stageClearStatus[level][0] = gRecvCmds[1][player];
                    gLinkSaveSlots[player].stageClearStatus[level][1] = gRecvCmds[2][player];
                    gLinkSaveSlots[player].stageClearStatus[level][2] = gRecvCmds[3][player];
                    break;
                case 1:
                    gLinkSaveSlots[player].stageClearStatus[level][3] = gRecvCmds[1][player];
                    gLinkSaveSlots[player].stageClearStatus[level][4] = gRecvCmds[2][player];
                    gLinkSaveSlots[player].stageClearStatus[level][5] = gRecvCmds[3][player];
                    break;
                case 2:
                    gLinkSaveSlots[player].stageClearStatus[level][6] = gRecvCmds[1][player];
                    break;
                }
            }
            break;
        }
    }
}
