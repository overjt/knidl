#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* subgame_c3648.c (0x080C3648-0x080C3F43, issue #98).
 *
 * Sub-game 2: the racers' movement (task type #96 variant 0's callbacks).
 * 
 *   sub_080c3efc / sub_080c3f20   the two per-frame bodies variant 0 installs
 *       in Task.lateUpdateCallback (racing / before the start and after the finish): run
 *       sub_080c383c, then sub_080c38c8 or sub_080c3d58, then sub_080c3e18.
 *   sub_080c383c   player 0 drives the camera (sub_080c3670) and rebuilds the
 *       course view (AirGrindDrawCourse) and keeps the leader's position in
 *       M37Game.unk1B8; every racer sets Task.layer/unk3E from the course
 *       record's unk18 and Task.unk28 from its unk08.
 *   sub_080c38c8   the racing step: holding A (M37Player.unk02 & 1) on the
 *       course (record unk14 != 0) accelerates Task.velX by the level's
 *       thresholds gUnk_080CFE3C[level][], a well-timed press gives a boost
 *       (sub_080c3698, capped by gUnk_080CFE3C[level][0]), and holding A
 *       while the record's unk14 is 0 starts a 24-frame penalty
 *       (M37Player.unk08); the tilt and animation frame
 *       Task.frame come from the tables gUnk_080CFEE4/gUnk_080CFEE9/
 *       gUnk_080CFF01, and player 0's effects are variant 2 tasks
 *       (sub_080c2078).
 *   sub_080c3d58   the idle step used before the start and after the finish.
 *   sub_080c3e18   the racer's screen position from the course record and
 *       the scale sub_080c623c, and the computer racers' distance fade
 *       (BlendColors on their palette row gUnk_08609D42[pal]).
 *   sub_080c3648 / sub_080c3670 / sub_080c37b8   the speed floor 0x18000,
 *       the camera clamp (scroll = min(pos, finish line), the script cursor's
 *       unk2/unk4 into the course record), and player 0's engine sound
 *       (song 400, pitch from the speed through m4aMPlayPitchControl). */

/* 16-byte object records, M37ObjSet.unk04[7] (sub_080c4664, sub_080c4790) */
struct M37Obj
{
    /*0x00*/ s16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ s32 unk4;
    /*0x08*/ s32 unk8;          /* 16.16; the ROM also reads its high half */
    /*0x0C*/ s32 unkC;          /* 16.16; the ROM also reads its high half */
};

/* M37Game.unk0EC (0xB8 bytes with the compiler's 2-byte tail pad): task
   type #96 variant 1's seven scrolling objects.  sub_080c4664 addresses an
   object as &set->unk04[i] off the set's own base (`lsls #4; adds #4`), so
   the records are a sub-struct, not flat fields; the two 16-colour rows
   AirGrindSetupRace fills end it exactly at M37Game.randomStates. */
struct M37ObjSet
{
    /*0x00*/ s32 unk00;         /* the scroll position last frame */
    /*0x04*/ struct M37Obj unk04[7];
    /*0x74*/ s16 unk74;         /* the last object's sprite id */
    /*0x76*/ u16 unk76[16];     /* M37Game + 0x162 */
    /*0x96*/ u16 unk96[16];     /* M37Game + 0x182 */
};

/* per-player records, M37Game.players[4] (0x34 bytes) */
struct M37Player
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u16 unk02;
    /*0x04*/ u16 unk04;
    /*0x06*/ s16 unk06;
    /*0x08*/ s16 unk08;
    /*0x0A*/ s16 unk0A;
    /*0x0C*/ s16 unk0C;
    /*0x0E*/ u16 unk0E;
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ u8 unk20;
    /*0x21*/ u8 unk21;
    /*0x22*/ u8 pad22[2];
    /*0x24*/ s32 unk24;
    /*0x28*/ s32 unk28;
    /*0x2C*/ s32 unk2C;
    /*0x30*/ s32 unk30;
};

/* gAirGrind, the game's state; always used through gAirGrindPtr */
struct M37Game
{
    /*0x000*/ s32 level;       /* the level (M36's AirGrindInit copies gSubGameLevel) */
    /*0x004*/ s32 raceTimes[4];
    /*0x014*/ s32 unk014;
    /*0x018*/ s32 unk018;
    /*0x01C*/ struct M37Player players[4];
    /*0x0EC*/ struct M37ObjSet unk0EC;
    /*0x1A4*/ s32 randomStates[5];    /* five LCG streams (AirGrindRandom, AirGrindRandomRange) */
    /*0x1B8*/ s32 unk1B8;
    /*0x1BC*/ u16 skyLineColors[160]; /* per-scanline colour, HBlank DMA source */
    /*0x2FC*/ u16 backdropColor;
    /*0x2FE*/ u8 pad2FE[2];
    /*0x300*/ u32 frameCount;       /* frame counter */
    /*0x304*/ s16 unk304;       /* sub_080c4f60's OAM list: entry count */
    /*0x306*/ s16 unk306[160];  /* ... and entries */
    /*0x446*/ u16 localPlayer;       /* gLocalPlayer */
    /*0x448*/ u16 playerCount;       /* gLinkPlayerCount */
    /*0x44A*/ u8 pad44A[2];
    /*0x44C*/ s32 unk44C;       /* a task index into gTasks */
    /*0x450*/ u8 unk450;
    /*0x451*/ u8 unk451;
    /*0x452*/ u8 pad452[2];
};

