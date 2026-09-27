#include "global.h"
#include "room.h"

/* The stage room lists gRoomTable[level][stage] points at (0x087E1F58-
 * 0x087E256F, issue #36 phase 2): each list holds the struct RoomDef headers
 * of one stage in room order, ended by a NULL.  gRoomIndex indexes them
 * (src/level_23948.c, src/level_242d0.c, src/block_318b4.c).  The headers
 * themselves stay structure-only data (seg 13), and their maps and graphics
 * are assets.  Carved by tools/carve_data.py. */

/* gRoomTable[7][1] */
struct RoomDef *const gUnk_087E1F58[] = {
    &gUnk_0835D08C,
    &gUnk_0835D300,
    &gUnk_0835D4F0,
    NULL,
};

/* gRoomTable[0][0] */
struct RoomDef *const gUnk_087E1F68[] = {
    &gUnk_0835DDC0,
    &gUnk_0835E2A0,
    &gUnk_0835E70C,
    &gUnk_0835E924,
    NULL,
};

/* gRoomTable[0][1] */
struct RoomDef *const gUnk_087E1F7C[] = {
    &gUnk_0835EF1C,
    &gUnk_0835F1A4,
    &gUnk_0835F544,
    &gUnk_0835FC24,
    &gUnk_0835FFE4,
    &gUnk_083601DC,
    NULL,
};

/* gRoomTable[0][2] */
struct RoomDef *const gUnk_087E1F98[] = {
    &gUnk_08360690,
    &gUnk_08360B38,
    &gUnk_08360F38,
    &gUnk_083613A0,
    NULL,
};

/* gRoomTable[0][3] */
struct RoomDef *const gUnk_087E1FAC[] = {
    &gUnk_0836179C,
    &gUnk_08361CB8,
    &gUnk_08362104,
    &gUnk_08362788,
    NULL,
};

/* gRoomTable[0][4] */
struct RoomDef *const gUnk_087E1FC0[] = {
    &gUnk_08362AD0,
    NULL,
};

/* gRoomTable[1][0] */
struct RoomDef *const gUnk_087E1FC8[] = {
    &gUnk_083630A0,
    &gUnk_083634E0,
    &gUnk_08363A0C,
    &gUnk_08363E04,
    NULL,
};

/* gRoomTable[1][1] */
struct RoomDef *const gUnk_087E1FDC[] = {
    &gUnk_08364014,
    &gUnk_083643BC,
    &gUnk_0836463C,
    &gUnk_08364A38,
    &gUnk_08365558,
    NULL,
};

/* gRoomTable[1][2] */
struct RoomDef *const gUnk_087E1FF4[] = {
    &gUnk_083657C0,
    &gUnk_08366340,
    &gUnk_0836663C,
    &gUnk_08366974,
    &gUnk_08366B98,
    &gUnk_08366D1C,
    NULL,
};

/* gRoomTable[1][3] */
struct RoomDef *const gUnk_087E2010[] = {
    &gUnk_08367274,
    &gUnk_0836753C,
    &gUnk_083677F4,
    &gUnk_08367A50,
    &gUnk_08367E8C,
    &gUnk_083684D4,
    &gUnk_08368710,
    NULL,
};

/* gRoomTable[1][4] */
struct RoomDef *const gUnk_087E2030[] = {
    &gUnk_08368D30,
    &gUnk_083692D8,
    &gUnk_08369BE0,
    &gUnk_0836A0A8,
    &gUnk_0836A318,
    &gUnk_0836A534,
    &gUnk_0836A6DC,
    NULL,
};

/* gRoomTable[1][5] */
struct RoomDef *const gUnk_087E2050[] = {
    &gUnk_0836A89C,
    NULL,
};

/* gRoomTable[2][0] */
struct RoomDef *const gUnk_087E2058[] = {
    &gUnk_0836AC58,
    &gUnk_0836AEE4,
    &gUnk_0836B160,
    &gUnk_0836B44C,
    &gUnk_0836B7B4,
    &gUnk_0836B968,
    NULL,
};

/* gRoomTable[2][1] */
struct RoomDef *const gUnk_087E2074[] = {
    &gUnk_0836BC70,
    &gUnk_0836C0B0,
    &gUnk_0836C50C,
    &gUnk_0836CA98,
    &gUnk_0836CDA8,
    &gUnk_0836D19C,
    &gUnk_0836D334,
    &gUnk_0836D4FC,
    NULL,
};

/* gRoomTable[2][2] */
struct RoomDef *const gUnk_087E2098[] = {
    &gUnk_0836DADC,
    &gUnk_0836DD44,
    &gUnk_0836E09C,
    &gUnk_0836E3F4,
    &gUnk_0836E660,
    NULL,
};

