#include "global.h"
#include "room.h"

/* The room headers of room_data (0x0835D08C-0x083A862B, issue #167): the
 * 333 struct RoomDef records (0x58 bytes) the stage room lists point at
 * (src/data/room_lists.c), in address order.  Every room loader reads
 * gRoomTable[level][stage][room] into gCurRoomDef (LoadRoom,
 * src/level_2296c.c:274; LoadHubRoom, src/level_23948.c:44;
 * LoadEndingRoom, src/level_242d0.c:221; and five more) and copies its
 * sizes, borders, object count and flags into the room cells; the camera
 * loads its palettes, tiles, metatile tiles and BG3 map
 * (src/camera_28b8c.c:298-340) and its entry point (:151), the doors are
 * walked by sub_08026b60 (src/door_26b60.c:26) and EnterDoor, the block
 * table unk10 by src/block_30804.c:459.  filler00 is {level, stage, room,
 * 0} in every record, but no code reads it.
 *
 * Each room lays out its metatile map, block layer, block table (unk10),
 * doors and object list right before its header: those are level layouts
 * (assets, docs/data.md section 1) and stay byte slices of baserom.gba in
 * the structure-only data file data/room_data.s, as do the palettes,
 * tiles, metatile tiles and BG3 maps (room_data, room_metatiles,
 * room_bg3_maps, compressed_graphics).  This file names them by their
 * labels only; a room without doors or objects has NULL there.
 *
 * No two headers are adjacent, so every record is its own named section
 * .room_def_<address>: linker.ld lists them between the data pieces of
 * room_data inside ONE output section (docs/data.md 5.2).  The records
 * are not const (gCurRoomDef and the room lists hold struct RoomDef *, so
 * the qualifier would reach the loaders under -Werror); the section
 * attribute is what places them in ROM.  The labels they point at are
 * declared here with the field's pointee type, without const, so the
 * initializers need no casts.  Every pointer is the symbol the data file
 * had (docs/data.md 5.2), so the link and the shift test see the same
 * relocations. */

#define ROOM_DEF(addr) __attribute__((section(".room_def_" #addr)))

/* bg2Palette, bg3Palette: {u16 byteSize, colours}, copied by
 * LoadBg2Gfx/LoadBg3Gfx (src/camera_28b8c.c:298-308) */
extern u16 gUnk_0835D548[];
extern u16 gUnk_0835D60C[];
extern u16 gUnk_0835D710[];
extern u16 gUnk_0835D814[];
extern u16 gUnk_0835D918[];
extern u16 gUnk_0848ADA0[];
extern u16 gUnk_0849096C[];
extern u16 gUnk_08497C74[];
extern u16 gUnk_0849A52C[];
extern u16 gUnk_0849A5F0[];
extern u16 gUnk_0849A6B4[];
extern u16 gUnk_0849A778[];
extern u16 gUnk_0849A83C[];
extern u16 gUnk_0849A940[];
extern u16 gUnk_0849AA04[];
extern u16 gUnk_0849AAC8[];
extern u16 gUnk_084A7BF8[];
extern u16 gUnk_084B2F9C[];
extern u16 gUnk_084B57E8[];
extern u16 gLevel2Stage5Room5Bg2Palette[];
extern u16 gLevel4Stage6Room0Bg2Palette[];
extern u16 gLevel4Stage2Room4Bg3Palette[];
extern u16 gUnk_084BACD8[];
extern u16 gLevel2Stage5Room5Bg3Palette[];
extern u16 gUnk_084BAEE0[];
extern u16 gUnk_084BAFA4[];
extern u16 gUnk_084BB068[];
extern u16 gUnk_084BB16C[];
extern u16 gUnk_084BB270[];
extern u16 gLevel5Stage6Room0Bg2Palette[];
extern u16 gUnk_084BB438[];
extern u16 gUnk_084BB4FC[];
extern u16 gUnk_084BB5C0[];
extern u16 gUnk_084BB600[];
extern u16 gLevel7Stage0Room1Bg3Palette[];
extern u16 gUnk_084BB808[];
extern u16 gLevel2Stage6Room2Bg3Palette[];
extern u16 gLevel2Stage6Room0Bg3Palette[];
extern u16 gUnk_084CE684[];
extern u16 gUnk_084D50EC[];
extern u16 gLevel6Stage1Room4Bg3Palette[];
extern u16 gUnk_084D52B4[];
extern u16 gLevel6Stage1Room5Bg3Palette[];
extern u16 gUnk_084D54BC[];
extern u16 gLevel8Stage0Room2Bg3Palette[];
extern u16 gLevel8Stage1Room3Bg3Palette[];
extern u16 gLevel8Stage2Room3Bg3Palette[];
extern u16 gLevel8Stage3Room3Bg3Palette[];
extern u16 gLevel8Stage4Room3Bg3Palette[];
extern u16 gLevel8Stage5Room3Bg3Palette[];
extern u16 gUnk_084E55D8[];
extern u16 gUnk_084E56DC[];
extern u16 gLevel1Stage5Room0Bg3Palette[];
extern u16 gLevel8Stage7Room0Bg2Palette[];
extern u16 gLevel5Stage6Room0Bg3Palette[];
extern u16 gLevel4Stage2Room2Bg2Palette[];
extern u16 gLevel8Stage7Room0Bg3Palette[];
extern u16 gUnk_084FEE04[];
extern u16 gUnk_08505A1C[];
extern u16 gUnk_08505B20[];
extern u16 gUnk_0850A9A8[];
extern u16 gUnk_0850AA6C[];
extern u16 gUnk_0850AB70[];
extern u16 gUnk_08510B9C[];
extern u16 gUnk_08510CA0[];
extern u16 gUnk_08510DA4[];
extern u16 gLevel5Stage1Room0Bg2Palette[];
extern u16 gUnk_08510FAC[];
extern u16 gUnk_08511070[];
extern u16 gUnk_08511134[];
extern u16 gUnk_085111F8[];
extern u16 gUnk_085112BC[];
extern u16 gLevel3Stage3Room2Bg3Palette[];
extern u16 gUnk_08524FEC[];
extern u16 gUnk_0852BB78[];
extern u16 gLevel1Stage0Room1Bg2Palette[];
extern u16 gUnk_0852BD00[];
extern u16 gUnk_0852BDC4[];
extern u16 gUnk_0852BEC8[];
extern u16 gUnk_0852BFCC[];
extern u16 gUnk_0852C0D0[];
extern u16 gUnk_0852D088[];
extern u16 gUnk_0852D18C[];
extern u16 gUnk_0852D250[];
extern u16 gUnk_0852D314[];
extern u16 gUnk_0852D3D8[];
extern u16 gUnk_0852D49C[];
extern u16 gUnk_0852D560[];
extern u16 gUnk_0852D624[];
extern u16 gUnk_0852D6E8[];
extern u16 gUnk_0852D7AC[];
extern u16 gUnk_08539BA4[];
extern u16 gUnk_08539C88[];
extern u16 gUnk_08539D8C[];
extern u16 gUnk_0853C698[];
extern u16 gUnk_0853C6BC[];
extern u16 gUnk_0853C7C0[];
extern u16 gUnk_0853C8C4[];
extern u16 gUnk_0853C9C8[];
extern u16 gUnk_0853CACC[];
extern u16 gLevel4Stage0Room1Bg3Palette[];
extern u16 gLevel6Stage1Room2Bg3Palette[];
extern u16 gUnk_0853CDD8[];
extern u16 gUnk_0853CEDC[];
extern u16 gUnk_0853CFE0[];
extern u16 gUnk_0853D0E4[];
extern u16 gUnk_0853D108[];
extern u16 gUnk_0853D12C[];
extern u16 gUnk_0853D230[];
extern u16 gUnk_0853D334[];
extern u16 gUnk_0853D438[];
extern u16 gUnk_0853D53C[];
extern u16 gUnk_0853D600[];
extern u16 gUnk_0853D6C4[];
extern u16 gUnk_0853D7C8[];
extern u16 gUnk_0853D8CC[];
extern u16 gUnk_0853D9D0[];
extern u16 gUnk_0853DAB4[];
extern u16 gUnk_0853DBB8[];
extern u16 gLevel1Stage5Room0Bg2Palette[];
extern u16 gUnk_0853DD00[];
extern u16 gUnk_0853DE04[];
extern u16 gUnk_0853DF08[];
extern u16 gUnk_0853E00C[];
extern u16 gUnk_0853E110[];
extern u16 gUnk_0853E214[];
extern u16 gUnk_0853E318[];
extern u16 gUnk_0853E41C[];
extern u16 gUnk_0853E520[];
extern u16 gUnk_0853E624[];
extern u16 gUnk_0853E728[];
extern u16 gUnk_0853E7EC[];

/* bg2Tiles, bg3Tiles: LZ77 tiles (RequestCopy mode 8, the same functions) */
extern u8 gUnk_083D1380[];
extern u8 gUnk_083D1FD8[];
extern u8 gUnk_083D4E14[];
extern u8 gUnk_083DB044[];
extern u8 gUnk_083E0A24[];
extern u8 gUnk_083E2A30[];
extern u8 gUnk_083E6548[];
extern u8 gUnk_083EB890[];
extern u8 gUnk_083EE8B4[];
extern u8 gLevel6Stage1Room2Bg3Tiles[];
extern u8 gUnk_083F7470[];
extern u8 gUnk_083F9FC8[];
extern u8 gUnk_083FE694[];
extern u8 gUnk_08401D3C[];
extern u8 gUnk_08407278[];
extern u8 gUnk_0840CAAC[];
extern u8 gUnk_0840F3A8[];
extern u8 gUnk_0840F56C[];
extern u8 gUnk_0840F754[];
extern u8 gUnk_0840F99C[];
extern u8 gUnk_0840FB54[];
extern u8 gUnk_0840FCD8[];
extern u8 gUnk_08412BD8[];
extern u8 gLevel8Stage7Room0Bg2Tiles[];
extern u8 gUnk_0841AD98[];
extern u8 gUnk_08421338[];
extern u8 gUnk_084278D8[];
extern u8 gUnk_0842E1C0[];
extern u8 gUnk_08430A34[];
extern u8 gUnk_08431740[];
extern u8 gUnk_08432FCC[];
extern u8 gUnk_08434EFC[];
extern u8 gUnk_08436318[];
extern u8 gUnk_08437924[];
extern u8 gUnk_08439268[];
extern u8 gUnk_0843A824[];
extern u8 gUnk_0843D610[];
extern u8 gUnk_0843FC10[];
extern u8 gLevel1Stage5Room0Bg2Tiles[];
extern u8 gUnk_08442F18[];
extern u8 gUnk_0844619C[];
extern u8 gUnk_0844B2FC[];
extern u8 gUnk_0844F9D0[];
extern u8 gUnk_08454A70[];
extern u8 gUnk_0845A054[];
extern u8 gUnk_08460458[];
extern u8 gUnk_0846697C[];
extern u8 gUnk_0846CFD8[];
extern u8 gUnk_0846F8DC[];
extern u8 gUnk_08474C08[];
extern u8 gUnk_08479B7C[];
extern u8 gUnk_0847E4B4[];
extern u8 gUnk_084809DC[];
extern u8 gUnk_08487704[];
extern u8 gUnk_08489D58[];
extern u8 gUnk_0848B7C0[];
extern u8 gUnk_0849138C[];
extern u8 gUnk_084985B0[];
extern u8 gUnk_0849BE08[];
extern u8 gUnk_084A33B8[];
extern u8 gUnk_084A861C[];
extern u8 gUnk_084B0C9C[];
extern u8 gUnk_084B37B0[];
extern u8 gUnk_084B5F90[];
extern u8 gLevel4Stage6Room0Bg2Tiles[];
extern u8 gUnk_084BB5F4[];
extern u8 gUnk_084BFECC[];
extern u8 gLevel7Stage0Room1Bg3Tiles[];
extern u8 gUnk_084CB360[];
extern u8 gUnk_084CF0A8[];
extern u8 gUnk_084D8E30[];
extern u8 gUnk_084DEE4C[];
extern u8 gUnk_084E6184[];
extern u8 gUnk_084EB4F4[];
extern u8 gLevel1Stage5Room0Bg3Tiles[];
extern u8 gLevel5Stage6Room0Bg3Tiles[];
extern u8 gLevel8Stage7Room0Bg3Tiles[];
extern u8 gUnk_084FF824[];
extern u8 gUnk_08506F4C[];
extern u8 gUnk_0850B598[];
extern u8 gUnk_08511CDC[];
extern u8 gUnk_08518880[];
extern u8 gUnk_0851EF74[];
extern u8 gUnk_08525A0C[];
extern u8 gUnk_0852F948[];
extern u8 gUnk_08531C38[];
extern u8 gUnk_08534160[];
extern u8 gUnk_08536548[];

/* metatileTiles: LZ77 metatile tables (LoadRoom, src/level_2296c.c) */
extern u8 gUnk_083A862C[];
extern u8 gUnk_083A8B50[];
extern u8 gUnk_083AA000[];
extern u8 gUnk_083AAAB0[];
extern u8 gUnk_083AB444[];
extern u8 gLevel8Stage7Room0MetatileTiles[];
extern u8 gUnk_083ACDCC[];
extern u8 gUnk_083AD728[];
extern u8 gUnk_083ADC20[];
extern u8 gUnk_083AE5A4[];
extern u8 gUnk_083AEBB4[];
extern u8 gUnk_083AF1FC[];
extern u8 gUnk_083AF7E0[];
extern u8 gUnk_083AFFE4[];
extern u8 gUnk_083B0700[];
extern u8 gUnk_083B1058[];
extern u8 gUnk_083B18FC[];
extern u8 gLevel1Stage5Room0MetatileTiles[];
extern u8 gUnk_083B2254[];
extern u8 gUnk_083B282C[];
extern u8 gUnk_083B39C4[];
extern u8 gUnk_083B4738[];
extern u8 gUnk_083B4D78[];
extern u8 gUnk_08497D38[];
extern u8 gUnk_084AF93C[];
extern u8 gUnk_084B3060[];
extern u8 gUnk_084B58AC[];
extern u8 gLevel4Stage6Room0MetatileTiles[];
extern u8 gUnk_084BB5E4[];
extern u8 gUnk_084BB8CC[];
extern u8 gUnk_084E57E0[];
extern u8 gUnk_0852D870[];
extern u8 gUnk_0852E06C[];
extern u8 gUnk_0852E7CC[];
extern u8 gUnk_0852F03C[];

/* bg3Map: struct BgMap (SelectBg3MapShape and LoadBg3Map,
 * src/camera_28b8c.c:321-340; DrawBg3Tile, src/bgmap_2a9cc.c) */
extern struct BgMap gUnk_083B5538;
extern struct BgMap gUnk_083B5E54;
extern struct BgMap gUnk_083B6770;
extern struct BgMap gUnk_083B708C;
extern struct BgMap gUnk_083B7994;
extern struct BgMap gUnk_083B82B0;
extern struct BgMap gLevel6Stage1Room2Bg3Map;
extern struct BgMap gUnk_083B954C;
extern struct BgMap gUnk_083B9C18;
extern struct BgMap gUnk_083BA534;
extern struct BgMap gUnk_083BADEC;
extern struct BgMap gUnk_083BB70C;
extern struct BgMap gUnk_083BC030;
extern struct BgMap gUnk_083BD638;
extern struct BgMap gUnk_083BF8C0;
extern struct BgMap gUnk_083C0E48;
extern struct BgMap gUnk_083C2DD0;
extern struct BgMap gUnk_083C5058;
extern struct BgMap gUnk_083C72E0;
extern struct BgMap gUnk_083C89E8;
extern struct BgMap gUnk_083C93D8;
extern struct BgMap gUnk_083C9CFC;
extern struct BgMap gUnk_083CA620;
extern struct BgMap gUnk_083CAF40;
extern struct BgMap gUnk_083CB85C;
extern struct BgMap gUnk_083CC17C;
extern struct BgMap gUnk_083CCA9C;
extern struct BgMap gUnk_083CD3B8;
extern struct BgMap gUnk_083CDCD4;
extern struct BgMap gUnk_083CE5F4;
extern struct BgMap gUnk_083CEF10;
extern struct BgMap gUnk_083CF82C;
extern struct BgMap gUnk_083D0148;
extern struct BgMap gUnk_083D0A64;
extern struct BgMap gUnk_0848AEA4;
extern struct BgMap gUnk_08490A70;
extern struct BgMap gUnk_0849ABCC;
extern struct BgMap gUnk_0849B4E8;
extern struct BgMap gUnk_084A7CFC;
extern struct BgMap gLevel7Stage0Room0Bg3Map;
extern struct BgMap gLevel7Stage0Room1Bg3Map;
extern struct BgMap gUnk_084CE788;
extern struct BgMap gLevel8Stage0Room2Bg3Map;
extern struct BgMap gLevel8Stage1Room3Bg3Map;
extern struct BgMap gLevel8Stage2Room3Bg3Map;
extern struct BgMap gLevel8Stage3Room3Bg3Map;
extern struct BgMap gLevel8Stage4Room3Bg3Map;
extern struct BgMap gLevel8Stage5Room3Bg3Map;
extern struct BgMap gUnk_084E5928;
extern struct BgMap gLevel1Stage5Room0Bg3Map;
extern struct BgMap gLevel5Stage6Room0Bg3Map;
extern struct BgMap gLevel8Stage7Room0Bg3Map;
extern struct BgMap gUnk_084FEF08;
extern struct BgMap gUnk_08505C24;
extern struct BgMap gUnk_0850AC74;
extern struct BgMap gUnk_085113C0;
extern struct BgMap gUnk_085175E4;
extern struct BgMap gUnk_08517F00;
extern struct BgMap gUnk_085250F0;
extern struct BgMap gUnk_0852C1D4;
extern struct BgMap gUnk_0852C930;
extern struct BgMap gLevel8Stage7Room1Bg3Map;
extern struct BgMap gLevel7Stage0Room2Bg3Map;

/* gRoomTable[7][1][0], 0x0835D08C, section .room_def_0835d08c */
extern u8 gLevel7Stage1Room0MetatileMap[];
extern u8 gLevel7Stage1Room0BlockLayer[];
extern u8 gLevel7Stage1Room0BlockMetatiles[];
extern u8 gLevel7Stage1Room0Objects[];
struct RoomDef gLevel7Stage1Room0 ROOM_DEF(0835d08c) = {
    .filler00 = { 7, 1, 0, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage1Room0MetatileMap,
    .blockLayer = gLevel7Stage1Room0BlockLayer,
    .blockMetatiles = gLevel7Stage1Room0BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = gLevel7Stage1Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[7][1][1], 0x0835D300, section .room_def_0835d300 */
extern u8 gLevel7Stage1Room1MetatileMap[];
extern u8 gLevel7Stage1Room1BlockLayer[];
extern u8 gLevel7Stage1Room1BlockMetatiles[];
extern u8 gLevel7Stage1Room1Objects[];
struct RoomDef gLevel7Stage1Room1 ROOM_DEF(0835d300) = {
    .filler00 = { 7, 1, 1, 0 },
    .bgm = 30,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage1Room1MetatileMap,
    .blockLayer = gLevel7Stage1Room1BlockLayer,
    .blockMetatiles = gLevel7Stage1Room1BlockMetatiles,
    .width = 16,
    .height = 23,
    .bg2Palette = gUnk_0835D918,
    .bg2Tiles = gUnk_08487704,
    .metatileTiles = gUnk_083B4738,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D814,
    .bg3Tiles = gUnk_084809DC,
    .bg3Map = &gUnk_083D0A64,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = gLevel7Stage1Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 328,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 5,
};

/* gRoomTable[7][1][2], 0x0835D4F0, section .room_def_0835d4f0 */
extern u8 gLevel7Stage1Room2MetatileMap[];
extern u8 gLevel7Stage1Room2BlockLayer[];
extern u8 gLevel7Stage1Room2BlockMetatiles[];
extern u8 gLevel7Stage1Room2Objects[];
struct RoomDef gLevel7Stage1Room2 ROOM_DEF(0835d4f0) = {
    .filler00 = { 7, 1, 2, 0 },
    .bgm = 27,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage1Room2MetatileMap,
    .blockLayer = gLevel7Stage1Room2BlockLayer,
    .blockMetatiles = gLevel7Stage1Room2BlockMetatiles,
    .width = 18,
    .height = 21,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 24,
    .borderY = 8,
    .bg3Palette = gUnk_0835D710,
    .bg3Tiles = gUnk_08412BD8,
    .bg3Map = &gUnk_083C89E8,
    .bg3BorderX = 8,
    .bg3BorderY = 16,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = gLevel7Stage1Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 48,
    .entryY = 296,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 5,
};

/* gRoomTable[0][0][0], 0x0835DDC0, section .room_def_0835ddc0 */
extern u8 gLevel0Stage0Room0MetatileMap[];
extern u8 gLevel0Stage0Room0BlockLayer[];
extern u8 gLevel0Stage0Room0BlockMetatiles[];
extern struct Door gLevel0Stage0Room0Doors[];
extern u8 gLevel0Stage0Room0Objects[];
struct RoomDef gLevel0Stage0Room0 ROOM_DEF(0835ddc0) = {
    .filler00 = { 0, 0, 0, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage0Room0MetatileMap,
    .blockLayer = gLevel0Stage0Room0BlockLayer,
    .blockMetatiles = gLevel0Stage0Room0BlockMetatiles,
    .width = 64,
    .height = 11,
    .bg2Palette = gUnk_0849A940,
    .bg2Tiles = gUnk_08442F18,
    .metatileTiles = gUnk_083B2254,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DD00,
    .bg3Tiles = gUnk_0844619C,
    .bg3Map = &gUnk_083CAF40,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 11,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel0Stage0Room0Doors,
    .objects = gLevel0Stage0Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 41,
    .entryY = 57,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][0][1], 0x0835E2A0, section .room_def_0835e2a0 */
extern u8 gLevel0Stage0Room1MetatileMap[];
extern u8 gLevel0Stage0Room1BlockLayer[];
extern u8 gLevel0Stage0Room1BlockMetatiles[];
extern struct Door gLevel0Stage0Room1Doors[];
extern u8 gLevel0Stage0Room1Objects[];
struct RoomDef gLevel0Stage0Room1 ROOM_DEF(0835e2a0) = {
    .filler00 = { 0, 0, 1, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage0Room1MetatileMap,
    .blockLayer = gLevel0Stage0Room1BlockLayer,
    .blockMetatiles = gLevel0Stage0Room1BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gUnk_0853DAB4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0849AAC8,
    .bg3Tiles = gUnk_084A33B8,
    .bg3Map = &gUnk_0849B4E8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 11,
    .objectsSortedByY = 0,
    .bgAnimSet = 1,
    .unk42 = 0,
    .doors = gLevel0Stage0Room1Doors,
    .objects = gLevel0Stage0Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][0][2], 0x0835E70C, section .room_def_0835e70c */
extern u8 gLevel0Stage0Room2MetatileMap[];
extern u8 gLevel0Stage0Room2BlockLayer[];
extern u8 gLevel0Stage0Room2BlockMetatiles[];
extern struct Door gLevel0Stage0Room2Doors[];
extern u8 gLevel0Stage0Room2Objects[];
struct RoomDef gLevel0Stage0Room2 ROOM_DEF(0835e70c) = {
    .filler00 = { 0, 0, 2, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage0Room2MetatileMap,
    .blockLayer = gLevel0Stage0Room2BlockLayer,
    .blockMetatiles = gLevel0Stage0Room2BlockMetatiles,
    .width = 64,
    .height = 11,
    .bg2Palette = gUnk_0853DAB4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849AAC8,
    .bg3Tiles = gUnk_084A33B8,
    .bg3Map = &gUnk_0849B4E8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 12,
    .objectsSortedByY = 0,
    .bgAnimSet = 1,
    .unk42 = 0,
    .doors = gLevel0Stage0Room2Doors,
    .objects = gLevel0Stage0Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 120,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][0][3], 0x0835E924, section .room_def_0835e924 */
extern u8 gLevel0Stage0Room3MetatileMap[];
extern u8 gLevel0Stage0Room3BlockLayer[];
extern u8 gLevel0Stage0Room3BlockMetatiles[];
extern struct Door gLevel0Stage0Room3Doors[];
extern u8 gLevel0Stage0Room3Objects[];
struct RoomDef gLevel0Stage0Room3 ROOM_DEF(0835e924) = {
    .filler00 = { 0, 0, 3, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage0Room3MetatileMap,
    .blockLayer = gLevel0Stage0Room3BlockLayer,
    .blockMetatiles = gLevel0Stage0Room3BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E41C,
    .bg3Tiles = gUnk_0846F8DC,
    .bg3Map = &gUnk_083CEF10,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel0Stage0Room3Doors,
    .objects = gLevel0Stage0Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 136,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][1][0], 0x0835EF1C, section .room_def_0835ef1c */
extern u8 gLevel0Stage1Room0MetatileMap[];
extern u8 gLevel0Stage1Room0BlockLayer[];
extern u8 gLevel0Stage1Room0BlockMetatiles[];
extern struct Door gLevel0Stage1Room0Doors[];
extern u8 gLevel0Stage1Room0Objects[];
struct RoomDef gLevel0Stage1Room0 ROOM_DEF(0835ef1c) = {
    .filler00 = { 0, 1, 0, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage1Room0MetatileMap,
    .blockLayer = gLevel0Stage1Room0BlockLayer,
    .blockMetatiles = gLevel0Stage1Room0BlockMetatiles,
    .width = 95,
    .height = 11,
    .bg2Palette = gUnk_0849A940,
    .bg2Tiles = gUnk_08442F18,
    .metatileTiles = gUnk_083B2254,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0852BDC4,
    .bg3Tiles = gUnk_0846697C,
    .bg3Map = &gUnk_083CE5F4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 10,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel0Stage1Room0Doors,
    .objects = gLevel0Stage1Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 48,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][1][1], 0x0835F1A4, section .room_def_0835f1a4 */
extern u8 gLevel0Stage1Room1MetatileMap[];
extern u8 gLevel0Stage1Room1BlockLayer[];
extern u8 gLevel0Stage1Room1BlockMetatiles[];
extern struct Door gLevel0Stage1Room1Doors[];
extern u8 gLevel0Stage1Room1Objects[];
struct RoomDef gLevel0Stage1Room1 ROOM_DEF(0835f1a4) = {
    .filler00 = { 0, 1, 1, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage1Room1MetatileMap,
    .blockLayer = gLevel0Stage1Room1BlockLayer,
    .blockMetatiles = gLevel0Stage1Room1BlockMetatiles,
    .width = 16,
    .height = 33,
    .bg2Palette = gUnk_0849A940,
    .bg2Tiles = gUnk_08442F18,
    .metatileTiles = gUnk_083B2254,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849AAC8,
    .bg3Tiles = gUnk_084A33B8,
    .bg3Map = &gUnk_0849B4E8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 2,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel0Stage1Room1Doors,
    .objects = gLevel0Stage1Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 56,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][1][2], 0x0835F544, section .room_def_0835f544 */
extern u8 gLevel0Stage1Room2MetatileMap[];
extern u8 gLevel0Stage1Room2BlockLayer[];
extern u8 gLevel0Stage1Room2BlockMetatiles[];
extern struct Door gLevel0Stage1Room2Doors[];
extern u8 gLevel0Stage1Room2Objects[];
struct RoomDef gLevel0Stage1Room2 ROOM_DEF(0835f544) = {
    .filler00 = { 0, 1, 2, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage1Room2MetatileMap,
    .blockLayer = gLevel0Stage1Room2BlockLayer,
    .blockMetatiles = gLevel0Stage1Room2BlockMetatiles,
    .width = 48,
    .height = 11,
    .bg2Palette = gUnk_0853DAB4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849AAC8,
    .bg3Tiles = gUnk_084A33B8,
    .bg3Map = &gUnk_0849B4E8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 7,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel0Stage1Room2Doors,
    .objects = gLevel0Stage1Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 8,
    .entryY = 24,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][1][3], 0x0835FC24, section .room_def_0835fc24 */
extern u8 gLevel0Stage1Room3MetatileMap[];
extern u8 gLevel0Stage1Room3BlockLayer[];
extern u8 gLevel0Stage1Room3BlockMetatiles[];
extern struct Door gLevel0Stage1Room3Doors[];
extern u8 gLevel0Stage1Room3Objects[];
struct RoomDef gLevel0Stage1Room3 ROOM_DEF(0835fc24) = {
    .filler00 = { 0, 1, 3, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage1Room3MetatileMap,
    .blockLayer = gLevel0Stage1Room3BlockLayer,
    .blockMetatiles = gLevel0Stage1Room3BlockMetatiles,
    .width = 64,
    .height = 11,
    .bg2Palette = gUnk_0849A52C,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C6BC,
    .bg3Tiles = gUnk_083D4E14,
    .bg3Map = &gUnk_083B5538,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 15,
    .objectsSortedByY = 0,
    .bgAnimSet = 2,
    .unk42 = 0,
    .doors = gLevel0Stage1Room3Doors,
    .objects = gLevel0Stage1Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 8,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][1][4], 0x0835FFE4, section .room_def_0835ffe4 */
extern u8 gLevel0Stage1Room4MetatileMap[];
extern u8 gLevel0Stage1Room4BlockLayer[];
extern u8 gLevel0Stage1Room4BlockMetatiles[];
extern struct Door gLevel0Stage1Room4Doors[];
extern u8 gLevel0Stage1Room4Objects[];
struct RoomDef gLevel0Stage1Room4 ROOM_DEF(0835ffe4) = {
    .filler00 = { 0, 1, 4, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage1Room4MetatileMap,
    .blockLayer = gLevel0Stage1Room4BlockLayer,
    .blockMetatiles = gLevel0Stage1Room4BlockMetatiles,
    .width = 16,
    .height = 24,
    .bg2Palette = gUnk_0849A52C,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C6BC,
    .bg3Tiles = gUnk_083D4E14,
    .bg3Map = &gUnk_083B5538,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 3,
    .objectCount = 5,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel0Stage1Room4Doors,
    .objects = gLevel0Stage1Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 328,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][1][5], 0x083601DC, section .room_def_083601dc */
extern u8 gLevel0Stage1Room5MetatileMap[];
extern u8 gLevel0Stage1Room5BlockLayer[];
extern u8 gLevel0Stage1Room5BlockMetatiles[];
extern struct Door gLevel0Stage1Room5Doors[];
extern u8 gLevel0Stage1Room5Objects[];
struct RoomDef gLevel0Stage1Room5 ROOM_DEF(083601dc) = {
    .filler00 = { 0, 1, 5, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage1Room5MetatileMap,
    .blockLayer = gLevel0Stage1Room5BlockLayer,
    .blockMetatiles = gLevel0Stage1Room5BlockMetatiles,
    .width = 16,
    .height = 13,
    .bg2Palette = gUnk_084BB068,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853C6BC,
    .bg3Tiles = gUnk_083D4E14,
    .bg3Map = &gUnk_083B5538,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 1,
    .unk42 = 0,
    .doors = gLevel0Stage1Room5Doors,
    .objects = gLevel0Stage1Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 184,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][2][0], 0x08360690, section .room_def_08360690 */
extern u8 gLevel0Stage2Room0MetatileMap[];
extern u8 gLevel0Stage2Room0BlockLayer[];
extern u8 gLevel0Stage2Room0BlockMetatiles[];
extern struct Door gLevel0Stage2Room0Doors[];
extern u8 gLevel0Stage2Room0Objects[];
struct RoomDef gLevel0Stage2Room0 ROOM_DEF(08360690) = {
    .filler00 = { 0, 2, 0, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage2Room0MetatileMap,
    .blockLayer = gLevel0Stage2Room0BlockLayer,
    .blockMetatiles = gLevel0Stage2Room0BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gUnk_0849A940,
    .bg2Tiles = gUnk_08442F18,
    .metatileTiles = gUnk_083B2254,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853CEDC,
    .bg3Tiles = gUnk_08401D3C,
    .bg3Map = &gUnk_083BADEC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 12,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel0Stage2Room0Doors,
    .objects = gLevel0Stage2Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 56,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][2][1], 0x08360B38, section .room_def_08360b38 */
extern u8 gLevel0Stage2Room1MetatileMap[];
extern u8 gLevel0Stage2Room1BlockLayer[];
extern u8 gLevel0Stage2Room1BlockMetatiles[];
extern struct Door gLevel0Stage2Room1Doors[];
extern u8 gLevel0Stage2Room1Objects[];
struct RoomDef gLevel0Stage2Room1 ROOM_DEF(08360b38) = {
    .filler00 = { 0, 2, 1, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage2Room1MetatileMap,
    .blockLayer = gLevel0Stage2Room1BlockLayer,
    .blockMetatiles = gLevel0Stage2Room1BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gUnk_0853DAB4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853CEDC,
    .bg3Tiles = gUnk_08401D3C,
    .bg3Map = &gUnk_083BADEC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 13,
    .objectsSortedByY = 0,
    .bgAnimSet = 1,
    .unk42 = 0,
    .doors = gLevel0Stage2Room1Doors,
    .objects = gLevel0Stage2Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][2][2], 0x08360F38, section .room_def_08360f38 */
extern u8 gLevel0Stage2Room2MetatileMap[];
extern u8 gLevel0Stage2Room2BlockLayer[];
extern u8 gLevel0Stage2Room2BlockMetatiles[];
extern struct Door gLevel0Stage2Room2Doors[];
extern u8 gLevel0Stage2Room2Objects[];
struct RoomDef gLevel0Stage2Room2 ROOM_DEF(08360f38) = {
    .filler00 = { 0, 2, 2, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage2Room2MetatileMap,
    .blockLayer = gLevel0Stage2Room2BlockLayer,
    .blockMetatiles = gLevel0Stage2Room2BlockMetatiles,
    .width = 18,
    .height = 25,
    .bg2Palette = gUnk_084D50EC,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 24,
    .borderY = 8,
    .bg3Palette = gUnk_0853CEDC,
    .bg3Tiles = gUnk_08401D3C,
    .bg3Map = &gUnk_083BADEC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 7,
    .objectsSortedByY = 1,
    .bgAnimSet = 2,
    .unk42 = 0,
    .doors = gLevel0Stage2Room2Doors,
    .objects = gLevel0Stage2Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 56,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][2][3], 0x083613A0, section .room_def_083613a0 */
extern u8 gLevel0Stage2Room3MetatileMap[];
extern u8 gLevel0Stage2Room3BlockLayer[];
extern u8 gLevel0Stage2Room3BlockMetatiles[];
extern struct Door gLevel0Stage2Room3Doors[];
extern u8 gLevel0Stage2Room3Objects[];
struct RoomDef gLevel0Stage2Room3 ROOM_DEF(083613a0) = {
    .filler00 = { 0, 2, 3, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage2Room3MetatileMap,
    .blockLayer = gLevel0Stage2Room3BlockLayer,
    .blockMetatiles = gLevel0Stage2Room3BlockMetatiles,
    .width = 66,
    .height = 11,
    .bg2Palette = gUnk_0853DAB4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CEDC,
    .bg3Tiles = gUnk_08401D3C,
    .bg3Map = &gUnk_083BADEC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 14,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel0Stage2Room3Doors,
    .objects = gLevel0Stage2Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][3][0], 0x0836179C, section .room_def_0836179c */
extern u8 gLevel0Stage3Room0MetatileMap[];
extern u8 gLevel0Stage3Room0BlockLayer[];
extern u8 gLevel0Stage3Room0BlockMetatiles[];
extern struct Door gLevel0Stage3Room0Doors[];
extern u8 gLevel0Stage3Room0Objects[];
struct RoomDef gLevel0Stage3Room0 ROOM_DEF(0836179c) = {
    .filler00 = { 0, 3, 0, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage3Room0MetatileMap,
    .blockLayer = gLevel0Stage3Room0BlockLayer,
    .blockMetatiles = gLevel0Stage3Room0BlockMetatiles,
    .width = 17,
    .height = 22,
    .bg2Palette = gUnk_0853D12C,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0835D814,
    .bg3Tiles = gUnk_084809DC,
    .bg3Map = &gUnk_083D0A64,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 5,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel0Stage3Room0Doors,
    .objects = gLevel0Stage3Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 199,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][3][1], 0x08361CB8, section .room_def_08361cb8 */
extern u8 gLevel0Stage3Room1MetatileMap[];
extern u8 gLevel0Stage3Room1BlockLayer[];
extern u8 gLevel0Stage3Room1BlockMetatiles[];
extern struct Door gLevel0Stage3Room1Doors[];
extern u8 gLevel0Stage3Room1Objects[];
struct RoomDef gLevel0Stage3Room1 ROOM_DEF(08361cb8) = {
    .filler00 = { 0, 3, 1, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage3Room1MetatileMap,
    .blockLayer = gLevel0Stage3Room1BlockLayer,
    .blockMetatiles = gLevel0Stage3Room1BlockMetatiles,
    .width = 64,
    .height = 11,
    .bg2Palette = gUnk_0853D12C,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D814,
    .bg3Tiles = gUnk_084809DC,
    .bg3Map = &gUnk_083D0A64,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 18,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel0Stage3Room1Doors,
    .objects = gLevel0Stage3Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 71,
    .entryY = 90,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][3][2], 0x08362104, section .room_def_08362104 */
extern u8 gLevel0Stage3Room2MetatileMap[];
extern u8 gLevel0Stage3Room2BlockLayer[];
extern u8 gLevel0Stage3Room2BlockMetatiles[];
extern struct Door gLevel0Stage3Room2Doors[];
extern u8 gLevel0Stage3Room2Objects[];
struct RoomDef gLevel0Stage3Room2 ROOM_DEF(08362104) = {
    .filler00 = { 0, 3, 2, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage3Room2MetatileMap,
    .blockLayer = gLevel0Stage3Room2BlockLayer,
    .blockMetatiles = gLevel0Stage3Room2BlockMetatiles,
    .width = 16,
    .height = 35,
    .bg2Palette = gUnk_0853E728,
    .bg2Tiles = gUnk_0847E4B4,
    .metatileTiles = gUnk_083B39C4,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D814,
    .bg3Tiles = gUnk_084809DC,
    .bg3Map = &gUnk_083D0A64,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 9,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel0Stage3Room2Doors,
    .objects = gLevel0Stage3Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 104,
    .entryY = 504,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][3][3], 0x08362788, section .room_def_08362788 */
extern u8 gLevel0Stage3Room3MetatileMap[];
extern u8 gLevel0Stage3Room3BlockLayer[];
extern u8 gLevel0Stage3Room3BlockMetatiles[];
extern struct Door gLevel0Stage3Room3Doors[];
extern u8 gLevel0Stage3Room3Objects[];
struct RoomDef gLevel0Stage3Room3 ROOM_DEF(08362788) = {
    .filler00 = { 0, 3, 3, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage3Room3MetatileMap,
    .blockLayer = gLevel0Stage3Room3BlockLayer,
    .blockMetatiles = gLevel0Stage3Room3BlockMetatiles,
    .width = 82,
    .height = 13,
    .bg2Palette = gUnk_0853D12C,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0835D814,
    .bg3Tiles = gUnk_084809DC,
    .bg3Map = &gUnk_083D0A64,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 19,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel0Stage3Room3Doors,
    .objects = gLevel0Stage3Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 120,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[0][4][0], 0x08362AD0, section .room_def_08362ad0 */
extern u8 gLevel0Stage4Room0MetatileMap[];
extern u8 gLevel0Stage4Room0BlockLayer[];
extern u8 gLevel0Stage4Room0BlockMetatiles[];
extern u8 gLevel0Stage4Room0Objects[];
struct RoomDef gLevel0Stage4Room0 ROOM_DEF(08362ad0) = {
    .filler00 = { 0, 4, 0, 0 },
    .bgm = 30,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel0Stage4Room0MetatileMap,
    .blockLayer = gLevel0Stage4Room0BlockLayer,
    .blockMetatiles = gLevel0Stage4Room0BlockMetatiles,
    .width = 16,
    .height = 23,
    .bg2Palette = gUnk_0835D918,
    .bg2Tiles = gUnk_08487704,
    .metatileTiles = gUnk_083B4738,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D814,
    .bg3Tiles = gUnk_084809DC,
    .bg3Map = &gUnk_083D0A64,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = gLevel0Stage4Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 5,
};

/* gRoomTable[1][0][0], 0x083630A0, section .room_def_083630a0 */
extern u8 gLevel1Stage0Room0MetatileMap[];
extern u8 gLevel1Stage0Room0BlockLayer[];
extern u8 gLevel1Stage0Room0BlockMetatiles[];
extern struct Door gLevel1Stage0Room0Doors[];
extern u8 gLevel1Stage0Room0Objects[];
struct RoomDef gLevel1Stage0Room0 ROOM_DEF(083630a0) = {
    .filler00 = { 1, 0, 0, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage0Room0MetatileMap,
    .blockLayer = gLevel1Stage0Room0BlockLayer,
    .blockMetatiles = gLevel1Stage0Room0BlockMetatiles,
    .width = 80,
    .height = 13,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E00C,
    .bg3Tiles = gUnk_08454A70,
    .bg3Map = &gUnk_083CCA9C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 15,
    .objectsSortedByY = 0,
    .bgAnimSet = 3,
    .unk42 = 0,
    .doors = gLevel1Stage0Room0Doors,
    .objects = gLevel1Stage0Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 74,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][0][1], 0x083634E0, section .room_def_083634e0 */
extern u8 gLevel1Stage0Room1MetatileMap[];
extern u8 gLevel1Stage0Room1BlockLayer[];
extern u8 gLevel1Stage0Room1BlockMetatiles[];
extern struct Door gLevel1Stage0Room1Doors[];
extern u8 gLevel1Stage0Room1Objects[];
struct RoomDef gLevel1Stage0Room1 ROOM_DEF(083634e0) = {
    .filler00 = { 1, 0, 1, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage0Room1MetatileMap,
    .blockLayer = gLevel1Stage0Room1BlockLayer,
    .blockMetatiles = gLevel1Stage0Room1BlockMetatiles,
    .width = 64,
    .height = 11,
    .bg2Palette = gLevel1Stage0Room1Bg2Palette,
    .bg2Tiles = gUnk_08442F18,
    .metatileTiles = gUnk_083B2254,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E00C,
    .bg3Tiles = gUnk_08454A70,
    .bg3Map = &gUnk_083CCA9C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 12,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage0Room1Doors,
    .objects = gLevel1Stage0Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 42,
    .entryY = 106,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][0][2], 0x08363A0C, section .room_def_08363a0c */
extern u8 gLevel1Stage0Room2MetatileMap[];
extern u8 gLevel1Stage0Room2BlockLayer[];
extern u8 gLevel1Stage0Room2BlockMetatiles[];
extern struct Door gLevel1Stage0Room2Doors[];
extern u8 gLevel1Stage0Room2Objects[];
struct RoomDef gLevel1Stage0Room2 ROOM_DEF(08363a0c) = {
    .filler00 = { 1, 0, 2, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage0Room2MetatileMap,
    .blockLayer = gLevel1Stage0Room2BlockLayer,
    .blockMetatiles = gLevel1Stage0Room2BlockMetatiles,
    .width = 63,
    .height = 11,
    .bg2Palette = gUnk_0853D12C,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E00C,
    .bg3Tiles = gUnk_08454A70,
    .bg3Map = &gUnk_083CCA9C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 15,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage0Room2Doors,
    .objects = gLevel1Stage0Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][0][3], 0x08363E04, section .room_def_08363e04 */
extern u8 gLevel1Stage0Room3MetatileMap[];
extern u8 gLevel1Stage0Room3BlockLayer[];
extern u8 gLevel1Stage0Room3BlockMetatiles[];
extern struct Door gLevel1Stage0Room3Doors[];
extern u8 gLevel1Stage0Room3Objects[];
struct RoomDef gLevel1Stage0Room3 ROOM_DEF(08363e04) = {
    .filler00 = { 1, 0, 3, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage0Room3MetatileMap,
    .blockLayer = gLevel1Stage0Room3BlockLayer,
    .blockMetatiles = gLevel1Stage0Room3BlockMetatiles,
    .width = 16,
    .height = 23,
    .bg2Palette = gUnk_0849A52C,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C6BC,
    .bg3Tiles = gUnk_083D4E14,
    .bg3Map = &gUnk_083B5538,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 4,
    .objectsSortedByY = 1,
    .bgAnimSet = 2,
    .unk42 = 0,
    .doors = gLevel1Stage0Room3Doors,
    .objects = gLevel1Stage0Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 216,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][1][0], 0x08364014, section .room_def_08364014 */
extern u8 gLevel1Stage1Room0MetatileMap[];
extern u8 gLevel1Stage1Room0BlockLayer[];
extern u8 gLevel1Stage1Room0BlockMetatiles[];
extern struct Door gLevel1Stage1Room0Doors[];
extern u8 gLevel1Stage1Room0Objects[];
struct RoomDef gLevel1Stage1Room0 ROOM_DEF(08364014) = {
    .filler00 = { 1, 1, 0, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage1Room0MetatileMap,
    .blockLayer = gLevel1Stage1Room0BlockLayer,
    .blockMetatiles = gLevel1Stage1Room0BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853E728,
    .bg2Tiles = gUnk_0847E4B4,
    .metatileTiles = gUnk_083B39C4,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DF08,
    .bg3Tiles = gUnk_0844F9D0,
    .bg3Map = &gUnk_083CC17C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 4,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage1Room0Doors,
    .objects = gLevel1Stage1Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 58,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][1][1], 0x083643BC, section .room_def_083643bc */
extern u8 gLevel1Stage1Room1MetatileMap[];
extern u8 gLevel1Stage1Room1BlockLayer[];
extern u8 gLevel1Stage1Room1BlockMetatiles[];
extern struct Door gLevel1Stage1Room1Doors[];
extern u8 gLevel1Stage1Room1Objects[];
struct RoomDef gLevel1Stage1Room1 ROOM_DEF(083643bc) = {
    .filler00 = { 1, 1, 1, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage1Room1MetatileMap,
    .blockLayer = gLevel1Stage1Room1BlockLayer,
    .blockMetatiles = gLevel1Stage1Room1BlockMetatiles,
    .width = 33,
    .height = 11,
    .bg2Palette = gUnk_0849A6B4,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DF08,
    .bg3Tiles = gUnk_0844F9D0,
    .bg3Map = &gUnk_083CC17C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 3,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage1Room1Doors,
    .objects = gLevel1Stage1Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 55,
    .entryY = 121,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][1][2], 0x0836463C, section .room_def_0836463c */
extern u8 gLevel1Stage1Room2MetatileMap[];
extern u8 gLevel1Stage1Room2BlockLayer[];
extern u8 gLevel1Stage1Room2BlockMetatiles[];
extern struct Door gLevel1Stage1Room2Doors[];
extern u8 gLevel1Stage1Room2Objects[];
struct RoomDef gLevel1Stage1Room2 ROOM_DEF(0836463c) = {
    .filler00 = { 1, 1, 2, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage1Room2MetatileMap,
    .blockLayer = gLevel1Stage1Room2BlockLayer,
    .blockMetatiles = gLevel1Stage1Room2BlockMetatiles,
    .width = 16,
    .height = 32,
    .bg2Palette = gUnk_0849A6B4,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DF08,
    .bg3Tiles = gUnk_0844F9D0,
    .bg3Map = &gUnk_083CC17C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage1Room2Doors,
    .objects = gLevel1Stage1Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 26,
    .entryY = 440,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][1][3], 0x08364A38, section .room_def_08364a38 */
extern u8 gLevel1Stage1Room3MetatileMap[];
extern u8 gLevel1Stage1Room3BlockLayer[];
extern u8 gLevel1Stage1Room3BlockMetatiles[];
extern struct Door gLevel1Stage1Room3Doors[];
extern u8 gLevel1Stage1Room3Objects[];
struct RoomDef gLevel1Stage1Room3 ROOM_DEF(08364a38) = {
    .filler00 = { 1, 1, 3, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage1Room3MetatileMap,
    .blockLayer = gLevel1Stage1Room3BlockLayer,
    .blockMetatiles = gLevel1Stage1Room3BlockMetatiles,
    .width = 52,
    .height = 11,
    .bg2Palette = gUnk_0850A9A8,
    .bg2Tiles = gUnk_0840CAAC,
    .metatileTiles = gUnk_083AAAB0,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DE04,
    .bg3Tiles = gUnk_0844B2FC,
    .bg3Map = &gUnk_083CB85C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 4,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage1Room3Doors,
    .objects = gLevel1Stage1Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 8,
    .entryY = 8,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][1][4], 0x08365558, section .room_def_08365558 */
extern u8 gLevel1Stage1Room4MetatileMap[];
extern u8 gLevel1Stage1Room4BlockLayer[];
extern u8 gLevel1Stage1Room4BlockMetatiles[];
extern struct Door gLevel1Stage1Room4Doors[];
extern u8 gLevel1Stage1Room4Objects[];
struct RoomDef gLevel1Stage1Room4 ROOM_DEF(08365558) = {
    .filler00 = { 1, 1, 4, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage1Room4MetatileMap,
    .blockLayer = gLevel1Stage1Room4BlockLayer,
    .blockMetatiles = gLevel1Stage1Room4BlockMetatiles,
    .width = 100,
    .height = 23,
    .bg2Palette = gUnk_0850A9A8,
    .bg2Tiles = gUnk_0840CAAC,
    .metatileTiles = gUnk_083AAAB0,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853DE04,
    .bg3Tiles = gUnk_0844B2FC,
    .bg3Map = &gUnk_083CB85C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 24,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage1Room4Doors,
    .objects = gLevel1Stage1Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 312,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][2][0], 0x083657C0, section .room_def_083657c0 */
extern u8 gLevel1Stage2Room0MetatileMap[];
extern u8 gLevel1Stage2Room0BlockLayer[];
extern u8 gLevel1Stage2Room0BlockMetatiles[];
extern struct Door gLevel1Stage2Room0Doors[];
extern u8 gLevel1Stage2Room0Objects[];
struct RoomDef gLevel1Stage2Room0 ROOM_DEF(083657c0) = {
    .filler00 = { 1, 2, 0, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage2Room0MetatileMap,
    .blockLayer = gLevel1Stage2Room0BlockLayer,
    .blockMetatiles = gLevel1Stage2Room0BlockMetatiles,
    .width = 32,
    .height = 13,
    .bg2Palette = gUnk_0849A52C,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E110,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 4,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage2Room0Doors,
    .objects = gLevel1Stage2Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 41,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][2][1], 0x08366340, section .room_def_08366340 */
extern u8 gLevel1Stage2Room1MetatileMap[];
extern u8 gLevel1Stage2Room1BlockLayer[];
extern u8 gLevel1Stage2Room1BlockMetatiles[];
extern struct Door gLevel1Stage2Room1Doors[];
extern u8 gLevel1Stage2Room1Objects[];
struct RoomDef gLevel1Stage2Room1 ROOM_DEF(08366340) = {
    .filler00 = { 1, 2, 1, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage2Room1MetatileMap,
    .blockLayer = gLevel1Stage2Room1BlockLayer,
    .blockMetatiles = gLevel1Stage2Room1BlockMetatiles,
    .width = 49,
    .height = 26,
    .bg2Palette = gUnk_0849A52C,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853C6BC,
    .bg3Tiles = gUnk_083D4E14,
    .bg3Map = &gUnk_083B5538,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 4,
    .objectCount = 12,
    .objectsSortedByY = 0,
    .bgAnimSet = 2,
    .unk42 = 0,
    .doors = gLevel1Stage2Room1Doors,
    .objects = gLevel1Stage2Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 377,
    .entryY = 328,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][2][2], 0x0836663C, section .room_def_0836663c */
extern u8 gLevel1Stage2Room2MetatileMap[];
extern u8 gLevel1Stage2Room2BlockLayer[];
extern u8 gLevel1Stage2Room2BlockMetatiles[];
extern struct Door gLevel1Stage2Room2Doors[];
extern u8 gLevel1Stage2Room2Objects[];
struct RoomDef gLevel1Stage2Room2 ROOM_DEF(0836663c) = {
    .filler00 = { 1, 2, 2, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage2Room2MetatileMap,
    .blockLayer = gLevel1Stage2Room2BlockLayer,
    .blockMetatiles = gLevel1Stage2Room2BlockMetatiles,
    .width = 32,
    .height = 11,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E110,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 8,
    .objectsSortedByY = 0,
    .bgAnimSet = 3,
    .unk42 = 0,
    .doors = gLevel1Stage2Room2Doors,
    .objects = gLevel1Stage2Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][2][3], 0x08366974, section .room_def_08366974 */
extern u8 gLevel1Stage2Room3MetatileMap[];
extern u8 gLevel1Stage2Room3BlockLayer[];
extern u8 gLevel1Stage2Room3BlockMetatiles[];
extern struct Door gLevel1Stage2Room3Doors[];
extern u8 gLevel1Stage2Room3Objects[];
struct RoomDef gLevel1Stage2Room3 ROOM_DEF(08366974) = {
    .filler00 = { 1, 2, 3, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage2Room3MetatileMap,
    .blockLayer = gLevel1Stage2Room3BlockLayer,
    .blockMetatiles = gLevel1Stage2Room3BlockMetatiles,
    .width = 32,
    .height = 13,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E110,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 0,
    .bgAnimSet = 3,
    .unk42 = 0,
    .doors = gLevel1Stage2Room3Doors,
    .objects = gLevel1Stage2Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][2][4], 0x08366B98, section .room_def_08366b98 */
extern u8 gLevel1Stage2Room4MetatileMap[];
extern u8 gLevel1Stage2Room4BlockLayer[];
extern u8 gLevel1Stage2Room4BlockMetatiles[];
extern struct Door gLevel1Stage2Room4Doors[];
extern u8 gLevel1Stage2Room4Objects[];
struct RoomDef gLevel1Stage2Room4 ROOM_DEF(08366b98) = {
    .filler00 = { 1, 2, 4, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage2Room4MetatileMap,
    .blockLayer = gLevel1Stage2Room4BlockLayer,
    .blockMetatiles = gLevel1Stage2Room4BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0849A52C,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E110,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 4,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage2Room4Doors,
    .objects = gLevel1Stage2Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][2][5], 0x08366D1C, section .room_def_08366d1c */
extern u8 gLevel1Stage2Room5MetatileMap[];
extern u8 gLevel1Stage2Room5BlockLayer[];
extern u8 gLevel1Stage2Room5BlockMetatiles[];
extern struct Door gLevel1Stage2Room5Doors[];
extern u8 gLevel1Stage2Room5Objects[];
struct RoomDef gLevel1Stage2Room5 ROOM_DEF(08366d1c) = {
    .filler00 = { 1, 2, 5, 0 },
    .bgm = 37,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage2Room5MetatileMap,
    .blockLayer = gLevel1Stage2Room5BlockLayer,
    .blockMetatiles = gLevel1Stage2Room5BlockMetatiles,
    .width = 18,
    .height = 12,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 24,
    .borderY = 8,
    .bg3Palette = gUnk_0853E110,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage2Room5Doors,
    .objects = gLevel1Stage2Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][3][0], 0x08367274, section .room_def_08367274 */
extern u8 gLevel1Stage3Room0MetatileMap[];
extern u8 gLevel1Stage3Room0BlockLayer[];
extern u8 gLevel1Stage3Room0BlockMetatiles[];
extern struct Door gLevel1Stage3Room0Doors[];
extern u8 gLevel1Stage3Room0Objects[];
struct RoomDef gLevel1Stage3Room0 ROOM_DEF(08367274) = {
    .filler00 = { 1, 3, 0, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage3Room0MetatileMap,
    .blockLayer = gLevel1Stage3Room0BlockLayer,
    .blockMetatiles = gLevel1Stage3Room0BlockMetatiles,
    .width = 80,
    .height = 13,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E214,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 17,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage3Room0Doors,
    .objects = gLevel1Stage3Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 89,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][3][1], 0x0836753C, section .room_def_0836753c */
extern u8 gLevel1Stage3Room1MetatileMap[];
extern u8 gLevel1Stage3Room1BlockLayer[];
extern u8 gLevel1Stage3Room1BlockMetatiles[];
extern struct Door gLevel1Stage3Room1Doors[];
extern u8 gLevel1Stage3Room1Objects[];
struct RoomDef gLevel1Stage3Room1 ROOM_DEF(0836753c) = {
    .filler00 = { 1, 3, 1, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage3Room1MetatileMap,
    .blockLayer = gLevel1Stage3Room1BlockLayer,
    .blockMetatiles = gLevel1Stage3Room1BlockMetatiles,
    .width = 16,
    .height = 22,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E214,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 8,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage3Room1Doors,
    .objects = gLevel1Stage3Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 88,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][3][2], 0x083677F4, section .room_def_083677f4 */
extern u8 gLevel1Stage3Room2MetatileMap[];
extern u8 gLevel1Stage3Room2BlockLayer[];
extern u8 gLevel1Stage3Room2BlockMetatiles[];
extern struct Door gLevel1Stage3Room2Doors[];
extern u8 gLevel1Stage3Room2Objects[];
struct RoomDef gLevel1Stage3Room2 ROOM_DEF(083677f4) = {
    .filler00 = { 1, 3, 2, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage3Room2MetatileMap,
    .blockLayer = gLevel1Stage3Room2BlockLayer,
    .blockMetatiles = gLevel1Stage3Room2BlockMetatiles,
    .width = 33,
    .height = 11,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E214,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage3Room2Doors,
    .objects = gLevel1Stage3Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 121,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][3][3], 0x08367A50, section .room_def_08367a50 */
extern u8 gLevel1Stage3Room3MetatileMap[];
extern u8 gLevel1Stage3Room3BlockLayer[];
extern u8 gLevel1Stage3Room3BlockMetatiles[];
extern struct Door gLevel1Stage3Room3Doors[];
extern u8 gLevel1Stage3Room3Objects[];
struct RoomDef gLevel1Stage3Room3 ROOM_DEF(08367a50) = {
    .filler00 = { 1, 3, 3, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage3Room3MetatileMap,
    .blockLayer = gLevel1Stage3Room3BlockLayer,
    .blockMetatiles = gLevel1Stage3Room3BlockMetatiles,
    .width = 16,
    .height = 23,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E214,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 9,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage3Room3Doors,
    .objects = gLevel1Stage3Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 39,
    .entryY = 105,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][3][4], 0x08367E8C, section .room_def_08367e8c */
extern u8 gLevel1Stage3Room4MetatileMap[];
extern u8 gLevel1Stage3Room4BlockLayer[];
extern u8 gLevel1Stage3Room4BlockMetatiles[];
extern struct Door gLevel1Stage3Room4Doors[];
extern u8 gLevel1Stage3Room4Objects[];
struct RoomDef gLevel1Stage3Room4 ROOM_DEF(08367e8c) = {
    .filler00 = { 1, 3, 4, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage3Room4MetatileMap,
    .blockLayer = gLevel1Stage3Room4BlockLayer,
    .blockMetatiles = gLevel1Stage3Room4BlockMetatiles,
    .width = 50,
    .height = 11,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E214,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 16,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage3Room4Doors,
    .objects = gLevel1Stage3Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 120,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][3][5], 0x083684D4, section .room_def_083684d4 */
extern u8 gLevel1Stage3Room5MetatileMap[];
extern u8 gLevel1Stage3Room5BlockLayer[];
extern u8 gLevel1Stage3Room5BlockMetatiles[];
extern struct Door gLevel1Stage3Room5Doors[];
extern u8 gLevel1Stage3Room5Objects[];
struct RoomDef gLevel1Stage3Room5 ROOM_DEF(083684d4) = {
    .filler00 = { 1, 3, 5, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage3Room5MetatileMap,
    .blockLayer = gLevel1Stage3Room5BlockLayer,
    .blockMetatiles = gLevel1Stage3Room5BlockMetatiles,
    .width = 80,
    .height = 14,
    .bg2Palette = gUnk_0850A9A8,
    .bg2Tiles = gUnk_0840CAAC,
    .metatileTiles = gUnk_083AAAB0,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E214,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 12,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage3Room5Doors,
    .objects = gLevel1Stage3Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][3][6], 0x08368710, section .room_def_08368710 */
extern u8 gLevel1Stage3Room6MetatileMap[];
extern u8 gLevel1Stage3Room6BlockLayer[];
extern u8 gLevel1Stage3Room6BlockMetatiles[];
extern struct Door gLevel1Stage3Room6Doors[];
extern u8 gLevel1Stage3Room6Objects[];
struct RoomDef gLevel1Stage3Room6 ROOM_DEF(08368710) = {
    .filler00 = { 1, 3, 6, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage3Room6MetatileMap,
    .blockLayer = gLevel1Stage3Room6BlockLayer,
    .blockMetatiles = gLevel1Stage3Room6BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853E728,
    .bg2Tiles = gUnk_0847E4B4,
    .metatileTiles = gUnk_083B39C4,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E214,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 5,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage3Room6Doors,
    .objects = gLevel1Stage3Room6Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][4][0], 0x08368D30, section .room_def_08368d30 */
extern u8 gLevel1Stage4Room0MetatileMap[];
extern u8 gLevel1Stage4Room0BlockLayer[];
extern u8 gLevel1Stage4Room0BlockMetatiles[];
extern struct Door gLevel1Stage4Room0Doors[];
extern u8 gLevel1Stage4Room0Objects[];
struct RoomDef gLevel1Stage4Room0 ROOM_DEF(08368d30) = {
    .filler00 = { 1, 4, 0, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage4Room0MetatileMap,
    .blockLayer = gLevel1Stage4Room0BlockLayer,
    .blockMetatiles = gLevel1Stage4Room0BlockMetatiles,
    .width = 80,
    .height = 11,
    .bg2Palette = gUnk_0853D12C,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DF08,
    .bg3Tiles = gUnk_0844F9D0,
    .bg3Map = &gUnk_083CC17C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 11,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage4Room0Doors,
    .objects = gLevel1Stage4Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 41,
    .entryY = 73,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][4][1], 0x083692D8, section .room_def_083692d8 */
extern u8 gLevel1Stage4Room1MetatileMap[];
extern u8 gLevel1Stage4Room1BlockLayer[];
extern u8 gLevel1Stage4Room1BlockMetatiles[];
extern struct Door gLevel1Stage4Room1Doors[];
extern u8 gLevel1Stage4Room1Objects[];
struct RoomDef gLevel1Stage4Room1 ROOM_DEF(083692d8) = {
    .filler00 = { 1, 4, 1, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage4Room1MetatileMap,
    .blockLayer = gLevel1Stage4Room1BlockLayer,
    .blockMetatiles = gLevel1Stage4Room1BlockMetatiles,
    .width = 80,
    .height = 11,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DF08,
    .bg3Tiles = gUnk_0844F9D0,
    .bg3Map = &gUnk_083CC17C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 14,
    .objectsSortedByY = 0,
    .bgAnimSet = 3,
    .unk42 = 0,
    .doors = gLevel1Stage4Room1Doors,
    .objects = gLevel1Stage4Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 105,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][4][2], 0x08369BE0, section .room_def_08369be0 */
extern u8 gLevel1Stage4Room2MetatileMap[];
extern u8 gLevel1Stage4Room2BlockLayer[];
extern u8 gLevel1Stage4Room2BlockMetatiles[];
extern struct Door gLevel1Stage4Room2Doors[];
extern u8 gLevel1Stage4Room2Objects[];
struct RoomDef gLevel1Stage4Room2 ROOM_DEF(08369be0) = {
    .filler00 = { 1, 4, 2, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage4Room2MetatileMap,
    .blockLayer = gLevel1Stage4Room2BlockLayer,
    .blockMetatiles = gLevel1Stage4Room2BlockMetatiles,
    .width = 90,
    .height = 21,
    .bg2Palette = gUnk_0853DAB4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853CEDC,
    .bg3Tiles = gUnk_08401D3C,
    .bg3Map = &gUnk_083BADEC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 24,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage4Room2Doors,
    .objects = gLevel1Stage4Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 216,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][4][3], 0x0836A0A8, section .room_def_0836a0a8 */
extern u8 gLevel1Stage4Room3MetatileMap[];
extern u8 gLevel1Stage4Room3BlockLayer[];
extern u8 gLevel1Stage4Room3BlockMetatiles[];
extern struct Door gLevel1Stage4Room3Doors[];
extern u8 gLevel1Stage4Room3Objects[];
struct RoomDef gLevel1Stage4Room3 ROOM_DEF(0836a0a8) = {
    .filler00 = { 1, 4, 3, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage4Room3MetatileMap,
    .blockLayer = gLevel1Stage4Room3BlockLayer,
    .blockMetatiles = gLevel1Stage4Room3BlockMetatiles,
    .width = 16,
    .height = 23,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CEDC,
    .bg3Tiles = gUnk_08401D3C,
    .bg3Map = &gUnk_083BADEC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 8,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage4Room3Doors,
    .objects = gLevel1Stage4Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 119,
    .entryY = 328,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][4][4], 0x0836A318, section .room_def_0836a318 */
extern u8 gLevel1Stage4Room4MetatileMap[];
extern u8 gLevel1Stage4Room4BlockLayer[];
extern u8 gLevel1Stage4Room4BlockMetatiles[];
extern struct Door gLevel1Stage4Room4Doors[];
extern u8 gLevel1Stage4Room4Objects[];
struct RoomDef gLevel1Stage4Room4 ROOM_DEF(0836a318) = {
    .filler00 = { 1, 4, 4, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage4Room4MetatileMap,
    .blockLayer = gLevel1Stage4Room4BlockLayer,
    .blockMetatiles = gLevel1Stage4Room4BlockMetatiles,
    .width = 32,
    .height = 14,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853CEDC,
    .bg3Tiles = gUnk_08401D3C,
    .bg3Map = &gUnk_083BADEC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 8,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage4Room4Doors,
    .objects = gLevel1Stage4Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][4][5], 0x0836A534, section .room_def_0836a534 */
extern u8 gLevel1Stage4Room5MetatileMap[];
extern u8 gLevel1Stage4Room5BlockLayer[];
extern u8 gLevel1Stage4Room5BlockMetatiles[];
extern struct Door gLevel1Stage4Room5Doors[];
extern u8 gLevel1Stage4Room5Objects[];
struct RoomDef gLevel1Stage4Room5 ROOM_DEF(0836a534) = {
    .filler00 = { 1, 4, 5, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage4Room5MetatileMap,
    .blockLayer = gLevel1Stage4Room5BlockLayer,
    .blockMetatiles = gLevel1Stage4Room5BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DF08,
    .bg3Tiles = gUnk_0844F9D0,
    .bg3Map = &gUnk_083CC17C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 4,
    .objectsSortedByY = 0,
    .bgAnimSet = 3,
    .unk42 = 0,
    .doors = gLevel1Stage4Room5Doors,
    .objects = gLevel1Stage4Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 121,
    .entryY = 41,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][4][6], 0x0836A6DC, section .room_def_0836a6dc */
extern u8 gLevel1Stage4Room6MetatileMap[];
extern u8 gLevel1Stage4Room6BlockLayer[];
extern u8 gLevel1Stage4Room6BlockMetatiles[];
extern struct Door gLevel1Stage4Room6Doors[];
extern u8 gLevel1Stage4Room6Objects[];
struct RoomDef gLevel1Stage4Room6 ROOM_DEF(0836a6dc) = {
    .filler00 = { 1, 4, 6, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage4Room6MetatileMap,
    .blockLayer = gLevel1Stage4Room6BlockLayer,
    .blockMetatiles = gLevel1Stage4Room6BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CEDC,
    .bg3Tiles = gUnk_08401D3C,
    .bg3Map = &gUnk_083BADEC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 3,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel1Stage4Room6Doors,
    .objects = gLevel1Stage4Room6Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 89,
    .entryY = 137,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[1][5][0], 0x0836A89C, section .room_def_0836a89c */
extern u8 gLevel1Stage5Room0MetatileMap[];
extern u8 gLevel1Stage5Room0BlockLayer[];
extern u8 gLevel1Stage5Room0BlockMetatiles[];
extern u8 gLevel1Stage5Room0Objects[];
struct RoomDef gLevel1Stage5Room0 ROOM_DEF(0836a89c) = {
    .filler00 = { 1, 5, 0, 0 },
    .bgm = 30,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel1Stage5Room0MetatileMap,
    .blockLayer = gLevel1Stage5Room0BlockLayer,
    .blockMetatiles = gLevel1Stage5Room0BlockMetatiles,
    .width = 16,
    .height = 12,
    .bg2Palette = gLevel1Stage5Room0Bg2Palette,
    .bg2Tiles = gLevel1Stage5Room0Bg2Tiles,
    .metatileTiles = gLevel1Stage5Room0MetatileTiles,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gLevel1Stage5Room0Bg3Palette,
    .bg3Tiles = gLevel1Stage5Room0Bg3Tiles,
    .bg3Map = &gLevel1Stage5Room0Bg3Map,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = gLevel1Stage5Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 5,
};

/* gRoomTable[2][0][0], 0x0836AC58, section .room_def_0836ac58 */
extern u8 gLevel2Stage0Room0MetatileMap[];
extern u8 gLevel2Stage0Room0BlockLayer[];
extern u8 gLevel2Stage0Room0BlockMetatiles[];
extern struct Door gLevel2Stage0Room0Doors[];
extern u8 gLevel2Stage0Room0Objects[];
struct RoomDef gLevel2Stage0Room0 ROOM_DEF(0836ac58) = {
    .filler00 = { 2, 0, 0, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage0Room0MetatileMap,
    .blockLayer = gLevel2Stage0Room0BlockLayer,
    .blockMetatiles = gLevel2Stage0Room0BlockMetatiles,
    .width = 48,
    .height = 11,
    .bg2Palette = gUnk_0853DBB8,
    .bg2Tiles = gUnk_0843FC10,
    .metatileTiles = gUnk_083B18FC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CEDC,
    .bg3Tiles = gUnk_08401D3C,
    .bg3Map = &gUnk_083BADEC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 8,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage0Room0Doors,
    .objects = gLevel2Stage0Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 56,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][0][1], 0x0836AEE4, section .room_def_0836aee4 */
extern u8 gLevel2Stage0Room1MetatileMap[];
extern u8 gLevel2Stage0Room1BlockLayer[];
extern u8 gLevel2Stage0Room1BlockMetatiles[];
extern struct Door gLevel2Stage0Room1Doors[];
extern u8 gLevel2Stage0Room1Objects[];
struct RoomDef gLevel2Stage0Room1 ROOM_DEF(0836aee4) = {
    .filler00 = { 2, 0, 1, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage0Room1MetatileMap,
    .blockLayer = gLevel2Stage0Room1BlockLayer,
    .blockMetatiles = gLevel2Stage0Room1BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853D53C,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849A83C,
    .bg3Tiles = gUnk_0849BE08,
    .bg3Map = &gUnk_0852C1D4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 5,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage0Room1Doors,
    .objects = gLevel2Stage0Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 55,
    .entryY = 121,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][0][2], 0x0836B160, section .room_def_0836b160 */
extern u8 gLevel2Stage0Room2MetatileMap[];
extern u8 gLevel2Stage0Room2BlockLayer[];
extern u8 gLevel2Stage0Room2BlockMetatiles[];
extern struct Door gLevel2Stage0Room2Doors[];
extern u8 gLevel2Stage0Room2Objects[];
struct RoomDef gLevel2Stage0Room2 ROOM_DEF(0836b160) = {
    .filler00 = { 2, 0, 2, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage0Room2MetatileMap,
    .blockLayer = gLevel2Stage0Room2BlockLayer,
    .blockMetatiles = gLevel2Stage0Room2BlockMetatiles,
    .width = 32,
    .height = 11,
    .bg2Palette = gUnk_0853DBB8,
    .bg2Tiles = gUnk_0843FC10,
    .metatileTiles = gUnk_083B18FC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DD00,
    .bg3Tiles = gUnk_0844619C,
    .bg3Map = &gUnk_083CAF40,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 9,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage0Room2Doors,
    .objects = gLevel2Stage0Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 120,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][0][3], 0x0836B44C, section .room_def_0836b44c */
extern u8 gLevel2Stage0Room3MetatileMap[];
extern u8 gLevel2Stage0Room3BlockLayer[];
extern u8 gLevel2Stage0Room3BlockMetatiles[];
extern struct Door gLevel2Stage0Room3Doors[];
extern u8 gLevel2Stage0Room3Objects[];
struct RoomDef gLevel2Stage0Room3 ROOM_DEF(0836b44c) = {
    .filler00 = { 2, 0, 3, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage0Room3MetatileMap,
    .blockLayer = gLevel2Stage0Room3BlockLayer,
    .blockMetatiles = gLevel2Stage0Room3BlockMetatiles,
    .width = 16,
    .height = 24,
    .bg2Palette = gUnk_0853D53C,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849A83C,
    .bg3Tiles = gUnk_0849BE08,
    .bg3Map = &gUnk_0852C1D4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 3,
    .objectCount = 2,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage0Room3Doors,
    .objects = gLevel2Stage0Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 41,
    .entryY = 201,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][0][4], 0x0836B7B4, section .room_def_0836b7b4 */
extern u8 gLevel2Stage0Room4MetatileMap[];
extern u8 gLevel2Stage0Room4BlockLayer[];
extern u8 gLevel2Stage0Room4BlockMetatiles[];
extern struct Door gLevel2Stage0Room4Doors[];
struct RoomDef gLevel2Stage0Room4 ROOM_DEF(0836b7b4) = {
    .filler00 = { 2, 0, 4, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage0Room4MetatileMap,
    .blockLayer = gLevel2Stage0Room4BlockLayer,
    .blockMetatiles = gLevel2Stage0Room4BlockMetatiles,
    .width = 16,
    .height = 12,
    .bg2Palette = gUnk_0853D53C,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849A83C,
    .bg3Tiles = gUnk_0849BE08,
    .bg3Map = &gUnk_0852C1D4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage0Room4Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 122,
    .entryY = 153,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][0][5], 0x0836B968, section .room_def_0836b968 */
extern u8 gLevel2Stage0Room5MetatileMap[];
extern u8 gLevel2Stage0Room5BlockLayer[];
extern u8 gLevel2Stage0Room5BlockMetatiles[];
extern struct Door gLevel2Stage0Room5Doors[];
extern u8 gLevel2Stage0Room5Objects[];
struct RoomDef gLevel2Stage0Room5 ROOM_DEF(0836b968) = {
    .filler00 = { 2, 0, 5, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage0Room5MetatileMap,
    .blockLayer = gLevel2Stage0Room5BlockLayer,
    .blockMetatiles = gLevel2Stage0Room5BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853D53C,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849A83C,
    .bg3Tiles = gUnk_0849BE08,
    .bg3Map = &gUnk_0852C1D4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage0Room5Doors,
    .objects = gLevel2Stage0Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 137,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][1][0], 0x0836BC70, section .room_def_0836bc70 */
extern u8 gLevel2Stage1Room0MetatileMap[];
extern u8 gLevel2Stage1Room0BlockLayer[];
extern u8 gLevel2Stage1Room0BlockMetatiles[];
extern struct Door gLevel2Stage1Room0Doors[];
extern u8 gLevel2Stage1Room0Objects[];
struct RoomDef gLevel2Stage1Room0 ROOM_DEF(0836bc70) = {
    .filler00 = { 2, 1, 0, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage1Room0MetatileMap,
    .blockLayer = gLevel2Stage1Room0BlockLayer,
    .blockMetatiles = gLevel2Stage1Room0BlockMetatiles,
    .width = 16,
    .height = 24,
    .bg2Palette = gUnk_085111F8,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849A83C,
    .bg3Tiles = gUnk_0849BE08,
    .bg3Map = &gUnk_0852C1D4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 10,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage1Room0Doors,
    .objects = gLevel2Stage1Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 39,
    .entryY = 281,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][1][1], 0x0836C0B0, section .room_def_0836c0b0 */
extern u8 gLevel2Stage1Room1MetatileMap[];
extern u8 gLevel2Stage1Room1BlockLayer[];
extern u8 gLevel2Stage1Room1BlockMetatiles[];
extern struct Door gLevel2Stage1Room1Doors[];
extern u8 gLevel2Stage1Room1Objects[];
struct RoomDef gLevel2Stage1Room1 ROOM_DEF(0836c0b0) = {
    .filler00 = { 2, 1, 1, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage1Room1MetatileMap,
    .blockLayer = gLevel2Stage1Room1BlockLayer,
    .blockMetatiles = gLevel2Stage1Room1BlockMetatiles,
    .width = 17,
    .height = 48,
    .bg2Palette = gUnk_0853DBB8,
    .bg2Tiles = gUnk_0843FC10,
    .metatileTiles = gUnk_083B18FC,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0853CEDC,
    .bg3Tiles = gUnk_08401D3C,
    .bg3Map = &gUnk_083BADEC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 12,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage1Room1Doors,
    .objects = gLevel2Stage1Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 136,
    .entryY = 696,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][1][2], 0x0836C50C, section .room_def_0836c50c */
extern u8 gLevel2Stage1Room2MetatileMap[];
extern u8 gLevel2Stage1Room2BlockLayer[];
extern u8 gLevel2Stage1Room2BlockMetatiles[];
extern struct Door gLevel2Stage1Room2Doors[];
extern u8 gLevel2Stage1Room2Objects[];
struct RoomDef gLevel2Stage1Room2 ROOM_DEF(0836c50c) = {
    .filler00 = { 2, 1, 2, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage1Room2MetatileMap,
    .blockLayer = gLevel2Stage1Room2BlockLayer,
    .blockMetatiles = gLevel2Stage1Room2BlockMetatiles,
    .width = 16,
    .height = 36,
    .bg2Palette = gUnk_085111F8,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849A83C,
    .bg3Tiles = gUnk_0849BE08,
    .bg3Map = &gUnk_0852C1D4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 10,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage1Room2Doors,
    .objects = gLevel2Stage1Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 119,
    .entryY = 536,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][1][3], 0x0836CA98, section .room_def_0836ca98 */
extern u8 gLevel2Stage1Room3MetatileMap[];
extern u8 gLevel2Stage1Room3BlockLayer[];
extern u8 gLevel2Stage1Room3BlockMetatiles[];
extern struct Door gLevel2Stage1Room3Doors[];
extern u8 gLevel2Stage1Room3Objects[];
struct RoomDef gLevel2Stage1Room3 ROOM_DEF(0836ca98) = {
    .filler00 = { 2, 1, 3, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage1Room3MetatileMap,
    .blockLayer = gLevel2Stage1Room3BlockLayer,
    .blockMetatiles = gLevel2Stage1Room3BlockMetatiles,
    .width = 32,
    .height = 23,
    .bg2Palette = gUnk_085111F8,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849A83C,
    .bg3Tiles = gUnk_0849BE08,
    .bg3Map = &gUnk_0849ABCC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 3,
    .objectCount = 9,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage1Room3Doors,
    .objects = gLevel2Stage1Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 216,
    .entryY = 329,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][1][4], 0x0836CDA8, section .room_def_0836cda8 */
extern u8 gLevel2Stage1Room4MetatileMap[];
extern u8 gLevel2Stage1Room4BlockLayer[];
extern u8 gLevel2Stage1Room4BlockMetatiles[];
extern struct Door gLevel2Stage1Room4Doors[];
extern u8 gLevel2Stage1Room4Objects[];
struct RoomDef gLevel2Stage1Room4 ROOM_DEF(0836cda8) = {
    .filler00 = { 2, 1, 4, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage1Room4MetatileMap,
    .blockLayer = gLevel2Stage1Room4BlockLayer,
    .blockMetatiles = gLevel2Stage1Room4BlockMetatiles,
    .width = 16,
    .height = 13,
    .bg2Palette = gUnk_085111F8,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853C8C4,
    .bg3Tiles = gUnk_083E2A30,
    .bg3Map = &gUnk_083B6770,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 4,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage1Room4Doors,
    .objects = gLevel2Stage1Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 137,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][1][5], 0x0836D19C, section .room_def_0836d19c */
extern u8 gLevel2Stage1Room5MetatileMap[];
extern u8 gLevel2Stage1Room5BlockLayer[];
extern u8 gLevel2Stage1Room5BlockMetatiles[];
extern struct Door gLevel2Stage1Room5Doors[];
extern u8 gLevel2Stage1Room5Objects[];
struct RoomDef gLevel2Stage1Room5 ROOM_DEF(0836d19c) = {
    .filler00 = { 2, 1, 5, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage1Room5MetatileMap,
    .blockLayer = gLevel2Stage1Room5BlockLayer,
    .blockMetatiles = gLevel2Stage1Room5BlockMetatiles,
    .width = 51,
    .height = 11,
    .bg2Palette = gUnk_085111F8,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C8C4,
    .bg3Tiles = gUnk_083E2A30,
    .bg3Map = &gUnk_083B6770,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 15,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage1Room5Doors,
    .objects = gLevel2Stage1Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 72,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][1][6], 0x0836D334, section .room_def_0836d334 */
extern u8 gLevel2Stage1Room6MetatileMap[];
extern u8 gLevel2Stage1Room6BlockLayer[];
extern u8 gLevel2Stage1Room6BlockMetatiles[];
extern struct Door gLevel2Stage1Room6Doors[];
extern u8 gLevel2Stage1Room6Objects[];
struct RoomDef gLevel2Stage1Room6 ROOM_DEF(0836d334) = {
    .filler00 = { 2, 1, 6, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage1Room6MetatileMap,
    .blockLayer = gLevel2Stage1Room6BlockLayer,
    .blockMetatiles = gLevel2Stage1Room6BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_085111F8,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849A83C,
    .bg3Tiles = gUnk_0849BE08,
    .bg3Map = &gUnk_0852C1D4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage1Room6Doors,
    .objects = gLevel2Stage1Room6Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 137,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][1][7], 0x0836D4FC, section .room_def_0836d4fc */
extern u8 gLevel2Stage1Room7MetatileMap[];
extern u8 gLevel2Stage1Room7BlockLayer[];
extern u8 gLevel2Stage1Room7BlockMetatiles[];
extern struct Door gLevel2Stage1Room7Doors[];
extern u8 gLevel2Stage1Room7Objects[];
struct RoomDef gLevel2Stage1Room7 ROOM_DEF(0836d4fc) = {
    .filler00 = { 2, 1, 7, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage1Room7MetatileMap,
    .blockLayer = gLevel2Stage1Room7BlockLayer,
    .blockMetatiles = gLevel2Stage1Room7BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_085111F8,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849A83C,
    .bg3Tiles = gUnk_0849BE08,
    .bg3Map = &gUnk_0852C1D4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage1Room7Doors,
    .objects = gLevel2Stage1Room7Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 200,
    .entryY = 89,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][2][0], 0x0836DADC, section .room_def_0836dadc */
extern u8 gLevel2Stage2Room0MetatileMap[];
extern u8 gLevel2Stage2Room0BlockLayer[];
extern u8 gLevel2Stage2Room0BlockMetatiles[];
extern struct Door gLevel2Stage2Room0Doors[];
extern u8 gLevel2Stage2Room0Objects[];
struct RoomDef gLevel2Stage2Room0 ROOM_DEF(0836dadc) = {
    .filler00 = { 2, 2, 0, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage2Room0MetatileMap,
    .blockLayer = gLevel2Stage2Room0BlockLayer,
    .blockMetatiles = gLevel2Stage2Room0BlockMetatiles,
    .width = 32,
    .height = 23,
    .bg2Palette = gUnk_08511134,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C8C4,
    .bg3Tiles = gUnk_083E2A30,
    .bg3Map = &gUnk_083B6770,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 13,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage2Room0Doors,
    .objects = gLevel2Stage2Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 104,
    .entryY = 200,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][2][1], 0x0836DD44, section .room_def_0836dd44 */
extern u8 gLevel2Stage2Room1MetatileMap[];
extern u8 gLevel2Stage2Room1BlockLayer[];
extern u8 gLevel2Stage2Room1BlockMetatiles[];
extern struct Door gLevel2Stage2Room1Doors[];
extern u8 gLevel2Stage2Room1Objects[];
struct RoomDef gLevel2Stage2Room1 ROOM_DEF(0836dd44) = {
    .filler00 = { 2, 2, 1, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage2Room1MetatileMap,
    .blockLayer = gLevel2Stage2Room1BlockLayer,
    .blockMetatiles = gLevel2Stage2Room1BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0850AA6C,
    .bg2Tiles = gUnk_0843D610,
    .metatileTiles = gUnk_083B1058,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C8C4,
    .bg3Tiles = gUnk_083E2A30,
    .bg3Map = &gUnk_083B6770,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 3,
    .objectsSortedByY = 0,
    .bgAnimSet = 5,
    .unk42 = 0,
    .doors = gLevel2Stage2Room1Doors,
    .objects = gLevel2Stage2Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 120,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][2][2], 0x0836E09C, section .room_def_0836e09c */
extern u8 gLevel2Stage2Room2MetatileMap[];
extern u8 gLevel2Stage2Room2BlockLayer[];
extern u8 gLevel2Stage2Room2BlockMetatiles[];
extern struct Door gLevel2Stage2Room2Doors[];
extern u8 gLevel2Stage2Room2Objects[];
struct RoomDef gLevel2Stage2Room2 ROOM_DEF(0836e09c) = {
    .filler00 = { 2, 2, 2, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage2Room2MetatileMap,
    .blockLayer = gLevel2Stage2Room2BlockLayer,
    .blockMetatiles = gLevel2Stage2Room2BlockMetatiles,
    .width = 16,
    .height = 23,
    .bg2Palette = gUnk_08511134,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849A83C,
    .bg3Tiles = gUnk_0849BE08,
    .bg3Map = &gUnk_0852C1D4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 7,
    .objectCount = 6,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage2Room2Doors,
    .objects = gLevel2Stage2Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 57,
    .entryY = 313,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][2][3], 0x0836E3F4, section .room_def_0836e3f4 */
extern u8 gLevel2Stage2Room3MetatileMap[];
extern u8 gLevel2Stage2Room3BlockLayer[];
extern u8 gLevel2Stage2Room3BlockMetatiles[];
extern struct Door gLevel2Stage2Room3Doors[];
extern u8 gLevel2Stage2Room3Objects[];
struct RoomDef gLevel2Stage2Room3 ROOM_DEF(0836e3f4) = {
    .filler00 = { 2, 2, 3, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage2Room3MetatileMap,
    .blockLayer = gLevel2Stage2Room3BlockLayer,
    .blockMetatiles = gLevel2Stage2Room3BlockMetatiles,
    .width = 16,
    .height = 23,
    .bg2Palette = gUnk_08511134,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849A83C,
    .bg3Tiles = gUnk_0849BE08,
    .bg3Map = &gUnk_0852C1D4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 6,
    .objectCount = 5,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage2Room3Doors,
    .objects = gLevel2Stage2Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 199,
    .entryY = 313,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][2][4], 0x0836E660, section .room_def_0836e660 */
extern u8 gLevel2Stage2Room4MetatileMap[];
extern u8 gLevel2Stage2Room4BlockLayer[];
extern u8 gLevel2Stage2Room4BlockMetatiles[];
extern struct Door gLevel2Stage2Room4Doors[];
extern u8 gLevel2Stage2Room4Objects[];
struct RoomDef gLevel2Stage2Room4 ROOM_DEF(0836e660) = {
    .filler00 = { 2, 2, 4, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage2Room4MetatileMap,
    .blockLayer = gLevel2Stage2Room4BlockLayer,
    .blockMetatiles = gLevel2Stage2Room4BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08511134,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C8C4,
    .bg3Tiles = gUnk_083E2A30,
    .bg3Map = &gUnk_083B6770,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 5,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage2Room4Doors,
    .objects = gLevel2Stage2Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 41,
    .entryY = 121,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][3][0], 0x0836E824, section .room_def_0836e824 */
extern u8 gLevel2Stage3Room0MetatileMap[];
extern u8 gLevel2Stage3Room0BlockLayer[];
extern u8 gLevel2Stage3Room0BlockMetatiles[];
extern struct Door gLevel2Stage3Room0Doors[];
extern u8 gLevel2Stage3Room0Objects[];
struct RoomDef gLevel2Stage3Room0 ROOM_DEF(0836e824) = {
    .filler00 = { 2, 3, 0, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage3Room0MetatileMap,
    .blockLayer = gLevel2Stage3Room0BlockLayer,
    .blockMetatiles = gLevel2Stage3Room0BlockMetatiles,
    .width = 16,
    .height = 13,
    .bg2Palette = gUnk_0853DBB8,
    .bg2Tiles = gUnk_0843FC10,
    .metatileTiles = gUnk_083B18FC,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage3Room0Doors,
    .objects = gLevel2Stage3Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 137,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][3][1], 0x0836EED8, section .room_def_0836eed8 */
extern u8 gLevel2Stage3Room1MetatileMap[];
extern u8 gLevel2Stage3Room1BlockLayer[];
extern u8 gLevel2Stage3Room1BlockMetatiles[];
extern struct Door gLevel2Stage3Room1Doors[];
struct RoomDef gLevel2Stage3Room1 ROOM_DEF(0836eed8) = {
    .filler00 = { 2, 3, 1, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage3Room1MetatileMap,
    .blockLayer = gLevel2Stage3Room1BlockLayer,
    .blockMetatiles = gLevel2Stage3Room1BlockMetatiles,
    .width = 16,
    .height = 108,
    .bg2Palette = gUnk_0853DBB8,
    .bg2Tiles = gUnk_0843FC10,
    .metatileTiles = gUnk_083B18FC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage3Room1Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 80,
    .entryY = 1672,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][3][2], 0x0836F0A4, section .room_def_0836f0a4 */
extern u8 gLevel2Stage3Room2MetatileMap[];
extern u8 gLevel2Stage3Room2BlockLayer[];
extern u8 gLevel2Stage3Room2BlockMetatiles[];
extern struct Door gLevel2Stage3Room2Doors[];
struct RoomDef gLevel2Stage3Room2 ROOM_DEF(0836f0a4) = {
    .filler00 = { 2, 3, 2, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage3Room2MetatileMap,
    .blockLayer = gLevel2Stage3Room2BlockLayer,
    .blockMetatiles = gLevel2Stage3Room2BlockMetatiles,
    .width = 16,
    .height = 13,
    .bg2Palette = gUnk_0853DBB8,
    .bg2Tiles = gUnk_0843FC10,
    .metatileTiles = gUnk_083B18FC,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 3,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage3Room2Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 88,
    .entryY = 137,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][3][3], 0x0836F270, section .room_def_0836f270 */
extern u8 gLevel2Stage3Room3MetatileMap[];
extern u8 gLevel2Stage3Room3BlockLayer[];
extern u8 gLevel2Stage3Room3BlockMetatiles[];
extern struct Door gLevel2Stage3Room3Doors[];
extern u8 gLevel2Stage3Room3Objects[];
struct RoomDef gLevel2Stage3Room3 ROOM_DEF(0836f270) = {
    .filler00 = { 2, 3, 3, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage3Room3MetatileMap,
    .blockLayer = gLevel2Stage3Room3BlockLayer,
    .blockMetatiles = gLevel2Stage3Room3BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08511134,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_0852C930,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage3Room3Doors,
    .objects = gLevel2Stage3Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][3][4], 0x0836F634, section .room_def_0836f634 */
extern u8 gLevel2Stage3Room4MetatileMap[];
extern u8 gLevel2Stage3Room4BlockLayer[];
extern u8 gLevel2Stage3Room4BlockMetatiles[];
extern struct Door gLevel2Stage3Room4Doors[];
extern u8 gLevel2Stage3Room4Objects[];
struct RoomDef gLevel2Stage3Room4 ROOM_DEF(0836f634) = {
    .filler00 = { 2, 3, 4, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage3Room4MetatileMap,
    .blockLayer = gLevel2Stage3Room4BlockLayer,
    .blockMetatiles = gLevel2Stage3Room4BlockMetatiles,
    .width = 16,
    .height = 47,
    .bg2Palette = gUnk_0853DBB8,
    .bg2Tiles = gUnk_0843FC10,
    .metatileTiles = gUnk_083B18FC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 11,
    .doorCount = 1,
    .objectCount = 11,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage3Room4Doors,
    .objects = gLevel2Stage3Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 104,
    .entryY = 713,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][3][5], 0x0836F7E4, section .room_def_0836f7e4 */
extern u8 gLevel2Stage3Room5MetatileMap[];
extern u8 gLevel2Stage3Room5BlockLayer[];
extern u8 gLevel2Stage3Room5BlockMetatiles[];
extern struct Door gLevel2Stage3Room5Doors[];
extern u8 gLevel2Stage3Room5Objects[];
struct RoomDef gLevel2Stage3Room5 ROOM_DEF(0836f7e4) = {
    .filler00 = { 2, 3, 5, 0 },
    .bgm = 37,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage3Room5MetatileMap,
    .blockLayer = gLevel2Stage3Room5BlockLayer,
    .blockMetatiles = gLevel2Stage3Room5BlockMetatiles,
    .width = 18,
    .height = 12,
    .bg2Palette = gUnk_08511134,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 24,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_0852C930,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage3Room5Doors,
    .objects = gLevel2Stage3Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 120,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][4][0], 0x0836F9B0, section .room_def_0836f9b0 */
extern u8 gLevel2Stage4Room0MetatileMap[];
extern u8 gLevel2Stage4Room0BlockLayer[];
extern u8 gLevel2Stage4Room0BlockMetatiles[];
extern struct Door gLevel2Stage4Room0Doors[];
extern u8 gLevel2Stage4Room0Objects[];
struct RoomDef gLevel2Stage4Room0 ROOM_DEF(0836f9b0) = {
    .filler00 = { 2, 4, 0, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage4Room0MetatileMap,
    .blockLayer = gLevel2Stage4Room0BlockLayer,
    .blockMetatiles = gLevel2Stage4Room0BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08511070,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C9C8,
    .bg3Tiles = gUnk_083E6548,
    .bg3Map = &gUnk_083B708C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 4,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage4Room0Doors,
    .objects = gLevel2Stage4Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 57,
    .entryY = 72,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][4][1], 0x0836FD44, section .room_def_0836fd44 */
extern u8 gLevel2Stage4Room1MetatileMap[];
extern u8 gLevel2Stage4Room1BlockLayer[];
extern u8 gLevel2Stage4Room1BlockMetatiles[];
extern struct Door gLevel2Stage4Room1Doors[];
extern u8 gLevel2Stage4Room1Objects[];
struct RoomDef gLevel2Stage4Room1 ROOM_DEF(0836fd44) = {
    .filler00 = { 2, 4, 1, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage4Room1MetatileMap,
    .blockLayer = gLevel2Stage4Room1BlockLayer,
    .blockMetatiles = gLevel2Stage4Room1BlockMetatiles,
    .width = 17,
    .height = 43,
    .bg2Palette = gUnk_0853DBB8,
    .bg2Tiles = gUnk_0843FC10,
    .metatileTiles = gUnk_083B18FC,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0853C9C8,
    .bg3Tiles = gUnk_083E6548,
    .bg3Map = &gUnk_083B708C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 7,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage4Room1Doors,
    .objects = gLevel2Stage4Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 120,
    .entryY = 616,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][4][2], 0x08370000, section .room_def_08370000 */
extern u8 gLevel2Stage4Room2MetatileMap[];
extern u8 gLevel2Stage4Room2BlockLayer[];
extern u8 gLevel2Stage4Room2BlockMetatiles[];
extern struct Door gLevel2Stage4Room2Doors[];
extern u8 gLevel2Stage4Room2Objects[];
struct RoomDef gLevel2Stage4Room2 ROOM_DEF(08370000) = {
    .filler00 = { 2, 4, 2, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage4Room2MetatileMap,
    .blockLayer = gLevel2Stage4Room2BlockLayer,
    .blockMetatiles = gLevel2Stage4Room2BlockMetatiles,
    .width = 32,
    .height = 11,
    .bg2Palette = gUnk_08511070,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_084A7CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 9,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage4Room2Doors,
    .objects = gLevel2Stage4Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 439,
    .entryY = 120,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][4][3], 0x083702FC, section .room_def_083702fc */
extern u8 gLevel2Stage4Room3MetatileMap[];
extern u8 gLevel2Stage4Room3BlockLayer[];
extern u8 gLevel2Stage4Room3BlockMetatiles[];
extern struct Door gLevel2Stage4Room3Doors[];
extern u8 gLevel2Stage4Room3Objects[];
struct RoomDef gLevel2Stage4Room3 ROOM_DEF(083702fc) = {
    .filler00 = { 2, 4, 3, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage4Room3MetatileMap,
    .blockLayer = gLevel2Stage4Room3BlockLayer,
    .blockMetatiles = gLevel2Stage4Room3BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08511070,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_0852C930,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 3,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage4Room3Doors,
    .objects = gLevel2Stage4Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 216,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][4][4], 0x083704E4, section .room_def_083704e4 */
extern u8 gLevel2Stage4Room4MetatileMap[];
extern u8 gLevel2Stage4Room4BlockLayer[];
extern u8 gLevel2Stage4Room4BlockMetatiles[];
extern struct Door gLevel2Stage4Room4Doors[];
extern u8 gLevel2Stage4Room4Objects[];
struct RoomDef gLevel2Stage4Room4 ROOM_DEF(083704e4) = {
    .filler00 = { 2, 4, 4, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage4Room4MetatileMap,
    .blockLayer = gLevel2Stage4Room4BlockLayer,
    .blockMetatiles = gLevel2Stage4Room4BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08511070,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C9C8,
    .bg3Tiles = gUnk_083E6548,
    .bg3Map = &gUnk_083B708C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage4Room4Doors,
    .objects = gLevel2Stage4Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 55,
    .entryY = 137,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][5][0], 0x08370684, section .room_def_08370684 */
extern u8 gLevel2Stage5Room0MetatileMap[];
extern u8 gLevel2Stage5Room0BlockLayer[];
extern u8 gLevel2Stage5Room0BlockMetatiles[];
extern struct Door gLevel2Stage5Room0Doors[];
extern u8 gLevel2Stage5Room0Objects[];
struct RoomDef gLevel2Stage5Room0 ROOM_DEF(08370684) = {
    .filler00 = { 2, 5, 0, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage5Room0MetatileMap,
    .blockLayer = gLevel2Stage5Room0BlockLayer,
    .blockMetatiles = gLevel2Stage5Room0BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08510FAC,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_0852C930,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 4,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage5Room0Doors,
    .objects = gLevel2Stage5Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 41,
    .entryY = 72,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][5][1], 0x08370AF4, section .room_def_08370af4 */
extern u8 gLevel2Stage5Room1MetatileMap[];
extern u8 gLevel2Stage5Room1BlockLayer[];
extern u8 gLevel2Stage5Room1BlockMetatiles[];
extern struct Door gLevel2Stage5Room1Doors[];
extern u8 gLevel2Stage5Room1Objects[];
struct RoomDef gLevel2Stage5Room1 ROOM_DEF(08370af4) = {
    .filler00 = { 2, 5, 1, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage5Room1MetatileMap,
    .blockLayer = gLevel2Stage5Room1BlockLayer,
    .blockMetatiles = gLevel2Stage5Room1BlockMetatiles,
    .width = 48,
    .height = 11,
    .bg2Palette = gUnk_08510FAC,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_084A7CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 7,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage5Room1Doors,
    .objects = gLevel2Stage5Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 57,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][5][2], 0x08370E08, section .room_def_08370e08 */
extern u8 gLevel2Stage5Room2MetatileMap[];
extern u8 gLevel2Stage5Room2BlockLayer[];
extern u8 gLevel2Stage5Room2BlockMetatiles[];
extern struct Door gLevel2Stage5Room2Doors[];
extern u8 gLevel2Stage5Room2Objects[];
struct RoomDef gLevel2Stage5Room2 ROOM_DEF(08370e08) = {
    .filler00 = { 2, 5, 2, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage5Room2MetatileMap,
    .blockLayer = gLevel2Stage5Room2BlockLayer,
    .blockMetatiles = gLevel2Stage5Room2BlockMetatiles,
    .width = 16,
    .height = 22,
    .bg2Palette = gUnk_08510FAC,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CACC,
    .bg3Tiles = gUnk_083EB890,
    .bg3Map = &gUnk_083B7994,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 7,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage5Room2Doors,
    .objects = gLevel2Stage5Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 296,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][5][3], 0x0837108C, section .room_def_0837108c */
extern u8 gLevel2Stage5Room3MetatileMap[];
extern u8 gLevel2Stage5Room3BlockLayer[];
extern u8 gLevel2Stage5Room3BlockMetatiles[];
extern struct Door gLevel2Stage5Room3Doors[];
extern u8 gLevel2Stage5Room3Objects[];
struct RoomDef gLevel2Stage5Room3 ROOM_DEF(0837108c) = {
    .filler00 = { 2, 5, 3, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage5Room3MetatileMap,
    .blockLayer = gLevel2Stage5Room3BlockLayer,
    .blockMetatiles = gLevel2Stage5Room3BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0850AA6C,
    .bg2Tiles = gUnk_0843D610,
    .metatileTiles = gUnk_083B1058,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CACC,
    .bg3Tiles = gUnk_083EB890,
    .bg3Map = &gUnk_083B7994,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 4,
    .objectsSortedByY = 0,
    .bgAnimSet = 6,
    .unk42 = 0,
    .doors = gLevel2Stage5Room3Doors,
    .objects = gLevel2Stage5Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 120,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][5][4], 0x08371388, section .room_def_08371388 */
extern u8 gLevel2Stage5Room4MetatileMap[];
extern u8 gLevel2Stage5Room4BlockLayer[];
extern u8 gLevel2Stage5Room4BlockMetatiles[];
extern struct Door gLevel2Stage5Room4Doors[];
extern u8 gLevel2Stage5Room4Objects[];
struct RoomDef gLevel2Stage5Room4 ROOM_DEF(08371388) = {
    .filler00 = { 2, 5, 4, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage5Room4MetatileMap,
    .blockLayer = gLevel2Stage5Room4BlockLayer,
    .blockMetatiles = gLevel2Stage5Room4BlockMetatiles,
    .width = 16,
    .height = 36,
    .bg2Palette = gUnk_0853DBB8,
    .bg2Tiles = gUnk_0843FC10,
    .metatileTiles = gUnk_083B18FC,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853CACC,
    .bg3Tiles = gUnk_083EB890,
    .bg3Map = &gUnk_083B7994,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 8,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage5Room4Doors,
    .objects = gLevel2Stage5Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 169,
    .entryY = 504,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][5][5], 0x08371728, section .room_def_08371728 */
extern u8 gLevel2Stage5Room5MetatileMap[];
extern u8 gLevel2Stage5Room5BlockLayer[];
extern u8 gLevel2Stage5Room5BlockMetatiles[];
extern struct Door gLevel2Stage5Room5Doors[];
extern u8 gLevel2Stage5Room5Objects[];
struct RoomDef gLevel2Stage5Room5 ROOM_DEF(08371728) = {
    .filler00 = { 2, 5, 5, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage5Room5MetatileMap,
    .blockLayer = gLevel2Stage5Room5BlockLayer,
    .blockMetatiles = gLevel2Stage5Room5BlockMetatiles,
    .width = 47,
    .height = 11,
    .bg2Palette = gLevel2Stage5Room5Bg2Palette,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gLevel2Stage5Room5Bg3Palette,
    .bg3Tiles = gUnk_0849BE08,
    .bg3Map = &gUnk_0849ABCC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 11,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage5Room5Doors,
    .objects = gLevel2Stage5Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 137,
    .unk54 = 6,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][5][6], 0x0837181C, section .room_def_0837181c */
extern u8 gLevel2Stage5Room6MetatileMap[];
extern u8 gLevel2Stage5Room6BlockLayer[];
extern u8 gLevel2Stage5Room6BlockMetatiles[];
struct RoomDef gLevel2Stage5Room6 ROOM_DEF(0837181c) = {
    .filler00 = { 2, 5, 6, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage5Room6MetatileMap,
    .blockLayer = gLevel2Stage5Room6BlockLayer,
    .blockMetatiles = gLevel2Stage5Room6BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08510FAC,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849A83C,
    .bg3Tiles = gUnk_0849BE08,
    .bg3Map = &gUnk_0849ABCC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 24,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][5][7], 0x08371D90, section .room_def_08371d90 */
extern u8 gLevel2Stage5Room7MetatileMap[];
extern u8 gLevel2Stage5Room7BlockLayer[];
extern u8 gLevel2Stage5Room7BlockMetatiles[];
extern struct Door gLevel2Stage5Room7Doors[];
extern u8 gLevel2Stage5Room7Objects[];
struct RoomDef gLevel2Stage5Room7 ROOM_DEF(08371d90) = {
    .filler00 = { 2, 5, 7, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage5Room7MetatileMap,
    .blockLayer = gLevel2Stage5Room7BlockLayer,
    .blockMetatiles = gLevel2Stage5Room7BlockMetatiles,
    .width = 16,
    .height = 78,
    .bg2Palette = gUnk_0853DBB8,
    .bg2Tiles = gUnk_0843FC10,
    .metatileTiles = gUnk_083B18FC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CACC,
    .bg3Tiles = gUnk_083EB890,
    .bg3Map = &gUnk_083B7994,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 21,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage5Room7Doors,
    .objects = gLevel2Stage5Room7Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 88,
    .entryY = 1208,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][5][8], 0x08372040, section .room_def_08372040 */
extern u8 gLevel2Stage5Room8MetatileMap[];
extern u8 gLevel2Stage5Room8BlockLayer[];
extern u8 gLevel2Stage5Room8BlockMetatiles[];
extern struct Door gLevel2Stage5Room8Doors[];
extern u8 gLevel2Stage5Room8Objects[];
struct RoomDef gLevel2Stage5Room8 ROOM_DEF(08372040) = {
    .filler00 = { 2, 5, 8, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage5Room8MetatileMap,
    .blockLayer = gLevel2Stage5Room8BlockLayer,
    .blockMetatiles = gLevel2Stage5Room8BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08510FAC,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CACC,
    .bg3Tiles = gUnk_083EB890,
    .bg3Map = &gUnk_083B7994,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage5Room8Doors,
    .objects = gLevel2Stage5Room8Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 73,
    .entryY = 121,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][5][9], 0x08372240, section .room_def_08372240 */
extern u8 gLevel2Stage5Room9MetatileMap[];
extern u8 gLevel2Stage5Room9BlockLayer[];
extern u8 gLevel2Stage5Room9BlockMetatiles[];
extern struct Door gLevel2Stage5Room9Doors[];
extern u8 gLevel2Stage5Room9Objects[];
struct RoomDef gLevel2Stage5Room9 ROOM_DEF(08372240) = {
    .filler00 = { 2, 5, 9, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage5Room9MetatileMap,
    .blockLayer = gLevel2Stage5Room9BlockLayer,
    .blockMetatiles = gLevel2Stage5Room9BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08510FAC,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CACC,
    .bg3Tiles = gUnk_083EB890,
    .bg3Map = &gUnk_083B7994,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel2Stage5Room9Doors,
    .objects = gLevel2Stage5Room9Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 201,
    .entryY = 72,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[2][6][0], 0x083723B0, section .room_def_083723b0 */
extern u8 gLevel2Stage6Room0MetatileMap[];
extern u8 gLevel2Stage6Room0BlockLayer[];
extern u8 gLevel2Stage6Room0BlockMetatiles[];
extern u8 gLevel2Stage6Room0Objects[];
struct RoomDef gLevel2Stage6Room0 ROOM_DEF(083723b0) = {
    .filler00 = { 2, 6, 0, 0 },
    .bgm = 30,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage6Room0MetatileMap,
    .blockLayer = gLevel2Stage6Room0BlockLayer,
    .blockMetatiles = gLevel2Stage6Room0BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0849AA04,
    .bg2Tiles = gUnk_08442F18,
    .metatileTiles = gUnk_083B2254,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gLevel2Stage6Room0Bg3Palette,
    .bg3Tiles = gUnk_0844619C,
    .bg3Map = &gUnk_083CAF40,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = gLevel2Stage6Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 128,
    .entryY = 136,
    .unk54 = 7,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 5,
};

/* gRoomTable[2][6][1], 0x08372518, section .room_def_08372518 */
extern u8 gLevel2Stage6Room1MetatileMap[];
extern u8 gLevel2Stage6Room1BlockLayer[];
extern u8 gLevel2Stage6Room1BlockMetatiles[];
struct RoomDef gLevel2Stage6Room1 ROOM_DEF(08372518) = {
    .filler00 = { 2, 6, 1, 0 },
    .bgm = 30,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage6Room1MetatileMap,
    .blockLayer = gLevel2Stage6Room1BlockLayer,
    .blockMetatiles = gLevel2Stage6Room1BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0849AA04,
    .bg2Tiles = gUnk_08442F18,
    .metatileTiles = gUnk_083B2254,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DD00,
    .bg3Tiles = gUnk_0844619C,
    .bg3Map = &gUnk_083CAF40,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 5,
};

/* gRoomTable[2][6][2], 0x08372680, section .room_def_08372680 */
extern u8 gLevel2Stage6Room2MetatileMap[];
extern u8 gLevel2Stage6Room2BlockLayer[];
extern u8 gLevel2Stage6Room2BlockMetatiles[];
struct RoomDef gLevel2Stage6Room2 ROOM_DEF(08372680) = {
    .filler00 = { 2, 6, 2, 0 },
    .bgm = 30,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel2Stage6Room2MetatileMap,
    .blockLayer = gLevel2Stage6Room2BlockLayer,
    .blockMetatiles = gLevel2Stage6Room2BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0849AA04,
    .bg2Tiles = gUnk_08442F18,
    .metatileTiles = gUnk_083B2254,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gLevel2Stage6Room2Bg3Palette,
    .bg3Tiles = gUnk_0844619C,
    .bg3Map = &gUnk_083CAF40,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 5,
};

/* gRoomTable[3][0][0], 0x08372A78, section .room_def_08372a78 */
extern u8 gLevel3Stage0Room0MetatileMap[];
extern u8 gLevel3Stage0Room0BlockLayer[];
extern u8 gLevel3Stage0Room0BlockMetatiles[];
extern struct Door gLevel3Stage0Room0Doors[];
extern u8 gLevel3Stage0Room0Objects[];
struct RoomDef gLevel3Stage0Room0 ROOM_DEF(08372a78) = {
    .filler00 = { 3, 0, 0, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage0Room0MetatileMap,
    .blockLayer = gLevel3Stage0Room0BlockLayer,
    .blockMetatiles = gLevel3Stage0Room0BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853C8C4,
    .bg3Tiles = gUnk_083E2A30,
    .bg3Map = &gUnk_083B6770,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 11,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage0Room0Doors,
    .objects = gLevel3Stage0Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 56,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][0][1], 0x08372D58, section .room_def_08372d58 */
extern u8 gLevel3Stage0Room1MetatileMap[];
extern u8 gLevel3Stage0Room1BlockLayer[];
extern u8 gLevel3Stage0Room1BlockMetatiles[];
extern struct Door gLevel3Stage0Room1Doors[];
extern u8 gLevel3Stage0Room1Objects[];
struct RoomDef gLevel3Stage0Room1 ROOM_DEF(08372d58) = {
    .filler00 = { 3, 0, 1, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage0Room1MetatileMap,
    .blockLayer = gLevel3Stage0Room1BlockLayer,
    .blockMetatiles = gLevel3Stage0Room1BlockMetatiles,
    .width = 16,
    .height = 25,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 9,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage0Room1Doors,
    .objects = gLevel3Stage0Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 120,
    .entryY = 330,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][0][2], 0x0837301C, section .room_def_0837301c */
extern u8 gLevel3Stage0Room2MetatileMap[];
extern u8 gLevel3Stage0Room2BlockLayer[];
extern u8 gLevel3Stage0Room2BlockMetatiles[];
extern struct Door gLevel3Stage0Room2Doors[];
extern u8 gLevel3Stage0Room2Objects[];
struct RoomDef gLevel3Stage0Room2 ROOM_DEF(0837301c) = {
    .filler00 = { 3, 0, 2, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage0Room2MetatileMap,
    .blockLayer = gLevel3Stage0Room2BlockLayer,
    .blockMetatiles = gLevel3Stage0Room2BlockMetatiles,
    .width = 48,
    .height = 11,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E41C,
    .bg3Tiles = gUnk_0846F8DC,
    .bg3Map = &gUnk_083CEF10,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 9,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage0Room2Doors,
    .objects = gLevel3Stage0Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 121,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][0][3], 0x083736A8, section .room_def_083736a8 */
extern u8 gLevel3Stage0Room3MetatileMap[];
extern u8 gLevel3Stage0Room3BlockLayer[];
extern u8 gLevel3Stage0Room3BlockMetatiles[];
extern struct Door gLevel3Stage0Room3Doors[];
extern u8 gLevel3Stage0Room3Objects[];
struct RoomDef gLevel3Stage0Room3 ROOM_DEF(083736a8) = {
    .filler00 = { 3, 0, 3, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage0Room3MetatileMap,
    .blockLayer = gLevel3Stage0Room3BlockLayer,
    .blockMetatiles = gLevel3Stage0Room3BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E41C,
    .bg3Tiles = gUnk_0846F8DC,
    .bg3Map = &gUnk_083CEF10,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 14,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage0Room3Doors,
    .objects = gLevel3Stage0Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 39,
    .entryY = 122,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][0][4], 0x08373914, section .room_def_08373914 */
extern u8 gLevel3Stage0Room4MetatileMap[];
extern u8 gLevel3Stage0Room4BlockLayer[];
extern u8 gLevel3Stage0Room4BlockMetatiles[];
extern struct Door gLevel3Stage0Room4Doors[];
extern u8 gLevel3Stage0Room4Objects[];
struct RoomDef gLevel3Stage0Room4 ROOM_DEF(08373914) = {
    .filler00 = { 3, 0, 4, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage0Room4MetatileMap,
    .blockLayer = gLevel3Stage0Room4BlockLayer,
    .blockMetatiles = gLevel3Stage0Room4BlockMetatiles,
    .width = 32,
    .height = 11,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C8C4,
    .bg3Tiles = gUnk_083E2A30,
    .bg3Map = &gUnk_083B6770,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage0Room4Doors,
    .objects = gLevel3Stage0Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][0][5], 0x08373AC4, section .room_def_08373ac4 */
extern u8 gLevel3Stage0Room5MetatileMap[];
extern u8 gLevel3Stage0Room5BlockLayer[];
extern u8 gLevel3Stage0Room5BlockMetatiles[];
extern struct Door gLevel3Stage0Room5Doors[];
extern u8 gLevel3Stage0Room5Objects[];
struct RoomDef gLevel3Stage0Room5 ROOM_DEF(08373ac4) = {
    .filler00 = { 3, 0, 5, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage0Room5MetatileMap,
    .blockLayer = gLevel3Stage0Room5BlockLayer,
    .blockMetatiles = gLevel3Stage0Room5BlockMetatiles,
    .width = 17,
    .height = 12,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage0Room5Doors,
    .objects = gLevel3Stage0Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 120,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][1][0], 0x0837410C, section .room_def_0837410c */
extern u8 gLevel3Stage1Room0MetatileMap[];
extern u8 gLevel3Stage1Room0BlockLayer[];
extern u8 gLevel3Stage1Room0BlockMetatiles[];
extern struct Door gLevel3Stage1Room0Doors[];
extern u8 gLevel3Stage1Room0Objects[];
struct RoomDef gLevel3Stage1Room0 ROOM_DEF(0837410c) = {
    .filler00 = { 3, 1, 0, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage1Room0MetatileMap,
    .blockLayer = gLevel3Stage1Room0BlockLayer,
    .blockMetatiles = gLevel3Stage1Room0BlockMetatiles,
    .width = 48,
    .height = 23,
    .bg2Palette = gUnk_084B57E8,
    .bg2Tiles = gUnk_084B5F90,
    .metatileTiles = gUnk_084B58AC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 8,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage1Room0Doors,
    .objects = gLevel3Stage1Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 105,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][1][1], 0x083742CC, section .room_def_083742cc */
extern u8 gLevel3Stage1Room1MetatileMap[];
extern u8 gLevel3Stage1Room1BlockLayer[];
extern u8 gLevel3Stage1Room1BlockMetatiles[];
extern struct Door gLevel3Stage1Room1Doors[];
extern u8 gLevel3Stage1Room1Objects[];
struct RoomDef gLevel3Stage1Room1 ROOM_DEF(083742cc) = {
    .filler00 = { 3, 1, 1, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage1Room1MetatileMap,
    .blockLayer = gLevel3Stage1Room1BlockLayer,
    .blockMetatiles = gLevel3Stage1Room1BlockMetatiles,
    .width = 17,
    .height = 12,
    .bg2Palette = gUnk_08497C74,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0853D438,
    .bg3Tiles = gUnk_084278D8,
    .bg3Map = &gUnk_083CA620,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 3,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage1Room1Doors,
    .objects = gLevel3Stage1Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 120,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][1][2], 0x083747D0, section .room_def_083747d0 */
extern u8 gLevel3Stage1Room2MetatileMap[];
extern u8 gLevel3Stage1Room2BlockLayer[];
extern u8 gLevel3Stage1Room2BlockMetatiles[];
extern struct Door gLevel3Stage1Room2Doors[];
extern u8 gLevel3Stage1Room2Objects[];
struct RoomDef gLevel3Stage1Room2 ROOM_DEF(083747d0) = {
    .filler00 = { 3, 1, 2, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage1Room2MetatileMap,
    .blockLayer = gLevel3Stage1Room2BlockLayer,
    .blockMetatiles = gLevel3Stage1Room2BlockMetatiles,
    .width = 32,
    .height = 24,
    .bg2Palette = gUnk_08497C74,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D438,
    .bg3Tiles = gUnk_084278D8,
    .bg3Map = &gUnk_083CA620,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 8,
    .objectsSortedByY = 0,
    .bgAnimSet = 8,
    .unk42 = 0,
    .doors = gLevel3Stage1Room2Doors,
    .objects = gLevel3Stage1Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 57,
    .entryY = 57,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][1][3], 0x08374B28, section .room_def_08374b28 */
extern u8 gLevel3Stage1Room3MetatileMap[];
extern u8 gLevel3Stage1Room3BlockLayer[];
extern u8 gLevel3Stage1Room3BlockMetatiles[];
extern struct Door gLevel3Stage1Room3Doors[];
extern u8 gLevel3Stage1Room3Objects[];
struct RoomDef gLevel3Stage1Room3 ROOM_DEF(08374b28) = {
    .filler00 = { 3, 1, 3, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage1Room3MetatileMap,
    .blockLayer = gLevel3Stage1Room3BlockLayer,
    .blockMetatiles = gLevel3Stage1Room3BlockMetatiles,
    .width = 64,
    .height = 11,
    .bg2Palette = gUnk_08497C74,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D438,
    .bg3Tiles = gUnk_084278D8,
    .bg3Map = &gUnk_083CA620,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 12,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage1Room3Doors,
    .objects = gLevel3Stage1Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 88,
    .entryY = 88,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][1][4], 0x08374C74, section .room_def_08374c74 */
extern u8 gLevel3Stage1Room4MetatileMap[];
extern u8 gLevel3Stage1Room4BlockLayer[];
extern u8 gLevel3Stage1Room4BlockMetatiles[];
extern struct Door gLevel3Stage1Room4Doors[];
extern u8 gLevel3Stage1Room4Objects[];
struct RoomDef gLevel3Stage1Room4 ROOM_DEF(08374c74) = {
    .filler00 = { 3, 1, 4, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage1Room4MetatileMap,
    .blockLayer = gLevel3Stage1Room4BlockLayer,
    .blockMetatiles = gLevel3Stage1Room4BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08497C74,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C9C8,
    .bg3Tiles = gUnk_083E6548,
    .bg3Map = &gUnk_083B708C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 5,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage1Room4Doors,
    .objects = gLevel3Stage1Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][1][5], 0x08374DBC, section .room_def_08374dbc */
extern u8 gLevel3Stage1Room5MetatileMap[];
extern u8 gLevel3Stage1Room5BlockLayer[];
extern u8 gLevel3Stage1Room5BlockMetatiles[];
extern struct Door gLevel3Stage1Room5Doors[];
extern u8 gLevel3Stage1Room5Objects[];
struct RoomDef gLevel3Stage1Room5 ROOM_DEF(08374dbc) = {
    .filler00 = { 3, 1, 5, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage1Room5MetatileMap,
    .blockLayer = gLevel3Stage1Room5BlockLayer,
    .blockMetatiles = gLevel3Stage1Room5BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08497C74,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C9C8,
    .bg3Tiles = gUnk_083E6548,
    .bg3Map = &gUnk_083B708C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage1Room5Doors,
    .objects = gLevel3Stage1Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 120,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][2][0], 0x0837531C, section .room_def_0837531c */
extern u8 gLevel3Stage2Room0MetatileMap[];
extern u8 gLevel3Stage2Room0BlockLayer[];
extern u8 gLevel3Stage2Room0BlockMetatiles[];
extern struct Door gLevel3Stage2Room0Doors[];
extern u8 gLevel3Stage2Room0Objects[];
struct RoomDef gLevel3Stage2Room0 ROOM_DEF(0837531c) = {
    .filler00 = { 3, 2, 0, 0 },
    .bgm = 2,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage2Room0MetatileMap,
    .blockLayer = gLevel3Stage2Room0BlockLayer,
    .blockMetatiles = gLevel3Stage2Room0BlockMetatiles,
    .width = 33,
    .height = 11,
    .bg2Palette = gUnk_0853DAB4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CEDC,
    .bg3Tiles = gUnk_08401D3C,
    .bg3Map = &gUnk_083BADEC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 6,
    .objectsSortedByY = 0,
    .bgAnimSet = 1,
    .unk42 = 0,
    .doors = gLevel3Stage2Room0Doors,
    .objects = gLevel3Stage2Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][2][1], 0x083761D0, section .room_def_083761d0 */
extern u8 gLevel3Stage2Room1MetatileMap[];
extern u8 gLevel3Stage2Room1BlockLayer[];
extern u8 gLevel3Stage2Room1BlockMetatiles[];
extern struct Door gLevel3Stage2Room1Doors[];
extern u8 gLevel3Stage2Room1Objects[];
struct RoomDef gLevel3Stage2Room1 ROOM_DEF(083761d0) = {
    .filler00 = { 3, 2, 1, 0 },
    .bgm = 2,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage2Room1MetatileMap,
    .blockLayer = gLevel3Stage2Room1BlockLayer,
    .blockMetatiles = gLevel3Stage2Room1BlockMetatiles,
    .width = 120,
    .height = 28,
    .bg2Palette = gUnk_0852D7AC,
    .bg2Tiles = gUnk_08536548,
    .metatileTiles = gUnk_0852F03C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_08539BA4,
    .bg3Tiles = gUnk_083FE694,
    .bg3Map = &gUnk_083BA534,
    .bg3BorderX = 0,
    .bg3BorderY = 0,
    .driftObjectIndex = 20,
    .doorCount = 3,
    .objectCount = 20,
    .objectsSortedByY = 0,
    .bgAnimSet = 9,
    .unk42 = 0,
    .doors = gLevel3Stage2Room1Doors,
    .objects = gLevel3Stage2Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 48,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 1,
    .unk57 = 0,
};

/* gRoomTable[3][2][2], 0x08376B24, section .room_def_08376b24 */
extern u8 gLevel3Stage2Room2MetatileMap[];
extern u8 gLevel3Stage2Room2BlockLayer[];
extern u8 gLevel3Stage2Room2BlockMetatiles[];
extern struct Door gLevel3Stage2Room2Doors[];
extern u8 gLevel3Stage2Room2Objects[];
struct RoomDef gLevel3Stage2Room2 ROOM_DEF(08376b24) = {
    .filler00 = { 3, 2, 2, 0 },
    .bgm = 2,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage2Room2MetatileMap,
    .blockLayer = gLevel3Stage2Room2BlockLayer,
    .blockMetatiles = gLevel3Stage2Room2BlockMetatiles,
    .width = 128,
    .height = 13,
    .bg2Palette = gUnk_0852D624,
    .bg2Tiles = gUnk_08531C38,
    .metatileTiles = gUnk_0852E06C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_08539BA4,
    .bg3Tiles = gUnk_083FE694,
    .bg3Map = &gUnk_083BA534,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 19,
    .doorCount = 1,
    .objectCount = 19,
    .objectsSortedByY = 0,
    .bgAnimSet = 10,
    .unk42 = 0,
    .doors = gLevel3Stage2Room2Doors,
    .objects = gLevel3Stage2Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 135,
    .entryY = 121,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 1,
    .unk57 = 0,
};

/* gRoomTable[3][2][3], 0x08376D94, section .room_def_08376d94 */
extern u8 gLevel3Stage2Room3MetatileMap[];
extern u8 gLevel3Stage2Room3BlockLayer[];
extern u8 gLevel3Stage2Room3BlockMetatiles[];
extern struct Door gLevel3Stage2Room3Doors[];
extern u8 gLevel3Stage2Room3Objects[];
struct RoomDef gLevel3Stage2Room3 ROOM_DEF(08376d94) = {
    .filler00 = { 3, 2, 3, 0 },
    .bgm = 2,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage2Room3MetatileMap,
    .blockLayer = gLevel3Stage2Room3BlockLayer,
    .blockMetatiles = gLevel3Stage2Room3BlockMetatiles,
    .width = 16,
    .height = 24,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08539BA4,
    .bg3Tiles = gUnk_083FE694,
    .bg3Map = &gUnk_083BA534,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 5,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage2Room3Doors,
    .objects = gLevel3Stage2Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 248,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][2][4], 0x08376F18, section .room_def_08376f18 */
extern u8 gLevel3Stage2Room4MetatileMap[];
extern u8 gLevel3Stage2Room4BlockLayer[];
extern u8 gLevel3Stage2Room4BlockMetatiles[];
extern struct Door gLevel3Stage2Room4Doors[];
extern u8 gLevel3Stage2Room4Objects[];
struct RoomDef gLevel3Stage2Room4 ROOM_DEF(08376f18) = {
    .filler00 = { 3, 2, 4, 0 },
    .bgm = 2,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage2Room4MetatileMap,
    .blockLayer = gLevel3Stage2Room4BlockLayer,
    .blockMetatiles = gLevel3Stage2Room4BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08539BA4,
    .bg3Tiles = gUnk_083FE694,
    .bg3Map = &gUnk_083BA534,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage2Room4Doors,
    .objects = gLevel3Stage2Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 24,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][2][5], 0x08377144, section .room_def_08377144 */
extern u8 gLevel3Stage2Room5MetatileMap[];
extern u8 gLevel3Stage2Room5BlockLayer[];
extern u8 gLevel3Stage2Room5BlockMetatiles[];
extern struct Door gLevel3Stage2Room5Doors[];
extern u8 gLevel3Stage2Room5Objects[];
struct RoomDef gLevel3Stage2Room5 ROOM_DEF(08377144) = {
    .filler00 = { 3, 2, 5, 0 },
    .bgm = 2,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage2Room5MetatileMap,
    .blockLayer = gLevel3Stage2Room5BlockLayer,
    .blockMetatiles = gLevel3Stage2Room5BlockMetatiles,
    .width = 16,
    .height = 13,
    .bg2Palette = gUnk_0852D7AC,
    .bg2Tiles = gUnk_08536548,
    .metatileTiles = gUnk_0852F03C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_08539BA4,
    .bg3Tiles = gUnk_083FE694,
    .bg3Map = &gUnk_083BA534,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 9,
    .unk42 = 0,
    .doors = gLevel3Stage2Room5Doors,
    .objects = gLevel3Stage2Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 1,
    .unk57 = 0,
};

/* gRoomTable[3][2][6], 0x08377334, section .room_def_08377334 */
extern u8 gLevel3Stage2Room6MetatileMap[];
extern u8 gLevel3Stage2Room6BlockLayer[];
extern u8 gLevel3Stage2Room6BlockMetatiles[];
extern struct Door gLevel3Stage2Room6Doors[];
extern u8 gLevel3Stage2Room6Objects[];
struct RoomDef gLevel3Stage2Room6 ROOM_DEF(08377334) = {
    .filler00 = { 3, 2, 6, 0 },
    .bgm = 2,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage2Room6MetatileMap,
    .blockLayer = gLevel3Stage2Room6BlockLayer,
    .blockMetatiles = gLevel3Stage2Room6BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0852D624,
    .bg2Tiles = gUnk_08531C38,
    .metatileTiles = gUnk_0852E06C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08539BA4,
    .bg3Tiles = gUnk_083FE694,
    .bg3Map = &gUnk_083BA534,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 5,
    .objectsSortedByY = 0,
    .bgAnimSet = 10,
    .unk42 = 0,
    .doors = gLevel3Stage2Room6Doors,
    .objects = gLevel3Stage2Room6Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 88,
    .entryY = 120,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][3][0], 0x08377AF0, section .room_def_08377af0 */
extern u8 gLevel3Stage3Room0MetatileMap[];
extern u8 gLevel3Stage3Room0BlockLayer[];
extern u8 gLevel3Stage3Room0BlockMetatiles[];
extern struct Door gLevel3Stage3Room0Doors[];
extern u8 gLevel3Stage3Room0Objects[];
struct RoomDef gLevel3Stage3Room0 ROOM_DEF(08377af0) = {
    .filler00 = { 3, 3, 0, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage3Room0MetatileMap,
    .blockLayer = gLevel3Stage3Room0BlockLayer,
    .blockMetatiles = gLevel3Stage3Room0BlockMetatiles,
    .width = 64,
    .height = 18,
    .bg2Palette = gUnk_084BB270,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_084CE684,
    .bg3Tiles = gUnk_084CF0A8,
    .bg3Map = &gUnk_084CE788,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 17,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage3Room0Doors,
    .objects = gLevel3Stage3Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][3][1], 0x08377FBC, section .room_def_08377fbc */
extern u8 gLevel3Stage3Room1MetatileMap[];
extern u8 gLevel3Stage3Room1BlockLayer[];
extern u8 gLevel3Stage3Room1BlockMetatiles[];
extern struct Door gLevel3Stage3Room1Doors[];
extern u8 gLevel3Stage3Room1Objects[];
struct RoomDef gLevel3Stage3Room1 ROOM_DEF(08377fbc) = {
    .filler00 = { 3, 3, 1, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage3Room1MetatileMap,
    .blockLayer = gLevel3Stage3Room1BlockLayer,
    .blockMetatiles = gLevel3Stage3Room1BlockMetatiles,
    .width = 64,
    .height = 11,
    .bg2Palette = gUnk_084BB270,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_084CE684,
    .bg3Tiles = gUnk_084CF0A8,
    .bg3Map = &gUnk_084CE788,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 13,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage3Room1Doors,
    .objects = gLevel3Stage3Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 41,
    .entryY = 122,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][3][2], 0x08378A80, section .room_def_08378a80 */
extern u8 gLevel3Stage3Room2MetatileMap[];
extern u8 gLevel3Stage3Room2BlockLayer[];
extern u8 gLevel3Stage3Room2BlockMetatiles[];
extern struct Door gLevel3Stage3Room2Doors[];
extern u8 gLevel3Stage3Room2Objects[];
struct RoomDef gLevel3Stage3Room2 ROOM_DEF(08378a80) = {
    .filler00 = { 3, 3, 2, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage3Room2MetatileMap,
    .blockLayer = gLevel3Stage3Room2BlockLayer,
    .blockMetatiles = gLevel3Stage3Room2BlockMetatiles,
    .width = 16,
    .height = 59,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gLevel3Stage3Room2Bg3Palette,
    .bg3Tiles = gUnk_0851EF74,
    .bg3Map = &gUnk_08517F00,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 15,
    .objectsSortedByY = 1,
    .bgAnimSet = 3,
    .unk42 = 0,
    .doors = gLevel3Stage3Room2Doors,
    .objects = gLevel3Stage3Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 56,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][3][3], 0x08378EB0, section .room_def_08378eb0 */
extern u8 gLevel3Stage3Room3MetatileMap[];
extern u8 gLevel3Stage3Room3BlockLayer[];
extern u8 gLevel3Stage3Room3BlockMetatiles[];
extern struct Door gLevel3Stage3Room3Doors[];
extern u8 gLevel3Stage3Room3Objects[];
struct RoomDef gLevel3Stage3Room3 ROOM_DEF(08378eb0) = {
    .filler00 = { 3, 3, 3, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage3Room3MetatileMap,
    .blockLayer = gLevel3Stage3Room3BlockLayer,
    .blockMetatiles = gLevel3Stage3Room3BlockMetatiles,
    .width = 48,
    .height = 11,
    .bg2Palette = gUnk_0849A5F0,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C7C0,
    .bg3Tiles = gUnk_083DB044,
    .bg3Map = &gUnk_083B5E54,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 8,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage3Room3Doors,
    .objects = gLevel3Stage3Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][3][4], 0x08379218, section .room_def_08379218 */
extern u8 gLevel3Stage3Room4MetatileMap[];
extern u8 gLevel3Stage3Room4BlockLayer[];
extern u8 gLevel3Stage3Room4BlockMetatiles[];
extern struct Door gLevel3Stage3Room4Doors[];
extern u8 gLevel3Stage3Room4Objects[];
struct RoomDef gLevel3Stage3Room4 ROOM_DEF(08379218) = {
    .filler00 = { 3, 3, 4, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage3Room4MetatileMap,
    .blockLayer = gLevel3Stage3Room4BlockLayer,
    .blockMetatiles = gLevel3Stage3Room4BlockMetatiles,
    .width = 32,
    .height = 11,
    .bg2Palette = gUnk_0849A5F0,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C7C0,
    .bg3Tiles = gUnk_083DB044,
    .bg3Map = &gUnk_083B5E54,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 3,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage3Room4Doors,
    .objects = gLevel3Stage3Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][4][0], 0x08379B84, section .room_def_08379b84 */
extern u8 gLevel3Stage4Room0MetatileMap[];
extern u8 gLevel3Stage4Room0BlockLayer[];
extern u8 gLevel3Stage4Room0BlockMetatiles[];
extern struct Door gLevel3Stage4Room0Doors[];
extern u8 gLevel3Stage4Room0Objects[];
struct RoomDef gLevel3Stage4Room0 ROOM_DEF(08379b84) = {
    .filler00 = { 3, 4, 0, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage4Room0MetatileMap,
    .blockLayer = gLevel3Stage4Room0BlockLayer,
    .blockMetatiles = gLevel3Stage4Room0BlockMetatiles,
    .width = 97,
    .height = 23,
    .bg2Palette = gUnk_0852D314,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853C8C4,
    .bg3Tiles = gUnk_083E2A30,
    .bg3Map = &gUnk_083B6770,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 16,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage4Room0Doors,
    .objects = gLevel3Stage4Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 296,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][4][1], 0x08379E60, section .room_def_08379e60 */
extern u8 gLevel3Stage4Room1MetatileMap[];
extern u8 gLevel3Stage4Room1BlockLayer[];
extern u8 gLevel3Stage4Room1BlockMetatiles[];
extern struct Door gLevel3Stage4Room1Doors[];
extern u8 gLevel3Stage4Room1Objects[];
struct RoomDef gLevel3Stage4Room1 ROOM_DEF(08379e60) = {
    .filler00 = { 3, 4, 1, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage4Room1MetatileMap,
    .blockLayer = gLevel3Stage4Room1BlockLayer,
    .blockMetatiles = gLevel3Stage4Room1BlockMetatiles,
    .width = 32,
    .height = 11,
    .bg2Palette = gUnk_0852D314,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 6,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage4Room1Doors,
    .objects = gLevel3Stage4Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 73,
    .entryY = 42,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][4][2], 0x0837A0CC, section .room_def_0837a0cc */
extern u8 gLevel3Stage4Room2MetatileMap[];
extern u8 gLevel3Stage4Room2BlockLayer[];
extern u8 gLevel3Stage4Room2BlockMetatiles[];
extern struct Door gLevel3Stage4Room2Doors[];
extern u8 gLevel3Stage4Room2Objects[];
struct RoomDef gLevel3Stage4Room2 ROOM_DEF(0837a0cc) = {
    .filler00 = { 3, 4, 2, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage4Room2MetatileMap,
    .blockLayer = gLevel3Stage4Room2BlockLayer,
    .blockMetatiles = gLevel3Stage4Room2BlockMetatiles,
    .width = 30,
    .height = 11,
    .bg2Palette = gUnk_0852D314,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853C9C8,
    .bg3Tiles = gUnk_083E6548,
    .bg3Map = &gUnk_083B708C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 5,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage4Room2Doors,
    .objects = gLevel3Stage4Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 104,
    .entryY = 120,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][4][3], 0x0837A52C, section .room_def_0837a52c */
extern u8 gLevel3Stage4Room3MetatileMap[];
extern u8 gLevel3Stage4Room3BlockLayer[];
extern u8 gLevel3Stage4Room3BlockMetatiles[];
extern struct Door gLevel3Stage4Room3Doors[];
extern u8 gLevel3Stage4Room3Objects[];
struct RoomDef gLevel3Stage4Room3 ROOM_DEF(0837a52c) = {
    .filler00 = { 3, 4, 3, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage4Room3MetatileMap,
    .blockLayer = gLevel3Stage4Room3BlockLayer,
    .blockMetatiles = gLevel3Stage4Room3BlockMetatiles,
    .width = 64,
    .height = 11,
    .bg2Palette = gUnk_0852D314,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CACC,
    .bg3Tiles = gUnk_083EB890,
    .bg3Map = &gUnk_083B7994,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 13,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage4Room3Doors,
    .objects = gLevel3Stage4Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 57,
    .entryY = 137,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][4][4], 0x0837AB88, section .room_def_0837ab88 */
extern u8 gLevel3Stage4Room4MetatileMap[];
extern u8 gLevel3Stage4Room4BlockLayer[];
extern u8 gLevel3Stage4Room4BlockMetatiles[];
extern struct Door gLevel3Stage4Room4Doors[];
extern u8 gLevel3Stage4Room4Objects[];
struct RoomDef gLevel3Stage4Room4 ROOM_DEF(0837ab88) = {
    .filler00 = { 3, 4, 4, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage4Room4MetatileMap,
    .blockLayer = gLevel3Stage4Room4BlockLayer,
    .blockMetatiles = gLevel3Stage4Room4BlockMetatiles,
    .width = 57,
    .height = 27,
    .bg2Palette = gUnk_0852D314,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 13,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage4Room4Doors,
    .objects = gLevel3Stage4Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 376,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][4][5], 0x0837AD38, section .room_def_0837ad38 */
extern u8 gLevel3Stage4Room5MetatileMap[];
extern u8 gLevel3Stage4Room5BlockLayer[];
extern u8 gLevel3Stage4Room5BlockMetatiles[];
extern struct Door gLevel3Stage4Room5Doors[];
extern u8 gLevel3Stage4Room5Objects[];
struct RoomDef gLevel3Stage4Room5 ROOM_DEF(0837ad38) = {
    .filler00 = { 3, 4, 5, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage4Room5MetatileMap,
    .blockLayer = gLevel3Stage4Room5BlockLayer,
    .blockMetatiles = gLevel3Stage4Room5BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0852D314,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage4Room5Doors,
    .objects = gLevel3Stage4Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 103,
    .entryY = 105,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][4][6], 0x0837AF10, section .room_def_0837af10 */
extern u8 gLevel3Stage4Room6MetatileMap[];
extern u8 gLevel3Stage4Room6BlockLayer[];
extern u8 gLevel3Stage4Room6BlockMetatiles[];
extern struct Door gLevel3Stage4Room6Doors[];
extern u8 gLevel3Stage4Room6Objects[];
struct RoomDef gLevel3Stage4Room6 ROOM_DEF(0837af10) = {
    .filler00 = { 3, 4, 6, 0 },
    .bgm = 37,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage4Room6MetatileMap,
    .blockLayer = gLevel3Stage4Room6BlockLayer,
    .blockMetatiles = gLevel3Stage4Room6BlockMetatiles,
    .width = 18,
    .height = 12,
    .bg2Palette = gUnk_0852D314,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 24,
    .borderY = 8,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage4Room6Doors,
    .objects = gLevel3Stage4Room6Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 136,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][5][0], 0x0837B7CC, section .room_def_0837b7cc */
extern u8 gLevel3Stage5Room0MetatileMap[];
extern u8 gLevel3Stage5Room0BlockLayer[];
extern u8 gLevel3Stage5Room0BlockMetatiles[];
extern struct Door gLevel3Stage5Room0Doors[];
extern u8 gLevel3Stage5Room0Objects[];
struct RoomDef gLevel3Stage5Room0 ROOM_DEF(0837b7cc) = {
    .filler00 = { 3, 5, 0, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage5Room0MetatileMap,
    .blockLayer = gLevel3Stage5Room0BlockLayer,
    .blockMetatiles = gLevel3Stage5Room0BlockMetatiles,
    .width = 32,
    .height = 23,
    .bg2Palette = gUnk_084BAEE0,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D230,
    .bg3Tiles = gUnk_0841AD98,
    .bg3Map = &gUnk_083C93D8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 11,
    .objectsSortedByY = 0,
    .bgAnimSet = 8,
    .unk42 = 0,
    .doors = gLevel3Stage5Room0Doors,
    .objects = gLevel3Stage5Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 57,
    .entryY = 121,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][5][1], 0x0837BB9C, section .room_def_0837bb9c */
extern u8 gLevel3Stage5Room1MetatileMap[];
extern u8 gLevel3Stage5Room1BlockLayer[];
extern u8 gLevel3Stage5Room1BlockMetatiles[];
extern struct Door gLevel3Stage5Room1Doors[];
extern u8 gLevel3Stage5Room1Objects[];
struct RoomDef gLevel3Stage5Room1 ROOM_DEF(0837bb9c) = {
    .filler00 = { 3, 5, 1, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage5Room1MetatileMap,
    .blockLayer = gLevel3Stage5Room1BlockLayer,
    .blockMetatiles = gLevel3Stage5Room1BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gUnk_0849A778,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_084BACD8,
    .bg3Tiles = gUnk_08421338,
    .bg3Map = &gUnk_083C9CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 15,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage5Room1Doors,
    .objects = gLevel3Stage5Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 105,
    .unk54 = 6,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][5][2], 0x0837BC90, section .room_def_0837bc90 */
extern u8 gLevel3Stage5Room2MetatileMap[];
extern u8 gLevel3Stage5Room2BlockLayer[];
extern u8 gLevel3Stage5Room2BlockMetatiles[];
struct RoomDef gLevel3Stage5Room2 ROOM_DEF(0837bc90) = {
    .filler00 = { 3, 5, 2, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage5Room2MetatileMap,
    .blockLayer = gLevel3Stage5Room2BlockLayer,
    .blockMetatiles = gLevel3Stage5Room2BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08497C74,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D334,
    .bg3Tiles = gUnk_08421338,
    .bg3Map = &gUnk_083C9CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 24,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][5][3], 0x0837BFA8, section .room_def_0837bfa8 */
extern u8 gLevel3Stage5Room3MetatileMap[];
extern u8 gLevel3Stage5Room3BlockLayer[];
extern u8 gLevel3Stage5Room3BlockMetatiles[];
extern struct Door gLevel3Stage5Room3Doors[];
extern u8 gLevel3Stage5Room3Objects[];
struct RoomDef gLevel3Stage5Room3 ROOM_DEF(0837bfa8) = {
    .filler00 = { 3, 5, 3, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage5Room3MetatileMap,
    .blockLayer = gLevel3Stage5Room3BlockLayer,
    .blockMetatiles = gLevel3Stage5Room3BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08497C74,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D230,
    .bg3Tiles = gUnk_0841AD98,
    .bg3Map = &gUnk_083C93D8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage5Room3Doors,
    .objects = gLevel3Stage5Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 39,
    .entryY = 42,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][5][4], 0x0837C288, section .room_def_0837c288 */
extern u8 gLevel3Stage5Room4MetatileMap[];
extern u8 gLevel3Stage5Room4BlockLayer[];
extern u8 gLevel3Stage5Room4BlockMetatiles[];
extern struct Door gLevel3Stage5Room4Doors[];
extern u8 gLevel3Stage5Room4Objects[];
struct RoomDef gLevel3Stage5Room4 ROOM_DEF(0837c288) = {
    .filler00 = { 3, 5, 4, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage5Room4MetatileMap,
    .blockLayer = gLevel3Stage5Room4BlockLayer,
    .blockMetatiles = gLevel3Stage5Room4BlockMetatiles,
    .width = 16,
    .height = 23,
    .bg2Palette = gUnk_084BAEE0,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D230,
    .bg3Tiles = gUnk_0841AD98,
    .bg3Map = &gUnk_083C93D8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 8,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage5Room4Doors,
    .objects = gLevel3Stage5Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 89,
    .entryY = 329,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][5][5], 0x0837C424, section .room_def_0837c424 */
extern u8 gLevel3Stage5Room5MetatileMap[];
extern u8 gLevel3Stage5Room5BlockLayer[];
extern u8 gLevel3Stage5Room5BlockMetatiles[];
extern struct Door gLevel3Stage5Room5Doors[];
extern u8 gLevel3Stage5Room5Objects[];
struct RoomDef gLevel3Stage5Room5 ROOM_DEF(0837c424) = {
    .filler00 = { 3, 5, 5, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage5Room5MetatileMap,
    .blockLayer = gLevel3Stage5Room5BlockLayer,
    .blockMetatiles = gLevel3Stage5Room5BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0849A778,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084BACD8,
    .bg3Tiles = gUnk_08421338,
    .bg3Map = &gUnk_083C9CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage5Room5Doors,
    .objects = gLevel3Stage5Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 89,
    .entryY = 56,
    .unk54 = 6,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][5][6], 0x0837C520, section .room_def_0837c520 */
extern u8 gLevel3Stage5Room6MetatileMap[];
extern u8 gLevel3Stage5Room6BlockLayer[];
extern u8 gLevel3Stage5Room6BlockMetatiles[];
struct RoomDef gLevel3Stage5Room6 ROOM_DEF(0837c520) = {
    .filler00 = { 3, 5, 6, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage5Room6MetatileMap,
    .blockLayer = gLevel3Stage5Room6BlockLayer,
    .blockMetatiles = gLevel3Stage5Room6BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08497C74,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D334,
    .bg3Tiles = gUnk_08421338,
    .bg3Map = &gUnk_083C9CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 24,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][5][7], 0x0837C698, section .room_def_0837c698 */
extern u8 gLevel3Stage5Room7MetatileMap[];
extern u8 gLevel3Stage5Room7BlockLayer[];
extern u8 gLevel3Stage5Room7BlockMetatiles[];
extern struct Door gLevel3Stage5Room7Doors[];
extern u8 gLevel3Stage5Room7Objects[];
struct RoomDef gLevel3Stage5Room7 ROOM_DEF(0837c698) = {
    .filler00 = { 3, 5, 7, 0 },
    .bgm = 11,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage5Room7MetatileMap,
    .blockLayer = gLevel3Stage5Room7BlockLayer,
    .blockMetatiles = gLevel3Stage5Room7BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_084BAEE0,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D230,
    .bg3Tiles = gUnk_0841AD98,
    .bg3Map = &gUnk_083C93D8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel3Stage5Room7Doors,
    .objects = gLevel3Stage5Room7Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 216,
    .entryY = 120,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[3][6][0], 0x0837D968, section .room_def_0837d968 */
extern u8 gLevel3Stage6Room0MetatileMap[];
extern u8 gLevel3Stage6Room0BlockLayer[];
extern u8 gLevel3Stage6Room0BlockMetatiles[];
extern u8 gLevel3Stage6Room0Objects[];
struct RoomDef gLevel3Stage6Room0 ROOM_DEF(0837d968) = {
    .filler00 = { 3, 6, 0, 0 },
    .bgm = 30,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel3Stage6Room0MetatileMap,
    .blockLayer = gLevel3Stage6Room0BlockLayer,
    .blockMetatiles = gLevel3Stage6Room0BlockMetatiles,
    .width = 16,
    .height = 144,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 12,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = gLevel3Stage6Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 2264,
    .unk54 = 2,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 5,
};

/* gRoomTable[4][0][0], 0x0837DE3C, section .room_def_0837de3c */
extern u8 gLevel4Stage0Room0MetatileMap[];
extern u8 gLevel4Stage0Room0BlockLayer[];
extern u8 gLevel4Stage0Room0BlockMetatiles[];
extern struct Door gLevel4Stage0Room0Doors[];
extern u8 gLevel4Stage0Room0Objects[];
struct RoomDef gLevel4Stage0Room0 ROOM_DEF(0837de3c) = {
    .filler00 = { 4, 0, 0, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage0Room0MetatileMap,
    .blockLayer = gLevel4Stage0Room0BlockLayer,
    .blockMetatiles = gLevel4Stage0Room0BlockMetatiles,
    .width = 80,
    .height = 14,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853C9C8,
    .bg3Tiles = gUnk_083E6548,
    .bg3Map = &gUnk_083B708C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 14,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage0Room0Doors,
    .objects = gLevel4Stage0Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 73,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][0][1], 0x0837E598, section .room_def_0837e598 */
extern u8 gLevel4Stage0Room1MetatileMap[];
extern u8 gLevel4Stage0Room1BlockLayer[];
extern u8 gLevel4Stage0Room1BlockMetatiles[];
extern struct Door gLevel4Stage0Room1Doors[];
extern u8 gLevel4Stage0Room1Objects[];
struct RoomDef gLevel4Stage0Room1 ROOM_DEF(0837e598) = {
    .filler00 = { 4, 0, 1, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage0Room1MetatileMap,
    .blockLayer = gLevel4Stage0Room1BlockLayer,
    .blockMetatiles = gLevel4Stage0Room1BlockMetatiles,
    .width = 16,
    .height = 82,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gLevel4Stage0Room1Bg3Palette,
    .bg3Tiles = gUnk_083EE8B4,
    .bg3Map = &gUnk_083B82B0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 15,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage0Room1Doors,
    .objects = gLevel4Stage0Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 56,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][0][2], 0x0837EB34, section .room_def_0837eb34 */
extern u8 gLevel4Stage0Room2MetatileMap[];
extern u8 gLevel4Stage0Room2BlockLayer[];
extern u8 gLevel4Stage0Room2BlockMetatiles[];
extern struct Door gLevel4Stage0Room2Doors[];
extern u8 gLevel4Stage0Room2Objects[];
struct RoomDef gLevel4Stage0Room2 ROOM_DEF(0837eb34) = {
    .filler00 = { 4, 0, 2, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage0Room2MetatileMap,
    .blockLayer = gLevel4Stage0Room2BlockLayer,
    .blockMetatiles = gLevel4Stage0Room2BlockMetatiles,
    .width = 81,
    .height = 14,
    .bg2Palette = gUnk_0853DAB4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0850AB70,
    .bg3Tiles = gUnk_0850B598,
    .bg3Map = &gUnk_0850AC74,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 10,
    .objectsSortedByY = 0,
    .bgAnimSet = 1,
    .unk42 = 0,
    .doors = gLevel4Stage0Room2Doors,
    .objects = gLevel4Stage0Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 24,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][0][3], 0x0837ECA8, section .room_def_0837eca8 */
extern u8 gLevel4Stage0Room3MetatileMap[];
extern u8 gLevel4Stage0Room3BlockLayer[];
extern u8 gLevel4Stage0Room3BlockMetatiles[];
extern struct Door gLevel4Stage0Room3Doors[];
extern u8 gLevel4Stage0Room3Objects[];
struct RoomDef gLevel4Stage0Room3 ROOM_DEF(0837eca8) = {
    .filler00 = { 4, 0, 3, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage0Room3MetatileMap,
    .blockLayer = gLevel4Stage0Room3BlockLayer,
    .blockMetatiles = gLevel4Stage0Room3BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 5,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage0Room3Doors,
    .objects = gLevel4Stage0Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 183,
    .entryY = 56,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][0][4], 0x0837EE60, section .room_def_0837ee60 */
extern u8 gLevel4Stage0Room4MetatileMap[];
extern u8 gLevel4Stage0Room4BlockLayer[];
extern u8 gLevel4Stage0Room4BlockMetatiles[];
extern struct Door gLevel4Stage0Room4Doors[];
extern u8 gLevel4Stage0Room4Objects[];
struct RoomDef gLevel4Stage0Room4 ROOM_DEF(0837ee60) = {
    .filler00 = { 4, 0, 4, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage0Room4MetatileMap,
    .blockLayer = gLevel4Stage0Room4BlockLayer,
    .blockMetatiles = gLevel4Stage0Room4BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 4,
    .unk42 = 0,
    .doors = gLevel4Stage0Room4Doors,
    .objects = gLevel4Stage0Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 42,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][1][0], 0x0837F028, section .room_def_0837f028 */
extern u8 gLevel4Stage1Room0MetatileMap[];
extern u8 gLevel4Stage1Room0BlockLayer[];
extern u8 gLevel4Stage1Room0BlockMetatiles[];
extern struct Door gLevel4Stage1Room0Doors[];
struct RoomDef gLevel4Stage1Room0 ROOM_DEF(0837f028) = {
    .filler00 = { 4, 1, 0, 0 },
    .bgm = 2,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage1Room0MetatileMap,
    .blockLayer = gLevel4Stage1Room0BlockLayer,
    .blockMetatiles = gLevel4Stage1Room0BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853DAB4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0850AB70,
    .bg3Tiles = gUnk_0850B598,
    .bg3Map = &gUnk_0850AC74,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage1Room0Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 16,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][1][1], 0x0837F73C, section .room_def_0837f73c */
extern u8 gLevel4Stage1Room1MetatileMap[];
extern u8 gLevel4Stage1Room1BlockLayer[];
extern u8 gLevel4Stage1Room1BlockMetatiles[];
extern struct Door gLevel4Stage1Room1Doors[];
extern u8 gLevel4Stage1Room1Objects[];
struct RoomDef gLevel4Stage1Room1 ROOM_DEF(0837f73c) = {
    .filler00 = { 4, 1, 1, 0 },
    .bgm = 2,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage1Room1MetatileMap,
    .blockLayer = gLevel4Stage1Room1BlockLayer,
    .blockMetatiles = gLevel4Stage1Room1BlockMetatiles,
    .width = 80,
    .height = 13,
    .bg2Palette = gUnk_0849A52C,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_085112BC,
    .bg3Tiles = gUnk_08511CDC,
    .bg3Map = &gUnk_085113C0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 4,
    .objectCount = 13,
    .objectsSortedByY = 0,
    .bgAnimSet = 2,
    .unk42 = 0,
    .doors = gLevel4Stage1Room1Doors,
    .objects = gLevel4Stage1Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 153,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][1][2], 0x0837FA44, section .room_def_0837fa44 */
extern u8 gLevel4Stage1Room2MetatileMap[];
extern u8 gLevel4Stage1Room2BlockLayer[];
extern u8 gLevel4Stage1Room2BlockMetatiles[];
extern struct Door gLevel4Stage1Room2Doors[];
extern u8 gLevel4Stage1Room2Objects[];
struct RoomDef gLevel4Stage1Room2 ROOM_DEF(0837fa44) = {
    .filler00 = { 4, 1, 2, 0 },
    .bgm = 2,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage1Room2MetatileMap,
    .blockLayer = gLevel4Stage1Room2BlockLayer,
    .blockMetatiles = gLevel4Stage1Room2BlockMetatiles,
    .width = 16,
    .height = 22,
    .bg2Palette = gUnk_084BAFA4,
    .bg2Tiles = gUnk_0847E4B4,
    .metatileTiles = gUnk_083B39C4,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_085112BC,
    .bg3Tiles = gUnk_08511CDC,
    .bg3Map = &gUnk_085113C0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 3,
    .objectCount = 7,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage1Room2Doors,
    .objects = gLevel4Stage1Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 200,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][1][3], 0x0837FDC8, section .room_def_0837fdc8 */
extern u8 gLevel4Stage1Room3MetatileMap[];
extern u8 gLevel4Stage1Room3BlockLayer[];
extern u8 gLevel4Stage1Room3BlockMetatiles[];
extern struct Door gLevel4Stage1Room3Doors[];
extern u8 gLevel4Stage1Room3Objects[];
struct RoomDef gLevel4Stage1Room3 ROOM_DEF(0837fdc8) = {
    .filler00 = { 4, 1, 3, 0 },
    .bgm = 2,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage1Room3MetatileMap,
    .blockLayer = gLevel4Stage1Room3BlockLayer,
    .blockMetatiles = gLevel4Stage1Room3BlockMetatiles,
    .width = 33,
    .height = 13,
    .bg2Palette = gUnk_084BAFA4,
    .bg2Tiles = gUnk_0847E4B4,
    .metatileTiles = gUnk_083B39C4,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_085112BC,
    .bg3Tiles = gUnk_08511CDC,
    .bg3Map = &gUnk_085113C0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 3,
    .objectCount = 5,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage1Room3Doors,
    .objects = gLevel4Stage1Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 488,
    .entryY = 168,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][1][4], 0x083800D0, section .room_def_083800d0 */
extern u8 gLevel4Stage1Room4MetatileMap[];
extern u8 gLevel4Stage1Room4BlockLayer[];
extern u8 gLevel4Stage1Room4BlockMetatiles[];
extern struct Door gLevel4Stage1Room4Doors[];
extern u8 gLevel4Stage1Room4Objects[];
struct RoomDef gLevel4Stage1Room4 ROOM_DEF(083800d0) = {
    .filler00 = { 4, 1, 4, 0 },
    .bgm = 2,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage1Room4MetatileMap,
    .blockLayer = gLevel4Stage1Room4BlockLayer,
    .blockMetatiles = gLevel4Stage1Room4BlockMetatiles,
    .width = 16,
    .height = 22,
    .bg2Palette = gUnk_084BAFA4,
    .bg2Tiles = gUnk_0847E4B4,
    .metatileTiles = gUnk_083B39C4,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_085112BC,
    .bg3Tiles = gUnk_08511CDC,
    .bg3Map = &gUnk_085113C0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 5,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage1Room4Doors,
    .objects = gLevel4Stage1Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 57,
    .entryY = 312,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][1][5], 0x083805BC, section .room_def_083805bc */
extern u8 gLevel4Stage1Room5MetatileMap[];
extern u8 gLevel4Stage1Room5BlockLayer[];
extern u8 gLevel4Stage1Room5BlockMetatiles[];
extern struct Door gLevel4Stage1Room5Doors[];
extern u8 gLevel4Stage1Room5Objects[];
struct RoomDef gLevel4Stage1Room5 ROOM_DEF(083805bc) = {
    .filler00 = { 4, 1, 5, 0 },
    .bgm = 2,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage1Room5MetatileMap,
    .blockLayer = gLevel4Stage1Room5BlockLayer,
    .blockMetatiles = gLevel4Stage1Room5BlockMetatiles,
    .width = 32,
    .height = 25,
    .bg2Palette = gUnk_0853DAB4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0850AB70,
    .bg3Tiles = gUnk_0850B598,
    .bg3Map = &gUnk_0850AC74,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 3,
    .objectCount = 9,
    .objectsSortedByY = 0,
    .bgAnimSet = 1,
    .unk42 = 0,
    .doors = gLevel4Stage1Room5Doors,
    .objects = gLevel4Stage1Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 326,
    .entryY = 344,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][1][6], 0x08380758, section .room_def_08380758 */
extern u8 gLevel4Stage1Room6MetatileMap[];
extern u8 gLevel4Stage1Room6BlockLayer[];
extern u8 gLevel4Stage1Room6BlockMetatiles[];
extern struct Door gLevel4Stage1Room6Doors[];
extern u8 gLevel4Stage1Room6Objects[];
struct RoomDef gLevel4Stage1Room6 ROOM_DEF(08380758) = {
    .filler00 = { 4, 1, 6, 0 },
    .bgm = 2,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage1Room6MetatileMap,
    .blockLayer = gLevel4Stage1Room6BlockLayer,
    .blockMetatiles = gLevel4Stage1Room6BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0850AB70,
    .bg3Tiles = gUnk_0850B598,
    .bg3Map = &gUnk_0850AC74,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage1Room6Doors,
    .objects = gLevel4Stage1Room6Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 137,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][2][0], 0x08380CFC, section .room_def_08380cfc */
extern u8 gLevel4Stage2Room0MetatileMap[];
extern u8 gLevel4Stage2Room0BlockLayer[];
extern u8 gLevel4Stage2Room0BlockMetatiles[];
extern struct Door gLevel4Stage2Room0Doors[];
extern u8 gLevel4Stage2Room0Objects[];
struct RoomDef gLevel4Stage2Room0 ROOM_DEF(08380cfc) = {
    .filler00 = { 4, 2, 0, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage2Room0MetatileMap,
    .blockLayer = gLevel4Stage2Room0BlockLayer,
    .blockMetatiles = gLevel4Stage2Room0BlockMetatiles,
    .width = 16,
    .height = 64,
    .bg2Palette = gUnk_084D50EC,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0852C0D0,
    .bg3Tiles = gUnk_0851EF74,
    .bg3Map = &gUnk_08517F00,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 23,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage2Room0Doors,
    .objects = gLevel4Stage2Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 984,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][2][1], 0x083811A8, section .room_def_083811a8 */
extern u8 gLevel4Stage2Room1MetatileMap[];
extern u8 gLevel4Stage2Room1BlockLayer[];
extern u8 gLevel4Stage2Room1BlockMetatiles[];
extern struct Door gLevel4Stage2Room1Doors[];
extern u8 gLevel4Stage2Room1Objects[];
struct RoomDef gLevel4Stage2Room1 ROOM_DEF(083811a8) = {
    .filler00 = { 4, 2, 1, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage2Room1MetatileMap,
    .blockLayer = gLevel4Stage2Room1BlockLayer,
    .blockMetatiles = gLevel4Stage2Room1BlockMetatiles,
    .width = 16,
    .height = 43,
    .bg2Palette = gUnk_08510DA4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0852C0D0,
    .bg3Tiles = gUnk_0851EF74,
    .bg3Map = &gUnk_08517F00,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 9,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage2Room1Doors,
    .objects = gLevel4Stage2Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 73,
    .entryY = 601,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][2][2], 0x08381CCC, section .room_def_08381ccc */
extern u8 gLevel4Stage2Room2MetatileMap[];
extern u8 gLevel4Stage2Room2BlockLayer[];
extern u8 gLevel4Stage2Room2BlockMetatiles[];
extern struct Door gLevel4Stage2Room2Doors[];
extern u8 gLevel4Stage2Room2Objects[];
struct RoomDef gLevel4Stage2Room2 ROOM_DEF(08381ccc) = {
    .filler00 = { 4, 2, 2, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage2Room2MetatileMap,
    .blockLayer = gLevel4Stage2Room2BlockLayer,
    .blockMetatiles = gLevel4Stage2Room2BlockMetatiles,
    .width = 162,
    .height = 13,
    .bg2Palette = gLevel4Stage2Room2Bg2Palette,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0852BFCC,
    .bg3Tiles = gUnk_08518880,
    .bg3Map = &gUnk_085175E4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 19,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage2Room2Doors,
    .objects = gLevel4Stage2Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 137,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][2][3], 0x083821BC, section .room_def_083821bc */
extern u8 gLevel4Stage2Room3MetatileMap[];
extern u8 gLevel4Stage2Room3BlockLayer[];
extern u8 gLevel4Stage2Room3BlockMetatiles[];
extern struct Door gLevel4Stage2Room3Doors[];
extern u8 gLevel4Stage2Room3Objects[];
struct RoomDef gLevel4Stage2Room3 ROOM_DEF(083821bc) = {
    .filler00 = { 4, 2, 3, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage2Room3MetatileMap,
    .blockLayer = gLevel4Stage2Room3BlockLayer,
    .blockMetatiles = gLevel4Stage2Room3BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gUnk_08510DA4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0852BFCC,
    .bg3Tiles = gUnk_08518880,
    .bg3Map = &gUnk_085175E4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 3,
    .objectCount = 7,
    .objectsSortedByY = 0,
    .bgAnimSet = 1,
    .unk42 = 0,
    .doors = gLevel4Stage2Room3Doors,
    .objects = gLevel4Stage2Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][2][4], 0x083825D8, section .room_def_083825d8 */
extern u8 gLevel4Stage2Room4MetatileMap[];
extern u8 gLevel4Stage2Room4BlockLayer[];
extern u8 gLevel4Stage2Room4BlockMetatiles[];
extern struct Door gLevel4Stage2Room4Doors[];
extern u8 gLevel4Stage2Room4Objects[];
struct RoomDef gLevel4Stage2Room4 ROOM_DEF(083825d8) = {
    .filler00 = { 4, 2, 4, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage2Room4MetatileMap,
    .blockLayer = gLevel4Stage2Room4BlockLayer,
    .blockMetatiles = gLevel4Stage2Room4BlockMetatiles,
    .width = 16,
    .height = 36,
    .bg2Palette = gUnk_084D50EC,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gLevel4Stage2Room4Bg3Palette,
    .bg3Tiles = gUnk_083D4E14,
    .bg3Map = &gUnk_083B5538,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 5,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage2Room4Doors,
    .objects = gLevel4Stage2Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 57,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][2][5], 0x083828C4, section .room_def_083828c4 */
extern u8 gLevel4Stage2Room5MetatileMap[];
extern u8 gLevel4Stage2Room5BlockLayer[];
extern u8 gLevel4Stage2Room5BlockMetatiles[];
extern struct Door gLevel4Stage2Room5Doors[];
extern u8 gLevel4Stage2Room5Objects[];
struct RoomDef gLevel4Stage2Room5 ROOM_DEF(083828c4) = {
    .filler00 = { 4, 2, 5, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage2Room5MetatileMap,
    .blockLayer = gLevel4Stage2Room5BlockLayer,
    .blockMetatiles = gLevel4Stage2Room5BlockMetatiles,
    .width = 16,
    .height = 13,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 5,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage2Room5Doors,
    .objects = gLevel4Stage2Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][3][0], 0x0838328C, section .room_def_0838328c */
extern u8 gLevel4Stage3Room0MetatileMap[];
extern u8 gLevel4Stage3Room0BlockLayer[];
extern u8 gLevel4Stage3Room0BlockMetatiles[];
extern struct Door gLevel4Stage3Room0Doors[];
extern u8 gLevel4Stage3Room0Objects[];
struct RoomDef gLevel4Stage3Room0 ROOM_DEF(0838328c) = {
    .filler00 = { 4, 3, 0, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage3Room0MetatileMap,
    .blockLayer = gLevel4Stage3Room0BlockLayer,
    .blockMetatiles = gLevel4Stage3Room0BlockMetatiles,
    .width = 131,
    .height = 11,
    .bg2Palette = gUnk_0849A6B4,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_085112BC,
    .bg3Tiles = gUnk_08511CDC,
    .bg3Map = &gUnk_085113C0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 7,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage3Room0Doors,
    .objects = gLevel4Stage3Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 88,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][3][1], 0x08383B28, section .room_def_08383b28 */
extern u8 gLevel4Stage3Room1MetatileMap[];
extern u8 gLevel4Stage3Room1BlockLayer[];
extern u8 gLevel4Stage3Room1BlockMetatiles[];
extern struct Door gLevel4Stage3Room1Doors[];
extern u8 gLevel4Stage3Room1Objects[];
struct RoomDef gLevel4Stage3Room1 ROOM_DEF(08383b28) = {
    .filler00 = { 4, 3, 1, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage3Room1MetatileMap,
    .blockLayer = gLevel4Stage3Room1BlockLayer,
    .blockMetatiles = gLevel4Stage3Room1BlockMetatiles,
    .width = 123,
    .height = 14,
    .bg2Palette = gUnk_084BB16C,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_084FEE04,
    .bg3Tiles = gUnk_084FF824,
    .bg3Map = &gUnk_084FEF08,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 17,
    .objectsSortedByY = 0,
    .bgAnimSet = 1,
    .unk42 = 0,
    .doors = gLevel4Stage3Room1Doors,
    .objects = gLevel4Stage3Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 104,
    .entryY = 168,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][3][2], 0x083840C0, section .room_def_083840c0 */
extern u8 gLevel4Stage3Room2MetatileMap[];
extern u8 gLevel4Stage3Room2BlockLayer[];
extern u8 gLevel4Stage3Room2BlockMetatiles[];
extern struct Door gLevel4Stage3Room2Doors[];
extern u8 gLevel4Stage3Room2Objects[];
struct RoomDef gLevel4Stage3Room2 ROOM_DEF(083840c0) = {
    .filler00 = { 4, 3, 2, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage3Room2MetatileMap,
    .blockLayer = gLevel4Stage3Room2BlockLayer,
    .blockMetatiles = gLevel4Stage3Room2BlockMetatiles,
    .width = 34,
    .height = 22,
    .bg2Palette = gUnk_0849A6B4,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_085112BC,
    .bg3Tiles = gUnk_08511CDC,
    .bg3Map = &gUnk_085113C0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 5,
    .doorCount = 1,
    .objectCount = 5,
    .objectsSortedByY = 0,
    .bgAnimSet = 2,
    .unk42 = 0,
    .doors = gLevel4Stage3Room2Doors,
    .objects = gLevel4Stage3Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 472,
    .entryY = 24,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][3][3], 0x083847B4, section .room_def_083847b4 */
extern u8 gLevel4Stage3Room3MetatileMap[];
extern u8 gLevel4Stage3Room3BlockLayer[];
extern u8 gLevel4Stage3Room3BlockMetatiles[];
extern struct Door gLevel4Stage3Room3Doors[];
extern u8 gLevel4Stage3Room3Objects[];
struct RoomDef gLevel4Stage3Room3 ROOM_DEF(083847b4) = {
    .filler00 = { 4, 3, 3, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage3Room3MetatileMap,
    .blockLayer = gLevel4Stage3Room3BlockLayer,
    .blockMetatiles = gLevel4Stage3Room3BlockMetatiles,
    .width = 16,
    .height = 81,
    .bg2Palette = gUnk_084BB16C,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084FEE04,
    .bg3Tiles = gUnk_084FF824,
    .bg3Map = &gUnk_084FEF08,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 21,
    .objectsSortedByY = 1,
    .bgAnimSet = 1,
    .unk42 = 0,
    .doors = gLevel4Stage3Room3Doors,
    .objects = gLevel4Stage3Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 216,
    .entryY = 24,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][3][4], 0x08384ADC, section .room_def_08384adc */
extern u8 gLevel4Stage3Room4MetatileMap[];
extern u8 gLevel4Stage3Room4BlockLayer[];
extern u8 gLevel4Stage3Room4BlockMetatiles[];
extern struct Door gLevel4Stage3Room4Doors[];
extern u8 gLevel4Stage3Room4Objects[];
struct RoomDef gLevel4Stage3Room4 ROOM_DEF(08384adc) = {
    .filler00 = { 4, 3, 4, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage3Room4MetatileMap,
    .blockLayer = gLevel4Stage3Room4BlockLayer,
    .blockMetatiles = gLevel4Stage3Room4BlockMetatiles,
    .width = 16,
    .height = 24,
    .bg2Palette = gUnk_0849A6B4,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_085112BC,
    .bg3Tiles = gUnk_08511CDC,
    .bg3Map = &gUnk_085113C0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 5,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage3Room4Doors,
    .objects = gLevel4Stage3Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 41,
    .entryY = 328,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][3][5], 0x08384CB0, section .room_def_08384cb0 */
extern u8 gLevel4Stage3Room5MetatileMap[];
extern u8 gLevel4Stage3Room5BlockLayer[];
extern u8 gLevel4Stage3Room5BlockMetatiles[];
extern struct Door gLevel4Stage3Room5Doors[];
extern u8 gLevel4Stage3Room5Objects[];
struct RoomDef gLevel4Stage3Room5 ROOM_DEF(08384cb0) = {
    .filler00 = { 4, 3, 5, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage3Room5MetatileMap,
    .blockLayer = gLevel4Stage3Room5BlockLayer,
    .blockMetatiles = gLevel4Stage3Room5BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_084BB16C,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084FEE04,
    .bg3Tiles = gUnk_084FF824,
    .bg3Map = &gUnk_084FEF08,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 1,
    .unk42 = 0,
    .doors = gLevel4Stage3Room5Doors,
    .objects = gLevel4Stage3Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 73,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][3][6], 0x08384E3C, section .room_def_08384e3c */
extern u8 gLevel4Stage3Room6MetatileMap[];
extern u8 gLevel4Stage3Room6BlockLayer[];
extern u8 gLevel4Stage3Room6BlockMetatiles[];
extern struct Door gLevel4Stage3Room6Doors[];
extern u8 gLevel4Stage3Room6Objects[];
struct RoomDef gLevel4Stage3Room6 ROOM_DEF(08384e3c) = {
    .filler00 = { 4, 3, 6, 0 },
    .bgm = 37,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage3Room6MetatileMap,
    .blockLayer = gLevel4Stage3Room6BlockLayer,
    .blockMetatiles = gLevel4Stage3Room6BlockMetatiles,
    .width = 18,
    .height = 11,
    .bg2Palette = gUnk_0849A6B4,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 24,
    .borderY = 8,
    .bg3Palette = gUnk_085112BC,
    .bg3Tiles = gUnk_08511CDC,
    .bg3Map = &gUnk_085113C0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage3Room6Doors,
    .objects = gLevel4Stage3Room6Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][4][0], 0x08385964, section .room_def_08385964 */
extern u8 gLevel4Stage4Room0MetatileMap[];
extern u8 gLevel4Stage4Room0BlockLayer[];
extern u8 gLevel4Stage4Room0BlockMetatiles[];
extern struct Door gLevel4Stage4Room0Doors[];
extern u8 gLevel4Stage4Room0Objects[];
struct RoomDef gLevel4Stage4Room0 ROOM_DEF(08385964) = {
    .filler00 = { 4, 4, 0, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage4Room0MetatileMap,
    .blockLayer = gLevel4Stage4Room0BlockLayer,
    .blockMetatiles = gLevel4Stage4Room0BlockMetatiles,
    .width = 80,
    .height = 25,
    .bg2Palette = gUnk_0852D088,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0849096C,
    .bg3Tiles = gUnk_0849138C,
    .bg3Map = &gUnk_08490A70,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 18,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage4Room0Doors,
    .objects = gLevel4Stage4Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 248,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][4][1], 0x08385FCC, section .room_def_08385fcc */
extern u8 gLevel4Stage4Room1MetatileMap[];
extern u8 gLevel4Stage4Room1BlockLayer[];
extern u8 gLevel4Stage4Room1BlockMetatiles[];
extern struct Door gLevel4Stage4Room1Doors[];
extern u8 gLevel4Stage4Room1Objects[];
struct RoomDef gLevel4Stage4Room1 ROOM_DEF(08385fcc) = {
    .filler00 = { 4, 4, 1, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage4Room1MetatileMap,
    .blockLayer = gLevel4Stage4Room1BlockLayer,
    .blockMetatiles = gLevel4Stage4Room1BlockMetatiles,
    .width = 80,
    .height = 11,
    .bg2Palette = gUnk_0852D088,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0849096C,
    .bg3Tiles = gUnk_0849138C,
    .bg3Map = &gUnk_08490A70,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage4Room1Doors,
    .objects = gLevel4Stage4Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 105,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][4][2], 0x08386348, section .room_def_08386348 */
extern u8 gLevel4Stage4Room2MetatileMap[];
extern u8 gLevel4Stage4Room2BlockLayer[];
extern u8 gLevel4Stage4Room2BlockMetatiles[];
extern struct Door gLevel4Stage4Room2Doors[];
struct RoomDef gLevel4Stage4Room2 ROOM_DEF(08386348) = {
    .filler00 = { 4, 4, 2, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage4Room2MetatileMap,
    .blockLayer = gLevel4Stage4Room2BlockLayer,
    .blockMetatiles = gLevel4Stage4Room2BlockMetatiles,
    .width = 48,
    .height = 11,
    .bg2Palette = gUnk_084B57E8,
    .bg2Tiles = gUnk_084B5F90,
    .metatileTiles = gUnk_084B58AC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849096C,
    .bg3Tiles = gUnk_0849138C,
    .bg3Map = &gUnk_08490A70,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 4,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage4Room2Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 8,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][4][3], 0x08386700, section .room_def_08386700 */
extern u8 gLevel4Stage4Room3MetatileMap[];
extern u8 gLevel4Stage4Room3BlockLayer[];
extern u8 gLevel4Stage4Room3BlockMetatiles[];
extern struct Door gLevel4Stage4Room3Doors[];
extern u8 gLevel4Stage4Room3Objects[];
struct RoomDef gLevel4Stage4Room3 ROOM_DEF(08386700) = {
    .filler00 = { 4, 4, 3, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage4Room3MetatileMap,
    .blockLayer = gLevel4Stage4Room3BlockLayer,
    .blockMetatiles = gLevel4Stage4Room3BlockMetatiles,
    .width = 48,
    .height = 12,
    .bg2Palette = gUnk_0852D250,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849096C,
    .bg3Tiles = gUnk_0849138C,
    .bg3Map = &gUnk_08490A70,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 5,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage4Room3Doors,
    .objects = gLevel4Stage4Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 73,
    .entryY = 42,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][4][4], 0x08386BC0, section .room_def_08386bc0 */
extern u8 gLevel4Stage4Room4MetatileMap[];
extern u8 gLevel4Stage4Room4BlockLayer[];
extern u8 gLevel4Stage4Room4BlockMetatiles[];
extern struct Door gLevel4Stage4Room4Doors[];
extern u8 gLevel4Stage4Room4Objects[];
struct RoomDef gLevel4Stage4Room4 ROOM_DEF(08386bc0) = {
    .filler00 = { 4, 4, 4, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage4Room4MetatileMap,
    .blockLayer = gLevel4Stage4Room4BlockLayer,
    .blockMetatiles = gLevel4Stage4Room4BlockMetatiles,
    .width = 64,
    .height = 11,
    .bg2Palette = gUnk_0852D088,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849096C,
    .bg3Tiles = gUnk_0849138C,
    .bg3Map = &gUnk_08490A70,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 13,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage4Room4Doors,
    .objects = gLevel4Stage4Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 39,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][4][5], 0x08386F80, section .room_def_08386f80 */
extern u8 gLevel4Stage4Room5MetatileMap[];
extern u8 gLevel4Stage4Room5BlockLayer[];
extern u8 gLevel4Stage4Room5BlockMetatiles[];
extern struct Door gLevel4Stage4Room5Doors[];
extern u8 gLevel4Stage4Room5Objects[];
struct RoomDef gLevel4Stage4Room5 ROOM_DEF(08386f80) = {
    .filler00 = { 4, 4, 5, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage4Room5MetatileMap,
    .blockLayer = gLevel4Stage4Room5BlockLayer,
    .blockMetatiles = gLevel4Stage4Room5BlockMetatiles,
    .width = 48,
    .height = 12,
    .bg2Palette = gUnk_0852D250,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849096C,
    .bg3Tiles = gUnk_0849138C,
    .bg3Map = &gUnk_08490A70,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage4Room5Doors,
    .objects = gLevel4Stage4Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][4][6], 0x08387308, section .room_def_08387308 */
extern u8 gLevel4Stage4Room6MetatileMap[];
extern u8 gLevel4Stage4Room6BlockLayer[];
extern u8 gLevel4Stage4Room6BlockMetatiles[];
extern struct Door gLevel4Stage4Room6Doors[];
extern u8 gLevel4Stage4Room6Objects[];
struct RoomDef gLevel4Stage4Room6 ROOM_DEF(08387308) = {
    .filler00 = { 4, 4, 6, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage4Room6MetatileMap,
    .blockLayer = gLevel4Stage4Room6BlockLayer,
    .blockMetatiles = gLevel4Stage4Room6BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853E728,
    .bg2Tiles = gUnk_0847E4B4,
    .metatileTiles = gUnk_083B39C4,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849096C,
    .bg3Tiles = gUnk_0849138C,
    .bg3Map = &gUnk_08490A70,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 4,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage4Room6Doors,
    .objects = gLevel4Stage4Room6Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 121,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][4][7], 0x08387498, section .room_def_08387498 */
extern u8 gLevel4Stage4Room7MetatileMap[];
extern u8 gLevel4Stage4Room7BlockLayer[];
extern u8 gLevel4Stage4Room7BlockMetatiles[];
extern struct Door gLevel4Stage4Room7Doors[];
extern u8 gLevel4Stage4Room7Objects[];
struct RoomDef gLevel4Stage4Room7 ROOM_DEF(08387498) = {
    .filler00 = { 4, 4, 7, 0 },
    .bgm = 15,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage4Room7MetatileMap,
    .blockLayer = gLevel4Stage4Room7BlockLayer,
    .blockMetatiles = gLevel4Stage4Room7BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853D12C,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0849096C,
    .bg3Tiles = gUnk_0849138C,
    .bg3Map = &gUnk_08490A70,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage4Room7Doors,
    .objects = gLevel4Stage4Room7Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][5][0], 0x083879E8, section .room_def_083879e8 */
extern u8 gLevel4Stage5Room0MetatileMap[];
extern u8 gLevel4Stage5Room0BlockLayer[];
extern u8 gLevel4Stage5Room0BlockMetatiles[];
extern struct Door gLevel4Stage5Room0Doors[];
extern u8 gLevel4Stage5Room0Objects[];
struct RoomDef gLevel4Stage5Room0 ROOM_DEF(083879e8) = {
    .filler00 = { 4, 5, 0, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage5Room0MetatileMap,
    .blockLayer = gLevel4Stage5Room0BlockLayer,
    .blockMetatiles = gLevel4Stage5Room0BlockMetatiles,
    .width = 81,
    .height = 13,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E00C,
    .bg3Tiles = gUnk_08454A70,
    .bg3Map = &gUnk_083CCA9C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 16,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage5Room0Doors,
    .objects = gLevel4Stage5Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 153,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][5][1], 0x083881D4, section .room_def_083881d4 */
extern u8 gLevel4Stage5Room1MetatileMap[];
extern u8 gLevel4Stage5Room1BlockLayer[];
extern u8 gLevel4Stage5Room1BlockMetatiles[];
extern struct Door gLevel4Stage5Room1Doors[];
extern u8 gLevel4Stage5Room1Objects[];
struct RoomDef gLevel4Stage5Room1 ROOM_DEF(083881d4) = {
    .filler00 = { 4, 5, 1, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage5Room1MetatileMap,
    .blockLayer = gLevel4Stage5Room1BlockLayer,
    .blockMetatiles = gLevel4Stage5Room1BlockMetatiles,
    .width = 81,
    .height = 14,
    .bg2Palette = gUnk_0852D18C,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E00C,
    .bg3Tiles = gUnk_08454A70,
    .bg3Map = &gUnk_083CCA9C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage5Room1Doors,
    .objects = gLevel4Stage5Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][5][2], 0x0838875C, section .room_def_0838875c */
extern u8 gLevel4Stage5Room2MetatileMap[];
extern u8 gLevel4Stage5Room2BlockLayer[];
extern u8 gLevel4Stage5Room2BlockMetatiles[];
extern struct Door gLevel4Stage5Room2Doors[];
extern u8 gLevel4Stage5Room2Objects[];
struct RoomDef gLevel4Stage5Room2 ROOM_DEF(0838875c) = {
    .filler00 = { 4, 5, 2, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage5Room2MetatileMap,
    .blockLayer = gLevel4Stage5Room2BlockLayer,
    .blockMetatiles = gLevel4Stage5Room2BlockMetatiles,
    .width = 79,
    .height = 13,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E00C,
    .bg3Tiles = gUnk_08454A70,
    .bg3Map = &gUnk_083CCA9C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 3,
    .objectCount = 14,
    .objectsSortedByY = 0,
    .bgAnimSet = 3,
    .unk42 = 0,
    .doors = gLevel4Stage5Room2Doors,
    .objects = gLevel4Stage5Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 42,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][5][3], 0x08388970, section .room_def_08388970 */
extern u8 gLevel4Stage5Room3MetatileMap[];
extern u8 gLevel4Stage5Room3BlockLayer[];
extern u8 gLevel4Stage5Room3BlockMetatiles[];
extern struct Door gLevel4Stage5Room3Doors[];
struct RoomDef gLevel4Stage5Room3 ROOM_DEF(08388970) = {
    .filler00 = { 4, 5, 3, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage5Room3MetatileMap,
    .blockLayer = gLevel4Stage5Room3BlockLayer,
    .blockMetatiles = gLevel4Stage5Room3BlockMetatiles,
    .width = 16,
    .height = 13,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_085112BC,
    .bg3Tiles = gUnk_08511CDC,
    .bg3Map = &gUnk_085113C0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 3,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel4Stage5Room3Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][5][4], 0x08388BB8, section .room_def_08388bb8 */
extern u8 gLevel4Stage5Room4MetatileMap[];
extern u8 gLevel4Stage5Room4BlockLayer[];
extern u8 gLevel4Stage5Room4BlockMetatiles[];
extern struct Door gLevel4Stage5Room4Doors[];
extern u8 gLevel4Stage5Room4Objects[];
struct RoomDef gLevel4Stage5Room4 ROOM_DEF(08388bb8) = {
    .filler00 = { 4, 5, 4, 0 },
    .bgm = 22,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage5Room4MetatileMap,
    .blockLayer = gLevel4Stage5Room4BlockLayer,
    .blockMetatiles = gLevel4Stage5Room4BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0852D18C,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_085112BC,
    .bg3Tiles = gUnk_08511CDC,
    .bg3Map = &gUnk_085113C0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 3,
    .objectsSortedByY = 0,
    .bgAnimSet = 2,
    .unk42 = 0,
    .doors = gLevel4Stage5Room4Doors,
    .objects = gLevel4Stage5Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 57,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[4][6][0], 0x0838928C, section .room_def_0838928c */
extern u8 gLevel4Stage6Room0MetatileMap[];
extern u8 gLevel4Stage6Room0BlockLayer[];
extern u8 gLevel4Stage6Room0BlockMetatiles[];
extern u8 gLevel4Stage6Room0Objects[];
struct RoomDef gLevel4Stage6Room0 ROOM_DEF(0838928c) = {
    .filler00 = { 4, 6, 0, 0 },
    .bgm = 30,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage6Room0MetatileMap,
    .blockLayer = gLevel4Stage6Room0BlockLayer,
    .blockMetatiles = gLevel4Stage6Room0BlockMetatiles,
    .width = 64,
    .height = 32,
    .bg2Palette = gLevel4Stage6Room0Bg2Palette,
    .bg2Tiles = gLevel4Stage6Room0Bg2Tiles,
    .metatileTiles = gLevel4Stage6Room0MetatileTiles,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08524FEC,
    .bg3Tiles = gUnk_08525A0C,
    .bg3Map = &gUnk_085250F0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = gLevel4Stage6Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 264,
    .unk54 = 1,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 5,
};

/* gRoomTable[4][6][1], 0x083893DC, section .room_def_083893dc */
extern u8 gLevel4Stage6Room1MetatileMap[];
extern u8 gLevel4Stage6Room1BlockLayer[];
extern u8 gLevel4Stage6Room1BlockMetatiles[];
extern u8 gLevel4Stage6Room1Objects[];
struct RoomDef gLevel4Stage6Room1 ROOM_DEF(083893dc) = {
    .filler00 = { 4, 6, 1, 0 },
    .bgm = 1,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel4Stage6Room1MetatileMap,
    .blockLayer = gLevel4Stage6Room1BlockLayer,
    .blockMetatiles = gLevel4Stage6Room1BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0849A52C,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08524FEC,
    .bg3Tiles = gUnk_08525A0C,
    .bg3Map = &gUnk_085250F0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = gLevel4Stage6Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 120,
    .entryY = 120,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 5,
};

/* gRoomTable[5][0][0], 0x08389A60, section .room_def_08389a60 */
extern u8 gLevel5Stage0Room0MetatileMap[];
extern u8 gLevel5Stage0Room0BlockLayer[];
extern u8 gLevel5Stage0Room0BlockMetatiles[];
extern struct Door gLevel5Stage0Room0Doors[];
extern u8 gLevel5Stage0Room0Objects[];
struct RoomDef gLevel5Stage0Room0 ROOM_DEF(08389a60) = {
    .filler00 = { 5, 0, 0, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage0Room0MetatileMap,
    .blockLayer = gLevel5Stage0Room0BlockLayer,
    .blockMetatiles = gLevel5Stage0Room0BlockMetatiles,
    .width = 48,
    .height = 23,
    .bg2Palette = gUnk_08505A1C,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08510B9C,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 15,
    .objectsSortedByY = 0,
    .bgAnimSet = 3,
    .unk42 = 0,
    .doors = gLevel5Stage0Room0Doors,
    .objects = gLevel5Stage0Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 88,
    .entryY = 280,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][0][1], 0x08389F2C, section .room_def_08389f2c */
extern u8 gLevel5Stage0Room1MetatileMap[];
extern u8 gLevel5Stage0Room1BlockLayer[];
extern u8 gLevel5Stage0Room1BlockMetatiles[];
extern struct Door gLevel5Stage0Room1Doors[];
extern u8 gLevel5Stage0Room1Objects[];
struct RoomDef gLevel5Stage0Room1 ROOM_DEF(08389f2c) = {
    .filler00 = { 5, 0, 1, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage0Room1MetatileMap,
    .blockLayer = gLevel5Stage0Room1BlockLayer,
    .blockMetatiles = gLevel5Stage0Room1BlockMetatiles,
    .width = 71,
    .height = 14,
    .bg2Palette = gUnk_0849AA04,
    .bg2Tiles = gUnk_08442F18,
    .metatileTiles = gUnk_083B2254,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_08510B9C,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 15,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage0Room1Doors,
    .objects = gLevel5Stage0Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 72,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][0][2], 0x0838A420, section .room_def_0838a420 */
extern u8 gLevel5Stage0Room2MetatileMap[];
extern u8 gLevel5Stage0Room2BlockLayer[];
extern u8 gLevel5Stage0Room2BlockMetatiles[];
extern struct Door gLevel5Stage0Room2Doors[];
extern u8 gLevel5Stage0Room2Objects[];
struct RoomDef gLevel5Stage0Room2 ROOM_DEF(0838a420) = {
    .filler00 = { 5, 0, 2, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage0Room2MetatileMap,
    .blockLayer = gLevel5Stage0Room2BlockLayer,
    .blockMetatiles = gLevel5Stage0Room2BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gUnk_08505A1C,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_08510B9C,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 17,
    .objectsSortedByY = 0,
    .bgAnimSet = 3,
    .unk42 = 0,
    .doors = gLevel5Stage0Room2Doors,
    .objects = gLevel5Stage0Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 73,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][0][3], 0x0838A668, section .room_def_0838a668 */
extern u8 gLevel5Stage0Room3MetatileMap[];
extern u8 gLevel5Stage0Room3BlockLayer[];
extern u8 gLevel5Stage0Room3BlockMetatiles[];
extern struct Door gLevel5Stage0Room3Doors[];
extern u8 gLevel5Stage0Room3Objects[];
struct RoomDef gLevel5Stage0Room3 ROOM_DEF(0838a668) = {
    .filler00 = { 5, 0, 3, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage0Room3MetatileMap,
    .blockLayer = gLevel5Stage0Room3BlockLayer,
    .blockMetatiles = gLevel5Stage0Room3BlockMetatiles,
    .width = 17,
    .height = 11,
    .bg2Palette = gUnk_0849A5F0,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0853C7C0,
    .bg3Tiles = gUnk_083DB044,
    .bg3Map = &gUnk_083B5E54,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage0Room3Doors,
    .objects = gLevel5Stage0Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 137,
    .entryY = 41,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][0][4], 0x0838A85C, section .room_def_0838a85c */
extern u8 gLevel5Stage0Room4MetatileMap[];
extern u8 gLevel5Stage0Room4BlockLayer[];
extern u8 gLevel5Stage0Room4BlockMetatiles[];
extern struct Door gLevel5Stage0Room4Doors[];
extern u8 gLevel5Stage0Room4Objects[];
struct RoomDef gLevel5Stage0Room4 ROOM_DEF(0838a85c) = {
    .filler00 = { 5, 0, 4, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage0Room4MetatileMap,
    .blockLayer = gLevel5Stage0Room4BlockLayer,
    .blockMetatiles = gLevel5Stage0Room4BlockMetatiles,
    .width = 17,
    .height = 11,
    .bg2Palette = gUnk_084BAFA4,
    .bg2Tiles = gUnk_0847E4B4,
    .metatileTiles = gUnk_083B39C4,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0853C7C0,
    .bg3Tiles = gUnk_083DB044,
    .bg3Map = &gUnk_083B5E54,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage0Room4Doors,
    .objects = gLevel5Stage0Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 136,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][0][5], 0x0838A9D0, section .room_def_0838a9d0 */
extern u8 gLevel5Stage0Room5MetatileMap[];
extern u8 gLevel5Stage0Room5BlockLayer[];
extern u8 gLevel5Stage0Room5BlockMetatiles[];
extern struct Door gLevel5Stage0Room5Doors[];
extern u8 gLevel5Stage0Room5Objects[];
struct RoomDef gLevel5Stage0Room5 ROOM_DEF(0838a9d0) = {
    .filler00 = { 5, 0, 5, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage0Room5MetatileMap,
    .blockLayer = gLevel5Stage0Room5BlockLayer,
    .blockMetatiles = gLevel5Stage0Room5BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08505A1C,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08510B9C,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 3,
    .unk42 = 0,
    .doors = gLevel5Stage0Room5Doors,
    .objects = gLevel5Stage0Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 24,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][1][0], 0x0838AF20, section .room_def_0838af20 */
extern u8 gLevel5Stage1Room0MetatileMap[];
extern u8 gLevel5Stage1Room0BlockLayer[];
extern u8 gLevel5Stage1Room0BlockMetatiles[];
extern struct Door gLevel5Stage1Room0Doors[];
extern u8 gLevel5Stage1Room0Objects[];
struct RoomDef gLevel5Stage1Room0 ROOM_DEF(0838af20) = {
    .filler00 = { 5, 1, 0, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage1Room0MetatileMap,
    .blockLayer = gLevel5Stage1Room0BlockLayer,
    .blockMetatiles = gLevel5Stage1Room0BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gLevel5Stage1Room0Bg2Palette,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0852BEC8,
    .bg3Tiles = gUnk_0846697C,
    .bg3Map = &gUnk_083CE5F4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 12,
    .objectsSortedByY = 0,
    .bgAnimSet = 1,
    .unk42 = 0,
    .doors = gLevel5Stage1Room0Doors,
    .objects = gLevel5Stage1Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 80,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][1][1], 0x0838B394, section .room_def_0838b394 */
extern u8 gLevel5Stage1Room1MetatileMap[];
extern u8 gLevel5Stage1Room1BlockLayer[];
extern u8 gLevel5Stage1Room1BlockMetatiles[];
extern struct Door gLevel5Stage1Room1Doors[];
extern u8 gLevel5Stage1Room1Objects[];
struct RoomDef gLevel5Stage1Room1 ROOM_DEF(0838b394) = {
    .filler00 = { 5, 1, 1, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage1Room1MetatileMap,
    .blockLayer = gLevel5Stage1Room1BlockLayer,
    .blockMetatiles = gLevel5Stage1Room1BlockMetatiles,
    .width = 32,
    .height = 23,
    .bg2Palette = gUnk_0852D3D8,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084D54BC,
    .bg3Tiles = gUnk_083E6548,
    .bg3Map = &gUnk_083B708C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 12,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage1Room1Doors,
    .objects = gLevel5Stage1Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 297,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][1][2], 0x0838B744, section .room_def_0838b744 */
extern u8 gLevel5Stage1Room2MetatileMap[];
extern u8 gLevel5Stage1Room2BlockLayer[];
extern u8 gLevel5Stage1Room2BlockMetatiles[];
extern struct Door gLevel5Stage1Room2Doors[];
extern u8 gLevel5Stage1Room2Objects[];
struct RoomDef gLevel5Stage1Room2 ROOM_DEF(0838b744) = {
    .filler00 = { 5, 1, 2, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage1Room2MetatileMap,
    .blockLayer = gLevel5Stage1Room2BlockLayer,
    .blockMetatiles = gLevel5Stage1Room2BlockMetatiles,
    .width = 16,
    .height = 36,
    .bg2Palette = gUnk_0849AA04,
    .bg2Tiles = gUnk_08442F18,
    .metatileTiles = gUnk_083B2254,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0852BEC8,
    .bg3Tiles = gUnk_0846697C,
    .bg3Map = &gUnk_083CE5F4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 12,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage1Room2Doors,
    .objects = gLevel5Stage1Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 57,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][1][3], 0x0838BB74, section .room_def_0838bb74 */
extern u8 gLevel5Stage1Room3MetatileMap[];
extern u8 gLevel5Stage1Room3BlockLayer[];
extern u8 gLevel5Stage1Room3BlockMetatiles[];
extern struct Door gLevel5Stage1Room3Doors[];
extern u8 gLevel5Stage1Room3Objects[];
struct RoomDef gLevel5Stage1Room3 ROOM_DEF(0838bb74) = {
    .filler00 = { 5, 1, 3, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage1Room3MetatileMap,
    .blockLayer = gLevel5Stage1Room3BlockLayer,
    .blockMetatiles = gLevel5Stage1Room3BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gUnk_08505A1C,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0852BEC8,
    .bg3Tiles = gUnk_0846697C,
    .bg3Map = &gUnk_083CE5F4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 14,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage1Room3Doors,
    .objects = gLevel5Stage1Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 73,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][1][4], 0x0838BD4C, section .room_def_0838bd4c */
extern u8 gLevel5Stage1Room4MetatileMap[];
extern u8 gLevel5Stage1Room4BlockLayer[];
extern u8 gLevel5Stage1Room4BlockMetatiles[];
extern struct Door gLevel5Stage1Room4Doors[];
extern u8 gLevel5Stage1Room4Objects[];
struct RoomDef gLevel5Stage1Room4 ROOM_DEF(0838bd4c) = {
    .filler00 = { 5, 1, 4, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage1Room4MetatileMap,
    .blockLayer = gLevel5Stage1Room4BlockLayer,
    .blockMetatiles = gLevel5Stage1Room4BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 3,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage1Room4Doors,
    .objects = gLevel5Stage1Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 137,
    .entryY = 25,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][1][5], 0x0838BF0C, section .room_def_0838bf0c */
extern u8 gLevel5Stage1Room5MetatileMap[];
extern u8 gLevel5Stage1Room5BlockLayer[];
extern u8 gLevel5Stage1Room5BlockMetatiles[];
extern struct Door gLevel5Stage1Room5Doors[];
extern u8 gLevel5Stage1Room5Objects[];
struct RoomDef gLevel5Stage1Room5 ROOM_DEF(0838bf0c) = {
    .filler00 = { 5, 1, 5, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage1Room5MetatileMap,
    .blockLayer = gLevel5Stage1Room5BlockLayer,
    .blockMetatiles = gLevel5Stage1Room5BlockMetatiles,
    .width = 17,
    .height = 11,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage1Room5Doors,
    .objects = gLevel5Stage1Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 136,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][1][6], 0x0838C068, section .room_def_0838c068 */
extern u8 gLevel5Stage1Room6MetatileMap[];
extern u8 gLevel5Stage1Room6BlockLayer[];
extern u8 gLevel5Stage1Room6BlockMetatiles[];
extern struct Door gLevel5Stage1Room6Doors[];
extern u8 gLevel5Stage1Room6Objects[];
struct RoomDef gLevel5Stage1Room6 ROOM_DEF(0838c068) = {
    .filler00 = { 5, 1, 6, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage1Room6MetatileMap,
    .blockLayer = gLevel5Stage1Room6BlockLayer,
    .blockMetatiles = gLevel5Stage1Room6BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0852D3D8,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084D54BC,
    .bg3Tiles = gUnk_083E6548,
    .bg3Map = &gUnk_083B708C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage1Room6Doors,
    .objects = gLevel5Stage1Room6Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 121,
    .entryY = 105,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][2][0], 0x0838C574, section .room_def_0838c574 */
extern u8 gLevel5Stage2Room0MetatileMap[];
extern u8 gLevel5Stage2Room0BlockLayer[];
extern u8 gLevel5Stage2Room0BlockMetatiles[];
extern struct Door gLevel5Stage2Room0Doors[];
extern u8 gLevel5Stage2Room0Objects[];
struct RoomDef gLevel5Stage2Room0 ROOM_DEF(0838c574) = {
    .filler00 = { 5, 2, 0, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage2Room0MetatileMap,
    .blockLayer = gLevel5Stage2Room0BlockLayer,
    .blockMetatiles = gLevel5Stage2Room0BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gUnk_0852D6E8,
    .bg2Tiles = gUnk_08534160,
    .metatileTiles = gUnk_0852E7CC,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_08510CA0,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 9,
    .objectsSortedByY = 0,
    .bgAnimSet = 11,
    .unk42 = 0,
    .doors = gLevel5Stage2Room0Doors,
    .objects = gLevel5Stage2Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 72,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][2][1], 0x0838CB58, section .room_def_0838cb58 */
extern u8 gLevel5Stage2Room1MetatileMap[];
extern u8 gLevel5Stage2Room1BlockLayer[];
extern u8 gLevel5Stage2Room1BlockMetatiles[];
extern struct Door gLevel5Stage2Room1Doors[];
extern u8 gLevel5Stage2Room1Objects[];
struct RoomDef gLevel5Stage2Room1 ROOM_DEF(0838cb58) = {
    .filler00 = { 5, 2, 1, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage2Room1MetatileMap,
    .blockLayer = gLevel5Stage2Room1BlockLayer,
    .blockMetatiles = gLevel5Stage2Room1BlockMetatiles,
    .width = 31,
    .height = 23,
    .bg2Palette = gUnk_0852D560,
    .bg2Tiles = gUnk_0852F948,
    .metatileTiles = gUnk_0852D870,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08510CA0,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 3,
    .objectCount = 5,
    .objectsSortedByY = 0,
    .bgAnimSet = 12,
    .unk42 = 0,
    .doors = gLevel5Stage2Room1Doors,
    .objects = gLevel5Stage2Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 120,
    .entryY = 185,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][2][2], 0x0838D190, section .room_def_0838d190 */
extern u8 gLevel5Stage2Room2MetatileMap[];
extern u8 gLevel5Stage2Room2BlockLayer[];
extern u8 gLevel5Stage2Room2BlockMetatiles[];
extern struct Door gLevel5Stage2Room2Doors[];
extern u8 gLevel5Stage2Room2Objects[];
struct RoomDef gLevel5Stage2Room2 ROOM_DEF(0838d190) = {
    .filler00 = { 5, 2, 2, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage2Room2MetatileMap,
    .blockLayer = gLevel5Stage2Room2BlockLayer,
    .blockMetatiles = gLevel5Stage2Room2BlockMetatiles,
    .width = 37,
    .height = 26,
    .bg2Palette = gUnk_0852D6E8,
    .bg2Tiles = gUnk_08534160,
    .metatileTiles = gUnk_0852E7CC,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_08510CA0,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 7,
    .objectCount = 13,
    .objectsSortedByY = 0,
    .bgAnimSet = 11,
    .unk42 = 0,
    .doors = gLevel5Stage2Room2Doors,
    .objects = gLevel5Stage2Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 296,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][2][3], 0x0838D7FC, section .room_def_0838d7fc */
extern u8 gLevel5Stage2Room3MetatileMap[];
extern u8 gLevel5Stage2Room3BlockLayer[];
extern u8 gLevel5Stage2Room3BlockMetatiles[];
extern struct Door gLevel5Stage2Room3Doors[];
extern u8 gLevel5Stage2Room3Objects[];
struct RoomDef gLevel5Stage2Room3 ROOM_DEF(0838d7fc) = {
    .filler00 = { 5, 2, 3, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage2Room3MetatileMap,
    .blockLayer = gLevel5Stage2Room3BlockLayer,
    .blockMetatiles = gLevel5Stage2Room3BlockMetatiles,
    .width = 46,
    .height = 36,
    .bg2Palette = gUnk_0852D6E8,
    .bg2Tiles = gUnk_08534160,
    .metatileTiles = gUnk_0852E7CC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08510CA0,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 10,
    .objectsSortedByY = 0,
    .bgAnimSet = 11,
    .unk42 = 0,
    .doors = gLevel5Stage2Room3Doors,
    .objects = gLevel5Stage2Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 215,
    .entryY = 520,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][2][4], 0x0838DBEC, section .room_def_0838dbec */
extern u8 gLevel5Stage2Room4MetatileMap[];
extern u8 gLevel5Stage2Room4BlockLayer[];
extern u8 gLevel5Stage2Room4BlockMetatiles[];
extern struct Door gLevel5Stage2Room4Doors[];
extern u8 gLevel5Stage2Room4Objects[];
struct RoomDef gLevel5Stage2Room4 ROOM_DEF(0838dbec) = {
    .filler00 = { 5, 2, 4, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage2Room4MetatileMap,
    .blockLayer = gLevel5Stage2Room4BlockLayer,
    .blockMetatiles = gLevel5Stage2Room4BlockMetatiles,
    .width = 48,
    .height = 13,
    .bg2Palette = gUnk_08505A1C,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_08510CA0,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 11,
    .objectsSortedByY = 0,
    .bgAnimSet = 3,
    .unk42 = 0,
    .doors = gLevel5Stage2Room4Doors,
    .objects = gLevel5Stage2Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 120,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][2][5], 0x0838DD58, section .room_def_0838dd58 */
extern u8 gLevel5Stage2Room5MetatileMap[];
extern u8 gLevel5Stage2Room5BlockLayer[];
extern u8 gLevel5Stage2Room5BlockMetatiles[];
extern struct Door gLevel5Stage2Room5Doors[];
struct RoomDef gLevel5Stage2Room5 ROOM_DEF(0838dd58) = {
    .filler00 = { 5, 2, 5, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage2Room5MetatileMap,
    .blockLayer = gLevel5Stage2Room5BlockLayer,
    .blockMetatiles = gLevel5Stage2Room5BlockMetatiles,
    .width = 16,
    .height = 21,
    .bg2Palette = gUnk_0852D560,
    .bg2Tiles = gUnk_0852F948,
    .metatileTiles = gUnk_0852D870,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08510CA0,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage2Room5Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 88,
    .entryY = 312,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][2][6], 0x0838DEC4, section .room_def_0838dec4 */
extern u8 gLevel5Stage2Room6MetatileMap[];
extern u8 gLevel5Stage2Room6BlockLayer[];
extern u8 gLevel5Stage2Room6BlockMetatiles[];
extern struct Door gLevel5Stage2Room6Doors[];
extern u8 gLevel5Stage2Room6Objects[];
struct RoomDef gLevel5Stage2Room6 ROOM_DEF(0838dec4) = {
    .filler00 = { 5, 2, 6, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage2Room6MetatileMap,
    .blockLayer = gLevel5Stage2Room6BlockLayer,
    .blockMetatiles = gLevel5Stage2Room6BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0852D560,
    .bg2Tiles = gUnk_0852F948,
    .metatileTiles = gUnk_0852D870,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08510CA0,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 12,
    .unk42 = 0,
    .doors = gLevel5Stage2Room6Doors,
    .objects = gLevel5Stage2Room6Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 217,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][2][7], 0x0838E038, section .room_def_0838e038 */
extern u8 gLevel5Stage2Room7MetatileMap[];
extern u8 gLevel5Stage2Room7BlockLayer[];
extern u8 gLevel5Stage2Room7BlockMetatiles[];
extern struct Door gLevel5Stage2Room7Doors[];
extern u8 gLevel5Stage2Room7Objects[];
struct RoomDef gLevel5Stage2Room7 ROOM_DEF(0838e038) = {
    .filler00 = { 5, 2, 7, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage2Room7MetatileMap,
    .blockLayer = gLevel5Stage2Room7BlockLayer,
    .blockMetatiles = gLevel5Stage2Room7BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0852D560,
    .bg2Tiles = gUnk_0852F948,
    .metatileTiles = gUnk_0852D870,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08510CA0,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 12,
    .unk42 = 0,
    .doors = gLevel5Stage2Room7Doors,
    .objects = gLevel5Stage2Room7Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 57,
    .entryY = 103,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][2][8], 0x0838E2C8, section .room_def_0838e2c8 */
extern u8 gLevel5Stage2Room8MetatileMap[];
extern u8 gLevel5Stage2Room8BlockLayer[];
extern u8 gLevel5Stage2Room8BlockMetatiles[];
extern struct Door gLevel5Stage2Room8Doors[];
extern u8 gLevel5Stage2Room8Objects[];
struct RoomDef gLevel5Stage2Room8 ROOM_DEF(0838e2c8) = {
    .filler00 = { 5, 2, 8, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage2Room8MetatileMap,
    .blockLayer = gLevel5Stage2Room8BlockLayer,
    .blockMetatiles = gLevel5Stage2Room8BlockMetatiles,
    .width = 17,
    .height = 11,
    .bg2Palette = gUnk_0852D560,
    .bg2Tiles = gUnk_0852F948,
    .metatileTiles = gUnk_0852D870,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_08510CA0,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 3,
    .objectCount = 3,
    .objectsSortedByY = 0,
    .bgAnimSet = 12,
    .unk42 = 0,
    .doors = gLevel5Stage2Room8Doors,
    .objects = gLevel5Stage2Room8Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][2][9], 0x0838E478, section .room_def_0838e478 */
extern u8 gLevel5Stage2Room9MetatileMap[];
extern u8 gLevel5Stage2Room9BlockLayer[];
extern u8 gLevel5Stage2Room9BlockMetatiles[];
extern struct Door gLevel5Stage2Room9Doors[];
extern u8 gLevel5Stage2Room9Objects[];
struct RoomDef gLevel5Stage2Room9 ROOM_DEF(0838e478) = {
    .filler00 = { 5, 2, 9, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage2Room9MetatileMap,
    .blockLayer = gLevel5Stage2Room9BlockLayer,
    .blockMetatiles = gLevel5Stage2Room9BlockMetatiles,
    .width = 16,
    .height = 13,
    .bg2Palette = gUnk_0852D560,
    .bg2Tiles = gUnk_0852F948,
    .metatileTiles = gUnk_0852D870,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_08510CA0,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 3,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 12,
    .unk42 = 0,
    .doors = gLevel5Stage2Room9Doors,
    .objects = gLevel5Stage2Room9Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][2][10], 0x0838E688, section .room_def_0838e688 */
extern u8 gLevel5Stage2Room10MetatileMap[];
extern u8 gLevel5Stage2Room10BlockLayer[];
extern u8 gLevel5Stage2Room10BlockMetatiles[];
extern struct Door gLevel5Stage2Room10Doors[];
extern u8 gLevel5Stage2Room10Objects[];
struct RoomDef gLevel5Stage2Room10 ROOM_DEF(0838e688) = {
    .filler00 = { 5, 2, 10, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage2Room10MetatileMap,
    .blockLayer = gLevel5Stage2Room10BlockLayer,
    .blockMetatiles = gLevel5Stage2Room10BlockMetatiles,
    .width = 17,
    .height = 13,
    .bg2Palette = gUnk_0852D560,
    .bg2Tiles = gUnk_0852F948,
    .metatileTiles = gUnk_0852D870,
    .borderX = 16,
    .borderY = 24,
    .bg3Palette = gUnk_08510CA0,
    .bg3Tiles = gUnk_0845A054,
    .bg3Map = &gUnk_083CD3B8,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 12,
    .unk42 = 0,
    .doors = gLevel5Stage2Room10Doors,
    .objects = gLevel5Stage2Room10Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 104,
    .entryY = 72,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][2][11], 0x0838E818, section .room_def_0838e818 */
extern u8 gLevel5Stage2Room11MetatileMap[];
extern u8 gLevel5Stage2Room11BlockLayer[];
extern u8 gLevel5Stage2Room11BlockMetatiles[];
extern struct Door gLevel5Stage2Room11Doors[];
extern u8 gLevel5Stage2Room11Objects[];
struct RoomDef gLevel5Stage2Room11 ROOM_DEF(0838e818) = {
    .filler00 = { 5, 2, 11, 0 },
    .bgm = 12,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage2Room11MetatileMap,
    .blockLayer = gLevel5Stage2Room11BlockLayer,
    .blockMetatiles = gLevel5Stage2Room11BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0852D3D8,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CACC,
    .bg3Tiles = gUnk_083EB890,
    .bg3Map = &gUnk_083B7994,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage2Room11Doors,
    .objects = gLevel5Stage2Room11Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 8,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][3][0], 0x0838EEE4, section .room_def_0838eee4 */
extern u8 gLevel5Stage3Room0MetatileMap[];
extern u8 gLevel5Stage3Room0BlockLayer[];
extern u8 gLevel5Stage3Room0BlockMetatiles[];
extern struct Door gLevel5Stage3Room0Doors[];
extern u8 gLevel5Stage3Room0Objects[];
struct RoomDef gLevel5Stage3Room0 ROOM_DEF(0838eee4) = {
    .filler00 = { 5, 3, 0, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage3Room0MetatileMap,
    .blockLayer = gLevel5Stage3Room0BlockLayer,
    .blockMetatiles = gLevel5Stage3Room0BlockMetatiles,
    .width = 48,
    .height = 26,
    .bg2Palette = gUnk_08505A1C,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_08510B9C,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 17,
    .objectsSortedByY = 0,
    .bgAnimSet = 3,
    .unk42 = 0,
    .doors = gLevel5Stage3Room0Doors,
    .objects = gLevel5Stage3Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 184,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][3][1], 0x0838F788, section .room_def_0838f788 */
extern u8 gLevel5Stage3Room1MetatileMap[];
extern u8 gLevel5Stage3Room1BlockLayer[];
extern u8 gLevel5Stage3Room1BlockMetatiles[];
extern struct Door gLevel5Stage3Room1Doors[];
extern u8 gLevel5Stage3Room1Objects[];
struct RoomDef gLevel5Stage3Room1 ROOM_DEF(0838f788) = {
    .filler00 = { 5, 3, 1, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage3Room1MetatileMap,
    .blockLayer = gLevel5Stage3Room1BlockLayer,
    .blockMetatiles = gLevel5Stage3Room1BlockMetatiles,
    .width = 35,
    .height = 33,
    .bg2Palette = gUnk_0849A6B4,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 16,
    .borderY = 24,
    .bg3Palette = gUnk_0853C6BC,
    .bg3Tiles = gUnk_083D4E14,
    .bg3Map = &gUnk_083B5538,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 3,
    .objectCount = 18,
    .objectsSortedByY = 0,
    .bgAnimSet = 2,
    .unk42 = 0,
    .doors = gLevel5Stage3Room1Doors,
    .objects = gLevel5Stage3Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 72,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][3][2], 0x0838F914, section .room_def_0838f914 */
extern u8 gLevel5Stage3Room2MetatileMap[];
extern u8 gLevel5Stage3Room2BlockLayer[];
extern u8 gLevel5Stage3Room2BlockMetatiles[];
extern struct Door gLevel5Stage3Room2Doors[];
extern u8 gLevel5Stage3Room2Objects[];
struct RoomDef gLevel5Stage3Room2 ROOM_DEF(0838f914) = {
    .filler00 = { 5, 3, 2, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage3Room2MetatileMap,
    .blockLayer = gLevel5Stage3Room2BlockLayer,
    .blockMetatiles = gLevel5Stage3Room2BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_08505A1C,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08510B9C,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 3,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage3Room2Doors,
    .objects = gLevel5Stage3Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 120,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][3][3], 0x0838FCF4, section .room_def_0838fcf4 */
extern u8 gLevel5Stage3Room3MetatileMap[];
extern u8 gLevel5Stage3Room3BlockLayer[];
extern u8 gLevel5Stage3Room3BlockMetatiles[];
extern struct Door gLevel5Stage3Room3Doors[];
extern u8 gLevel5Stage3Room3Objects[];
struct RoomDef gLevel5Stage3Room3 ROOM_DEF(0838fcf4) = {
    .filler00 = { 5, 3, 3, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage3Room3MetatileMap,
    .blockLayer = gLevel5Stage3Room3BlockLayer,
    .blockMetatiles = gLevel5Stage3Room3BlockMetatiles,
    .width = 16,
    .height = 36,
    .bg2Palette = gUnk_08505A1C,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08510B9C,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 10,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage3Room3Doors,
    .objects = gLevel5Stage3Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 520,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][3][4], 0x083900AC, section .room_def_083900ac */
extern u8 gLevel5Stage3Room4MetatileMap[];
extern u8 gLevel5Stage3Room4BlockLayer[];
extern u8 gLevel5Stage3Room4BlockMetatiles[];
extern struct Door gLevel5Stage3Room4Doors[];
extern u8 gLevel5Stage3Room4Objects[];
struct RoomDef gLevel5Stage3Room4 ROOM_DEF(083900ac) = {
    .filler00 = { 5, 3, 4, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage3Room4MetatileMap,
    .blockLayer = gLevel5Stage3Room4BlockLayer,
    .blockMetatiles = gLevel5Stage3Room4BlockMetatiles,
    .width = 48,
    .height = 13,
    .bg2Palette = gUnk_08505A1C,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_08510B9C,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 16,
    .objectsSortedByY = 0,
    .bgAnimSet = 3,
    .unk42 = 0,
    .doors = gLevel5Stage3Room4Doors,
    .objects = gLevel5Stage3Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 41,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][3][5], 0x083902B0, section .room_def_083902b0 */
extern u8 gLevel5Stage3Room5MetatileMap[];
extern u8 gLevel5Stage3Room5BlockLayer[];
extern u8 gLevel5Stage3Room5BlockMetatiles[];
extern struct Door gLevel5Stage3Room5Doors[];
extern u8 gLevel5Stage3Room5Objects[];
struct RoomDef gLevel5Stage3Room5 ROOM_DEF(083902b0) = {
    .filler00 = { 5, 3, 5, 0 },
    .bgm = 21,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage3Room5MetatileMap,
    .blockLayer = gLevel5Stage3Room5BlockLayer,
    .blockMetatiles = gLevel5Stage3Room5BlockMetatiles,
    .width = 17,
    .height = 11,
    .bg2Palette = gUnk_0849A6B4,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0853C6BC,
    .bg3Tiles = gUnk_083D4E14,
    .bg3Map = &gUnk_083B5538,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 2,
    .unk42 = 0,
    .doors = gLevel5Stage3Room5Doors,
    .objects = gLevel5Stage3Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][4][0], 0x0839059C, section .room_def_0839059c */
extern u8 gLevel5Stage4Room0MetatileMap[];
extern u8 gLevel5Stage4Room0BlockLayer[];
extern u8 gLevel5Stage4Room0BlockMetatiles[];
extern struct Door gLevel5Stage4Room0Doors[];
extern u8 gLevel5Stage4Room0Objects[];
struct RoomDef gLevel5Stage4Room0 ROOM_DEF(0839059c) = {
    .filler00 = { 5, 4, 0, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage4Room0MetatileMap,
    .blockLayer = gLevel5Stage4Room0BlockLayer,
    .blockMetatiles = gLevel5Stage4Room0BlockMetatiles,
    .width = 33,
    .height = 13,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 0,
    .bgAnimSet = 4,
    .unk42 = 0,
    .doors = gLevel5Stage4Room0Doors,
    .objects = gLevel5Stage4Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 56,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][4][1], 0x08390BA8, section .room_def_08390ba8 */
extern u8 gLevel5Stage4Room1MetatileMap[];
extern u8 gLevel5Stage4Room1BlockLayer[];
extern u8 gLevel5Stage4Room1BlockMetatiles[];
extern struct Door gLevel5Stage4Room1Doors[];
extern u8 gLevel5Stage4Room1Objects[];
struct RoomDef gLevel5Stage4Room1 ROOM_DEF(08390ba8) = {
    .filler00 = { 5, 4, 1, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage4Room1MetatileMap,
    .blockLayer = gLevel5Stage4Room1BlockLayer,
    .blockMetatiles = gLevel5Stage4Room1BlockMetatiles,
    .width = 32,
    .height = 45,
    .bg2Palette = gUnk_0852BB78,
    .bg2Tiles = gUnk_08442F18,
    .metatileTiles = gUnk_083B2254,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 9,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage4Room1Doors,
    .objects = gLevel5Stage4Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 680,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][4][2], 0x08391138, section .room_def_08391138 */
extern u8 gLevel5Stage4Room2MetatileMap[];
extern u8 gLevel5Stage4Room2BlockLayer[];
extern u8 gLevel5Stage4Room2BlockMetatiles[];
extern struct Door gLevel5Stage4Room2Doors[];
extern u8 gLevel5Stage4Room2Objects[];
struct RoomDef gLevel5Stage4Room2 ROOM_DEF(08391138) = {
    .filler00 = { 5, 4, 2, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage4Room2MetatileMap,
    .blockLayer = gLevel5Stage4Room2BlockLayer,
    .blockMetatiles = gLevel5Stage4Room2BlockMetatiles,
    .width = 16,
    .height = 25,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 6,
    .objectsSortedByY = 1,
    .bgAnimSet = 4,
    .unk42 = 0,
    .doors = gLevel5Stage4Room2Doors,
    .objects = gLevel5Stage4Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 56,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][4][3], 0x08391C40, section .room_def_08391c40 */
extern u8 gLevel5Stage4Room3MetatileMap[];
extern u8 gLevel5Stage4Room3BlockLayer[];
extern u8 gLevel5Stage4Room3BlockMetatiles[];
extern struct Door gLevel5Stage4Room3Doors[];
extern u8 gLevel5Stage4Room3Objects[];
struct RoomDef gLevel5Stage4Room3 ROOM_DEF(08391c40) = {
    .filler00 = { 5, 4, 3, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage4Room3MetatileMap,
    .blockLayer = gLevel5Stage4Room3BlockLayer,
    .blockMetatiles = gLevel5Stage4Room3BlockMetatiles,
    .width = 64,
    .height = 23,
    .bg2Palette = gUnk_084BB068,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 17,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage4Room3Doors,
    .objects = gLevel5Stage4Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 312,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][4][4], 0x08391D9C, section .room_def_08391d9c */
extern u8 gLevel5Stage4Room4MetatileMap[];
extern u8 gLevel5Stage4Room4BlockLayer[];
extern u8 gLevel5Stage4Room4BlockMetatiles[];
extern struct Door gLevel5Stage4Room4Doors[];
extern u8 gLevel5Stage4Room4Objects[];
struct RoomDef gLevel5Stage4Room4 ROOM_DEF(08391d9c) = {
    .filler00 = { 5, 4, 4, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage4Room4MetatileMap,
    .blockLayer = gLevel5Stage4Room4BlockLayer,
    .blockMetatiles = gLevel5Stage4Room4BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0852D49C,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage4Room4Doors,
    .objects = gLevel5Stage4Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 88,
    .entryY = 8,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][5][0], 0x08392308, section .room_def_08392308 */
extern u8 gLevel5Stage5Room0MetatileMap[];
extern u8 gLevel5Stage5Room0BlockLayer[];
extern u8 gLevel5Stage5Room0BlockMetatiles[];
extern struct Door gLevel5Stage5Room0Doors[];
extern u8 gLevel5Stage5Room0Objects[];
struct RoomDef gLevel5Stage5Room0 ROOM_DEF(08392308) = {
    .filler00 = { 5, 5, 0, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage5Room0MetatileMap,
    .blockLayer = gLevel5Stage5Room0BlockLayer,
    .blockMetatiles = gLevel5Stage5Room0BlockMetatiles,
    .width = 48,
    .height = 26,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 9,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage5Room0Doors,
    .objects = gLevel5Stage5Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][5][1], 0x08392778, section .room_def_08392778 */
extern u8 gLevel5Stage5Room1MetatileMap[];
extern u8 gLevel5Stage5Room1BlockLayer[];
extern u8 gLevel5Stage5Room1BlockMetatiles[];
extern struct Door gLevel5Stage5Room1Doors[];
extern u8 gLevel5Stage5Room1Objects[];
struct RoomDef gLevel5Stage5Room1 ROOM_DEF(08392778) = {
    .filler00 = { 5, 5, 1, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage5Room1MetatileMap,
    .blockLayer = gLevel5Stage5Room1BlockLayer,
    .blockMetatiles = gLevel5Stage5Room1BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gUnk_0852D49C,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 17,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage5Room1Doors,
    .objects = gLevel5Stage5Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 88,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][5][2], 0x08392AA8, section .room_def_08392aa8 */
extern u8 gLevel5Stage5Room2MetatileMap[];
extern u8 gLevel5Stage5Room2BlockLayer[];
extern u8 gLevel5Stage5Room2BlockMetatiles[];
extern struct Door gLevel5Stage5Room2Doors[];
struct RoomDef gLevel5Stage5Room2 ROOM_DEF(08392aa8) = {
    .filler00 = { 5, 5, 2, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage5Room2MetatileMap,
    .blockLayer = gLevel5Stage5Room2BlockLayer,
    .blockMetatiles = gLevel5Stage5Room2BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0849A5F0,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage5Room2Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][5][3], 0x08392F0C, section .room_def_08392f0c */
extern u8 gLevel5Stage5Room3MetatileMap[];
extern u8 gLevel5Stage5Room3BlockLayer[];
extern u8 gLevel5Stage5Room3BlockMetatiles[];
extern struct Door gLevel5Stage5Room3Doors[];
extern u8 gLevel5Stage5Room3Objects[];
struct RoomDef gLevel5Stage5Room3 ROOM_DEF(08392f0c) = {
    .filler00 = { 5, 5, 3, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage5Room3MetatileMap,
    .blockLayer = gLevel5Stage5Room3BlockLayer,
    .blockMetatiles = gLevel5Stage5Room3BlockMetatiles,
    .width = 62,
    .height = 13,
    .bg2Palette = gUnk_0849A5F0,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 10,
    .objectsSortedByY = 0,
    .bgAnimSet = 2,
    .unk42 = 0,
    .doors = gLevel5Stage5Room3Doors,
    .objects = gLevel5Stage5Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 88,
    .entryY = 88,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][5][4], 0x08393430, section .room_def_08393430 */
extern u8 gLevel5Stage5Room4MetatileMap[];
extern u8 gLevel5Stage5Room4BlockLayer[];
extern u8 gLevel5Stage5Room4BlockMetatiles[];
extern struct Door gLevel5Stage5Room4Doors[];
extern u8 gLevel5Stage5Room4Objects[];
struct RoomDef gLevel5Stage5Room4 ROOM_DEF(08393430) = {
    .filler00 = { 5, 5, 4, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage5Room4MetatileMap,
    .blockLayer = gLevel5Stage5Room4BlockLayer,
    .blockMetatiles = gLevel5Stage5Room4BlockMetatiles,
    .width = 80,
    .height = 13,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 14,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage5Room4Doors,
    .objects = gLevel5Stage5Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][5][5], 0x083935AC, section .room_def_083935ac */
extern u8 gLevel5Stage5Room5MetatileMap[];
extern u8 gLevel5Stage5Room5BlockLayer[];
extern u8 gLevel5Stage5Room5BlockMetatiles[];
extern struct Door gLevel5Stage5Room5Doors[];
struct RoomDef gLevel5Stage5Room5 ROOM_DEF(083935ac) = {
    .filler00 = { 5, 5, 5, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage5Room5MetatileMap,
    .blockLayer = gLevel5Stage5Room5BlockLayer,
    .blockMetatiles = gLevel5Stage5Room5BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 4,
    .unk42 = 0,
    .doors = gLevel5Stage5Room5Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][5][6], 0x08393760, section .room_def_08393760 */
extern u8 gLevel5Stage5Room6MetatileMap[];
extern u8 gLevel5Stage5Room6BlockLayer[];
extern u8 gLevel5Stage5Room6BlockMetatiles[];
extern struct Door gLevel5Stage5Room6Doors[];
extern u8 gLevel5Stage5Room6Objects[];
struct RoomDef gLevel5Stage5Room6 ROOM_DEF(08393760) = {
    .filler00 = { 5, 5, 6, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage5Room6MetatileMap,
    .blockLayer = gLevel5Stage5Room6BlockLayer,
    .blockMetatiles = gLevel5Stage5Room6BlockMetatiles,
    .width = 17,
    .height = 13,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 16,
    .borderY = 24,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage5Room6Doors,
    .objects = gLevel5Stage5Room6Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][5][7], 0x083938C0, section .room_def_083938c0 */
extern u8 gLevel5Stage5Room7MetatileMap[];
extern u8 gLevel5Stage5Room7BlockLayer[];
extern u8 gLevel5Stage5Room7BlockMetatiles[];
extern struct Door gLevel5Stage5Room7Doors[];
extern u8 gLevel5Stage5Room7Objects[];
struct RoomDef gLevel5Stage5Room7 ROOM_DEF(083938c0) = {
    .filler00 = { 5, 5, 7, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage5Room7MetatileMap,
    .blockLayer = gLevel5Stage5Room7BlockLayer,
    .blockMetatiles = gLevel5Stage5Room7BlockMetatiles,
    .width = 16,
    .height = 13,
    .bg2Palette = gUnk_0849A5F0,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 2,
    .unk42 = 0,
    .doors = gLevel5Stage5Room7Doors,
    .objects = gLevel5Stage5Room7Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][5][8], 0x08393A7C, section .room_def_08393a7c */
extern u8 gLevel5Stage5Room8MetatileMap[];
extern u8 gLevel5Stage5Room8BlockLayer[];
extern u8 gLevel5Stage5Room8BlockMetatiles[];
extern struct Door gLevel5Stage5Room8Doors[];
extern u8 gLevel5Stage5Room8Objects[];
struct RoomDef gLevel5Stage5Room8 ROOM_DEF(08393a7c) = {
    .filler00 = { 5, 5, 8, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage5Room8MetatileMap,
    .blockLayer = gLevel5Stage5Room8BlockLayer,
    .blockMetatiles = gLevel5Stage5Room8BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0849A5F0,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage5Room8Doors,
    .objects = gLevel5Stage5Room8Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 136,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][5][9], 0x08393C2C, section .room_def_08393c2c */
extern u8 gLevel5Stage5Room9MetatileMap[];
extern u8 gLevel5Stage5Room9BlockLayer[];
extern u8 gLevel5Stage5Room9BlockMetatiles[];
extern struct Door gLevel5Stage5Room9Doors[];
extern u8 gLevel5Stage5Room9Objects[];
struct RoomDef gLevel5Stage5Room9 ROOM_DEF(08393c2c) = {
    .filler00 = { 5, 5, 9, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage5Room9MetatileMap,
    .blockLayer = gLevel5Stage5Room9BlockLayer,
    .blockMetatiles = gLevel5Stage5Room9BlockMetatiles,
    .width = 18,
    .height = 11,
    .bg2Palette = gUnk_0849A5F0,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 24,
    .borderY = 8,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage5Room9Doors,
    .objects = gLevel5Stage5Room9Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][5][10], 0x08393DC8, section .room_def_08393dc8 */
extern u8 gLevel5Stage5Room10MetatileMap[];
extern u8 gLevel5Stage5Room10BlockLayer[];
extern u8 gLevel5Stage5Room10BlockMetatiles[];
extern struct Door gLevel5Stage5Room10Doors[];
extern u8 gLevel5Stage5Room10Objects[];
struct RoomDef gLevel5Stage5Room10 ROOM_DEF(08393dc8) = {
    .filler00 = { 5, 5, 10, 0 },
    .bgm = 37,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage5Room10MetatileMap,
    .blockLayer = gLevel5Stage5Room10BlockLayer,
    .blockMetatiles = gLevel5Stage5Room10BlockMetatiles,
    .width = 18,
    .height = 12,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 24,
    .borderY = 8,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel5Stage5Room10Doors,
    .objects = gLevel5Stage5Room10Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 152,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[5][6][0], 0x08393EF4, section .room_def_08393ef4 */
extern u8 gLevel5Stage6Room0MetatileMap[];
extern u8 gLevel5Stage6Room0BlockLayer[];
extern u8 gLevel5Stage6Room0BlockMetatiles[];
extern u8 gLevel5Stage6Room0Objects[];
struct RoomDef gLevel5Stage6Room0 ROOM_DEF(08393ef4) = {
    .filler00 = { 5, 6, 0, 0 },
    .bgm = 30,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel5Stage6Room0MetatileMap,
    .blockLayer = gLevel5Stage6Room0BlockLayer,
    .blockMetatiles = gLevel5Stage6Room0BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gLevel5Stage6Room0Bg2Palette,
    .bg2Tiles = gUnk_084985B0,
    .metatileTiles = gUnk_08497D38,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gLevel5Stage6Room0Bg3Palette,
    .bg3Tiles = gLevel5Stage6Room0Bg3Tiles,
    .bg3Map = &gLevel5Stage6Room0Bg3Map,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = gLevel5Stage6Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 137,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 5,
};

/* gRoomTable[6][0][0], 0x08394458, section .room_def_08394458 */
extern u8 gLevel6Stage0Room0MetatileMap[];
extern u8 gLevel6Stage0Room0BlockLayer[];
extern u8 gLevel6Stage0Room0BlockMetatiles[];
extern struct Door gLevel6Stage0Room0Doors[];
extern u8 gLevel6Stage0Room0Objects[];
struct RoomDef gLevel6Stage0Room0 ROOM_DEF(08394458) = {
    .filler00 = { 6, 0, 0, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage0Room0MetatileMap,
    .blockLayer = gLevel6Stage0Room0BlockLayer,
    .blockMetatiles = gLevel6Stage0Room0BlockMetatiles,
    .width = 67,
    .height = 13,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 14,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage0Room0Doors,
    .objects = gLevel6Stage0Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 88,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][0][1], 0x08394C58, section .room_def_08394c58 */
extern u8 gLevel6Stage0Room1MetatileMap[];
extern u8 gLevel6Stage0Room1BlockLayer[];
extern u8 gLevel6Stage0Room1BlockMetatiles[];
extern struct Door gLevel6Stage0Room1Doors[];
extern u8 gLevel6Stage0Room1Objects[];
struct RoomDef gLevel6Stage0Room1 ROOM_DEF(08394c58) = {
    .filler00 = { 6, 0, 1, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage0Room1MetatileMap,
    .blockLayer = gLevel6Stage0Room1BlockLayer,
    .blockMetatiles = gLevel6Stage0Room1BlockMetatiles,
    .width = 47,
    .height = 12,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 10,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage0Room1Doors,
    .objects = gLevel6Stage0Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][0][2], 0x083950F8, section .room_def_083950f8 */
extern u8 gLevel6Stage0Room2MetatileMap[];
extern u8 gLevel6Stage0Room2BlockLayer[];
extern u8 gLevel6Stage0Room2BlockMetatiles[];
extern struct Door gLevel6Stage0Room2Doors[];
extern u8 gLevel6Stage0Room2Objects[];
struct RoomDef gLevel6Stage0Room2 ROOM_DEF(083950f8) = {
    .filler00 = { 6, 0, 2, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage0Room2MetatileMap,
    .blockLayer = gLevel6Stage0Room2BlockLayer,
    .blockMetatiles = gLevel6Stage0Room2BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gUnk_0852BB78,
    .bg2Tiles = gUnk_08442F18,
    .metatileTiles = gUnk_083B2254,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 16,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage0Room2Doors,
    .objects = gLevel6Stage0Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][0][3], 0x08395298, section .room_def_08395298 */
extern u8 gLevel6Stage0Room3MetatileMap[];
extern u8 gLevel6Stage0Room3BlockLayer[];
extern u8 gLevel6Stage0Room3BlockMetatiles[];
extern struct Door gLevel6Stage0Room3Doors[];
extern u8 gLevel6Stage0Room3Objects[];
struct RoomDef gLevel6Stage0Room3 ROOM_DEF(08395298) = {
    .filler00 = { 6, 0, 3, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage0Room3MetatileMap,
    .blockLayer = gLevel6Stage0Room3BlockLayer,
    .blockMetatiles = gLevel6Stage0Room3BlockMetatiles,
    .width = 17,
    .height = 11,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage0Room3Doors,
    .objects = gLevel6Stage0Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 136,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][0], 0x08395450, section .room_def_08395450 */
extern u8 gLevel6Stage1Room0MetatileMap[];
extern u8 gLevel6Stage1Room0BlockLayer[];
extern u8 gLevel6Stage1Room0BlockMetatiles[];
extern struct Door gLevel6Stage1Room0Doors[];
struct RoomDef gLevel6Stage1Room0 ROOM_DEF(08395450) = {
    .filler00 = { 6, 1, 0, 0 },
    .bgm = 41,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room0MetatileMap,
    .blockLayer = gLevel6Stage1Room0BlockLayer,
    .blockMetatiles = gLevel6Stage1Room0BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_084BB4FC,
    .bg2Tiles = gUnk_0843FC10,
    .metatileTiles = gUnk_083B18FC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CFE0,
    .bg3Tiles = gUnk_08407278,
    .bg3Map = &gUnk_083BB70C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 4,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room0Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][1], 0x0839570C, section .room_def_0839570c */
extern u8 gLevel6Stage1Room1MetatileMap[];
extern u8 gLevel6Stage1Room1BlockLayer[];
extern u8 gLevel6Stage1Room1BlockMetatiles[];
extern struct Door gLevel6Stage1Room1Doors[];
struct RoomDef gLevel6Stage1Room1 ROOM_DEF(0839570c) = {
    .filler00 = { 6, 1, 1, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room1MetatileMap,
    .blockLayer = gLevel6Stage1Room1BlockLayer,
    .blockMetatiles = gLevel6Stage1Room1BlockMetatiles,
    .width = 16,
    .height = 22,
    .bg2Palette = gUnk_084BB438,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CFE0,
    .bg3Tiles = gUnk_08407278,
    .bg3Map = &gUnk_083BB70C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room1Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 280,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][2], 0x083959C8, section .room_def_083959c8 */
extern u8 gLevel6Stage1Room2MetatileMap[];
extern u8 gLevel6Stage1Room2BlockLayer[];
extern u8 gLevel6Stage1Room2BlockMetatiles[];
extern struct Door gLevel6Stage1Room2Doors[];
struct RoomDef gLevel6Stage1Room2 ROOM_DEF(083959c8) = {
    .filler00 = { 6, 1, 2, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room2MetatileMap,
    .blockLayer = gLevel6Stage1Room2BlockLayer,
    .blockMetatiles = gLevel6Stage1Room2BlockMetatiles,
    .width = 16,
    .height = 22,
    .bg2Palette = gUnk_084BB438,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gLevel6Stage1Room2Bg3Palette,
    .bg3Tiles = gLevel6Stage1Room2Bg3Tiles,
    .bg3Map = &gLevel6Stage1Room2Bg3Map,
    .bg3BorderX = 0,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room2Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 280,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][3], 0x08395C90, section .room_def_08395c90 */
extern u8 gLevel6Stage1Room3MetatileMap[];
extern u8 gLevel6Stage1Room3BlockLayer[];
extern u8 gLevel6Stage1Room3BlockMetatiles[];
extern struct Door gLevel6Stage1Room3Doors[];
struct RoomDef gLevel6Stage1Room3 ROOM_DEF(08395c90) = {
    .filler00 = { 6, 1, 3, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room3MetatileMap,
    .blockLayer = gLevel6Stage1Room3BlockLayer,
    .blockMetatiles = gLevel6Stage1Room3BlockMetatiles,
    .width = 16,
    .height = 22,
    .bg2Palette = gUnk_084BB438,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084D52B4,
    .bg3Tiles = gUnk_083EE8B4,
    .bg3Map = &gUnk_083B82B0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room3Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 280,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][4], 0x08395F34, section .room_def_08395f34 */
extern u8 gLevel6Stage1Room4MetatileMap[];
extern u8 gLevel6Stage1Room4BlockLayer[];
extern u8 gLevel6Stage1Room4BlockMetatiles[];
extern struct Door gLevel6Stage1Room4Doors[];
struct RoomDef gLevel6Stage1Room4 ROOM_DEF(08395f34) = {
    .filler00 = { 6, 1, 4, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room4MetatileMap,
    .blockLayer = gLevel6Stage1Room4BlockLayer,
    .blockMetatiles = gLevel6Stage1Room4BlockMetatiles,
    .width = 16,
    .height = 22,
    .bg2Palette = gUnk_084BB438,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gLevel6Stage1Room4Bg3Palette,
    .bg3Tiles = gUnk_083E2A30,
    .bg3Map = &gUnk_083B6770,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room4Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 280,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][5], 0x083961E4, section .room_def_083961e4 */
extern u8 gLevel6Stage1Room5MetatileMap[];
extern u8 gLevel6Stage1Room5BlockLayer[];
extern u8 gLevel6Stage1Room5BlockMetatiles[];
extern struct Door gLevel6Stage1Room5Doors[];
struct RoomDef gLevel6Stage1Room5 ROOM_DEF(083961e4) = {
    .filler00 = { 6, 1, 5, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room5MetatileMap,
    .blockLayer = gLevel6Stage1Room5BlockLayer,
    .blockMetatiles = gLevel6Stage1Room5BlockMetatiles,
    .width = 16,
    .height = 22,
    .bg2Palette = gUnk_084BB438,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gLevel6Stage1Room5Bg3Palette,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room5Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 280,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][6], 0x083964AC, section .room_def_083964ac */
extern u8 gLevel6Stage1Room6MetatileMap[];
extern u8 gLevel6Stage1Room6BlockLayer[];
extern u8 gLevel6Stage1Room6BlockMetatiles[];
extern struct Door gLevel6Stage1Room6Doors[];
struct RoomDef gLevel6Stage1Room6 ROOM_DEF(083964ac) = {
    .filler00 = { 6, 1, 6, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room6MetatileMap,
    .blockLayer = gLevel6Stage1Room6BlockLayer,
    .blockMetatiles = gLevel6Stage1Room6BlockMetatiles,
    .width = 16,
    .height = 22,
    .bg2Palette = gUnk_084BB438,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CDD8,
    .bg3Tiles = gUnk_083F7470,
    .bg3Map = &gUnk_083B954C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room6Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][7], 0x08396754, section .room_def_08396754 */
extern u8 gLevel6Stage1Room7MetatileMap[];
extern u8 gLevel6Stage1Room7BlockLayer[];
extern u8 gLevel6Stage1Room7BlockMetatiles[];
extern struct Door gLevel6Stage1Room7Doors[];
struct RoomDef gLevel6Stage1Room7 ROOM_DEF(08396754) = {
    .filler00 = { 6, 1, 7, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room7MetatileMap,
    .blockLayer = gLevel6Stage1Room7BlockLayer,
    .blockMetatiles = gLevel6Stage1Room7BlockMetatiles,
    .width = 16,
    .height = 22,
    .bg2Palette = gUnk_084BB438,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CFE0,
    .bg3Tiles = gUnk_08407278,
    .bg3Map = &gUnk_083BB70C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room7Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 280,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][8], 0x08396A00, section .room_def_08396a00 */
extern u8 gLevel6Stage1Room8MetatileMap[];
extern u8 gLevel6Stage1Room8BlockLayer[];
extern u8 gLevel6Stage1Room8BlockMetatiles[];
extern struct Door gLevel6Stage1Room8Doors[];
struct RoomDef gLevel6Stage1Room8 ROOM_DEF(08396a00) = {
    .filler00 = { 6, 1, 8, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room8MetatileMap,
    .blockLayer = gLevel6Stage1Room8BlockLayer,
    .blockMetatiles = gLevel6Stage1Room8BlockMetatiles,
    .width = 16,
    .height = 22,
    .bg2Palette = gUnk_084BB438,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084D52B4,
    .bg3Tiles = gUnk_083EE8B4,
    .bg3Map = &gUnk_083B82B0,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room8Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 280,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][9], 0x08396CB8, section .room_def_08396cb8 */
extern u8 gLevel6Stage1Room9MetatileMap[];
extern u8 gLevel6Stage1Room9BlockLayer[];
extern u8 gLevel6Stage1Room9BlockMetatiles[];
extern struct Door gLevel6Stage1Room9Doors[];
struct RoomDef gLevel6Stage1Room9 ROOM_DEF(08396cb8) = {
    .filler00 = { 6, 1, 9, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room9MetatileMap,
    .blockLayer = gLevel6Stage1Room9BlockLayer,
    .blockMetatiles = gLevel6Stage1Room9BlockMetatiles,
    .width = 16,
    .height = 22,
    .bg2Palette = gUnk_084BB438,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CDD8,
    .bg3Tiles = gUnk_083F7470,
    .bg3Map = &gUnk_083B954C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room9Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 280,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][10], 0x0839706C, section .room_def_0839706c */
extern u8 gLevel6Stage1Room10MetatileMap[];
extern u8 gLevel6Stage1Room10BlockLayer[];
extern u8 gLevel6Stage1Room10BlockMetatiles[];
extern struct Door gLevel6Stage1Room10Doors[];
extern u8 gLevel6Stage1Room10Objects[];
struct RoomDef gLevel6Stage1Room10 ROOM_DEF(0839706c) = {
    .filler00 = { 6, 1, 10, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room10MetatileMap,
    .blockLayer = gLevel6Stage1Room10BlockLayer,
    .blockMetatiles = gLevel6Stage1Room10BlockMetatiles,
    .width = 16,
    .height = 60,
    .bg2Palette = gUnk_084BB4FC,
    .bg2Tiles = gUnk_0843FC10,
    .metatileTiles = gUnk_083B18FC,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E41C,
    .bg3Tiles = gUnk_0846F8DC,
    .bg3Map = &gUnk_083CEF10,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room10Doors,
    .objects = gLevel6Stage1Room10Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 136,
    .entryY = 872,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][11], 0x08397358, section .room_def_08397358 */
extern u8 gLevel6Stage1Room11MetatileMap[];
extern u8 gLevel6Stage1Room11BlockLayer[];
extern u8 gLevel6Stage1Room11BlockMetatiles[];
extern struct Door gLevel6Stage1Room11Doors[];
extern u8 gLevel6Stage1Room11Objects[];
struct RoomDef gLevel6Stage1Room11 ROOM_DEF(08397358) = {
    .filler00 = { 6, 1, 11, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room11MetatileMap,
    .blockLayer = gLevel6Stage1Room11BlockLayer,
    .blockMetatiles = gLevel6Stage1Room11BlockMetatiles,
    .width = 32,
    .height = 11,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E41C,
    .bg3Tiles = gUnk_0846F8DC,
    .bg3Map = &gUnk_083CEF10,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 3,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room11Doors,
    .objects = gLevel6Stage1Room11Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 72,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][12], 0x08397508, section .room_def_08397508 */
extern u8 gLevel6Stage1Room12MetatileMap[];
extern u8 gLevel6Stage1Room12BlockLayer[];
extern u8 gLevel6Stage1Room12BlockMetatiles[];
extern struct Door gLevel6Stage1Room12Doors[];
extern u8 gLevel6Stage1Room12Objects[];
struct RoomDef gLevel6Stage1Room12 ROOM_DEF(08397508) = {
    .filler00 = { 6, 1, 12, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room12MetatileMap,
    .blockLayer = gLevel6Stage1Room12BlockLayer,
    .blockMetatiles = gLevel6Stage1Room12BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853D53C,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E41C,
    .bg3Tiles = gUnk_0846F8DC,
    .bg3Map = &gUnk_083CEF10,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 5,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room12Doors,
    .objects = gLevel6Stage1Room12Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][13], 0x08397680, section .room_def_08397680 */
extern u8 gLevel6Stage1Room13MetatileMap[];
extern u8 gLevel6Stage1Room13BlockLayer[];
extern u8 gLevel6Stage1Room13BlockMetatiles[];
extern struct Door gLevel6Stage1Room13Doors[];
extern u8 gLevel6Stage1Room13Objects[];
struct RoomDef gLevel6Stage1Room13 ROOM_DEF(08397680) = {
    .filler00 = { 6, 1, 13, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room13MetatileMap,
    .blockLayer = gLevel6Stage1Room13BlockLayer,
    .blockMetatiles = gLevel6Stage1Room13BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853D53C,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CFE0,
    .bg3Tiles = gUnk_08407278,
    .bg3Map = &gUnk_083BB70C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room13Doors,
    .objects = gLevel6Stage1Room13Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][14], 0x08397804, section .room_def_08397804 */
extern u8 gLevel6Stage1Room14MetatileMap[];
extern u8 gLevel6Stage1Room14BlockLayer[];
extern u8 gLevel6Stage1Room14BlockMetatiles[];
extern struct Door gLevel6Stage1Room14Doors[];
extern u8 gLevel6Stage1Room14Objects[];
struct RoomDef gLevel6Stage1Room14 ROOM_DEF(08397804) = {
    .filler00 = { 6, 1, 14, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room14MetatileMap,
    .blockLayer = gLevel6Stage1Room14BlockLayer,
    .blockMetatiles = gLevel6Stage1Room14BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0852D560,
    .bg2Tiles = gUnk_0852F948,
    .metatileTiles = gUnk_0852D870,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853CFE0,
    .bg3Tiles = gUnk_08407278,
    .bg3Map = &gUnk_083BB70C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room14Doors,
    .objects = gLevel6Stage1Room14Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][15], 0x08397970, section .room_def_08397970 */
extern u8 gLevel6Stage1Room15MetatileMap[];
extern u8 gLevel6Stage1Room15BlockLayer[];
extern u8 gLevel6Stage1Room15BlockMetatiles[];
extern struct Door gLevel6Stage1Room15Doors[];
extern u8 gLevel6Stage1Room15Objects[];
struct RoomDef gLevel6Stage1Room15 ROOM_DEF(08397970) = {
    .filler00 = { 6, 1, 15, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room15MetatileMap,
    .blockLayer = gLevel6Stage1Room15BlockLayer,
    .blockMetatiles = gLevel6Stage1Room15BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_084A7CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room15Doors,
    .objects = gLevel6Stage1Room15Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][16], 0x08397B54, section .room_def_08397b54 */
extern u8 gLevel6Stage1Room16MetatileMap[];
extern u8 gLevel6Stage1Room16BlockLayer[];
extern u8 gLevel6Stage1Room16BlockMetatiles[];
extern struct Door gLevel6Stage1Room16Doors[];
extern u8 gLevel6Stage1Room16Objects[];
struct RoomDef gLevel6Stage1Room16 ROOM_DEF(08397b54) = {
    .filler00 = { 6, 1, 16, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room16MetatileMap,
    .blockLayer = gLevel6Stage1Room16BlockLayer,
    .blockMetatiles = gLevel6Stage1Room16BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853DAB4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_084A7CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room16Doors,
    .objects = gLevel6Stage1Room16Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][17], 0x08397CFC, section .room_def_08397cfc */
extern u8 gLevel6Stage1Room17MetatileMap[];
extern u8 gLevel6Stage1Room17BlockLayer[];
extern u8 gLevel6Stage1Room17BlockMetatiles[];
extern struct Door gLevel6Stage1Room17Doors[];
extern u8 gLevel6Stage1Room17Objects[];
struct RoomDef gLevel6Stage1Room17 ROOM_DEF(08397cfc) = {
    .filler00 = { 6, 1, 17, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room17MetatileMap,
    .blockLayer = gLevel6Stage1Room17BlockLayer,
    .blockMetatiles = gLevel6Stage1Room17BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0849A52C,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_084A7CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room17Doors,
    .objects = gLevel6Stage1Room17Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][18], 0x08397ED0, section .room_def_08397ed0 */
extern u8 gLevel6Stage1Room18MetatileMap[];
extern u8 gLevel6Stage1Room18BlockLayer[];
extern u8 gLevel6Stage1Room18BlockMetatiles[];
extern struct Door gLevel6Stage1Room18Doors[];
extern u8 gLevel6Stage1Room18Objects[];
struct RoomDef gLevel6Stage1Room18 ROOM_DEF(08397ed0) = {
    .filler00 = { 6, 1, 18, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room18MetatileMap,
    .blockLayer = gLevel6Stage1Room18BlockLayer,
    .blockMetatiles = gLevel6Stage1Room18BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853D12C,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_084A7CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room18Doors,
    .objects = gLevel6Stage1Room18Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][19], 0x0839804C, section .room_def_0839804c */
extern u8 gLevel6Stage1Room19MetatileMap[];
extern u8 gLevel6Stage1Room19BlockLayer[];
extern u8 gLevel6Stage1Room19BlockMetatiles[];
extern struct Door gLevel6Stage1Room19Doors[];
extern u8 gLevel6Stage1Room19Objects[];
struct RoomDef gLevel6Stage1Room19 ROOM_DEF(0839804c) = {
    .filler00 = { 6, 1, 19, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room19MetatileMap,
    .blockLayer = gLevel6Stage1Room19BlockLayer,
    .blockMetatiles = gLevel6Stage1Room19BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853D53C,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_084A7CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room19Doors,
    .objects = gLevel6Stage1Room19Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][20], 0x08398220, section .room_def_08398220 */
extern u8 gLevel6Stage1Room20MetatileMap[];
extern u8 gLevel6Stage1Room20BlockLayer[];
extern u8 gLevel6Stage1Room20BlockMetatiles[];
extern struct Door gLevel6Stage1Room20Doors[];
extern u8 gLevel6Stage1Room20Objects[];
struct RoomDef gLevel6Stage1Room20 ROOM_DEF(08398220) = {
    .filler00 = { 6, 1, 20, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room20MetatileMap,
    .blockLayer = gLevel6Stage1Room20BlockLayer,
    .blockMetatiles = gLevel6Stage1Room20BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853DAB4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_084A7CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room20Doors,
    .objects = gLevel6Stage1Room20Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][21], 0x083983D8, section .room_def_083983d8 */
extern u8 gLevel6Stage1Room21MetatileMap[];
extern u8 gLevel6Stage1Room21BlockLayer[];
extern u8 gLevel6Stage1Room21BlockMetatiles[];
extern struct Door gLevel6Stage1Room21Doors[];
extern u8 gLevel6Stage1Room21Objects[];
struct RoomDef gLevel6Stage1Room21 ROOM_DEF(083983d8) = {
    .filler00 = { 6, 1, 21, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room21MetatileMap,
    .blockLayer = gLevel6Stage1Room21BlockLayer,
    .blockMetatiles = gLevel6Stage1Room21BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0849A52C,
    .bg2Tiles = gUnk_083D1FD8,
    .metatileTiles = gUnk_083A8B50,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_084A7CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room21Doors,
    .objects = gLevel6Stage1Room21Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][22], 0x083985D0, section .room_def_083985d0 */
extern u8 gLevel6Stage1Room22MetatileMap[];
extern u8 gLevel6Stage1Room22BlockLayer[];
extern u8 gLevel6Stage1Room22BlockMetatiles[];
extern struct Door gLevel6Stage1Room22Doors[];
extern u8 gLevel6Stage1Room22Objects[];
struct RoomDef gLevel6Stage1Room22 ROOM_DEF(083985d0) = {
    .filler00 = { 6, 1, 22, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room22MetatileMap,
    .blockLayer = gLevel6Stage1Room22BlockLayer,
    .blockMetatiles = gLevel6Stage1Room22BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853D12C,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_084A7CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room22Doors,
    .objects = gLevel6Stage1Room22Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][23], 0x0839875C, section .room_def_0839875c */
extern u8 gLevel6Stage1Room23MetatileMap[];
extern u8 gLevel6Stage1Room23BlockLayer[];
extern u8 gLevel6Stage1Room23BlockMetatiles[];
extern struct Door gLevel6Stage1Room23Doors[];
extern u8 gLevel6Stage1Room23Objects[];
struct RoomDef gLevel6Stage1Room23 ROOM_DEF(0839875c) = {
    .filler00 = { 6, 1, 23, 0 },
    .bgm = 38,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room23MetatileMap,
    .blockLayer = gLevel6Stage1Room23BlockLayer,
    .blockMetatiles = gLevel6Stage1Room23BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853D53C,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084A7BF8,
    .bg3Tiles = gUnk_084A861C,
    .bg3Map = &gUnk_084A7CFC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room23Doors,
    .objects = gLevel6Stage1Room23Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][1][24], 0x08398858, section .room_def_08398858 */
extern u8 gLevel6Stage1Room24MetatileMap[];
extern u8 gLevel6Stage1Room24BlockLayer[];
extern u8 gLevel6Stage1Room24BlockMetatiles[];
extern struct Door gLevel6Stage1Room24Doors[];
struct RoomDef gLevel6Stage1Room24 ROOM_DEF(08398858) = {
    .filler00 = { 6, 1, 24, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage1Room24MetatileMap,
    .blockLayer = gLevel6Stage1Room24BlockLayer,
    .blockMetatiles = gLevel6Stage1Room24BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853D53C,
    .bg2Tiles = gUnk_0842E1C0,
    .metatileTiles = gUnk_083ACDCC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E41C,
    .bg3Tiles = gUnk_0846F8DC,
    .bg3Map = &gUnk_083CEF10,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage1Room24Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 8,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][2][0], 0x08398DDC, section .room_def_08398ddc */
extern u8 gLevel6Stage2Room0MetatileMap[];
extern u8 gLevel6Stage2Room0BlockLayer[];
extern u8 gLevel6Stage2Room0BlockMetatiles[];
extern struct Door gLevel6Stage2Room0Doors[];
extern u8 gLevel6Stage2Room0Objects[];
struct RoomDef gLevel6Stage2Room0 ROOM_DEF(08398ddc) = {
    .filler00 = { 6, 2, 0, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage2Room0MetatileMap,
    .blockLayer = gLevel6Stage2Room0BlockLayer,
    .blockMetatiles = gLevel6Stage2Room0BlockMetatiles,
    .width = 64,
    .height = 11,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 12,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage2Room0Doors,
    .objects = gLevel6Stage2Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 56,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][2][1], 0x08399208, section .room_def_08399208 */
extern u8 gLevel6Stage2Room1MetatileMap[];
extern u8 gLevel6Stage2Room1BlockLayer[];
extern u8 gLevel6Stage2Room1BlockMetatiles[];
extern struct Door gLevel6Stage2Room1Doors[];
extern u8 gLevel6Stage2Room1Objects[];
struct RoomDef gLevel6Stage2Room1 ROOM_DEF(08399208) = {
    .filler00 = { 6, 2, 1, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage2Room1MetatileMap,
    .blockLayer = gLevel6Stage2Room1BlockLayer,
    .blockMetatiles = gLevel6Stage2Room1BlockMetatiles,
    .width = 16,
    .height = 48,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 12,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage2Room1Doors,
    .objects = gLevel6Stage2Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 120,
    .entryY = 664,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][2][2], 0x083995EC, section .room_def_083995ec */
extern u8 gLevel6Stage2Room2MetatileMap[];
extern u8 gLevel6Stage2Room2BlockLayer[];
extern u8 gLevel6Stage2Room2BlockMetatiles[];
extern struct Door gLevel6Stage2Room2Doors[];
extern u8 gLevel6Stage2Room2Objects[];
struct RoomDef gLevel6Stage2Room2 ROOM_DEF(083995ec) = {
    .filler00 = { 6, 2, 2, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage2Room2MetatileMap,
    .blockLayer = gLevel6Stage2Room2BlockLayer,
    .blockMetatiles = gLevel6Stage2Room2BlockMetatiles,
    .width = 66,
    .height = 12,
    .bg2Palette = gUnk_084B2F9C,
    .bg2Tiles = gUnk_084B37B0,
    .metatileTiles = gUnk_084B3060,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 12,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage2Room2Doors,
    .objects = gLevel6Stage2Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 56,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][2][3], 0x08399CB8, section .room_def_08399cb8 */
extern u8 gLevel6Stage2Room3MetatileMap[];
extern u8 gLevel6Stage2Room3BlockLayer[];
extern u8 gLevel6Stage2Room3BlockMetatiles[];
extern struct Door gLevel6Stage2Room3Doors[];
extern u8 gLevel6Stage2Room3Objects[];
struct RoomDef gLevel6Stage2Room3 ROOM_DEF(08399cb8) = {
    .filler00 = { 6, 2, 3, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage2Room3MetatileMap,
    .blockLayer = gLevel6Stage2Room3BlockLayer,
    .blockMetatiles = gLevel6Stage2Room3BlockMetatiles,
    .width = 59,
    .height = 13,
    .bg2Palette = gUnk_084BB068,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 17,
    .objectsSortedByY = 0,
    .bgAnimSet = 1,
    .unk42 = 0,
    .doors = gLevel6Stage2Room3Doors,
    .objects = gLevel6Stage2Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 88,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][3][0], 0x0839A214, section .room_def_0839a214 */
extern u8 gLevel6Stage3Room0MetatileMap[];
extern u8 gLevel6Stage3Room0BlockLayer[];
extern u8 gLevel6Stage3Room0BlockMetatiles[];
extern struct Door gLevel6Stage3Room0Doors[];
extern u8 gLevel6Stage3Room0Objects[];
struct RoomDef gLevel6Stage3Room0 ROOM_DEF(0839a214) = {
    .filler00 = { 6, 3, 0, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage3Room0MetatileMap,
    .blockLayer = gLevel6Stage3Room0BlockLayer,
    .blockMetatiles = gLevel6Stage3Room0BlockMetatiles,
    .width = 64,
    .height = 11,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0848ADA0,
    .bg3Tiles = gUnk_0848B7C0,
    .bg3Map = &gUnk_0848AEA4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 16,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage3Room0Doors,
    .objects = gLevel6Stage3Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 56,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][3][1], 0x0839A4C8, section .room_def_0839a4c8 */
extern u8 gLevel6Stage3Room1MetatileMap[];
extern u8 gLevel6Stage3Room1BlockLayer[];
extern u8 gLevel6Stage3Room1BlockMetatiles[];
extern struct Door gLevel6Stage3Room1Doors[];
extern u8 gLevel6Stage3Room1Objects[];
struct RoomDef gLevel6Stage3Room1 ROOM_DEF(0839a4c8) = {
    .filler00 = { 6, 3, 1, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage3Room1MetatileMap,
    .blockLayer = gLevel6Stage3Room1BlockLayer,
    .blockMetatiles = gLevel6Stage3Room1BlockMetatiles,
    .width = 17,
    .height = 12,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0848ADA0,
    .bg3Tiles = gUnk_0848B7C0,
    .bg3Map = &gUnk_0848AEA4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 4,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage3Room1Doors,
    .objects = gLevel6Stage3Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 120,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][3][2], 0x0839A774, section .room_def_0839a774 */
extern u8 gLevel6Stage3Room2MetatileMap[];
extern u8 gLevel6Stage3Room2BlockLayer[];
extern u8 gLevel6Stage3Room2BlockMetatiles[];
extern struct Door gLevel6Stage3Room2Doors[];
extern u8 gLevel6Stage3Room2Objects[];
struct RoomDef gLevel6Stage3Room2 ROOM_DEF(0839a774) = {
    .filler00 = { 6, 3, 2, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage3Room2MetatileMap,
    .blockLayer = gLevel6Stage3Room2BlockLayer,
    .blockMetatiles = gLevel6Stage3Room2BlockMetatiles,
    .width = 17,
    .height = 12,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0848ADA0,
    .bg3Tiles = gUnk_0848B7C0,
    .bg3Map = &gUnk_0848AEA4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 4,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage3Room2Doors,
    .objects = gLevel6Stage3Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 120,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][3][3], 0x0839AA40, section .room_def_0839aa40 */
extern u8 gLevel6Stage3Room3MetatileMap[];
extern u8 gLevel6Stage3Room3BlockLayer[];
extern u8 gLevel6Stage3Room3BlockMetatiles[];
extern struct Door gLevel6Stage3Room3Doors[];
extern u8 gLevel6Stage3Room3Objects[];
struct RoomDef gLevel6Stage3Room3 ROOM_DEF(0839aa40) = {
    .filler00 = { 6, 3, 3, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage3Room3MetatileMap,
    .blockLayer = gLevel6Stage3Room3BlockLayer,
    .blockMetatiles = gLevel6Stage3Room3BlockMetatiles,
    .width = 17,
    .height = 12,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0848ADA0,
    .bg3Tiles = gUnk_0848B7C0,
    .bg3Map = &gUnk_0848AEA4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 4,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage3Room3Doors,
    .objects = gLevel6Stage3Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 232,
    .entryY = 72,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][3][4], 0x0839AD18, section .room_def_0839ad18 */
extern u8 gLevel6Stage3Room4MetatileMap[];
extern u8 gLevel6Stage3Room4BlockLayer[];
extern u8 gLevel6Stage3Room4BlockMetatiles[];
extern struct Door gLevel6Stage3Room4Doors[];
extern u8 gLevel6Stage3Room4Objects[];
struct RoomDef gLevel6Stage3Room4 ROOM_DEF(0839ad18) = {
    .filler00 = { 6, 3, 4, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage3Room4MetatileMap,
    .blockLayer = gLevel6Stage3Room4BlockLayer,
    .blockMetatiles = gLevel6Stage3Room4BlockMetatiles,
    .width = 17,
    .height = 12,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0848ADA0,
    .bg3Tiles = gUnk_0848B7C0,
    .bg3Map = &gUnk_0848AEA4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 4,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage3Room4Doors,
    .objects = gLevel6Stage3Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 72,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][3][5], 0x0839B260, section .room_def_0839b260 */
extern u8 gLevel6Stage3Room5MetatileMap[];
extern u8 gLevel6Stage3Room5BlockLayer[];
extern u8 gLevel6Stage3Room5BlockMetatiles[];
extern struct Door gLevel6Stage3Room5Doors[];
extern u8 gLevel6Stage3Room5Objects[];
struct RoomDef gLevel6Stage3Room5 ROOM_DEF(0839b260) = {
    .filler00 = { 6, 3, 5, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage3Room5MetatileMap,
    .blockLayer = gLevel6Stage3Room5BlockLayer,
    .blockMetatiles = gLevel6Stage3Room5BlockMetatiles,
    .width = 48,
    .height = 14,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0848ADA0,
    .bg3Tiles = gUnk_0848B7C0,
    .bg3Map = &gUnk_0848AEA4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 8,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage3Room5Doors,
    .objects = gLevel6Stage3Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 104,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][3][6], 0x0839BB28, section .room_def_0839bb28 */
extern u8 gLevel6Stage3Room6MetatileMap[];
extern u8 gLevel6Stage3Room6BlockLayer[];
extern u8 gLevel6Stage3Room6BlockMetatiles[];
extern struct Door gLevel6Stage3Room6Doors[];
extern u8 gLevel6Stage3Room6Objects[];
struct RoomDef gLevel6Stage3Room6 ROOM_DEF(0839bb28) = {
    .filler00 = { 6, 3, 6, 0 },
    .bgm = 17,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage3Room6MetatileMap,
    .blockLayer = gLevel6Stage3Room6BlockLayer,
    .blockMetatiles = gLevel6Stage3Room6BlockMetatiles,
    .width = 25,
    .height = 35,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 24,
    .borderY = 24,
    .bg3Palette = gUnk_0848ADA0,
    .bg3Tiles = gUnk_0848B7C0,
    .bg3Map = &gUnk_0848AEA4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 11,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage3Room6Doors,
    .objects = gLevel6Stage3Room6Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 504,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][4][0], 0x0839BED4, section .room_def_0839bed4 */
extern u8 gLevel6Stage4Room0MetatileMap[];
extern u8 gLevel6Stage4Room0BlockLayer[];
extern u8 gLevel6Stage4Room0BlockMetatiles[];
extern struct Door gLevel6Stage4Room0Doors[];
extern u8 gLevel6Stage4Room0Objects[];
struct RoomDef gLevel6Stage4Room0 ROOM_DEF(0839bed4) = {
    .filler00 = { 6, 4, 0, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage4Room0MetatileMap,
    .blockLayer = gLevel6Stage4Room0BlockLayer,
    .blockMetatiles = gLevel6Stage4Room0BlockMetatiles,
    .width = 48,
    .height = 11,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 7,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage4Room0Doors,
    .objects = gLevel6Stage4Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 88,
    .entryY = 72,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][4][1], 0x0839C0D0, section .room_def_0839c0d0 */
extern u8 gLevel6Stage4Room1MetatileMap[];
extern u8 gLevel6Stage4Room1BlockLayer[];
extern u8 gLevel6Stage4Room1BlockMetatiles[];
extern struct Door gLevel6Stage4Room1Doors[];
extern u8 gLevel6Stage4Room1Objects[];
struct RoomDef gLevel6Stage4Room1 ROOM_DEF(0839c0d0) = {
    .filler00 = { 6, 4, 1, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage4Room1MetatileMap,
    .blockLayer = gLevel6Stage4Room1BlockLayer,
    .blockMetatiles = gLevel6Stage4Room1BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage4Room1Doors,
    .objects = gLevel6Stage4Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 88,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][4][2], 0x0839C57C, section .room_def_0839c57c */
extern u8 gLevel6Stage4Room2MetatileMap[];
extern u8 gLevel6Stage4Room2BlockLayer[];
extern u8 gLevel6Stage4Room2BlockMetatiles[];
extern struct Door gLevel6Stage4Room2Doors[];
extern u8 gLevel6Stage4Room2Objects[];
struct RoomDef gLevel6Stage4Room2 ROOM_DEF(0839c57c) = {
    .filler00 = { 6, 4, 2, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage4Room2MetatileMap,
    .blockLayer = gLevel6Stage4Room2BlockLayer,
    .blockMetatiles = gLevel6Stage4Room2BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 13,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage4Room2Doors,
    .objects = gLevel6Stage4Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][4][3], 0x0839CB34, section .room_def_0839cb34 */
extern u8 gLevel6Stage4Room3MetatileMap[];
extern u8 gLevel6Stage4Room3BlockLayer[];
extern u8 gLevel6Stage4Room3BlockMetatiles[];
extern struct Door gLevel6Stage4Room3Doors[];
extern u8 gLevel6Stage4Room3Objects[];
struct RoomDef gLevel6Stage4Room3 ROOM_DEF(0839cb34) = {
    .filler00 = { 6, 4, 3, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage4Room3MetatileMap,
    .blockLayer = gLevel6Stage4Room3BlockLayer,
    .blockMetatiles = gLevel6Stage4Room3BlockMetatiles,
    .width = 48,
    .height = 11,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 9,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage4Room3Doors,
    .objects = gLevel6Stage4Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 88,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][4][4], 0x0839CD48, section .room_def_0839cd48 */
extern u8 gLevel6Stage4Room4MetatileMap[];
extern u8 gLevel6Stage4Room4BlockLayer[];
extern u8 gLevel6Stage4Room4BlockMetatiles[];
extern struct Door gLevel6Stage4Room4Doors[];
extern u8 gLevel6Stage4Room4Objects[];
struct RoomDef gLevel6Stage4Room4 ROOM_DEF(0839cd48) = {
    .filler00 = { 6, 4, 4, 0 },
    .bgm = 18,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage4Room4MetatileMap,
    .blockLayer = gLevel6Stage4Room4BlockLayer,
    .blockMetatiles = gLevel6Stage4Room4BlockMetatiles,
    .width = 17,
    .height = 11,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 16,
    .borderY = 8,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 5,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage4Room4Doors,
    .objects = gLevel6Stage4Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 8,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][5][0], 0x0839D254, section .room_def_0839d254 */
extern u8 gLevel6Stage5Room0MetatileMap[];
extern u8 gLevel6Stage5Room0BlockLayer[];
extern u8 gLevel6Stage5Room0BlockMetatiles[];
extern struct Door gLevel6Stage5Room0Doors[];
extern u8 gLevel6Stage5Room0Objects[];
struct RoomDef gLevel6Stage5Room0 ROOM_DEF(0839d254) = {
    .filler00 = { 6, 5, 0, 0 },
    .bgm = 20,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage5Room0MetatileMap,
    .blockLayer = gLevel6Stage5Room0BlockLayer,
    .blockMetatiles = gLevel6Stage5Room0BlockMetatiles,
    .width = 80,
    .height = 13,
    .bg2Palette = gUnk_0853E7EC,
    .bg2Tiles = gUnk_08489D58,
    .metatileTiles = gUnk_083B4D78,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 15,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage5Room0Doors,
    .objects = gLevel6Stage5Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 72,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][5][1], 0x0839D5B4, section .room_def_0839d5b4 */
extern u8 gLevel6Stage5Room1MetatileMap[];
extern u8 gLevel6Stage5Room1BlockLayer[];
extern u8 gLevel6Stage5Room1BlockMetatiles[];
extern struct Door gLevel6Stage5Room1Doors[];
extern u8 gLevel6Stage5Room1Objects[];
struct RoomDef gLevel6Stage5Room1 ROOM_DEF(0839d5b4) = {
    .filler00 = { 6, 5, 1, 0 },
    .bgm = 20,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage5Room1MetatileMap,
    .blockLayer = gLevel6Stage5Room1BlockLayer,
    .blockMetatiles = gLevel6Stage5Room1BlockMetatiles,
    .width = 50,
    .height = 11,
    .bg2Palette = gUnk_0853E7EC,
    .bg2Tiles = gUnk_08489D58,
    .metatileTiles = gUnk_083B4D78,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage5Room1Doors,
    .objects = gLevel6Stage5Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][5][2], 0x0839D8BC, section .room_def_0839d8bc */
extern u8 gLevel6Stage5Room2MetatileMap[];
extern u8 gLevel6Stage5Room2BlockLayer[];
extern u8 gLevel6Stage5Room2BlockMetatiles[];
extern struct Door gLevel6Stage5Room2Doors[];
extern u8 gLevel6Stage5Room2Objects[];
struct RoomDef gLevel6Stage5Room2 ROOM_DEF(0839d8bc) = {
    .filler00 = { 6, 5, 2, 0 },
    .bgm = 20,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage5Room2MetatileMap,
    .blockLayer = gLevel6Stage5Room2BlockLayer,
    .blockMetatiles = gLevel6Stage5Room2BlockMetatiles,
    .width = 46,
    .height = 11,
    .bg2Palette = gUnk_0853E7EC,
    .bg2Tiles = gUnk_08489D58,
    .metatileTiles = gUnk_083B4D78,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 8,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage5Room2Doors,
    .objects = gLevel6Stage5Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][5][3], 0x0839DAFC, section .room_def_0839dafc */
extern u8 gLevel6Stage5Room3MetatileMap[];
extern u8 gLevel6Stage5Room3BlockLayer[];
extern u8 gLevel6Stage5Room3BlockMetatiles[];
extern struct Door gLevel6Stage5Room3Doors[];
extern u8 gLevel6Stage5Room3Objects[];
struct RoomDef gLevel6Stage5Room3 ROOM_DEF(0839dafc) = {
    .filler00 = { 6, 5, 3, 0 },
    .bgm = 20,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage5Room3MetatileMap,
    .blockLayer = gLevel6Stage5Room3BlockLayer,
    .blockMetatiles = gLevel6Stage5Room3BlockMetatiles,
    .width = 32,
    .height = 11,
    .bg2Palette = gUnk_0853E7EC,
    .bg2Tiles = gUnk_08489D58,
    .metatileTiles = gUnk_083B4D78,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 6,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage5Room3Doors,
    .objects = gLevel6Stage5Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][5][4], 0x0839DED4, section .room_def_0839ded4 */
extern u8 gLevel6Stage5Room4MetatileMap[];
extern u8 gLevel6Stage5Room4BlockLayer[];
extern u8 gLevel6Stage5Room4BlockMetatiles[];
extern struct Door gLevel6Stage5Room4Doors[];
extern u8 gLevel6Stage5Room4Objects[];
struct RoomDef gLevel6Stage5Room4 ROOM_DEF(0839ded4) = {
    .filler00 = { 6, 5, 4, 0 },
    .bgm = 20,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage5Room4MetatileMap,
    .blockLayer = gLevel6Stage5Room4BlockLayer,
    .blockMetatiles = gLevel6Stage5Room4BlockMetatiles,
    .width = 64,
    .height = 13,
    .bg2Palette = gUnk_0853E7EC,
    .bg2Tiles = gUnk_08489D58,
    .metatileTiles = gUnk_083B4D78,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 12,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage5Room4Doors,
    .objects = gLevel6Stage5Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 56,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][5][5], 0x0839E190, section .room_def_0839e190 */
extern u8 gLevel6Stage5Room5MetatileMap[];
extern u8 gLevel6Stage5Room5BlockLayer[];
extern u8 gLevel6Stage5Room5BlockMetatiles[];
extern struct Door gLevel6Stage5Room5Doors[];
extern u8 gLevel6Stage5Room5Objects[];
struct RoomDef gLevel6Stage5Room5 ROOM_DEF(0839e190) = {
    .filler00 = { 6, 5, 5, 0 },
    .bgm = 20,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage5Room5MetatileMap,
    .blockLayer = gLevel6Stage5Room5BlockLayer,
    .blockMetatiles = gLevel6Stage5Room5BlockMetatiles,
    .width = 33,
    .height = 12,
    .bg2Palette = gUnk_0853E7EC,
    .bg2Tiles = gUnk_08489D58,
    .metatileTiles = gUnk_083B4D78,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 9,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage5Room5Doors,
    .objects = gLevel6Stage5Room5Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 72,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][5][6], 0x0839E590, section .room_def_0839e590 */
extern u8 gLevel6Stage5Room6MetatileMap[];
extern u8 gLevel6Stage5Room6BlockLayer[];
extern u8 gLevel6Stage5Room6BlockMetatiles[];
extern struct Door gLevel6Stage5Room6Doors[];
extern u8 gLevel6Stage5Room6Objects[];
struct RoomDef gLevel6Stage5Room6 ROOM_DEF(0839e590) = {
    .filler00 = { 6, 5, 6, 0 },
    .bgm = 20,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage5Room6MetatileMap,
    .blockLayer = gLevel6Stage5Room6BlockLayer,
    .blockMetatiles = gLevel6Stage5Room6BlockMetatiles,
    .width = 48,
    .height = 13,
    .bg2Palette = gUnk_0853E7EC,
    .bg2Tiles = gUnk_08489D58,
    .metatileTiles = gUnk_083B4D78,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 10,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage5Room6Doors,
    .objects = gLevel6Stage5Room6Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 120,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][5][7], 0x0839E944, section .room_def_0839e944 */
extern u8 gLevel6Stage5Room7MetatileMap[];
extern u8 gLevel6Stage5Room7BlockLayer[];
extern u8 gLevel6Stage5Room7BlockMetatiles[];
extern struct Door gLevel6Stage5Room7Doors[];
extern u8 gLevel6Stage5Room7Objects[];
struct RoomDef gLevel6Stage5Room7 ROOM_DEF(0839e944) = {
    .filler00 = { 6, 5, 7, 0 },
    .bgm = 20,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage5Room7MetatileMap,
    .blockLayer = gLevel6Stage5Room7BlockLayer,
    .blockMetatiles = gLevel6Stage5Room7BlockMetatiles,
    .width = 16,
    .height = 38,
    .bg2Palette = gUnk_0853E7EC,
    .bg2Tiles = gUnk_08489D58,
    .metatileTiles = gUnk_083B4D78,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 8,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage5Room7Doors,
    .objects = gLevel6Stage5Room7Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 408,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][5][8], 0x0839EBB4, section .room_def_0839ebb4 */
extern u8 gLevel6Stage5Room8MetatileMap[];
extern u8 gLevel6Stage5Room8BlockLayer[];
extern u8 gLevel6Stage5Room8BlockMetatiles[];
extern struct Door gLevel6Stage5Room8Doors[];
extern u8 gLevel6Stage5Room8Objects[];
struct RoomDef gLevel6Stage5Room8 ROOM_DEF(0839ebb4) = {
    .filler00 = { 6, 5, 8, 0 },
    .bgm = 20,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage5Room8MetatileMap,
    .blockLayer = gLevel6Stage5Room8BlockLayer,
    .blockMetatiles = gLevel6Stage5Room8BlockMetatiles,
    .width = 16,
    .height = 25,
    .bg2Palette = gUnk_0853E7EC,
    .bg2Tiles = gUnk_08489D58,
    .metatileTiles = gUnk_083B4D78,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 3,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel6Stage5Room8Doors,
    .objects = gLevel6Stage5Room8Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 136,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[6][6][0], 0x0839EE14, section .room_def_0839ee14 */
extern u8 gLevel6Stage6Room0MetatileMap[];
extern u8 gLevel6Stage6Room0BlockLayer[];
extern u8 gLevel6Stage6Room0BlockMetatiles[];
extern u8 gLevel6Stage6Room0Objects[];
struct RoomDef gLevel6Stage6Room0 ROOM_DEF(0839ee14) = {
    .filler00 = { 6, 6, 0, 0 },
    .bgm = 27,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel6Stage6Room0MetatileMap,
    .blockLayer = gLevel6Stage6Room0BlockLayer,
    .blockMetatiles = gLevel6Stage6Room0BlockMetatiles,
    .width = 18,
    .height = 21,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 24,
    .borderY = 8,
    .bg3Palette = gUnk_0835D710,
    .bg3Tiles = gUnk_08412BD8,
    .bg3Map = &gUnk_083C89E8,
    .bg3BorderX = 8,
    .bg3BorderY = 16,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = gLevel6Stage6Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 24,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 5,
};

/* gRoomTable[7][0][0], 0x0839F3D8, section .room_def_0839f3d8 */
extern u8 gLevel7Stage0Room0MetatileMap[];
extern u8 gLevel7Stage0Room0BlockLayer[];
extern u8 gLevel7Stage0Room0BlockMetatiles[];
extern u8 gLevel7Stage0Room0Objects[];
struct RoomDef gLevel7Stage0Room0 ROOM_DEF(0839f3d8) = {
    .filler00 = { 7, 0, 0, 0 },
    .bgm = -1,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage0Room0MetatileMap,
    .blockLayer = gLevel7Stage0Room0BlockLayer,
    .blockMetatiles = gLevel7Stage0Room0BlockMetatiles,
    .width = 16,
    .height = 68,
    .bg2Palette = gUnk_084BB808,
    .bg2Tiles = gUnk_084CB360,
    .metatileTiles = gUnk_084BB8CC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084BB600,
    .bg3Tiles = gUnk_084BFECC,
    .bg3Map = &gLevel7Stage0Room0Bg3Map,
    .bg3BorderX = 0,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = gLevel7Stage0Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 24,
    .entryY = 852,
    .unk54 = 4,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 5,
};

/* gRoomTable[7][0][1], 0x0839F684, section .room_def_0839f684 */
extern u8 gLevel7Stage0Room1MetatileMap[];
extern u8 gLevel7Stage0Room1BlockLayer[];
extern u8 gLevel7Stage0Room1BlockMetatiles[];
extern struct Door gLevel7Stage0Room1Doors[];
extern u8 gLevel7Stage0Room1Objects[];
struct RoomDef gLevel7Stage0Room1 ROOM_DEF(0839f684) = {
    .filler00 = { 7, 0, 1, 0 },
    .bgm = -1,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage0Room1MetatileMap,
    .blockLayer = gLevel7Stage0Room1BlockLayer,
    .blockMetatiles = gLevel7Stage0Room1BlockMetatiles,
    .width = 32,
    .height = 24,
    .bg2Palette = gUnk_084BB5C0,
    .bg2Tiles = gUnk_084BB5F4,
    .metatileTiles = gUnk_084BB5E4,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gLevel7Stage0Room1Bg3Palette,
    .bg3Tiles = gLevel7Stage0Room1Bg3Tiles,
    .bg3Map = &gLevel7Stage0Room1Bg3Map,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel7Stage0Room1Doors,
    .objects = gLevel7Stage0Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 25,
    .entryY = 296,
    .unk54 = 5,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 5,
};

/* gRoomTable[7][0][2], 0x0839FC40, section .room_def_0839fc40 */
extern u8 gLevel7Stage0Room2MetatileMap[];
extern u8 gLevel7Stage0Room2BlockLayer[];
extern u8 gLevel7Stage0Room2BlockMetatiles[];
struct RoomDef gLevel7Stage0Room2 ROOM_DEF(0839fc40) = {
    .filler00 = { 7, 0, 2, 0 },
    .bgm = -1,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage0Room2MetatileMap,
    .blockLayer = gLevel7Stage0Room2BlockLayer,
    .blockMetatiles = gLevel7Stage0Room2BlockMetatiles,
    .width = 16,
    .height = 68,
    .bg2Palette = gUnk_084BB808,
    .bg2Tiles = gUnk_084CB360,
    .metatileTiles = gUnk_084BB8CC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_084BB600,
    .bg3Tiles = gUnk_084BFECC,
    .bg3Map = &gLevel7Stage0Room2Bg3Map,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 4,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[7][2][0], 0x083A0058, section .room_def_083a0058 */
extern u8 gLevel7Stage2Room0MetatileMap[];
extern u8 gLevel7Stage2Room0BlockLayer[];
extern u8 gLevel7Stage2Room0BlockMetatiles[];
extern struct Door gLevel7Stage2Room0Doors[];
extern u8 gLevel7Stage2Room0Objects[];
struct RoomDef gLevel7Stage2Room0 ROOM_DEF(083a0058) = {
    .filler00 = { 7, 2, 0, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage2Room0MetatileMap,
    .blockLayer = gLevel7Stage2Room0BlockLayer,
    .blockMetatiles = gLevel7Stage2Room0BlockMetatiles,
    .width = 80,
    .height = 12,
    .bg2Palette = gUnk_0853C698,
    .bg2Tiles = gUnk_083D1380,
    .metatileTiles = gUnk_083A862C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E624,
    .bg3Tiles = gUnk_08479B7C,
    .bg3Map = &gUnk_083D0148,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 6,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel7Stage2Room0Doors,
    .objects = gLevel7Stage2Room0Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[7][2][1], 0x083A0370, section .room_def_083a0370 */
extern u8 gLevel7Stage2Room1MetatileMap[];
extern u8 gLevel7Stage2Room1BlockLayer[];
extern u8 gLevel7Stage2Room1BlockMetatiles[];
extern struct Door gLevel7Stage2Room1Doors[];
extern u8 gLevel7Stage2Room1Objects[];
struct RoomDef gLevel7Stage2Room1 ROOM_DEF(083a0370) = {
    .filler00 = { 7, 2, 1, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage2Room1MetatileMap,
    .blockLayer = gLevel7Stage2Room1BlockLayer,
    .blockMetatiles = gLevel7Stage2Room1BlockMetatiles,
    .width = 64,
    .height = 12,
    .bg2Palette = gUnk_0853C698,
    .bg2Tiles = gUnk_083D1380,
    .metatileTiles = gUnk_083A862C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DD00,
    .bg3Tiles = gUnk_0844619C,
    .bg3Map = &gUnk_083CAF40,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel7Stage2Room1Doors,
    .objects = gLevel7Stage2Room1Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[7][2][2], 0x083A06DC, section .room_def_083a06dc */
extern u8 gLevel7Stage2Room2MetatileMap[];
extern u8 gLevel7Stage2Room2BlockLayer[];
extern u8 gLevel7Stage2Room2BlockMetatiles[];
extern struct Door gLevel7Stage2Room2Doors[];
extern u8 gLevel7Stage2Room2Objects[];
struct RoomDef gLevel7Stage2Room2 ROOM_DEF(083a06dc) = {
    .filler00 = { 7, 2, 2, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage2Room2MetatileMap,
    .blockLayer = gLevel7Stage2Room2BlockLayer,
    .blockMetatiles = gLevel7Stage2Room2BlockMetatiles,
    .width = 64,
    .height = 12,
    .bg2Palette = gUnk_0853C698,
    .bg2Tiles = gUnk_083D1380,
    .metatileTiles = gUnk_083A862C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DD00,
    .bg3Tiles = gUnk_0844619C,
    .bg3Map = &gUnk_083CAF40,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel7Stage2Room2Doors,
    .objects = gLevel7Stage2Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[7][2][3], 0x083A0AA0, section .room_def_083a0aa0 */
extern u8 gLevel7Stage2Room3MetatileMap[];
extern u8 gLevel7Stage2Room3BlockLayer[];
extern u8 gLevel7Stage2Room3BlockMetatiles[];
extern struct Door gLevel7Stage2Room3Doors[];
extern u8 gLevel7Stage2Room3Objects[];
struct RoomDef gLevel7Stage2Room3 ROOM_DEF(083a0aa0) = {
    .filler00 = { 7, 2, 3, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage2Room3MetatileMap,
    .blockLayer = gLevel7Stage2Room3BlockLayer,
    .blockMetatiles = gLevel7Stage2Room3BlockMetatiles,
    .width = 32,
    .height = 12,
    .bg2Palette = gUnk_0853C698,
    .bg2Tiles = gUnk_083D1380,
    .metatileTiles = gUnk_083A862C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DD00,
    .bg3Tiles = gUnk_0844619C,
    .bg3Map = &gUnk_083CAF40,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel7Stage2Room3Doors,
    .objects = gLevel7Stage2Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[7][2][4], 0x083A0D2C, section .room_def_083a0d2c */
extern u8 gLevel7Stage2Room4MetatileMap[];
extern u8 gLevel7Stage2Room4BlockLayer[];
extern u8 gLevel7Stage2Room4BlockMetatiles[];
extern struct Door gLevel7Stage2Room4Doors[];
extern u8 gLevel7Stage2Room4Objects[];
struct RoomDef gLevel7Stage2Room4 ROOM_DEF(083a0d2c) = {
    .filler00 = { 7, 2, 4, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage2Room4MetatileMap,
    .blockLayer = gLevel7Stage2Room4BlockLayer,
    .blockMetatiles = gLevel7Stage2Room4BlockMetatiles,
    .width = 48,
    .height = 11,
    .bg2Palette = gUnk_0853C698,
    .bg2Tiles = gUnk_083D1380,
    .metatileTiles = gUnk_083A862C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DD00,
    .bg3Tiles = gUnk_0844619C,
    .bg3Map = &gUnk_083CAF40,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0,
    .doorCount = 2,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel7Stage2Room4Doors,
    .objects = gLevel7Stage2Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 40,
    .entryY = 40,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[7][2][5], 0x083A0F00, section .room_def_083a0f00 */
extern u8 gLevel7Stage2Room5MetatileMap[];
extern u8 gLevel7Stage2Room5BlockLayer[];
extern u8 gLevel7Stage2Room5BlockMetatiles[];
extern struct Door gLevel7Stage2Room5Doors[];
struct RoomDef gLevel7Stage2Room5 ROOM_DEF(083a0f00) = {
    .filler00 = { 7, 2, 5, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage2Room5MetatileMap,
    .blockLayer = gLevel7Stage2Room5BlockLayer,
    .blockMetatiles = gLevel7Stage2Room5BlockMetatiles,
    .width = 32,
    .height = 11,
    .bg2Palette = gUnk_0853C698,
    .bg2Tiles = gUnk_083D1380,
    .metatileTiles = gUnk_083A862C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DD00,
    .bg3Tiles = gUnk_0844619C,
    .bg3Map = &gUnk_083CAF40,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel7Stage2Room5Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[7][2][6], 0x083A142C, section .room_def_083a142c */
extern u8 gLevel7Stage2Room6MetatileMap[];
extern u8 gLevel7Stage2Room6BlockLayer[];
extern u8 gLevel7Stage2Room6BlockMetatiles[];
extern struct Door gLevel7Stage2Room6Doors[];
extern u8 gLevel7Stage2Room6Objects[];
struct RoomDef gLevel7Stage2Room6 ROOM_DEF(083a142c) = {
    .filler00 = { 7, 2, 6, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage2Room6MetatileMap,
    .blockLayer = gLevel7Stage2Room6BlockLayer,
    .blockMetatiles = gLevel7Stage2Room6BlockMetatiles,
    .width = 22,
    .height = 29,
    .bg2Palette = gUnk_0853C698,
    .bg2Tiles = gUnk_083D1380,
    .metatileTiles = gUnk_083A862C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0852BDC4,
    .bg3Tiles = gUnk_0846697C,
    .bg3Map = &gUnk_083CE5F4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 8,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel7Stage2Room6Doors,
    .objects = gLevel7Stage2Room6Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[7][2][7], 0x083A1644, section .room_def_083a1644 */
extern u8 gLevel7Stage2Room7MetatileMap[];
extern u8 gLevel7Stage2Room7BlockLayer[];
extern u8 gLevel7Stage2Room7BlockMetatiles[];
extern struct Door gLevel7Stage2Room7Doors[];
struct RoomDef gLevel7Stage2Room7 ROOM_DEF(083a1644) = {
    .filler00 = { 7, 2, 7, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage2Room7MetatileMap,
    .blockLayer = gLevel7Stage2Room7BlockLayer,
    .blockMetatiles = gLevel7Stage2Room7BlockMetatiles,
    .width = 41,
    .height = 13,
    .bg2Palette = gUnk_0853C698,
    .bg2Tiles = gUnk_083D1380,
    .metatileTiles = gUnk_083A862C,
    .borderX = 8,
    .borderY = 24,
    .bg3Palette = gUnk_0852BDC4,
    .bg3Tiles = gUnk_0846697C,
    .bg3Map = &gUnk_083CE5F4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel7Stage2Room7Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[7][2][8], 0x083A17CC, section .room_def_083a17cc */
extern u8 gLevel7Stage2Room8MetatileMap[];
extern u8 gLevel7Stage2Room8BlockLayer[];
extern u8 gLevel7Stage2Room8BlockMetatiles[];
extern struct Door gLevel7Stage2Room8Doors[];
extern u8 gLevel7Stage2Room8Objects[];
struct RoomDef gLevel7Stage2Room8 ROOM_DEF(083a17cc) = {
    .filler00 = { 7, 2, 8, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage2Room8MetatileMap,
    .blockLayer = gLevel7Stage2Room8BlockLayer,
    .blockMetatiles = gLevel7Stage2Room8BlockMetatiles,
    .width = 31,
    .height = 11,
    .bg2Palette = gUnk_0853C698,
    .bg2Tiles = gUnk_083D1380,
    .metatileTiles = gUnk_083A862C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0852BDC4,
    .bg3Tiles = gUnk_0846697C,
    .bg3Map = &gUnk_083CE5F4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel7Stage2Room8Doors,
    .objects = gLevel7Stage2Room8Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[7][2][9], 0x083A1B64, section .room_def_083a1b64 */
extern u8 gLevel7Stage2Room9MetatileMap[];
extern u8 gLevel7Stage2Room9BlockLayer[];
extern u8 gLevel7Stage2Room9BlockMetatiles[];
struct RoomDef gLevel7Stage2Room9 ROOM_DEF(083a1b64) = {
    .filler00 = { 7, 2, 9, 0 },
    .bgm = -1,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage2Room9MetatileMap,
    .blockLayer = gLevel7Stage2Room9BlockLayer,
    .blockMetatiles = gLevel7Stage2Room9BlockMetatiles,
    .width = 50,
    .height = 16,
    .bg2Palette = gUnk_0853C698,
    .bg2Tiles = gUnk_083D1380,
    .metatileTiles = gUnk_083A862C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0852BDC4,
    .bg3Tiles = gUnk_0846697C,
    .bg3Map = &gUnk_083CE5F4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[7][2][10], 0x083A1FD8, section .room_def_083a1fd8 */
extern u8 gLevel7Stage2Room10MetatileMap[];
extern u8 gLevel7Stage2Room10BlockLayer[];
extern u8 gLevel7Stage2Room10BlockMetatiles[];
struct RoomDef gLevel7Stage2Room10 ROOM_DEF(083a1fd8) = {
    .filler00 = { 7, 2, 10, 0 },
    .bgm = -1,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel7Stage2Room10MetatileMap,
    .blockLayer = gLevel7Stage2Room10BlockLayer,
    .blockMetatiles = gLevel7Stage2Room10BlockMetatiles,
    .width = 100,
    .height = 13,
    .bg2Palette = gUnk_0853C698,
    .bg2Tiles = gUnk_083D1380,
    .metatileTiles = gUnk_083A862C,
    .borderX = 8,
    .borderY = 20,
    .bg3Palette = gUnk_0852BDC4,
    .bg3Tiles = gUnk_0846697C,
    .bg3Map = &gUnk_083CE5F4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 0,
};

/* gRoomTable[8][0][0], 0x083A24F0, section .room_def_083a24f0 */
extern u8 gLevel8Stage0Room0MetatileMap[];
extern u8 gLevel8Stage0Room0BlockLayer[];
extern u8 gLevel8Stage0Room0BlockMetatiles[];
extern struct Door gLevel8Stage0Room0Doors[];
struct RoomDef gLevel8Stage0Room0 ROOM_DEF(083a24f0) = {
    .filler00 = { 8, 0, 0, 0 },
    .bgm = 4,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage0Room0MetatileMap,
    .blockLayer = gLevel8Stage0Room0BlockLayer,
    .blockMetatiles = gLevel8Stage0Room0BlockMetatiles,
    .width = 32,
    .height = 22,
    .bg2Palette = gUnk_0853D600,
    .bg2Tiles = gUnk_08430A34,
    .metatileTiles = gUnk_083AD728,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D0E4,
    .bg3Tiles = gUnk_0840F3A8,
    .bg3Map = &gUnk_083BC030,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 11,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage0Room0Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 1,
};

/* gRoomTable[8][0][1], 0x083A2578, section .room_def_083a2578 */
extern u8 gLevel8Stage0Room1MetatileMap[];
extern u8 gLevel8Stage0Room1BlockLayer[];
extern u8 gLevel8Stage0Room1BlockMetatiles[];
struct RoomDef gLevel8Stage0Room1 ROOM_DEF(083a2578) = {
    .filler00 = { 8, 0, 1, 0 },
    .bgm = 4,
    .mapsCompressed = 0,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage0Room1MetatileMap,
    .blockLayer = gLevel8Stage0Room1BlockLayer,
    .blockMetatiles = gLevel8Stage0Room1BlockMetatiles,
    .width = 2,
    .height = 2,
    .bg2Palette = gUnk_0853D600,
    .bg2Tiles = gUnk_08430A34,
    .metatileTiles = gUnk_083AD728,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D0E4,
    .bg3Tiles = gUnk_0840F3A8,
    .bg3Map = &gUnk_083BC030,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 8,
};

/* gRoomTable[8][0][2], 0x083A2694, section .room_def_083a2694 */
extern u8 gLevel8Stage0Room2MetatileMap[];
extern u8 gLevel8Stage0Room2BlockLayer[];
extern u8 gLevel8Stage0Room2BlockMetatiles[];
extern struct Door gLevel8Stage0Room2Doors[];
extern u8 gLevel8Stage0Room2Objects[];
struct RoomDef gLevel8Stage0Room2 ROOM_DEF(083a2694) = {
    .filler00 = { 8, 0, 2, 0 },
    .bgm = 25,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage0Room2MetatileMap,
    .blockLayer = gLevel8Stage0Room2BlockLayer,
    .blockMetatiles = gLevel8Stage0Room2BlockMetatiles,
    .width = 16,
    .height = 12,
    .bg2Palette = gUnk_084BB5C0,
    .bg2Tiles = gUnk_084BB5F4,
    .metatileTiles = gUnk_084BB5E4,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gLevel8Stage0Room2Bg3Palette,
    .bg3Tiles = gUnk_084D8E30,
    .bg3Map = &gLevel8Stage0Room2Bg3Map,
    .bg3BorderX = 16,
    .bg3BorderY = 16,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage0Room2Doors,
    .objects = gLevel8Stage0Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 3,
};

/* gRoomTable[8][0][3], 0x083A291C, section .room_def_083a291c */
extern u8 gLevel8Stage0Room3MetatileMap[];
extern u8 gLevel8Stage0Room3BlockLayer[];
extern u8 gLevel8Stage0Room3BlockMetatiles[];
extern struct Door gLevel8Stage0Room3Doors[];
extern u8 gLevel8Stage0Room3Objects[];
struct RoomDef gLevel8Stage0Room3 ROOM_DEF(083a291c) = {
    .filler00 = { 8, 0, 3, 0 },
    .bgm = 25,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage0Room3MetatileMap,
    .blockLayer = gLevel8Stage0Room3BlockLayer,
    .blockMetatiles = gLevel8Stage0Room3BlockMetatiles,
    .width = 16,
    .height = 36,
    .bg2Palette = gUnk_0849A940,
    .bg2Tiles = gUnk_08442F18,
    .metatileTiles = gUnk_083B2254,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gUnk_08505B20,
    .bg3Tiles = gUnk_08506F4C,
    .bg3Map = &gUnk_08505C24,
    .bg3BorderX = 16,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage0Room3Doors,
    .objects = gLevel8Stage0Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 128,
    .entryY = 536,
    .unk54 = 3,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 2,
};

/* gRoomTable[8][0][4], 0x083A2AC4, section .room_def_083a2ac4 */
extern u8 gLevel8Stage0Room4MetatileMap[];
extern u8 gLevel8Stage0Room4BlockLayer[];
extern u8 gLevel8Stage0Room4BlockMetatiles[];
struct RoomDef gLevel8Stage0Room4 ROOM_DEF(083a2ac4) = {
    .filler00 = { 8, 0, 4, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage0Room4MetatileMap,
    .blockLayer = gLevel8Stage0Room4BlockLayer,
    .blockMetatiles = gLevel8Stage0Room4BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853D12C,
    .bg2Tiles = gUnk_0840FCD8,
    .metatileTiles = gUnk_083AB444,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853DD00,
    .bg3Tiles = gUnk_0844619C,
    .bg3Map = &gUnk_083CAF40,
    .bg3BorderX = 0,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 6,
};

/* gRoomTable[8][1][0], 0x083A32BC, section .room_def_083a32bc */
extern u8 gLevel8Stage1Room0MetatileMap[];
extern u8 gLevel8Stage1Room0BlockLayer[];
extern u8 gLevel8Stage1Room0BlockMetatiles[];
extern struct Door gLevel8Stage1Room0Doors[];
struct RoomDef gLevel8Stage1Room0 ROOM_DEF(083a32bc) = {
    .filler00 = { 8, 1, 0, 0 },
    .bgm = 5,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage1Room0MetatileMap,
    .blockLayer = gLevel8Stage1Room0BlockLayer,
    .blockMetatiles = gLevel8Stage1Room0BlockMetatiles,
    .width = 48,
    .height = 23,
    .bg2Palette = gUnk_0853D6C4,
    .bg2Tiles = gUnk_08431740,
    .metatileTiles = gUnk_083ADC20,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D0E4,
    .bg3Tiles = gUnk_0840F3A8,
    .bg3Map = &gUnk_083BD638,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 15,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage1Room0Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 1,
};

/* gRoomTable[8][1][1], 0x083A3344, section .room_def_083a3344 */
extern u8 gLevel8Stage1Room1MetatileMap[];
extern u8 gLevel8Stage1Room1BlockLayer[];
extern u8 gLevel8Stage1Room1BlockMetatiles[];
struct RoomDef gLevel8Stage1Room1 ROOM_DEF(083a3344) = {
    .filler00 = { 8, 1, 1, 0 },
    .bgm = 5,
    .mapsCompressed = 0,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage1Room1MetatileMap,
    .blockLayer = gLevel8Stage1Room1BlockLayer,
    .blockMetatiles = gLevel8Stage1Room1BlockMetatiles,
    .width = 2,
    .height = 2,
    .bg2Palette = gUnk_0853D6C4,
    .bg2Tiles = gUnk_08431740,
    .metatileTiles = gUnk_083ADC20,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D0E4,
    .bg3Tiles = gUnk_0840F3A8,
    .bg3Map = &gUnk_083BD638,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 6,
};

/* gRoomTable[8][1][2], 0x083A34E4, section .room_def_083a34e4 */
extern u8 gLevel8Stage1Room2MetatileMap[];
extern u8 gLevel8Stage1Room2BlockLayer[];
extern u8 gLevel8Stage1Room2BlockMetatiles[];
extern struct Door gLevel8Stage1Room2Doors[];
extern u8 gLevel8Stage1Room2Objects[];
struct RoomDef gLevel8Stage1Room2 ROOM_DEF(083a34e4) = {
    .filler00 = { 8, 1, 2, 0 },
    .bgm = 39,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage1Room2MetatileMap,
    .blockLayer = gLevel8Stage1Room2BlockLayer,
    .blockMetatiles = gLevel8Stage1Room2BlockMetatiles,
    .width = 18,
    .height = 12,
    .bg2Palette = gUnk_084E56DC,
    .bg2Tiles = gUnk_084EB4F4,
    .metatileTiles = gUnk_084E57E0,
    .borderX = 24,
    .borderY = 16,
    .bg3Palette = gUnk_084E55D8,
    .bg3Tiles = gUnk_084E6184,
    .bg3Map = &gUnk_084E5928,
    .bg3BorderX = 16,
    .bg3BorderY = 16,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 7,
    .unk42 = 0,
    .doors = gLevel8Stage1Room2Doors,
    .objects = gLevel8Stage1Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 4,
};

/* gRoomTable[8][1][3], 0x083A360C, section .room_def_083a360c */
extern u8 gLevel8Stage1Room3MetatileMap[];
extern u8 gLevel8Stage1Room3BlockLayer[];
extern u8 gLevel8Stage1Room3BlockMetatiles[];
extern struct Door gLevel8Stage1Room3Doors[];
extern u8 gLevel8Stage1Room3Objects[];
struct RoomDef gLevel8Stage1Room3 ROOM_DEF(083a360c) = {
    .filler00 = { 8, 1, 3, 0 },
    .bgm = 25,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage1Room3MetatileMap,
    .blockLayer = gLevel8Stage1Room3BlockLayer,
    .blockMetatiles = gLevel8Stage1Room3BlockMetatiles,
    .width = 16,
    .height = 12,
    .bg2Palette = gUnk_084BB5C0,
    .bg2Tiles = gUnk_084BB5F4,
    .metatileTiles = gUnk_084BB5E4,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gLevel8Stage1Room3Bg3Palette,
    .bg3Tiles = gUnk_084DEE4C,
    .bg3Map = &gLevel8Stage1Room3Bg3Map,
    .bg3BorderX = 16,
    .bg3BorderY = 16,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage1Room3Doors,
    .objects = gLevel8Stage1Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 136,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 3,
};

/* gRoomTable[8][1][4], 0x083A38FC, section .room_def_083a38fc */
extern u8 gLevel8Stage1Room4MetatileMap[];
extern u8 gLevel8Stage1Room4BlockLayer[];
extern u8 gLevel8Stage1Room4BlockMetatiles[];
extern struct Door gLevel8Stage1Room4Doors[];
extern u8 gLevel8Stage1Room4Objects[];
struct RoomDef gLevel8Stage1Room4 ROOM_DEF(083a38fc) = {
    .filler00 = { 8, 1, 4, 0 },
    .bgm = 25,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage1Room4MetatileMap,
    .blockLayer = gLevel8Stage1Room4BlockLayer,
    .blockMetatiles = gLevel8Stage1Room4BlockMetatiles,
    .width = 16,
    .height = 36,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gUnk_08505B20,
    .bg3Tiles = gUnk_08506F4C,
    .bg3Map = &gUnk_08505C24,
    .bg3BorderX = 16,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 2,
    .objectCount = 6,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage1Room4Doors,
    .objects = gLevel8Stage1Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 128,
    .entryY = 536,
    .unk54 = 3,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 2,
};

/* gRoomTable[8][1][5], 0x083A3A74, section .room_def_083a3a74 */
extern u8 gLevel8Stage1Room5MetatileMap[];
extern u8 gLevel8Stage1Room5BlockLayer[];
extern u8 gLevel8Stage1Room5BlockMetatiles[];
struct RoomDef gLevel8Stage1Room5 ROOM_DEF(083a3a74) = {
    .filler00 = { 8, 1, 5, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage1Room5MetatileMap,
    .blockLayer = gLevel8Stage1Room5BlockLayer,
    .blockMetatiles = gLevel8Stage1Room5BlockMetatiles,
    .width = 16,
    .height = 12,
    .bg2Palette = gUnk_0853E318,
    .bg2Tiles = gUnk_0846CFD8,
    .metatileTiles = gUnk_083B282C,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gUnk_0853C9C8,
    .bg3Tiles = gUnk_083E6548,
    .bg3Map = &gUnk_083B708C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 3,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 6,
};

/* gRoomTable[8][2][0], 0x083A3FF4, section .room_def_083a3ff4 */
extern u8 gLevel8Stage2Room0MetatileMap[];
extern u8 gLevel8Stage2Room0BlockLayer[];
extern u8 gLevel8Stage2Room0BlockMetatiles[];
extern struct Door gLevel8Stage2Room0Doors[];
struct RoomDef gLevel8Stage2Room0 ROOM_DEF(083a3ff4) = {
    .filler00 = { 8, 2, 0, 0 },
    .bgm = 6,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage2Room0MetatileMap,
    .blockLayer = gLevel8Stage2Room0BlockLayer,
    .blockMetatiles = gLevel8Stage2Room0BlockMetatiles,
    .width = 16,
    .height = 47,
    .bg2Palette = gUnk_08539C88,
    .bg2Tiles = gUnk_08432FCC,
    .metatileTiles = gUnk_083AE5A4,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D0E4,
    .bg3Tiles = gUnk_0840F3A8,
    .bg3Map = &gUnk_083BF8C0,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 16,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage2Room0Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 1,
};

/* gRoomTable[8][2][1], 0x083A407C, section .room_def_083a407c */
extern u8 gLevel8Stage2Room1MetatileMap[];
extern u8 gLevel8Stage2Room1BlockLayer[];
extern u8 gLevel8Stage2Room1BlockMetatiles[];
struct RoomDef gLevel8Stage2Room1 ROOM_DEF(083a407c) = {
    .filler00 = { 8, 2, 1, 0 },
    .bgm = 6,
    .mapsCompressed = 0,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage2Room1MetatileMap,
    .blockLayer = gLevel8Stage2Room1BlockLayer,
    .blockMetatiles = gLevel8Stage2Room1BlockMetatiles,
    .width = 2,
    .height = 2,
    .bg2Palette = gUnk_08539C88,
    .bg2Tiles = gUnk_08432FCC,
    .metatileTiles = gUnk_083AE5A4,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D0E4,
    .bg3Tiles = gUnk_0840F3A8,
    .bg3Map = &gUnk_083BF8C0,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 8,
};

/* gRoomTable[8][2][2], 0x083A421C, section .room_def_083a421c */
extern u8 gLevel8Stage2Room2MetatileMap[];
extern u8 gLevel8Stage2Room2BlockLayer[];
extern u8 gLevel8Stage2Room2BlockMetatiles[];
extern struct Door gLevel8Stage2Room2Doors[];
extern u8 gLevel8Stage2Room2Objects[];
struct RoomDef gLevel8Stage2Room2 ROOM_DEF(083a421c) = {
    .filler00 = { 8, 2, 2, 0 },
    .bgm = 39,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage2Room2MetatileMap,
    .blockLayer = gLevel8Stage2Room2BlockLayer,
    .blockMetatiles = gLevel8Stage2Room2BlockMetatiles,
    .width = 18,
    .height = 12,
    .bg2Palette = gUnk_084E56DC,
    .bg2Tiles = gUnk_084EB4F4,
    .metatileTiles = gUnk_084E57E0,
    .borderX = 24,
    .borderY = 16,
    .bg3Palette = gUnk_084E55D8,
    .bg3Tiles = gUnk_084E6184,
    .bg3Map = &gUnk_084E5928,
    .bg3BorderX = 16,
    .bg3BorderY = 16,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 7,
    .unk42 = 0,
    .doors = gLevel8Stage2Room2Doors,
    .objects = gLevel8Stage2Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 4,
};

/* gRoomTable[8][2][3], 0x083A433C, section .room_def_083a433c */
extern u8 gLevel8Stage2Room3MetatileMap[];
extern u8 gLevel8Stage2Room3BlockLayer[];
extern u8 gLevel8Stage2Room3BlockMetatiles[];
extern struct Door gLevel8Stage2Room3Doors[];
extern u8 gLevel8Stage2Room3Objects[];
struct RoomDef gLevel8Stage2Room3 ROOM_DEF(083a433c) = {
    .filler00 = { 8, 2, 3, 0 },
    .bgm = 25,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage2Room3MetatileMap,
    .blockLayer = gLevel8Stage2Room3BlockLayer,
    .blockMetatiles = gLevel8Stage2Room3BlockMetatiles,
    .width = 16,
    .height = 12,
    .bg2Palette = gUnk_084BB5C0,
    .bg2Tiles = gUnk_084BB5F4,
    .metatileTiles = gUnk_084BB5E4,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gLevel8Stage2Room3Bg3Palette,
    .bg3Tiles = gUnk_084D8E30,
    .bg3Map = &gLevel8Stage2Room3Bg3Map,
    .bg3BorderX = 16,
    .bg3BorderY = 16,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage2Room3Doors,
    .objects = gLevel8Stage2Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 3,
};

/* gRoomTable[8][2][4], 0x083A45DC, section .room_def_083a45dc */
extern u8 gLevel8Stage2Room4MetatileMap[];
extern u8 gLevel8Stage2Room4BlockLayer[];
extern u8 gLevel8Stage2Room4BlockMetatiles[];
extern struct Door gLevel8Stage2Room4Doors[];
extern u8 gLevel8Stage2Room4Objects[];
struct RoomDef gLevel8Stage2Room4 ROOM_DEF(083a45dc) = {
    .filler00 = { 8, 2, 4, 0 },
    .bgm = 25,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage2Room4MetatileMap,
    .blockLayer = gLevel8Stage2Room4BlockLayer,
    .blockMetatiles = gLevel8Stage2Room4BlockMetatiles,
    .width = 16,
    .height = 36,
    .bg2Palette = gUnk_0853DBB8,
    .bg2Tiles = gUnk_0843FC10,
    .metatileTiles = gUnk_083B18FC,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gUnk_08505B20,
    .bg3Tiles = gUnk_08506F4C,
    .bg3Map = &gUnk_08505C24,
    .bg3BorderX = 16,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage2Room4Doors,
    .objects = gLevel8Stage2Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 128,
    .entryY = 536,
    .unk54 = 3,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 2,
};

/* gRoomTable[8][2][5], 0x083A46FC, section .room_def_083a46fc */
extern u8 gLevel8Stage2Room5MetatileMap[];
extern u8 gLevel8Stage2Room5BlockLayer[];
extern u8 gLevel8Stage2Room5BlockMetatiles[];
struct RoomDef gLevel8Stage2Room5 ROOM_DEF(083a46fc) = {
    .filler00 = { 8, 2, 5, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage2Room5MetatileMap,
    .blockLayer = gLevel8Stage2Room5BlockLayer,
    .blockMetatiles = gLevel8Stage2Room5BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853DBB8,
    .bg2Tiles = gUnk_0843FC10,
    .metatileTiles = gUnk_083B18FC,
    .borderX = 4,
    .borderY = 8,
    .bg3Palette = gUnk_0835D60C,
    .bg3Tiles = gUnk_083F9FC8,
    .bg3Map = &gUnk_083B9C18,
    .bg3BorderX = 0,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 6,
};

/* gRoomTable[8][3][0], 0x083A4D80, section .room_def_083a4d80 */
extern u8 gLevel8Stage3Room0MetatileMap[];
extern u8 gLevel8Stage3Room0BlockLayer[];
extern u8 gLevel8Stage3Room0BlockMetatiles[];
extern struct Door gLevel8Stage3Room0Doors[];
struct RoomDef gLevel8Stage3Room0 ROOM_DEF(083a4d80) = {
    .filler00 = { 8, 3, 0, 0 },
    .bgm = 7,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage3Room0MetatileMap,
    .blockLayer = gLevel8Stage3Room0BlockLayer,
    .blockMetatiles = gLevel8Stage3Room0BlockMetatiles,
    .width = 48,
    .height = 23,
    .bg2Palette = gUnk_08539D8C,
    .bg2Tiles = gUnk_08434EFC,
    .metatileTiles = gUnk_083AEBB4,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D0E4,
    .bg3Tiles = gUnk_0840F56C,
    .bg3Map = &gUnk_083C0E48,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 16,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage3Room0Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 1,
};

/* gRoomTable[8][3][1], 0x083A4E08, section .room_def_083a4e08 */
extern u8 gLevel8Stage3Room1MetatileMap[];
extern u8 gLevel8Stage3Room1BlockLayer[];
extern u8 gLevel8Stage3Room1BlockMetatiles[];
struct RoomDef gLevel8Stage3Room1 ROOM_DEF(083a4e08) = {
    .filler00 = { 8, 3, 1, 0 },
    .bgm = 7,
    .mapsCompressed = 0,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage3Room1MetatileMap,
    .blockLayer = gLevel8Stage3Room1BlockLayer,
    .blockMetatiles = gLevel8Stage3Room1BlockMetatiles,
    .width = 2,
    .height = 2,
    .bg2Palette = gUnk_08539D8C,
    .bg2Tiles = gUnk_08434EFC,
    .metatileTiles = gUnk_083AEBB4,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D0E4,
    .bg3Tiles = gUnk_0840F56C,
    .bg3Map = &gUnk_083C0E48,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 8,
};

/* gRoomTable[8][3][2], 0x083A4FA8, section .room_def_083a4fa8 */
extern u8 gLevel8Stage3Room2MetatileMap[];
extern u8 gLevel8Stage3Room2BlockLayer[];
extern u8 gLevel8Stage3Room2BlockMetatiles[];
extern struct Door gLevel8Stage3Room2Doors[];
extern u8 gLevel8Stage3Room2Objects[];
struct RoomDef gLevel8Stage3Room2 ROOM_DEF(083a4fa8) = {
    .filler00 = { 8, 3, 2, 0 },
    .bgm = 39,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage3Room2MetatileMap,
    .blockLayer = gLevel8Stage3Room2BlockLayer,
    .blockMetatiles = gLevel8Stage3Room2BlockMetatiles,
    .width = 18,
    .height = 12,
    .bg2Palette = gUnk_084E56DC,
    .bg2Tiles = gUnk_084EB4F4,
    .metatileTiles = gUnk_084E57E0,
    .borderX = 24,
    .borderY = 16,
    .bg3Palette = gUnk_084E55D8,
    .bg3Tiles = gUnk_084E6184,
    .bg3Map = &gUnk_084E5928,
    .bg3BorderX = 16,
    .bg3BorderY = 16,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 7,
    .unk42 = 0,
    .doors = gLevel8Stage3Room2Doors,
    .objects = gLevel8Stage3Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 4,
};

/* gRoomTable[8][3][3], 0x083A50D0, section .room_def_083a50d0 */
extern u8 gLevel8Stage3Room3MetatileMap[];
extern u8 gLevel8Stage3Room3BlockLayer[];
extern u8 gLevel8Stage3Room3BlockMetatiles[];
extern struct Door gLevel8Stage3Room3Doors[];
extern u8 gLevel8Stage3Room3Objects[];
struct RoomDef gLevel8Stage3Room3 ROOM_DEF(083a50d0) = {
    .filler00 = { 8, 3, 3, 0 },
    .bgm = 25,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage3Room3MetatileMap,
    .blockLayer = gLevel8Stage3Room3BlockLayer,
    .blockMetatiles = gLevel8Stage3Room3BlockMetatiles,
    .width = 16,
    .height = 12,
    .bg2Palette = gUnk_084BB5C0,
    .bg2Tiles = gUnk_084BB5F4,
    .metatileTiles = gUnk_084BB5E4,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gLevel8Stage3Room3Bg3Palette,
    .bg3Tiles = gUnk_084DEE4C,
    .bg3Map = &gLevel8Stage3Room3Bg3Map,
    .bg3BorderX = 16,
    .bg3BorderY = 16,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage3Room3Doors,
    .objects = gLevel8Stage3Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 120,
    .entryY = 152,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 3,
};

/* gRoomTable[8][3][4], 0x083A5354, section .room_def_083a5354 */
extern u8 gLevel8Stage3Room4MetatileMap[];
extern u8 gLevel8Stage3Room4BlockLayer[];
extern u8 gLevel8Stage3Room4BlockMetatiles[];
extern struct Door gLevel8Stage3Room4Doors[];
extern u8 gLevel8Stage3Room4Objects[];
struct RoomDef gLevel8Stage3Room4 ROOM_DEF(083a5354) = {
    .filler00 = { 8, 3, 4, 0 },
    .bgm = 25,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage3Room4MetatileMap,
    .blockLayer = gLevel8Stage3Room4BlockLayer,
    .blockMetatiles = gLevel8Stage3Room4BlockMetatiles,
    .width = 16,
    .height = 36,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gUnk_08505B20,
    .bg3Tiles = gUnk_08506F4C,
    .bg3Map = &gUnk_08505C24,
    .bg3BorderX = 16,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage3Room4Doors,
    .objects = gLevel8Stage3Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 128,
    .entryY = 536,
    .unk54 = 3,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 2,
};

/* gRoomTable[8][3][5], 0x083A5480, section .room_def_083a5480 */
extern u8 gLevel8Stage3Room5MetatileMap[];
extern u8 gLevel8Stage3Room5BlockLayer[];
extern u8 gLevel8Stage3Room5BlockMetatiles[];
struct RoomDef gLevel8Stage3Room5 ROOM_DEF(083a5480) = {
    .filler00 = { 8, 3, 5, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage3Room5MetatileMap,
    .blockLayer = gLevel8Stage3Room5BlockLayer,
    .blockMetatiles = gLevel8Stage3Room5BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0835D548,
    .bg2Tiles = gUnk_083E0A24,
    .metatileTiles = gUnk_083AA000,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08539BA4,
    .bg3Tiles = gUnk_083FE694,
    .bg3Map = &gUnk_083BA534,
    .bg3BorderX = 0,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 6,
};

/* gRoomTable[8][4][0], 0x083A5BE8, section .room_def_083a5be8 */
extern u8 gLevel8Stage4Room0MetatileMap[];
extern u8 gLevel8Stage4Room0BlockLayer[];
extern u8 gLevel8Stage4Room0BlockMetatiles[];
extern struct Door gLevel8Stage4Room0Doors[];
struct RoomDef gLevel8Stage4Room0 ROOM_DEF(083a5be8) = {
    .filler00 = { 8, 4, 0, 0 },
    .bgm = 8,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage4Room0MetatileMap,
    .blockLayer = gLevel8Stage4Room0BlockLayer,
    .blockMetatiles = gLevel8Stage4Room0BlockMetatiles,
    .width = 48,
    .height = 23,
    .bg2Palette = gUnk_0853D7C8,
    .bg2Tiles = gUnk_08436318,
    .metatileTiles = gUnk_083AF1FC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D0E4,
    .bg3Tiles = gUnk_0840F754,
    .bg3Map = &gUnk_083C2DD0,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 17,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage4Room0Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 1,
};

/* gRoomTable[8][4][1], 0x083A5C70, section .room_def_083a5c70 */
extern u8 gLevel8Stage4Room1MetatileMap[];
extern u8 gLevel8Stage4Room1BlockLayer[];
extern u8 gLevel8Stage4Room1BlockMetatiles[];
struct RoomDef gLevel8Stage4Room1 ROOM_DEF(083a5c70) = {
    .filler00 = { 8, 4, 1, 0 },
    .bgm = 8,
    .mapsCompressed = 0,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage4Room1MetatileMap,
    .blockLayer = gLevel8Stage4Room1BlockLayer,
    .blockMetatiles = gLevel8Stage4Room1BlockMetatiles,
    .width = 2,
    .height = 2,
    .bg2Palette = gUnk_0853D7C8,
    .bg2Tiles = gUnk_08436318,
    .metatileTiles = gUnk_083AF1FC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D0E4,
    .bg3Tiles = gUnk_0840F754,
    .bg3Map = &gUnk_083C2DD0,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 8,
};

/* gRoomTable[8][4][2], 0x083A5E10, section .room_def_083a5e10 */
extern u8 gLevel8Stage4Room2MetatileMap[];
extern u8 gLevel8Stage4Room2BlockLayer[];
extern u8 gLevel8Stage4Room2BlockMetatiles[];
extern struct Door gLevel8Stage4Room2Doors[];
extern u8 gLevel8Stage4Room2Objects[];
struct RoomDef gLevel8Stage4Room2 ROOM_DEF(083a5e10) = {
    .filler00 = { 8, 4, 2, 0 },
    .bgm = 39,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage4Room2MetatileMap,
    .blockLayer = gLevel8Stage4Room2BlockLayer,
    .blockMetatiles = gLevel8Stage4Room2BlockMetatiles,
    .width = 18,
    .height = 12,
    .bg2Palette = gUnk_084E56DC,
    .bg2Tiles = gUnk_084EB4F4,
    .metatileTiles = gUnk_084E57E0,
    .borderX = 24,
    .borderY = 16,
    .bg3Palette = gUnk_084E55D8,
    .bg3Tiles = gUnk_084E6184,
    .bg3Map = &gUnk_084E5928,
    .bg3BorderX = 16,
    .bg3BorderY = 16,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 7,
    .unk42 = 0,
    .doors = gLevel8Stage4Room2Doors,
    .objects = gLevel8Stage4Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 4,
};

/* gRoomTable[8][4][3], 0x083A5F3C, section .room_def_083a5f3c */
extern u8 gLevel8Stage4Room3MetatileMap[];
extern u8 gLevel8Stage4Room3BlockLayer[];
extern u8 gLevel8Stage4Room3BlockMetatiles[];
extern struct Door gLevel8Stage4Room3Doors[];
extern u8 gLevel8Stage4Room3Objects[];
struct RoomDef gLevel8Stage4Room3 ROOM_DEF(083a5f3c) = {
    .filler00 = { 8, 4, 3, 0 },
    .bgm = 25,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage4Room3MetatileMap,
    .blockLayer = gLevel8Stage4Room3BlockLayer,
    .blockMetatiles = gLevel8Stage4Room3BlockMetatiles,
    .width = 16,
    .height = 12,
    .bg2Palette = gUnk_084BB5C0,
    .bg2Tiles = gUnk_084BB5F4,
    .metatileTiles = gUnk_084BB5E4,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gLevel8Stage4Room3Bg3Palette,
    .bg3Tiles = gUnk_084DEE4C,
    .bg3Map = &gLevel8Stage4Room3Bg3Map,
    .bg3BorderX = 16,
    .bg3BorderY = 16,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 2,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage4Room3Doors,
    .objects = gLevel8Stage4Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 3,
};

/* gRoomTable[8][4][4], 0x083A61E4, section .room_def_083a61e4 */
extern u8 gLevel8Stage4Room4MetatileMap[];
extern u8 gLevel8Stage4Room4BlockLayer[];
extern u8 gLevel8Stage4Room4BlockMetatiles[];
extern struct Door gLevel8Stage4Room4Doors[];
extern u8 gLevel8Stage4Room4Objects[];
struct RoomDef gLevel8Stage4Room4 ROOM_DEF(083a61e4) = {
    .filler00 = { 8, 4, 4, 0 },
    .bgm = 25,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage4Room4MetatileMap,
    .blockLayer = gLevel8Stage4Room4BlockLayer,
    .blockMetatiles = gLevel8Stage4Room4BlockMetatiles,
    .width = 16,
    .height = 36,
    .bg2Palette = gUnk_0853DAB4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gUnk_08505B20,
    .bg3Tiles = gUnk_08506F4C,
    .bg3Map = &gUnk_08505C24,
    .bg3BorderX = 16,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage4Room4Doors,
    .objects = gLevel8Stage4Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 120,
    .entryY = 536,
    .unk54 = 3,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 2,
};

/* gRoomTable[8][4][5], 0x083A633C, section .room_def_083a633c */
extern u8 gLevel8Stage4Room5MetatileMap[];
extern u8 gLevel8Stage4Room5BlockLayer[];
extern u8 gLevel8Stage4Room5BlockMetatiles[];
struct RoomDef gLevel8Stage4Room5 ROOM_DEF(083a633c) = {
    .filler00 = { 8, 4, 5, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage4Room5MetatileMap,
    .blockLayer = gLevel8Stage4Room5BlockLayer,
    .blockMetatiles = gLevel8Stage4Room5BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0853DAB4,
    .bg2Tiles = gUnk_0843A824,
    .metatileTiles = gUnk_083B0700,
    .borderX = 4,
    .borderY = 8,
    .bg3Palette = gUnk_0853CEDC,
    .bg3Tiles = gUnk_08401D3C,
    .bg3Map = &gUnk_083BADEC,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 1,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 6,
};

/* gRoomTable[8][5][0], 0x083A6AE0, section .room_def_083a6ae0 */
extern u8 gLevel8Stage5Room0MetatileMap[];
extern u8 gLevel8Stage5Room0BlockLayer[];
extern u8 gLevel8Stage5Room0BlockMetatiles[];
extern struct Door gLevel8Stage5Room0Doors[];
struct RoomDef gLevel8Stage5Room0 ROOM_DEF(083a6ae0) = {
    .filler00 = { 8, 5, 0, 0 },
    .bgm = 9,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage5Room0MetatileMap,
    .blockLayer = gLevel8Stage5Room0BlockLayer,
    .blockMetatiles = gLevel8Stage5Room0BlockMetatiles,
    .width = 48,
    .height = 23,
    .bg2Palette = gUnk_0853D8CC,
    .bg2Tiles = gUnk_08437924,
    .metatileTiles = gUnk_083AF7E0,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D108,
    .bg3Tiles = gUnk_0840F99C,
    .bg3Map = &gUnk_083C5058,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 17,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage5Room0Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 1,
};

/* gRoomTable[8][5][1], 0x083A6B68, section .room_def_083a6b68 */
extern u8 gLevel8Stage5Room1MetatileMap[];
extern u8 gLevel8Stage5Room1BlockLayer[];
extern u8 gLevel8Stage5Room1BlockMetatiles[];
struct RoomDef gLevel8Stage5Room1 ROOM_DEF(083a6b68) = {
    .filler00 = { 8, 5, 1, 0 },
    .bgm = 9,
    .mapsCompressed = 0,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage5Room1MetatileMap,
    .blockLayer = gLevel8Stage5Room1BlockLayer,
    .blockMetatiles = gLevel8Stage5Room1BlockMetatiles,
    .width = 2,
    .height = 2,
    .bg2Palette = gUnk_0853D8CC,
    .bg2Tiles = gUnk_08437924,
    .metatileTiles = gUnk_083AF7E0,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D108,
    .bg3Tiles = gUnk_0840F99C,
    .bg3Map = &gUnk_083C5058,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 8,
};

/* gRoomTable[8][5][2], 0x083A6D08, section .room_def_083a6d08 */
extern u8 gLevel8Stage5Room2MetatileMap[];
extern u8 gLevel8Stage5Room2BlockLayer[];
extern u8 gLevel8Stage5Room2BlockMetatiles[];
extern struct Door gLevel8Stage5Room2Doors[];
extern u8 gLevel8Stage5Room2Objects[];
struct RoomDef gLevel8Stage5Room2 ROOM_DEF(083a6d08) = {
    .filler00 = { 8, 5, 2, 0 },
    .bgm = 39,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage5Room2MetatileMap,
    .blockLayer = gLevel8Stage5Room2BlockLayer,
    .blockMetatiles = gLevel8Stage5Room2BlockMetatiles,
    .width = 18,
    .height = 12,
    .bg2Palette = gUnk_084E56DC,
    .bg2Tiles = gUnk_084EB4F4,
    .metatileTiles = gUnk_084E57E0,
    .borderX = 24,
    .borderY = 16,
    .bg3Palette = gUnk_084E55D8,
    .bg3Tiles = gUnk_084E6184,
    .bg3Map = &gUnk_084E5928,
    .bg3BorderX = 16,
    .bg3BorderY = 16,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 7,
    .unk42 = 0,
    .doors = gLevel8Stage5Room2Doors,
    .objects = gLevel8Stage5Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 56,
    .entryY = 136,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 4,
};

/* gRoomTable[8][5][3], 0x083A6E24, section .room_def_083a6e24 */
extern u8 gLevel8Stage5Room3MetatileMap[];
extern u8 gLevel8Stage5Room3BlockLayer[];
extern u8 gLevel8Stage5Room3BlockMetatiles[];
extern struct Door gLevel8Stage5Room3Doors[];
extern u8 gLevel8Stage5Room3Objects[];
struct RoomDef gLevel8Stage5Room3 ROOM_DEF(083a6e24) = {
    .filler00 = { 8, 5, 3, 0 },
    .bgm = 25,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage5Room3MetatileMap,
    .blockLayer = gLevel8Stage5Room3BlockLayer,
    .blockMetatiles = gLevel8Stage5Room3BlockMetatiles,
    .width = 16,
    .height = 12,
    .bg2Palette = gUnk_084BB5C0,
    .bg2Tiles = gUnk_084BB5F4,
    .metatileTiles = gUnk_084BB5E4,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gLevel8Stage5Room3Bg3Palette,
    .bg3Tiles = gUnk_084D8E30,
    .bg3Map = &gLevel8Stage5Room3Bg3Map,
    .bg3BorderX = 16,
    .bg3BorderY = 16,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 1,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage5Room3Doors,
    .objects = gLevel8Stage5Room3Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 3,
};

/* gRoomTable[8][5][4], 0x083A70A0, section .room_def_083a70a0 */
extern u8 gLevel8Stage5Room4MetatileMap[];
extern u8 gLevel8Stage5Room4BlockLayer[];
extern u8 gLevel8Stage5Room4BlockMetatiles[];
extern struct Door gLevel8Stage5Room4Doors[];
extern u8 gLevel8Stage5Room4Objects[];
struct RoomDef gLevel8Stage5Room4 ROOM_DEF(083a70a0) = {
    .filler00 = { 8, 5, 4, 0 },
    .bgm = 25,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage5Room4MetatileMap,
    .blockLayer = gLevel8Stage5Room4BlockLayer,
    .blockMetatiles = gLevel8Stage5Room4BlockMetatiles,
    .width = 16,
    .height = 36,
    .bg2Palette = gUnk_0852D6E8,
    .bg2Tiles = gUnk_08534160,
    .metatileTiles = gUnk_0852E7CC,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gUnk_08505B20,
    .bg3Tiles = gUnk_08506F4C,
    .bg3Map = &gUnk_08505C24,
    .bg3BorderX = 16,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage5Room4Doors,
    .objects = gLevel8Stage5Room4Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 128,
    .entryY = 536,
    .unk54 = 3,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 2,
};

/* gRoomTable[8][5][5], 0x083A71E4, section .room_def_083a71e4 */
extern u8 gLevel8Stage5Room5MetatileMap[];
extern u8 gLevel8Stage5Room5BlockLayer[];
extern u8 gLevel8Stage5Room5BlockMetatiles[];
struct RoomDef gLevel8Stage5Room5 ROOM_DEF(083a71e4) = {
    .filler00 = { 8, 5, 5, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage5Room5MetatileMap,
    .blockLayer = gLevel8Stage5Room5BlockLayer,
    .blockMetatiles = gLevel8Stage5Room5BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0852D6E8,
    .bg2Tiles = gUnk_08534160,
    .metatileTiles = gUnk_0852E7CC,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_08510B9C,
    .bg3Tiles = gUnk_08460458,
    .bg3Map = &gUnk_083CDCD4,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 11,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 6,
};

/* gRoomTable[8][6][0], 0x083A77C4, section .room_def_083a77c4 */
extern u8 gLevel8Stage6Room0MetatileMap[];
extern u8 gLevel8Stage6Room0BlockLayer[];
extern u8 gLevel8Stage6Room0BlockMetatiles[];
extern struct Door gLevel8Stage6Room0Doors[];
struct RoomDef gLevel8Stage6Room0 ROOM_DEF(083a77c4) = {
    .filler00 = { 8, 6, 0, 0 },
    .bgm = 10,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage6Room0MetatileMap,
    .blockLayer = gLevel8Stage6Room0BlockLayer,
    .blockMetatiles = gLevel8Stage6Room0BlockMetatiles,
    .width = 32,
    .height = 23,
    .bg2Palette = gUnk_0853D9D0,
    .bg2Tiles = gUnk_08439268,
    .metatileTiles = gUnk_083AFFE4,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D0E4,
    .bg3Tiles = gUnk_0840FB54,
    .bg3Map = &gUnk_083C72E0,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 11,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage6Room0Doors,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 1,
};

/* gRoomTable[8][6][1], 0x083A784C, section .room_def_083a784c */
extern u8 gLevel8Stage6Room1MetatileMap[];
extern u8 gLevel8Stage6Room1BlockLayer[];
extern u8 gLevel8Stage6Room1BlockMetatiles[];
struct RoomDef gLevel8Stage6Room1 ROOM_DEF(083a784c) = {
    .filler00 = { 8, 6, 1, 0 },
    .bgm = 10,
    .mapsCompressed = 0,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage6Room1MetatileMap,
    .blockLayer = gLevel8Stage6Room1BlockLayer,
    .blockMetatiles = gLevel8Stage6Room1BlockMetatiles,
    .width = 2,
    .height = 2,
    .bg2Palette = gUnk_0853D9D0,
    .bg2Tiles = gUnk_08439268,
    .metatileTiles = gUnk_083AFFE4,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853D0E4,
    .bg3Tiles = gUnk_0840FB54,
    .bg3Map = &gUnk_083C72E0,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 8,
};

/* gRoomTable[8][6][2], 0x083A7ADC, section .room_def_083a7adc */
extern u8 gLevel8Stage6Room2MetatileMap[];
extern u8 gLevel8Stage6Room2BlockLayer[];
extern u8 gLevel8Stage6Room2BlockMetatiles[];
extern struct Door gLevel8Stage6Room2Doors[];
extern u8 gLevel8Stage6Room2Objects[];
struct RoomDef gLevel8Stage6Room2 ROOM_DEF(083a7adc) = {
    .filler00 = { 8, 6, 2, 0 },
    .bgm = 25,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage6Room2MetatileMap,
    .blockLayer = gLevel8Stage6Room2BlockLayer,
    .blockMetatiles = gLevel8Stage6Room2BlockMetatiles,
    .width = 16,
    .height = 36,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 8,
    .borderY = 16,
    .bg3Palette = gUnk_08505B20,
    .bg3Tiles = gUnk_08506F4C,
    .bg3Map = &gUnk_08505C24,
    .bg3BorderX = 16,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 1,
    .objectCount = 6,
    .objectsSortedByY = 1,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = gLevel8Stage6Room2Doors,
    .objects = gLevel8Stage6Room2Objects,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 128,
    .entryY = 536,
    .unk54 = 3,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 2,
};

/* gRoomTable[8][6][3], 0x083A7C28, section .room_def_083a7c28 */
extern u8 gLevel8Stage6Room3MetatileMap[];
extern u8 gLevel8Stage6Room3BlockLayer[];
extern u8 gLevel8Stage6Room3BlockMetatiles[];
struct RoomDef gLevel8Stage6Room3 ROOM_DEF(083a7c28) = {
    .filler00 = { 8, 6, 3, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage6Room3MetatileMap,
    .blockLayer = gLevel8Stage6Room3BlockLayer,
    .blockMetatiles = gLevel8Stage6Room3BlockMetatiles,
    .width = 16,
    .height = 11,
    .bg2Palette = gUnk_0852BB78,
    .bg2Tiles = gUnk_08442F18,
    .metatileTiles = gUnk_083B2254,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0853E520,
    .bg3Tiles = gUnk_08474C08,
    .bg3Map = &gUnk_083CF82C,
    .bg3BorderX = 8,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 6,
};

/* gRoomTable[8][7][0], 0x083A8364, section .room_def_083a8364 */
extern u8 gLevel8Stage7Room0MetatileMap[];
extern u8 gLevel8Stage7Room0BlockLayer[];
extern u8 gLevel8Stage7Room0BlockMetatiles[];
struct RoomDef gLevel8Stage7Room0 ROOM_DEF(083a8364) = {
    .filler00 = { 8, 7, 0, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage7Room0MetatileMap,
    .blockLayer = gLevel8Stage7Room0BlockLayer,
    .blockMetatiles = gLevel8Stage7Room0BlockMetatiles,
    .width = 16,
    .height = 74,
    .bg2Palette = gLevel8Stage7Room0Bg2Palette,
    .bg2Tiles = gLevel8Stage7Room0Bg2Tiles,
    .metatileTiles = gLevel8Stage7Room0MetatileTiles,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gLevel8Stage7Room0Bg3Palette,
    .bg3Tiles = gLevel8Stage7Room0Bg3Tiles,
    .bg3Map = &gLevel8Stage7Room0Bg3Map,
    .bg3BorderX = 8,
    .bg3BorderY = 8,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 1,
    .unk56 = 0,
    .unk57 = 7,
};

/* gRoomTable[8][7][1], 0x083A85D4, section .room_def_083a85d4 */
extern u8 gLevel8Stage7Room1MetatileMap[];
extern u8 gLevel8Stage7Room1BlockLayer[];
extern u8 gLevel8Stage7Room1BlockMetatiles[];
struct RoomDef gLevel8Stage7Room1 ROOM_DEF(083a85d4) = {
    .filler00 = { 8, 7, 1, 0 },
    .bgm = 0,
    .mapsCompressed = 1,
    .filler06 = { 0, 0 },
    .metatileMap = gLevel8Stage7Room1MetatileMap,
    .blockLayer = gLevel8Stage7Room1BlockLayer,
    .blockMetatiles = gLevel8Stage7Room1BlockMetatiles,
    .width = 48,
    .height = 11,
    .bg2Palette = gUnk_0852BD00,
    .bg2Tiles = gUnk_084B0C9C,
    .metatileTiles = gUnk_084AF93C,
    .borderX = 8,
    .borderY = 8,
    .bg3Palette = gUnk_0835D710,
    .bg3Tiles = gUnk_08412BD8,
    .bg3Map = &gLevel8Stage7Room1Bg3Map,
    .bg3BorderX = 0,
    .bg3BorderY = 0,
    .driftObjectIndex = 0xFFFF,
    .doorCount = 0,
    .objectCount = 0,
    .objectsSortedByY = 0,
    .bgAnimSet = 0,
    .unk42 = 0,
    .doors = NULL,
    .objects = NULL,
    .filler4C = { 0, 0, 0, 0 },
    .entryX = 64,
    .entryY = 64,
    .unk54 = 0,
    .unk55 = 0,
    .unk56 = 0,
    .unk57 = 6,
};
