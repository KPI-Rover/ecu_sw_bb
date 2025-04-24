#include "IMUController.h"

#include <rc/mpu.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

IMUController::IMUController() : data_{}, isStarted_(false), configuration_(rc_mpu_default_config()), actualData_({}) {
    configuration_.i2c_bus = kI2cBus;
    configuration_.gpio_interrupt_pin_chip = kGpioIntPinChip;
    configuration_.gpio_interrupt_pin = kGpioIntPinPin;
    configuration_.dmp_sample_rate = kDmpSampleRate;
    configuration_.enable_magnetometer = kEnableMagnetometer;
}

int IMUController::Init() {
    if (rc_mpu_initialize_dmp(&data_, configuration_) != 0) {
        std::cout << "Initializing MPU failed" << '\n';
        return -1;
    }

    return 0;
}

void IMUController::SetEnable() { isStarted_ = true; }

void IMUController::SetDisable() { isStarted_ = false; }

std::vector<float> IMUController::GetData() {
    if (!isStarted_) {
        return {};
    }

    rc_mpu_read_accel(&data_);
    rc_mpu_read_gyro(&data_);

    actualData_.resize(kActualDataSize);
    const std::vector<float> kAccelData = GetAccel();
    const int kAccelDataSize = static_cast<int>(kAccelData.size());
    std::copy(kAccelData.begin(), kAccelData.begin() + kAccelDataSize, actualData_.begin());

    const std::vector<float> kGyroData = GetGyro();
    const int kGyroDataSize = static_cast<int>(kGyroData.size());
    std::copy(kGyroData.begin(), kGyroData.begin() + kGyroDataSize, actualData_.begin() + kAccelDataSize);

    const std::vector<float> kQaternionData = GetQaternion();
    const int kQaternionDataSize = static_cast<int>(kQaternionData.size());
    std::copy(kQaternionData.begin(), kQaternionData.begin() + kQaternionDataSize,
              actualData_.begin() + kAccelDataSize + kGyroDataSize);

    return actualData_;
}

std::vector<float> IMUController::GetAccel() {
    std::vector<float> ret_val;

    ret_val.push_back(static_cast<float>(data_.accel[0]));
    ret_val.push_back(static_cast<float>(data_.accel[1]));
    ret_val.push_back(static_cast<float>(data_.accel[2]));

    return ret_val;
}

std::vector<float> IMUController::GetGyro() {
    std::vector<float> ret_val;

    ret_val.push_back(static_cast<float>(data_.gyro[0]));
    ret_val.push_back(static_cast<float>(data_.gyro[1]));
    ret_val.push_back(static_cast<float>(data_.gyro[2]));

    return ret_val;
}

std::vector<float> IMUController::GetQaternion() {
    std::vector<float> ret_val;

    ret_val.push_back(static_cast<float>(data_.dmp_quat[QUAT_W]));
    ret_val.push_back(static_cast<float>(data_.dmp_quat[QUAT_X]));
    ret_val.push_back(static_cast<float>(data_.dmp_quat[QUAT_Y]));
    ret_val.push_back(static_cast<float>(data_.dmp_quat[QUAT_Z]));

    return ret_val;
}

void IMUController::Stop() { rc_mpu_power_off(); }

uint8_t IMUController::GetId() { return kIdGetCommand; }
