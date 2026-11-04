#include "crsf.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>

// Simple test runner class
class TestRunner {
public:
    void assertTrue(bool condition, const std::string& message) {
        if (!condition) {
            std::cout << "FAIL: " << message << std::endl;
            failedTests_++;
        } else {
            std::cout << "PASS: " << message << std::endl;
            passedTests_++;
        }
    }
    
    void assertFalse(bool condition, const std::string& message) {
        assertTrue(!condition, message);
    }
    
    void assertEqual(const std::string& actual, const std::string& expected, const std::string& message) {
        if (actual == expected) {
            std::cout << "PASS: " << message << std::endl;
            passedTests_++;
        } else {
            std::cout << "FAIL: " << message << " (expected: " << expected << ", got: " << actual << ")" << std::endl;
            failedTests_++;
        }
    }
    
    void printSummary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Passed: " << passedTests_ << std::endl;
        std::cout << "Failed: " << failedTests_ << std::endl;
        std::cout << "Total: " << (passedTests_ + failedTests_) << std::endl;
    }
    
private:
    int passedTests_ = 0;
    int failedTests_ = 0;
};

// Global test runner instance
TestRunner testRunner;

void testBasePacket() {
    std::cout << "\n=== Testing Base CRSF Packet ===" << std::endl;
    
    // Create a buffer for the packet
    std::vector<uint8_t> buffer(66, 0);
    etl::span<uint8_t> data(buffer);
    
    // Set up packet data manually for testing
    data[0] = 0xEE;  // address
    data[1] = 0x05;  // length (type + payload + crc)
    data[2] = static_cast<uint8_t>(crsf::PacketType::Data);  // type
    data[3] = 0x01;  // payload byte 1
    data[4] = 0x02;  // payload byte 2
    data[5] = 0x03;  // payload byte 3
    // CRC will be calculated by the packet
    
    // Test packet creation with factory function
    auto packet = crsf::CRSFPacket::createPacket(crsf::PacketType::Data, data);
    
    testRunner.assertTrue(packet != nullptr, "Packet should be created successfully");
    testRunner.assertTrue(packet->getAddress() == 0xEE, "Address should be set correctly");
    testRunner.assertTrue(packet->getType() == static_cast<uint8_t>(crsf::PacketType::Data), "Type should be set correctly");
    testRunner.assertTrue(packet->getLength() == 5, "Length should be set correctly"); // type(1) + payload(3) + crc(1) = 5
    
    // Test getters
    auto retrievedPayload = packet->getPayload();
    testRunner.assertTrue(retrievedPayload.size() == 3, "Payload size should be 3");
    testRunner.assertTrue(retrievedPayload[0] == 0x01, "First payload byte should be 0x01");
    testRunner.assertTrue(retrievedPayload[1] == 0x02, "Second payload byte should be 0x02");
    testRunner.assertTrue(retrievedPayload[2] == 0x03, "Third payload byte should be 0x03");
    
    // Test setters
    packet->setAddress(0xAA);
    packet->setType(static_cast<uint8_t>(crsf::PacketType::RCChannelsPacked));
    
    // Create new payload for testing
    std::vector<uint8_t> newPayloadData = {0x04, 0x05, 0x06};
    etl::span<uint8_t> newPayload(newPayloadData);
    packet->setPayload(newPayload);
    
    testRunner.assertTrue(packet->getAddress() == 0xAA, "Address should be updated");
    testRunner.assertTrue(packet->getType() == static_cast<uint8_t>(crsf::PacketType::RCChannelsPacked), "Type should be updated");
    
    auto finalPayload = packet->getPayload();
    testRunner.assertTrue(finalPayload.size() == 3, "New payload size should be 3");
    testRunner.assertTrue(finalPayload[0] == 0x04, "New first payload byte should be 0x04");
}

