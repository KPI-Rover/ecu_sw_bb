#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <vector>

#include "motor.h"
#include "motorConfig.h"
#include "motorsController.h"
#include "protocolHandler.h"

// Include librobotcontrol mocks
#include "mocks/librobotcontrol/include/mock_rc_motor.h"
#include "mocks/librobotcontrol/include/mock_rc_encoder.h"
#include "mocks/librobotcontrol/include/mock_rc_start_stop.h"
#include "mocks/librobotcontrol/include/mock_rc_time.h"

class ProtocolTest : public ::testing::Test {
protected:
    MotorController motors_processor;
    ProtocolHanlder* protocol_handler;
    const uint8_t kMotorNumber = 4;

    void SetUp() override {
        // Reset all mock objects
        ResetMockRCMotor();
        ResetMockRCEncoder();
        ResetMockRCTime();
        
        // Set up default behaviors for mocks instead of expectations
        ON_CALL(getStartStopMock(), kill_existing_process(::testing::_))
            .WillByDefault(::testing::Return(0));
        ON_CALL(getStartStopMock(), enable_signal_handler())
            .WillByDefault(::testing::Return(0));
        ON_CALL(GetMockRCMotor(), init_freq(::testing::_))
            .WillByDefault(::testing::Return(0));
        ON_CALL(GetMockRCEncoder(), init())
            .WillByDefault(::testing::Return(0));
        
        // For motors_
        for (int i = 1; i <= 4; i++) {
            ON_CALL(GetMockRCEncoder(), read(i))
                .WillByDefault(::testing::Return(42));
            ON_CALL(GetMockRCEncoder(), write(i, 0))
                .WillByDefault(::testing::Return(0));
        }
        
        // Set up actual expectations for initialization
        EXPECT_CALL(getStartStopMock(), kill_existing_process(::testing::_))
            .WillOnce(::testing::Return(0));
        EXPECT_CALL(getStartStopMock(), enable_signal_handler())
            .WillOnce(::testing::Return(0));
        EXPECT_CALL(GetMockRCMotor(), init_freq(::testing::_))
            .WillOnce(::testing::Return(0));
        EXPECT_CALL(GetMockRCEncoder(), init())
            .WillOnce(::testing::Return(0));
        
        // Initialize with the same configuration as in main.cpp
        MotorConfig shassis_array[] = {
            MotorConfig(1, false),
            MotorConfig(2, false),
            MotorConfig(3, true),
            MotorConfig(4, true)
        };

        motors_processor.Init(shassis_array, kMotorNumber);
        protocol_handler = new ProtocolHanlder(&motors_processor);
    }

    void TearDown() override {
        // Clean up expectations for Destroy
        EXPECT_CALL(GetMockRCMotor(), cleanup())
            .WillOnce(::testing::Return(0));
        EXPECT_CALL(GetMockRCEncoder(), cleanup())
            .WillOnce(::testing::Return(0));
            
        delete protocol_handler;
        motors_processor.Destroy();
        
        // Clean up mock objects to prevent memory leaks
        DestroyMockRCMotor();
        DestroyMockRCEncoder();
        DestroyMockRCTime();
        DestroyStartStopMock();
    }
};

// Test ID_GET_API_VERSION (0x01)
TEST_F(ProtocolTest, GetApiVersionTest) {
    std::vector<uint8_t> request = {ID_GET_API_VERSION};
    std::vector<uint8_t> response = protocol_handler->HandleMessage(request);
    
    ASSERT_EQ(response.size(), 2);
    ASSERT_EQ(response[0], ID_GET_API_VERSION);
    ASSERT_EQ(response[1], 1);  // API version 1
}

// Test ID_SET_MOTOR_SPEED (0x02)
TEST_F(ProtocolTest, SetMotorSpeedTest) {
    int32_t rpm = 10000;  // Example RPM value
    int32_t rpm_network_order = htonl(rpm);
    uint8_t protocol_motor_id = 0;  // Protocol uses 0-based indexing
    uint8_t librc_motor_id = protocol_motor_id + 1;  // librobotcontrol uses 1-based indexing
    
    // Setup expectations for setting motor speed with 1-based index for librobotcontrol
    EXPECT_CALL(GetMockRCMotor(), set(librc_motor_id, ::testing::_))
        .WillOnce(::testing::Return(0));
    
    std::vector<uint8_t> request = {ID_SET_MOTOR_SPEED, protocol_motor_id};
    request.insert(request.end(), 
                  reinterpret_cast<uint8_t*>(&rpm_network_order),
                  reinterpret_cast<uint8_t*>(&rpm_network_order) + sizeof(int32_t));
    
    std::vector<uint8_t> response = protocol_handler->HandleMessage(request);
    
    ASSERT_EQ(response.size(), 1);
    ASSERT_EQ(response[0], ID_SET_MOTOR_SPEED);
}

// Test ID_SET_ALL_MOTORS_SPEED (0x03)
TEST_F(ProtocolTest, SetAllMotorsSpeedTest) {
    std::vector<int32_t> rpms = {10000, 15000, 20000, 25000};  // Example RPM values
    
    // Setup expectations for setting all motor speeds with 1-based indices for librobotcontrol
    for (int i = 1; i <= 4; i++) {
        EXPECT_CALL(GetMockRCMotor(), set(i, ::testing::_))
            .WillOnce(::testing::Return(0));
    }
    
    std::vector<uint8_t> request = {ID_SET_ALL_MOTORS_SPEED};
    for (int32_t rpm : rpms) {
        int32_t rpm_network_order = htonl(rpm);
        request.insert(request.end(), 
                      reinterpret_cast<uint8_t*>(&rpm_network_order),
                      reinterpret_cast<uint8_t*>(&rpm_network_order) + sizeof(int32_t));
    }
    
    std::vector<uint8_t> response = protocol_handler->HandleMessage(request);
    
    ASSERT_EQ(response.size(), 1);
    ASSERT_EQ(response[0], ID_SET_ALL_MOTORS_SPEED);
}

