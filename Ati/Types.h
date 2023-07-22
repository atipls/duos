#pragma once

#include <stdint.h>

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

typedef intptr_t isize;
typedef uintptr_t usize;

typedef volatile u32 RW32;
typedef volatile u32 RO32;
typedef volatile u32 WO32;

typedef int boolean;
#define FALSE 0
#define TRUE 1