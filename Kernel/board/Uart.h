#pragma once
#include "Gpio.h"
#include <util/Mmio.h>

extern void Delay(u32 clocks);

struct UartBase {
    RW32 dr;
    RW32 rsrecr;
    RW32 reserved0[4];
    RW32 fr;
    RW32 reserved1;
    RW32 ilpr;
    RW32 ibrd;
    RW32 fbrd;
    RW32 lcrh;
    RW32 cr;
    RW32 ifls;
    RW32 imsc;
    RW32 ris;
    RW32 mis;
    RW32 icr;
    RW32 dmacr;
};

#define UART ((UartBase *) (MMIO_BASE + 0x201000))

namespace Uart {
    void Initialize();

    u8 Read();
    void Write(u8 data);
    void Write(char const *str);
}