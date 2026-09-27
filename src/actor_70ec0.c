/* game_code_and_rodata 0x08070EC0-0x08072D8C (issue #79, module M19 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08070EC0 0x08072D8C src/actor_70ec0.c --newpb
 *
 * M19 batch 1: the class-3 entries for task types #74 (sub_08071030) and the
 * warp-star/intro coroutines they install, plus the shared sprite-frame and
 * palette helpers the rest of the bank calls.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"


/* RAM cells and ROM tables */
extern s16 gPlayerHealth[];
extern s16 gViewRect[];
extern s16 gSpriteCameraX;
extern s16 gSpriteCameraY;
extern s16 gUnk_0300244C;
extern s16 gUnk_0873FF98[];
extern s32 gUnk_030023D4;
extern s32 gUnk_0873FB94[];
extern s8 gUnk_02006160;
extern s8 gUnk_030023B8;
extern struct PlayerState gPlayerStates[];
extern struct Task * gCurTask;
extern struct Task gTasks[];
extern u16 gUnk_020055C0;
extern u16 gLocalPlayer;
extern u16 gPlayerCount;
extern u32 gUnk_02004B4C;
extern u32 gUnk_02005584;
extern u32 gUnk_02007D00[];
extern u32 gUnk_0824A9CC[];
extern u32 gUnk_0825D2C8[];
extern u32 gUnk_0873F554[];
extern u32 gUnk_0873F5CC[];
extern u32 gUnk_0873FB7C[];
extern u32 gUnk_0873FBAC[];
extern u32 gUnk_0873FBB8[];
extern u32 gUnk_0873FBC4[];
extern u32 gUnk_0873FC2C[];
extern u32 gUnk_08752D50[];
extern u32 gUnk_08752D8C[];
extern u8 gUnk_020061E0;
extern u8 gUnk_02007CF0;
extern u8 gUnk_03001F30;
extern u8 gActivePlayerMask;
extern u8 gActivePlayerCount;
extern u8 gUnk_0873FAE8[];
extern vs32 gCurTaskIdx;

/* callees */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern s32 PlaySfx(u32 a);
extern s32 IsOnScreen(s16 a, s16 b);
extern s32 sub_08025f00();
extern s32 sub_08027750();
extern s32 sub_080277f0();
extern s32 sub_08040934();
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);
extern s32 CreateChildTaskAt(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern s32 CreateActorByKind(u8 cls, u32 sub, u8 p3, u8 p4, int x, int y, u16 prio);
extern u32 RandomRange(u32 range);
extern u32 TaskIsOnScreen(void);
extern u32 sub_08025e88(s32 i);
extern u32 ActorCheckHits(void);
extern u8 ActorIsInView(void);
extern void TaskYieldTrampoline(u32 a);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void StopSfxOnPlayer(s32 player, s32 songId);
extern void TaskFree(s32 id);
extern void TaskMove(void);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *a, u32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStopY(void);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void sub_0801bcac(u32 *p);
extern void RequestScreenShake(u32 a);
extern void ActorSetState(u8 v);
extern void ActorSetAttackBox(u32 v);
extern void ActorDestroy(void);
extern void ActorDrawWorldInView(void);
extern void ActorMove(void);
extern void TaskMoveRelativeToView(void);
extern void sub_08067108(void);
extern void sub_0806d4e4(u32 a, s32 b);
extern void sub_0806ef38(void);
extern void sub_08070454(void);
extern void sub_08070498(u32 a, s32 b);
extern void sub_08070758(void);
extern void sub_08074bb0(int a, int b, int c);

/* defined below */
void sub_08071098(void);
void sub_080711d0(void);
u16 sub_08071360(s32 idx);
void sub_08071778(void);
void sub_080719a0(void);
void sub_08071830(void);

void sub_08070ec0(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 gfx;
    s8 sign;
    u16 dx;
    u16 dy;
    s16 x;
    s16 y;

    t = gCurTask;
    dx = t->unk48 - gSpriteCameraX;
    dy = t->unk4A - gSpriteCameraY;
    v = gTasks[t->unk44].unk18;
    t->unk18 = v;
    if (v <= -2)
        return;
    if (v == -1)
        t->unk40 |= 0xC00;
    else
        t->unk40 &= 0xF3FF;
    u = gCurTask;
    if (u->unk18 > 0)
    {
        sign = (u->unk3E & 0x8000) ? -1 : 1;
        u->unk3E &= 0x7FFF;
        gfx = DrawAffineSprite((s32)gUnk_0824A9CC,
                           (u16)gUnk_0873FF98[u->unk18 >> 16] * sign,
                           gUnk_0873FF98[u->unk18 >> 16], 0);
        if (sign < 0)
            gCurTask->unk3E |= 0x8000;
    }
    else
    {
        gfx = (s32)gUnk_0824A9CC;
    }
    x = dx;
    y = dy;
    if (IsOnScreen(x, y) == 0)
        return;
    QueueSprite(gCurTask->layer, gfx, gCurTask->unk3E,
                 gCurTask->unk40 | 0x800, x, y);
}

void sub_08070ffc(void)
{
    {
        struct Task *t = gCurTask;

        t->unk00 = (u32)sub_08070454;
        t->taskClass = 4;
    }
    {
        struct Task *t = gCurTask;

        t->unk08 = 0;
        t->posY = 0;
        t->posX = 0;
        t->layer = 7;
    }
    TaskSetFrame(-1);
}

void sub_08071030(void)
{
    {
        struct Task *t = gCurTask;

        if (t->unk74 != 23)
            t->unk00 = (u32)ActorMove;
        else
            t->unk00 = (u32)TaskMoveRelativeToView;
    }
    {
        struct Task *t = gCurTask;

        t->unk0C = (u32)ActorDrawWorldInView;
        t->layer = 11;
    }
    {
        struct Task *t = gCurTask;

        t->unk38 = gUnk_08752D50;
        t->unk04 = (u32)sub_08071098;
    }
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_0873FBAC);
}

