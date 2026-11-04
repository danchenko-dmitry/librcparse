#include "msp.h"
#include <iostream>
#include <vector>
#include <cassert>

// Simple test runner class
class TestRunner {
private:
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;

public:
    void assertTrue(bool condition, const std::string& message) {
        totalTests++;
        if (condition) {
            passedTests++;
            std::cout << "PASS: " << message << std::endl;
        } else {
            failedTests++;
            std::cout << "FAIL: " << message << std::endl;
        }
    }

    void printSummary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        std::cout << "Total: " << totalTests << std::endl;
    }
};

TestRunner testRunner;

void testBasePacket() {
    std::cout << "\n=== Testing Base MSP Packet ===" << std::endl;
    
    // Create a buffer for the packet
    std::vector<uint8_t> buffer(32, 0);
    etl::span<uint8_t> data(buffer);
    
    // Test basic packet creation
    msp::MSPPacket packet(data);
    
    testRunner.assertTrue(packet.getDirection() == static_cast<uint8_t>(msp::PacketDirection::RESPONSE), 
                         "Default direction should be RESPONSE");
    testRunner.assertTrue(packet.getTotalSize() >= msp::MIN_PACKET_SIZE, 
                         "Packet should have minimum size");
    
    // Test setting properties
    packet.setDirection(static_cast<uint8_t>(msp::PacketDirection::REQUEST));
    packet.setCommand(static_cast<uint8_t>(msp::PacketType::MSP_STATUS));
    packet.setPayloadSize(5);
    
    testRunner.assertTrue(packet.getDirection() == static_cast<uint8_t>(msp::PacketDirection::REQUEST), 
                         "Direction should be updated");
    testRunner.assertTrue(packet.getCommand() == static_cast<uint8_t>(msp::PacketType::MSP_STATUS), 
                         "Command should be updated");
    testRunner.assertTrue(packet.getPayloadSize() == 5, 
                         "Payload size should be updated");
}

void testRawIMU() {
    std::cout << "\n=== Testing MSP_RAW_IMU Packet ===" << std::endl;
    
    // Create a buffer for the packet
    std::vector<uint8_t> buffer(32, 0);
    etl::span<uint8_t> data(buffer);
    
    // Test Raw IMU packet creation
    msp::MSPRawIMU rawIMU(data);
    
    testRunner.assertTrue(rawIMU.getCommand() == static_cast<uint8_t>(msp::PacketType::MSP_RAW_IMU), 
                         "Command should be MSP_RAW_IMU");
    testRunner.assertTrue(rawIMU.getPayloadSize() == msp::MSPRawIMU::PAYLOAD_SIZE, 
                         "Payload size should be correct");
    
    // Test setting IMU data
    rawIMU.setAccX(1000);
    rawIMU.setAccY(2000);
    rawIMU.setAccZ(3000);
    rawIMU.setGyroX(4000);
    rawIMU.setGyroY(5000);
    rawIMU.setGyroZ(6000);
    rawIMU.setMagX(7000);
    rawIMU.setMagY(8000);
    rawIMU.setMagZ(9000);
    
    testRunner.assertTrue(rawIMU.getAccX() == 1000, "AccX should be 1000");
    testRunner.assertTrue(rawIMU.getAccY() == 2000, "AccY should be 2000");
    testRunner.assertTrue(rawIMU.getAccZ() == 3000, "AccZ should be 3000");
    testRunner.assertTrue(rawIMU.getGyroX() == 4000, "GyroX should be 4000");
    testRunner.assertTrue(rawIMU.getGyroY() == 5000, "GyroY should be 5000");
    testRunner.assertTrue(rawIMU.getGyroZ() == 6000, "GyroZ should be 6000");
    testRunner.assertTrue(rawIMU.getMagX() == 7000, "MagX should be 7000");
    testRunner.assertTrue(rawIMU.getMagY() == 8000, "MagY should be 8000");
    testRunner.assertTrue(rawIMU.getMagZ() == 9000, "MagZ should be 9000");
    
    // Calculate CRC before validation
    rawIMU.calculateCRC();
    
    // Test validation
    testRunner.assertTrue(rawIMU.isValid(), "Raw IMU packet should be valid");
}

