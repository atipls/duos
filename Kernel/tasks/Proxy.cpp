#include "Proxy.h"
#include <support/Runtime.h>
#include <board/Uart.h>

void CalculateChecksum(ProxyCall *call) {
    u8 *buffer = (u8 *) call->buffer;
    call->checksum = 0x811C9DC5;
    for (usize i = 0; i < sizeof(call->buffer); ++i) {
        call->checksum ^= buffer[i];
        call->checksum *= 0x01000193;
    }

    call->checksum ^= call->opcode;
    call->checksum *= 0x01000193;
}

bool VerifyChecksum(ProxyCall *call) {
    u8 *buffer = (u8 *) call->buffer;
    usize checksum = 0x811C9DC5;
    for (usize i = 0; i < sizeof(call->buffer); ++i) {
        checksum ^= buffer[i];
        checksum *= 0x01000193;
    }

    checksum ^= call->opcode;
    checksum *= 0x01000193;

    return checksum == call->checksum;
}

void WriteResponse(ProxyCall *call) {
    CalculateChecksum(call);

    u8 *buffer = (u8 *) &call;
    for (usize i = 0; i < sizeof(*call); i++)
        Uart::TxByte(buffer[i]);
}

void HandleProxyCall(ProxyCall *call) {
    if (!VerifyChecksum(call))
        return;

    if (call->status == PROXY_STATUS_ERROR)
        return;

    switch (call->opcode) {
        case PROXY_OPCODE_NOP:
        case PROXY_OPCODE_PONG:
            break;
        case PROXY_OPCODE_PING:
            call->opcode = PROXY_OPCODE_PONG;
            WriteResponse(call);
            break;
        case PROXY_OPCODE_READ:
            call->buffer[0] = *(usize *) call->buffer[0];
            WriteResponse(call);
            break;
        case PROXY_OPCODE_WRITE:
            *(usize *) call->buffer[0] = call->buffer[1];
            break;
        default:
            break;
    }
}

[[noreturn]] void Proxy::TaskEntry() {
    u8 buffer[sizeof(ProxyCall)];
    usize bufferIndex = 0;

    while (true) {
        if (bufferIndex < sizeof(ProxyCall))
            buffer[bufferIndex++] = Uart::RxByte();

        if (bufferIndex == sizeof(ProxyCall)) {
            auto *call = (ProxyCall *) buffer;
            HandleProxyCall(call);
            bufferIndex = 0;
        }
    }
}

void Proxy::Send(ProxyOpcode opcode, ProxyStatus status, u32 *buffer, usize size) {
    ProxyCall call = {};
    call.opcode = opcode;
    call.status = status;
    Runtime::Copy(call.buffer, buffer, size);
    CalculateChecksum(&call);

    u8 *buffer8 = (u8 *) &call;
    for (usize i = 0; i < sizeof(call); i++)
        Uart::TxByte(buffer8[i]);
}