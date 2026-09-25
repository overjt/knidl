#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_49738.c (0x08049738-0x08049B47, issue #88).
 *
 * The player's ability sprite-tile loaders, called by M09's player task
 * (sub_08032688, sub_08032bd0), M10's actions 13 and 21, M13's action
 * 29, M14 and M18's ability objects (src/actor_6ef5c.c).  sub_08049738
 * uploads the tiles of the ability PlayerState.unk0D (a 25-way switch:
 * abilities 0, 1, 3, 5, 6, 8-11, 13, 14, 16, 19, 20 and 24 have tiles)
 * from its ROM table into the player's OBJ tiles at
 * 0x06010000 + (Task.unk40 & 0x7FF) * 32, four 1D rows queued with the
 * VRAM transfer queue sub_080017e4; ability 0 also queues its palette
 * gUnk_081AC358 into OBJ palette slot (Task.unk40 >> 12) + 1.  Cases
 * with the same row layout share one body in the ROM (cross-jumping).
 * sub_08049a58 uploads ability 2's tiles gUnk_081BE45C at +0x100, and
 * again at +0x180 when gUnk_03002444 is set. */

extern u8 gUnk_06010000[];              /* OBJ tile VRAM (M13's sub_08049738) */
extern u8 gUnk_03001470[];              /* OBJ palette buffer (M11 spelling) */
extern u8 gUnk_081AC378[];
extern u8 gUnk_081AC358[];
extern u8 gUnk_081BBD70[];
extern u8 gUnk_081BFE38[];
extern u8 gUnk_081CC328[];
extern u8 gUnk_081CF260[];
extern u8 gUnk_081D5B04[];
extern u8 gUnk_081DCDFC[];
extern u8 gUnk_081E1D0C[];
extern u8 gUnk_081E43B4[];
extern u8 gUnk_081EFD60[];
extern u8 gUnk_081F1AE0[];
extern u8 gUnk_081F6CEC[];
extern u8 gUnk_08200D08[];
extern u8 gUnk_082036D8[];
extern u8 gUnk_082181F0[];
extern s8 gUnk_03002444;
extern u8 gUnk_081BE45C[];

void sub_080017e4(u32 mode, void *src, void *dst, u32 size);   /* early_1518; effect_5afac's pointer spelling */

