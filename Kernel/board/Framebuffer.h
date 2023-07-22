#pragma once

#include <Ati/Types.h>

struct FramebufferInformation {
    RW32 *address;
    u32 width;
    u32 height;
    u32 depth;
    u32 pitch;
};

namespace Framebuffer {
    bool Initialize(u32 width, u32 height, u32 depth);

    FramebufferInformation *GetInformation();
}