void testStatus() {
    std::cout << "\n=== Testing MSP_STATUS Packet ===" << std::endl;
    
    // Create a buffer for the packet
    std::vector<uint8_t> buffer(32, 0);
    etl::span<uint8_t> data(buffer);
    
    // Test Status packet creation
    msp::MSPStatus status(data);
    
    testRunner.assertTrue(status.getCommand() == static_cast<uint8_t>(msp::PacketType::MSP_STATUS), 
                         "Command should be MSP_STATUS");
    testRunner.assertTrue(status.getPayloadSize() == msp::MSPStatus::PAYLOAD_SIZE, 
                         "Payload size should be correct");
    
    // Test setting status data
    status.setCycleTime(1000);
    status.setI2cErrorCount(5);
    status.setSensor(0x1234);
    status.setFlag(0x567890AB);
    status.setCurrentProfile(2);
    
    testRunner.assertTrue(status.getCycleTime() == 1000, "Cycle time should be 1000");
    testRunner.assertTrue(status.getI2cErrorCount() == 5, "I2C error count should be 5");
    testRunner.assertTrue(status.getSensor() == 0x1234, "Sensor should be 0x1234");
    testRunner.assertTrue(status.getFlag() == 0x567890AB, "Flag should be 0x567890AB");
    testRunner.assertTrue(status.getCurrentProfile() == 2, "Current profile should be 2");
    
    // Calculate CRC before validation
    status.calculateCRC();
    
    // Test validation
    testRunner.assertTrue(status.isValid(), "Status packet should be valid");
}

void testBindingFunctions() {
    std::cout << "\n=== Testing MSP Binding Functions ===" << std::endl;
    
    // Test Raw IMU binding
    std::vector<uint8_t> buffer1(32, 0);
    etl::span<uint8_t> data1(buffer1);
    msp::MSPRawIMU rawIMU(data1);
    msp::bindRawIMU(rawIMU, msp::PacketDirection::RESPONSE, 
                    1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, data1);
    
    testRunner.assertTrue(rawIMU.getDirection() == static_cast<uint8_t>(msp::PacketDirection::RESPONSE), 
                         "Direction should be RESPONSE");
    testRunner.assertTrue(rawIMU.getCommand() == static_cast<uint8_t>(msp::PacketType::MSP_RAW_IMU), 
                         "Command should be MSP_RAW_IMU");
    testRunner.assertTrue(rawIMU.getAccX() == 1000, "AccX should be 1000");
    testRunner.assertTrue(rawIMU.getAccY() == 2000, "AccY should be 2000");
    testRunner.assertTrue(rawIMU.getAccZ() == 3000, "AccZ should be 3000");
    
    // Test Status binding
    std::vector<uint8_t> buffer2(32, 0);
    etl::span<uint8_t> data2(buffer2);
    msp::MSPStatus status(data2);
    msp::bindStatus(status, msp::PacketDirection::RESPONSE, 
                    1000, 5, 0x1234, 0x567890AB, 2, data2);
    
    testRunner.assertTrue(status.getDirection() == static_cast<uint8_t>(msp::PacketDirection::RESPONSE), 
                         "Direction should be RESPONSE");
    testRunner.assertTrue(status.getCommand() == static_cast<uint8_t>(msp::PacketType::MSP_STATUS), 
                         "Command should be MSP_STATUS");
    testRunner.assertTrue(status.getCycleTime() == 1000, "Cycle time should be 1000");
    testRunner.assertTrue(status.getI2cErrorCount() == 5, "I2C error count should be 5");
}

