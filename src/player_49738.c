#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "player.h"

/* player_49738.c (0x08049738-0x08049B47, issue #88).
 *
 * The player's ability sprite-tile loaders, called by M09's player task
 * (Task_Player, PlayerStartRequestedAction), M10's actions 13 and 21, M13's action
 * 29, M14 and M18's ability objects (src/actor_6ef5c.c).  LoadAbilityTiles
 * uploads the tiles of the ability PlayerState.ability (a 25-way switch:
 * abilities 0, 1, 3, 5, 6, 8-11, 13, 14, 16, 19, 20 and 24 have tiles)
 * from its ROM table into the player's OBJ tiles at
 * 0x06010000 + (Task.tileWord & 0x7FF) * 32, four 1D rows queued with the
 * VRAM transfer queue RequestCopy; ability 0 also queues its palette
 * gUnk_081AC358 into OBJ palette slot (Task.tileWord >> 12) + 1.  Cases
 * with the same row layout share one body in the ROM (cross-jumping).
 * PlayerLoadSparkTiles uploads ability 2's tiles gUnk_081BE45C at +0x100, and
 * again at +0x180 when gInHub is set. */

void RequestCopy(u32 mode, void *src, void *dst, u32 size);   /* early_1518; effect_5afac's pointer spelling */

