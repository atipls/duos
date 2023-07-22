#pragma once
#include "Ati/Types.h"

enum ProxyOpcode {
    PROXY_OPCODE_NOP,
    PROXY_OPCODE_PING,
    PROXY_OPCODE_PONG,
    PROXY_OPCODE_READ,
    PROXY_OPCODE_WRITE,

    PROXY_OPCODE_COUNT = 0xFFFFFFFF,// Force 32-bit
};

enum ProxyStatus {
    PROXY_STATUS_REQUEST,
    PROXY_STATUS_RESPONSE,
    PROXY_STATUS_EVENT,
    PROXY_STATUS_ERROR,

    PROXY_STATUS_COUNT = 0xFFFFFFFF,// Force 32-bit
};

struct ProxyCall {
    ProxyOpcode opcode;
    ProxyStatus status;
    usize checksum;
    u32 buffer[5];
};

namespace Proxy {
    [[noreturn]] void TaskEntry();

    void Send(ProxyOpcode opcode, ProxyStatus status, u32 *buffer, usize size);

    template<usize size>
    void Send(ProxyOpcode opcode, ProxyStatus status, u32 (&buffer)[size]) {
        Send(opcode, status, buffer, size);
    }

}// namespace Proxy