void sub_08049738(void)
{
    u8 *vram = gUnk_06010000 + ((gUnk_03002490->unk40 & 0x7FF) << 5);

    switch (gUnk_03002490->unk88->unk0D) {
    case 0:
        sub_080017e4(1, gUnk_081AC378, vram + 0x180, 128);
        sub_080017e4(1, gUnk_081AC378 + 128, vram + 0x580, 128);
        sub_080017e4(1, gUnk_081AC378 + 256, vram + 0x980, 128);
        sub_080017e4(1, gUnk_081AC378 + 384, vram + 0xD80, 128);
        sub_080017e4(2, gUnk_081AC358,
                     gUnk_03001470 + (((gUnk_03002490->unk40 >> 12) + 1) << 5), 32);
        break;
    case 1:
        sub_080017e4(1, gUnk_081BBD70, vram + 0x180, 128);
        sub_080017e4(1, gUnk_081BBD70 + 128, vram + 0x580, 128);
        sub_080017e4(1, gUnk_081BBD70 + 256, vram + 0x980, 128);
        sub_080017e4(1, gUnk_081BBD70 + 384, vram + 0xD80, 128);
        break;
    case 3:
        sub_080017e4(1, gUnk_081BFE38, vram + 0x100, 256);
        sub_080017e4(1, gUnk_081BFE38 + 256, vram + 0x500, 256);
        sub_080017e4(1, gUnk_081BFE38 + 512, vram + 0x900, 256);
        sub_080017e4(1, gUnk_081BFE38 + 768, vram + 0xD00, 256);
        break;
    case 5:
        sub_080017e4(1, gUnk_081CC328, vram + 0x100, 256);
        sub_080017e4(1, gUnk_081CC328 + 256, vram + 0x500, 256);
        sub_080017e4(1, gUnk_081CC328 + 512, vram + 0x900, 256);
        sub_080017e4(1, gUnk_081CC328 + 768, vram + 0xD00, 256);
        break;
    case 6:
        sub_080017e4(1, gUnk_081CF260, vram + 0x180, 128);
        sub_080017e4(1, gUnk_081CF260 + 128, vram + 0x580, 128);
        sub_080017e4(1, gUnk_081CF260 + 256, vram + 0x980, 128);
        sub_080017e4(1, gUnk_081CF260 + 384, vram + 0xD80, 128);
        break;
    case 8:
        sub_080017e4(1, gUnk_081D5B04, vram + 0x100, 256);
        sub_080017e4(1, gUnk_081D5B04 + 256, vram + 0x500, 256);
        sub_080017e4(1, gUnk_081D5B04 + 512, vram + 0x900, 256);
        sub_080017e4(1, gUnk_081D5B04 + 768, vram + 0xD00, 256);
        break;
    case 9:
        sub_080017e4(1, gUnk_081DCDFC, vram + 0x180, 128);
        sub_080017e4(1, gUnk_081DCDFC + 128, vram + 0x580, 128);
        sub_080017e4(1, gUnk_081DCDFC + 256, vram + 0x980, 128);
        sub_080017e4(1, gUnk_081DCDFC + 384, vram + 0xD80, 128);
        break;
    case 10:
        sub_080017e4(1, gUnk_081E1D0C, vram + 0x180, 128);
        sub_080017e4(1, gUnk_081E1D0C + 128, vram + 0x580, 128);
        sub_080017e4(1, gUnk_081E1D0C + 256, vram + 0x980, 128);
        sub_080017e4(1, gUnk_081E1D0C + 384, vram + 0xD80, 128);
        break;
    case 11:
        sub_080017e4(1, gUnk_081E43B4, vram + 0x100, 64);
        sub_080017e4(1, gUnk_081E43B4 + 64, vram + 0x500, 64);
        break;
    case 13:
        sub_080017e4(1, gUnk_081EFD60, vram + 0x100, 128);
        sub_080017e4(1, gUnk_081EFD60 + 128, vram + 0x500, 128);
        sub_080017e4(1, gUnk_081EFD60 + 256, vram + 0x900, 128);
        sub_080017e4(1, gUnk_081EFD60 + 384, vram + 0xD00, 128);
        break;
    case 14:
        sub_080017e4(1, gUnk_081F1AE0, vram + 0x180, 128);
        sub_080017e4(1, gUnk_081F1AE0 + 128, vram + 0x580, 128);
        sub_080017e4(1, gUnk_081F1AE0 + 256, vram + 0x980, 128);
        sub_080017e4(1, gUnk_081F1AE0 + 384, vram + 0xD80, 128);
        break;
    case 16:
        sub_080017e4(1, gUnk_081F6CEC, vram + 0x100, 256);
        sub_080017e4(1, gUnk_081F6CEC + 256, vram + 0x500, 256);
        sub_080017e4(1, gUnk_081F6CEC + 512, vram + 0x900, 256);
        sub_080017e4(1, gUnk_081F6CEC + 768, vram + 0xD00, 256);
        break;
    case 19:
        sub_080017e4(1, gUnk_08200D08, vram + 0x100, 256);
        sub_080017e4(1, gUnk_08200D08 + 256, vram + 0x500, 256);
        sub_080017e4(1, gUnk_08200D08 + 512, vram + 0x900, 256);
        sub_080017e4(1, gUnk_08200D08 + 768, vram + 0xD00, 256);
        break;
    case 20:
        sub_080017e4(1, gUnk_082036D8, vram + 0x100, 192);
        sub_080017e4(1, gUnk_082036D8 + 192, vram + 0x500, 192);
        sub_080017e4(1, gUnk_082036D8 + 384, vram + 0x900, 192);
        sub_080017e4(1, gUnk_082036D8 + 576, vram + 0xD00, 192);
        break;
    case 24:
        sub_080017e4(1, gUnk_082181F0, vram + 0x100, 256);
        sub_080017e4(1, gUnk_082181F0 + 256, vram + 0x500, 256);
        sub_080017e4(1, gUnk_082181F0 + 512, vram + 0x900, 256);
        sub_080017e4(1, gUnk_082181F0 + 768, vram + 0xD00, 256);
        break;
    }
}

void sub_08049a58(void)
{
    struct Task *t = gUnk_03002490;
    u32 off = (t->unk40 & 0x7FF) << 5;

    if (t->unk88->unk0D == 2) {
        u8 *src = gUnk_081BE45C;
        sub_080017e4(1, src, (void *)(off + 0x06010100), 128);
        sub_080017e4(1, src + 128, (void *)(off + 0x06010500), 128);
        sub_080017e4(1, src + 256, (void *)(off + 0x06010900), 128);
        sub_080017e4(1, src + 384, (void *)(off + 0x06010D00), 128);
        if (gUnk_03002444 != 0) {
            sub_080017e4(1, src, (void *)(off + 0x06010180), 128);
            sub_080017e4(1, src + 128, (void *)(off + 0x06010580), 128);
            sub_080017e4(1, src + 256, (void *)(off + 0x06010980), 128);
            sub_080017e4(1, src + 384, (void *)(off + 0x06010D80), 128);
        }
    }
}
