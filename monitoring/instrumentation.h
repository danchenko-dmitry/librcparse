#pragma once

#include "../packet_catcher/instrumentation.h"

#include <string>
#include <vector>
#include <etl/map.h>
#include "monitoring/processor.h"
#include "monitoring/counter.h"

namespace monitoring = ::monitoring;
namespace metrics = ::monitoring;

namespace packet_catcher_instrumentation {

template <typename T>
struct TypeDescriptor {
    T type;
    std::string label;
};

template <typename T, size_t N>
struct PCITypeCounter {
    etl::map<T, metrics::Counter<uint64_t> *, N> packet_type_counter_map_;
public:
    void InitCounter(std::vector<TypeDescriptor<T> > typesDescriptors, std::string metricName, monitoring::EventProcessor *processor);
    void OnPacketType(T packet_type);
};

struct PCIMetrics : public PacketCatcherInstrumentation {
private:
    metrics::Counter<uint64_t> *totalBytesProcessed;
    metrics::Counter<uint64_t> *totalBytesValidated;
    metrics::Counter<uint64_t> *totalBytesDiscarded;

    metrics::Counter<uint64_t> *bytesDiscardedByTooBigPacket;
    metrics::Counter<uint64_t> *tooBigPacketAlarm;

    metrics::Counter<uint64_t> *bytesDiscardedByBufferOverflow;
    metrics::Counter<uint64_t> *bufferOverflowAlarm;

    metrics::Counter<uint64_t> *bytesDiscardedByPacketLimit;
    metrics::Counter<uint64_t> *packetLimitAlarm;

    metrics::Counter<uint64_t> *bytesDiscardedByGarbageData;

    metrics::Counter<uint64_t> *bytesDiscardedByCorruptedPacket;
    metrics::Counter<uint64_t> *packetsReceived;
    metrics::Counter<uint64_t> *packetsCorrupted;
public:
    PCIMetrics();
    PCIMetrics(monitoring::EventProcessor *processor, std::string metricName);
    PCIMetrics(monitoring::EventProcessor *processor, std::string metricName, std::string label);
    virtual ~PCIMetrics();

    void InitMetrics(monitoring::EventProcessor *processor, std::string metricName, std::string label) noexcept;
    virtual void OnBytesProcessed(size_t bytes_read) noexcept override;
    virtual void OnBytesDiscarded(size_t bytes_discarded) noexcept override;
    virtual void OnBytesValidated(size_t bytes_validated) noexcept override;
    virtual void OnBytesDiscardedByTooBigPacket(size_t bytes_discarded_by_too_big_packet) noexcept override;
    virtual void OnBytesDiscardedByBufferOverflow(size_t bytes_discarded_by_buffer_overflow) noexcept override;
    virtual void OnBytesDiscardedByPacketLimit(size_t bytes_discarded_by_packet_limit) noexcept override;
    virtual void OnBytesDiscardedByGarbageData(size_t bytes_discarded_by_garbage_data) noexcept override;
    virtual void OnBytesDiscardedByCorruptedPacket(size_t bytes_discarded_by_corrupted_packet) noexcept override;
    virtual void OnPacketsReceived(size_t packets_received) noexcept override;
    virtual void OnPacketsCorrupted(size_t packets_corrupted) noexcept override;
    virtual void OnTooBigPacketAlarm() noexcept override;
    virtual void OnBufferOverflowAlarm() noexcept override;
    virtual void OnPacketLimitAlarm() noexcept override;

    void printStatistics(const char *prefix) const;
};

}
