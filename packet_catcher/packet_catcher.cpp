#include "packet_catcher.h"
#include <algorithm>
#include <cstring> // Required for std::memcpy
#include <iostream> // Required for std::cout

PacketCatcher::PacketCatcher() 
    : buffer_(), bufferSize_(0), protocolCallbacks_(), instrumentation_(nullptr) {
    // Buffer is automatically initialized to zeros
}

void PacketCatcher::setInstrumentation(packet_catcher_instrumentation::PacketCatcherInstrumentation* instrumentation) {
    instrumentation_ = instrumentation;
}

packet_catcher_instrumentation::Statistics PacketCatcher::getStatistics() const {
    packet_catcher_instrumentation::Statistics stats;
    if (instrumentation_) {
        // Try to cast to PCICout to get statistics
        auto* pci_cout = dynamic_cast<packet_catcher_instrumentation::PCICout*>(instrumentation_);
        if (pci_cout) {
            stats = pci_cout->getStatistics();
        }
    }
    return stats;
}

void PacketCatcher::resetStatistics() {
    if (instrumentation_) {
        // Try to cast to PCICout to reset statistics
        auto* pci_cout = dynamic_cast<packet_catcher_instrumentation::PCICout*>(instrumentation_);
        if (pci_cout) {
            pci_cout->resetStatistics();
        }
    }
}

void PacketCatcher::tooBigPacketReceived(const uint8_t* data, size_t length) {
    (void)data; // Suppress unused parameter warning
    if (instrumentation_) {
        instrumentation_->OnBytesDiscardedByTooBigPacket(length);
        instrumentation_->OnBytesDiscarded(length);
        instrumentation_->OnTooBigPacketAlarm();
    }
}

void PacketCatcher::bufferOverflowReceived(const uint8_t* data, size_t length) {
    (void)data; // Suppress unused parameter warning
    if (instrumentation_) {
        instrumentation_->OnBytesDiscardedByBufferOverflow(length);
        instrumentation_->OnBytesDiscarded(length);
        instrumentation_->OnBufferOverflowAlarm();
    }
}

void PacketCatcher::processBytes(const uint8_t* data, size_t length) {
    if (!data || length == 0) return;

    if (instrumentation_) {
        instrumentation_->OnBytesProcessed(length);
    }
    
    // CRITICAL: Check if buffer can accommodate the entire data chunk contiguously
    // We need to ensure data is written in one contiguous piece for etl::span linking
    
    if (length > MAX_BUFFER_SIZE) {
        // Data chunk is larger than buffer capacity - this is a fundamental problem
        // In a real-time system, this might indicate corrupted data or wrong configuration
        tooBigPacketReceived(data, length);
        return;
    }
    
    // Check if we can write the data contiguously
    if (canAccommodateContiguously(length)) {
        // We have enough space - write data normally
        writeDataContiguously(data, length);
    } else {
        // Not enough space - this indicates invalid protocol data
        // Clear the buffer and report the alarm situation
        bufferOverflowReceived(buffer_.data(), bufferSize_);
        
        clearBuffer();
        
        // Now write the new data
        writeDataContiguously(data, length);
    }
    
    processBuffer();
}

bool PacketCatcher::canAccommodateContiguously(size_t length) const {
    if (length > MAX_BUFFER_SIZE) {
        return false;
    }
    
    // Check if we have enough space in the buffer
    return (bufferSize_ + length) <= MAX_BUFFER_SIZE;
}

void PacketCatcher::writeDataContiguously(const uint8_t* data, size_t length) {
    // Use bulk copy operation which can be hardware accelerated
    std::memcpy(buffer_.data() + bufferSize_, data, length);
    bufferSize_ += length;
}

void PacketCatcher::clearBuffer() {
    discardDataFromStartTo(bufferSize_);
}


void PacketCatcher::processValidPacket(const uint8_t* data, size_t size) {
    if (instrumentation_) {
        instrumentation_->OnBytesValidated(size);
        instrumentation_->OnPacketsReceived(1);
    }
    if (protocolCallbacks_.processValidPacket) {
        // Call the callback with the raw packet data
        protocolCallbacks_.processValidPacket(data, size);
    }
}

void PacketCatcher::garbageDataReceived(const uint8_t* data, size_t length) {
    (void)data; // Suppress unused parameter warning
    if (instrumentation_) {
        instrumentation_->OnBytesDiscardedByGarbageData(length);
        instrumentation_->OnBytesDiscarded(length);
    }
}

void PacketCatcher::packetsHitLimit(const uint8_t* data, size_t length) {
    (void)data; // Suppress unused parameter warning
    if (instrumentation_) {
        instrumentation_->OnBytesDiscardedByPacketLimit(length);
        instrumentation_->OnBytesDiscarded(length);
        instrumentation_->OnPacketLimitAlarm();
    }
}

void PacketCatcher::corruptedPacket(const uint8_t* data, size_t length) {
    (void)data; // Suppress unused parameter warning
    if (instrumentation_) {
        instrumentation_->OnBytesDiscardedByCorruptedPacket(length);
        instrumentation_->OnBytesDiscarded(length);
        instrumentation_->OnPacketsCorrupted(1);
    }
}

