#ifndef GUARD_COLLISION_H
#define GUARD_COLLISION_H

#include "gba/types.h"

/* collision.h: the RAM cells and ROM tables of the box-vs-terrain collision
   engine, the actor-vs-collider hit tests (M06) and the map queries (M07). 
   One declaration per symbol, with the type its consumers prove (issue #36
   phase 2, docs/header-conventions.md). */

struct PlayerState;

/* An actor's attack box (ROM), pointed to by gAttackBox during the
   actor-vs-player hit tests: signed offsets from the actor's position
   (unk00/unk01) and the box edges relative to that point (left unk02, top
   unk03, right unk04, bottom unk05), then the attack's kind and flags. */
struct AttackBox
{
    /*0x00*/ s8 offsetX;
    /*0x01*/ s8 offsetY;
    /*0x02*/ s8 left;
    /*0x03*/ s8 top;
    /*0x04*/ s8 right;
    /*0x05*/ s8 bottom;
    /*0x06*/ u8 unk06;
    /*0x07*/ u8 unk07;
    /*0x08*/ u8 damage;
    /*0x09*/ u8 hitEffect;
    /*0x0A*/ u16 unk0A;
    /*0x0C*/ u16 unk0C;
    /*0x0E*/ u16 unk0E;
    /*0x10*/ u16 unk10;
    /*0x12*/ u16 unk12;
    /*0x14*/ u32 unk14;
    /*0x18*/ u16 unk18;
    /*0x1A*/ u16 unk1A;
};

/* A player's body box, pointed to by each entry of the hit list
   gPlayerColliders and cached in gColliderBodyBox: the same six signed offsets,
   then per-box bytes. */
struct BodyBox
{
    /*0x00*/ s8 offsetX;
    /*0x01*/ s8 offsetY;
    /*0x02*/ s8 left;
    /*0x03*/ s8 top;
    /*0x04*/ s8 right;
    /*0x05*/ s8 bottom;
    /*0x06*/ u8 unk06;
    /*0x07*/ u8 unk07;
    /*0x08*/ u8 unk08;
    /*0x09*/ u8 unk09;
    /*0x0A*/ u8 unk0A;
    /*0x0B*/ u8 unk0B;
    /*0x0C*/ u8 damage;
    /*0x0D*/ u8 hitEffect;
    /*0x0E*/ u16 unk0E;
    /*0x10*/ u16 unk10;
};

struct Collider
{
    /*0x00*/ u8 slot;
    /*0x01*/ u8 filler01;
    /*0x02*/ u16 x;
    /*0x04*/ u16 y;
    /*0x06*/ u8 filler06[2];
    /*0x08*/ u8 *bodyBox;
};

/* The probe result block, filled by the terrain probes and mirrored into
   gTerrainResult by TerrainProbeEnd. */
struct Unk03005530
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 ceilingHits;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 slope;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 onGround;
    /*0x07*/ u8 waterFlags;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 atDoor;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
    /*0x0E*/ u8 onSlipperyFloor;
    /*0x0F*/ u8 damage;
    /*0x10*/ u8 unk10;
};

/* gTerrainResult: M06's collision result block; M09 reads unk8 with ldrsh.
   A 16-bit test of unk0/unk1 together is `*(u16 *)&gTerrainResult`
   (M12's PlayerActionBurningUpdate). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 ceilingHits;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 slope;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 atDoor;
    /*0x0B*/ u8 onSlipperyFloor;
    /*0x0C*/ u8 damage;
    /*0x0D*/ u8 unkD;
};

