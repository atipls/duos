set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

set(USB_LIBRARY_SOURCES
        usb/uspilibrary.cpp
        usb/dwhcidevice.cpp
        usb/dwhciregister.cpp
        usb/dwhcixferstagedata.cpp
        usb/usbconfigparser.cpp
        usb/usbdevice.cpp
        usb/usbdevicefactory.cpp
        usb/usbendpoint.cpp
        usb/usbrequest.cpp
        usb/usbstandardhub.cpp
        usb/DeviceNameService.cpp
        usb/macaddress.cpp
        usb/usbfunction.cpp
        usb/smsc951x.cpp
        usb/lan7800.cpp
        usb/string.cpp
        usb/util.cpp
        usb/usbmassdevice.cpp
        usb/dwhciframeschednper.cpp
        usb/dwhciframeschedper.cpp
        usb/keymap.cpp
        usb/usbkeyboard.cpp
        usb/dwhcirootport.cpp
        usb/usbmouse.cpp
        usb/dwhciframeschednsplit.cpp
        usb/usbgamepad.cpp
        usb/synchronize.cpp
        usb/usbstring.cpp
        usb/usbmidi.cpp
        )

set(KERNEL_SOURCES
        board/Framebuffer.cpp
        board/Framebuffer.h
        board/Gpio.cpp
        board/Gpio.h
        board/Interrupt.cpp
        board/Interrupt.h
        board/InterruptHandlers.cpp
        board/InterruptStubs.S
        board/Mailbox.cpp
        board/Mailbox.h
        board/Registers.h
        board/Timer.cpp
        board/Uart.cpp
        mem/Atag.h
        mem/Memory.cpp
        mem/Memory.h
        task/Task.cpp
        task/Task.h
        task/ContextSwitch.s
        tasks/Proxy.cpp
        support/Runtime.cpp
        support/flanterm.c
        support/flanterm-fb.c

        ${USB_LIBRARY_SOURCES}

        boot.s
        Main.cpp
        usb/uspios.h support/Assert.h support/Logging.cpp support/Logging.h support/Assert.cpp board/InterruptHandlers.cpp)

include_directories(.)
add_executable(Kernel ${KERNEL_SOURCES})
target_compile_definitions(Kernel PRIVATE -DRASPPI=1)

add_custom_command(
        TARGET Kernel POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} ./Kernel/Kernel${CMAKE_EXECUTABLE_SUFFIX} -O binary ./kernel.img
        COMMAND ${CMAKE_OBJDUMP} -l -S -D ./Kernel/Kernel${CMAKE_EXECUTABLE_SUFFIX} > ./kernel.asm
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Convert the ELF output file to a binary image")