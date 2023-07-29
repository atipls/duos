#include "Kernel/tasks/Proxy.h"
#include <Ati/Types.h>
#include <board/Framebuffer.h>
#include <board/Gpio.h>
#include <board/Interrupt.h>
#include <board/Timer.h>
#include <board/Uart.h>

#include <mem/Memory.h>

#include <task/Task.h>
#include <usb/uspi.h>

void DelayClocks(usize clocks) {
    for (u32 i = 0; i < clocks; i++) {
        asm volatile("nop");
    }
}

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_DEPTH 32

static void KeyPressedHandler(const char *pString) {
    Uart::Writef("Key pressed: %s\n", pString);
}

static void MouseHandler(u32 buttons, i32 dx, i32 dy) {
    Uart::Writef("Mouse status: %d (%d;%d)\n", buttons, dx, dy);
}

static void KernelThread() {
    while (true) {
        Gpio::Set(47, true);
        Uart::Write("Hello from thread two!\n");
        DelayClocks(1500000);
        Gpio::Set(47, false);

        DelayClocks(1500000);
    }
}

extern "C" [[noreturn]] __attribute__((unused)) void KernelMain(usize r0, usize r1, usize atagsAddress) {
    Uart::Initialize();
    Memory::Initialize(atagsAddress);
    Interrupt::Initialize();

    if (!Framebuffer::Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH)) {
        Uart::Write("Failed to initialize the framebuffer!\n");
    }

    auto framebuffer = Framebuffer::GetInformation();
    auto address = (volatile u8 *) framebuffer->address;

    // Fancy rainbow
    for (int y = 0; y < framebuffer->height; y++) {
        for (int x = 0; x < framebuffer->width; x++) {
            u32 offset = y * framebuffer->pitch + x * (framebuffer->depth / 8);
            address[offset] = x * 255 / framebuffer->width;
            address[offset + 1] = y * 255 / framebuffer->height;
            address[offset + 2] = 255 - x * 255 / framebuffer->width;
            address[offset + 3] = 255;
        }
    }

    Tasks::Initialize();

#if 1
    if (!USPiInitialize()) {
        Uart::Write("Failed to initialize the USB stack!\n");
    }

    if (!USPiKeyboardAvailable()) {
        Uart::Write("No keyboard detected!\n");
    }

    // USPiKeyboardRegisterKeyPressedHandler(KeyPressedHandler);
    // USPiMouseRegisterStatusHandler(MouseHandler);
#endif

    Tasks::Create(Proxy::TaskEntry, TaskKind_Kernel, "Proxy Client");
    Tasks::Create(KernelThread, TaskKind_Kernel, "Test thread");

    while (true) {
        Gpio::Set(47, true);
        Uart::Write("Hello!\n");
        DelayClocks(1500000);
        Gpio::Set(47, false);

        DelayClocks(1500000);
    }
}