/* IWRAM */
/* Actor-vs-player hit test cells (M17's src/actor_673ec.c widths). */
extern s16 gHitMidpointX;
extern u8 gUnk_03001F24;
extern s8 gAttackHitDuration;
extern u8 gHitDirection;
extern s16 gHitMidpointY;
extern u16 gAttackY; /* actor y */
extern u8 gHitTimer;
extern u16 gAttackX; /* actor x */
extern u16 gAttackHealth;
extern struct AttackBox *gAttackBox; /* the actor's attack box (s32 in actor_673ec.c) */
extern u8 gHitKind; /* hit result */
extern u8 gAttackLastHitterSlot;
extern u16 gHitHealthLeft;
extern s16 gAttackFacing;
extern u8 gHitterColliderClass;
extern u8 gHitterColliderKind;
extern u8 gHitterSlot;
extern u8 gAttackLastHitter;
extern u8 gHitEffect;
extern u8 gAttackLastHitterClass;
extern u8 gPlayerColliderCount; /* number of hit-list entries */
extern s16 gAttackBoxBottom; /* attack box bottom */
extern struct Collider gColliderClass20[];
extern s16 gColliderTop; /* body box top */
extern u8 gColliderPlayer; /* the current entry's player index */
extern struct Collider gColliderClass10[];
extern s16 gAttackBoxRight; /* attack box right */
extern s16 gColliderBottom; /* body box bottom */
extern u8 gColliderSlot; /* the current entry's task index */
extern s16 gColliderX; /* the current entry's x */
extern s16 gColliderY; /* the current entry's y */
extern s16 gAttackBoxLeft; /* attack box left */
extern u8 gColliderClass10Count;
extern struct Collider gPlayerColliders[];
extern s16 gAttackBoxTop; /* attack box top */
extern s16 gColliderRight; /* body box right */
extern struct BodyBox *gColliderBodyBox; /* the current entry's body box */
extern s16 gColliderLeft; /* body box left */
extern struct PlayerState *gColliderPlayerState; /* the current entry's player */
extern u8 gColliderClass20Count;
extern u16 gTerrainSlopeIndexLeft;
extern u16 gTerrainPixelIndex; /* pixel offset inside the queried cell */
extern s16 gTerrainPrevBoxLeft; /* box left (room-relative) */
extern u16 gTerrainTileRight;
extern s32 gTerrainVelY; /* Task.velY */
extern s16 gTerrainPrevX; /* actor x (room-relative) */
extern s16 gTerrainBoxLeft; /* box left offset */
extern s16 gTerrainPrevY; /* actor y (room-relative) */
extern struct Unk03005530 gTerrainProbeResult;
extern u16 gTerrainClampedTopY;
extern struct Unk03005550 gTerrainResult;
extern s16 gTerrainProbeX; /* probe x */
extern u8 gTerrainFacing; /* Task.facing */
extern u8 gTerrainBoundsClamp;
extern u16 gTerrainSlopeIndexRight;
extern s16 gTerrainProbeY; /* probe y */
extern u16 gTerrainSlopeIndex; /* queried cell: byte 2 */
extern u16 gTerrainTile; /* queried cell: tile set */
extern s16 gTerrainBoxTop; /* box top offset */
extern s32 gTerrainDriftY;
extern s16 gTerrainBoxBottom; /* box bottom offset */
extern u16 gTerrainTileBelow; /* cell below: tile set */
extern s16 gTerrainPrevBoxRight; /* box right (room-relative) */
extern u16 gTerrainTileLeft; /* cell to the left: tile set */
extern s32 gTerrainVelX; /* Task.velX */
extern s16 gTerrainBoxRight; /* box right offset */
extern s8 *gTerrainTileShape;
extern s16 gTerrainPrevBoxTop; /* box top (room-relative) */
extern s32 gTerrainDriftX;
extern u16 gTerrainSlopeIndexBelow; /* cell below: byte 2 */
extern s16 gTerrainPrevBoxBottom; /* box bottom (room-relative) */

/* ROM */
extern u16 gUnk_08732218[];
extern u16 gColliderClass10KindBits[];
extern u16 gColliderClass20KindHitKinds[];
extern u16 gUnk_08732242[];
extern u32 gUnk_08732254[];
extern u32 gColliderClass20KindBits[];
extern u32 gUnk_0873229C[];
extern s8 *const gCollisionTileShapes[]; /* per-tile-set pixel attribute tables */
extern u8 gCollisionTileSlope[];
extern u8 gUnk_08732DF0[];
extern s8 gCollisionTileShapeClass[];
extern s8 gCollisionTileDamaging[];
extern s8 *const gCollisionTileDamageShapes[];
extern u8 gCollisionTileSlippery[];
extern s8 gUnk_087335F0[];
extern s8 gCollisionTileOneWay[];
extern s8 gUnk_087337F0[];
extern s8 gUnk_087338F0[];
extern s8 gUnk_087339F0[];
extern s8 gCollisionTileDoor[];
extern u8 *const gCollisionTilePushDown[];
extern u8 *const gCollisionTilePushUp[];
extern u8 *const gCollisionTilePushRight[];
extern u8 *const gCollisionTilePushLeft[];
extern u8 *const gCollisionTileFloorSnap[];
extern u16 gSlopeIndexTiles[]; /* indexed by the cell's byte 2 */
extern u16 gUnk_08735098[];


/* Functions (defined in the files named above each group). */

struct Task;

