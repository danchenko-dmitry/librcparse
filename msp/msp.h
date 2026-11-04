#ifndef MSP_H
#define MSP_H

#include <cstdint>
#include <vector>
#include <etl/span.h>
#include <memory>
#include <array>
#include <stdexcept>

namespace msp {

// MSP protocol constants
constexpr size_t MSP_HEADER_SIZE = 3;        // '$', 'M', '<' or '>'
constexpr size_t MSP_PAYLOAD_SIZE = 1;       // payload size byte
constexpr size_t MSP_CMD_SIZE = 1;           // command byte
constexpr size_t MSP_CRC_SIZE = 1;           // crc field
constexpr size_t MIN_PACKET_SIZE = 6;        // header + size + cmd + crc
constexpr size_t MAX_PACKET_SIZE = 255;      // MSP spec maximum
constexpr size_t MAX_PAYLOAD_SIZE = 255;     // MSP spec maximum payload

// MSP packet direction
enum class PacketDirection : uint8_t {
    REQUEST = '<',   // Request to flight controller
    RESPONSE = '>'   // Response from flight controller
};

// MSP Packet Types (Command IDs) - Aligned with Betaflight msp_protocol.h
enum class PacketType : uint8_t {
    // Core API commands (1-99)
    MSP_API_VERSION = 1,        // out message: Get API version
    MSP_FC_VARIANT = 2,         // out message: Get flight controller variant
    MSP_FC_VERSION = 3,         // out message: Get flight controller version
    MSP_BOARD_INFO = 4,         // out message: Get board information
    MSP_BUILD_INFO = 5,         // out message: Get build information
    MSP_NAME = 10,              // out message: Returns user set board name
    MSP_SET_NAME = 11,          // in message:  Sets board name
    
    // Multiwii original MSP commands (101-139) - Most important ones
    MSP_STATUS = 101,           // out message: Cycletime & errors_count & sensor present & box activation
    MSP_RAW_IMU = 102,         // out message: 9 DOF
    MSP_SERVO = 103,           // out message: Servos
    MSP_MOTOR = 104,           // out message: Motors
    MSP_RC = 105,              // out message: RC channels and more
    MSP_RAW_GPS = 106,         // out message: Fix, numsat, lat, lon, alt, speed, ground course
    MSP_COMP_GPS = 107,        // out message: Distance home, direction home
    MSP_ATTITUDE = 108,        // out message: 2 angles 1 heading
    MSP_ALTITUDE = 109,        // out message: Altitude, variometer
    MSP_ANALOG = 110,          // out message: Vbat, powermetersum, rssi if available on RX
    MSP_RC_TUNING = 111,       // out message: RC rate, rc expo, rollpitch rate, yaw rate, dyn throttle PID
    MSP_PID = 112,             // out message: P I D coeff (9 are used currently)
    MSP_BOXNAMES = 116,        // out message: The aux switch names
    MSP_PIDNAMES = 117,        // out message: The PID names
    MSP_WP = 118,              // out message: Get a WP, WP# is in the payload
    MSP_BOXIDS = 119,          // out message: Get the permanent IDs associated to BOXes
    MSP_SERVO_CONFIGURATIONS = 120, // out message: All servo configurations
    MSP_MOTOR_TELEMETRY = 139, // out message: Per-motor telemetry data
    
    // Set commands (200-229)
    MSP_SET_RAW_RC = 200,      // in message:  8 rc chan
    MSP_SET_RAW_GPS = 201,     // in message:  Fix, numsat, lat, lon, alt, speed
    MSP_SET_PID = 202,         // in message:  P I D coeff (9 are used currently)
    MSP_SET_RC_TUNING = 204,   // in message:  RC rate, rc expo, rollpitch rate, yaw rate, dyn throttle PID
    MSP_ACC_CALIBRATION = 205, // in message:  No param - calibrate accelerometer
    MSP_MAG_CALIBRATION = 206, // in message:  No param - calibrate magnetometer
    MSP_RESET_CONF = 208,      // in message:  No param - reset settings
    MSP_SET_WP = 209,          // in message:  Sets a given WP
    MSP_SELECT_SETTING = 210,  // in message:  Select setting number (0-2)
    MSP_SET_HEADING = 211,     // in message:  Define a new heading hold direction
    MSP_SET_SERVO_CONFIGURATION = 212, // in message:  Servo settings
    MSP_SET_MOTOR = 214,       // in message:  PropBalance function
    
