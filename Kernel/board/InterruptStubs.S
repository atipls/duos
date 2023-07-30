#define EXCEPTION_DIVISION_BY_ZERO 0
#define EXCEPTION_UNDEFINED_INSTRUCTION 1
#define EXCEPTION_PREFETCH_ABORT 2
#define EXCEPTION_DATA_ABORT 3

.macro DefineExceptionHandler name, exception, pcOffset
.global \name
\name:
    ldr sp, =__exception_stack_top
    sub lr, lr, #\pcOffset
    stmfd sp!, {lr}
    mrs lr, spsr
    stmfd sp!, {lr}
    stmfd sp, {r0-r14}^
    sub sp, sp, #4 * 15
    mov r1, sp
    cps #0x12
    mov r2, sp
    mov r3, lr
    cps #0x1F
    mov sp, r1
    stmfd sp!, {r2, r3}
    mov r1, sp
    mov r0, #\exception
    b ExceptionHandler
.endm

.section ".text"

DefineExceptionHandler Exception_UndefinedInstruction_Handler, EXCEPTION_UNDEFINED_INSTRUCTION, 4
DefineExceptionHandler Exception_PrefetchAbort_Handler,        EXCEPTION_PREFETCH_ABORT,        4
DefineExceptionHandler Exception_DataAbort_Handler,            EXCEPTION_DATA_ABORT,            8

.global Exception_Irq_Handler
Exception_Irq_Handler:
	ldr sp, =__interrupt_stack_top
    sub lr, lr, #4
    stmfd sp!, {r0-r12, lr}
    bl InterruptHandler
    ldmfd sp!, {r0-r12, pc}^