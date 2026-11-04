#ifndef PACKET_CATCHER_H
#define PACKET_CATCHER_H

#include "protocol_callbacks.h"
#include "instrumentation.h"
#include <etl/array.h>
#include <etl/span.h>
#include <functional>
#include <memory>

// Maximum buffer size for packet processing - enough for 3 max packets
constexpr size_t MAX_BUFFER_SIZE = 100 * 64; // 3 * max packet size (64 bytes per protocol spec)

class PacketCatcher {
public:    
    PacketCatcher();
    
    // Set the protocol callbacks for packet detection and validation
    void setProtocolCallbacks(const ProtocolCallbacks& callbacks);

    // Process incoming bytes and add them to the internal buffer
    // CRITICAL: This function ensures that data is written contiguously in the buffer
    // to prevent data fragmentation that would break etl::span linking in packet objects.
    // If the buffer cannot accommodate the entire data chunk, it will clear the buffer
    // and report an invalid protocol situation.
    void processBytes(const uint8_t* data, size_t length);
    
    // Check if the buffer can accommodate the given length of data contiguously
    bool canAccommodateContiguously(size_t length) const;
    
    // Get the current buffer size
    size_t getBufferSize() const { return bufferSize_; }
    
    // Get the maximum buffer capacity
    size_t getMaxBufferSize() const { return MAX_BUFFER_SIZE; }
    
    // Check if buffer is empty
    bool isBufferEmpty() const { return bufferSize_ == 0; }
    
    // Check if buffer is full
    bool isBufferFull() const { return bufferSize_ == MAX_BUFFER_SIZE; }
    
    // Get buffer data for testing/debugging (returns a span of the current buffer contents)
    etl::span<const uint8_t> getBufferData() const { return etl::span<const uint8_t>(buffer_.data(), bufferSize_); }


    void setInstrumentation(packet_catcher_instrumentation::PacketCatcherInstrumentation* instrumentation);
    
    // Get statistics (only works with PCICout instrumentation)
    packet_catcher_instrumentation::Statistics getStatistics() const;
    
    // Reset statistics (only works with PCICout instrumentation)
    void resetStatistics();

private:
    // Simple growing/shrinking buffer using etl::array
    etl::array<uint8_t, MAX_BUFFER_SIZE> buffer_;
    size_t bufferSize_;           // Current number of bytes in buffer
    
    ProtocolCallbacks protocolCallbacks_;  // Protocol-specific callbacks
    packet_catcher_instrumentation::PacketCatcherInstrumentation* instrumentation_;
    
    // Process the buffer to find complete packets
    // Returns true if a packet was processed, false if no packets were processed
    bool processBuffer();
    
    // Process a valid packet (internal use)
    void processValidPacket(const uint8_t* data, size_t size);
    
    // Helper functions for packet detection and validation (use protocol callbacks)
    bool isValidPacketHead(size_t startPos) const;
    bool isCompletePacket(size_t startPos) const;
    bool validatePacket(size_t startPos, size_t packetLength) const;
    size_t findNextPacketStart(size_t startPos) const;
    
    // Helper method to calculate unprocessed packets in buffer
    size_t calculateUnprocessedPackets() const;
    
    // Helper method to search for a valid packet start within a specific range
    // Used for data recovery from corrupted packets
    size_t findValidPacketStartInRange(size_t startPos, size_t endPos) const;
    
    // Helper method to search for a valid packet beginning from a given position
    // If found after startPos, moves valid data back to startPos, discarding skipped data
    bool searchBeginOfPacketAndDiscardSkiped(size_t startPos);
    
    // Buffer management functions
    void writeDataContiguously(const uint8_t* data, size_t length);
    void clearBuffer();
    void discardDataFromStartTo(size_t bytesToRemove);
    void skipDataFromStartTo(size_t bytesToRemove);
    
    // Statistics update functions
    void tooBigPacketReceived(const uint8_t* data, size_t length);
    void bufferOverflowReceived(const uint8_t* data, size_t length);
    void garbageDataReceived(const uint8_t* data, size_t length);
    void packetsHitLimit(const uint8_t* data, size_t length);
    void corruptedPacket(const uint8_t* data, size_t length);
    
    // Constants for packet processing
    static constexpr size_t MAX_PACKETS_PER_CALL = 35; // Maximum packets processed per processBytes call
};

#endif // PACKET_CATCHER_H
