#pragma once

#include <cstdint>
#include <functional>


namespace crsf {

// CRSF protocol callback functions
bool crsfIsPacketHead(const uint8_t* data, size_t size);
bool crsfIsValidPacket(const uint8_t* data, size_t size);
size_t crsfGetTotalPacketSize(const uint8_t* data, size_t size);
void crsfProcessValidPacket(const uint8_t* data, size_t size);

} // namespace crsf
