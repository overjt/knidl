
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s32 gUnk_02006190[];
extern s32 gUnk_02007D00[];
extern s16 gViewRect[];
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern vu16 gTaskSlotTypes[];
extern s16 gUnk_03005628[];
extern u32 gUnk_082D8638[];
extern u16 * gUnk_087482A8[];
extern s16 gUnk_08748374[];
extern struct AnimCmd gUnk_08748384[];
extern struct AnimCmd gUnk_08748398[];
extern s32 gUnk_087483B8[];
extern s32 gUnk_08748410[];
extern u16 gUnk_08748420[];
extern s16 gUnk_08748430[];
extern u32 gUnk_0874844C[];
extern u32 gUnk_08748478[];
extern u32 gUnk_087484C4[];
extern u32 gUnk_087484CC[];
extern u32 gUnk_08748820[];
extern u32 gUnk_087538E0[];

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern s32 PlaySfx(s32 id);
extern void StopSfxOnPlayer(s32 player, s32 songId);
extern void TaskFree(s32 id);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, s32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskStopX(void);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStopY(void);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void TaskStepForward(s16 a);
extern void sub_080261d4(s32 a);
extern void ActorSetState(u16 v);
extern void sub_08063a00(u32 v);
extern s32 TaskGetNearestPlayerDx(void);
extern void TaskGetNearestPlayerPos(void);
extern void TaskFaceNearestPlayer(void);
extern s32 ActorStartAnim(struct AnimCmd *p);
extern s32 ActorTickAnimFacingNearestPlayer(s32 n);
extern u8 TaskGetYDirBitToNearestPlayer(void);
extern u8 TaskGetXDirBitToNearestPlayer(void);
extern s32 TaskGetNearestPlayerScreenPos(void);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateChildTaskAtOffsetFacing(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);
extern void sub_08065438(void);
extern void ActorFlashPalette(void *src, u32 size);
extern void sub_08066468(void);
extern void sub_080664e0(struct AnimCmd *p);
extern void sub_080666f8(struct AnimCmd *p);
extern s32 sub_08067120(s16 x, s16 y, s16 dir, u8 p8);
extern u8 ActorCollideTerrain(void);
extern void sub_0806d08c(s16 a, s16 b, s16 c);
extern s32 sub_0806d1e8(s16 a, s16 b);
extern void sub_0809fca4(void);
extern void sub_0809fd20(void);
extern void sub_080a0028(void);
extern void sub_080a0098(void);
extern void sub_080a00ec(void);

/* Defined below */
void sub_080a0a38(void);
void sub_080a0b10(void);
void sub_080a1550(void);

void sub_080a0274(void)
{
    CreateChildTaskAtOffsetFacing(178, -24, -8, 0);
}

void sub_080a028c(void)
{
    struct ActorSpawn sp;
    struct Actor *a;

    a = gCurTask->unk8C;
    sp.unk00 = 11;
    sp.unk04 = 113;
    sp.unk08 = 0;
    sp.unk09 = 0;
    sp.unk0C = 32;
    sp.unk0E = 16;
    sp.unk10 = a->unk20;
    sp.unk0A = 0;
    gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&sp, 1);
}

void sub_080a02d4(u8 a)
{
    struct Task *t;
    u16 x;
    u16 y;
    s16 yy;
    s32 d;

    t = gCurTask;
    y = t->unk4A + 24;
    if (a == 1)
    {
        x = t->unk48 + 32;
        d = 1;
    }
    else
    {
        x = t->unk48 + (t->unk43 << 5);
        d = 0;
    }
    yy = y;
    sub_08067120((s16)x, yy, d, 0);
    if (a == 1)
        sub_08067120(gCurTask->unk48 - 32, yy, -1, 0);
}

