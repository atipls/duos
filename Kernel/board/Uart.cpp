#include "Uart.h"
#include "Gpio.h"

void Uart::Initialize() {
    UART->cr = 0x00000000;

    Gpio::EnablePin(14);
    Gpio::EnablePin(15);

    UART->icr = 0x7FF;
    UART->ibrd = 1;
    UART->fbrd = 40;
    UART->lcrh = (1 << 4) | (1 << 5) | (1 << 6);
    UART->imsc = (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
                 (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10);
    UART->cr = (1 << 0) | (1 << 8) | (1 << 9);
}

u8 Uart::Read() {
    while (UART->fr & (1 << 4)) {}
    return UART->dr;
}

void Uart::Write(u8 data) {
    while (UART->fr & (1 << 5)) {}
    UART->dr = data;
}

void Uart::Write(char const *str) {
    while (*str) {
        Write(*str++);
    }
}
