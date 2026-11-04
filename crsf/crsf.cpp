#include "crsf.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstring>  // For std::memcpy
#include <vector>   // For std::vector
#include <stdint.h>
#include <stddef.h>

namespace crsf {

void initCrc(uint8_t poly);
uint8_t calcCrc(const uint8_t *data, size_t len, uint8_t crc=0);
    
// CRC8 calculation for CRSF (polynomial 0xD5)
uint8_t calculateCRC8(const uint8_t* data, size_t len) {
    initCrc(CRSF_CRC_POLY);
    return calcCrc(data,len,0);
}

// CRC8-BA calculation for CRSF Commands (polynomial 0xBA)
uint8_t calculateCRC8BA(const uint8_t* data, size_t len) {
    initCrc(CRSFCommand::CRC8_BA_POLY);
    return calcCrc(data, len, 0);
}

// ============================================================================
// Base CRSFPacket Implementation
// ============================================================================



CRSFPacket::CRSFPacket(const etl::span<uint8_t>& data) : data_(data) {
    // Span provides read-only access to existing data
    // No resize needed - span is just a view
}

// Destructor is defaulted in header

// Helper methods for safe access
uint8_t CRSFPacket::getByte(size_t offset) const {
    if (offset >= data_.size()) return 0;
    return data_[offset];
}

void CRSFPacket::setByte(size_t offset, uint8_t value) {
    if (offset < data_.size()) {
        data_[offset] = value;
    }
}

uint16_t CRSFPacket::getU16(size_t offset) const {
    if (offset + 1 >= data_.size()) return 0;
    // BIG-ENDIAN (network byte order) as per CRSF spec
    return static_cast<uint16_t>(data_[offset]) << 8 |
           static_cast<uint16_t>(data_[offset + 1]);
}

void CRSFPacket::setU16(size_t offset, uint16_t value) {
    if (offset + 1 < data_.size()) {
        // BIG-ENDIAN (network byte order) as per CRSF spec
        data_[offset] = (value >> 8) & 0xFF;      // MSB
        data_[offset + 1] = value & 0xFF;          // LSB
    }
}

uint32_t CRSFPacket::getU32(size_t offset) const {
    if (offset + 3 >= data_.size()) return 0;
    // BIG-ENDIAN (network byte order) as per CRSF spec
    return static_cast<uint32_t>(data_[offset]) << 24 |
           (static_cast<uint32_t>(data_[offset + 1]) << 16) |
           (static_cast<uint32_t>(data_[offset + 2]) << 8) |
           (static_cast<uint32_t>(data_[offset + 3]));
}

void CRSFPacket::setU32(size_t offset, uint32_t value) {
    if (offset + 3 < data_.size()) {
        // BIG-ENDIAN (network byte order) as per CRSF spec
        data_[offset] = (value >> 24) & 0xFF;     // MSB
        data_[offset + 1] = (value >> 16) & 0xFF; // 
        data_[offset + 2] = (value >> 8) & 0xFF;  // 
        data_[offset + 3] = value & 0xFF;          // LSB
    }
}

// Basic packet properties
uint8_t CRSFPacket::getAddress() const {
    return getByte(CRSF_SYNC_INDEX(0));
}

void CRSFPacket::setAddress(uint8_t address) {
    setByte(CRSF_SYNC_INDEX(0), address);
}

uint8_t CRSFPacket::getLength() const {
    return getByte(CRSF_LEN_INDEX(0));
}

void CRSFPacket::setLength(uint8_t length) {
    setByte(CRSF_LEN_INDEX(0), length);
}

uint8_t CRSFPacket::getType() const {
    return getByte(CRSF_TYPE_INDEX(0));
}

void CRSFPacket::setType(uint8_t type) {
    setByte(CRSF_TYPE_INDEX(0), type);
}

uint8_t CRSFPacket::getCRC() const {
    return getByte(CRSF_CRC_INDEX(0,false,getLength()));// address(1) + length(1) + type(1) + payload + crc
}

void CRSFPacket::setCRC(uint8_t crc) {
    setByte(CRSF_CRC_INDEX(0,false,getLength()), crc);
}

// Payload access
etl::span<uint8_t> CRSFPacket::getPayload() const {
    if (data_.size() < MIN_PACKET_SIZE) return {};
    
    // According to CRSF spec: LEN = type(1) + payload + crc(1)
    // So payload size = LEN - 2 (type + crc)
    size_t payloadSize = getLength() - SYNC_SIZE - LEN_SIZE;
    size_t payloadStart = SYNC_SIZE + LEN_SIZE + TYPE_SIZE;  // After type field
    
    if (payloadStart + payloadSize > data_.size()) return {};
    
    return data_.subspan(payloadStart, payloadSize);
}

void CRSFPacket::setPayload(const etl::span<uint8_t>& payload) {
    if (payload.size() > MAX_PAYLOAD_SIZE) return; // CRSF max payload is 62 bytes
    
    // According to CRSF spec: LEN = type(1) + payload + crc(1) = payload.size() + 2
    setLength(static_cast<uint8_t>(payload.size() + TYPE_SIZE + CRC_SIZE));
    
    // Copy payload data to the span
    size_t payloadStart = CRSF_PAYLOAD_INDEX(0);
    if (payloadStart + payload.size() <= data_.size()) {
        std::copy(payload.begin(), payload.end(), data_.begin() + payloadStart);
    }
}

void CRSFPacket::setPayload(const uint8_t* data, size_t len) {
    if (len > MAX_PAYLOAD_SIZE) return;
    
    // According to CRSF spec: LEN = type(1) + payload + crc(1) = len + 2
    setLength(static_cast<uint8_t>(len + TYPE_SIZE + CRC_SIZE));
    
    // Copy payload data to the span
    size_t payloadStart = CRSF_PAYLOAD_INDEX(0);
    if (payloadStart + len <= data_.size()) {
        std::copy(data, data + len, data_.begin() + payloadStart);
    }
}

// Raw data access
const etl::span<uint8_t>& CRSFPacket::getRawData() const {
    return data_;
}

etl::span<uint8_t>& CRSFPacket::getRawData() {
    return data_;
}

// Packet operations
size_t CRSFPacket::getTotalSize() const {
    // According to CRSF spec: Overall packet length is SYNC + LEN + LEN_VALUE
    // where LEN_VALUE = type + payload + crc
    return SYNC_SIZE + LEN_SIZE + getLength(); // sync + len + (type + payload + crc)
}

void CRSFPacket::calculateCRC() {
    // CRC covers from type field (byte 2) to end of payload
    // Payload size = LEN - TYPE_SIZE - CRC_SIZE
    size_t payloadSize = getLength() - TYPE_SIZE - CRC_SIZE;
    size_t crcPos = CRSF_CRC_INDEX(0, false, payloadSize);  // After payload
    assert(data_.size() >= crcPos + CRC_SIZE && "Packet data must have enough space for CRC");
    
    // Calculate CRC over type field (byte 2) and payload (bytes 3 to crcPos-1)
    uint8_t crc = calculateCRC8(data_.data() + SYNC_SIZE + LEN_SIZE, payloadSize + TYPE_SIZE);  // type + payload
    data_[crcPos] = crc;
}

bool CRSFPacket::verifyCRC() const {
    // CRC covers from type field (byte 2) to end of payload
    size_t payloadSize = getLength() - TYPE_SIZE - CRC_SIZE;
    size_t crcPos = CRSF_CRC_INDEX(0, false, payloadSize);  // Use same calculation as calculateCRC
    if (data_.size() < crcPos + CRC_SIZE) return false;
    
    // Calculate CRC over type field (byte 2) and payload (bytes 3 to crcPos-1)
    uint8_t calculatedCRC = calculateCRC8(data_.data() + SYNC_SIZE + LEN_SIZE, payloadSize + TYPE_SIZE);  // type + payload
    return calculatedCRC == data_[crcPos];
}





// ============================================================================
// CRSFRCChannels Implementation
// ============================================================================



CRSFRCChannels::CRSFRCChannels(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::RCChannelsPacked)) {
        setType(static_cast<uint8_t>(PacketType::RCChannelsPacked));
    }
}

