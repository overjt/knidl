#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "room.h"

/* collision_player_probe.c (0x0801BAA4-0x0801BCAB, issue #84).
 *
 * The player's per-frame terrain entry point.  M09's player task
 * (src/player_task.c) calls it with the player's box record; it is the body
 * of src/collision_collide_box.c's entry points, but the room-relative position comes
 * from PlayerState.prevPixelX/unk60 and the box offsets from PlayerState.prevTerrainBox,
 * both wall probes run (in the order the facing gTerrainFacing picks), and
 * the probes' 8.8 push gTerrainDriftX/gTerrainDriftY is written back to
 * PlayerState.driftVelX/unk58 on top of gRoomDriftVelX/gRoomDriftVelY. */

/* The player's per-frame terrain entry point (M09's player task passes the
   player's box record): the position comes from PlayerState.prevPixelX/unk60
   and the box offsets from PlayerState.prevTerrainBox instead of the task. */
void PlayerProbeTerrain(u32 p)
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
        gTerrainProbeResult.floorRow = (gTerrainProbeY + gTerrainBoxBottom + 16) >> 4;
    }
    TerrainClampBoxToPlayerBounds();
    if (gTerrainProbeResult.onGround != 0)
    {
        if ((s8)gTerrainFacing == 1)
        {
            TerrainProbeWallLeftOnGround();
            TerrainProbeWallRightOnGround();
        }
        else
        {
            TerrainProbeWallRightOnGround();
            TerrainProbeWallLeftOnGround();
        }
        sub_0801c8dc();
        TerrainProbeFloorInCameraBounds();
    }
    else
    {
        if ((s8)gTerrainFacing == 1)
        {
            TerrainProbeWallLeftInAir();
            TerrainProbeWallRightInAir();
        }
        else
        {
            TerrainProbeWallRightInAir();
            TerrainProbeWallLeftInAir();
        }
        TerrainProbeCeiling();
        TerrainProbeLandingInCameraBounds();
    }
    TerrainProbeWaterAndDrift();
    TerrainProbeDamage();
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