/* gRoomTable[2][3] */
struct RoomDef *const gUnk_087E20B0[] = {
    &gUnk_0836E824,
    &gUnk_0836EED8,
    &gUnk_0836F0A4,
    &gUnk_0836F270,
    &gUnk_0836F634,
    &gUnk_0836F7E4,
    NULL,
};

/* gRoomTable[2][4] */
struct RoomDef *const gUnk_087E20CC[] = {
    &gUnk_0836F9B0,
    &gUnk_0836FD44,
    &gUnk_08370000,
    &gUnk_083702FC,
    &gUnk_083704E4,
    NULL,
};

/* gRoomTable[2][5] */
struct RoomDef *const gUnk_087E20E4[] = {
    &gUnk_08370684,
    &gUnk_08370AF4,
    &gUnk_08370E08,
    &gUnk_0837108C,
    &gUnk_08371388,
    &gUnk_08371728,
    &gUnk_0837181C,
    &gUnk_08371D90,
    &gUnk_08372040,
    &gUnk_08372240,
    NULL,
};

/* gRoomTable[2][6] */
struct RoomDef *const gUnk_087E2110[] = {
    &gUnk_083723B0,
    &gUnk_08372518,
    &gUnk_08372680,
    NULL,
};

/* gRoomTable[3][0] */
struct RoomDef *const gUnk_087E2120[] = {
    &gUnk_08372A78,
    &gUnk_08372D58,
    &gUnk_0837301C,
    &gUnk_083736A8,
    &gUnk_08373914,
    &gUnk_08373AC4,
    NULL,
};

/* gRoomTable[3][1] */
struct RoomDef *const gUnk_087E213C[] = {
    &gUnk_0837410C,
    &gUnk_083742CC,
    &gUnk_083747D0,
    &gUnk_08374B28,
    &gUnk_08374C74,
    &gUnk_08374DBC,
    NULL,
};

/* gRoomTable[3][2] */
struct RoomDef *const gUnk_087E2158[] = {
    &gUnk_0837531C,
    &gUnk_083761D0,
    &gUnk_08376B24,
    &gUnk_08376D94,
    &gUnk_08376F18,
    &gUnk_08377144,
    &gUnk_08377334,
    NULL,
};

/* gRoomTable[3][3] */
struct RoomDef *const gUnk_087E2178[] = {
    &gUnk_08377AF0,
    &gUnk_08377FBC,
    &gUnk_08378A80,
    &gUnk_08378EB0,
    &gUnk_08379218,
    NULL,
};

/* gRoomTable[3][4] */
struct RoomDef *const gUnk_087E2190[] = {
    &gUnk_08379B84,
    &gUnk_08379E60,
    &gUnk_0837A0CC,
    &gUnk_0837A52C,
    &gUnk_0837AB88,
    &gUnk_0837AD38,
    &gUnk_0837AF10,
    NULL,
};

/* gRoomTable[3][5] */
struct RoomDef *const gUnk_087E21B0[] = {
    &gUnk_0837B7CC,
    &gUnk_0837BB9C,
    &gUnk_0837BC90,
    &gUnk_0837BFA8,
    &gUnk_0837C288,
    &gUnk_0837C424,
    &gUnk_0837C520,
    &gUnk_0837C698,
    NULL,
};

/* gRoomTable[3][6] */
struct RoomDef *const gUnk_087E21D4[] = {
    &gUnk_0837D968,
    NULL,
};

/* gRoomTable[4][0] */
struct RoomDef *const gUnk_087E21DC[] = {
    &gUnk_0837DE3C,
    &gUnk_0837E598,
    &gUnk_0837EB34,
    &gUnk_0837ECA8,
    &gUnk_0837EE60,
    NULL,
};

/* gRoomTable[4][1] */
struct RoomDef *const gUnk_087E21F4[] = {
    &gUnk_0837F028,
    &gUnk_0837F73C,
    &gUnk_0837FA44,
    &gUnk_0837FDC8,
    &gUnk_083800D0,
    &gUnk_083805BC,
    &gUnk_08380758,
    NULL,
};

/* gRoomTable[4][2] */
struct RoomDef *const gUnk_087E2214[] = {
    &gUnk_08380CFC,
    &gUnk_083811A8,
    &gUnk_08381CCC,
    &gUnk_083821BC,
    &gUnk_083825D8,
    &gUnk_083828C4,
    NULL,
};