void sub_080a0358(void)
{
    struct Task *t;
    s32 n;
    s32 r;

    t = gCurTask;
    switch (t->unk1C)
    {
    case 3:
        n = 46;
        break;
    case 7:
        r = CreateChildTaskHere(182, 0);
        t = gCurTask;
        t->unk46 = r;
        n = 52;
        break;
    case 8:
        TaskGetNearestPlayerPos();
        gCurTask->unk20 = gUnk_030023B4;
        if (gUnk_030023B4 < gUnk_03005628[0] - 87)
            gCurTask->unk20 = gUnk_03005628[0] - 87;
        if (gCurTask->unk20 > gUnk_03005628[1] + 87)
            gCurTask->unk20 = gUnk_03005628[1] + 87;
        if (gCurTask->unk48 == gUnk_030023B4)
            goto tail;
        if ((u8)TaskGetXDirBitToNearestPlayer() == 4)
        {
            t = gCurTask;
            t->unk20 -= 16;
        }
        else
        {
            t = gCurTask;
            t->unk20 += 16;
        }
        t = gCurTask;
        n = 8;
        break;
    default:
        t = gCurTask;
        n = 0;
        break;
    }
    t->unk34 = n;
tail:
    if (gCurTask->unk20 < gUnk_03005628[0] - 87)
        gCurTask->unk20 = gUnk_03005628[0] - 87;
    if (gCurTask->unk20 > gUnk_03005628[1] + 87)
        gCurTask->unk20 = gUnk_03005628[1] + 87;
}

void sub_080a043c(void)
{
    struct Task *t;
    s32 n;
    s32 one;

    TaskGetNearestPlayerScreenPos();
    t = gCurTask;
    one = 1;
    t->unk20 = one;
    if (gUnk_030023D4 <= 111)
    {
        t->unk24 = 2;
    }
    else
    {
        n = (t->unk30 + 1) & 3;
        t->unk30 = n;
        if (n == 0)
        {
            t->unk24 = one;
            t->unk20 = 4;
        }
        else
        {
            t->unk24 = 0;
        }
    }
}

void sub_080a0480(u8 a)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    u16 f;

    if (a == 0)
    {
        if (abs(TaskGetNearestPlayerDx()) <= 21)
            gCurTask->unk24 = 0;
        else
            gCurTask->unk24 = 1;
    }
    else
    {
        t = gCurTask;
        f = t->unk43;
        TaskFaceNearestPlayer();
        u = gCurTask;
        if ((s16)f != u->unk43)
            u->unk24 = 0;
        else
            u->unk24 = 1;
    }
    v = gCurTask;
    if (v->unk24 == 1)
    {
        if (v->unk78 < (s16)v->unk70)
            TaskSetMotionXFacing(136 << 10, 0x5A5A5A5A);
        else
            TaskSetMotionXFacing(160 << 9, 0x5A5A5A5A);
    }
}

u8 sub_080a0538(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    switch (t->unk14)
    {
    case 3:
        TaskStopY();
        break;
    case 4:
        t->unk24 = 4;
        t->unk7A = 0;
        sub_0806d08c(24, 8, 32);
        TaskSetFrame(23);
        u = gCurTask;
        TaskSetMotionY(-u->unk58, -u->unk60, u->unk68);
        break;
    }
    return 0;
}

void sub_080a0588(void)
{
    gCurTask->unk54 = 0;
}

u8 sub_080a0598(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk14 == 4)
    {
        t->unk24 = 4;
        TaskSetFrame(23);
        u = gCurTask;
        TaskSetMotionY(-u->unk58, -u->unk60, u->unk68);
    }
    return 0;
}

void sub_080a05c8(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk34 = 0;
    t->unk30 = 0;
    t->unk2C = 0;
    t->unk28 = 0;
    t->unk24 = 0;
    t->unk20 = 0;
    t->unk1C = 0;
    t->unk18 = 0;
    t->unk46 = 0xFFFF;
    gUnk_02007D00[8] = -1;
    t->unk46 = -1;
    TaskFaceNearestPlayer();
    ActorSetState(0);
}

u8 sub_080a060c(void)
{
    struct Task *t;
    s16 dx;
    u16 dy;
    s16 y;

    t = gCurTask;
    dx = t->unk48 - gViewRect[0];
    dy = t->unk4A - gViewRect[2];
    if ((u16)(dx + 19) <= 278)
    {
        y = dy;
        if (y > -20)
        {
            if (y <= 179)
                return 1;
        }
    }
    return 0;
}