/* per-player records of gAirGrindCourse, M37Course.players[4] (0x3C bytes) */
struct M37CoursePlayer
{
    /*0x00*/ s32 coursePos;
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ s32 unk20;
    /*0x24*/ s32 unk24;
    /*0x28*/ s32 unk28;
    /*0x2C*/ s32 unk2C;
    /*0x30*/ s32 unk30;
    /*0x34*/ s32 prevCoursePos;
    /*0x38*/ s32 unk38;
};

/* gAirGrindCourse, reached through gAirGrindCoursePtr (and directly by the
   0x080C5284-0x080C623C builder) */
struct M37Course
{
    /*0x000*/ s32 scrollPos;
    /*0x004*/ s32 unk004;
    /*0x008*/ s32 unk008;
    /*0x00C*/ s32 unk00C;
    /*0x010*/ s32 finishLine;
    /*0x014*/ s32 unk014;
    /*0x018*/ struct M37CoursePlayer players[4];
    /*0x108*/ s32 unk108;
    /*0x10C*/ s32 unk10C;
    /*0x110*/ s32 unk110;
};

/* gAirGrindScript: a cursor into one of the u16-pair scripts gUnk_087572EC[]
   (sub_080c51c0 clears it, sub_080c51d4 steps it, 0x8000 = end, 0x9999 =
   loop) */
struct M37Script
{
    /*0x00*/ u8 scriptId;           /* script id, 0 = none */
    /*0x01*/ u8 step;           /* step */
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
};

extern struct M37Game *gAirGrindPtr;
extern struct M37Course *gAirGrindCoursePtr;
extern u16 gAirGrindFrame;
extern struct M37Script gAirGrindScript;
extern s32 gUnk_080CFE3C[][3];
extern u8 gUnk_080CFEE4[];
extern u8 gUnk_080CFEE9[];
extern s8 gUnk_080CFF01[];
extern u8 gUnk_080CFE2C[][4];
extern u16 gUnk_08609D42[][16];
extern u16 gUnk_080CFF1C[];
extern u16 gObjPalette[];

void m4aMPlayPitchControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, s16 pitch);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlaySfx(s32 id);
void StopSfxOnPlayer(s32 player, s32 songId);

void sub_080c2078(s32 a, s32 b, s32 c);
void sub_080c523c(u16 id);
void AirGrindDrawCourse(void);
s32 sub_080c623c(s32 x);

void sub_080c3648(void)
{
    struct Task *t = gCurTask;

    t->accelX = -0x3800;
    if (t->velX < 0x18000) {
        t->accelX = 0;
        t->velX = 0x18000;
    }
}

void sub_080c3670(s32 pos)
{
    struct M37Course *c = gAirGrindCoursePtr;

    c->scrollPos = pos > c->finishLine ? c->finishLine : pos;
    c->unk004 = gAirGrindScript.unk4;
    c->unk008 = gAirGrindScript.unk2;
}

s32 sub_080c3698(s32 player)
{
    s32 speed = gCurTask->velX;
    s32 ret = 0;

    if (gAirGrindCoursePtr->players[player].coursePos > gAirGrindCoursePtr->finishLine - 50)
        return 0;
    if (abs(gAirGrindPtr->players[player].unk10) <= abs((speed * 3) >> 16)) {
        if (abs(gAirGrindPtr->players[player].unk10) <= abs(speed >> 16)) {
            if (player == 0)
                sub_080c2078(0, gCurTask->unk18, 6);
            gAirGrindPtr->players[player].unk01++;
            gCurTask->velX += 0x10000;
            if (gCurTask->velX > gUnk_080CFE3C[gAirGrindPtr->level][0])
                gCurTask->velX = gUnk_080CFE3C[gAirGrindPtr->level][0];
            ret = 2;
        } else {
            if (player == 0)
                sub_080c2078(0, gCurTask->unk18, 7);
            gCurTask->velX += 0x8000;
            if (gCurTask->velX > gUnk_080CFE3C[gAirGrindPtr->level][0])
                gCurTask->velX = gUnk_080CFE3C[gAirGrindPtr->level][0];
            ret = 1;
        }
        gAirGrindPtr->players[player].unk0A = 5;
    }
    return ret;
}

