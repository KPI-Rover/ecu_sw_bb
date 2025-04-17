#ifndef MOCK_RC_IMU_H
#define MOCK_RC_IMU_H

#include <gmock/gmock.h>
#include <rc/mpu.h>

#include <memory>

/**
 * @brief Mock class used in the testing implementation for rc/mpu functionality
 * This class is used in IMUController.cpp and needs to match the expected interface
 */
class MockRCMPU {
   public:
    MOCK_METHOD(int, initialize_dmp, (rc_mpu_data_t* data, rc_mpu_config_t config), ());
    MOCK_METHOD(void, power_off, (), ());
    MOCK_METHOD(int, read_accel, (rc_mpu_data_t* data), ());
    MOCK_METHOD(int, read_gyro, (rc_mpu_data_t* data), ());
    MOCK_METHOD(std::vector<float>, get_accel, (), ());
    MOCK_METHOD(std::vector<float>, get_gyro, (), ());
    MOCK_METHOD(std::vector<float>, get_quaternion, (), ());
};

/**
 * @brief Get the MockRCMPU instance
 *
 * @return MockRCMPU& Reference to the mock instance
 */
MockRCMPU& GetMockRCMPU();

/**
 * @brief Reset the MockRCMPU instance
 */
void ResetMockRCMPU();

/**
 * @brief Destroy the MockRCMPU instance
 * This releases the memory used by the mock object
 */
void DestroyMockRCMPU();

#endif
