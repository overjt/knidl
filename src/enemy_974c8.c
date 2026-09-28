#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "camera.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern s32 PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
extern void RequestScreenShake(s32 a);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(void *p);
extern void ActorSetAttackBox(void *p);
extern void sub_08063a00(void *p);
extern void sub_080689c8(s32 i, s32 d);
extern u32 ActorCheckHits(void);
extern u32 sub_08068f68(void);
extern u32 sub_0806914c(void *p);
extern u8 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
extern u8 sub_0806acf8(void);

void FireLionFlameCheckParent(void)
{
    if (gTaskSlotTypes[gCurTask->parent] != 55)
        TaskFree(gCurTaskIdx);
}

void Task_PhanPhan(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)sub_08065438;
    t->layer = 11;
    gCurTask->frameTable = gPhanPhanFrames;
    gUnk_02007D00[8]++;
    sub_08066088(0);
    sub_08097580();
    ActorCollideTerrain();
    sub_080666cc(gUnk_08744888);
    if (sub_08067060() != 0) {
        gCurTask->updateCallback = (u32)sub_080975c8;
        sub_08097694();
    } else {
        gCurTask->updateCallback = (u32)sub_080975fc;
        sub_08066580();
        ActorSetState(1);
        sub_0809773c();
    }
}

void sub_08097580(void)
{
    struct Task *t;

    sub_08063a00(gUnk_08745238);
    TaskFaceNearestPlayer();
    t = gCurTask;
    t->unk28 = 0;
    t->unk2C = 0;
    t->unk1C = 0;
    t->unk18 = -1;
    sub_08066ae0();
}

void sub_080975ac(void)
{
    CallTableEntry(gCurTask->state, 9, gUnk_0874489C);
}

void sub_080975c8(void)
{
    CallTableEntry(gCurTask->updateState, 9, gUnk_087448C0);
    if (gCurTask->unk2C != 0)
        sub_080984b4();
    sub_08068f68();
    ActorReactToHit();
}

void sub_080975fc(void)
{
    struct Task *t;

    t = gCurTask;
    if ((t->hitTimer != 0 && t->u8C.actor->hitState != 0) || t->state == 8)
        ActorFlashPalette(gUnk_082BFBA4, 16);
    else
        sub_08066468();
    if (sub_0806acf8() == 0) {
        if (ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 9, gUnk_087448C0);
    } else {
        CallTableEntry(gCurTask->updateState, 9, gUnk_087448C0);
    }
    if (gCurTask->unk2C != 0)
        sub_080984b4();
    sub_08068f68();
    ActorReactToHit();
}

void sub_08097694(void)
{
    struct Task *t;

    ActorSetState(0);
    gCurTask->updateState = 0;
    gCurTask->onGround = 0;
    gCurTask->unk28 = 0;
    TaskSetFrame(18);
    gCurTask->accelY = 0x2500;
    TaskYieldTrampoline(24);
    t = gCurTask;
    t->updateCallback = (u32)sub_080975fc;
    if (t->unk28 == 0) {
        do {
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 == 0);
    }
    PlaySfx(0x1F7);
    RequestScreenShake(2);
    TaskSetFrame(19);
    TaskYieldTrampoline(24);
    sub_08066580();
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08097714(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(sub_080975ac, gCurTaskIdx);
}

void sub_0809773c(void)
{
    struct Task *t;
    s32 r;
    s16 v;

    gCurTask->updateState = 1;
    gCurTask->unk2C = 1;
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_087448E4[gCurTask->unk74]) {
        TaskFaceNearestPlayer();
        TaskStop();
        v = gUnk_087448E6[gCurTask->unk74];
        if (abs(TaskGetNearestPlayerDx()) <= v) {
            gUnk_030023D4 = r = RandomRange(8);
            if (r > 3)
                goto anim;
            sub_0809780c();
        } else {
            gUnk_030023D4 = r = RandomRange(8);
            if (r > 3) {
                if (r <= 4) {
anim:
                    sub_08097844();
                } else {
                    sub_0809794c();
                }
            }
        }
        gCurTask->unk6C++;
    }
    sub_080983a4();
    TaskSleepForever();
}