void sub_08071098(void)
{
    CallTableEntry(gCurTask->unk15, 3, gUnk_0873FBB8);
    if (gUnk_02006160 == -1 || gUnk_02006160 == gCurTaskIdx)
    {
        if (ActorCheckHits())
            sub_080711d0();
    }
}

void sub_080710e0(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_0873FBAC);
}

void sub_080710fc(void)
{
    u32 off;

    {
        struct Task *t = gCurTask;

        if (t->unk38 != gUnk_08752D50)
            return;
        t->unk6E++;
        if (t->unk6E > 5)
            t->unk6E = 0;
    }
    {
        struct Task *t = gCurTask;

        u32 dst;

        t->frame = 13;
        off = (t->unk40 & 0x7FF) << 5;
        dst = 0x06010000 + off;
        RequestCopy(1, gUnk_0873FB7C[t->unk6E], dst, 128);
    }
    RequestCopy(1, gUnk_0873FB7C[gCurTask->unk6E] + 128, 0x06010400 + off, 128);
    RequestCopy(1, gUnk_0873FB7C[gCurTask->unk6E] + 256, 0x06010800 + off, 128);
    RequestCopy(1, gUnk_0873FB7C[gCurTask->unk6E] + 384, 0x06010C00 + off, 128);
}

void sub_080711d0(void)
{
    if (gUnk_0300244C != 0 && gPlayerHealth[gCurTask->unk7E] <= 0)
        return;
    if (gUnk_020061E0 == 0)
    {
        struct Task *t = gCurTask;
        u16 f;

        t->frame = 13;
        if (t->unk74 != 23)
        {
            t->unk4A -= 16;
            t->posX = t->unk48 << 16;
            t->posY = t->unk4A << 16;
        }
        else
        {
            t->unk4A -= 8;
            t->posX = (t->unk48 - gViewRect[0]) << 16;
            t->posY = (t->unk4A - gViewRect[2]) << 16;
        }
        f = sub_08071360(gCurTask->unk7E);
        {
            struct Task *t2 = gCurTask;

            t2->unk40 = f;
            gUnk_02006160 = gCurTaskIdx;
            gUnk_02007CF0 = 2;
            if (t2->unk46 != -1)
            {
                TaskFree(t2->unk46);
                gCurTask->unk46 = 0xFFFF;
            }
        }
        ActorSetAttackBox((u32)gUnk_0873F554);
    }
    gPlayerStates[gCurTask->unk7E].unk04 = 16;
    sub_08040934(gCurTask->unk7E);
    gCurTask->unk28++;
    sub_08070498(gCurTask->unk7E, gCurTaskIdx);
    sub_0806d4e4(0, 0);
    if (gLocalPlayer == gCurTask->unk7E)
        PlaySfx(219);
    if (gUnk_030023B8 == 7)
        gUnk_02007D00[9] = 1;
    ActorSetState(1);
    TaskSetEntry(sub_080710e0, gCurTaskIdx);
}

u16 sub_08071360(s32 idx)
{
    u16 v = gTasks[idx].unk40 & 0x7FF;
    u32 off = v << 5;

    RequestCopy(1, (u32)gUnk_0825D2C8, 0x06010180 + off, 128);
    RequestCopy(1, (u32)gUnk_0825D2C8 + 128, 0x06010580 + off, 128);
    RequestCopy(1, (u32)gUnk_0825D2C8 + 256, 0x06010980 + off, 128);
    RequestCopy(1, (u32)gUnk_0825D2C8 + 384, 0x06010D80 + off, 128);
    gUnk_020061E0++;
    return v + 0xF00C;
}

