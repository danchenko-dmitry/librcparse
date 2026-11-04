#include "instrumentation.h"
#include "../crsf/crsf.h"
#include "../msp/msp.h"
#include "monitoring/processor.h"
#include "monitoring/counter.h"

namespace monitoring = ::monitoring;
namespace metrics = ::monitoring;

namespace packet_catcher_instrumentation {

// Implementation for PacketCatcherInstrumentationMetrics
PCIMetrics::PCIMetrics() : totalBytesProcessed(nullptr), totalBytesValidated(nullptr), totalBytesDiscarded(nullptr), bytesDiscardedByTooBigPacket(nullptr), tooBigPacketAlarm(nullptr), bytesDiscardedByBufferOverflow(nullptr), bufferOverflowAlarm(nullptr), bytesDiscardedByPacketLimit(nullptr), packetLimitAlarm(nullptr), bytesDiscardedByGarbageData(nullptr), bytesDiscardedByCorruptedPacket(nullptr), packetsReceived(nullptr), packetsCorrupted(nullptr) {
}

PCIMetrics::PCIMetrics(monitoring::EventProcessor *processor, std::string metricName) : totalBytesProcessed(nullptr), totalBytesValidated(nullptr), totalBytesDiscarded(nullptr), bytesDiscardedByTooBigPacket(nullptr), tooBigPacketAlarm(nullptr), bytesDiscardedByBufferOverflow(nullptr), bufferOverflowAlarm(nullptr), bytesDiscardedByPacketLimit(nullptr), packetLimitAlarm(nullptr), bytesDiscardedByGarbageData(nullptr), bytesDiscardedByCorruptedPacket(nullptr), packetsReceived(nullptr), packetsCorrupted(nullptr) {
    InitMetrics(processor, metricName, "");
}

PCIMetrics::PCIMetrics(monitoring::EventProcessor *processor, std::string metricName, std::string label) : totalBytesProcessed(nullptr), totalBytesValidated(nullptr), totalBytesDiscarded(nullptr), bytesDiscardedByTooBigPacket(nullptr), tooBigPacketAlarm(nullptr), bytesDiscardedByBufferOverflow(nullptr), bufferOverflowAlarm(nullptr), bytesDiscardedByPacketLimit(nullptr), packetLimitAlarm(nullptr), bytesDiscardedByGarbageData(nullptr), bytesDiscardedByCorruptedPacket(nullptr), packetsReceived(nullptr), packetsCorrupted(nullptr) {
    InitMetrics(processor, metricName, label);
}

PCIMetrics::~PCIMetrics() {
    if (totalBytesProcessed) {
        delete totalBytesProcessed;
    }
    if (totalBytesValidated) {
        delete totalBytesValidated;
    }
    if (totalBytesDiscarded) {
        delete totalBytesDiscarded;
    }
    if (bytesDiscardedByTooBigPacket) {
        delete bytesDiscardedByTooBigPacket;
    }
    if (tooBigPacketAlarm) {
        delete tooBigPacketAlarm;
    }
    if (bytesDiscardedByBufferOverflow) {
        delete bytesDiscardedByBufferOverflow;
    }
    if (bufferOverflowAlarm) {
        delete bufferOverflowAlarm;
    }
    if (bytesDiscardedByPacketLimit) {
        delete bytesDiscardedByPacketLimit;
    }
    if (packetLimitAlarm) {
        delete packetLimitAlarm;
    }
    if (bytesDiscardedByGarbageData) {
        delete bytesDiscardedByGarbageData;
    }
    if (bytesDiscardedByCorruptedPacket) {
        delete bytesDiscardedByCorruptedPacket;
    }
    if (packetsReceived) {
        delete packetsReceived;
    }
    if (packetsCorrupted) {
        delete packetsCorrupted;
    }
}

void PCIMetrics::InitMetrics(monitoring::EventProcessor *processor, std::string metricName,std::string label) noexcept {
    // Create meaningful labels for each metric type
    monitoring::Labels totalBytesProcessedLabels;
    totalBytesProcessedLabels["metric_type"] = "total_bytes_processed";
    if (label!="") {
        totalBytesProcessedLabels["catcher-id"] = label;
    }
    
    monitoring::Labels totalBytesValidatedLabels;
    totalBytesValidatedLabels["metric_type"] = "total_bytes_validated";
    if (label!="") {
        totalBytesValidatedLabels["catcher-id"] = label;
    }
    
    monitoring::Labels totalBytesDiscardedLabels;
    totalBytesDiscardedLabels["metric_type"] = "total_bytes_discarded";
    if (label!="") {
        totalBytesDiscardedLabels["catcher-id"] = label;
    }
    
    monitoring::Labels bytesDiscardedByTooBigPacketLabels;
    bytesDiscardedByTooBigPacketLabels["metric_type"] = "bytes_discarded_by_too_big_packet";
    if (label!="") {
        bytesDiscardedByTooBigPacketLabels["catcher-id"] = label;
    }
    
    monitoring::Labels bytesDiscardedByBufferOverflowLabels;
    bytesDiscardedByBufferOverflowLabels["metric_type"] = "bytes_discarded_by_buffer_overflow";
    if (label!="") {
        bytesDiscardedByBufferOverflowLabels["catcher-id"] = label;
    }
    
    monitoring::Labels bytesDiscardedByPacketLimitLabels;
    bytesDiscardedByPacketLimitLabels["metric_type"] = "bytes_discarded_by_packet_limit";
    if (label!="") {
        bytesDiscardedByPacketLimitLabels["catcher-id"] = label;
    }
    
    monitoring::Labels bytesDiscardedByGarbageDataLabels;
    bytesDiscardedByGarbageDataLabels["metric_type"] = "bytes_discarded_by_garbage_data";
    if (label!="") {
        bytesDiscardedByGarbageDataLabels["catcher-id"] = label;
    }
    
    monitoring::Labels bytesDiscardedByCorruptedPacketLabels;
    bytesDiscardedByCorruptedPacketLabels["metric_type"] = "bytes_discarded_by_corrupted_packet";
    if (label!="") {
        bytesDiscardedByCorruptedPacketLabels["catcher-id"] = label;
    }
    
    monitoring::Labels packetsReceivedLabels;
    packetsReceivedLabels["metric_type"] = "packets_received";
    if (label!="") {
        packetsReceivedLabels["catcher-id"] = label;
    }
    
    monitoring::Labels packetsCorruptedLabels;
    packetsCorruptedLabels["metric_type"] = "packets_corrupted";
    if (label!="") {
        packetsCorruptedLabels["catcher-id"] = label;
    }
    
    monitoring::Labels alarmTooBigPacketAlarmLabels;
    alarmTooBigPacketAlarmLabels["metric_type"] = "too_big_packet_alarm";
    if (label!="") {
        alarmTooBigPacketAlarmLabels["catcher-id"] = label;
    }
    
    monitoring::Labels alarmBufferOverflowAlarmLabels;
    alarmBufferOverflowAlarmLabels["metric_type"] = "buffer_overflow_alarm";
    if (label!="") {
        alarmBufferOverflowAlarmLabels["catcher-id"] = label;
    }
    
    monitoring::Labels alarmPacketLimitAlarmLabels;
    alarmPacketLimitAlarmLabels["metric_type"] = "packet_limit_alarm";
    if (label!="") {
        alarmPacketLimitAlarmLabels["catcher-id"] = label;
    }
    
    totalBytesProcessed = new monitoring::Counter<uint64_t>(metricName+"_total", processor, "Total bytes processed", "count", totalBytesProcessedLabels);
    totalBytesValidated = new monitoring::Counter<uint64_t>(metricName+"_total", processor, "Total bytes validated", "count", totalBytesValidatedLabels);
    totalBytesDiscarded = new monitoring::Counter<uint64_t>(metricName+"_total", processor, "Total bytes discarded", "count", totalBytesDiscardedLabels);
    
    bytesDiscardedByTooBigPacket = new monitoring::Counter<uint64_t>(metricName+"_bytes_discarded_by", processor, "Bytes discarded by too big packet", "count", bytesDiscardedByTooBigPacketLabels);
    tooBigPacketAlarm = new monitoring::Counter<uint64_t>(metricName+"_alarm", processor, "Too big packet alarm", "count", alarmTooBigPacketAlarmLabels);
    
    bytesDiscardedByBufferOverflow = new monitoring::Counter<uint64_t>(metricName+"_bytes_discarded_by", processor, "Bytes discarded by buffer overflow", "count", bytesDiscardedByBufferOverflowLabels);
    bufferOverflowAlarm = new monitoring::Counter<uint64_t>(metricName+"_alarm", processor, "Buffer overflow alarm", "count", alarmBufferOverflowAlarmLabels);
    
    bytesDiscardedByPacketLimit = new monitoring::Counter<uint64_t>(metricName+"_bytes_discarded_by", processor, "Bytes discarded by packet limit", "count", bytesDiscardedByPacketLimitLabels);
    packetLimitAlarm = new monitoring::Counter<uint64_t>(metricName+"_alarm", processor, "Packet limit alarm", "count", alarmPacketLimitAlarmLabels);
    
    bytesDiscardedByGarbageData = new monitoring::Counter<uint64_t>(metricName+"_bytes_discarded_by", processor, "Bytes discarded by garbage data", "count", bytesDiscardedByGarbageDataLabels);
    
    bytesDiscardedByCorruptedPacket = new monitoring::Counter<uint64_t>(metricName+"_bytes_discarded_by", processor, "Bytes discarded by corrupted packet", "count", bytesDiscardedByCorruptedPacketLabels);
    packetsReceived = new monitoring::Counter<uint64_t>(metricName+"_packets", processor, "Packets received", "count", packetsReceivedLabels);
    packetsCorrupted = new monitoring::Counter<uint64_t>(metricName+"_packets", processor, "Packets corrupted", "count", packetsCorruptedLabels);
}

void PCIMetrics::OnBytesProcessed(size_t bytes_read) noexcept {
    if (totalBytesProcessed) {
        totalBytesProcessed->Add(bytes_read);
    }
}

void PCIMetrics::OnBytesDiscarded(size_t bytes_discarded) noexcept {
    if (totalBytesDiscarded) {
        totalBytesDiscarded->Add(bytes_discarded);
    }
}

void PCIMetrics::OnBytesValidated(size_t bytes_validated) noexcept {
    if (totalBytesValidated) {
        totalBytesValidated->Add(bytes_validated);
    }
}

void PCIMetrics::OnBytesDiscardedByTooBigPacket(size_t bytes_discarded_by_too_big_packet) noexcept {
    if (bytesDiscardedByTooBigPacket) {
        bytesDiscardedByTooBigPacket->Add(bytes_discarded_by_too_big_packet);
    }
}

void PCIMetrics::OnBytesDiscardedByBufferOverflow(size_t bytes_discarded_by_buffer_overflow) noexcept {
    if (bytesDiscardedByBufferOverflow) {
        bytesDiscardedByBufferOverflow->Add(bytes_discarded_by_buffer_overflow);
    }
}

void PCIMetrics::OnBytesDiscardedByPacketLimit(size_t bytes_discarded_by_packet_limit) noexcept {
    if (bytesDiscardedByPacketLimit) {
        bytesDiscardedByPacketLimit->Add(bytes_discarded_by_packet_limit);
    }
}

void PCIMetrics::OnBytesDiscardedByGarbageData(size_t bytes_discarded_by_garbage_data) noexcept {
    if (bytesDiscardedByGarbageData) {
        bytesDiscardedByGarbageData->Add(bytes_discarded_by_garbage_data);
    }
}

void PCIMetrics::OnBytesDiscardedByCorruptedPacket(size_t bytes_discarded_by_corrupted_packet) noexcept {
    if (bytesDiscardedByCorruptedPacket) {
        bytesDiscardedByCorruptedPacket->Add(bytes_discarded_by_corrupted_packet);
    }
}

void PCIMetrics::OnPacketsReceived(size_t packets_received) noexcept {
    if (packetsReceived) {
        packetsReceived->Add(packets_received);
    }
}

void PCIMetrics::OnPacketsCorrupted(size_t packets_corrupted) noexcept {
    if (packetsCorrupted) {
        packetsCorrupted->Add(packets_corrupted);
    }
}

void PCIMetrics::OnTooBigPacketAlarm() noexcept {
    if (tooBigPacketAlarm) {
        tooBigPacketAlarm->Add(1);
    }
}

void PCIMetrics::OnBufferOverflowAlarm() noexcept {
    if (bufferOverflowAlarm) {
        bufferOverflowAlarm->Add(1);
    }
}

void PCIMetrics::OnPacketLimitAlarm() noexcept {
    if (packetLimitAlarm) {
        packetLimitAlarm->Add(1);
    }
}

void PCIMetrics::printStatistics(const char *prefix) const {
    (void)prefix;
    //std::cout << "📊 " << prefix << " Metrics - Instrumentation active (OpenTelemetry metrics)" << std::endl;
    // Note: Individual metric values would need to be retrieved from the monitoring system
    // This is a placeholder implementation
}

// Implementation for PacketCatcherInstrumentationTypeMetrics
template <typename T, size_t N>
void PCITypeCounter<T, N>::InitCounter(std::vector<TypeDescriptor<T> > typesDescriptors, std::string metricName, monitoring::EventProcessor *processor) {
    for (const auto& descriptor : typesDescriptors) {
        monitoring::Labels labels;
        labels["packet_type"] = descriptor.label;
        packet_type_counter_map_[descriptor.type] = new metrics::Counter<uint64_t>(metricName, processor, "Packet type metric counter", "count", labels);
    }
}

template <typename T, size_t N>
void PCITypeCounter<T, N>::OnPacketType(T packet_type) {
    if (packet_type_counter_map_[packet_type]) {
        packet_type_counter_map_[packet_type]->Add(1);
    }
}

/*
// Implementation for PacketCatcherInstrumentationTypeMetrics
template <typename T, size_t N>
void PCITypeDuration<T, N>::InitDuration(std::vector<TypeDescriptor<T> > typesDescriptors, std::vector<uint64_t> boundaries, std::string metricName, monitoring::EventProcessor *processor) {
    processor->ConfigureHistogram<uint64_t>(metricName + "_microseconds", "Packet type metric histogram", boundaries, "microseconds", true);
    for (const auto& descriptor : typesDescriptors) {
        monitoring::Labels labels;
        labels["packet_type"] = descriptor.label;
        packet_type_duration_map_[descriptor.type] = new metrics::Histogram<uint64_t>(metricName, processor, "Packet type metric", "microseconds", labels);
    }
}

template <typename T, size_t N>
void PCITypeDuration<T, N>::OnPacketTypeDuration(T packet_type, uint64_t duration) {
    if (packet_type_duration_map_[packet_type]) {
        packet_type_duration_map_[packet_type]->Record(duration);
    }
}

*/

// Explicit template instantiations for CRSF and MSP
template struct PCITypeCounter<crsf::PacketType, 40>;
template struct PCITypeCounter<msp::PacketType, 50>;

}