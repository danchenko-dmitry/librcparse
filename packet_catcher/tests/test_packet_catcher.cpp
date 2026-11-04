#include "packet_catcher.h"
#include "instrumentation.h"
#include <iostream>
#include <vector>
#include <memory>
#include <cassert>

// Test runner for automated testing
class TestRunner {
private:
    size_t passedTests = 0;
    size_t failedTests = 0;
    
public:
    template<typename T>
    void assertEqual(const T& actual, const T& expected, const std::string& message) {
        if (actual == expected) {
            passedTests++;
            std::cout << "PASS: " << message << std::endl;
        } else {
            failedTests++;
            std::cout << "FAIL: " << message << " (expected " << static_cast<size_t>(expected) << ", got " << static_cast<size_t>(actual) << ")" << std::endl;
        }
    }
    
    // Specialization for vector<uint8_t> to avoid stream operator issues
    void assertEqual(const std::vector<uint8_t>& actual, const std::vector<uint8_t>& expected, const std::string& message) {
        if (actual == expected) {
            passedTests++;
            std::cout << "PASS: " << message << std::endl;
        } else {
            failedTests++;
            std::cout << "FAIL: " << message << " (vector sizes: expected " << expected.size() << ", got " << actual.size() << ")" << std::endl;
        }
    }
    
    void assertTrue(bool condition, const std::string& message) {
        if (condition) {
            passedTests++;
            std::cout << "PASS: " << message << std::endl;
        } else {
            failedTests++;
            std::cout << "FAIL: " << message << std::endl;
        }
    }
    
    void assertFalse(bool condition, const std::string& message) {
        assertTrue(!condition, message);
    }
    
    void printSummary() const {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        std::cout << "Total: " << (passedTests + failedTests) << std::endl;
    }
    
    size_t getPassedTests() const { return passedTests; }
    size_t getFailedTests() const { return failedTests; }
};

// Global test runner instance
TestRunner testRunner;

// Helper function to create PacketCatcher with instrumentation
std::pair<std::unique_ptr<PacketCatcher>, std::unique_ptr<packet_catcher_instrumentation::PCICout>> createPacketCatcherWithInstrumentation() {
    auto catcher = std::make_unique<PacketCatcher>();
    auto pci_cout = std::make_unique<packet_catcher_instrumentation::PCICout>();
    catcher->setInstrumentation(pci_cout.get());
    return {std::move(catcher), std::move(pci_cout)};
}

// Test data structures
struct ReceivedPacketInfo {
    uint8_t sync;
    uint8_t length;
    uint8_t type;
    std::vector<uint8_t> payload;
};
std::vector<ReceivedPacketInfo> receivedPackets;

// Mock protocol callbacks for testing
ProtocolCallbacks createMockProtocolCallbacks() {
    ProtocolCallbacks callbacks;
    
    // Simple protocol: SYNC(1) + LENGTH(1) + TYPE(1) + PAYLOAD + CRC(1)
    callbacks.isPacketHead = [](const uint8_t* data, size_t size) -> bool {
        if (size < 1) return false;
        // Mock protocol: packet starts with 0xAA
        return data[0] == 0xAA;
    };
    
    callbacks.isValidPacket = [](const uint8_t* data, size_t size) -> bool {
        if (size < 4) return false; // Need at least SYNC + LENGTH + TYPE + CRC
        
        uint8_t sync = data[0];
        uint8_t length = data[1];
        (void)data[2]; // type - not used in validation
        
        // Basic validation: check sync byte and length consistency
        if (sync != 0xAA) return false;
        if (length < 3 || length > 64) return false; // 3 = TYPE + PAYLOAD + CRC, max 64 total
        if (size < length) return false;
        
        // Mock CRC validation: just check if last byte is not 0xFF
        uint8_t crc = data[length - 1];
        return crc != 0xFF;
    };
    
    callbacks.getTotalPacketSize = [](const uint8_t* data, size_t size) -> size_t {
        if (size < 2) return 0; // Need at least SYNC + LENGTH
        
        uint8_t sync = data[0];
        uint8_t length = data[1];
        
        if (sync != 0xAA) return 0;
        if (length < 3 || length > 64) return 0;
        
        return length;
    };
    
    callbacks.processValidPacket = [](const uint8_t* data, size_t size) {
        if (size >= 4) {
            ReceivedPacketInfo info;
            info.sync = data[0];
            info.length = data[1];
            info.type = data[2];
            
            // Extract payload (everything between TYPE and CRC)
            size_t payloadSize = size - 4; // SYNC + LENGTH + TYPE + CRC
            if (payloadSize > 0) {
                info.payload.assign(data + 3, data + 3 + payloadSize);
            }
            
            receivedPackets.push_back(info);
        }
    };
    
    return callbacks;
}

