#include "Memory.h"
#include <Ati/IntrusiveList.h>
#include <board/Interrupt.h>
#include <board/Mailbox.h>
#include <support/Logging.h>
#include <support/Runtime.h>

#define BLOCK_ALIGN 16
#define ALIGN_MASK (BLOCK_ALIGN - 1)
#define PAGE_MASK (PAGE_SIZE - 1)

#define BLOCK_MAGIC 0x424C4D43
#define FREEPAGE_MAGIC 0x50474D43

#define MEM_HEAP_START 0x400000

struct BlockHeader {
    u32 nMagic;
    u32 nSize;
    BlockHeader *pNext;
    u32 nPadding;
    u8 Data[0];
};

struct BlockBucket {
    unsigned int nSize;
    unsigned int nCount;
    unsigned int nMaxCount;
    BlockHeader *pFreeList;
};

struct FreePage {
    unsigned int nMagic;
    FreePage *pNext;
};

struct PageBucket {
    unsigned int nCount;
    unsigned int nMaxCount;
    FreePage *pFreeList;
};

static u8 *s_pNextBlock;
static u8 *s_pBlockLimit;
static u8 *s_pNextPage;
static u8 *s_pPageLimit;

static BlockBucket s_BlockBucket[] = {{0x40}, {0x400}, {0x1000}, {0x4000}, {0x40000}, {0x80000}, {0}};
static PageBucket s_PageBucket;

void Memory::Initialize() {
    Mailbox::PropertyInitialize();
    Mailbox::PropertyAdd(TAG_GET_ARM_MEMORY);
    if (!Mailbox::PropertyExecute())
        return;

    auto armMemoryProperty = Mailbox::PropertyGet(TAG_GET_ARM_MEMORY);
    if (!armMemoryProperty)
        return;

    auto memoryBase = armMemoryProperty->buffer32[0];
    auto memorySize = armMemoryProperty->buffer32[1];
    auto memoryLimit = memoryBase + memorySize;

    if (memoryBase < MEM_HEAP_START)
        memoryBase = MEM_HEAP_START;

    memorySize = memoryLimit - memoryBase;

    u32 quarterSize = memorySize / 4;

    s_pNextBlock = (u8 *) memoryBase;
    s_pBlockLimit = (u8 *) (memoryBase + quarterSize);
    s_pNextPage = (u8 *) ((memoryBase + quarterSize + PAGE_SIZE) & ~PAGE_MASK);
    s_pPageLimit = (u8 *) memoryLimit;

    Logging::Info("kernel", "Memory: %d MiB available", memorySize / 1024 / 1024);
}

void *Memory::AllocatePage() {
    Interrupt::EnterCriticalSection();

    if (++s_PageBucket.nCount > s_PageBucket.nMaxCount)
        s_PageBucket.nMaxCount = s_PageBucket.nCount;

    FreePage *freePage;
    if ((freePage = s_PageBucket.pFreeList) != nullptr) {
        s_PageBucket.pFreeList = freePage->pNext;
        freePage->nMagic = 0;
    } else {
        freePage = (FreePage *) s_pNextPage;
        s_pNextPage += PAGE_SIZE;

        if (s_pNextPage > s_pPageLimit) {
            Interrupt::LeaveCriticalSection();
            return nullptr;
        }
    }

    Interrupt::LeaveCriticalSection();

    return freePage;
}

void Memory::DeallocatePage(void *address) {
    auto pFreePage = (FreePage *) address;

    Interrupt::EnterCriticalSection();

    pFreePage->nMagic = FREEPAGE_MAGIC;

    pFreePage->pNext = s_PageBucket.pFreeList;
    s_PageBucket.pFreeList = pFreePage;
    s_PageBucket.nCount--;

    Interrupt::LeaveCriticalSection();
}

void *Memory::Allocate(usize size) {
    Interrupt::EnterCriticalSection();

    BlockBucket *pBucket;
    for (pBucket = s_BlockBucket; pBucket->nSize > 0; pBucket++) {
        if (size <= pBucket->nSize) {
            size = pBucket->nSize;
            if (++pBucket->nCount > pBucket->nMaxCount)
                pBucket->nMaxCount = pBucket->nCount;
            break;
        }
    }

    BlockHeader *pBlockHeader;
    if (pBucket->nSize > 0 && (pBlockHeader = pBucket->pFreeList) != nullptr) {
        pBucket->pFreeList = pBlockHeader->pNext;
    } else {
        pBlockHeader = (BlockHeader *) s_pNextBlock;

        s_pNextBlock += (sizeof(BlockHeader) + size + BLOCK_ALIGN - 1) & ~ALIGN_MASK;
        if (s_pNextBlock > s_pBlockLimit) {
            Interrupt::LeaveCriticalSection();
            return nullptr;
        }

        pBlockHeader->nMagic = BLOCK_MAGIC;
        pBlockHeader->nSize = (unsigned) size;
    }

    Interrupt::LeaveCriticalSection();

    pBlockHeader->pNext = nullptr;

    return pBlockHeader->Data;
}

void Memory::Deallocate(void *address) {
    auto pBlockHeader = (BlockHeader *) ((u32) address - sizeof(BlockHeader));
    for (auto pBucket = s_BlockBucket; pBucket->nSize > 0; pBucket++) {
        if (pBlockHeader->nSize == pBucket->nSize) {
            Interrupt::EnterCriticalSection();

            pBlockHeader->pNext = pBucket->pFreeList;
            pBucket->pFreeList = pBlockHeader;
            pBucket->nCount--;

            Interrupt::LeaveCriticalSection();

            break;
        }
    }
}