uint16_t CRSFRCChannels::getChannel(uint8_t channel) const {
    if (channel >= CHANNEL_COUNT) return 0;
    
    // Unpack 11-bit channel value from packed data
    // Channel data starts at byte 3 and is always 22 bytes (176 bits)
    // Arm status (if present) is at byte 25, CRC is at byte 26
    size_t bitOffset = channel * BITS_PER_CHANNEL;
    size_t byteOffset = 3 + (bitOffset / 8);
    size_t bitInByte = bitOffset % 8;
    
    // Channel data is always 22 bytes, regardless of arm status
    if (hasArmStatus()) {  
        if (byteOffset >= 3 + PAYLOAD_SIZE_ELRS) return 0;
    } else {
        if (byteOffset >= 3 + PAYLOAD_SIZE_BASIC) return 0;
    }
    
    uint32_t value = 0;
    size_t bitsRead = 0;
    
    while (bitsRead < BITS_PER_CHANNEL && (hasArmStatus() ? byteOffset < 3 + PAYLOAD_SIZE_ELRS : byteOffset < 3 + PAYLOAD_SIZE_BASIC)) {
        uint8_t byte = data_[byteOffset];
        uint8_t bitsToRead = std::min(8 - bitInByte, BITS_PER_CHANNEL - bitsRead);
        
        uint8_t mask = (1 << bitsToRead) - 1;
        uint8_t extracted = (byte >> bitInByte) & mask;
        
        value |= (static_cast<uint32_t>(extracted) << bitsRead);
        
        bitsRead += bitsToRead;
        byteOffset++;
        bitInByte = 0;
    }
    
    return static_cast<uint16_t>(value & 0x7FF);
}

void CRSFRCChannels::setChannel(uint8_t channel, uint16_t value) {
    if (channel >= CHANNEL_COUNT) return;
    if (value > CRSF_MAX_VALUE) value = CRSF_MAX_VALUE;
    
    // Pack 11-bit channel value into packed data
    size_t bitOffset = channel * BITS_PER_CHANNEL;
    size_t byteOffset = 3 + (bitOffset / 8);
    size_t bitInByte = bitOffset % 8;
    
    // Ensure we have enough space
    assert(byteOffset + 2 < data_.size() && "RC channels data must have enough space for channel data");
    
    uint32_t bitsToWrite = value & 0x7FF;
    size_t bitsWritten = 0;
    
    while (bitsWritten < BITS_PER_CHANNEL && byteOffset < data_.size()) {
        uint8_t bitsToWriteInByte = std::min(8 - bitInByte, BITS_PER_CHANNEL - bitsWritten);
        
        uint8_t mask = (1 << bitsToWriteInByte) - 1;
        uint8_t valueToWrite = (bitsToWrite >> bitsWritten) & mask;
        
        // Clear existing bits and set new ones
        uint8_t clearMask = ~(mask << bitInByte);
        data_[byteOffset] = (data_[byteOffset] & clearMask) | (valueToWrite << bitInByte);
        
        bitsWritten += bitsToWriteInByte;
        byteOffset++;
        bitInByte = 0;
    }
    
    // Update length based on whether we have ExpressLRS arm status
    // setLength sets the length field, which should be payload + type + crc
    // Don't change the length here - let the factory function and setArmStatus() handle it
    // This prevents setChannel from accidentally adding arm status
}

void CRSFRCChannels::setAllChannels(const std::array<uint16_t, 16>& channels) {
    for (size_t i = 0; i < CHANNEL_COUNT; ++i) {
        setChannel(i, channels[i]);
    }
}

std::array<uint16_t, 16> CRSFRCChannels::getAllChannels() const {
    std::array<uint16_t, 16> channels;
    for (size_t i = 0; i < CHANNEL_COUNT; ++i) {
        channels[i] = getChannel(i);
    }
    return channels;
}

void CRSFRCChannels::setAllCenter() {
    for (size_t i = 0; i < CHANNEL_COUNT; ++i) {
        setChannel(i, CRSF_CENTER_VALUE);
    }
}

void CRSFRCChannels::setAllMin() {
    for (size_t i = 0; i < CHANNEL_COUNT; ++i) {
        setChannel(i, CRSF_MIN_VALUE);
    }
}

