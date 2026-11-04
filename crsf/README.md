# CRSF Protocol Implementation

This is a complete rewrite of the CRSF (Crossfire) protocol implementation with a focus on flexibility and type safety.

## Design Philosophy

The new implementation is built around the following principles:

1. **Byte Array Centric**: All CRSF packets are built around a `std::vector<uint8_t>` that contains the raw packet data
2. **Getter/Setter Interface**: All packet fields are accessed through getter and setter methods for safety and validation
3. **Type Wrappers**: Different packet types are implemented as wrapper classes that inherit from the base `CRSFPacket` class
4. **Factory Pattern**: Packet creation is handled through factory methods that automatically determine the correct wrapper type

## Architecture

### Base Class: `CRSFPacket`

The `CRSFPacket` class is the foundation that:
- Wraps a `std::vector<uint8_t>` containing the raw packet data
- Provides safe access methods (`getByte`, `setByte`, `getU16`, `setU16`, etc.)
- Handles basic packet operations (CRC calculation, validation)
- Manages packet size and memory allocation automatically

### Packet Type Wrappers

Each CRSF packet type has its own wrapper class:

- **`CRSFRCChannels`**: Handles 16-channel RC data with 11-bit resolution
- **`CRSFLinkStatistics`**: Manages link quality and signal strength data
- **`CRSFGPSData`**: Handles GPS position, speed, and altitude data
- **`CRSFBatterySensor`**: Manages battery voltage, current, and capacity data

### Key Features

- **Automatic Memory Management**: Packets automatically resize as needed
- **Type Safety**: Each wrapper provides type-specific methods and validation
- **CRC Handling**: Built-in CRC calculation and verification
- **Flexible Construction**: Can be created from existing data or built from scratch
- **Factory Methods**: Automatic packet type detection and wrapper creation

## Usage Examples

### Creating RC Channels Packet

```cpp
#include "crsf.h"

// Create a new RC channels packet
crsf::CRSFRCChannels rcPacket;
rcPacket.setAddress(0xEE);  // Broadcast address

// Set channel values (0-2047 range)
rcPacket.setChannel(0, 1024);  // Throttle center
rcPacket.setChannel(1, 1536);  // Elevator up
rcPacket.setChannel(2, 512);   // Aileron left
rcPacket.setChannel(3, 1024);  // Rudder center

// Calculate CRC and get raw data
rcPacket.calculateCRC();
std::vector<uint8_t> rawData = rcPacket.getRawData();
```

### Creating GPS Data Packet

```cpp
crsf::CRSFGPSData gpsPacket;
gpsPacket.setAddress(0xEE);

// Set GPS coordinates in degrees (converted automatically)
gpsPacket.setLatitudeDegrees(52.5200);   // Berlin latitude
gpsPacket.setLongitudeDegrees(13.4050);  // Berlin longitude
gpsPacket.setGroundSpeed(120);           // 120 km/h
gpsPacket.setAltitude(100);              // 100 meters

gpsPacket.calculateCRC();
```

### Parsing Existing Data

```cpp
// Raw packet data from somewhere
std::vector<uint8_t> rawData = {0xEE, 0x0A, 0x14, ...};

// Use factory to create appropriate wrapper
auto packet = crsf::CRSFPacket::createPacket(rawData);

// Cast to specific type if needed
if (auto linkStats = dynamic_cast<crsf::CRSFLinkStatistics*>(packet.get())) {
    uint8_t rssi = linkStats->getUplinkRSSI1();
    // ... work with link statistics
}
```

### Working with Battery Data

```cpp
crsf::CRSFBatterySensor batteryPacket;
batteryPacket.setAddress(0xEE);

// Set values in human-readable units
batteryPacket.setVoltageVolts(12.5);    // 12.5V
batteryPacket.setCurrentAmps(2.3);      // 2.3A
batteryPacket.setCapacity(1500);        // 1500 mAh consumed
batteryPacket.setRemaining(75);         // 75% remaining

// Get values back
double voltage = batteryPacket.getVoltageVolts();
double current = batteryPacket.getCurrentAmps();
```

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Testing

```bash
# Run the test program
./test_crsf

# Or run through CMake
make test
```

## Packet Structure

All CRSF packets follow this structure:
```
[Address][Length][Type][Payload...][CRC]
  1 byte  1 byte 1 byte 0-62 bytes 1 byte
```

- **Address**: Device address (0xEE for broadcast)
- **Length**: Payload length (0-62 bytes)
- **Type**: Packet type identifier
- **Payload**: Variable length data
- **CRC**: 8-bit checksum (polynomial 0xD5)

## Supported Packet Types

- **0x02**: GPS Data
- **0x07**: Vario
- **0x08**: Battery Sensor
- **0x09**: Baro Altitude
- **0x14**: Link Statistics
- **0x16**: RC Channels Packed
- **0x1E**: Attitude
- **0x21**: Flight Mode
- **0x28**: Device Ping
- **0x29**: Device Info
- **0x2B**: Parameter Settings Entry
- **0x2C**: Parameter Read
- **0x2D**: Parameter Write
- **0x32**: Command
- **0x33**: Data

## Benefits of the New Design

1. **Memory Efficiency**: No fixed-size arrays, only allocated what's needed
2. **Type Safety**: Compile-time type checking for packet operations
3. **Extensibility**: Easy to add new packet types
4. **Validation**: Built-in packet validation and CRC checking
5. **Flexibility**: Can work with existing data or create new packets
6. **Performance**: Efficient byte-level operations with bounds checking

## Future Enhancements

- Add more packet type wrappers
- Implement packet serialization/deserialization
- Add packet validation rules
- Support for packet fragmentation and reassembly
- Integration with UART communication layers
