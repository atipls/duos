#pragma once

#include <Ati/Types.h>

#define assert(condition) if (!(condition)) Assert::Panic(#condition, __FILE__, __LINE__)

namespace Assert {
    void Panic(const char *message, const char *file, u32 line);
}