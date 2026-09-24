    .syntax unified
    .cpu arm7tdmi
    .thumb
    push {r4, r5, r6, r7, lr}
    mov r7, sl
    mov r6, r9
    mov r5, r8
    push {r5, r6, r7}
    sub sp, #8
    ldr r3, [pc, #108]
    adds r1, r3, #0
    movs r2, #0
    adds r0, r1, #0
    adds r0, #36
.L_08018e2a:
    str r2, [r0, #0]
    subs r0, #4
    cmp r0, r1
    bge.n .L_08018e2a
    ldr r0, [pc, #96]
    ldr r1, [r0, #0]
    adds r1, #108
    movs r2, #0
    strh r2, [r1, #0]
.L_08018e3c:
    movs r0, #0
    mov r8, r0
    movs r1, #12
    mov sl, r1
    movs r7, #0
    movs r2, #24
    mov r9, r2
.L_08018e4a:
    ldr r3, [pc, #72]
    ldr r0, [r3, #0]
    adds r0, #108
    movs r4, #0
    ldrsh r0, [r0, r4]
    ldr r3, [pc, #56]
    cmp r0, #49
    bgt.n .L_08018ec4
    adds r4, r7, r3
    ldr r1, [r4, #0]
    cmp r1, #0
    bne.n .L_08018ec4
    mov r2, r9
    adds r0, r2, r3
    str r1, [r0, #0]
    movs r0, #16
    bl sub_08002ee8
    adds r2, r0, #0
    ldr r3, [pc, #32]
    ldr r0, [r3, #0]
    adds r0, #67
    ldrb r0, [r0, #0]
    lsls r0, r0, #24
    asrs r0, r0, #24
    cmp r0, #1
    bne.n .L_08018e9c
    ldr r1, [pc, #20]
    lsls r2, r2, #1
    adds r0, r2, r1
    movs r3, #0
    ldrsh r0, [r0, r3]
    lsls r0, r0, #16
    b.n .L_08018eaa
    movs r0, r0
    .word gUnk_02006040
    .word gUnk_03002490
    .word 0x08732150
.L_08018e9c:
    ldr r1, [pc, #60]
    lsls r2, r2, #1
    adds r0, r2, r1
    movs r3, #0
    ldrsh r0, [r0, r3]
    lsls r0, r0, #16
    negs r0, r0
.L_08018eaa:
    str r0, [r4, #0]
    adds r0, r1, #0
    adds r3, r2, #0
    ldr r1, [pc, #44]
    mov r4, sl
    adds r2, r4, r1
    adds r0, #32
    adds r0, r3, r0
    movs r3, #0
    ldrsh r0, [r0, r3]
    lsls r0, r0, #16
    str r0, [r2, #0]
    adds r3, r1, #0
.L_08018ec4:
    adds r2, r7, r3
    ldr r1, [r2, #0]
    cmp r1, #0
    bge.n .L_08018ece
    negs r1, r1
.L_08018ece:
    movs r0, #160
    lsls r0, r0, #12
    cmp r1, r0
    bgt.n .L_08018ee4
    movs r0, #0
    str r0, [r2, #0]
    b.n .L_08018fa2
    .word 0x08732150
    .word gUnk_02006040
.L_08018ee4:
    ldr r0, [r2, #0]
    cmp r0, #0
    blt.n .L_08018ef8
    ldr r4, [pc, #8]
    adds r0, r0, r4
    str r0, [r2, #0]
    b.n .L_08018f06
    movs r0, r0
    .word 0xFFFFA000
.L_08018ef8:
    .short 0x4648
    adds r1, r0, r3
    ldr r0, [r1, #0]
    movs r2, #192
    lsls r2, r2, #7
    adds r0, r0, r2
    str r0, [r1, #0]
.L_08018f06:
    mov r0, r8
    adds r0, #6
    adds r1, r3, #0
    adds r5, r7, r1
    lsls r0, r0, #2
    adds r0, r0, r1
    ldr r1, [r5, #0]
    ldr r0, [r0, #0]
    adds r1, r1, r0
    str r1, [r5, #0]
    cmp r1, #0
    bge.n .L_08018f20
    negs r1, r1
.L_08018f20:
    asrs r0, r1, #20
    adds r2, r0, #2
    mov r4, sl
    adds r1, r4, r3
    ldr r3, [r1, #0]
    cmp r3, #0
    bge.n .L_08018f30
    negs r3, r3
.L_08018f30:
    ldr r4, [pc, #160]
    ands r4, r3
    lsrs r4, r2
    ldr r0, [r1, #0]
    cmp r0, #0
    ble.n .L_08018f3e
    negs r4, r4
.L_08018f3e:
    adds r6, r0, r4
    str r6, [r1, #0]
    ldr r5, [r5, #0]
    cmp r5, #0
    beq.n .L_08018fa2
    ldr r0, [pc, #140]
    ldr r4, [r0, #0]
    adds r3, r4, #0
    adds r3, #68
    movs r2, #0
    ldrsh r1, [r3, r2]
    lsls r0, r1, #3
    adds r0, r0, r1
    lsls r0, r0, #4
    ldr r1, [pc, #128]
    adds r0, r0, r1
    adds r0, #72
    movs r1, #0
    ldrsh r2, [r0, r1]
    asrs r0, r5, #16
    adds r2, r2, r0
    subs r2, #8
    ldr r0, [pc, #116]
    str r2, [r0, #0]
    ldr r5, [pc, #116]
    movs r0, #0
    ldrsh r1, [r3, r0]
    lsls r0, r1, #3
    adds r0, r0, r1
    lsls r0, r0, #4
    ldr r1, [pc, #96]
    adds r0, r0, r1
    adds r0, #74
    movs r3, #0
    ldrsh r1, [r0, r3]
    asrs r0, r6, #16
    adds r1, r1, r0
    adds r1, #16
    str r1, [r5, #0]
    adds r4, #66
    ldrb r0, [r4, #0]
    str r2, [sp, #0]
    lsls r1, r1, #16
    asrs r1, r1, #16
    str r1, [sp, #4]
    ldr r1, [pc, #76]
    movs r2, #0
    movs r3, #0
    bl sub_08001a94
.L_08018fa2:
    movs r4, #4
    add sl, r4
    adds r7, #4
    add r9, r4
    movs r0, #1
    add r8, r0
    mov r1, r8
    cmp r1, #2
    bgt.n .L_08018fb6
    b.n .L_08018e4a
.L_08018fb6:
    ldr r2, [pc, #32]
    ldr r1, [r2, #0]
    adds r1, #108
    ldrh r0, [r1, #0]
    adds r0, #1
    strh r0, [r1, #0]
    lsls r0, r0, #16
    asrs r0, r0, #16
    cmp r0, #60
    bgt.n .L_08018fec
    movs r0, #1
    bl TaskYieldTrampoline
    b.n .L_08018e3c
    movs r0, r0
    .word 0xFFFF0000
    .word gUnk_03002490
    .word gUnk_03002790
    .word gUnk_03001F2C
    .word gUnk_03002448
    .word gUnk_080D2148
.L_08018fec:
    bl TaskDispatchTrampoline
    add sp, #8
    pop {r3, r4, r5}
    mov r8, r3
    mov r9, r4
    mov sl, r5
    pop {r4, r5, r6, r7}
    pop {r0}
    bx r0