void LoadAbilityTiles(void)
{
    u8 *vram = gObjVram + ((gCurTask->tileWord & 0x7FF) << 5);

    switch (gCurTask->player->ability) {
    case 0:
        RequestCopy(1, gUnk_081AC378, vram + 0x180, 128);
        RequestCopy(1, gUnk_081AC378 + 128, vram + 0x580, 128);
        RequestCopy(1, gUnk_081AC378 + 256, vram + 0x980, 128);
        RequestCopy(1, gUnk_081AC378 + 384, vram + 0xD80, 128);
        RequestCopy(2, gUnk_081AC358,
                     gObjPalette + (((gCurTask->tileWord >> 12) + 1) << 5), 32);
        break;
    case 1:
        RequestCopy(1, gUnk_081BBD70, vram + 0x180, 128);
        RequestCopy(1, gUnk_081BBD70 + 128, vram + 0x580, 128);
        RequestCopy(1, gUnk_081BBD70 + 256, vram + 0x980, 128);
        RequestCopy(1, gUnk_081BBD70 + 384, vram + 0xD80, 128);
        break;
    case 3:
        RequestCopy(1, gUnk_081BFE38, vram + 0x100, 256);
        RequestCopy(1, gUnk_081BFE38 + 256, vram + 0x500, 256);
        RequestCopy(1, gUnk_081BFE38 + 512, vram + 0x900, 256);
        RequestCopy(1, gUnk_081BFE38 + 768, vram + 0xD00, 256);
        break;
    case 5:
        RequestCopy(1, gUnk_081CC328, vram + 0x100, 256);
        RequestCopy(1, gUnk_081CC328 + 256, vram + 0x500, 256);
        RequestCopy(1, gUnk_081CC328 + 512, vram + 0x900, 256);
        RequestCopy(1, gUnk_081CC328 + 768, vram + 0xD00, 256);
        break;
    case 6:
        RequestCopy(1, gUnk_081CF260, vram + 0x180, 128);
        RequestCopy(1, gUnk_081CF260 + 128, vram + 0x580, 128);
        RequestCopy(1, gUnk_081CF260 + 256, vram + 0x980, 128);
        RequestCopy(1, gUnk_081CF260 + 384, vram + 0xD80, 128);
        break;
    case 8:
        RequestCopy(1, gUnk_081D5B04, vram + 0x100, 256);
        RequestCopy(1, gUnk_081D5B04 + 256, vram + 0x500, 256);
        RequestCopy(1, gUnk_081D5B04 + 512, vram + 0x900, 256);
        RequestCopy(1, gUnk_081D5B04 + 768, vram + 0xD00, 256);
        break;
    case 9:
        RequestCopy(1, gUnk_081DCDFC, vram + 0x180, 128);
        RequestCopy(1, gUnk_081DCDFC + 128, vram + 0x580, 128);
        RequestCopy(1, gUnk_081DCDFC + 256, vram + 0x980, 128);
        RequestCopy(1, gUnk_081DCDFC + 384, vram + 0xD80, 128);
        break;
    case 10:
        RequestCopy(1, gUnk_081E1D0C, vram + 0x180, 128);
        RequestCopy(1, gUnk_081E1D0C + 128, vram + 0x580, 128);
        RequestCopy(1, gUnk_081E1D0C + 256, vram + 0x980, 128);
        RequestCopy(1, gUnk_081E1D0C + 384, vram + 0xD80, 128);
        break;
    case 11:
        RequestCopy(1, gUnk_081E43B4, vram + 0x100, 64);
        RequestCopy(1, gUnk_081E43B4 + 64, vram + 0x500, 64);
        break;
    case 13:
        RequestCopy(1, gUnk_081EFD60, vram + 0x100, 128);
        RequestCopy(1, gUnk_081EFD60 + 128, vram + 0x500, 128);
        RequestCopy(1, gUnk_081EFD60 + 256, vram + 0x900, 128);
        RequestCopy(1, gUnk_081EFD60 + 384, vram + 0xD00, 128);
        break;
    case 14:
        RequestCopy(1, gUnk_081F1AE0, vram + 0x180, 128);
        RequestCopy(1, gUnk_081F1AE0 + 128, vram + 0x580, 128);
        RequestCopy(1, gUnk_081F1AE0 + 256, vram + 0x980, 128);
        RequestCopy(1, gUnk_081F1AE0 + 384, vram + 0xD80, 128);
        break;
    case 16:
        RequestCopy(1, gUnk_081F6CEC, vram + 0x100, 256);
        RequestCopy(1, gUnk_081F6CEC + 256, vram + 0x500, 256);
        RequestCopy(1, gUnk_081F6CEC + 512, vram + 0x900, 256);
        RequestCopy(1, gUnk_081F6CEC + 768, vram + 0xD00, 256);
        break;
    case 19:
        RequestCopy(1, gUnk_08200D08, vram + 0x100, 256);
        RequestCopy(1, gUnk_08200D08 + 256, vram + 0x500, 256);
        RequestCopy(1, gUnk_08200D08 + 512, vram + 0x900, 256);
        RequestCopy(1, gUnk_08200D08 + 768, vram + 0xD00, 256);
        break;
    case 20:
        RequestCopy(1, gUnk_082036D8, vram + 0x100, 192);
        RequestCopy(1, gUnk_082036D8 + 192, vram + 0x500, 192);
        RequestCopy(1, gUnk_082036D8 + 384, vram + 0x900, 192);
        RequestCopy(1, gUnk_082036D8 + 576, vram + 0xD00, 192);
        break;
    case 24:
        RequestCopy(1, gUnk_082181F0, vram + 0x100, 256);
        RequestCopy(1, gUnk_082181F0 + 256, vram + 0x500, 256);
        RequestCopy(1, gUnk_082181F0 + 512, vram + 0x900, 256);
        RequestCopy(1, gUnk_082181F0 + 768, vram + 0xD00, 256);
        break;
    }
}

void PlayerLoadSparkTiles(void)
{
    struct Task *t = gCurTask;
    u32 off = (t->tileWord & 0x7FF) << 5;

    if (t->player->ability == 2) {
        u8 *src = gUnk_081BE45C;
        RequestCopy(1, src, (void *)(off + (OBJ_VRAM0 + 0x100)), 128);
        RequestCopy(1, src + 128, (void *)(off + (OBJ_VRAM0 + 0x500)), 128);
        RequestCopy(1, src + 256, (void *)(off + (OBJ_VRAM0 + 0x900)), 128);
        RequestCopy(1, src + 384, (void *)(off + (OBJ_VRAM0 + 0xD00)), 128);
        if (gInHub != 0) {
            RequestCopy(1, src, (void *)(off + (OBJ_VRAM0 + 0x180)), 128);
            RequestCopy(1, src + 128, (void *)(off + (OBJ_VRAM0 + 0x580)), 128);
            RequestCopy(1, src + 256, (void *)(off + (OBJ_VRAM0 + 0x980)), 128);
            RequestCopy(1, src + 384, (void *)(off + (OBJ_VRAM0 + 0xD80)), 128);
        }
    }
}