void sub_080c37b8(s32 a, s32 on)
{
    if (a == 0) {
        if (on) {
            if (gAirGrindPtr->unk014 == -1)
                gAirGrindPtr->unk014 = PlaySfx(400);
            else
                m4aMPlayPitchControl(gMPlayTable[gAirGrindPtr->unk014].info, 0xFFFF,
                                     (gCurTask->velX - 0x40000) >> 5);
        } else {
            if (gAirGrindPtr->unk014 != -1) {
                StopSfxOnPlayer(gAirGrindPtr->unk014, 400);
                gAirGrindPtr->unk014 = -1;
            }
        }
    }
}

void sub_080c383c(s32 player)
{
    s32 max;
    s32 i;
    u16 rank;

    if (player == 0) {
        sub_080c3670((gCurTask->posX >> 16) + 48);
        AirGrindDrawCourse();
        max = 0;
        for (i = 0; i < 4; i++) {
            if (gAirGrindCoursePtr->players[i].coursePos > max)
                max = gAirGrindCoursePtr->players[i].coursePos;
        }
        gAirGrindPtr->unk1B8 = max;
    }
    rank = (u16)gAirGrindCoursePtr->players[player].unk18;
    gCurTask->layer = rank + 4;
    if (rank > 2)
        rank = 2;
    gCurTask->spriteFlags = (rank << 13) & 0x6000;
    gCurTask->unk28 = gAirGrindCoursePtr->players[player].unk08;
}

void sub_080c38c8(s32 player)
{
    s32 ret;

    if (gAirGrindPtr->players[player].unk08 != 0) {
        sub_080c3648();
        if (player == 0 && gAirGrindPtr->players[0].unk08 > 19)
            sub_080c2078(0, gCurTask->unk18, (gAirGrindPtr->players[0].unk08 & 1) ? 3 : 4);
    } else if (gAirGrindPtr->players[player].unk02 & 1) {
        if (gAirGrindCoursePtr->players[player].unk14 != 0) {
            ret = 0;
            sub_080c37b8(player, 1);
            if (gCurTask->velX > gUnk_080CFE3C[gAirGrindPtr->level][1])
                gCurTask->accelX = 0x200;
            else if (gCurTask->velX > gUnk_080CFE3C[gAirGrindPtr->level][2])
                gCurTask->accelX = 0xA00;
            else
                gCurTask->accelX = 0x8000;
            if (player == 0) {
                if ((gAirGrindFrame & 3) == 0)
                    sub_080c2078(0, gCurTask->unk18, 0);
                if ((gAirGrindFrame & 7) == 1)
                    sub_080c2078(0, gCurTask->unk18, 1);
            }
            if (gAirGrindPtr->players[player].unk0A == 0 && gAirGrindPtr->players[player].unk00 == 0)
                ret = sub_080c3698(player);
            if (gAirGrindPtr->players[player].unk04 & 1) {
                if (player == 0 && gAirGrindPtr->unk450 == 0)
                    sub_080c2078(0, gCurTask->unk18, 2);
                gAirGrindPtr->players[player].unk0C = 4;
                if (ret == 1) {
                    gAirGrindPtr->players[player].unk0C = 6;
                    if (player == 0)
                        PlaySfx(149);
                } else if (ret == 2) {
                    gAirGrindPtr->players[player].unk0C = 8;
                    if (player == 0)
                        PlaySfx(149);
                }
            }
            gAirGrindPtr->players[player].unk00 = 1;
        } else {
            gAirGrindPtr->players[player].unk0C = 0;
            sub_080c37b8(player, 0);
            sub_080c3648();
            gAirGrindPtr->players[player].unk08 = 24;
            if (player == 0)
                sub_080c523c(3);
            sub_080c2078(player, gCurTask->unk18, 8);
            gAirGrindPtr->players[player].unk00 = 0;
        }
        gAirGrindPtr->players[player].unk14 -= 0x20000;
        if (gAirGrindPtr->players[player].unk14 < 0)
            gAirGrindPtr->players[player].unk14 = 0;
    } else {
        gAirGrindPtr->players[player].unk0C = 0;
        sub_080c37b8(player, 0);
        if (gCurTask->velX < 0x18000) {
            gCurTask->accelX = 0;
            gCurTask->velX = 0x18000;
        } else {
            gCurTask->accelX = -0x800;
        }
        gAirGrindPtr->players[player].unk14 += 0x20000;
        if (gAirGrindPtr->players[player].unk14 > 0x80000)
            gAirGrindPtr->players[player].unk14 = 0x80000;
        if (gAirGrindPtr->players[player].unk00 == 1) {
            if (player == 0 && gAirGrindPtr->unk451 == 0)
                sub_080c2078(0, gCurTask->unk18, 5);
            if (gAirGrindPtr->players[player].unk0A == 0)
                sub_080c3698(player);
        }
        gAirGrindPtr->players[player].unk00 = 0;
    }

    if (gAirGrindPtr->players[player].unk0C != 0) {
        gAirGrindPtr->players[player].unk18 += 0x20000;
    } else {
        gAirGrindPtr->players[player].unk18 -= 0x20000;
        if (gAirGrindPtr->players[player].unk18 < 0)
            gAirGrindPtr->players[player].unk18 = 0;
    }
    if (gAirGrindPtr->players[player].unk10 < -20)
        gAirGrindPtr->players[player].unk1C = gAirGrindCoursePtr->players[player].unk28;
    gAirGrindPtr->players[player].unk10 = gAirGrindPtr->players[player].unk1C - gAirGrindCoursePtr->players[player].coursePos;
    if (gAirGrindPtr->players[player].unk08 == 0) {
        if (gCurTask->frame > 8)
            gAirGrindPtr->players[player].unk06 = 0;
        if (gAirGrindCoursePtr->players[player].unk1C > 16)
            gCurTask->frame = 0;
        else if (gAirGrindCoursePtr->players[player].unk1C < -16)
            gCurTask->frame = 6;
        else
            gCurTask->frame = 3;
        if (gAirGrindPtr->players[player].unk02 & 1) {
            if (gAirGrindPtr->players[player].unk06 > 0)
                gAirGrindPtr->players[player].unk06--;
        } else {
            if (gAirGrindPtr->players[player].unk06 <= 3)
                gAirGrindPtr->players[player].unk06++;
        }
        gCurTask->frame += gUnk_080CFEE4[gAirGrindPtr->players[player].unk06];
    } else {
        if (gCurTask->frame <= 8)
            gAirGrindPtr->players[player].unk06 = 0;
        if (player == 0 && gAirGrindPtr->players[0].unk06 == 0)
            PlaySfx(401);
        gCurTask->frame = 9;
        if (++gAirGrindPtr->players[player].unk06 > 23)
            gAirGrindPtr->players[player].unk06 = 0;
        gCurTask->frame += gUnk_080CFEE9[gAirGrindPtr->players[player].unk06];
        gAirGrindPtr->players[player].unk14 = gUnk_080CFF01[gAirGrindPtr->players[player].unk06] << 16;
    }
}

