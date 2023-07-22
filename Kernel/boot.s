.section ".text.boot"
.global _start

_start:
  ldr sp, =__binary_end
  add sp, sp, #0x20000 // 128KB stack, must match up with task stack size

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
