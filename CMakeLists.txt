cmake_minimum_required(VERSION 3.24)

project(SimpleSystem LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
add_subdirectory(Kernel)

set(QEMU_FLAGS
        -M raspi0
        -kernel ${CMAKE_BINARY_DIR}/Kernel/Kernel.elf
        -m 512M
        -serial stdio
        -monitor telnet:127.0.0.1:1235,server,nowait
        -device usb-kbd
        -device usb-mouse
        -semihosting
        )

add_custom_target(
        run
        COMMAND qemu-system-arm ${QEMU_FLAGS}
        DEPENDS Kernel
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(
        debug
        COMMAND qemu-system-arm ${QEMU_FLAGS} -s -S
        DEPENDS Kernel
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)