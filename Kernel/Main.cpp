#include "board/Interrupt.h"
#include "board/Timer.h"
#include <Ati/Types.h>
#include <board/Gpio.h>
#include <board/Uart.h>

namespace __cxxabiv1 {
    __extension__ typedef int __guard __attribute__((mode(__DI__)));
    extern "C" int __cxa_guard_acquire(__guard *);
    extern "C" void __cxa_guard_release(__guard *);
    extern "C" void __cxa_guard_abort(__guard *);

    extern "C" int __cxa_guard_acquire(__guard *g) { return !*(char *) (g); }
    extern "C" void __cxa_guard_release(__guard *g) { *(char *) g = 1; }
    extern "C" void __cxa_guard_abort(__guard *) {}
}// namespace __cxxabiv1

void Delay(u32 clocks) {
    for (u32 i = 0; i < clocks; i++) {
        asm volatile("nop");
    }
}

extern "C" [[noreturn]] __attribute__((unused)) void KernelMain(usize r0, usize r1, usize atags) {
    GPIO->gpfSel[3] &= ~(7 << 21);
    GPIO->gpfSel[3] |= (1 << 21);

    Gpio::ConfigurePin(47, Gpio::Function::Output);

    for (int i = 0; i < 20; i++) {
        Gpio::Put(47, true);
        Delay(1500000);
        Gpio::Put(47, false);
        Delay(1500000);
    }

    Uart::Initialize();
    Uart::Write("Hello, world\n");

    Interrupt::Initialize();

    Timer::Initialize();
    Timer::Alert(1000);

    while (true) {
        Gpio::Put(47, true);
        Uart::Write("Hello!");
        Delay(1500000);
        Gpio::Put(47, false);

        Delay(1500000);
    }
}