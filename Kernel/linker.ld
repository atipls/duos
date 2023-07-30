ENTRY(_start)

SECTIONS
{
    . = 0x8000;
    .text : {
        *(.text*)
    } . = ALIGN(4096);
    __text_end = .;

    .rodata : {
        *(.rodata)
    } . = ALIGN(4096);

    .data : {
        *(.data)
    } . = ALIGN(4096);

    __bss_start = .;
    .bss : {
        *(.bss*)
    }
    . = ALIGN(4096);
    __bss_end = .;

    . += 0x10000;
    __kernel_stack_top = .;
    . += 0x10000;
    __exception_stack_top = .;
    . += 0x10000;
    __interrupt_stack_top = .;
}