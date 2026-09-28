#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "hud.h"
#include "collision.h"
#include "player.h"
#include "effect.h"
#include "actor.h"
#include "enemy.h"
#include "save.h"

/* RAM cells / ROM tables */
/* Not from room.h: this file's view of gUnk_02007D64 differs (lesson 3.517). */
extern s16 gMaxHealth;
extern s16 gPlayerHealth[];
extern s8 gUnk_02005590[];
struct Unk020055D8Entry
{
    /*0x00*/ s8 kind;
    /*0x01*/ s8 unk1;
    /*0x02*/ s8 unk2;
    /*0x03*/ s8 unk3;
    /*0x04*/ u16 x;
    /*0x06*/ u16 y;
};
struct Unk020055D8
{
    /*0x00*/ s16 count;
    /*0x02*/ s16 sortedByY;
    /*0x04*/ struct Unk020055D8Entry *entries;
};
extern struct Unk020055D8 gRoomObjectList;
extern u8 gUnk_02005E10[];
extern u8 gUnk_020069F0;
extern u32 gUnk_02007BF0[8][8];
extern s16 gPlayerLives[];
extern u16 gUnk_02007D60;
extern u32 gUnk_02007D64[];
extern s16 gUnk_0200AF0C;
extern u8 gUnk_0200B04C;
extern u8 gUnk_0200B078;
extern u8 gUnk_0200D080;
extern s16 gCameraAnchorY;
extern s32 gUnk_03001F2C;
extern u8 gMetaKnightmareMode;
extern s16 gViewRect[];
extern u32 gUnk_03002160;
extern u8 gActivePlayerMask;
extern s32 gUnk_03002344;
extern u8 gActivePlayerCount;
extern s8 gLevelIndex;
extern s16 gCameraAnchorX;
extern u32 gBigSwitchFlags[];
extern u16 gGameState;
extern s32 gCurSaveSlot;
extern u32 gStageIndex[];
extern s32 gUnk_03002448;
extern u8 gExtraMode;
extern s8 gRoomIndex;
extern s16 gRoomBounds[];
extern struct Unk03005680 gScrollLock;

/* External functions */
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
extern s32 PlaySfx(s32 id);
extern u32 TaskIsOnScreen(void);
extern void TaskSetEntry(void *a, u32 i);
extern void HudStartHpBar();
extern s32 GetCollisionTileAtOffset(s16 x, s16 y, s32 c, s32 d);
extern void ExitClearedStage();
extern void sub_08025a30();
extern void sub_08025acc();
extern void sub_08025b5c();
extern void RequestScreenShake(u32 a);
extern void sub_080275cc();
extern void StartScrollLock();
extern s32 CreateMapEvent();
extern void sub_0802ffe8();
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void TaskBreakBlocksNoPlayer();
extern void sub_08030db8();
extern void ActorLoadDef(struct ActorDef *d);
extern void ActorSetState();
extern void ActorSetStateSlot(u32 i, u16 v);
extern void ActorSetHitReactions(u32 v);
extern void ActorSetAttackBox(u32 v);
extern void sub_080639f0(struct ActorAux *v);
extern void sub_08063a00(u32 v);
extern s32 TaskGetDxTo(u32 i);
extern s32 TaskIsInRectSlot(struct Rect *r, u32 i);
extern s32 ActorStartAnimNoFlip(struct AnimCmd *p);
extern void AngleToVector(s16 t, s16 mag);
extern u16 TaskGetAngleToNearestPlayer(s32 prec);
extern s16 ActorComputeHealth(void);
extern s32 CreateInhalableStar(s16 x, s16 y, s16 dir, u8 p8);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u32 ActorCheckHits(void);
extern u32 sub_08068f68(void);
extern u32 ActorCollideTerrain(void);
extern u32 sub_0806951c(void);
extern u32 sub_08069888(void);
extern u32 ActorReactToHit(void);

/* Module functions */
void sub_080a2b2c();
void sub_080b54a4();
s32 sub_080b5670();
s32 sub_080b5840();
s32 sub_080b590c();
void sub_080b59d8();
s32 sub_080b5a94();
s32 sub_080b5bdc();
s32 sub_080b5d84();

