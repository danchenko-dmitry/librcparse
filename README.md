# librcparse

C++20 library for parsing RC serial protocols. It frames a byte stream into packets and decodes CRSF and MSP.

## Components

- **packet_catcher** — buffers incoming bytes, finds packet boundaries, and validates frames through protocol callbacks.
- **crsf** — CRSF (Crossfire / ELRS) packets: RC channels, link statistics, GPS, battery, and related types. CRC polynomial `0xD5`.
- **msp** — MultiWii Serial Protocol (Betaflight-style) packets: IMU, status, attitude, altitude, PID, and motor telemetry.

`crsf` and `msp` expose typed packet wrappers over raw bytes (`std::vector` / `etl::span`) with getters, setters, and CRC checks.

## Build

The tree expects a sibling [uart-rp-test](../uart-rp-test) checkout (ETL, common CMake options, domain libraries). Override the path with `-DUART_RP_TEST_ROOT=...`.

```bash
cmake -B build -S .
cmake --build build
ctest --test-dir build
```

Other projects pull it in with `add_subdirectory`.