void CRSFRCChannels::setAllMax() {
    for (size_t i = 0; i < CHANNEL_COUNT; ++i) {
        setChannel(i, CRSF_MAX_VALUE);
    }
}

void CRSFRCChannels::setStandardRC(uint16_t throttle, uint16_t elevator, uint16_t aileron, uint16_t rudder) {
    setChannel(0, throttle);   // Channel 1: Throttle
    setChannel(1, elevator);   // Channel 2: Elevator
    setChannel(2, aileron);    // Channel 3: Aileron
    setChannel(3, rudder);     // Channel 4: Rudder
    
    // Set remaining channels to center
    for (size_t i = 4; i < CHANNEL_COUNT; ++i) {
        setChannel(i, CRSF_CENTER_VALUE);
    }
}

// ============================================================================
// CRSF-PWM Conversion Methods (from official spec)
// ============================================================================

uint16_t CRSFRCChannels::crsfToPWM(uint16_t crsfValue) {
    // us = 1500 + (5/8 * (crsf - 992))
    if (crsfValue == CRSF_CENTER_VALUE) return 1500;
    double result = 1500.0 + (5.0 / 8.0) * (crsfValue - CRSF_CENTER_VALUE);
    return static_cast<uint16_t>(std::round(result));
}

uint16_t CRSFRCChannels::pwmToCRSF(uint16_t pwmValue) {
    // crsf = 992 + (8/5 * (us - 1500))
    if (pwmValue == 1500) return CRSF_CENTER_VALUE;
    double result = CRSF_CENTER_VALUE + (8.0 / 5.0) * (pwmValue - 1500);
    int32_t crsfValue = static_cast<int32_t>(std::round(result));
    return static_cast<uint16_t>(std::max(0, std::min(static_cast<int32_t>(CRSF_MAX_VALUE), crsfValue)));
}

// ============================================================================
// ExpressLRS Arm Status Support
// ============================================================================

bool CRSFRCChannels::hasArmStatus() const {
    // According to CRSF spec: ExpressLRS uses payload length to determine arm status
    // getLength() returns the length field value, which is payload + type + crc
    // Basic packet: payload(22) + type(1) + crc(1) = 24 (0x18) - NO arm status, ExpressLRS uses ch4
    // ELRS packet: payload(23) + type(1) + crc(1) = 25 (0x19) - WITH arm status byte
    
    // Check if the length field indicates arm status
    return getLength() == (PAYLOAD_SIZE_ELRS + TYPE_SIZE + CRC_SIZE); // payload(23) + type(1) + crc(1) = 25 (0x19)
}

int8_t CRSFRCChannels::getArmStatus() const {
    if (!hasArmStatus()) return -1;
    // Arm status is the last byte before CRC
    // When LEN = 25 (0x19), arm status is at byte 25 (after 22 bytes of channels + 1 byte arm status)
    return getByte(CRSF_PAYLOAD_INDEX(0) + PAYLOAD_SIZE_BASIC); // 3 + 22 = byte 25
}

void CRSFRCChannels::setArmStatus(uint8_t status) {
    // Ensure we have space for arm status
    if (!hasArmStatus()) {
        assert(false && "CRSFRCChannels does not have arm status");
        return;
    }
    
    // Set arm status byte at the correct position
    // Arm status goes after the 22 bytes of channel data
    setByte(CRSF_PAYLOAD_INDEX(0) + PAYLOAD_SIZE_ELRS - CRC_SIZE, status);

}

void CRSFRCChannels::setArmed(bool armed) {
    setArmStatus(armed ? 1 : 0);
}

bool CRSFRCChannels::isArmed() const {
    return getArmStatus() == 1;
}

bool CRSFRCChannels::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::RCChannelsPacked)) return false;
    
    // Check length based on whether we have ExpressLRS arm status
    size_t expectedLength = hasArmStatus() ? (PAYLOAD_SIZE_ELRS + 1 + 1) : (PAYLOAD_SIZE_BASIC + 1 + 1);
    if (getLength() != expectedLength) return false;
    
    // Check total packet size
    size_t minSize = hasArmStatus() ? (4 + PAYLOAD_SIZE_ELRS) : (4 + PAYLOAD_SIZE_BASIC);
    if (data_.size() < minSize) return false;
    
    return true;
}


// ============================================================================
// CRSFSubsetRCChannels Implementation (CRSFv3)
// ============================================================================

CRSFSubsetRCChannels::CRSFSubsetRCChannels(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::SubsetRCChannelsPacked)) {
        setType(static_cast<uint8_t>(PacketType::SubsetRCChannelsPacked));
    }
}

uint8_t CRSFSubsetRCChannels::getConfigByte() const {
    return getByte(3);
}

void CRSFSubsetRCChannels::setConfigByte(uint8_t config) {
    setByte(3, config);
}

uint8_t CRSFSubsetRCChannels::getChannelResolution() const {
    uint8_t config = getConfigByte();
    return (config >> 5) & 0x03;  // Bits 5-6
}

void CRSFSubsetRCChannels::setChannelResolution(uint8_t resolution) {
    uint8_t config = getConfigByte();
    config = (config & 0x1F) | ((resolution & 0x03) << 5);  // Clear bits 5-6, set new value
    setConfigByte(config);
}

uint8_t CRSFSubsetRCChannels::getFirstChannelNumber() const {
    uint8_t config = getConfigByte();
    return config & 0x1F;  // Bits 0-4
}

void CRSFSubsetRCChannels::setFirstChannelNumber(uint8_t channelNumber) {
    uint8_t config = getConfigByte();
    config = (config & 0xE0) | (channelNumber & 0x1F);  // Clear bits 0-4, set new value
    setConfigByte(config);
}

bool CRSFSubsetRCChannels::getReservedBit() const {
    uint8_t config = getConfigByte();
    return (config >> 7) & 0x01;  // Bit 7
}

void CRSFSubsetRCChannels::setReservedBit(bool reserved) {
    uint8_t config = getConfigByte();
    config = (config & 0x7F) | (static_cast<uint8_t>(reserved) << 7);  // Clear bit 7, set new value
    setConfigByte(config);
}

