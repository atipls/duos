ENTRY(_start)

SECTIONS
{
    __text_start = .;
    . = 0x8000;
    .text : {
        KEEP(*(.text.boot))
        *(.text)
    }
    . = ALIGN(4096);
    __text_end = .;

    .rodata : {
        *(.rodata)
    }
    . = ALIGN(4096);

    .data : {
        *(.data)

        __exception_stack_end = .;
        . += 0x4000;
        __exception_stack_start = .;
    }
    . = ALIGN(4096);

    __bss_start = .;
    .bss :
    {
        bss = .;
        *(.bss)
    }
    . = ALIGN(4096);
    __bss_end = .;
    __bss_size = __bss_end - __bss_start;

    __binary_end = .;
}