void testRCChannels() {
    std::cout << "\n=== Testing RC Channels Packet ===" << std::endl;
    
    // Create a buffer for the packet
    std::vector<uint8_t> buffer(66, 0);
    etl::span<uint8_t> data(buffer);
    
    // Test RC channels creation (without arm status initially)
    // Use CRSF values directly (0-1984 range) as per CRSF specification
    crsf::CRSFRCChannels rcPacket(data);
    crsf::bindRCChannels(rcPacket, 0xEE, {192, 992, 1792, 192, 992, 1792, 192, 992, 1792, 192, 992, 1792, 192, 992, 1792, 192}, false, false, data);
    
    testRunner.assertTrue(rcPacket.getAddress() == 0xEE, "Address should be set correctly");
    testRunner.assertTrue(rcPacket.getType() == static_cast<uint8_t>(crsf::PacketType::RCChannelsPacked), "Type should be RCChannelsPacked");
    
    testRunner.assertTrue(rcPacket.getLength() == 24, "Length should be 24 (type(1) + payload(22) + crc(1))");
    
    // Test getters
    auto channels = rcPacket.getAllChannels();
    testRunner.assertTrue(channels.size() == 16, "Should have 16 channels");
    testRunner.assertTrue(channels[0] == 192, "First channel should be 192");
    testRunner.assertTrue(channels[1] == 992, "Second channel should be 992");
    testRunner.assertTrue(channels[2] == 1792, "Third channel should be 1792");
    
    // Test setters
    rcPacket.setChannel(0, 1000);
    rcPacket.setChannel(1, 1500);
    rcPacket.setChannel(2, 1800);  // Use valid CRSF value (0-1984 range)
    
    testRunner.assertTrue(rcPacket.getChannel(0) == 1000, "Channel 0 should be updated to 1000");
    testRunner.assertTrue(rcPacket.getChannel(1) == 1500, "Channel 1 should be updated to 1500");
    testRunner.assertTrue(rcPacket.getChannel(2) == 1800, "Channel 2 should be updated to 1800");
    
    // Test validation
    testRunner.assertTrue(rcPacket.isValid(), "RC Channels packet should be valid");
    
    // Test RC channels with arm status (should have length 25)
    std::vector<uint8_t> bufferWithArm(66, 0);
    etl::span<uint8_t> dataWithArm(bufferWithArm);
    crsf::CRSFRCChannels rcPacketWithArm(dataWithArm);
    crsf::bindRCChannels(rcPacketWithArm, 0xEE, {192, 992, 1792, 192, 992, 1792, 192, 992, 1792, 192, 992, 1792, 192, 992, 1792, 192}, true, true, dataWithArm);
    
    testRunner.assertTrue(rcPacketWithArm.getLength() == 25, "Length should be 25 (type(1) + payload(23) + crc(1)) when arm status is included");
    testRunner.assertTrue(rcPacketWithArm.isArmed(), "Should be armed when arm status is included");
    
    // Test that setArmed works correctly when arm status is included
    rcPacketWithArm.setArmed(false);
    testRunner.assertTrue(!rcPacketWithArm.isArmed(), "Should not be armed after setting to false");
}

void testPacketFactory() {
    std::cout << "\n=== Testing Packet Binding Functions ===" << std::endl;
    
    // Test RC Channels binding
    std::vector<uint8_t> rcBuffer(66, 0);
    etl::span<uint8_t> rcData(rcBuffer);
    crsf::CRSFRCChannels rcPacket(rcData);
    crsf::bindRCChannels(rcPacket, 0xEE, {192, 992, 1792, 192, 992, 1792, 192, 992, 1792, 192, 992, 1792, 192, 992, 1792, 192}, false, false, rcData);
    testRunner.assertTrue(rcPacket.getAddress() == 0xEE, "RC Channels binding should work");
    
    // Test Generic Packet factory (this one still returns unique_ptr)
    std::vector<uint8_t> genericBuffer(66, 0);
    etl::span<uint8_t> genericData(genericBuffer);
    std::vector<uint8_t> payloadData = {0x01, 0x02, 0x03};
    etl::span<uint8_t> payload(payloadData);
    auto genericPacket = crsf::CRSFPacket::createPacket(crsf::PacketType::Data, genericData);
    testRunner.assertTrue(genericPacket != nullptr, "Generic Packet factory should work");
}

