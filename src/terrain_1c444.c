#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "room.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0801C444-0x0801C51C.
 *
 * sub_0801c444: the same entry-point body as src/terrain_1bcac.c, clearing
 * the pending-collision flag and running the tile-edge probe sub_08020b38.
 */

s32 TerrainQueryPixelAndSides(u32 x, u32 y);
void TerrainProbeBegin(const s8 *p);
void sub_080207a0(void);
void sub_0801c690(void);
void sub_0801c8dc(void);
void sub_0801d394(void);
void sub_0801dc88(void);
void sub_0801e178(void);
void sub_0801f540(void);
void sub_0801f800(void);
void sub_08022810(void);
void sub_0801fc48(void);
void sub_0801ff84(void);
void TerrainProbeWater(void);
void sub_080222b0(s16 x, s16 y);
s32 GetTilePushUp(u16 a);
s32 GetTilePushLeft(u16 a);

void TerrainProbeBegin(const s8 *p);
void TerrainProbeEnd(const s8 *p);
void sub_08020b38(void);

void sub_0801c444(const s8 *p)
{
    TerrainProbeBegin(p);
    if (gTerrainProbeResult.unkB & 0x80)
        gTerrainProbeResult.unkB = 0;
    gTerrainVelX = gCurTask->velX;
    gTerrainVelY = gCurTask->velY;
    gTerrainPrevX = ((gTerrainProbeX << 16) + (gCurTask->posX & 0xFFFF) - gTerrainVelX) >> 16;
    gTerrainPrevY = ((gTerrainProbeY << 16) + (gCurTask->posY & 0xFFFF) - gTerrainVelY) >> 16;
    gTerrainPrevBoxLeft = gTerrainPrevX + gTerrainBoxLeft;
    gTerrainPrevBoxRight = gTerrainPrevX + gTerrainBoxRight;
    gTerrainPrevBoxTop = gTerrainPrevY + gTerrainBoxTop;
    gTerrainPrevBoxBottom = gTerrainPrevY + gTerrainBoxBottom;
    sub_08020b38();
    TerrainProbeEnd(p);
}