void sub_080713f8(int x, int y, int c)
{
    CreateActorByKind(5, 0, 0, c, x, y, 0);
}

void sub_08071418(void)
{
    gCurTask->unk15 = 0;
    {
        struct Task *t = gCurTask;

        t->unk28 = 0;
        t->unk2C = 16;
        t->unk30 = 0;
    }
    gCurTask->unk46 = CreateChildTaskHere(164, 0);
    while (1)
    {
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
        gCurTask->frame = 10;
        TaskYieldTrampoline(1);
        gCurTask->frame = 1;
        TaskYieldTrampoline(3);
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->frame = 2;
        TaskYieldTrampoline(3);
        gCurTask->frame = 12;
        TaskYieldTrampoline(1);
        gCurTask->frame = 3;
        TaskYieldTrampoline(3);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->frame = 1;
        TaskYieldTrampoline(3);
        gCurTask->frame = 11;
        TaskYieldTrampoline(1);
        gCurTask->frame = 2;
        TaskYieldTrampoline(3);
        gCurTask->frame = 12;
        TaskYieldTrampoline(1);
        gCurTask->frame = 3;
        TaskYieldTrampoline(3);
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
        gCurTask->frame = 10;
        TaskYieldTrampoline(1);
        gCurTask->frame = 1;
        TaskYieldTrampoline(3);
        gCurTask->frame = 11;
        TaskYieldTrampoline(1);
        gCurTask->frame = 2;
        TaskYieldTrampoline(3);
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->frame = 3;
        TaskYieldTrampoline(3);
    }
}

void sub_0807156c(void)
{
    if (gUnk_02007CF0 != 0 && gActivePlayerCount != 1)
    {
        struct Task *t = gCurTask;

        if (t->unk46 != -1)
        {
            TaskFree(t->unk46);
            gCurTask->unk46 = 0xFFFF;
        }
        ActorSetState(2);
        TaskSetEntry(sub_080710e0, gCurTaskIdx);
    }
    else
    {
        {
            struct Task *t = gCurTask;

            if (t->unk2C <= 0)
            {
                t->unk58 = gUnk_0873FB94[t->unk30];
                t->unk30++;
                if (t->unk30 > 5)
                    t->unk30 = 0;
                gCurTask->unk2C = 16;
            }
        }
        gCurTask->unk2C--;
    }
}

void sub_0807160c(void)
{
    gCurTask->unk15 = 1;
    TaskStop();
    while (1)
    {
        gCurTask->unk58 = 0x4000;
        TaskYieldTrampoline(18);
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(18);
    }
}

void sub_08071640(void)
{
    sub_080710fc();
    if (gCurTask->unk28 == gActivePlayerCount && sub_08027750())
    {
        struct Task *t = gCurTask;
        u32 v = t->unk74;

        if (v == 0)
        {
            v = sub_08025e88(gCurTaskIdx);
            t = gCurTask;
        }
        t->unk14 = v;
        TaskSetEntry(sub_08071778, gCurTaskIdx);
    }
}

void sub_08071694(void)
{
    gCurTask->unk04 = 0;
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->unk38 = gUnk_08752D8C;
        t->frame = 0;
    }
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(2);
    gCurTask->frame = 6;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame = -1;
    TaskYieldTrampoline(1);
    gCurTask->frame = 9;
    TaskYieldTrampoline(1);
    gCurTask->frame = -1;
    TaskYieldTrampoline(1);
    gCurTask->frame = 10;
    TaskYieldTrampoline(1);
    ActorDestroy();
}

void sub_08071774(void)
{
}

void sub_08071778(void)
{
    struct Task *t;

    {
        struct Task *u = gCurTask;

        u->unk0C = (u32)sub_080719a0;
        u->layer = 10;
    }
    t = gCurTask;
    t->unk38 = gUnk_08752D50;
    t->unk04 = (u32)sub_08071830;
    t->unk18 = 0;
    t->unk28 = 0;
    t->unk78 = 0xFFFF;
    t->unk70 = 0;
    if (t->unk74 == 0)
        gCurTask->facing = gUnk_0873FAE8[sub_08025e88(gCurTaskIdx)];
    else
        t->facing = gUnk_0873FAE8[t->unk74];
    CreateChildTaskAt(97, gViewRect[0] + 120, gViewRect[2] + 80, 0);
    CallTableEntry(gCurTask->unk14, 26, gUnk_0873FBC4);
}

void sub_08071830(void)
{
    sub_080710fc();
    CallTableEntry(gCurTask->unk15, 26, gUnk_0873FC2C);
}

void sub_08071850(void)
{
    CallTableEntry(gCurTask->unk14, 26, gUnk_0873FBC4);
}

