#include "global.h"
#include "room.h"

/* The room table gRoomTable[level][stage] (0x087E1D58-0x087E1E77, issue #36
 * phase 2): levels 0-8 (8 = the hub), eight stages each, every entry a
 * stage's room list (src/data/room_lists.c) or NULL.  The room loaders
 * read gRoomTable[level][stage][room] into gCurRoomDef (LoadRoom and its
 * siblings in src/level_23948.c, src/level_242d0.c) and the camera tasks
 * read the next room's palettes through it (src/camtask_2d38c.c).
 * Carved by tools/carve_data.py. */

struct RoomDef *const *const gRoomTable[9][8] = {
    { /* level 0 */
        gUnk_087E1F68,
        gUnk_087E1F7C,
        gUnk_087E1F98,
        gUnk_087E1FAC,
        gUnk_087E1FC0,
        NULL,
        NULL,
        NULL,
    },
    { /* level 1 */
        gUnk_087E1FC8,
        gUnk_087E1FDC,
        gUnk_087E1FF4,
        gUnk_087E2010,
        gUnk_087E2030,
        gUnk_087E2050,
        NULL,
        NULL,
    },
    { /* level 2 */
        gUnk_087E2058,
        gUnk_087E2074,
        gUnk_087E2098,
        gUnk_087E20B0,
        gUnk_087E20CC,
        gUnk_087E20E4,
        gUnk_087E2110,
        NULL,
    },
    { /* level 3 */
        gUnk_087E2120,
        gUnk_087E213C,
        gUnk_087E2158,
        gUnk_087E2178,
        gUnk_087E2190,
        gUnk_087E21B0,
        gUnk_087E21D4,
        NULL,
    },
    { /* level 4 */
        gUnk_087E21DC,
        gUnk_087E21F4,
        gUnk_087E2214,
        gUnk_087E2230,
        gUnk_087E2250,
        gUnk_087E2274,
        gUnk_087E228C,
        NULL,
    },
    { /* level 5 */
        gUnk_087E2298,
        gUnk_087E22B4,
        gUnk_087E22D4,
        gUnk_087E2308,
        gUnk_087E2324,
        gUnk_087E233C,
        gUnk_087E236C,
        NULL,
    },
    { /* level 6 */
        gUnk_087E2374,
        gUnk_087E2388,
        gUnk_087E23F0,
        gUnk_087E2404,
        gUnk_087E2424,
        gUnk_087E243C,
        gUnk_087E2464,
        NULL,
    },
    { /* level 7 */
        gUnk_087E246C,
        gUnk_087E1F58,
        gUnk_087E247C,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
    { /* level 8 */
        gUnk_087E24AC,
        gUnk_087E24C4,
        gUnk_087E24E0,
        gUnk_087E24FC,
        gUnk_087E2518,
        gUnk_087E2534,
        gUnk_087E2550,
        gUnk_087E2564,
    },
};
