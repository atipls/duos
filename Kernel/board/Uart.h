#pragma once
#include <cstdarg>
#include "Registers.h"

namespace Uart {
    void Initialize();

    u8 RxByte();
    void TxByte(u8 data);

    void Read(void *buffer, u64 size);
    void Write(const void *buffer, u64 size);

    void Write(char const *str);

    void Writev(const char *format, va_list args);
    void Writef(const char *format, ...);
}