#include "msp.h"
#include <cstring>
#include <algorithm>
#include <iostream>

namespace msp {

// MSP CRC calculation (XOR of all bytes)
uint8_t calculateCRC8(const uint8_t* data, size_t len) {
    uint8_t crc = 0;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
    }
    return crc;
}

bool validatePacket(const MSPPacket& packet) {
    if (packet.getTotalSize() < MIN_PACKET_SIZE) {
        return false;
    }

    if (!packet.verifyCRC()) {
        return false;
    }

    switch (packet.getDirection()) {
        case PacketDirection::REQUEST:
            return packet.getPayloadSize() == 0;
        case PacketDirection::RESPONSE:
            switch (packet.getType()) {
                case PacketType::MSP_RAW_IMU:
                    return MSPRawIMU::PAYLOAD_SIZE == packet.getPayloadSize();
                case PacketType::MSP_STATUS:
                    return MSPStatus::PAYLOAD_SIZE == packet.getPayloadSize();
                case PacketType::MSP_MOTOR_TELEMETRY:
                    return MSPMotorTelemetry::PAYLOAD_SIZE == packet.getPayloadSize();
                case PacketType::MSP_ATTITUDE:
                    return MSPAttitude::PAYLOAD_SIZE == packet.getPayloadSize();
                case PacketType::MSP_ALTITUDE:
                    return MSPAltitude::PAYLOAD_SIZE == packet.getPayloadSize();
                case PacketType::MSP_PID:
                    return MSPPID::PAYLOAD_SIZE == packet.getPayloadSize();
                case PacketType::MSP_ACC_TRIM:
                    return MSPAccTrim::PAYLOAD_SIZE == packet.getPayloadSize();
                default:
                    return false;
            }
        default:
            return false;
    }
}

// ============================================================================
// MSPPacket Implementation
// ============================================================================

MSPPacket::MSPPacket() {
}

MSPPacket::MSPPacket(const etl::span<uint8_t>& data) {
    data_ = data;
}

// Bind a packet to a data span, validate the packet, return false if the packet is invalid.
bool MSPPacket::bind(const etl::span<uint8_t>& data) {
    data_ = data;
    return validatePacket(*this);
}

// Initialize a packet with a type, direction, and data, calculate the CRC, and validate the packet. return false if the packet is invalid. 
// (size of payload must be equal to the size of the payload for the type)
bool MSPPacket::init(PacketType type,PacketDirection direction,const etl::span<uint8_t>& data) {
    data_ = data;
    setType(type);
    setDirection(direction);
    setHeader();
    setPayloadSize(data.size() - MIN_PACKET_SIZE);  // data.size() is total packet size, payload = total - header/crc
    calculateCRC();
    return validatePacket(*this);
}

uint8_t MSPPacket::getByte(size_t offset) const {
    if (offset < data_.size()) {
        return data_[offset];
    }
    throw std::out_of_range("Byte access out of range");
}

void MSPPacket::setByte(size_t offset, uint8_t value) {
    if (offset < data_.size()) {
        data_[offset] = value;
    } else {
        std::cout << "Byte access out of range: offset " << offset << " size " << data_.size() << std::endl;
        throw std::out_of_range("Byte access out of range");
    }
}

uint16_t MSPPacket::getU16(size_t offset) const {
    if (offset + 1 < data_.size()) {
        return static_cast<uint16_t>(data_[offset]) | 
               (static_cast<uint16_t>(data_[offset + 1]) << 8);
    }
    throw std::out_of_range("U16 access out of range");
}

