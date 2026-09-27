/* game_code_and_rodata 0x080692FC-0x0806A344 (issue #64, module M18 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080692FC 0x0806A344 src/actor_692fc.c --newpb
 *
 * The player-input dispatch layer of the actor core.  Six probe wrappers
 * (0x080694E0-0x080696A0) snapshot the current task's directional state into a
 * 6-byte stack record (ActorGetTerrainBox) and hand it to one of the input decoders
 * at 0x0801BCAC..0x0801C3A4; the three big dispatchers (ActorCollideTerrain,
 * sub_080696a0, sub_08069888) then walk the actor's seven-entry handler table
 * at Actor.unk54, calling the first handler that claims the frame.  The tail
 * of the module is the class-1 "carried" task body: state machine entry
 * points (sub_08069ae4/sub_08069bbc), the sub_08069c8c sound dispatcher and
 * the sub_08069dc4/sub_08069e48 push/pop of the actor's transform.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* The 6-byte directional record ActorGetTerrainBox fills on the stack: three raw
   bytes copied from Actor.unk50 plus three that are negated when the task
   faces left (Task.unk43 == -1). */
struct InputState
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 unk02;
    /*0x03*/ u8 unk03;
    /*0x04*/ u8 unk04;
    /*0x05*/ u8 unk05;
};

/* Seven-entry handler table hanging off Actor.unk54; every entry is a
   u8 (*)(void) that returns 1 when it consumed the frame. */
struct ActorHandlers
{
    /*0x00*/ u32 unk00;
    /*0x04*/ u32 unk04;
    /*0x08*/ u32 unk08;
    /*0x0C*/ u32 unk0C;
    /*0x10*/ u32 unk10;
    /*0x14*/ u32 unk14;
    /*0x18*/ u32 unk18;
};

/* Block Actor.unk5C points at: two s8 mode bytes and two u8 (*)(void)
   hooks.  Compare struct ActorAux, which is the Actor.unk60 block. */
struct ActorVt
{
    /*0x00*/ s8 unk00;
    /*0x01*/ s8 unk01;
    /*0x02*/ u8 filler02[2];
    /*0x04*/ u32 unk04;
    /*0x08*/ u32 unk08;
};

extern vs16 gTaskSlotTypes[];
extern u8 gTerrainResult[];
extern u16 gLocalPlayer;
extern u16 gGameState;
extern u16 gUnk_0873E58C[];
extern s16 gUnk_0873E5A4[];
extern u32 gUnk_0873F910[];

