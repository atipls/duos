.section ".text"

.global SwitchToTask

SwitchToTask:
    push {lr}
    push {sp}
    mrs r12, cpsr
    push {r0-r12}
    str sp, [r0]

    ldr sp, [r1]
    pop {r0-r12}
    msr cpsr, r12
    pop {lr, pc}