void testValidation() {
    std::cout << "\n=== Testing Packet Validation ===" << std::endl;
    
    // Test valid packet - create with correct CRC
    std::vector<uint8_t> validBuffer = {0xEE, 0x03, 0x33, 0x01, 0x02, 0x03, 0x00}; // Will calculate correct CRC
    etl::span<uint8_t> validData(validBuffer);
    auto validPacket = crsf::CRSFPacket::createPacket(crsf::PacketType::Data, validData);
    testRunner.assertTrue(validPacket != nullptr, "Valid packet should be created");
    
    // Calculate correct CRC for the packet
    validPacket->calculateCRC();
    testRunner.assertTrue(validPacket->verifyCRC(), "Valid packet should pass CRC verification");
    
    // Test invalid packet (wrong length)
    std::vector<uint8_t> invalidBuffer = {0xEE, 0x01, 0x33, 0x01, 0x02, 0x03, 0x00}; // Length says 1 but payload is 3 bytes
    etl::span<uint8_t> invalidData(invalidBuffer);
    auto invalidPacket = crsf::CRSFPacket::createPacket(crsf::PacketType::Data, invalidData);
    testRunner.assertTrue(invalidPacket != nullptr, "Invalid packet should be created");
    // Check that declared length doesn't match actual payload size
    // Length field says 1, but we have 3 payload bytes (0x01, 0x02, 0x03)
    testRunner.assertTrue(invalidPacket->getLength() != 3, "Invalid packet should have mismatched length");
    
    // Test packet with wrong CRC
    std::vector<uint8_t> badCRCBuffer = {0xEE, 0x03, 0x33, 0x01, 0x02, 0x03, 0xFF}; // Wrong CRC
    etl::span<uint8_t> badCRCData(badCRCBuffer);
    auto badCRCPacket = crsf::CRSFPacket::createPacket(crsf::PacketType::Data, badCRCData);
    testRunner.assertTrue(badCRCPacket != nullptr, "Packet with wrong CRC should be created");
    testRunner.assertTrue(!badCRCPacket->verifyCRC(), "Packet with wrong CRC should fail CRC verification");
}

void testSubsetRCChannels() {
    std::cout << "\n=== Testing SUBSET RC CHANNELS Packet (CRSFv3) ===" << std::endl;
    
    // Create a buffer for the packet
    std::vector<uint8_t> buffer(66, 0);
    etl::span<uint8_t> data(buffer);
    
    // Test Subset RC Channels creation with 11-bit resolution
    std::vector<uint16_t> channels = {1000, 1500, 2000, 1000, 1500, 2000};
    crsf::CRSFSubsetRCChannels subsetPacket(data);
    crsf::bindSubsetRCChannels11Bit(subsetPacket, 0xEE, 0, channels, data);
    
    testRunner.assertTrue(subsetPacket.getAddress() == 0xEE, "Address should be set correctly");
    testRunner.assertTrue(subsetPacket.getType() == static_cast<uint8_t>(crsf::PacketType::SubsetRCChannelsPacked), "Type should be SubsetRCChannelsPacked");
    testRunner.assertTrue(subsetPacket.getLength() == 10, "Length should be 10 (type(1) + payload(8) + crc(1))");
    
    // Test getters
    testRunner.assertTrue(subsetPacket.getChannelCount() == 6, "Should have 6 channels");
    testRunner.assertTrue(subsetPacket.getChannel(0) == 1000, "First channel should be 1000");
    testRunner.assertTrue(subsetPacket.getChannel(1) == 1500, "Second channel should be 1500");
    testRunner.assertTrue(subsetPacket.getChannel(2) == 2000, "Third channel should be 2000");
    
    // Test setters
    subsetPacket.setChannel(0, 1200);
    subsetPacket.setChannel(1, 1800);
    
    testRunner.assertTrue(subsetPacket.getChannel(0) == 1200, "Channel 0 should be updated to 1200");
    testRunner.assertTrue(subsetPacket.getChannel(1) == 1800, "Channel 1 should be updated to 1800");
    
    // Test validation
    testRunner.assertTrue(subsetPacket.isValid(), "Subset RC Channels packet should be valid");
}

