#include "global.h"
#include "camera.h"

/* The rooms' BG animation script lists (0x087E1E78-0x087E1F57, issue #167):
 * gRoomBgAnimScripts[RoomDef.bgAnimSet] is a NULL-ended list of the
 * scripts (src/data/bg_anim_scripts.c) LoadRoomBgAnims
 * (src/camera_bg_anims.c) loads into the slots gBgAnims[], one slot per
 * script.  Set 0 means no BG animation (LoadRoomBgAnims tests it first),
 * so entry 0 is NULL; entry 13 is NULL too, and no room uses it (the 333
 * RoomDefs use sets 0-12).  Not const at the scripts: the lists hold the
 * struct BgAnimCmd * that LoadRoomBgAnims stores in
 * BgAnim.script.  Carved by tools/carve_data.py. */

/* gRoomBgAnimScripts[1] */
struct BgAnimCmd *const gRoomBgAnimSet1[] = {
    gRoomBgAnimSet1Script0,
    gRoomBgAnimSet1Script1,
    gRoomBgAnimSet1Script2,
    gRoomBgAnimSet1Script3,
    gRoomBgAnimSet1Script4,
    gRoomBgAnimSet1Script5,
    gRoomBgAnimSet1Script6,
    gRoomBgAnimSet1Script7,
    gRoomBgAnimSet1Script8,
    gRoomBgAnimSet1Script9,
    NULL,
};

/* gRoomBgAnimScripts[2] */
struct BgAnimCmd *const gRoomBgAnimSet2[] = {
    gRoomBgAnimSet2Script0,
    NULL,
};

/* gRoomBgAnimScripts[3] */
struct BgAnimCmd *const gRoomBgAnimSet3[] = {
    gRoomBgAnimSet3Script0,
    NULL,
};

/* gRoomBgAnimScripts[4] */
struct BgAnimCmd *const gRoomBgAnimSet4[] = {
    gRoomBgAnimSet4Script0,
    NULL,
};

/* gRoomBgAnimScripts[5] */
struct BgAnimCmd *const gRoomBgAnimSet5[] = {
    gRoomBgAnimSet5Script0,
    gRoomBgAnimSet5Script1,
    gRoomBgAnimSet5Script2,
    NULL,
};

/* gRoomBgAnimScripts[6] */
struct BgAnimCmd *const gRoomBgAnimSet6[] = {
    gRoomBgAnimSet6Script0,
    gRoomBgAnimSet6Script1,
    gRoomBgAnimSet6Script2,
    NULL,
};

/* gRoomBgAnimScripts[7] */
struct BgAnimCmd *const gRoomBgAnimSet7[] = {
    gRoomBgAnimSet7Script0,
    NULL,
};

/* gRoomBgAnimScripts[8] */
struct BgAnimCmd *const gRoomBgAnimSet8[] = {
    gRoomBgAnimSet8Script0,
    NULL,
};

/* gRoomBgAnimScripts[9] */
struct BgAnimCmd *const gRoomBgAnimSet9[] = {
    gRoomBgAnimSet9Script0,
    gRoomBgAnimSet9Script1,
    gRoomBgAnimSet9Script2,
    NULL,
};

/* gRoomBgAnimScripts[10] */
struct BgAnimCmd *const gRoomBgAnimSet10[] = {
    gRoomBgAnimSet10Script0,
    gRoomBgAnimSet10Script1,
    gRoomBgAnimSet10Script2,
    NULL,
};

/* gRoomBgAnimScripts[11] */
struct BgAnimCmd *const gRoomBgAnimSet11[] = {
    gRoomBgAnimSet11Script0,
    gRoomBgAnimSet11Script1,
    NULL,
};

/* gRoomBgAnimScripts[12] */
struct BgAnimCmd *const gRoomBgAnimSet12[] = {
    gRoomBgAnimSet12Script0,
    NULL,
};

struct BgAnimCmd *const *const gRoomBgAnimScripts[] = {
    NULL,
    gRoomBgAnimSet1,
    gRoomBgAnimSet2,
    gRoomBgAnimSet3,
    gRoomBgAnimSet4,
    gRoomBgAnimSet5,
    gRoomBgAnimSet6,
    gRoomBgAnimSet7,
    gRoomBgAnimSet8,
    gRoomBgAnimSet9,
    gRoomBgAnimSet10,
    gRoomBgAnimSet11,
    gRoomBgAnimSet12,
    NULL,
};