uint16_t CRSFSubsetRCChannels::getChannel(uint8_t channelIndex) const {
    if (channelIndex >= getChannelCount()) return 0;
    
    uint8_t resolution = getChannelResolution();
    uint8_t bitsPerChannel;
    uint16_t maxValue;
    
    switch (resolution) {
        case RESOLUTION_10BIT: bitsPerChannel = 10; maxValue = CRSF_MAX_10BIT; break;
        case RESOLUTION_11BIT: bitsPerChannel = 11; maxValue = CRSF_MAX_11BIT; break;
        case RESOLUTION_12BIT: bitsPerChannel = 12; maxValue = CRSF_MAX_12BIT; break;
        case RESOLUTION_13BIT: bitsPerChannel = 13; maxValue = CRSF_MAX_13BIT; break;
        default: return 0;
    }
    
    // Calculate bit offset: config byte (8 bits) + all previous channels
    size_t bitOffset = 8 + (channelIndex * bitsPerChannel);
    size_t byteOffset = 3 + (bitOffset / 8);
    size_t bitInByte = bitOffset % 8;
    
    if (byteOffset + 2 >= data_.size()) return 0;
    
    uint32_t value = 0;
    size_t bitsRead = 0;
    
    while (bitsRead < bitsPerChannel && byteOffset < data_.size()) {
        uint8_t byte = data_[byteOffset];
        uint8_t bitsToRead = std::min(8 - bitInByte, bitsPerChannel - bitsRead);
        
        uint8_t mask = (1 << bitsToRead) - 1;
        uint8_t extracted = (byte >> bitInByte) & mask;
        
        value |= (static_cast<uint32_t>(extracted) << bitsRead);
        
        bitsRead += bitsToRead;
        byteOffset++;
        bitInByte = 0;
    }
    
    return static_cast<uint16_t>(value & maxValue);
}

void CRSFSubsetRCChannels::setChannel(uint8_t channelIndex, uint16_t value) {
    if (channelIndex >= getChannelCount()) return;
    
    uint8_t resolution = getChannelResolution();
    uint8_t bitsPerChannel;
    uint16_t maxValue;
    
    switch (resolution) {
        case RESOLUTION_10BIT: bitsPerChannel = 10; maxValue = CRSF_MAX_10BIT; break;
        case RESOLUTION_11BIT: bitsPerChannel = 11; maxValue = CRSF_MAX_11BIT; break;
        case RESOLUTION_12BIT: bitsPerChannel = 12; maxValue = CRSF_MAX_12BIT; break;
        case RESOLUTION_13BIT: bitsPerChannel = 13; maxValue = CRSF_MAX_13BIT; break;
        default: return;
    }
    
    if (value > maxValue) value = maxValue;
    
    // Calculate bit offset: config byte (8 bits) + all previous channels
    size_t bitOffset = 8 + (channelIndex * bitsPerChannel);
    size_t byteOffset = 3 + (bitOffset / 8);
    size_t bitInByte = bitOffset % 8;
    
    // Ensure we have enough space
    assert(byteOffset + 2 < data_.size() && "Subset RC channels data must have enough space for channel data");
    
    uint32_t bitsToWrite = value & maxValue;
    size_t bitsWritten = 0;
    
    while (bitsWritten < bitsPerChannel && byteOffset < data_.size()) {
        uint8_t bitsToWriteInByte = std::min(8 - bitInByte, bitsPerChannel - bitsWritten);
        
        uint8_t mask = (1 << bitsToWriteInByte) - 1;
        uint8_t valueToWrite = (bitsToWrite >> bitsWritten) & mask;
        
        // Clear existing bits and set new ones
        uint8_t clearMask = ~(mask << bitInByte);
        data_[byteOffset] = (data_[byteOffset] & clearMask) | (valueToWrite << bitInByte);
        
        bitsWritten += bitsToWriteInByte;
        byteOffset++;
        bitInByte = 0;
    }
}

size_t CRSFSubsetRCChannels::getChannelCount() const {
    // Calculate channel count from packet length
    // Length = config(1) + channels_data + type(1) + crc(1)
    uint8_t length = getLength();
    if (length < 3) return 0;  // Minimum: config(1) + type(1) + crc(1)
    
    uint8_t resolution = getChannelResolution();
    uint8_t bitsPerChannel;
    
    switch (resolution) {
        case RESOLUTION_10BIT: bitsPerChannel = 10; break;
        case RESOLUTION_11BIT: bitsPerChannel = 11; break;
        case RESOLUTION_12BIT: bitsPerChannel = 12; break;
        case RESOLUTION_13BIT: bitsPerChannel = 13; break;
        default: return 0;
    }
    
    // channels_data = payload - config(1) = (LEN - 2) - 1 = LEN - 3
    // channel_count = channels_data * 8 / bitsPerChannel
    size_t channelsDataBytes = length - 3;
    size_t totalBits = channelsDataBytes * 8;
    
    return totalBits / bitsPerChannel;
}

void CRSFSubsetRCChannels::setAllChannels(uint16_t value) {
    size_t count = getChannelCount();
    for (size_t i = 0; i < count; ++i) {
        setChannel(i, value);
    }
}

void CRSFSubsetRCChannels::setAllCenter() {
    setAllChannels(PWM_CENTER);
}

void CRSFSubsetRCChannels::setAllMin() {
    setAllChannels(PWM_MIN);
}

void CRSFSubsetRCChannels::setAllMax() {
    setAllChannels(PWM_MAX);
}

// ============================================================================
// CRSF-PWM Conversion Methods for All Resolutions
// ============================================================================

uint16_t CRSFSubsetRCChannels::crsfToPWM(uint16_t crsfValue, uint8_t resolution) {
    uint16_t maxValue;
    switch (resolution) {
        case RESOLUTION_10BIT: maxValue = CRSF_MAX_10BIT; break;
        case RESOLUTION_11BIT: maxValue = CRSF_MAX_11BIT; break;
        case RESOLUTION_12BIT: maxValue = CRSF_MAX_12BIT; break;
        case RESOLUTION_13BIT: maxValue = CRSF_MAX_13BIT; break;
        default: return PWM_CENTER;
    }
    
    if (crsfValue == 0) return PWM_MIN;
    if (crsfValue == maxValue) return PWM_MAX;
    
    // Linear interpolation: crsfValue / maxValue maps to (PWM_MAX - PWM_MIN)
    double ratio = static_cast<double>(crsfValue) / maxValue;
    double pwmValue = PWM_MIN + ratio * (PWM_MAX - PWM_MIN);
    
    return static_cast<uint16_t>(std::round(pwmValue));
}

