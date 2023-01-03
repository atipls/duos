#include "Memory.h"


void Memory::Initialize() {

}


void *s_heapStart = nullptr;
void *Memory::Allocate(usize size) {
    if (s_heapStart == nullptr) {
        s_heapStart = (void *) 0x190000; // Randomly chosen address.
    }

    void *result = s_heapStart;
    s_heapStart = (void *) ((usize) s_heapStart + size);
    return result;
}

void Free(void *ptr) {

}