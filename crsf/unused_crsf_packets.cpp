/*
namespace crsf {


// ============================================================================
// CRSFLinkRXID Implementation (CRSFv3)
// ============================================================================

CRSFLinkRXID::CRSFLinkRXID(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::LinkRXID)) {
        setType(static_cast<uint8_t>(PacketType::LinkRXID));
    }
}

uint8_t CRSFLinkRXID::getDownlinkRSSIdBm() const {
    return getByte(3);
}

void CRSFLinkRXID::setDownlinkRSSIdBm(uint8_t rssi) {
    setByte(3, rssi);
}

uint8_t CRSFLinkRXID::getDownlinkRSSIPercent() const {
    return getByte(4);
}

void CRSFLinkRXID::setDownlinkRSSIPercent(uint8_t percent) {
    if (percent > RSSI_PERCENT_MAX) percent = RSSI_PERCENT_MAX;
    setByte(4, percent);
}

uint8_t CRSFLinkRXID::getDownlinkLinkQuality() const {
    return getByte(5);
}

void CRSFLinkRXID::setDownlinkLinkQuality(uint8_t quality) {
    if (quality > LINK_QUALITY_MAX) quality = LINK_QUALITY_MAX;
    setByte(5, quality);
}

int8_t CRSFLinkRXID::getDownlinkSNR() const {
    return static_cast<int8_t>(getByte(6));
}

void CRSFLinkRXID::setDownlinkSNR(int8_t snr) {
    if (snr < SNR_MIN) snr = SNR_MIN;
    if (snr > SNR_MAX) snr = SNR_MAX;
    setByte(6, static_cast<uint8_t>(snr));
}

uint8_t CRSFLinkRXID::getUplinkRFPowerIndex() const {
    return getByte(7);
}

void CRSFLinkRXID::setUplinkRFPowerIndex(uint8_t powerIndex) {
    if (powerIndex > RF_POWER_INDEX_MAX) powerIndex = RF_POWER_INDEX_MAX;
    setByte(7, powerIndex);
}

// ============================================================================
// Utility Methods for RSSI Conversion
// ============================================================================

int8_t CRSFLinkRXID::getDownlinkRSSIdBmSigned() const {
    // CRSF stores RSSI as (dBm * -1), so convert back
    return -static_cast<int8_t>(getDownlinkRSSIdBm());
}

void CRSFLinkRXID::setDownlinkRSSIdBmSigned(int8_t dbm) {
    // CRSF stores RSSI as (dBm * -1)
    setDownlinkRSSIdBm(static_cast<uint8_t>(-dbm));
}

bool CRSFLinkRXID::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::LinkRXID)) return false;
    
    // Check minimum packet size
    if (data_.size() < 8) return false;  // header(3) + payload(5)
    
    // Check length field
    if (getLength() != (PAYLOAD_SIZE + 1 + 1)) return false;  // payload + type + crc
    
    // Check RSSI percent range
    uint8_t rssiPercent = getDownlinkRSSIPercent();
    if (rssiPercent > RSSI_PERCENT_MAX) return false;
    
    // Check Link Quality range
    uint8_t linkQuality = getDownlinkLinkQuality();
    if (linkQuality > LINK_QUALITY_MAX) return false;
    
    // Check SNR range
    int8_t snr = getDownlinkSNR();
    if (snr < SNR_MIN || snr > SNR_MAX) return false;
    
    // Check RF Power Index range
    uint8_t rfPowerIndex = getUplinkRFPowerIndex();
    if (rfPowerIndex > RF_POWER_INDEX_MAX) return false;
    
    return true;
}

// ============================================================================
// CRSFLinkTXID Implementation (CRSFv3)
// ============================================================================

CRSFLinkTXID::CRSFLinkTXID(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::LinkTXID)) {
        setType(static_cast<uint8_t>(PacketType::LinkTXID));
    }
}

uint8_t CRSFLinkTXID::getUplinkRSSIdBm() const {
    return getByte(3);
}

void CRSFLinkTXID::setUplinkRSSIdBm(uint8_t rssi) {
    setByte(3, rssi);
}

uint8_t CRSFLinkTXID::getUplinkRSSIPercent() const {
    return getByte(4);
}

void CRSFLinkTXID::setUplinkRSSIPercent(uint8_t percent) {
    if (percent > RSSI_PERCENT_MAX) percent = RSSI_PERCENT_MAX;
    setByte(4, percent);
}

uint8_t CRSFLinkTXID::getUplinkLinkQuality() const {
    return getByte(5);
}

void CRSFLinkTXID::setUplinkLinkQuality(uint8_t quality) {
    if (quality > LINK_QUALITY_MAX) quality = LINK_QUALITY_MAX;
    setByte(5, quality);
}

int8_t CRSFLinkTXID::getUplinkSNR() const {
    return static_cast<int8_t>(getByte(6));
}

void CRSFLinkTXID::setUplinkSNR(int8_t snr) {
    if (snr < SNR_MIN) snr = SNR_MIN;
    if (snr > SNR_MAX) snr = SNR_MAX;
    setByte(6, static_cast<uint8_t>(snr));
}

uint8_t CRSFLinkTXID::getDownlinkRFPower() const {
    return getByte(7);
}

void CRSFLinkTXID::setDownlinkRFPower(uint8_t power) {
    if (power > RF_POWER_MAX) power = RF_POWER_MAX;
    setByte(7, power);
}

uint8_t CRSFLinkTXID::getPacketRateFPS() const {
    return getByte(8);
}

void CRSFLinkTXID::setPacketRateFPS(uint8_t fps) {
    if (fps > FPS_MAX) fps = FPS_MAX;
    setByte(8, fps);
}

// ============================================================================
// Utility Methods for RSSI and FPS Conversion
// ============================================================================

int8_t CRSFLinkTXID::getUplinkRSSIdBmSigned() const {
    // CRSF stores RSSI as (dBm * -1), so convert back
    return -static_cast<int8_t>(getUplinkRSSIdBm());
}

void CRSFLinkTXID::setUplinkRSSIdBmSigned(int8_t dbm) {
    // CRSF stores RSSI as (dBm * -1)
    setUplinkRSSIdBm(static_cast<uint8_t>(-dbm));
}

uint16_t CRSFLinkTXID::getPacketRateFPSRaw() const {
    // Convert stored value (FPS/10) back to actual FPS
    return static_cast<uint16_t>(getPacketRateFPS()) * 10;
}

void CRSFLinkTXID::setPacketRateFPSRaw(uint16_t fps) {
    // Convert FPS to stored value (FPS/10)
    uint8_t storedValue = static_cast<uint8_t>(fps / 10);
    if (storedValue > FPS_MAX) storedValue = FPS_MAX;
    setPacketRateFPS(storedValue);
}

bool CRSFLinkTXID::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::LinkTXID)) return false;
    
    // Check minimum packet size
    if (data_.size() < 9) return false;  // header(3) + payload(6)
    
    // Check length field
    if (getLength() != (PAYLOAD_SIZE + 1 + 1)) return false;  // payload + type + crc
    
    // Check RSSI percent range
    uint8_t rssiPercent = getUplinkRSSIPercent();
    if (rssiPercent > RSSI_PERCENT_MAX) return false;
    
    // Check Link Quality range
    uint8_t linkQuality = getUplinkLinkQuality();
    if (linkQuality > LINK_QUALITY_MAX) return false;
    
    // Check SNR range
    int8_t snr = getUplinkSNR();
    if (snr < SNR_MIN || snr > SNR_MAX) return false;
    
    // Check RF Power range
    uint8_t rfPower = getDownlinkRFPower();
    if (rfPower > RF_POWER_MAX) return false;
    
    // Check FPS range
    uint8_t fps = getPacketRateFPS();
    if (fps > FPS_MAX) return false;
    
    return true;
}

// ============================================================================
// CRSFAttitude Implementation
// ============================================================================

CRSFAttitude::CRSFAttitude(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::Attitude)) {
        setType(static_cast<uint8_t>(PacketType::Attitude));
    }
}

// ============================================================================
// Basic Field Getters and Setters
// ============================================================================

int16_t CRSFAttitude::getPitch() const {
    return static_cast<int16_t>(getU16(3));
}

void CRSFAttitude::setPitch(int16_t pitch) {
    if (pitch < ANGLE_MIN) pitch = ANGLE_MIN;
    if (pitch > ANGLE_MAX) pitch = ANGLE_MAX;
    setU16(3, static_cast<uint16_t>(pitch));
}

int16_t CRSFAttitude::getRoll() const {
    return static_cast<int16_t>(getU16(5));
}

void CRSFAttitude::setRoll(int16_t roll) {
    if (roll < ANGLE_MIN) roll = ANGLE_MIN;
    if (roll > ANGLE_MAX) roll = ANGLE_MAX;
    setU16(5, static_cast<uint16_t>(roll));
}

int16_t CRSFAttitude::getYaw() const {
    return static_cast<int16_t>(getU16(7));
}

void CRSFAttitude::setYaw(int16_t yaw) {
    if (yaw < ANGLE_MIN) yaw = ANGLE_MIN;
    if (yaw > ANGLE_MAX) yaw = ANGLE_MAX;
    setU16(7, static_cast<uint16_t>(yaw));
}

// ============================================================================
// Utility Methods for Angle Conversion
// ============================================================================

double CRSFAttitude::getPitchDegrees() const {
    return getPitchRadians() * RADIANS_TO_DEGREES;
}

void CRSFAttitude::setPitchDegrees(double degrees) {
    setPitchRadians(degrees * DEGREES_TO_RADIANS);
}

double CRSFAttitude::getRollDegrees() const {
    return getRollRadians() * RADIANS_TO_DEGREES;
}

void CRSFAttitude::setRollDegrees(double degrees) {
    setRollRadians(degrees * DEGREES_TO_RADIANS);
}

double CRSFAttitude::getYawDegrees() const {
    return getYawRadians() * RADIANS_TO_DEGREES;
}

void CRSFAttitude::setYawDegrees(double degrees) {
    setYawRadians(degrees * DEGREES_TO_RADIANS);
}

double CRSFAttitude::getPitchRadians() const {
    return static_cast<double>(getPitch()) * STORED_TO_RADIANS;
}

void CRSFAttitude::setPitchRadians(double radians) {
    // Clamp to ±π range
    if (radians < -M_PI) radians = -M_PI;
    if (radians > M_PI) radians = M_PI;
    
    int16_t storedValue = static_cast<int16_t>(std::round(radians * RADIANS_TO_STORED));
    setPitch(storedValue);
}

double CRSFAttitude::getRollRadians() const {
    return static_cast<double>(getRoll()) * STORED_TO_RADIANS;
}

void CRSFAttitude::setRollRadians(double radians) {
    // Clamp to ±π range
    if (radians < -M_PI) radians = -M_PI;
    if (radians > M_PI) radians = M_PI;
    
    int16_t storedValue = static_cast<int16_t>(std::round(radians * RADIANS_TO_STORED));
    setRoll(storedValue);
}

double CRSFAttitude::getYawRadians() const {
    return static_cast<double>(getYaw()) * STORED_TO_RADIANS;
}

void CRSFAttitude::setYawRadians(double radians) {
    // Clamp to ±π range
    if (radians < -M_PI) radians = -M_PI;
    if (radians > M_PI) radians = M_PI;
    
    int16_t storedValue = static_cast<int16_t>(std::round(radians * RADIANS_TO_STORED));
    int16_t normalizedValue = storedValue;
    
    // Normalize yaw to ±π range (handle wraparound)
    while (normalizedValue > ANGLE_MAX) normalizedValue -= 2 * ANGLE_MAX;
    while (normalizedValue < ANGLE_MIN) normalizedValue += 2 * ANGLE_MAX;
    
    setYaw(normalizedValue);
}

bool CRSFAttitude::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::Attitude)) return false;
    
    // Check minimum packet size
    if (data_.size() < 9) return false;  // header(3) + payload(6)
    
    // Check length field
    if (getLength() != (PAYLOAD_SIZE + 1 + 1)) return false;  // payload + type + crc
    
    // Check angle ranges
    int16_t pitch = getPitch();
    int16_t roll = getRoll();
    int16_t yaw = getYaw();
    
    if (pitch < ANGLE_MIN || pitch > ANGLE_MAX) return false;
    if (roll < ANGLE_MIN || roll > ANGLE_MAX) return false;
    if (yaw < ANGLE_MIN || yaw > ANGLE_MAX) return false;
    
    return true;
}

// ============================================================================
// CRSFFlightMode Implementation
// ============================================================================

CRSFFlightMode::CRSFFlightMode(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::FlightMode)) {
        setType(static_cast<uint8_t>(PacketType::FlightMode));
    }
}

// ============================================================================
// Flight Mode String Methods
// ============================================================================

std::string CRSFFlightMode::getFlightMode() const {
    std::string mode;
    // Read string from payload starting at offset 3 (after header)
    for (size_t i = 3; i < data_.size() && i < (3 + MAX_STRING_LENGTH); ++i) {
        uint8_t byte = getByte(i);
        if (byte == 0) break;  // Null terminator
        mode += static_cast<char>(byte);
    }
    return mode;
}

void CRSFFlightMode::setFlightMode(const std::string& mode) {
    // Truncate if too long, allowing full MAX_STRING_LENGTH characters
    std::string truncatedMode = mode.substr(0, MAX_STRING_LENGTH);
    
    // Clear existing payload
    for (size_t i = 3; i < (3 + PAYLOAD_SIZE); ++i) {
        if (i < data_.size()) {
            setByte(i, 0);
        }
    }
    
    // Set the string bytes
    for (size_t i = 0; i < truncatedMode.length(); ++i) {
        if ((3 + i) < data_.size()) {
            setByte(3 + i, static_cast<uint8_t>(truncatedMode[i]));
        }
    }
    
    // Set null terminator
    if ((3 + truncatedMode.length()) < data_.size()) {
        setByte(3 + truncatedMode.length(), 0);
    }
    
    // Update length field
    // LEN = type(1) + payload + crc(1) = (string.length() + 1) + 2 = string.length() + 3
    setLength(static_cast<uint8_t>(truncatedMode.length() + 3));
}

// ============================================================================
// Utility Methods
// ============================================================================

size_t CRSFFlightMode::getFlightModeLength() const {
    return getFlightMode().length();
}

bool CRSFFlightMode::isEmpty() const {
    return getFlightMode().empty();
}

bool CRSFFlightMode::isAcro() const {
    return getFlightMode() == MODE_ACRO;
}

bool CRSFFlightMode::isHorizon() const {
    return getFlightMode() == MODE_HORIZON;
}

bool CRSFFlightMode::isAngle() const {
    return getFlightMode() == MODE_ANGLE;
}

bool CRSFFlightMode::isRTH() const {
    return getFlightMode() == MODE_RTH;
}

bool CRSFFlightMode::isLand() const {
    return getFlightMode() == MODE_LAND;
}

bool CRSFFlightMode::isTakeoff() const {
    return getFlightMode() == MODE_TAKEOFF;
}

bool CRSFFlightMode::isManual() const {
    return getFlightMode() == MODE_MANUAL;
}

bool CRSFFlightMode::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::FlightMode)) return false;
    
    // Check minimum packet size
    if (data_.size() < (3 + PAYLOAD_SIZE)) return false;  // header(3) + payload(14)
    
    // Check length field
    std::string mode = getFlightMode();
    // LEN = type(1) + payload + crc(1) = (string.length() + 1) + 2 = string.length() + 3
    if (getLength() != (mode.length() + 3)) return false;
    
    // Check string length
    if (mode.length() > MAX_STRING_LENGTH) return false;
    
    return true;
}

// ============================================================================
// CRSFDevicePing Implementation
// ============================================================================

CRSFDevicePing::CRSFDevicePing(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::DevicePing)) {
        setType(static_cast<uint8_t>(PacketType::DevicePing));
    }
}

// ============================================================================
// Extended Address Methods
// ============================================================================

uint8_t CRSFDevicePing::getExtendedDestination() const {
    return getByte(3);
}

void CRSFDevicePing::setExtendedDestination(uint8_t destination) {
    setByte(3, destination);
}

uint8_t CRSFDevicePing::getExtendedSource() const {
    return getByte(4);
}

void CRSFDevicePing::setExtendedSource(uint8_t source) {
    setByte(4, source);
}

// ============================================================================
// Utility Methods
// ============================================================================

bool CRSFDevicePing::isBroadcast() const {
    return getExtendedDestination() == ADDRESS_BROADCAST;
}

bool CRSFDevicePing::isTargeted() const {
    return getExtendedDestination() != ADDRESS_BROADCAST;
}

bool CRSFDevicePing::isFromRadio() const {
    return getExtendedSource() == ADDRESS_RADIO_TRANSMITTER;
}

bool CRSFDevicePing::isFromFlightController() const {
    return getExtendedSource() == ADDRESS_FLIGHT_CONTROLLER;
}

bool CRSFDevicePing::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::DevicePing)) return false;
    
    // Check minimum packet size
    if (data_.size() < 6) return false;  // header(3) + payload(2) + crc(1)
    
    // Check length field
    if (getLength() != (PAYLOAD_SIZE + 1 + 1)) return false;  // payload + type + crc
    
    return true;
}

// ============================================================================
// CRSFDeviceInfo Implementation
// ============================================================================

CRSFDeviceInfo::CRSFDeviceInfo(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::DeviceInfo)) {
        setType(static_cast<uint8_t>(PacketType::DeviceInfo));
    }
}

// ============================================================================
// Display Name Methods
// ============================================================================

std::string CRSFDeviceInfo::getDisplayName() const {
    // Read null-terminated string from payload starting at byte 3
    std::string name;
    for (size_t i = 3; i < 3 + MAX_DISPLAY_NAME_LENGTH && i < data_.size(); ++i) {
        if (data_[i] == 0) break;  // Stop at null terminator
        name += static_cast<char>(data_[i]);
    }
    return name;
}

void CRSFDeviceInfo::setDisplayName(const std::string& name) {
    // Truncate if too long and ensure null termination
    std::string truncatedName = name.substr(0, MAX_DISPLAY_NAME_LENGTH);
    
    // Write the string to payload starting at byte 3
    for (size_t i = 0; i < truncatedName.length(); ++i) {
        setByte(3 + i, static_cast<uint8_t>(truncatedName[i]));
    }
    
    // Add null terminator
    setByte(3 + truncatedName.length(), 0);
    
    // Fill remaining bytes with nulls to ensure consistent layout
    for (size_t i = truncatedName.length() + 1; i < 16; ++i) {
        setByte(3 + i, 0);
    }
    
    // Update length field
    setLength(PAYLOAD_SIZE + 1 + 1);  // payload + type + crc
}

size_t CRSFDeviceInfo::getDisplayNameLength() const {
    return getDisplayName().length();
}

bool CRSFDeviceInfo::isDisplayNameEmpty() const {
    return getDisplayName().empty();
}

// ============================================================================
// Serial Number Methods
// ============================================================================

std::string CRSFDeviceInfo::getSerialNumber() const {
    // Serial number starts at byte 19 (3 + 16 for display name)
    std::string serial;
    for (size_t i = 19; i < 19 + SERIAL_NUMBER_LENGTH && i < data_.size(); ++i) {
        if (data_[i] == 0) break;  // Stop at null terminator
        serial += static_cast<char>(data_[i]);
    }
    return serial;
}

void CRSFDeviceInfo::setSerialNumber(const std::string& serial) {
    // Truncate or pad to exactly 4 bytes
    std::string paddedSerial = serial.substr(0, SERIAL_NUMBER_LENGTH);
    while (paddedSerial.length() < SERIAL_NUMBER_LENGTH) {
        paddedSerial += '\0';  // Pad with nulls
    }
    
    // Write to payload starting at byte 19
    for (size_t i = 0; i < SERIAL_NUMBER_LENGTH; ++i) {
        setByte(19 + i, static_cast<uint8_t>(paddedSerial[i]));
    }
}

bool CRSFDeviceInfo::isELRSDevice() const {
    return getSerialNumber() == SERIAL_ELRS;
}

// ============================================================================
// Version Methods
// ============================================================================

uint32_t CRSFDeviceInfo::getHardwareVersion() const {
    // Hardware version starts at byte 23 (19 + 4 for serial)
    return getU32(23);
}

void CRSFDeviceInfo::setHardwareVersion(uint32_t version) {
    setU32(23, version);
}

uint32_t CRSFDeviceInfo::getSoftwareVersion() const {
    // Software version starts at byte 27 (23 + 4 for hardware)
    return getU32(27);
}

void CRSFDeviceInfo::setSoftwareVersion(uint32_t version) {
    setU32(27, version);
}

uint8_t CRSFDeviceInfo::getSoftwareVersionMajor() const {
    // Extract major version from 32-bit software version (bits 24-31)
    uint32_t swVersion = getSoftwareVersion();
    return static_cast<uint8_t>((swVersion >> 24) & 0xFF);
}

uint8_t CRSFDeviceInfo::getSoftwareVersionMinor() const {
    // Extract minor version from 32-bit software version (bits 16-23)
    uint32_t swVersion = getSoftwareVersion();
    return static_cast<uint8_t>((swVersion >> 16) & 0xFF);
}

uint8_t CRSFDeviceInfo::getSoftwareVersionRevision() const {
    // Extract revision from 32-bit software version (bits 8-15)
    uint32_t swVersion = getSoftwareVersion();
    return static_cast<uint8_t>((swVersion >> 8) & 0xFF);
}

void CRSFDeviceInfo::setSoftwareVersion(uint8_t major, uint8_t minor, uint8_t revision) {
    // Pack major, minor, revision into 32-bit software version
    uint32_t swVersion = (static_cast<uint32_t>(major) << 24) | 
                         (static_cast<uint32_t>(minor) << 16) | 
                         (static_cast<uint32_t>(revision) << 8);
    setSoftwareVersion(swVersion);
}

// ============================================================================
// Configuration Methods
// ============================================================================

uint8_t CRSFDeviceInfo::getConfigParameterCount() const {
    // Config parameter count is at byte 31
    return getByte(31);
}

void CRSFDeviceInfo::setConfigParameterCount(uint8_t count) {
    setByte(31, count);
}

uint8_t CRSFDeviceInfo::getParameterProtocolVersion() const {
    // Parameter protocol version is at byte 32
    return getByte(32);
}

void CRSFDeviceInfo::setParameterProtocolVersion(uint8_t version) {
    setByte(32, version);
}

// ============================================================================
// Utility Methods
// ============================================================================

bool CRSFDeviceInfo::hasConfigParameters() const {
    return getConfigParameterCount() > 0;
}

bool CRSFDeviceInfo::isExpressLRSDevice() const {
    return isELRSDevice() && getDisplayNameLength() <= MAX_DISPLAY_NAME_LENGTH;
}

bool CRSFDeviceInfo::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::DeviceInfo)) return false;
    
    // Check minimum packet size
    if (data_.size() < 35) return false;  // header(3) + payload(30) + crc(1) + extended(2)
    
    // Check length field
    if (getLength() != (PAYLOAD_SIZE + 1 + 1)) return false;  // payload + type + crc
    
    // Check display name length
    if (getDisplayNameLength() > MAX_DISPLAY_NAME_LENGTH) return false;
    
    // Check serial number length
    if (getSerialNumber().length() != SERIAL_NUMBER_LENGTH) return false;
    
    return true;
}

// ============================================================================
// CRSFParameterSettingsEntry Implementation
// ============================================================================

CRSFParameterSettingsEntry::CRSFParameterSettingsEntry(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::ParameterSettingsEntry)) {
        setType(static_cast<uint8_t>(PacketType::ParameterSettingsEntry));
    }
}

// ============================================================================
// Basic Field Methods
// ============================================================================

uint8_t CRSFParameterSettingsEntry::getFieldIndex() const {
    return getByte(3);
}

void CRSFParameterSettingsEntry::setFieldIndex(uint8_t index) {
    setByte(3, index);
}

uint8_t CRSFParameterSettingsEntry::getChunksRemaining() const {
    return getByte(4);
}

void CRSFParameterSettingsEntry::setChunksRemaining(uint8_t chunks) {
    setByte(4, chunks);
}

uint8_t CRSFParameterSettingsEntry::getParent() const {
    return getByte(5);
}

void CRSFParameterSettingsEntry::setParent(uint8_t parent) {
    setByte(5, parent);
}

uint8_t CRSFParameterSettingsEntry::getParameterType() const {
    return getByte(6) & TYPE_MASK;  // Clear hidden flag
}

void CRSFParameterSettingsEntry::setParameterType(uint8_t type) {
    uint8_t current = getByte(6);
    uint8_t hidden = current & HIDDEN_FLAG;
    setByte(6, (type & TYPE_MASK) | hidden);
}

bool CRSFParameterSettingsEntry::isHidden() const {
    return (getByte(6) & HIDDEN_FLAG) != 0;
}

void CRSFParameterSettingsEntry::setHidden(bool hidden) {
    uint8_t current = getByte(6);
    uint8_t type = current & TYPE_MASK;
    setByte(6, type | (hidden ? HIDDEN_FLAG : 0));
}

// ============================================================================
// Label Methods
// ============================================================================

std::string CRSFParameterSettingsEntry::getLabel() const {
    // Label starts at byte 7
    std::string label;
    for (size_t i = 7; i < data_.size(); ++i) {
        if (data_[i] == 0) break;  // Stop at null terminator
        label += static_cast<char>(data_[i]);
    }
    return label;
}

void CRSFParameterSettingsEntry::setLabel(const std::string& label) {
    // Write label starting at byte 7
    for (size_t i = 0; i < label.length() && (7 + i) < data_.size(); ++i) {
        setByte(7 + i, static_cast<uint8_t>(label[i]));
    }
    // Add null terminator
    if (7 + label.length() < data_.size()) {
        setByte(7 + label.length(), 0);
    }
}

size_t CRSFParameterSettingsEntry::getLabelLength() const {
    return getLabel().length();
}

// ============================================================================
// Type-specific Value Methods
// ============================================================================

// Integer types (0x00-0x07) - template implementation
template<typename T>
T CRSFParameterSettingsEntry::getCurrentValue() const {
    // Find the start of value section after label
    size_t labelEnd = 7 + getLabelLength() + 1;  // +1 for null terminator
    if (labelEnd >= data_.size()) return 0;
    
    // Read value based on type size
    if constexpr (sizeof(T) == 1) {
        return static_cast<T>(getByte(labelEnd));
    } else if constexpr (sizeof(T) == 2) {
        return static_cast<T>(getU16(labelEnd));
    } else if constexpr (sizeof(T) == 4) {
        return static_cast<T>(getU32(labelEnd));
    } else if constexpr (sizeof(T) == 8) {
        // For 64-bit, read as two 32-bit values
        uint32_t high = getU32(labelEnd);
        uint32_t low = getU32(labelEnd + 4);
        return static_cast<T>((static_cast<uint64_t>(high) << 32) | low);
    }
    return 0;
}

template<typename T>
T CRSFParameterSettingsEntry::getMinValue() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    size_t typeSize = getTypeSize();
    if (labelEnd + typeSize >= data_.size()) return 0;
    
    if constexpr (sizeof(T) == 1) {
        return static_cast<T>(getByte(labelEnd + typeSize));
    } else if constexpr (sizeof(T) == 2) {
        return static_cast<T>(getU16(labelEnd + typeSize));
    } else if constexpr (sizeof(T) == 4) {
        return static_cast<T>(getU32(labelEnd + typeSize));
    } else if constexpr (sizeof(T) == 8) {
        uint32_t high = getU32(labelEnd + typeSize);
        uint32_t low = getU32(labelEnd + typeSize + 4);
        return static_cast<T>((static_cast<uint64_t>(high) << 32) | low);
    }
    return 0;
}

template<typename T>
T CRSFParameterSettingsEntry::getMaxValue() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    size_t typeSize = getTypeSize();
    if (labelEnd + typeSize * 2 >= data_.size()) return 0;
    
    if constexpr (sizeof(T) == 1) {
        return static_cast<T>(getByte(labelEnd + typeSize * 2));
    } else if constexpr (sizeof(T) == 2) {
        return static_cast<T>(getU16(labelEnd + typeSize * 2));
    } else if constexpr (sizeof(T) == 4) {
        return static_cast<T>(getU32(labelEnd + typeSize * 2));
    } else if constexpr (sizeof(T) == 8) {
        uint32_t high = getU32(labelEnd + typeSize * 2);
        uint32_t low = getU32(labelEnd + typeSize * 2 + 4);
        return static_cast<T>((static_cast<uint64_t>(high) << 32) | low);
    }
    return 0;
}

template<typename T>
T CRSFParameterSettingsEntry::getDefaultValue() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    size_t typeSize = getTypeSize();
    if (labelEnd + typeSize * 3 >= data_.size()) return 0;
    
    if constexpr (sizeof(T) == 1) {
        return static_cast<T>(getByte(labelEnd + typeSize * 3));
    } else if constexpr (sizeof(T) == 2) {
        return static_cast<T>(getU16(labelEnd + typeSize * 3));
    } else if constexpr (sizeof(T) == 4) {
        return static_cast<T>(getU32(labelEnd + typeSize * 3));
    } else if constexpr (sizeof(T) == 8) {
        uint32_t high = getU32(labelEnd + typeSize * 3);
        uint32_t low = getU32(labelEnd + typeSize * 3 + 4);
        return static_cast<T>((static_cast<uint64_t>(high) << 32) | low);
    }
    return 0;
}

std::string CRSFParameterSettingsEntry::getUnits() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    size_t typeSize = getTypeSize();
    size_t unitsStart = labelEnd + typeSize * 4;
    
    std::string units;
    for (size_t i = unitsStart; i < data_.size(); ++i) {
        if (data_[i] == 0) break;
        units += static_cast<char>(data_[i]);
    }
    return units;
}

// Float type (0x08)
float CRSFParameterSettingsEntry::getFloatCurrentValue() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    if (labelEnd + 16 >= data_.size()) return 0.0f;
    
    int32_t rawValue = getU32(labelEnd);
    return static_cast<float>(rawValue) / 1000.0f;  // Assuming 3 decimal places
}

float CRSFParameterSettingsEntry::getFloatMinValue() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    if (labelEnd + 20 >= data_.size()) return 0.0f;
    
    int32_t rawValue = getU32(labelEnd + 4);
    return static_cast<float>(rawValue) / 1000.0f;
}

float CRSFParameterSettingsEntry::getFloatMaxValue() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    if (labelEnd + 24 >= data_.size()) return 0.0f;
    
    int32_t rawValue = getU32(labelEnd + 8);
    return static_cast<float>(rawValue) / 1000.0f;
}

uint8_t CRSFParameterSettingsEntry::getFloatPrecision() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    if (labelEnd + 28 >= data_.size()) return 0;
    
    return getByte(labelEnd + 12);
}

uint32_t CRSFParameterSettingsEntry::getFloatStep() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    if (labelEnd + 32 >= data_.size()) return 0;
    
    return getU32(labelEnd + 13);
}

// Select type (0x09)
uint8_t CRSFParameterSettingsEntry::getSelectCurrentIndex() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    if (labelEnd >= data_.size()) return 0;
    
    return getByte(labelEnd);
}

std::vector<std::string> CRSFParameterSettingsEntry::getSelectOptions() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    if (labelEnd + 4 >= data_.size()) return {};
    
    // Skip current, min, max, default values
    size_t optionsStart = labelEnd + 4;
    
    std::vector<std::string> options;
    std::string currentOption;
    
    for (size_t i = optionsStart; i < data_.size(); ++i) {
        if (data_[i] == ';') {
            if (!currentOption.empty()) {
                options.push_back(currentOption);
                currentOption.clear();
            }
        } else if (data_[i] == 0) {
            if (!currentOption.empty()) {
                options.push_back(currentOption);
            }
            break;
        } else {
            currentOption += static_cast<char>(data_[i]);
        }
    }
    
    return options;
}

std::string CRSFParameterSettingsEntry::getSelectCurrentOption() const {
    std::vector<std::string> options = getSelectOptions();
    uint8_t currentIndex = getSelectCurrentIndex();
    
    if (currentIndex < options.size()) {
        return options[currentIndex];
    }
    return "";
}

// String type (0x0A)
std::string CRSFParameterSettingsEntry::getStringValue() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    if (labelEnd >= data_.size()) return "";
    
    std::string value;
    for (size_t i = labelEnd; i < data_.size(); ++i) {
        if (data_[i] == 0) break;
        value += static_cast<char>(data_[i]);
    }
    return value;
}

uint8_t CRSFParameterSettingsEntry::getStringMaxLength() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    size_t valueEnd = labelEnd;
    
    // Find end of string value
    for (size_t i = labelEnd; i < data_.size(); ++i) {
        if (data_[i] == 0) {
            valueEnd = i;
            break;
        }
    }
    
    if (valueEnd + 1 < data_.size()) {
        return getByte(valueEnd + 1);
    }
    return 0;
}

// Folder type (0x0B)
std::string CRSFParameterSettingsEntry::getFolderDisplayName() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    if (labelEnd >= data_.size()) return "";
    
    std::string displayName;
    for (size_t i = labelEnd; i < data_.size(); ++i) {
        if (data_[i] == 0) break;
        displayName += static_cast<char>(data_[i]);
    }
    return displayName;
}

// Info type (0x0C)
std::string CRSFParameterSettingsEntry::getInfoDisplay() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    if (labelEnd >= data_.size()) return "";
    
    std::string display;
    for (size_t i = labelEnd; i < data_.size(); ++i) {
        if (data_[i] == 0) break;
        display += static_cast<char>(data_[i]);
    }
    return display;
}

// Command type (0x0D)
uint8_t CRSFParameterSettingsEntry::getCommandStep() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    if (labelEnd >= data_.size()) return 0;
    
    return getByte(labelEnd);
}

uint8_t CRSFParameterSettingsEntry::getCommandTimeout() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    if (labelEnd + 1 >= data_.size()) return 0;
    
    return getByte(labelEnd + 1);
}

std::string CRSFParameterSettingsEntry::getCommandInfo() const {
    size_t labelEnd = 7 + getLabelLength() + 1;
    if (labelEnd + 2 >= data_.size()) return "";
    
    std::string info;
    for (size_t i = labelEnd + 2; i < data_.size(); ++i) {
        if (data_[i] == 0) break;
        info += static_cast<char>(data_[i]);
    }
    return info;
}

// ============================================================================
// Utility Methods
// ============================================================================

bool CRSFParameterSettingsEntry::isTopLevel() const {
    return getParent() == 0;
}

bool CRSFParameterSettingsEntry::hasParent() const {
    return getParent() != 0;
}

bool CRSFParameterSettingsEntry::isIntegerType() const {
    uint8_t type = getParameterType();
    return type >= TYPE_UINT8 && type <= TYPE_INT64;
}

bool CRSFParameterSettingsEntry::isFloatType() const {
    return getParameterType() == TYPE_FLOAT;
}

bool CRSFParameterSettingsEntry::isSelectType() const {
    return getParameterType() == TYPE_SELECT;
}

bool CRSFParameterSettingsEntry::isStringType() const {
    return getParameterType() == TYPE_STRING;
}

bool CRSFParameterSettingsEntry::isFolderType() const {
    return getParameterType() == TYPE_FOLDER;
}

bool CRSFParameterSettingsEntry::isInfoType() const {
    return getParameterType() == TYPE_INFO;
}

bool CRSFParameterSettingsEntry::isCommandType() const {
    return getParameterType() == TYPE_COMMAND;
}

bool CRSFParameterSettingsEntry::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::ParameterSettingsEntry)) return false;
    
    // Check minimum packet size
    if (data_.size() < 8) return false;  // header(3) + field_index + chunks + parent + type + label_min(1)
    
    // Check label length
    if (getLabelLength() == 0) return false;
    
    return true;
}

// Helper method to get type size
size_t CRSFParameterSettingsEntry::getTypeSize() const {
    uint8_t type = getParameterType();
    switch (type) {
        case TYPE_UINT8:
        case TYPE_INT8:
            return 1;
        case TYPE_UINT16:
        case TYPE_INT16:
            return 2;
        case TYPE_UINT32:
        case TYPE_INT32:
            return 4;
        case TYPE_UINT64:
        case TYPE_INT64:
            return 8;
        default:
            return 0;
    }
}

// ============================================================================
// CRSFParameterRead Implementation
// ============================================================================

CRSFParameterRead::CRSFParameterRead(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::ParameterRead)) {
        setType(static_cast<uint8_t>(PacketType::ParameterRead));
    }
}

uint8_t CRSFParameterRead::getFieldIndex() const {
    return getByte(3);
}

void CRSFParameterRead::setFieldIndex(uint8_t index) {
    setByte(3, index);
}

uint8_t CRSFParameterRead::getChunkIndex() const {
    return getByte(4);
}

void CRSFParameterRead::setChunkIndex(uint8_t chunk) {
    setByte(4, chunk);
}

bool CRSFParameterRead::isFolderRequest() const {
    return getFieldIndex() == FIELD_INDEX_FOLDER;
}

bool CRSFParameterRead::isParameterRequest() const {
    return getFieldIndex() >= FIELD_INDEX_FIRST_PARAMETER;
}

bool CRSFParameterRead::isTopLevelRequest() const {
    return getFieldIndex() == FIELD_INDEX_FOLDER;
}

bool CRSFParameterRead::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::ParameterRead)) return false;
    
    // Check minimum packet size
    if (data_.size() < 6) return false;  // header(3) + field_index + chunk_index + crc(1)
    
    // Check length field
    if (getLength() != (PAYLOAD_SIZE + 1 + 1)) return false;  // payload + type + crc
    
    return true;
}

// ============================================================================
// CRSFParameterWrite Implementation
// ============================================================================

CRSFParameterWrite::CRSFParameterWrite(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::ParameterWrite)) {
        setType(static_cast<uint8_t>(PacketType::ParameterWrite));
    }
}

uint8_t CRSFParameterWrite::getFieldIndex() const {
    return getByte(3);
}

void CRSFParameterWrite::setFieldIndex(uint8_t index) {
    setByte(3, index);
}

uint8_t CRSFParameterWrite::getUint8Value() const {
    if (data_.size() < 5) return 0;  // header(3) + field_index + value + crc
    return getByte(4);
}

void CRSFParameterWrite::setUint8Value(uint8_t value) {
    if (data_.size() < 5) return;
    setByte(4, value);
    setLength(2);  // field_index + value
    calculateCRC();
}

uint16_t CRSFParameterWrite::getUint16Value() const {
    if (data_.size() < 6) return 0;  // header(3) + field_index + value(2) + crc
    return getU16(4);
}

void CRSFParameterWrite::setUint16Value(uint16_t value) {
    if (data_.size() < 6) return;
    setU16(4, value);
    setLength(3);  // field_index + value(2)
    calculateCRC();
}

uint32_t CRSFParameterWrite::getUint32Value() const {
    if (data_.size() < 8) return 0;  // header(3) + field_index + value(4) + crc
    return getU32(4);
}

void CRSFParameterWrite::setUint32Value(uint32_t value) {
    if (data_.size() < 8) return;
    setU32(4, value);
    setLength(5);  // field_index + value(4)
    calculateCRC();
}

std::string CRSFParameterWrite::getStringValue() const {
    if (data_.size() < 5) return "";
    
    std::string value;
    for (size_t i = 4; i < data_.size() - 1; ++i) {  // -1 for CRC
        if (data_[i] == 0) break;  // Stop at null terminator
        value += static_cast<char>(data_[i]);
    }
    return value;
}

void CRSFParameterWrite::setStringValue(const std::string& value) {
    size_t requiredSize = 3 + 1 + value.length() + 1;  // header + field_index + value + null + crc
    if (data_.size() < requiredSize) return;
    
    // Write string value starting at byte 4
    for (size_t i = 0; i < value.length(); ++i) {
        setByte(4 + i, static_cast<uint8_t>(value[i]));
    }
    setByte(4 + value.length(), 0);  // null terminator
    
    setLength(static_cast<uint8_t>(value.length() + 1));  // field_index + value + null
    calculateCRC();
}

size_t CRSFParameterWrite::getValueSize() const {
    return getLength() - 1;  // length - field_index
}

bool CRSFParameterWrite::isCommand() const {
    // Check if this looks like a command (typically small integer values)
    size_t valueSize = getValueSize();
    return valueSize == 1 && getUint8Value() <= 10;  // Command steps are usually 0-10
}

bool CRSFParameterWrite::isIntegerValue() const {
    size_t valueSize = getValueSize();
    return valueSize == 1 || valueSize == 2 || valueSize == 4;
}

bool CRSFParameterWrite::isStringValue() const {
    size_t valueSize = getValueSize();
    return valueSize > 4;  // String values are typically longer
}

bool CRSFParameterWrite::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::ParameterWrite)) return false;
    
    // Check minimum packet size
    if (data_.size() < 6) return false;  // header(3) + field_index + value_min(1) + crc(1)
    
    // Check length field
    if (getLength() < 1) return false;  // Must have at least field_index
    
    return true;
}

// ============================================================================
// CRSFELRSStatus Implementation
// ============================================================================

CRSFELRSStatus::CRSFELRSStatus(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::ELRSStatus)) {
        setType(static_cast<uint8_t>(PacketType::ELRSStatus));
    }
}

uint8_t CRSFELRSStatus::getPacketsBad() const {
    return getByte(3);
}

void CRSFELRSStatus::setPacketsBad(uint8_t count) {
    setByte(3, count);
}

uint16_t CRSFELRSStatus::getPacketsGood() const {
    return getU16(4);
}

void CRSFELRSStatus::setPacketsGood(uint16_t count) {
    setU16(4, count);
}

uint8_t CRSFELRSStatus::getFlags() const {
    return getByte(6);
}

void CRSFELRSStatus::setFlags(uint8_t flags) {
    setByte(6, flags);
}

bool CRSFELRSStatus::isConnected() const {
    return (getFlags() & FLAG_CONNECTED) != 0;
}

void CRSFELRSStatus::setConnected(bool connected) {
    uint8_t flags = getFlags();
    if (connected) {
        flags |= FLAG_CONNECTED;
    } else {
        flags &= ~FLAG_CONNECTED;
    }
    setFlags(flags);
}

bool CRSFELRSStatus::hasModelMismatch() const {
    return (getFlags() & FLAG_MODEL_MISMATCH) != 0;
}

void CRSFELRSStatus::setModelMismatch(bool mismatch) {
    uint8_t flags = getFlags();
    if (mismatch) {
        flags |= FLAG_MODEL_MISMATCH;
    } else {
        flags &= ~FLAG_MODEL_MISMATCH;
    }
    setFlags(flags);
}

bool CRSFELRSStatus::isArmed() const {
    return (getFlags() & FLAG_ARMED) != 0;
}

void CRSFELRSStatus::setArmed(bool armed) {
    uint8_t flags = getFlags();
    if (armed) {
        flags |= FLAG_ARMED;
    } else {
        flags &= ~FLAG_ARMED;
    }
    setFlags(flags);
}

bool CRSFELRSStatus::hasBaudRateTooLow() const {
    return (getFlags() & FLAG_BAUD_RATE_TOO_LOW) != 0;
}

void CRSFELRSStatus::setBaudRateTooLow(bool tooLow) {
    uint8_t flags = getFlags();
    if (tooLow) {
        flags |= FLAG_BAUD_RATE_TOO_LOW;
    } else {
        flags &= ~FLAG_BAUD_RATE_TOO_LOW;
    }
    setFlags(flags);
}

std::string CRSFELRSStatus::getMessage() const {
    if (data_.size() < 8) return "";  // header(3) + packets_bad + packets_good(2) + flags + message_start
    
    std::string message;
    for (size_t i = 7; i < data_.size() - 1; ++i) {  // -1 for CRC
        if (data_[i] == 0) break;  // Stop at null terminator
        message += static_cast<char>(data_[i]);
    }
    return message;
}

void CRSFELRSStatus::setMessage(const std::string& message) {
    size_t requiredSize = 3 + 4 + message.length() + 1;  // header + payload + message + null + crc
    if (data_.size() < requiredSize) return;
    
    // Write message starting at byte 7 (after payload)
    for (size_t i = 0; i < message.length(); ++i) {
        setByte(7 + i, static_cast<uint8_t>(message[i]));
    }
    setByte(7 + message.length(), 0);  // null terminator
    
    // Update length and recalculate CRC
    setLength(static_cast<uint8_t>(4 + message.length() + 1));  // payload + message + null
    calculateCRC();
}

double CRSFELRSStatus::getPacketSuccessRate() const {
    uint16_t good = getPacketsGood();
    uint8_t bad = getPacketsBad();
    uint32_t total = good + bad;
    
    if (total == 0) return 0.0;
    return static_cast<double>(good) / static_cast<double>(total) * 100.0;
}

bool CRSFELRSStatus::hasWarnings() const {
    uint8_t flags = getFlags();
    return (flags & (FLAG_MODEL_MISMATCH | FLAG_BAUD_RATE_TOO_LOW)) != 0;
}

bool CRSFELRSStatus::hasErrors() const {
    uint8_t flags = getFlags();
    return (flags & FLAG_MODEL_MISMATCH) != 0;  // Model mismatch is considered an error
}

bool CRSFELRSStatus::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::ELRSStatus)) return false;
    
    // Check minimum packet size
    if (data_.size() < 8) return false;  // header(3) + payload(4) + crc(1)
    
    // Check length field
    if (getLength() < MIN_PAYLOAD_SIZE) return false;
    
    return true;
}

// ============================================================================
// CRSFCommand Implementation
// ============================================================================

CRSFCommand::CRSFCommand(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::Command)) {
        setType(static_cast<uint8_t>(PacketType::Command));
    }
}

uint8_t CRSFCommand::getCommandRealm() const {
    return getByte(3);
}

void CRSFCommand::setCommandRealm(uint8_t realm) {
    setByte(3, realm);
}

uint8_t CRSFCommand::getSubcommand() const {
    return getByte(4);
}

void CRSFCommand::setSubcommand(uint8_t subcommand) {
    setByte(4, subcommand);
}

etl::span<uint8_t> CRSFCommand::getCommandPayload() const {
    if (data_.size() < 6) return etl::span<uint8_t>();  // header(3) + realm + subcommand + min_payload
    
    size_t payloadStart = 5;
    size_t payloadEnd = data_.size() - 2;  // -2 for CRC and CRC8-BA
    
    if (payloadEnd <= payloadStart) return etl::span<uint8_t>();
    
    return etl::span<uint8_t>(data_.data() + payloadStart, payloadEnd - payloadStart);
}

void CRSFCommand::setCommandPayload(const etl::span<uint8_t>& payload) {
    if (data_.size() < 5 + payload.size() + 2) return;  // header + realm + subcommand + payload + crc + crc8-ba
    
    // Copy payload starting at byte 5 (after realm and subcommand)
    for (size_t i = 0; i < payload.size(); ++i) {
        setByte(5 + i, payload[i]);
    }
    
    // Update length and recalculate CRCs
    setLength(static_cast<uint8_t>(2 + payload.size() + 1));  // realm + subcommand + payload + crc8-ba
    calculateCRC();
    calculateCRC8BA();
}

uint8_t CRSFCommand::getCRC8BA() const {
    if (data_.size() < 6) return 0;  // header(3) + realm + subcommand + min_payload + crc8-ba
    
    return data_[data_.size() - 1];  // Last byte is CRC8-BA
}

void CRSFCommand::setCRC8BA(uint8_t crc) {
    if (data_.size() < 6) return;
    
    data_[data_.size() - 1] = crc;
}

bool CRSFCommand::verifyCRC8BA() const {
    if (data_.size() < 6) return false;
    
    // Calculate CRC8-BA from type (byte 2) to end of payload (excluding CRC8-BA)
    uint8_t calculatedCRC = calculateCRC8BAFromData(data_.data() + 2, data_.size() - 3);
    return calculatedCRC == getCRC8BA();
}

void CRSFCommand::calculateCRC8BA() {
    if (data_.size() < 6) return;
    
    // Calculate CRC8-BA from type (byte 2) to end of payload (excluding CRC8-BA)
    uint8_t crc = calculateCRC8BAFromData(data_.data() + 2, data_.size() - 3);
    setCRC8BA(crc);
}

// Helper method for CRC8-BA calculation
uint8_t CRSFCommand::calculateCRC8BAFromData(const uint8_t* data, size_t length) const {
    uint8_t crc = 0;
    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ CRC8_BA_POLY;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

bool CRSFCommand::isReceiverCommand() const {
    return getCommandRealm() == REALM_RX;
}

bool CRSFCommand::isGeneralCommand() const {
    return getCommandRealm() == REALM_GENERAL;
}

bool CRSFCommand::isBindCommand() const {
    return isReceiverCommand() && getSubcommand() == SUBCMD_RX_BIND;
}

bool CRSFCommand::isModelSelectCommand() const {
    return isReceiverCommand() && getSubcommand() == SUBCMD_MODEL_SELECT_ID;
}

bool CRSFCommand::isSpeedProposalCommand() const {
    return isGeneralCommand() && getSubcommand() == SUBCMD_GENERAL_CRSF_SPEED_PROPOSAL;
}

bool CRSFCommand::isSpeedResponseCommand() const {
    return isGeneralCommand() && getSubcommand() == SUBCMD_GENERAL_CRSF_SPEED_RESPONSE;
}

uint8_t CRSFCommand::getReceiverID() const {
    if (!isModelSelectCommand() || data_.size() < 6) return 0;
    return getByte(5);
}

void CRSFCommand::setReceiverID(uint8_t id) {
    if (!isModelSelectCommand() || data_.size() < 6) return;
    setByte(5, id);
    calculateCRC8BA();
}

uint8_t CRSFCommand::getPortID() const {
    if ((!isSpeedProposalCommand() && !isSpeedResponseCommand()) || data_.size() < 6) return 0;
    return getByte(5);
}

void CRSFCommand::setPortID(uint8_t portId) {
    if ((!isSpeedProposalCommand() && !isSpeedResponseCommand()) || data_.size() < 6) return;
    setByte(5, portId);
    calculateCRC8BA();
}

uint32_t CRSFCommand::getProposedBaudRate() const {
    if (!isSpeedProposalCommand() || data_.size() < 10) return 0;
    return getU32(6);
}

void CRSFCommand::setProposedBaudRate(uint32_t baudRate) {
    if (!isSpeedProposalCommand() || data_.size() < 10) return;
    setU32(6, baudRate);
    calculateCRC8BA();
}

bool CRSFCommand::isSpeedAccepted() const {
    if (!isSpeedResponseCommand() || data_.size() < 7) return false;
    return getByte(6) != 0;
}

void CRSFCommand::setSpeedAccepted(bool accepted) {
    if (!isSpeedResponseCommand() || data_.size() < 7) return;
    setByte(6, accepted ? 1 : 0);
    calculateCRC8BA();
}

bool CRSFCommand::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::Command)) return false;
    
    // Check minimum packet size
    if (data_.size() < 7) return false;  // header(3) + realm + subcommand + min_payload + crc + crc8-ba
    
    // Check length field
    if (getLength() < MIN_PAYLOAD_SIZE) return false;
    
    return true;
}

// ============================================================================
// CRSFRadioID Implementation
// ============================================================================

CRSFRadioID::CRSFRadioID(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::RadioID)) {
        setType(static_cast<uint8_t>(PacketType::RadioID));
    }
}

uint8_t CRSFRadioID::getSubtype() const {
    return getByte(3);
}

void CRSFRadioID::setSubtype(uint8_t subtype) {
    setByte(3, subtype);
}

etl::span<uint8_t> CRSFRadioID::getSubtypeData() const {
    if (data_.size() < 5) return etl::span<uint8_t>();  // header(3) + subtype + min_data
    
    size_t dataStart = 4;
    size_t dataEnd = data_.size() - 1;  // -1 for CRC
    
    if (dataEnd <= dataStart) return etl::span<uint8_t>();
    
    return etl::span<uint8_t>(data_.data() + dataStart, dataEnd - dataStart);
}

void CRSFRadioID::setSubtypeData(const etl::span<uint8_t>& subtypeData) {
    if (data_.size() < 4 + subtypeData.size() + 1) return;  // header + subtype + data + crc
    
    // Copy subtype data starting at byte 4 (after subtype)
    for (size_t i = 0; i < subtypeData.size(); ++i) {
        setByte(4 + i, subtypeData[i]);
    }
    
    // Update length and recalculate CRC
    setLength(static_cast<uint8_t>(1 + subtypeData.size()));  // subtype + data
    calculateCRC();
}

bool CRSFRadioID::isOpenTXSync() const {
    return getSubtype() == SUBTYPE_OPENTX_SYNC;
}

bool CRSFRadioID::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::RadioID)) return false;
    
    // Check minimum packet size
    if (data_.size() < 5) return false;  // header(3) + subtype + min_data + crc
    
    // Check length field
    if (getLength() < MIN_PAYLOAD_SIZE) return false;
    
    return true;
}

// ============================================================================
// CRSFOpenTXSync Implementation
// ============================================================================

CRSFOpenTXSync::CRSFOpenTXSync(const etl::span<uint8_t>& data) : CRSFRadioID(data) {
    if (getSubtype() != SUBTYPE_OPENTX_SYNC) {
        setSubtype(SUBTYPE_OPENTX_SYNC);
    }
}

int32_t CRSFOpenTXSync::getPacketInterval() const {
    return getU32(4);
}

void CRSFOpenTXSync::setPacketInterval(int32_t interval) {
    setU32(4, interval);
}

int32_t CRSFOpenTXSync::getPhaseShiftCorrection() const {
    return getU32(8);
}

void CRSFOpenTXSync::setPhaseShiftCorrection(int32_t correction) {
    setU32(8, correction);
}

double CRSFOpenTXSync::getPacketIntervalHz() const {
    int32_t interval = getPacketInterval();
    if (interval <= 0) return 0.0;
    return 10000000.0 / static_cast<double>(interval);  // Convert from μs×10 to Hz
}

void CRSFOpenTXSync::setPacketIntervalHz(double hz) {
    if (hz <= 0.0) {
        setPacketInterval(0);
    } else {
        int32_t interval = static_cast<int32_t>(std::round(10000000.0 / hz));  // Convert from Hz to μs×10
        setPacketInterval(interval);
    }
}

bool CRSFOpenTXSync::isValid() const {
    if (!CRSFRadioID::isValid()) return false;
    
    // Check minimum packet size for OpenTX Sync
    if (data_.size() < 12) return false;  // header(3) + subtype + payload(8) + crc
    
    // Check length field
    if (getLength() < PAYLOAD_SIZE) return false;
    
    return true;
}

// ============================================================================
// CRSFMSPReq Implementation
// ============================================================================

CRSFMSPReq::CRSFMSPReq(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::MSPReq)) {
        setType(static_cast<uint8_t>(PacketType::MSPReq));
    }
}

uint16_t CRSFMSPReq::getMSPFlags() const {
    return getU16(3);
}

void CRSFMSPReq::setMSPFlags(uint16_t flags) {
    setU16(3, flags);
}

uint16_t CRSFMSPReq::getFunction() const {
    return getU16(5);
}

void CRSFMSPReq::setFunction(uint16_t function) {
    setU16(5, function);
}

uint16_t CRSFMSPReq::getPayloadLength() const {
    return getU16(7);
}

void CRSFMSPReq::setPayloadLength(uint16_t length) {
    setU16(7, length);
}

etl::span<uint8_t> CRSFMSPReq::getMSPPayload() const {
    if (data_.size() < 10) return etl::span<uint8_t>();  // header(3) + flags(2) + function(2) + length(2) + min_payload
    
    size_t payloadStart = 9;
    size_t payloadEnd = data_.size() - 1;  // -1 for CRC
    
    if (payloadEnd <= payloadStart) return etl::span<uint8_t>();
    
    return etl::span<uint8_t>(data_.data() + payloadStart, payloadEnd - payloadStart);
}

void CRSFMSPReq::setMSPPayload(const etl::span<uint8_t>& payload) {
    if (data_.size() < 9 + payload.size() + 1) return;  // header + flags + function + length + payload + crc
    
    // Copy payload starting at byte 9 (after flags, function, and length)
    for (size_t i = 0; i < payload.size(); ++i) {
        setByte(9 + i, payload[i]);
    }
    
    // Update payload length and recalculate CRC
    setPayloadLength(static_cast<uint16_t>(payload.size()));
    calculateCRC();
}

bool CRSFMSPReq::hasError() const {
    return (getMSPFlags() & FLAG_ERROR_MASK) != 0;
}

uint8_t CRSFMSPReq::getVersion() const {
    return static_cast<uint8_t>((getMSPFlags() & FLAG_VERSION_MASK) >> 8);
}

bool CRSFMSPReq::isBeginningOfFrame() const {
    return (getMSPFlags() & FLAG_BEGINNING_OF_FRAME) != 0;
}

bool CRSFMSPReq::isFirstFrame() const {
    return (getMSPFlags() & FLAG_FIRST_FRAME) != 0;
}

bool CRSFMSPReq::isLastFrame() const {
    return (getMSPFlags() & FLAG_LAST_FRAME) != 0;
}

bool CRSFMSPReq::isValid() const {
    if (getType() != static_cast<uint8_t>(PacketType::MSPReq)) return false;
    
    // Check minimum packet size
    if (data_.size() < 10) return false;  // header(3) + flags(2) + function(2) + length(2) + min_payload + crc
    
    // Check length field - should be at least 6 (flags + function + length)
    if (getLength() < MIN_PAYLOAD_SIZE) return false;
    
    // Check payload length consistency
    if (getPayloadLength() != getMSPPayload().size()) return false;
    
    return true;
}



CRSFLinkStatistics::CRSFLinkStatistics(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::LinkStatistics)) {
        setType(static_cast<uint8_t>(PacketType::LinkStatistics));
    }
}

uint8_t CRSFLinkStatistics::getUplinkRSSI1() const { return getByte(3); }
void CRSFLinkStatistics::setUplinkRSSI1(uint8_t value) { setByte(3, value); }

uint8_t CRSFLinkStatistics::getUplinkRSSI2() const { return getByte(4); }
void CRSFLinkStatistics::setUplinkRSSI2(uint8_t value) { setByte(4, value); }

uint8_t CRSFLinkStatistics::getUplinkLinkQuality() const { return getByte(5); }
void CRSFLinkStatistics::setUplinkLinkQuality(uint8_t value) { setByte(5, value); }

int8_t CRSFLinkStatistics::getUplinkSNR() const { return static_cast<int8_t>(getByte(6)); }
void CRSFLinkStatistics::setUplinkSNR(int8_t value) { setByte(6, static_cast<uint8_t>(value)); }

uint8_t CRSFLinkStatistics::getActiveAntenna() const { return getByte(7); }
void CRSFLinkStatistics::setActiveAntenna(uint8_t value) { setByte(7, value); }

uint8_t CRSFLinkStatistics::getRFMode() const { return getByte(8); }
void CRSFLinkStatistics::setRFMode(uint8_t value) { setByte(8, value); }

uint8_t CRSFLinkStatistics::getUplinkTxPower() const { return getByte(9); }
void CRSFLinkStatistics::setUplinkTxPower(uint8_t value) { setByte(9, value); }

uint8_t CRSFLinkStatistics::getDownlinkRSSI() const { return getByte(10); }
void CRSFLinkStatistics::setDownlinkRSSI(uint8_t value) { setByte(10, value); }

uint8_t CRSFLinkStatistics::getDownlinkLinkQuality() const { return getByte(11); }
void CRSFLinkStatistics::setDownlinkLinkQuality(uint8_t value) { setByte(11, value); }

int8_t CRSFLinkStatistics::getDownlinkSNR() const { return static_cast<int8_t>(getByte(12)); }
void CRSFLinkStatistics::setDownlinkSNR(int8_t value) { setByte(12, static_cast<uint8_t>(value)); }

// ============================================================================
// Utility Methods for RSSI and TX Power Conversion
// ============================================================================

int8_t CRSFLinkStatistics::getUplinkRSSI1dBm() const {
    // CRSF stores RSSI as (dBm * -1), so convert back
    return -static_cast<int8_t>(getUplinkRSSI1());
}

void CRSFLinkStatistics::setUplinkRSSI1dBm(int8_t dbm) {
    // CRSF stores RSSI as (dBm * -1)
    setUplinkRSSI1(static_cast<uint8_t>(-dbm));
}

int8_t CRSFLinkStatistics::getUplinkRSSI2dBm() const {
    // CRSF stores RSSI as (dBm * -1), so convert back
    return -static_cast<int8_t>(getUplinkRSSI2());
}

void CRSFLinkStatistics::setUplinkRSSI2dBm(int8_t dbm) {
    // CRSF stores RSSI as (dBm * -1)
    setUplinkRSSI2(static_cast<uint8_t>(-dbm));
}

int8_t CRSFLinkStatistics::getDownlinkRSSIdBm() const {
    // CRSF stores RSSI as (dBm * -1), so convert back
    return -static_cast<int8_t>(getDownlinkRSSI());
}

void CRSFLinkStatistics::setDownlinkRSSIdBm(int8_t dbm) {
    // CRSF stores RSSI as (dBm * -1)
    setDownlinkRSSI(static_cast<uint8_t>(-dbm));
}

uint16_t CRSFLinkStatistics::getUplinkTxPowerMW() const {
    // Convert CRSF enum to milliwatts
    switch (getUplinkTxPower()) {
        case TX_POWER_0MW: return 0;
        case TX_POWER_10MW: return 10;
        case TX_POWER_25MW: return 25;
        case TX_POWER_100MW: return 100;
        case TX_POWER_500MW: return 500;
        case TX_POWER_1000MW: return 1000;
        case TX_POWER_2000MW: return 2000;
        case TX_POWER_50MW: return 50;
        default: return 0;
    }
}

void CRSFLinkStatistics::setUplinkTxPowerMW(uint16_t mw) {
    // Convert milliwatts to CRSF enum
    uint8_t crsfValue;
    if (mw == 0) crsfValue = TX_POWER_0MW;
    else if (mw <= 10) crsfValue = TX_POWER_10MW;
    else if (mw <= 25) crsfValue = TX_POWER_25MW;
    else if (mw <= 50) crsfValue = TX_POWER_50MW;
    else if (mw <= 100) crsfValue = TX_POWER_100MW;
    else if (mw <= 500) crsfValue = TX_POWER_500MW;
    else if (mw <= 1000) crsfValue = TX_POWER_1000MW;
    else crsfValue = TX_POWER_2000MW;
    setUplinkTxPower(crsfValue);
}

bool CRSFLinkStatistics::isValid() const {
    return getType() == static_cast<uint8_t>(PacketType::LinkStatistics) &&
           getLength() == (PAYLOAD_SIZE + 1 + 1) && // payload(10) + type(1) + crc(1) = 12
           data_.size() >= 4 + PAYLOAD_SIZE;
}

// ============================================================================
// CRSFGPSData Implementation
// ============================================================================



CRSFGPSData::CRSFGPSData(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::GPS)) {
        setType(static_cast<uint8_t>(PacketType::GPS));
    }
}

uint32_t CRSFGPSData::getLatitude() const { return getU32(3); }
void CRSFGPSData::setLatitude(uint32_t value) { setU32(3, value); }

uint32_t CRSFGPSData::getLongitude() const { return getU32(7); }
void CRSFGPSData::setLongitude(uint32_t value) { setU32(7, value); }

uint16_t CRSFGPSData::getGroundSpeed() const { return getU16(11); }
void CRSFGPSData::setGroundSpeed(uint16_t value) { setU16(11, value); }

uint16_t CRSFGPSData::getGroundCourse() const { return getU16(13); }
void CRSFGPSData::setGroundCourse(uint16_t value) { setU16(13, value); }

uint16_t CRSFGPSData::getAltitude() const { return getU16(15); }
void CRSFGPSData::setAltitude(uint16_t value) { setU16(15, value); }

uint8_t CRSFGPSData::getSatellites() const { return getByte(17); }
void CRSFGPSData::setSatellites(uint8_t value) { setByte(17, value); }

double CRSFGPSData::getLatitudeDegrees() const {
    return static_cast<double>(getLatitude()) / LAT_LON_SCALE;
}

void CRSFGPSData::setLatitudeDegrees(double degrees) {
    setLatitude(static_cast<uint32_t>(degrees * LAT_LON_SCALE));
}

double CRSFGPSData::getLongitudeDegrees() const {
    return static_cast<double>(getLongitude()) / LAT_LON_SCALE;
}

void CRSFGPSData::setLongitudeDegrees(double degrees) {
    setLongitude(static_cast<uint32_t>(degrees * LAT_LON_SCALE));
}

bool CRSFGPSData::isValid() const {
    return getType() == static_cast<uint8_t>(PacketType::GPS) &&
           getLength() == PAYLOAD_SIZE &&
           data_.size() >= 4 + PAYLOAD_SIZE;
}

// ============================================================================
// CRSFBatterySensor Implementation
// ============================================================================



CRSFBatterySensor::CRSFBatterySensor(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::BatterySensor)) {
        setType(static_cast<uint8_t>(PacketType::BatterySensor));
    }
}

int16_t CRSFBatterySensor::getVoltage() const { 
    // Get signed 16-bit value from payload (big-endian)
    uint16_t raw = getU16(3);
    return static_cast<int16_t>(raw);
}
void CRSFBatterySensor::setVoltage(int16_t value) { 
    // Set signed 16-bit value to payload (big-endian)
    setU16(3, static_cast<uint16_t>(value));
}

int16_t CRSFBatterySensor::getCurrent() const { 
    // Get signed 16-bit value from payload (big-endian)
    uint16_t raw = getU16(5);
    return static_cast<int16_t>(raw);
}
void CRSFBatterySensor::setCurrent(int16_t value) { 
    // Set signed 16-bit value to payload (big-endian)
    setU16(5, static_cast<uint16_t>(value));
}

int32_t CRSFBatterySensor::getCapacity() const { 
    // 3-byte capacity field - BIG-ENDIAN (network byte order) as per CRSF spec
    // Handle signed 24-bit value (int24_t)
    if (data_.size() < 10) return 0;
    
    // Extract 24-bit value and handle sign extension
    uint32_t raw = static_cast<uint32_t>(data_[7]) << 16 |
           (static_cast<uint32_t>(data_[8]) << 8) |
                   static_cast<uint32_t>(data_[9]);
    
    // Sign extend if negative (bit 23 is set)
    if (raw & 0x800000) {
        raw |= 0xFF000000; // Set upper 8 bits for negative values
    }
    
    return static_cast<int32_t>(raw);
}

void CRSFBatterySensor::setCapacity(int32_t value) {
    assert(data_.size() >= 10 && "Battery sensor data must have enough space for capacity field");
    // BIG-ENDIAN (network byte order) as per CRSF spec
    // Store only the lower 24 bits
    uint32_t unsignedValue = static_cast<uint32_t>(value) & 0xFFFFFF;
    data_[7] = (unsignedValue >> 16) & 0xFF; // MSB
    data_[8] = (unsignedValue >> 8) & 0xFF;  // 
    data_[9] = unsignedValue & 0xFF;          // LSB
}

int8_t CRSFBatterySensor::getRemaining() const { 
    // Get signed 8-bit value from payload
    uint8_t raw = getByte(10);
    return static_cast<int8_t>(raw);
}
void CRSFBatterySensor::setRemaining(int8_t value) { 
    // Set signed 8-bit value to payload
    setByte(10, static_cast<uint8_t>(value));
}

double CRSFBatterySensor::getVoltageVolts() const {
    return static_cast<double>(getVoltage()) / VOLTAGE_SCALE;
}

void CRSFBatterySensor::setVoltageVolts(double volts) {
    setVoltage(static_cast<int16_t>(volts * VOLTAGE_SCALE));
}

double CRSFBatterySensor::getCurrentAmps() const {
    return static_cast<double>(getCurrent()) / CURRENT_SCALE;
}

void CRSFBatterySensor::setCurrentAmps(double amps) {
    setCurrent(static_cast<int16_t>(amps * CURRENT_SCALE));
}

bool CRSFBatterySensor::isValid() const {
    return getType() == static_cast<uint8_t>(PacketType::BatterySensor) &&
           getLength() == (PAYLOAD_SIZE + 1 + 1) && // payload(8) + type(1) + crc(1) = 10
           data_.size() >= 4 + PAYLOAD_SIZE;
}

// ============================================================================
// CRSFVario Implementation
// ============================================================================

CRSFVario::CRSFVario(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::Vario)) {
        setType(static_cast<uint8_t>(PacketType::Vario));
    }
}

int16_t CRSFVario::getVerticalSpeed() const { 
    // Get signed 16-bit value from payload (big-endian)
    uint16_t raw = getU16(3);
    return static_cast<int16_t>(raw);
}

void CRSFVario::setVerticalSpeed(int16_t value) { 
    // Set signed 16-bit value to payload (big-endian)
    setU16(3, static_cast<uint16_t>(value));
}

double CRSFVario::getVerticalSpeedMPS() const {
    // Convert from cm/s to m/s
    return static_cast<double>(getVerticalSpeed()) / SPEED_SCALE;
}

void CRSFVario::setVerticalSpeedMPS(double mps) {
    // Convert from m/s to cm/s
    setVerticalSpeed(static_cast<int16_t>(mps * SPEED_SCALE));
}

bool CRSFVario::isValid() const {
    return getType() == static_cast<uint8_t>(PacketType::Vario) &&
           getLength() == (PAYLOAD_SIZE + 1 + 1) && // payload(2) + type(1) + crc(1) = 4
           data_.size() >= 4 + PAYLOAD_SIZE;
}

// ============================================================================
// CRSFBaroAltitude Implementation
// ============================================================================

CRSFBaroAltitude::CRSFBaroAltitude(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::BaroAltitude)) {
        setType(static_cast<uint8_t>(PacketType::BaroAltitude));
    }
}

double CRSFBaroAltitude::getAltitudeMeters() const {
    return decodeAltitude(getRawAltitude());
}

void CRSFBaroAltitude::setAltitudeMeters(double altitudeMeters) {
    setRawAltitude(encodeAltitude(altitudeMeters));
}

uint16_t CRSFBaroAltitude::getRawAltitude() const {
    return getU16(3);
}

void CRSFBaroAltitude::setRawAltitude(uint16_t rawAltitude) {
    setU16(3, rawAltitude);
}

bool CRSFBaroAltitude::hasVerticalSpeed() const {
    // Check if we have 4 bytes of payload (altitude + vertical speed)
    return getLength() >= (MAX_PAYLOAD_SIZE + 1 + 1); // payload(4) + type(1) + crc(1) = 6
}

int16_t CRSFBaroAltitude::getVerticalSpeed() const {
    if (!hasVerticalSpeed()) return 0;
    
    // Get signed 16-bit value from payload (big-endian)
    uint16_t raw = getU16(5);
    return static_cast<int16_t>(raw);
}

void CRSFBaroAltitude::setVerticalSpeed(int16_t value) {
    // Ensure we have enough space for vertical speed
    if (data_.size() < 8) return; // sync(1) + len(1) + type(1) + altitude(2) + speed(2) + crc(1)
    
    // Set signed 16-bit value to payload (big-endian)
    setU16(5, static_cast<uint16_t>(value));
    
    // Update length field to include vertical speed
    setLength(MAX_PAYLOAD_SIZE + 1 + 1); // payload(4) + type(1) + crc(1) = 6
}

double CRSFBaroAltitude::getVerticalSpeedMPS() const {
    // Convert from cm/s to m/s
    return static_cast<double>(getVerticalSpeed()) / SPEED_SCALE;
}

void CRSFBaroAltitude::setVerticalSpeedMPS(double mps) {
    // Convert from m/s to cm/s
    setVerticalSpeed(static_cast<int16_t>(mps * SPEED_SCALE));
}

uint16_t CRSFBaroAltitude::encodeAltitude(double altitudeMeters) {
    // CRSF altitude encoding logic:
    // If altitude is in range -1000.0m to 2276.7m, use decimeters + 10000
    // If altitude is in range 0m to 32767m, use meters directly with high bit set
    
    if (altitudeMeters >= -1000.0 && altitudeMeters <= 2276.7) {
        // Mode 1: decimeters + 10000 (high bit NOT set)
        int16_t decimeters = static_cast<int16_t>(altitudeMeters * ALTITUDE_SCALE);
        return static_cast<uint16_t>(decimeters + static_cast<int16_t>(ALTITUDE_OFFSET));
    } else if (altitudeMeters >= 0.0 && altitudeMeters <= 32767.0) {
        // Mode 2: meters directly (high bit SET)
        return static_cast<uint16_t>(altitudeMeters) | HIGH_BIT_MASK;
    } else {
        // Out of range, clamp to valid range
        if (altitudeMeters < -1000.0) altitudeMeters = -1000.0;
        if (altitudeMeters > 32767.0) altitudeMeters = 32767.0;
        
        // Use decimeters mode for clamped values
        int16_t decimeters = static_cast<int16_t>(altitudeMeters * ALTITUDE_SCALE);
        return static_cast<uint16_t>(decimeters + static_cast<int16_t>(ALTITUDE_OFFSET));
    }
}

double CRSFBaroAltitude::decodeAltitude(uint16_t rawAltitude) {
    // CRSF altitude decoding logic:
    // Check high bit to determine encoding mode
    
    if (rawAltitude & HIGH_BIT_MASK) {
        // Mode 2: high bit set, altitude is in meters directly
        return static_cast<double>(rawAltitude & ~HIGH_BIT_MASK);
    } else {
        // Mode 1: high bit not set, altitude is in decimeters + 10000
        int16_t decimeters = static_cast<int16_t>(rawAltitude) - static_cast<int16_t>(ALTITUDE_OFFSET);
        return static_cast<double>(decimeters) / ALTITUDE_SCALE;
    }
}

bool CRSFBaroAltitude::isValid() const {
    // Check basic packet validity
    if (getType() != static_cast<uint8_t>(PacketType::BaroAltitude)) return false;
    
    // Check payload size (2 or 4 bytes)
    uint8_t payloadSize = getLength() - 1 - 1; // length - type(1) - crc(1)
    if (payloadSize != MIN_PAYLOAD_SIZE && payloadSize != MAX_PAYLOAD_SIZE) return false;
    
    // Check total packet size
    if (data_.size() < 4 + static_cast<size_t>(payloadSize)) return false;
    
    // Validate altitude value
    uint16_t rawAltitude = getRawAltitude();
    double altitudeMeters = decodeAltitude(rawAltitude);
    
    // Check if decoded altitude is in valid range
    if (altitudeMeters < -1000.0 || altitudeMeters > 32767.0) return false;
    
    return true;
}

// ============================================================================
// CRSFHeartbeat Implementation
// ============================================================================

CRSFHeartbeat::CRSFHeartbeat(const etl::span<uint8_t>& data) : CRSFPacket(data) {
    if (getType() != static_cast<uint8_t>(PacketType::Heartbeat)) {
        setType(static_cast<uint8_t>(PacketType::Heartbeat));
    }
}

uint16_t CRSFHeartbeat::getOriginDeviceAddress() const {
    return getU16(3);
}

void CRSFHeartbeat::setOriginDeviceAddress(uint16_t address) {
    setU16(3, address);
}

// Utility methods for common device addresses
void CRSFHeartbeat::setFlightController() {
    setOriginDeviceAddress(ADDRESS_FLIGHT_CONTROLLER);
}

void CRSFHeartbeat::setRadio() {
    setOriginDeviceAddress(ADDRESS_RADIO);
}

void CRSFHeartbeat::setReceiver() {
    setOriginDeviceAddress(ADDRESS_RECEIVER);
}

void CRSFHeartbeat::setGimbal() {
    setOriginDeviceAddress(ADDRESS_GIMBAL);
}

void CRSFHeartbeat::setBlackbox() {
    setOriginDeviceAddress(ADDRESS_BLACKBOX);
}

void CRSFHeartbeat::setESC() {
    setOriginDeviceAddress(ADDRESS_ESC);
}

void CRSFHeartbeat::setCamera() {
    setOriginDeviceAddress(ADDRESS_CAMERA);
}

void CRSFHeartbeat::setVTX() {
    setOriginDeviceAddress(ADDRESS_VTX);
}

void CRSFHeartbeat::setGPS() {
    setOriginDeviceAddress(ADDRESS_GPS);
}

void CRSFHeartbeat::setBattery() {
    setOriginDeviceAddress(ADDRESS_BATTERY);
}

bool CRSFHeartbeat::isValid() const {
    // Check basic packet validity
    if (getType() != static_cast<uint8_t>(PacketType::Heartbeat)) return false;
    
    // Check payload size (2 bytes for device address)
    uint8_t payloadSize = getLength() - 1 - 1; // length - type(1) - crc(1)
    if (payloadSize != PAYLOAD_SIZE) return false;
    
    // Check total packet size
    if (data_.size() < 4 + static_cast<size_t>(payloadSize)) return false;
    
    return true;
}


std::unique_ptr<CRSFLinkStatistics> createLinkStatistics(
    uint8_t address,
    uint8_t uplinkRSSI1,
    uint8_t uplinkRSSI2,
    uint8_t uplinkLinkQuality,
    uint8_t uplinkSNR,
    uint8_t activeAntenna,
    uint8_t rfMode,
    uint8_t uplinkTxPower,
    uint8_t downlinkRSSI,
    uint8_t downlinkLinkQuality,
    uint8_t downlinkSNR,
    etl::span<uint8_t>& data) {
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte (using address as sync for compatibility)
    data[1] = static_cast<uint8_t>(10 + 1 + 1); // LEN: payload(10) + type(1) + crc(1) = 12
    data[2] = static_cast<uint8_t>(PacketType::LinkStatistics);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFLinkStatistics>(data);
    
    // Set all the link statistics values
    packet->setUplinkRSSI1(uplinkRSSI1);
    packet->setUplinkRSSI2(uplinkRSSI2);
    packet->setUplinkLinkQuality(uplinkLinkQuality);
    packet->setUplinkSNR(uplinkSNR);
    packet->setActiveAntenna(activeAntenna);
    packet->setRFMode(rfMode);
    packet->setUplinkTxPower(uplinkTxPower);
    packet->setDownlinkRSSI(downlinkRSSI);
    packet->setDownlinkLinkQuality(downlinkLinkQuality);
    packet->setDownlinkSNR(downlinkSNR);
    
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFGPSData> createGPSData(
    uint8_t address,
    uint32_t latitude,
    uint32_t longitude,
    uint16_t groundSpeed,
    uint16_t groundCourse,
    uint16_t altitude,
    uint8_t satellites,
    etl::span<uint8_t>& data) {
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte (using address as sync for compatibility)
    data[1] = static_cast<uint8_t>(15 + 1 + 1); // LEN: payload(15) + type(1) + crc(1) = 17
    data[2] = static_cast<uint8_t>(PacketType::GPS);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFGPSData>(data);
    packet->setLatitude(latitude);
    packet->setLongitude(longitude);
    packet->setGroundSpeed(groundSpeed);
    packet->setGroundCourse(groundCourse);
    packet->setAltitude(altitude);
    packet->setSatellites(satellites);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFGPSData> createGPSDataDegrees(
    uint8_t address,
    double latitudeDegrees,
    double longitudeDegrees,
    uint16_t groundSpeed,
    uint16_t groundCourse,
    uint16_t altitude,
    uint8_t satellites,
    etl::span<uint8_t>& data) {
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte (using address as sync for compatibility)
    data[1] = static_cast<uint8_t>(15 + 1 + 1); // LEN: payload(15) + type(1) + crc(1) = 17
    data[2] = static_cast<uint8_t>(PacketType::GPS);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFGPSData>(data);
    packet->setLatitudeDegrees(latitudeDegrees);
    packet->setLongitudeDegrees(longitudeDegrees);
    packet->setGroundSpeed(groundSpeed);
    packet->setGroundCourse(groundCourse);
    packet->setAltitude(altitude);
    packet->setSatellites(satellites);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFBatterySensor> createBatterySensor(
    uint8_t address,
    int16_t voltage,
    int16_t current,
    int32_t capacity,
    int8_t remaining,
    etl::span<uint8_t>& data) {
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte (using address as sync for compatibility)
    data[1] = static_cast<uint8_t>(1 + 10 + 1); // LEN: type(1) + payload(10) + crc(1)
    data[2] = static_cast<uint8_t>(PacketType::BatterySensor);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFBatterySensor>(data);
    packet->setVoltage(voltage);
    packet->setCurrent(current);
    packet->setCapacity(capacity);
    packet->setRemaining(remaining);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFBatterySensor> createBatterySensorUnits(
    uint8_t address,
    double voltageVolts,
    double currentAmps,
    int32_t capacity,
    int8_t remaining,
    etl::span<uint8_t>& data) {
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte (using address as sync for compatibility)
    data[1] = static_cast<uint8_t>(1 + 10 + 1); // LEN: type(1) + payload(10) + crc(1)
    data[2] = static_cast<uint8_t>(PacketType::BatterySensor);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFBatterySensor>(data);
    packet->setVoltageVolts(voltageVolts);
    packet->setCurrentAmps(currentAmps);
    packet->setCapacity(capacity);
    packet->setRemaining(remaining);
    packet->calculateCRC();
    return packet;
}

// ============================================================================
// VARIO Factory Functions
// ============================================================================

std::unique_ptr<CRSFVario> createVario(
    uint8_t address,
    int16_t verticalSpeed,
    etl::span<uint8_t>& data) {
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte (using address as sync for compatibility)
    data[1] = static_cast<uint8_t>(1 + 2 + 1); // LEN: type(1) + payload(2) + crc(1) = 4
    data[2] = static_cast<uint8_t>(PacketType::Vario);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFVario>(data);
    packet->setVerticalSpeed(verticalSpeed);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFVario> createVarioMPS(
    uint8_t address,
    double verticalSpeedMPS,
    etl::span<uint8_t>& data) {
    
    // Convert m/s to cm/s for CRSF protocol
    int16_t verticalSpeedCMS = static_cast<int16_t>(verticalSpeedMPS * 100.0);
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte (using address as sync for compatibility)
    data[1] = static_cast<uint8_t>(1 + 2 + 1); // LEN: type(1) + payload(2) + crc(1) = 4
    data[2] = static_cast<uint8_t>(PacketType::Vario);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFVario>(data);
    packet->setVerticalSpeed(verticalSpeedCMS);
    packet->calculateCRC();
    return packet;
}

// ============================================================================
// BARO ALTITUDE Factory Functions
// ============================================================================

std::unique_ptr<CRSFBaroAltitude> createBaroAltitude(
    uint8_t address,
    double altitudeMeters,
    etl::span<uint8_t>& data) {
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte (using address as sync for compatibility)
    data[1] = static_cast<uint8_t>(1 + 2 + 1); // LEN: type(1) + payload(2) + crc(1) = 4
    data[2] = static_cast<uint8_t>(PacketType::BaroAltitude);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFBaroAltitude>(data);
    packet->setAltitudeMeters(altitudeMeters);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFBaroAltitude> createBaroAltitudeWithVario(
    uint8_t address,
    double altitudeMeters,
    int16_t verticalSpeed,
    etl::span<uint8_t>& data) {
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte (using address as sync for compatibility)
    data[1] = static_cast<uint8_t>(1 + 4 + 1); // LEN: type(1) + payload(4) + crc(1) = 6
    data[2] = static_cast<uint8_t>(PacketType::BaroAltitude);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFBaroAltitude>(data);
    packet->setAltitudeMeters(altitudeMeters);
    packet->setVerticalSpeed(verticalSpeed);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFBaroAltitude> createBaroAltitudeWithVarioMPS(
    uint8_t address,
    double altitudeMeters,
    double verticalSpeedMPS,
    etl::span<uint8_t>& data) {
    
    // Convert m/s to cm/s for CRSF protocol
    int16_t verticalSpeedCMS = static_cast<int16_t>(verticalSpeedMPS * 100.0);
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte (using address as sync for compatibility)
    data[1] = static_cast<uint8_t>(1 + 4 + 1); // LEN: type(1) + payload(4) + crc(1) = 6
    data[2] = static_cast<uint8_t>(PacketType::BaroAltitude);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFBaroAltitude>(data);
    packet->setAltitudeMeters(altitudeMeters);
    packet->setVerticalSpeed(verticalSpeedCMS);
    packet->calculateCRC();
    return packet;
}

// ============================================================================
// HEARTBEAT Factory Functions
// ============================================================================

std::unique_ptr<CRSFHeartbeat> createHeartbeat(
    uint8_t address,
    uint16_t originDeviceAddress,
    etl::span<uint8_t>& data) {
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte (using address as sync for compatibility)
    data[1] = static_cast<uint8_t>(1 + 2 + 1); // LEN: type(1) + payload(2) + crc(1) = 4
    data[2] = static_cast<uint8_t>(PacketType::Heartbeat);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFHeartbeat>(data);
    packet->setOriginDeviceAddress(originDeviceAddress);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFHeartbeat> createFlightControllerHeartbeat(
    uint8_t address,
    etl::span<uint8_t>& data) {
    
    return createHeartbeat(address, CRSFHeartbeat::ADDRESS_FLIGHT_CONTROLLER, data);
}

std::unique_ptr<CRSFHeartbeat> createRadioHeartbeat(
    uint8_t address,
    etl::span<uint8_t>& data) {
    
    return createHeartbeat(address, CRSFHeartbeat::ADDRESS_RADIO, data);
}



// ============================================================================
// LINK RX ID Factory Functions (CRSFv3)
// ============================================================================

std::unique_ptr<CRSFLinkRXID> createLinkRXID(
    uint8_t address,
    uint8_t downlinkRSSIdBm,
    uint8_t downlinkRSSIPercent,
    uint8_t downlinkLinkQuality,
    int8_t downlinkSNR,
    uint8_t uplinkRFPowerIndex,
    etl::span<uint8_t>& data) {
    
    // Ensure we have enough space
    if (data.size() < 8) return nullptr;  // header(3) + payload(5)
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte
    data[1] = static_cast<uint8_t>(CRSFLinkRXID::PAYLOAD_SIZE + 1 + 1); // LEN: payload + type(1) + crc(1)
    data[2] = static_cast<uint8_t>(PacketType::LinkRXID);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFLinkRXID>(data);
    
    // Set all fields
    packet->setDownlinkRSSIdBm(downlinkRSSIdBm);
    packet->setDownlinkRSSIPercent(downlinkRSSIPercent);
    packet->setDownlinkLinkQuality(downlinkLinkQuality);
    packet->setDownlinkSNR(downlinkSNR);
    packet->setUplinkRFPowerIndex(uplinkRFPowerIndex);
    
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFLinkRXID> createLinkRXIDWithSignedRSSI(
    uint8_t address,
    int8_t downlinkRSSIdBm,
    uint8_t downlinkRSSIPercent,
    uint8_t downlinkLinkQuality,
    int8_t downlinkSNR,
    uint8_t uplinkRFPowerIndex,
    etl::span<uint8_t>& data) {
    
    // Convert signed dBm to unsigned CRSF format
    uint8_t crsfRSSI = static_cast<uint8_t>(-downlinkRSSIdBm);
    
    return createLinkRXID(address, crsfRSSI, downlinkRSSIPercent, 
                         downlinkLinkQuality, downlinkSNR, uplinkRFPowerIndex, data);
}

// ============================================================================
// LINK TX ID Factory Functions (CRSFv3)
// ============================================================================

std::unique_ptr<CRSFLinkTXID> createLinkTXID(
    uint8_t address,
    uint8_t uplinkRSSIdBm,
    uint8_t uplinkRSSIPercent,
    uint8_t uplinkLinkQuality,
    int8_t uplinkSNR,
    uint8_t downlinkRFPower,
    uint8_t packetRateFPS,
    etl::span<uint8_t>& data) {
    
    // Ensure we have enough space
    if (data.size() < 9) return nullptr;  // header(3) + payload(6)
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte
    data[1] = static_cast<uint8_t>(CRSFLinkTXID::PAYLOAD_SIZE + 1 + 1); // LEN: payload + type(1) + crc(1)
    data[2] = static_cast<uint8_t>(PacketType::LinkTXID);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFLinkTXID>(data);
    
    // Set all fields
    packet->setUplinkRSSIdBm(uplinkRSSIdBm);
    packet->setUplinkRSSIPercent(uplinkRSSIPercent);
    packet->setUplinkLinkQuality(uplinkLinkQuality);
    packet->setUplinkSNR(uplinkSNR);
    packet->setDownlinkRFPower(downlinkRFPower);
    packet->setPacketRateFPS(packetRateFPS);
    
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFLinkTXID> createLinkTXIDWithSignedRSSI(
    uint8_t address,
    int8_t uplinkRSSIdBm,
    uint8_t uplinkRSSIPercent,
    uint8_t uplinkLinkQuality,
    int8_t uplinkSNR,
    uint8_t downlinkRFPower,
    uint8_t packetRateFPS,
    etl::span<uint8_t>& data) {
    
    // Convert signed dBm to unsigned CRSF format
    uint8_t crsfRSSI = static_cast<uint8_t>(-uplinkRSSIdBm);
    
    return createLinkTXID(address, crsfRSSI, uplinkRSSIPercent, 
                         uplinkLinkQuality, uplinkSNR, downlinkRFPower, packetRateFPS, data);
}

std::unique_ptr<CRSFLinkTXID> createLinkTXIDWithCommonFPS(
    uint8_t address,
    uint8_t uplinkRSSIdBm,
    uint8_t uplinkRSSIPercent,
    uint8_t uplinkLinkQuality,
    int8_t uplinkSNR,
    uint8_t downlinkRFPower,
    uint16_t fpsHz,
    etl::span<uint8_t>& data) {
    
    // Convert FPS to stored value (FPS/10)
    uint8_t packetRateFPS = static_cast<uint8_t>(fpsHz / 10);
    if (packetRateFPS > CRSFLinkTXID::FPS_MAX) packetRateFPS = CRSFLinkTXID::FPS_MAX;
    
    return createLinkTXID(address, uplinkRSSIdBm, uplinkRSSIPercent, 
                         uplinkLinkQuality, uplinkSNR, downlinkRFPower, packetRateFPS, data);
}

// ============================================================================
// ATTITUDE Factory Functions
// ============================================================================

std::unique_ptr<CRSFAttitude> createAttitude(
    uint8_t address,
    int16_t pitch,
    int16_t roll,
    int16_t yaw,
    etl::span<uint8_t>& data) {
    
    // Ensure we have enough space
    if (data.size() < 9) return nullptr;  // header(3) + payload(6)
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte
    data[1] = static_cast<uint8_t>(CRSFAttitude::PAYLOAD_SIZE + 1 + 1); // LEN: payload + type(1) + crc(1)
    data[2] = static_cast<uint8_t>(PacketType::Attitude);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFAttitude>(data);
    
    // Set all fields
    packet->setPitch(pitch);
    packet->setRoll(roll);
    packet->setYaw(yaw);
    
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFAttitude> createAttitudeDegrees(
    uint8_t address,
    double pitchDegrees,
    double rollDegrees,
    double yawDegrees,
    etl::span<uint8_t>& data) {
    
    // Convert degrees to radians, then to stored format
    double pitchRadians = pitchDegrees * CRSFAttitude::DEGREES_TO_RADIANS;
    double rollRadians = rollDegrees * CRSFAttitude::DEGREES_TO_RADIANS;
    double yawRadians = yawDegrees * CRSFAttitude::DEGREES_TO_RADIANS;
    
    return createAttitude(address, 
                         static_cast<int16_t>(std::round(pitchRadians * CRSFAttitude::RADIANS_TO_STORED)),
                         static_cast<int16_t>(std::round(rollRadians * CRSFAttitude::RADIANS_TO_STORED)),
                         static_cast<int16_t>(std::round(yawRadians * CRSFAttitude::RADIANS_TO_STORED)),
                         data);
}

std::unique_ptr<CRSFAttitude> createAttitudeRadians(
    uint8_t address,
    double pitchRadians,
    double rollRadians,
    double yawRadians,
    etl::span<uint8_t>& data) {
    
    // Convert radians to stored format
    return createAttitude(address,
                         static_cast<int16_t>(std::round(pitchRadians * CRSFAttitude::RADIANS_TO_STORED)),
                         static_cast<int16_t>(std::round(rollRadians * CRSFAttitude::RADIANS_TO_STORED)),
                         static_cast<int16_t>(std::round(yawRadians * CRSFAttitude::RADIANS_TO_STORED)),
                         data);
}

std::unique_ptr<CRSFAttitude> createAttitudeLevel(
    uint8_t address,
    etl::span<uint8_t>& data) {
    
    // Create a level attitude packet (all angles = 0)
    return createAttitude(address, 0, 0, 0, data);
}

// ============================================================================
// FLIGHT MODE Factory Functions
// ============================================================================

std::unique_ptr<CRSFFlightMode> createFlightMode(
    uint8_t address,
    const std::string& mode,
    etl::span<uint8_t>& data) {
    
    // Ensure we have enough space
    if (data.size() < (3 + CRSFFlightMode::PAYLOAD_SIZE)) return nullptr;  // header(3) + payload(14)
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte
    data[1] = static_cast<uint8_t>(mode.length() + 1 + 1); // LEN: string + type(1) + crc(1)
    data[2] = static_cast<uint8_t>(PacketType::FlightMode);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFFlightMode>(data);
    
    // Set the flight mode string
    packet->setFlightMode(mode);
    
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFFlightMode> createAcroMode(
    uint8_t address,
    etl::span<uint8_t>& data) {
    return createFlightMode(address, CRSFFlightMode::MODE_ACRO, data);
}

std::unique_ptr<CRSFFlightMode> createHorizonMode(
    uint8_t address,
    etl::span<uint8_t>& data) {
    return createFlightMode(address, CRSFFlightMode::MODE_HORIZON, data);
}

std::unique_ptr<CRSFFlightMode> createAngleMode(
    uint8_t address,
    etl::span<uint8_t>& data) {
    return createFlightMode(address, CRSFFlightMode::MODE_ANGLE, data);
}

std::unique_ptr<CRSFFlightMode> createRTHMode(
    uint8_t address,
    etl::span<uint8_t>& data) {
    return createFlightMode(address, CRSFFlightMode::MODE_RTH, data);
}

std::unique_ptr<CRSFFlightMode> createLandMode(
    uint8_t address,
    etl::span<uint8_t>& data) {
    return createFlightMode(address, CRSFFlightMode::MODE_LAND, data);
}

std::unique_ptr<CRSFFlightMode> createTakeoffMode(
    uint8_t address,
    etl::span<uint8_t>& data) {
    return createFlightMode(address, CRSFFlightMode::MODE_TAKEOFF, data);
}

std::unique_ptr<CRSFFlightMode> createManualMode(
    uint8_t address,
    etl::span<uint8_t>& data) {
    return createFlightMode(address, CRSFFlightMode::MODE_MANUAL, data);
}

// ============================================================================
// DEVICE PING Factory Functions
// ============================================================================

std::unique_ptr<CRSFDevicePing> createDevicePing(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data) {
    
    // Ensure we have enough space
    if (data.size() < 6) return nullptr;  // header(3) + payload(2) + crc(1)
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte
    data[1] = static_cast<uint8_t>(CRSFDevicePing::PAYLOAD_SIZE + 1 + 1); // LEN: payload + type(1) + crc(1)
    data[2] = static_cast<uint8_t>(PacketType::DevicePing);
    
    // Create packet directly from the provided span
    auto packet = std::make_unique<CRSFDevicePing>(data);
    
    // Set extended addresses
    packet->setExtendedDestination(extendedDestination);
    packet->setExtendedSource(extendedSource);
    
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFDevicePing> createBroadcastPing(
    uint8_t address,
    uint8_t extendedSource,
    etl::span<uint8_t>& data) {
    
    return createDevicePing(address, CRSFDevicePing::ADDRESS_BROADCAST, extendedSource, data);
}

std::unique_ptr<CRSFDevicePing> createTargetedPing(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data) {
    
    return createDevicePing(address, extendedDestination, extendedSource, data);
}

std::unique_ptr<CRSFDevicePing> createRadioPing(
    uint8_t address,
    uint8_t extendedDestination,
    etl::span<uint8_t>& data) {
    
    return createDevicePing(address, extendedDestination, CRSFDevicePing::ADDRESS_RADIO_TRANSMITTER, data);
}

// ============================================================================
// DEVICE INFO Factory Functions
// ============================================================================

std::unique_ptr<CRSFDeviceInfo> createDeviceInfo(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    const std::string& displayName,
    const std::string& serialNumber,
    uint32_t hardwareVersion,
    uint32_t softwareVersion,
    uint8_t configParameterCount,
    uint8_t parameterProtocolVersion,
    etl::span<uint8_t>& data) {
    
    // Ensure we have enough space
    if (data.size() < 35) return nullptr;  // header(3) + payload(30) + crc(1) + extended(2)
    
    // Set the packet header according to CRSF spec
    data[0] = address;  // SYNC byte
    // LEN = type(1) + payload + crc(1) = 1 + 30 + 1 = 32
    data[1] = 32;
    data[2] = static_cast<uint8_t>(PacketType::DeviceInfo);
    
    // Directly write display name to payload (bytes 3-18)
    std::string truncatedName = displayName.substr(0, CRSFDeviceInfo::MAX_DISPLAY_NAME_LENGTH);
    for (size_t i = 0; i < truncatedName.length(); ++i) {
        data[3 + i] = static_cast<uint8_t>(truncatedName[i]);
    }
    data[3 + truncatedName.length()] = 0;  // null terminator
    // Fill remaining display name bytes with nulls to ensure consistent layout
    for (size_t i = truncatedName.length() + 1; i < 16; ++i) {
        data[3 + i] = 0;
    }
    
    // Write serial number to payload (bytes 19-22)
    std::string paddedSerial = serialNumber.substr(0, CRSFDeviceInfo::SERIAL_NUMBER_LENGTH);
    while (paddedSerial.length() < CRSFDeviceInfo::SERIAL_NUMBER_LENGTH) {
        paddedSerial += '\0';  // Pad with nulls
    }
    for (size_t i = 0; i < CRSFDeviceInfo::SERIAL_NUMBER_LENGTH; ++i) {
        data[19 + i] = static_cast<uint8_t>(paddedSerial[i]);
    }
    
    // Write hardware version to payload (bytes 23-26)
    data[23] = (hardwareVersion >> 24) & 0xFF;
    data[24] = (hardwareVersion >> 16) & 0xFF;
    data[25] = (hardwareVersion >> 8) & 0xFF;
    data[26] = hardwareVersion & 0xFF;
    
    // Write software version to payload (bytes 27-30)
    data[27] = (softwareVersion >> 24) & 0xFF;
    data[28] = (softwareVersion >> 16) & 0xFF;
    data[29] = (softwareVersion >> 8) & 0xFF;
    data[30] = softwareVersion & 0xFF;
    
    // Write config parameter count to payload (byte 31)
    data[31] = configParameterCount;
    
    // Write parameter protocol version to payload (byte 32)
    data[32] = parameterProtocolVersion;
    
    // Create packet from the populated data
    auto packet = std::make_unique<CRSFDeviceInfo>(data);
    
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFDeviceInfo> createELRSDeviceInfo(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    const std::string& displayName,
    uint32_t hardwareVersion,
    uint8_t configParameterCount,
    etl::span<uint8_t>& data) {
    
    return createDeviceInfo(
        address, extendedDestination, extendedSource,
        displayName, CRSFDeviceInfo::SERIAL_ELRS,
        hardwareVersion, 0,  // Software version 0 for ELRSv2
        configParameterCount, 0, data  // Protocol version 0
    );
}

std::unique_ptr<CRSFDeviceInfo> createELRSDeviceInfoWithVersion(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    const std::string& displayName,
    uint32_t hardwareVersion,
    uint8_t majorVersion,
    uint8_t minorVersion,
    uint8_t revisionVersion,
    uint8_t configParameterCount,
    etl::span<uint8_t>& data) {
    
    // Ensure we have enough space
    if (data.size() < 35) return nullptr;
    
    // Set the packet header
    data[0] = address;
    data[1] = static_cast<uint8_t>(CRSFDeviceInfo::PAYLOAD_SIZE + 1 + 1);
    data[2] = static_cast<uint8_t>(PacketType::DeviceInfo);
    
    auto packet = std::make_unique<CRSFDeviceInfo>(data);
    
    // Set fields
    packet->setDisplayName(displayName);
    packet->setSerialNumber(CRSFDeviceInfo::SERIAL_ELRS);
    packet->setHardwareVersion(hardwareVersion);
    packet->setSoftwareVersion(majorVersion, minorVersion, revisionVersion);  // ELRSv3 format
    packet->setConfigParameterCount(configParameterCount);
    packet->setParameterProtocolVersion(0);
    
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFDeviceInfo> createFlightControllerInfo(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    const std::string& displayName,
    uint32_t hardwareVersion,
    uint32_t softwareVersion,
    uint8_t configParameterCount,
    etl::span<uint8_t>& data) {
    
    return createDeviceInfo(
        address, extendedDestination, extendedSource,
        displayName, "FC  ",  // 4-char serial for flight controller
        hardwareVersion, softwareVersion,
        configParameterCount, 0, data  // Protocol version 0
    );
}

// ============================================================================
// PARAMETER SETTINGS ENTRY Factory Functions
// ============================================================================

std::unique_ptr<CRSFParameterSettingsEntry> createParameterSettingsEntry(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t chunksRemaining,
    uint8_t parent,
    uint8_t type,
    bool hidden,
    const std::string& label,
    etl::span<uint8_t>& data) {
    
    // Calculate required size: header(3) + fields(4) + label + null + crc(1)
    size_t payloadSize = 4 + label.length() + 1;  // fields + label + null
    size_t totalSize = 3 + payloadSize + 1;  // header + payload + crc
    if (data.size() < totalSize) return nullptr;
    
    // Set packet header
    data[0] = address;
    data[1] = static_cast<uint8_t>(totalSize-2);  // Length = fields + label + null (excluding header)
    data[2] = static_cast<uint8_t>(PacketType::ParameterSettingsEntry);
    
    // Set basic fields
    data[3] = fieldIndex;
    data[4] = chunksRemaining;
    data[5] = parent;
    data[6] = (type & 0x7F) | (hidden ? 0x80 : 0x00);

    
    // Set label
    for (size_t i = 0; i < label.length(); ++i) {
        data[7 + i] = static_cast<uint8_t>(label[i]);
    }
    data[7 + label.length()] = 0;  // null terminator
 
    // Create packet and calculate CRC
    auto packet = std::make_unique<CRSFParameterSettingsEntry>(data);

    packet->calculateCRC();


    return packet;
}

std::unique_ptr<CRSFParameterSettingsEntry> createIntegerParameter(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t chunksRemaining,
    uint8_t parent,
    uint8_t type,
    bool hidden,
    const std::string& label,
    int32_t currentValue,
    int32_t minValue,
    int32_t maxValue,
    int32_t defaultValue,
    const std::string& units,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t typeSize = (type <= 0x01) ? 1 : (type <= 0x03) ? 2 : 4;  // 1, 2, or 4 bytes
    size_t requiredSize = 3 + 4 + label.length() + 1 + typeSize * 4 + units.length() + 1;
    if (data.size() < requiredSize) return nullptr;
    
    // Create base packet
    auto packet = createParameterSettingsEntry(address, extendedDestination, extendedSource,
        fieldIndex, chunksRemaining, parent, type, hidden, label, data);
    
    if (!packet) return nullptr;
    
    // Add values after label
    size_t valueStart = 7 + label.length() + 1;
    
    // Write values based on type size
    if (typeSize == 1) {
        data[valueStart] = static_cast<uint8_t>(currentValue);
        data[valueStart + 1] = static_cast<uint8_t>(minValue);
        data[valueStart + 2] = static_cast<uint8_t>(maxValue);
        data[valueStart + 3] = static_cast<uint8_t>(defaultValue);
    } else if (typeSize == 2) {
        data[valueStart] = (currentValue >> 8) & 0xFF;
        data[valueStart + 1] = currentValue & 0xFF;
        data[valueStart + 2] = (minValue >> 8) & 0xFF;
        data[valueStart + 3] = minValue & 0xFF;
        data[valueStart + 4] = (maxValue >> 8) & 0xFF;
        data[valueStart + 5] = maxValue & 0xFF;
        data[valueStart + 6] = (currentValue >> 8) & 0xFF;
        data[valueStart + 7] = currentValue & 0xFF;
    } else {  // 4 bytes
        data[valueStart] = (currentValue >> 24) & 0xFF;
        data[valueStart + 1] = (currentValue >> 16) & 0xFF;
        data[valueStart + 2] = (currentValue >> 8) & 0xFF;
        data[valueStart + 3] = currentValue & 0xFF;
        data[valueStart + 4] = (minValue >> 0) & 0xFF;
        data[valueStart + 5] = (minValue >> 8) & 0xFF;
        data[valueStart + 6] = (minValue >> 16) & 0xFF;
        data[valueStart + 7] = (minValue >> 24) & 0xFF;
        data[valueStart + 8] = (maxValue >> 0) & 0xFF;
        data[valueStart + 9] = (maxValue >> 8) & 0xFF;
        data[valueStart + 10] = (maxValue >> 16) & 0xFF;
        data[valueStart + 11] = (maxValue >> 24) & 0xFF;
        data[valueStart + 12] = (defaultValue >> 0) & 0xFF;
        data[valueStart + 13] = (defaultValue >> 8) & 0xFF;
        data[valueStart + 14] = (defaultValue >> 16) & 0xFF;
        data[valueStart + 15] = (defaultValue >> 24) & 0xFF;
    }
    
    // Write units
    size_t unitsStart = valueStart + typeSize * 4;
    for (size_t i = 0; i < units.length(); ++i) {
        data[unitsStart + i] = static_cast<uint8_t>(units[i]);
    }
    data[unitsStart + units.length()] = 0;
    
    // Update length and recalculate CRC
    packet->setLength(static_cast<uint8_t>(requiredSize - 3));
    packet->calculateCRC();
    
    return packet;
}

std::unique_ptr<CRSFParameterSettingsEntry> createFloatParameter(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t chunksRemaining,
    uint8_t parent,
    bool hidden,
    const std::string& label,
    float currentValue,
    float minValue,
    float maxValue,
    uint8_t precision,
    float step,
    const std::string& units,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 4 + label.length() + 1 + 16 + units.length() + 1;  // 16 bytes for float values
    if (data.size() < requiredSize) return nullptr;
    
    // Create base packet
    auto packet = createParameterSettingsEntry(address, extendedDestination, extendedSource,
        fieldIndex, chunksRemaining, parent, CRSFParameterSettingsEntry::TYPE_FLOAT, hidden, label, data);
    
    if (!packet) return nullptr;
    
    // Add float values after label
    size_t valueStart = 7 + label.length() + 1;
    
    // Convert floats to fixed-point integers (assuming 3 decimal places)
    int32_t currentInt = static_cast<int32_t>(currentValue * 1000);
    int32_t minInt = static_cast<int32_t>(minValue * 1000);
    int32_t maxInt = static_cast<int32_t>(maxValue * 1000);
    uint32_t stepInt = static_cast<uint32_t>(step * 1000);
    
    // Write values
    data[valueStart] = (currentInt >> 24) & 0xFF;
    data[valueStart + 1] = (currentInt >> 16) & 0xFF;
    data[valueStart + 2] = (currentInt >> 8) & 0xFF;
    data[valueStart + 3] = currentInt & 0xFF;
    data[valueStart + 4] = (minInt >> 24) & 0xFF;
    data[valueStart + 5] = (minInt >> 16) & 0xFF;
    data[valueStart + 6] = (minInt >> 8) & 0xFF;
    data[valueStart + 7] = minInt & 0xFF;
    data[valueStart + 8] = (maxInt >> 24) & 0xFF;
    data[valueStart + 9] = (maxInt >> 16) & 0xFF;
    data[valueStart + 10] = (maxInt >> 8) & 0xFF;
    data[valueStart + 11] = maxInt & 0xFF;
    data[valueStart + 12] = precision;
    data[valueStart + 13] = (stepInt >> 24) & 0xFF;
    data[valueStart + 14] = (stepInt >> 16) & 0xFF;
    data[valueStart + 15] = (stepInt >> 8) & 0xFF;
    data[valueStart + 16] = stepInt & 0xFF;
    
    // Write units
    size_t unitsStart = valueStart + 17;
    for (size_t i = 0; i < units.length(); ++i) {
        data[unitsStart + i] = static_cast<uint8_t>(units[i]);
    }
    data[unitsStart + units.length()] = 0;
    
    // Update length and recalculate CRC
    packet->setLength(static_cast<uint8_t>(requiredSize - 3));
    packet->calculateCRC();
    
    return packet;
}

std::unique_ptr<CRSFParameterSettingsEntry> createSelectParameter(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t chunksRemaining,
    uint8_t parent,
    bool hidden,
    const std::string& label,
    uint8_t currentIndex,
    const std::vector<std::string>& options,
    const std::string& units,
    etl::span<uint8_t>& data) {
    
    // Build options string
    std::string optionsStr;
    for (size_t i = 0; i < options.size(); ++i) {
        if (i > 0) optionsStr += ";";
        optionsStr += options[i];
    }
    
    // Calculate required size
    size_t requiredSize = 3 + 4 + label.length() + 1 + 4 + optionsStr.length() + 1 + units.length() + 1;
    if (data.size() < requiredSize) return nullptr;
    
    // Create base packet
    auto packet = createParameterSettingsEntry(address, extendedDestination, extendedSource,
        fieldIndex, chunksRemaining, parent, CRSFParameterSettingsEntry::TYPE_SELECT, hidden, label, data);
    
    if (!packet) return nullptr;
    
    // Add select values after label
    size_t valueStart = 7 + label.length() + 1;
    
    // Write current, min, max, default values
    data[valueStart] = currentIndex;
    data[valueStart + 1] = 0;  // min value
    data[valueStart + 2] = static_cast<uint8_t>(options.size() - 1);  // max value
    data[valueStart + 3] = 0;  // default value
    
    // Write options string
    size_t optionsStart = valueStart + 4;
    for (size_t i = 0; i < optionsStr.length(); ++i) {
        data[optionsStart + i] = static_cast<uint8_t>(optionsStr[i]);
    }
    data[optionsStart + optionsStr.length()] = 0;
    
    // Write units
    size_t unitsStart = optionsStart + optionsStr.length() + 1;
    for (size_t i = 0; i < units.length(); ++i) {
        data[unitsStart + i] = static_cast<uint8_t>(units[i]);
    }
    data[unitsStart + units.length()] = 0;
    
    // Update length and recalculate CRC
    packet->setLength(static_cast<uint8_t>(requiredSize - 3));
    packet->calculateCRC();
    
    return packet;
}

std::unique_ptr<CRSFParameterSettingsEntry> createStringParameter(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t chunksRemaining,
    uint8_t parent,
    bool hidden,
    const std::string& label,
    const std::string& value,
    uint8_t maxLength,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 4 + label.length() + 1 + value.length() + 1 + 1;  // +1 for maxLength
    if (data.size() < requiredSize) return nullptr;
    
    // Create base packet
    auto packet = createParameterSettingsEntry(address, extendedDestination, extendedSource,
        fieldIndex, chunksRemaining, parent, CRSFParameterSettingsEntry::TYPE_STRING, hidden, label, data);
    
    if (!packet) return nullptr;
    
    // Add string value after label
    size_t valueStart = 7 + label.length() + 1;
    
    // Write string value
    for (size_t i = 0; i < value.length(); ++i) {
        data[valueStart + i] = static_cast<uint8_t>(value[i]);
    }
    data[valueStart + value.length()] = 0;
    
    // Write max length
    data[valueStart + value.length() + 1] = maxLength;
    
    // Update length and recalculate CRC
    packet->setLength(static_cast<uint8_t>(requiredSize - 3));
    packet->calculateCRC();
    
    return packet;
}

std::unique_ptr<CRSFParameterSettingsEntry> createFolderParameter(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t chunksRemaining,
    uint8_t parent,
    bool hidden,
    const std::string& label,
    const std::string& displayName,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 4 + label.length() + 1 + displayName.length() + 1;
    if (data.size() < requiredSize) return nullptr;
    
    // Create base packet
    auto packet = createParameterSettingsEntry(address, extendedDestination, extendedSource,
        fieldIndex, chunksRemaining, parent, CRSFParameterSettingsEntry::TYPE_FOLDER, hidden, label, data);
    
    // Add display name after label
    size_t valueStart = 7 + label.length() + 1;
    
    // Write display name
    for (size_t i = 0; i < displayName.length(); ++i) {
        data[valueStart + i] = static_cast<uint8_t>(displayName[i]);
    }
    data[valueStart + displayName.length()] = 0;
    
    // Update length and recalculate CRC
    packet->setLength(static_cast<uint8_t>(requiredSize - 3));
    packet->calculateCRC();
    
    return packet;
}

std::unique_ptr<CRSFParameterSettingsEntry> createInfoParameter(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t chunksRemaining,
    uint8_t parent,
    bool hidden,
    const std::string& label,
    const std::string& displayText,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 4 + label.length() + 1 + displayText.length() + 1;
    if (data.size() < requiredSize) return nullptr;
    
    // Create base packet
    auto packet = createParameterSettingsEntry(address, extendedDestination, extendedSource,
        fieldIndex, chunksRemaining, parent, CRSFParameterSettingsEntry::TYPE_INFO, hidden, label, data);
    
    if (!packet) return nullptr;
    
    // Add display text after label
    size_t valueStart = 7 + label.length() + 1;
    
    // Write display text
    for (size_t i = 0; i < displayText.length(); ++i) {
        data[valueStart + i] = static_cast<uint8_t>(displayText[i]);
    }
    data[valueStart + displayText.length()] = 0;
    
    // Update length and recalculate CRC
    packet->setLength(static_cast<uint8_t>(requiredSize - 3));
    packet->calculateCRC();
    
    return packet;
}

std::unique_ptr<CRSFParameterSettingsEntry> createCommandParameter(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t chunksRemaining,
    uint8_t parent,
    bool hidden,
    const std::string& label,
    uint8_t step,
    uint8_t timeout,
    const std::string& info,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 4 + label.length() + 1 + 2 + info.length() + 1;  // +2 for step and timeout
    if (data.size() < requiredSize) return nullptr;
    
    // Create base packet
    auto packet = createParameterSettingsEntry(address, extendedDestination, extendedSource,
        fieldIndex, chunksRemaining, parent, CRSFParameterSettingsEntry::TYPE_COMMAND, hidden, label, data);
    
    if (!packet) return nullptr;
    
    // Add command values after label
    size_t valueStart = 7 + label.length() + 1;
    
    // Write step and timeout
    data[valueStart] = step;
    data[valueStart + 1] = timeout;
    
    // Write info string
    size_t infoStart = valueStart + 2;
    for (size_t i = 0; i < info.length(); ++i) {
        data[infoStart + i] = static_cast<uint8_t>(info[i]);
    }
    data[infoStart + info.length()] = 0;
    
    // Update length and recalculate CRC
    packet->setLength(static_cast<uint8_t>(requiredSize - 3));
    packet->calculateCRC();
    
    return packet;
}

// ============================================================================
// PARAMETER READ Factory Functions
// ============================================================================

std::unique_ptr<CRSFParameterRead> createParameterRead(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t chunkIndex,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 2 + 1;  // header + field_index + chunk_index + crc
    if (data.size() < requiredSize) return nullptr;
    
    // Set packet header
    data[0] = address;
    data[1] = static_cast<uint8_t>(requiredSize - 3);  // Length excluding header
    data[2] = static_cast<uint8_t>(PacketType::ParameterRead);
    
    // Set payload fields
    data[3] = fieldIndex;
    data[4] = chunkIndex;
    
    auto packet = std::make_unique<CRSFParameterRead>(data);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFParameterRead> createFolderRequest(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data) {
    
    return createParameterRead(address, extendedDestination, extendedSource, 
        CRSFParameterRead::FIELD_INDEX_FOLDER, 0, data);
}

std::unique_ptr<CRSFParameterRead> createParameterRequest(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    etl::span<uint8_t>& data) {
    
    return createParameterRead(address, extendedDestination, extendedSource, 
        fieldIndex, 0, data);  // Default to chunk 0
}

// ============================================================================
// PARAMETER WRITE Factory Functions
// ============================================================================

std::unique_ptr<CRSFParameterWrite> createParameterWrite(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 1 + 1;  // header + field_index + value_min(1) + crc
    if (data.size() < requiredSize) return nullptr;
    
    // Set packet header
    data[0] = address;
    data[1] = static_cast<uint8_t>(requiredSize - 3);  // Length excluding header
    data[2] = static_cast<uint8_t>(PacketType::ParameterWrite);
    
    // Set field index
    data[3] = fieldIndex;
    
    // Set default value (0)
    data[4] = 0;
    
    auto packet = std::make_unique<CRSFParameterWrite>(data);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFParameterWrite> createUint8ParameterWrite(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t value,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 1 + 1 + 1;  // header + field_index + value + crc
    if (data.size() < requiredSize) return nullptr;
    
    // Set packet header
    data[0] = address;
    data[1] = static_cast<uint8_t>(requiredSize - 3);  // Length excluding header
    data[2] = static_cast<uint8_t>(PacketType::ParameterWrite);
    
    // Set field index
    data[3] = fieldIndex;
    
    // Set value
    data[4] = value;
    
    auto packet = std::make_unique<CRSFParameterWrite>(data);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFParameterWrite> createUint16ParameterWrite(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint16_t value,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 1 + 2 + 1;  // header + field_index + value(2) + crc
    if (data.size() < requiredSize) return nullptr;
    
    // Set packet header
    data[0] = address;
    data[1] = static_cast<uint8_t>(requiredSize - 3);  // Length excluding header
    data[2] = static_cast<uint8_t>(PacketType::ParameterWrite);
    
    // Set field index
    data[3] = fieldIndex;
    
    // Set value (big-endian)
    data[4] = (value >> 8) & 0xFF;
    data[5] = value & 0xFF;
    
    auto packet = std::make_unique<CRSFParameterWrite>(data);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFParameterWrite> createUint32ParameterWrite(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint32_t value,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 1 + 4 + 1;  // header + field_index + value(4) + crc
    if (data.size() < requiredSize) return nullptr;
    
    // Set packet header
    data[0] = address;
    data[1] = static_cast<uint8_t>(requiredSize - 3);  // Length excluding header
    data[2] = static_cast<uint8_t>(PacketType::ParameterWrite);
    
    // Set field index
    data[3] = fieldIndex;
    
    // Set value (big-endian)
    data[4] = (value >> 24) & 0xFF;
    data[5] = (value >> 16) & 0xFF;
    data[6] = (value >> 8) & 0xFF;
    data[7] = value & 0xFF;
    
    auto packet = std::make_unique<CRSFParameterWrite>(data);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFParameterWrite> createStringParameterWrite(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    const std::string& value,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 1 + value.length() + 1 + 1;  // header + field_index + value + null + crc
    if (data.size() < requiredSize) return nullptr;
    
    // Set packet header
    data[0] = address;
    data[1] = static_cast<uint8_t>(requiredSize - 3);  // Length excluding header
    data[2] = static_cast<uint8_t>(PacketType::ParameterWrite);
    
    // Set field index
    data[3] = fieldIndex;
    
    // Set string value
    for (size_t i = 0; i < value.length(); ++i) {
        data[4 + i] = static_cast<uint8_t>(value[i]);
    }
    data[4 + value.length()] = 0;  // null terminator
    
    auto packet = std::make_unique<CRSFParameterWrite>(data);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFParameterWrite> createCommandParameterWrite(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t commandStep,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 1 + 1 + 1;  // header + field_index + command_step + crc
    if (data.size() < requiredSize) return nullptr;
    
    // Set packet header
    data[0] = address;
    data[1] = static_cast<uint8_t>(requiredSize - 3);  // Length excluding header
    data[2] = static_cast<uint8_t>(PacketType::ParameterWrite);
    
    // Set field index
    data[3] = fieldIndex;
    
    // Set command step
    data[4] = commandStep;
    
    auto packet = std::make_unique<CRSFParameterWrite>(data);
    packet->calculateCRC();
    return packet;
}

// ============================================================================
// EXPRESSLRS STATUS Factory Functions
// ============================================================================

std::unique_ptr<CRSFELRSStatus> createELRSStatus(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t packetsBad,
    uint16_t packetsGood,
    uint8_t flags,
    const std::string& message,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 4 + message.length() + 1 + 1;  // header + payload + message + null + crc
    if (data.size() < requiredSize) return nullptr;
    
    // Set packet header
    data[0] = address;
    data[1] = static_cast<uint8_t>(requiredSize - 3);  // Length excluding header
    data[2] = static_cast<uint8_t>(PacketType::ELRSStatus);
    
    // Set payload fields
    data[3] = packetsBad;
    data[4] = (packetsGood >> 8) & 0xFF;  // big-endian
    data[5] = packetsGood & 0xFF;
    data[6] = flags;
    
    // Set message
    for (size_t i = 0; i < message.length(); ++i) {
        data[7 + i] = static_cast<uint8_t>(message[i]);
    }
    data[7 + message.length()] = 0;  // null terminator
    
    auto packet = std::make_unique<CRSFELRSStatus>(data);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFELRSStatus> createConnectedStatus(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t packetsBad,
    uint16_t packetsGood,
    bool armed,
    const std::string& message,
    etl::span<uint8_t>& data) {
    
    uint8_t flags = CRSFELRSStatus::FLAG_CONNECTED;
    if (armed) {
        flags |= CRSFELRSStatus::FLAG_ARMED;
    }
    
    return createELRSStatus(address, extendedDestination, extendedSource, 
        packetsBad, packetsGood, flags, message, data);
}

std::unique_ptr<CRSFELRSStatus> createWarningStatus(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t packetsBad,
    uint16_t packetsGood,
    uint8_t warningFlags,
    const std::string& warningMessage,
    etl::span<uint8_t>& data) {
    
    return createELRSStatus(address, extendedDestination, extendedSource, 
        packetsBad, packetsGood, warningFlags, warningMessage, data);
}

// ============================================================================
// CRSF COMMAND Factory Functions
// ============================================================================

std::unique_ptr<CRSFCommand> createCommand(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t commandRealm,
    uint8_t subcommand,
    const etl::span<uint8_t>& payload,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 2 + payload.size() + 2;  // header + realm + subcommand + payload + crc + crc8-ba
    if (data.size() < requiredSize) return nullptr;
    
    // Set packet header
    data[0] = address;
    data[1] = static_cast<uint8_t>(requiredSize - 3);  // Length excluding header
    data[2] = static_cast<uint8_t>(PacketType::Command);
    
    // Set command fields
    data[3] = commandRealm;
    data[4] = subcommand;
    
    // Copy payload
    for (size_t i = 0; i < payload.size(); ++i) {
        data[5 + i] = payload[i];
    }
    
    auto packet = std::make_unique<CRSFCommand>(data);
    packet->calculateCRC();
    packet->calculateCRC8BA();
    return packet;
}

std::unique_ptr<CRSFCommand> createReceiverBindCommand(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data) {
    
    // Bind command has no additional payload
    etl::span<uint8_t> emptyPayload;
    return createCommand(address, extendedDestination, extendedSource, 
        CRSFCommand::REALM_RX, CRSFCommand::SUBCMD_RX_BIND, emptyPayload, data);
}

std::unique_ptr<CRSFCommand> createModelSelectCommand(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t receiverID,
    etl::span<uint8_t>& data) {
    
    // Model select command payload is just the receiver ID
    std::vector<uint8_t> payload = {receiverID};
    etl::span<uint8_t> payloadSpan(payload);
    
    return createCommand(address, extendedDestination, extendedSource, 
        CRSFCommand::REALM_RX, CRSFCommand::SUBCMD_MODEL_SELECT_ID, payloadSpan, data);
}

std::unique_ptr<CRSFCommand> createSpeedProposalCommand(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t portID,
    uint32_t proposedBaudRate,
    etl::span<uint8_t>& data) {
    
    // Speed proposal payload: portID + baudRate(4 bytes, big-endian)
    std::vector<uint8_t> payload(5);
    payload[0] = portID;
    payload[1] = (proposedBaudRate >> 24) & 0xFF;
    payload[2] = (proposedBaudRate >> 16) & 0xFF;
    payload[3] = (proposedBaudRate >> 8) & 0xFF;
    payload[4] = proposedBaudRate & 0xFF;
    
    etl::span<uint8_t> payloadSpan(payload);
    
    return createCommand(address, extendedDestination, extendedSource, 
        CRSFCommand::REALM_GENERAL, CRSFCommand::SUBCMD_GENERAL_CRSF_SPEED_PROPOSAL, payloadSpan, data);
}

std::unique_ptr<CRSFCommand> createSpeedResponseCommand(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t portID,
    bool accepted,
    etl::span<uint8_t>& data) {
    
    // Speed response payload: portID + accept(1 byte)
    std::vector<uint8_t> payload = {portID, static_cast<uint8_t>(accepted ? 1 : 0)};
    etl::span<uint8_t> payloadSpan(payload);
    
    return createCommand(address, extendedDestination, extendedSource, 
        CRSFCommand::REALM_GENERAL, CRSFCommand::SUBCMD_GENERAL_CRSF_SPEED_RESPONSE, payloadSpan, data);
}

// ============================================================================
// CRSF RADIO ID Factory Functions
// ============================================================================

std::unique_ptr<CRSFRadioID> createRadioID(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t subtype,
    const etl::span<uint8_t>& subtypeData,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 1 + subtypeData.size() + 1;  // header + subtype + data + crc
    if (data.size() < requiredSize) return nullptr;
    
    // Set packet header
    data[0] = address;
    data[1] = static_cast<uint8_t>(requiredSize - 3);  // Length excluding header
    data[2] = static_cast<uint8_t>(PacketType::RadioID);
    
    // Set subtype
    data[3] = subtype;
    
    // Copy subtype data
    for (size_t i = 0; i < subtypeData.size(); ++i) {
        data[4 + i] = subtypeData[i];
    }
    
    auto packet = std::make_unique<CRSFRadioID>(data);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFOpenTXSync> createOpenTXSync(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    int32_t packetInterval,
    int32_t phaseShiftCorrection,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 1 + 8 + 1;  // header + subtype + payload(8) + crc
    if (data.size() < requiredSize) return nullptr;
    
    // Set packet header
    data[0] = address;
    data[1] = static_cast<uint8_t>(requiredSize - 3);  // Length excluding header
    data[2] = static_cast<uint8_t>(PacketType::RadioID);
    
    // Set subtype
    data[3] = CRSFRadioID::SUBTYPE_OPENTX_SYNC;
    
    // Set packet interval (big-endian)
    data[4] = (packetInterval >> 24) & 0xFF;
    data[5] = (packetInterval >> 16) & 0xFF;
    data[6] = (packetInterval >> 8) & 0xFF;
    data[7] = packetInterval & 0xFF;
    
    // Set phase shift correction (big-endian)
    data[8] = (phaseShiftCorrection >> 24) & 0xFF;
    data[9] = (phaseShiftCorrection >> 16) & 0xFF;
    data[10] = (phaseShiftCorrection >> 8) & 0xFF;
    data[11] = phaseShiftCorrection & 0xFF;
    
    auto packet = std::make_unique<CRSFOpenTXSync>(data);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFOpenTXSync> createOpenTXSyncHz(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    double packetIntervalHz,
    int32_t phaseShiftCorrection,
    etl::span<uint8_t>& data) {
    
    // Convert Hz to μs×10
    int32_t packetInterval = 0;
    if (packetIntervalHz > 0.0) {
        packetInterval = static_cast<int32_t>(std::round(10000000.0 / packetIntervalHz));
    }
    
    return createOpenTXSync(address, extendedDestination, extendedSource, 
        packetInterval, phaseShiftCorrection, data);
}

// ============================================================================
// CRSF MSP REQUEST Factory Functions
// ============================================================================

std::unique_ptr<CRSFMSPReq> createMSPReq(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint16_t mspFlags,
    uint16_t function,
    const etl::span<uint8_t>& payload,
    etl::span<uint8_t>& data) {
    
    // Calculate required size
    size_t requiredSize = 3 + 6 + payload.size() + 1;  // header + flags + function + length + payload + crc
    if (data.size() < requiredSize) return nullptr;
    
    // Set packet header
    data[0] = address;
    data[1] = static_cast<uint8_t>(requiredSize-2);  // Length: as required by CRSF common spec
    data[2] = static_cast<uint8_t>(PacketType::MSPReq);
    
    // Set MSP fields (big-endian)
    data[3] = (mspFlags >> 8) & 0xFF;
    data[4] = mspFlags & 0xFF;
    data[5] = (function >> 8) & 0xFF;
    data[6] = function & 0xFF;
    data[7] = (payload.size() >> 8) & 0xFF;
    data[8] = payload.size() & 0xFF;
    
    // Copy payload
    for (size_t i = 0; i < payload.size(); ++i) {
        data[9 + i] = payload[i];
    }
    
    auto packet = std::make_unique<CRSFMSPReq>(data);
    packet->calculateCRC();
    return packet;
}

std::unique_ptr<CRSFMSPReq> createVTXConfigRequest(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data) {
    
    // VTX config request has no additional payload
    etl::span<uint8_t> emptyPayload;
    uint16_t mspFlags = 0x3300;  // No error, version 3, beginning of frame, first frame
    
    return createMSPReq(address, extendedDestination, extendedSource, 
        mspFlags, CRSFMSPReq::FUNCTION_VTX_CONFIG, emptyPayload, data);
}

std::unique_ptr<CRSFMSPReq> createOSDConfigRequest(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data) {
    
    // OSD config request has no additional payload
    etl::span<uint8_t> emptyPayload;
    uint16_t mspFlags = 0x3300;  // No error, version 3, beginning of frame, first frame
    
    return createMSPReq(address, extendedDestination, extendedSource, 
        mspFlags, CRSFMSPReq::FUNCTION_OSD_CONFIG, emptyPayload, data);
}

std::unique_ptr<CRSFMSPReq> createBetaflightConfigRequest(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data) {
    
    // Betaflight config request has no additional payload
    etl::span<uint8_t> emptyPayload;
    static constexpr uint16_t mspFlags = 0x3300;  // No error, version 3, beginning of frame, first frame
    
    return createMSPReq(address, extendedDestination, extendedSource, 
        mspFlags, CRSFMSPReq::FUNCTION_BF_CONFIG, emptyPayload, data);
}

std::unique_ptr<CRSFPacket> createGenericPacket(
    uint8_t sync,           // Changed from 'address' to 'sync' (0xC8 or 0xEE)
    PacketType type,
    const etl::span<uint8_t>& payload,
    etl::span<uint8_t>& data) {
    
    // Set the packet header according to CRSF spec
    data[0] = sync;                                    // SYNC byte
    data[1] = static_cast<uint8_t>(payload.size() + 2); // LEN: type(1) + payload + crc(1) = payload.size() + 2
    data[2] = static_cast<uint8_t>(type);              // TYPE
    
    // Copy payload if provided
    if (!payload.empty()) {
        std::copy(payload.begin(), payload.end(), data.begin() + 3);
    }
    
    // Calculate CRC directly to the data buffer
    size_t crcPos = 3 + payload.size();
    uint8_t crc = calculateCRC8(data.data(), crcPos);
    data[crcPos] = crc;
    
    // Create packet from the completed data
    auto packet = std::make_unique<CRSFPacket>(data);
    
    return packet;
}

}

*/