void sub_080a0658(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 i;
    s32 n;
    s32 n2;
    s32 n3;

    if (sub_080a060c() != 0)
    {
        i = RandomRange(3);
        n = ActorStartAnim(gUnk_08748384);
        t = gCurTask;
        t->unk24 = n;
        if (t->unk78 < (s16)t->unk70)
        {
            i += 3;
            n2 = ActorStartAnim(gUnk_08748398);
            gCurTask->unk24 = n2;
        }
        u = gCurTask;
        if (u->unk2C != 0)
            u->unk20 = 0;
        else
            u->unk20 = gUnk_08748374[i];
        gCurTask->unk2C = 0;
    }
    else
    {
        n3 = ActorStartAnim(gUnk_08748398);
        v = gCurTask;
        v->unk24 = n3;
        v->unk20 = 1;
    }
}

void sub_080a06f0(void)
{
    struct Task *t;
    struct Actor *a;
    u16 *tab;
    u16 *p;
    s32 q;
    s32 i;
    s32 n;

    t = gCurTask;
    a = t->unk8C;
    n = t->unk18 + 1;
    t->unk18 = n;
    if (n == 6)
    {
        t->unk18 = 0;
        ActorSetState(4);
        TaskSetEntry(sub_080a0b10, gCurTaskIdx);
    }
    else
    {
        tab = gUnk_087482A8[a->unk1C];
        i = RandomRange(8);
        q = i * 4 + (s32)tab;
        ActorSetState(*(u16 *)q);
        gCurTask->unk1C = *(u16 *)(q + 2);
        TaskSetEntry(sub_080a0b10, gCurTaskIdx);
    }
}

void sub_080a0768(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    TaskSetFrame(37);
    TaskYieldTrampoline(24);
    gCurTask->unk7A = 0;
    t = gCurTask;
    t->unk3C++;
    PlaySfx(0x21E);
    TaskSetMotionY(0xFFFB0000, 160 << 6, 160 << 11);
    while (gCurTask->unk58 < 0)
        TaskYieldTrampoline(1);
    TaskStop();
    PlaySfx(136 << 2);
    TaskSetFrame(36);
    TaskYieldTrampoline(4);
    u = gCurTask;
    u->unk3C--;
    TaskYieldTrampoline(1);
    v = gCurTask;
    v->unk3C--;
    v->unk60 = 160 << 6;
    v->unk68 = 160 << 11;
    while ((gCurTask->unk7A & 1) == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    PlaySfx(0x1F7);
    sub_080a02d4(0);
    sub_080261d4(4);
    TaskYieldTrampoline(20);
}

void sub_080a0844(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 i;
    u16 a;
    u16 b;
    u16 c;
    u16 d;
    s32 z;

    i = (u16)(gCurTask->unk24 * 3);
    a = gUnk_08748420[i];
    b = gUnk_08748420[i + 1];
    c = gUnk_08748420[i + 2];
    d = gUnk_08748420[i + 3];
    TaskSetFrame(39);
    TaskYieldTrampoline(24);
    PlaySfx(136 << 2);
    TaskSetFrame(36);
    TaskYieldTrampoline(a);
    t = gCurTask;
    t->unk3C--;
    z = 0;
    TaskYieldTrampoline(b);
    sub_080261d4(d);
    sub_080a02d4(0);
    u = gCurTask;
    u->unk3C--;
    TaskYieldTrampoline(c);
    v = gCurTask;
    v->unk20--;
    v->unk6C = z;
    while ((s16)gCurTask->unk6C < gCurTask->unk20)
    {
        w = gCurTask;
        w->unk3C--;
        TaskYieldTrampoline(6);
        PlaySfx(136 << 2);
        gCurTask->unk3C = 36;
        TaskYieldTrampoline(a);
        gCurTask->unk3C--;
        TaskYieldTrampoline(b);
        gCurTask->unk3C--;
        TaskYieldTrampoline(c);
        gCurTask->unk6C++;
    }
}

void sub_080a094c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 anim;

    t = gCurTask;
    anim = (t->unk1C == 7) ? 30 : 13;
    while (1)
    {
        gCurTask->unk7A = 0;
        TaskYieldTrampoline(1);
        TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
        gCurTask->unk58 = 0;
        PlaySfx(0x201);
        TaskSetFrame((s16)anim);
        TaskYieldTrampoline(4);
        gCurTask->unk7A = 0;
        u = gCurTask;
        u->unk58 = 0xFFFF8000;
        u->unk3C++;
        TaskYieldTrampoline(10);
        gCurTask->unk58 = 128 << 8;
        TaskYieldTrampoline(10);
        gCurTask->unk58 = 0;
        PlaySfx(0x201);
        v = gCurTask;
        v->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk7A = 0;
        w = gCurTask;
        w->unk58 = 0xFFFF8000;
        w->unk3C++;
        TaskYieldTrampoline(10);
        gCurTask->unk58 = 128 << 8;
        TaskYieldTrampoline(10);
    }
}

