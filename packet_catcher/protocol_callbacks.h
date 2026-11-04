#pragma once

#include <cstdint>
#include <functional>

// Generic protocol callbacks structure for packet detection and validation
struct ProtocolCallbacks {
    // Check if data at the given position looks like a valid packet start
    std::function<bool(const uint8_t* data, size_t size)> isPacketHead;
    
    // Validate that the packet data is correct (CRC, structure, etc.)
    std::function<bool(const uint8_t* data, size_t size)> isValidPacket;
    
    // Get the size of a packet starting at the given position
    std::function<size_t(const uint8_t* data, size_t size)> getTotalPacketSize;

    // Process complete & valid packet
    std::function<void(const uint8_t* data, size_t size)> processValidPacket;
    
    // Process discarded data (data that doesn't match the protocol)
    // If this callback is not initialized (null), data is discarded silently
    std::function<void(const uint8_t* data, size_t size)> discardedData;
};