// Helper function to create a valid mock packet
std::vector<uint8_t> createValidMockPacket(uint8_t type, const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> packet;
    
    packet.push_back(0xAA); // SYNC
    packet.push_back(static_cast<uint8_t>(payload.size() + 3)); // LENGTH: TYPE + PAYLOAD + CRC
    packet.push_back(type); // TYPE
    packet.insert(packet.end(), payload.begin(), payload.end()); // PAYLOAD
    packet.push_back(0x00); // CRC (valid)
    
    return packet;
}

// Helper function to create a corrupted mock packet
std::vector<uint8_t> createCorruptedMockPacket(uint8_t type, const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> packet;
    
    packet.push_back(0xAA); // SYNC
    packet.push_back(static_cast<uint8_t>(payload.size() + 3)); // LENGTH: TYPE + PAYLOAD + CRC
    packet.push_back(type); // TYPE
    packet.insert(packet.end(), payload.begin(), payload.end()); // PAYLOAD
    packet.push_back(0xFF); // CRC (invalid)
    
    return packet;
}

// Test basic packet catcher initialization
void testBasicInitialization() {
    std::cout << "\n=== Testing Basic Initialization ===" << std::endl;
    
    auto [catcher, pci_cout] = createPacketCatcherWithInstrumentation();
    
    testRunner.assertTrue(catcher->isBufferEmpty(), "Buffer should be empty initially");
    testRunner.assertEqual(catcher->getBufferSize(), size_t{0}, "Initial buffer size should be 0");
    testRunner.assertEqual(catcher->getMaxBufferSize(), size_t{MAX_BUFFER_SIZE}, "Max buffer size should be MAX_BUFFER_SIZE");
    testRunner.assertFalse(catcher->isBufferFull(), "Buffer should not be full initially");
    
    auto stats = catcher->getStatistics();
            testRunner.assertEqual(stats.totalBytesProcessed, size_t{0}, "Initial total bytes processed should be 0");
        testRunner.assertEqual(stats.packetsReceived, size_t{0}, "Initial packets received should be 0");
}

// Test single packet processing
void testSinglePacketProcessing() {
    std::cout << "\n=== Testing Single Packet Processing ===" << std::endl;
    
    auto [catcher, pci_cout] = createPacketCatcherWithInstrumentation();
    catcher->setProtocolCallbacks(createMockProtocolCallbacks());
    
    // Clear received packets
    receivedPackets.clear();
    
    // Create a valid packet
    std::vector<uint8_t> payload = {0x01, 0x02, 0x03};
    auto packet = createValidMockPacket(0x10, payload);
    
    // Process the packet
    catcher->processBytes(packet.data(), packet.size());
    
    // Verify packet was received
            testRunner.assertEqual(receivedPackets.size(), size_t{1}, "One packet should be received");
        testRunner.assertEqual(receivedPackets[0].sync, uint8_t{0xAA}, "Packet sync should be 0xAA");
        testRunner.assertEqual(receivedPackets[0].type, uint8_t{0x10}, "Packet type should be 0x10");
        testRunner.assertEqual(receivedPackets[0].payload, payload, "Packet payload should match");
        
        // Verify buffer is empty after processing
        testRunner.assertTrue(catcher->isBufferEmpty(), "Buffer should be empty after processing");
        
        // Verify statistics
        auto stats = catcher->getStatistics();
        testRunner.assertEqual(stats.packetsReceived, size_t{1}, "One packet should be counted as received");
        testRunner.assertEqual(stats.totalBytesProcessed, packet.size(), "Total bytes processed should match packet size");
}

