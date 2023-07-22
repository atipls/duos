#include "Mailbox.h"
#include "support/Runtime.h"
#include <cstdarg>

enum MailboxBufferOffset {
    PT_OSIZE = 0,
    PT_OREQUEST_OR_RESPONSE = 1,
};

enum MailboxTagState {
    TAG_STATE_REQUEST = 0,
    TAG_STATE_RESPONSE = 1,
};

enum MailboxTagOffset {
    T_OIDENT = 0,
    T_OVALUE_SIZE = 1,
    T_ORESPONSE = 2,
    T_OVALUE = 3,
};

enum MailboxStatus {
    ARM_MS_FULL = 0x80000000,
    ARM_MS_EMPTY = 0x40000000,
    ARM_MS_LEVEL = 0x400000FF,
};

static u32 s_mailboxBuffer[8192] __attribute__((aligned(16)));
static u32 s_mailboxBufferOffset = 0;
static MailboxProperty s_mailboxProperty;

usize Mailbox::Read(MailboxChannel channel) {
    u32 value = 0;
    while ((value & 0xF) != channel) {
        while (MAILBOX_BASE->status & ARM_MS_EMPTY) {}
        value = MAILBOX_BASE->read;
    }

    return value >> 4;
}

void Mailbox::Write(MailboxChannel channel, usize value) {
    value &= ~(0xF);
    value |= channel;

    while ((MAILBOX_BASE->status & ARM_MS_FULL) != 0) {}

    MAILBOX_BASE->write = value;
}

void Mailbox::PropertyInitialize() {
    s_mailboxBuffer[PT_OSIZE] = 12;
    s_mailboxBuffer[PT_OREQUEST_OR_RESPONSE] = TAG_STATE_REQUEST;
    s_mailboxBufferOffset = 2;
    s_mailboxBuffer[s_mailboxBufferOffset] = 0;
}

void Mailbox::PropertyAdd(MailboxTag tag, ...) {
    va_list vaList;
    va_start(vaList, tag);

    s_mailboxBuffer[s_mailboxBufferOffset++] = tag;

    switch (tag) {
        case TAG_GET_FIRMWARE_VERSION:
        case TAG_GET_BOARD_MODEL:
        case TAG_GET_BOARD_REVISION:
        case TAG_GET_BOARD_MAC_ADDRESS:
        case TAG_GET_BOARD_SERIAL:
        case TAG_GET_ARM_MEMORY:
        case TAG_GET_VC_MEMORY:
        case TAG_GET_DMA_CHANNELS:
            s_mailboxBuffer[s_mailboxBufferOffset++] = 8;
            s_mailboxBuffer[s_mailboxBufferOffset++] = TAG_STATE_REQUEST;
            s_mailboxBufferOffset += 2;
            break;

        case TAG_GET_CLOCKS:
        case TAG_GET_COMMAND_LINE:
            s_mailboxBuffer[s_mailboxBufferOffset++] = 256;
            s_mailboxBuffer[s_mailboxBufferOffset++] = TAG_STATE_REQUEST;
            s_mailboxBufferOffset += 256 >> 2;
            break;

        case TAG_ALLOCATE_BUFFER:
        case TAG_GET_MAX_CLOCK_RATE:
        case TAG_GET_MIN_CLOCK_RATE:
        case TAG_GET_CLOCK_RATE:
            s_mailboxBuffer[s_mailboxBufferOffset++] = 8;
            s_mailboxBuffer[s_mailboxBufferOffset++] = TAG_STATE_REQUEST;
            s_mailboxBuffer[s_mailboxBufferOffset++] = va_arg(vaList, u32);
            s_mailboxBuffer[s_mailboxBufferOffset++] = 0;
            break;

        case TAG_SET_CLOCK_RATE:
            s_mailboxBuffer[s_mailboxBufferOffset++] = 12;
            s_mailboxBuffer[s_mailboxBufferOffset++] = TAG_STATE_REQUEST;
            s_mailboxBuffer[s_mailboxBufferOffset++] = va_arg(vaList, u32); /* Clock ID */
            s_mailboxBuffer[s_mailboxBufferOffset++] = va_arg(vaList, u32); /* Rate (in Hz) */
            s_mailboxBuffer[s_mailboxBufferOffset++] = va_arg(vaList, u32); /* Skip turbo setting if == 1 */
            break;

        case TAG_GET_PHYSICAL_SIZE:
        case TAG_SET_PHYSICAL_SIZE:
        case TAG_TEST_PHYSICAL_SIZE:
        case TAG_GET_VIRTUAL_SIZE:
        case TAG_SET_VIRTUAL_SIZE:
        case TAG_TEST_VIRTUAL_SIZE:
        case TAG_GET_VIRTUAL_OFFSET:
        case TAG_SET_VIRTUAL_OFFSET:
            s_mailboxBuffer[s_mailboxBufferOffset++] = 8;
            s_mailboxBuffer[s_mailboxBufferOffset++] = TAG_STATE_REQUEST;

            if ((tag == TAG_SET_PHYSICAL_SIZE) ||
                (tag == TAG_SET_VIRTUAL_SIZE) ||
                (tag == TAG_SET_VIRTUAL_OFFSET) ||
                (tag == TAG_TEST_PHYSICAL_SIZE) ||
                (tag == TAG_TEST_VIRTUAL_SIZE)) {
                s_mailboxBuffer[s_mailboxBufferOffset++] = va_arg(vaList, u32); /* Width */
                s_mailboxBuffer[s_mailboxBufferOffset++] = va_arg(vaList, u32); /* Height */
            } else {
                s_mailboxBufferOffset += 2;
            }
            break;

        case TAG_GET_ALPHA_MODE:
        case TAG_SET_ALPHA_MODE:
        case TAG_GET_DEPTH:
        case TAG_SET_DEPTH:
        case TAG_GET_PIXEL_ORDER:
        case TAG_SET_PIXEL_ORDER:
        case TAG_GET_PITCH:
            s_mailboxBuffer[s_mailboxBufferOffset++] = 4;
            s_mailboxBuffer[s_mailboxBufferOffset++] = TAG_STATE_REQUEST;

            if ((tag == TAG_SET_DEPTH) ||
                (tag == TAG_SET_PIXEL_ORDER) ||
                (tag == TAG_SET_ALPHA_MODE)) {
                s_mailboxBuffer[s_mailboxBufferOffset++] = va_arg(vaList, u32);
            } else {
                s_mailboxBufferOffset += 1;
            }
            break;

        case TAG_GET_OVERSCAN:
        case TAG_SET_OVERSCAN:
            s_mailboxBuffer[s_mailboxBufferOffset++] = 16;
            s_mailboxBuffer[s_mailboxBufferOffset++] = TAG_STATE_REQUEST;

            if (tag == TAG_SET_OVERSCAN) {
                s_mailboxBuffer[s_mailboxBufferOffset++] = va_arg(vaList, u32); /* Top pixels */
                s_mailboxBuffer[s_mailboxBufferOffset++] = va_arg(vaList, u32); /* Bottom pixels */
                s_mailboxBuffer[s_mailboxBufferOffset++] = va_arg(vaList, u32); /* Left pixels */
                s_mailboxBuffer[s_mailboxBufferOffset++] = va_arg(vaList, u32); /* Right pixels */
            } else {
                s_mailboxBufferOffset += 4;
            }
            break;

        case TAG_SET_POWER_STATE:
            s_mailboxBuffer[s_mailboxBufferOffset++] = 8;
            s_mailboxBuffer[s_mailboxBufferOffset++] = TAG_STATE_REQUEST;
            s_mailboxBuffer[s_mailboxBufferOffset++] = va_arg(vaList, u32); /* Device ID */
            s_mailboxBuffer[s_mailboxBufferOffset++] = va_arg(vaList, u32); /* Device state */
            break;

        default:
            s_mailboxBufferOffset--;
            break;
    }

    s_mailboxBuffer[s_mailboxBufferOffset] = 0;
    va_end(vaList);
}