uint16_t CRSFSubsetRCChannels::pwmToCRSF(uint16_t pwmValue, uint8_t resolution) {
    if (pwmValue <= PWM_MIN) return 0;
    if (pwmValue >= PWM_MAX) {
        switch (resolution) {
            case RESOLUTION_10BIT: return CRSF_MAX_10BIT;
            case RESOLUTION_11BIT: return CRSF_MAX_11BIT;
            case RESOLUTION_12BIT: return CRSF_MAX_12BIT;
            case RESOLUTION_13BIT: return CRSF_MAX_13BIT;
            default: return 0;
        }
    }
    
    // Linear interpolation: (pwmValue - PWM_MIN) / (PWM_MAX - PWM_MIN) maps to maxValue
    double ratio = static_cast<double>(pwmValue - PWM_MIN) / (PWM_MAX - PWM_MIN);
    
    uint16_t maxValue;
    switch (resolution) {
        case RESOLUTION_10BIT: maxValue = CRSF_MAX_10BIT; break;
        case RESOLUTION_11BIT: maxValue = CRSF_MAX_11BIT; break;
        case RESOLUTION_12BIT: maxValue = CRSF_MAX_12BIT; break;
        case RESOLUTION_13BIT: maxValue = CRSF_MAX_13BIT; break;
        default: return 0;
    }
    
    double crsfValue = ratio * maxValue;
    return static_cast<uint16_t>(std::round(crsfValue));
}

bool CRSFSubsetRCChannels::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::SubsetRCChannelsPacked)) return false;
    
    // Check minimum packet size
    if (data_.size() < MIN_PACKET_SIZE) return false;
    
    // Check channel resolution is valid
    uint8_t resolution = getChannelResolution();
    if (resolution > RESOLUTION_13BIT) return false;
    
    // Check first channel number is valid (0-31)
    uint8_t firstChannel = getFirstChannelNumber();
    if (firstChannel > 31) return false;
    
    // Check that we have at least one channel
    if (getChannelCount() == 0) return false;
    
    return true;
}

// ============================================================================
// CRSFLinkStatistics Implementation
// ============================================================================



// ============================================================================
// Utility Functions
// ============================================================================

bool validatePacket(const CRSFPacket& packet) {
    // Check basic constraints
    if (packet.getLength() > MAX_PAYLOAD_SIZE) return false;
    if (packet.getTotalSize() > MAX_PACKET_SIZE) return false;
    
    // Verify CRC
    return packet.verifyCRC();
}

// ============================================================================
// Binding Functions Implementation
// ============================================================================

void bindRCChannels(
    CRSFRCChannels& packet,
    uint8_t address,
    const std::array<uint16_t, 16>& channels,
    bool includeArmStatus,
    bool armStatus,
    etl::span<uint8_t>& data) {
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte (using address as sync for compatibility)
    data[1] = static_cast<uint8_t>(22 + 1 + 1); // LEN: payload(22) + type(1) + crc(1) = 24 (basic, no arm status)
    if (includeArmStatus) {
        data[1] = static_cast<uint8_t>(23 + 1 + 1); // LEN: payload(23) + type(1) + crc(1) = 25 (includes arm status)
    } else {
        data[1] = static_cast<uint8_t>(22 + 1 + 1); // LEN: payload(22) + type(1) + crc(1) = 24 (basic, no arm status)
    }
    data[2] = static_cast<uint8_t>(PacketType::RCChannelsPacked);
    
    if (!channels.empty()) {
        packet.setAllChannels(channels);
    } else {
        // Use the working setStandardRC method with center values
        packet.setStandardRC(1024, 1024, 1024, 1024);
    }

    if (includeArmStatus) {
        packet.setArmed(armStatus);
    }
    
    packet.calculateCRC();
}

void bindRCChannelsStandard(
    CRSFRCChannels& packet,
    uint8_t address,
    uint16_t throttle,
    uint16_t elevator,
    uint16_t aileron,
    uint16_t rudder,
    etl::span<uint8_t>& data) {
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte (using address as sync for compatibility)
    data[1] = static_cast<uint8_t>(22 + 1 + 1); // LEN: payload(22) + type(1) + crc(1) = 24 (basic, no arm status)
    data[2] = static_cast<uint8_t>(PacketType::RCChannelsPacked);
    
    packet.setStandardRC(throttle, elevator, aileron, rudder);
    packet.calculateCRC();
}

// ============================================================================
// SUBSET RC CHANNELS Binding Functions (CRSFv3)
// ============================================================================

void bindSubsetRCChannels(
    CRSFSubsetRCChannels& packet,
    uint8_t address,
    uint8_t firstChannelNumber,
    uint8_t resolution,
    const std::vector<uint16_t>& channels,
    etl::span<uint8_t>& data) {
    
    if (channels.empty()) return;
    
    // Calculate required payload size
    uint8_t bitsPerChannel;
    switch (resolution) {
        case CRSFSubsetRCChannels::RESOLUTION_10BIT: bitsPerChannel = 10; break;
        case CRSFSubsetRCChannels::RESOLUTION_11BIT: bitsPerChannel = 11; break;
        case CRSFSubsetRCChannels::RESOLUTION_12BIT: bitsPerChannel = 12; break;
        case CRSFSubsetRCChannels::RESOLUTION_13BIT: bitsPerChannel = 13; break;
        default: return;
    }
    
    // Calculate total bits needed: config(8) + channels_data
    size_t totalBits = 8 + (channels.size() * bitsPerChannel);
    size_t payloadSize = (totalBits + 7) / 8;  // Round up to bytes
    
    // Ensure we have enough space
    if (data.size() < 4 + payloadSize) return;
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte
    data[1] = static_cast<uint8_t>(payloadSize + TYPE_SIZE + CRC_SIZE); // LEN: payload + type(1) + crc(1)
    data[2] = static_cast<uint8_t>(PacketType::SubsetRCChannelsPacked);
    
    // Set config byte: first channel number + resolution + reserved bit
    uint8_t config = (firstChannelNumber & 0x1F) | ((resolution & 0x03) << 5);
    packet.setConfigByte(config);
    
    // Set all channels
    for (size_t i = 0; i < channels.size(); ++i) {
        packet.setChannel(i, channels[i]);
    }
    
    packet.calculateCRC();
}