// Test multiple packet processing
void testMultiplePacketProcessing() {
    std::cout << "\n=== Testing Multiple Packet Processing ===" << std::endl;
    
    auto [catcher, pci_cout] = createPacketCatcherWithInstrumentation();
    catcher->setProtocolCallbacks(createMockProtocolCallbacks());
    
    // Clear received packets
    receivedPackets.clear();
    
    // Create multiple valid packets
    std::vector<uint8_t> payload1 = {0x01, 0x02};
    std::vector<uint8_t> payload2 = {0x03, 0x04, 0x05};
    auto packet1 = createValidMockPacket(0x20, payload1);
    auto packet2 = createValidMockPacket(0x30, payload2);
    
    // Combine packets
    std::vector<uint8_t> combined;
    combined.insert(combined.end(), packet1.begin(), packet1.end());
    combined.insert(combined.end(), packet2.begin(), packet2.end());
    
    // Process combined data
    catcher->processBytes(combined.data(), combined.size());
    
    // Verify both packets were received
            testRunner.assertEqual(receivedPackets.size(), size_t{2}, "Two packets should be received");
        testRunner.assertEqual(receivedPackets[0].type, uint8_t{0x20}, "First packet type should be 0x20");
        testRunner.assertEqual(receivedPackets[1].type, uint8_t{0x30}, "Second packet type should be 0x30");
        
        // Verify buffer is empty after processing
        testRunner.assertTrue(catcher->isBufferEmpty(), "Buffer should be empty after processing");
        
        // Verify statistics
        auto stats = catcher->getStatistics();
        testRunner.assertEqual(stats.packetsReceived, size_t{2}, "Two packets should be counted as received");
}

// Test incomplete packet handling
void testIncompletePacketHandling() {
    std::cout << "\n=== Testing Incomplete Packet Handling ===" << std::endl;
    
    auto [catcher, pci_cout] = createPacketCatcherWithInstrumentation();
    catcher->setProtocolCallbacks(createMockProtocolCallbacks());
    
    // Clear received packets
    receivedPackets.clear();
    
    // Create a valid packet
    std::vector<uint8_t> payload = {0x01, 0x02, 0x03};
    auto packet = createValidMockPacket(0x40, payload);
    
    // Send only part of the packet
    size_t partialSize = packet.size() - 2; // Remove last 2 bytes
    catcher->processBytes(packet.data(), partialSize);
    
    // Verify no packet was received yet
            testRunner.assertEqual(receivedPackets.size(), size_t{0}, "No packet should be received yet");
        testRunner.assertFalse(catcher->isBufferEmpty(), "Buffer should not be empty");
        testRunner.assertEqual(catcher->getBufferSize(), partialSize, "Buffer should contain partial packet");
        
        // Send the remaining bytes
        catcher->processBytes(packet.data() + partialSize, packet.size() - partialSize);
        
        // Verify packet was received
        testRunner.assertEqual(receivedPackets.size(), size_t{1}, "One packet should be received after completion");
        testRunner.assertEqual(receivedPackets[0].type, uint8_t{0x40}, "Packet type should be 0x40");
        testRunner.assertTrue(catcher->isBufferEmpty(), "Buffer should be empty after processing");
}

