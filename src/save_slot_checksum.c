#include "gba/gba.h"
#include "gba/agb_sram.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "hud.h"
#include "save.h"

u32 UpdateSaveSlotChecksum(s32 a)
{
    gSaveSlots[a].checksum = CalcSaveSlotChecksum(a);
}
