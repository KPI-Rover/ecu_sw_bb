#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>  // Add this include for std::cout

#include "../include/motor.h"
#include "mocks/librobotcontrol/include/mock_rc_encoder.h"
#include "mocks/librobotcontrol/include/mock_rc_motor.h"

// Remove the MIN_RPM and MAX_RPM constants as we'll use Motor::kMinRpm and Motor::kMaxRpm

// Test fixture for motor tests
class MotorTest : public ::testing::Test {
   protected:
    Motor* motor = nullptr;

    void SetUp() override {
        // Reset the mock before each test
        ResetMockRCMotor();
        ResetMockRCEncoder();
    }

    void TearDown() override {
        // Reset the mock after each test
        ResetMockRCMotor();
        ResetMockRCEncoder();

        // Clean up any motor object if created
        if (motor != nullptr) {
            delete motor;
            motor = nullptr;
        }
    }
};

TEST_F(MotorTest, MotorGoCallsRcMotorSet) {
    // Set up the expectation before creating the motor
    // Expect rc_motor_set to be called with channel 0 and duty cycle 256/1000.0
    EXPECT_CALL(GetMockRCMotor(), set(0, ::testing::DoubleEq(1))).WillOnce(::testing::Return(0));

    motor = new Motor(0, false, {1.5f, 0.056f, 1.5f});
    int result = motor->MotorGo(Motor::kMaxRpm);

    EXPECT_EQ(0, result) << "MotorGo should return success";
}

// Test MotorGo with inverted motor direction
TEST_F(MotorTest, MotorGoInverted) {
    // For inverted motor, the duty cycle should be negative
    EXPECT_CALL(GetMockRCMotor(), set(0, ::testing::Lt(0))).WillOnce(::testing::Return(0));

    motor = new Motor(0, true, {1.5f, 0.056f, 1.5f});
    int result = motor->MotorGo(Motor::kMaxRpm);

    EXPECT_EQ(0, result);
}

// Test MotorGo with RPM above maximum
TEST_F(MotorTest, MotorGoAboveMaxRPM) {
    // Verify behavior with RPM above maximum (should clamp to MAX_RPM)
    EXPECT_CALL(GetMockRCMotor(), set(0, ::testing::DoubleEq(1))).WillOnce(::testing::Return(0));

    motor = new Motor(0, false, {1.5f, 0.056f, 1.5f});
    int result = motor->MotorGo(Motor::kMaxRpm + 1);

    EXPECT_EQ(0, result);
}

// Test MotorGo with RPM below -maximum
TEST_F(MotorTest, MotorGoBelowMinusMaxRPM) {
    // Verify behaviour with RPM below -maximum (should clamp to -MAX_RPM)
    EXPECT_CALL(GetMockRCMotor(), set(0, ::testing::DoubleEq(-1))).WillOnce(::testing::Return(0));

    motor = new Motor(0, false, {1.5f, 0.056f, 1.5f});
    int result = motor->MotorGo(-Motor::kMaxRpm - 1);

    EXPECT_EQ(0, result);
}

// Test MotorGo with set function returning error
TEST_F(MotorTest, MotorGoSetError) {
    EXPECT_CALL(GetMockRCMotor(), set(0, ::testing::_)).WillOnce(::testing::Return(-1));

    motor = new Motor(0, false, {1.5f, 0.056f, 1.5f});
    int result = motor->MotorGo(Motor::kMaxRpm);

    EXPECT_NE(0, result) << "MotorGo should return error";
}

// Test MotorStop function
TEST_F(MotorTest, MotorStop) {
    // Stopping motor should set duty cycle to 0
    EXPECT_CALL(GetMockRCMotor(), brake(0)).WillOnce(::testing::Return(0));

    motor = new Motor(0, false, {1.5f, 0.056f, 1.5f});
    int result = motor->MotorStop();

    EXPECT_EQ(0, result) << "MotorStop should return success";
}

// Test MotorStop with error
TEST_F(MotorTest, MotorStopError) {
    EXPECT_CALL(GetMockRCMotor(), brake(0)).WillOnce(::testing::Return(-1));

    motor = new Motor(0, false, {1.5f, 0.056f, 1.5f});
    int result = motor->MotorStop();

    EXPECT_NE(0, result) << "MotorStop should return error";
}

// Test GetEncoderCounter function
TEST_F(MotorTest, GetEncoderCounter) {
    EXPECT_CALL(GetMockRCEncoder(), read(0)).WillOnce(::testing::Return(42));

    motor = new Motor(0, false, {1.5f, 0.056f, 1.5f});
    int result = motor->GetEncoderCounter();

    EXPECT_EQ(42, -result) << "GetEncoderCounter should return encoder value";
}

// Test GetEncoderCounter with error
TEST_F(MotorTest, GetEncoderCounterError) {
    EXPECT_CALL(GetMockRCEncoder(), read(0)).WillOnce(::testing::Return(-1));

    motor = new Motor(0, false, {1.5f, 0.056f, 1.5f});
    int result = motor->GetEncoderCounter();

    EXPECT_EQ(1, result) << "GetEncoderCounter should return error";
}