bool PacketCatcher::processBuffer() {
    // First, check if we have incomplete data at the beginning that can't form a valid packet
    // This prevents incomplete data from interfering with subsequent complete packets

    bool packetProcessed = false;
    size_t packetProcessedCount = 0;

    size_t pos = 0;
    
    // Process packets directly from buffer
    for (; bufferSize_ > 0 ; ) {

        
        pos = findNextPacketStart(pos);
        
        if (pos >= bufferSize_) {
            // it can be a valid packet, but we need more data to process it
            // so we need to wait for more data
            break;
        }

        if (pos != 0) {      
            garbageDataReceived(buffer_.data(),pos);      
            discardDataFromStartTo(pos); // Count as discarded (garbage data)
            pos = 0; // Reset position after discarding
        }

        // Check if we have a complete packet
        if (!isCompletePacket(pos)) {
            // Incomplete packet, wait for more data
            break;
        }

        // Get packet length using protocol callback
        size_t packetLength = protocolCallbacks_.getTotalPacketSize(buffer_.data() + pos, bufferSize_ - pos);
        if (packetLength == 0) {
            // Invalid packet length, skip this position
            pos++;
            continue;
        }
        
        
        // Validate the packet
        if (validatePacket(pos, packetLength)) {
            if (packetProcessedCount < MAX_PACKETS_PER_CALL) {
                processValidPacket(buffer_.data() + pos, packetLength);
                
                // Mark bytes to remove for processed packets
                skipDataFromStartTo(pos + packetLength); // Don't count as discarded (normal processing)
                
                packetProcessedCount++;
            } else {
                packetsHitLimit(buffer_.data()+pos,packetLength);
                
                // When we hit the limit, we still need to discard this valid packet
                // but don't process it (just count it as discarded)
                discardDataFromStartTo(pos + packetLength); // Count as discarded (hit packet limit)
                packetProcessedCount++;
                
                // Continue processing to find and discard remaining valid packets
                // but don't break - let the loop continue to handle remaining packets
            }
            
            packetProcessed = true;
        } else {
            // Corrupted packet - search INSIDE it for valid packet start
            corruptedPacket(buffer_.data()+pos,packetLength);
            
            pos++; // lets push to the next loop .. (it will find next packet and discard corrupt body)
        }
    }
    
    return packetProcessed;
}

void PacketCatcher::skipDataFromStartTo(size_t bytesToRemove) {
    if (bytesToRemove <= 0){
        return;
    }

    // Use bulk shift operation which can be hardware accelerated
    if (bytesToRemove < bufferSize_) {
        std::memmove(buffer_.data(), buffer_.data() + bytesToRemove, bufferSize_ - bytesToRemove);
        bufferSize_ -= bytesToRemove;
    } else {
        // If we're removing more bytes than we have, clear the entire buffer
        bufferSize_ = 0;
    }
}

void PacketCatcher::discardDataFromStartTo(size_t bytesToRemove) {
    if (instrumentation_) {
        instrumentation_->OnBytesDiscarded(bytesToRemove);
    }

    // Call discarded data callback if initialized and we're discarding data
    if (bytesToRemove > 0 && protocolCallbacks_.discardedData) {
        protocolCallbacks_.discardedData(buffer_.data(), bytesToRemove);
    }
    
    skipDataFromStartTo(bytesToRemove);
}

void PacketCatcher::setProtocolCallbacks(const ProtocolCallbacks& callbacks) {
    protocolCallbacks_ = callbacks;
}

bool PacketCatcher::isValidPacketHead(size_t startPos) const {
    if (startPos >= bufferSize_) {
        return false;
    }
    
    // Use protocol callback to check if this looks like a valid packet start
    if (protocolCallbacks_.isPacketHead) {
        return protocolCallbacks_.isPacketHead(buffer_.data() + startPos, bufferSize_ - startPos);
    }

    // Fallback to basic checks if no callback is set
    // Without protocol callbacks, we can't determine if this is a valid packet start
    return false;
}

bool PacketCatcher::isCompletePacket(size_t startPos) const {
    if (startPos >= bufferSize_) return false;

    size_t totalPacketSize = protocolCallbacks_.getTotalPacketSize(buffer_.data() + startPos, bufferSize_ - startPos);

    if (startPos+totalPacketSize <= bufferSize_) {
        return true;
    }

    return false;
}

bool PacketCatcher::validatePacket(size_t startPos, size_t packetLength) const {
    if (startPos + packetLength > bufferSize_) return false;
    
    // Use protocol callback to validate the packet
    if (protocolCallbacks_.isValidPacket) {
        return protocolCallbacks_.isValidPacket(buffer_.data() + startPos, packetLength);
    }
    
    // Fallback to basic validation if no callback is set
    // Without protocol callbacks, we can't validate the packet
    return false;
}

size_t PacketCatcher::findNextPacketStart(size_t startPos) const {
    for (size_t i = startPos; i < bufferSize_; ++i) {
        if (isValidPacketHead(i)) {
            return i;
        }
    }
    
    return bufferSize_; // No valid packet start found
}
