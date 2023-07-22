#include "Framebuffer.h"
#include "Mailbox.h"

static FramebufferInformation s_framebufferInformation;

bool Framebuffer::Initialize(u32 width, u32 height, u32 depth) {
    Mailbox::PropertyInitialize();
    Mailbox::PropertyAdd(TAG_SET_PHYSICAL_SIZE, width, height);
    Mailbox::PropertyAdd(TAG_SET_VIRTUAL_SIZE, width, height);
    Mailbox::PropertyAdd(TAG_SET_DEPTH, depth);
    Mailbox::PropertyAdd(TAG_SET_VIRTUAL_OFFSET, 0, 0);
    Mailbox::PropertyAdd(TAG_SET_OVERSCAN, 0, 0, 0, 0);
    Mailbox::PropertyAdd(TAG_ALLOCATE_BUFFER);
    Mailbox::PropertyAdd(TAG_GET_PHYSICAL_SIZE);
    Mailbox::PropertyAdd(TAG_GET_DEPTH);
    Mailbox::PropertyAdd(TAG_GET_PITCH);

    if (!Mailbox::PropertyExecute())
        return false;

    auto framebufferAddressProperty = Mailbox::PropertyGet(TAG_ALLOCATE_BUFFER);
    if (!framebufferAddressProperty)
        return false;

    s_framebufferInformation.address = (RW32 *) framebufferAddressProperty->buffer32[0];

    auto framebufferPhysicalSizeProperty = Mailbox::PropertyGet(TAG_GET_PHYSICAL_SIZE);
    if (!framebufferPhysicalSizeProperty)
        return false;

    s_framebufferInformation.width = framebufferPhysicalSizeProperty->buffer32[0];
    s_framebufferInformation.height = framebufferPhysicalSizeProperty->buffer32[1];

    auto framebufferDepthProperty = Mailbox::PropertyGet(TAG_GET_DEPTH);
    if (!framebufferDepthProperty)
        return false;

    s_framebufferInformation.depth = framebufferDepthProperty->buffer32[0];

    auto framebufferPitchProperty = Mailbox::PropertyGet(TAG_GET_PITCH);
    if (!framebufferPitchProperty)
        return false;

    s_framebufferInformation.pitch = framebufferPitchProperty->buffer32[0];

    return true;
}

FramebufferInformation *Framebuffer::GetInformation() {
    return &s_framebufferInformation;
}