void sub_080b5024(void)
{
    struct Unk020055D8Entry *e;
    struct Unk0873EEA0 *d;
    s32 i;
    s32 r;

    if (gRoomObjectList.count == 0)
        return;
    e = gRoomObjectList.entries;
    for (i = 0; i < gRoomObjectList.count; e++, i++)
    {
        if (e->kind == -1)
            return;
        r = -1;
        switch (e->kind)
        {
        case 0:
        case 1:
        case 2:
        case 4:
        case 6:
        case 7:
            break;
        case 3:
            r = CreateActorByKind(2, e->unk1, e->unk2, e->unk3, e->x, e->y,
                             (gUnk_020060A0[gUnk_02006130[i]].unk1 << 12) | ((gUnk_020060A0[gUnk_02006130[i]].unk2 * 2) + 16));
            gUnk_02008014[0] = r;
            gUnk_020055D0 = gUnk_02000034 = 1;
            break;
        case 8:
            r = CreateActorByKind(3, 8, e->unk2, e->unk3, e->x, e->y,
                             (gUnk_020060A0[gUnk_02006130[i]].unk1 << 12) | ((gUnk_020060A0[gUnk_02006130[i]].unk2 * 2) + 16));
            gUnk_02008014[0] = r;
            gUnk_020055D0 = gUnk_02000034 = 1;
            break;
        case 5:
            if (gUnk_08756178[e->unk1] == -1)
                continue;
            d = gUnk_0873F180[gUnk_08756178[e->unk1]];
            gUnk_03001F2C = 0;
            switch (e->unk1)
            {
            case 4:
                if (gUnk_0200B078 == 3)
                {
                    if (!((gUnk_0200B04C >> (e->unk3 - 1)) & 1))
                        continue;
                    sub_0802ffe8(e->unk3 - 1, e->x, e->y);
                }
                else
                {
                    if (gUnk_087324DA[e->unk2][0] != 0xFFFF)
                        gRoomBounds[0] = gUnk_087324DA[e->unk2][0];
                    if (gUnk_087324DA[e->unk2][1] != 0xFFFF)
                        gRoomBounds[1] = gUnk_087324DA[e->unk2][1];
                    if (gUnk_087324DA[e->unk2][2] != 0xFFFF)
                        gRoomBounds[2] = gUnk_087324DA[e->unk2][2];
                    if (gUnk_087324DA[e->unk2][3] != 0xFFFF)
                        gRoomBounds[3] = gUnk_087324DA[e->unk2][3];
                }
                break;
            case 1:
                if (gBigSwitchFlags[0] & (1 << e->unk2))
                    continue;
                break;
            case 8:
                gUnk_03001F2C = e->unk2;
                break;
            case 2:
                if (e->unk2 == 1)
                    sub_08065dfc(gUnk_020060A0[gUnk_02006130[i]].unk1);
                break;
            case 3:
            }
            if (d != NULL)
                r = CreateActorByKind(5, gUnk_08756178[e->unk1], gUnk_03001F2C, 0, e->x, e->y,
                                 (gUnk_020060A0[gUnk_02006130[i]].unk1 << 12) | ((gUnk_020060A0[gUnk_02006130[i]].unk2 * 2) + 16));
            else
                r = CreateActorByKind(5, gUnk_08756178[e->unk1], gUnk_03001F2C, 0, e->x, e->y, 0);
            break;
        }
        if (r != -1)
            gUnk_02005590[r - 32] = i;
    }
}

s32 sub_080b5338(s32 i)
{
    s32 i6;
    s32 res5;
    u8 *e4;
    u8 *b;
    u8 *pw2;
    s32 w8;
    u32 w9;
    s32 vb;
    u8 *pw;
    u8 *pd;
    u8 *pz;
    u32 *pw3;
    s32 m;
    u8 *e2;
    s32 w3;
    s32 w4;
    s32 r;

    i6 = i;
    res5 = -1;
    if (((u8 *)gUnk_02008020)[i6] != 0)
        goto fail;
    pd = (u8 *)&gRoomObjectList;
    w8 = i6 << 3;
    w9 = (u32)*(u8 **)(pd + 4);
    e4 = (u8 *)(w9 + w8);
    switch (*(s8 *)e4)
    {
    case 0:
    case 3:
    case 5:
    case 7:
    case 8:
        break;
    case 1:
        if (gActivePlayerCount <= (s8)e4[3] >> 5)
            goto fail;
        res5 = sub_080b5bdc(e4, i6);
        break;
    case 2:
        gUnk_0200D080 = 1;
        res5 = CreateActorByKind(1, *(s8 *)(e4 + 1), e4[2], e4[3], *(u16 *)(e4 + 4), *(u16 *)(e4 + 6),
            (pw2 = (u8 *)gUnk_020060A0, (((s32)*(s8 *)((b = (u8 *)((u32)pw2 + ((s32)(s8)*((u8 *)gUnk_02006130 + i6) << 2))) + 1) << 12) | ((*(s16 *)(b + 2) << 1) + 16))));
        break;
    case 4:
        if (*(s8 *)(e4 + 1) == 5)
        {
            sub_0806704c();
            break;
        }
        pw = (u8 *)gUnk_02007BF0;
        pw3 = (u32 *)(((s32)*(s8 *)gStageIndex << 2) + ((s32)gLevelIndex << 5) + (u32)pw);
        m = 1 << *(s8 *)(e4 + 3);
        w3 = *pw3;
        w3 &= m;
        if (w3 != 0)
            goto fail;
        res5 = CreateActorByKind(6, *(s8 *)(e4 + 1), e4[2], 0, *(u16 *)(e4 + 4), *(u16 *)(e4 + 6), w3);
        break;
    case 6:
        if (*(s8 *)(e4 + 1) != 0)
            break;
        res5 = sub_080b5d84(e4);
        break;
    }
    pz = (u8 *)gUnk_02008020 + i6;
    *pz = 1;
    if (res5 != -1)
        goto ok;
fail:
    r = 0;
    goto out;
ok:
    e2 = (u8 *)gUnk_02005590;
    w4 = res5;
    w4 -= 32;
    *(u8 *)(w4 + (u32)e2) = i6;
    r = 1;
out:
    return r;
}

