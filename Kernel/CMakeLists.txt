set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

set(KERNEL_SOURCES
        boot.s
        Main.cpp
        board/Gpio.cpp
        board/Uart.cpp
        board/Interrupt.cpp
        board/Timer.cpp
        mem/Memory.cpp
        task/Task.cpp util/Runtime.cpp util/Runtime.h)

include_directories(.)
add_executable(Kernel ${KERNEL_SOURCES})

add_custom_command(
        TARGET Kernel POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} ./Kernel/Kernel${CMAKE_EXECUTABLE_SUFFIX} -O binary ./kernel.img
        COMMAND ${CMAKE_OBJDUMP} -l -S -D ./Kernel/Kernel${CMAKE_EXECUTABLE_SUFFIX} > ./kernel.asm
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Convert the ELF output file to a binary image" )