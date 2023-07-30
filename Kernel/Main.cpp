#include "Kernel/tasks/Proxy.h"
#include <Ati/Types.h>
#include <board/Framebuffer.h>
#include <board/Gpio.h>
#include <board/Interrupt.h>
#include <board/Uart.h>

#include <mem/Memory.h>

#include "support/Logging.h"
#include "support/flanterm-fb.h"
#include <task/Task.h>
#include <task/TaskTimer.h>
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
    Logging::Info("kernel", "Key pressed: %s", pString);
}

static void MouseHandler(u32 buttons, i32 dx, i32 dy) {
    Logging::Info("kernel", "Mouse status: %d (%d;%d)", buttons, dx, dy);
}

static void KernelThread() {
    while (true) {
        Gpio::Set(47, true);
        Logging::Info("kernel", "Hello from thread two! Interrupts are %s", Interrupt::Status() ? "enabled" : "disabled");
        DelayClocks(15000000);
        Gpio::Set(47, false);

        DelayClocks(15000000);
    }
}

static void ClearBssSection() {
    extern u8 __bss_start;
    extern u8 __bss_end;

    for (auto bss = &__bss_start; bss < &__bss_end; bss++)
        *bss = 0;
}

extern "C" [[noreturn]] __attribute__((unused)) void KernelMain() {
    ClearBssSection();

    Uart::Initialize();
    if (!Framebuffer::Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH)) {
        Logging::Error("kernel", "Failed to initialize the framebuffer!");
    }

    auto framebuffer = Framebuffer::GetInformation();
    Logging::TerminalContext = flanterm_fb_simple_init(
            (u32 *) framebuffer->address,
            framebuffer->width,
            framebuffer->height,
            framebuffer->pitch);

    Logging::Info("kernel", "Framebuffer initialized at 0x%X", framebuffer->address);
    Logging::Info("kernel", "Framebuffer size: %dx%d", framebuffer->width, framebuffer->height);

    Memory::Initialize();
    Interrupt::Initialize();
    Tasks::Initialize();
    TaskTimer::Initialize();

#if 1
    if (!USPiInitialize()) {
        Logging::Error("kernel", "Failed to initialize the USB stack!");
    }

    if (!USPiKeyboardAvailable()) {
        Logging::Error("kernel", "No keyboard detected!");
    }

    USPiKeyboardRegisterKeyPressedHandler(KeyPressedHandler);
    USPiMouseRegisterStatusHandler(MouseHandler);
#endif

    // Tasks::Create(Proxy::TaskEntry, TaskKind_Kernel, "Proxy Client");
    // Tasks::Create(KernelThread, TaskKind_Kernel, "Test thread");

    while (true) {
        Gpio::Set(47, true);
        Logging::Info("kernel", "Hello!");
        DelayClocks(150000000);
        Gpio::Set(47, false);
        DelayClocks(150000000);
    }
}