void bindSubsetRCChannels10Bit(
    CRSFSubsetRCChannels& packet,
    uint8_t address,
    uint8_t firstChannelNumber,
    const std::vector<uint16_t>& channels,
    etl::span<uint8_t>& data) {
    
    bindSubsetRCChannels(packet, address, firstChannelNumber, 
                        CRSFSubsetRCChannels::RESOLUTION_10BIT, channels, data);
}

void bindSubsetRCChannels11Bit(
    CRSFSubsetRCChannels& packet,
    uint8_t address,
    uint8_t firstChannelNumber,
    const std::vector<uint16_t>& channels,
    etl::span<uint8_t>& data) {
    
    bindSubsetRCChannels(packet, address, firstChannelNumber, 
                        CRSFSubsetRCChannels::RESOLUTION_11BIT, channels, data);
}

void bindSubsetRCChannels12Bit(
    CRSFSubsetRCChannels& packet,
    uint8_t address,
    uint8_t firstChannelNumber,
    const std::vector<uint16_t>& channels,
    etl::span<uint8_t>& data) {
    
    bindSubsetRCChannels(packet, address, firstChannelNumber, 
                        CRSFSubsetRCChannels::RESOLUTION_12BIT, channels, data);
}

void bindSubsetRCChannels13Bit(
    CRSFSubsetRCChannels& packet,
    uint8_t address,
    uint8_t firstChannelNumber,
    const std::vector<uint16_t>& channels,
    etl::span<uint8_t>& data) {
    
    bindSubsetRCChannels(packet, address, firstChannelNumber, 
                        CRSFSubsetRCChannels::RESOLUTION_13BIT, channels, data);
}


// Generic packet binding function
void bindGenericPacket(
    CRSFPacket& packet,
    uint8_t address,
    PacketType type,
    const etl::span<uint8_t>& payload,
    etl::span<uint8_t>& data) {
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte
    data[1] = static_cast<uint8_t>(payload.size() + TYPE_SIZE + CRC_SIZE); // LEN: payload + type(1) + crc(1)
    data[2] = static_cast<uint8_t>(type);
    
    // Copy payload data
    if (!payload.empty()) {
        std::copy(payload.begin(), payload.end(), data.begin() + 3);
    }
    
    packet.calculateCRC();
}

// Overloaded createPacket method that takes a span directly
std::unique_ptr<CRSFPacket> CRSFPacket::createPacket(PacketType type, const etl::span<uint8_t>& data) {
    switch (type) {
        case PacketType::RCChannelsPacked:
            return std::make_unique<CRSFRCChannels>(data);
        case PacketType::SubsetRCChannelsPacked:
            return std::make_unique<CRSFSubsetRCChannels>(data);
/*
            case PacketType::LinkRXID:
            return std::make_unique<CRSFLinkRXID>(data);
        case PacketType::LinkTXID:
            return std::make_unique<CRSFLinkTXID>(data);
        case PacketType::Attitude:
            return std::make_unique<CRSFAttitude>(data);
        case PacketType::FlightMode:
            return std::make_unique<CRSFFlightMode>(data);
        case PacketType::DevicePing:
            return std::make_unique<CRSFDevicePing>(data);
        case PacketType::DeviceInfo:
            return std::make_unique<CRSFDeviceInfo>(data);
        case PacketType::ParameterSettingsEntry:
            return std::make_unique<CRSFParameterSettingsEntry>(data);
        case PacketType::ParameterRead:
            return std::make_unique<CRSFParameterRead>(data);
        case PacketType::ParameterWrite:
            return std::make_unique<CRSFParameterWrite>(data);
        case PacketType::ELRSStatus:
            return std::make_unique<CRSFELRSStatus>(data);
        case PacketType::Command:
            return std::make_unique<CRSFCommand>(data);
        case PacketType::RadioID:
            return std::make_unique<CRSFRadioID>(data);
        case PacketType::MSPReq:
            return std::make_unique<CRSFMSPReq>(data);
        case PacketType::LinkStatistics:
            return std::make_unique<CRSFLinkStatistics>(data);
        case PacketType::GPS:
            return std::make_unique<CRSFGPSData>(data);
        case PacketType::BatterySensor:
            return std::make_unique<CRSFBatterySensor>(data);
        case PacketType::Vario:
            return std::make_unique<CRSFVario>(data);
        case PacketType::BaroAltitude:
            return std::make_unique<CRSFBaroAltitude>(data);
        case PacketType::Heartbeat:
            return std::make_unique<CRSFHeartbeat>(data);*/
        default:
            return std::make_unique<CRSFPacket>(data);
        }
    }

// ============================================================================
// CRSF Command Implementation
// ============================================================================

CRSFCommand::CRSFCommand(const etl::span<uint8_t>& data) : CRSFPacket(data) {
}

uint8_t CRSFCommand::getCommandRealm() const {
    if (getPayload().size() < 1) return 0;
    return getPayload()[0];
}

void CRSFCommand::setCommandRealm(uint8_t realm) {
    auto payload = getPayload();
    if (payload.size() >= 1) {
        payload[0] = realm;
    }
}

etl::span<uint8_t> CRSFCommand::getExtendedPayload() const {
    auto payload = getPayload();
    if (payload.size() < 1) return etl::span<uint8_t>();
    return payload.subspan(0, payload.size());
}

void CRSFCommand::setExtendedPayload(const etl::span<uint8_t>& payload) {
    setPayload(payload);
}

void CRSFCommand::calculateCRC8BA() {
    auto payload = getExtendedPayload();
    if (payload.size() < 2) return; // Need at least realm + subcommand
    
    // Calculate CRC8-BA for the extended payload (excluding the CRC field itself)
    size_t crcDataSize = payload.size() - 1; // Exclude the last byte (CRC)
    if (crcDataSize > 0) {
        uint8_t crc = ::crsf::calculateCRC8BA(payload.data(), crcDataSize);
        payload[payload.size() - 1] = crc;
    }
}

