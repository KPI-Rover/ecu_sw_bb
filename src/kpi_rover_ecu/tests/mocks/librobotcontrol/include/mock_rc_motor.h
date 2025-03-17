#ifndef MOCK_RC_MOTOR_H
#define MOCK_RC_MOTOR_H

#include <rc/motor.h>
#include <gmock/gmock.h>
#include <memory>

/**
 * @brief Mock class used in the testing implementation
 * This class is used in motor.cpp and needs to match the expected interface
 */
class MockRCMotor {
public:
    MOCK_METHOD(int, init, (), ());
    MOCK_METHOD(int, cleanup, (), ());
    MOCK_METHOD(int, set, (int motor, double duty), ());
    MOCK_METHOD(int, free_spin, (int motor), ());
    MOCK_METHOD(int, brake, (int motor), ());
    MOCK_METHOD(int, set_all, (double duty), ());
    MOCK_METHOD(int, init_freq, (int frequency), ());
};

/**
 * @brief Get the MockRCMotor instance
 * 
 * @return MockRCMotor& Reference to the mock instance
 */
MockRCMotor& GetMockRCMotor();

/**
 * @brief Reset the MockRCMotor instance
 */
void ResetMockRCMotor();

/**
 * @brief Destroy the MockRCMotor instance
 * This releases the memory used by the mock object
 */
void DestroyMockRCMotor();

#endif // MOCK_RC_MOTOR_H
