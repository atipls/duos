#pragma once

#include "Gpio.h"

struct InterruptBase {
    RW32 irqBasicPending;
    RW32 irqPending1;
    RW32 irqPending2;
    RW32 fiqControl;
    RW32 enableIrq1;
    RW32 enableIrq2;
    RW32 enableBasicIrq;
    RW32 disableIrq1;
    RW32 disableIrq2;
    RW32 disableBasicIrq;
};

using InterruptHandler = void (*)();
using InterruptClearer = void (*)();

#define NUMBER_OF_INTERRUPTS 72

#define INTERRUPT ((InterruptBase *) (MMIO_BASE + 0xB000))

namespace Interrupt {
    void Initialize();

    bool Status();

    void Enable();
    void Disable();

    void Register(usize irq, InterruptHandler handler, InterruptClearer clearer);
    void Unregister(usize irq);

    inline bool IsBasic(usize irq) { return irq >= 64; }
    inline bool IsGpu1(usize irq) { return irq < 32; }
    inline bool IsGpu2(usize irq) { return irq >= 32 && irq < 64; }

    bool IsPending(usize irq);

    bool Handle(usize irq);

};