extern u32 RandomRange(u32 range);
extern void PlaySfx(s32 id);
extern void TaskSetSkipMask(s32 a, s32 b);
extern void TaskSetEntry(void *fn, s32 i);
extern void TaskSetFrame(s32 a);
extern void AddPlayerLives(s32 a, s32 b);
extern void sub_0800a42c(void);
extern void sub_0801bcac(struct InputState *p);
extern void sub_0801bde0(struct InputState *p);
extern void sub_0801bf1c(struct InputState *p);
extern void sub_0801c030(struct InputState *p);
extern void sub_0801c12c(struct InputState *p);
extern void sub_0801c230(struct InputState *p);
extern void sub_0801c30c(struct InputState *p);
extern u32 sub_0801c3a4(struct InputState *p);
extern u32 sub_0802205c(struct InputState *p);
extern void TaskInitWaterFlagsSlot(s32 i);
extern void sub_0804087c(s32 a);
extern void ActorSetState(u8 v);
extern void ActorSetTerrainHandlers(u32 v);
extern s32 TaskGetFacingToward(u32 i);
extern void ActorDestroy(void);
extern s32 CreateChildTaskAt(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern void ActorAwardScore(s32 a, s32 b);
extern void ActorPlaySfx(u32 def, u32 which);
extern void ActorCheckHits(void);
extern u32 ActorDie(void);
extern void sub_0806b26c(void);
extern u32 sub_0806bb7c(void);
extern void sub_0806d65c(void);
extern void sub_0806d77c(void);
extern void sub_0806df28(s32 a, s32 b);
extern void sub_0806ee2c(void);
extern void sub_080b4240(void);
extern void sub_080b460c(void);
extern void sub_080b54d0(s32 i);

void ActorGetTerrainBox(struct InputState *out);
u32 sub_08069ae4(s8 a);
u32 ActorReactToHit(void);
s8 sub_08069c48(void);
void sub_08069c8c(void);
void sub_08069d78(void);
void sub_08069dc4(void);
void sub_08069e48(void);
u32 sub_08069ea0(void);
void sub_08069f0c(void);
void sub_08069f70(void);
void sub_08069fb0(void);
void sub_0806a158(void);
u32 sub_0806a25c(void);

u32 ActorCollideTerrain(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    u32 fn;
    s32 i;
    u8 r;
    s8 k;
    s8 f;
    struct InputState v;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    t = gCurTask;
    a = t->unk8C;
    r = 0;
    i = 4;
    k = t->unk7A;
    f = t->unk7B;
    ActorGetTerrainBox(&v);
    sub_0801bcac(&v);
    u = gCurTask;
    if ((u->unk7B & 0x80) != 0)
        CreateChildTaskAt(140, u->unk48, ((s16 *)gTerrainResult)[i], 0);
    if ((f & 1) != 0)
        goto b1;
    if ((f & 0x40) == 0)
        goto s1;
b1:
    if ((gCurTask->unk7B & 1) == 0 && (gCurTask->unk7B & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk0C;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s1:
    if (r == 1)
        return 1;
    if ((f & 1) == 0)
        goto b2;
    if ((f & 0x40) == 0)
        goto s2;
b2:
    if ((gCurTask->unk7B & 1) != 0 && (gCurTask->unk7B & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk08;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s2:
    if (r == 1)
        return 1;
    if (k == 1)
    {
        if ((gCurTask->unk7A & 1) != 0)
            goto s3;
        fn = ((struct ActorHandlers *)a->unk54)->unk04;
    }
    else
    {
        if ((gCurTask->unk7A & 1) == 0)
            goto s3;
        fn = ((struct ActorHandlers *)a->unk54)->unk00;
    }
    if (fn != 0)
        r = ((u8 (*)(void))fn)();
s3:
    if (r == 1)
        return 1;
    if ((gTerrainResult[0] & 3) != 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk10;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    if (r == 1)
        return 1;
    if ((gCurTask->unk7A & 1) != 0 && (gTerrainResult[3] & 1) != 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk14;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    if (r == 1)
        return 1;
    if ((gCurTask->unk7A & 1) == 0 && (gTerrainResult[1] & 1) != 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk18;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    return r;
}

u32 sub_080694e0(void)
{
    struct InputState v;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    ActorGetTerrainBox(&v);
    sub_0802205c(&v);
}

u32 sub_0806951c(void)
{
    struct InputState v;
    u32 r;

    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorGetTerrainBox(&v);
        sub_0801c230(&v);
        if ((*(u32 *)gTerrainResult & 0x00FFFFFF) != 0)
            r = 1;
        else
            r = 0;
        return r;
    }
    return 0;
}

u32 sub_0806956c(void)
{
    struct InputState v;
    u32 r;

    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorGetTerrainBox(&v);
        sub_0801c030(&v);
        if ((*(u32 *)gTerrainResult & 0x00FFFF00) != 0)
            r = 1;
        else
            r = 0;
        return r;
    }
    return 0;
}

u32 sub_080695bc(void)
{
    struct InputState v;
    u32 r;

    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorGetTerrainBox(&v);
        sub_0801bf1c(&v);
        if (gTerrainResult[0] != 0)
            r = 1;
        else
            r = 0;
        return r;
    }
    return 0;
}

u32 sub_08069604(void)
{
    struct InputState v;
    u32 r;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    r = 0;
    ActorGetTerrainBox(&v);
    sub_0801c30c(&v);
    if (gTerrainResult[0] != 0 || gTerrainResult[4] != 0 || gTerrainResult[1] != 0)
        r = 1;
    return r;
}

u32 sub_08069660(void)
{
    struct InputState v;
    u32 r;

    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorGetTerrainBox(&v);
        r = (u8)sub_0801c3a4(&v);
    }
    else
    {
        r = 0;
    }
    return r;
}

u32 sub_080696a0(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    u32 fn;
    s32 i;
    u8 r;
    s8 k;
    s8 f;
    struct InputState v;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    t = gCurTask;
    a = t->unk8C;
    r = 0;
    i = 4;
    k = t->unk7A;
    f = t->unk7B;
    ActorGetTerrainBox(&v);
    sub_0801bde0(&v);
    sub_080b460c();
    u = gCurTask;
    if ((u->unk7B & 0x80) != 0)
        CreateChildTaskAt(140, u->unk48, ((s16 *)gTerrainResult)[i], 0);
    if ((f & 1) != 0)
        goto b1;
    if ((f & 0x40) == 0)
        goto s1;
b1:
    if ((gCurTask->unk7B & 1) == 0 && (gCurTask->unk7B & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk0C;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s1:
    if (r == 1)
        return 1;
    if ((f & 1) == 0)
        goto b2;
    if ((f & 0x40) == 0)
        goto s2;
b2:
    if ((gCurTask->unk7B & 1) != 0 && (gCurTask->unk7B & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk08;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s2:
    if (r == 1)
        return 1;
    if (k == 1)
    {
        if ((gCurTask->unk7A & 1) != 0)
            goto s3;
        fn = ((struct ActorHandlers *)a->unk54)->unk04;
    }
    else
    {
        if ((gCurTask->unk7A & 1) == 0)
            goto s3;
        fn = ((struct ActorHandlers *)a->unk54)->unk00;
    }
    if (fn != 0)
        r = ((u8 (*)(void))fn)();
s3:
    if (r == 1)
        return 1;
    if ((gTerrainResult[0] & 3) != 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk10;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    if (r == 1)
        return 1;
    if ((gCurTask->unk7A & 1) != 0 && (gTerrainResult[3] & 1) != 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk14;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    if (r == 1)
        return 1;
    if ((gCurTask->unk7A & 1) == 0 && (gTerrainResult[1] & 1) != 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk18;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    return r;
}

u32 sub_08069888(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    u32 fn;
    s32 i;
    u8 r;
    s8 k;
    s8 f;
    struct InputState v;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    t = gCurTask;
    a = t->unk8C;
    r = 0;
    i = 4;
    k = t->unk7A;
    f = t->unk7B;
    ActorGetTerrainBox(&v);
    sub_0801c12c(&v);
    u = gCurTask;
    if ((u->unk7B & 0x80) != 0)
        CreateChildTaskAt(140, u->unk48, ((s16 *)gTerrainResult)[i], 0);
    if ((f & 1) == 0)
        goto b1;
    if ((f & 0x40) == 0)
        goto s1;
b1:
    if ((gCurTask->unk7B & 1) != 0 && (gCurTask->unk7B & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk08;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s1:
    if (r == 1)
        return 1;
    if (k == 1)
    {
        if ((gCurTask->unk7A & 1) != 0)
            goto s2;
        fn = ((struct ActorHandlers *)a->unk54)->unk04;
    }
    else
    {
        if ((gCurTask->unk7A & 1) == 0)
            goto s2;
        fn = ((struct ActorHandlers *)a->unk54)->unk00;
    }
    if (fn != 0)
        r = ((u8 (*)(void))fn)();
s2:
    if (r == 1)
        return 1;
    return r;
}

u32 sub_080699a8(void)
{
    struct Task *t;
    struct Task *u;
    s16 y;
    s16 m;
    s16 d;

    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        t = gCurTask;
        if ((t->unk24 & 0xFFFF0000) != 0
         && (t->unk7A & 1) != 0
         && (gTerrainResult[4] == 1 || gTerrainResult[4] == 2
          || gTerrainResult[4] == 3 || gTerrainResult[4] == 4))
        {
            if (t->unk54 >= 0)
            {
                m = t->unk48 - 16;
                y = m | 0xF;
            }
            else
            {
                m = t->unk48 + 16;
                y = m & 0xFFF0;
            }
            u = gCurTask;
            d = y - u->unk48;
            u->unk48 = y + d;
            u->unk4A = u->unk24;
            u->unk4C = u->unk48 << 16;
            u->unk50 = u->unk4A << 16;
            return 1;
        }
    }
    return 0;
}

void ActorGetTerrainBox(struct InputState *out)
{
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->unk8C;
    out->unk01 = ((struct InputState *)a->unk50)->unk01;
    out->unk02 = ((struct InputState *)a->unk50)->unk02;
    out->unk03 = ((struct InputState *)a->unk50)->unk03;
    if (t->unk43 == -1)
    {
        out->unk00 = -((struct InputState *)a->unk50)->unk00;
        out->unk04 = -((struct InputState *)a->unk50)->unk05;
        out->unk05 = -((struct InputState *)a->unk50)->unk04;
    }
    else
    {
        out->unk00 = ((struct InputState *)a->unk50)->unk00;
        out->unk04 = ((struct InputState *)a->unk50)->unk04;
        out->unk05 = ((struct InputState *)a->unk50)->unk05;
    }
}

void sub_08069ac4(s32 i)
{
    struct Task *t;

    t = &gTasks[i];
    TaskInitWaterFlagsSlot(i);
    t->unk7A = 1;
}

u32 sub_08069ae4(s8 a)
{
    u32 r;

    r = 0;
    switch (a)
    {
    case 1:
        r = sub_0806a25c();
        break;
    case 3:
        r = sub_0806bb7c();
        break;
    case 4:
        r = sub_0806bb7c();
        break;
    case 2:
    case 5:
        r = sub_08069ea0();
        break;
    case 6:
    case 7:
    case 8:
        break;
    }
    return r;
}

u32 ActorReactToHit(void)
{
    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    return sub_08069ae4(gCurTask->unk7C);
}

u32 sub_08069b84(void)
{
    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    return sub_08069ae4(sub_08069c48());
}

u32 sub_08069bbc(void)
{
    u32 r;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    gCurTask->unk43 = 1;
    r = 0;
    switch ((s8)gCurTask->unk7C)
    {
    case 1:
    case 2:
    case 5:
    case 7:
        /* sub_0806a158 is void in the ROM but its result is consumed here:
           the original had no prototype in scope at this point, so the
           implicit `int ()` declaration was used.  The cast reproduces the
           direct `bl` without tripping -Wimplicit -Werror. */
        r = ((u32 (*)(void))sub_0806a158)();
        break;
    case 3:
    case 4:
        r = sub_0806bb7c();
        break;
    case 6:
    case 8:
        break;
    }
    return r;
}

s8 sub_08069c48(void)
{
    struct Task *t;
    u8 *g;
    u8 v;
    s32 c;

    t = gCurTask;
    if ((s8)t->unk7C != 0)
    {
        v = t->unk7C;
    }
    else
    {
        g = gTerrainResult;
        if (g[12] != 0)
        {
            v = g[12];
            c = ((s8 *)g)[12];
            t->unk7C = c;
            gCurTask->unk82 = 0;
        }
        else
        {
            v = 0;
        }
    }
    return v;
}

void sub_08069c8c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk8C->unk06 == 16 || t->unk8C->unk06 == 32)
    {
        u = &gTasks[t->unk7E];
        switch (u->unk80)
        {
        case 3:
            PlaySfx(145);
            break;
        case 4:
            PlaySfx(146);
            break;
        case 9:
            PlaySfx(132);
            break;
        case 12:
            PlaySfx(139);
            break;
        case 13:
        case 14:
            PlaySfx(142);
            break;
        case 0:
        case 1:
        case 2:
        case 5:
        case 6:
        case 7:
        case 8:
        case 10:
        case 11:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
            sub_08069d78();
            break;
        }
    }
    else
    {
        sub_08069d78();
    }
}

void sub_08069d78(void)
{
    switch (gCurTask->unk72)
    {
    case 0:
    case 3:
    case 4:
    case 5:
        PlaySfx(127);
        break;
    case 1:
    case 2:
        PlaySfx(508);
        break;
    }
}

void sub_08069dc4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;

    t = gCurTask;
    a = t->unk8C;
    TaskSetSkipMask(7, gCurTaskIdx);
    u = gCurTask;
    u->unk4C = u->unk48 << 16;
    u->unk50 = u->unk4A << 16;
    a->unk14 = u->unk3C;
    TaskSetFrame(0);
    gCurTask->unk08 = (u32)sub_08069fb0;
    a->unk01 = 11;
    v = gCurTask;
    v->unk8C->unk22 = v->unk40 & 0xF000;
    if (v->unk82 > 3)
        v->unk82 = 0;
    sub_0806df28(gCurTask->unk82, 1);
}

void sub_08069e48(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;

    t = gCurTask;
    a = t->unk8C;
    TaskSetSkipMask(0, gCurTaskIdx);
    u = gCurTask;
    u->unk08 = 0;
    u->unk48 = u->unk4C >> 16;
    u->unk4A = u->unk50 >> 16;
    u->unk3C = a->unk14;
    u->unk40 = (u->unk40 & 0xFFF) | u->unk8C->unk22;
}

u32 sub_08069ea0(void)
{
    struct Actor *a;
    struct ActorVt *p;
    u8 r;

    a = gCurTask->unk8C;
    p = (struct ActorVt *)a->unk5C;
    r = 0;
    sub_08069c8c();
    if ((gCurTask->unk72 == 1 || gCurTask->unk72 == 2) && a->unk05 != 2)
        sub_0800a42c();
    if (p != NULL)
    {
        if (p->unk00 != -1)
        {
            sub_08069dc4();
            r = 0;
        }
        else if (p->unk04 != 0)
        {
            r = ((u8 (*)(void))p->unk04)();
        }
        else
        {
            sub_0806ee2c();
        }
    }
    else
    {
        sub_0806ee2c();
    }
    return r;
}

void sub_08069f0c(void)
{
    struct Task *t;
    struct Actor *a;
    s16 j;

    t = gCurTask;
    a = t->unk8C;
    j = gUnk_0873E5A4[(s8)a->unk01] * 2;
    t->unk48 += gUnk_0873E58C[j];
    t->unk4A += gUnk_0873E58C[j + 1];
    if ((s8)--a->unk01 < 0)
        sub_08069e48();
}

void sub_08069f70(void)
{
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->unk8C;
    if ((a->unk01 & 1) == 0)
        t->unk40 = (t->unk40 & 0xFFF) | 0xF000;
    else
        t->unk40 = (t->unk40 & 0xFFF) | a->unk22;
}

void sub_08069fb0(void)
{
    ActorCheckHits();
    ActorReactToHit();
    sub_08069f70();
    sub_08069f0c();
}

void sub_08069fc8(void)
{
    if (gTaskSlotTypes[gCurTaskIdx] == 107 || gTaskSlotTypes[gCurTaskIdx] == 109
     || gTaskSlotTypes[gCurTaskIdx] == 137)
        sub_0806d77c();
    else
        sub_0806d65c();
}

void sub_0806a008(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk72 == 1 || t->unk72 == 2)
        gCurTask->unk43 = TaskGetFacingToward(t->unk7F);
}

s16 sub_0806a03c(void)
{
    s16 r;

    r = 0;
    switch ((s8)gCurTask->unk7D)
    {
    case 7:
        r += 64;
    case 6:
        r += 64;
    case 5:
        r += 64;
    case 4:
        r += 64;
    case 3:
        r += 64;
    case 2:
        r += 64;
    case 1:
        r += 64;
    case 0:
        break;
    }
    return r;
}

void sub_0806a0cc(void)
{
    u32 v;

    switch (RandomRange(3))
    {
    default:
        v = 167;
        break;
    case 0:
        v = 109;
        break;
    case 1:
        v = 166;
        break;
    }
    ActorPlaySfx(v, 0);
}

void sub_0806a0f0(s32 a)
{
    struct Task *t;
    struct Actor *b;

    t = gCurTask;
    b = t->unk8C;
    if (a == -2)
        t->unk18 = 0;
    else
        t->unk18 = a;
    b->unk05 = 2;
    ActorSetTerrainHandlers((u32)gUnk_0873F910);
    if (gCurTask->unk7A & 1)
        ActorSetState(1);
    else
        ActorSetState(0);
    TaskSetEntry(sub_0806b26c, gCurTaskIdx);
}

/* No return value: the ROM's epilogue is `pop {r0}; bx r0`.  Its caller
   sub_08069bbc nevertheless propagates whatever r0 holds - see the comment
   there. */
void sub_0806a158(void)
{
    struct Task *t;

    if (gCurTask->unk76 != 0)
        sub_080b54d0(gCurTaskIdx);
    t = gCurTask;
    switch (t->unk76)
    {
    case 1:
        if (gLocalPlayer == t->unk7E)
            PlaySfx(220);
        AddPlayerLives(1, gCurTask->unk7E);
        ActorDestroy();
        break;
    case 3:
        if (gLocalPlayer == t->unk7E)
            PlaySfx(198);
        sub_0804087c(gCurTask->unk7E);
        ActorDestroy();
        break;
    case 2:
        if (gLocalPlayer == t->unk7E)
            PlaySfx(198);
        ActorSetState(0);
        TaskSetEntry(sub_080b4240, gCurTaskIdx);
        break;
    case 4:
        if (gLocalPlayer == t->unk7E)
            PlaySfx(198);
        ActorSetState(1);
        TaskSetEntry(sub_080b4240, gCurTaskIdx);
        break;
    default:
        ActorDestroy();
        break;
    }
}

u32 sub_0806a25c(void)
{
    struct Actor *a;
    struct ActorVt *p;
    struct Task *t;
    u8 r;

    a = gCurTask->unk8C;
    p = (struct ActorVt *)a->unk5C;
    r = 0;
    sub_08069c8c();
    if (gCurTask->unk72 == 1 || gCurTask->unk72 == 2)
        sub_0800a42c();
    t = gCurTask;
    if (t->unk72 == 1)
        ActorAwardScore(t->unk7F, 1);
    else
        ActorAwardScore(t->unk7F, 2);
    if (p != NULL)
    {
        if (p->unk01 != -1)
        {
            TaskSetEntry(ActorDie, gCurTaskIdx);
            r = 1;
        }
        else
        {
            if (gCurTask->unk72 == 1)
            {
                if (gGameState != 19)
                    PlaySfx(510);
                else
                    PlaySfx(514);
            }
            if (p->unk08 != 0)
                r = ((u8 (*)(void))p->unk08)();
            else
                sub_0806ee2c();
        }
        a->unk05 = 2;
    }
    else
    {
        sub_0806ee2c();
    }
    if (a->unk0D == 0)
        a->unk1A = 0xFFFF;
    return r;
}
