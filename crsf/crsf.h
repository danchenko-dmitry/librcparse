#ifndef CRSF_H
#define CRSF_H

#include <cstdint>
#include <vector>
#include <etl/span.h>
#include <memory>
#include <array>
#include <stdexcept>

namespace crsf {

// CRSF protocol constants - CORRECTED according to official spec
constexpr size_t SYNC_SIZE = 1;        // sync byte (0xC8 or 0xEE)
constexpr size_t LEN_SIZE = 1;         // length field
constexpr size_t TYPE_SIZE = 1;        // type field
constexpr size_t CRC_SIZE = 1;         // crc field
constexpr size_t MIN_PACKET_SIZE = 4;  // sync + len + type + crc (minimum)
constexpr size_t MAX_PACKET_SIZE = 64; // CRSF spec maximum
constexpr size_t MAX_PAYLOAD_SIZE = MAX_PACKET_SIZE-SYNC_SIZE-LEN_SIZE-TYPE_SIZE-CRC_SIZE; // CRSF spec maximum
constexpr size_t BASE_PACKET_SIZE = SYNC_SIZE+LEN_SIZE+TYPE_SIZE+CRC_SIZE; // sync + len + type + crc (minimum)

#define CRSF_PACKET_TOTAL_LEN(payload_len) (SYNC_SIZE+LEN_SIZE+TYPE_SIZE+payload_len+CRC_SIZE)
#define CRSF_SYNC_INDEX(pos) (pos)
#define CRSF_LEN_INDEX(pos) (pos+SYNC_SIZE)
#define CRSF_TYPE_INDEX(pos) (pos+SYNC_SIZE+LEN_SIZE)
#define CRSF_PAYLOAD_INDEX(pos) (pos+SYNC_SIZE+LEN_SIZE+TYPE_SIZE)
#define CRSF_CRC_INDEX(pos,extended,payload_len) (pos+SYNC_SIZE+LEN_SIZE+TYPE_SIZE+(extended ? 2:0)+payload_len)

#define CRSF_CRC_POLY 0xd5

// CRSF Packet Types
enum class PacketType : uint8_t {
    GPS = 0x02,
    Vario = 0x07,
    BatterySensor = 0x08,
    BaroAltitude = 0x09,
    Heartbeat = 0x0B,
    LinkStatistics = 0x14,
    RCChannelsPacked = 0x16,
    SubsetRCChannelsPacked = 0x17,
    LinkRXID = 0x18,
    LinkTXID = 0x19,
    Attitude = 0x1E,
    FlightMode = 0x21,
    DevicePing = 0x28,
    DeviceInfo = 0x29,
    ParameterSettingsEntry = 0x2B,
    ParameterRead = 0x2C,
    ParameterWrite = 0x2D,
    ELRSStatus = 0x2E,
    Command = 0x32,
    Data = 0x33,
    RadioID = 0x3A,
    MSPReq = 0x7A
};

// CRSF Command IDs
enum class CommandID : uint8_t {
    Read = 0x2C,
    Write = 0x2D,
    SettingsEntry = 0x2B,
    Version = 0x28,
    DeviceInfo = 0x29
};

// CRSF Command Realms
enum class CommandRealm : uint8_t {
    RX = 0x10,      // Receiver commands
    General = 0x0A  // General commands
};

// CRSF Command Subcommands - RX Realm
enum class CommandRXSubcommand : uint8_t {
    Bind = 0x01,                    // Enter binding mode
    ModelSelectID = 0x05           // Set Receiver / Model ID
};

// CRSF Command Subcommands - General Realm
enum class CommandGeneralSubcommand : uint8_t {
    CRSFSpeedProposal = 0x70,      // Proposed new CRSF port speed
    CRSFSpeedResponse = 0x71       // Response to proposed CRSF port speed
};

// Base CRSF Packet class that wraps a byte array
class CRSFPacket {
protected:
    etl::span<uint8_t> data_;
    
