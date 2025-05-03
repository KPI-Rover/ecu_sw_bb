#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <vector>

#include "motor.h"
#include "motorConfig.h"
#include "motorsController.h"

// Include librobotcontrol mocks
#include "mocks/librobotcontrol/include/mock_rc_encoder.h"
#include "mocks/librobotcontrol/include/mock_rc_motor.h"
#include "mocks/librobotcontrol/include/mock_rc_start_stop.h"
#include "mocks/librobotcontrol/include/mock_rc_time.h"

class MotorControllerTest : public ::testing::Test {
   protected:
    MotorController motor_controller;
    const uint8_t kMotorNumber = 4;

    void SetUp() override {
        // Reset all mock objects
        ResetMockRCMotor();
        ResetMockRCEncoder();
        setupDefaultMockActions();  // Ensure the mock is initialized
        ResetMockRCTime();

        // Set up default behaviors for mocks instead of expectations
        ON_CALL(getStartStopMock(), kill_existing_process(::testing::_)).WillByDefault(::testing::Return(0));
        ON_CALL(getStartStopMock(), enable_signal_handler()).WillByDefault(::testing::Return(0));
        ON_CALL(GetMockRCMotor(), init_freq(::testing::_)).WillByDefault(::testing::Return(0));
        ON_CALL(GetMockRCEncoder(), init()).WillByDefault(::testing::Return(0));

        // Initialize with the same configuration as in the provided example
        std::vector<MotorConfig> motor_configs = {
            MotorConfig(1, false, {1.5f, 0.056f, 1.5f}),
            MotorConfig(2, false, {1.5f, 0.056f, 1.5f}),
            MotorConfig(3, true, {1.5f, 0.056f, 1.5f}),
            MotorConfig(4, true, {1.5f, 0.056f, 1.5f})
        };

        motor_controller.Init(motor_configs, kMotorNumber);
    }

    void TearDown() override {
        // Clean up expectations for Destroy
        EXPECT_CALL(GetMockRCMotor(), cleanup()).WillOnce(::testing::Return(0));
        EXPECT_CALL(GetMockRCEncoder(), cleanup()).WillOnce(::testing::Return(0));

        motor_controller.Destroy();

        // Clean up mock objects to prevent memory leaks
        DestroyMockRCMotor();
        DestroyMockRCEncoder();
        DestroyStartStopMock();
        DestroyMockRCTime();
    }
};

// Test MotorController::Init
TEST_F(MotorControllerTest, Init) {
    // Expectations are already set in SetUp, no additional checks needed here
    ASSERT_EQ(motor_controller.GetMotorsNumber(), kMotorNumber);
}

// Test MotorController::SetMotorRPM
TEST_F(MotorControllerTest, SetMotorRPM) {
    int channel = 1;
    int rpm = 1000;

    EXPECT_CALL(GetMockRCMotor(), set(channel, ::testing::_)).WillOnce(::testing::Return(0));

    int result = motor_controller.SetMotorRPM(channel - 1, rpm); // Convert to 0-based index
    ASSERT_EQ(result, 0);
}

// Test MotorController::SetMotorRPM with invalid channel
TEST_F(MotorControllerTest, SetMotorRPMInvalidChannel) {
    int invalid_channel = kMotorNumber + 1; // Out of range
    int rpm = 1000;

    int result = motor_controller.SetMotorRPM(invalid_channel, rpm);
    ASSERT_EQ(result, -1);
}

// Test MotorController::StopMotor
TEST_F(MotorControllerTest, StopMotor) {
    int channel = 1;

    EXPECT_CALL(GetMockRCMotor(), brake(channel)).WillOnce(::testing::Return(0));

    int result = motor_controller.StopMotor(channel - 1); // Convert to 0-based index
    ASSERT_EQ(result, 0);
}

// Test MotorController::GetEncoderCounter
TEST_F(MotorControllerTest, GetEncoderCounter) {
    int channel = 1;
    int encoder_value = 42;

    EXPECT_CALL(GetMockRCEncoder(), read(channel)).WillOnce(::testing::Return(encoder_value));

    int result = motor_controller.GetEncoderCounter(channel - 1); // Convert to 0-based index
    ASSERT_EQ(-result, encoder_value);
}

// Test MotorController::Destroy
TEST_F(MotorControllerTest, Destroy) {
    // Expectations are already set in TearDown, no additional checks needed here
    SUCCEED();
}