void sub_0807186c(int a, int b, int c, int d)
{
    {
        struct Task *t = gCurTask;

        t->unk78 = (s8)a;
        t->unk74 = b;
    }
    gCurTask->unk75 = c;
    gCurTask->unk34 = (s16)d;
}

void sub_08071898(void)
{
    {
        struct Task *t = gCurTask;

        t->unk78 = 0xFFFF;
        t->unk74 = 0;
    }
    gCurTask->unk75 = 0;
    gCurTask->unk34 = 0;
}

void sub_080718c0(void)
{
    struct Task *t = gCurTask;
    u32 v;
    s32 r;

    if (t->unk78 == -1)
        return;
    t->unk70--;
    if ((s16)t->unk70 > 0)
        return;
    if ((s8)t->unk75 == -1)
    {
        if (t->unk78 == 2 || t->unk78 == 0)
        {
            v = (u8)(u16)t->unk78;
        }
        else
        {
            r = RandomRange(3);
            v = 1;
            if (r != 0)
                v = 0;
        }
        sub_08074bb0(gCurTask->unk74 << 5, (s16)gCurTask->unk34, v);
        gCurTask->unk78 = 0xFFFF;
    }
    else
    {
        gUnk_030023D4 = RandomRange(32) - 16;
        gUnk_030023D4 += gCurTask->unk74 << 5;
        if (gUnk_030023D4 < 0)
            gUnk_030023D4 += 512;
        {
            u16 a1 = gUnk_030023D4;
            s32 a2 = (s16)gCurTask->unk34;

            v = RandomRange(3);
            /* r (the other branch's RNG result) carries the byte, so regmove keeps the AND on v */
            r = (u8)gCurTask->unk78;
            sub_08074bb0(a1, a2, v & r);
        }
        gCurTask->unk70 = (s8)gCurTask->unk75;
    }
}

void sub_080719a0(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    s32 v;
    s32 gfx;
    s8 sign;
    u32 *g;

    v = t->unk18;
    if (v <= -2)
    {
        if (gUnk_03001F30 != 0)
            return;
        g = t->unk38;
        QueueSprite(t->layer, g[t->frame], t->unk3E, t->unk40,
                     t->unk48 - gSpriteCameraX, t->unk4A - gSpriteCameraY);
        return;
    }
    if (v == -1)
        t->unk40 |= 0xC00;
    else
        t->unk40 &= 0xF3FF;
    {
        struct Task *w = gCurTask;

        if (w->unk18 > 0)
        {
            w->unk18 += w->unk28;
            if (w->unk18 < 0)
                w->unk18 = 0;
            if (gCurTask->unk18 > 0x3F0000)
                gCurTask->unk18 = 0x3F0000;
        }
    }
    if (gUnk_03001F30 != 0)
        return;
    if (gCurTask->unk38 == NULL)
        return;
    if (gCurTask->frame == -1)
        return;
    if (ActorIsInView() == 0)
        return;
    if (TaskIsOnScreen() == 0)
        return;
    u = gCurTask;
    g = u->unk38;
    if (u->unk18 > 0)
    {
        sign = (u->unk3E & 0x8000) ? -1 : 1;
        u->unk3E &= 0x7FFF;
        gfx = DrawAffineSprite(g[u->frame],
                           (u16)gUnk_0873FF98[u->unk18 >> 16] * sign,
                           gUnk_0873FF98[u->unk18 >> 16], 0);
        {
            struct Task *x = gCurTask;

            QueueSprite(x->layer, gfx, x->unk3E, x->unk40,
                         x->unk48 - gSpriteCameraX, x->unk4A - gSpriteCameraY);
        }
        if (sign < 0)
            gCurTask->unk3E |= 0x8000;
    }
    else
    {
        QueueSprite(u->layer, g[u->frame], u->unk3E, u->unk40,
                     u->unk48 - gSpriteCameraX, u->unk4A - gSpriteCameraY);
    }
}

void sub_08071bb0(u16 a)
{
    s32 i;

    if (gUnk_03001F30 != 0)
        while (1)
            ;
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            struct Task *t = &gTasks[i];

            t->facing = gCurTask->facing;
            t->posX = t->unk48 << 16;
            t->posY = t->unk4A << 16;
            t->unk14 = a;
            TaskSetEntry(sub_0806ef38, i);
        }
    }
}

void sub_08071c38(u16 a)
{
    s32 i;

    if (gUnk_03001F30 != 1)
        while (1)
            ;
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            struct Task *t = &gTasks[i];

            t->facing = gCurTask->facing;
            t->posX = t->unk48 << 16;
            t->posY = t->unk4A << 16;
            t->unk14 = a;
            TaskSetEntry(sub_08070758, i);
        }
    }
}