void sub_080b54a4(s32 a)
{
    if (gUnk_02005590[a - 32] != -1)
    {
        gUnk_02008020[gUnk_02005590[a - 32]] = 0;
        gUnk_02005590[a - 32] = -1;
    }
}

void sub_080b54d0(s32 a)
{
    s32 av;
    u8 *pbase;
    u8 *pd;
    u8 *pb2;
    s32 wi;
    s8 *e3;
    u8 *p2;
    s32 w;
    s32 k;
    s32 w9;
    u8 **wdp;
    u8 *wd;
    s32 m;

    av = a;
    if ((s8)*(u8 *)gUnk_02007D64 == 4)
        return;
    pbase = (u8 *)gUnk_02005590;
    w = av;
    w -= 32;
    e3 = (s8 *)(w + (u32)pbase);
    if (*e3 == -1)
        return;
    pb2 = (u8 *)gUnk_02007BF0;
    p2 = (u8 *)((((s32)*(s8 *)gStageIndex << 2) + ((s32)gLevelIndex << 5)) + (u32)pb2);
    pd = (u8 *)&gRoomObjectList;
    wi = *e3;
    wd = *(u8 **)(pd + 4);
    w9 = *(u8 *)(((wi << 3) + (u32)wd) + 3);
    k = (s8)w9;
    m = 1 << k;
    *(u32 *)p2 = *(u32 *)p2 | m;
}

void sub_080b5540(s32 a, s32 b)
{
    u8 *pbase;
    u8 *pa;
    u8 *pb;

    pbase = (u8 *)gUnk_02005590;
    b -= 32;
    pb = (u8 *)(b + (u32)pbase);
    a -= 32;
    pa = (u8 *)(a + (u32)pbase);
    *pb = *pa;
    *pa = 255;
}

void sub_080b5558(void)
{
    u16 mask;
    s32 i;

    mask = 0;
    CpuSet(gUnk_03001570, gUnk_02005E10, 96);
    for (i = 0; i < 10 && gUnk_020060A0[i].unk0 != -1; i++)
    {
        if (!((mask >> gUnk_020060A0[i].unk1) & 1))
        {
            sub_08065dd0(gUnk_020060A0[i].unk1, gUnk_020060A0[i].unk0);
            mask |= 1 << i;
        }
    }
    CpuSet(gUnk_03001570, gUnk_02005F10, 96);
}

s32 sub_080b55d8(u32 a, u32 b)
{
    s32 r;
    u32 n;

    if (a + gUnk_0200000C > 512)
        gUnk_0200000C = 0;
    if (b + gUnk_02007D40 > 14)
        gUnk_02007D40 = 8;
    /* stand-in: the volatile read keeps cse from reusing this load for the
       `+= b` below (the ROM loads the palette cursor twice) */
    r = (gUnk_0200000C << 16) | *(vu16 *)&gUnk_02007D40;
    n = gUnk_0200000C + a;
    gUnk_02007D40 += b;
    n &= 0xFFF0;
    n += 16;
    gUnk_0200000C = n;
    return r;
}

s32 sub_080b5628(u32 a)
{
    s32 r;
    u32 n;

    if (a + gUnk_0200000C > 512)
        gUnk_0200000C = 0;
    r = gUnk_0200000C;
    n = ((r + a) & 0xFFF0) + 16;
    gUnk_0200000C = n;
    return r;
}

s32 sub_080b5654(u32 a)
{
    s32 r;

    if (a + gUnk_02007D40 > 14)
        gUnk_02007D40 = 8;
    r = gUnk_02007D40;
    gUnk_02007D40 = r + a;
    return r;
}
