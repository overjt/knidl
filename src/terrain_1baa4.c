#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "room.h"

/* terrain_1baa4.c (0x0801BAA4-0x0801BCAB, issue #84).
 *
 * The player's per-frame terrain entry point.  M09's player task
 * (src/player_32688.c) calls it with the player's box record; it is the body
 * of src/terrain_1bcac.c's entry points, but the room-relative position comes
 * from PlayerState.prevPixelX/unk60 and the box offsets from PlayerState.prevTerrainBox,
 * both wall probes run (in the order the facing gTerrainFacing picks), and
 * the probes' 8.8 push gTerrainDriftX/gTerrainDriftY is written back to
 * PlayerState.driftVelX/unk58 on top of gRoomDriftVelX/gRoomDriftVelY. */

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
    gTerrainDriftX = gTerrainDriftY = zero;
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
    if (gTerrainDriftX & 0x8000)
        r = ((gTerrainDriftX << 8) | 0xFF000000) + gRoomDriftVelX;
    else
        r = (gTerrainDriftX << 8) + gRoomDriftVelX;
    ps->driftVelX = r;
    ps = gCurTask->player;
    if (gTerrainDriftY & 0x8000)
        r = ((gTerrainDriftY << 8) | 0xFF000000) + gRoomDriftVelY;
    else
        r = (gTerrainDriftY << 8) + gRoomDriftVelY;
    ps->driftVelY = r;
    gTerrainResult.atDoor = gTerrainProbeResult.atDoor;
    gTerrainResult.unk6 = gTerrainProbeResult.unkD & 15;
    TerrainProbeEnd((const s8 *)p);
}