    // Multiple MSP and special commands (230-255)
    MSP_SET_ACC_TRIM = 239,    // in message:  Set acc angle trim values
    MSP_ACC_TRIM = 240,        // out message: Get acc angle trim values
    MSP_SERVO_MIX_RULES = 241, // out message: Get servo mixer configuration
    MSP_SET_SERVO_MIX_RULE = 242, // in message:  Set servo mixer configuration
    
    // Legacy support
    MSP_IDENT = 100            // Legacy - replaced by MSP_API_VERSION
};

// TODO: need to check if return values are correct:
inline std::string getMspPacketTypeName(PacketType type) {
    switch (type) {
        // Core API commands
        case PacketType::MSP_API_VERSION: return "MSP_API_VERSION";
        case PacketType::MSP_FC_VARIANT: return "MSP_FC_VARIANT";
        case PacketType::MSP_FC_VERSION: return "MSP_FC_VERSION";
        case PacketType::MSP_BOARD_INFO: return "MSP_BOARD_INFO";
        case PacketType::MSP_BUILD_INFO: return "MSP_BUILD_INFO";
        case PacketType::MSP_NAME: return "MSP_NAME";
        case PacketType::MSP_SET_NAME: return "MSP_SET_NAME";
        
        // Legacy support
        case PacketType::MSP_IDENT: return "MSP_IDENT";
        
        // Multiwii original commands
        case PacketType::MSP_STATUS: return "MSP_STATUS";
        case PacketType::MSP_RAW_IMU: return "MSP_RAW_IMU";
        case PacketType::MSP_MOTOR: return "MSP_MOTOR";
        case PacketType::MSP_RC: return "MSP_RC";
        case PacketType::MSP_RAW_GPS: return "MSP_RAW_GPS";
        case PacketType::MSP_COMP_GPS: return "MSP_COMP_GPS";
        case PacketType::MSP_ATTITUDE: return "MSP_ATTITUDE";
        case PacketType::MSP_ALTITUDE: return "MSP_ALTITUDE";
        case PacketType::MSP_ANALOG: return "MSP_ANALOG";
        case PacketType::MSP_RC_TUNING: return "MSP_RC_TUNING";
        case PacketType::MSP_PID: return "MSP_PID";
        case PacketType::MSP_BOXNAMES: return "MSP_BOXNAMES";
        case PacketType::MSP_PIDNAMES: return "MSP_PIDNAMES";
        case PacketType::MSP_WP: return "MSP_WP";
        case PacketType::MSP_BOXIDS: return "MSP_BOXIDS";
        case PacketType::MSP_SERVO: return "MSP_SERVO";
        case PacketType::MSP_SERVO_CONFIGURATIONS: return "MSP_SERVO_CONFIGURATIONS";
        case PacketType::MSP_SET_RAW_RC: return "MSP_SET_RAW_RC";
        case PacketType::MSP_SET_RAW_GPS: return "MSP_SET_RAW_GPS";
        case PacketType::MSP_SET_PID: return "MSP_SET_PID";
        case PacketType::MSP_SET_RC_TUNING: return "MSP_SET_RC_TUNING";
        case PacketType::MSP_ACC_CALIBRATION: return "MSP_ACC_CALIBRATION";
        case PacketType::MSP_MAG_CALIBRATION: return "MSP_MAG_CALIBRATION";
        case PacketType::MSP_RESET_CONF: return "MSP_RESET_CONF";
        case PacketType::MSP_SET_WP: return "MSP_SET_WP";
        case PacketType::MSP_SELECT_SETTING: return "MSP_SELECT_SETTING";
        case PacketType::MSP_SET_HEADING: return "MSP_SET_HEADING";
        case PacketType::MSP_SET_SERVO_CONFIGURATION: return "MSP_SET_SERVO_CONFIGURATION";
        case PacketType::MSP_SET_MOTOR: return "MSP_SET_MOTOR";
        case PacketType::MSP_SET_ACC_TRIM: return "MSP_SET_ACC_TRIM";
        case PacketType::MSP_ACC_TRIM: return "MSP_ACC_TRIM";
        case PacketType::MSP_SERVO_MIX_RULES: return "MSP_SERVO_MIX_RULES";
        case PacketType::MSP_SET_SERVO_MIX_RULE: return "MSP_SET_SERVO_MIX_RULE";
        case PacketType::MSP_MOTOR_TELEMETRY: return "MSP_MOTOR_TELEMETRY";
        default: return "Unknown(" + std::to_string(static_cast<int>(type)) + ")";
    }
}

// Base MSP Packet class that wraps a byte array
class MSPPacket {
protected:
    etl::span<uint8_t> data_;
    
