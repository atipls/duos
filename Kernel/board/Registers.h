#pragma once

#include <Ati/Types.h>

struct GpioPin {
    RW32 reserved;
    RW32 data[2];
};

struct GpioBase {
    RW32 gpfSel[6];
    GpioPin gpSet;
    GpioPin gpClr;
    GpioPin gpLev;
    GpioPin gpEds;
    GpioPin gpRen;
    GpioPin gpFen;
    GpioPin gpHen;
    GpioPin gpLen;
    GpioPin gpAren;
    GpioPin gpAfen;

    RW32 reserved0;

    RW32 gpPuid;
    RW32 gpPudclk[2];
};

enum class GpioFunction {
    Input = 0,
    Output = 1,
    Alt0 = 4,
    Alt1 = 5,
    Alt2 = 6,
    Alt3 = 7,
    Alt4 = 3,
    Alt5 = 2,
};

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

struct TimerBase {
    RW32 cs;
    RW32 clo;
    RW32 chi;
    RW32 c0;
    RW32 c1;
    RW32 c2;
    RW32 c3;
};

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

struct ExceptionTableBase {
    u32 UndefinedInstruction;
    u32 SupervisorCall;
    u32 PrefetchAbort;
    u32 DataAbort;
    u32 Unused;
    u32 IRQ;
    u32 FIQ;
};

struct MailboxBase {
    RW32 read;
    RW32 unused[3];
    RW32 poll;
    RW32 sender;
    RW32 status;
    RW32 config;
    RW32 write;
};

#define MMIO_BASE 0x20000000
#define GPU_CACHED_BASE 0x40000000
#define GPU_UNCACHED_BASE 0xC0000000

#define GPU_L2_CACHE_ENABLED// normally enabled (can be disabled in config.txt)
#ifdef GPU_L2_CACHE_ENABLED
#define GPU_MEM_BASE GPU_CACHED_BASE
#else
#define GPU_MEM_BASE GPU_UNCACHED_BASE
#endif

#define BUS_ADDRESS(phys) (((phys) & ~0xC0000000) | GPU_MEM_BASE)

#define GPIO_BASE ((GpioBase *) (MMIO_BASE + 0x200000))
#define UART0_BASE ((UartBase *) (MMIO_BASE + 0x201000))
#define UART1_BASE ((UartBase *) (MMIO_BASE + 0x215000))
#define TIMER_BASE ((TimerBase *) (MMIO_BASE + 0x3000))
#define INTERRUPT_BASE ((InterruptBase *) (MMIO_BASE + 0xB200))
#define EXCEPTION_TABLE_BASE ((ExceptionTableBase *) 0x00000004)
#define MAILBOX_BASE ((MailboxBase *) (MMIO_BASE + 0xB880))

#define USB_BASE (MMIO_BASE + 0x980000)
#define USB_CORE_BASE USB_BASE
#define USB_HOST_BASE (USB_BASE + 0x400)
#define USB_POWER_BASE (USB_BASE + 0xE00)

// Configurable options
#define UART_BASE UART0_BASE

extern void DelayClocks(usize clocks);