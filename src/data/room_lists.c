#include "global.h"
#include "room.h"

/* The stage room lists gRoomTable[level][stage] points at (0x087E1F58-
 * 0x087E256F, issue #36 phase 2): each list holds the struct RoomDef headers
 * of one stage in room order, ended by a NULL.  gRoomIndex indexes them
 * (src/room_hub.c, src/room_enter_exit.c, src/player_block_anims.c).  The headers
 * themselves stay structure-only data (seg 13), and their maps and graphics
 * are assets.  Carved by tools/carve_data.py. */

/* gRoomTable[7][1] */
struct RoomDef *const gLevel7Stage1Rooms[] = {
    &gLevel7Stage1Room0,
    &gLevel7Stage1Room1,
    &gLevel7Stage1Room2,
    NULL,
};

/* gRoomTable[0][0] */
struct RoomDef *const gLevel0Stage0Rooms[] = {
    &gLevel0Stage0Room0,
    &gLevel0Stage0Room1,
    &gLevel0Stage0Room2,
    &gLevel0Stage0Room3,
    NULL,
};

/* gRoomTable[0][1] */
struct RoomDef *const gLevel0Stage1Rooms[] = {
    &gLevel0Stage1Room0,
    &gLevel0Stage1Room1,
    &gLevel0Stage1Room2,
    &gLevel0Stage1Room3,
    &gLevel0Stage1Room4,
    &gLevel0Stage1Room5,
    NULL,
};

/* gRoomTable[0][2] */
struct RoomDef *const gLevel0Stage2Rooms[] = {
    &gLevel0Stage2Room0,
    &gLevel0Stage2Room1,
    &gLevel0Stage2Room2,
    &gLevel0Stage2Room3,
    NULL,
};

/* gRoomTable[0][3] */
struct RoomDef *const gLevel0Stage3Rooms[] = {
    &gLevel0Stage3Room0,
    &gLevel0Stage3Room1,
    &gLevel0Stage3Room2,
    &gLevel0Stage3Room3,
    NULL,
};

/* gRoomTable[0][4] */
struct RoomDef *const gLevel0Stage4Rooms[] = {
    &gLevel0Stage4Room0,
    NULL,
};

/* gRoomTable[1][0] */
struct RoomDef *const gLevel1Stage0Rooms[] = {
    &gLevel1Stage0Room0,
    &gLevel1Stage0Room1,
    &gLevel1Stage0Room2,
    &gLevel1Stage0Room3,
    NULL,
};

/* gRoomTable[1][1] */
struct RoomDef *const gLevel1Stage1Rooms[] = {
    &gLevel1Stage1Room0,
    &gLevel1Stage1Room1,
    &gLevel1Stage1Room2,
    &gLevel1Stage1Room3,
    &gLevel1Stage1Room4,
    NULL,
};

/* gRoomTable[1][2] */
struct RoomDef *const gLevel1Stage2Rooms[] = {
    &gLevel1Stage2Room0,
    &gLevel1Stage2Room1,
    &gLevel1Stage2Room2,
    &gLevel1Stage2Room3,
    &gLevel1Stage2Room4,
    &gLevel1Stage2Room5,
    NULL,
};

/* gRoomTable[1][3] */
struct RoomDef *const gLevel1Stage3Rooms[] = {
    &gLevel1Stage3Room0,
    &gLevel1Stage3Room1,
    &gLevel1Stage3Room2,
    &gLevel1Stage3Room3,
    &gLevel1Stage3Room4,
    &gLevel1Stage3Room5,
    &gLevel1Stage3Room6,
    NULL,
};

/* gRoomTable[1][4] */
struct RoomDef *const gLevel1Stage4Rooms[] = {
    &gLevel1Stage4Room0,
    &gLevel1Stage4Room1,
    &gLevel1Stage4Room2,
    &gLevel1Stage4Room3,
    &gLevel1Stage4Room4,
    &gLevel1Stage4Room5,
    &gLevel1Stage4Room6,
    NULL,
};

/* gRoomTable[1][5] */
struct RoomDef *const gLevel1Stage5Rooms[] = {
    &gLevel1Stage5Room0,
    NULL,
};