    // Helper methods for safe access
    uint8_t getByte(size_t offset) const;
    void setByte(size_t offset, uint8_t value);
    uint16_t getU16(size_t offset) const;
    void setU16(size_t offset, uint16_t value);
    uint32_t getU32(size_t offset) const;
    void setU32(size_t offset, uint32_t value);
    
public:
    // Constructors
    CRSFPacket(const etl::span<uint8_t>& data);
    
    // Virtual destructor for inheritance
    virtual ~CRSFPacket() = default;
    
    // Basic packet properties
    uint8_t getAddress() const;
    void setAddress(uint8_t address);
    
    uint8_t getLength() const;
    void setLength(uint8_t length);
    
    uint8_t getType() const;
    void setType(uint8_t type);
    
    uint8_t getCRC() const;
    void setCRC(uint8_t crc);
    
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
    static std::unique_ptr<CRSFPacket> createPacket(PacketType type, const etl::span<uint8_t>& data);
};

// RC Channels packet wrapper
class CRSFRCChannels : public CRSFPacket {
public:
    CRSFRCChannels(const etl::span<uint8_t>& data);
    
    // Channel access methods
    uint16_t getChannel(uint8_t channel) const;
    void setChannel(uint8_t channel, uint16_t value);
    
    // Bulk operations
    void setAllChannels(const std::array<uint16_t, 16>& channels);
    std::array<uint16_t, 16> getAllChannels() const;
    
    // Utility methods
    void setAllCenter();
    void setAllMin();
    void setAllMax();
    void setStandardRC(uint16_t throttle, uint16_t elevator, uint16_t aileron, uint16_t rudder);
    
    // CRSF-PWM conversion methods (from official spec)
    static uint16_t crsfToPWM(uint16_t crsfValue);
    static uint16_t pwmToCRSF(uint16_t pwmValue);
    
    // ExpressLRS arm status support
    bool hasArmStatus() const;
    int8_t getArmStatus() const;
    void setArmStatus(uint8_t status);
    void setArmed(bool armed);
    bool isArmed() const;
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t CHANNEL_COUNT = 16;
    static constexpr size_t BITS_PER_CHANNEL = 11;
    static constexpr size_t PACKED_SIZE = 22;
    
    // CRSF channel value constants (from official spec)
    static constexpr uint16_t CRSF_MIN_VALUE = 0;      // 0
    static constexpr uint16_t CRSF_MAX_VALUE = 1984;  // 1984
    static constexpr uint16_t CRSF_CENTER_VALUE = 992; // 992 (1500μs)
    static constexpr uint16_t CRSF_MIN_PWM = 172;     // 172 (988μs)
    static constexpr uint16_t CRSF_MAX_PWM = 1811;    // 1811 (2012μs)
    
    // ExpressLRS support
    static constexpr size_t PAYLOAD_SIZE_BASIC = 22;   // 16 channels × 11 bits
    static constexpr size_t PAYLOAD_SIZE_ELRS = 23;    // + 1 byte arm status
};

// Subset RC Channels packet wrapper (CRSFv3)
class CRSFSubsetRCChannels : public CRSFPacket {
public:
    CRSFSubsetRCChannels(const etl::span<uint8_t>& data);
    

    // get packet size
    static size_t PacketSize(bool v4);

    // Config byte access methods
    uint8_t getConfigByte() const;
    void setConfigByte(uint8_t config);
    
    // Channel resolution methods
    uint8_t getChannelResolution() const;  // Returns 10, 11, 12, or 13
    void setChannelResolution(uint8_t resolution);
    
    // First channel number methods
    uint8_t getFirstChannelNumber() const;  // Returns 0-31 (0 = ch1)
    void setFirstChannelNumber(uint8_t channelNumber);
    
    // Reserved bit methods
    bool getReservedBit() const;
    void setReservedBit(bool reserved);
    
    // Channel access methods
    uint16_t getChannel(uint8_t channelIndex) const;  // channelIndex is 0-based within packet
    void setChannel(uint8_t channelIndex, uint16_t value);
    