// Test ID_GET_ENCODER (0x04)
TEST_F(ProtocolTest, GetEncoderTest) {
    uint8_t protocol_motor_id = 1;  // Protocol uses 0-based indexing
    uint8_t librc_motor_id = protocol_motor_id + 1;  // librobotcontrol uses 1-based indexing
    int encoder_value = 42;
    
    // Setup expectations for getting encoder value with 1-based index for librobotcontrol
    EXPECT_CALL(GetMockRCEncoder(), read(librc_motor_id))
        .WillOnce(::testing::Return(encoder_value));
    EXPECT_CALL(GetMockRCEncoder(), write(librc_motor_id, 0))
        .WillOnce(::testing::Return(0));
    
    std::vector<uint8_t> request = {ID_GET_ENCODER, protocol_motor_id};
    std::vector<uint8_t> response = protocol_handler->HandleMessage(request);
    
    ASSERT_EQ(response.size(), 5);  // 1 byte for command ID + 4 bytes for encoder value
    ASSERT_EQ(response[0], ID_GET_ENCODER);
    
    // Check encoder value in response
    int32_t returned_encoder_value;
    memcpy(&returned_encoder_value, &response[1], sizeof(int32_t));
    returned_encoder_value = ntohl(returned_encoder_value);
    
    ASSERT_EQ(returned_encoder_value, encoder_value);
}

// Test ID_GET_ALL_ENCODERS (0x05)
TEST_F(ProtocolTest, GetAllEncodersTest) {
    int encoder_value = 42;
    
    // Setup expectations for getting all encoder values with 1-based indices for librobotcontrol
    for (int i = 0; i < kMotorNumber; i++) {
        uint8_t librc_motor_id = i + 1;  // Convert from 0-based to 1-based
        EXPECT_CALL(GetMockRCEncoder(), read(librc_motor_id))
            .WillOnce(::testing::Return(encoder_value));
        EXPECT_CALL(GetMockRCEncoder(), write(librc_motor_id, 0))
            .WillOnce(::testing::Return(0));
    }
    
    std::vector<uint8_t> request = {ID_GET_ALL_ENCODERS};
    std::vector<uint8_t> response = protocol_handler->HandleMessage(request);
    
    ASSERT_EQ(response.size(), 1 + 4 * kMotorNumber);  // 1 byte for command ID + 4 bytes per motor
    ASSERT_EQ(response[0], ID_GET_ALL_ENCODERS);
    
    // Check all encoder values in response
    for (int i = 0; i < kMotorNumber; i++) {
        int32_t returned_encoder_value;
        memcpy(&returned_encoder_value, &response[1 + i * sizeof(int32_t)], sizeof(int32_t));
        returned_encoder_value = ntohl(returned_encoder_value);
        
        ASSERT_EQ(returned_encoder_value, encoder_value);
    }
}

// Test invalid command ID
TEST_F(ProtocolTest, InvalidCommandTest) {
    std::vector<uint8_t> request = {0xFF};  // Invalid command ID
    std::vector<uint8_t> response = protocol_handler->HandleMessage(request);
    
    ASSERT_EQ(response.size(), 0);  // Should return empty response
}

// Test MotorsStopMessage
TEST_F(ProtocolTest, MotorsStopMessageTest) {
    std::vector<uint8_t> stop_message = protocol_handler->MotorsStopMessage();
    
    ASSERT_EQ(stop_message.size(), 1 + 4 * kMotorNumber);  // 1 byte for command ID + 4 bytes per motor
    ASSERT_EQ(stop_message[0], ID_SET_ALL_MOTORS_SPEED);
    
    // Check that each motor is set to stop (RPM = 0)
    for (int i = 0; i < kMotorNumber; i++) {
        int32_t rpm;
        memcpy(&rpm, &stop_message[1 + i * sizeof(int32_t)], sizeof(int32_t));
        rpm = ntohl(rpm);
        
        ASSERT_EQ(rpm, 0);
    }
}

// Test error handling when setting motor speed
TEST_F(ProtocolTest, SetMotorSpeedErrorTest) {
    int32_t rpm = 10000;
    int32_t rpm_network_order = htonl(rpm);
    uint8_t protocol_motor_id = 2;  // Protocol uses 0-based indexing
    uint8_t librc_motor_id = protocol_motor_id + 1;  // librobotcontrol uses 1-based indexing
    
    // Setup expectation for failure with 1-based index for librobotcontrol
    EXPECT_CALL(GetMockRCMotor(), set(librc_motor_id, ::testing::_))
        .WillOnce(::testing::Return(-1));  // Simulate error
    
    std::vector<uint8_t> request = {ID_SET_MOTOR_SPEED, protocol_motor_id};
    request.insert(request.end(), 
                  reinterpret_cast<uint8_t*>(&rpm_network_order),
                  reinterpret_cast<uint8_t*>(&rpm_network_order) + sizeof(int32_t));
    
    std::vector<uint8_t> response = protocol_handler->HandleMessage(request);
    
    ASSERT_EQ(response.size(), 0);  // Should return empty response on error
}