void sub_08071cc0(int x, int y, int c)
{
    s32 id = CreateActorByKind(5, 0, 0, c, x >> 16, y >> 16, 0);

    if (id != -1)
    {
        struct Task *t = &gTasks[id];

        t->unk14 = c;
        TaskSetEntry(sub_08071778, id);
        t->unk40 = sub_08071360(gCurTaskIdx);
    }
    gUnk_020055C0 = id;
    sub_08067108();
}

void sub_08071d2c(void)
{
    PlaySfx(219);
    TaskSetMotionY(0x30000, -0x4000, 0x30000);
    TaskYieldTrampoline(4);
    PlaySfx(216);
    gCurTask->unk24 = 0;
}

void sub_08071d60(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    t = gCurTask;
    {
        t->posX = (t->unk48 - gViewRect[0]) << 16;
        t->posY = (t->unk4A - gViewRect[2]) << 16;
        t->unk00 = (u32)TaskMoveRelativeToView;
        t->unk24 = 0;
    }
    {
        s32 r;

        gUnk_02005584 = 216;
        r = PlaySfx(216);
        gUnk_02004B4C = r;
    }
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = Div(0x780000 - gCurTask->posY, 32);
    TaskYieldTrampoline(32);
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    CreateChildTaskAt(148, gCurTask->unk48, gCurTask->unk4A + 16, 0);
    RequestScreenShake(2);
    PlaySfx(272);
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(32);
    sub_0807186c(1, 4, 4, 0x400);
    gCurTask->unk58 = -0x8000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = -0x14000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = -0x28000;
    TaskYieldTrampoline(40);
    sub_08025f00();
    gCurTask->unk24 = 1;
    TaskSleepForever();
}

void sub_08071e74(void)
{
    sub_080718c0();
}

void sub_08071e80(void)
{
    gCurTask->unk15 = 1;
    gCurTask->unk00 = (u32)TaskMove;
    TaskStop();
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    gCurTask->unk58 = 0x20000;
    TaskSleepForever();
}

void sub_08071ebc(void)
{
    struct Task *t;

    sub_080718c0();
    sub_0801bcac(gUnk_0873F5CC);
    t = gCurTask;
    if (t->unk7A & 1)
    {
        sub_080277f0(t->unk48, t->unk4A);
        RequestScreenShake(4);
        StopSfxOnPlayer(gUnk_02004B4C, gUnk_02005584);
        PlaySfx(219);
        sub_0806d4e4(0, 0);
        PlaySfx(272);
        if (gUnk_03001F30 == 0)
            sub_08071bb0(3);
        else
            sub_08071c38(3);
        gUnk_020061E0 = 0;
        ActorDestroy();
    }
}

void sub_08071f54(void)
{
    struct Task *t;

    gCurTask->unk15 = 5;
    t = gCurTask;
    t->posX = (t->unk48 - gViewRect[0]) << 16;
    t->posY = (t->unk4A - gViewRect[2]) << 16;
    t->unk00 = (u32)TaskMoveRelativeToView;
    t->unk24 = 0;
    TaskStop();
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    {
        s32 r;

        gUnk_02005584 = 126;
        r = PlaySfx(126);
        gUnk_02004B4C = r;
    }
    gCurTask->unk54 = -0x8000;
    sub_0807186c(1, 4, 4, 0x600);
    gCurTask->unk58 = -0x80000;
    TaskYieldTrampoline(6);
    sub_0807186c(1, 5, 4, 0x600);
    gCurTask->unk58 = -0x40000;
    TaskYieldTrampoline(6);
    sub_0807186c(1, 6, 4, 0x600);
    gCurTask->unk58 = -0x10000;
    TaskYieldTrampoline(6);
    sub_0807186c(1, 7, 6, 0x600);
    gCurTask->unk58 = -0x8000;
    TaskYieldTrampoline(6);
    sub_0807186c(-1, 0, 0, 0);
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(6);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(6);
    gCurTask->unk58 = 0x40000;
    TaskYieldTrampoline(6);
    PlaySfx(272);
    sub_0807186c(1, 4, 4, 0x600);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x20000;
        u->unk58 = -0x80000;
    }
    TaskYieldTrampoline(12);
    TaskStop();
    sub_08025f00();
    gCurTask->unk24 = 1;
    TaskSleepForever();
}

void sub_080720dc(void)
{
    sub_080718c0();
}

