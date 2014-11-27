
.equ locked,1
.equ unlocked,0

.global mutex_lock
mutex_lock:
    ldr r2, =locked
    swp r1, r2, [r0] @ swap R2 with location [R0], [R0] value placed in R1
    cmp r1, r2
    beq mutex_lock
    mov pc, lr

.global mutex_unlock
mutex_unlock:
    ldr r1, =unlocked
    str r1, [r0]
    mov pc, lr
