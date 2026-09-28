#include "global.h"
#include "room.h"

/* The room table gRoomTable[level][stage] (0x087E1D58-0x087E1E77, issue #36
 * phase 2): levels 0-8 (8 = the hub), eight stages each, every entry a
 * stage's room list (src/data/room_lists.c) or NULL.  The room loaders
 * read gRoomTable[level][stage][room] into gCurRoomDef (sub_08023948,
 * sub_08023ca0 and sub_08023fd4 in src/level_23948.c, and src/level_242d0.c)
 * and the camera tasks read the next room's palettes through it
 * (src/camtask_2d38c.c).
 * Carved by tools/carve_data.py. */

struct RoomDef *const *const gRoomTable[9][8] = {
    { /* level 0 */
        gLevel0Stage0Rooms,
        gLevel0Stage1Rooms,
        gLevel0Stage2Rooms,
        gLevel0Stage3Rooms,
        gLevel0Stage4Rooms,
        NULL,
        NULL,
        NULL,
    },
    { /* level 1 */
        gLevel1Stage0Rooms,
        gLevel1Stage1Rooms,
        gLevel1Stage2Rooms,
        gLevel1Stage3Rooms,
        gLevel1Stage4Rooms,
        gLevel1Stage5Rooms,
        NULL,
        NULL,
    },
    { /* level 2 */
        gLevel2Stage0Rooms,
        gLevel2Stage1Rooms,
        gLevel2Stage2Rooms,
        gLevel2Stage3Rooms,
        gLevel2Stage4Rooms,
        gLevel2Stage5Rooms,
        gLevel2Stage6Rooms,
        NULL,
    },
    { /* level 3 */
        gLevel3Stage0Rooms,
        gLevel3Stage1Rooms,
        gLevel3Stage2Rooms,
        gLevel3Stage3Rooms,
        gLevel3Stage4Rooms,
        gLevel3Stage5Rooms,
        gLevel3Stage6Rooms,
        NULL,
    },
    { /* level 4 */
        gLevel4Stage0Rooms,
        gLevel4Stage1Rooms,
        gLevel4Stage2Rooms,
        gLevel4Stage3Rooms,
        gLevel4Stage4Rooms,
        gLevel4Stage5Rooms,
        gLevel4Stage6Rooms,
        NULL,
    },
    { /* level 5 */
        gLevel5Stage0Rooms,
        gLevel5Stage1Rooms,
        gLevel5Stage2Rooms,
        gLevel5Stage3Rooms,
        gLevel5Stage4Rooms,
        gLevel5Stage5Rooms,
        gLevel5Stage6Rooms,
        NULL,
    },
    { /* level 6 */
        gLevel6Stage0Rooms,
        gLevel6Stage1Rooms,
        gLevel6Stage2Rooms,
        gLevel6Stage3Rooms,
        gLevel6Stage4Rooms,
        gLevel6Stage5Rooms,
        gLevel6Stage6Rooms,
        NULL,
    },
    { /* level 7 */
        gLevel7Stage0Rooms,
        gLevel7Stage1Rooms,
        gLevel7Stage2Rooms,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
    { /* level 8 */
        gLevel8Stage0Rooms,
        gLevel8Stage1Rooms,
        gLevel8Stage2Rooms,
        gLevel8Stage3Rooms,
        gLevel8Stage4Rooms,
        gLevel8Stage5Rooms,
        gLevel8Stage6Rooms,
        gLevel8Stage7Rooms,
    },
};