// Test corrupted packet handling
void testCorruptedPacketHandling() {
    std::cout << "\n=== Testing Corrupted Packet Handling ===" << std::endl;
    
    auto [catcher, pci_cout] = createPacketCatcherWithInstrumentation();
    catcher->setProtocolCallbacks(createMockProtocolCallbacks());
    
    // Clear received packets
    receivedPackets.clear();
    
    // Create a corrupted packet
    std::vector<uint8_t> payload = {0x01, 0x02};
    auto corruptedPacket = createCorruptedMockPacket(0x50, payload);
    
    // Process the corrupted packet
    catcher->processBytes(corruptedPacket.data(), corruptedPacket.size());
    
    // Verify no packet was received
            testRunner.assertEqual(receivedPackets.size(), size_t{0}, "No packet should be received from corrupted data");
        
        // Verify statistics
        auto stats = catcher->getStatistics();
        testRunner.assertEqual(stats.packetsCorrupted, size_t{1}, "One packet should be counted as corrupted");
    
    // Verify buffer is empty (corrupted packet should be discarded)
    testRunner.assertTrue(catcher->isBufferEmpty(), "Buffer should be empty after discarding corrupted packet");
}

// Test garbage data handling
void testGarbageDataHandling() {
    std::cout << "\n=== Testing Garbage Data Handling ===" << std::endl;
    
    auto [catcher, pci_cout] = createPacketCatcherWithInstrumentation();
    catcher->setProtocolCallbacks(createMockProtocolCallbacks());
    
    // Clear received packets
    receivedPackets.clear();
    
    // Create garbage data (doesn't start with 0xAA)
    std::vector<uint8_t> garbage = {0xBB, 0xCC, 0xDD, 0xEE};
    
    // Process garbage data
    catcher->processBytes(garbage.data(), garbage.size());
    
    // Verify no packet was received
    testRunner.assertEqual(receivedPackets.size(), size_t{0}, "No packet should be received from garbage data");
    
    // Verify buffer is empty (garbage should be discarded)
    testRunner.assertTrue(catcher->isBufferEmpty(), "Buffer should be empty after discarding garbage");
    
    // Verify statistics
    auto stats = catcher->getStatistics();
    testRunner.assertEqual(stats.totalBytesDiscarded, garbage.size(), "Garbage bytes should be counted as discarded");
}

// Test buffer overflow handling
void testBufferOverflowHandling() {
    std::cout << "\n=== Testing Buffer Overflow Handling ===" << std::endl;
    
    auto [catcher, pci_cout] = createPacketCatcherWithInstrumentation();
    catcher->setProtocolCallbacks(createMockProtocolCallbacks());
    
    // Clear received packets
    receivedPackets.clear();
    
    // Create data larger than buffer capacity
    std::vector<uint8_t> largeData(200, 0xAA); // Larger than MAX_BUFFER_SIZE (192)
    
    // Process oversized data
    catcher->processBytes(largeData.data(), largeData.size());
    
    // Verify no packet was received
    testRunner.assertEqual(receivedPackets.size(), size_t{0}, "No packet should be received from oversized data");
    
    // Verify statistics
    auto stats = catcher->getStatistics();
    testRunner.assertEqual(stats.totalBytesDiscarded, largeData.size(), "Oversized data should be counted as discarded");
    testRunner.assertEqual(stats.bufferOverflowAlarm, size_t{1}, "Buffer overflow alarm should be triggered");
}