void sub_080c3d58(s32 player)
{
    sub_080c37b8(player, 0);
    if (gCurTask->state == 0) {
        gAirGrindPtr->players[player].unk14 -= 0x20000;
        if (gAirGrindPtr->players[player].unk14 < 0)
            gAirGrindPtr->players[player].unk14 = 0;
    }
    if (gCurTask->frame > 8)
        gAirGrindPtr->players[player].unk06 = 0;
    if (gAirGrindCoursePtr->players[player].unk1C > 16)
        gCurTask->frame = 0;
    else if (gAirGrindCoursePtr->players[player].unk1C < -16)
        gCurTask->frame = 6;
    else
        gCurTask->frame = 3;
    if (gCurTask->state == 0)
        gAirGrindPtr->players[player].unk06 = 3;
    gCurTask->frame += gUnk_080CFEE4[gAirGrindPtr->players[player].unk06];
}

void sub_080c3e18(s32 player)
{
    s32 scale;
    u8 pal;
    u16 ratio;

    scale = sub_080c623c(gAirGrindCoursePtr->players[player].unk08);
    gCurTask->pixelX = gAirGrindCoursePtr->players[player].unk0C + ((scale * gAirGrindPtr->players[player].unk18) >> 24);
    gCurTask->pixelY = gAirGrindCoursePtr->players[player].unk10 - ((scale * gAirGrindPtr->players[player].unk14) >> 24);
    if (player != 0) {
        pal = gUnk_080CFE2C[gAirGrindPtr->localPlayer][player];
        ratio = 0;
        if (scale < 256)
            ratio = 256 - scale;
        /* colour 1 of palette pal: the ROM scales the whole index
           (pal * 16 + 1) by 2, so the offset is written in bytes */
        BlendColors(gUnk_08609D42[pal], gUnk_080CFF1C, ratio, 15,
                     (u16 *)((u8 *)gObjPalette + (pal * 16 + 1) * 2));
    }
}

void sub_080c3efc(void)
{
    s32 player = gCurTask->unk1C;

    sub_080c383c(player);
    sub_080c38c8(player);
    sub_080c3e18(player);
}

void sub_080c3f20(void)
{
    s32 player = gCurTask->unk1C;

    sub_080c383c(player);
    sub_080c3d58(player);
    sub_080c3e18(player);
}
