#include "Interrupt.h"
#include "support/Logging.h"

extern "C" {
extern void Exception_UndefinedInstruction_Handler();
extern void Exception_PrefetchAbort_Handler();
extern void Exception_DataAbort_Handler();
extern void Exception_Irq_Handler();
}

struct IrqHandler {
    void (*handler)(void *);
    void *userData;
};

static IrqHandler s_irqHandlers[NUMBER_OF_INTERRUPTS];

void Interrupt::Initialize() {
    auto EXCEPTIONS = EXCEPTION_TABLE_BASE;

    EXCEPTIONS->UndefinedInstruction = ARM_OPCODE_BRANCH(ARM_DISTANCE(EXCEPTIONS->UndefinedInstruction, Exception_UndefinedInstruction_Handler));
    EXCEPTIONS->PrefetchAbort = ARM_OPCODE_BRANCH(ARM_DISTANCE(EXCEPTIONS->PrefetchAbort, Exception_PrefetchAbort_Handler));
    EXCEPTIONS->DataAbort = ARM_OPCODE_BRANCH(ARM_DISTANCE(EXCEPTIONS->DataAbort, Exception_DataAbort_Handler));
    EXCEPTIONS->IRQ = ARM_OPCODE_BRANCH(ARM_DISTANCE(EXCEPTIONS->IRQ, Exception_Irq_Handler));

    INTERRUPT_BASE->disableBasicIrq = 0xFFFFFFFF;
    INTERRUPT_BASE->disableIrq1 = 0xFFFFFFFF;
    INTERRUPT_BASE->disableIrq2 = 0xFFFFFFFF;

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

void Interrupt::Register(usize irq, void (*handler)(void *), void *userData) {
    if (irq >= NUMBER_OF_INTERRUPTS)
        return;

    auto irqHandler = &s_irqHandlers[irq];

    irqHandler->handler = handler;
    irqHandler->userData = userData;

    if (IsBasic(irq))
        INTERRUPT_BASE->enableBasicIrq |= 1 << (irq - 64);
    else if (IsGpu1(irq))
        INTERRUPT_BASE->enableIrq1 |= 1 << irq;
    else
        INTERRUPT_BASE->enableIrq2 |= 1 << (irq - 32);
}

void Interrupt::Unregister(usize irq) {
    if (irq >= NUMBER_OF_INTERRUPTS)
        return;

    auto irqHandler = &s_irqHandlers[irq];

    irqHandler->handler = nullptr;
    irqHandler->userData = nullptr;

    if (IsBasic(irq))
        INTERRUPT_BASE->disableBasicIrq |= 1 << (irq - 64);
    else if (IsGpu1(irq))
        INTERRUPT_BASE->disableIrq1 |= 1 << irq;
    else
        INTERRUPT_BASE->disableIrq2 |= 1 << (irq - 32);
}

bool Interrupt::IsPending(usize irq) {
    if (IsBasic(irq))
        return INTERRUPT_BASE->irqBasicPending & (1 << (irq - 64));
    else if (IsGpu1(irq))
        return INTERRUPT_BASE->irqPending1 & (1 << irq);
    return INTERRUPT_BASE->irqPending2 & (1 << (irq - 32));
}

bool Interrupt::Handle(usize irq) {
    // Logging::Debug("irq", "irq=%d", irq);
    auto irqHandler = &s_irqHandlers[irq];
    if (!irqHandler->handler) {
        Unregister(irq);
        return false;
    }

    irqHandler->handler(irqHandler->userData);

    return true;
}