void testSubsetRCChannelsFactory() {
    std::cout << "\n=== Testing SUBSET RC CHANNELS Binding Functions ===" << std::endl;
    
    // Test bindSubsetRCChannels11Bit
    std::vector<uint8_t> buffer1(66, 0);
    etl::span<uint8_t> data1(buffer1);
    std::vector<uint16_t> channels1 = {1000, 1500, 2000};
    crsf::CRSFSubsetRCChannels subsetPacket1(data1);
    crsf::bindSubsetRCChannels11Bit(subsetPacket1, 0xEE, 0, channels1, data1);
    
    testRunner.assertTrue(subsetPacket1.getAddress() == 0xEE, "Address should be 0xEE");
    testRunner.assertTrue(subsetPacket1.getChannelCount() == 3, "Should have 3 channels");
    
    // Test bindSubsetRCChannels12Bit
    std::vector<uint8_t> buffer2(66, 0);
    etl::span<uint8_t> data2(buffer2);
    std::vector<uint16_t> channels2 = {1000, 1500, 2000, 1000, 1500, 2000, 1000, 1500};
    crsf::CRSFSubsetRCChannels subsetPacket2(data2);
    crsf::bindSubsetRCChannels12Bit(subsetPacket2, 0xC8, 1, channels2, data2);
    
    testRunner.assertTrue(subsetPacket2.getAddress() == 0xC8, "Address should be 0xC8");
    testRunner.assertTrue(subsetPacket2.getChannelCount() == 8, "Should have 8 channels");
    
    // Test bindSubsetRCChannels13Bit
    std::vector<uint8_t> buffer3(66, 0);
    etl::span<uint8_t> data3(buffer3);
    std::vector<uint16_t> channels3 = {1000, 1500, 2000, 1000, 1500, 2000, 1000, 1500, 2000, 1000, 1500, 2000, 1000, 1500, 2000, 1000};
    crsf::CRSFSubsetRCChannels subsetPacket3(data3);
    crsf::bindSubsetRCChannels13Bit(subsetPacket3, 0xEE, 2, channels3, data3);
    
    testRunner.assertTrue(subsetPacket3.getAddress() == 0xEE, "Address should be 0xEE");
    testRunner.assertTrue(subsetPacket3.getChannelCount() == 16, "Should have 16 channels");
    
    // Test PWM to CRSF conversion
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::pwmToCRSF(1000, 11) == 0, "1000μs should convert to CRSF 0 for 11-bit");
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::pwmToCRSF(1500, 11) == 1023, "1500μs should convert to CRSF 1023 for 11-bit");
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::pwmToCRSF(2000, 11) == 2047, "2000μs should convert to CRSF 2047 for 11-bit");
    
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::pwmToCRSF(1000, 12) == 0, "1000μs should convert to CRSF 0 for 12-bit");
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::pwmToCRSF(1500, 12) == 2047, "1500μs should convert to CRSF 2047 for 12-bit");
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::pwmToCRSF(2000, 12) == 4095, "2000μs should convert to CRSF 4095 for 12-bit");
    
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::pwmToCRSF(1000, 13) == 0, "1000μs should convert to CRSF 0 for 13-bit");
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::pwmToCRSF(1500, 13) == 4095, "1500μs should convert to CRSF 4095 for 13-bit");
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::pwmToCRSF(2000, 13) == 8191, "2000μs should convert to CRSF 8191 for 13-bit");
    
    // Test CRSF to PWM conversion
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::crsfToPWM(0, 11) == 1000, "CRSF 0 should convert to 1000μs for 11-bit");
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::crsfToPWM(1023, 11) == 1500, "CRSF 1023 should convert to 1500μs for 11-bit");
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::crsfToPWM(2047, 11) == 2000, "CRSF 2047 should convert to 2000μs for 11-bit");
    
    // Test constants
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::PWM_MIN == 998, "PWM_MIN should be 998");
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::PWM_MAX == 2012, "PWM_MAX should be 2012");
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::PWM_CENTER == 1500, "PWM_CENTER should be 1500");
    
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::CRSF_MAX_10BIT == 1023, "CRSF_MAX_10BIT should be 1023");
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::CRSF_MAX_11BIT == 2047, "CRSF_MAX_11BIT should be 2047");
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::CRSF_MAX_12BIT == 4095, "CRSF_MAX_12BIT should be 4095");
    testRunner.assertTrue(crsf::CRSFSubsetRCChannels::CRSF_MAX_13BIT == 8191, "CRSF_MAX_13BIT should be 8191");
}