void sub_080720e8(void)
{
    struct Task *t;

    gCurTask->unk15 = 6;
    t = gCurTask;
    t->posX = (t->unk48 - gViewRect[0]) << 16;
    t->posY = (t->unk4A - gViewRect[2]) << 16;
    t->unk00 = (u32)TaskMoveRelativeToView;
    t->unk24 = 0;
    sub_0807186c(1, 4, 4, 0x300);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x40000;
        u->unk58 = -0x80000;
    }
    TaskYieldTrampoline(4);
    sub_0807186c(1, 2, 4, 0x300);
    gCurTask->unk58 = -0x60000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = -0x40000;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = -0x20000;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(3);
    sub_0807186c(-1, 0, 0, 0);
    gCurTask->unk58 = 0x40000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0x60000;
    TaskYieldTrampoline(4);
    PlaySfx(272);
    gCurTask->unk54 = 0;
    TaskYieldTrampoline(2);
    sub_0807186c(1, 7, 4, 0x300);
    {
        struct Task *u = gCurTask;

        u->unk54 = 0x10000;
        u->unk58 = -0x80000;
    }
    TaskYieldTrampoline(4);
    gCurTask->unk54 = 0x20000;
    TaskYieldTrampoline(3);
    {
        struct Task *u = gCurTask;

        u->unk54 = 0x40000;
        u->unk58 = -0x60000;
    }
    TaskYieldTrampoline(4);
    gCurTask->unk58 = -0x40000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = -0x20000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(4);
    sub_0807186c(-1, 0, 0, 0);
    gCurTask->unk58 = 0x40000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0x60000;
    TaskYieldTrampoline(8);
    PlaySfx(272);
    {
        struct Task *u = gCurTask;

        u->unk54 = 0;
        u->unk58 = -0x60000;
    }
    TaskYieldTrampoline(2);
    sub_0807186c(1, 1, 4, 0x300);
    gCurTask->unk54 = -0x60000;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = -0x40000;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = -0x20000;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 0x40000;
    TaskYieldTrampoline(3);
    sub_0807186c(-1, 0, 0, 0);
    gCurTask->unk58 = 0x60000;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 0x80000;
    TaskYieldTrampoline(3);
    PlaySfx(272);
    TaskStop();
    gCurTask->unk58 = -0x60000;
    TaskYieldTrampoline(2);
    sub_0807186c(1, 7, 4, 0x300);
    {
        struct Task *u = gCurTask;

        u->unk54 = 0x40000;
        u->unk58 = -0x20000;
    }
    TaskYieldTrampoline(14);
    gCurTask->unk58 = -0x10000;
    TaskYieldTrampoline(10);
    sub_0807186c(1, 6, 8, 0x300);
    gCurTask->unk58 = -0x8000;
    TaskYieldTrampoline(8);
    sub_0807186c(1, 6, 10, 0x300);
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    sub_08025f00();
    gCurTask->unk24 = 1;
    TaskSleepForever();
}

void sub_0807237c(void)
{
    sub_080718c0();
}

void sub_08072388(void)
{
    struct Task *t;

    gCurTask->unk15 = 8;
    t = gCurTask;
    t->posX = (t->unk48 - gViewRect[0]) << 16;
    t->posY = (t->unk4A - gViewRect[2]) << 16;
    t->unk00 = (u32)TaskMoveRelativeToView;
    t->unk24 = 0;
    {
        s32 r;

        gUnk_02005584 = 126;
        r = PlaySfx(126);
        gUnk_02004B4C = r;
    }
    gCurTask->unk18 = 0x3F0000;
    sub_08071898();
    TaskStop();
    {
        struct Task *u = gCurTask;

        u->unk54 = 0x60000;
        u->unk58 = -0x30000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->unk54 = 0x20000;
        u->unk58 = -0x20000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->unk54 = 0x10000;
        u->unk58 = -0x10000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x10000;
        u->unk58 = -0x8000;
    }
    TaskYieldTrampoline(6);
    gCurTask->unk54 = -0x20000;
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x40000;
        u->unk58 = 0x2000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x60000;
        u->unk58 = 0x8000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x40000;
        u->unk58 = 0x10000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x20000;
        u->unk58 = 0x20000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x10000;
        u->unk58 = 0x30000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->unk54 = 0x8000;
        u->unk58 = 0x20000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->unk54 = 0x10000;
        u->unk58 = 0x10000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->unk54 = 0x20000;
        u->unk58 = 0x8000;
    }
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 0x40000;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 0x20000;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 0x10000;
    TaskYieldTrampoline(12);
    CreateChildTaskAt(148, gCurTask->unk48, gCurTask->unk4A + 16, 0);
    RequestScreenShake(2);
    PlaySfx(272);
    gCurTask->unk28 = -0x2600;
    sub_0807186c(1, 4, 3, 0x400);
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x2000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = -0x2000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = -0x8000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = -0x10000;
    TaskYieldTrampoline(8);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x8000;
        u->unk58 = 0x8000;
    }
    TaskYieldTrampoline(8);
    TaskStop();
    gCurTask->unk58 = -0x20000;
    TaskYieldTrampoline(10);
    gCurTask->unk58 = -0x40000;
    TaskYieldTrampoline(10);
    gCurTask->unk58 = -0x60000;
    TaskYieldTrampoline(16);
    sub_0807186c(-1, 0, 0, 0);
    TaskStop();
    gCurTask->unk54 = 0xC000;
    TaskYieldTrampoline(48);
    {
        struct Task *u = gCurTask;

        u->unk18 = 0x100000;
        u->unk28 = -0xC00;
    }
    StopSfxOnPlayer(gUnk_02004B4C, gUnk_02005584);
    {
        s32 r;

        gUnk_02005584 = 217;
        r = PlaySfx(217);
        gUnk_02004B4C = r;
    }
    TaskStop();
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(40);
    gCurTask->unk58 = 0x6000;
    TaskYieldTrampoline(40);
    gCurTask->unk58 = 0x4000;
    TaskYieldTrampoline(40);
    gCurTask->unk58 = 0x2000;
    TaskYieldTrampoline(40);
    gCurTask->unk58 = 0x1000;
    TaskYieldTrampoline(40);
    sub_08025f00();
    gCurTask->unk24 = 1;
    TaskSleepForever();
}

