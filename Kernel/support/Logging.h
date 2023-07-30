#pragma once

#include <Ati/Types.h>

#include "flanterm.h"

namespace Logging {
    enum LogLevel {
        LogLevel_Debug,
        LogLevel_Info,
        LogLevel_Warn,
        LogLevel_Error,
        LogLevel_Fatal,
    };

    void Write(const char *source, int level, const char *message, ...);

    void Debug(const char *source, const char *message, ...);
    void Info(const char *source, const char *message, ...);
    void Warn(const char *source, const char *message, ...);
    void Error(const char *source, const char *message, ...);
    void Fatal(const char *source, const char *message, ...);

    extern flanterm_context *TerminalContext;
}
