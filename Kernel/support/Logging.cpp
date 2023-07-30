#include <cstdarg>

#include "Logging.h"
#include "flanterm.h"
#include "Runtime.h"
#include <board/Uart.h>

static const char *LogLevelNames[] = {
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL",
};

flanterm_context *Logging::TerminalContext = nullptr;

static void LoggerWriteUart(const char *source, int level, const char *message, va_list args) {
    Uart::Writef("[%-9s]: ", source);
    switch (level) {
        case Logging::LogLevel_Debug:
            Uart::Writef("\033[34m DEBUG \033[0m");
            break;
        case Logging::LogLevel_Info:
            Uart::Writef("\033[32m INFO  \033[0m");
            break;

        case Logging::LogLevel_Warn:
            Uart::Writef("\033[33m WARN  \033[0m");
            break;

        case Logging::LogLevel_Error:
            Uart::Writef("\033[31m ERROR \033[0m");
            break;
    }

    Uart::Writev(message, args);
    Uart::Write("\n");
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


static void FramebufferWriteByte(char byte) {
    flanterm_write(Logging::TerminalContext, &byte, 1);
}

void FramebufferWritev(const char *format, va_list args) {
    char *outputCursor = (char *) format;

    while (*outputCursor != '\0') {
        if (*outputCursor == '%') {
            if (*++outputCursor == '%') {
                FramebufferWriteByte('%');

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
                        FramebufferWriteByte(chArg);
                        if (nWidth > 1) {
                            for (usize i = 1; i < nWidth; i++)
                                FramebufferWriteByte(' ');
                        }
                    } else {
                        if (nWidth > 1) {
                            for (usize i = 1; i < nWidth; i++)
                                FramebufferWriteByte(' ');
                        }
                        FramebufferWriteByte(chArg);
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
                            FramebufferWriteByte('-');
                        }
                        flanterm_write(Logging::TerminalContext, NumBuf, 12);
                        if (nWidth > nLen) {
                            for (usize i = 0; i < nWidth - nLen; i++)
                                FramebufferWriteByte(' ');
                        }
                    } else {
                        if (nWidth > nLen) {
                            for (usize i = 0; i < nWidth - nLen; i++)
                                FramebufferWriteByte(' ');
                        }
                        if (bMinus) {
                            FramebufferWriteByte('-');
                        }
                        flanterm_write(Logging::TerminalContext, NumBuf, 12);
                    }
                    break;

                case 'o':
                    nBase = 8;
                    goto FormatNumber;

                case 's':
                    pArg = va_arg(args, const char *);
                    nLen = Runtime::StringLength(pArg);
                    if (bLeft) {
                        flanterm_write(Logging::TerminalContext, pArg, nLen);
                        if (nWidth > nLen) {
                            for (usize i = 0; i < nWidth - nLen; i++)
                                FramebufferWriteByte(' ');
                        }
                    } else {
                        if (nWidth > nLen) {
                            for (usize i = 0; i < nWidth - nLen; i++)
                                FramebufferWriteByte(' ');
                        }
                        flanterm_write(Logging::TerminalContext, pArg, nLen);
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
                        flanterm_write(Logging::TerminalContext, NumBuf, 12);
                        if (nWidth > nLen) {
                            for (usize i = 0; i < nWidth - nLen; i++)
                                FramebufferWriteByte(' ');
                        }
                    } else {
                        if (nWidth > nLen) {
                            for (usize i = 0; i < nWidth - nLen; i++)
                                FramebufferWriteByte(bNull ? '0' : ' ');
                        }
                        flanterm_write(Logging::TerminalContext, NumBuf, 12);
                    }
                    break;

                default:
                    FramebufferWriteByte('%');
                    FramebufferWriteByte(*outputCursor);
                    break;
            }
        } else {
            FramebufferWriteByte(*outputCursor);
        }

        outputCursor++;
    }
}

void FramebufferWritef(char const *format, ...) {
    va_list args;
    va_start(args, format);
    FramebufferWritev(format, args);
    va_end(args);
}

static void LoggerWriteFramebuffer(const char *source, int level, const char *message, va_list args) {
    if (!Logging::TerminalContext)
        return;

    FramebufferWritef("[%-9s]: ", source);
    switch (level) {
        case Logging::LogLevel_Debug:
            FramebufferWritef("\033[34m DEBUG \033[0m");
            break;
        case Logging::LogLevel_Info:
            FramebufferWritef("\033[32m INFO  \033[0m");
            break;

        case Logging::LogLevel_Warn:
            FramebufferWritef("\033[33m WARN  \033[0m");
            break;

        case Logging::LogLevel_Error:
            FramebufferWritef("\033[31m ERROR \033[0m");
            break;
    }

    FramebufferWritev(message, args);
    FramebufferWriteByte('\n');
}

static void LoggerWrite(const char *source, int level, const char *message, va_list args) {
    LoggerWriteUart(source, level, message, args);
}


void Logging::Write(const char *source, int level, const char *message, ...) {
    va_list args;
    va_start(args, message);
    LoggerWrite(source, level, message, args);
    va_end(args);
}

void Logging::Debug(const char *source, const char *message, ...) {
    va_list args;
    va_start(args, message);
    LoggerWrite(source, LogLevel_Debug, message, args);
    va_end(args);
}

void Logging::Info(const char *source, const char *message, ...) {
    va_list args;
    va_start(args, message);
    LoggerWrite(source, LogLevel_Info, message, args);
    va_end(args);
}

void Logging::Warn(const char *source, const char *message, ...) {
    va_list args;
    va_start(args, message);
    LoggerWrite(source, LogLevel_Warn, message, args);
    va_end(args);
}

void Logging::Error(const char *source, const char *message, ...) {
    va_list args;
    va_start(args, message);
    LoggerWrite(source, LogLevel_Error, message, args);
    va_end(args);
}

void Logging::Fatal(const char *source, const char *message, ...) {
    va_list args;
    va_start(args, message);
    LoggerWrite(source, LogLevel_Fatal, message, args);
    va_end(args);

    while (true) {
        asm("wfi");
    }
}

void LogWrite(const char *pSource,// short name of module
                         unsigned Severity,  // see above
                         const char *pMessage, ...) {
    switch (Severity) {
        case 1: Severity = Logging::LogLevel_Error; break;
        case 2: Severity = Logging::LogLevel_Warn; break;
        case 3: Severity = Logging::LogLevel_Info; break;
        case 4: Severity = Logging::LogLevel_Debug; break;
        default: Severity = Logging::LogLevel_Info; break;
    }
    va_list args;
    va_start(args, pMessage);
    LoggerWrite(pSource, (i32) Severity, pMessage, args);
    va_end(args);
}