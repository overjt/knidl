#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "hud.h"
#include "room.h"
#include "player.h"
#include "actor.h"
#include "enemy.h"

/* hud_b5670.c (0x080B5670-0x080B583F, issue #97).
 *
 * Graphics loader for one object of the room's object list gRoomObjectList
 * (called by src/hud_b4ea8.c's LoadRoomObjectGfx for every kind-1 entry, which
 * counts the return values): e is the entry, idx its index in the list and n
 * the next free graphics slot of gRoomObjectGfxSlots.  If an earlier kind-1 entry
 * already uses the same graphics descriptor gEnemyGfx[e->unk1], the
 * entry shares that slot (gRoomObjectGfxSlotIds[idx]) and the function returns 0.
 * Otherwise it claims slot n: it allocates OBJ tiles (AllocObjTiles) and
 * copies the descriptor's tiles (through gUnk_02020000 when compressed), then
 * shares the palette of an earlier slot in the same palette group
 * gUnk_0873EF48[kind] or allocates one (AllocObjPalettes) and copies it into the
 * palette buffer gObjPalette; a non-zero high nibble of e->unk2 is passed
 * to LoadEnemyPaletteVariant with the slot's palette.  It returns 1.
 *
 * Matching notes (issue #97): LoadEnemyPaletteVariant takes three arguments; the VRAM
 * base is a pointer local that global allocation drops (lesson 3.258), and the
 * byte the ROM keeps at [sp, #4] is the compiler's own copy of
 * gRoomObjectGfxSlotIds[i] (lesson 3.474). */

struct RoomObjectEntry
{
    /*0x00*/ s8 kind;
    /*0x01*/ s8 subtype;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 filler3;
    /*0x04*/ u16 x;
    /*0x06*/ u16 y;
};

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);

s32 LoadRoomEnemyGfx(struct RoomObjectEntry *e, s32 idx, s32 n)
{
    struct RoomObjectGfx *d;
    s32 i;
    s32 cnt;
    u32 vram;

    /* a plain pointer local that global alloc drops: reload rematerialises
       `ldr r0, =gObjVram` at both uses, which the ROM's reload order
       needs (lesson 3.258) */
    vram = (u32)gObjVram;
    d = gEnemyGfx[e->subtype];
    if (d == NULL)
        return 0;
    for (i = 0; i < idx; i++)
    {
        if (gRoomObjectList.entries[i].kind == 1)
        {
            if (gRoomObjectGfxSlotIds[i] != -1
             && gEnemyGfx[gRoomObjectGfxSlots[gRoomObjectGfxSlotIds[i]].subtype] == d)
            {
                /* the compiler's QImode copy of this byte is what spills to
                   the ROM's `mov r5, sp; strb r0, [r5, #4]` slot */
                gRoomObjectGfxSlotIds[idx] = gRoomObjectGfxSlotIds[i];
                return 0;
            }
        }
    }
    gRoomObjectGfxSlotIds[idx] = n;
    gRoomObjectGfxSlots[n].subtype = e->subtype;
    if (d->tileCount != 0)
    {
        gRoomObjectGfxSlots[n].tileOffset = AllocObjTiles(d->tileCount);
        if (d->tilesCompressed != 0)
        {
            LZ77UnCompVram((void *)d->tiles, gUnk_02020000);
            RequestCopy(4, (u32)gUnk_02020000, (gRoomObjectGfxSlots[n].tileOffset << 6) + vram, d->tileCount << 5);
        }
        else
        {
            RequestCopy(4, d->tiles, (gRoomObjectGfxSlots[n].tileOffset << 6) + vram, d->tileCount << 5);
        }
    }
    if (d->paletteBankCount != 0)
    {
        cnt = 0;
        if (gUnk_0873EF48[e->subtype] != -1)
        {
            for (i = 0; i < n; i++)
            {
                if (gRoomObjectList.entries[i].kind == 1
                 && gUnk_0873EF48[gRoomObjectGfxSlots[i].subtype] == gUnk_0873EF48[e->subtype])
                {
                    gRoomObjectGfxSlots[n].paletteBank = gRoomObjectGfxSlots[i].paletteBank;
                    cnt++;
                    break;
                }
            }
        }
        if (cnt == 0)
        {
            gRoomObjectGfxSlots[n].paletteBank = AllocObjPalettes(d->paletteBankCount);
            RequestCopy(2, d->palette, (u32)gObjPalette + (gRoomObjectGfxSlots[n].paletteBank << 5), d->paletteBankCount << 5);
        }
        if (e->unk2 >> 4)
            LoadEnemyPaletteVariant(gRoomObjectGfxSlots[n].paletteBank, e->subtype, e->unk2 >> 4);
    }
    return 1;
}