void sub_080a0a0c(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk04 = (u32)sub_080a0a38;
    sub_080a05c8();
    CallTableEntry(gCurTask->unk14, 11, gUnk_0874844C);
}

void sub_080a0a38(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->unk15, 11, gUnk_08748478);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
        sub_0809fca4();
}

void sub_080a0a84(void)
{
    s32 n;
    s32 m;

    sub_0809fca4();
    ActorFlashPalette(gUnk_082D8638, 16);
    if (gUnk_02006190[5] <= 0)
    {
        n = gUnk_02006190[4];
        if (n <= 11)
        {
            gUnk_02006190[4] = n + 2;
            gUnk_02006190[5] = gUnk_08748430[n + 3];
        }
        else
        {
            gUnk_02006190[4] = 0;
            m = gUnk_02006190[3] - 1;
            gUnk_02006190[3] = m;
            gUnk_02006190[5] = gUnk_08748430[1];
            if (m <= 0)
            {
                sub_0809fd20();
                return;
            }
        }
    }
    gUnk_02006190[5]--;
    gCurTask->unk48 += gUnk_08748430[gUnk_02006190[4]];
}

void sub_080a0b10(void)
{
    TaskFaceNearestPlayer();
    CallTableEntry(gCurTask->unk14, 11, gUnk_0874844C);
}

void sub_080a0b30(void)
{
    struct Task *t;
    s32 z;

    t = gCurTask;
    z = 0;
    t->unk15 = z;
    TaskStop();
    ActorCollideTerrain();
    sub_080666f8(gUnk_08748384);
    sub_080664e0(gUnk_08748384);
    sub_08063a00((u32)gUnk_08748820);
    gCurTask->unk2C = z;
    sub_080a0658();
    TaskSleepForever();
}

void sub_080a0b74(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    t = gCurTask;
    n = ActorTickAnimFacingNearestPlayer(t->unk24);
    u = gCurTask;
    u->unk24 = n;
    n = u->unk20 - 1;
    u->unk20 = n;
    if (n == 0)
    {
        u->unk1C = 3;
        ActorSetState(2);
        TaskSetEntry(sub_080a0b10, gCurTaskIdx);
    }
}

void sub_080a0bb4(void)
{
    struct Task *t;
    s32 one;

    t = gCurTask;
    one = 1;
    t->unk15 = one;
    TaskStop();
    gCurTask->unk7A = one;
    sub_080a0658();
    TaskSleepForever();
}

void sub_080a0bdc(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    t = gCurTask;
    n = ActorTickAnimFacingNearestPlayer(t->unk24);
    u = gCurTask;
    u->unk24 = n;
    if (u->unk20 == 0)
        sub_080a06f0();
    else
        u->unk20--;
}

void sub_080a0c08(void)
{
    gCurTask->unk15 = 2;
    TaskStop();
    sub_080a0358();
    sub_080a094c();
}