void testMSPRequest() {
    std::cout << "\n=== Testing MSP Request Packets ===" << std::endl;
    
    // Test Raw IMU request
    std::vector<uint8_t> buffer1(32, 0);
    etl::span<uint8_t> data1(buffer1);
    msp::MSPRequest rawIMURequest(data1);
    msp::bindRawIMURequest(rawIMURequest, data1);
    
    testRunner.assertTrue(rawIMURequest.getDirection() == static_cast<uint8_t>(msp::PacketDirection::REQUEST), 
                         "Direction should be REQUEST");
    testRunner.assertTrue(rawIMURequest.getRequestCommand() == msp::PacketType::MSP_RAW_IMU, 
                         "Command should be MSP_RAW_IMU");
    testRunner.assertTrue(rawIMURequest.getPayloadSize() == 0, 
                         "Payload size should be 0 for requests");
    testRunner.assertTrue(rawIMURequest.isValid(), 
                         "Raw IMU request should be valid");
    
    // Test Status request
    std::vector<uint8_t> buffer2(32, 0);
    etl::span<uint8_t> data2(buffer2);
    msp::MSPRequest statusRequest(data2);
    msp::bindStatusRequest(statusRequest, data2);
    
    testRunner.assertTrue(statusRequest.getDirection() == static_cast<uint8_t>(msp::PacketDirection::REQUEST), 
                         "Direction should be REQUEST");
    testRunner.assertTrue(statusRequest.getRequestCommand() == msp::PacketType::MSP_STATUS, 
                         "Command should be MSP_STATUS");
    testRunner.assertTrue(statusRequest.getPayloadSize() == 0, 
                         "Payload size should be 0 for requests");
    testRunner.assertTrue(statusRequest.isValid(), 
                         "Status request should be valid");
    
    // Test generic request
    std::vector<uint8_t> buffer3(32, 0);
    etl::span<uint8_t> data3(buffer3);
    msp::MSPRequest genericRequest(data3);
    msp::bindGenericRequest(genericRequest, msp::PacketType::MSP_ATTITUDE, data3);
    
    testRunner.assertTrue(genericRequest.getDirection() == static_cast<uint8_t>(msp::PacketDirection::REQUEST), 
                         "Direction should be REQUEST");
    testRunner.assertTrue(genericRequest.getRequestCommand() == msp::PacketType::MSP_ATTITUDE, 
                         "Command should be MSP_ATTITUDE");
    testRunner.assertTrue(genericRequest.getPayloadSize() == 0, 
                         "Payload size should be 0 for requests");
    testRunner.assertTrue(genericRequest.isValid(), 
                         "Generic request should be valid");
    
    // Test packet size for requests
    testRunner.assertTrue(rawIMURequest.getTotalSize() == msp::MIN_PACKET_SIZE, 
                         "Request packet should have minimum size");
}

void testCRCFunctionality() {
    std::cout << "\n=== Testing MSP CRC Functionality ===" << std::endl;
    
    // Test CRC calculation
    std::vector<uint8_t> buffer(32, 0);
    etl::span<uint8_t> data(buffer);
    
    msp::MSPRawIMU rawIMU(data);
    msp::bindRawIMU(rawIMU, msp::PacketDirection::RESPONSE, 
                    1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, data);
    
    // Calculate CRC
    rawIMU.calculateCRC();
    
    // Verify CRC was written
    uint8_t crcValue = rawIMU.getCRC();
    testRunner.assertTrue(crcValue != 0, "CRC should not be zero after calculation");
    
    // Test CRC verification
    bool crcValid = rawIMU.verifyCRC();
    testRunner.assertTrue(crcValid, "CRC verification should pass for valid packet");
    
    // Test CRC corruption
    rawIMU.setCRC(crcValue ^ 0x01);  // Flip one bit in CRC
    bool crcInvalid = rawIMU.verifyCRC();
    testRunner.assertTrue(!crcInvalid, "CRC verification should fail for corrupted packet");
    
    std::cout << "CRC tests completed successfully!" << std::endl;
}

int main() {
    std::cout << "Starting MSP Library Tests..." << std::endl;
    
    try {
        testBasePacket();
        testRawIMU();
        testStatus();
        testBindingFunctions();
        testMSPRequest();
        testCRCFunctionality();
        
        testRunner.printSummary();
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
