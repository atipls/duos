#pragma once
#include <Ati/Types.h>

enum AtagTag {
    ATAG_NONE = 0x00000000,
    ATAG_CORE = 0x54410001,
    ATAG_MEMORY = 0x54410002,
    ATAG_INITRD = 0x54420005,
    ATAG_CMDLINE = 0x54410009,
};

struct AtagMemory {
    u32 size;
    u32 start;
};

struct AtagInitrd {
    u32 start;
    u32 size;
};

struct AtagCmdline {
    char cmdline[1];
};

struct Atag {
    uint32_t size;
    AtagTag tag;
    union {
        AtagMemory memory;
        AtagInitrd initrd;
        AtagCmdline cmdline;
    };
};