void testCRCFunctionality() {
    std::cout << "\n=== Testing CRSF CRC Functionality ===" << std::endl;
    
    // Test 1: Simple packet with known CRC
    std::vector<uint8_t> buffer1(32, 0);
    etl::span<uint8_t> data1(buffer1);
    
    // Create a simple packet: address=0xC8, type=0x29 (DeviceInfo), length=4
    data1[0] = 0xC8;  // address
    data1[1] = 0x04;  // length (type + payload + crc = 1 + 2 + 1 = 4)
    data1[2] = 0x29;  // type (DeviceInfo)
    data1[3] = 0x01;  // payload byte 1
    data1[4] = 0x02;  // payload byte 2
    
    // Calculate CRC manually using CRSF polynomial 0xD5 (not simple XOR!)
    // CRC covers type + payload (bytes 2-4)
    uint8_t expectedCRC1 = crsf::calculateCRC8(&data1[2], 3);  // type + 2 payload bytes
    
    // Test calculateCRC method
    auto packet1 = std::make_unique<crsf::CRSFPacket>(data1);
    packet1->calculateCRC();
    
    uint8_t actualCRC1 = data1[5];  // CRC should be at position 5
    testRunner.assertTrue(actualCRC1 == expectedCRC1, 
        "CRC calculation should match manual calculation");
    
    // Test 2: RC Channels packet CRC verification
    std::vector<uint8_t> buffer2(66, 0);
    etl::span<uint8_t> data2(buffer2);
    
    // Create RC Channels packet for CRC testing
    crsf::CRSFRCChannels rcPacket(data2);
    crsf::bindRCChannels(rcPacket, 0xC8, {192, 992, 1792, 192, 992, 1792, 192, 992, 1792, 192, 992, 1792, 192, 992, 1792, 192}, false, false, data2);
    
    // Calculate CRC
    rcPacket.calculateCRC();
    
    // Verify CRC was written (should be at position 3 + 22 = 25 for basic packet)
    uint8_t crcValue = data2[25];  // CRC is at byte 25 for basic packet
    testRunner.assertTrue(crcValue != 0, "CRC should not be zero after calculation");
    
    // Test 3: Verify CRC verification
    bool crcValid = rcPacket.verifyCRC();
    testRunner.assertTrue(crcValid, "CRC verification should pass for valid packet");
    
    // Test 4: Corrupt CRC and verify it fails
    data2[25] ^= 0x01;  // Flip one bit in CRC
    bool crcInvalid = rcPacket.verifyCRC();
    testRunner.assertTrue(!crcInvalid, "CRC verification should fail for corrupted packet");
    
    std::cout << "CRC tests completed successfully!" << std::endl;
}

void testCommandPacket() {
    std::cout << "\n=== Testing CRSF Command Packet ===" << std::endl;
    
    // Test basic command packet creation
    std::vector<uint8_t> data(8); // sync + len + type + realm + subcommand + payload + crc8-ba + crc
    etl::span<uint8_t> dataSpan(data);
    
    CRSFCommand command(dataSpan);
    command.setAddress(0xC8);
    command.setType(static_cast<uint8_t>(PacketType::Command));
    command.setLength(5); // realm + subcommand + payload + crc8-ba
    
    // Set extended payload: realm + subcommand + crc8-ba
    std::vector<uint8_t> extendedPayload = {0x10, 0x01, 0x00}; // RX realm, Bind subcommand, CRC8-BA placeholder
    command.setExtendedPayload(etl::span<uint8_t>(extendedPayload));
    command.calculateCRC8BA();
    command.calculateCRC();
    
    testRunner.assertTrue(command.isValid(), "Command packet is valid");
    testRunner.assertTrue(command.getCommandRealm() == 0x10, "Command realm is correct");
    testRunner.assertTrue(command.verifyCRC8BA(), "CRC8-BA verification passes");
    testRunner.assertTrue(command.verifyCRC(), "Main CRC verification passes");
}

