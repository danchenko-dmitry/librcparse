# MSP (MultiWii Serial Protocol) Library

This library provides a C++ implementation of the MultiWii Serial Protocol (MSP) following the same design patterns as the CRSF library in this project.

## Overview

The MSP protocol is a binary message-based protocol used for communication between flight controllers and ground stations, configurators, and other devices. It's widely used in the drone/RC community, particularly with Betaflight, Cleanflight, and other flight controller firmware.

## Features

- **MSP Packet Types**: Support for the requested MSP packet types:
  - `MSP_RAW_IMU` (102) - Raw IMU data (accelerometer, gyroscope, magnetometer)
  - `MSP_STATUS` (101) - Flight controller status information
  - `MSP_MOTOR_TELEMETRY` (244) - Motor telemetry data (RPM, current, temperature)
  - `MSP_ATTITUDE` (108) - Attitude data (roll, pitch, yaw)
  - `MSP_ALTITUDE` (109) - Altitude and variometer data
  - `MSP_PID` (112) - PID controller settings
  - `MSP_ACC_TRIM` (240) - Accelerometer trim values

- **Binding Functions**: Zero-copy packet initialization using caller-provided buffers
- **CRC Validation**: Built-in CRC calculation and verification
- **Type Safety**: Strongly typed packet classes with proper accessors
- **Memory Efficient**: Uses `etl::span` for zero-copy operations

## Protocol Structure

MSP packets follow this structure:
```
$M<|> [size] [cmd] [data] [crc]
```

Where:
- `$M` - MSP header
- `<` or `>` - Direction (request/response)
- `size` - Payload size in bytes
- `cmd` - Command ID
- `data` - Payload data
- `crc` - XOR checksum of size + cmd + data

## Usage

### Basic Packet Creation

```cpp
#include "msp.h"

// Create a buffer for the packet
std::vector<uint8_t> buffer(32, 0);
etl::span<uint8_t> data(buffer);

// Create and bind a Raw IMU packet
msp::MSPRawIMU rawIMU(data);
msp::bindRawIMU(rawIMU, msp::PacketDirection::RESPONSE,
                1000, 2000, 3000,  // accX, accY, accZ
                4000, 5000, 6000,  // gyroX, gyroY, gyroZ
                7000, 8000, 9000,  // magX, magY, magZ
                data);

// The packet is now ready to send
size_t packetSize = rawIMU.getTotalSize();
```

### Packet Validation

```cpp
// Validate packet integrity
if (rawIMU.isValid()) {
    // Packet is valid, safe to use
    int16_t accX = rawIMU.getAccX();
    int16_t gyroY = rawIMU.getGyroY();
}
```

### CRC Calculation

```cpp
// Calculate CRC for the packet
rawIMU.calculateCRC();

// Verify CRC
if (rawIMU.verifyCRC()) {
    // CRC is valid
}
```

## Packet Types

### MSP_RAW_IMU (102)
Contains raw sensor data from accelerometer, gyroscope, and magnetometer.

**Payload (18 bytes):**
- `accX`, `accY`, `accZ` (6 bytes) - Accelerometer data
- `gyroX`, `gyroY`, `gyroZ` (6 bytes) - Gyroscope data  
- `magX`, `magY`, `magZ` (6 bytes) - Magnetometer data

### MSP_STATUS (101)
Contains flight controller status information.

**Payload (11 bytes):**
- `cycleTime` (2 bytes) - Main loop cycle time
- `i2cErrorCount` (2 bytes) - I2C error count
- `sensor` (2 bytes) - Sensor flags
- `flag` (4 bytes) - Status flags
- `currentProfile` (1 byte) - Current profile number

### MSP_MOTOR_TELEMETRY (244)
Contains motor telemetry data for up to 4 motors.

**Payload (24 bytes):**
- `motor1RPM` to `motor4RPM` (8 bytes) - Motor RPM values
- `motor1Current` to `motor4Current` (8 bytes) - Motor current values
- `motor1Temperature` to `motor4Temperature` (8 bytes) - Motor temperature values

### MSP_ATTITUDE (108)
Contains attitude data in degrees.

**Payload (6 bytes):**
- `roll` (2 bytes) - Roll angle
- `pitch` (2 bytes) - Pitch angle
- `yaw` (2 bytes) - Yaw angle

### MSP_ALTITUDE (109)
Contains altitude and variometer data.

**Payload (6 bytes):**
- `altitude` (4 bytes) - Altitude in cm
- `variometer` (2 bytes) - Vertical speed in cm/s

### MSP_PID (112)
Contains PID controller settings.

**Payload (30 bytes):**
- `rollP`, `rollI`, `rollD` (3 bytes) - Roll PID values
- `pitchP`, `pitchI`, `pitchD` (3 bytes) - Pitch PID values
- `yawP`, `yawI`, `yawD` (3 bytes) - Yaw PID values
- Additional PID values for other axes

### MSP_ACC_TRIM (240)
Contains accelerometer trim values.

**Payload (4 bytes):**
- `accTrimPitch` (2 bytes) - Pitch trim
- `accTrimRoll` (2 bytes) - Roll trim

## Building

The library uses CMake and can be built as part of the main project:

```bash
# Build the MSP library
cmake -B build -S .
cmake --build build --target msp

# Build and run tests
cmake --build build --target test_msp
./build/test_msp
```

## References

- [Reefwing MSP Library](https://github.com/Reefwing-Software/Reefwing-MSP)
- [MSP Python Library](https://pypi.org/project/msp-python3/)
- [Betaflight MSP Implementation](https://github.com/betaflight/betaflight/blob/master/src/main/msp/msp.c)

## License

This library follows the same license as the main project.
