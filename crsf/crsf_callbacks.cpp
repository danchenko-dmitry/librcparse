#include "crsf_callbacks.h"
#include "crsf.h"
#include <cassert>
#include <vector>
#include <iostream>
#include <iomanip>

namespace crsf {

// Check if data at the given position looks like a valid CRSF packet start
bool crsfIsPacketHead(const uint8_t* data, size_t size) {
    if (size < MIN_PACKET_SIZE) return false;
    
    // Check sync byte (0xC8 or 0xEE) according to CRSF spec
    uint8_t sync = data[0];
    if (sync != 0xC8 && sync != 0xEE) return false;
    
    // Check if length field is reasonable (type + payload + crc)
    uint8_t length = data[1];
    if (length < 2 || length > 61) return false; // min: type(1) + crc(1), max: 61
    
    // Check if total packet size would be reasonable
    size_t totalSize = SYNC_SIZE + LEN_SIZE + length;
    if (totalSize > MAX_PACKET_SIZE) return false;
    
    // Check if the packet type is valid (CRSF packet types are 0x02-0x33, including 0x32 for Command)
    uint8_t packetType = data[2];
    if (packetType < 0x02 || packetType > 0x33) return false;
    
    // Special handling for Command packets (0x32) - they have extended payload structure
    if (packetType == static_cast<uint8_t>(PacketType::Command)) {
        // Command packets need at least: realm + subcommand + crc8-ba
        if (length < 3) return false; // type(1) + realm(1) + subcommand(1) + crc8-ba(1)
    }
    
    return true;
}

// Validate that the CRSF packet data is correct (CRC, structure, etc.)
bool crsfIsValidPacket(const uint8_t* data, size_t size) {
    if (size < MIN_PACKET_SIZE) {
        //std::cout << "DEBUG: Packet too small: " << size << " < " << MIN_PACKET_SIZE << std::endl;
        return false;
    }
    
    // Basic packet structure validation
    uint8_t sync = data[0];
    if (sync != 0xC8 && sync != 0xEE) {
        //std::cout << "DEBUG: Invalid sync byte: 0x" << std::hex << (int)sync << std::endl;
        return false; // Must be valid sync byte
    }
    
    uint8_t declaredLength = data[1];
    if (SYNC_SIZE + LEN_SIZE + declaredLength != size) {
        //std::cout << "DEBUG: Length mismatch: declared=" << (int)declaredLength << ", expected=" << (SYNC_SIZE + LEN_SIZE + declaredLength) << ", actual=" << size << std::endl;
        return false;
    }
    
    uint8_t packetType = data[2];
    if (packetType < 0x02 || packetType > 0x33) {
        //std::cout << "DEBUG: Invalid packet type: 0x" << std::hex << (int)packetType << std::endl;
        return false; // CRSF packet types are 0x02-0x33
    }
    
    // Special handling for Command packets (0x32) - they have CRC8-BA
    if (packetType == static_cast<uint8_t>(PacketType::Command)) {
        // For command packets, we need to verify both the main CRC and CRC8-BA
        // Main CRC covers type + extended payload (excluding sync byte)
        uint8_t calculatedCRC = calculateCRC8(&data[2], size - 3); // type + extended payload
        uint8_t storedCRC = data[size - 1];
        
        if (calculatedCRC != storedCRC) return false;
        
        // Verify CRC8-BA for the extended payload (realm + subcommand + payload + crc8-ba)
        // The extended payload starts at data[3] (after sync + len + type)
        size_t extendedPayloadSize = declaredLength - 1; // Exclude the main CRC
        if (extendedPayloadSize >= 2) { // Need at least realm + subcommand
            uint8_t calculatedCRC8BA = ::crsf::calculateCRC8BA(&data[3], extendedPayloadSize - 1); // Exclude CRC8-BA itself
            uint8_t storedCRC8BA = data[size - 2]; // CRC8-BA is the second-to-last byte
            return calculatedCRC8BA == storedCRC8BA;
        }
        return false;
    }
    
    // Calculate and verify CRC for regular packets
    // CRC covers type + payload (bytes 2 onwards, excluding sync byte)
    uint8_t calculatedCRC = calculateCRC8(&data[2], size - 3); // type + payload
    uint8_t storedCRC = data[size - 1];
    
    bool crcMatch = (calculatedCRC == storedCRC);
    if (!crcMatch) {
        //std::cout << "DEBUG: CRC mismatch: calculated=0x" << std::hex << (int)calculatedCRC << ", stored=0x" << (int)storedCRC << std::endl;
    }
    
    return crcMatch;
}

// Get the size of a CRSF packet starting at the given position
size_t crsfGetTotalPacketSize(const uint8_t* data, size_t size) {
    if (size < MIN_PACKET_SIZE) return 0;
    
    // Get the declared length from the packet (includes type + payload + crc)
    uint8_t declaredLength = data[1];
    
    // Total packet size = sync + len + declaredLength
    size_t totalPacketSize = SYNC_SIZE + LEN_SIZE + declaredLength;
    
    return totalPacketSize;
}

// Process a valid CRSF packet
void crsfProcessValidPacket(const uint8_t* data, size_t size) {
    if (size < MIN_PACKET_SIZE) return;
    
    uint8_t packetType = data[2];
    
    // Handle different packet types
    switch (static_cast<PacketType>(packetType)) {
        case PacketType::Command: {
            // Create a command packet wrapper for processing
            std::vector<uint8_t> packetData(data, data + size);
            etl::span<uint8_t> packetSpan(packetData);
            CRSFCommand command(packetSpan);
            
            // You can add specific command processing here
            // For now, we'll just acknowledge that we received a command
            break;
        }
        // Add other packet type handlers as needed
        default:
            // Generic packet processing
            break;
    }
}

} // namespace crsf