/* The collider classes are the high nibble of a box's byte 8, which
 * RegisterCollider (src/player_1a76c.c) sorts into three lists.  Class 0x00
 * is the players' bodies (gPlayerColliders).  The other two are named by
 * their class value, because no role word is true for every registrant
 * (#155 run 4):
 * - class 0x10 (gColliderClass10): every PlayerObject task (#6), the water
 *   shot, the kicked ice block and the Backdrop/Throw held and thrown
 *   enemies.  HitTestColliderClass10 assumes an owner player, skips the
 *   last hitter's entries and damages the collider's own task;
 * - class 0x20 (gColliderClass20): the player's own moves, Meta Knight's
 *   attacks, the player-centred effects (spark aura, mike, crash) and one
 *   ownerless actor, the defeat explosion.  HitTestColliderClass20 accepts
 *   ownerless entries, never damages the collider and reacts by the box's
 *   kind nibble (inhale, grab, slide, high fall, explosion, strike,
 *   freeze). */

/* src/hitbox_1a8c8.c */
u8 HitTestPlayerColliders(void);
u8 HitTestColliderClass10(void);

/* src/hitbox_1b24c.c */
u8 HitTestColliderClass20(void);

/* src/hitbox_1b7dc.c */
void PlaceAttackBox(void);
void CalcHitDamageAndDirection(void);
void HitRecordHitter(void);

/* src/terrain_1baa4.c */
void PlayerProbeTerrain(u32 p);

/* src/terrain_1c444.c */
void TerrainCollideBoxTileEdge(const s8 *p);

/* src/terrain_1c51c.c */
void TerrainProbeBegin(const s8 *p);
void TerrainProbeEnd(const s8 *p);

/* src/terrain_1c690.c */
void TerrainProbeWallRightOnGround(void);
void TerrainProbeWallLeftOnGround(void);

/* src/terrain_1c8dc.c */
void sub_0801c8dc(void);

/* src/terrain_1c930.c */
void sub_0801c930(void);

/* src/terrain_1d394.c */
void TerrainProbeFloor(void);

/* src/terrain_1d9c8.c */
void TerrainProbeWallRightInAir(void);
void TerrainProbeWallLeftInAir(void);
void TerrainProbeCeiling(void);

/* src/terrain_1e178.c */
void sub_0801e178(void);

/* src/terrain_1ecd0.c */
void TerrainProbeLanding(void);

/* src/terrain_1f540.c */
void sub_0801f540(void);
void sub_0801f6b0(void);
void TerrainProbeCeilingNoSlopeLink(void);
void sub_0801f9b8(void);
void TerrainProbeFloorNoSlopeLink(void);
void TerrainProbeLandingNoSlopeLink(void);

/* src/terrain_1ff84.c */
void TerrainProbeAlongVelocity(void);
void sub_08020698(void);

/* src/terrain_2069c.c */
u32 TerrainProbePointStop(void);

/* src/terrain_207a0.c */
void TerrainProbePointPushOut(void);
void TerrainProbeTileEdge(void);

/* src/terrain_21130.c */
void TerrainProbeWaterAndDrift(void);

/* src/terrain_2136c.c */
void TerrainProbeWater(void);

/* src/terrain_214e0.c */
void TerrainProbeWaterAtPoint(void);
void TerrainProbeDamage(void);
s32 TerrainQueryPixel(u32 x, u32 y);
s32 TerrainQueryPixelAndBelow(u32 x, u32 y);
s32 TerrainQueryPixelAndSides(u32 x, u32 y);
s32 TerrainQueryDamage(u32 x, u32 y);
s32 GetTileFloorSnap(u16 a);
s32 GetTilePushDown(u16 a);
s32 GetTilePushUp(u16 a);
s32 GetTilePushRight(u16 a);
s32 GetTilePushLeft(u16 a);
void TerrainLoadFloorAttributes(u16 a);
s32 GetTileShapeAtPixel(u32 x, u32 y);

/* src/terrain_21b18.c */
s32 GetCollisionTileAtPixel(u16 x, u16 y);
s32 GetCollisionTile(u32 x, u32 y);
s32 sub_08021b70(u32 x, u32 y);
u16 sub_08021c14(s16 x, s16 y);
u8 IsWaterAtPixel(s16 x, s16 y);
void TerrainInitOneWayFloor(s32 x, s32 y);
void sub_0802233c(s8 *off);
void TaskInitWaterFlags(void);
void TaskInitWaterFlagsSlot(s32 id);
s32 sub_08022540(u32 x, u32 y);
u16 sub_0802259c(u16 x, u16 y);
s32 IsFullBlockAtPixel(u16 x, u16 y);
void TerrainClampBoxToPlayerBounds(void);
s32 IsTaskBelowPlayerBounds(struct Task *t);
s32 IsAtPlayerBoundsTop(s32 y, s32 i);
s32 ClampTaskToRoom(struct Task *t);
s32 sub_080228c4(struct Task *t);
s32 IsTaskBelowRoom(struct Task *t);

#endif /* GUARD_COLLISION_H */
