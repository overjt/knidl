#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_1baa4.c (0x0801BAA4-0x0801BCAB, issue #84).
 *
 * The player's per-frame terrain entry point.  M09's player task
 * (src/player_32688.c) calls it with the player's box record; it is the body
 * of src/terrain_1bcac.c's entry points, but the room-relative position comes
 * from PlayerState.prevPixelX/unk60 and the box offsets from PlayerState.prevTerrainBox,
 * both wall probes run (in the order the facing gTerrainFacing picks), and
 * the probes' 8.8 push gUnk_030055A8/gUnk_03005580 is written back to
 * PlayerState.unk54/unk58 on top of gUnk_030055F0/gUnk_03005618. */

/* The probe result block, filled by the terrain probes and mirrored into
   gTerrainResult by TerrainProbeEnd. */
struct Unk03005530
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 onGround;
    /*0x07*/ u8 unk7;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
    /*0x0E*/ u8 unkE;
    /*0x0F*/ u8 unkF;
    /*0x10*/ u8 unk10;
};

struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

extern s16 gTerrainPrevBoxLeft;           /* box left (room-relative) */
extern s32 gTerrainVelY;           /* Task.velY */
extern s16 gTerrainPrevX;           /* actor x (room-relative) */
extern s16 gTerrainPrevY;           /* actor y (room-relative) */
extern struct Unk03005530 gTerrainProbeResult;
extern struct Unk03005550 gTerrainResult;
extern s16 gTerrainProbeX;           /* probe x */
extern u8 gTerrainFacing;            /* Task.facing */
extern s16 gTerrainProbeY;           /* probe y */
extern s32 gUnk_03005580;
extern s16 gTerrainBoxBottom;           /* box bottom offset */
extern s16 gTerrainPrevBoxRight;           /* box right (room-relative) */
extern s32 gTerrainVelX;           /* Task.velX */
extern s16 gTerrainPrevBoxTop;           /* box top (room-relative) */
extern s32 gUnk_030055A8;
extern s16 gTerrainPrevBoxBottom;           /* box bottom (room-relative) */
extern s32 gUnk_030055F0;
extern s32 gUnk_03005618;

void TerrainProbeBegin(const s8 *p);
void TerrainProbeEnd(const s8 *p);
void sub_0801c690(void);
void sub_0801c7cc(void);
void sub_0801c8dc(void);
void sub_0801c930(void);
void sub_0801d9c8(void);
void sub_0801dc88(void);
void sub_0801dee8(void);
void sub_0801e178(void);
void sub_08021130(void);
void sub_08021564(void);
void sub_08022650(void);

/* The player's per-frame terrain entry point (M09's player task passes the
   player's box record): the position comes from PlayerState.prevPixelX/unk60
   and the box offsets from PlayerState.prevTerrainBox instead of the task. */
void sub_0801baa4(u32 p)
{
    s8 *box;
    s32 zero;
    struct PlayerState *ps;
    s32 r;

    TerrainProbeBegin((const s8 *)p);
    gTerrainProbeResult.unkD = gCurTask->player->unk50 << 4;
    zero = 0;
    gTerrainPrevX = gCurTask->player->prevPixelX + (box = (s8 *)gCurTask->player->prevTerrainBox)[0];
    gTerrainPrevY = box[1] + gCurTask->player->prevPixelY;
    gTerrainPrevBoxLeft = gTerrainPrevX + box[4];
    gTerrainPrevBoxRight = gTerrainPrevX + box[5];
    gTerrainPrevBoxTop = gTerrainPrevY + box[2];
    gTerrainPrevBoxBottom = gTerrainPrevY + box[3];
    gTerrainVelX = gTerrainProbeX - gTerrainPrevX;
    gTerrainVelY = gTerrainProbeY - gTerrainPrevY;
    gUnk_030055A8 = gUnk_03005580 = zero;
    if (gTerrainProbeResult.unkB & 0x80)
    {
        gTerrainProbeResult.unkB = 1;
        gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 16) >> 4;
    }
    sub_08022650();
    if (gTerrainProbeResult.onGround != 0)
    {
        if ((s8)gTerrainFacing == 1)
        {
            sub_0801c7cc();
            sub_0801c690();
        }
        else
        {
            sub_0801c690();
            sub_0801c7cc();
        }
        sub_0801c8dc();
        sub_0801c930();
    }
    else
    {
        if ((s8)gTerrainFacing == 1)
        {
            sub_0801dc88();
            sub_0801d9c8();
        }
        else
        {
            sub_0801d9c8();
            sub_0801dc88();
        }
        sub_0801dee8();
        sub_0801e178();
    }
    sub_08021130();
    sub_08021564();
    ps = gCurTask->player;
    if (gUnk_030055A8 & 0x8000)
        r = ((gUnk_030055A8 << 8) | 0xFF000000) + gUnk_030055F0;
    else
        r = (gUnk_030055A8 << 8) + gUnk_030055F0;
    ps->unk54 = r;
    ps = gCurTask->player;
    if (gUnk_03005580 & 0x8000)
        r = ((gUnk_03005580 << 8) | 0xFF000000) + gUnk_03005618;
    else
        r = (gUnk_03005580 << 8) + gUnk_03005618;
    ps->unk58 = r;
    gTerrainResult.unkA = gTerrainProbeResult.unkA;
    gTerrainResult.unk6 = gTerrainProbeResult.unkD & 15;
    TerrainProbeEnd((const s8 *)p);
}