/* gRoomTable[2][0] */
struct RoomDef *const gLevel2Stage0Rooms[] = {
    &gLevel2Stage0Room0,
    &gLevel2Stage0Room1,
    &gLevel2Stage0Room2,
    &gLevel2Stage0Room3,
    &gLevel2Stage0Room4,
    &gLevel2Stage0Room5,
    NULL,
};

/* gRoomTable[2][1] */
struct RoomDef *const gLevel2Stage1Rooms[] = {
    &gLevel2Stage1Room0,
    &gLevel2Stage1Room1,
    &gLevel2Stage1Room2,
    &gLevel2Stage1Room3,
    &gLevel2Stage1Room4,
    &gLevel2Stage1Room5,
    &gLevel2Stage1Room6,
    &gLevel2Stage1Room7,
    NULL,
};

/* gRoomTable[2][2] */
struct RoomDef *const gLevel2Stage2Rooms[] = {
    &gLevel2Stage2Room0,
    &gLevel2Stage2Room1,
    &gLevel2Stage2Room2,
    &gLevel2Stage2Room3,
    &gLevel2Stage2Room4,
    NULL,
};

/* gRoomTable[2][3] */
struct RoomDef *const gLevel2Stage3Rooms[] = {
    &gLevel2Stage3Room0,
    &gLevel2Stage3Room1,
    &gLevel2Stage3Room2,
    &gLevel2Stage3Room3,
    &gLevel2Stage3Room4,
    &gLevel2Stage3Room5,
    NULL,
};

/* gRoomTable[2][4] */
struct RoomDef *const gLevel2Stage4Rooms[] = {
    &gLevel2Stage4Room0,
    &gLevel2Stage4Room1,
    &gLevel2Stage4Room2,
    &gLevel2Stage4Room3,
    &gLevel2Stage4Room4,
    NULL,
};

/* gRoomTable[2][5] */
struct RoomDef *const gLevel2Stage5Rooms[] = {
    &gLevel2Stage5Room0,
    &gLevel2Stage5Room1,
    &gLevel2Stage5Room2,
    &gLevel2Stage5Room3,
    &gLevel2Stage5Room4,
    &gLevel2Stage5Room5,
    &gLevel2Stage5Room6,
    &gLevel2Stage5Room7,
    &gLevel2Stage5Room8,
    &gLevel2Stage5Room9,
    NULL,
};

/* gRoomTable[2][6] */
struct RoomDef *const gLevel2Stage6Rooms[] = {
    &gLevel2Stage6Room0,
    &gLevel2Stage6Room1,
    &gLevel2Stage6Room2,
    NULL,
};

/* gRoomTable[3][0] */
struct RoomDef *const gLevel3Stage0Rooms[] = {
    &gLevel3Stage0Room0,
    &gLevel3Stage0Room1,
    &gLevel3Stage0Room2,
    &gLevel3Stage0Room3,
    &gLevel3Stage0Room4,
    &gLevel3Stage0Room5,
    NULL,
};

/* gRoomTable[3][1] */
struct RoomDef *const gLevel3Stage1Rooms[] = {
    &gLevel3Stage1Room0,
    &gLevel3Stage1Room1,
    &gLevel3Stage1Room2,
    &gLevel3Stage1Room3,
    &gLevel3Stage1Room4,
    &gLevel3Stage1Room5,
    NULL,
};

/* gRoomTable[3][2] */
struct RoomDef *const gLevel3Stage2Rooms[] = {
    &gLevel3Stage2Room0,
    &gLevel3Stage2Room1,
    &gLevel3Stage2Room2,
    &gLevel3Stage2Room3,
    &gLevel3Stage2Room4,
    &gLevel3Stage2Room5,
    &gLevel3Stage2Room6,
    NULL,
};

/* gRoomTable[3][3] */
struct RoomDef *const gLevel3Stage3Rooms[] = {
    &gLevel3Stage3Room0,
    &gLevel3Stage3Room1,
    &gLevel3Stage3Room2,
    &gLevel3Stage3Room3,
    &gLevel3Stage3Room4,
    NULL,
};

/* gRoomTable[3][4] */
struct RoomDef *const gLevel3Stage4Rooms[] = {
    &gLevel3Stage4Room0,
    &gLevel3Stage4Room1,
    &gLevel3Stage4Room2,
    &gLevel3Stage4Room3,
    &gLevel3Stage4Room4,
    &gLevel3Stage4Room5,
    &gLevel3Stage4Room6,
    NULL,
};