    // Helper methods for safe access
    uint8_t getByte(size_t offset) const;
    void setByte(size_t offset, uint8_t value);
    uint16_t getU16(size_t offset) const;
    void setU16(size_t offset, uint16_t value);
    uint32_t getU32(size_t offset) const;
    void setU32(size_t offset, uint32_t value);
    int16_t getI16(size_t offset) const;
    void setI16(size_t offset, int16_t value);
    
public:
    // Constructors
    MSPPacket();
    MSPPacket(const etl::span<uint8_t>& data);

    bool init(PacketType type,PacketDirection direction,const etl::span<uint8_t>& data);
    bool bind(const etl::span<uint8_t>& data);
    
    // Virtual destructor for inheritance
    virtual ~MSPPacket() = default;
    
    // Basic packet properties
    PacketDirection getDirection() const;
    void setDirection(PacketDirection direction);

    void setType(PacketType type);
    PacketType getType() const;
    
    uint8_t getPayloadSize() const;
    void setPayloadSize(uint8_t size);
    
    uint8_t getCommand() const;
    void setCommand(uint8_t command);
    
    uint8_t getCRC() const;
    void setCRC(uint8_t crc);
    
    // Header management
    void setHeader();
    
    // Payload access
    etl::span<uint8_t> getPayload() const;
    void setPayload(const etl::span<uint8_t>& payload);
    void setPayload(const uint8_t* data, size_t len);
    
    // Raw data access
    const etl::span<uint8_t>& getRawData() const;
    etl::span<uint8_t>& getRawData();
    
    // Packet operations
    size_t getTotalSize() const;
    void calculateCRC();
    bool verifyCRC() const;
    
    // Factory method to create typed packet wrappers
    static std::unique_ptr<MSPPacket> createPacket(PacketType type, const etl::span<uint8_t>& data);
};

// MSP Request class for requests without payload
class MSPRequest : public MSPPacket {
public:
    MSPRequest(const etl::span<uint8_t>& data);
    
    // Set the command type for the request
    void setRequestCommand(PacketType command);
    
    // Get the command type
    PacketType getRequestCommand() const;
    
    // Validation - requests should have no payload and valid CRC
    bool isValid() const;
    
    // Constants
    static constexpr size_t PAYLOAD_SIZE = 0;  // Requests have no payload

    inline static size_t getPacketSize() {return PAYLOAD_SIZE+MSP_HEADER_SIZE+MSP_PAYLOAD_SIZE+MSP_CMD_SIZE+MSP_CRC_SIZE;}
};

// MSP_RAW_IMU packet wrapper
class MSPRawIMU : public MSPPacket {
public:
    MSPRawIMU(const etl::span<uint8_t>& data);
    
    // IMU data access methods
    int16_t getAccX() const;
    void setAccX(int16_t value);
    int16_t getAccY() const;
    void setAccY(int16_t value);
    int16_t getAccZ() const;
    void setAccZ(int16_t value);
    
    int16_t getGyroX() const;
    void setGyroX(int16_t value);
    int16_t getGyroY() const;
    void setGyroY(int16_t value);
    int16_t getGyroZ() const;
    void setGyroZ(int16_t value);
    
    int16_t getMagX() const;
    void setMagX(int16_t value);
    int16_t getMagY() const;
    void setMagY(int16_t value);
    int16_t getMagZ() const;
    void setMagZ(int16_t value);
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t PAYLOAD_SIZE = 18;  // 6 * 3 (acc, gyro, mag)
};

// MSP_STATUS packet wrapper
class MSPStatus : public MSPPacket {
public:
    MSPStatus(const etl::span<uint8_t>& data);
    
    // Status data access methods
    uint16_t getCycleTime() const;
    void setCycleTime(uint16_t value);
    
    uint16_t getI2cErrorCount() const;
    void setI2cErrorCount(uint16_t value);
    
    uint16_t getSensor() const;
    void setSensor(uint16_t value);
    
