qemu-system-arm -M raspi0 \
        -kernel debug/Kernel/Kernel.elf \
        -m 512M \
        -serial stdio \
        -monitor telnet:127.0.0.1:1235,server,nowait \
        -device usb-kbd \
        -device usb-mouse
