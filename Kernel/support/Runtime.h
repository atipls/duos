#pragma once

#include <Ati/Types.h>

namespace Runtime {
    void Copy(void *dst, void const *src, usize size);
    void Fill(void *dst, u8 value, usize size);
    void Zero(void *dst, usize size);
    void Move(void *dst, void const *src, usize size);

    usize StringLength(char const *str);
}