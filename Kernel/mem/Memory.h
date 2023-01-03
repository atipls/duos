#pragma once
#include <Ati/Types.h>

namespace Memory {
    void Initialize();

    void *Allocate(usize size);
    void Free(void *ptr);

}// namespace Memory

template<typename T>
T *allocate() {
    return (T *) Memory::Allocate(sizeof(T));
}

template<typename T>
T *allocate(usize count) {
    return (T *) Memory::Allocate(sizeof(T) * count);
}

template<typename T>
void deallocate(T *ptr) {
    Memory::Free(ptr);
}