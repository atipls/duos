#include "Interrupt.h"
#include "Uart.h"

extern "C" {
void MoveExceptionVectors();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
void __attribute__((interrupt("ABORT"))) ResetHandler() {}
void __attribute__((interrupt("UNDEF"))) UndefinedInstructionHandler() {}
void __attribute__((interrupt("SWI"))) SoftwareInterruptHandler() {}
void __attribute__((interrupt("ABORT"))) PrefetchAbortHandler() {}
void __attribute__((interrupt("ABORT"))) DataAbortHandler() {}
void __attribute__((interrupt("FIQ"))) FiqHandler() {}
#pragma GCC diagnostic pop
}

extern "C" __attribute__((unused)) void IrqHandler() {
    Uart::Write("irq\n");

    for (int i = 0; i < NUMBER_OF_INTERRUPTS; i++) {
        if (Interrupt::IsPending(i) && Interrupt::Handle(i))
            return;
    }
}



static InterruptHandler InterruptHandlers[NUMBER_OF_INTERRUPTS];
static InterruptClearer InterruptClearers[NUMBER_OF_INTERRUPTS];

extern RW32 ExceptionVectors[8];

void Interrupt::Initialize() {
    INTERRUPT->disableBasicIrq = 0xFFFFFFFF;
    INTERRUPT->disableIrq1 = 0xFFFFFFFF;
    INTERRUPT->disableIrq2 = 0xFFFFFFFF;

    RW32 *OldVectors = (RW32 *) 0x0;
    for (int i = 0; i < 8; i++)
        OldVectors[i] = ExceptionVectors[i];

    Enable();
}

bool Interrupt::Status() {
    int cpsr;
    __asm__ __volatile__("mrs %[cpsr], cpsr"
                         : [cpsr] "=r"(cpsr)::);
    return ((cpsr >> 7) & 1) == 0;
}

void Interrupt::Enable() {
    if (!Status())
        __asm__ __volatile__("cpsie i");
}

void Interrupt::Disable() {
    if (Status())
        __asm__ __volatile__("cpsid i");
}

void Interrupt::Register(usize irq, InterruptHandler handler, InterruptClearer clearer) {
    if (irq >= NUMBER_OF_INTERRUPTS)
        return;

    InterruptHandlers[irq] = handler;
    InterruptClearers[irq] = clearer;

    if (IsBasic(irq))
        INTERRUPT->enableBasicIrq |= 1 << (irq - 64);
    else if (IsGpu1(irq))
        INTERRUPT->enableIrq1 |= 1 << irq;
    else
        INTERRUPT->enableIrq2 |= 1 << (irq - 32);
}

void Interrupt::Unregister(usize irq) {
    if (irq >= NUMBER_OF_INTERRUPTS)
        return;

    InterruptHandlers[irq] = nullptr;
    InterruptClearers[irq] = nullptr;

    if (IsBasic(irq))
        INTERRUPT->disableBasicIrq |= 1 << (irq - 64);
    else if (IsGpu1(irq))
        INTERRUPT->disableIrq1 |= 1 << irq;
    else
        INTERRUPT->disableIrq2 |= 1 << (irq - 32);
}

bool Interrupt::IsPending(usize irq)  {
    if (IsBasic(irq))
        return INTERRUPT->irqBasicPending & (1 << (irq - 64));
    else if (IsGpu1(irq))
        return INTERRUPT->irqPending1 & (1 << irq);
    return INTERRUPT->irqPending2 & (1 << (irq - 32));
}

bool Interrupt::Handle(usize irq)  {
    if (InterruptHandlers[irq] != nullptr) {
        InterruptClearers[irq]();
        Enable();
        InterruptHandlers[irq]();
        Disable();
        return true;
    }
    return false;
}