void MSPPacket::setU16(size_t offset, uint16_t value) {
    if (offset + 1 < data_.size()) {
        data_[offset] = static_cast<uint8_t>(value & 0xFF);
        data_[offset + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    } else {
        throw std::out_of_range("U16 access out of range");
    }
}

uint32_t MSPPacket::getU32(size_t offset) const {
    if (offset + 3 < data_.size()) {
        return static_cast<uint32_t>(data_[offset]) |
               (static_cast<uint32_t>(data_[offset + 1]) << 8) |
               (static_cast<uint32_t>(data_[offset + 2]) << 16) |
               (static_cast<uint32_t>(data_[offset + 3]) << 24);
    }
    throw std::out_of_range("U32 access out of range");
}

void MSPPacket::setU32(size_t offset, uint32_t value) {
    if (offset + 3 < data_.size()) {
        data_[offset] = static_cast<uint8_t>(value & 0xFF);
        data_[offset + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
        data_[offset + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
        data_[offset + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
    } else {
        throw std::out_of_range("U32 access out of range");
    }
}

int16_t MSPPacket::getI16(size_t offset) const {
    return static_cast<int16_t>(getU16(offset));
}

void MSPPacket::setI16(size_t offset, int16_t value) {
    setU16(offset, static_cast<uint16_t>(value));
}

PacketDirection MSPPacket::getDirection() const {
    return static_cast<PacketDirection>(getByte(2));
}

void MSPPacket::setHeader() {
    setByte(0, '$');
    setByte(1, 'M');
    setByte(2, '<');
}

void MSPPacket::setDirection(PacketDirection direction) {
    setByte(2, static_cast<uint8_t>(direction));
}

void MSPPacket::setType(PacketType type) {
    setByte(4, static_cast<uint8_t>(type));  // Fixed: set byte 4 (command), not byte 3 (size)
}

PacketType MSPPacket::getType() const {
    return static_cast<PacketType>(getByte(4));  // Fixed: read from byte 4 (command), not byte 3 (size)
}

uint8_t MSPPacket::getPayloadSize() const {
    return getByte(3);
}

void MSPPacket::setPayloadSize(uint8_t size) {
    setByte(3, size);
}

uint8_t MSPPacket::getCommand() const {
    return getByte(4);
}

void MSPPacket::setCommand(uint8_t command) {
    setByte(4, command);
}

uint8_t MSPPacket::getCRC() const {
    size_t payloadSize = getPayloadSize();
    return getByte(5 + payloadSize);
}

void MSPPacket::setCRC(uint8_t crc) {
    size_t payloadSize = getPayloadSize();
    setByte(5 + payloadSize, crc);
}

etl::span<uint8_t> MSPPacket::getPayload() const {
    size_t payloadSize = getPayloadSize();
    if (payloadSize > 0 && 5 + payloadSize <= data_.size()) {
        return etl::span<uint8_t>(data_.data() + 5, payloadSize);
    }
    return etl::span<uint8_t>();
}

void MSPPacket::setPayload(const etl::span<uint8_t>& payload) {
    if (payload.size() <= MAX_PAYLOAD_SIZE && 5 + payload.size() <= data_.size()) {
        setPayloadSize(static_cast<uint8_t>(payload.size()));
        std::copy(payload.begin(), payload.end(), data_.begin() + 5);
    }
}

void MSPPacket::setPayload(const uint8_t* data, size_t len) {
    if (len <= MAX_PAYLOAD_SIZE && 5 + len <= data_.size()) {
        setPayloadSize(static_cast<uint8_t>(len));
        std::copy(data, data + len, data_.begin() + 5);
    }
}

const etl::span<uint8_t>& MSPPacket::getRawData() const {
    return data_;
}

etl::span<uint8_t>& MSPPacket::getRawData() {
    return data_;
}

size_t MSPPacket::getTotalSize() const {
    return MSP_HEADER_SIZE + MSP_PAYLOAD_SIZE + MSP_CMD_SIZE + getPayloadSize() + MSP_CRC_SIZE;
}

void MSPPacket::calculateCRC() {
    size_t payloadSize = getPayloadSize();
    size_t crcStart = 3;  // Start from payload size
    size_t crcEnd = 5 + payloadSize;  // End before CRC
    
    uint8_t crc = calculateCRC8(data_.data() + crcStart, crcEnd - crcStart);
    setCRC(crc);
}

bool MSPPacket::verifyCRC() const {
    size_t payloadSize = getPayloadSize();
    size_t crcStart = 3;  // Start from payload size
    size_t crcEnd = 5 + payloadSize;  // End before CRC
    
    uint8_t expectedCRC = calculateCRC8(data_.data() + crcStart, crcEnd - crcStart);
    return getCRC() == expectedCRC;
}

std::unique_ptr<MSPPacket> MSPPacket::createPacket(PacketType type, const etl::span<uint8_t>& data) {
    switch (type) {
        case PacketType::MSP_RAW_IMU:
            return std::make_unique<MSPRawIMU>(data);
        case PacketType::MSP_STATUS:
            return std::make_unique<MSPStatus>(data);
        case PacketType::MSP_MOTOR_TELEMETRY:
            return std::make_unique<MSPMotorTelemetry>(data);
        case PacketType::MSP_ATTITUDE:
            return std::make_unique<MSPAttitude>(data);
        case PacketType::MSP_ALTITUDE:
            return std::make_unique<MSPAltitude>(data);
        case PacketType::MSP_PID:
            return std::make_unique<MSPPID>(data);
        case PacketType::MSP_ACC_TRIM:
            return std::make_unique<MSPAccTrim>(data);
        default:
            return std::make_unique<MSPPacket>(data);
    }
}

// ============================================================================
// MSPRequest Implementation
// ============================================================================

MSPRequest::MSPRequest(const etl::span<uint8_t>& data) : MSPPacket(data) {
    // Set direction to REQUEST by default
    setDirection(PacketDirection::REQUEST);
    setPayloadSize(PAYLOAD_SIZE);  // 0 for requests
}

void MSPRequest::setRequestCommand(PacketType command) {
    setCommand(static_cast<uint8_t>(command));
}

PacketType MSPRequest::getRequestCommand() const {
    return static_cast<PacketType>(getCommand());
}

bool MSPRequest::isValid() const {
    return getPayloadSize() == PAYLOAD_SIZE && 
           getDirection() == PacketDirection::REQUEST &&
           MSPPacket::verifyCRC();
}

// ============================================================================
// MSPRawIMU Implementation
// ============================================================================

MSPRawIMU::MSPRawIMU(const etl::span<uint8_t>& data) : MSPPacket(data) {
    setCommand(static_cast<uint8_t>(PacketType::MSP_RAW_IMU));
    setPayloadSize(PAYLOAD_SIZE);
}

int16_t MSPRawIMU::getAccX() const { return getI16(5); }
void MSPRawIMU::setAccX(int16_t value) { setI16(5, value); }

int16_t MSPRawIMU::getAccY() const { return getI16(7); }
void MSPRawIMU::setAccY(int16_t value) { setI16(7, value); }

int16_t MSPRawIMU::getAccZ() const { return getI16(9); }
void MSPRawIMU::setAccZ(int16_t value) { setI16(9, value); }

int16_t MSPRawIMU::getGyroX() const { return getI16(11); }
void MSPRawIMU::setGyroX(int16_t value) { setI16(11, value); }

int16_t MSPRawIMU::getGyroY() const { return getI16(13); }
void MSPRawIMU::setGyroY(int16_t value) { setI16(13, value); }

int16_t MSPRawIMU::getGyroZ() const { return getI16(15); }
void MSPRawIMU::setGyroZ(int16_t value) { setI16(15, value); }

int16_t MSPRawIMU::getMagX() const { return getI16(17); }
void MSPRawIMU::setMagX(int16_t value) { setI16(17, value); }

int16_t MSPRawIMU::getMagY() const { return getI16(19); }
void MSPRawIMU::setMagY(int16_t value) { setI16(19, value); }

int16_t MSPRawIMU::getMagZ() const { return getI16(21); }
void MSPRawIMU::setMagZ(int16_t value) { setI16(21, value); }

bool MSPRawIMU::isValid() const {
    return getPayloadSize() == PAYLOAD_SIZE && MSPPacket::verifyCRC();
}

// ============================================================================
// MSPStatus Implementation
// ============================================================================

MSPStatus::MSPStatus(const etl::span<uint8_t>& data) : MSPPacket(data) {
    setCommand(static_cast<uint8_t>(PacketType::MSP_STATUS));
    setPayloadSize(PAYLOAD_SIZE);
}

uint16_t MSPStatus::getCycleTime() const { return getU16(5); }
void MSPStatus::setCycleTime(uint16_t value) { setU16(5, value); }

uint16_t MSPStatus::getI2cErrorCount() const { return getU16(7); }
void MSPStatus::setI2cErrorCount(uint16_t value) { setU16(7, value); }

uint16_t MSPStatus::getSensor() const { return getU16(9); }
void MSPStatus::setSensor(uint16_t value) { setU16(9, value); }

uint32_t MSPStatus::getFlag() const { return getU32(11); }
void MSPStatus::setFlag(uint32_t value) { setU32(11, value); }

uint8_t MSPStatus::getCurrentProfile() const { return getByte(15); }
void MSPStatus::setCurrentProfile(uint8_t value) { setByte(15, value); }

bool MSPStatus::isValid() const {
    return getPayloadSize() == PAYLOAD_SIZE && MSPPacket::verifyCRC();
}

// ============================================================================
// MSPMotorTelemetry Implementation
// ============================================================================

MSPMotorTelemetry::MSPMotorTelemetry(const etl::span<uint8_t>& data) : MSPPacket(data) {
    setCommand(static_cast<uint8_t>(PacketType::MSP_MOTOR_TELEMETRY));
    setPayloadSize(PAYLOAD_SIZE);
}

uint16_t MSPMotorTelemetry::getMotor1RPM() const { return getU16(5); }
void MSPMotorTelemetry::setMotor1RPM(uint16_t value) { setU16(5, value); }

uint16_t MSPMotorTelemetry::getMotor2RPM() const { return getU16(7); }
void MSPMotorTelemetry::setMotor2RPM(uint16_t value) { setU16(7, value); }

uint16_t MSPMotorTelemetry::getMotor3RPM() const { return getU16(9); }
void MSPMotorTelemetry::setMotor3RPM(uint16_t value) { setU16(9, value); }

uint16_t MSPMotorTelemetry::getMotor4RPM() const { return getU16(11); }
void MSPMotorTelemetry::setMotor4RPM(uint16_t value) { setU16(11, value); }

uint16_t MSPMotorTelemetry::getMotor1Current() const { return getU16(13); }
void MSPMotorTelemetry::setMotor1Current(uint16_t value) { setU16(13, value); }

uint16_t MSPMotorTelemetry::getMotor2Current() const { return getU16(15); }
void MSPMotorTelemetry::setMotor2Current(uint16_t value) { setU16(15, value); }

uint16_t MSPMotorTelemetry::getMotor3Current() const { return getU16(17); }
void MSPMotorTelemetry::setMotor3Current(uint16_t value) { setU16(17, value); }

uint16_t MSPMotorTelemetry::getMotor4Current() const { return getU16(19); }
void MSPMotorTelemetry::setMotor4Current(uint16_t value) { setU16(19, value); }

uint16_t MSPMotorTelemetry::getMotor1Temperature() const { return getU16(21); }
void MSPMotorTelemetry::setMotor1Temperature(uint16_t value) { setU16(21, value); }

uint16_t MSPMotorTelemetry::getMotor2Temperature() const { return getU16(23); }
void MSPMotorTelemetry::setMotor2Temperature(uint16_t value) { setU16(23, value); }

uint16_t MSPMotorTelemetry::getMotor3Temperature() const { return getU16(25); }
void MSPMotorTelemetry::setMotor3Temperature(uint16_t value) { setU16(25, value); }

uint16_t MSPMotorTelemetry::getMotor4Temperature() const { return getU16(27); }
void MSPMotorTelemetry::setMotor4Temperature(uint16_t value) { setU16(27, value); }

bool MSPMotorTelemetry::isValid() const {
    return getPayloadSize() == PAYLOAD_SIZE && MSPPacket::verifyCRC();
}

// ============================================================================
// MSPAttitude Implementation
// ============================================================================

MSPAttitude::MSPAttitude(const etl::span<uint8_t>& data) : MSPPacket(data) {
    setCommand(static_cast<uint8_t>(PacketType::MSP_ATTITUDE));
    setPayloadSize(PAYLOAD_SIZE);
}

int16_t MSPAttitude::getRoll() const { return getI16(5); }
void MSPAttitude::setRoll(int16_t value) { setI16(5, value); }

int16_t MSPAttitude::getPitch() const { return getI16(7); }
void MSPAttitude::setPitch(int16_t value) { setI16(7, value); }

int16_t MSPAttitude::getYaw() const { return getI16(9); }
void MSPAttitude::setYaw(int16_t value) { setI16(9, value); }

bool MSPAttitude::isValid() const {
    return getPayloadSize() == PAYLOAD_SIZE && MSPPacket::verifyCRC();
}

// ============================================================================
// MSPAltitude Implementation
// ============================================================================

MSPAltitude::MSPAltitude(const etl::span<uint8_t>& data) : MSPPacket(data) {
    setCommand(static_cast<uint8_t>(PacketType::MSP_ALTITUDE));
    setPayloadSize(PAYLOAD_SIZE);
}

int32_t MSPAltitude::getAltitude() const { return static_cast<int32_t>(getU32(5)); }
void MSPAltitude::setAltitude(int32_t value) { setU32(5, static_cast<uint32_t>(value)); }

int16_t MSPAltitude::getVariometer() const { return getI16(9); }
void MSPAltitude::setVariometer(int16_t value) { setI16(9, value); }

bool MSPAltitude::isValid() const {
    return getPayloadSize() == PAYLOAD_SIZE && MSPPacket::verifyCRC();
}

// ============================================================================
// MSPPID Implementation
// ============================================================================

MSPPID::MSPPID(const etl::span<uint8_t>& data) : MSPPacket(data) {
    setCommand(static_cast<uint8_t>(PacketType::MSP_PID));
    setPayloadSize(PAYLOAD_SIZE);
}

uint8_t MSPPID::getRollP() const { return getByte(5); }
void MSPPID::setRollP(uint8_t value) { setByte(5, value); }

uint8_t MSPPID::getRollI() const { return getByte(6); }
void MSPPID::setRollI(uint8_t value) { setByte(6, value); }

uint8_t MSPPID::getRollD() const { return getByte(7); }
void MSPPID::setRollD(uint8_t value) { setByte(7, value); }

uint8_t MSPPID::getPitchP() const { return getByte(8); }
void MSPPID::setPitchP(uint8_t value) { setByte(8, value); }

uint8_t MSPPID::getPitchI() const { return getByte(9); }
void MSPPID::setPitchI(uint8_t value) { setByte(9, value); }

uint8_t MSPPID::getPitchD() const { return getByte(10); }
void MSPPID::setPitchD(uint8_t value) { setByte(10, value); }

uint8_t MSPPID::getYawP() const { return getByte(11); }
void MSPPID::setYawP(uint8_t value) { setByte(11, value); }

uint8_t MSPPID::getYawI() const { return getByte(12); }
void MSPPID::setYawI(uint8_t value) { setByte(12, value); }

uint8_t MSPPID::getYawD() const { return getByte(13); }
void MSPPID::setYawD(uint8_t value) { setByte(13, value); }

bool MSPPID::isValid() const {
    return getPayloadSize() == PAYLOAD_SIZE && MSPPacket::verifyCRC();
}

// ============================================================================
// MSPAccTrim Implementation
// ============================================================================

MSPAccTrim::MSPAccTrim(const etl::span<uint8_t>& data) : MSPPacket(data) {
    setCommand(static_cast<uint8_t>(PacketType::MSP_ACC_TRIM));
    setPayloadSize(PAYLOAD_SIZE);
}

int16_t MSPAccTrim::getAccTrimPitch() const { return getI16(5); }
void MSPAccTrim::setAccTrimPitch(int16_t value) { setI16(5, value); }

int16_t MSPAccTrim::getAccTrimRoll() const { return getI16(7); }
void MSPAccTrim::setAccTrimRoll(int16_t value) { setI16(7, value); }

bool MSPAccTrim::isValid() const {
    return getPayloadSize() == PAYLOAD_SIZE && MSPPacket::verifyCRC();
}


} // namespace msp
