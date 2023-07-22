#pragma once
#include "Registers.h"

#define ARM_OPCODE_BRANCH(distance)	(0xEA000000 | (distance))
#define ARM_DISTANCE(from, to)		((u32 *) &(to) - (u32 *) &(from) - 2)

#define NUMBER_OF_INTERRUPTS 72

namespace Interrupt {
    void Initialize();

    bool Status();

    void Enable();
    void Disable();

    void Register(usize irq, void (*handler)(void *), void *userData = nullptr);
    void Unregister(usize irq);

    inline bool IsBasic(usize irq) { return irq >= 64; }
    inline bool IsGpu1(usize irq) { return irq < 32; }
    inline bool IsGpu2(usize irq) { return irq >= 32 && irq < 64; }

    bool IsPending(usize irq);

    bool Handle(usize irq);
};// namespace Interrupt