bool Mailbox::PropertyExecute() {
    s_mailboxBuffer[PT_OSIZE] = (s_mailboxBufferOffset + 1) << 2;
    s_mailboxBuffer[PT_OREQUEST_OR_RESPONSE] = 0;

    Write(MB0_TAGS_ARM_TO_VC, (u32) s_mailboxBuffer);

    return !!Read(MB0_TAGS_ARM_TO_VC);
}

MailboxProperty *Mailbox::PropertyGet(MailboxTag tag) {
    u32 *tagBuffer = nullptr;

    u32 index = 2;
    while (index < (s_mailboxBuffer[PT_OSIZE] >> 2)) {
        if (s_mailboxBuffer[index] == tag) {
            tagBuffer = &s_mailboxBuffer[index];
            break;
        }
        index += (s_mailboxBuffer[index + 1] >> 2) + 3;
    }

    if (!tagBuffer)
        return nullptr;

    s_mailboxProperty.tag = tag;
    s_mailboxProperty.size = tagBuffer[T_ORESPONSE] & 0xFFFF;
    Runtime::Copy(s_mailboxProperty.buffer, &tagBuffer[T_OVALUE], s_mailboxProperty.size);

    return &s_mailboxProperty;
}

int SetPowerStateOn (unsigned nDeviceId) {
    Mailbox::PropertyInitialize();
    Mailbox::PropertyAdd(TAG_SET_POWER_STATE, nDeviceId, 1 | 1 << 1);
    Mailbox::PropertyExecute();

    MailboxProperty *pMailboxProperty = Mailbox::PropertyGet(TAG_SET_POWER_STATE);
    if (!pMailboxProperty)
        return 0;

    return (int)pMailboxProperty->buffer32[1];
}

int GetMACAddress (unsigned char Buffer[6]) {
    Mailbox::PropertyInitialize();
    Mailbox::PropertyAdd(TAG_GET_BOARD_MAC_ADDRESS, 0);
    Mailbox::PropertyExecute();

    MailboxProperty *pMailboxProperty = Mailbox::PropertyGet(TAG_GET_BOARD_MAC_ADDRESS);
    if (!pMailboxProperty)
        return -1;

    Runtime::Copy(Buffer, pMailboxProperty->buffer, 6);

    return 0;
}