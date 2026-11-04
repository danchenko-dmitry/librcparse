#pragma once

#include <iostream>

namespace packet_catcher_instrumentation {

struct Statistics {
    size_t totalBytesProcessed = 0;
    size_t totalBytesValidated = 0;
    size_t totalBytesDiscarded = 0;
    size_t bytesDiscardedByTooBigPacket = 0;
    size_t bytesDiscardedByBufferOverflow = 0;
    size_t bytesDiscardedByPacketLimit = 0;
    size_t bytesDiscardedByGarbageData = 0;
    size_t bytesDiscardedByCorruptedPacket = 0;
    size_t packetsReceived = 0;
    size_t packetsCorrupted = 0;
    size_t tooBigPacketAlarm = 0;
    size_t bufferOverflowAlarm = 0;
    size_t packetLimitAlarm = 0;
};

class PacketCatcherInstrumentation {
public:
    virtual ~PacketCatcherInstrumentation() = default;
    virtual void OnBytesProcessed(size_t bytes_read) noexcept = 0;
    virtual void OnBytesDiscarded(size_t bytes_discarded) noexcept = 0;
    virtual void OnBytesValidated(size_t bytes_validated) noexcept = 0;
    virtual void OnBytesDiscardedByTooBigPacket(size_t bytes_discarded_by_too_big_packet) noexcept = 0;
    virtual void OnBytesDiscardedByBufferOverflow(size_t bytes_discarded_by_buffer_overflow) noexcept = 0;
    virtual void OnBytesDiscardedByPacketLimit(size_t bytes_discarded_by_packet_limit) noexcept = 0;
    virtual void OnBytesDiscardedByGarbageData(size_t bytes_discarded_by_garbage_data) noexcept = 0;
    virtual void OnBytesDiscardedByCorruptedPacket(size_t bytes_discarded_by_corrupted_packet) noexcept = 0;
    virtual void OnPacketsReceived(size_t packets_received) noexcept = 0;
    virtual void OnPacketsCorrupted(size_t packets_corrupted) noexcept = 0;
    virtual void OnTooBigPacketAlarm() noexcept = 0;
    virtual void OnBufferOverflowAlarm() noexcept = 0;
    virtual void OnPacketLimitAlarm() noexcept = 0;
};

// In-process counters. Does not export metrics.
struct PCICout : public PacketCatcherInstrumentation {
private:
    size_t totalBytesProcessed = 0;
    size_t totalBytesValidated = 0;
    size_t totalBytesDiscarded = 0;

    size_t bytesDiscardedByTooBigPacket = 0;
    size_t tooBigPacketAlarm = 0;

    size_t bytesDiscardedByBufferOverflow = 0;
    size_t bufferOverflowAlarm = 0;

    size_t bytesDiscardedByPacketLimit = 0;
    size_t packetLimitAlarm = 0;

    size_t bytesDiscardedByGarbageData = 0;
    size_t bytesDiscardedByCorruptedPacket = 0;

    size_t packetsReceived = 0;
    size_t packetsCorrupted = 0;

public:
    virtual void OnBytesProcessed(size_t bytes_read) noexcept override { totalBytesProcessed += bytes_read; }
    virtual void OnBytesDiscarded(size_t bytes_discarded) noexcept override { totalBytesDiscarded += bytes_discarded; }
    virtual void OnBytesValidated(size_t bytes_validated) noexcept override { totalBytesValidated += bytes_validated; }
    virtual void OnBytesDiscardedByTooBigPacket(size_t bytes_discarded_by_too_big_packet) noexcept override { bytesDiscardedByTooBigPacket += bytes_discarded_by_too_big_packet; }
    virtual void OnBytesDiscardedByBufferOverflow(size_t bytes_discarded_by_buffer_overflow) noexcept override { bytesDiscardedByBufferOverflow += bytes_discarded_by_buffer_overflow; }
    virtual void OnBytesDiscardedByPacketLimit(size_t bytes_discarded_by_packet_limit) noexcept override { bytesDiscardedByPacketLimit += bytes_discarded_by_packet_limit; }
    virtual void OnBytesDiscardedByGarbageData(size_t bytes_discarded_by_garbage_data) noexcept override { bytesDiscardedByGarbageData += bytes_discarded_by_garbage_data; }
    virtual void OnBytesDiscardedByCorruptedPacket(size_t bytes_discarded_by_corrupted_packet) noexcept override { bytesDiscardedByCorruptedPacket += bytes_discarded_by_corrupted_packet; }
    virtual void OnPacketsReceived(size_t packets_received) noexcept override { this->packetsReceived += packets_received; }
    virtual void OnPacketsCorrupted(size_t packets_corrupted) noexcept override { this->packetsCorrupted += packets_corrupted; }
    virtual void OnTooBigPacketAlarm() noexcept override { tooBigPacketAlarm++; }
    virtual void OnBufferOverflowAlarm() noexcept override { bufferOverflowAlarm++; }
    virtual void OnPacketLimitAlarm() noexcept override { packetLimitAlarm++; }

    Statistics getStatistics() const {
        Statistics stats;
        stats.totalBytesProcessed = totalBytesProcessed;
        stats.totalBytesValidated = totalBytesValidated;
        stats.totalBytesDiscarded = totalBytesDiscarded;
        stats.bytesDiscardedByTooBigPacket = bytesDiscardedByTooBigPacket;
        stats.bytesDiscardedByBufferOverflow = bytesDiscardedByBufferOverflow;
        stats.bytesDiscardedByPacketLimit = bytesDiscardedByPacketLimit;
        stats.bytesDiscardedByGarbageData = bytesDiscardedByGarbageData;
        stats.bytesDiscardedByCorruptedPacket = bytesDiscardedByCorruptedPacket;
        stats.packetsReceived = packetsReceived;
        stats.packetsCorrupted = packetsCorrupted;
        stats.tooBigPacketAlarm = tooBigPacketAlarm;
        stats.bufferOverflowAlarm = bufferOverflowAlarm;
        stats.packetLimitAlarm = packetLimitAlarm;
        return stats;
    }

    void resetStatistics() {
        totalBytesProcessed = 0;
        totalBytesValidated = 0;
        totalBytesDiscarded = 0;
        bytesDiscardedByTooBigPacket = 0;
        tooBigPacketAlarm = 0;
        bytesDiscardedByBufferOverflow = 0;
        bufferOverflowAlarm = 0;
        bytesDiscardedByPacketLimit = 0;
        packetLimitAlarm = 0;
        bytesDiscardedByGarbageData = 0;
        bytesDiscardedByCorruptedPacket = 0;
        packetsReceived = 0;
        packetsCorrupted = 0;
    }

    void printStatistics(const char *prefix) const {
        std::cout << "📊 " << prefix << " Stats - UART: " << totalBytesProcessed
                << " bytes, " << totalBytesValidated << " validated, " << totalBytesDiscarded << " discarded | "
                << "Packets: " << packetsReceived << " | " << packetsCorrupted << " corrupted" << std::endl;
    }
};

}