    // Channel count methods
    size_t getChannelCount() const;  // Number of channels in this packet
    
    // Utility methods
    void setAllChannels(uint16_t value);
    void setAllCenter();
    void setAllMin();
    void setAllMax();
    
    // CRSF-PWM conversion methods for all resolutions
    static uint16_t crsfToPWM(uint16_t crsfValue, uint8_t resolution);
    static uint16_t pwmToCRSF(uint16_t pwmValue, uint8_t resolution);
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr uint8_t RESOLUTION_10BIT = 0;
    static constexpr uint8_t RESOLUTION_11BIT = 1;
    static constexpr uint8_t RESOLUTION_12BIT = 2;
    static constexpr uint8_t RESOLUTION_13BIT = 3;
    
    // PWM conversion constants for each resolution
    static constexpr uint16_t PWM_MIN = 998;   // 998μs
    static constexpr uint16_t PWM_MAX = 2012;  // 2012μs
    static constexpr uint16_t PWM_CENTER = 1500; // 1500μs
    
    // CRSF value ranges for each resolution
    static constexpr uint16_t CRSF_MAX_10BIT = 1023;
    static constexpr uint16_t CRSF_MAX_11BIT = 2047;
    static constexpr uint16_t CRSF_MAX_12BIT = 4095;
    static constexpr uint16_t CRSF_MAX_13BIT = 8191;
};

// ============================================================================
// CRSF Command Packet Classes
// ============================================================================

// Base CRSF Command packet class
class CRSFCommand : public CRSFPacket {
public:
    CRSFCommand(const etl::span<uint8_t>& data);
    
    // Command realm access
    uint8_t getCommandRealm() const;
    void setCommandRealm(uint8_t realm);
    
    // Extended payload access (realm + subcommand + payload + crc8-ba)
    etl::span<uint8_t> getExtendedPayload() const;
    void setExtendedPayload(const etl::span<uint8_t>& payload);
    
    // CRC8-BA calculation and verification
    void calculateCRC8BA();
    bool verifyCRC8BA() const;
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr uint8_t CRC8_BA_POLY = 0xBA;
    static constexpr size_t MIN_EXTENDED_PAYLOAD_SIZE = 2; // realm + subcommand
    static constexpr size_t MAX_EXTENDED_PAYLOAD_SIZE = 58; // 64 - 6 (sync + len + type + crc)
};

// CRSF Command RX (Receiver) packet class
class CRSFCommandRX : public CRSFCommand {
public:
    CRSFCommandRX(const etl::span<uint8_t>& data);
    
    // Subcommand access
    uint8_t getSubcommand() const;
    void setSubcommand(uint8_t subcommand);
    
    // Command-specific payload access
    etl::span<uint8_t> getCommandPayload() const;
    void setCommandPayload(const etl::span<uint8_t>& payload);
    
    // Bind command methods
    bool isBindCommand() const;
    void setBindCommand();
    
    // Model Select ID command methods
    bool isModelSelectIDCommand() const;
    uint8_t getModelID() const;
    void setModelSelectIDCommand(uint8_t modelID);
    
    // Validation
    bool isValid() const;
};

// CRSF Command General packet class
class CRSFCommandGeneral : public CRSFCommand {
public:
    CRSFCommandGeneral(const etl::span<uint8_t>& data);
    
    // Subcommand access
    uint8_t getSubcommand() const;
    void setSubcommand(uint8_t subcommand);
    
    // Command-specific payload access
    etl::span<uint8_t> getCommandPayload() const;
    void setCommandPayload(const etl::span<uint8_t>& payload);
    
    // CRSF Speed Proposal command methods
    bool isCRSFSpeedProposal() const;
    uint8_t getPortID() const;
    uint32_t getProposedBaudRate() const;
    void setCRSFSpeedProposal(uint8_t portID, uint32_t baudRate);
    
    // CRSF Speed Response command methods
    bool isCRSFSpeedResponse() const;
    uint8_t getResponsePortID() const;
    bool isAccepted() const;
    void setCRSFSpeedResponse(uint8_t portID, bool accepted);
    
