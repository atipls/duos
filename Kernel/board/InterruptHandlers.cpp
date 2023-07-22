#include "Framebuffer.h"
#include "Interrupt.h"
#include <Ati/Types.h>
#include <support/Logging.h>

struct TAbortFrame {
    u32 sp_irq;
    u32 lr_irq;
    u32 r0;
    u32 r1;
    u32 r2;
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;
    u32 r8;
    u32 r9;
    u32 r10;
    u32 r11;
    u32 r12;
    u32 sp;
    u32 lr;
    u32 spsr;
    u32 pc;
};

void DumpStackTrace (u32 *pStackPtr)
{

    for (unsigned i = 0; i < 64; i++, pStackPtr++)
    {
        extern unsigned char __text_end;

        if (   *pStackPtr >= 0x8000
            && *pStackPtr < (u32) &__text_end)
        {
            Logging::Error("crash", "stack[%u] is 0x%X", i, (unsigned) *pStackPtr);
        }
    }
}

const char *ExceptionNames[] = {
    "\x1B[31mDivide By Zero\x1B[0m",
    "\x1B[31mUndefined Instruction\x1B[0m",
    "\x1B[31mPrefetch Abort\x1B[0m",
    "\x1B[31mData Abort\x1B[0m",
};

extern "C" void ExceptionHandler(u32 exceptionCode, TAbortFrame *abortFrame) {
    Interrupt::Disable();

    u32 lr = abortFrame->lr;
    u32 sp = abortFrame->sp;

    if ((abortFrame->spsr & 0x1F) == 0x12) {
        lr = abortFrame->lr_irq;
        sp = abortFrame->sp_irq;
    }

    Logging::Error("crash", "exception=%s lr=0x%X, sp=0x%X, pc=0x%X", ExceptionNames[exceptionCode], lr, sp, abortFrame->pc);
    DumpStackTrace(&sp);

    auto framebuffer = Framebuffer::GetInformation();
    auto address = (volatile u8 *) framebuffer->address;
    for (int y = 0; y < framebuffer->height; y++) {
        for (int x = 0; x < framebuffer->width; x++) {
            u32 offset = y * framebuffer->pitch + x * (framebuffer->depth / 8);
            address[offset + 0] = exceptionCode == 1 ? 0xFF : 0x00;
            address[offset + 1] = exceptionCode == 2 ? 0xFF : 0x00;
            address[offset + 2] = exceptionCode == 3 ? 0xFF : 0x00;
            address[offset + 3] = 0xFF;
        }
    }

    while (true)
        ;
}

extern "C" void InterruptHandler() {
    for (int i = 0; i < NUMBER_OF_INTERRUPTS; i++) {
        if (Interrupt::IsPending(i) && Interrupt::Handle(i))
            return;
    }
}