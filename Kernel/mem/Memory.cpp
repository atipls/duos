#include "Memory.h"
#include "Atag.h"
#include "support/Logging.h"
#include <Ati/IntrusiveList.h>
#include <support/Runtime.h>

struct HeapSegmentHeader {
    u32 isAllocated;
    usize totalSize;

    HeapSegmentHeader *prev;
    HeapSegmentHeader *next;
};

extern "C" usize __binary_end;

static Page *s_pages = nullptr;
static usize s_pageCount = 0;
static IntrusiveList<Page> s_freePages;

static HeapSegmentHeader *s_heapSegments;

static u32 GetMemorySize(Atag *atags) {
    while (atags->tag != ATAG_NONE) {
        if (atags->tag == ATAG_MEMORY)
            return atags->memory.size;
        atags = (Atag *) (((u32 *) atags) + atags->size);
    }

    return 256 * 1024 * 1024;// 256 MB, in qemu probably
}

void Memory::Initialize(usize atagsAddress) {
    auto memorySize = GetMemorySize((Atag *) atagsAddress);

    s_pageCount = memorySize / PAGE_SIZE;
    s_pages = (Page *) ((usize) &__binary_end + KERNEL_STACK_SIZE);
    Logging::Info("memory", "available=%dMB, pages=%d", memorySize / 1024 / 1024, s_pageCount);

    auto pageTableSize = s_pageCount * sizeof(Page);
    auto pageTableEnd = (usize) s_pages + pageTableSize;
    pageTableEnd += pageTableEnd % PAGE_SIZE ? PAGE_SIZE - (pageTableEnd % PAGE_SIZE) : 0;

    Runtime::Zero(s_pages, pageTableSize);

    auto currentPageIndex = 0;
    auto kernelAllocatedPages = pageTableEnd / PAGE_SIZE;

    for (; currentPageIndex < kernelAllocatedPages; currentPageIndex++) {
        s_pages[currentPageIndex].address = currentPageIndex * PAGE_SIZE;
        s_pages[currentPageIndex].flags.isAllocated = 1;
        s_pages[currentPageIndex].flags.isKernelData = 1;
    }

    // Kernel heap.
    for (; currentPageIndex < kernelAllocatedPages + (KERNEL_HEAP_SIZE / PAGE_SIZE); currentPageIndex++) {
        s_pages[currentPageIndex].address = currentPageIndex * PAGE_SIZE;
        s_pages[currentPageIndex].flags.isAllocated = 1;
        s_pages[currentPageIndex].flags.isKernelHeap = 1;
    }

    for (; currentPageIndex < s_pageCount; currentPageIndex++) {
        s_pages[currentPageIndex].flags.isAllocated = 0;
        s_freePages.Add(&s_pages[currentPageIndex]);
    }

    s_heapSegments = (HeapSegmentHeader *) pageTableEnd;
    Runtime::Zero(s_heapSegments, sizeof(HeapSegmentHeader));
    s_heapSegments->totalSize = KERNEL_HEAP_SIZE;
}

void *Memory::AllocatePage() {
    if (!s_freePages.GetCount())
        return nullptr;

    auto page = s_freePages.PopHead();
    page->flags.isAllocated = 1;
    page->flags.isKernelData = 1;

    auto address = (void *) ((page - s_pages) * PAGE_SIZE);
    Runtime::Zero(address, PAGE_SIZE);

    return address;
}

void Memory::DeallocatePage(void *address) {
    auto page = s_pages + ((uint32_t) address / PAGE_SIZE);
    page->flags.isAllocated = 0;
    s_freePages.Add(page);
}

void *Memory::Allocate(usize size) {
    HeapSegmentHeader *curr, *best = nullptr;

    size += sizeof(HeapSegmentHeader);
    size += size % 16 ? 16 - (size % 16) : 0;

    usize bestDifference = ~0;
    for (curr = s_heapSegments; curr; curr = curr->next) {
        usize difference = curr->totalSize - size;
        if (!curr->isAllocated && difference < bestDifference && curr->totalSize >= size) {
            best = curr;
            bestDifference = difference;
        }
    }

    if (!best)
        return nullptr;

    if (bestDifference > 2 * sizeof(HeapSegmentHeader)) {
        auto targetAddress = (HeapSegmentHeader *) ((usize) best + size);
        Runtime::Zero(targetAddress, sizeof(HeapSegmentHeader));

        curr = best->next;
        best->next = targetAddress;
        best->next->next = curr;
        best->next->prev = best;
        best->next->totalSize = best->totalSize - size;
        best->totalSize = size;
    }

    best->isAllocated = 1;

    return best + 1;
}

void Memory::Deallocate(void *address) {
    return;
    if (!address) return;

    auto segment = (HeapSegmentHeader *) ((usize) address - sizeof(HeapSegmentHeader));
    segment->isAllocated = 0;

    while (segment->prev != nullptr && !segment->prev->isAllocated) {
        segment->prev->next = segment->next;
        segment->next->prev = segment->prev;
        segment->prev->totalSize += segment->totalSize;
        segment = segment->prev;
    }

    while (segment->next != nullptr && !segment->next->isAllocated) {
        segment->next->next->prev = segment;
        segment->next = segment->next->next;
        segment->totalSize += segment->next->totalSize;
        segment = segment->next;
    }
}