void sub_08072678(void)
{
    sub_080718c0();
}

void sub_08072684(void)
{
    struct Task *t;

    gCurTask->unk15 = 10;
    t = gCurTask;
    t->posX = (t->unk48 - gViewRect[0]) << 16;
    t->posY = (t->unk4A - gViewRect[2]) << 16;
    t->unk00 = (u32)TaskMoveRelativeToView;
    t->unk24 = 0;
    {
        s32 r;

        gUnk_02005584 = 250;
        r = PlaySfx(250);
        gUnk_02004B4C = r;
    }
    sub_08071898();
    TaskStop();
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(6);
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(6);
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x8000;
        u->unk58 = -0x8000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x10000;
        u->unk58 = -0x10000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x30000;
        u->unk58 = -0x40000;
    }
    TaskYieldTrampoline(4);
    gCurTask->unk58 = -0x20000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = -0x10000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(4);
    gCurTask->unk54 = -0x20000;
    TaskYieldTrampoline(8);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x10000;
        u->unk58 = 0x10000;
    }
    TaskYieldTrampoline(8);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x8000;
        u->unk58 = 0x8000;
    }
    TaskYieldTrampoline(8);
    sub_0807186c(1, 6, 4, 0x300);
    gCurTask->unk6C = 0;
    do
    {
        {
            struct Task *u = gCurTask;

            u->unk54 = 0x20000;
            u->unk58 = -0x20000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *u = gCurTask;

            u->unk54 = -0x20000;
            u->unk58 = 0x20000;
        }
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    CreateChildTaskAt(148, gCurTask->unk48, gCurTask->unk4A + 16, 0);
    RequestScreenShake(2);
    PlaySfx(272);
    {
        struct Task *u = gCurTask;

        u->unk54 = 0x20000;
        u->unk58 = -0x20000;
    }
    TaskYieldTrampoline(2);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x20000;
        u->unk58 = 0x20000;
    }
    TaskYieldTrampoline(2);
    {
        struct Task *u = gCurTask;

        u->unk54 = 0x80000;
        u->unk58 = -0x60000;
    }
    TaskYieldTrampoline(12);
    gCurTask->unk54 = 0x60000;
    TaskYieldTrampoline(4);
    gCurTask->unk54 = 0x40000;
    TaskYieldTrampoline(4);
    gCurTask->unk54 = 0x30000;
    TaskYieldTrampoline(4);
    TaskStop();
    sub_08025f00();
    gCurTask->unk24 = 1;
    TaskSleepForever();
}

void sub_080728a4(void)
{
    sub_080718c0();
}