void testCommandRXPacket() {
    std::cout << "\n=== Testing CRSF Command RX Packet ===" << std::endl;
    
    // Test bind command
    std::vector<uint8_t> data(7); // sync + len + type + realm + subcommand + crc8-ba + crc
    etl::span<uint8_t> dataSpan(data);
    
    CRSFCommandRX commandRX(dataSpan);
    commandRX.setAddress(0xC8);
    commandRX.setType(static_cast<uint8_t>(PacketType::Command));
    commandRX.setLength(4); // realm + subcommand + crc8-ba
    
    commandRX.setBindCommand();
    commandRX.calculateCRC8BA();
    commandRX.calculateCRC();
    
    testRunner.assertTrue(commandRX.isValid(), "Command RX packet is valid");
    testRunner.assertTrue(commandRX.isBindCommand(), "Is bind command");
    testRunner.assertTrue(commandRX.getCommandRealm() == static_cast<uint8_t>(CommandRealm::RX), "Command realm is RX");
    testRunner.assertTrue(commandRX.getSubcommand() == static_cast<uint8_t>(CommandRXSubcommand::Bind), "Subcommand is Bind");
    
    // Test model select ID command
    std::vector<uint8_t> data2(8); // sync + len + type + realm + subcommand + modelID + crc8-ba + crc
    etl::span<uint8_t> dataSpan2(data2);
    
    CRSFCommandRX commandRX2(dataSpan2);
    commandRX2.setAddress(0xC8);
    commandRX2.setType(static_cast<uint8_t>(PacketType::Command));
    commandRX2.setLength(5); // realm + subcommand + modelID + crc8-ba
    
    commandRX2.setModelSelectIDCommand(54); // Model ID 54
    commandRX2.calculateCRC8BA();
    commandRX2.calculateCRC();
    
    testRunner.assertTrue(commandRX2.isValid(), "Model Select ID command packet is valid");
    testRunner.assertTrue(commandRX2.isModelSelectIDCommand(), "Is model select ID command");
    testRunner.assertTrue(commandRX2.getModelID() == 54, "Model ID is correct");
}

void testCommandGeneralPacket() {
    std::cout << "\n=== Testing CRSF Command General Packet ===" << std::endl;
    
    // Test CRSF speed proposal command
    std::vector<uint8_t> data(12); // sync + len + type + realm + subcommand + portID + baudRate(4) + crc8-ba + crc
    etl::span<uint8_t> dataSpan(data);
    
    CRSFCommandGeneral commandGeneral(dataSpan);
    commandGeneral.setAddress(0xC8);
    commandGeneral.setType(static_cast<uint8_t>(PacketType::Command));
    commandGeneral.setLength(9); // realm + subcommand + portID + baudRate(4) + crc8-ba
    
    commandGeneral.setCRSFSpeedProposal(1, 416666); // Port ID 1, 416666 bps
    commandGeneral.calculateCRC8BA();
    commandGeneral.calculateCRC();
    
    testRunner.assertTrue(commandGeneral.isValid(), "CRSF speed proposal packet is valid");
    testRunner.assertTrue(commandGeneral.isCRSFSpeedProposal(), "Is CRSF speed proposal");
    testRunner.assertTrue(commandGeneral.getCommandRealm() == static_cast<uint8_t>(CommandRealm::General), "Command realm is General");
    testRunner.assertTrue(commandGeneral.getSubcommand() == static_cast<uint8_t>(CommandGeneralSubcommand::CRSFSpeedProposal), "Subcommand is CRSFSpeedProposal");
    testRunner.assertTrue(commandGeneral.getPortID() == 1, "Port ID is correct");
    testRunner.assertTrue(commandGeneral.getProposedBaudRate() == 416666, "Proposed baud rate is correct");
    
    // Test CRSF speed response command
    std::vector<uint8_t> data2(8); // sync + len + type + realm + subcommand + portID + accept + crc8-ba + crc
    etl::span<uint8_t> dataSpan2(data2);
    
    CRSFCommandGeneral commandGeneral2(dataSpan2);
    commandGeneral2.setAddress(0xC8);
    commandGeneral2.setType(static_cast<uint8_t>(PacketType::Command));
    commandGeneral2.setLength(5); // realm + subcommand + portID + accept + crc8-ba
    
    commandGeneral2.setCRSFSpeedResponse(1, true); // Port ID 1, accepted
    commandGeneral2.calculateCRC8BA();
    commandGeneral2.calculateCRC();
    
    testRunner.assertTrue(commandGeneral2.isValid(), "CRSF speed response packet is valid");
    testRunner.assertTrue(commandGeneral2.isCRSFSpeedResponse(), "Is CRSF speed response");
    testRunner.assertTrue(commandGeneral2.getResponsePortID() == 1, "Response port ID is correct");
    testRunner.assertTrue(commandGeneral2.isAccepted(), "Response is accepted");
}

