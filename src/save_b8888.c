#include "gba/gba.h"
#include "gba/agb_sram.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "hud.h"
#include "room.h"
#include "save.h"

/* Not from link.h: this file's view of gRecvCmds differs (lesson 3.517). */
extern u16 gPlayerCount;
extern u16 gRecvCmds[];
extern u16 gSendCmd[4];
extern void RunLinkFrame(void);

void sub_080b8888(void)
{
    s32 n;
    s32 i;

    LinkStartRecordExchange();
    gLinkSaveSlotPart = 0;
    do
    {
        RunLinkFrame();
        n = 0;
        if (gLinkSaveSlotPart <= 26)
        {
            gLinkSaveSlotPart++;
        }
        else
        {
            for (i = 0; i < gPlayerCount; i++)
            {
                if ((gRecvCmds[i] & 0xFF00) == (204 << 7) && (gRecvCmds[i] & 255) == 27)
                    n++;
            }
        }
    } while (n != gPlayerCount);
    LinkStopKeyExchange();
}