// Test mixed valid and invalid data
void testMixedDataHandling() {
    std::cout << "\n=== Testing Mixed Data Handling ===" << std::endl;
    
    auto [catcher, pci_cout] = createPacketCatcherWithInstrumentation();
    catcher->setProtocolCallbacks(createMockProtocolCallbacks());
    
    // Clear received packets
    receivedPackets.clear();
    
    // Create mixed data: garbage + valid packet + garbage
    std::vector<uint8_t> garbage1 = {0xBB, 0xCC};
    std::vector<uint8_t> payload = {0x01, 0x02};
    auto validPacket = createValidMockPacket(0x60, payload);
    std::vector<uint8_t> garbage2 = {0xDD, 0xEE};
    
    // Combine data
    std::vector<uint8_t> mixed;
    mixed.insert(mixed.end(), garbage1.begin(), garbage1.end());
    mixed.insert(mixed.end(), validPacket.begin(), validPacket.end());
    mixed.insert(mixed.end(), garbage2.begin(), garbage2.end());
    
    // Process mixed data
    catcher->processBytes(mixed.data(), mixed.size());
    
    // Verify valid packet was received
    testRunner.assertEqual(receivedPackets.size(), size_t{1}, "One valid packet should be received");
    testRunner.assertEqual(receivedPackets[0].type, uint8_t{0x60}, "Packet type should be 0x60");
    
    // Verify buffer is empty
    testRunner.assertTrue(catcher->isBufferEmpty(), "Buffer should be empty after processing");
    
    // Verify statistics
    auto stats = catcher->getStatistics();
    testRunner.assertEqual(stats.packetsReceived, size_t{1}, "One packet should be counted as received");
    testRunner.assertEqual(stats.totalBytesDiscarded, garbage1.size() + garbage2.size(), "Garbage bytes should be counted as discarded");
}

// Test statistics reset
void testStatisticsReset() {
    std::cout << "\n=== Testing Statistics Reset ===" << std::endl;
    
    auto [catcher, pci_cout] = createPacketCatcherWithInstrumentation();
    catcher->setProtocolCallbacks(createMockProtocolCallbacks());
    
    // Process some data to generate statistics
    std::vector<uint8_t> payload = {0x01};
    auto packet = createValidMockPacket(0x70, payload);
    catcher->processBytes(packet.data(), packet.size());
    
    // Verify statistics are not zero
    auto stats = catcher->getStatistics();
    testRunner.assertFalse(stats.packetsReceived == 0, "Statistics should not be zero after processing");
    
    // Reset statistics
    catcher->resetStatistics();
    
    // Verify statistics are zero
    stats = catcher->getStatistics();
    testRunner.assertEqual(stats.packetsReceived, size_t{0}, "Packets received should be zero after reset");
    testRunner.assertEqual(stats.totalBytesProcessed, size_t{0}, "Total bytes processed should be zero after reset");
}

// Test protocol callbacks update
void testProtocolCallbacksUpdate() {
    std::cout << "\n=== Testing Protocol Callbacks Update ===" << std::endl;
    
    auto [catcher, pci_cout] = createPacketCatcherWithInstrumentation();
    
    // Initially no callbacks set
    std::vector<uint8_t> data = {0xAA, 0x03, 0x10, 0x00}; // Valid packet format
    catcher->processBytes(data.data(), data.size());
    
    // Verify no packet was processed (no callbacks)
    auto stats = catcher->getStatistics();
    testRunner.assertEqual(stats.packetsReceived, size_t{0}, "No packets should be processed without callbacks");
    
    // Set callbacks
    catcher->setProtocolCallbacks(createMockProtocolCallbacks());
    
    // Process same data again
    catcher->processBytes(data.data(), data.size());
    
    // Verify packet was processed
    stats = catcher->getStatistics();
    testRunner.assertEqual(stats.packetsReceived, size_t{1}, "Packet should be processed after setting callbacks");
}

// Main test function (can be called from test runner)
int run_packet_catcher_tests() {
    std::cout << "Starting PacketCatcher Tests..." << std::endl;
    
    // Reset test runner for clean run
    testRunner = TestRunner();
    
    // Run all tests
    testBasicInitialization();
    testSinglePacketProcessing();
    testMultiplePacketProcessing();
    testIncompletePacketHandling();
    testCorruptedPacketHandling();
    testGarbageDataHandling();
    testBufferOverflowHandling();
    testMixedDataHandling();
    testStatisticsReset();
    testProtocolCallbacksUpdate();
    
    // Print test summary
    testRunner.printSummary();
    
    // Return 0 if all tests passed, 1 if any failed
    return (testRunner.getFailedTests() == 0) ? 0 : 1;
}
