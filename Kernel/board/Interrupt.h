#pragma once
#include "Registers.h"

#define ARM_OPCODE_BRANCH(distance) (0xEA000000 | (distance))
#define ARM_DISTANCE(from, to) ((u32 *) &(to) - (u32 *) &(from) -2)

#define NUMBER_OF_INTERRUPTS 72

#define InvalidateInstructionCache()            \
    __asm volatile("mcr p15, 0, %0, c7, c5,  0" \
                   :                            \
                   : "r"(0)                     \
                   : "memory")

#define FlushPrefetchBuffer() __asm volatile("mcr p15, 0, %0, c7, c5,  4" \
                                             :                            \
                                             : "r"(0)                     \
                                             : "memory")

#define FlushBranchTargetCache()                \
    __asm volatile("mcr p15, 0, %0, c7, c5,  6" \
                   :                            \
                   : "r"(0)                     \
                   : "memory")

#define InvalidateDataCache() __asm volatile("mcr p15, 0, %0, c7, c6,  0" \
                                             :                            \
                                             : "r"(0)                     \
                                             : "memory")

#define CleanDataCache() __asm volatile("mcr p15, 0, %0, c7, c10, 0" \
                                        :                            \
                                        : "r"(0)                     \
                                        : "memory")

#define DataSyncBarrier() __asm volatile("mcr p15, 0, %0, c7, c10, 4" \
                                         :                            \
                                         : "r"(0)                     \
                                         : "memory")

#define DataMemBarrier() __asm volatile("mcr p15, 0, %0, c7, c10, 5" \
                                        :                            \
                                        : "r"(0)                     \
                                        : "memory")

#define InstructionSyncBarrier() FlushPrefetchBuffer()
#define InstructionMemBarrier() FlushPrefetchBuffer()

namespace Interrupt {
    void Initialize();

    bool Status();

    void Enable();
    void Disable();

    void EnterCriticalSection();
    void LeaveCriticalSection();

    void Register(usize irq, void (*handler)(void *), void *userData = nullptr);
    void Unregister(usize irq);

    inline bool IsBasic(usize irq) { return irq >= 64; }
    inline bool IsGpu1(usize irq) { return irq < 32; }
    inline bool IsGpu2(usize irq) { return irq >= 32 && irq < 64; }

    bool IsPending(usize irq);

    bool Handle(usize irq);
};// namespace Interrupt