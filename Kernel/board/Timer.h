#pragma once
#include "Gpio.h"

struct TimerBase {
    RW32 cs;
    RW32 clo;
    RW32 chi;
    RW32 c0;
    RW32 c1;
    RW32 c2;
    RW32 c3;
};

#define TIMER ((TimerBase *) (MMIO_BASE + 0x3000))

namespace Timer {
    void Initialize();
    u32 GetTicks();
    void Alert(u32 usecs);
    void Delay(u32 usecs);
}// namespace Timer