void sub_080a0c28(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 v2;
    s32 w;
    s32 f;
    s32 f2;

    t = gCurTask;
    if (t->unk1C == 8)
    {
        v = t->unk20 - t->unk48;
        f = t->unk43;
        if (v < 0)
        {
            if (f != -1)
                goto zero;
        }
        else if (f != 1)
        {
        zero:
            v = 0;
        }
        w = v;
    }
    else
    {
        v2 = TaskGetNearestPlayerDx();
        f2 = gCurTask->unk43;
        if (v2 < 0)
        {
            if (f2 != -1)
                goto zero2;
        }
        else if (f2 != 1)
        {
        zero2:
            v2 = 0;
        }
        w = v2;
    }
    w = abs(w);
    u = gCurTask;
    if (w <= u->unk34 && u->unk1C != 11)
    {
        u->unk7A = 1;
        ActorSetState(gCurTask->unk1C);
        TaskSetEntry(sub_080a0b10, gCurTaskIdx);
    }
}

void sub_080a0ccc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 z;
    s32 k;
    s32 r;

    t = gCurTask;
    z = 0;
    t->unk15 = 3;
    TaskStop();
    gCurTask->unk7A = z;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFC0000, 128 << 8, 128 << 11);
    TaskSetFrame(17);
    while ((gCurTask->unk7A & 1) == 0)
        TaskYieldTrampoline(1);
    PlaySfx(0x1F7);
    sub_080261d4(4);
    r = sub_0806d1e8(24, 32);
    u = gCurTask;
    u->unk46 = r;
    sub_080a0274();
    TaskSetMotionXFacing(160 << 9, k = 0x5A5A5A5A);
    v = gCurTask;
    v->unk3C++;
    TaskYieldTrampoline(10);
    TaskSetMotionXFacing(128 << 9, k);
    TaskYieldTrampoline(10);
    TaskSetMotionXFacing(128 << 8, k);
    w = gCurTask;
    w->unk3C++;
    TaskYieldTrampoline(10);
    TaskSetMotionXFacing(128 << 6, k);
    TaskYieldTrampoline(10);
    TaskStop();
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a0dbc(void)
{
    struct Task *t;
    s16 *p;

    t = gCurTask;
    if (t->unk14 != 3)
    {
        p = &t->unk46;
        if (*p != -1)
        {
            TaskFree(*p);
            gCurTask->unk46 = 0xFFFF;
        }
        TaskSetEntry(sub_080a0b10, gCurTaskIdx);
    }
}

void sub_080a0e08(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 i;
    s32 z;

    t = gCurTask;
    z = 0;
    t->unk15 = 4;
    TaskStop();
    u = gCurTask;
    u->unk20 = -2;
    u->unk24 = -2;
    TaskSetFrame(25);
    TaskYieldTrampoline(20);
    gCurTask->unk6C = z;
    do
    {
        TaskSetFrame(26);
        TaskYieldTrampoline(10);
        gCurTask->unk3C++;
        TaskYieldTrampoline(10);
    } while ((s16)++gCurTask->unk6C <= 3);
    PlaySfx(0x21F);
    gCurTask->unk7A = 0;
    v = gCurTask;
    v->unk3C++;
    for (i = 0; i <= 10; i += 2)
    {
        gCurTask->unk58 = gUnk_087483B8[i];
        TaskYieldTrampoline(gUnk_087483B8[i + 1]);
    }
    TaskStop();
    TaskSetFrame(22);
    w = gCurTask;
    w->unk68 = 192 << 9;
    w->unk64 = 192 << 9;
    w->unk20 = 150 << 1;
    while (1)
    {
        TaskYieldTrampoline(12);
        TaskFaceNearestPlayer();
    }
}

