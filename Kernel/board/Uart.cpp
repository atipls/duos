#include "Uart.h"
#include "Gpio.h"
#include "support/Runtime.h"

void Uart::Initialize() {
    Gpio::Enable(14);
    Gpio::Enable(15);

    UART_BASE->cr = 0x00000000;
    UART_BASE->icr = 0x7FF;
    UART_BASE->ibrd = 1;
    UART_BASE->fbrd = 40;
    UART_BASE->lcrh = (1 << 4) | (1 << 5) | (1 << 6);
    UART_BASE->imsc = (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
                      (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10);
    UART_BASE->cr = (1 << 0) | (1 << 8) | (1 << 9);
}

u8 Uart::RxByte() {
    return 0;
    while (UART_BASE->fr & (1 << 4)) {}
    return UART_BASE->dr;
}

void Uart::TxByte(u8 data) {
    return;
    while (UART_BASE->fr & (1 << 5)) {}
    UART_BASE->dr = data;
}

void Uart::Read(void *buffer, u64 size) {
    u8 *byteBuffer = reinterpret_cast<u8 *>(buffer);
    for (u64 i = 0; i < size; i++)
        byteBuffer[i] = RxByte();
}

void Uart::Write(const void *buffer, u64 size) {
    const u8 *byteBuffer = reinterpret_cast<const u8 *>(buffer);
    for (u64 i = 0; i < size; i++)
        TxByte(byteBuffer[i]);
}

void Uart::Write(const char *str) {
    while (*str) {
        TxByte(*str);
        str++;
    }
}
static char *ntoa(char *pDest, unsigned long ulNumber, unsigned nBase, boolean bUpcase) {
    unsigned long ulDigit;

    unsigned long ulDivisor = 1UL;
    while (1) {
        ulDigit = ulNumber / ulDivisor;
        if (ulDigit < nBase) {
            break;
        }

        ulDivisor *= nBase;
    }

    char *p = pDest;
    while (1) {
        ulNumber %= ulDivisor;

        *p++ = ulDigit < 10 ? '0' + ulDigit : '0' + ulDigit + 7 + (bUpcase ? 0 : 0x20);

        ulDivisor /= nBase;
        if (ulDivisor == 0) {
            break;
        }

        ulDigit = ulNumber / ulDivisor;
    }

    *p = '\0';

    return pDest;
}


void Uart::Writev(const char *format, va_list args) {
    char *outputCursor = (char *) format;

    while (*outputCursor != '\0') {
        if (*outputCursor == '%') {
            if (*++outputCursor == '%') {
                TxByte('%');

                outputCursor++;

                continue;
            }

            boolean bLeft = FALSE;
            if (*outputCursor == '-') {
                bLeft = TRUE;

                outputCursor++;
            }

            boolean bNull = FALSE;
            if (*outputCursor == '0') {
                bNull = TRUE;

                outputCursor++;
            }

            usize nWidth = 0;
            while ('0' <= *outputCursor && *outputCursor <= '9') {
                nWidth = nWidth * 10 + (*outputCursor - '0');

                outputCursor++;
            }

            boolean bLong = FALSE;
            if (*outputCursor == 'l') {
                bLong = TRUE;

                outputCursor++;
            }

            char chArg;
            const char *pArg;
            unsigned long ulArg;
            usize nLen;
            unsigned nBase;
            char NumBuf[11 + 1];
            boolean bMinus = FALSE;
            long lArg;

            switch (*outputCursor) {
                case 'c':
                    chArg = (char) va_arg(args, int);
                    if (bLeft) {
                        TxByte(chArg);
                        if (nWidth > 1) {
                            for (usize i = 1; i < nWidth; i++)
                                TxByte(' ');
                        }
                    } else {
                        if (nWidth > 1) {
                            for (usize i = 1; i < nWidth; i++)
                                TxByte(' ');
                        }
                        TxByte(chArg);
                    }
                    break;

                case 'd':
                    if (bLong) {
                        lArg = va_arg(args, long);
                    } else {
                        lArg = va_arg(args, int);
                    }
                    if (lArg < 0) {
                        bMinus = TRUE;
                        lArg = -lArg;
                    }
                    ntoa(NumBuf, (unsigned long) lArg, 10, FALSE);
                    nLen = Runtime::StringLength(NumBuf) + (bMinus ? 1 : 0);
                    if (bLeft) {
                        if (bMinus) {
                            TxByte('-');
                        }
                        Write(NumBuf);
                        if (nWidth > nLen) {
                            for (usize i = 0; i < nWidth - nLen; i++)
                                TxByte(' ');
                        }
                    } else {
                        if (nWidth > nLen) {
                            for (usize i = 0; i < nWidth - nLen; i++)
                                TxByte(' ');
                        }
                        if (bMinus) {
                            TxByte('-');
                        }
                        Write(NumBuf);
                    }
                    break;

                case 'o':
                    nBase = 8;
                    goto FormatNumber;

                case 's':
                    pArg = va_arg(args, const char *);
                    nLen = Runtime::StringLength(pArg);
                    if (bLeft) {
                        Write(pArg);
                        if (nWidth > nLen) {
                            for (usize i = 0; i < nWidth - nLen; i++)
                                TxByte(' ');
                        }
                    } else {
                        if (nWidth > nLen) {
                            for (usize i = 0; i < nWidth - nLen; i++)
                                TxByte(' ');
                        }
                        Write(pArg);
                    }
                    break;

                case 'u':
                    nBase = 10;
                    goto FormatNumber;

                case 'x':
                case 'X':
                    nBase = 16;
                    goto FormatNumber;

                FormatNumber:
                    if (bLong) {
                        ulArg = va_arg(args, unsigned long);
                    } else {
                        ulArg = va_arg(args, unsigned);
                    }
                    ntoa(NumBuf, ulArg, nBase, *outputCursor == 'X');
                    nLen = Runtime::StringLength(NumBuf);
                    if (bLeft) {
                        Write(NumBuf);
                        if (nWidth > nLen) {
                            for (usize i = 0; i < nWidth - nLen; i++)
                                TxByte(' ');
                        }
                    } else {
                        if (nWidth > nLen) {
                            for (usize i = 0; i < nWidth - nLen; i++)
                                TxByte(bNull ? '0' : ' ');
                        }
                        Write(NumBuf);
                    }
                    break;

                default:
                    TxByte('%');
                    TxByte(*outputCursor);
                    break;
            }
        } else {
            TxByte(*outputCursor);
        }

        outputCursor++;
    }
}

void Uart::Writef(char const *format, ...) {
    va_list args;
    va_start(args, format);
    Writev(format, args);
    va_end(args);
}