void sub_080728b0(void)
{
    struct Task *t;

    gCurTask->unk15 = 11;
    t = gCurTask;
    t->posX = (t->unk48 - gViewRect[0]) << 16;
    t->posY = (t->unk4A - gViewRect[2]) << 16;
    t->unk00 = (u32)TaskMoveRelativeToView;
    t->unk24 = 0;
    sub_08071898();
    TaskStop();
    gCurTask->unk54 = 0x80000;
    TaskYieldTrampoline(1);
    gCurTask->unk6C = 0;
    while (1)
    {
        sub_0807186c(1, 7, 4, 0x300);
        {
            struct Task *u = gCurTask;

            u->unk54 = 0x80000;
            u->unk58 = -0x20000;
        }
        TaskYieldTrampoline(12);
        gCurTask->unk54 = 0x60000;
        TaskYieldTrampoline(6);
        gCurTask->unk54 = 0x40000;
        TaskYieldTrampoline(6);
        sub_0807186c(1, 5, -1, 0x300);
        gCurTask->unk54 = 0x20000;
        TaskYieldTrampoline(4);
        gCurTask->unk54 = 0x10000;
        TaskYieldTrampoline(4);
        gCurTask->unk54 = 0x8000;
        TaskYieldTrampoline(4);
        gCurTask->unk18 = -1;
        sub_0807186c(1, 1, 4, 0x300);
        {
            struct Task *u = gCurTask;

            u->unk54 = -0x10000;
            u->unk58 = -0x14000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *u = gCurTask;

            u->unk54 = -0x20000;
            u->unk58 = -0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *u = gCurTask;

            u->unk54 = -0x40000;
            u->unk58 = -0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *u = gCurTask;

            u->unk54 = -0x80000;
            u->unk58 = 0x30000;
        }
        TaskYieldTrampoline(18);
        {
            struct Task *u = gCurTask;

            u->unk54 = -0x40000;
            u->unk58 = 0x10000;
        }
        TaskYieldTrampoline(4);
        gCurTask->unk54 = -0x20000;
        TaskYieldTrampoline(2);
        gCurTask->unk18 = 0;
        sub_0807186c(1, 7, 4, 0x300);
        {
            struct Task *u = gCurTask;

            u->unk54 = 0x20000;
            u->unk58 = -0x8000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *u = gCurTask;

            u->unk54 = 0x40000;
            u->unk58 = -0x10000;
        }
        TaskYieldTrampoline(4);
        gCurTask->unk6C++;
        if ((s16)gCurTask->unk6C > 2)
            break;
    }
    {
        struct Task *u = gCurTask;

        u->unk54 = 0x80000;
        u->unk58 = -0x20000;
    }
    TaskYieldTrampoline(4);
    gCurTask->unk58 = -0x30000;
    TaskYieldTrampoline(4);
    sub_0807186c(1, 5, -1, 0x300);
    gCurTask->unk58 = -0x60000;
    TaskYieldTrampoline(4);
    sub_0807186c(1, 4, -1, 0x300);
    gCurTask->unk58 = -0x80000;
    TaskYieldTrampoline(8);
    TaskStop();
    TaskYieldTrampoline(4);
    sub_08025f00();
    gCurTask->unk24 = 1;
    TaskSleepForever();
}

void sub_08072af4(void)
{
    sub_080718c0();
}

void sub_08072b00(void)
{
    struct Task *t;

    gCurTask->unk15 = 12;
    t = gCurTask;
    t->posX = (t->unk48 - gViewRect[0]) << 16;
    t->posY = (t->unk4A - gViewRect[2]) << 16;
    t->unk00 = (u32)TaskMoveRelativeToView;
    {
        s32 r;

        gUnk_02005584 = 218;
        r = PlaySfx(218);
        gUnk_02004B4C = r;
    }
    TaskStop();
    sub_0807186c(1, 7, 4, 0x300);
    {
        struct Task *u = gCurTask;

        u->unk54 = 0x60000;
        u->unk58 = -0x30000;
    }
    TaskYieldTrampoline(14);
    sub_0807186c(1, 5, 4, 0x300);
    gCurTask->unk58 = -0x40000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = -0x60000;
    TaskYieldTrampoline(4);
    sub_0807186c(1, 4, 4, 0x300);
    gCurTask->unk6C = 0;
    do
    {
        {
            struct Task *u = gCurTask;

            u->unk54 = 0x60000;
            u->unk58 = -0x60000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *u = gCurTask;

            u->unk54 = -0x60000;
            u->unk58 = 0x60000;
        }
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    TaskStop();
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk58 = -0x60000;
        TaskYieldTrampoline(2);
        gCurTask->unk58 = 0x60000;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk58 = -0x20000;
        TaskYieldTrampoline(1);
        gCurTask->unk58 = 0x20000;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    sub_08071898();
    TaskStopY();
    TaskYieldTrampoline(8);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x800;
        u->unk58 = 0x800;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x2000;
        u->unk58 = 0x2000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x8000;
        u->unk58 = 0x8000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x10000;
        u->unk58 = 0x10000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x20000;
        u->unk58 = 0x20000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gCurTask;

        u->unk54 = -0x40000;
        u->unk58 = 0x40000;
    }
    TaskYieldTrampoline(6);
    gCurTask->unk54 = -0x80000;
    TaskYieldTrampoline(6);
    gCurTask->facing = 255;
    sub_080277f0(gCurTask->unk48, gCurTask->unk4A);
    RequestScreenShake(4);
    StopSfxOnPlayer(gUnk_02004B4C, gUnk_02005584);
    PlaySfx(219);
    sub_0806d4e4(0, 0);
    PlaySfx(272);
    if (gUnk_03001F30 == 0)
        sub_08071bb0(4);
    else
        sub_08071c38(4);
    gUnk_020061E0 = 0;
    ActorDestroy();
}

void sub_08072d80(void)
{
    sub_080718c0();
}
