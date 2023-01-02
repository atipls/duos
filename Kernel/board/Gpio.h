#pragma once
#include <Ati/Types.h>
#include <util/Mmio.h>

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

#define GPIO ((GpioBase *) (MMIO_BASE + 0x200000))

namespace Gpio {
    enum Function {
        Input = 0,
        Output = 1,
        Alt0 = 4,
        Alt1 = 5,
        Alt2 = 6,
        Alt3 = 7,
        Alt4 = 3,
        Alt5 = 2,
    };

    void EnablePin(u32 pin);
    void DisablePin(u32 pin);

    void ConfigurePin(u32 pin, Function function);
    void ModifyPin(u32 pin, bool value);
}// namespace Gpio