void testCommandBindingFunctions() {
    std::cout << "\n=== Testing CRSF Command Binding Functions ===" << std::endl;
    
    // Test bind command binding
    std::vector<uint8_t> data(7);
    etl::span<uint8_t> dataSpan(data);
    CRSFCommandRX commandRX(dataSpan);
    
    bindBindCommand(commandRX, 0xC8, dataSpan);
    
    testRunner.assertTrue(commandRX.isValid(), "Bind command binding creates valid packet");
    testRunner.assertTrue(commandRX.isBindCommand(), "Bound packet is bind command");
    
    // Test model select ID command binding
    std::vector<uint8_t> data2(8);
    etl::span<uint8_t> dataSpan2(data2);
    CRSFCommandRX commandRX2(dataSpan2);
    
    bindModelSelectIDCommand(commandRX2, 0xC8, 54, dataSpan2);
    
    testRunner.assertTrue(commandRX2.isValid(), "Model Select ID command binding creates valid packet");
    testRunner.assertTrue(commandRX2.isModelSelectIDCommand(), "Bound packet is model select ID command");
    testRunner.assertTrue(commandRX2.getModelID() == 54, "Bound packet has correct model ID");
    
    // Test CRSF speed proposal binding
    std::vector<uint8_t> data3(12);
    etl::span<uint8_t> dataSpan3(data3);
    CRSFCommandGeneral commandGeneral(dataSpan3);
    
    bindCRSFSpeedProposalCommand(commandGeneral, 0xC8, 1, 416666, dataSpan3);
    
    testRunner.assertTrue(commandGeneral.isValid(), "CRSF speed proposal binding creates valid packet");
    testRunner.assertTrue(commandGeneral.isCRSFSpeedProposal(), "Bound packet is CRSF speed proposal");
    testRunner.assertTrue(commandGeneral.getPortID() == 1, "Bound packet has correct port ID");
    testRunner.assertTrue(commandGeneral.getProposedBaudRate() == 416666, "Bound packet has correct baud rate");
    
    // Test CRSF speed response binding
    std::vector<uint8_t> data4(8);
    etl::span<uint8_t> dataSpan4(data4);
    CRSFCommandGeneral commandGeneral2(dataSpan4);
    
    bindCRSFSpeedResponseCommand(commandGeneral2, 0xC8, 1, true, dataSpan4);
    
    testRunner.assertTrue(commandGeneral2.isValid(), "CRSF speed response binding creates valid packet");
    testRunner.assertTrue(commandGeneral2.isCRSFSpeedResponse(), "Bound packet is CRSF speed response");
    testRunner.assertTrue(commandGeneral2.getResponsePortID() == 1, "Bound packet has correct response port ID");
    testRunner.assertTrue(commandGeneral2.isAccepted(), "Bound packet indicates acceptance");
}

int main() {
    std::cout << "Starting CRSF Library Tests..." << std::endl;
    
    try {
        testBasePacket();
        testRCChannels();
        testSubsetRCChannels();
        testSubsetRCChannelsFactory();
        testPacketFactory();
        testValidation();
        testCRCFunctionality();
        testCommandPacket();
        testCommandRXPacket();
        testCommandGeneralPacket();
        testCommandBindingFunctions();
        
        testRunner.printSummary();
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
