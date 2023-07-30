#pragma once
#include "Registers.h"

namespace Timer {
    u32 GetTicks();
    void Alert(u32 usecs);
}// namespace Timer