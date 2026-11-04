# CRSF

Typed wrappers over a CRSF frame in a caller-owned `etl::span<uint8_t>`.

```
[sync][len][type][payload][crc]
```

`len` counts type, payload, and CRC. The frame is at most 64 bytes. CRC-8 (`0xD5`) covers type and payload. Sync is `0xC8` or `0xEE` (`getAddress` / `setAddress`).

## Packets

- `CRSFPacket` — address, length, type, payload, CRC.
- `CRSFRCChannels` — 16 channels, 11-bit (0–1984, center 992). Optional ExpressLRS arm byte.
- `CRSFSubsetRCChannels` — CRSFv3 subset, 10–13 bit resolution.
- `CRSFCommand`, `CRSFCommandRX`, `CRSFCommandGeneral` — bind, model select, baud proposal and response. Extra CRC-8 (`0xBA`).

`bind*` functions write a packet into the span and set the CRC. `CRSFPacket::createPacket` wraps an existing buffer by type.

```cpp
std::array<uint8_t, crsf::MAX_PACKET_SIZE> buf{};
etl::span<uint8_t> data(buf);
crsf::CRSFRCChannels packet(data);
crsf::bindRCChannelsStandard(packet, 0xEE, 992, 992, 992, 992, data);
```