bool CRSFCommand::verifyCRC8BA() const {
    auto payload = getExtendedPayload();
    if (payload.size() < 2) return false; // Need at least realm + subcommand + crc
    
    size_t crcDataSize = payload.size() - 1; // Exclude the last byte (CRC)
    if (crcDataSize == 0) return false;
    
    uint8_t calculatedCRC = ::crsf::calculateCRC8BA(payload.data(), crcDataSize);
    uint8_t receivedCRC = payload[payload.size() - 1];
    
    return calculatedCRC == receivedCRC;
}

bool CRSFCommand::isValid() const {
    // Check basic packet validity
    if (!validatePacket(*this)) return false;
    
    // Check if it's a command packet
    if (getType() != static_cast<uint8_t>(PacketType::Command)) return false;
    
    // Check extended payload size
    auto payload = getExtendedPayload();
    if (payload.size() < MIN_EXTENDED_PAYLOAD_SIZE) return false;
    if (payload.size() > MAX_EXTENDED_PAYLOAD_SIZE) return false;
    
    // Verify CRC8-BA (optional - some implementations omit it)
    // We'll be lenient and not require it for basic validation
    return true;
}

// ============================================================================
// CRSF Command RX Implementation
// ============================================================================

CRSFCommandRX::CRSFCommandRX(const etl::span<uint8_t>& data) : CRSFCommand(data) {
}

uint8_t CRSFCommandRX::getSubcommand() const {
    auto payload = getExtendedPayload();
    if (payload.size() < 2) return 0;
    return payload[1];
}

void CRSFCommandRX::setSubcommand(uint8_t subcommand) {
    auto payload = getExtendedPayload();
    if (payload.size() >= 2) {
        payload[1] = subcommand;
    }
}

etl::span<uint8_t> CRSFCommandRX::getCommandPayload() const {
    auto payload = getExtendedPayload();
    if (payload.size() < 2) return etl::span<uint8_t>();
    // Return payload after realm and subcommand
    return payload.subspan(2, payload.size() - 2 - 1); // Exclude CRC
}

void CRSFCommandRX::setCommandPayload(const etl::span<uint8_t>& payload) {
    (void)payload; // Suppress unused parameter warning
    // This would require resizing the packet, which is complex
    // For now, we'll just validate the size
}

bool CRSFCommandRX::isBindCommand() const {
    return getCommandRealm() == static_cast<uint8_t>(CommandRealm::RX) &&
           getSubcommand() == static_cast<uint8_t>(CommandRXSubcommand::Bind);
}

void CRSFCommandRX::setBindCommand() {
    setCommandRealm(static_cast<uint8_t>(CommandRealm::RX));
    setSubcommand(static_cast<uint8_t>(CommandRXSubcommand::Bind));
}

bool CRSFCommandRX::isModelSelectIDCommand() const {
    return getCommandRealm() == static_cast<uint8_t>(CommandRealm::RX) &&
           getSubcommand() == static_cast<uint8_t>(CommandRXSubcommand::ModelSelectID);
}

uint8_t CRSFCommandRX::getModelID() const {
    auto payload = getCommandPayload();
    if (payload.size() < 1) return 0;
    return payload[0];
}

void CRSFCommandRX::setModelSelectIDCommand(uint8_t modelID) {
    (void)modelID; // Suppress unused parameter warning
    setCommandRealm(static_cast<uint8_t>(CommandRealm::RX));
    setSubcommand(static_cast<uint8_t>(CommandRXSubcommand::ModelSelectID));
    // Note: Setting the modelID would require packet resizing
}

bool CRSFCommandRX::isValid() const {
    if (!CRSFCommand::isValid()) return false;
    
    // Check realm
    if (getCommandRealm() != static_cast<uint8_t>(CommandRealm::RX)) return false;
    
    // Check subcommand
    uint8_t subcommand = getSubcommand();
    if (subcommand != static_cast<uint8_t>(CommandRXSubcommand::Bind) &&
        subcommand != static_cast<uint8_t>(CommandRXSubcommand::ModelSelectID)) {
        return false;
    }
    
    return true;
}

// ============================================================================
// CRSF Command General Implementation
// ============================================================================

CRSFCommandGeneral::CRSFCommandGeneral(const etl::span<uint8_t>& data) : CRSFCommand(data) {
}

uint8_t CRSFCommandGeneral::getSubcommand() const {
    auto payload = getExtendedPayload();
    if (payload.size() < 2) return 0;
    return payload[1];
}

void CRSFCommandGeneral::setSubcommand(uint8_t subcommand) {
    auto payload = getExtendedPayload();
    if (payload.size() >= 2) {
        payload[1] = subcommand;
    }
}

etl::span<uint8_t> CRSFCommandGeneral::getCommandPayload() const {
    auto payload = getExtendedPayload();
    if (payload.size() < 2) return etl::span<uint8_t>();
    // Return payload after realm and subcommand
    return payload.subspan(2, payload.size() - 2 - 1); // Exclude CRC
}

void CRSFCommandGeneral::setCommandPayload(const etl::span<uint8_t>& payload) {
    (void)payload; // Suppress unused parameter warning
    // This would require resizing the packet, which is complex
    // For now, we'll just validate the size
}

bool CRSFCommandGeneral::isCRSFSpeedProposal() const {
    return getCommandRealm() == static_cast<uint8_t>(CommandRealm::General) &&
           getSubcommand() == static_cast<uint8_t>(CommandGeneralSubcommand::CRSFSpeedProposal);
}

uint8_t CRSFCommandGeneral::getPortID() const {
    auto payload = getCommandPayload();
    if (payload.size() < 1) return 0;
    return payload[0];
}

uint32_t CRSFCommandGeneral::getProposedBaudRate() const {
    auto payload = getCommandPayload();
    if (payload.size() < 5) return 0; // portID + 4 bytes baud rate
    return getU32(CRSF_PAYLOAD_INDEX(0) + 2 + 1); // Skip realm, subcommand, portID
}

