#include "Runtime.h"

void Runtime::Copy(void *dst, void const *src, usize size) {
    u8 *dst8 = (u8 *) dst;
    u8 const *src8 = (u8 const *) src;
    for (usize i = 0; i < size; i++)
        dst8[i] = src8[i];
}


void Runtime::Fill(void *dst, u8 value, usize size) {
    u8 *dst8 = (u8 *) dst;
    for (usize i = 0; i < size; i++)
        dst8[i] = value;
}

void Runtime::Zero(void *dst, usize size) {
    Fill(dst, 0, size);
}

void Runtime::Move(void *dst, void const *src, usize size) {
    u8 *dst8 = (u8 *) dst;
    u8 const *src8 = (u8 const *) src;
    if (dst8 < src8) {
        for (usize i = 0; i < size; i++)
            dst8[i] = src8[i];
    } else {
        for (usize i = size; i > 0; i--)
            dst8[i - 1] = src8[i - 1];
    }
}

usize Runtime::Length(char const *str) {
    usize length = 0;
    while (str[length] != '\0')
        length++;
    return length;
}