.macro safe_svcmode_maskall reg:req
    mrs \reg, cpsr
    eor \reg, \reg, #0x1A
    tst \reg, #0x1F
    bic \reg, \reg, #0x1F
    orr \reg, \reg, #0xC0 | 0x13
    bne 1f
    orr \reg, \reg, #0x100
    adr lr, 2f
    msr spsr_cxsf, \reg
.word 0xE12EF30E            /* msr ELR_hyp, lr */
.word 0xE160006E            /* eret */
1:    msr    cpsr_c, \reg
2:
.endm

.text

.global _start
_start:
    safe_svcmode_maskall r0
    cps #0x1F
    ldr sp, =__kernel_stack_top

    // Enable the FPU
    ldr r0, =(0xF << 20)
    mcr p15, 0, r0, c1, c0, 2
    mov r3, #0x40000000
    vmsr fpexc, r3

    b KernelMain