/* gRoomTable[3][5] */
struct RoomDef *const gLevel3Stage5Rooms[] = {
    &gLevel3Stage5Room0,
    &gLevel3Stage5Room1,
    &gLevel3Stage5Room2,
    &gLevel3Stage5Room3,
    &gLevel3Stage5Room4,
    &gLevel3Stage5Room5,
    &gLevel3Stage5Room6,
    &gLevel3Stage5Room7,
    NULL,
};

/* gRoomTable[3][6] */
struct RoomDef *const gLevel3Stage6Rooms[] = {
    &gLevel3Stage6Room0,
    NULL,
};

/* gRoomTable[4][0] */
struct RoomDef *const gLevel4Stage0Rooms[] = {
    &gLevel4Stage0Room0,
    &gLevel4Stage0Room1,
    &gLevel4Stage0Room2,
    &gLevel4Stage0Room3,
    &gLevel4Stage0Room4,
    NULL,
};

/* gRoomTable[4][1] */
struct RoomDef *const gLevel4Stage1Rooms[] = {
    &gLevel4Stage1Room0,
    &gLevel4Stage1Room1,
    &gLevel4Stage1Room2,
    &gLevel4Stage1Room3,
    &gLevel4Stage1Room4,
    &gLevel4Stage1Room5,
    &gLevel4Stage1Room6,
    NULL,
};

/* gRoomTable[4][2] */
struct RoomDef *const gLevel4Stage2Rooms[] = {
    &gLevel4Stage2Room0,
    &gLevel4Stage2Room1,
    &gLevel4Stage2Room2,
    &gLevel4Stage2Room3,
    &gLevel4Stage2Room4,
    &gLevel4Stage2Room5,
    NULL,
};

/* gRoomTable[4][3] */
struct RoomDef *const gLevel4Stage3Rooms[] = {
    &gLevel4Stage3Room0,
    &gLevel4Stage3Room1,
    &gLevel4Stage3Room2,
    &gLevel4Stage3Room3,
    &gLevel4Stage3Room4,
    &gLevel4Stage3Room5,
    &gLevel4Stage3Room6,
    NULL,
};

/* gRoomTable[4][4] */
struct RoomDef *const gLevel4Stage4Rooms[] = {
    &gLevel4Stage4Room0,
    &gLevel4Stage4Room1,
    &gLevel4Stage4Room2,
    &gLevel4Stage4Room3,
    &gLevel4Stage4Room4,
    &gLevel4Stage4Room5,
    &gLevel4Stage4Room6,
    &gLevel4Stage4Room7,
    NULL,
};

/* gRoomTable[4][5] */
struct RoomDef *const gLevel4Stage5Rooms[] = {
    &gLevel4Stage5Room0,
    &gLevel4Stage5Room1,
    &gLevel4Stage5Room2,
    &gLevel4Stage5Room3,
    &gLevel4Stage5Room4,
    NULL,
};

/* gRoomTable[4][6] */
struct RoomDef *const gLevel4Stage6Rooms[] = {
    &gLevel4Stage6Room0,
    &gLevel4Stage6Room1,
    NULL,
};

/* gRoomTable[5][0] */
struct RoomDef *const gLevel5Stage0Rooms[] = {
    &gLevel5Stage0Room0,
    &gLevel5Stage0Room1,
    &gLevel5Stage0Room2,
    &gLevel5Stage0Room3,
    &gLevel5Stage0Room4,
    &gLevel5Stage0Room5,
    NULL,
};

/* gRoomTable[5][1] */
struct RoomDef *const gLevel5Stage1Rooms[] = {
    &gLevel5Stage1Room0,
    &gLevel5Stage1Room1,
    &gLevel5Stage1Room2,
    &gLevel5Stage1Room3,
    &gLevel5Stage1Room4,
    &gLevel5Stage1Room5,
    &gLevel5Stage1Room6,
    NULL,
};

