#pragma once
#include <Ati/Types.h>
#include <new>

#define PAGE_SIZE 4096
#define KERNEL_HEAP_SIZE (1024 * 1024 * 32) // 32 MiB
#define KERNEL_STACK_SIZE (PAGE_SIZE * 32) // 128 KiB


struct PageFlags {
    u8 isAllocated : 1;
    u8 isKernelData : 1;
    u8 isKernelHeap : 1;
    u32 reserved : 29;
};

struct Page {
    usize address;
    PageFlags flags;

    Page *prev;
    Page *next;
};

namespace Memory {
    void Initialize();

    void *AllocatePage();
    void DeallocatePage(void *address);

    void *Allocate(usize size);
    void Deallocate(void *address);

}// namespace Memory

template<typename T>
T *allocate() {
    return (T *)  Memory::Allocate(sizeof(T));
}

template<typename T>
void deallocate(T *address) {
    Memory::Deallocate(address);
}