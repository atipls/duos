#pragma once

#include <Ati/Types.h>

namespace TaskTimer {
    void Initialize();
    void DelayUs(u32 us);
    void DelayMs(u32 ms);

    void HandleTimerInterrupt(void *);
}