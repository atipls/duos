#include "Gpio.h"
#include "Uart.h"

void Gpio::EnablePin(u32 pin) {
    GPIO->gpPuid = 0;
    Delay(150);
    GPIO->gpPudclk[pin / 32] = 1 << (pin % 32);

    Delay(150);
    GPIO->gpPuid = 0;
    GPIO->gpPudclk[pin / 32] = 0;
}

void Gpio::DisablePin(u32 pin) {
    GPIO->gpPuid = 0;
    Delay(150);
    GPIO->gpPudclk[pin / 32] = 1 << (pin % 32);

    Delay(150);
    GPIO->gpPuid = 0;
    GPIO->gpPudclk[pin / 32] = 0;
}

void Gpio::ConfigurePin(u32 pin, Function function) {
    u8 bitStart = (pin * 3) % 30;
    u8 reg = pin / 10;

    u32 selector = GPIO->gpfSel[reg];
    selector &= ~(7 << bitStart);
    selector |= (function << bitStart);

    GPIO->gpfSel[reg] = selector;
}

void Gpio::Put(u32 pin, bool value) {
    if (value) {
        GPIO->gpSet.data[pin / 32] = 1 << (pin % 32);
    } else {
        GPIO->gpClr.data[pin / 32] = 1 << (pin % 32);
    }
}