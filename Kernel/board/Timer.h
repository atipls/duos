#pragma once
#include "Registers.h"

namespace Timer {
    u32 GetTicks();
    void Alert(u32 usecs);

    void Delay(u32 usecs);
    void DelayMs(u32 msecs);
}// namespace Timer