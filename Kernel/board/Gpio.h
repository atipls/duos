#pragma once
#include "Registers.h"

namespace Gpio {
    void Enable(u32 pin);
    void Disable(u32 pin);

    void Configure(u32 pin, GpioFunction function);

    void Set(u32 pin, bool value);
}