/* gRoomTable[4][3] */
struct RoomDef *const gUnk_087E2230[] = {
    &gUnk_0838328C,
    &gUnk_08383B28,
    &gUnk_083840C0,
    &gUnk_083847B4,
    &gUnk_08384ADC,
    &gUnk_08384CB0,
    &gUnk_08384E3C,
    NULL,
};

/* gRoomTable[4][4] */
struct RoomDef *const gUnk_087E2250[] = {
    &gUnk_08385964,
    &gUnk_08385FCC,
    &gUnk_08386348,
    &gUnk_08386700,
    &gUnk_08386BC0,
    &gUnk_08386F80,
    &gUnk_08387308,
    &gUnk_08387498,
    NULL,
};

/* gRoomTable[4][5] */
struct RoomDef *const gUnk_087E2274[] = {
    &gUnk_083879E8,
    &gUnk_083881D4,
    &gUnk_0838875C,
    &gUnk_08388970,
    &gUnk_08388BB8,
    NULL,
};

/* gRoomTable[4][6] */
struct RoomDef *const gUnk_087E228C[] = {
    &gUnk_0838928C,
    &gUnk_083893DC,
    NULL,
};

/* gRoomTable[5][0] */
struct RoomDef *const gUnk_087E2298[] = {
    &gUnk_08389A60,
    &gUnk_08389F2C,
    &gUnk_0838A420,
    &gUnk_0838A668,
    &gUnk_0838A85C,
    &gUnk_0838A9D0,
    NULL,
};

/* gRoomTable[5][1] */
struct RoomDef *const gUnk_087E22B4[] = {
    &gUnk_0838AF20,
    &gUnk_0838B394,
    &gUnk_0838B744,
    &gUnk_0838BB74,
    &gUnk_0838BD4C,
    &gUnk_0838BF0C,
    &gUnk_0838C068,
    NULL,
};

/* gRoomTable[5][2] */
struct RoomDef *const gUnk_087E22D4[] = {
    &gUnk_0838C574,
    &gUnk_0838CB58,
    &gUnk_0838D190,
    &gUnk_0838D7FC,
    &gUnk_0838DBEC,
    &gUnk_0838DD58,
    &gUnk_0838DEC4,
    &gUnk_0838E038,
    &gUnk_0838E2C8,
    &gUnk_0838E478,
    &gUnk_0838E688,
    &gUnk_0838E818,
    NULL,
};

/* gRoomTable[5][3] */
struct RoomDef *const gUnk_087E2308[] = {
    &gUnk_0838EEE4,
    &gUnk_0838F788,
    &gUnk_0838F914,
    &gUnk_0838FCF4,
    &gUnk_083900AC,
    &gUnk_083902B0,
    NULL,
};

/* gRoomTable[5][4] */
struct RoomDef *const gUnk_087E2324[] = {
    &gUnk_0839059C,
    &gUnk_08390BA8,
    &gUnk_08391138,
    &gUnk_08391C40,
    &gUnk_08391D9C,
    NULL,
};

/* gRoomTable[5][5] */
struct RoomDef *const gUnk_087E233C[] = {
    &gUnk_08392308,
    &gUnk_08392778,
    &gUnk_08392AA8,
    &gUnk_08392F0C,
    &gUnk_08393430,
    &gUnk_083935AC,
    &gUnk_08393760,
    &gUnk_083938C0,
    &gUnk_08393A7C,
    &gUnk_08393C2C,
    &gUnk_08393DC8,
    NULL,
};

/* gRoomTable[5][6] */
struct RoomDef *const gUnk_087E236C[] = {
    &gUnk_08393EF4,
    NULL,
};

/* gRoomTable[6][0] */
struct RoomDef *const gUnk_087E2374[] = {
    &gUnk_08394458,
    &gUnk_08394C58,
    &gUnk_083950F8,
    &gUnk_08395298,
    NULL,
};

/* gRoomTable[6][1] */
struct RoomDef *const gUnk_087E2388[] = {
    &gUnk_08395450,
    &gUnk_0839570C,
    &gUnk_083959C8,
    &gUnk_08395C90,
    &gUnk_08395F34,
    &gUnk_083961E4,
    &gUnk_083964AC,
    &gUnk_08396754,
    &gUnk_08396A00,
    &gUnk_08396CB8,
    &gUnk_0839706C,
    &gUnk_08397358,
    &gUnk_08397508,
    &gUnk_08397680,
    &gUnk_08397804,
    &gUnk_08397970,
    &gUnk_08397B54,
    &gUnk_08397CFC,
    &gUnk_08397ED0,
    &gUnk_0839804C,
    &gUnk_08398220,
    &gUnk_083983D8,
    &gUnk_083985D0,
    &gUnk_0839875C,
    &gUnk_08398858,
    NULL,
};

