#include "msp_callbacks.h"
#include "msp.h"
#include "msp_errors.h"
#include <cassert>

namespace msp {

tl::expected<MspPacketHead, std::error_code> mspGetTypeHead(const uint8_t* data, size_t size) {
    // as we need to get from packet maximum data we will not check if the packet is valid:
    if (size < 4) return tl::unexpected(std::error_code(msp::MspErrorCode::INVALID_PACKET));
    
    // Check MSP header: '$M<|>'
    if (data[0] != '$') return tl::unexpected(std::error_code(msp::MspErrorCode::INVALID_PACKET));
    if (data[1] != 'M') return tl::unexpected(std::error_code(msp::MspErrorCode::INVALID_PACKET));

    // Check direction byte ('<' for request, '>' for response)
    if (data[2] != static_cast<uint8_t>(PacketDirection::REQUEST) && 
        data[2] != static_cast<uint8_t>(PacketDirection::RESPONSE)) {
        return tl::unexpected(std::error_code(msp::MspErrorCode::INVALID_PACKET));
    }
    
    MspPacketHead packetHead;
    packetHead.direction = data[2];
    packetHead.type = data[3];

    return tl::expected<MspPacketHead, std::error_code>(packetHead);
}

// Check if data at the given position looks like a valid MSP packet start
bool mspIsPacketHead(const uint8_t* data, size_t size) {
    if (size < MIN_PACKET_SIZE) return false;
    
    // Check MSP header: '$M<|>'
    if (data[0] != '$') return false;
    if (data[1] != 'M') return false;
    
    // Check direction byte ('<' for request, '>' for response)
    uint8_t direction = data[2];
    if (direction != static_cast<uint8_t>(PacketDirection::REQUEST) && 
        direction != static_cast<uint8_t>(PacketDirection::RESPONSE)) {
        return false;
    }
    
    // Check if payload size is reasonable
    uint8_t payloadSize = data[3];
    if (payloadSize > MAX_PAYLOAD_SIZE) return false;
    
    // Check if total packet size would be reasonable
    size_t totalSize = MSP_HEADER_SIZE + MSP_PAYLOAD_SIZE + MSP_CMD_SIZE + payloadSize + MSP_CRC_SIZE;
    if (totalSize > MAX_PACKET_SIZE) return false;
    
    // Check if the command is valid (MSP command range is 100-244)
    uint8_t command = data[4];
    if (command < 100 || command > 244) return false;
    
    return true;
}

// Validate that the MSP packet data is correct (CRC, structure, etc.)
bool mspIsValidPacket(const uint8_t* data, size_t size) {
    if (size < MIN_PACKET_SIZE) return false;
    
    // Basic packet structure validation
    if (data[0] != '$' || data[1] != 'M') return false;
    
    uint8_t direction = data[2];
    if (direction != static_cast<uint8_t>(PacketDirection::REQUEST) && 
        direction != static_cast<uint8_t>(PacketDirection::RESPONSE)) {
        return false;
    }
    
    uint8_t declaredPayloadSize = data[3];
    uint8_t command = data[4];
    
    // Check command range
    if (command < 100 || command > 244) return false;
    
    // Check if packet size matches declared size
    size_t expectedSize = MSP_HEADER_SIZE + MSP_PAYLOAD_SIZE + MSP_CMD_SIZE + declaredPayloadSize + MSP_CRC_SIZE;
    if (size != expectedSize) return false;
    
    // Calculate and verify CRC
    // CRC covers size + cmd + data (skip header '$M<|>')
    uint8_t calculatedCRC = calculateCRC8(&data[MSP_HEADER_SIZE], size - MSP_HEADER_SIZE - MSP_CRC_SIZE);
    uint8_t storedCRC = data[size - 1];
    
    return calculatedCRC == storedCRC;
}

// Get the size of an MSP packet starting at the given position
size_t mspGetTotalPacketSize(const uint8_t* data, size_t size) {
    if (size < MIN_PACKET_SIZE) return 0;
    
    // Get the declared payload size from the packet
    uint8_t declaredPayloadSize = data[3];
    
    // Total packet size = header + size + cmd + payload + crc
    size_t totalPacketSize = MSP_HEADER_SIZE + MSP_PAYLOAD_SIZE + MSP_CMD_SIZE + declaredPayloadSize + MSP_CRC_SIZE;
    
    return totalPacketSize;
}

// Process a valid MSP packet (placeholder for future implementation)
void mspProcessValidPacket(const uint8_t* data, size_t size) {
    // This function can be used to:
    // - Parse specific MSP packet types
    // - Extract telemetry data
    // - Handle different MSP commands
    // - Log packet information
    // - Forward packets to other systems
    
    // For now, this is a placeholder that can be extended based on specific needs
    (void)data;  // Suppress unused parameter warning
    (void)size;  // Suppress unused parameter warning
}

} // namespace msp
