#include <cstdarg>

#include "Logging.h"
#include <board/Uart.h>

static const char *LogLevelNames[] = {
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL",
};

static void LoggerWrite(const char *source, int level, const char *message, va_list args) {
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