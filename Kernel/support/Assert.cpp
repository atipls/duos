
#include "Assert.h"
#include <board/Uart.h>

void Assert::Panic(const char *message, const char *file, u32 line) {
    Uart::Writef("Assertion failed: %s\n", message);
    Uart::Writef("  File: %s\n", file);
    Uart::Writef("  Line: %d\n", line);
}