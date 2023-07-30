.section ".text"

.global SwitchToTask
SwitchToTask:
    vmrs r2, fpexc
    vmrs r3, fpscr
    stmia r0!, {r0, r2-r14}
    vstmia r0, {d0-d15}

    ldmia r1!, {r0, r2-r14}
    vmsr fpexc, r2
    vmsr fpscr, r3
    vldmia r1, {d0-d15}

    bx lr