void sub_080a0ec8(void)
{
    struct Task *t;
    s32 m2;
    s32 n;
    s32 n2;

    t = gCurTask;
    n = t->unk20;
    m2 = -2;
    if (n == m2)
        return;
    if (n == 0)
    {
        ActorSetState(5);
        TaskSetEntry(sub_080a0b10, gCurTaskIdx);
        return;
    }
    t->unk20 = n - 1;
    n2 = t->unk24;
    if (n2 != m2)
    {
        t->unk24 = n2 - 1;
        if (n2 <= 0)
        {
            TaskSetFrame(22);
            gCurTask->unk24 = m2;
        }
    }
    if ((u8)TaskGetYDirBitToNearestPlayer() == 2)
        gCurTask->unk60 = 0xFFFFFB00;
    else
        gCurTask->unk60 = 160 << 3;
    if ((u8)TaskGetXDirBitToNearestPlayer() == 8)
        gCurTask->unk5C = 0xFFFFF800;
    else
        gCurTask->unk5C = 128 << 4;
    TaskSetFrame(gCurTask->unk3C);
}

void sub_080a0f7c(void)
{
    struct Task *t;
    s32 z;
    s32 i;

    t = gCurTask;
    z = 0;
    t->unk15 = 5;
    TaskStop();
    gCurTask->unk7A = z;
    TaskSetFrame(22);
    TaskSetFrame(29);
    PlaySfx(0x21A);
    sub_080a028c();
    for (i = 0; i <= 3; i++)
    {
        TaskSetMotionXFacing(gUnk_08748410[i], 0x5A5A5A5A);
        TaskYieldTrampoline(2);
    }
    TaskStopX();
    TaskSetFrame(43);
    TaskSetMotionY(0, 148 << 6, 192 << 10);
    while ((gCurTask->unk7A & 1) == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(41);
    TaskYieldTrampoline(10);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a1030(void)
{
    if (gCurTask->unk14 != 5)
        TaskSetEntry(sub_080a0b10, gCurTaskIdx);
}

void sub_080a1058(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 z;

    t = gCurTask;
    z = 0;
    t->unk15 = 6;
    TaskStop();
    TaskSetFrame(41);
    TaskYieldTrampoline(24);
    gCurTask->unk7A = z;
    sub_080a0480(0);
    u = gCurTask;
    u->unk3C++;
    PlaySfx(0x21E);
    TaskSetMotionY(0xFFFB0000, 160 << 6, 160 << 11);
    while (gCurTask->unk58 < 0)
        TaskYieldTrampoline(1);
    TaskStop();
    v = gCurTask;
    v->unk3C++;
    v->unk60 = 160 << 6;
    v->unk68 = 160 << 11;
    if (v->unk24 != 0)
        sub_080a0480(1);
    while ((gCurTask->unk7A & 1) == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    PlaySfx(252 << 1);
    sub_080261d4(2);
    sub_080a02d4(1);
    TaskSetFrame(41);
    TaskYieldTrampoline(34);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a1140(void)
{
    if (gCurTask->unk14 != 6)
        TaskSetEntry(sub_080a0b10, gCurTaskIdx);
}

void sub_080a1168(void)
{
    gCurTask->unk15 = 7;
    TaskStop();
    sub_080a043c();
    if (gCurTask->unk24 != 2)
        sub_080a0844();
    else
        sub_080a0768();
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a11a0(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk14 != 7)
    {
        TaskFree(t->unk46);
        gCurTask->unk46 = 0xFFFF;
        TaskSetEntry(sub_080a0b10, gCurTaskIdx);
    }
}

void sub_080a11e0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    s16 *p;
    s32 z;
    s32 z2;

    t = gCurTask;
    z = 0;
    t->unk15 = 8;
    TaskStop();
    gCurTask->unk20 = z;
    TaskSetFrame(24);
    TaskYieldTrampoline(8);
    u = gCurTask;
    u->unk3C++;
    TaskYieldTrampoline(20);
    sub_080a0098();
    while (gUnk_02007D00[0] != 1)
    {
        TaskSetFrame(26);
        TaskYieldTrampoline(4);
        v = gCurTask;
        v->unk3C++;
        TaskYieldTrampoline(6);
    }
    w = gCurTask;
    z2 = 0;
    w->unk20 = z2;
    if (gUnk_02007D00[9] != -1)
    {
        StopSfxOnPlayer(gUnk_02007D00[9], 0x21B);
        gUnk_02007D00[9] = -1;
    }
    x = gCurTask;
    p = &x->unk46;
    if (*p != -1)
    {
        TaskFree(*p);
        gCurTask->unk46 = 0xFFFF;
    }
    PlaySfx(135 << 2);
    TaskSetFrame(28);
    gCurTask->unk6C = z2;
    do
    {
        TaskStepForward(-2);
        TaskYieldTrampoline(2);
        TaskStepForward(2);
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->unk6C <= 3);
    TaskStepForward(0);
    TaskYieldTrampoline(14);
    ActorSetState(9);
    TaskSleepForever();
}

void sub_080a12e0(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    if (gUnk_02006190[7] == 1)
    {
        ActorFlashPalette(gUnk_082D8638, 16);
        if (gUnk_02006190[5] <= 0)
        {
            gUnk_02006190[7] = 0;
            sub_08066468();
        }
        else
        {
            gUnk_02006190[5]--;
        }
    }
    t = gCurTask;
    if (t->unk20 != 0)
    {
        if (gUnk_02007D00[8] != -1)
        {
            if (t->unk46 != -1)
            {
                TaskFree(t->unk46);
                gCurTask->unk46 = 0xFFFF;
            }
        }
        sub_080a00ec();
        n = gUnk_02007D00[0];
        if (n == -1)
        {
            u = gCurTask;
            if (u->unk34 <= 0)
            {
                if (u->unk46 != -1)
                {
                    TaskFree(u->unk46);
                    gCurTask->unk46 = 0xFFFF;
                }
                if (gUnk_02007D00[9] != n)
                {
                    StopSfxOnPlayer(gUnk_02007D00[9], 0x21B);
                    gUnk_02007D00[9] = n;
                }
                ActorSetState(1);
                TaskSetEntry(sub_080a0b10, gCurTaskIdx);
            }
            else
            {
                u->unk34--;
            }
        }
    }
    if (gCurTask->unk14 != 8)
    {
        sub_08066468();
        TaskSetEntry(sub_080a0b10, gCurTaskIdx);
    }
}

void sub_080a13d4(void)
{
    gCurTask->unk15 = 9;
    TaskStop();
    sub_080a0028();
    TaskYieldTrampoline(51);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a1400(void)
{
    if (gUnk_02006190[7] == 1)
    {
        ActorFlashPalette(gUnk_082D8638, 16);
        if (gUnk_02006190[5] <= 0)
        {
            gUnk_02006190[7] = 0;
            sub_08066468();
        }
        else
        {
            gUnk_02006190[5]--;
        }
    }
    if (gCurTask->unk14 != 9)
    {
        if (gUnk_02006190[5] > 0)
        {
            gUnk_02006190[7] = 0;
            sub_08066468();
        }
        TaskSetEntry(sub_080a0b10, gCurTaskIdx);
    }
}

void sub_080a146c(void)
{
    struct Task *t;
    struct Actor *a;
    u8 f;

    t = gCurTask;
    a = t->unk8C;
    t->unk15 = 10;
    TaskStop();
    TaskSetFrame(43);
    TaskSetMotionY(0, 148 << 6, 192 << 10);
    while ((gCurTask->unk7A & 1) == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(41);
    TaskYieldTrampoline(10);
    f = a->unk1C;
    ActorSetState(1);
    a->unk1C = (s8)f;
    TaskSleepForever();
}

void sub_080a14e4(void)
{
    if (gCurTask->unk14 != 10)
        TaskSetEntry(sub_080a0b10, gCurTaskIdx);
}

void sub_080a150c(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk0C = (u32)sub_08065438;
    t->unk04 = (u32)sub_080a1550;
    t->unk38 = gUnk_087538E0;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 2, gUnk_087484C4);
}

void sub_080a1550(void)
{
    ActorCollideTerrain();
    CallTableEntry(gCurTask->unk15, 2, gUnk_087484CC);
}

void sub_080a1570(void)
{
    TaskFaceNearestPlayer();
    CallTableEntry(gCurTask->unk14, 2, gUnk_087484C4);
}
