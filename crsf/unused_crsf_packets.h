
/*
namespace crsf {


// GPS Data packet wrapper
class CRSFGPSData : public CRSFPacket {
    public:
        CRSFGPSData(const etl::span<uint8_t>& data);
        
        // GPS data access methods
        uint32_t getLatitude() const;
        void setLatitude(uint32_t value);
        
        uint32_t getLongitude() const;
        void setLongitude(uint32_t value);
        
        uint16_t getGroundSpeed() const;
        void setGroundSpeed(uint16_t value);
        
        uint16_t getGroundCourse() const;
        void setGroundCourse(uint16_t value);
        
        uint16_t getAltitude() const;
        void setAltitude(uint16_t value);
        
        uint8_t getSatellites() const;
        void setSatellites(uint8_t value);
        
        // Utility methods
        double getLatitudeDegrees() const;
        void setLatitudeDegrees(double degrees);
        double getLongitudeDegrees() const;
        void setLongitudeDegrees(double degrees);
        
        // Validation
        bool isValid() const;
        
        // Constants
        static constexpr size_t PAYLOAD_SIZE = 15;
        static constexpr double LAT_LON_SCALE = 10000000.0;
    };


// Vario packet wrapper
class CRSFVario : public CRSFPacket {
    public:
        CRSFVario(const etl::span<uint8_t>& data);
        
        // Vario data access methods
        int16_t getVerticalSpeed() const;
        void setVerticalSpeed(int16_t value);
        
        // Utility methods for different units
        double getVerticalSpeedMPS() const;  // meters per second
        void setVerticalSpeedMPS(double mps); // meters per second
        
        // Validation
        bool isValid() const;
        
        // Constants
        static constexpr size_t PAYLOAD_SIZE = 2;
        static constexpr double SPEED_SCALE = 100.0;  // cm/s to m/s conversion
    };


// Battery Sensor packet wrapper
class CRSFBatterySensor : public CRSFPacket {
    public:
        CRSFBatterySensor(const etl::span<uint8_t>& data);
        
        // Battery data access methods
        int16_t getVoltage() const;
        void setVoltage(int16_t value);
        
        int16_t getCurrent() const;
        void setCurrent(int16_t value);
        
        int32_t getCapacity() const;
        void setCapacity(int32_t value);
        
        int8_t getRemaining() const;
        void setRemaining(int8_t value);
        
        // Utility methods
        double getVoltageVolts() const;
        void setVoltageVolts(double volts);
        double getCurrentAmps() const;
        void setCurrentAmps(double amps);
        
        // Validation
        bool isValid() const;
        
        // Constants
        static constexpr size_t PAYLOAD_SIZE = 8;
        static constexpr double VOLTAGE_SCALE = 100.0;
        static constexpr double CURRENT_SCALE = 100.0;
    };


// Baro Altitude packet wrapper
class CRSFBaroAltitude : public CRSFPacket {
    public:
        CRSFBaroAltitude(const etl::span<uint8_t>& data);
        
        // Altitude access methods
        double getAltitudeMeters() const;
        void setAltitudeMeters(double altitudeMeters);
        
        // Raw altitude value access
        uint16_t getRawAltitude() const;
        void setRawAltitude(uint16_t rawAltitude);
        
        // Vertical speed access methods (optional)
        bool hasVerticalSpeed() const;
        int16_t getVerticalSpeed() const;
        void setVerticalSpeed(int16_t value);
        
        // Utility methods for different units
        double getVerticalSpeedMPS() const;  // meters per second
        void setVerticalSpeedMPS(double mps); // meters per second
        
        // Altitude encoding helpers
        static uint16_t encodeAltitude(double altitudeMeters);
        static double decodeAltitude(uint16_t rawAltitude);
        
        // Validation
        bool isValid() const;
        
        // Constants
        static constexpr size_t MIN_PAYLOAD_SIZE = 2;  // altitude only
        static constexpr size_t MAX_PAYLOAD_SIZE = 4;  // altitude + vertical speed
        static constexpr double ALTITUDE_OFFSET = 10000.0;  // decimeters offset
        static constexpr double ALTITUDE_SCALE = 10.0;      // decimeters to meters
        static constexpr double SPEED_SCALE = 100.0;        // cm/s to m/s conversion
        static constexpr uint16_t HIGH_BIT_MASK = 0x8000;   // high bit for altitude mode
    };
    

// Heartbeat packet wrapper
class CRSFHeartbeat : public CRSFPacket {
    public:
        CRSFHeartbeat(const etl::span<uint8_t>& data);
        
        // Device address access methods
        uint16_t getOriginDeviceAddress() const;
        void setOriginDeviceAddress(uint16_t address);
        
        // Utility methods for common device addresses
        void setFlightController();
        void setRadio();
        void setReceiver();
        void setGimbal();
        void setBlackbox();
        void setESC();
        void setCamera();
        void setVTX();
        void setGPS();
        void setBattery();
        
        // Validation
        bool isValid() const;
        
        // Constants
        static constexpr size_t PAYLOAD_SIZE = 2;  // device address (2 bytes)
        
        // Standard CRSF device addresses
        static constexpr uint16_t ADDRESS_FLIGHT_CONTROLLER = 0xC8;  // 200
        static constexpr uint16_t ADDRESS_RADIO = 0xEE;              // 238
        static constexpr uint16_t ADDRESS_RECEIVER = 0xEC;           // 236
        static constexpr uint16_t ADDRESS_GIMBAL = 0xC2;            // 194
        static constexpr uint16_t ADDRESS_BLACKBOX = 0xC4;          // 196
        static constexpr uint16_t ADDRESS_ESC = 0xC6;               // 198
        static constexpr uint16_t ADDRESS_CAMERA = 0xCA;            // 202
        static constexpr uint16_t ADDRESS_VTX = 0xCC;               // 204
        static constexpr uint16_t ADDRESS_GPS = 0xCE;               // 206
        static constexpr uint16_t ADDRESS_BATTERY = 0xD0;           // 208
    };
    
    

// Link Statistics packet wrapper
class CRSFLinkStatistics : public CRSFPacket {
    public:
        CRSFLinkStatistics(const etl::span<uint8_t>& data);
        
        // Link statistics access methods
        uint8_t getUplinkRSSI1() const;
        void setUplinkRSSI1(uint8_t value);
        
        uint8_t getUplinkRSSI2() const;
        void setUplinkRSSI2(uint8_t value);
        
        uint8_t getUplinkLinkQuality() const;
        void setUplinkLinkQuality(uint8_t value);
        
        int8_t getUplinkSNR() const;
        void setUplinkSNR(int8_t value);
        
        uint8_t getActiveAntenna() const;
        void setActiveAntenna(uint8_t value);
        
        uint8_t getRFMode() const;
        void setRFMode(uint8_t value);
        
        uint8_t getUplinkTxPower() const;
        void setUplinkTxPower(uint8_t value);
        
        uint8_t getDownlinkRSSI() const;
        void setDownlinkRSSI(uint8_t value);
        
        uint8_t getDownlinkLinkQuality() const;
        void setDownlinkLinkQuality(uint8_t value);
        
        int8_t getDownlinkSNR() const;
        void setDownlinkSNR(int8_t value);
        
        // Utility methods for RSSI conversion
        int8_t getUplinkRSSI1dBm() const;  // Convert to actual dBm
        void setUplinkRSSI1dBm(int8_t dbm); // Set from actual dBm
        int8_t getUplinkRSSI2dBm() const;  // Convert to actual dBm
        void setUplinkRSSI2dBm(int8_t dbm); // Set from actual dBm
        int8_t getDownlinkRSSIdBm() const;  // Convert to actual dBm
        void setDownlinkRSSIdBm(int8_t dbm); // Set from actual dBm
        
        // Utility methods for TX Power conversion
        uint16_t getUplinkTxPowerMW() const;  // Convert to milliwatts
        void setUplinkTxPowerMW(uint16_t mw); // Set from milliwatts
        
        // Validation
        bool isValid() const;
        
        // Constants
        static constexpr size_t PAYLOAD_SIZE = 10;
        
        // Standard CRSF values
        static constexpr uint8_t ANTENNA_1 = 0;
        static constexpr uint8_t ANTENNA_2 = 1;
        
        // TX Power enum values (from CRSF spec)
        static constexpr uint8_t TX_POWER_0MW = 0;
        static constexpr uint8_t TX_POWER_10MW = 1;
        static constexpr uint8_t TX_POWER_25MW = 2;
        static constexpr uint8_t TX_POWER_100MW = 3;
        static constexpr uint8_t TX_POWER_500MW = 4;
        static constexpr uint8_t TX_POWER_1000MW = 5;
        static constexpr uint8_t TX_POWER_2000MW = 6;
        static constexpr uint8_t TX_POWER_50MW = 7;
    };
    
// Link RX ID packet wrapper (CRSFv3)
class CRSFLinkRXID : public CRSFPacket {
public:
    CRSFLinkRXID(const etl::span<uint8_t>& data);
    
    // Downlink RSSI dBm methods
    uint8_t getDownlinkRSSIdBm() const;
    void setDownlinkRSSIdBm(uint8_t rssi);
    
    // Downlink RSSI percent methods
    uint8_t getDownlinkRSSIPercent() const;
    void setDownlinkRSSIPercent(uint8_t percent);
    
    // Downlink Link Quality methods
    uint8_t getDownlinkLinkQuality() const;
    void setDownlinkLinkQuality(uint8_t quality);
    
    // Downlink SNR methods
    int8_t getDownlinkSNR() const;
    void setDownlinkSNR(int8_t snr);
    
    // Uplink RF Power Index methods
    uint8_t getUplinkRFPowerIndex() const;
    void setUplinkRFPowerIndex(uint8_t powerIndex);
    
    // Utility methods for RSSI conversion
    int8_t getDownlinkRSSIdBmSigned() const;
    void setDownlinkRSSIdBmSigned(int8_t dbm);
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t PAYLOAD_SIZE = 5;  // 5 bytes: RSSI_dBm + RSSI_percent + Link_Quality + SNR + RF_Power_Index
    
    // RSSI percent range
    static constexpr uint8_t RSSI_PERCENT_MIN = 0;
    static constexpr uint8_t RSSI_PERCENT_MAX = 100;
    
    // Link Quality range
    static constexpr uint8_t LINK_QUALITY_MIN = 0;
    static constexpr uint8_t LINK_QUALITY_MAX = 100;
    
    // SNR range (typical values)
    static constexpr int8_t SNR_MIN = -20;
    static constexpr int8_t SNR_MAX = 20;
    
    // RF Power Index range
    static constexpr uint8_t RF_POWER_INDEX_MIN = 0;
    static constexpr uint8_t RF_POWER_INDEX_MAX = 7;
};

// Link TX ID packet wrapper (CRSFv3)
class CRSFLinkTXID : public CRSFPacket {
public:
    CRSFLinkTXID(const etl::span<uint8_t>& data);
    
    // Uplink RSSI dBm methods
    uint8_t getUplinkRSSIdBm() const;
    void setUplinkRSSIdBm(uint8_t rssi);
    
    // Uplink RSSI percent methods
    uint8_t getUplinkRSSIPercent() const;
    void setUplinkRSSIPercent(uint8_t percent);
    
    // Uplink Link Quality methods
    uint8_t getUplinkLinkQuality() const;
    void setUplinkLinkQuality(uint8_t quality);
    
    // Uplink SNR methods
    int8_t getUplinkSNR() const;
    void setUplinkSNR(int8_t snr);
    
    // Downlink RF Power methods
    uint8_t getDownlinkRFPower() const;
    void setDownlinkRFPower(uint8_t power);
    
    // Packet Rate FPS methods
    uint8_t getPacketRateFPS() const;
    void setPacketRateFPS(uint8_t fps);
    
    // Utility methods for RSSI conversion
    int8_t getUplinkRSSIdBmSigned() const;
    void setUplinkRSSIdBmSigned(int8_t dbm);
    
    // Utility methods for FPS conversion
    uint16_t getPacketRateFPSRaw() const;  // Returns FPS * 10
    void setPacketRateFPSRaw(uint16_t fps); // Sets FPS * 10
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t PAYLOAD_SIZE = 6;  // 6 bytes: RSSI_dBm + RSSI_percent + Link_Quality + SNR + RF_Power + FPS
    
    // RSSI percent range
    static constexpr uint8_t RSSI_PERCENT_MIN = 0;
    static constexpr uint8_t RSSI_PERCENT_MAX = 100;
    
    // Link Quality range
    static constexpr uint8_t LINK_QUALITY_MIN = 0;
    static constexpr uint8_t LINK_QUALITY_MAX = 100;
    
    // SNR range (typical values)
    static constexpr int8_t SNR_MIN = -20;
    static constexpr int8_t SNR_MAX = 20;
    
    // RF Power range
    static constexpr uint8_t RF_POWER_MIN = 0;
    static constexpr uint8_t RF_POWER_MAX = 255;
    
    // Packet Rate FPS range (FPS * 10)
    static constexpr uint8_t FPS_MIN = 0;      // 0 Hz
    static constexpr uint8_t FPS_MAX = 100;    // 1000 Hz (0x64)
    
    // Common FPS values
    static constexpr uint8_t FPS_50HZ = 5;     // 50 Hz
    static constexpr uint8_t FPS_150HZ = 15;   // 150 Hz
    static constexpr uint8_t FPS_333HZ = 33;   // 333 Hz
    static constexpr uint8_t FPS_1000HZ = 100; // 1000 Hz
};

// Attitude packet wrapper
class CRSFAttitude : public CRSFPacket {
public:
    CRSFAttitude(const etl::span<uint8_t>& data);
    
    // Pitch angle methods (radians/10000)
    int16_t getPitch() const;
    void setPitch(int16_t pitch);
    
    // Roll angle methods (radians/10000)
    int16_t getRoll() const;
    void setRoll(int16_t roll);
    
    // Yaw angle methods (radians/10000)
    int16_t getYaw() const;
    void setYaw(int16_t yaw);
    
    // Utility methods for angle conversion
    double getPitchDegrees() const;
    void setPitchDegrees(double degrees);
    double getRollDegrees() const;
    void setRollDegrees(double degrees);
    double getYawDegrees() const;
    void setYawDegrees(double degrees);
    
    double getPitchRadians() const;
    void setPitchRadians(double radians);
    double getRollRadians() const;
    void setRollRadians(double radians);
    double getYawRadians() const;
    void setYawRadians(double radians);
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t PAYLOAD_SIZE = 6;  // 6 bytes: pitch(2) + roll(2) + yaw(2)
    
    // Angle range constants (radians/10000)
    static constexpr int16_t ANGLE_MIN = -31415;  // -π radians × 10000
    static constexpr int16_t ANGLE_MAX = 31415;   // +π radians × 10000
    
    // Conversion constants
    static constexpr double RADIANS_TO_DEGREES = 180.0 / M_PI;
    static constexpr double DEGREES_TO_RADIANS = M_PI / 180.0;
    static constexpr double STORED_TO_RADIANS = 1.0 / 10000.0;
    static constexpr double RADIANS_TO_STORED = 10000.0;
    
    // Common angle values (radians/10000)
    static constexpr int16_t ANGLE_0_DEG = 0;           // 0°
    static constexpr int16_t ANGLE_45_DEG = 7853;       // 45° = π/4 × 10000
    static constexpr int16_t ANGLE_90_DEG = 15708;      // 90° = π/2 × 10000
    static constexpr int16_t ANGLE_180_DEG = 31415;     // 180° = π × 10000
    static constexpr int16_t ANGLE_NEG_45_DEG = -7853;  // -45° = -π/4 × 10000
    static constexpr int16_t ANGLE_NEG_90_DEG = -15708; // -90° = -π/2 × 10000
    static constexpr int16_t ANGLE_NEG_180_DEG = -31415; // -180° = -π × 10000
};

// Flight Mode packet wrapper
class CRSFFlightMode : public CRSFPacket {
public:
    CRSFFlightMode(const etl::span<uint8_t>& data);
    
    // Flight mode string methods
    std::string getFlightMode() const;
    void setFlightMode(const std::string& mode);
    
    // Utility methods
    size_t getFlightModeLength() const;
    bool isEmpty() const;
    bool isAcro() const;
    bool isHorizon() const;
    bool isAngle() const;
    bool isRTH() const;
    bool isLand() const;
    bool isTakeoff() const;
    bool isManual() const;
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t MAX_STRING_LENGTH = 13;  // Max characters (excluding null terminator)
    static constexpr size_t PAYLOAD_SIZE = 14;       // 14 bytes: string(13) + null(1)
    
    // Common flight mode strings
    static constexpr const char* MODE_ACRO = "ACRO";
    static constexpr const char* MODE_HORIZON = "HORIZON";
    static constexpr const char* MODE_ANGLE = "ANGLE";
    static constexpr const char* MODE_RTH = "RTH";
    static constexpr const char* MODE_LAND = "LAND";
    static constexpr const char* MODE_TAKEOFF = "TAKEOFF";
    static constexpr const char* MODE_MANUAL = "MANUAL";
    static constexpr const char* MODE_ALTHOLD = "ALTHOLD";
    static constexpr const char* MODE_LOITER = "LOITER";
    static constexpr const char* MODE_WAYPOINT = "WAYPOINT";
    static constexpr const char* MODE_CRUISE = "CRUISE";
    static constexpr const char* MODE_CIRCLE = "CIRCLE";
    static constexpr const char* MODE_EMERGENCY = "EMERGENCY";
};

// Device Ping packet wrapper
class CRSFDevicePing : public CRSFPacket {
public:
    CRSFDevicePing(const etl::span<uint8_t>& data);
    
    // Extended destination methods (target device)
    uint8_t getExtendedDestination() const;
    void setExtendedDestination(uint8_t destination);
    
    // Extended source methods (source device)
    uint8_t getExtendedSource() const;
    void setExtendedSource(uint8_t source);
    
    // Utility methods
    bool isBroadcast() const;
    bool isTargeted() const;
    bool isFromRadio() const;
    bool isFromFlightController() const;
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t PAYLOAD_SIZE = 2;  // 2 bytes: extended_destination + extended_source
    
    // Common addresses
    static constexpr uint8_t ADDRESS_BROADCAST = 0x00;
    static constexpr uint8_t ADDRESS_RADIO_TRANSMITTER = 0xEA;
    static constexpr uint8_t ADDRESS_FLIGHT_CONTROLLER = 0xC8;
    static constexpr uint8_t ADDRESS_RECEIVER = 0xEC;
    static constexpr uint8_t ADDRESS_GIMBAL = 0xC2;
    static constexpr uint8_t ADDRESS_BLACKBOX = 0xC4;
    static constexpr uint8_t ADDRESS_ESC = 0xC6;
    static constexpr uint8_t ADDRESS_CAMERA = 0xCA;
    static constexpr uint8_t ADDRESS_VTX = 0xCC;
    static constexpr uint8_t ADDRESS_GPS = 0xCE;
    static constexpr uint8_t ADDRESS_BATTERY = 0xD0;
};

// Device Info packet wrapper
class CRSFDeviceInfo : public CRSFPacket {
public:
    CRSFDeviceInfo(const etl::span<uint8_t>& data);
    
    // Display name methods
    std::string getDisplayName() const;
    void setDisplayName(const std::string& name);
    size_t getDisplayNameLength() const;
    bool isDisplayNameEmpty() const;
    
    // Serial number methods
    std::string getSerialNumber() const;
    void setSerialNumber(const std::string& serial);
    bool isELRSDevice() const;
    
    // Version methods
    uint32_t getHardwareVersion() const;
    void setHardwareVersion(uint32_t version);
    uint32_t getSoftwareVersion() const;
    void setSoftwareVersion(uint32_t version);
    
    // Software version parsing (ELRSv3 format: 00 MAJ MIN REV)
    uint8_t getSoftwareVersionMajor() const;
    uint8_t getSoftwareVersionMinor() const;
    uint8_t getSoftwareVersionRevision() const;
    void setSoftwareVersion(uint8_t major, uint8_t minor, uint8_t revision);
    
    // Configuration methods
    uint8_t getConfigParameterCount() const;
    void setConfigParameterCount(uint8_t count);
    uint8_t getParameterProtocolVersion() const;
    void setParameterProtocolVersion(uint8_t version);
    
    // Utility methods
    bool hasConfigParameters() const;
    bool isExpressLRSDevice() const;
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t MAX_DISPLAY_NAME_LENGTH = 15;  // ExpressLRS limitation
    static constexpr size_t SERIAL_NUMBER_LENGTH = 4;
    static constexpr size_t PAYLOAD_SIZE = 30;  // display_name(16) + serial(4) + hw_ver(4) + sw_ver(4) + param_count(1) + protocol_ver(1)
    
    // Common serial numbers
    static constexpr const char* SERIAL_ELRS = "ELRS";
    static constexpr const char* SERIAL_EDGETX = "EDGE";
    
    // Common display names
    static constexpr const char* DISPLAY_NAME_ELRS_TX = "ELRS TX";
    static constexpr const char* DISPLAY_NAME_ELRS_RX = "ELRS RX";
    static constexpr const char* DISPLAY_NAME_FLIGHT_CONTROLLER = "FC";
    static constexpr const char* DISPLAY_NAME_RECEIVER = "RX";
    static constexpr const char* DISPLAY_NAME_GPS = "GPS";
    static constexpr const char* DISPLAY_NAME_BATTERY = "BAT";
};

// Parameter Settings Entry packet wrapper
class CRSFParameterSettingsEntry : public CRSFPacket {
public:
    CRSFParameterSettingsEntry(const etl::span<uint8_t>& data);
    
    // Basic field methods
    uint8_t getFieldIndex() const;
    void setFieldIndex(uint8_t index);
    uint8_t getChunksRemaining() const;
    void setChunksRemaining(uint8_t chunks);
    uint8_t getParent() const;
    void setParent(uint8_t parent);
    uint8_t getParameterType() const;
    void setParameterType(uint8_t type);
    bool isHidden() const;
    void setHidden(bool hidden);
    
    // Label methods
    std::string getLabel() const;
    void setLabel(const std::string& label);
    size_t getLabelLength() const;
    
    // Type-specific value methods
    // Integer types (0x00-0x07)
    template<typename T>
    T getCurrentValue() const;
    template<typename T>
    T getMinValue() const;
    template<typename T>
    T getMaxValue() const;
    template<typename T>
    T getDefaultValue() const;
    std::string getUnits() const;
    
    // Float type (0x08)
    float getFloatCurrentValue() const;
    float getFloatMinValue() const;
    float getFloatMaxValue() const;
    uint8_t getFloatPrecision() const;
    uint32_t getFloatStep() const;
    
    // Select type (0x09)
    uint8_t getSelectCurrentIndex() const;
    std::vector<std::string> getSelectOptions() const;
    std::string getSelectCurrentOption() const;
    
    // String type (0x0A)
    std::string getStringValue() const;
    uint8_t getStringMaxLength() const;
    
    // Folder type (0x0B)
    std::string getFolderDisplayName() const;
    
    // Info type (0x0C)
    std::string getInfoDisplay() const;
    
    // Command type (0x0D)
    uint8_t getCommandStep() const;
    uint8_t getCommandTimeout() const;
    std::string getCommandInfo() const;
    
    // Utility methods
    bool isTopLevel() const;
    bool hasParent() const;
    bool isIntegerType() const;
    bool isFloatType() const;
    bool isSelectType() const;
    bool isStringType() const;
    bool isFolderType() const;
    bool isInfoType() const;
    bool isCommandType() const;
    
    // Validation
    bool isValid() const;
    
    // Helper methods
    size_t getTypeSize() const;
    
    // Constants
    static constexpr size_t MIN_PAYLOAD_SIZE = 5;  // field_index + chunks + parent + type + label_min
    
    // Data type constants
    static constexpr uint8_t TYPE_UINT8 = 0x00;
    static constexpr uint8_t TYPE_INT8 = 0x01;
    static constexpr uint8_t TYPE_UINT16 = 0x02;
    static constexpr uint8_t TYPE_INT16 = 0x03;
    static constexpr uint8_t TYPE_UINT32 = 0x04;
    static constexpr uint8_t TYPE_INT32 = 0x05;
    static constexpr uint8_t TYPE_UINT64 = 0x06;
    static constexpr uint8_t TYPE_INT64 = 0x07;
    static constexpr uint8_t TYPE_FLOAT = 0x08;
    static constexpr uint8_t TYPE_SELECT = 0x09;
    static constexpr uint8_t TYPE_STRING = 0x0A;
    static constexpr uint8_t TYPE_FOLDER = 0x0B;
    static constexpr uint8_t TYPE_INFO = 0x0C;
    static constexpr uint8_t TYPE_COMMAND = 0x0D;
    static constexpr uint8_t TYPE_VTX = 0x0F;
    
    // Command step constants
    static constexpr uint8_t COMMAND_IDLE = 0;
    static constexpr uint8_t COMMAND_CLICK = 1;
    static constexpr uint8_t COMMAND_EXECUTING = 2;
    static constexpr uint8_t COMMAND_ASKCONFIRM = 3;
    static constexpr uint8_t COMMAND_CONFIRMED = 4;
    static constexpr uint8_t COMMAND_CANCEL = 5;
    static constexpr uint8_t COMMAND_QUERY = 6;
    
    // Hidden flag
    static constexpr uint8_t HIDDEN_FLAG = 0x80;
    static constexpr uint8_t TYPE_MASK = 0x7F;
};

// Parameter Read packet wrapper
class CRSFParameterRead : public CRSFPacket {
public:
    CRSFParameterRead(const etl::span<uint8_t>& data);
    
    // Field index methods
    uint8_t getFieldIndex() const;
    void setFieldIndex(uint8_t index);
    
    // Chunk index methods
    uint8_t getChunkIndex() const;
    void setChunkIndex(uint8_t chunk);
    
    // Utility methods
    bool isFolderRequest() const;
    bool isParameterRequest() const;
    bool isTopLevelRequest() const;
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t PAYLOAD_SIZE = 2;  // field_index + chunk_index
    
    // Special field indices
    static constexpr uint8_t FIELD_INDEX_FOLDER = 0x00;  // Top-level folder
    static constexpr uint8_t FIELD_INDEX_FIRST_PARAMETER = 0x01;  // First actual parameter
};

// Parameter Write packet wrapper
class CRSFParameterWrite : public CRSFPacket {
public:
    CRSFParameterWrite(const etl::span<uint8_t>& data);
    
    // Field index methods
    uint8_t getFieldIndex() const;
    void setFieldIndex(uint8_t index);
    
    // Value methods
    uint8_t getUint8Value() const;
    void setUint8Value(uint8_t value);
    uint16_t getUint16Value() const;
    void setUint16Value(uint16_t value);
    uint32_t getUint32Value() const;
    void setUint32Value(uint32_t value);
    std::string getStringValue() const;
    void setStringValue(const std::string& value);
    
    // Utility methods
    size_t getValueSize() const;
    bool isCommand() const;
    bool isIntegerValue() const;
    bool isStringValue() const;
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t MIN_PAYLOAD_SIZE = 1;  // field_index + min_value(1)
    
    // Common field indices (examples)
    static constexpr uint8_t FIELD_INDEX_BIND = 0x11;  // Bind command (example)
    static constexpr uint8_t FIELD_INDEX_SAVE = 0x12;  // Save command (example)
    static constexpr uint8_t FIELD_INDEX_REBOOT = 0x13;  // Reboot command (example)
};

// ExpressLRS Status packet wrapper
class CRSFELRSStatus : public CRSFPacket {
public:
    CRSFELRSStatus(const etl::span<uint8_t>& data);
    
    // Packet statistics methods
    uint8_t getPacketsBad() const;
    void setPacketsBad(uint8_t count);
    
    uint16_t getPacketsGood() const;
    void setPacketsGood(uint16_t count);
    
    // Status flags methods
    uint8_t getFlags() const;
    void setFlags(uint8_t flags);
    
    // Individual flag methods
    bool isConnected() const;
    void setConnected(bool connected);
    bool hasModelMismatch() const;
    void setModelMismatch(bool mismatch);
    bool isArmed() const;
    void setArmed(bool armed);
    bool hasBaudRateTooLow() const;
    void setBaudRateTooLow(bool tooLow);
    
    // Message methods
    std::string getMessage() const;
    void setMessage(const std::string& message);
    
    // Utility methods
    double getPacketSuccessRate() const;
    bool hasWarnings() const;
    bool hasErrors() const;
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t MIN_PAYLOAD_SIZE = 4;  // packets_bad + packets_good(2) + flags
    
    // Status flags
    static constexpr uint8_t FLAG_CONNECTED = 0x01;
    static constexpr uint8_t FLAG_MODEL_MISMATCH = 0x04;
    static constexpr uint8_t FLAG_ARMED = 0x08;
    static constexpr uint8_t FLAG_NOT_WHILE_CONNECTED = 0x20;
    static constexpr uint8_t FLAG_BAUD_RATE_TOO_LOW = 0x40;
};

// CRSF Command packet wrapper
class CRSFCommand : public CRSFPacket {
public:
    CRSFCommand(const etl::span<uint8_t>& data);
    
    // Command realm methods
    uint8_t getCommandRealm() const;
    void setCommandRealm(uint8_t realm);
    
    // Subcommand methods
    uint8_t getSubcommand() const;
    void setSubcommand(uint8_t subcommand);
    
    // Payload methods
    etl::span<uint8_t> getCommandPayload() const;
    void setCommandPayload(const etl::span<uint8_t>& payload);
    
    // CRC8-BA methods
    uint8_t getCRC8BA() const;
    void setCRC8BA(uint8_t crc);
    bool verifyCRC8BA() const;
    void calculateCRC8BA();
    
    // Utility methods
    bool isReceiverCommand() const;
    bool isGeneralCommand() const;
    bool isBindCommand() const;
    bool isModelSelectCommand() const;
    bool isSpeedProposalCommand() const;
    bool isSpeedResponseCommand() const;
    
    // Specific command payload methods
    uint8_t getReceiverID() const;  // For MODEL_SELECT_ID
    void setReceiverID(uint8_t id);
    
    uint8_t getPortID() const;      // For speed commands
    void setPortID(uint8_t portId);
    uint32_t getProposedBaudRate() const;  // For speed proposal
    void setProposedBaudRate(uint32_t baudRate);
    bool isSpeedAccepted() const;   // For speed response
    void setSpeedAccepted(bool accepted);
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t MIN_PAYLOAD_SIZE = 2;  // realm + subcommand
    
    // Command realms
    static constexpr uint8_t REALM_RX = 0x10;
    static constexpr uint8_t REALM_GENERAL = 0x0A;
    
    // Receiver subcommands
    static constexpr uint8_t SUBCMD_RX_BIND = 0x01;
    static constexpr uint8_t SUBCMD_MODEL_SELECT_ID = 0x05;
    
    // General subcommands
    static constexpr uint8_t SUBCMD_GENERAL_CRSF_SPEED_PROPOSAL = 0x70;
    static constexpr uint8_t SUBCMD_GENERAL_CRSF_SPEED_RESPONSE = 0x71;
    
    // CRC8-BA polynomial
    static constexpr uint8_t CRC8_BA_POLY = 0xBA;
    
private:
    // Helper method for CRC8-BA calculation
    uint8_t calculateCRC8BAFromData(const uint8_t* data, size_t length) const;
};

// CRSF Radio ID packet wrapper
class CRSFRadioID : public CRSFPacket {
public:
    CRSFRadioID(const etl::span<uint8_t>& data);
    
    // Subtype methods
    uint8_t getSubtype() const;
    void setSubtype(uint8_t subtype);
    
    // Data methods
    etl::span<uint8_t> getSubtypeData() const;
    void setSubtypeData(const etl::span<uint8_t>& data);
    
    // Utility methods
    bool isOpenTXSync() const;
    
    // Validation
    virtual bool isValid() const;
    
    // Constants
    static constexpr size_t MIN_PAYLOAD_SIZE = 1;  // subtype
    
    // Subtypes
    static constexpr uint8_t SUBTYPE_OPENTX_SYNC = 0x10;
};

// OpenTX Sync packet wrapper (subtype of Radio ID)
class CRSFOpenTXSync : public CRSFRadioID {
public:
    CRSFOpenTXSync(const etl::span<uint8_t>& data);
    
    // Packet interval methods
    int32_t getPacketInterval() const;
    void setPacketInterval(int32_t interval);
    
    // Phase shift correction methods
    int32_t getPhaseShiftCorrection() const;
    void setPhaseShiftCorrection(int32_t correction);
    
    // Utility methods
    double getPacketIntervalHz() const;
    void setPacketIntervalHz(double hz);
    
    // Validation
    bool isValid() const override;
    
    // Constants
    static constexpr size_t PAYLOAD_SIZE = 8;  // packet_interval(4) + phase_shift(4)
};

// CRSF MSP Request packet wrapper
class CRSFMSPReq : public CRSFPacket {
public:
    CRSFMSPReq(const etl::span<uint8_t>& data);
    
    // MSP flags methods
    uint16_t getMSPFlags() const;
    void setMSPFlags(uint16_t flags);
    
    // Function methods
    uint16_t getFunction() const;
    void setFunction(uint16_t function);
    
    // Payload length methods
    uint16_t getPayloadLength() const;
    void setPayloadLength(uint16_t length);
    
    // MSP payload methods
    etl::span<uint8_t> getMSPPayload() const;
    void setMSPPayload(const etl::span<uint8_t>& payload);
    
    // Utility methods
    bool hasError() const;
    uint8_t getVersion() const;
    bool isBeginningOfFrame() const;
    bool isFirstFrame() const;
    bool isLastFrame() const;
    
    // Validation
    bool isValid() const;
    
    // Constants
    static constexpr size_t MIN_PAYLOAD_SIZE = 6;  // flags(2) + function(2) + payload_length(2)
    
    // MSP flag bits
    static constexpr uint16_t FLAG_ERROR_MASK = 0x00FF;
    static constexpr uint16_t FLAG_VERSION_MASK = 0x0F00;
    static constexpr uint16_t FLAG_BEGINNING_OF_FRAME = 0x1000;
    static constexpr uint16_t FLAG_FIRST_FRAME = 0x2000;
    static constexpr uint16_t FLAG_LAST_FRAME = 0x4000;
    
    // Common MSP functions
    static constexpr uint16_t FUNCTION_VTX_CONFIG = 0x0088;
    static constexpr uint16_t FUNCTION_OSD_CONFIG = 0x0084;
    static constexpr uint16_t FUNCTION_BF_CONFIG = 0x0085;
};



// Link Statistics factory functions
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
    etl::span<uint8_t>& data
);

// GPS Data factory functions
std::unique_ptr<CRSFGPSData> createGPSData(
    uint8_t address,
    uint32_t latitude,
    uint32_t longitude,
    uint16_t groundSpeed,
    uint16_t groundCourse,
    uint16_t altitude,
    uint8_t satellites,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFGPSData> createGPSDataDegrees(
    uint8_t address,
    double latitudeDegrees,
    double longitudeDegrees,
    uint16_t groundSpeed,
    uint16_t groundCourse,
    uint16_t altitude,
    uint8_t satellites,
    etl::span<uint8_t>& data
);

// Battery Sensor factory functions
std::unique_ptr<CRSFBatterySensor> createBatterySensor(
    uint8_t address,
    int16_t voltage,
    int16_t current,
    int32_t capacity,
    int8_t remaining,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFBatterySensor> createBatterySensorUnits(
    uint8_t address,
    double voltageVolts,
    double currentAmps,
    int32_t capacity,
    int8_t remaining,
    etl::span<uint8_t>& data
);

// Vario factory functions
std::unique_ptr<CRSFVario> createVario(
    uint8_t address,
    int16_t verticalSpeed,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFVario> createVarioMPS(
    uint8_t address,
    double verticalSpeedMPS,
    etl::span<uint8_t>& data
);

// Baro Altitude factory functions
std::unique_ptr<CRSFBaroAltitude> createBaroAltitude(
    uint8_t address,
    double altitudeMeters,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFBaroAltitude> createBaroAltitudeWithVario(
    uint8_t address,
    double altitudeMeters,
    int16_t verticalSpeed,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFBaroAltitude> createBaroAltitudeWithVarioMPS(
    uint8_t address,
    double altitudeMeters,
    double verticalSpeedMPS,
    etl::span<uint8_t>& data
);

// Heartbeat factory functions
std::unique_ptr<CRSFHeartbeat> createHeartbeat(
    uint8_t address,
    uint16_t originDeviceAddress,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFHeartbeat> createFlightControllerHeartbeat(
    uint8_t address,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFHeartbeat> createRadioHeartbeat(
    uint8_t address,
    etl::span<uint8_t>& data
);


// Link RX ID factory functions
std::unique_ptr<CRSFLinkRXID> createLinkRXID(
    uint8_t address,
    uint8_t downlinkRSSIdBm,
    uint8_t downlinkRSSIPercent,
    uint8_t downlinkLinkQuality,
    int8_t downlinkSNR,
    uint8_t uplinkRFPowerIndex,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFLinkRXID> createLinkRXIDWithSignedRSSI(
    uint8_t address,
    int8_t downlinkRSSIdBm,
    uint8_t downlinkRSSIPercent,
    uint8_t downlinkLinkQuality,
    int8_t downlinkSNR,
    uint8_t uplinkRFPowerIndex,
    etl::span<uint8_t>& data
);

// Link TX ID factory functions
std::unique_ptr<CRSFLinkTXID> createLinkTXID(
    uint8_t address,
    uint8_t uplinkRSSIdBm,
    uint8_t uplinkRSSIPercent,
    uint8_t uplinkLinkQuality,
    int8_t uplinkSNR,
    uint8_t downlinkRFPower,
    uint8_t packetRateFPS,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFLinkTXID> createLinkTXIDWithSignedRSSI(
    uint8_t address,
    int8_t uplinkRSSIdBm,
    uint8_t uplinkRSSIPercent,
    uint8_t uplinkLinkQuality,
    int8_t uplinkSNR,
    uint8_t downlinkRFPower,
    uint8_t packetRateFPS,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFLinkTXID> createLinkTXIDWithCommonFPS(
    uint8_t address,
    uint8_t uplinkRSSIdBm,
    uint8_t uplinkRSSIPercent,
    uint8_t uplinkLinkQuality,
    int8_t uplinkSNR,
    uint8_t downlinkRFPower,
    uint16_t fpsHz,
    etl::span<uint8_t>& data
);

// Attitude factory functions
std::unique_ptr<CRSFAttitude> createAttitude(
    uint8_t address,
    int16_t pitch,
    int16_t roll,
    int16_t yaw,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFAttitude> createAttitudeDegrees(
    uint8_t address,
    double pitchDegrees,
    double rollDegrees,
    double yawDegrees,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFAttitude> createAttitudeRadians(
    uint8_t address,
    double pitchRadians,
    double rollRadians,
    double yawRadians,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFAttitude> createAttitudeLevel(
    uint8_t address,
    etl::span<uint8_t>& data
);

// Flight Mode factory functions
std::unique_ptr<CRSFFlightMode> createFlightMode(
    uint8_t address,
    const std::string& mode,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFFlightMode> createAcroMode(
    uint8_t address,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFFlightMode> createHorizonMode(
    uint8_t address,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFFlightMode> createAngleMode(
    uint8_t address,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFFlightMode> createRTHMode(
    uint8_t address,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFFlightMode> createLandMode(
    uint8_t address,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFFlightMode> createTakeoffMode(
    uint8_t address,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFFlightMode> createManualMode(
    uint8_t address,
    etl::span<uint8_t>& data
);

// Device Ping factory functions
std::unique_ptr<CRSFDevicePing> createDevicePing(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFDevicePing> createBroadcastPing(
    uint8_t address,
    uint8_t extendedSource,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFDevicePing> createTargetedPing(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFDevicePing> createRadioPing(
    uint8_t address,
    uint8_t extendedDestination,
    etl::span<uint8_t>& data
);

// Device Info factory functions
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
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFDeviceInfo> createELRSDeviceInfo(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    const std::string& displayName,
    uint32_t hardwareVersion,
    uint8_t configParameterCount,
    etl::span<uint8_t>& data
);

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
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFDeviceInfo> createFlightControllerInfo(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    const std::string& displayName,
    uint32_t hardwareVersion,
    uint32_t softwareVersion,
    uint8_t configParameterCount,
    etl::span<uint8_t>& data
);

// Parameter Settings Entry factory functions
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
    etl::span<uint8_t>& data
);

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
    etl::span<uint8_t>& data
);

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
    etl::span<uint8_t>& data
);

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
    etl::span<uint8_t>& data
);

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
    etl::span<uint8_t>& data
);

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
    etl::span<uint8_t>& data
);

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
    etl::span<uint8_t>& data
);

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
    etl::span<uint8_t>& data
);

// Parameter Read factory functions
std::unique_ptr<CRSFParameterRead> createParameterRead(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t chunkIndex,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFParameterRead> createFolderRead(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFParameterRead> createParameterReadByIndex(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    etl::span<uint8_t>& data
);

// Parameter Read factory functions
std::unique_ptr<CRSFParameterRead> createParameterRead(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t chunkIndex,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFParameterRead> createFolderRequest(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFParameterRead> createParameterRequest(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    etl::span<uint8_t>& data
);

// Parameter Write factory functions
std::unique_ptr<CRSFParameterWrite> createParameterWrite(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFParameterWrite> createUint8ParameterWrite(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t value,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFParameterWrite> createUint16ParameterWrite(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint16_t value,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFParameterWrite> createUint32ParameterWrite(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint32_t value,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFParameterWrite> createStringParameterWrite(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    const std::string& value,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFParameterWrite> createCommandParameterWrite(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t fieldIndex,
    uint8_t commandStep,
    etl::span<uint8_t>& data
);

// ExpressLRS Status factory functions
std::unique_ptr<CRSFELRSStatus> createELRSStatus(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t packetsBad,
    uint16_t packetsGood,
    uint8_t flags,
    const std::string& message,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFELRSStatus> createConnectedStatus(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t packetsBad,
    uint16_t packetsGood,
    bool armed,
    const std::string& message,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFELRSStatus> createWarningStatus(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t packetsBad,
    uint16_t packetsGood,
    uint8_t warningFlags,
    const std::string& warningMessage,
    etl::span<uint8_t>& data
);

// CRSF Command factory functions
std::unique_ptr<CRSFCommand> createCommand(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t commandRealm,
    uint8_t subcommand,
    const etl::span<uint8_t>& payload,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFCommand> createReceiverBindCommand(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFCommand> createModelSelectCommand(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t receiverID,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFCommand> createSpeedProposalCommand(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t portID,
    uint32_t proposedBaudRate,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFCommand> createSpeedResponseCommand(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t portID,
    bool accepted,
    etl::span<uint8_t>& data
);

// CRSF Radio ID factory functions
std::unique_ptr<CRSFRadioID> createRadioID(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint8_t subtype,
    const etl::span<uint8_t>& subtypeData,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFOpenTXSync> createOpenTXSync(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    int32_t packetInterval,
    int32_t phaseShiftCorrection,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFOpenTXSync> createOpenTXSyncHz(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    double packetIntervalHz,
    int32_t phaseShiftCorrection,
    etl::span<uint8_t>& data
);

// CRSF MSP Request factory functions
std::unique_ptr<CRSFMSPReq> createMSPReq(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    uint16_t mspFlags,
    uint16_t function,
    const etl::span<uint8_t>& payload,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFMSPReq> createVTXConfigRequest(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFMSPReq> createOSDConfigRequest(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data
);

std::unique_ptr<CRSFMSPReq> createBetaflightConfigRequest(
    uint8_t address,
    uint8_t extendedDestination,
    uint8_t extendedSource,
    etl::span<uint8_t>& data
);


}

*/