    // Validation
    bool isValid() const;
};

// Utility functions
uint8_t calculateCRC8(const uint8_t* data, size_t len);
uint8_t calculateCRC8BA(const uint8_t* data, size_t len);
bool validatePacket(const CRSFPacket& packet);

// ============================================================================
// Binding Functions for Initializing CRSF Packets
// ============================================================================

// RC Channels binding functions
void bindRCChannels(
    CRSFRCChannels& packet,
    uint8_t address,
    const std::array<uint16_t, 16>& channels,
    bool includeArmStatus,
    bool armStatus,
    etl::span<uint8_t>& data
);

void bindRCChannelsStandard(
    CRSFRCChannels& packet,
    uint8_t address,
    uint16_t throttle,
    uint16_t elevator,
    uint16_t aileron,
    uint16_t rudder,
    etl::span<uint8_t>& data
);

// Subset RC Channels binding functions
void bindSubsetRCChannels(
    CRSFSubsetRCChannels& packet,
    uint8_t address,
    uint8_t firstChannelNumber,
    uint8_t resolution,
    const std::vector<uint16_t>& channels,
    etl::span<uint8_t>& data
);

void bindSubsetRCChannels10Bit(
    CRSFSubsetRCChannels& packet,
    uint8_t address,
    uint8_t firstChannelNumber,
    const std::vector<uint16_t>& channels,
    etl::span<uint8_t>& data
);

void bindSubsetRCChannels11Bit(
    CRSFSubsetRCChannels& packet,
    uint8_t address,
    uint8_t firstChannelNumber,
    const std::vector<uint16_t>& channels,
    etl::span<uint8_t>& data
);

void bindSubsetRCChannels12Bit(
    CRSFSubsetRCChannels& packet,
    uint8_t address,
    uint8_t firstChannelNumber,
    const std::vector<uint16_t>& channels,
    etl::span<uint8_t>& data
);

void bindSubsetRCChannels13Bit(
    CRSFSubsetRCChannels& packet,
    uint8_t address,
    uint8_t firstChannelNumber,
    const std::vector<uint16_t>& channels,
    etl::span<uint8_t>& data
);

// Generic packet binding function
void bindGenericPacket(
    CRSFPacket& packet,
    uint8_t address,
    PacketType type,
    const etl::span<uint8_t>& payload,
    etl::span<uint8_t>& data
);

// Command packet binding functions
void bindCommandPacket(
    CRSFCommand& packet,
    uint8_t address,
    uint8_t commandRealm,
    uint8_t subcommand,
    const etl::span<uint8_t>& commandPayload,
    etl::span<uint8_t>& data
);

void bindCommandRXPacket(
    CRSFCommandRX& packet,
    uint8_t address,
    CommandRXSubcommand subcommand,
    const etl::span<uint8_t>& commandPayload,
    etl::span<uint8_t>& data
);

void bindCommandGeneralPacket(
    CRSFCommandGeneral& packet,
    uint8_t address,
    CommandGeneralSubcommand subcommand,
    const etl::span<uint8_t>& commandPayload,
    etl::span<uint8_t>& data
);

// Specific command binding functions
void bindBindCommand(
    CRSFCommandRX& packet,
    uint8_t address,
    etl::span<uint8_t>& data
);

void bindModelSelectIDCommand(
    CRSFCommandRX& packet,
    uint8_t address,
    uint8_t modelID,
    etl::span<uint8_t>& data
);

void bindCRSFSpeedProposalCommand(
    CRSFCommandGeneral& packet,
    uint8_t address,
    uint8_t portID,
    uint32_t baudRate,
    etl::span<uint8_t>& data
);

void bindCRSFSpeedResponseCommand(
    CRSFCommandGeneral& packet,
    uint8_t address,
    uint8_t portID,
    bool accepted,
    etl::span<uint8_t>& data
);

} // namespace crsf

#endif // CRSF_H