    uint32_t getFlag() const;
    void setFlag(uint32_t value);
    
    uint8_t getCurrentProfile() const;
    void setCurrentProfile(uint8_t value);
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t PAYLOAD_SIZE = 11;  // 2+2+2+4+1
};

// MSP_MOTOR_TELEMETRY packet wrapper
class MSPMotorTelemetry : public MSPPacket {
public:
    MSPMotorTelemetry(const etl::span<uint8_t>& data);
    
    // Motor telemetry data access methods
    uint16_t getMotor1RPM() const;
    void setMotor1RPM(uint16_t value);
    uint16_t getMotor2RPM() const;
    void setMotor2RPM(uint16_t value);
    uint16_t getMotor3RPM() const;
    void setMotor3RPM(uint16_t value);
    uint16_t getMotor4RPM() const;
    void setMotor4RPM(uint16_t value);
    
    uint16_t getMotor1Current() const;
    void setMotor1Current(uint16_t value);
    uint16_t getMotor2Current() const;
    void setMotor2Current(uint16_t value);
    uint16_t getMotor3Current() const;
    void setMotor3Current(uint16_t value);
    uint16_t getMotor4Current() const;
    void setMotor4Current(uint16_t value);
    
    uint16_t getMotor1Temperature() const;
    void setMotor1Temperature(uint16_t value);
    uint16_t getMotor2Temperature() const;
    void setMotor2Temperature(uint16_t value);
    uint16_t getMotor3Temperature() const;
    void setMotor3Temperature(uint16_t value);
    uint16_t getMotor4Temperature() const;
    void setMotor4Temperature(uint16_t value);
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t PAYLOAD_SIZE = 24;  // 4*2 + 4*2 + 4*2 (rpm, current, temp)
};

// MSP_ATTITUDE packet wrapper
class MSPAttitude : public MSPPacket {
public:
    MSPAttitude(const etl::span<uint8_t>& data);
    
    // Attitude data access methods
    int16_t getRoll() const;
    void setRoll(int16_t value);
    int16_t getPitch() const;
    void setPitch(int16_t value);
    int16_t getYaw() const;
    void setYaw(int16_t value);
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t PAYLOAD_SIZE = 6;  // 3 * 2 (roll, pitch, yaw)
};

// MSP_ALTITUDE packet wrapper
class MSPAltitude : public MSPPacket {
public:
    MSPAltitude(const etl::span<uint8_t>& data);
    
    // Altitude data access methods
    int32_t getAltitude() const;
    void setAltitude(int32_t value);
    int16_t getVariometer() const;
    void setVariometer(int16_t value);
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t PAYLOAD_SIZE = 6;  // 4 + 2 (altitude, variometer)
};

// MSP_PID packet wrapper
class MSPPID : public MSPPacket {
public:
    MSPPID(const etl::span<uint8_t>& data);
    
    // PID data access methods
    uint8_t getRollP() const;
    void setRollP(uint8_t value);
    uint8_t getRollI() const;
    void setRollI(uint8_t value);
    uint8_t getRollD() const;
    void setRollD(uint8_t value);
    
    uint8_t getPitchP() const;
    void setPitchP(uint8_t value);
    uint8_t getPitchI() const;
    void setPitchI(uint8_t value);
    uint8_t getPitchD() const;
    void setPitchD(uint8_t value);
    
    uint8_t getYawP() const;
    void setYawP(uint8_t value);
    uint8_t getYawI() const;
    void setYawI(uint8_t value);
    uint8_t getYawD() const;
    void setYawD(uint8_t value);
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t PAYLOAD_SIZE = 30;  // 9 * 3 (roll, pitch, yaw PIDs)
};

// MSP_ACC_TRIM packet wrapper
class MSPAccTrim : public MSPPacket {
public:
    MSPAccTrim(const etl::span<uint8_t>& data);
    
    // Acc trim data access methods
    int16_t getAccTrimPitch() const;
    void setAccTrimPitch(int16_t value);
    int16_t getAccTrimRoll() const;
    void setAccTrimRoll(int16_t value);
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t PAYLOAD_SIZE = 4;  // 2 + 2 (pitch, roll trim)
};

// Utility functions
uint8_t calculateCRC8(const uint8_t* data, size_t len);
bool validatePacket(const MSPPacket& packet);

} // namespace msp

#endif // MSP_H