void sub_0809780c(void)
{
    TaskSetFrame(4);
    TaskYieldTrampoline(8);
    TaskSetFrame(5);
    TaskYieldTrampoline(8);
    TaskSetFrame(6);
    TaskYieldTrampoline(8);
    TaskSetFrame(7);
    TaskYieldTrampoline(8);
}

void sub_08097844(void)
{
    struct Task *t;
    struct Task *u;

    TaskStop();
    TaskSetFrame(22);
    TaskYieldTrampoline(5);
    TaskSetFrame(26);
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    t = gCurTask;
    t->unk28 = 0;
    TaskSetMotionXFacing(-gUnk_087448EC[t->unk74], 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 0x8000, 0x30000);
    TaskSetFrame(23);
    TaskYieldTrampoline(4);
    TaskSetFrame(25);
    TaskYieldTrampoline(4);
    while (gCurTask->unk28 == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(24);
    TaskYieldTrampoline(5);
    TaskSetFrame(27);
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    u = gCurTask;
    u->unk28 = 0;
    TaskSetMotionXFacing(-gUnk_087448EC[u->unk74], 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 0x8000, 0x30000);
    TaskSetFrame(19);
    TaskYieldTrampoline(4);
    TaskSetFrame(17);
    TaskYieldTrampoline(4);
    while (gCurTask->unk28 == 0)
        TaskYieldTrampoline(1);
}

void sub_0809794c(void)
{
    struct Task *t;
    struct Task *u;

    TaskStop();
    TaskSetFrame(22);
    TaskYieldTrampoline(5);
    TaskSetFrame(26);
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    t = gCurTask;
    t->unk28 = 0;
    TaskSetMotionXFacing(gUnk_087448EC[t->unk74], 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 0x8000, 0x30000);
    TaskSetFrame(23);
    TaskYieldTrampoline(4);
    TaskSetFrame(25);
    TaskYieldTrampoline(4);
    while (gCurTask->unk28 == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(24);
    TaskYieldTrampoline(5);
    TaskSetFrame(27);
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    u = gCurTask;
    u->unk28 = 0;
    TaskSetMotionXFacing(gUnk_087448EC[u->unk74], 0x5A5A5A5A);
    TaskSetMotionY(-0x20000, 0x8000, 0x30000);
    TaskSetFrame(19);
    TaskYieldTrampoline(4);
    TaskSetFrame(17);
    TaskYieldTrampoline(4);
    while (gCurTask->unk28 == 0)
        TaskYieldTrampoline(1);
}

void sub_08097a54(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(sub_080975ac, gCurTaskIdx);
}

void sub_08097a7c(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    gCurTask->updateState = 2;
    gCurTask->unk2C = 1;
    TaskStop();
    TaskFaceNearestPlayer();
    n = gUnk_087448F4[gCurTask->unk74 * 2 + RandomRange(2)];
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < n) {
        gCurTask->onGround = 0;
        u = gCurTask;
        u->unk28 = 0;
        TaskSetMotionY(-0x50000, 0x5000, 0x30000);
        TaskSetFrame(17);
        TaskYieldTrampoline(16);
        TaskSetFrame(18);
        TaskYieldTrampoline(14);
        TaskSetFrame(19);
        TaskYieldTrampoline(2);
        while (gCurTask->unk28 == 0)
            TaskYieldTrampoline(1);
        PlaySfx(0x1F7);
        RequestScreenShake(2);
        gCurTask->unk6C++;
    }
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08097b4c(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(sub_080975ac, gCurTaskIdx);
}

void sub_08097b74(void)
{
    struct Task *t;
    s32 v;

    gCurTask->updateState = 3;
    gCurTask->unk2C = 1;
    TaskSetMotionXFacing(-0x30000, 0x3000);
    ActorStopAnim();
    TaskSetFrame(11);
    TaskYieldTrampoline(5);
    TaskSetFrame(12);
    TaskYieldTrampoline(4);
    TaskSetFrame(13);
    TaskYieldTrampoline(3);
    TaskSetFrame(14);
    TaskYieldTrampoline(2);
    TaskSetFrame(15);
    TaskYieldTrampoline(2);
    TaskStop();
    v = ActorStartAnim(gUnk_08744900);
    t = gCurTask;
    t->unk24 = v;
    if (t->unk74 == 0)
        TaskYieldTrampoline(16);
    PlaySfx(500);
    CreateDustTrail(1, 3, 8, 10);
    TaskSetMotionXFacing(gUnk_087448F8[gCurTask->unk74], 0x5A5A5A5A);
    TaskYieldTrampoline(48);
    TaskStop();
    ActorStopAnim();
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08097c44(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->state != 3) {
        TaskSetEntry(sub_080975ac, gCurTaskIdx);
    } else {
        gCurTask->unk24 = ActorTickAnim(t->unk24);
    }
}

void sub_08097c78(void)
{
    struct Task *t;
    s32 zero;

    gCurTask->updateState = 4;
    zero = 0;
    PlaySfx(0x1F7);
    RequestScreenShake(4);
    ActorSetAttackBox(gUnk_08745270);
    sub_08063a00(gUnk_0874528C);
    t = gCurTask;
    t->unk2C = zero;
    t->onGround = zero;
    gCurTask->unk28 = zero;
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskSetMotionY(-0x30000, 0x2500, 0x30000);
    TaskSetFrame(28);
    while (gCurTask->unk28 == 0)
        TaskYieldTrampoline(1);
    RequestScreenShake(4);
    PlaySfx(0x1F7);
    CreateDustTrail(0, 1, 8, 10);
    TaskSetMotionXFacing(-0x18000, 0x5A5A5A5A);
    TaskSetFrame(29);
    TaskYieldTrampoline(2);
    TaskStop();
    if (gCurTask->unk74 == 0)
        TaskYieldTrampoline(40);
    ActorSetAttackBox(gUnk_0874521C);
    sub_08063a00(gUnk_08745238);
    TaskSetFrame(14);
    TaskYieldTrampoline(4);
    TaskSetFrame(15);
    TaskYieldTrampoline(3);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08097d7c(void)
{
    if (gCurTask->state != 4)
        TaskSetEntry(sub_080975ac, gCurTaskIdx);
}

void sub_08097da4(void)
{
    struct Task *t;
    s32 zero;

    gCurTask->updateState = 5;
    zero = 0;
    TaskFaceNearestPlayer();
    gCurTask->onGround = zero;
    t = gCurTask;
    t->unk28 = zero;
    t->unk2C = 1;
    TaskSetMotionXFacing(gUnk_08744924[t->unk74], 0x5A5A5A5A);
    TaskSetMotionY(-0x48000, 0x2500, 0x30000);
    TaskSetFrame(17);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    TaskSetFrame(18);
    while (gCurTask->unk28 == 0)
        TaskYieldTrampoline(1);
    PlaySfx(0x1F7);
    RequestScreenShake(2);
    CreateLandingDust(16, 12);
    TaskStop();
    TaskSetFrame(19);
    TaskYieldTrampoline(8);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08097e68(void)
{
    if (gCurTask->state != 5)
        TaskSetEntry(sub_080975ac, gCurTaskIdx);
}

void sub_08097e90(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 zero;
    s32 d;
    s32 k;

    gCurTask->updateState = 6;
    zero = 0;
    TaskStop();
    gCurTask->unk2C = zero;
    ActorStopAnim();
    TaskSetFrame(40);
    t = gCurTask;
    if (t->onGround == 0) {
        t->unk28 = zero;
        TaskSetMotionY(0, 0x2500, 0x30000);
        gCurTask->unk6C = zero;
        do {
            TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            u = gCurTask;
            u->unk6C++;
        } while ((s16)u->unk6C <= 3);
        TaskStopX();
        while (gCurTask->unk28 == 0)
            TaskYieldTrampoline(1);
        TaskYieldTrampoline(20);
    } else {
        t->unk6C = zero;
        do {
            TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            TaskYieldTrampoline(2);
            v = gCurTask;
            v->unk6C++;
        } while ((s16)v->unk6C <= 3);
        TaskStopX();
        TaskYieldTrampoline(4);
    }
    gUnk_030023D4 = d = gCurTask->pixelX - gViewRect[0];
    k = gCurTask->facing;
    if (120 - d >= 0)
        goto chk2;
    if (k <= 0)
        goto yes;
    goto other;
chk2:
    if (k < 0)
        goto other;
yes:
    sub_0809809c();
    sub_08068920(gCurTask->unk18, 8);
    if (gLocalPlayer == gCurTask->unk18)
        PlaySfx(0x23A);
    gCurTask->unk18 = -1;
    TaskSetFrame(32);
    TaskYieldTrampoline(4);
    TaskSetFrame(39);
    TaskYieldTrampoline(8);
    goto tail;
other:
    sub_08098140();
    sub_08068920(gCurTask->unk18, 9);
    if (gLocalPlayer == gCurTask->unk18)
        PlaySfx(0x23A);
    gCurTask->unk18 = -1;
    TaskSetFrame(43);
    TaskYieldTrampoline(4);
    TaskSetFrame(44);
    TaskYieldTrampoline(8);
tail:
    TaskSetFrame(4);
    TaskYieldTrampoline(8);
    TaskSetFrame(5);
    TaskYieldTrampoline(8);
    TaskSetFrame(6);
    TaskYieldTrampoline(8);
    TaskSetFrame(7);
    TaskYieldTrampoline(8);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_0809809c(void)
{
    struct Task *t;

    gCurTask->unk6C = 0;
    do {
        if (gLocalPlayer == gCurTask->unk18)
            PlaySfx(0x239);
        TaskSetFrame(30);
        TaskYieldTrampoline(4);
        TaskSetFrame(34);
        TaskYieldTrampoline(2);
        TaskSetFrame(31);
        TaskYieldTrampoline(1);
        TaskSetFrame(35);
        TaskYieldTrampoline(2);
        TaskSetFrame(32);
        TaskYieldTrampoline(4);
        TaskSetFrame(36);
        TaskYieldTrampoline(4);
        TaskSetFrame(33);
        TaskYieldTrampoline(5);
        TaskSetFrame(37);
        TaskYieldTrampoline(4);
        t = gCurTask;
        t->unk6C++;
    } while ((s16)t->unk6C <= 2);
}

void sub_08098140(void)
{
    TaskSetFrame(40);
    TaskYieldTrampoline(8);
    TaskSetFrame(41);
    TaskYieldTrampoline(8);
    TaskSetFrame(42);
    TaskYieldTrampoline(4);
}

void sub_0809816c(void)
{
    if (gCurTask->state != 6)
        TaskSetEntry(sub_080975ac, gCurTaskIdx);
}

void sub_08098194(void)
{
    s32 zero;

    gCurTask->updateState = 7;
    zero = 0;
    gCurTask->unk2C = zero;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskYieldTrampoline(15);
    gCurTask->onGround = zero;
    gCurTask->unk28 = zero;
    gCurTask->unk24 = ActorStartAnim(gUnk_08744900);
    TaskSetMotionY(-0x38000, 0x2000, 0x30000);
    TaskYieldTrampoline(gUnk_0874492C[RandomRange(8)]);
    ActorStopAnim();
    PlaySfx(506);
    CreatePhanPhanApple();
    TaskSetFrame(14);
    TaskYieldTrampoline(3);
    TaskSetFrame(15);
    TaskYieldTrampoline(3);
    TaskSetFrame(8);
    TaskYieldTrampoline(3);
    TaskSetFrame(18);
    while (gCurTask->unk28 == 0)
        TaskYieldTrampoline(1);
    TaskSetFrame(19);
    TaskYieldTrampoline(8);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_08098268(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->state != 7) {
        TaskSetEntry(sub_080975ac, gCurTaskIdx);
    } else {
        gCurTask->unk24 = ActorTickAnim(t->unk24);
    }
}

void sub_0809829c(void)
{
    s32 zero;

    gCurTask->updateState = 8;
    zero = 0;
    TaskStop();
    gCurTask->unk2C = zero;
    gUnk_02007D00[8]--;
    if (gUnk_02007D00[8] <= 0)
        sub_0806684c();
    sub_080667c0(1, 28);
    CreateChildTaskHere(142, 0);
    ActorSetHitReactions(gUnk_0874544C);
    gCurTask->onGround = zero;
    gCurTask->unk28 = zero;
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    TaskSetMotionY(-0x30000, 0x2500, 0x30000);
    TaskSetFrame(28);
    ActorSetAttackBox(gUnk_08745254);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    ActorSetAttackBox(gUnk_087452A8);
    sub_08063a00(gUnk_087452C4);
    while (gCurTask->unk28 == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(29);
    RequestScreenShake(4);
    CreateChildTaskHere(141, 0);
    CreateDustTrail(0, 1, -4, 12);
    TaskYieldTrampoline(170);
    sub_0806ad18();
    ActorDie();
}

void sub_080983a0(void)
{
}

void sub_080983a4(void)
{
    struct Task *t;
    s32 k;
    s32 r;

    t = gCurTask;
    t->unk1C++;
    if (t->unk1C > 2) {
        t->unk1C = 0;
        ActorSetState(7);
        return;
    }
    k = 0;
    gUnk_030023D4 = gPlayerStates[TaskFindNearestPlayer()].mode;
    switch (gUnk_030023D4) {
    case 14:
        k = 2;
        break;
    case 4:
    case 5:
        k = 1;
        break;
    }
    if (gCurTask->unk74 != 0)
        k += 3;
    gUnk_030023D4 = r = RandomRange(8);
    if (r < gUnk_08744934[k]) {
        ActorSetState(3);
        return;
    }
    if (r < gUnk_0874494C[k]) {
        ActorSetState(5);
        return;
    }
    ActorSetState(2);
}

void CreatePhanPhanApple(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct ActorSpawn *p;
    s8 d;

    sp.subtype = 35;
    sp.taskType = 138;
    p = &sp;
    t = gCurTask;
    p->variant = t->variant;
    p->spawnArg = t->unk74;
    p->tileWord = t->u8C.actor->savedTileWord;
    p->x = 12;
    p->y = 8;
    p->checkTerrain = 1;
    d = t->facing;
    TaskFaceNearestPlayer();
    gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&sp, 1);
    gCurTask->facing = d;
}

void sub_080984b4(void)
{
    struct Task *t;
    struct PlayerState *p;
    s32 i;

    if (sub_0806914c(gUnk_087452E0) != 0) {
        p = gPlayerStates;
        t = gCurTask;
        i = t->hitterSlot;
        if (p[i].ability != 17 || p[i].mode != 13) {
            t->unk18 = i;
            TaskFaceToward(i);
            PlaySfx(568);
            sub_080685ec(gCurTask->unk18, gCurTaskIdx, 7);
            ActorSetState(6);
            TaskSetEntry(sub_080975ac, gCurTaskIdx);
        }
    }
}

s32 sub_08098528(void)
{
    TaskStopY();
    gCurTask->unk28 = 1;
    return 0;
}

s32 sub_08098540(void)
{
    struct Task *t;
    s32 d;

    t = gCurTask;
    if (t->state != 3)
        return 0;
    d = t->facing;
    if (t->velX < 0) {
        if (d < 0)
            goto hit;
        goto miss;
    }
    if (d <= 0)
        goto miss;
hit:
    ActorSetState(4);
    return 1;
miss:
    TaskStopX();
    return 0;
}

s32 sub_0809857c(void)
{
    CreateChildTaskHere(142, 0);
    RequestScreenShake(2);
    return 0;
}

s32 sub_08098594(void)
{
    struct Task *t;

    ActorFaceHitter();
    t = gCurTask;
    if (t->unk18 >= 0) {
        sub_080689c8(t->unk18, -t->facing);
        gCurTask->unk18 = -1;
    }
    ActorSetState(8);
    TaskSetEntry(sub_080975ac, gCurTaskIdx);
    return 1;
}

void Task_GrandWheelieMiniWheelie(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    t->updateCallback = (u32)sub_0809869c;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    zero = 0;
    gCurTask->frameTable = gGrandWheelieMiniWheelieFrames;
    TaskFaceNearestPlayer();
    TaskStop();
    TaskSetMotionXFacing(gUnk_087454D8[0] >> 1, 0x5A5A5A5A);
    TaskSetMotionY(gUnk_087454EC[gCurTask->unk74], 0x3000, 0x30000);
    gCurTask->onGround = zero;
    gCurTask->unk2C = zero;
    while (1) {
        u = gCurTask;
        switch (u->unk2C) {
        case 0:
            if (u->u8C.actor->animScript != gUnk_087454B8)
                gCurTask->unk28 = ActorStartAnim(gUnk_087454B8);
            break;
        case 1:
            if (u->u8C.actor->animScript != gUnk_087454C4)
                gCurTask->unk28 = ActorStartAnim(gUnk_087454C4);
            break;
        }
        TaskYieldTrampoline(1);
    }
}

void sub_0809869c(void)
{
    struct Task *t;
    s32 x;

    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
    x = ActorTickAnim(gCurTask->unk28);
    t = gCurTask;
    t->unk28 = x;
    if (t->unk2C != 0 && sub_08098748() != 0)
        TaskSetMotionXFacing(gUnk_087454D8[gCurTask->unk1C], 0x5A5A5A5A);
}

void sub_080986ec(void)
{
    struct Task *t;

    TaskStopY();
    t = gCurTask;
    t->unk1C = -1;
    t->unk2C = 1;
}

void sub_08098708(void)
{
    gCurTask->unk2C = 0;
}

void sub_08098718(void)
{
    sub_0806a0f0(-2);
}

void sub_08098728(void)
{
    RequestScreenShake(1);
    ActorReactToDefeat();
}

void sub_08098738(void)
{
    struct Task *t;

    t = gCurTask;
    t->velY = -t->velY;
}

s32 sub_08098748(void)
{
    struct Task *t;
    s32 v;

    v = sub_08094d10();
    t = gCurTask;
    if (t->unk1C != (s8)v) {
        t->unk1C = (s8)v;
        return 1;
    }
    return 0;
}

void Task_PhanPhanApple(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    struct Task *y;
    struct Task *z;
    struct Task *q;
    s32 zero;
    s32 v;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    zero = 0;
    u = gCurTask;
    u->frameTable = gPhanPhanAppleFrames;
    u->updateCallback = (u32)PhanPhanAppleUpdate;
    TaskFaceNearestPlayer();
    v = ActorStartAnim(gUnk_0874550C);
    w = gCurTask;
    w->unk28 = v;
    w->onGround = zero;
    y = gCurTask;
    y->unk34 = zero;
    TaskSetMotionXFacing(gUnk_087454F4[y->unk74], 0x5A5A5A5A);
    TaskSetMotionY(0x2AF00, 0x3000, 0x30000);
    while (gCurTask->unk34 == 0)
        TaskYieldTrampoline(1);
    gCurTask->onGround = 0;
    z = gCurTask;
    z->unk34 = 0;
    TaskSetMotionXFacing(gUnk_087454FC[z->unk74], 0x5A5A5A5A);
    TaskSetMotionY(gUnk_08745504[gCurTask->unk74], 0x3000, 0x30000);
    while (gCurTask->unk34 == 0)
        TaskYieldTrampoline(1);
    while (1) {
        gCurTask->onGround = 0;
        q = gCurTask;
        q->unk34 = 0;
        q->velY = -0x30000;
        while (gCurTask->unk34 == 0)
            TaskYieldTrampoline(1);
    }
}

void PhanPhanAppleUpdate(void)
{
    struct Task *t;
    s32 x;

    x = ActorTickAnim(gCurTask->unk28);
    t = gCurTask;
    t->unk28 = x;
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

s32 sub_080988a4(void)
{
    gCurTask->unk34 = 1;
    return 0;
}

s32 sub_080988b4(void)
{
    ActorReactToDefeat();
    return 1;
}

void sub_080988c0(void)
{
}

s32 sub_080988c4(void)
{
    if (gCurTask->state == 0) {
        ActorSetState(16);
        TaskSetEntry(MrFrostyEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}
