#define EXCEPTION_DIVISION_BY_ZERO 0
#define EXCEPTION_UNDEFINED_INSTRUCTION 1
#define EXCEPTION_PREFETCH_ABORT 2
#define EXCEPTION_DATA_ABORT 3

.macro DefineExceptionHandler name, exception, pcOffset
.globl	\name
\name:
    ldr sp, =__exception_stack_start
	sub	lr, lr, #\pcOffset
	stmfd sp!, {lr}
	mrs	lr, spsr
	stmfd sp!, {lr}
	stmfd sp, {r0-r14}^
	sub	sp, sp, #4 * 15
	mov	r1, sp
	cps	#0x12
	mov	r2, sp
	mov	r3, lr
	cps	#0x1F
	mov	sp, r1
	stmfd sp!, {r2, r3}
	mov	r1, sp
	mov	r0, #\exception
	b ExceptionHandler
.endm

DefineExceptionHandler Exception_UndefinedInstruction_Handler, EXCEPTION_UNDEFINED_INSTRUCTION, 4
DefineExceptionHandler Exception_PrefetchAbort_Handler,        EXCEPTION_PREFETCH_ABORT,        4
DefineExceptionHandler Exception_DataAbort_Handler,            EXCEPTION_DATA_ABORT,            8

.globl Exception_Irq_Handler

Exception_Irq_Handler:
    sub   lr, lr, #4
    srsdb sp!, #0x13
    cpsid if, #0x13
    push  {r0-r3, r12, lr}
    and   r1, sp, #4
    sub   sp, sp, r1
    push  {r1}
    bl    InterruptHandler
    pop   {r1}
    add   sp, sp, r1
    pop   {r0-r3, r12, lr}
    rfeia sp!