/* gRoomTable[5][2] */
struct RoomDef *const gLevel5Stage2Rooms[] = {
    &gLevel5Stage2Room0,
    &gLevel5Stage2Room1,
    &gLevel5Stage2Room2,
    &gLevel5Stage2Room3,
    &gLevel5Stage2Room4,
    &gLevel5Stage2Room5,
    &gLevel5Stage2Room6,
    &gLevel5Stage2Room7,
    &gLevel5Stage2Room8,
    &gLevel5Stage2Room9,
    &gLevel5Stage2Room10,
    &gLevel5Stage2Room11,
    NULL,
};

/* gRoomTable[5][3] */
struct RoomDef *const gLevel5Stage3Rooms[] = {
    &gLevel5Stage3Room0,
    &gLevel5Stage3Room1,
    &gLevel5Stage3Room2,
    &gLevel5Stage3Room3,
    &gLevel5Stage3Room4,
    &gLevel5Stage3Room5,
    NULL,
};

/* gRoomTable[5][4] */
struct RoomDef *const gLevel5Stage4Rooms[] = {
    &gLevel5Stage4Room0,
    &gLevel5Stage4Room1,
    &gLevel5Stage4Room2,
    &gLevel5Stage4Room3,
    &gLevel5Stage4Room4,
    NULL,
};

/* gRoomTable[5][5] */
struct RoomDef *const gLevel5Stage5Rooms[] = {
    &gLevel5Stage5Room0,
    &gLevel5Stage5Room1,
    &gLevel5Stage5Room2,
    &gLevel5Stage5Room3,
    &gLevel5Stage5Room4,
    &gLevel5Stage5Room5,
    &gLevel5Stage5Room6,
    &gLevel5Stage5Room7,
    &gLevel5Stage5Room8,
    &gLevel5Stage5Room9,
    &gLevel5Stage5Room10,
    NULL,
};

/* gRoomTable[5][6] */
struct RoomDef *const gLevel5Stage6Rooms[] = {
    &gLevel5Stage6Room0,
    NULL,
};

/* gRoomTable[6][0] */
struct RoomDef *const gLevel6Stage0Rooms[] = {
    &gLevel6Stage0Room0,
    &gLevel6Stage0Room1,
    &gLevel6Stage0Room2,
    &gLevel6Stage0Room3,
    NULL,
};

/* gRoomTable[6][1] */
struct RoomDef *const gLevel6Stage1Rooms[] = {
    &gLevel6Stage1Room0,
    &gLevel6Stage1Room1,
    &gLevel6Stage1Room2,
    &gLevel6Stage1Room3,
    &gLevel6Stage1Room4,
    &gLevel6Stage1Room5,
    &gLevel6Stage1Room6,
    &gLevel6Stage1Room7,
    &gLevel6Stage1Room8,
    &gLevel6Stage1Room9,
    &gLevel6Stage1Room10,
    &gLevel6Stage1Room11,
    &gLevel6Stage1Room12,
    &gLevel6Stage1Room13,
    &gLevel6Stage1Room14,
    &gLevel6Stage1Room15,
    &gLevel6Stage1Room16,
    &gLevel6Stage1Room17,
    &gLevel6Stage1Room18,
    &gLevel6Stage1Room19,
    &gLevel6Stage1Room20,
    &gLevel6Stage1Room21,
    &gLevel6Stage1Room22,
    &gLevel6Stage1Room23,
    &gLevel6Stage1Room24,
    NULL,
};

/* gRoomTable[6][2] */
struct RoomDef *const gLevel6Stage2Rooms[] = {
    &gLevel6Stage2Room0,
    &gLevel6Stage2Room1,
    &gLevel6Stage2Room2,
    &gLevel6Stage2Room3,
    NULL,
};

/* gRoomTable[6][3] */
struct RoomDef *const gLevel6Stage3Rooms[] = {
    &gLevel6Stage3Room0,
    &gLevel6Stage3Room1,
    &gLevel6Stage3Room2,
    &gLevel6Stage3Room3,
    &gLevel6Stage3Room4,
    &gLevel6Stage3Room5,
    &gLevel6Stage3Room6,
    NULL,
};

/* gRoomTable[6][4] */
struct RoomDef *const gLevel6Stage4Rooms[] = {
    &gLevel6Stage4Room0,
    &gLevel6Stage4Room1,
    &gLevel6Stage4Room2,
    &gLevel6Stage4Room3,
    &gLevel6Stage4Room4,
    NULL,
};

