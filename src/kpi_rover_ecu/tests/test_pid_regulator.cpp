#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <cmath>

#include "../include/PIDRegulator.h"

// Test fixture for PIDRegulator tests
class PIDRegulatorTest : public ::testing::Test {
   protected:
    PIDRegulator pid_regulator;

    void SetUp() override {
        // Initialize PIDRegulator with some coefficients
        std::array<float, 3> coefficients = {1.0f, 0.5f, 0.1f};
        pid_regulator.Init(coefficients);
    }
};

TEST_F(PIDRegulatorTest, Init) {
    std::array<float, 3> coefficients = {2.0f, 1.0f, 0.2f};
    pid_regulator.Init(coefficients);

    // Run the PID regulator with a known error and time delta to check if the coefficients are set correctly
    float error = 1.0f;
    float time_dt = 1.0f;
    int result = pid_regulator.Run(error, time_dt);

    // Check if the result is as expected
    float p_term = coefficients[0] * error;
    float i_term = coefficients[1] * error * time_dt;
    float d_term = coefficients[2] * (error / time_dt);

    int expected = static_cast<int>(p_term + i_term + d_term);
    ASSERT_EQ(result, expected);
}

TEST_F(PIDRegulatorTest, Run_PositiveError) {
    float error = 10.0f;
    float time_dt = 1.0f;

    int result = pid_regulator.Run(error, time_dt);

    // Expected result calculation
    float p_term = 1.0f * error;
    float i_term = 0.5f * error * time_dt;
    float d_term = 0.1f * (error / time_dt);

    int expected = static_cast<int>(p_term + i_term + d_term);

    ASSERT_EQ(result, expected);
}

TEST_F(PIDRegulatorTest, Run_NegativeError) {
    float error = -10.0f;
    float time_dt = 1.0f;

    int result = pid_regulator.Run(error, time_dt);

    // Expected result calculation
    float p_term = 1.0f * error;
    float i_term = 0.5f * error * time_dt;
    float d_term = 0.1f * (error / time_dt);

    int expected = static_cast<int>(p_term + i_term + d_term);

    ASSERT_EQ(result, expected);
}

TEST_F(PIDRegulatorTest, Run_ZeroTimeDt) {
    float error = 10.0f;
    float time_dt = 0.0f;

    int result = pid_regulator.Run(error, time_dt);

    // Expected result calculation
    float p_term = 1.0f * error;
    float i_term = 0.5f * error * time_dt;
    float d_term = 0.0f;  // Derivative term should be zero when time_dt is zero

    int expected = static_cast<int>(p_term + i_term);

    ASSERT_EQ(result, expected);
}
