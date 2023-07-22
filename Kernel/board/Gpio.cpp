#include "Gpio.h"

void Gpio::Enable(u32 pin) {
    GPIO_BASE->gpPuid = 0;
    DelayClocks(150);
    GPIO_BASE->gpPudclk[pin / 32] = 1 << (pin % 32);

    DelayClocks(150);
    GPIO_BASE->gpPuid = 0;
    GPIO_BASE->gpPudclk[pin / 32] = 0;
}

void Gpio::Disable(u32 pin) {
    GPIO_BASE->gpPuid = 0;
    DelayClocks(150);
    GPIO_BASE->gpPudclk[pin / 32] = 1 << (pin % 32);

    DelayClocks(150);
    GPIO_BASE->gpPuid = 0;
    GPIO_BASE->gpPudclk[pin / 32] = 0;
}

void Gpio::Configure(u32 pin, GpioFunction function) {
    u8 bitStart = (pin * 3) % 30;
    u8 reg = pin / 10;

    u32 selector = GPIO_BASE->gpfSel[reg];
    selector &= ~(7 << bitStart);
    selector |= ((u32)function << bitStart);

    GPIO_BASE->gpfSel[reg] = selector;
}

void Gpio::Set(u32 pin, bool value) {
    auto pinRegister = value ? &GPIO_BASE->gpSet : &GPIO_BASE->gpClr;

    pinRegister->data[pin / 32] = 1 << (pin % 32);
}