/* gRoomTable[6][5] */
struct RoomDef *const gLevel6Stage5Rooms[] = {
    &gLevel6Stage5Room0,
    &gLevel6Stage5Room1,
    &gLevel6Stage5Room2,
    &gLevel6Stage5Room3,
    &gLevel6Stage5Room4,
    &gLevel6Stage5Room5,
    &gLevel6Stage5Room6,
    &gLevel6Stage5Room7,
    &gLevel6Stage5Room8,
    NULL,
};

/* gRoomTable[6][6] */
struct RoomDef *const gLevel6Stage6Rooms[] = {
    &gLevel6Stage6Room0,
    NULL,
};

/* gRoomTable[7][0] */
struct RoomDef *const gLevel7Stage0Rooms[] = {
    &gLevel7Stage0Room0,
    &gLevel7Stage0Room1,
    &gLevel7Stage0Room2,
    NULL,
};

/* gRoomTable[7][2] */
struct RoomDef *const gLevel7Stage2Rooms[] = {
    &gLevel7Stage2Room0,
    &gLevel7Stage2Room1,
    &gLevel7Stage2Room2,
    &gLevel7Stage2Room3,
    &gLevel7Stage2Room4,
    &gLevel7Stage2Room5,
    &gLevel7Stage2Room6,
    &gLevel7Stage2Room7,
    &gLevel7Stage2Room8,
    &gLevel7Stage2Room9,
    &gLevel7Stage2Room10,
    NULL,
};

/* gRoomTable[8][0] */
struct RoomDef *const gLevel8Stage0Rooms[] = {
    &gLevel8Stage0Room0,
    &gLevel8Stage0Room1,
    &gLevel8Stage0Room2,
    &gLevel8Stage0Room3,
    &gLevel8Stage0Room4,
    NULL,
};

/* gRoomTable[8][1] */
struct RoomDef *const gLevel8Stage1Rooms[] = {
    &gLevel8Stage1Room0,
    &gLevel8Stage1Room1,
    &gLevel8Stage1Room2,
    &gLevel8Stage1Room3,
    &gLevel8Stage1Room4,
    &gLevel8Stage1Room5,
    NULL,
};

/* gRoomTable[8][2] */
struct RoomDef *const gLevel8Stage2Rooms[] = {
    &gLevel8Stage2Room0,
    &gLevel8Stage2Room1,
    &gLevel8Stage2Room2,
    &gLevel8Stage2Room3,
    &gLevel8Stage2Room4,
    &gLevel8Stage2Room5,
    NULL,
};

/* gRoomTable[8][3] */
struct RoomDef *const gLevel8Stage3Rooms[] = {
    &gLevel8Stage3Room0,
    &gLevel8Stage3Room1,
    &gLevel8Stage3Room2,
    &gLevel8Stage3Room3,
    &gLevel8Stage3Room4,
    &gLevel8Stage3Room5,
    NULL,
};

/* gRoomTable[8][4] */
struct RoomDef *const gLevel8Stage4Rooms[] = {
    &gLevel8Stage4Room0,
    &gLevel8Stage4Room1,
    &gLevel8Stage4Room2,
    &gLevel8Stage4Room3,
    &gLevel8Stage4Room4,
    &gLevel8Stage4Room5,
    NULL,
};

/* gRoomTable[8][5] */
struct RoomDef *const gLevel8Stage5Rooms[] = {
    &gLevel8Stage5Room0,
    &gLevel8Stage5Room1,
    &gLevel8Stage5Room2,
    &gLevel8Stage5Room3,
    &gLevel8Stage5Room4,
    &gLevel8Stage5Room5,
    NULL,
};

/* gRoomTable[8][6] */
struct RoomDef *const gLevel8Stage6Rooms[] = {
    &gLevel8Stage6Room0,
    &gLevel8Stage6Room1,
    &gLevel8Stage6Room2,
    &gLevel8Stage6Room3,
    NULL,
};

/* gRoomTable[8][7] */
struct RoomDef *const gLevel8Stage7Rooms[] = {
    &gLevel8Stage7Room0,
    &gLevel8Stage7Room1,
    NULL,
};