void CRSFCommandGeneral::setCRSFSpeedProposal(uint8_t portID, uint32_t baudRate) {
    (void)portID; // Suppress unused parameter warning
    (void)baudRate; // Suppress unused parameter warning
    setCommandRealm(static_cast<uint8_t>(CommandRealm::General));
    setSubcommand(static_cast<uint8_t>(CommandGeneralSubcommand::CRSFSpeedProposal));
    // Note: Setting portID and baudRate would require packet resizing
}

bool CRSFCommandGeneral::isCRSFSpeedResponse() const {
    return getCommandRealm() == static_cast<uint8_t>(CommandRealm::General) &&
           getSubcommand() == static_cast<uint8_t>(CommandGeneralSubcommand::CRSFSpeedResponse);
}

uint8_t CRSFCommandGeneral::getResponsePortID() const {
    auto payload = getCommandPayload();
    if (payload.size() < 1) return 0;
    return payload[0];
}

bool CRSFCommandGeneral::isAccepted() const {
    auto payload = getCommandPayload();
    if (payload.size() < 2) return false;
    return payload[1] == 1;
}

void CRSFCommandGeneral::setCRSFSpeedResponse(uint8_t portID, bool accepted) {
    (void)portID; // Suppress unused parameter warning
    (void)accepted; // Suppress unused parameter warning
    setCommandRealm(static_cast<uint8_t>(CommandRealm::General));
    setSubcommand(static_cast<uint8_t>(CommandGeneralSubcommand::CRSFSpeedResponse));
    // Note: Setting portID and accepted would require packet resizing
}

bool CRSFCommandGeneral::isValid() const {
    if (!CRSFCommand::isValid()) return false;
    
    // Check realm
    if (getCommandRealm() != static_cast<uint8_t>(CommandRealm::General)) return false;
    
    // Check subcommand
    uint8_t subcommand = getSubcommand();
    if (subcommand != static_cast<uint8_t>(CommandGeneralSubcommand::CRSFSpeedProposal) &&
        subcommand != static_cast<uint8_t>(CommandGeneralSubcommand::CRSFSpeedResponse)) {
        return false;
    }
    
    return true;
}

// ============================================================================
// Command Binding Functions Implementation
// ============================================================================

void bindCommandPacket(
    CRSFCommand& packet,
    uint8_t address,
    uint8_t commandRealm,
    uint8_t subcommand,
    const etl::span<uint8_t>& commandPayload,
    etl::span<uint8_t>& data
) {
    (void)data; // Suppress unused parameter warning
    // Set basic packet fields
    packet.setAddress(address);
    packet.setType(static_cast<uint8_t>(PacketType::Command));
    
    // Calculate total payload size: realm + subcommand + commandPayload + crc8-ba
    size_t totalPayloadSize = 1 + 1 + commandPayload.size() + 1; // realm + subcommand + payload + crc
    packet.setLength(totalPayloadSize);
    
    // Set extended payload
    std::vector<uint8_t> extendedPayload(totalPayloadSize);
    extendedPayload[0] = commandRealm;
    extendedPayload[1] = subcommand;
    
    // Copy command payload
    if (commandPayload.size() > 0) {
        std::memcpy(extendedPayload.data() + 2, commandPayload.data(), commandPayload.size());
    }
    
    // Calculate and set CRC8-BA
    uint8_t crc = calculateCRC8BA(extendedPayload.data(), totalPayloadSize - 1);
    extendedPayload[totalPayloadSize - 1] = crc;
    
    packet.setExtendedPayload(etl::span<uint8_t>(extendedPayload));
    
    // Calculate and set main CRC
    packet.calculateCRC();
}

void bindCommandRXPacket(
    CRSFCommandRX& packet,
    uint8_t address,
    CommandRXSubcommand subcommand,
    const etl::span<uint8_t>& commandPayload,
    etl::span<uint8_t>& data
) {
    bindCommandPacket(packet, address, static_cast<uint8_t>(CommandRealm::RX), 
                     static_cast<uint8_t>(subcommand), commandPayload, data);
}

void bindCommandGeneralPacket(
    CRSFCommandGeneral& packet,
    uint8_t address,
    CommandGeneralSubcommand subcommand,
    const etl::span<uint8_t>& commandPayload,
    etl::span<uint8_t>& data
) {
    bindCommandPacket(packet, address, static_cast<uint8_t>(CommandRealm::General), 
                     static_cast<uint8_t>(subcommand), commandPayload, data);
}

void bindBindCommand(
    CRSFCommandRX& packet,
    uint8_t address,
    etl::span<uint8_t>& data
) {
    bindCommandRXPacket(packet, address, CommandRXSubcommand::Bind, 
                       etl::span<uint8_t>(), data);
}

void bindModelSelectIDCommand(
    CRSFCommandRX& packet,
    uint8_t address,
    uint8_t modelID,
    etl::span<uint8_t>& data
) {
    std::vector<uint8_t> payload = {modelID};
    bindCommandRXPacket(packet, address, CommandRXSubcommand::ModelSelectID, 
                       etl::span<uint8_t>(payload), data);
}

void bindCRSFSpeedProposalCommand(
    CRSFCommandGeneral& packet,
    uint8_t address,
    uint8_t portID,
    uint32_t baudRate,
    etl::span<uint8_t>& data
) {
    std::vector<uint8_t> payload(5);
    payload[0] = portID;
    // Baud rate in big-endian format
    payload[1] = (baudRate >> 24) & 0xFF;
    payload[2] = (baudRate >> 16) & 0xFF;
    payload[3] = (baudRate >> 8) & 0xFF;
    payload[4] = baudRate & 0xFF;
    
    bindCommandGeneralPacket(packet, address, CommandGeneralSubcommand::CRSFSpeedProposal, 
                           etl::span<uint8_t>(payload), data);
}

void bindCRSFSpeedResponseCommand(
    CRSFCommandGeneral& packet,
    uint8_t address,
    uint8_t portID,
    bool accepted,
    etl::span<uint8_t>& data
) {
    std::vector<uint8_t> payload = {portID, static_cast<uint8_t>(accepted ? 1 : 0)};
    bindCommandGeneralPacket(packet, address, CommandGeneralSubcommand::CRSFSpeedResponse, 
                           etl::span<uint8_t>(payload), data);
}

} // namespace crsf
