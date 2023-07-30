
#include "Assert.h"
#include "Logging.h"
#include <board/Uart.h>

void Assert::Panic(const char *message, const char *file, u32 line) {
    Logging::Error("assert", "Assertion failed: %s\n", message);
    Logging::Debug("assert", "  File: %s\n", file);
    Logging::Debug("assert", "  Line: %d\n", line);
}