/* gRoomTable[6][2] */
struct RoomDef *const gUnk_087E23F0[] = {
    &gUnk_08398DDC,
    &gUnk_08399208,
    &gUnk_083995EC,
    &gUnk_08399CB8,
    NULL,
};

/* gRoomTable[6][3] */
struct RoomDef *const gUnk_087E2404[] = {
    &gUnk_0839A214,
    &gUnk_0839A4C8,
    &gUnk_0839A774,
    &gUnk_0839AA40,
    &gUnk_0839AD18,
    &gUnk_0839B260,
    &gUnk_0839BB28,
    NULL,
};

/* gRoomTable[6][4] */
struct RoomDef *const gUnk_087E2424[] = {
    &gUnk_0839BED4,
    &gUnk_0839C0D0,
    &gUnk_0839C57C,
    &gUnk_0839CB34,
    &gUnk_0839CD48,
    NULL,
};

/* gRoomTable[6][5] */
struct RoomDef *const gUnk_087E243C[] = {
    &gUnk_0839D254,
    &gUnk_0839D5B4,
    &gUnk_0839D8BC,
    &gUnk_0839DAFC,
    &gUnk_0839DED4,
    &gUnk_0839E190,
    &gUnk_0839E590,
    &gUnk_0839E944,
    &gUnk_0839EBB4,
    NULL,
};

/* gRoomTable[6][6] */
struct RoomDef *const gUnk_087E2464[] = {
    &gUnk_0839EE14,
    NULL,
};

/* gRoomTable[7][0] */
struct RoomDef *const gUnk_087E246C[] = {
    &gUnk_0839F3D8,
    &gUnk_0839F684,
    &gUnk_0839FC40,
    NULL,
};

/* gRoomTable[7][2] */
struct RoomDef *const gUnk_087E247C[] = {
    &gUnk_083A0058,
    &gUnk_083A0370,
    &gUnk_083A06DC,
    &gUnk_083A0AA0,
    &gUnk_083A0D2C,
    &gUnk_083A0F00,
    &gUnk_083A142C,
    &gUnk_083A1644,
    &gUnk_083A17CC,
    &gUnk_083A1B64,
    &gUnk_083A1FD8,
    NULL,
};

/* gRoomTable[8][0] */
struct RoomDef *const gUnk_087E24AC[] = {
    &gUnk_083A24F0,
    &gUnk_083A2578,
    &gUnk_083A2694,
    &gUnk_083A291C,
    &gUnk_083A2AC4,
    NULL,
};

/* gRoomTable[8][1] */
struct RoomDef *const gUnk_087E24C4[] = {
    &gUnk_083A32BC,
    &gUnk_083A3344,
    &gUnk_083A34E4,
    &gUnk_083A360C,
    &gUnk_083A38FC,
    &gUnk_083A3A74,
    NULL,
};

/* gRoomTable[8][2] */
struct RoomDef *const gUnk_087E24E0[] = {
    &gUnk_083A3FF4,
    &gUnk_083A407C,
    &gUnk_083A421C,
    &gUnk_083A433C,
    &gUnk_083A45DC,
    &gUnk_083A46FC,
    NULL,
};

/* gRoomTable[8][3] */
struct RoomDef *const gUnk_087E24FC[] = {
    &gUnk_083A4D80,
    &gUnk_083A4E08,
    &gUnk_083A4FA8,
    &gUnk_083A50D0,
    &gUnk_083A5354,
    &gUnk_083A5480,
    NULL,
};

/* gRoomTable[8][4] */
struct RoomDef *const gUnk_087E2518[] = {
    &gUnk_083A5BE8,
    &gUnk_083A5C70,
    &gUnk_083A5E10,
    &gUnk_083A5F3C,
    &gUnk_083A61E4,
    &gUnk_083A633C,
    NULL,
};

/* gRoomTable[8][5] */
struct RoomDef *const gUnk_087E2534[] = {
    &gUnk_083A6AE0,
    &gUnk_083A6B68,
    &gUnk_083A6D08,
    &gUnk_083A6E24,
    &gUnk_083A70A0,
    &gUnk_083A71E4,
    NULL,
};

/* gRoomTable[8][6] */
struct RoomDef *const gUnk_087E2550[] = {
    &gUnk_083A77C4,
    &gUnk_083A784C,
    &gUnk_083A7ADC,
    &gUnk_083A7C28,
    NULL,
};

/* gRoomTable[8][7] */
struct RoomDef *const gUnk_087E2564[] = {
    &gUnk_083A8364,
    &gUnk_083A85D4,
    NULL,
};
