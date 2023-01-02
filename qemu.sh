qemu-system-arm \
    -M raspi0 \
    -append "earlyprintk earlycon=pl011,0x20201000 console=ttyAMA0 loglevel=8" \
    -kernel deploy/Kernel/Kernel.elf \
    -m 512M \
    -serial stdio \
    -nographic \
    -monitor telnet:127.0.0.1:1235,server,nowait \
;