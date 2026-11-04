# MSP

Typed wrappers over a MultiWii Serial Protocol frame in a caller-owned `etl::span<uint8_t>`. Command IDs follow Betaflight.

```
$ M <|> [size] [cmd] [payload] [crc]
```

`<` is a request, `>` is a response. `size` is the payload length, at most 255 bytes. The checksum is the XOR of size, command, and payload.

## Packets

- `MSPPacket` — direction, command, payload, checksum.
- `MSPRequest` — empty request.
- `MSPRawIMU` — accelerometer, gyroscope, magnetometer (18 bytes).
- `MSPStatus` — cycle time, I2C errors, sensors, flags, profile (11 bytes).
- `MSPMotorTelemetry` — RPM, current, and temperature for four motors (24 bytes).
- `MSPAttitude` — roll, pitch, yaw (6 bytes).
- `MSPAltitude` — altitude and variometer (6 bytes).
- `MSPPID` — PID bytes; accessors for roll, pitch, and yaw (30 bytes).
- `MSPAccTrim` — pitch and roll trim (4 bytes).

A typed constructor sets the command and payload size. `bind` attaches an existing frame and validates it. `MSPPacket::createPacket` picks the wrapper by command. Other `PacketType` values are names only.

```cpp
std::array<uint8_t, 32> buf{};
etl::span<uint8_t> data(buf);
msp::MSPRawIMU imu(data);
imu.setHeader();
imu.setDirection(msp::PacketDirection::RESPONSE);
imu.setAccX(1000);
imu.calculateCRC();
```
