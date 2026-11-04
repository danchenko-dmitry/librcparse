#pragma once

#include <cstdint>
#include <functional>
#include <system_error>
#include "tl/expected.hpp"

namespace msp {

struct MspPacketHead {
    uint8_t direction;
    uint8_t type;
};
    
// MSP protocol callback functions for packet catcher
tl::expected<MspPacketHead, std::error_code> mspGetTypeHead(const uint8_t* data, size_t size);
bool mspIsPacketHead(const uint8_t* data, size_t size);
bool mspIsValidPacket(const uint8_t* data, size_t size);
size_t mspGetTotalPacketSize(const uint8_t* data, size_t size);
void mspProcessValidPacket(const uint8_t* data, size_t size);


} // namespace msp
