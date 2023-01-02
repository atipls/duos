.section ".text.boot"
.org 0x8000

.global _start

_start:
  mov sp, #0x8000

  ldr r4, =__bss_start
  ldr r9, =__bss_end
  mov r5, #0
  mov r6, #0
  mov r7, #0
  mov r8, #0
  b 2f

1:
  stmia r4!, {r5-r8}

2:
  cmp r4, r9
  blo 1b

  mov r2, #0x100
  bl KernelMain

halt:
    b halt

.section ".text"

.global MoveExceptionVectors
.global IrqHandlerWrapper
.global SwitchToThread
.global ExceptionVectors

ExceptionVectors:
  ldr pc, ResetHandlerAddress
  ldr pc, UndefinedInstructionHandlerAddress
  ldr pc, SoftwareInterruptHandlerAddress
  ldr pc, PrefetchAbortHandlerAddress
  ldr pc, DataAbortHandlerAddress
  nop // Reserved.
  ldr pc, IrqHandlerAddress
  ldr pc, FiqHandlerAddress

ResetHandlerAddress:                .word ResetHandler
UndefinedInstructionHandlerAddress: .word UndefinedInstructionHandler
SoftwareInterruptHandlerAddress:    .word SoftwareInterruptHandler
PrefetchAbortHandlerAddress:        .word PrefetchAbortHandler
DataAbortHandlerAddress:            .word DataAbortHandler
IrqHandlerAddress:                  .word IrqHandlerWrapper
FiqHandlerAddress:                  .word FiqHandler

MoveExceptionVectors:
    push  {r4-r9}
    ldr   r0, =ExceptionVectors
    mov   r1, #0x0000
    ldmia r0!,{r2-r9}
    stmia r1!,{r2-r9}
    ldmia r0!,{r2-r8}
    stmia r1!,{r2-r8}
    pop   {r4-r9}
    blx   lr

IrqHandlerWrapper:
    sub   lr, lr, #4
    srsdb sp!, #0x13
    cpsid if, #0x13
    push  {r0-r3, r12, lr}
    and   r1, sp, #4
    sub   sp, sp, r1
    push  {r1}
    bl    IrqHandler
    pop   {r1}
    add   sp, sp, r1
    pop   {r0-r3, r12, lr}
    rfeia sp!
