
.equ locked,1
.equ unlocked,0

.global spinlock_lock
spinlock_lock:
    ldr r2, =locked
    swp r1, r2, [r0] @ swap R2 with location [R0], [R0] value placed in R1
    cmp r1, r2
    beq spinlock_lock
    mov pc, lr

.global spinlock_unlock
spinlock_unlock:
    ldr r1, =unlocked
    str r1, [r0]
    mov pc, lr
