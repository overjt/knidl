#include "gba/gba.h"
#include "gba/agb_sram.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "hud.h"
#include "save.h"

s32 WriteSaveSlot(s32 a);
void ClearSaveSlot(s32 a);
u32 CalcSaveSlotChecksum(s32 a);
u32 UpdateSaveSlotChecksum(s32 a);

u32 UpdateSaveSlotChecksum(s32 a)
{
    gSaveSlots[a].checksum = CalcSaveSlotChecksum(a);
}
