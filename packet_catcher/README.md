# packet_catcher

Turns a byte stream into frames. The protocol is supplied through callbacks; the catcher only buffers bytes and finds boundaries.

`processBytes` appends a chunk to a contiguous buffer (`MAX_BUFFER_SIZE`, 6400 bytes). A chunk larger than the buffer is dropped. A chunk that does not fit clears the buffer, then the new bytes are kept. An incomplete tail stays until the next call.

## Callbacks

`ProtocolCallbacks`:

- `isPacketHead` — this offset looks like a frame start.
- `getTotalPacketSize` — full frame length, or 0 if the header is not readable yet.
- `isValidPacket` — structure and checksum.
- `processValidPacket` — a complete valid frame. At most 35 frames are delivered per call; the rest of that call are discarded.
- `discardedData` — optional. Bytes skipped before the next head.

Bytes before a head are garbage. A frame that fails `isValidPacket` is corrupted, and the scan resumes at the next byte. CRSF and MSP provide these callbacks (`crsfIsPacketHead`, `mspIsPacketHead`, and the matching size and validation functions).

```cpp
ProtocolCallbacks cb;
cb.isPacketHead = msp::mspIsPacketHead;
cb.getTotalPacketSize = msp::mspGetTotalPacketSize;
cb.isValidPacket = msp::mspIsValidPacket;
cb.processValidPacket = [](const uint8_t* data, size_t size) { /* handle frame */ };

PacketCatcher catcher;
catcher.setProtocolCallbacks(cb);
catcher.processBytes(bytes, n);
```

Optional `PacketCatcherInstrumentation` counts bytes and frames. `PCICout` keeps the counters locally; `PCIMetrics` exports them. `getStatistics` and `resetStatistics` work only with `PCICout`.
